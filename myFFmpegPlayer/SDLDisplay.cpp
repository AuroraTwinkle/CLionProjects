//
// Created by aurora on 19-5-12.
//

#include "SDLDisplay.h"


SDLDisplay::SDLDisplay() :preCurFramePTS(0.0),preFramePTS(0.0),curFramePTS(0.0),pAvStreamVideo(nullptr),pAvStreamAudio(nullptr),audioClock(0),delay(0), demuxer(Demuxer()), window(nullptr), texture(
        nullptr), renderer(nullptr), pAvCodecContext(nullptr) {}

bool SDLDisplay::initSDL2(const std::string &windowName, const std::string &url) {

    if (!this->demuxer.openUrl(url)) {
        return false;
    }
    this->videoDecoder = Decoder(demuxer.getPAvCodecVideoParameters());
    this->audioDecoder = Decoder(demuxer.getPAvCodecAudioParameters());
    this->videoDecoder.initDecoder();
    this->audioDecoder.initDecoder();
    this->packetQueueAudio = std::make_shared<PacketQueue>();
    this->packetQueueVideo = std::make_shared<PacketQueue>();

    this->window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    demuxer.getPAvCodecVideoParameters()->width,
                                    demuxer.getPAvCodecVideoParameters()->height, SDL_WINDOW_OPENGL);
    this->renderer = SDL_CreateRenderer(this->window, -1, 0);
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STATIC,
                                      demuxer.getPAvCodecVideoParameters()->width,
                                      demuxer.getPAvCodecVideoParameters()->height);
    this->pAvStreamVideo = demuxer.getPavStreamVideo();
    this->pAvStreamAudio = demuxer.getPavStreamAudio();
    return true;
}


SDLDisplay::~SDLDisplay() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
}


void SDLDisplay::drawing() {

    AVFrame *pFrame = nullptr;
    pFrame = av_frame_alloc();

    while (true) {
        if (videoDecoder.startDecode(packetQueueVideo, pFrame)) {
            double pts = videoDecoder.getPTS(pAvStreamVideo, pFrame);
            double frameDelay = 0.0;

            if (pts != 0) {
                this->videoClock = pts;
            } else {
                pts = this->videoClock;
            }
            frameDelay = av_q2d(pAvStreamVideo->codec->time_base);
            frameDelay += pFrame->repeat_pict / (frameDelay * 2);
            this->videoClock += frameDelay;
            this->curFramePTS = pts;
            this->delay = getDelay() * 1000 + 0.5;
            SDL_UpdateYUVTexture(texture, nullptr, pFrame->data[0], pFrame->linesize[0],
                                 pFrame->data[1], pFrame->linesize[1],
                                 pFrame->data[2], pFrame->linesize[2]);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            SDL_UpdateWindowSurface(window);
        } else {
            av_free(pFrame);
            break;
        }

        SDL_Delay(delay);
        std::cout<<delay<<std::endl;
    }
}


void SDLDisplay::startDemuxer() {
    this->demuxer.readFrame(this->packetQueueVideo, this->packetQueueAudio);
}

bool SDLDisplay::initAudioSpec() {

    memset(&wantAudioSpec, 0, sizeof(wantAudioSpec));
    wantAudioSpec.channels = this->audioDecoder.getPavCodecContext()->channels;
    wantAudioSpec.freq = this->audioDecoder.getPavCodecContext()->sample_rate;
    wantAudioSpec.format = AUDIO_S16SYS;
    wantAudioSpec.silence = 0;
    wantAudioSpec.samples = 2048;

    if (0 > SDL_OpenAudio(&wantAudioSpec, nullptr)) {
        log("fail to open audio device.");
        return false;
    }
    wantFrame.format = AV_SAMPLE_FMT_S16;
    wantFrame.sample_rate = wantAudioSpec.freq;
    wantFrame.channel_layout = av_get_default_channel_layout(wantAudioSpec.channels);
    wantFrame.channels = wantAudioSpec.channels;
    SDL_PauseAudio(0);
    return true;
}


int SDLDisplay::decodeAudioFrame(uint8_t *buff) {
    static AVPacket pkt;
    static int audio_pkt_size = 0;
    static AVFrame frame;

    while (true) {
        if (pkt.data)
            av_packet_unref(&pkt);

        if (!packetQueueAudio->getPacket(pkt, 1))
            return -1;
        if (pkt.pts != AV_NOPTS_VALUE) {
            this->audioClock = pkt.pts * av_q2d(pAvStreamAudio->time_base);
        }
        audio_pkt_size = pkt.size;
        while (0 < audio_pkt_size) {

            avcodec_send_packet(audioDecoder.getPavCodecContext(), &pkt);
            avcodec_receive_frame(audioDecoder.getPavCodecContext(), &frame);

            if (0 > frame.pkt_size) {
                audio_pkt_size = 0;
                break;
            }

            int size = audioConverter.convertAudioFrame(wantFrame, frame, buff);
            if (size < 0) {
                break;
            }
            return size;
        }
    }
}

void SDLDisplay::playAudio() {
    initAudioSpec();//init audio device
    static uint8_t audio_buff[192000 * 3 / 2];
    while (true) {
        int len = decodeAudioFrame(audio_buff);
        if (len < 0) {
            break;
        }
        SDL_QueueAudio(1, audio_buff, len);
    }

}

double SDLDisplay::getDelay() {
    double retDelay = 0.0;
    double frameDelay = 0.0;
    double curAudioClock = 0.0;
    double compare = 0.0;
    double threshold = 0.0;

    frameDelay = curFramePTS - preFramePTS;
    if (frameDelay <= 0 || frameDelay >= 1.0) {
        frameDelay = preCurFramePTS;
    }

    preCurFramePTS = frameDelay;
    preFramePTS = curFramePTS;

    curAudioClock = this->audioClock;


    compare = curFramePTS - curAudioClock;


    threshold = frameDelay;

    if (compare <= -threshold) {
        retDelay = frameDelay / 2;
    } else if (compare >= threshold) {
        retDelay = frameDelay * 2;
    } else {
        retDelay = frameDelay;
    }

    return retDelay;
}




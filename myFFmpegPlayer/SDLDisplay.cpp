//
// Created by aurora on 19-5-12.
//

#include "SDLDisplay.h"


SDLDisplay::SDLDisplay() : demuxer(Demuxer()), pSwrContextAudio(nullptr), window(nullptr), texture(
        nullptr), renderer(nullptr), pAvCodecContext(nullptr) {}

bool SDLDisplay::initSDL2(const std::string &windowName, const std::string &url) {

    if(!this->demuxer.openUrl(url)){
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
    AVFrame *pFrame1 = nullptr;
    pFrame1 = av_frame_alloc();
    initAudioSpec();

    while (true) {

        SDL_Delay(33);

        if(audioDecoder.startDecode(packetQueueAudio,pFrame1)){
            SDL_QueueAudio(1,pFrame1->data[1],pFrame1->linesize[0]);
        }
        if (videoDecoder.startDecode(packetQueueVideo, pFrame)) {
            SDL_UpdateYUVTexture(texture, nullptr, pFrame->data[0], pFrame->linesize[0],
                                 pFrame->data[1], pFrame->linesize[1],
                                 pFrame->data[2], pFrame->linesize[2]);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            SDL_UpdateWindowSurface(window);
        } else {
            av_free(pFrame1);
            av_free(pFrame);
            break;
        }

    }
}


void SDLDisplay::startDemuxer() {
    this->demuxer.readFrame(this->packetQueueVideo, this->packetQueueAudio);
}

bool SDLDisplay::initAudioSpec() {
    pSwrContextAudio = swr_alloc();
    if (nullptr == pSwrContextAudio) {
        log("fail to load alloc for pSwrContextAudio.");
        return false;
    }
    av_opt_set_channel_layout(pSwrContextAudio, "in_channel_layout",
                              this->audioDecoder.getPavCodecContext()->channel_layout, 0);
    av_opt_set_channel_layout(pSwrContextAudio, "out_channel_layout",
                              this->audioDecoder.getPavCodecContext()->channel_layout, 0);
    av_opt_set_int(pSwrContextAudio, "in_sample_rate", this->audioDecoder.getPavCodecContext()->sample_rate, 0);
    av_opt_set_int(pSwrContextAudio, "out_sample_rate", this->audioDecoder.getPavCodecContext()->sample_rate, 0);
    av_opt_set_sample_fmt(pSwrContextAudio, "in_sample_fmt", this->audioDecoder.getPavCodecContext()->sample_fmt, 0);
    av_opt_set_sample_fmt(pSwrContextAudio, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

    if (0 != swr_init(pSwrContextAudio)) {
        log("fail to init swr.");
        return false;
    }

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

void SDLDisplay::audio_callback(void *userdata, Uint8 *stream, int len) {

    auto *aCodecCtx = (AVCodecContext *) userdata;
    int len1, audio_size;

    static uint8_t audio_buff[192000 * 3 / 2];
    static unsigned int audio_buf_size = 0;
    static unsigned int audio_buf_index = 0;

    SDL_memset(stream, 0, len);

    while (len > 0) {
        if (audio_buf_index >= audio_buf_size) {
            audio_size = audio_decode_frame(audioDecoder.getPavCodecContext(), audio_buff);
            if (audio_size < 0) {
                audio_buf_size = 1024;
                memset(audio_buff, 0, audio_buf_size);
            } else
                audio_buf_size = audio_size;

            audio_buf_index = 0;
        }
        len1 = audio_buf_size - audio_buf_index;
        if (len1 > len)
            len1 = len;

        SDL_MixAudio(stream, audio_buff + audio_buf_index, len, SDL_MIX_MAXVOLUME);

        len -= len1;
        stream += len1;
        audio_buf_index += len1;
    }
}

int SDLDisplay::audio_decode_frame(AVCodecContext *pContext, uint8_t *buff) {
    static AVPacket pkt;
    static uint8_t *audio_pkt_data = nullptr;
    static int audio_pkt_size = 0;
    static AVFrame frame;

    int len1 = 0;

    SwrContext *swr_ctx = nullptr;

    while (true) {
        if (pkt.data)
            av_packet_unref(&pkt);

        if (!packetQueueAudio->addPacket(pkt))
            return -1;

        audio_pkt_data = pkt.data;
        audio_pkt_size = pkt.size;
        while (audio_pkt_size > 0) {
            avcodec_send_packet(audioDecoder.getPavCodecContext(), &pkt);
            avcodec_receive_frame(audioDecoder.getPavCodecContext(), &frame);

            len1 = frame.pkt_size;
            if (len1 < 0) {
                audio_pkt_size = 0;
                break;
            }

            audio_pkt_data += len1;
            audio_pkt_size -= len1;
//            if (frame.channels > 0 && frame.channel_layout == 0)
//                frame.channel_layout = av_get_default_channel_layout(frame.channels);
//            else if (frame.channels == 0 && frame.channel_layout > 0)
//                frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);

            if (swr_ctx) {
                swr_free(&swr_ctx);
                swr_ctx = nullptr;
            }

            swr_ctx = swr_alloc_set_opts(nullptr, wantFrame.channel_layout, (AVSampleFormat) wantFrame.format,
                                         wantFrame.sample_rate,
                                         frame.channel_layout, (AVSampleFormat) frame.format, frame.sample_rate, 0,
                                         nullptr);

            if (!swr_ctx || swr_init(swr_ctx) < 0) {
                //cout << "swr_init failed" << endl;
                break;
            }

            int dst_nb_samples = (int) av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
                                                      wantFrame.sample_rate, wantFrame.format, AV_ROUND_INF);
            int len2 = swr_convert(swr_ctx, &buff, dst_nb_samples,
                                   (const uint8_t **) frame.data, frame.nb_samples);
            if (len2 < 0) {
                //cout << "swr_convert failed" << endl;
                break;
            }

            return wantFrame.channels * len2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        }


    }
}



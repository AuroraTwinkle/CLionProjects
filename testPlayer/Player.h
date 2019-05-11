//
// Created by aurora on 19-5-9.
//

#ifndef TESTPLAYER_PLAYER_H
#define TESTPLAYER_PLAYER_H


#include <iostream>
#include <assert.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libpostproc/postprocess.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

#include "SDL2/SDL.h"
#include "SDL2/SDL_thread.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_audio.h"


#define ERROR_SIZE 128
#define SDL_AUDIO_BUFFER_SIZE 2048


typedef struct _AudioPacket {
    AVPacketList *first, *last;
    int nb_packets, size;
    SDL_mutex *mutex;
    SDL_cond *cond;
} AudioPacketQueue;

class Player {

public:

    //construtor
    explicit Player(const std::string& url) {

        audioStream = -1;

        AVDictionary *option = nullptr;

        //av_dict_set(&option, "buffer_size", "425984", 0);
        //av_dict_set(&option, "max_delay", "500000", 0);
        //av_dict_set(&option, "timeout", "20000000", 0);
        //av_dict_set(&option, "rtsp_transport", "tcp", 0);

        //init ffmpeg
        av_register_all();

        avformat_network_init();

        //open video
        int res = avformat_open_input(&pFormatCtx, url.c_str(), nullptr, &option);

        //check video opened
        if (res != 0) {
            getError(res);
            exit(-1);
        }

        //get video info
        res = avformat_find_stream_info(pFormatCtx, NULL);
        if (res < 0) {
            getError(res);
            exit(-1);
        }

        //get video stream
        videoStream = getCodecParameters();
        if (videoStream == -1) {
            std::cout
                    << "Error opening your video using AVCodecParameters, does not have codecpar_type type AVMEDIA_TYPE_VIDEO"
                    << std::endl;
            exit(-1);
        }

        if (findCodec() < 0) exit(-1);

    }
#pragma clang diagnostic pop

    ~Player(void) {

        av_free(buffer);
        av_free(pFrameRGB);

        // Free the YUV frame
        av_free(pFrame);

        // Close the codecs
        avcodec_close(pCodecCtx);

        // Close the video file
        avformat_close_input(&pFormatCtx);

    }


    int alocarMemoria();

    int lerFramesVideo();

    int initSdlWindow();

    static int getAudioPacket(AudioPacketQueue *, AVPacket *, int);

private:

    int videoStream;
    int audioStream;


    AVFormatContext *pFormatCtx = NULL;

    AVCodecParameters *pCodecParameters = NULL;


    AVCodecParameters *pCodecAudioParameters = NULL;


    AVCodecContext *pCodecCtx = NULL;

    AVCodecContext *pCodecAudioCtx = NULL;

    SDL_AudioSpec wantedSpec = {0};


    AVCodec *pCodec = NULL;


    AVCodec *pAudioCodec = NULL;


    AVFrame *pFrame = NULL;


    AVFrame *pFrameRGB = NULL;


    uint8_t *buffer = NULL;


    SDL_Window *screen{};

    SDL_Renderer *renderer{};

    SDL_Texture *sdlTexture{};


    static void getError(int erro);

    int getCodecParameters();

    int findCodec();

    static void initAudioPacket(AudioPacketQueue *);

    static int putAudioPacket(AudioPacketQueue *, AVPacket *);

};


#endif //TESTPLAYER_PLAYER_H

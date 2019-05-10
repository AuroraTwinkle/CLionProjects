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
#define FORMATO AV_PIX_FMT_RGB24
#define SDL_AUDIO_BUFFER_SIZE 1024;


typedef struct _AudioPacket {
    AVPacketList *first, *last;
    int nb_packets, size;
    SDL_mutex *mutex;
    SDL_cond *cond;
} AudioPacket;

class Player {

public:

    //construtor
    Player(std::string endereco) {

        audioStream = -1;

        AVDictionary *option = nullptr;

        //av_dict_set(&option, "buffer_size", "425984", 0);
        //av_dict_set(&option, "max_delay", "500000", 0);
        //av_dict_set(&option, "timeout", "20000000", 0);
        //av_dict_set(&option, "rtsp_transport", "udp", 0);

        //init ffmpeg
        av_register_all();

        avformat_network_init();

        //open video
        int res = avformat_open_input(&pFormatCtx, endereco.c_str(), nullptr, &option);

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

        if (lerCodecVideo() < 0) exit(-1);

    }

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


    void getDetailInfoStream(void);

    int alocarMemoria(void);

    int lerFramesVideo(void);

    int criarDisplay(void);

    static int getAudioPacket(AudioPacket *, AVPacket *, int);

private:

    void memsetAudioPacket(AudioPacket *pq);

    //armazena o �ndice do determinado Stream a ser transmitido
    int videoStream;

    //stream de audio
    int audioStream;

    //contem informa��es sobre o arquivo de v�deo, incluindo os codecs, etc
    AVFormatContext *pFormatCtx = NULL;

    //contem informa��es do codec do v�deo, obtidas atraves de
    //pFormatCtx->streams[i]->codecpar
    //olhando o codec_type e vendo se � transmissao de video do tipo AVMEDIA_TYPE_VIDEO
    AVCodecParameters *pCodecParameters = NULL;

    //Audio COdec Parametrs
    AVCodecParameters *pCodecAudioParameters = NULL;

    //informa��es do codecParameters, por�m copiadas. o pCodecParameters serve como um backup das informa��es do v�deo
    AVCodecContext *pCodecCtx = NULL;

    AVCodecContext *pCodecAudioCtx = NULL;

    SDL_AudioSpec wantedSpec = {0}, audioSpec = {0};

    //guarda o codec do v�deo
    AVCodec *pCodec = NULL;

    //guarda o codec do audio
    AVCodec *pAudioCodec = NULL;

    //estrutura que guarda o frame
    AVFrame *pFrame = NULL;

    //estrutura que guarda o frame RGB
    AVFrame *pFrameRGB = NULL;

    //buffer para leitura dos frames
    uint8_t *buffer = NULL;

    //estrutura que armazena a convers�o para RGB
    struct SwsContext *sws_ctx = NULL;

    //surface window para exibir o video
    //pode ter m�ltiplas screens
    SDL_Window *screen;

    SDL_Renderer *renderer;

    SDL_Texture *bmp;

    //exibe o erro com rela��o ao seu respectivo c�digo
    void getError(int erro);

    int getCodecParameters(void);

    int lerCodecVideo(void);

    int PacketQueuePut(AudioPacket *, const AVPacket *);

    void initAudioPacket(AudioPacket *);

    int putAudioPacket(AudioPacket *, AVPacket *);

};


#endif //TESTPLAYER_PLAYER_H
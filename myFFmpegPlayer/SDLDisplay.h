//
// Created by aurora on 19-5-12.
//

#ifndef MYFFMPEGPLAYER_SDLDISPLAY_H
#define MYFFMPEGPLAYER_SDLDISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif
#include <SDL2/SDL.h>
#include <libswresample/swresample.h>
#include <libavdevice/avdevice.h>
#ifdef __cplusplus
}
#endif

#include "Decoder.h"
#include "PacketQueue.h"
#include "Demuxer.h"
#include "AudioConverter.h"
#include <string>
#include <thread>

#define SDL_QUIT_EVENT (SDL_USEREVENT+1)

class SDLDisplay {
public:
    SDLDisplay();

    virtual ~SDLDisplay();

public:
    bool initSDL2(const std::string &windowName, const std::string &url);

    void startDemuxer();


    void playAudio();

    //void refreshEvent();

    void eventLoop();
    void drawing();

private:


    bool initAudioSpec();

    int decodeAudioFrame(uint8_t *buff);

    double getDelay();


private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_AudioSpec wantAudioSpec{};
    AVCodecContext *pAvCodecContext;
    AVFrame wantFrame{};
    AVStream *pAvStreamVideo;
    AVStream *pAvStreamAudio;
    Demuxer demuxer;
    Decoder videoDecoder;
    Decoder audioDecoder;
    AudioConverter audioConverter;
    std::shared_ptr<PacketQueue> packetQueueVideo;
    std::shared_ptr<PacketQueue> packetQueueAudio;
    MyLog log;

    int playState;

    double audioClock;
    double videoClock;
    double preFramePTS;
    double curFramePTS;
    double preCurFramePTS;
    int delay;
};


#endif //MYFFMPEGPLAYER_SDLDISPLAY_H

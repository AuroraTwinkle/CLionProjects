//
// Created by aurora on 19-5-12.
//

#ifndef MYFFMPEGPLAYER_SDLDISPLAY_H
#define MYFFMPEGPLAYER_SDLDISPLAY_H

#ifdef __cplusplus
extern "C"{
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

class SDLDisplay {
public:
    SDLDisplay();

    virtual ~SDLDisplay();

public:
    bool initSDL2(const std::string& windowName, const std::string &url);
    void startDemuxer();
    void drawing();
    void playAudio();
private:
    bool initAudioSpec();
    //void audio_callback(int len);
    int decodeAudioFrame(uint8_t *buff);
private:
    SDL_Window *window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_AudioSpec wantAudioSpec;
    AVCodecContext* pAvCodecContext;
    AVFrame wantFrame;
    Demuxer demuxer;
    Decoder videoDecoder;
    Decoder audioDecoder;
    AudioConverter audioConverter;
    std::shared_ptr<PacketQueue> packetQueueVideo;
    std::shared_ptr<PacketQueue> packetQueueAudio;
    MyLog log;
};


#endif //MYFFMPEGPLAYER_SDLDISPLAY_H

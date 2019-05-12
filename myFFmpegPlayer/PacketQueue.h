//
// Created by aurora on 19-5-12.
//

#ifndef MYFFMPEGPLAYER_PACKETQUEUE_H
#define MYFFMPEGPLAYER_PACKETQUEUE_H

#ifdef __cplusplus
extern "C"{
#endif
#include <libavformat/avformat.h>
#include <SDL2/SDL.h>
#ifdef __cplusplus
}
#endif

#include "MyLog.h"
class PacketQueue {
public:
    PacketQueue();

    virtual ~PacketQueue();

public:
    bool addPacket(AVPacket *avPacket);
    bool getPacket(AVPacket &avPacket,int block);
private:
    AVPacketList *pLast;
    AVPacketList *pFirst;
    int size;
    int nb_packets;
    SDL_mutex* pSdlMutex;
    SDL_cond* pSdlCond;
    MyLog log;
};


#endif //MYFFMPEGPLAYER_PACKETQUEUE_H

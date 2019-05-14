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

#include <list>
#include <condition_variable>
#include <mutex>
#include "MyLog.h"

class PacketQueue {
public:
    PacketQueue();

    virtual ~PacketQueue();

public:
    bool addPacket(const AVPacket &avPacket);
    bool getPacket(AVPacket &avPacket,int block);
    int getNumPackets() const ;
private:
    std::list<AVPacket> packetList;
    int size;
    std::mutex mutex;
    std::condition_variable conditionVariable;
    MyLog log;
};


#endif //MYFFMPEGPLAYER_PACKETQUEUE_H

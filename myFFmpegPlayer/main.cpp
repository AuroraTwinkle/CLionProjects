#include <memory>
#include "MyLog.h"
#include "PacketQueue.h"
#include "Decoder.h"
#include "Demuxer.h"
#include "SDLDisplay.h"
#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
}
#endif

#include <thread>


int main(int argc,const char *argv[]) {

    SDLDisplay player=SDLDisplay();
    if(!player.initSDL2("myStreamPlayer",argv[1]))
    {
        return 0;
    }
    std::thread demuxThread(&SDLDisplay::startDemuxer,&player);
    demuxThread.detach();
    std::thread playAudio(&SDLDisplay::playAudio,&player);
    playAudio.detach();
    std::thread refreshThread(&SDLDisplay::drawing,&player);
    refreshThread.detach();
    player.eventLoop();

    return 0;
}
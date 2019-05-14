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
    std::thread drawingThread(&SDLDisplay::drawing,&player);
    drawingThread.join();

//    MyLog log;
//    Demuxer demuxer= Demuxer();
//    std::shared_ptr<PacketQueue> pPacketQueueVideo(new PacketQueue());
//    std::shared_ptr<PacketQueue> pPacketQueueAudio(new PacketQueue());
//
//
//    if(2 > argc){
//        log("not enough environment parameter.");
//        return 0;
//    }
//    demuxer.openUrl(argv[1]);
//
//    Decoder pDecoderVideo=Decoder(demuxer.getPAvCodecVideoParameters());
//
//    pDecoderVideo.initDecoder();
//
//
//    std::thread th(&Decoder::startDecode,&pDecoderVideo,pPacketQueueVideo);
//
//    th.detach();
//
//    std::thread th1(&Demuxer::readFrame,&demuxer,pPacketQueueVideo,pPacketQueueAudio);
//    th1.join();



    return 0;
}
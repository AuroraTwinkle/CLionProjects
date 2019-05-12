#include <memory>
#include "MyLog.h"
#include "PacketQueue.h"
#include "Decoder.h"
#include "Demuxer.h"

int main(int argc,const char *argv[]) {
    MyLog log;
    std::unique_ptr<Demuxer> pDemuxer(new Demuxer());
    std::shared_ptr<PacketQueue> pPacketQueueVideo(new PacketQueue());
    std::shared_ptr<PacketQueue> pPacketQueueAudio(new PacketQueue());
    std::unique_ptr<Decoder> pDecoderVideo;

    if(argc<2){
        log("not enough environment parameter.");
        return 0;
    }
    pDemuxer->openUrl(argv[1]);

    pDemuxer->readFrame(pPacketQueueVideo,pPacketQueueAudio);

    return 0;
}
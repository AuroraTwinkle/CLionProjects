//
// Created by aurora on 19-5-12.
//

#ifndef MYFFMPEGPLAYER_DECODER_H
#define MYFFMPEGPLAYER_DECODER_H
#ifdef __cplusplus
extern "C"{
#endif
#include <libavcodec/avcodec.h>

#ifdef __cplusplus
}
#endif

#include "MyLog.h"
#include "PacketQueue.h"
#include <memory>

#include <thread>
class Decoder {
public:
    Decoder();
    explicit Decoder(AVCodecParameters* pAvCodecParameters);
    virtual ~Decoder();

public:
    bool initDecoder();
    bool startDecode(const std::shared_ptr<PacketQueue>& pPacketQueue,AVFrame *avFrame);
    AVCodecContext* getPavCodecContext()const ;
    double getPTS(AVStream *avStream, AVFrame *avFrame);
private:
    AVCodec* pAvCodec;
    AVCodecContext* pAvCodecContext;
    AVCodecParameters* pAvCodecParameters;
    MyLog log;


};


#endif //MYFFMPEGPLAYER_DECODER_H

//
// Created by aurora on 19-5-12.
//

#include "Decoder.h"

Decoder::Decoder(AVCodecParameters* pAvCodecParameters):pAvCodecParameters(pAvCodecParameters) {}

bool Decoder::initDecoder() {
    this->pAvCodec=avcodec_find_decoder(this->pAvCodecParameters->codec_id);
    if(this->pAvCodec== nullptr){
        log("fail to find decoder.");
        return false;
    }
    this->pAvCodecContext=avcodec_alloc_context3(this->pAvCodec);
    if(this->pAvCodecContext== nullptr){
        log("fail to allocate block for avCodecContext.");
        return false;
    }
    if(avcodec_parameters_to_context(this->pAvCodecContext,this->pAvCodecParameters)<0){
        log("fail to init parameters for avCodecContext.");
        return false;
    }
    if(avcodec_open2(this->pAvCodecContext,this->pAvCodec, nullptr)<0){
        log("fail to open avcoderc.");
        return false;
    }
    return true;
}

Decoder::~Decoder() {
    if(!pAvCodecContext){
        avcodec_free_context(&this->pAvCodecContext);
    }
    if(!pAvCodecParameters){
        avcodec_parameters_free(&this->pAvCodecParameters);
    }

}

bool Decoder::startDecode(const std::shared_ptr<PacketQueue>& pPacketQueue,AVFrame *avFrame) {
    AVPacket avPacket;
    if(pPacketQueue->getPacket(avPacket,1))
    {
        avcodec_send_packet(pAvCodecContext,&avPacket);
        avcodec_receive_frame(pAvCodecContext,avFrame);
        return true;
    }
    return false;
}

Decoder::Decoder() {

}

AVCodecContext *Decoder::getPavCodecContext() const {
    return this->pAvCodecContext;
}



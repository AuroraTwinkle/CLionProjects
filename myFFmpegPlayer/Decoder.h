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

class Decoder {
public:
    explicit Decoder(AVCodecParameters* pAvCodecParameters);

    virtual ~Decoder();

public:
    bool initDecoder();
private:
    AVCodec* pAvCodec;
    AVCodecContext* pAvCodecContext;
    AVCodecParameters* pAvCodecParameters;
    MyLog log;
};


#endif //MYFFMPEGPLAYER_DECODER_H

//
// Created by aurora on 19-5-14.
//

#ifndef MYFFMPEGPLAYER_AUDIOCONVERTER_H
#define MYFFMPEGPLAYER_AUDIOCONVERTER_H
#ifdef __cplusplus
extern "C" {
#endif
#include <libswresample/swresample.h>
#ifdef __cplusplus
}
#endif

#include "MyLog.h"

class AudioConverter {
public:
    AudioConverter();

public:
    int convertAudioFrame(AVFrame wantFrame, AVFrame &srcFrame, uint8_t *buff);

private:
    bool initSwrContext(AVFrame wantFrame, AVFrame &srcFrame);

private:
    SwrContext *pSwrContextAudio;
    MyLog log;
};


#endif //MYFFMPEGPLAYER_AUDIOCONVERTER_H

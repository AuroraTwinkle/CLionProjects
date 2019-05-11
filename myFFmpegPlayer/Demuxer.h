//
// Created by aurora on 19-5-11.
//

#ifndef MYFFMPEGPLAYER_DEMUXER_H
#define MYFFMPEGPLAYER_DEMUXER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libavformat/avformat.h>

#ifdef __cplusplus
}
#endif

#include "MyLog.h"

class Demuxer {
public:
    Demuxer();

    virtual ~Demuxer();

public:
    bool openUrl(char const*url);
private:
    AVFormatContext *pAvFormatContext;
    AVCodecParameters* pAvCodecVideoParameters;
public:
    AVCodecParameters *getPAvCodecVideoParameters() const;

    AVCodecParameters *getPAvCodecAudioParameters() const;

private:
    AVCodecParameters* pAvCodecAudioParameters;
    AVDictionary* option;
    int videoStream;
    int audioStream;
    MyLog log;
};


#endif //MYFFMPEGPLAYER_DEMUXER_H

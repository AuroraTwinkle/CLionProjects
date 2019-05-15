//
// Created by aurora on 19-5-11.
//

#ifndef MYFFMPEGPLAYER_DEMUXER_H
#define MYFFMPEGPLAYER_DEMUXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>

#ifdef __cplusplus
}
#endif

#include <memory>
#include "PacketQueue.h"
#include "MyLog.h"

class Demuxer {
public:
    Demuxer();

    virtual ~Demuxer();

public:
    bool openUrl(const std::string &url);

    bool readFrame(const std::shared_ptr<PacketQueue> &ptrVideo, const std::shared_ptr<PacketQueue> &ptrAudio);

private:
    AVFormatContext *pAvFormatContext;
    AVCodecParameters *pAvCodecVideoParameters;
public:
    AVCodecParameters *getPAvCodecVideoParameters() const;

    AVCodecParameters *getPAvCodecAudioParameters() const;

    AVStream *getPavStreamVideo() const;
    AVStream *getPavStreamAudio() const;

private:
    AVCodecParameters *pAvCodecAudioParameters;
    AVDictionary *option;
    int videoStream;
    int audioStream;
    MyLog log;
};


#endif //MYFFMPEGPLAYER_DEMUXER_H

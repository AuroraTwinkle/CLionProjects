//
// Created by aurora on 19-5-11.
//

#include "Demuxer.h"

Demuxer::Demuxer() {
    av_register_all();
    this->audioStream = -1;
    this->videoStream = -1;
    this->pAvCodecAudioParameters = nullptr;
    this->pAvFormatContext = nullptr;
    this->pAvCodecVideoParameters = nullptr;
    this->option = nullptr;
    av_dict_set(&option, "buffer_size", "425984", 0);
    av_dict_set(&option, "max_delay", "500000", 0);
    av_dict_set(&option, "timeout", "20000000", 0);
    av_dict_set(&option, "rtsp_transport", "tcp", 0);
}

Demuxer::~Demuxer() {
    avformat_close_input(&this->pAvFormatContext);
    avcodec_parameters_free(&this->pAvCodecVideoParameters);
    avcodec_parameters_free(&this->pAvCodecAudioParameters);
}

bool Demuxer::openUrl(char const *url) {

    int ret = avformat_open_input(&this->pAvFormatContext, url, nullptr, &option);
    if (ret != 0) {
        log("fail to open the file or net stream.");
        return false;
    }
    ret = avformat_find_stream_info(pAvFormatContext, nullptr);
    if (ret < 0) {
        log("fail to find any information about this file or net stream.");
        return false;
    }
    for (unsigned int i = 0; i < pAvFormatContext->nb_streams; i++) {
        if (0 > videoStream && AVMEDIA_TYPE_VIDEO == pAvFormatContext->streams[i]->codecpar->codec_type) {
            this->videoStream = i;
        }
        if (0 > audioStream && AVMEDIA_TYPE_AUDIO == pAvFormatContext->streams[i]->codecpar->codec_type) {
            this->audioStream = i;
        }
        if (0 > this->videoStream && 0 > this->audioStream)break;
    }
    if(0 > this->videoStream){
        log("fail to find any video stream.");
        return false;
    }
    this->pAvCodecVideoParameters=pAvFormatContext->streams[videoStream]->codecpar;
    if(0 < this->audioStream){
        this->pAvCodecAudioParameters=pAvFormatContext->streams[audioStream]->codecpar;
    }
    log("succeed to open file.");
    return true;
}

AVCodecParameters *Demuxer::getPAvCodecVideoParameters() const {
    return this->pAvCodecVideoParameters;
}

AVCodecParameters *Demuxer::getPAvCodecAudioParameters() const {
    return this->pAvCodecAudioParameters;
}
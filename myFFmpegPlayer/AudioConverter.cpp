//
// Created by aurora on 19-5-14.
//

#include "AudioConverter.h"

AudioConverter::AudioConverter() : pSwrContextAudio(nullptr) {}

bool AudioConverter::initSwrContext(AVFrame wantFrame, AVFrame &srcFrame) {
    if (srcFrame.channels > 0 && srcFrame.channel_layout == 0)
        srcFrame.channel_layout = av_get_default_channel_layout(srcFrame.channels);
    else if (srcFrame.channels == 0 && srcFrame.channel_layout > 0)
        srcFrame.channels = av_get_channel_layout_nb_channels(srcFrame.channel_layout);
    if (!pSwrContextAudio) {
        this->pSwrContextAudio = swr_alloc_set_opts(nullptr, wantFrame.channel_layout,
                                                    (AVSampleFormat) wantFrame.format,
                                                    wantFrame.sample_rate,
                                                    srcFrame.channel_layout, (AVSampleFormat) srcFrame.format,
                                                    srcFrame.sample_rate, 0,
                                                    nullptr);
        return !(!pSwrContextAudio || swr_init(pSwrContextAudio) < 0);
    }
    return false;
}

int AudioConverter::convertAudioFrame(AVFrame wantFrame, AVFrame &srcFrame, uint8_t *buff) {
    int ret = -1;
    if (initSwrContext(wantFrame, srcFrame)) {
        int nbSamples = (int) av_rescale_rnd(
                swr_get_delay(pSwrContextAudio, srcFrame.sample_rate) + srcFrame.nb_samples,
                wantFrame.sample_rate, wantFrame.format, AV_ROUND_INF);
        int len = swr_convert(pSwrContextAudio, &buff, nbSamples,
                              (const uint8_t **) srcFrame.data, srcFrame.nb_samples);
        if (len < 0) {
            log("fail to convert audio.");
            ret = -1;
        } else {
            ret = wantFrame.channels * len * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        }

    }

    if (this->pSwrContextAudio) {
        swr_free(&this->pSwrContextAudio);
        this->pSwrContextAudio = nullptr;
    }
    return ret;
}



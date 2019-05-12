//
// Created by aurora on 19-5-12.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue() {
    this->pFirst = nullptr;
    this->pLast = nullptr;
    this->size = 0;
    this->nb_packets = 0;
    this->pSdlCond = SDL_CreateCond();
    this->pSdlMutex = SDL_CreateMutex();
}

bool PacketQueue::addPacket(AVPacket *avPacket) {
    AVPacketList *avPacketList = nullptr;
    AVPacket *pkt = nullptr;
    pkt = (AVPacket *) av_mallocz_array(1, sizeof(AVPacket));
    if (!pkt) {
        log("fail to allocate block for pkt.");
        return false;
    }
    if (0 > av_packet_ref(pkt, avPacket)) {
        log("fail to hold a ref for avPacket.");
        return false;
    }
    avPacketList = (AVPacketList *) av_malloc(sizeof(AVPacketList));
    if (!avPacketList) {
        log("fail to malloc block for avPacketList.");
        return false;
    }

    avPacketList->pkt = *pkt;
    avPacketList->next = nullptr;

    SDL_LockMutex(this->pSdlMutex);//lock

    //add packet
    if (!this->pLast) {
        this->pFirst = avPacketList;
    } else {
        this->pLast->next = avPacketList;
    }
    this->pLast = avPacketList;

    this->nb_packets++;
    this->size += pkt->size;

    SDL_CondSignal(this->pSdlCond);
    SDL_UnlockMutex(this->pSdlMutex);

    return true;
}

bool PacketQueue::getPacket(AVPacket &avPacket, int block) {
    AVPacketList *avPacketList = nullptr;

    SDL_LockMutex(this->pSdlMutex);
    while (true) {
        avPacketList = this->pFirst;
        if (avPacketList) {
            this->pFirst = avPacketList->next;
            if (!this->pFirst) {
                this->pLast = nullptr;
            }
            this->nb_packets--;
            this->size -= avPacketList->pkt.size;
            avPacket = avPacketList->pkt;
            av_free(avPacketList);
            SDL_UnlockMutex(this->pSdlMutex);
            return true;
        } else if (!block) {
            return false;
        } else {
            SDL_CondWait(this->pSdlCond, this->pSdlMutex);
        }
    }
}

PacketQueue::~PacketQueue() {
    av_free(this->pLast);
    av_free(this->pFirst);
    SDL_DestroyMutex(this->pSdlMutex);
    SDL_DestroyCond(this->pSdlCond);
}

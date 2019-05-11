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

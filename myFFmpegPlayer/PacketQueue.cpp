//
// Created by aurora on 19-5-12.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue():size(0) {}

bool PacketQueue::addPacket(const AVPacket &avPacket) {
    AVPacket pkt=avPacket;

    std::unique_lock<std::mutex> lock(this->mutex);//lock

    //add packet
    this->packetList.push_back(pkt);
    this->size += pkt.size;

    lock.unlock();
    this->conditionVariable.notify_one();

    return true;
}

bool PacketQueue::getPacket(AVPacket &avPacket, int block) {

    std::unique_lock<std::mutex> lock(this->mutex);
    while (true) {
        if (!this->packetList.empty()) {
            avPacket=(this->packetList.front());
            this->size -= avPacket.size;
            packetList.pop_front();
            lock.unlock();
            return true;
        } else if (!block) {
            return false;
        } else {
            this->conditionVariable.wait(lock);
        }
    }
}

PacketQueue::~PacketQueue() {
}

int PacketQueue::getNumPackets() const {
    return packetList.size();
}


#include "friendlybufferhelper.h"

FriendlyBufferHelper::FriendlyBufferHelper() {}

void FriendlyBufferHelper::readArray(char *dat, int len) {
    memcpy(dat, pt, len);
    pt += len;
}

void FriendlyBufferHelper::writeArray(char *dat, int len) {
    memcpy(pt, dat, len);
    pt += len;
}

void FriendlyBufferHelper::reset() {
    pt = buf;
}

int FriendlyBufferHelper::size() {
    return pt - buf;
}

void FriendlyBufferHelper::setBuffer(char *buf) {
    this->buf = buf;
    pt = buf;
}

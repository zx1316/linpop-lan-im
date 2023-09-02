#ifndef FRIENDLYBUFFERHELPER_H
#define FRIENDLYBUFFERHELPER_H
#include <cstring>

// 自己造个轮子用来处理char * 的顺序读取和写入
// 注意：QByteArray的data方法得到的char * 只能读取，不要写入！
// 注意：不提供任何越界检查！
// 宇宙万法的那个源头它是什么？它是char *。

class FriendlyBufferHelper {
private:
    char *buf;
    char *pt;
public:
    FriendlyBufferHelper();
    template<typename T>
    void read(T *varPt) {
        *varPt = *(T *) pt;
        pt += sizeof(T);
    }
    template<typename T>
    void write(T dat) {
        *(T *) pt = dat;
        pt += sizeof(T);
    }
    void readArray(char *dat, int len);
    void writeArray(char *dat, int len);
    void reset();
    int size();
    void setBuffer(char *buf);
    char *getBuffer();
    void skip(int len);
};

#endif // FRIENDLYBUFFERHELPER_H

#ifndef SINGLETONARBOLAVL_H
#define SINGLETONARBOLAVL_H


#include "arbolavl.h"

class SingletonArbolAVL {
public:
    static ArbolAVL& getInstance() {
        static ArbolAVL instance;
        return instance;
    }

private:
    SingletonArbolAVL() {}
    SingletonArbolAVL(const SingletonArbolAVL&) = delete;
    SingletonArbolAVL& operator=(const SingletonArbolAVL&) = delete;
};


#endif // SINGLETONARBOLAVL_H

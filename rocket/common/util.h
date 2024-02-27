//
// Created by zyl on 2024/1/10.
//

#ifndef RPC_UTIL_H
#define RPC_UTIL_H

#include <sys/types.h>
#include <unistd.h>

namespace rocket{
    pid_t getPid();
    pid_t getThreadId();
    int64_t getNowMs();
}

#endif //RPC_UTIL_H

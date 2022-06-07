//
// Created by Chris Pugh on 9/8/15.
//

#ifndef NATIVEACTIVITYTEST_UTIL_H
#define NATIVEACTIVITYTEST_UTIL_H

namespace Virtuoso
{
    namespace Android
    {
        int64_t getTimeNsec()
        {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            return (int64_t) now.tv_sec*1000000000LL + now.tv_nsec;
        }
    }
}

#endif //NATIVEACTIVITYTEST_UTIL_H

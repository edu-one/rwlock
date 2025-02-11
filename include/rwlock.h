/* Copyright (C) Denys Valchuk - All Rights Reserved
 * ZHZhbGNodWtAZ21haWwuY29tCg== 
 */

#ifndef __DV_INCLUDE_RWLOCK_H__
#define __DV_INCLUDE_RWLOCK_H__

#include <mutex>
#include <condition_variable>
#include <cstdint>

namespace dv::rwlock {
    class RWLock {
        std::mutex lock_;
        std::condition_variable cond_;
        size_t activeReaders_ = 0;
        size_t waitingWriters_ = 0;
        bool isWriting_ = false;
    public:
        RWLock() = default;
        ~RWLock() = default;

        void readLock();
        void readUnlock();

        void writeLock();
        void writeUnlock();
    };
} // namespace dv::rwlock

#endif // __DV_INCLUDE_RWLOCK_H__

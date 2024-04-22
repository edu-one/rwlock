/* Copyright (C) Denys Valchuk - All Rights Reserved
 * ZHZhbGNodWtAZ21haWwuY29tCg== 
 */

#ifndef __DV_INCLUDE_RWLOCK_H__
#define __DV_INCLUDE_RWLOCK_H__

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace dv::rwlock {
    class RWLock {
        std::mutex wLock_;
        std::condition_variable wLocked_;
        std::atomic_uint32_t rCount_ = 0;
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

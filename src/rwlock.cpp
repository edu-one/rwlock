/* Copyright (C) Denys Valchuk - All Rights Reserved
 * ZHZhbGNodWtAZ21haWwuY29tCg==
 */

#include "rwlock.h"

#include <stdexcept>

namespace dv::rwlock {

    void RWLock::readLock() {
        if (rCount_.fetch_add(1, std::memory_order_relaxed) == 0) {
            wLock_.lock();
        }
    }
    
    void RWLock::readUnlock() {
        if (rCount_.fetch_sub(1, std::memory_order_relaxed) == 1) {
            wLock_.unlock();
            wLocked_.notify_one();
        }
    }


    void RWLock::writeLock() {
        std::unique_lock<std::mutex> lock(wLock_);
        wLocked_.wait(lock, [this] { return rCount_ == 0; });
    }

    void RWLock::writeUnlock() {
        wLock_.unlock();
    }

} // namespace dv::rwlock

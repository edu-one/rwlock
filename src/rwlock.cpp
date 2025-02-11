/* Copyright (C) Denys Valchuk - All Rights Reserved
 * ZHZhbGNodWtAZ21haWwuY29tCg==
 */

#include "rwlock.h"

#include <stdexcept>

namespace dv::rwlock {

    void RWLock::readLock() {
        std::unique_lock<std::mutex> lock(lock_);
        cond_.wait(lock, [this] { return !(waitingWriters_ > 0 || isWriting_); });
        ++activeReaders_;
    }
    
    void RWLock::readUnlock() {
        bool notify = false;
        {
            std::unique_lock<std::mutex> lock(lock_);
            if (activeReaders_ == 0) {
                throw std::runtime_error("readUnlock called without readLock");
            }
            --activeReaders_;
            if (activeReaders_ == 0) {
                notify = true;
            }
        }
        if (notify) {
            cond_.notify_all();
        }
    }


    void RWLock::writeLock() {
        std::unique_lock<std::mutex> lock(lock_);
        ++waitingWriters_;
        cond_.wait(lock, [this] { return !(activeReaders_ > 0 || isWriting_); });
        --waitingWriters_;
        isWriting_ = true;
    }

    void RWLock::writeUnlock() {
        {
            std::unique_lock<std::mutex> lock(lock_);
            if (!isWriting_) {
                throw std::runtime_error("writeUnlock called without writeLock");
            }
            isWriting_ = false;
        }
        cond_.notify_all();
    }

} // namespace dv::rwlock

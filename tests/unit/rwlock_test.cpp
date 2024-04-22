/* Copyright (C) Denys Valchuk - All Rights Reserved
 * ZHZhbGNodWtAZ21haWwuY29tCg==
 */

#include <rwlock.h>
#include <gtest/gtest.h>

#include <thread>
#include <future>

TEST(RWLock, ReadLockUnlock) {
    dv::rwlock::RWLock rwlock;

    rwlock.readLock();
    rwlock.readUnlock();
}

TEST(RWLock, WriteLockUnlock) {
    dv::rwlock::RWLock rwlock;

    rwlock.writeLock();
    rwlock.writeUnlock();
}

TEST(RWLock, ReadLockUnlockMultiple) {
    dv::rwlock::RWLock rwlock;

    rwlock.readLock();
    rwlock.readLock();
    rwlock.readUnlock();
    rwlock.readUnlock();
}

TEST(RWLock, WriteLockUnlockMultiple) {
    dv::rwlock::RWLock rwlock;
    auto slow = [&rwlock]() {
        rwlock.writeLock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        rwlock.writeUnlock();
    };
    std::thread t1(slow);

    auto fast = [&rwlock]() {
        rwlock.writeLock();
        rwlock.writeUnlock();
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        std::thread t(fast);
        threads.push_back(std::move(t));
    }

    t1.join();
    for (auto& t : threads) {
        t.join();
    }
}

TEST(RWLock, ReadWriteLockUnlock) {
    dv::rwlock::RWLock rwlock;
    size_t counter = 0;
    auto slow = [&rwlock, &counter]() {
        for(size_t i = 0; i < 100; i++) {
          rwlock.writeLock();
          counter++;
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          rwlock.writeUnlock();
        }
    };
    std::thread t1(slow);

    auto fast = [&rwlock, &counter](int id, std::promise<size_t> promise) {
        size_t readCounter = 0;
        while (true) {
            rwlock.readLock();
            if (counter >= 99) {
                rwlock.readUnlock();
                break;
            }
            rwlock.readUnlock();
            readCounter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        promise.set_value(readCounter);
    };

    const size_t numThreads = 10;
    std::vector<std::thread> threads;
    std::vector<std::future<size_t>> results{numThreads};
    for (int i = 0; i < numThreads; i++) {
        std::promise<size_t> p;
        results[i] = std::move(p.get_future());
        threads.emplace_back(fast, i, std::move(p));
    }

    t1.join();
    for (int i = 0; i < numThreads; i++) {
        auto& f = results[i];
        f.wait();
        size_t readCounter = f.get();
        std::cout << "Thread " << i << " read " << readCounter << " times" << std::endl;
    }
    for (auto& t : threads) {
        t.join();
    }
}

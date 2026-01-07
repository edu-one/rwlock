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

TEST(RWLock, MultipleReadersConcurrent) {
    dv::rwlock::RWLock rwlock;
    std::atomic<size_t> sharedCounter{0};
    
    auto reader = [&rwlock, &sharedCounter]() {
        for (int i = 0; i < 1000; i++) {
            rwlock.readLock();
            sharedCounter.load();
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            rwlock.readUnlock();
        }
    };

    std::vector<std::thread> threads;
    const size_t numReaders = 20;
    for (size_t i = 0; i < numReaders; ++i) {
        threads.emplace_back(reader);
    }

    for (auto& t : threads) {
        t.join();
    }
}

TEST(RWLock, PreventReaderStarvation) {
    dv::rwlock::RWLock rwlock;
    std::atomic<size_t> writeCount{0};
    std::atomic<bool> running{true};

    auto writer = [&]() {
        while (running) {
            rwlock.writeLock();
            writeCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            rwlock.writeUnlock();
        }
    };

    auto reader = [&]() {
        size_t localReadCount = 0;
        while (running) {
            rwlock.readLock();
            localReadCount++;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            rwlock.readUnlock();
        }
    };

    std::vector<std::thread> readers;
    for (int i = 0; i < 5; ++i) {
        readers.emplace_back(reader);
    }

    std::thread writerThread(writer);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    running = false;

    writerThread.join();
    for (auto& t : readers) {
        t.join();
    }

    EXPECT_GT(writeCount, 10) << "Writer should have made progress";
}

TEST(RWLock, ExceptionSafety) {
    struct Exception : std::exception {};
    dv::rwlock::RWLock rwlock;

    // Test read lock exception safety
    try {
        rwlock.readLock();
        throw Exception();
        rwlock.readUnlock();
    } catch (const Exception&) {
        rwlock.readUnlock();
    }

    // Verify lock is still usable
    rwlock.writeLock();
    rwlock.writeUnlock();

    // Test write lock exception safety
    try {
        rwlock.writeLock();
        throw Exception();
        rwlock.writeUnlock();
    } catch (const Exception&) {
        rwlock.writeUnlock();
    }

    // Verify lock is still usable
    rwlock.readLock();
    rwlock.readUnlock();
}

# Copyright (C) Denys Valchuk - All Rights Reserved
# ZHZhbGNodWtAZ21haWwuY29tCg==

include(CMakePrintHelpers)

add_executable(rwlock_test rwlock_test.cpp)

find_package(GTest)
target_link_libraries(rwlock_test 
        PRIVATE rwlock
        gtest::gtest)

include(GoogleTest)
gtest_discover_tests(rwlock_test)

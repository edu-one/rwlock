# Copyright (C) Denys Valchuk - All Rights Reserved
# ZHZhbGNodWtAZ21haWwuY29tCg==

cmake_minimum_required(VERSION 3.23)

project(rwlock VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

option(WITH_TESTS "build tests?" ON)

add_library(rwlock src/rwlock.cpp)
target_include_directories(rwlock PUBLIC include)

if(WITH_TESTS)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
  enable_testing()
  add_subdirectory(tests)
endif()

config:
    cmake --preset conan-debug

build: config
    cmake --build --preset conan-debug

test: build
    ctest --preset conan-debug
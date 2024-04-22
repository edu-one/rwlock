rwlock C++ small test project
===============================

# Description
This is a small project to implement one more Read-Write lock in C++.

# Project layout
```
├── CMakeLists.txt
├── README.md
├── include      # Header files
│   └── rwlock.h
├── src          # Source files
│   └── rwlock.cpp
├── tests        # Various tests
│   ├── integration
│   ├── performance
│   └── unit
├── conanfile.py # Conan package manager file
```

# Due to dependencies are managed by Conan, you need to install it first
```bash
pip install -r requirements.txt
```

# Build
```bash
conan install . --build=missing
cmake --preset conan-debug
cmake --build --preset conan-debug
```

# Run tests
```bash
ctest --preset conan-debug
```


>📝
> Based on the template https://github.com/valden/cpptest

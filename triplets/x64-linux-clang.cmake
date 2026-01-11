# x64-linux-clang.cmake

# target architecture
set(VCPKG_TARGET_ARCHITECTURE x64)

# specify Clang as the compiler
set(VCPKG_C_COMPILER /usr/bin/clang)
set(VCPKG_CXX_COMPILER /usr/bin/clang++)

# default linkage rules 
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CRT_LINKAGE dynamic)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)


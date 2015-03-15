rm -rf build 2>/dev/null

BUILD_PREFIX=avr-

mkdir build
cd build
cmake -G "Unix Makefiles" -D CMAKE_C_COMPILER=${BUILD_PREFIX}gcc -D CMAKE_CXX_COMPILER=${BUILD_PREFIX}g++ -D CMAKE_OBJCOPY=${BUILD_PREFIX}objcopy -D CMAKE_LINKER=${BUILD_PREFIX}ld -D CMAKE_SYSTEM_NAME=Generic ..

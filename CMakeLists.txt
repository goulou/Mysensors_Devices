# Project name
project("Devices")

# CMake version
cmake_minimum_required(VERSION 2.6)

# Default Options
# Adjust to the board

#Set this variable to "avr" or "sam"
set(ARDUINO_ARCH "avr")
set(MCU "atmega328p")
set(MCU_MACRO "__AVR_ATmega328P__")
set(CPU_SPEED "16000000")
set(PORT "/dev/ttyUSB0")
set(PORT_SPEED "115200")
set(PIN_VARIANT "standard")
set(PROGRAMMER "stk500v1")
set(COMPILE_FLAGS "")

#PATH
# Configure these according to your MySensors and Arduino installations
set(MYSENSORS_PATH "${CMAKE_SOURCE_DIR}/MySensors" CACHE PATH "Mysensors path")
set(ARDUINO_PATH  "${CMAKE_SOURCE_DIR}/Arduino"   CACHE PATH "Path to arduino installation")
set(ARDUINO_TOOLSET_PATH  "/usr"   CACHE PATH "Path to arduino binaries")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
file(GLOB SRC_FILES_CPP ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SRC_FILES_C   ${CMAKE_SOURCE_DIR}/*.c  )


include(MySensors_cmake/mysensors.cmake)

add_subdirectory(Devices/)


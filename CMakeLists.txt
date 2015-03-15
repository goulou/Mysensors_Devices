# Project name
project("Kitchen1")

# CMake version
cmake_minimum_required(VERSION 2.6)

# Options 
# Adjust to the board

#Set this variable to "avr" or "sam"
set(ARDUINO_ARCH "avr" CACHE STRING "Processor Architecture")
set(MCU "atmega328p" CACHE STRING "Processor Type")
set(CPU_SPEED "16000000" CACHE STRING "Speed of the CPU")
set(PORT "/dev/ttyUSB0" CACHE STRING "USB Port")
set(PORT_SPEED "115200" CACHE STRING "Serial Port Speed")
set(PIN_VARIANT "standard" CACHE STRING "Pin Selection Variant. Either standard, mega, leonardo, eightanaloginputs")
set(PROGRAMMER "stk500v1" CACHE STRING "Programmer Type")
set(COMPILE_FLAGS "" CACHE STRING "Additional Compiler Flags")

#PATH
# Configure these according to your MySensors and Arduino installations
set(MYSENSORS_PATH "${CMAKE_SOURCE_DIR}/MySensors" CACHE PATH "Mysensors path")
set(ARDUINO_PATH  "${CMAKE_SOURCE_DIR}/Arduino"   CACHE PATH "Path to arduino installation")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
file(GLOB SRC_FILES_CPP ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SRC_FILES_C   ${CMAKE_SOURCE_DIR}/*.c  )


include(MySensors_cmake/mysensors.cmake)

target_link_libraries(${PROJECT_NAME} MySensors_DHT)
target_link_libraries(${PROJECT_NAME} MySensors_DallasTemperature)
target_link_libraries(${PROJECT_NAME} MySensors_OneWire)


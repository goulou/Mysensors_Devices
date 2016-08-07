# CMake version
cmake_minimum_required(VERSION 2.8)

# Project name
setup_mysensor_project(${CMAKE_CURRENT_BINARY_DIR})

#Set this variable to "avr" or "sam"
set(MCU "atmega328p")
set(MCU_MACRO "__AVR_ATmega328P__")
set(CPU_SPEED "8000000")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
file(GLOB SRC_FILES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SRC_FILES_C   ${CMAKE_CURRENT_SOURCE_DIR}/*.c  )

add_definitions("-DSERIAL_TX_BUFFER_SIZE=64")
add_definitions("-DMAX_DIGITAL_INPUT=4")
add_definitions("-DMAX_RELAY_COUNT=4")
make_arduino_program(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Arduino_SPI_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} Arduino_SI7021_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} Common_${DEVICE_NAME})

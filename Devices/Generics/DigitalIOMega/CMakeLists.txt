# CMake version
cmake_minimum_required(VERSION 2.8)

# Project name
setup_mysensor_project(${CMAKE_CURRENT_BINARY_DIR})

#Set this variable to "avr" or "sam"
set(MCU "atmega2560")
set(MCU_MACRO "__AVR_ATmega2560__")
set(CPU_SPEED "16000000")
set(PIN_VARIANT "mega")


# Set own source files
# Simply list all your C / C++ source (not header!) files here
file(GLOB SRC_FILES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SRC_FILES_C   ${CMAKE_CURRENT_SOURCE_DIR}/*.c  )

add_definitions("-DDIGITAL_INPUT_NO_DEBOUNCE=1")
add_definitions("-DSERIAL_TX_BUFFER_SIZE=64")
add_definitions("-DMAX_DIGITAL_INPUT=32")
add_definitions("-DMAX_RELAY_COUNT=32")
add_definitions("-DMY_RF24_CS_PIN=15")
add_definitions("-DMY_RF24_CE_PIN=14")
make_arduino_program(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Arduino_SPI_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} Arduino_SI7021_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} Common_${DEVICE_NAME})

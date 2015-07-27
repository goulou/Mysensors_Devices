# Project name
project("EEPROM_Clear_1Mhz")

set(DEVICE_NAME ${PROJECT_NAME})
# CMake version
cmake_minimum_required(VERSION 2.6)

#Set this variable to "avr" or "sam"
set(MCU "atmega328p")
set(MCU_MACRO "__AVR_ATmega328P__")
set(CPU_SPEED "1000000")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
file(GLOB SRC_FILES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SRC_FILES_C   ${CMAKE_CURRENT_SOURCE_DIR}/*.c  )

make_arduino_program(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Arduino_LiquidCrystal_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_DHT_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_DallasTemperature_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_OneWire_${DEVICE_NAME})


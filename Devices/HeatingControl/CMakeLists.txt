# Project name
project("HeatingControl")

set(DEVICE_NAME ${PROJECT_NAME})
# CMake version
cmake_minimum_required(VERSION 2.6)

#Set this variable to "avr" or "sam"
set(MCU "atmega2560" CACHE STRING "Processor Type")
set(MCU_MACRO "__AVR_ATmega2560__" CACHE STRING "Processor Macro")
set(CPU_SPEED "16000000" CACHE STRING "Speed of the CPU")
set(PIN_VARIANT "mega" CACHE STRING "Pin Selection Variant. Either standard, mega, leonardo, eightanaloginputs")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
file(GLOB SRC_FILES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SRC_FILES_C   ${CMAKE_CURRENT_SOURCE_DIR}/*.c  )

make_arduino_program(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Arduino_LiquidCrystal_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_DHT_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_DallasTemperature_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_OneWire_${DEVICE_NAME})


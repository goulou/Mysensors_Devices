# Project name
get_filename_component(DEVICE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${DEVICE_NAME})

# CMake version
cmake_minimum_required(VERSION 2.6)

#Set this variable to "avr" or "sam"
set(MCU "atmega2560")
set(MCU_MACRO "__AVR_ATmega2560__")
set(CPU_SPEED "16000000")
set(PIN_VARIANT "mega")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
file(GLOB SRC_FILES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SRC_FILES_C   ${CMAKE_CURRENT_SOURCE_DIR}/*.c  )

add_definitions(-DDEBUG)
make_arduino_program(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Arduino_LiquidCrystal_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_DHT_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_DallasTemperature_${DEVICE_NAME})
target_link_libraries(${PROJECT_NAME} MySensors_OneWire_${DEVICE_NAME})


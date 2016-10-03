/*
 * digital_input.hpp
 *
 *  Created on: Mar 9, 2016
 *      Author: goulou
 */

#ifndef SOURCE_DIRECTORY__COMMON_DIGITAL_INPUT_HPP_
#define SOURCE_DIRECTORY__COMMON_DIGITAL_INPUT_HPP_

#include <MyMessage.h>

#include <stdint.h>

void setup_digital_input();

void present_digital_inputs();

boolean loop_digital_inputs();
uint8_t get_digital_input_pin_for_id(uint8_t id);

#include "digital_input.cpp"
#endif /* SOURCE_DIRECTORY__COMMON_DIGITAL_INPUT_HPP_ */

/*
 * digital_input.hpp
 *
 *  Created on: Mar 9, 2016
 *      Author: goulou
 */

#ifndef SOURCE_DIRECTORY__COMMON_DIGITAL_INPUT_HPP_
#define SOURCE_DIRECTORY__COMMON_DIGITAL_INPUT_HPP_


#include <stdint.h>
#include <MySensor.h>

void setup_digital_input(MySensor& gw, const uint8_t* pins, const uint8_t* ids, uint8_t count, boolean present);

void present_digital_inputs(MySensor& gw);

boolean loop_digital_inputs(MySensor& gw);

#endif /* SOURCE_DIRECTORY__COMMON_DIGITAL_INPUT_HPP_ */

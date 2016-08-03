/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_RELAY_NODE_HPP_
#define COMMON_RELAY_NODE_HPP_



void setup_digital_output(const uint8_t* pins, const uint8_t* ids, uint8_t count, uint8_t on_state, uint8_t off_state, boolean present);
void present_digital_output();
boolean loop_digital_output();
boolean incoming_message_digital_output(const MyMessage &message);

#include "digital_output.cpp"

#endif /* COMMON_DHT_NODE_HPP_ */

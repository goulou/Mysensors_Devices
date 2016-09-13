/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_RELAY_NODE_HPP_
#define COMMON_RELAY_NODE_HPP_



void setup_digital_output();
void present_digital_output();
boolean loop_digital_output();
boolean incoming_message_digital_output(const MyMessage &message);
uint8_t change_digital_output(uint8_t index);
bool set_digital_output(uint8_t index, uint8_t value);

#include "digital_output.cpp"

#endif /* COMMON_DHT_NODE_HPP_ */

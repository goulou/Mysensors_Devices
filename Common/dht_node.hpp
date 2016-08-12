/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_DHT_NODE_HPP_
#define COMMON_DHT_NODE_HPP_



void setup_dht_device(uint8_t pin, int max_try, boolean present=true);
void present_dht_device();
boolean loop_dht_device();
float dht_get_last_temp();

#include "dht_node.cpp"

#endif /* COMMON_DHT_NODE_HPP_ */

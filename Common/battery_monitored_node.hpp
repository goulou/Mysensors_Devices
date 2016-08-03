/*
 * battery_monitored_node.hpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_BATTERY_MONITORED_NODE_HPP_
#define COMMON_BATTERY_MONITORED_NODE_HPP_



void battery_monitored_node_setup(uint8_t pin);
void battery_monitored_node_loop();

#include "battery_monitored_node.cpp"
#endif /* COMMON_BATTERY_MONITORED_NODE_HPP_ */

/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_SI7021_NODE_HPP_
#define COMMON_SI7021_NODE_HPP_



void setup_si7021(int max_try, boolean present=true, boolean debug=true);
void present_si7021();
boolean loop_si7021();
float si7021_get_last_temp();

#include "si7021_node.cpp"
#endif /* COMMON_SI7021_NODE_HPP_ */

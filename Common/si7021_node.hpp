/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_SI7021_NODE_HPP_
#define COMMON_SI7021_NODE_HPP_



void setup_si7021(MySensor& gw, int max_try, boolean present=true);
void present_si7021(MySensor& gw);
boolean loop_si7021(MySensor& gw);
float si7021_get_last_temp();

#endif /* COMMON_SI7021_NODE_HPP_ */

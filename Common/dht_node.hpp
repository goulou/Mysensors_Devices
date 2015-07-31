/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_DHT_NODE_HPP_
#define COMMON_DHT_NODE_HPP_



void setup_dht(MySensor& gw, uint8_t pin, int max_try, boolean present=true);
void present_dht(MySensor& gw);
boolean loop_dht(MySensor& gw);
float dht_get_last_temp();

#endif /* COMMON_DHT_NODE_HPP_ */

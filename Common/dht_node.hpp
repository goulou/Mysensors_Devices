/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_DHT_NODE_HPP_
#define COMMON_DHT_NODE_HPP_



void setup_dht(MySensor& gw, uint8_t pin, int max_try);
boolean loop_dht(MySensor& gw);


#endif /* COMMON_DHT_NODE_HPP_ */

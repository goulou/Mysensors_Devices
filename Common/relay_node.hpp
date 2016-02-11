/*
 * dht_node.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#ifndef COMMON_RELAY_NODE_HPP_
#define COMMON_RELAY_NODE_HPP_



void setup_relay(MySensor& gw, const uint8_t* pins, const uint8_t* ids, uint8_t count, uint8_t on_state, uint8_t off_state, boolean present);
void present_relays(MySensor& gw);
boolean loop_relays(MySensor& gw);
boolean incoming_message_relay(MySensor& gw, const MyMessage &message);


#endif /* COMMON_DHT_NODE_HPP_ */

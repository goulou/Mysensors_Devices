/*
 * 1w_node.hpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#pragma once


void setup_onewire(bool present=false);
void present_onewire();
void loop_onewire();

#include "1w_node.cpp"

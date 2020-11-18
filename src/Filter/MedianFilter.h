#ifndef __MEDIAN_FILTER_H__
#define __MEDIAN_FILTER_H__

#include <Arduino.h>

class Median
{
public:
	Median();
	float filtered(float value);

private:
	float buffer[3];
	uint8_t counter = 0;
};
#endif
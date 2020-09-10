#pragma once

#include <Arduino.h>

class Median
{
public:
	Median();
	float filtered(float value);

private:
	float buffer[3];
	byte counter = 0;
};
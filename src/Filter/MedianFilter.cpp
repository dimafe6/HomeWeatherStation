#include "./MedianFilter.h"

Median::Median() {}

float Median::filtered(float value)
{
	buffer[counter] = value;
	if (++counter > 2)
		counter = 0;

	float a = buffer[0];
	float b = buffer[1];
	float c = buffer[2];
	float middle;

	if ((a <= b) && (a <= c))
	{
		middle = (b <= c) ? b : c;
	}
	else
	{
		if ((b <= a) && (b <= c))
		{
			middle = (a <= c) ? a : c;
		}
		else
		{
			middle = (a <= b) ? a : b;
		}
	}
	return middle;
}

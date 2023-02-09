#ifndef STD_DEF
#include <stdint.h>
#endif

//pseudo-rng in various ranges and formats taken from Eskil Steenberg's library "forge"
//I do not know the origin or name of the actual algorithms

float randf(uint32_t seed)
{
	seed = (seed << 13) ^ seed;
	return (((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) * 0.5f;
}

double randd(uint32_t seed)
{
	seed = (seed << 13) ^ seed;
	return (((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.5;
}

float randnf(uint32_t seed)
{
//	static float array[7] = {0, -1, -0.4, 0.6, -0.1, 0.7, -0.2};
//	return array[seed % 7];
	seed = (seed << 13) ^ seed;
	return (((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) - 1.0f;
}

double randnd(uint32_t seed)
{
	seed = (seed << 13) ^ seed;
	return (((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1.0;
}

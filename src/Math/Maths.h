#pragma once

#include <stdlib.h>
// TODO: CHECK IF THIS CLASS IS NEEDED
namespace rayTracer
{
	// inlined functions

	//--------------------------------------------------------float to min
	inline uint32_t float_to_int(double x) 
	{
		return ((x) >= 0 ? (uint32_t)((x)+0.5) : (uint32_t)((x)-0.5));
	}

	// ---------------------------------------------------- rand_int
	// a wrapper for rand()
	inline int rand_int(void) 
	{ 
		return(rand());
	}


	// ---------------------------------------------------- rand_float

	inline float rand_float(void) 
	{
		return((float)rand() / ((float)RAND_MAX) + 1);
	}


	// ---------------------------------------------------- set_rand_seed

	inline void set_rand_seed(const int seed) 
	{
		srand(seed);
	}

	// ---------------------------------------------------- float to byte (unsigned char)
	inline uint8_t u8fromfloat(float x)
	{
		//return (uint8_t)(x * 255.99f);
		return ((x * 255.99f) >= 255.0f ? 255 : (uint8_t)(x * 255.99f));
	}

	// ---------------------------------------------------- byte (unsigned char) to float
	inline  float u8tofloat(uint8_t x)
	{
		return (float)(x / 255.99f);
	}

	#define MIN(a, b)		( ( a ) < ( b ) ? ( a ) : ( b ) )
	#define MAX(a, b)		( ( a ) > ( b ) ? ( a ) : ( b ) )
	#define MIN3(a, b, c)		( ( a ) < ( b ) \
	? ( ( a ) < ( c ) ? ( a ) : ( c ) ) \
	: ( ( b ) < ( c ) ? ( b ) : ( c ) ) )
	#define MAX3(a, b, c)		( ( a ) > ( b ) \
	? ( ( a ) > ( c ) ? ( a ) : ( c ) ) \
	: ( ( b ) > ( c ) ? ( b ) : ( c ) ) )
}



/*
 Copyright (c) 2009 NetAllied Systems GmbH
 
 This file is part of Common libftoa.
 
 Licensed under the MIT Open Source License, 
 for details please see LICENSE file or the website
 http://www.opensource.org/licenses/mit-license.php
 */

#include "ftoa.h"
#include "itoa.h"
#include <float.h>
#include <stdbool.h>
#include <string.h>

#include <math.h>
	
typedef union {
	long	L;
	float	F;
} LF_t;



bool isPositiveInfinity(float v)
{
	LF_t x;
	x.F = v;
	return x.L == 0x7F800000L;
}

bool isNegativeInfinity(float v)
{
	LF_t x;
	x.F = v;
	return x.L == (long)0xFF800000L;
}


bool isNaN(float v)
{
	return v!=v;
}


float roundingSummand(float f, int maxLength, int* dezmialPointPos)
{
	float fabs = fabsf(f);
	*dezmialPointPos = 0;
	
	if ( fabs < 0.00001 )
		*dezmialPointPos = -6;
	else if ( fabs < 0.0001 )
		*dezmialPointPos = -5;
	else if ( fabs < 0.001 )
		*dezmialPointPos = -4;
	else if ( fabs < 0.01 )
		*dezmialPointPos = -3;
	else if ( fabs < 0.1 )
		*dezmialPointPos = -2;
	else if ( fabs < 1 )
		*dezmialPointPos = -1;
	else if ( fabs < 10 )
		*dezmialPointPos = 0;
	else if ( fabs < 100 )
		*dezmialPointPos = 1;
	else if ( fabs < 1000 )
		*dezmialPointPos = 2;
	else if ( fabs < 10000 )
		*dezmialPointPos = 3;
	else if ( fabs < 100000 )
		*dezmialPointPos = 4;
	else if ( fabs < 1000000 )
		*dezmialPointPos = 5;
	
	static const float roundingSummands[] = {   0.5e-11f,      // < 0.00001 
		0.5e-10f,      // 0.00001 - 0.0001 
		0.5e-9f,      // 0.0001 - 0.001
		0.5e-8f,      // 0.001 - 0.01
		0.5e-7f,      // 0.01 - 0.1
		0.5e-6f,      // 0.1 - 1     
		0.5e-5f,      // 1 - 10    : 0.000005
		0.5e-4f,      // 10 - 100 
		0.5e-3f,      // 100 - 1000 
		0.5e-2f,      // 1000 - 10000 
		0.5e-1f,      // 10000 - 100000 
		0.5e0f,      // 100000 - 1000000 
	};
	
	float rS = roundingSummands[*dezmialPointPos + 12 - maxLength];
	
	return (f > 0) ? rS : -rS;
}

char* ftoa_no_exponent(float f, char* buffer, int maxLength)
{
	long mantissa, int_part, frac_part;
	short exp2;
	LF_t x;
	char *p = buffer;
	
	int dezmialPointPos = 0;
	float rS = roundingSummand(f, maxLength, &dezmialPointPos);
	if ( dezmialPointPos < 0 )
	{
		maxLength = maxLength - dezmialPointPos; 
	}
	
	x.F = f + rS;
	
	exp2 = (unsigned char)(x.L >> 23) - 127;
	mantissa = (x.L & 0xFFFFFF) | 0x800000;
	frac_part = 0;
	int_part = 0;
	
	
	// handle numbers in non exponential representation
	if (exp2 >= 23)
		int_part = mantissa << (exp2 - 23);
	else if (exp2 >= 0) 
	{
		int_part = mantissa >> (23 - exp2);
		frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
	}
	else /* if (exp2 < 0) */
		frac_part = (mantissa  & 0xFFFFFF) >> -(exp2 + 1);
	
	p = buffer;
	
	if (x.L < 0)
	{
		*p++ = '-';
		maxLength++;
	}
	
	if (int_part == 0)
		*p++ = '0';
	else
	{
		itoa( int_part, p, 10);
		size_t bytesWritten = strlen(p);
		p += bytesWritten;
	}
	
	char m = (char)(p - buffer);
	
	if ((frac_part != 0) && (maxLength > m))
	{
		*p++ = '.';
		char max;
		
		max = (char)(FTOA_BUFFER_SIZE - (p - buffer) - 1);
		if (max > maxLength)
			max = maxLength;
		/* print BCD */
		for (; m < max; m++)
		{
			/* frac_part *= 10;	*/
			frac_part *= 10;// (frac_part << 3) + (frac_part << 1);    
			
			*p++ = (char)(frac_part >> 24) + '0';
			frac_part &= 0xFFFFFF;
		}
		/* delete ending zeros and decimal point if necessary */
		for (--p; p[0] == '0' ; --p)
		{
			if ( p[-1] == '.' )
			{
				p -= 2;
				break;
			}
		}
		
		if ( p[0] == '.' )
		{
			--p;
		}
		
		++p;
	}
	
	return p;
}

int ftoa(float f, char* buffer)
{
	static const float lowerLimitForNonExponetialNotation  = 0.001f;    // Positive numbers smaller will be expressed in exponential representation
	static const float upperLimitForNonExponetialNotation  = 999999;    // Positive numbers bigger will be expressed in exponential representation
	static const float negativeLowerLimitForNonExponetialNotation  = -999999;    // Negative numbers smaller will be expressed in exponential representation
	static const float negativeUpperLimitForNonExponetialNotation  = -0.001f;    // Negative numbers bigger will be expressed in exponential representation
	int exp10 = 0;
	char *p = 0;
	
	if (f == 0.0)
	{
		buffer[0] = '0';
		buffer[1] = 0;
		return 1;
	}
	else if ( isNaN(f) )
	{
		buffer[0] = 'N';
		buffer[1] = 'a';
		buffer[2] = 'N';
		buffer[3] = 0;
		return 3;
	}
	else if ( isPositiveInfinity(f) )
	{
		buffer[0] = 'I';
		buffer[1] = 'N';
		buffer[2] = 'F';
		buffer[3] = 0;
		return 3;
	}
	else if ( isNegativeInfinity(f) )
	{
		buffer[0] = '-';
		buffer[1] = 'I';
		buffer[2] = 'N';
		buffer[3] = 'F';
		buffer[4] = 0;
		return 4;
	}
	
	if ( ((f > 0) && ((f > upperLimitForNonExponetialNotation) || (f < lowerLimitForNonExponetialNotation))) 
		|| ((f < 0) && ((f > negativeUpperLimitForNonExponetialNotation) || (f < negativeLowerLimitForNonExponetialNotation))) )
	{
		// handle big numbers in exponential representation
		// we determine the exponent in exponential representation
		exp10 = (int)log10(f > 0 ? f : -f);
		
		if ( exp10 < 0)
			exp10--;
		
		float factor = pow((float)10, -exp10);
		
		p = ftoa_no_exponent(f*factor, buffer,6);
		
		*p++ = 'e';
		itoa( exp10, p, 10);
		size_t bytesWritten = strlen(p);
		p += bytesWritten;
	}
	else
	{
		p = ftoa_no_exponent(f, buffer,7);
	}
	
	*p = 0;
	
	return (int)(p - buffer);
}
	


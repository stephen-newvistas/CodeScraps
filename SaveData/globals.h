#ifndef GLOBALS_H
#define GLOBALS_H

#define DISPLAY_DATA

#include <string.h>

typedef unsigned char	uchar;
//typedef unsigned char	bool;
typedef unsigned char 	byte;

typedef signed char		int8;
typedef unsigned char	uint8;
typedef signed short	int16;
typedef unsigned short	uint16;
typedef signed int		int32;
typedef unsigned int	uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

typedef signed char		int8_t;
typedef unsigned char	uint8_t;
typedef signed short	int16_t;
typedef unsigned short	uint16_t;
typedef signed int		int32_t;
typedef unsigned int	uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;

#ifndef NULL
	#define NULL ((void *)0)
#endif

#define IN	1
#define OUT	0

#define FALSE	0
#define TRUE	1

#define LOW		0
#define HIGH	1

#define OFF		0
#define ON		1

#define OPEN	1
#define CLOSE	0
#define CLOSED	0

#define CLEAR	0
#define SET		1

#define FINSHED	0
#define BUSY	1

#define LESS_THAN	 -1
#define EQUAL		 0
#define GREATER_THAN 1

#define OUTPUT	0
#define INPUT	1

#define BIT0	(1<<0)
#define BIT1	(1<<1)
#define BIT2	(1<<2)
#define BIT3	(1<<3)
#define BIT4	(1<<4)
#define BIT5	(1<<5)
#define BIT6	(1<<6)
#define BIT7	(1<<7)




#endif	//GLOBALS_H

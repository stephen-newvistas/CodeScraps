#include "loops.h"

//	loops.c
void (*_500us)( void );
void (*_1ms)( void );
void (*_10ms)( void );
void (*_100ms)( void );
void (*_1s)( void );

void Loops_500us( void );
void Loops_1ms( void );
void Loops_10ms( void );
void Loops_100ms( void );
void Loops_1s( void );


typedef struct {
	unsigned char oneMilliSecond;
	unsigned char tenMilliSeconds;
	unsigned char oneHundredMilliSeconds;
	unsigned char oneSecond;
} TimerFlags_t;

TimerFlags_t  timerFlags = { 	.oneMilliSecond = 0 ,
								.tenMilliSeconds = 0 ,
								.oneHundredMilliSeconds = 0 ,
								.oneSecond = 0
							};

struct timeval timeout = { 	.tv_usec = 	250000 ,
							.tv_sec = 	0
						 };

void Loops_Init( void *_fiveHundredMicro() , void *_oneMil() , void *_tenMil() , void *_oneHundredMil() , void *_oneSec() ){
	_500us = _fiveHundredMicro;
	_1ms = _oneMil;
	_10ms = _tenMil;
	_100ms = _oneHundredMil;
	_1s = _oneSec;
}

void Loops_Timer(){

	pselect( 0 , NULL , NULL , NULL , &timeout , NULL );

	static int count = 0;
	if( count++ == 1 ){
		timerFlags.oneMilliSecond = 1;
		count = 0;
	}

	Loops_500us();
	if( timerFlags.oneMilliSecond ){
		Loops_1ms();
	}
	else if( timerFlags.tenMilliSeconds ){
		Loops_10ms();
	}
	else if( timerFlags.oneHundredMilliSeconds ){
		Loops_100ms();
	}
	else if( timerFlags.oneSecond ){
		Loops_1s();
	}
}

void Loops_500us(){
	_500us();
}

void Loops_1ms(){
	timerFlags.oneMilliSecond = 0;
	_1ms();

	static int count = 0;
	if( count % 10 == 2 ){
		timerFlags.tenMilliSeconds = 1;
	}
	if( count % 100 == 4 ){
		timerFlags.oneHundredMilliSeconds = 1;
	}
	if( count == 1000 ){
		timerFlags.oneSecond = 1;
	}
	if( count++ == 1000 )
		count = 0;
}

void Loops_10ms(){
	timerFlags.tenMilliSeconds = 0;
	_10ms();
}

void Loops_100ms(){
	timerFlags.oneHundredMilliSeconds = 0;
	_100ms();
}

void Loops_1s(){
	timerFlags.oneSecond = 0;
	_1s();
}


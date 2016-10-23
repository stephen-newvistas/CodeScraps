#include <stdio.h>
#include <stdlib.h>

#include "rPiModule/UART.h"
#include "rPiModule/loops.h"


//	functions for timing
void Routines_500us( void );
void Routines_1ms( void );
void Routines_10ms( void );
void Routines_100ms( void );
void Routines_1s( void );

//	functions to reroute messages to appropriate comm destination
int *MessageFromBus( Packet *_message );
int *SystemError();


/*////////////////////// Timing Loops /////////////////////////////
*	Place functions to be called in these loops
*/
void Routines_500us( void ){
}

void Routines_1ms( void ){
	static int i = 0;
	switch( i++ ){

		case 0:
			break;

		case 1:
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
			break;

		case 5:
			break;

		case 6:
			break;

		case 7:
			break;

		case 8:
			break;

		case 9:
			break;

		default:
			i = 0;
			break;
	}
}

void Routines_10ms( void ){
	static int i = 0;
	switch( i++ ){

		case 0:
			UART_ReadBus();
			break;

		case 1:
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
			break;

		case 5:
			break;

		case 6:
			break;

		case 7:
			break;

		case 8:
			break;

		case 9:
			break;

		default:
			i = 0;
			break;
	}
}

void Routines_100ms( void ){
	static int i = 0;
	switch( i++ ){

		case 0:
			break;

		case 1:
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
			break;

		case 5:
			break;

		case 6:
			break;

		case 7:
			break;

		case 8:
			break;

		case 9:
			break;

		default:
			i = 0;
			break;
	}
}

void Routines_1s( void ){
	static int i = 0;
	switch( i++ ){

		case 0:
			break;

		case 1:
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
			break;

		case 5:
			break;

		case 6:
			break;

		case 7:
			break;

		case 8:
			break;

		case 9:
			break;

		default:
			i = 0;
			break;
	}
}


int main(){

	//	init the timing loops
	Loops_Init( Routines_500us , Routines_1ms , Routines_10ms , Routines_100ms , Routines_1s );
	//	init the UART
	UART_Init( MessageFromBus , SystemError );

}

int *MessageFromBus( Packet *_message ){
}

int *SystemError(){
}

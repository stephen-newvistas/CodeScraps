#ifndef UART_H
#define UART_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>

#include "../rs485/packetformatter.h"
#include "../rs485/charactercode.h"
#include "../rs485/devicehandler.h"
#include "../globals.h"

#include <IPC.h>

#define BUFFERSIZE 512

#ifndef READTIMEOUT
	#define READTIMEOUT  500000000ul
#endif

#ifndef WRITETIMEOUT
	#define WRITETIMEOUT 500000000ul
#endif

enum UART_ERROR { UART_NO_ERROR , UART_SEND_ERROR , UART_READ_ERROR , UART_NO_DATA };

void UART_Init( void*(*f1)(Packet *) , void*(*f2)(char *) );
int UART_ConvertToPacket( Packet *_packet , unsigned char *_buffer );
int UART_ReadDevice( int );
int UART_ReadBus( void );
int UART_AddPacketToQueue( Packet * , int );
int UART_ProcessPacket( Packet *_packet );
int UART_Process();



#endif

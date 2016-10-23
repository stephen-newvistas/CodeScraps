#include "UART.h"

/*	UART
*	Maintains the communication to/from the RS485 bus. main() calls ReadBus to
*	see if a message is available, if so the message is passed to main().
*/

//	generics to assign passed in functions
void 	(*PassMessageToMain)( Packet * );
void    (*PassErrorToMain)( char* );

void 	*IncomingMessage_UART( Packet * );
int 	SendMessage( Packet * );

#define QUEUESIZE1 16

typedef struct QueueFormat_t{
	Packet packet;
	int comm;
};

typedef struct Queue_t{
	struct QueueFormat_t queue[ QUEUESIZE1 ];
	int next;
	int load;
};

static struct Queue_t queue = {.next = 0 , .load = 0 };

/*	UAR_Init
*	Gets passed in a function from main() to be called when a message is available
*	on the bus. Also can pass an error.
*
*	Inputs:	void *_IncomingMessageFunction() - pointer to function to get called
*				when a message is received on the buss, passes message to main()
*			void *_PassErrorFunction() - calls a function to pass error message
*				to main()
*
*	Outputs:	none
*
*	Notes:
*
*/
void UART_Init( void *_IncomingMessageFunction() , void *_PassErrorFunction() ){
	initSerial();
	PassMessageToMain = _IncomingMessageFunction;
	PassErrorToMain = _PassErrorFunction;
}

/*	SendMessage
*	Writes the message contained within the passed packet to the bus.
*
*	Inputs:	Packet *_message
*
*	Outputs:	UART_NO_ERROR , UART_SEND_ERROR
*
*	Notes:	Logs error.
			Uses deviceHandler.c->writeDevice()
*
*/
int SendMessage( Packet *_message ){
	int retry = 3, writeResult;
	while( retry-- > 0 ){
		if( ( writeResult = writeDevice( _message->deviceID, _message->command , _message->packetData , _message->packetLength , WRITETIMEOUT ) ) < 1 )
			{}
		else
			return UART_NO_ERROR;
	}
	puts( "Couldn't send packet.");
	return UART_SEND_ERROR;


//	int writeResult = writeDevice( _message->deviceID, _message->command , _message->packetData , _message->packetLength );
//	if( writeResult < 1 ){
//		LogError( "Couldn't send packet.");
//		return UART_SEND_ERROR;
//	}
//	return UART_NO_ERROR;
}

/*	IncomingMessage_UART
*	When a message is received from the bus this is called and the message
*	is passed on to main()
*
*	Inputs:	unsigned char * _message
*
*	Outputs: none
*
*	Notes:	calls the function passed if from main() in UART_Init and passes
*		the input message
*
*/
void *IncomingMessage_UART( Packet *_packet ){
	PassMessageToMain( _packet );
}

/*	UART_ReadDevice
*	Checks the bus at a specific address to see if any information is available.
*	If so, send the read message to main() via IncomingMessage_UART()
*	**may call ModuleManager.AddPacketToQueue() rather than pass message to main()
*
*	Inputs: int _deviceID - address of device on bus
*
*	Outputs: UART_NO_ERROR , UART_READ_ERROR , UART_NO_DATA
*
*	Notes: uses devicehandler.c->readDevice()
*
*/
int UART_ReadDevice( int _deviceID ){
	unsigned char readBuffer[BUFFERSIZE];
    memset( &readBuffer , '\0', BUFFERSIZE);
	int readResult;

	readResult = readDevice( _deviceID, readBuffer, BUFFERSIZE , READTIMEOUT );
	if (readResult < 0){
	  puts( "ERROR: Couldn't read packet 3.");
	  return UART_READ_ERROR;
	}
	else if( readResult == 0 ){
//		LogDebug( "No data to read." );
		return UART_NO_DATA;
	}
	else{
		puts( "Data read." );
//		char message[TRAFFIC_MESSAGE_SIZE];
//		memset( &message , '\0', TRAFFIC_MESSAGE_SIZE);
//		sprintf( message , "I:" );
//		message[2] = &readBuffer[0];
//		int i = 0;
//		for( i = 0 ; ( i < TRAFFIC_MESSAGE_SIZE ) | ( i < BUFFERSIZE ) ; i++ ){
//			sprintf( message , "%s%s" , message , &readBuffer[i] );
//		}
//		LogTraffic( message );
//		IncomingMessage_UART( &readBuffer[0] );
		Packet packet;
		ConvertToPacket( &packet , readBuffer );
//		IncomingMessage_UART( &packet );
		UART_AddPacketToQueue( &packet , IN );
		return UART_NO_ERROR;
	};
}

/*	UART_ReadBus
*	Calls ReadDevice for each every module that is registered.
*
*	Inputs: none
*
*	Outputs: UART_NO_ERROR
*
*	Notes: none
*
*/
int UART_ReadBus(){
	int i = 0;
	for( i = 0 ; i < 10 ; i++ ){
		// go through list of addresses to read
	}
	return UART_NO_ERROR;
}

/*	AddPacketToQueue
*
*	Inputs:	Packet *_packet - packet to add to queue
*
*	Outputs:
*
*	Notes:
*
*/
int UART_AddPacketToQueue( Packet *_packet , int _comm ){
	//	for now, skipping queue and just sending message directly
	switch ( _comm ){
		case IN:
			IncomingMessage_UART( _packet );
			break;
		case OUT:
			SendMessage( _packet );
			break;
	}

//	queue.queue[ queue.load ].packet = *_packet;
//	queue.queue[ queue.load ].comm = _comm;
//	queue.load = ( queue.load < QUEUESIZE1 - 1 ) ? queue.load + 1 : 0;
	return 0;
}

/*	ProcessQueue
*
*	Inputs:	Packet *_packet - packet to add to queue
*
*	Outputs:
*
*	Notes:
*
*/
int UART_Process(){
	if( queue.next == queue.load )
		return 1;
	switch ( queue.queue[ queue.next ].comm ){
		case IN:
			IncomingMessage_UART( &queue.queue[ queue.next ].packet );
			break;
		case OUT:
			SendMessage( &queue.queue[ queue.next ].packet );
			break;
	}
	queue.next = ( queue.next < QUEUESIZE1 - 1 ) ? queue.next + 1 : 0;
	return 0;
}


/* */

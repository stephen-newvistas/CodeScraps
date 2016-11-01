#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <IPC.h>
#include "../../rpiuart/packetformatter.h"
//#include "../Libraries/EscapeSerial/escapeSerial.h"
#include "usb_serial.h"
#include "../../MMC/globals.h"
#include "../../MMC/structures.h"
#include "../../MMC/recordData.h"

struct stringPack_t *StringSplit(  const char* , struct stringPack_t* );

//#pragma pack(1)
//typedef struct Packet{
//	unsigned char deviceID;
//	unsigned char sourceID;
//	unsigned char command;
//	unsigned short packetLength;
//	unsigned char packetData[1024];
//};
//#pragma pack(0)

int main( int argc , char **argv ){

//	toilet_t toilet;
//	toilet.modules.refractometer->data.currentSlotTemperature = 23.103;
//	toilet.modules.refractometer->data.heater = 0x01;
//	toilet.modules.refractometer->data.light = 0x01;
//	toilet.modules.refractometer->data.pwm = 0x55;
//	toilet.modules.refractometer->data.setPointTemperature = 23.5;
//	toilet.modules.refractometer->data.specificGravity = 0x12345678;
//
//	RecordData( &toilet );
//
//	return 0;

/*
/*
	int fd[5];
	char *message = malloc( 64 );
	memset( message , '\0' , 64 );

	USBserial_scan( fd );

	while( 1 ){

	char *trimmed = malloc( USB_SERIAL_BUFSIZE );
	char *message = malloc( USB_SERIAL_BUFSIZE );
//	char trimmed[USB_SERIAL_BUFSIZE];
//	char message[ USB_SERIAL_BUFSIZE ];
	memset( message , '\0' , USB_SERIAL_BUFSIZE );
	memset( trimmed , '\0' , USB_SERIAL_BUFSIZE );
	if( USBserial_read( fd[0] , message ) > 0 ){
//		printf( "%i::IncomingUSB: %s" , _fileDescriptor , message );
		int i = 0;
		while( ( *(message + i ) != '\r' ) & ( i < USB_SERIAL_BUFSIZE - 10 ) ){
			memcpy( (trimmed + i ) , (message + i++ ) , 1 );
		}
	}
	if( USBserial_read( fd[1] , message ) > 0 ){
//		printf( "%i::IncomingUSB: %s" , _fileDescriptor , message );
		int i = 0;
		while( ( *(message + i ) != '\r' ) & ( i < USB_SERIAL_BUFSIZE - 10 ) ){
			memcpy( (trimmed + i ) , (message + i++ ) , 1 );
		}
	}
	if( USBserial_read( fd[2] , message ) > 0 ){
//		printf( "%i::IncomingUSB: %s" , _fileDescriptor , message );
		int i = 0;
		while( ( *(message + i ) != '\r' ) & ( i < USB_SERIAL_BUFSIZE - 10 ) ){
			memcpy( (trimmed + i ) , (message + i++ ) , 1 );
		}
	}
		free( message ); //#free
		free( trimmed ); //#free


	}
*/



	struct ipcQueue_t ipcQ;
	Packet p , r;
	Packet *message = (struct Packet*)malloc( 1024 );
	Packet packet;
	unsigned char *arrayToSend = malloc( 1024 );
	unsigned char *arrayReceived = malloc( 1024 );

	IPC_InitQueue( &ipcQ , 1 );
	IPC_OpenQueue( &ipcQ );



//	//	this is to test escape sequences
//	memset( &p.string , '\0' , sizeof( char )*510 );
////	unsigned char *k = "Words and words and lots of other stuff, too.";
////	memcpy( &p.string , k , 51 );
////
////	p.length = 0x04;
////	p.returner = 0x0D;
////	p.nuller = 0x00;
////	p.ender = 0x0A;
//

//	while( 1  ){
//		printf( "Messages waiting on queue 1: %i\n" , IPC_PeekMessage( &ipcQ ) );
//		sleep( 1 );
//	}

/*
	while( 1 ){
		if( IPC_PeekMessage( &ipcQ ) ){
			arrayReceived =  IPC_ReadMessage( &ipcQ );
			printf( "\nData: %s\n" , arrayReceived );
			ConvertFromEscapedArray( arrayReceived , &message );
			printf( "Message Received:\n" );
			printf( "command:       %x\n" , message.command );
			printf( "sourceID:      %x\n" , message.sourceID );
			printf( "deviceID:      %x\n" , message.deviceID );
			printf( "packetLength:  %x\n" , message.packetLength );
			int i;
			for( i = 0 ; i < message.packetLength ; i++ ){
				printf( "packetData[%i]: %x\n" , i , message.packetData[i] );
			}
			printf( "....................\n" );
		}
	}
*/

// To mimic messages sent from GUI

	unsigned char build[ 1048 ];
	unsigned short s;
	int indexer = 0, i = 0;

	while( 1 ){
		indexer = 0;
		printf( "Please enter deviceID:  [\"00\" to exit]" );
		scanf( "%x" , &build[indexer++] );
		if( build[0] == 0x00 )
			exit(0);
		printf( "Please enter sourceID: " );
		scanf( "%x" , &build[indexer++] );
		printf( "Please enter command: " );
		scanf( "%x" , &build[indexer++] );
		printf( "Please enter packetLength LSB: " );
		scanf( "%x" , &build[indexer++] );
		printf( "Please enter packetLength MSB: " );
		scanf( "%x" , &build[indexer++] );
		s = build[4];
		s <<= 8;
		s &= 0xFF00;
		s = s | build[3];
		for( i = 0 ; i < s ; i++ ){
			printf( "Please enter packetData[%i]: " , i );
			scanf( "%x" , &build[indexer++] );
		}

		memcpy( &packet , &build , s + 5 );

		ConvertToEscapedArray( &packet , arrayToSend );
		for( i = 0 ; i < 15 ; i++ )
			printf( "%x" , arrayToSend[i] );
		IPC_SendMessage( &ipcQ , arrayToSend );
		printf( "\nMessage sent.\n" );
	}

//	while( 1 ){
//
//		printf( "Please enter command: [\"00\" to exit]" );
//		scanf( "%x" , &packet.command );
//		if( packet.command == 0x00 )
//			exit(0);
//		printf( "Please enter deviceID: " );
//		scanf( "%x" , &packet.deviceID );
//		printf( "Please enter sourceID: " );
//		scanf( "%x" , &packet.sourceID );
//		printf( "Please enter packetLength: " );
//		scanf( "%hu" , &packet.packetLength );
//		int i;
//		for( i = 0 ; i < packet.packetLength ; i++ ){
//			printf( "Please enter packetData[%i]: " , i );
//			scanf( "%x" , &packet.packetData[i] );
//		}
//
//		ConvertToEscapedArray( &packet , arrayToSend );
//		for( i = 0 ; i < 15 ; i++ )
//			printf( "%x" , arrayToSend[i] );
//		IPC_SendMessage( &ipcQ , arrayToSend );
//		printf( "\nMessage sent.\n" );
//	}


//	if( IPC_PeekMessage( &ipcQ ) ){
//		arrayReceived =  IPC_ReadMessage( &ipcQ );
//	}
//
//	ConvertFromEscapedArray( arrayReceived , &r );

//    const char firstString[] = "This,may,be,a,string,";
//    char *secondString = "00,11,22,33";
//    char *stringHolder = firstString, *temp;
//
//	struct stringPack_t sp;
//	StringSplit( secondString , &sp );
//
//	int i;
//	for( i = 0 ; i < sp.length ; i++ ){
//		printf( "%i\n" , atoi( sp.data[i] ) );
//	}
//
//    printf("Hello world!\n");
    return 0;
}



/*  StringSplit
*	Accepts a string and a stringPack_t object and tokenizes (parses) the string based on the values
*	from delim[]. It modifies the object passed in and also returns it. The object contains a list
*	of the values in the string and the numbers of values.
*
*	Inputs:		cont char *str - this is the string to tokenize
				struct stringPack_t - this is the packet to pack with the data
*
*	Outputs: struct stringPack_t - the packet to pack
*
*	Notes:	does not account for dead spaces, ie: "happy,,go" would not count ",,"
*			code modified from www.quora.com Costya Perepelitsa
*
*/
//struct stringPack_t *StringSplit( const char* _str , struct stringPack_t *_stringPack ) {
//    const char delim[] = ",";
//    // copy the original string so that we don't overwrite parts of it
//    // (don't do this if you don't need to keep the old line,
//    // as this is less efficient)
//    char *s = strdup( _str );
//    // these three variables are part of a very common idiom to
//    // implement a dynamically-growing array
//    size_t tokens_alloc = 1;
//    size_t tokens_used = 0;
//    _stringPack->data = calloc(tokens_alloc, sizeof(char*));
////    char **tokens = calloc(tokens_alloc, sizeof(char*));
//    char *token, *strtok_ctx;
//    for (token = strtok_r(s, delim, &strtok_ctx);
//            token != NULL;
//            token = strtok_r(NULL, delim, &strtok_ctx)) {
//        // check if we need to allocate more space for tokens
//        if (tokens_used == tokens_alloc) {
//            tokens_alloc *= 2;
//            _stringPack->data = realloc(_stringPack->data, tokens_alloc * sizeof(char*));
//        }
//        _stringPack->data[tokens_used++] = strdup(token);
//    }
//    // cleanup
//    if (tokens_used == 0) {
//        free(_stringPack->data);
//        _stringPack->data = NULL;
//    } else {
//        _stringPack->data = realloc(_stringPack->data, tokens_used * sizeof(char*));
//    }
//    _stringPack->length = tokens_used;
////    *numtokens = tokens_used;
//    free(s);
////    return tokens;
//	return _stringPack;
//}

/*

// To mimic messages sent from GUI


	while( 1 ){

		unsigned char c;
		unsigned short s ,t;
		printf( "Please enter command: [\"00\" to exit]" );
		scanf( "%x" , &packet.command );
		if( packet.command == 0x00 )
			exit(0);
//		scanf( "%X" , &c );
//		if( c == 0x00 )
//			exit(0);
//		memcpy( &message->command , &c , 1 );
//		scanf( "%x" , &packet.command );
		printf( "Please enter deviceID: " );
//		scanf( "%x" , &c );
//		memcpy( &message->deviceID , &c , 1 );
		scanf( "%x" , &packet.deviceID );
		printf( "Please enter sourceID: " );
//		scanf( "%x" , &c );
//		memcpy( &message->sourceID , &c , 1 );
		scanf( "%x" , &packet.sourceID );
		printf( "Please enter packetLength: " );
//		scanf( "%hu" , &s );
		//s = ( s&0x00FF << 8 ) | ( s&0x00FF );
//		memcpy( &message->packetLength , &s , 2 );
//		c = s << 8;
//		memcpy( &message->packetLength , &c, 1 );
//		c = s;
//		memcpy( &( message->packetLength + 1) , &c , 1 );
		scanf( "%hu" , &packet.packetLength );
		int i;
		for( i = 0 ; i < packet.packetLength ; i++ ){
			printf( "Please enter packetData[%i]: " , i );
//			scanf( "%x" , &c );
//			memcpy( &message->packetData[i] , &c , 1 );
			scanf( "%x" , &packet.packetData[i] );
		}
		*/

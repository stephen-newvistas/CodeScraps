#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <IPC.h>
#include "../../rpiuart/packetformatter.h"
#include "../../Libraries/EscapeSerial/escapeSerial.h"
#include "/home/pi/MMC/globals.h"

int main( int argc , char **argv ){

	struct ipcQueue_t ipcQ;
	Packet packet;
	unsigned char *arrayToSend = malloc( 1024 );

	IPC_InitQueue( &ipcQ , 1 );
	IPC_OpenQueue( &ipcQ );


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

        for( i = 0 ; i < s + 5 ; i++ )
            printf( "%x " , build[i] );

		ConvertToEscapedArray( &packet , arrayToSend );
		IPC_SendMessage( &ipcQ , arrayToSend );
		printf( "\nMessage sent.\n" );
	}

    return 0;
}


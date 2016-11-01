#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <IPC.h>
#include "../../rpiuart/packetformatter.h"
#include "../../MMC/globals.h"


int main( int argc , char *argv[] ){
	int queueNumber = atoi( argv[0] );

	if( argc != 2 )
		return 1;

	struct ipcQueue_t ipcQ;

	IPC_InitQueue( &ipcQ , queueNumber );
	IPC_OpenQueue( &ipcQ );

	while( 1  ){
		printf( "Messages waiting on queue %i: %i\n" , IPC_PeekMessage( &ipcQ ) , queueNumber );
		sleep( 1 );
	}

    return 0;
}



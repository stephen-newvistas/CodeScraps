#define _XOPEN_SOURCE 1 /* for ptsname */
#define _GNU_SOURCE   1 /* "" */

#include "bluetoothTxRx.h"


void Print( unsigned char *_p ){
    Bluetooth_SendMessage( _p );
}

int main(int argc, char **argv){

    /*  setup  */
    Bluetooth_Init( Print );
    Bluetooth_Setup();

	/* main loop */
	while (1) {

        Bluetooth_CheckMessage();

	} /* while(1) */

	return 0;
}

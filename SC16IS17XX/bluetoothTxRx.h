#ifndef BLUETOOTHTXRXL_H_INCLUDED
#define BLUETOOTHTXRXL_H_INCLUDED

#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

#include "sc16is7xx.h"

void    Bluetooth_Init( void*(*f1)(unsigned char * ));//Packet *) );
void    Bluetooth_Setup( void );
int     Bluetooth_SendMessage( unsigned char * ); //Packet *_packet );
int     Bluetooth_CheckMessage( unsigned char * ); //Packet *_packet );

enum BT_ERROR { BT_ERROR_PTMX , BT_ERROR_SPT_NAME , BT_ERROR_SPT_MODE , BT_ERROR_SPT_UNLOCK , BT_ERROR_I2C_OPEN , BT_ERROR_SC16IS7XX_CREATE , BT_ERROR_SC16IS7XX_BAUD , BT_ERROR_GPIO};



void loop( void );


#endif // BLUETOOTHTXRXL_H_INCLUDED

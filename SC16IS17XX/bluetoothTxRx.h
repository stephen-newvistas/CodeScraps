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
#include "bluetoothTxRxl.h"

extern int message;

void canwrite( void );

void xwrite( void );

void ywrite( unsigned char *_buffer , int _size );

void canread( void );

void xread( void );

void clearbuffer( void );
void setup( void );

void loop( void );


#endif // BLUETOOTHTXRXL_H_INCLUDED

#ifndef USB_SERIAL_H
#define USB_SERIAL_H

#include <IPC.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <dirent.h>

#define USB_SERIAL_BUFSIZE 1024
#define PORT_LENGTH 13
#define USB_MAX_DEVICES 10

#define USB_READ_QUEUE  3
#define USB_WRITE_QUEUE 4


#ifndef MAX_MESSAGE_LENGTH
	#define MAX_MESSAGE_LENGTH 512
#endif


typedef struct {
	int fileDescriptor;
	unsigned char port[PORT_LENGTH];
} usb_t;

extern int  USBserial_init( char *);
extern int  USBserial_read( int , char * );
extern int  USBserial_write( int , unsigned char[MAX_MESSAGE_LENGTH] );
extern int 	USBserial_scan( int *_fdSet );

#endif

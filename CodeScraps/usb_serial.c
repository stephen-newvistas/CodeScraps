#include "usb_serial.h"

#ifndef BUFFERSIZE
	#define BUFFERSIZE 512
#endif

#ifndef ERROR_MESSAGE_SIZE
	#define ERROR_MESSAGE_SIZE 512
#endif

#define SERIAL_PORT "/dev/ttyS0"

//#define DEBUG


static int max_fd = 0;
static fd_set orig_fdset;
static struct timespec timeout;

static int open_port( char * );



/*	serial_scan
*	Checks the system directory and creates a list of port names of any tty port that resembles
*	an Arduino. Then it will init that port and create a file descriptor for it. The function
*	returns an array of the file descriptors.
*
*	Inputs:	int *_fdSet - pass in a pointer to an int array to hold the file descriptors
*
*	Outputs:	none
*
*	Notes:
*
*/
int USBserial_scan( int *_fdSet ){

	char *ports[USB_MAX_DEVICES] , temp[13];
	memset( _fdSet , '\0' , USB_MAX_DEVICES );
	int i = 0 , j = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir( "/dev/" );
	if( d ) {
		while( ( dir = readdir( d) ) != NULL ){
			if( ( strncmp( dir->d_name , "ttyAMA" , 6 ) == 0 ) || ( strncmp( dir->d_name , "serial" , 6 ) == 0 )){
				ports[i] = malloc( 13 );
				memset( temp , '\0' , 13 );
				sprintf( temp , "%s%s" , "/dev/" , dir->d_name );
				strcpy( ports[i] , temp );
//				ports[i] = temp;
				printf( "%s%s\n" , "Available " , ports[ i ] );
				i++;
			}
		}
	}
	else{
		puts( "error" );
	}
	closedir(d );

	for( j = 0 ; j < i ; j++ ){
		_fdSet[j] = USBserial_init( ports[j] );
	}

	return _fdSet;

}

/*	serial_read
*	Reads a message from the file descriptor passed in, and puts it in the array
*	from the char pointer passed in.
*
*	Inputs:	int _fileDescriptor - fd to read from
*			char *_retMessage - pointer to array to place message
*
*	Outputs:	returns number of characters read, else -1 if no data
*
*	Notes:
*
*/
int USBserial_read(  int _fileDescriptor , char *_retMessage ){

	FD_ZERO(&orig_fdset);
	FD_SET( _fileDescriptor, &orig_fdset);
	  timeout.tv_sec = 0;
	  timeout.tv_nsec = 5000000ul;
	int retval = pselect( max_fd + 1 , &orig_fdset , NULL , NULL , &timeout , NULL );
	if( retval ){
		memset( _retMessage , '\0' , USB_SERIAL_BUFSIZE );
	//	read port
		char buf;
		size_t length = 1;
		int ret, spot = 0, j = 0;
		do{
			ret = read( _fileDescriptor , &buf , length );
			while( ret == -1 )
				ret = read( _fileDescriptor , &buf , length );
			sprintf(  &_retMessage[spot] ,"%c" , buf );
			spot += ret;
			j++;
		}
		while(  ret > 0 && buf != '\n' && buf != '\0' );

		return j - 1;
	}
	else
		return -1;
}

/*	serial_write
*	Writes the passed in message to the passed in file descriptor. Returns number
*	of characters written, else returns -1.
*
*	Inputs:	int _fileDescriptor - fd to write to
*			unsigned char _retMessage - message written
*
*	Outputs:	returns number of characters read, else -1 if no data
*
*	Notes:
*
*/
int USBserial_write( int _fileDescriptor , unsigned char _outMessage[ MAX_MESSAGE_LENGTH ] ){

	FD_ZERO(&orig_fdset);
	FD_SET( _fileDescriptor, &orig_fdset);
	timeout.tv_sec = 0;
	timeout.tv_nsec = 5000000ul;
	int retval = pselect( max_fd + 1 , NULL , &orig_fdset , NULL , &timeout , NULL );

	int length = 0;
	while(  _outMessage[ length++ ] != '\0'  )
		{}
	int ret = write( _fileDescriptor , _outMessage , length );
	if( ret < 1 ){
		return  -1;
	}
	return ret;
}

/******************************************************************************
 * Function static int open_port(char*)
 *
 * This function is called to open, setup and configure port.
 *
 * PreCondition:    None
 *
 * Input:           'name' - name of the port to open e.g. dev/ttyAMA0.
 *
 * Output:          file descriptor ID
 *
 * Side Effects:    None
 *
 *****************************************************************************/
static int open_port(char *name){
  int fd;
  struct termios options;

  fd = open(name, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)  {
    perror("open_port: Unable to open serial port - ");
  }
  else{
    // get the current options from fd
    tcgetattr(fd, &options);

    // set input and output baud rate
    cfsetispeed(&options, B57600 );
    cfsetospeed(&options, B57600 );

    // set some flags
    options.c_cflag |= (CLOCAL | CREAD);

    // 8 data no parity 1 stop
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_iflag &= ~IXON;
    options.c_iflag &= ~IXOFF;
    options.c_iflag &= ~IXANY;
    options.c_iflag &= ~INLCR;
    options.c_iflag &= ~IGNCR;
    options.c_iflag &= ~ICRNL;

    options.c_oflag = 0;

    // turn off cononical processing and echo
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // set flags
    tcsetattr(fd, TCSANOW, &options);
    tcsetattr(fd, TCSAFLUSH, &options);
    fcntl(fd, F_SETFL, FNDELAY);

    // return file descriptor
    return(fd);
  } // end of else fd == -1

  return (-1);
} // end of open_port

/******************************************************************************
 * Function void initSerial(void)
 *
 * This function is called to initialize the serial communication.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *****************************************************************************/
int USBserial_init( char *_port ){
  timeout.tv_sec = 1;//0;
  timeout.tv_nsec = 0;//5000000ul;

  int fileDescriptor = open_port( _port);

  if( fileDescriptor <= 0 ){
    printf("Error couldn't open %s" , _port );
	exit(1);
  }

  max_fd = ( fileDescriptor > max_fd) ? fileDescriptor : max_fd;
  return fileDescriptor;
}



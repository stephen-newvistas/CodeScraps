#include "bluetoothTxRx.h"



#define DEFAULT_DEVICE    "/dev/i2c-1"
#define DEFAULT_I2C_ADDR  0x48
#define DEFAULT_BAUDRATE  115200



int fd_i2c;                                 //  fd for sc16
char *i2c_devname;         //  device handle for i2c on rPi
int baudrate;            //  buad rate of sc16's serial port
int i2c_addr;            //  i2c address of sc16
struct sc16is7xx *sc16is7xx;                //  sc16 object
unsigned char buf_rx[128];                 //   buffer for rx data
unsigned char buf_tx[128];                 //   buffer for tx data
ssize_t txlvl, rxlvl;                       //
int ptyfd;                                  //  used for Linux psuedoterminal
int nfds;                                   //
int i, j;                                   //
fd_set readfds, writefds;                   //
struct timeval to;                          // used for timeouts
unsigned int xfers, txcounter, rxcounter;   //
int gpio_out;                               //  #SCD I don't what this is for
int message;
int count;
unsigned char *buf_ptr;

void canwrite( void );
void xwrite( void );
void ywrite( unsigned char *_buffer , int *_size );
void canread( void );
void xread( void );
void yread( void );
int  zread( unsigned char * );
void clearbuffer( void );

void (*CalledFunction)( unsigned char *_p );

void    Bluetooth_Init( void *_IncomingMessageFunction() ){//Packet *) );
    CalledFunction = _IncomingMessageFunction;
}

void canwrite( void ){

		/* Is there space available to write? */
		txlvl = sc16is7xx_get_txlvl( sc16is7xx );   //  returns number of bytes available to write (in buffer)
		if( txlvl == -1 ){                          //  if error getting txlvl...
			perror( "sc16is7xx_get_txlvl()" );
			exit(1);    //#SCD create return code
		}

		if( txlvl > 0 ){
			if( txlvl > (signed int)sizeof( buf_rx ) )
                txlvl = (signed int)sizeof( buf_rx );

            /*  adds ptyfd to the read fd set  */
			FD_SET( ptyfd , &readfds );
			if ( nfds < ( ptyfd + 1) )         //   set nfds to one more than ptyfd
				nfds = ptyfd + 1;              //    for calling select(...) below
		}
}

void xwrite( void ){
    canwrite();

    if( FD_ISSET( ptyfd , &readfds) ){
        j = sc16is7xx_write(sc16is7xx, buf_rx, i);          //  writes buf_rx to sc16is7xx

        if (j < 0) {                                        //  if error writing to sc16...
            perror("sc16is7xx_write()");
            exit(1);    //#SCD create return code
        }
    }
}

void ywrite( unsigned char *_buffer , int *_size ){
    canwrite();

    if( FD_ISSET( ptyfd , &readfds ) ){
        j = sc16is7xx_write( sc16is7xx, (const void*)_buffer, *_size );          //  writes buf_rx to sc16is7xx
        *_size = 0;
        if (j < 0) {                                        //  if error writing to sc16...
            perror("sc16is7xx_write()");
            exit(1);    //#SCD create return code
        }
    }
}

void canread( void ){
		/* is there something in the receive buffer? */
		rxlvl = sc16is7xx_get_rxlvl( sc16is7xx );   //  returns number of byte available to read (in buffer)
		if( rxlvl == -1 ){                          //  if error getting rxlvl...
			perror( "sc16is7xx_get_rxlvl()" );
			exit(1);    //#SCD create return code
		}

		if( rxlvl > 0 ){
			if( rxlvl > (signed int)sizeof( buf_rx ) )
				rxlvl = (signed int)sizeof( buf_rx );

			/* add ptyfd to the write fd set  */
			FD_SET( ptyfd , &writefds );
			if( nfds < ptyfd + 1 )          //  set nfds to one more that ptyfd
				nfds = ptyfd + 1;           //   for calling select(...) below
		}
}

void xread( void ){
        canread();

		if( FD_ISSET( ptyfd , &writefds ) ){
            i = sc16is7xx_read( sc16is7xx , buf_rx , sizeof( buf_rx ) );   //   read sc16 write to buf_rx, i is number of bytes written
            if( i < 0 ){                                                   //   if error...
                perror("sc16is7xx_read()");
                exit(1);
            }

            if( i > 0 ){
                memcpy( buf_ptr , buf_rx , i );
                buf_ptr += i;
                count += i;
            }

            if( count == 5 ){
                count = 0;
                message = 1;
            }
        }


}

void yread( void ){
    unsigned char a;
    canread();
    if( rxlvl > 0 ){
        while( rxlvl > 0 ){
            canread();

            if( FD_ISSET( ptyfd , &writefds ) ){
                i = sc16is7xx_read( sc16is7xx , &a , 1 );   //   read sc16 write to buf_rx, i is number of bytes written
                if( i < 0 ){                                                   //   if error...
                    perror("sc16is7xx_read()");
                    exit(1);
                }

                if( i > 0 ){
                    memcpy( buf_ptr , &a , i );
                    buf_ptr += i;
                    count += i;
                }

                if( count >= 5 ){
                    message = 1;
                    buf_ptr = buf_tx;
                }
            }
        }
        CalledFunction( buf_tx );
    }
}

int zread( unsigned char *_buffer ){
    unsigned char a;
    int counter = 0;
    canread();

    if( rxlvl > 0 ){
        while( rxlvl > 0 ){
            canread();

            if( FD_ISSET( ptyfd , &writefds ) ){
                i = sc16is7xx_read( sc16is7xx , &a , 1 );   //   read sc16 write to buf_rx, i is number of bytes written
                if( i < 0 ){                                                   //   if error...
                    perror("sc16is7xx_read()");
                    return -1;
                }

                if( i > 0 ){
                    memcpy( buf_ptr , &a , i );
                    buf_ptr += i;
                    count += i;
                    counter += 1;
                }

                if( count >= 5 ){
                    message = 1;
                    buf_ptr = buf_tx;
                }
            }
        }
        memcpy( _buffer , buf_tx , counter );
    }
    return counter;
}

void clearbuffer( void ){
    /* Setup the timeout */
    to.tv_sec = 0;
    to.tv_usec = (rxlvl > 0) ? 0 : 1000; /* 1ms */

    /* Call select and return number of fds available   */
    i = select(nfds, &readfds, &writefds, NULL, &to);
    if( i == -1 ){                      //  if error with select()...
        perror( "select()" );
        exit(1);            //#SCD create return code
    }
}

void Bluetooth_Setup( void ){ //#SCD create int as return when return codes are created
    /*  Set the values for the i2c port and sc16 chip  */
	i2c_devname = DEFAULT_DEVICE;
	i2c_addr = DEFAULT_I2C_ADDR;
	baudrate = DEFAULT_BAUDRATE;
	gpio_out = -1;
	message = 0;
	count = 0;
	buf_ptr = buf_tx;

    /*  Create and open a psuedoterminal  */
	ptyfd = open( "/dev/ptmx", O_RDWR|O_NOCTTY|O_NONBLOCK );
	if( ptyfd == -1 ){                  //  if error opening psuedoterminal...
		perror("/dev/ptmx");
		return BT_ERROR_PTMX;
	}

    /* Place the name of the slave psuedoterminal SPT in buf_rx  */
	if( ptsname_r( ptyfd , (char*)buf_rx , sizeof(buf_rx) ) ){
		perror("ptsname_r()");          //  if error getting SPT name...
		return BT_ERROR_SPT_NAME;
	}

    /*  For debug purposes print to screen the name of the virtual terminal  */
	fprintf(stderr,"Virtual terminal is %s.\n", buf_rx);    //#SCD debug

    /*  Change the mode and owner of the SPT to the UID of the calling process  */
	if( grantpt( ptyfd ) == -1 ){       //  if error changing mode...
		perror( "grantpt()" );
		return BT_ERROR_SPT_MODE;
	}

    /*  Unlocks the SPT corresponding to ptyfd, must be called before opening SPT  */
	if( unlockpt( ptyfd ) == -1 ){      //  if error unlocking SPT...
		perror( "unlockpt()" );
		return BT_ERROR_SPT_UNLOCK;
	}

    /*  Open i2c port  */
	if( ( fd_i2c = open( i2c_devname , O_RDWR|O_NOCTTY ) ) == -1 ){
		perror( i2c_devname );          //  if error opening port...
		return BT_ERROR_I2C_OPEN;
	}

    /*  Create new sc16 instance  */
	sc16is7xx = sc16is7xx_new(fd_i2c, i2c_addr, 0/*flags*/);
	if( !sc16is7xx ){                                                   //  if error creating instance...
		fprintf( stderr , "Could not create sc16is7xx instance.\n" );
		return BT_ERROR_SC16IS7XX_CREATE;
	}

    /*  Set baud rate of sc16  */
	if( sc16is7xx_set_baud( sc16is7xx , baudrate ) == -1 ){     //  if error setting baud...
		perror( "sc16is7xx_set_baud()" );
		return BT_ERROR_SC16IS7XX_BAUD;
	}

    /*  Setting GPIOs on sc16 ***NOT USED***  */
	if( gpio_out != -1 ){
		fprintf(stderr, "setting GPIO to 0x%04x\n", gpio_out & 0xff);
		for( i = 7 ; i >= 0 ; i-- )
			fprintf(stderr," D%d=%d", i, !!(gpio_out & (1<<i)));
		fprintf( stderr , "\n" );
		if( sc16is7xx_gpio_out( sc16is7xx , 0xff , gpio_out & 0xff ) == -1 ){
			perror( "sc16is7xx_gpio_out" );
			return BT_ERROR_GPIO;
		}
	}

    /*  Print baud rate for debug purposes  */
	fprintf(stderr,"Baudrate is %d.\n\n", baudrate);    //#SCD debug
}

unsigned char gear[1024];
int hold;

void loop(){

    FD_ZERO( &readfds );        //  clear read fd set
    FD_ZERO( &writefds );       //  clear write fd set
    nfds = 0;                   //

    clearbuffer();



    yread();


    if( message ){
        ywrite( buf_tx, &count );
        message = 0;
    }


    /* For debug purposes print stats of sc16  */
    sc16is7xx_stats(sc16is7xx, &xfers, &txcounter, &rxcounter);
    fprintf(stderr,"\r%u %u %u %4d %4d\r", xfers, txcounter, rxcounter, txlvl, rxlvl);
    fflush(stderr);
}

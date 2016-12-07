#define _XOPEN_SOURCE 1 /* for ptsname */
#define _GNU_SOURCE   1 /* "" */

#include "sc16is7xx.h"

#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

#define DEFAULT_DEVICE    "/dev/i2c-1"
#define DEFAULT_I2C_ADDR  0x48
#define DEFAULT_BAUDRATE  115200


int fd_i2c;                                 //  fd for sc16
char *i2c_devname;         //  device handle for i2c on rPi
int baudrate;            //  buad rate of sc16's serial port
int i2c_addr;            //  i2c address of sc16
struct sc16is7xx *sc16is7xx;                //  sc16 object
unsigned char buf_rx[128];                 //   buffer for rx data
unsigned char bur_tx[128];                 //   buffer for tx data
ssize_t txlvl, rxlvl;                       //
int ptyfd;                                  //  used for Linux psuedoterminal
int nfds;                                   //
int i, j;                                   //
fd_set readfds, writefds;                   //
struct timeval to;                          // used for timeouts
unsigned int xfers, txcounter, rxcounter;   //
int gpio_out = -1;                          //  #SCD I don't what this is for

int main(int argc, char **argv){

    printf( "here\n\r" );

    /*  setup  */
    setup();

	/* main loop */
	while (1) {

        loop();

	} /* while(1) */

	return 0;
}

void xwrite( void ){
        j = sc16is7xx_write(sc16is7xx, buf_rx, i);          //  writes buf_rx to sc16is7xx

        if (j < 0) {                                        //  if error writing to sc16...
            perror("sc16is7xx_write()");
            exit(1);    //#SCD create return code
        }
}

void xread( void ){
        i = sc16is7xx_read( sc16is7xx , buf_rx , sizeof( buf_rx ) );   //   read sc16 write to buf_rx, i is number of bytes written
        if( i < 0 ){                                                   //   if error...
            perror("sc16is7xx_read()");
            exit(1);
        }
}

void setup( void ){ //#SCD create int as return when return codes are created
    /*  Set the values for the i2c port and sc16 chip  */
	i2c_devname = DEFAULT_DEVICE;
	i2c_addr = DEFAULT_I2C_ADDR;
	baudrate = DEFAULT_BAUDRATE;
	gpio_out = -1;

    /*  Create and open a psuedoterminal  */
	ptyfd = open( "/dev/ptmx", O_RDWR|O_NOCTTY|O_NONBLOCK );
	if( ptyfd == -1 ){                  //  if error opening psuedoterminal...
		perror("/dev/ptmx");
		exit(1);
	}

    /* Place the name of the slave psuedoterminal SPT in buf_rx  */
	if( ptsname_r( ptyfd , (char*)buf_rx , sizeof(buf_rx) ) ){
		perror("ptsname_r()");          //  if error getting SPT name...
		exit(1);
	}

    /*  For debug purposes print to screen the name of the virtual terminal  */
	fprintf(stderr,"Virtual terminal is %s.\n", buf_rx);    //#SCD debug

    /*  Change the mode and owner of the SPT to the UID of the calling process  */
	if( grantpt( ptyfd ) == -1 ){       //  if error changing mode...
		perror( "grantpt()" );
		exit(1);    //  #SCD create return code
	}

    /*  Unlocks the SPT corresponding to ptyfd, must be called before opening SPT  */
	if( unlockpt( ptyfd ) == -1 ){      //  if error unlocking SPT...
		perror( "unlockpt()" );
		exit(1);    //  #SCD create return code
	}

    /*  Open i2c port  */
	if( ( fd_i2c = open( i2c_devname , O_RDWR|O_NOCTTY ) ) == -1 ){
		perror( i2c_devname );          //  if error opening port...
		exit(1);    //  #SCD create return code
	}

    /*  Create new sc16 instance  */
	sc16is7xx = sc16is7xx_new(fd_i2c, i2c_addr, 0/*flags*/);
	if( !sc16is7xx ){                                                   //  if error creating instance...
		fprintf( stderr , "Could not create sc16is7xx instance.\n" );
		exit(1);    //  #SCD create return code
	}

    /*  Set baud rate of sc16  */
	if( sc16is7xx_set_baud( sc16is7xx , baudrate ) == -1 ){     //  if error setting baud...
		perror( "sc16is7xx_set_baud()" );
		exit(1);    //  #SCD create return code
	}

    /*  Setting GPIOs on sc16 ***NOT USED***  */
	if( gpio_out != -1 ){
		fprintf(stderr, "setting GPIO to 0x%04x\n", gpio_out & 0xff);
		for( i = 7 ; i >= 0 ; i-- )
			fprintf(stderr," D%d=%d", i, !!(gpio_out & (1<<i)));
		fprintf( stderr , "\n" );
		if( sc16is7xx_gpio_out( sc16is7xx , 0xff , gpio_out & 0xff ) == -1 ){
			perror( "sc16is7xx_gpio_out" );
			exit(1);    //  #SCD create return code
		}
	}

    /*  Print baud rate for debug purposes  */
	fprintf(stderr,"Baudrate is %d.\n\n", baudrate);    //#SCD debug
}

void loop(){

		FD_ZERO( &readfds );        //  clear read fd set
		FD_ZERO( &writefds );       //  clear write fd set
		nfds = 0;                   //

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

        /* Setup the timeout */
		to.tv_sec = 0;
		to.tv_usec = (rxlvl > 0) ? 0 : 1000; /* 1ms */

        /* Call select and return number of fds available   */
		i = select(nfds, &readfds, &writefds, NULL, &to);
		if( i == -1 ){                      //  if error with select()...
			perror( "select()" );
			exit(1);            //#SCD create return code
		}

		sc16is7xx_stats(sc16is7xx, &xfers, &txcounter, &rxcounter);

		/* For debug purposes print stats of sc16  */
		fprintf(stderr,"\r%u %u %u %4d %4d\r", xfers, txcounter, rxcounter, txlvl, rxlvl);
		fflush(stderr);

		/* amuse Marco */ //#SCD huh?
		if ((txlvl==64) && (rxlvl==0) && (gpio_out != -1)) {
			gpio_out = gpio_out ^ 0xff;
			if (sc16is7xx_gpio_out(sc16is7xx, 0xff, gpio_out & 0xff) == -1) {
				perror("sc16is7xx_gpio_out");
				exit(1);
			}
		}

		if (i == 0)         //  if nothing was on the '7xx receive buffer do nothing
			return;

		/* check if we can read from pty -> write to uart */
		if( FD_ISSET( ptyfd , &readfds) ){                      //  if readfds has been added to ptyfd
			i = read( ptyfd , buf_rx , txlvl );                 //   read from ptyfd store data in buf_rx
			if( i < 0 ){
				perror( "read(pty)" );
				exit(1);    //#SCD create return code
			}

            xwrite();

            /*  check to see if number of bytes read equals number of bytes written */
			if (i != j) {
				fprintf(stderr,"Wrote only %d of %d bytes to UART!\n",j, i);
			}
		}

        //  read the contents of the sc16is7xx register and write to buf
		if( FD_ISSET( ptyfd , &writefds ) ){                                //  check to see if writefds is contained in ptyfds
            xread();

			j = write(ptyfd, buf_rx, i);              //    write buf_rx to ptyfd

			if (j < 0) {
				perror("write(pty)");
				exit(1);
			}

			if (i != j) {
				fprintf(stderr,
					"Wrote only %d of %d bytes to pty.",
					j, i);
			}
		}
}


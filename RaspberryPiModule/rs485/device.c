#include "../config.h"
#include "devicehandler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>

#include "charactercode.h"
#include "packetformatter.h"
//#include "command.h"
#include "charactercode.h"
#include "io.h"



#ifndef BUFFERSIZE
#define BUFFERSIZE 512
#endif

#define RPIADDRESS 0xFE

#define ACK 0x8D
#define NAK 0x8C
#define ESCAPE 0x8E

#if defined ( RPI2TTY )
    #define SERIAL_PORT "/dev/ttyAMA0"
#else
    #define SERIAL_PORT "/dev/ttyS0"
#endif

//#define DEBUG

ssize_t kread(int fd, void *buf, size_t count);
int writeDevice(int devID, unsigned char command, unsigned char *message, unsigned int length, unsigned long timeO);


unsigned char debBuf[256];
unsigned char debptr = 0;


static int max_fd = 0;
static fd_set orig_fdset;
static struct timespec timeout;
static int fileDescriptor;

unsigned int badCnt = 0;
unsigned int goodCnt = 0;


void
debugit(unsigned char c)
{
  debBuf[debptr++] = c;
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
static int
open_port(char *name)
{
  int fd;
  struct termios options;

  fd = open(name, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
  {
    perror("open_port: Unable to open serial port - ");
  }
  else
  {
    // get the current options from fd
    tcgetattr(fd, &options);

    // set input and output baud rate
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

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
 * Function static void sendPoll(int)
 *
 * This function is called to poll device using ID.
 *
 * PreCondition:    None
 *
 * Input:           'devID' - ID of device where to issue polling.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *****************************************************************************/
static void
sendPoll(int devID)
{
  unsigned char buffer[2];
  buffer[0] = STARTPOLL;
  buffer[1] = devID;
  write(fileDescriptor, buffer, 2);
}

/******************************************************************************
 * Function static void sendChar(unsigned char)
 *
 * This function is called to send single character on the serial communication.
 *
 * PreCondition:    None
 *
 * Input:           'character' - character to send in serial communication.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *****************************************************************************/
static void
sendChar(unsigned char character)
{
  write(fileDescriptor, &character, 1);
}

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
void
initSerial(void)
{
  timeout.tv_sec = 0;
  timeout.tv_nsec = 5000000ul;

  fileDescriptor = open_port("/dev/ttyS0");
  //fileDescriptor = open_port("/dev/ttyUSB0");

  if(fileDescriptor <= 0)
  {
    printf("Error couldn't open %s" , "/dev/ttyS0" );
	exit(1);
  }

  FD_ZERO(&orig_fdset);
  FD_SET(fileDescriptor, &orig_fdset);

  max_fd = (fileDescriptor > max_fd)? fileDescriptor:max_fd;
}

/******************************************************************************
 * Function void getFileDescriptor(void)
 *
 * This function is called to get the file descriptor that is activated during the initialization.
 *
 * PreCondition:    initialization() must be called first.
 *
 * Input:           None
 *
 * Output:          file descriptor id
 *
 * Side Effects:    None
 *
 *****************************************************************************/
int
getFileDescriptor()
{
	return fileDescriptor;
}


/******************************************************************************
 * Function int writeDevice(int, Command, unsigned char*, unsigned char)
 *
 * This function is called to send a packet to serial line.
 *
 * PreCondition:    initialization() must be called first.
 *
 * Input:           'deviceID' - id of the slave device where to send the data.
 *                  'command'  - type of command to be send to slave device.
 *							               - command type:
 *												          READDEVICE
 *												          READVERSION
 *												          READDATEFIRMWARE
 *												          READDATA
 *												          WRITEDATA
 *												          WRITEFIRMWARE
 *												          PROGRAMFIRMWARE
 *					        'message' - data to be send to the slave device.
 *					        'length'  - the size of 'message' or data to be send in slave device.
 *
 * Output:          1 or greater - number of bytes written
 *					        0            - nothing has written
 *					       -1            - error
 *
 * Side Effects:    None
 *
 *****************************************************************************/
int
writeDevice(int devID, unsigned char command, unsigned char *message, unsigned int length, unsigned long timeO)
{
  int done;
  int tcount;
  int retVal;
  int byteCount = 0;
  int result;
  const unsigned char resendMax = 3;
  unsigned char resendCount = 0;
  unsigned char tMess[8];
  fd_set cur_fdset;
  struct timespec writeTimeout;

  Packet packet;
  packet.deviceID = devID;
  packet.sourceID = RPIADDRESS;
  packet.command = command; // getCommandChar(command);
  packet.packetLength = length;

  writeTimeout.tv_sec = 0;
  writeTimeout.tv_nsec = timeO;

  int index;
  for(index = 0; index < length; index++)
  {
    packet.packetData[index] = message[index];
  }

  unsigned char buffer[BUFFERSIZE];

  int resultSize = getFormattedPacket(&packet, buffer);

  #ifdef DEBUG
    printf("Size of packet = %d\n",resultSize);
  #endif

  retVal = 0;

  while(resendCount < resendMax)
  {
    retVal = write(fileDescriptor, buffer, resultSize);

    #ifdef DEBUG
      printf("Write return val: %d, resend count: %d\n", retVal, resendCount);
    #endif

    tcount = 0;
    done = 0;
    while (!done && retVal > 0)
    {
      cur_fdset = orig_fdset;

      result = kpselect(max_fd+1, &cur_fdset, NULL, NULL, &writeTimeout, NULL);
      //Read Feedback (ACKCHAR or NAKCHAR) from PIC on Serial Communication.
      if (result > 0)
      {
        byteCount = kread(fileDescriptor, tMess, 8);

        #ifdef DEBUG
          printf("Read %d bytes\n",byteCount);
        #endif

        if (byteCount > 0)
        {
          switch(tMess[0])
          {
            case ACKCHAR:
              done = 1;

              #ifdef DEBUG
                printf("writeDevice:ACK received from device %d.\n", devID);
              #endif

              return 1;
              break;
            case NAKCHAR:
              done = 1;

              //#ifdef DEBUG
              printf("writeDevice:NACK received from device %d.\n", devID);
              //#endif

              //return 0;
              break;
          }
        }
      }
      else if (result == 0)
      {
        tcount++;
        printf("writeDevice: timeout %d\n",tcount);
        if (tcount >= 10)
        {
          tcount = 0;
          done = 1;
        //return 0;
        }
      }
    }

    resendCount++;
  }
printf("Bad we failed resendCount = %d\n",resendCount);
  return 0;
}

/******************************************************************************
 * Function int readDevice(int, unsigned char*, int)
 *
 * This function is called to read a packet to serial line.
 *
 * PreCondition:    initialization() must be called first.
 *
 * Input:           'deviceID'    - id of the slave device where to read the data.
 *					        'retMessage'  - data buffer where to store the read data.
 *					        'length'      - the size of data to be read in serial line.
 *
 * Output:          1 or greater  - number of bytes written
 *					        0             - nothing has written
 *					       -1             - error
 *
 * Side Effects:    None
 *
 *****************************************************************************/
int
readDevice(int devID, unsigned char *retMessage, int length, unsigned long timeOut)
{
  int retVal;
  int byteCount = 0;
  int index;
  int result;
  int done;
  int tcount;
  unsigned char tempBuf[256];
  unsigned int rState = 0;
  unsigned int sum = 0;
  int tLength;
  int retMPtr;
  int escaped = 0;
  int nextstate;
  int retry;
  struct timespec readTimeout;

  fd_set cur_fdset;



  //Send poll to device before reading the data.
  sendPoll(devID);

  retry = 0;

  tcount = 0;
  done = 0;

  while (!done)
  {
    cur_fdset = orig_fdset;

    readTimeout.tv_sec = 0;
    readTimeout.tv_nsec = timeOut;

    retVal = kpselect(max_fd+1, &cur_fdset, NULL, NULL, &readTimeout, NULL);

    if (retVal < 0)
    {
      printf("************************* we have an error:%s\n", strerror(errno));
    }
    else if (retVal > 0)
    {
      byteCount = kread(fileDescriptor, tempBuf, 1);
      //tcflush(fileDescriptor, TCIOFLUSH);

      //for (result = 0; result < byteCount; result++)
      //{
      //  printf("0x%02X ",tempBuf[result]);
      //}
      //printf("\n");

#ifdef DEBUG
      printf("readDevice: byteCount = %d Char = 0x%02X\n",byteCount,tempBuf[0]);
#endif

      result = -1;
      index = 0;
      if(byteCount > 0)
      {
        tcount = 0;
        while (index < byteCount)
        {
          // If we escaped handle the escaped character
          if (escaped)
          {
            escaped = 0;
            switch(tempBuf[index])
            {
              case 0x8E:
                tempBuf[index] = 0x8E;
                break;
              case 0x01:
                tempBuf[index] = 0x7F;
                break;
              case 0x02:
                tempBuf[index] = 0x8F;
                break;
            }
            rState = nextstate;
          }
          // handle the input packet
          debugit('S');
          debugit(rState);
          debugit('C');
          debugit(tempBuf[index]);
          switch (rState)
          {
            case 0: // find start
              #ifdef DEBUG
                printf("readDevice: State = 0 0x%02X\n",tempBuf[index]);
              #endif
              if (tempBuf[index] == STARTPACKET)
              {
                retMPtr = 0;
                retMessage[retMPtr++] = STARTPACKET;
                sum = STARTPACKET;
                readTimeout.tv_nsec = 400000000ul;
                rState = 1;
              }
              else
              if (tempBuf[index] == ACK)
              {
                printf("readDevice - ERROR: Did get ack\n");
                return 0;
              }
              else
              {
                printf("readDevice - ERROR: Didn't get ack\n");
                //return -1;
              }
              break;
            case 1: // get device ID
              #ifdef DEBUG
                printf("readDevice: State = 1 0x%02X\n",tempBuf[index]);
              #endif
              if (tempBuf[index] == ESCAPE)
              {
                escaped = 1;
                nextstate = 11;
              }
              else
              {
                retMessage[retMPtr++] = tempBuf[index];
                sum += tempBuf[index];
                if (tempBuf[index] == RPIADDRESS)
                  rState = 2;
                else
                  rState = 0;
              }
              break;
            case 11:
              retMessage[retMPtr++] = tempBuf[index];
              sum += tempBuf[index];
              rState = 2;
              break;
            case 2: // get source ID
              #ifdef DEBUG
                printf("readDevice: State = 2 0x%02X\n", tempBuf[index]);
              #endif
              if (tempBuf[index] == ESCAPE)
              {
                escaped = 1;
                nextstate = 12;
              }
              else
              {
                retMessage[retMPtr++] = tempBuf[index];
                sum += tempBuf[index];
                rState = 3;
              }
              break;
            case 12:
              retMessage[retMPtr++] = tempBuf[index];
              sum += tempBuf[index];
              rState = 3;
              break;
            case 3: // get Command
              #ifdef DEBUG
                printf("readDevice: State = 2 0x%02X\n",tempBuf[index]);
              #endif
              if (tempBuf[index] == ESCAPE)
              {
                escaped = 1;
                nextstate = 13;
              }
              else
              {
                retMessage[retMPtr++] = tempBuf[index];
                sum += tempBuf[index];
                rState = 4;
              }
              break;
            case 13:
              retMessage[retMPtr++] = tempBuf[index];
              sum += tempBuf[index];
              rState = 4;
              break;
            case 4: // Payload Length MSB
              #ifdef DEBUG
                printf("readDevice: State = 3 0x%02X\n",tempBuf[index]);
              #endif
              if (tempBuf[index] == ESCAPE)
              {
                escaped = 1;
                nextstate = 14;
              }
              else
              {
                retMessage[retMPtr++] = tempBuf[index];
                sum += tempBuf[index];
                rState = 5;
                tLength = tempBuf[index];
              }
              break;
            case 14:
              retMessage[retMPtr++] = tempBuf[index];
              sum += tempBuf[index];
              rState = 5;
              tLength = tempBuf[index];
              break;
            case 5: // Payload Length LSB
              #ifdef DEBUG
                printf("readDevice: State = 4 0x%02X\n",tempBuf[index]);
              #endif
              if (tempBuf[index] == ESCAPE)
              {
                escaped = 1;
                nextstate = 15;
              }
              else
              {
                retMessage[retMPtr++] = tempBuf[index];
                sum += tempBuf[index];
                rState = 6;
                tLength = (tLength << 8) | tempBuf[index];
              }
              #ifdef DEBUG
                printf("readDevice: payload length = %d\n",tLength);
              #endif
              break;
            case 15:
              retMessage[retMPtr++] = tempBuf[index];
              sum += tempBuf[index];
              rState = 6;
              tLength = (tLength << 8) | tempBuf[index];
              break;
            case 6: // get payload
              #ifdef DEBUG
                printf("readDevice: State = 5 0x%02X\n",tempBuf[index]);
              #endif
              if (tempBuf[index] == ESCAPE)
              {
                escaped = 1;
                nextstate = 16;
              }
              else
              {
                retMessage[retMPtr++] = tempBuf[index];
                sum += tempBuf[index];
                tLength--;
                if (tLength <= 0)
                  rState = 7;
              }
              #ifdef DEBUG
                printf("readDevice: payload length = %d %d\n",tLength,rState);
              #endif
              break;
            case 16:
              retMessage[retMPtr++] = tempBuf[index];
              sum += tempBuf[index];
              tLength--;
              if (tLength <= 0)
                rState = 7;
              else
                rState = 6;
              break;
            case 7: // get check sum
              #ifdef DEBUG
                printf("readDevice: State = 6 0x%02X\n",tempBuf[index]);
              #endif
              if (tempBuf[index] == ESCAPE)
              {
                escaped = 1;
                nextstate = 17;
              }
              else
              {
                #ifdef DEBUG
                  printf("readDevice: State 6 of the decode state machine\n");
                #endif
                rState = 0;
                sum = sum & 0xFF;
                sum = (~sum)+1;
                sum = sum & 0xFF;
                retMessage[retMPtr++] = tempBuf[index];
                #ifdef DEBUG
                  printf("readDevice: checksum compare 0x%02X 0x%02X\n",tempBuf[index],sum);
                #endif
                if ((tempBuf[index] == sum))
                {
                  // send ack
                  sendChar(ACK);
                  return retMPtr;
                }
                else
                {
                  // send nak
                  retry++;
                  if (retry > 2)
                  {
                    printf("bad 0x%02X 0x%02X\n",sum,tempBuf[index]);
                    return -1;
                  }
                  sendChar(NAK);
                }
              }
              break;
            case 17:
              rState = 0;
              sum = sum & 0xFF;
              sum = (~sum)+1;
              sum = sum & 0xFF;
              retMessage[retMPtr++] = tempBuf[index];
              if ((tempBuf[index] == sum))
              {
                //#ifdef DEBUG
                printf("readDevice: Good 0x%02X 0x%02X\n",sum,tempBuf[index]);
                //#endif
                // send ack
                sendChar(ACK);
                return retMPtr;
              }
              else
              {
                // send nak
                //#ifdef DEBUG
                printf("readDevice: bad 0x%02X 0x%02X\n",sum,tempBuf[index]);
                //#endif
                sendChar(NAK);
                return -1;
              }
              break;
          }
          index++;
        }
      }
    }
    else //if (byteCount == 0)
    {
      // we have a timeout
      badCnt++;
      printf("\n\rreadDevice we have timed out %d %d %d %d\n",rState,retVal,goodCnt,badCnt);
      //printf("timeout = %d\n",timeOut);
      byteCount = kread(fileDescriptor, tempBuf, 1);
      printf("byteCount = %d\n",byteCount);
      // NOTE: this doesn't seem to work well.  It seems the
      // characters we need are in the buffer and we missed them
      // some how??  TODO need to fix this.
      //sendPoll(devID);
      //exit(1);

      tcount++;
      debugit('T');
      debugit(tcount);
      if (tcount >= 3)
      {
        printf("readDevice we have timed out 3 times\n");
        done = 1;
        result = 0;
        //while (1) ;
      }
    }
  }

  printf("readDevice - return result %d\n",result);
  return result;
}

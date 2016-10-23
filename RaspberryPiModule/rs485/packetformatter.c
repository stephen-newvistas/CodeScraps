#include <stdio.h>
#include <string.h>
#include "packetformatter.h"
#include "charactercode.h"
//#include "command.h"

//#define DEBUG

static unsigned int checkSum;
static unsigned int packetSize;
static unsigned char buffer[1024];

/******************************************************************************
 * Function ConvertToPacket( Packet *_packet , unsigned char *_buffer )
 *
 * This function is passed an empty packet a buffer with a message. The message
 * is broken down and placed into the packet.
 *
 *
 * PreCondition:    None
 *
 * Input:           '_packet' - empty packet , '_buffer' - message
 *
 * Output:
 *
 * Side Effects:    None
 *
 *****************************************************************************/
int ConvertToPacket( Packet *_packet , unsigned char *_buffer ){
	memset( _packet , '\0' , sizeof( Packet ) );

	_packet->deviceID = (int)_buffer[1] ;
  _packet->sourceID = (int)_buffer[2] ;
	_packet->command = _buffer[3];
	_packet->packetLength = _buffer[4];
	_packet->packetLength = _packet->packetLength << 8;
	_packet->packetLength |= _buffer[5];

	int i;
	for( i = 0 ; i < _packet->packetLength ; i++ ){
		_packet->packetData[i] = *( _buffer + i + 6 );
	}
	return 0;
}

/******************************************************************************
 * Function unsigned short escapedToSpecialChar(unsigned char)
 *
 * This function is called to convert a data if it is a special character (0x7F, 0x8F, 0x8E)
 * to equivalent escaped character.
 *
 *       e.g. Special Character      Escaped Character
 *              0x7F             =      0x8E 0x01
 *              0x8F             =      0x8E 0x02
 *              0x8E             =      0x8E 0x8E
 *
 * PreCondition:    None
 *
 * Input:           'c' - character to convert if it is an special character.
 *                  
 * Output:          Equivalent escaped character
 *
 * Side Effects:    None
 *
 *****************************************************************************/
unsigned short
specialToEscapedChar(unsigned char c)
{
  
  unsigned short escapedChar = 0;
  char isSpecialChar = ((STARTPOLL == c) || (STARTPACKET == c) || (FLAGBYTE == c));

  if(isSpecialChar)
  {
    escapedChar = (FLAGBYTE << 8);
   switch(c)
   {
     case STARTPACKET:
	   escapedChar |= 0x01;
       break;
     case STARTPOLL:
       escapedChar |= 0x02;
       break;
     case FLAGBYTE:
       escapedChar |= FLAGBYTE;
       break;	   
   }
  }
  else
  {
    escapedChar = (unsigned char) c;
  }
  
  return escapedChar;
}

/******************************************************************************
 * Function static void insertValueToBuffer(unsigned char)
 *
 * This function is called to insert the value to buffer.
 *
 * PreCondition:    None
 *
 * Input:           'value' - value to insert in the buffer.
 *                  
 * Output:          None
 *
 * Side Effects:    Increment the packetSize variable
 *
 *****************************************************************************/
static void 
insertValueToBuffer(unsigned char value)
{
  buffer[packetSize] = value;
  packetSize++;
}

/******************************************************************************
 * Function static void processPacketData(unsigned char)
 *
 * This function is called to process the packet if it is a special character it will
 * convert to escaped character e.g. 0x7F = 0x8E 0x01, 0x8E = 0x8E 0x8E, 0x8F = 0x8E 0x02.
 * The size of return value of specialToEscapedChar function is 2 bytes so it must be inserted
 * in the buffer twice.
 *
 * PreCondition:    None
 *
 * Input:           'packetData' - value to convert if it is a special character.
 *                  
 * Output:          None
 *
 * Side Effects:    None
 *
 *****************************************************************************/
static void
processPacketData(unsigned char packetData)
{

  unsigned short packetDataResult = specialToEscapedChar(packetData);
  
  if( (packetDataResult >> 8) != 0)
  {
    insertValueToBuffer((unsigned char)(packetDataResult>> 8));
  }
  
  insertValueToBuffer((unsigned char) packetDataResult);
  
}

/******************************************************************************
 * Function void getFormattedPacket(Packet *, unsigned char *)
 *
 * This function is called to process the packet in to a series of bytes that
 * conforms into the protocol of the communication.
 * Protocol Format Example: 
 *                          0x7F - Start Packet
 *                          0xFE - Device ID
 *                          0x01 - Source ID
 *                          0x05 - Command type
 *                          0x00 - Payload length 1st byte
 *                          0x01 - Payload length 2nd byte
 *                          0x04 - Payload or data
 *                          0x76 - checksum
 *                           
 *
 * PreCondition:    None
 *
 * Input:           'packet' - instance of Packet data structure that contains the 
 *                             following information:
 *                                                    + device id
 *                                                    + source id
 *                                                    + command
 *                                                    + packet or payload length
 *                                                    + packet or payload
 *
 *                  'packetBuffer' - empty buffer that will be the holder of the process data or packet.
 *                  
 * Output:          Total size of data inserted in the buffer.
 *
 * Side Effects:    None
 *
 *****************************************************************************/
int getFormattedPacket(Packet * packet, unsigned char * packetBuffer)
{
  
  checkSum = 0;
  packetSize = 0;
  
  insertValueToBuffer(STARTPACKET);
  checkSum += STARTPACKET;
  
  #ifdef DEBUG
  printf("Start Packet: %x\n", STARTPACKET);
  #endif
  
  processPacketData(packet->deviceID);
  checkSum += packet->deviceID;
  
  #ifdef DEBUG
  printf("Device ID: %x\n", packet->deviceID);
  #endif
  
  processPacketData(packet->sourceID);
  checkSum += packet->sourceID;
  
  #ifdef DEBUG
  printf("Source ID: %x\n", packet->sourceID);
  #endif
  
  processPacketData(packet->command);
  checkSum += packet->command;
  
  #ifdef DEBUG
  printf("Command: %x\n", packet->command);
  #endif
  
  unsigned char mostSignificantBytePacketLength = packet->packetLength >> 8;
  unsigned char leastSignificantBytePacketLength = (unsigned char) packet->packetLength;
  processPacketData(mostSignificantBytePacketLength);
  checkSum += mostSignificantBytePacketLength;
  processPacketData(leastSignificantBytePacketLength);
  checkSum += leastSignificantBytePacketLength;
  
  #ifdef DEBUG
  printf("Packet length MSB: %x\n", mostSignificantBytePacketLength);
  printf("Packet length LSB: %x\n", leastSignificantBytePacketLength);
  #endif
  
  int i;
  for(i = 0; i < packet->packetLength; i++)
  {
    processPacketData(packet->packetData[i]);
	checkSum += packet->packetData[i];
	
	#ifdef DEBUG
	printf("Data %d: %x\n", i, packet->packetData[i]);
	#endif
  }
  
  unsigned char checksumLSB = (unsigned char)(checkSum & 0xFF);
  unsigned char twosComplementCheckSum = ~checksumLSB + 1;
  processPacketData(twosComplementCheckSum);
  
  #ifdef DEBUG
  printf("Checksum: %x\n", twosComplementCheckSum);
  #endif
  
  memcpy(packetBuffer, buffer, packetSize);
  
  return packetSize;
}

/******************************************************************************
 * Function void getFormattedBootloaderPacket(unsigned char *, unsigned char *, int)
 *
 * This function is called to process the data in to a series of bytes that
 * conforms into the bootloader protocol.
 * Protocol Format Example: 
 *                          0x55 - STX or Start of TeXt
 *                          0x55 - STX or Start of TeXt
 *                          0x01 - Data
 *                          0x03 - Data
 *                          0x0C - Checksum
 *                          0x04 - ETX or End of TeXt
 *                           
 *
 * PreCondition:    None
 *
 * Input:           
 *                  'buffer' - empty buffer that will be the holder of the process.
 *                  'data'   - values to be send in bootloader.
 *                  'size'   - size of data to be send.
 *                  
 * Output:          Total size of data inserted in the buffer.
 *
 * Side Effects:    None
 *
 *****************************************************************************/
int 
getFormattedBootloaderPacket(unsigned char *buffer, unsigned char *data, int size)
{
  int i;
  int j;
  int sum;
  
  i = 0;
  buffer[i++] = STX;
  buffer[i++] = STX;
  sum = 0;
  
  for(j = 0; j < size; j++)
  {
    if(data[j] == ETX || data[j] == STX || data[j] == ENQ)
    {
      buffer[j+i++] = ENQ;
    }
	
    buffer[j+i] = data[j];
    sum += data[j];
  }
  
  sum = ((sum * -1) & 0xFF);
  if (sum == ETX || sum == STX || sum == ENQ)
    buffer[j+i++] = ENQ;

  buffer[j+i] = sum;
  j++;
  buffer[j+i] = ETX;
  j++;

  //printf("getFormattedBootloaderPacket return = %d\n",j+i);
  
  return (j+i);
}



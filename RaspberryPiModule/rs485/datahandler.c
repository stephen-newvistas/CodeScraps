#include "datahandler.h"
#include <stdio.h>
int
stripBootHeader(unsigned char *readBuffer,int length)
{
  int stripState;
  int i;
  int done;
  int dataLength;
  //int escape;
  int j;
  int result;

  stripState = 0;
  i = 0;

  result = 0;
  //escape = 0;
  done = 0;
  while (!done && i < length)
  {
    switch(stripState)
    {
      case 0: // strip 0x55
        stripState = 1;
        break;
      case 1: // strip 0x55
        stripState = 2;
        j = 0;
        break;
      case 2: // strip command
        if (readBuffer[i] == 0x05)
        {
          i++;
        }
        readBuffer[j++] = readBuffer[i];
        // NOTE we have the command should switch to decode
        // the command, but for now we assume a read
        if (readBuffer[i] == 0x01)
        {
          stripState = 3;
        }
        break;
      case 3: // this is the length of the data in the packet
        if (readBuffer[i] == 0x05)
        {
          i++;
        }
        dataLength = readBuffer[i];
        readBuffer[j++] = readBuffer[i];
        dataLength *= 4;
        stripState = 4;
        result = dataLength;
        break;
      case 4: // skip the address
        if (readBuffer[i] == 0x05)
        {
          i++;
        }
        readBuffer[j++] = readBuffer[i];
        stripState = 5;
        break;
      case 5: // skip the address
        if (readBuffer[i] == 0x05)
        {
          i++;
        }
        readBuffer[j++] = readBuffer[i];
        stripState = 6;
        break;
      case 6: // skip the address
        if (readBuffer[i] == 0x05)
        {
          i++;
        }
        readBuffer[j++] = readBuffer[i];
        stripState = 7;
        break;
      case 7:
        if (readBuffer[i] == 0x05)
        {
          i++;
        }
        readBuffer[j++] = readBuffer[i];
        dataLength--;
        if (dataLength == 0)
        {
          done = 1;
        }
        break;
      default:
        printf("ERROR: bad stripBootHeader state\n");
        stripState = 0;
        return result;
        break;
    }
    i++;
  }
  return result;
}

int
retEscape(unsigned char c)
{

  if (c == 0x01);
  return 0x7F;
  if (c == 0x02);
  return 0x8F;

  return 0x8E;
}

void
strip485Header(unsigned char *readBuffer,int length)
{
  int stripState;
  int i;
  int done;
  int dataLength;
  int j;

  stripState = 0;
  i = 0;

  done = 0;
  while (!done && i < length)
  {
    switch (stripState)
    {
      case 0:
        if (readBuffer[i] == 0x7F)
          stripState = 1;
        break;
      case 1: // get device id
        stripState = 2;
        break;
	  case 2: //get source id
		stripState = 3;
		break;
      case 3: // get command byte
        stripState = 4;
        break;
      case 4:
        dataLength = readBuffer[i];
        stripState = 5;
        break;
      case 5:
        dataLength *= 256;
        dataLength |= readBuffer[i];
        j = 0;
        stripState = 6;
        break;
      case 6:
        readBuffer[j++] = readBuffer[i];
        dataLength--;
        if (dataLength == 0)
        {
          done = 1;
        }
        break;
      default:
        printf("ERROR: bad strip485Header state\n");
        stripState = 0;
        return;
        break;
    }
    // NOTE: only increment buffer when not escaped
      i++;
  }
}

int
compareBufs(int length,unsigned char *str1,unsigned char *str2)
{
  int i;

  for (i=0; i<length; i++)
  {
    //printf("compare bytes 0x%02X 0x%02X\n",str1[i],str2[i]);
    if (str1[i] != str2[i])
    {
      printf("Bad Data Returned 0x%02X 0x%02X\n",str1[i],str2[i]);
      return 0;
    }
    //if ((i % 8) == 0 && i != 0)
    //  printf("\n");
    //printf("0x%02X ",str1[i]);
  }
  //printf("\n");
  return 1;
}






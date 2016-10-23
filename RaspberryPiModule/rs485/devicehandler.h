#ifndef DEVICEHANDLER
#define DEVICEHANDLER

//#include "command.h"

#define WRITEATTEMPTS 3

extern void initSerial(void);
extern int getFileDescriptor();
extern int writeDevice(int devID, unsigned char command, unsigned char *message, unsigned int length, unsigned long timeO);
extern int readDevice(int devID, unsigned char *retMessage, int length, unsigned long timeOut);

#endif //DEVICEHANDLER

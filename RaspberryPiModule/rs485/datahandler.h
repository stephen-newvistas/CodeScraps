#ifndef DATAHANDLER
#define DATAHANDLER

int stripBootHeader(unsigned char *readBuffer,int length);
int retEscape(unsigned char c);
void strip485Header(unsigned char *readBuffer,int length);
int compareBufs(int length,unsigned char *str1,unsigned char *str2);
#endif

#ifndef LOOPS_H_INCLUDED
#define LOOPS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>

//	loops.h
void Loops_Init( void*(*f1)(void) , void*(*f2)(void) , void*(*f3)(void) , void*(*f4)(void) , void*(*f5)(void) );
void Loops_Timer();

#endif // LOOPS_H_INCLUDED

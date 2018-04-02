#ifndef __MESSAGES_H
#define __MESSAGES_H

#define ERROR_MOD_PRI_INEX 0
#define ERROR_MEM_INSUF 1

extern char * const messages[];

char * const getMessage(int index);

//https://regex101.com/r/Ba1g47/1/
//principal(){} 
#define RGX_PRINCIPAL "^(principal\\(\\))((\\R)?(\\{))(\\R|.)*(\\})$"


#endif
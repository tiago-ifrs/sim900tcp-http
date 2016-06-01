
#ifndef _GPRS_H
#define _GPRS_H
#include <SoftwareSerial.h>
extern char bufGprs[_SS_MAX_RX_BUFF];
void lerGprs(void);
void resetGprs(void);
#endif

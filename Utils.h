#ifndef UTILS_h
#define UTILS_h

#include <Arduino.h>
#include <EEPROM.h>

/*#define DEBUG_SENDDATA
#define DEBUG_READTEMP
#define DEBUG_POWER
#define DEBUG_CLOCK
#define DEBUG_ALARMS
#define DEBUG_READTEMP
#define DEBUG_SERVER
#define DEBUG_NOIP
#define DEBUG_MENU
#define DEBUG_WDT
#define DEBUG_GETADDRESS*/
#define DEBUG_MONITOR

void	WDTReset();

bool	ethernetConnect();
bool	xyvelyEnabled();
bool	alarmsEnabled();

bool	checkBit(byte in, int bit);

void	showMemory();
int getFreeMem();

String  FormatTemp(float V);

void	expanderSetInput(int i2caddr, byte dir);
void	expanderWrite(int i2caddr, byte data);
byte	expanderRead(int i2caddr);

#endif




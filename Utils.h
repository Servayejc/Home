#ifndef UTILS_h
#define UTILS_h

#include <Arduino.h>
#include <EEPROM.h>



bool	ethernetConnect();
bool	xyvelyEnabled();
bool	alarmsEnabled();

bool	checkBit(byte in, int bit);

void	showMemory();
String  FormatTemp(float V);

void	expanderSetInput(int i2caddr, byte dir);
void	expanderWrite(int i2caddr, byte data);
byte	expanderRead(int i2caddr);

#endif




#ifndef _HCONFIG_h
#define _HCONFIG_h

#include <Task.h>
#include "Globals.h"
#include "Utils.h"

struct _group {
	byte time[4];
	byte setpoint[4];
};

struct _channel {
	_group group[2];
};

struct _config {
	_channel channel[6];
};


class HConfig {
public:
   HConfig();
   void setDefaultConfig();
   void loadFromEEPROM();
   void saveToEEPROM();
   String getTimeString(byte channel, byte group, byte ndx);
   byte getTime(byte channel, byte group, byte ndx);
   void setTime(byte channel, byte group, byte ndx, byte value);  
   byte getSetPoint(byte channel, byte group, byte ndx); 
   void setSetPoint(byte channel, byte group, byte ndx, byte value);	
   void setPair(byte EditIndex, String tag, String val);
   _config data;
 private:
  byte calcTime(byte H, byte M);
  
};


extern HConfig Config;

#endif

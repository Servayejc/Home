#ifndef _ALARMS_h
#define _ALARMS_h

#include "Arduino.h"


class AlarmClass
{
 protected:
	long      Time;
	int      Priority;
	String	  Text;	
	void  init();	  
 public:
	AlarmClass(byte aPriority, String aText);
	String GetMessage();
};


extern AlarmClass Alarm;

#endif



 


  
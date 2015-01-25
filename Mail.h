#ifndef MAIL_h
#define MAIL_h

#include <Task.h>
#include "Globals.h"


class SendAlarm :
public TimedTask
{
public:
	SendAlarm(uint32_t _rate);
	void  init();
	void  run(uint32_t now);
	bool canRun(uint32_t now);
	void logToXively();
	void addAlarm(String Ala);
	void addEvent(String Event);
	String GetAlarms();
	String GetEvents();
private:
	uint32_t rate;
	String AlarmText;
	String EventText;
	bool     on;
};
#endif



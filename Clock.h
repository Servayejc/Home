#ifndef CLOCK_h
#define CLOCK_h
#include <RTClib.h>

#include <Task.h>

#include <inttypes.h>

typedef unsigned long   time_t;

typedef struct  {
	uint8_t Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Wday;   // day of week, sunday is day 1
	uint8_t Day;
	uint8_t Month;
	uint16_t Year;
	// uint8_t Days;
	// offset from 1970;
} 	tmElements_t, TimeElements, *tmElementsPtr_t;


class Clock :
public TimedTask
{
public:
	Clock(uint32_t _rate);
	long   RealTime;
	void  init();
	void  run(uint32_t now);
	void  NTPUpdate();
	void	breakTime(time_t timeInput, tmElements_t &tm, int Offset);
	String	GetTimeString();
	String	GetDateString();
	String	GetDateTimeString();
    void readBytesInRam(uint8_t address, uint8_t length, uint8_t* p_data);
	void writeBytesInRam(uint8_t address, uint8_t length, uint8_t* p_data);
private:
	void     sendNTPpacket(IPAddress &address);
	time_t   getNtpTime();
	uint32_t rate;
	bool     on;
	void     readTime();
	byte     Day;
	int      timeZone;
};

#endif

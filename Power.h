#ifndef POWER_h
#define POWER_h

#include <Task.h>
#include "Globals.h"

class Power :
public TimedTask
{
public:
	Power(uint32_t _rate);
	void  init();
	void  run(uint32_t now);
private:
	void calcEnergy(int channel, long seconds);
	uint32_t rate;
	byte  OldValue;
	byte  count;
	bool  on;
	byte  data;
	byte  oldData;
	int	  TotalPower;
	long  time[8];
	int   Pow[8];
	int   Energy[8];
	void  ReadPower();
};

#endif

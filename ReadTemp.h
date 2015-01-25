#ifndef READTEMP_h
#define READTEMP_h

#include <Task.h>
#include "Globals.h"

// OneWire channels

//static OneWire ds(OneWireReadAddress);


// Read temperatures task
class ReadTemp :
public TimedTask
{
public:
	ReadTemp(uint32_t _rate);
	void  init();
	void  run(uint32_t now);

	void  startConversion();
	void  getAddress();
	bool  HasData();
	String GetData();
private:
	float readOneTemp(byte Ndx);
	uint32_t rate;
	bool     on;
	int      data[9];
	int	  NT;
};
#endif

#ifndef REGULATION_H_
#define REGULATION_H_

#include <PID_v1.h>
#include <Task.h>


#define RelayPin 6

//Define Variables we'll be connecting to
double Setpoint[2], Input[2], Output[2];

PID PIDs[2] = {
	PID(&Input[0], &Output[0], &Setpoint[0],2,5,1, DIRECT),
	PID(&Input[1], &Output[1], &Setpoint[1],2,5,1, DIRECT)
};



unsigned long WindowSize = 5000;
unsigned long windowStartTime;




class Regulation :
public TimedTask
{
	public:
	// Create a new ReadTemp for the specified pin and rate.
	Regulation(uint32_t _rate);
	void  init();
	void  run(uint32_t now);
	bool canRun(uint32_t now);
	private:
	uint32_t rate;
};


#endif /* REGULATION_H_ */
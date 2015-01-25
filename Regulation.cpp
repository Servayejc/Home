#include "Regulation.h"
#include <Arduino.h>
#include "Utils.h"
#include "Globals.h"


Regulation::Regulation(uint32_t _rate) :
TimedTask(millis()),
rate(_rate)
{
}

void Regulation::init()
{
	//turn the PIDs on
	for (byte i = 0; i < 2; i++)
	{
		PIDs[i].SetMode(AUTOMATIC);
		Setpoint[i] = 100;
	}
}

bool Regulation::canRun(uint32_t now)
{
	return (now >= runTime) & runFlag;
}

void Regulation::run(uint32_t now)
{
	windowStartTime = now;
	for (byte i = 0; i < 2; i++){
		Setpoint[i] = 100;
		Input[i] = analogRead(i);
		PIDs[i].Compute();
		Serial.println(Output[i]);
	}

	/* if(millis() - windowStartTime > WindowSize)
	{ //time to shift the Relay Window
	windowStartTime += WindowSize;
	}
	if(Output < millis() - windowStartTime) digitalWrite(RelayPin,HIGH);
	else digitalWrite(RelayPin,LOW);*/
	
	incRunTime(rate);
}


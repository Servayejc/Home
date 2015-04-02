#include <Arduino.h>
#include <Wire.h>
#include <MemoryFree.h>
#include <UIPEthernet.h>
#include <OneWire.h>
#include <avr/wdt.h>
#include "Globals.h"
#include "Utils.h"


//OneWire ds(OneWireReadAddress);
//OneWire ds2(OneWireNetwork);

void WDTReset() {
	wdt_reset();	
}


bool ethernetConnect()
{
	if (xyvelyEnabled() || alarmsEnabled()) {
		if (!EthernetConnected){
			if (Ethernet.begin(mac) != 0) {
				EthernetConnected = true;
			}
		}
	}
	return (EthernetConnected);
}

bool xyvelyEnabled()
{
	return true; //((readSetup() && ENABLEXYVELY) == 0);
}

bool alarmsEnabled()
{
	return true; //((readSetup() && ENABLEALARMS) == 0);
}

bool checkBit(byte in, int bit)
{
	return in & (1 << bit);
}

void showMemory()
{
	Serial.print("Memory used = ");
	Serial.print(0x4000 - freeMemory());
	Serial.println(" of 16384");
};

String FormatTemp(float V)
{
	int      FirstPart = (int) V / 100;
	int      LastPart = (int) V % 100;
	String   R = " ";
	R += FirstPart;
	R += ".";
	R += abs(LastPart);
	return R;
};

// I2C routines to talk to 8574 and 8574A
/* ===================== I2C Expender Set Input mode ====================*/
void expanderSetInput(int i2caddr, byte dir) {
	Wire.beginTransmission(i2caddr);
	Wire.write(dir);  // outputs high for input
	Wire.endTransmission();
}

/* ===================== I2C Expender Write Data ====================*/
void expanderWrite(int i2caddr, byte data){
	Wire.beginTransmission(i2caddr);
	Wire.write(data);
	Wire.endTransmission();
}

/* ===================== I2C Expender Read Data ====================*/
byte expanderRead(int i2caddr) {
	byte i2cdata = 0;
	Wire.requestFrom(i2caddr, 1, true);
	if(Wire.available()) {
		i2cdata = Wire.read();
	}
	return  i2cdata;
	Serial.println(i2cdata);
}



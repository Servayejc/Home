#include <EEPROMEx.h>
#include "HConfig.h"
#include "Globals.h"
#include "Utils.h"

HConfig::HConfig()
{
}

byte HConfig::calcTime(byte H, byte M) {
	return H*6 + M/10; // period of 10 minutes
}

void HConfig::setDefaultConfig() {
	for (byte c = 0; c < 6; c++) {
		for (byte g = 0; g < 2; g++) {
			setTime(c, g, c, calcTime(6,00));
			setSetPoint(c, g, c, 22);
			setTime(c, g, c, calcTime(7,00));
			setSetPoint(c, g, c, 18);
			setTime(c, g, c, calcTime(16,00));
			setSetPoint(c, g, c, 22);
			setTime(c, g, c, calcTime(22,00));
			setSetPoint(c, g, c, 18);
		}
	}
}

void HConfig::loadFromEEPROM() {
	EEPROM.readBlock<_config>(0, data); 
}

void HConfig::saveToEEPROM() {
	EEPROM.writeBlock<_config>(0, data);
}

 byte HConfig::getSetPoint(byte channel, byte group, byte ndx) {
    return data.channel[channel].group[group].setpoint[ndx];
} 

void HConfig::setSetPoint(byte channel, byte group,  byte ndx,  byte value){
	data.channel[channel].group[group].setpoint[ndx] = value;
}	  

void HConfig::setTime(byte channel,  byte group,  byte ndx,  byte value) {
   data.channel[channel].group[group].time[ndx] = value;
}

byte HConfig::getTime(byte channel,  byte group,  byte ndx) {
  return data.channel[channel].group[group].time[ndx];
}

String HConfig::getTimeString(byte channel, byte group, byte ndx) {
	byte x = data.channel[channel].group[group].time[ndx];
	
	byte H = x/6;
	byte M = x % 6;
	String result(' ');
	if (H < 10)
	{
		result += '0';
		result += H;
	}
	else
	{
		result += H;
	}
	
	result += ':';
	
	if (M > 0)
	{
		result += M*10;
	}
	else
	{
		result += "00";
	}

	return result;
}

void HConfig::setPair(byte EditIndex, String tag, String val) {
	int value;
	int tagNum = tag.toInt();
	if (tagNum < 100) {
		int ndx = val.indexOf("%3A"); // %3A = ':'
		//   Serial.println(val);
		if (ndx > -1) {
			int H = val.substring(0,ndx).toInt();
			int M = val.substring(ndx+3, val.length()).toInt();
			value = calcTime(H,M);
		}
		else
		{
			value = val.toInt();
		}
		
		byte x = tagNum % 10 -1;
		switch (tagNum / 10) {
		case 1 :
			setTime(EditIndex,0,x,value);
			break;
		case 2 :
			setSetPoint(EditIndex,0,x,value);
			break;
		case 3 :
			setTime(EditIndex,1,x,value);
			break;
		case 4 :
			setSetPoint(EditIndex,1,x,value);
			break;
		}
	}
}

HConfig Config;
#ifndef MENU_h
#define MENU_h

#include <Task.h>
#include "Globals.h"


class Menu:
public TimedTask
{
public:
	Menu(uint32_t _rate);
	void  init();
	void  run(uint32_t now);
	void  LCDSetMenu(String aMenuText);
	void  LCDSetStatus(String aStatus);
	void  LCDSetTime();
	void  LCDSetAlarm();
	void  getAddress();
	void  BlinkLed14();
	void  setLed(int Ndx);
private:
	uint32_t rate;
	byte  OldValue;
	byte  count;
	byte  UpdateTimeCounter;
	bool  LCD_Enabled;
	bool  on;
	bool  Led14Status;
	int   MenuPos;
	int   Index;
	int   ShowTimeCounter;
	
    void  ShowTemperature();
	void  IsInstalled();
	void  Execute();
	void  InitHardware();
	void  fillLine(int ndx);
	byte  readSetup();
};
#endif

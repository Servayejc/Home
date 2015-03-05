#ifndef _GLOBALS_h
#define _GLOBALS_h

#include <Arduino.h>
#include <LiquidTWI2.h>
#include <UIPEthernet.h>
#include <OneWire.h>
#include "Clock.h"
#include "Menu.h"
#include "ReadTemp.h"
#include "Power.h"
#include "SendData.h"
#include "Mail.h"
#include "credentials.h"
#include "NoIP.h"
#include "Server.h"

class WebSvrClass;
class Menu;
class ReadTemp;
class Power;
class SendData; 
class SendAlarm;
class Clock;
class NoIPUpdater;
class WebServer;
class HConfig;



// arduino inputs used for OneWire
#define SummerTime				4 // 0 = Normal time
#define OneWireNetwork			5
#define OneWireReadAddress		6
#define TestMode				7 // 0 = Test
#define LCDEnable				8 // 0 = LCD enabled

// number of thermometers
#define SensorsCount			5

// 8574		range is 0x20-0x27   8 I/O
// 8574A	range is 0x38-0x3F	 8 I/O
// MC23017	range is 0x20-0x27	16 I/O

#define ExpanderLowAdd			0x20	// 8574 (1) X0-X7		
#define ExpanderHightAdd		0x21	// 8574 (2) X8-X15		
#define LedAddress				0x22	// MCP23017 LEDs

// Menu
#define LiquidTWIOffset			7		// 0x27
#define MenuItemsCount			17

// Leds
#define MenuLed					0
#define ClockLed				1
#define ReadTempLed				2
#define ReadPowerLed			3
#define SendDataLed				4
#define SendAlarmsLed			5
#define NoIPLed					6
#define ServerLed				7

//Global data
extern byte mac[];
extern float Temperatures[SensorsCount];		
extern float TT[SensorsCount];					
extern const byte Sensors[SensorsCount][8];		
extern byte OnWireAddress[9];					
extern const char *MenuTexts[MenuItemsCount];
extern const byte MenuStruct[MenuItemsCount][5];
extern bool EthernetConnected;
extern OneWire ds;
extern OneWire ds2;
extern LiquidTWI2 lcd;
extern UIPClient client;


extern Menu menu;
extern Clock clock;
extern Power power;
extern ReadTemp readT;
extern SendData sendData;
extern SendAlarm sendAlarm;
extern NoIPUpdater NoIP;
extern WebSvrClass WebSvr;


#endif


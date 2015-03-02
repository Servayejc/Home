//#include <EEPROMVar.h>
#include <EEPROMex.h>
#include <HttpClient.h>
#include <b64.h>

//#include "WebServer.h"
#include "NoIP.h"
#include "Credentials.h"
#include "Globals.h"
#include <Task.h>
#include <TaskScheduler.h>
#include <OneWire.h>
#include <Wire.h>
#include <LiquidTWI2.h>
#include <Temboo.h>
#include <SPI.h>
#include <MemoryFree.h>
#include <UIPEthernet.h>
#include <RTClib.h>
#include <avr/wdt.h>

#include "Utils.h"
#include "ReadTemp.h"
#include "Clock.h"
#include "SendData.h"
#include "Mail.h"
#include "Power.h"
#include "Menu.h"
#include "NoIP.h"
#include "Server.h"
#include <PID_v1.h>

void setup() {
	Serial.begin(115200);
	ethernetConnect();
	pinMode(14, OUTPUT);
	//setDefaultConfig();
	//EEPROM.writeBlock<_config>(0, config);
	EEPROM.readBlock<_config>(0, config);
}

// Main program.
void loop() {
    WebServer	ArduinoServer(200);
	// Initialize the tasks
	
	readT.init();
	readT.getAddress();
	power.init();
	menu.init();
	clock.init();
	sendData.init();
	sendAlarm.init();
	NoIP.init();
	ArduinoServer.init();

	// Start the tasks
	menu.setRunnable();
	clock.setRunnable();
	readT.setRunnable();
	power.setRunnable();
	sendData.setRunnable();
	sendAlarm.setRunnable();
	NoIP.setRunnable();
	ArduinoServer.setRunnable();

	//set the watchdog to 8 second
	wdt_enable(WDTO_8S);
	Serial.println("Watchdog enabled.");

	// Initialize the scheduler.
	Task *tasks[] = {/* &clock,*/ &ArduinoServer/* , &sendData, &sendAlarm,  &readT, &power, &menu, &NoIP*/ };
	TaskScheduler sched(tasks, NUM_TASKS(tasks));

	// Run the scheduler - never returns.
	//showMemory();
	//delay(2000);
	sched.run();
}


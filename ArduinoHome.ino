#include "jc.h"
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
#include <PID_v1.h>

void setup() {
	Serial.begin(115200);
	ethernetConnect();
	pinMode(14, OUTPUT);
}

// Main program.
void loop() {
	//set the watchdog to 8 second
	wdt_enable(WDTO_8S);
	Serial.println("Watchdog enabled.");
	
    // Initialize the tasks
	readT.init();
	readT.getAddress();
	power.init();
	menu.init();
	clock.init();
	sendData.init();
	sendAlarm.init();

	// Start the tasks
	menu.setRunnable();
	clock.setRunnable();
	readT.setRunnable();
	power.setRunnable();
	sendData.setRunnable();
	sendAlarm.setRunnable();

	// Initialize the scheduler.
	Task *tasks[] = { &clock, &sendData, &sendAlarm,  &readT, &power, &menu, };
	TaskScheduler sched(tasks, NUM_TASKS(tasks));

	// Run the scheduler - never returns.
	showMemory();
	delay(2000);
	sched.run();
}


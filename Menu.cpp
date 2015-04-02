#include <Arduino.h>
#include <Wire.h>
#include "Menu.h"
#include "Globals.h"
#include "Utils.h"
#include <avr/wdt.h>

extern float   Temperatures[5];			// ????
extern byte    OnWireAddress[9];		// ????
extern String  Time;					// ????

Menu::Menu(uint32_t _rate) :
TimedTask(millis()),
	rate(_rate),
	on(false)
{
}

void Menu::IsInstalled(){
	// Test if LCD-LED-Button board is connected
	pinMode(LCDEnable,INPUT_PULLUP);
	LCD_Enabled = digitalRead(LCDEnable) == LOW;
}

void Menu::InitHardware(){
	lcd.setMCPType(LTI_TYPE_MCP23017);
	// set up the LCD's number of rows and columns:
	lcd.begin(20, 4);

	// Set port A as Output for LEDs
	Wire.beginTransmission(LedAddress);
	Wire.write(MCP23017_IODIRA);
	Wire.write(0x00);  // all inputs on port B
	Wire.endTransmission();
	// Set port B as input with pull-up for settings
	Wire.beginTransmission(LedAddress);
	Wire.write(MCP23017_IODIRB);
	Wire.write(0xFF);  // all inputs on port B
	Wire.endTransmission();
	Wire.beginTransmission(LedAddress);
	Wire.write(MCP23017_GPPUB);
	Wire.write(0xFF);  // all pull up on port B
	Wire.endTransmission();
}

void Menu::init()
{
	IsInstalled();
	Serial.println("Menu");
	if (LCD_Enabled) {
		InitHardware();

		OldValue = 0;
		MenuPos = 2;
		// set default menu
		LCDSetMenu(MenuTexts[MenuPos]);
		count = 0;
		menu.ShowTimeCounter = 0;
	}
}


void Menu::run(uint32_t now)
{
	wdt_reset();
	if (LCD_Enabled) {
		setLed(MenuLed);
		UpdateTimeCounter ++;
		if (UpdateTimeCounter >= 10) {
			UpdateTimeCounter = 0;
			LCDSetTime();
		}
		// read MCP23017 portA (buttons)
		byte  n = lcd.readButtons();
		if (n != OldValue)
		{
			OldValue = n;
            Serial.print(n);
			
			byte  OldMenuPos = MenuPos;
			if (checkBit(n, 0)) MenuPos = MenuStruct[MenuPos][1];	// S
			if (checkBit(n, 1)) MenuPos = MenuStruct[MenuPos][0];	// E
			if (checkBit(n, 2)) MenuPos = MenuStruct[MenuPos][3];	// N
			if (checkBit(n, 3)) MenuPos = MenuStruct[MenuPos][2];	// W
			if (checkBit(n, 4)) Execute();							// Enter
			if (MenuPos == 0) MenuPos = OldMenuPos;
			
			if (OldMenuPos != MenuPos)
			{
				LCDSetMenu(MenuTexts[MenuPos]);
				ShowTemperature();
				Index = 0;
				
			}
		}
	}
	incRunTime(rate);
	WDTReset();
}

void Menu::ShowTemperature() {
	if (MenuStruct[MenuPos][4] != 255)
	{
		float    T = Temperatures[MenuStruct[MenuPos][4]];
		String   Data = FormatTemp(T);
		lcd.setCursor(14,0);
		lcd.print(Data);
	}
}

void Menu::Execute()
{
		Serial.print('-');
		Serial.print(MenuPos);
		Serial.println('-');
	if (LCD_Enabled) {	
		switch (MenuPos)
		{
		case 2:
			sendAlarm.addAlarm("This is a test alarm");
			break;

		case 3:
			Serial.println("Crash");
			while(1)
			 ; // do nothing until the watchdog timer kicks in and resets the program.
			break;

		case 5:
			readT.getAddress();
			for (int i = 0; i < 8; i++)
			{  // we need 9 bytes
				lcd.print(OnWireAddress[i], HEX);
				lcd.print(" ");
			}
			break;

		case 9:
		case 11:
			if (MenuStruct[MenuPos][4] != 255)
			{
				float    T = Temperatures[MenuStruct[MenuPos][4]];
				String   Data = FormatTemp(T);
				lcd.print(Data);
			}
			break;
		}
	}	
}


void Menu::fillLine(int ndx){
	if (LCD_Enabled) {
		for (byte i = 0; i < 20-ndx; i++) {
			lcd.print(" ");
		}
	}
}

void Menu::LCDSetMenu(String aMenuText) {
	if (LCD_Enabled) {
		lcd.setCursor(0, 0);
		lcd.print(aMenuText);
		fillLine(aMenuText.length());
	}
}

void Menu::LCDSetStatus(String aStatus){
	if (LCD_Enabled) {
		lcd.setCursor(0, 3);
		lcd.print(aStatus);
		fillLine(aStatus.length());
	}
}

void Menu::LCDSetTime() {
	if (LCD_Enabled) {
		lcd.setCursor(0, 2);
		lcd.print(clock.GetTimeString());
		fillLine(8);
	}
}

void Menu::LCDSetAlarm() {
	if (LCD_Enabled) {
		lcd.setCursor(0, 1);
		//lcd.print(sendAlarm::AlarmText);
		//fillLine(AlarmText.length());
	}
}

void Menu::getAddress() {
	// read address
	ds2.reset();
	ds2.write(0x33, 1);
	for (int i = 0; i < 8; i++)
	{  // we need 9 bytes
		OnWireAddress[i] = ds2.read();
		Serial.print(OnWireAddress[i], HEX);
		Serial.print(" ");
	}
	Serial.println();
}

void Menu::BlinkLed14() {
	if (Led14Status == false)
	{
		digitalWrite(14, HIGH);
		Led14Status = true;
	}
	else
	{
		digitalWrite(14, LOW);
		Led14Status = false;
	}
}

void Menu::setLed(int Ndx) {
	if (LCD_Enabled) {
		Wire.beginTransmission(LedAddress);
		Wire.write(0x12);      // address bank A
		Wire.write(1 << Ndx);  // value to send
		Wire.endTransmission();
	}
}

byte Menu::readSetup() {
	if (LCD_Enabled) {
		Wire.beginTransmission(LedAddress);
		Wire.write(MCP23017_GPIOB); // set MCP23017 memory pointer to GPIOB address
		Wire.endTransmission();
		Wire.requestFrom(LedAddress, 1); // request one byte of data from MCP20317
		byte inputs=Wire.read(); // store the incoming byte into "inputs"
		Serial.println(inputs, BIN);
		//return inputs;
	}
	return 0;
}
#include <Arduino.h>
#include <Wire.h>
#include <UIPEthernet.h>
#include <RTClib.h>

#include "Clock.h"
#include "Utils.h"
#include "Globals.h"
#include "Credentials.h"

typedef unsigned long time_t;

RTC_DS1307 RTC;

Clock::Clock(uint32_t _rate) :
TimedTask(millis()),
rate(_rate),
on(false)
{
}

void Clock::readTime()
{
	DateTime NOW = RTC.now();
	RealTime = NOW.get();
	// Daylight 
	if (digitalRead(SummerTime) == LOW) 
	{
	    timeZone = -4;
	}
	else
	{
		timeZone = -5;
	}
}

void Clock::init()
{
	pinMode(SummerTime,INPUT_PULLUP);
	RTC.begin();
    Wire.begin();
	readTime();
	// check power failure
	// time is saved in RTC memory in run function
	// in case of power failure, RTC continue to run, but time is no more saved..
	// compute the difference to know the power failure duration
	DateTime CurrentTime = RTC.now();
	time_t  CT = CurrentTime.get();
    
	// get last saved time
	DateTime LastTimeSaved = RTC.lastSaved();
	time_t  LT = LastTimeSaved.get();

	// calc diff in seconds between time and saved time
	if ((CT - LT) > 1)
	{
		String   Ala = "Power outage of ";
	    	tmElements_t TE;
		breakTime(CT - LT, TE, 1900);
		//Ala += TE.Days;
		int H = TE.Day * 24;
		if (TE.Hour + H < 10) Ala += '0';
		Ala += TE.Hour;
		Ala += "h ";
		//if (TE.Minute < 10) Ala += '0';
		Ala += TE.Minute;
		Ala += "min ";
		//if (TE.Second < 10) Ala += '0';
		Ala += TE.Second;
		Ala += "sec.";
		sendAlarm.addAlarm(Ala);
		Serial.println(Ala);
	}
	NTPUpdate();
    // Read RTC Day register
	byte data[1];
	RTC.readBytesInRam(4, 1, data);
	Day = data[0];
}

void  Clock::NTPUpdate() {	
	if (ethernetConnect()) {
		menu.LCDSetStatus("Adjust RTC via NTP");
		time_t NP = getNtpTime();
		if (NP !=0) { 
			tmElements_t TE;
			breakTime(NP, TE, 1970);
			Serial.println("NTP OK");
			sendAlarm.addAlarm("RTC Updated");
			RTC.adjust(DateTime(TE.Year,TE.Month,TE.Day,TE.Hour,TE.Minute,TE.Second));
		}
		else {
			sendAlarm.addAlarm("RTC Failed");
			Serial.println("NTP Failed");
		}
		menu.LCDSetStatus(" ");   
	}
}

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

void Clock::breakTime(time_t timeInput, tmElements_t &tm, int Offset){
	// break the given time_t into time components
	// this is a more compact version of the C library localtime function
	// note that year is offset from 1970 !!!
	uint8_t year;
	uint8_t month, monthLength;
	time_t time;
	unsigned long days;

	time = timeInput;
	tm.Second = time % 60;
	time /= 60; // now it is minutes
	tm.Minute = time % 60;
	time /= 60; // now it is hours
	tm.Hour = time % 24;
	time /= 24; // now it is days
	tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1
	
	year = 0;
	days = 0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
		year++;
	}
	
	tm.Year = year+Offset;
	
	days -= LEAP_YEAR(year) ? 366 : 365;
	time  -= days; // now it is days in this year, starting at 0
	//	tm.Days = days;
	days=0;
	month=0;
	monthLength=0;
	for (month=0; month<12; month++) {
		if (month==1) { // february
			if (LEAP_YEAR(year)) {
				monthLength=29;
				} else {
				monthLength=28;
			}
			} else {
			monthLength = monthDays[month];
		}
		
		if (time >= monthLength) {
			time -= monthLength;
			} else {
			break;
		}
	}
	tm.Month = month + 1;  // jan is month 1
	tm.Day = time + 1;     // day of month
}

String Clock::GetTimeString() {
	tmElements_t TE;
	DateTime CurrentTime = RealTime;
	time_t CT = CurrentTime.get();
	String Temp("");
	breakTime(CT, TE, 2000);
	if (TE.Hour < 10) Temp = "0";
	{Temp += TE.Hour;}
	Temp += ":";
	if (TE.Minute < 10) Temp += '0';
	Temp += TE.Minute;
	Temp += ":";
	if (TE.Second < 10) Temp += '0';
	Temp += TE.Second;
	return Temp;
}

String Clock::GetDateString() {
	tmElements_t TE;
	DateTime CurrentTime = RealTime;
	time_t CT = CurrentTime.get();
	breakTime(CT, TE, 2000);
	String Temp("");
	Temp += TE.Year;
	Temp += "-";
	if (TE.Month < 10) Temp += '0';
	Temp += TE.Month;
	Temp += "-";
	if (TE.Day < 10) Temp += '0';
	Temp += TE.Day;
	return Temp;
}

String Clock::GetDateTimeString() {
	String Temp = GetDateString();
	Temp += " ";
	Temp += GetTimeString();
	return Temp;
}


void Clock::run(uint32_t now)
{
	// Read RTC Day register
	byte data[1];
	RTC.readBytesInRam(4, 1, data);
	// Update Clock from NTP server when day change 
	if (Day != data[0]){
	  Day = data[0];
	  NTPUpdate();
	}
	Serial.print('.');
	menu.setLed(ClockLed);
	menu.BlinkLed14();
	// save current time in RTC memory
	RTC.saveTime();
	readTime();
	// maintain ethernet connexion alive
	if (EthernetConnected) {
		Ethernet.maintain();
	}
	incRunTime(rate);
}

// NTP Servers:
// IPAddress timeServer(132, 163, 4, 101);
// time-a.timefreq.bldrdoc.gov
IPAddress timeServer(132, 163, 4, 102);
// time-b.timefreq.bldrdoc.gov

// IPAddress timeServer(132, 163, 4, 103);
// time-c.timefreq.bldrdoc.gov



EthernetUDP Udp;

/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// send an NTP request to the time server at the given address


char NTPserverName[] = "time.nist.gov";

void Clock::sendNTPpacket(IPAddress &address)
{
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;			// Stratum, or type of clock
	packetBuffer[2] = 6;			// Polling Interval
	packetBuffer[3] = 0xEC;			// Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;
	// all NTP fields have been given values, now
	// we can send a packet requesting a timestamp:
	//	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.beginPacket(NTPserverName, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}


time_t Clock::getNtpTime()
{
	while (Udp.parsePacket() > 0) ; // discard any previously received packets
	//	Serial.println("Transmit NTP Request");
	sendNTPpacket(timeServer);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			//		Serial.println("Receive NTP Response");
			Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			return secsSince1900 - 2208988800UL + timeZone * 3600;
		}
	}
	Serial.println("No NTP Response :-(");
	return 0; // return 0 if unable to get the time
}




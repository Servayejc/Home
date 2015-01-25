#include <Arduino.h>
#include "SendData.h"
#include "Utils.h"
#include "Globals.h"

volatile boolean  Connected;

SendData::SendData(uint32_t _rate) :
TimedTask(millis()),
rate(_rate),
on(false)
{
}

void SendData::init()
{
	
}

bool SendData::canRun(uint32_t now)
{
	return (now >= runTime) & runFlag  & readT.HasData(); // & ethernetConnect();
}

void SendData::SendToXyvely(String Data, bool IsAlarm){
	IPAddress server(216, 52, 233, 121); //Xively
	Serial.println("connecting...");
	if (client.connect(server, 80))
	{
		client.print("PUT /v2/feeds/");
		if (IsAlarm == true) {
			client.print(ALARM_FEED);
			//Serial.println(ALARM_FEED);
		}
		else {
			client.print(FEED);
			//Serial.println(FEED);
		}
		client.println(".csv HTTP/1.1");
		client.println("Host: api.pachube.com");
		client.print("X-pachubeApiKey: ");
		if (IsAlarm == true) {
			client.println(ALARM_APIKEY);
			//Serial.println(ALARM_APIKEY);
		}
		else {
			client.println(APIKEY);
			//Serial.println(APIKEY);
		}
		client.print("User-Agent: ");
		client.println(USERAGENT);
		client.print("Content-Length: ");
		client.println(Data.length());
		Serial.print("===");
		Serial.print(Data);
		Serial.println("===");
		// last pieces of the HTTP PUT request:
		client.println("Content-Type: text/csv");
		client.println("Connection: close");

		// here's the actual content of the PUT request:
		client.println(Data);
		Serial.println("send ok");
	}
	else
	{
		// if you couldn't make a connection:
		Serial.println("connection failed");
		Serial.println("disconnecting.");
		client.stop();
	}
}

void SendData::run(uint32_t now)
{
	if (readT.HasData()) {
		String Data = readT.GetData();
		menu.setLed(SendDataLed);
		menu.LCDSetStatus("Sending data");
		
		Serial.println("Send data");
		
		//Serial.println(Data);
		SendToXyvely(Data, false);
		Data = "";
		menu.LCDSetStatus(" ");
		incRunTime(rate);
	}
}


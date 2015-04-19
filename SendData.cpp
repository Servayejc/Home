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
	#ifdef DEBUG_SENDDATA
		Serial.println("connecting...");
	#endif	
	if (client.connect(server, 80))
	{
		client.print("PUT /v2/feeds/");
		if (IsAlarm == true) {
			client.print(ALARM_FEED);
			#ifdef DEBUG_SENDDATA
				Serial.println(ALARM_FEED);
			#endif	
		}
		else {
			client.print(FEED);
			#ifdef DEBUG_SENDDATA
				Serial.println(FEED);
			#endif
		}
		client.println(".csv HTTP/1.1");
		client.println("Host: api.pachube.com");
		client.print("X-pachubeApiKey: ");
		if (IsAlarm == true) {
			client.println(ALARM_APIKEY);
			#ifdef DEBUG_SENDDATA
				Serial.println(ALARM_APIKEY);
			#endif	
		}
		else {
			client.println(APIKEY);
			#ifdef DEBUG_SENDDATA
				Serial.println(APIKEY);
			#endif  
		}
		client.print("User-Agent: ");
		client.println(USERAGENT);
		client.print("Content-Length: ");
		client.println(Data.length());
		#ifdef DEBUG_SENDDATA
			Serial.print("===");
			Serial.print(Data);
			Serial.println("===");
			Serial.println("send ok");
		#endif
		// last pieces of the HTTP PUT request:
		client.println("Content-Type: text/csv");
		client.println("Connection: close");

		// here's the actual content of the PUT request:
		client.println(Data);
		
	}
	else
	{
		// if you couldn't make a connection:
		#ifdef DEBUG_SENDDATA
			Serial.println("connection failed");
			Serial.println("disconnecting.");
		#endif
		client.stop();
	}
}

void SendData::run(uint32_t now)
{
	if (readT.HasData()) {
		String Data = readT.GetData();
		menu.setLed(SendDataLed);
		menu.LCDSetStatus("Sending data");
		#ifdef DEBUG_SENDDATA
			Serial.println("Send data");
			Serial.println(Data);
		#endif
	
		SendToXyvely(Data, false);
		Data = "";
		menu.LCDSetStatus(" ");
		incRunTime(rate);
		WDTReset();
	}
}


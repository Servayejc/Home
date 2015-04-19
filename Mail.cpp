#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <UIPEthernet.h>
#include <Temboo.h>
#include "Mail.h"
#include "Utils.h"
#include "Globals.h"
#include "Credentials.h"



UIPClient      sendMailClient;

SendAlarm::SendAlarm(uint32_t _rate) :
TimedTask(millis()),
rate(_rate),
on(false)
{
}

void SendAlarm::init()
{
}

bool SendAlarm::canRun(uint32_t now) {
	return (now >= runTime) & runFlag & alarmsEnabled();// & ethernetConnect();
}

void SendAlarm::logToXively()
{
	String	Data = "\n";
	Data += "999";
	Data += ',';
	Data += '"';
	Data += AlarmText;
	Data += '"';
	Data += "\n";
	sendData.SendToXyvely(Data, true);
}

void SendAlarm::addAlarm(String Ala)
{
	AlarmText += clock.GetDateTimeString();
	AlarmText += "\t";
	AlarmText += Ala;
	AlarmText += "\n";
};

void SendAlarm::addEvent(String Event)
{
	EventText += clock.GetDateTimeString();
	EventText += "\t";
	EventText += Event;
	EventText += "\n";
};

String SendAlarm::GetAlarms()
{
	return AlarmText;
};	

String SendAlarm::GetEvents()
{
	return EventText;
};
	
void SendAlarm::run(uint32_t now)
{
	#ifdef DEBUG_ALARMS
		Serial.println("alarms");
	#endif
	
	AlarmText.trim();
	if (AlarmText.length() > 0)
	{
		menu.setLed(SendAlarmsLed);
		#ifdef DEBUG_ALARMS
			Serial.print("-");
			Serial.print(AlarmText);
			Serial.println("-");
		#endif
		// send to Xively
		logToXively();
		
		// send email
		menu.LCDSetStatus("Sending Alarm");
		#ifdef DEBUG_ALARMS
			Serial.println("Sending alarm.");
		#endif
		
		TembooChoreo   SendEmailChoreo(sendMailClient);

		// Set Temboo account credentials
		SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
		SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
		SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
		// Set profile to use for execution
		SendEmailChoreo.setProfile("arduinoTemperatureEmail");
		// Set Choreo inputs
		String   MessageBodyValue = String("Alarm!\n") + AlarmText;
		SendEmailChoreo.addInput("MessageBody", MessageBodyValue);
		SendEmailChoreo.addInput("Subject", "Alarme Maison");
		// Identify the Choreo to run
		SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
		// Run the Choreo
		unsigned int   returnCode = SendEmailChoreo.run();

		// A return code of zero means everything worked
		if (returnCode == 0)
		{
			#ifdef DEBUG_ALARMS
				Serial.println("Done!\n");
			#endif
			AlarmText = "";
		}
		else
		{
			// A non-zero return code means there was an error
			// Read and print the error message
			while (SendEmailChoreo.available())
			{
				char  c = SendEmailChoreo.read();
				#ifdef DEBUG_ALARMS
					Serial.print(c);
				#endif
			}
			#ifdef DEBUG_ALARMS
				Serial.println();
				Serial.println("Wait 15 sec before retry! \n");
			#endif
			incRunTime(15000); //  error, wait 15 sec;
		}

		SendEmailChoreo.close();
		#ifdef DEBUG_ALARMS
			showMemory();
		#endif
		menu.LCDSetStatus(" ");
	}
	
	incRunTime(rate);
}

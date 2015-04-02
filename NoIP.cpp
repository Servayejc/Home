#include "NoIP.h"
#include "Globals.h"
#include "Utils.h"

/*
GET /nic/update?hostname=mytest.testdomain.com&myip=1.2.3.4 HTTP/1.0
Host: dynupdate.no-ip.com
Authorization: Basic base64-encoded-auth-string
User-Agent: Bobs Update Client WindowsXP/1.2 bob@somedomain.com
http://username:password@dynupdate.no-ip.com/nic/update?hostname=mytest.testdomain.com&myip=1.2.3.4
*/


NoIPUpdater::NoIPUpdater(uint32_t _rate) :
TimedTask(millis()),
rate(_rate),
on(false)
{
}

void NoIPUpdater::init() {
	next = 0;
	dataFound = false;
	CurrentIP = "0";
}

boolean  NoIPUpdater::isData(char c){
	if (c == '\n' && current_line_is_blank) {
		return true;
	}
	if (c == '\n'){
		current_line_is_blank = true;
	}
	else if (c != '\r'){
		current_line_is_blank = false;
	}
	return false;
}

void NoIPUpdater::ProcessResponse(){
	int size;
	ExternalIP = "";
	while((size = client.available()) > 0)
	{
		Ethernet.maintain();
		char c = client.read();
		if (isData(c)){
			dataFound = true;
		}
		if (dataFound) {
			if (c > ' ' ){
				ExternalIP +=c;
			}
		}
	}
}

void NoIPUpdater::close() {
	Serial.println();
	Serial.println("Client disconnect");
	client.stop();
	current_line_is_blank = false;
	done = true;
}

void NoIPUpdater::GetExternalIP() {
	Serial.println("GetExtIP-Client connect");
	if (client.connect("myip.dnsdynamic.org",80))
	{
	    next = millis() + 2000;    // 2 seconds
		Serial.println("GetExtIP-Client connected");
		client.println("GET / HTTP/1.1");
		client.println("Host: myip.dnsdynamic.org");
		client.println();
		while(client.available()==0)
		{
			if (next - millis() < 0)
			close();  // after 2 sec timeout
		}
		ProcessResponse();
		Serial.print("GetExtIP-");
		Serial.println(ExternalIP);
		close();
	}
	else
  		Serial.println("GetExtIP-Connect failed");
}

void NoIPUpdater::UpdateIP() {
	done = false;
	next = 0;
	menu.setLed(NoIPLed);
	Serial.println("NoIP connecting");
	if (ethernetConnect()) {
		// if you get a connection, report back via serial:
		if (client.connect("dynupdate.no-ip.com", 80)) {
			Serial.println("connected");
			next = millis() + 2000;    // 2 seconds
			// Make a HTTP request:
			//replace yourhost.no-ip.org with your no-ip hostname
			client.print("GET /nic/update?hostname=maisonnh.noip.me&myip=");
			client.print(ExternalIP);
			client.println(" HTTP/1.0");
			client.println("Host: dynupdate.no-ip.com");
			//encode your username:password (make sure colon is between username and password)
			//to base64 at http://www.opinionatedgeek.com/dotnet/tools/base64encode/
			//and replace the string below after Basic with your encoded string
			//clear text username:password is not accepted
			client.println("Authorization: Basic amMuc2VydmF5ZUByYWRpb2FjdGlmLmNvbTpQcmFsaW5lMjAwMw==");
			client.println("User-Agent: Arduino Sketch/1.0 user@host.com");
			client.println();
			
			while(done == false) {
				Ethernet.maintain();
				while(client.available()==0)
				{
					Ethernet.maintain();
					if (next - millis() < 0)
					close();
				}
				int size;
				Serial.print(size);
				while((size = client.available()) > 0)
				{
					Ethernet.maintain();
					char c = client.read();
					if (isData(c)){
						dataFound = true;
					}
					if (dataFound) {
						if (c > ' ' ){
							Serial.print(c);
						}
					}
				}
				close();
			}
		}
	}
}

void NoIPUpdater::run(uint32_t now) {
	GetExternalIP();
	Serial.println(ExternalIP);
	Serial.println(CurrentIP);
	if (CurrentIP != ExternalIP) {
		UpdateIP();
	}  
	incRunTime(rate);
	WDTReset();
}


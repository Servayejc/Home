#include <UIPEthernet.h>
#include <HTTPClient.h>
#include <EEPROMEx.h>
#include <avr/wdt.h>
#include "Server.h"
#include "Globals.h"
#include "Utils.h"

#define DEFAULT 1
#define EDIT 	2
#define SAVE	3

EthernetServer server = EthernetServer(80);

WebServer::WebServer(uint32_t _rate) :
TimedTask(millis()),
rate(_rate),
on(false)
{
}

void WebServer::init() {
	ethernetConnect();
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());
	server.begin();
	//load_config();
}

void WebServer::processPair(String tag, String val) {
	int value;
	int tagNum = tag.toInt();
	int ndx = val.indexOf("%3A"); // %3A = ':'
	//   Serial.println(val);
	if (ndx > -1) {
		int H = val.substring(0,ndx).toInt();
		int M = val.substring(ndx+3, val.length()).toInt();
		value = minutes(H,M);
	}
	else
	{
		value = val.toInt();
	}
	
	//Serial.print(tagNum);
	//Serial.print(':');
	//Serial.println(value);
	
	// Write data back to config;
	byte x = tagNum % 10 -1;
	
	if (tagNum < 100) {
		switch (tagNum / 10) {
		case 1 :
			config.channel[EditIndex].group[0].time[x] = value;
			break;
		case 2 :
			config.channel[EditIndex].group[0].setpoint[x] = value;
			break;
		case 3 :
			config.channel[EditIndex].group[1].time[x] = value;
			break;
		case 4 :
			config.channel[EditIndex].group[1].setpoint[x] = value;
			break;
		}
	}
}

void WebServer::ProcessData(String aLine) {
	Serial.println("process data");
	int ndx1;
	int ndx2;
	while (aLine.length() > 0) {
		ndx1 = aLine.indexOf("=");
		if (ndx1 == -1) break;  // no more pair
		ndx2 = aLine.indexOf("&");
		if (ndx2 == -1) {
			ndx2 = aLine.length();
		} // in case of last pair
		String tag = aLine.substring(0,ndx1);
		String val = aLine.substring(ndx1+1, ndx2);
		aLine = aLine.substring(ndx2+1,aLine.length());
		if (tag.toInt() == 100) EditIndex = val.toInt();
		processPair(tag, val);
	}
}

void WebServer::load_config() {

}

int WebServer::EEPROM_writeConf() {
	Serial.println("Write Conf");
	byte Address = 0;
	
	const byte* p = (const byte*)(const void*)&CONFIGURATION;
	int i;
	for (i = 0; i < sizeof(CONFIGURATION); i++)
	EEPROM.write(Address++, *p++);
	return i;
}

int WebServer::EEPROM_readConf() {
	Serial.println("Read Conf");
	byte Address = 0;
	byte* p = (byte*)(void*)&CONFIGURATION;
	int i;
	for (i = 0; i < sizeof(CONFIGURATION); i++)
	*p++ = EEPROM.read(Address++);
	return i;
}

void WebServer::Input(int ID, String Value) {
	client.print("<td><input class=""none"" type=""text"" name=""");
	client.print(ID);
	client.print(""" value = """);
	client.print(Value);
	client.println("""></td>");
}

void WebServer::TableRow(String V1,String V2,String V3,String V4) {
	client.print("<tr><td>");
	client.print(V1);
	client.print("</td><td>");
	client.print(V2);
	client.print("</td><td>");
	client.print(V3);
	client.print("</td><td><input type='radio' name='100' Value='");
	client.print(V4);
	client.println("'</td></tr>");
}

void WebServer::ConfigRowT(byte c, byte g)  {
	client.print("<tr>");
	for (byte n = 0; n < 4; n++) {
		client.print("<td>");
		client.print(changetime(c,g,n));
		client.print("</td>");
	}
	client.print("<td>");
	client.print("<input type='radio' name='100' Value='");
	client.print(c);
	client.print("'</td>");
	client.println("<tr>");
		
	client.print("<tr>");
		for (byte n = 0; n < 4; n++) {
			client.print("<td>");
			client.print((config.channel[c].group[g].setpoint[n]));
			client.print("</td>");
		}
	client.print("<td>");
	client.print("<input type='radio' name='100' Value='");
	client.print(c);
	client.print("'</td>");
	client.println("<tr>");
}

void WebServer::HTTP200()
{
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println("Connection: close");
	client.println();
}

void WebServer::Head()
{
	client.println("<head><title>Maison NH</title>");
	client.println("<style>table, th, td {border: 1px solid black;border-collapse: collapse;}");
	client.println("input.none {border-style: none;}");
	client.println("</style></head>");
}

void WebServer::Default() {
	client.println("<body><h1>Temperatures</h1>");
	client.println("<form action=""EDIT"" method=""post"">");
	client.println("<table style='width:50%'>");
	//client.println("<tr><th>Sensor</th><th>Temp</th><th>Setpoint</th><th>Edit</th></tr>");
	
    for (byte c = 0; c < 6; c++) {
	 	for (byte g = 0; g < 2; g++) {
			ConfigRowT(c,g);
		 }
	}
	
	/*TableRow("Exterieur","-25","NA","0");
	TableRow("Foyer","35","NA","1");
	TableRow("Salon","21.5","22","2");
	TableRow("Sous-Sol","19.2","19","3");
	TableRow("Chambre3","20","19","4");
	TableRow("Salle de bain","23","22","5");*/
	client.println("</table>");
	client.println("<br>");
	client.println("<input type=submit value=submit>");
	client.println("</form>");
	client.println("<br>");
	//client.println("Save settings in EEPROM");*/
	
	//client.println("<input type=submit value=submit>");
	//	client.println("<form action=""EEPROM"" method=""post"">");
	//	client.println("</form>");
	client.println("</body>");
}

void WebServer::Edit(byte channel) {
	
	client.println("<body><h1>Edit</h1>");
	client.print("Editing channel: ");
	client.println(EditIndex);
	client.println("<form action=""HOME"" method=""post"">");
	client.print("<input type=""hidden"" name=""100"" value=""");
	client.print(EditIndex);
	client.println(""">");
	for (byte g = 0; g < 2; g++) {
		client.println("<table>");
		if (g == 0)
		client.println("Semaine");
		else
		client.println("Samedi et dimanche");
		
		client.println("<tr>");
		client.println("<td>");
		client.println("Heure");
		client.println("</td>");
		
		for (byte i = 0; i < 4; i++) {
			Input(i+11+20*g,changetime(channel,g,i));
		}
		client.println("</tr>");
		
		client.println("<tr>");
		client.println("<td>");
		client.println("Temperature");
		client.println("</td>");
		
		for (byte i = 0; i < 4; i++) {
			Input(i+21+20*g,String(config.channel[channel].group[g].setpoint[i]));
		}
		client.println("</tr>");
		client.println("</table>");
		client.println("<br>");
	}
	client.println("<input type=submit value=submit>");
	client.println("</form>");
	client.println("</body>");
}

void WebServer::Eeprom() {
	client.println("<body><h1>Configuration saved</h1>");
	client.println("<form action=""HOME"" method=""post"">");
	client.println("<input type=submit value=submit>");
	client.println("</form>");
	client.println("</body>");
}

void WebServer::ProcessReceivedLine(String line) {
	//Serial.println(line);
	
	if ((HeaderEnd == true) && (dataSize > 0)) {
		line = line.substring(0,dataSize);
	}
	if (line.length() == 0 ) {
		HeaderEnd = true;
	}
	// when client ask edit
	if (line.indexOf("POST /EDIT") == 0) {
		page = EDIT;
	}
	// when the client submit the edit
	if (line.indexOf("POST /HOME") == 0) {
		page = DEFAULT;
	}
	// when the client ask to save
	if (line.indexOf("POST /EEPROM") == 0) {
		//		EEPROM_writeConf();
		page = SAVE;
	}
	// when client ask the page
	if (line.indexOf("GET / ") == 0) {
		page = DEFAULT;
	}
	
	if (line.indexOf("Content-Length") == 0) {
		dataSize = line.substring(line.indexOf("Content-Length")+15).toInt();
	}
	
	if ((HeaderEnd == true) && (line.length() > 0)) {
		ProcessData(line);
	}
}

void WebServer::readContent() {
	String line("");
	HeaderEnd = false;
	while (client.connected()) {
		if (client.available()) {
			char c = client.read();
			if (c != '\n' && c != '\r') {
				line += c;
			}
			else
			{
				if (c == '\n') {
					ProcessReceivedLine(line);
					line = "";
				}
			}
		}
		else
		{
			line += '\n';
			ProcessReceivedLine(line);
			line = "";
			break;
		}
	}
}

void WebServer::executeCommand() {
	HTTP200();
	client.println("<!DOCTYPE HTML>");
	client.println("<html>");
	Head();
	if (page == DEFAULT)
	{Default();
	}
	if (page == EDIT)
	{Edit(EditIndex);
	}
	if (page == SAVE)
	{Eeprom();
	}
	client.println("</html>");
}

void WebServer::run(uint32_t now) {
	// listen for incoming clients
	client = server.available();
	if (client) {
		menu.setLed(ServerLed);
		Serial.println("Client");
		readContent();
		executeCommand();
		client.stop();
		Serial.println("client disconnected");
	}
	wdt_reset();
	Ethernet.maintain();
	incRunTime(rate);
}

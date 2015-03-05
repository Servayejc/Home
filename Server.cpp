#include <UIPEthernet.h>
#include <HTTPClient.h>
#include <EEPROMEx.h>
#include <avr/wdt.h>
#include "Server.h"
#include "Globals.h"
#include "Utils.h"
#include "HConfig.h"

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

// HTML

void WebServer::tr()				{ client.print("<tr>"); }
void WebServer::ctr()				{ client.print("</tr>"); }
void WebServer::td(String value)	{ client.print("<td>"); 
                                      client.print(value);}
void WebServer::td()				{ client.print("<td>"); }
void WebServer::ctd()				{ client.print("</td>"); }
void WebServer::tdv(String value)   { client.print("<td>"); 
                                      client.print(value);
								      client.print("</td>");}
void WebServer::form(String name) { client.print("<form action=""");
									client.print(name);
									client.print(""" method=""post"">"); }
void WebServer::cform() { client.print("</form>"); }
void WebServer::br() { client.print("<br>"); }
void WebServer::table() { client.print("<table>"); }
void WebServer::ctable() { client.print("</table>"); }
void WebServer::submit() {client.print("<input type=submit value=submit>");}
void WebServer::body(String title) {client.print("<body><h1>"); client.print(title); client.print("</h1>");}
void WebServer::cbody() {client.print("</body>");}

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
		Config.setPair( EditIndex, tag, val);
	}
}

void WebServer::Input(int ID, String Value) {
	td();
	client.print("<input class=""none"" type=""text"" name=""");
	client.print(ID);
	client.print(""" value = """);
	client.print(Value);
	client.println(""">");
	ctd();
}

void WebServer::TableRow(String V1,String V2,String V3,String V4) {
	tr(); 
	tdv(V1);
	tdv(V2);
	tdv(V3);
	td();
		client.print("<input type='radio' name='100' Value='");
		client.print(V4);
		client.print("'>");
	ctd(); 
	ctr();
}

void WebServer::ConfigRowT(byte c, byte g)  {
	tr();
		for (byte n = 0; n < 4; n++) {
			tdv(Config.getTimeString(c,g,n));
		}
		td();
			client.print("<input type='radio' name='100' Value='");
			client.print(c);
			client.print("'>");
		ctd();
	ctr();	

	tr();
		for (byte n = 0; n < 4; n++) {
		 	tdv(String(Config.getSetPoint(c,g,n)));
		}
		td();	
			client.print("<input type='radio' name='100' Value='");
			client.print(c);
			client.print("'>");
		ctd();
	ctr();
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
	client.println("<!DOCTYPE HTML>");
	client.println("<html>");
	client.print("<head><title>Maison NH</title>");
	client.print("<style>table, th, td {border: 1px solid black;border-collapse: collapse;}");
	client.print("input.none {border-style: none;}");
	client.print("</style></head>");
}

void WebServer::Default() {
	body(String("Configuration"));
	form(String("EDIT"));
		table();
			for (byte c = 0; c < 6; c++) {
				for (byte g = 0; g < 2; g++) {
					ConfigRowT(c,g);
				}
			}
		ctable();
		br();
		submit();
	cform();
	br();
	submit();
	cbody();
}

void WebServer::Edit(byte channel) {
	body(String("Edit"));
	client.print("Editing channel: ");
	client.println(EditIndex);
	form(String("HOME"));
	client.print("<input type='hidden' name='100' value='");
	client.print(EditIndex);
	client.println("'>");
	for (byte g = 0; g < 2; g++) {

		if (g == 0)
			client.println("Semaine");
		else
			client.println("Samedi et dimanche");
		table();
			tr();
				td();
					client.println("Heure");
				ctd();
				for (byte i = 0; i < 4; i++) {
					Input(i+11+20*g,Config.getTimeString(channel,g,i));
				}
			ctr();
			
			tr();
				td();
					client.println("Temperature");
				ctd();
				for (byte i = 0; i < 4; i++) {
					Input(i+21+20*g,String(Config.getSetPoint(channel,g,i)));
				}
			ctr();
		ctable();
		br();
	}
	submit();
	cform();
	cbody();
}

void WebServer::Eeprom() {

	body(String("Configuration saved"));
	form(String("HOME"));
	submit();
	cform();
	cbody();
}

void WebServer::ProcessReceivedLine(String line) {
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
	Head();
	if (page == DEFAULT){Default();}
	if (page == EDIT) 	{Edit(EditIndex);}
	if (page == SAVE) 	{Eeprom();}
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

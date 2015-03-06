#ifndef _SERVER_h
#define _SERVER_h

#include "Arduino.h"
#include <Task.h>
#include <UIPEthernet.h>


class WebServer :
public TimedTask
{
 public:
	WebServer(uint32_t _rate);
	void  init();
	void  run(uint32_t now);
	
private:
	uint32_t rate;
	bool on;

	boolean HeaderEnd;
	int page;
	int dataSize;
	byte EditIndex;	
	
	void body();
	void TableRow(String V1,String V2,String V3,String V4);
	void ConfigRowT(byte c, byte g);
	void Head();
	void Default();
	void Params();
	void Edit(byte channel);
	void Eeprom();
	void HTTP200();
	void Response();
	void settings();
	void setAuto();
	void ProcessReceivedLine(String line);
	void processPair(String tag, String val);
	void ProcessData(String);
	void readResponse();
	void executeCommand();
	void readContent();

	// HTML
	void tr();
	void ctr();
	void td(String value);
	void td();
	void ctd();
	void tdv(String value);
	void form(String name);
	void cform();
	void br();
	void table();
	void ctable();
	void submit(String caption);
	void body(String title);
	void cbody();
	void Input(int ID, String Value);

	EthernetClient client;
};

#endif


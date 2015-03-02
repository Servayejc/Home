#ifndef _SERVER_h
#define _SERVER_h

#include "Arduino.h"
#include <Task.h>
#include <UIPEthernet.h>

#define MAGIC 27



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
	bool fan_state; 
	char *filename;
	int  EEPROM_readConf();
	int  EEPROM_writeConf();
	void load_config(); 
	struct config_t
	{
		int magic;
		int on_temp;
		int off_temp;
		int check_time;
		unsigned long submit_time;
		char server_name[30];
		char server_url[20];
		}  CONFIGURATION;
	boolean HeaderEnd;
	int page;
	int dataSize;
	byte EditIndex;	
	
	void Input(int ID, String Value);
	void TableRow(String V1,String V2,String V3,String V4);
	void ConfigRowT(byte c, byte g);
	void Head();
	void Default();
	void Edit(byte channel);
	void Eeprom();
	void HTTP200();
	void Response();
	void forceOn();
	void forceOff();
	void settings();
	void setAuto();
	void ProcessReceivedLine(String line);
	void processPair(String tag, String val);
	void ProcessData(String);
	void readResponse();
	void executeCommand();
	void readContent();

	bool override;
	
	EthernetClient client;
};

//sdsdd

#endif


#ifndef _NOIP_h
#define _NOIP_h

#include "Arduino.h"
#include <Task.h>
#include <UIPEthernet.h>

class NoIPUpdater :
public TimedTask
{
	public:
	NoIPUpdater(uint32_t _rate);
	void  init();
	void  run(uint32_t now);

	private:
	uint32_t rate;
	bool     on;
    String ExternalIP;
	String CurrentIP;
	boolean dataFound; 
	boolean  isData(char c);
	boolean current_line_is_blank;
	signed long next;
	boolean done;
    void ProcessResponse();
	void GetExternalIP();
    void UpdateIP();
	void close();
};

#endif



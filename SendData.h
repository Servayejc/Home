#ifndef SENDDATA_h
#define SENDDATA_h

#include <UIPEthernet.h>
#include <Task.h>
#include "Globals.h"

class SendData :
   public TimedTask
{
public:
   // Create a new ReadTemp for the specified pin and rate.
   SendData(uint32_t _rate);
   void  init();
   void  run(uint32_t now);
   bool canRun(uint32_t now);
   void SendToXyvely(String Data, bool IsAlarm);
private:
   uint32_t rate;                // Blink rate.
   bool     on;
   void     prepareDataToSend();
   String   Data;
};

#endif


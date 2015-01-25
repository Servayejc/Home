#include "Alarms.h"
#include "Globals.h"


AlarmClass::AlarmClass(byte aPriority, String aText):

	Priority(aPriority),
	Text(aText)
{
}

void AlarmClass::init()
{
	//Time = RealTime;
}

String AlarmClass::GetMessage()
{
	String Msg(Priority);
	Msg += " ";
	int   Sec = Time % 60;
		long  X = Time / 60;
		int   Min = X % 60;
		X = X / 60;
		int   Hour = X % 24;
		X = X / 24;
    Msg += Hour;
    Msg += ":";
    Msg += Min;
    Msg += ":";
    Msg += Sec;
    Msg += " "; 
	Msg += Text;
	return Msg; 
}





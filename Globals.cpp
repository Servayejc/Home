#include "Globals.h"
float memtest[4000];
// Internet
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x48, 0x99};
bool EthernetConnected = false;

// ReadTemp
float Temperatures[SensorsCount];
float TT[SensorsCount];
//int NT;

byte OnWireAddress[9];


LiquidTWI2 lcd(LiquidTWIOffset);

//Menu
const char *MenuTexts[MenuItemsCount] =
{
	"",
	"SOUS-SOL",
	"REZ",
	"PREMIER",
	"ALARMES",
	"I2C ADRESSE",
	"Salle familiale",
	"Sdb-1",
	"Chambre-4",
	"Salon",
	"Sdb-2",
	"Exterieur",
	"Chambre-1",
	"Chambre-2",
	"Chambre-3",
	"Sdb-3",
	"Local Technique"
};

const byte       MenuStruct[MenuItemsCount][5] =
{
	//E,  S,  W,  N, TH
	{ 0,  0,  0,  0, 255 },    //0
	{ 6,  2,  0,  5, 255 },    //1  SOUS-SOL
	{ 9,  3,  0,  1, 255 },	   //2  REZ
	{12,  4,  0,  2, 255 },    //3  PREMIER
	{ 0,  5,  0,  3, 255 },    //4  ALARMES
	{ 0,  1,  0,  4, 255 },	   //5  I2C ADRESSE
	{ 7,  0,  1,  0, 255 },    //6  Salle familiale
	{ 8,  0,  6,  0, 255 },    //7  Sdb-1
	{16,  0,  7,  0, 255 },	   //8  Chambre-4
	{10,  0,  2,  0,   0 },    //9  Salon
	{11,  0,  9,  0, 255 },    //10 Sdb-2
	{ 2,  0, 10,  0,   1 },	   //11 Exterieur
	{13,  0,  3,  0, 255 },    //12 Chambre-1
	{14,  0, 12,  0, 255 },    //13 Chambre-2
	{15,  0, 13,  0, 255 },    //14 Chambre-3
	{ 3,  0, 14,  0, 255 },    //15 Sdb-3
	{ 1,  0,  8,  0, 255 }     //16 Local Technique
};

// OneWire sensors
const byte    Sensors[SensorsCount][8] =
{
	{ 0x28, 0x65, 0xFB, 0x96, 0x05, 0x00, 0x00, 0x3D },		// Tempuino T1
	{ 0x28, 0x8F, 0x15, 0x97, 0x05, 0x00, 0x00, 0x2C }		// Tempuino T2
	//	{ 0x28, 0xD7, 0x03, 0x97, 0x05, 0x00, 0x00, 0x43 }		// Tempuino read address
	/*      {0x28, 0x12, 0x16, 0x4D, 0x03, 0x00, 0x00, 0x52},// Salon
	{0x28, 0xBC, 0x03, 0x4D, 0x03, 0x00, 0x00, 0xB3},// Sous-sol
	{0x28, 0x7C, 0xB0, 0x54, 0x03, 0x00, 0x00, 0x86},// Extérieur
	{0x28, 0x27, 0xBE, 0x54, 0x03, 0x00, 0x00, 0xF2},// Foyer
	{0x28, 0x5F, 0xCA, 0x54, 0x03, 0x00, 0x00, 0xB9} // Chambre Raph
	// {0x28, 0x44, 0xCA, 0x54, 0x03, 0x00, 0x00, 0x1A} // SDB Haut	  */
	};

	// Ethernet
	UIPClient	client;
	Power		power(2000);
	Menu		menu(100);
	Clock		clock(1000);
	ReadTemp	readT(1500);
	SendData	sendData(60000);
	SendAlarm	sendAlarm(15000);
	NoIPUpdater NoIP(600000);
	WebServer	webServer(200);
    
   CApplicationMonitor ApplicationMonitor;



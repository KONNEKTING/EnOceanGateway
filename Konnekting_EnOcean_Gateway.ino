#include "Arduino.h"

#include <KonnektingDevice.h>
#include "kdevice_KNX_EnOcean_Gateway.h"
#include "EEPROM.h"
#include "Enocean.h"
#include <Wire.h>

// ################################################
// ### DEBUG CONFIGURATION
// ################################################
// DEBUG-MODE
//#define KDEBUG // comment this line to disable DEBUG mode

#ifdef KDEBUG
#include <DebugUtil.h>
// Get correct serial port for debugging
#define DEBUGSERIAL SerialUSB
#endif

// ################################################
// ### KONNEKTING Configuration
// ################################################
#define KNX_SERIAL Serial1 // Leonardo/Micro/Zero etc. use Serial1

// ################################################
// ### IO Configuration
// ################################################
#define PROG_LED_PIN    A2
#define PROG_BUTTON_PIN A1
#define LED_YELLOW      25

// Enocean Device Libraries
#include "EnOceanDevice_RPS_simple.h"
#include "EnOceanDevice_RPS_sender.h"
#include "EnOceanDevice_NodOn_2Ch.h"

// ################################################
// ### EnOcean Devices Instance creation
// ###   order doesn't matter here
// ################################################

EnOceanDevice_RPS_simple taster1 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster2 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster3 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster4 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster5 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster6 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster7 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster8 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple taster9 = EnOceanDevice_RPS_simple();
EnOceanDevice_RPS_simple tasterA = EnOceanDevice_RPS_simple();

EnOceanDevice_RPS_sender rpsSender1 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender2 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender3 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender4 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender5 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender6 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender7 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender8 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSender9 = EnOceanDevice_RPS_sender();
EnOceanDevice_RPS_sender rpsSenderA = EnOceanDevice_RPS_sender();

EnOceanDevice_NodOn_2Ch actor1 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor2 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor3 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor4 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor5 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor6 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor7 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor8 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actor9 = EnOceanDevice_NodOn_2Ch();
EnOceanDevice_NodOn_2Ch actorA = EnOceanDevice_NodOn_2Ch();





void setup()
{
	//****************** Init IO *******************************************************
	pinMode(LED_YELLOW, OUTPUT);
	digitalWrite(LED_YELLOW, HIGH);
	Wire.begin();

	//****************** Init Debug Interface ********************************************
#ifdef KDEBUG
	// Start debug serial with 9600 bauds
	DEBUGSERIAL.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(__SAMD21G18A__)
	// wait for serial port to connect. Needed for Leonardo/Micro/ProMicro/Zero only
	while (!DEBUGSERIAL)
	{
	}
#endif
	// make debug serial port known to debug class
	// Means: KONNEKTING will use the same serial port for console debugging
	Debug.setPrintStream(&DEBUGSERIAL);
	Debug.println(F("KONNEKTING EnOcean Gateway"));
#endif

	//****************** Init Debug KONNEKTING ********************************************
	Konnekting.setMemoryReadFunc(&readEeprom);
	Konnekting.setMemoryWriteFunc(&writeEeprom);
	Konnekting.setMemoryUpdateFunc(&updateEeprom);

	// Initialize KNX enabled Arduino Board
	Konnekting.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);

	//****************** Read Parameter ***************************************************
#ifdef KDEBUG
	Debug.println(F("** READ Parameter ******************"));
#endif

	// ####################################################################################
	// ###
	// ### EnOcean Devices configuration from the definition in Konnekting
	// ### ATTENTION: The order defines the position from which com-obj and params data is read
	// ###
	// ####################################################################################
#ifdef KDEBUG
	Debug.println(F("** READ Taster 1 ******************"));
#endif
	enOcean.configureDevice(taster1);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 2 ******************"));
#endif
	enOcean.configureDevice(taster2);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 3 ******************"));
#endif
	enOcean.configureDevice(taster3);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 4 ******************"));
#endif
	enOcean.configureDevice(taster4);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 5 ******************"));
#endif
	enOcean.configureDevice(taster5);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 6 ******************"));
#endif
	enOcean.configureDevice(taster6);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 7 ******************"));
#endif
	enOcean.configureDevice(taster7);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 8 ******************"));
#endif
	enOcean.configureDevice(taster8);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 9 ******************"));
#endif
	enOcean.configureDevice(taster9);
#ifdef KDEBUG
	Debug.println(F("** READ Taster 10 *****************"));
#endif
	enOcean.configureDevice(tasterA);

#ifdef KDEBUG
	Debug.println(F("** READ Sender 1 ******************"));
#endif
	enOcean.configureDevice(rpsSender1);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 2 ******************"));
#endif
	enOcean.configureDevice(rpsSender2);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 3 ******************"));
#endif
	enOcean.configureDevice(rpsSender3);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 4 ******************"));
#endif
	enOcean.configureDevice(rpsSender4);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 5 ******************"));
#endif
	enOcean.configureDevice(rpsSender5);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 6 ******************"));
#endif
	enOcean.configureDevice(rpsSender6);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 7 ******************"));
#endif
	enOcean.configureDevice(rpsSender7);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 8 ******************"));
#endif
	enOcean.configureDevice(rpsSender8);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 9 ******************"));
#endif
	enOcean.configureDevice(rpsSender9);
#ifdef KDEBUG
	Debug.println(F("** READ Sender 10 *****************"));
#endif
	enOcean.configureDevice(rpsSenderA);

#ifdef KDEBUG
	Debug.println(F("** READ Aktor 1 ******************"));
#endif
	enOcean.configureDevice(actor1);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 2 ******************"));
#endif
	enOcean.configureDevice(actor2);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 3 ******************"));
#endif
	enOcean.configureDevice(actor3);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 4 ******************"));
#endif
	enOcean.configureDevice(actor4);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 5 ******************"));
#endif
	enOcean.configureDevice(actor5);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 6 ******************"));
#endif
	enOcean.configureDevice(actor6);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 7 ******************"));
#endif
	enOcean.configureDevice(actor7);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 8 ******************"));
#endif
	enOcean.configureDevice(actor8);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 9 ******************"));
#endif
	enOcean.configureDevice(actor9);
#ifdef KDEBUG
	Debug.println(F("** READ Aktor 10 *****************"));
#endif
	enOcean.configureDevice(actorA);

#ifdef KDEBUG
	Debug.println(F("ID0: 0x%08x"), taster1.tasterId);
#endif

	// Init EnOcean operation if not yet done by any device during setup
	enOcean.init();

  
#ifdef KDEBUG
	Debug.println(F("Ready for normal operation"));
#endif
	digitalWrite(LED_YELLOW, LOW);
}

void loop()
{
	Knx.task();
	if (Konnekting.isReadyForApplication())
		enOcean.task();
}

void knxEvents(byte index)
{
	enOcean.handleKnxEvents(index);
}



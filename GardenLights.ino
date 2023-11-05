//#define _DEBUG

#define USE_SPECIALIST_METHODS // For TimeAlarms Class

#include "GardenLights.h"

#include <Wire.h>
#include <SPI.h>

#include <TimeLib.h>				// https://github.com/PaulStoffregen/Time
#include <TimeAlarms.h>				// https://github.com/PaulStoffregen/TimeAlarms
#include <DS1307RTC.h>				// https://github.com/PaulStoffregen/DS1307RTC
//#include <Button.h>					// https://github.com/t3db0t/Button/blob/master/readme.md
#include <Sunrise.h>				// http://www.andregoncalves.info/ag_blog/?p=47

#include <UIPEthernet.h>    // https://github.com/UIPEthernet/UIPEthernet
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient

#include <avr/wdt.h>

//Button rfPin1;
//Button rfPin2;
//Button rfPin3;
//Button rfPin4;
//Button rfPin5;
//Button rfPin6;
//Button rfPin7;
//Button rfPin8;
//Button rfPin9;
//Button rfPin10;
//Button rfPin11;
//Button rfPin12;
//
//Button btnPin1;
//Button btnPin2;
//Button btnPin3;
//Button btnPin4;
//
//Button btnPin5;
//Button btnPin6;
//Button btnPin7;
//Button btnPin8;
//
//Button btnPin9;
//Button btnPin10;
//Button btnPin11;
//Button btnPin12;
//
//Button btnPin13;
//Button btnPin14;
//Button btnPin15;
//Button btnPin16;

byte relayPins[RELAY_COUNT] = {
  PIN_LIGHT_GREEN,				// 1
  PIN_LIGHT_XXX,			    // 2
  PIN_LIGHT_DOWN_YARD,		// 3
  PIN_LIGHT_UPPER_YARD,		// 4

  PIN_LIGHT_BACK_YARD,		// 5
  PIN_LIGHT_YYY,					// 6
  PIN_LIGHT_STADIUM,		  // 7
  PIN_LIGHT_ZZZ,					// 8

  PIN_LIGHT_HOUSE_FRONT,			// 9
  PIN_LIGHT_FRONT_YARD,			// 10
  PIN_LIGHT_GAZEBO,			// 11
  PIN_LIGHT_SIDE_YARD   // 12
};

//Button* RfButtons[RF_CHANNEL_COUNT] = { &rfPin1, &rfPin2, &rfPin3, &rfPin4, &rfPin5, &rfPin6, &rfPin7, &rfPin8, &rfPin9, &rfPin10, &rfPin11, &rfPin12 };

//byte RfButtonsRelayMapping[RF_CHANNEL_COUNT - 2] = {
//  RELAY_LIGHT_HOUSE_FRONT, RELAY_LIGHT_FRONT_YARD, RELAY_LIGHT_GAZEBO, RELAY_LIGHT_SIDE_YARD,
//  RELAY_LIGHT_BACK_YARD, RELAY_LIGHT_UPPER_YARD, RELAY_LIGHT_DOWN_YARD, RELAY_LIGHT_STADIUM, RELAY_LIGHT_GREEN, RELAY_XXXX
//};

//Button* PushButtons[BUTTON_COUNT] = { &btnPin1, &btnPin2, &btnPin3, &btnPin4, &btnPin5, &btnPin6, &btnPin7, &btnPin8, &btnPin9, &btnPin10, &btnPin11, &btnPin12, &btnPin13, &btnPin14, &btnPin15, &btnPin16 };

unsigned long halfSecondTicks = 0;
unsigned long secondTicks = 0;

//unsigned int lightsControlerState = 0;

void setup()
{
	wdt_disable();

	Serial.begin(115200);
	Serial.println();
	Serial.println(F("Initializing.. ver. 3.0.7"));

	pinMode(PIN_BLINKING_LED, OUTPUT);
	digitalWrite(PIN_BLINKING_LED, LOW); // Turn on led at start

	pinMode(PIN_MANUAL_MODE_LED, OUTPUT);
	digitalWrite(PIN_MANUAL_MODE_LED, LOW); // Turn on manual mode led at start

	// Init relay
	for (byte i = 0; i < RELAY_COUNT; i++)
	{
		digitalWrite(relayPins[i], HIGH);
		pinMode(relayPins[i], OUTPUT);
	}

	if (dtNBR_ALARMS != 30)
		Serial.println("Alarm count mismatch");

	Wire.begin();

	InitSun(); // Sunrise / sunset

	//// RF PINS
//	rfPin1.Begin(RF_PIN_1, BUTTON_PULLDOWN, false);
//	rfPin2.Begin(RF_PIN_2, BUTTON_PULLDOWN, false);
//	rfPin3.Begin(RF_PIN_3, BUTTON_PULLDOWN, false);
//	rfPin4.Begin(RF_PIN_4, BUTTON_PULLDOWN, false);
//
//	rfPin5.Begin(RF_PIN_5, BUTTON_PULLDOWN, false);
//	rfPin6.Begin(RF_PIN_6, BUTTON_PULLDOWN, false);
//	rfPin7.Begin(RF_PIN_7, BUTTON_PULLDOWN, false);
//	rfPin8.Begin(RF_PIN_8, BUTTON_PULLDOWN, false);
//
//	rfPin9.Begin(RF_PIN_9, BUTTON_PULLDOWN, false);
//	rfPin10.Begin(RF_PIN_10, BUTTON_PULLDOWN, false);
//	rfPin11.Begin(RF_PIN_11, BUTTON_PULLDOWN, false);
//	rfPin12.Begin(RF_PIN_12, BUTTON_PULLDOWN, false);
//
//	// SWITCH BUTTONS
//	btnPin1.Begin(BUTTON_PIN_1, BUTTON_PULLUP, true);
//	btnPin2.Begin(BUTTON_PIN_2, BUTTON_PULLUP, true);
//	btnPin3.Begin(BUTTON_PIN_3, BUTTON_PULLUP, true);
//	btnPin4.Begin(BUTTON_PIN_4, BUTTON_PULLUP, true);
//
//	btnPin5.Begin(BUTTON_PIN_5, BUTTON_PULLUP, true);
//	btnPin6.Begin(BUTTON_PIN_6, BUTTON_PULLUP, true);
//	btnPin7.Begin(BUTTON_PIN_7, BUTTON_PULLUP, true);
//	btnPin8.Begin(BUTTON_PIN_8, BUTTON_PULLUP, true);
//
//	btnPin9.Begin(BUTTON_PIN_9, BUTTON_PULLUP, true);
//	btnPin10.Begin(BUTTON_PIN_10, BUTTON_PULLUP, true);
//	btnPin11.Begin(BUTTON_PIN_11, BUTTON_PULLUP, true);
//	btnPin12.Begin(BUTTON_PIN_12, BUTTON_PULLUP, true);
//
//	btnPin13.Begin(BUTTON_PIN_13, BUTTON_PULLUP, true);
//	btnPin14.Begin(BUTTON_PIN_14, BUTTON_PULLUP, true);
//	btnPin15.Begin(BUTTON_PIN_15, BUTTON_PULLUP, true);
//	btnPin16.Begin(BUTTON_PIN_16, BUTTON_PULLUP, true);

//	for (byte i = 0; i < RF_CHANNEL_COUNT; i++)
//		RfButtons[i]->process();
//
//	for (byte i = 0; i < BUTTON_COUNT; i++)
//		PushButtons[i]->process();

	readSettings();

	showNextEvent();

	InitEthernet();

	InitMqtt();

	wdt_enable(WDTO_8S);

	Serial.println("Start");
}

void loop()
{
	static unsigned long previousMillis = 0xFFFFFFFF; // will store last time LED was updated
	unsigned long _current_millis = millis();

	uint32_t dt = previousMillis > _current_millis ? 1 + previousMillis + ~_current_millis : _current_millis - previousMillis;

	if (dt >= 500)
	{
		wdt_reset();

		// save the last time we blinked the LED
		previousMillis = _current_millis;
		oncePerHalfSecond();
	}

	//Serial.println("Ping..");
	//delay(500);

//	// PROCESS SWITCH BUTTONS
//	for (byte i = 0; i < BUTTON_COUNT; i++)
//	{
//		if (PushButtons[i]->uniquePress())
//			ButtonPressed(i);
//	}
//
//	// PROCESS RF BUTTONS
//	for (byte i = 0; i < RF_CHANNEL_COUNT; i++)
//	{
//		if (RfButtons[i]->stateChanged())
//			RfButtonPressed(i);
//	}
//

	ProcessMqtt();

	Alarm.delay(0);
}

void oncePerHalfSecond(void)
{
	halfSecondTicks++;

	// Blinking
	static uint8_t blinkingLedState = LOW;
	//static uint8_t blinkingLedManualMode = LOW;

	blinkingLedState = !blinkingLedState;
	digitalWrite(PIN_BLINKING_LED, blinkingLedState);

	//if (!getAutomaticMode())
	//{
	//	blinkingLedManualMode = !blinkingLedManualMode;
	//	digitalWrite(PIN_MANUAL_MODE_LED, blinkingLedManualMode);
	//}

	if ((halfSecondTicks % 2) == 0)
		oncePerSecond();
}

void oncePerSecond()
{
	if ((secondTicks % 5) == 0)
		oncePer5Second();

	if ((secondTicks % 60) == 0)
		oncePer1Minute();

	secondTicks++;
}

void oncePer5Second()
{
	ReconnectMqtt();
}

void oncePer1Minute()
{
	if (secondTicks > 0) // do not publish on startup
		PublishAllStates();
}

//void RfButtonPressed(byte btn)
//{
//	if (btn >= RF_CHANNEL_COUNT)
//		return;
//
//	switch (btn)
//	{
//	case 10: // button 11
//		for (byte id = 0; id < RELAY_COUNT; id++)
//			relayOn(id);
//#ifdef _DEBUG
//		Serial.println("All Lights On");
//#endif
//		break;
//	case 11: // button 12
//		for (byte id = 0; id < RELAY_COUNT; id++)
//			relayOff(id);
//#ifdef _DEBUG
//		Serial.println("All Lights Off");
//#endif
//		break;
//	default:
//		byte relayId = RfButtonsRelayMapping[btn];
//#ifdef _DEBUG
//		Serial.print("Rf button pressed: ");
//		Serial.print(btn);
//		Serial.print(", Mapped to relay: ");
//		Serial.println(relayId);
//#endif
//		relayToggle(relayId);
//		break;
//	}
//}
//
//void ButtonPressed(byte btn)
//{
//#ifdef _DEBUG
//	Serial.print("Push button pressed: ");
//	Serial.println(btn);
//#endif
//
//	if (btn >= BUTTON_COUNT)
//		return;
//
//	//	bool isShiftOff = !btnPin17.isPressed(false);
//	//#ifdef _DEBUG
//	//	Serial.print("Shift: ");
//	//	Serial.println(!isShiftOff);
//	//#endif
//	bool state;
//
//	switch (btn)
//	{
//		//case BTN_HOUSE_1:
//		//	if (isShiftOff)
//		//		relayToggle(RELAY_LIGHT_HOUSE_FRONT);
//		//	else
//		//		relayToggle(RELAY_LIGHT_UPPER_YARD);
//		//	break;
//		//case BTN_HOUSE_2:
//		//	if (isShiftOff)
//		//		relayToggle(RELAY_LIGHT_FRONT_YARD);
//		//	else
//		//		relayToggle(RELAY_LIGHT_STADIUM);
//		//	break;
//		//case BTN_HOUSE_3:
//		//	if (isShiftOff)
//		//		relayToggle(RELAY_LIGHT_GAZEBO);
//		//	else
//		//		relayToggle(RELAY_LIGHT_GREEN);
//		//	break;
//		//case BTN_HOUSE_4:
//		//	if (isShiftOff)
//		//		relayToggle(RELAY_LIGHT_SIDE_YARD);
//		//	else
//		//		relayToggle(RELAY_XXXX);
//		//	break;
//		//case BTN_HOUSE_5:
//		//	if (isShiftOff)
//		//		relayToggle(RELAY_LIGHT_BACK_YARD);
//		//	else
//		//		relayToggle(RELAY_YYYY);
//		//	break;
//		//case BTN_HOUSE_6:
//		//	if (isShiftOff)
//		//		relayToggle(RELAY_LIGHT_DOWN_YARD);
//		//	else
//		//		relayToggle(RELAY_ZZZZ);
//		//	break;
//		//case BTN_HOUSE_7:
//		//	// Shift
//		//	break;
//
//	case BTN_GAZEBO_1:
//	case BTN_OUT_1:
//		relayToggle(RELAY_LIGHT_HOUSE_FRONT);
//		break;
//	case BTN_GAZEBO_2:
//	case BTN_OUT_2:
//		relayToggle(RELAY_LIGHT_FRONT_YARD);
//		break;
//	case BTN_GAZEBO_3:
//	case BTN_OUT_3:
//		relayToggle(RELAY_LIGHT_GAZEBO);
//		break;
//	case BTN_GAZEBO_4:
//	case BTN_OUT_4:
//		state = relayToggle(RELAY_LIGHT_SIDE_YARD);
//		relaySet(RELAY_LIGHT_BACK_YARD, state);
//		break;
//	case BTN_GAZEBO_5:
//	case BTN_OUT_5:
//		relayToggle(RELAY_LIGHT_DOWN_YARD);
//		break;
//
//	default:
//		break;
//	}
//}
//
void relaySet(byte id, bool state)
{
	if (state)
		relayOn(id);
	else
		relayOff(id);
}

void relayOn(byte id)
{
	if (id < RELAY_COUNT)
	{
		digitalWrite(relayPins[id], LOW);
		PublishLightState(id, true);
	}
}

void relayOff(byte id)
{
	if (id < RELAY_COUNT)
	{
		digitalWrite(relayPins[id], HIGH);
		PublishLightState(id, false);
	}
}

bool relayToggle(byte id)
{
	bool newState = false;
	if (id < RELAY_COUNT)
	{
		newState = digitalRead(relayPins[id]);
		digitalWrite(relayPins[id], !newState);
		PublishLightState(id, newState);
	}
	return newState;
}

bool isRelayOn(byte id)
{
	if (id < RELAY_COUNT)
		return !digitalRead(relayPins[id]);
	return false;
}

//boolean state_set_error_bit(int mask)
//{
//	if (!state_is_error_bit_set(mask))
//	{
//		lightsControlerState |= mask;
//		PublishControllerState();
//		return true;
//	}
  //
//	return false;
//}

//boolean state_clear_error_bit(int mask)
//{
//	if (state_is_error_bit_set(mask))
//	{
//		lightsControlerState &= ~mask;
//		PublishControllerState();
//		return true;
//	}
  //
//	return false;
//}

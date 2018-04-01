#include <arduino.h>

const byte RELAY_COUNT = 12;
const byte RF_CHANNEL_COUNT = 12;
const byte BUTTON_COUNT = 17; // total = 20. 3 is free

const byte PIN_BLINKING_LED = 2; // LED_BUILTIN;
const byte PIN_MANUAL_MODE_LED = 3;

const byte RF_PIN_1 = 14;
const byte RF_PIN_2 = 23;
const byte RF_PIN_3 = 25;
const byte RF_PIN_4 = 27;
const byte RF_PIN_5 = 29;
const byte RF_PIN_6 = 31;
const byte RF_PIN_7 = 33;
const byte RF_PIN_8 = 35;
const byte RF_PIN_9 = 37;
const byte RF_PIN_10 = 39;
const byte RF_PIN_11 = 41;
const byte RF_PIN_12 = 43;

const byte BUTTON_PIN_1 = A0;
const byte BUTTON_PIN_2 = A1;
const byte BUTTON_PIN_3 = A2;
const byte BUTTON_PIN_4 = A3;

const byte BUTTON_PIN_5 = A4;
const byte BUTTON_PIN_6 = A5;
const byte BUTTON_PIN_7 = A6;
const byte BUTTON_PIN_8 = A7;

const byte BUTTON_PIN_9 = A8;
const byte BUTTON_PIN_10 = A9;
const byte BUTTON_PIN_11 = A10;
const byte BUTTON_PIN_12 = A11;

const byte BUTTON_PIN_13 = A12;
const byte BUTTON_PIN_14 = A13;
const byte BUTTON_PIN_15 = A14;
const byte BUTTON_PIN_16 = A15;

const byte BUTTON_PIN_17 = 4;
const byte BUTTON_PIN_18 = 5;
const byte BUTTON_PIN_19 = 6;
const byte BUTTON_PIN_20 = 7;

const byte PIN_LIGHT_GREEN = 22;
const byte PIN_LIGHT_XXX = 24;
const byte PIN_LIGHT_DOWN_YARD = 26;
const byte PIN_LIGHT_UPPER_YARD = 28;

const byte PIN_LIGHT_BACK_YARD = 30;
const byte PIN_LIGHT_YYY = 32;
const byte PIN_LIGHT_STADIUM = 34;
const byte PIN_LIGHT_ZZZ = 36;

const byte PIN_LIGHT_HOUSE_FRONT = 38;
const byte PIN_LIGHT_FRONT_YARD = 40;
const byte PIN_LIGHT_GAZEBO = 42;
const byte PIN_LIGHT_SIDE_YARD = 44;

const byte BTN_GAZEBO_1 = 15; // Orange
const byte BTN_GAZEBO_2 = 14; // Orange/White
const byte BTN_GAZEBO_3 = 13; // Green
const byte BTN_GAZEBO_4 = 12; // Green/White
const byte BTN_GAZEBO_5 = 11; // Brown

const byte BTN_OUT_1 = 10;
const byte BTN_OUT_2 = 9;
const byte BTN_OUT_3 = 8;
const byte BTN_OUT_4 = 3;
const byte BTN_OUT_5 = 2;

const byte BTN_HOUSE_1 = 1;
const byte BTN_HOUSE_2 = 0;
const byte BTN_HOUSE_3 = 7;
const byte BTN_HOUSE_4 = 6;
const byte BTN_HOUSE_5 = 5;
const byte BTN_HOUSE_6 = 4;
const byte BTN_HOUSE_7 = 16;


const byte RELAY_LIGHT_GREEN = 0;
const byte RELAY_XXXX = 1;
const byte RELAY_LIGHT_DOWN_YARD = 2;
const byte RELAY_LIGHT_UPPER_YARD = 3;

const byte RELAY_LIGHT_BACK_YARD = 4;
const byte RELAY_YYYY = 5;
const byte RELAY_LIGHT_STADIUM = 6;
const byte RELAY_ZZZZ = 7;

const byte RELAY_LIGHT_HOUSE_FRONT = 8;
const byte RELAY_LIGHT_FRONT_YARD = 9;
const byte RELAY_LIGHT_GAZEBO = 10;
const byte RELAY_LIGHT_SIDE_YARD = 11;

typedef struct OnOffSettingStructure {

	bool isActive;
	int onOffset; // min. + is after sunset, - = before sunset
	char offType;
	int offValue; // min. 
	// for OFF_SUNRISE + is after sunsrise, - = before sunrise
	// for OFF_TIME + is time
	// for OFF_DURATION + is duration
};


const unsigned int ERR_GENERAL = 1;

#define state_is_error_bit_set(__mask__) ((lightsControlerState & (__mask__)) != 0)

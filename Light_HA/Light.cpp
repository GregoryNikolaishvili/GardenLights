#include <ArduinoHA.h>
#include "HALightRelay.h"
#include "Light_HA.h"
#include <UIPEthernet.h>  // https://github.com/UIPEthernet/UIPEthernet
#include <avr/wdt.h>

// LED and timer variables
unsigned long halfSecondTicks = 0;
unsigned long secondTicks = 0;

EthernetClient client;
HADevice device;
HAMqtt mqtt(client, device, RELAY_COUNT);

struct RelayConfig {
  const char* uniqueId;
  const char* name;
  byte pin;
};

RelayConfig relays[RELAY_COUNT] = {
  { "house_front", "House Front", PIN_LIGHT_HOUSE_FRONT },
  { "front_yard", "Front Yard", PIN_LIGHT_FRONT_YARD },
  { "side_yard", "Side Yard", PIN_LIGHT_SIDE_YARD },
  { "back_yard", "Back Yard", PIN_LIGHT_BACK_YARD },
  { "back_of_back_yard", "Back of Back Yard", PIN_LIGHT_BACK_OF_BACK_YARD },
  { "nana", "Nana", PIN_LIGHT_NANA },
  { "firepit", "Firepit", PIN_LIGHT_FIREPIT },
  { "lower_yard", "Lower Yard", PIN_LIGHT_LOWER_YARD },
  { "gazebo", "Gazebo", PIN_LIGHT_GAZEBO },
  { "stadium", "Stadium", PIN_LIGHT_STADIUM },
  { "green_front", "Green Front", PIN_LIGHT_GREEN_FRONT },
  { "green_side", "Green Side", PIN_LIGHT_GREEN_SIDE }
};

HALightRelayX* relayObjects[RELAY_COUNT];

void setup() {
  wdt_disable();

  Serial.begin(115200);
  Serial.println();
  Serial.println(F("Initializing.. ver. 4.0.0"));

  pinMode(PIN_BLINKING_LED, OUTPUT);
  digitalWrite(PIN_BLINKING_LED, LOW);  // Turn on LED at start

  // pinMode(PIN_MANUAL_MODE_LED, OUTPUT);
  // digitalWrite(PIN_MANUAL_MODE_LED, LOW);  // Turn on manual mode LED at start

  InitEthernet();

  // Init relay pins and create relay objects
  for (byte i = 0; i < RELAY_COUNT; i++) {
    RelayConfig relay = relays[i];

    digitalWrite(relay.pin, HIGH);
    pinMode(relay.pin, OUTPUT);

    relayObjects[i] = new HALightRelayX(relay.uniqueId, relay.name, relay.pin);
    relayObjects[i]->onCommand(onRelayCommand);
  }

  device.enableSharedAvailability();
  device.enableLastWill();
  device.setName("Light controller");
  device.setSoftwareVersion("4.0.0");
  device.setManufacturer("Gregory Nikolaishvili");

  InitMqtt();

  wdt_enable(WDTO_8S);

  Serial.println("Start");
}

void loop() {
  Ethernet.maintain();

  static unsigned long previousMillis = 0;  // will store last time LED was updated
  unsigned long currentMillis = millis();

  uint32_t dt = (currentMillis >= previousMillis) ? (currentMillis - previousMillis) : (1 + previousMillis + ~currentMillis);

  if (dt >= 500) {
    wdt_reset();

    previousMillis = currentMillis;
    oncePerHalfSecond();
  }

  mqtt.loop();
}

static void onRelayCommand(bool state, HASwitch* sender) {
  byte pin = ((HALightRelayX*)sender)->getPin();
  digitalWrite(pin, state ? LOW : HIGH);
  sender->setState(state);  // report state back to Home Assistant
}

static void oncePerHalfSecond(void) {
  halfSecondTicks++;

  // Blinking
  static uint8_t blinkingLedState = LOW;
  blinkingLedState = !blinkingLedState;
  digitalWrite(PIN_BLINKING_LED, blinkingLedState);
}

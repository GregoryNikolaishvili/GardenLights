#include "network.h"
#include <UIPEthernet.h>  // https://github.com/UIPEthernet/UIPEthernet
#include <ArduinoHA.h>

IPAddress ip(192, 168, 68, 9);
IPAddress gateway(192, 168, 68, 1);
IPAddress subnet(255, 255, 252, 0);

byte mac[] = {0x54, 0x34, 0x41, 0x30, 0x30, 0x09};

void initEthernet(HADevice &device)
{
  Serial.println(F("Starting ethernet.."));

  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  // W5100.setRetransmissionTime(0x07D0);
  // W5100.setRetransmissionCount(3);
  delay(2000);

  Serial.print(F("IP Address: "));
  Serial.println(Ethernet.localIP());

  device.setUniqueId(mac, sizeof(mac));
  // Serial.print(F("Chip revision: "));
  // Serial.println(enc.getrev());
}
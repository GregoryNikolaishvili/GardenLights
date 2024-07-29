#ifndef _NETWORK_H
#define _NETWORK_H

#define UIP_CONNECT_TIMEOUT 2 //  UIP_CONNECT_TIMEOUT was set to -1 in uipethernet-conf.h

#include <ArduinoHA.h>

#define MQTT_BROKER "192.168.68.25"
#define MQTT_USERNAME "cha"
#define MQTT_PASSWORD "BatoBato02@"

void initEthernet(HADevice& device);

#endif
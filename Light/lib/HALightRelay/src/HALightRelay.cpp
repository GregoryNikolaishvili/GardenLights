#ifndef _HALIGHTRELAY
#define _HALIGHTRELAY

#include "HALightRelay.h"

HALightRelayX::HALightRelayX(const char* uniqueId, const char* name, byte pinId)
	: HALight(uniqueId), _pinId(pinId)
{
  setState(LOW);
  setName(name);
  setIcon("mdi:lightbulb");
}

#endif

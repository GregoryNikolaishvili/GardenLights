#ifndef _HALIGHTRELAY_H
#define _HALIGHTRELAY_H

#include "HALightRelay.h"

HALightRelayX::HALightRelayX(const char* uniqueId, const char* name, byte pinId)
	: HALight(uniqueId), _pinId(pinId)
{
  setState(LOW);
  setName(name);
  setIcon("mdi:lightbulb");
}

#endif

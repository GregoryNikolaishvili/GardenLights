#ifndef _HALIGHTRELAYX_h
#define _HALIGHTRELAYX_h

#include <ArduinoHA.h>

class HALightRelayX : public HALight
{
public:
  HALightRelayX(const char *uniqueId, const char *name, byte pinId);

  inline byte getPin() const
  {
    return _pinId;
  }

private:
  byte _pinId;

  /// The name that was set using setName method. It can be nullptr.
  const char *_name;
};

#endif

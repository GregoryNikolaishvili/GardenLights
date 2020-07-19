bool automaticMode = false;
OnOffSettingStructure onOffSettings[RELAY_COUNT];

int sunriseMin;
int sunsetMin;

const byte DEF_SETTINGS_VERSION = 0x00;
const int STORAGE_ADDRESS_SETTINGS = 0;
//const int STORAGE_ADDRESS_DATA = 100;

const char OFF_SUNRISE = 'S';
const char OFF_TIME = 'T';
const char OFF_DURATION = 'D';

void setAutomaticMode(bool value)
{
  automaticMode = value;
  digitalWrite(PIN_MANUAL_MODE_LED, value);

  if (automaticMode)
    setCurrentRelayStates();
}

//bool inline getAutomaticMode()
//{
//	return automaticMode;
//}

void readSettings()
{
  for (byte i = 0; i < RELAY_COUNT; i++)
  {
    onOffSettings[i].isActive = false;
    onOffSettings[i].onOffset = 0;
    onOffSettings[i].offType = OFF_SUNRISE;
    onOffSettings[i].offValue = 0;
  }

  byte v = eeprom_read_byte((uint8_t *)STORAGE_ADDRESS_SETTINGS);
  if (v != DEF_SETTINGS_VERSION)
  {
    eeprom_update_byte((uint8_t *)STORAGE_ADDRESS_SETTINGS, DEF_SETTINGS_VERSION);
    saveSettings(false);

    //saveData("00180;1;2;3;4;5;6;7;8;9;A;B;", 28);
  }
  else
  {
    setAutomaticMode(false); // switch to manual mode
    bool auto_mode = eeprom_read_byte((uint8_t *)(STORAGE_ADDRESS_SETTINGS + 1)); // just read here and set is later

    eeprom_read_block((void*)&onOffSettings, (void*)(STORAGE_ADDRESS_SETTINGS + 2), sizeof(onOffSettings));
    for (byte i = 0; i < RELAY_COUNT; i++)
    {
      if ((onOffSettings[i].offType != OFF_SUNRISE) && (onOffSettings[i].offType != OFF_TIME) && (onOffSettings[i].offType != OFF_DURATION))
        onOffSettings[i].offType = OFF_SUNRISE;
    }
    settingsChanged(false);
    setAutomaticMode(auto_mode);
  }
}

void saveSettings(bool publish)
{
  eeprom_update_byte((uint8_t *)(STORAGE_ADDRESS_SETTINGS + 1), automaticMode);
  eeprom_update_block((const void*)&onOffSettings, (void*)(STORAGE_ADDRESS_SETTINGS + 2), sizeof(onOffSettings));

  settingsChanged(publish);
}

void settingsChanged(bool publish)
{
  resetAlarms(0, 0);

  if (publish)
	  PublishSettings();
}

//void saveData(const void* data, int length)
//{
//  if (length > 256)
//    length = 256;
//
//  eeprom_update_word((uint16_t *)STORAGE_ADDRESS_DATA, length);
//  eeprom_update_block(data, (void*)(STORAGE_ADDRESS_DATA + 2), length);
//}
//

void resetAlarms(int tag, int tag2)
{
  recalcSunriseSunset();

  freeAlarms();

  Alarm.alarmRepeat(0, resetAlarms, NULL); // 00:00:00 at midnight. eventName = NULL means don't show in next event

  OnOffSettingStructure* onOff;
  for (byte id = 0; id < RELAY_COUNT; id++)
  {
    onOff = &onOffSettings[id];
    if (onOff->isActive)
    {
      time_t tm_on = getOnTime(onOff);
      time_t tm_off = getOffTime(onOff);

      Alarm.alarmRepeat(tm_on, relayOnCheckMode, "Relay ON", (int)id);
      Alarm.alarmRepeat(tm_off, relayOffCheckMode, "Relay OFF", (int)id);

      Serial.print("Relay #");
      Serial.print(id);
      Serial.print(". ON: ");
      printTime(&Serial, tm_on);

      Serial.print(", OFF: ");
      printTime(&Serial, tm_off);
      Serial.println();
    }
  }

  if (automaticMode)
    setCurrentRelayStates();
}

void freeAlarms()
{
  for (byte id = 0; id < dtNBR_ALARMS; id++)
  {
    if (Alarm.isAllocated(id))
      Alarm.free(id);
  }
}

void relayOnCheckMode(int id, int tag2)
{
  if (automaticMode)
    relayOn(id);
  else
  {
    Serial.print("Relay On skipped: #");
    Serial.println(id);
  }
  showNextEvent();
}

void relayOffCheckMode(int id, int tag2)
{
  if (automaticMode)
    relayOff(id);
  else
  {
    Serial.print("Relay Off skipped: #");
    Serial.println(id);
  }
  showNextEvent();
}

void showNextEvent()
{
  AlarmClass* alarm = Alarm.getNextTriggerAlarm();
  if (alarm)
  {
    Serial.print("Next alarm event at: ");
    printTime(&Serial, alarm->value);
    Serial.print(", ");
    Serial.println(alarm->eventName);
  }
}


time_t getOnTime(OnOffSettingStructure* onOff)
{
  return (sunsetMin + onOff->onOffset) * SECS_PER_MIN;
}


time_t getOffTime(OnOffSettingStructure* onOff)
{
  time_t value = 0;
  switch (onOff->offType)
  {
    case OFF_SUNRISE:
      value = (sunriseMin + onOff->offValue) * SECS_PER_MIN;
      break;
    case OFF_TIME:
      value = onOff->offValue * SECS_PER_MIN;
      break;
    case OFF_DURATION:
      value = getOnTime(onOff) + onOff->offValue * SECS_PER_MIN;
      break;
  }
  return value;
}

void setCurrentRelayStates()
{
  OnOffSettingStructure* onOff;
  for (byte id = 0; id < RELAY_COUNT; id++)
  {
    onOff = &onOffSettings[id];
    if (onOff->isActive)
    {
      time_t tm_on = getOnTime(onOff);
      time_t tm_off = getOffTime(onOff);

      relaySet(id, getRelayStateByTime(tm_on / SECS_PER_MIN, tm_off / SECS_PER_MIN));
    }
    else
      relaySet(id, false);
  }
}

boolean getRelayStateByTime(int onTime, int offTime)
{
  time_t time_now = now();
  time_now = time_now - previousMidnight(time_now);
  int itime_now = time_now / SECS_PER_MIN;

//  Serial.print("onTime: ");
//  Serial.println(onTime);
//  Serial.print("offTime: ");
//  Serial.println(offTime);

  boolean b;
  if (onTime < offTime)
  {
    b = (itime_now >= onTime) && (itime_now < offTime);
  }
  else if (onTime > offTime)
  {
    int t = onTime;
    onTime = offTime;
    offTime = t;
    b = !((itime_now >= onTime) && (itime_now < offTime));
  }
  else // if equals
  {
    b = false;
  }
  return b;
}

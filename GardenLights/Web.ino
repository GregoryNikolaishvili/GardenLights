//byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x09 };
//IPAddress ip(192, 168, 2, 9);
//EthernetServer server(80);
//
//void InitWeb()
//{
//  Serial.println("Starting web server..");
//
//  Ethernet.begin(mac, ip);
//  server.begin();
//
//  Serial.print("IP Address: ");
//  Serial.println(Ethernet.localIP());
//}
//
//void ProcessWeb()
//{
//  size_t size;
//
//  if (EthernetClient client = server.available())
//  {
//    while ((size = client.available()) > 0)
//    {
//      uint8_t* msg = (uint8_t*)malloc(size + 1);
//      size = client.read(msg, size);
//      Serial.write(msg, size);
//      Serial.println();
//
//      msg[size] = 0;
//      ProcessCommand(&client, (const char*)msg);
//
//      //Serial.print("Size=");
//      //Serial.print(size);
//      //Serial.print(": ");
//      //Serial.print(": ");
//      //for (int i = 0; i < size; i++)
//      //{
//      //	Serial.print(data[i], HEX);
//      //	Serial.print(' ');
//      //}
//      //Serial.println();
//
//      free(msg);
//    }
//    client.stop();
//  }
//}
//
//
//void ProcessCommand(EthernetClient *client, const char* command)
//{
//  if (command[0] == '#') // set relay
//  {
//    byte id = hexCharToByte(command[1]);
//    if (id >= RELAY_COUNT)
//      return;
//
//    if (command[2] == '0')
//      relayOff(id);
//    else
//      relayOn(id);
//  }
//  else if (command[0] == '@') // get settings
//  {
//    OutputSettings(client);
//  }
//  else if (command[0] == 'A')
//  {
//    setAutomaticMode(true);
//    saveSettings();
//  }
//  else if (command[0] == 'M')
//  {
//    setAutomaticMode(false);
//    saveSettings();
//  }
//  else if (command[0] == '*') // set all settings
//  {
//    // *TT0001S0000F3401S6753T0000S0000F0000S0000F0000S0000F0000S0000F0000S0000F0000S0000F0000S0000F0000S0000F0000S0000F0000S0000
//
//    setAutomaticMode(command[1] != 'F');
//    command += 2;
//
//    for (byte id = 0; id < RELAY_COUNT; id++)
//    {
//      onOffSettings[id].isActive = *command != 'F';
//      command++;
//
//      onOffSettings[id].onOffset = readHex(command, 4);
//      command += 4;
//
//      onOffSettings[id].offType = *command;
//      command++;
//
//      onOffSettings[id].offValue = readHex(command, 4);
//      command += 4;
//    }
//
//    command++; // *
//
//    int length = readHex(command, 4);
//    Serial.print("save data. len=");
//    Serial.println(length);
//    Serial.println(command);
//    saveData(command, length + 4);
//    saveSettings();
//  }
//
//  OutputState(client);
//}
//
//
//void OutputState(Print* client)
//{
//  // $ + 12 switches + autoatic_mode + datetime  + sunrise/sunset 123456789012TYYMMDDHHmmssxxxxxxxx. 34 chars
//
//  client->print('$');
//  client->print(automaticMode ? 'T' : 'F');
//
//  // Relays
//  for (byte id = 0; id < RELAY_COUNT; id++)
//    client->print(isRelayOn(id));
//
//  //Time
//  time_t _now = now();
//
//  print2Digits(client, byte(year(_now) - 2000));
//  print2Digits(client, byte(month(_now)));
//  print2Digits(client, byte(day(_now)));
//
//  print2Digits(client, byte(hour(_now)));
//  print2Digits(client, byte(minute(_now)));
//  print2Digits(client, byte(second(_now)));
//
//  printHexInt16(client, sunriseMin);
//  printHexInt16(client, sunsetMin);
//
//  client->print('\r');
//  client->print('\n');
//}
//
//void OutputSettings(Print* client)
//{
//  // *
//  client->print('*');
//  client->print(automaticMode ? 'T' : 'F');
//
//  for (byte i = 0; i < RELAY_COUNT; i++)
//  {
//    client->print(onOffSettings[i].isActive ? 'T' : 'F');
//    printHexInt16(client, onOffSettings[i].onOffset);
//    client->print(onOffSettings[i].offType);
//    printHexInt16(client, onOffSettings[i].offValue);
//  }
//
//  client->print('*');
//  int len = eeprom_read_word((uint16_t *)STORAGE_ADDRESS_DATA);
//  Serial.print("load data. len=");
//  Serial.println(len);
//
//  for (int i = 0; i < len; i++)
//  {
//    byte b = eeprom_read_byte((uint8_t *)(STORAGE_ADDRESS_DATA + 2 + i));
//    client->write(b);
//  }
//}

#include "mqtt.h"
//#include "utility/w5500.h"

#define UIP_CONNECT_TIMEOUT	2
//  UIP_CONNECT_TIMEOUT was set to -1 in uipethernet-conf.h

byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x09 };
IPAddress ip(192, 168, 1, 9);

EthernetClient ethClient;
PubSubClient mqttClient("192.168.1.23", 1883, callback, ethClient);     // Initialize a MQTT mqttClient instance

bool doLog = true;

#define MQTT_BUFFER_SIZE 256

char buffer[MQTT_BUFFER_SIZE];

void InitEthernet()
{
	Serial.println(F("Starting ethernet.."));

	Ethernet.begin(mac, ip);
	//W5100.setRetransmissionTime(0x07D0);
	//W5100.setRetransmissionCount(3);
	delay(2000);

	Serial.print(F("IP Address: "));
	Serial.println(Ethernet.localIP());

	//Serial.print(F("Chip revision: "));
	//Serial.println(enc.getrev());
}

void InitMqtt()
{
	mqttClient.setBufferSize(320);
	mqttClient.setSocketTimeout(5);
	ReconnectMqtt();
}

void ProcessMqtt()
{
	mqttClient.loop();
}

void PublishMqtt(const char* topic, const char* message, int len, boolean retained)
{
	if (doLog)
	{
		Serial.print(F("Publish. topic="));
		Serial.print(topic);
		Serial.print(F(", length="));
		Serial.print(len);

		Serial.print(F(", payload="));
		for (int i = 0; i < len; i++)
			Serial.print(message[i]);
		Serial.println();
	}
	mqttClient.publish(topic, (const byte*)message, len, retained);
}

void PublishAlive()
{
	if (!mqttClient.connected()) return;

	const char* topic = "cha/lc/alive";
	int len = setHexInt32(buffer, now() - 4L * SECS_PER_HOUR, 0);
	PublishMqtt(topic, buffer, len, false);
}

void ReconnectMqtt() {

	if (!mqttClient.connected()) {

		Serial.print("Connecting to MQTT broker...");

		wdt_reset();
		// Attempt to connect
		if (mqttClient.connect("light", "hub/controller/light", 1, true, "{\"state\":\"disconnected\"}")) {
			Serial.println("connected");

			wdt_reset();
			// Once connected, publish an announcement...
			mqttClient.publish("hub/controller/light", "{\"state\":\"connected\"}", true);  // Publish ethernet connected status to MQTT topic

			wdt_reset();
			// ... and resubscribe
			mqttClient.subscribe("chac/lc/#", 1);           // Subscribe to a MQTT topic, qos = 1

			mqttClient.publish("hubcommand/gettime2", "chac/lc/settime2", false);     // request time

			wdt_reset();
			PublishSettings();
			PublishAllStates();
		}
		else {
			Serial.print("failed, rc=");
			Serial.println(mqttClient.state());

			//      wdt_reset();
			//      
			//      enc.powerOff();
			//      delay(2000);
			//      wdt_reset();
			//      
			//      enc.powerOn();
			//      delay(2000);
			//      wdt_reset();
			//      
			//      Ethernet.begin(mac, ip);
			//      delay(2000);
		}
	}
	wdt_reset();
}

void PublishAllStates() {
	if (!mqttClient.connected()) return;

	doLog = false;

	for (byte id = 0; id < RELAY_COUNT; id++)
	{
		PublishLightState(id, isRelayOn(id));
	}

	doLog = true;
}

void PublishLightState(byte id, bool value)
{
	if (!mqttClient.connected()) return;

	char topic[12];
	strcpy(topic, "cha/lc/rs/?");
	topic[10] = byteToHexChar(id);

	PublishMqtt(topic, value ? "1" : "0", 1, true);
}

void PublishSettings()
{
	if (!mqttClient.connected()) return;

	const char* topic = "cha/lc/settings";
	int idx = 0;

	buffer[idx++] = automaticMode ? 'T' : 'F';
	for (byte i = 0; i < RELAY_COUNT; i++)
	{
		buffer[idx++] = onOffSettings[i].isActive ? 'T' : 'F';
		idx = setHexInt16(buffer, onOffSettings[i].onOffset, idx);
		buffer[idx++] = onOffSettings[i].offType;
		idx = setHexInt16(buffer, onOffSettings[i].offValue, idx);

		idx = setHexInt16(buffer, onOffTimes[i].onTime, idx);
		idx = setHexInt16(buffer, onOffTimes[i].offTime, idx);
	}

	PublishMqtt(topic, buffer, idx, true);
}

void PublishTime()
{
	if (!mqttClient.connected()) return;

	const char* topic = "cha/lc/time";
	int len = setHexInt32(buffer, now() - 4L * SECS_PER_HOUR, 0);
	PublishMqtt(topic, buffer, len, false);
}

//void PublishNamesAndOrder()
//{
//	if (!mqttClient.connected()) return;
//
//	const char* topic = "cha/lc/names";
//
//	int length = eeprom_read_word((uint16_t *)STORAGE_ADDRESS_DATA);
//	Serial.print("load name & order data. len=");
//	Serial.println(length);
//
//	for (int i = 0; i < length; i++)
//	{
//		byte b = eeprom_read_byte((uint8_t *)(STORAGE_ADDRESS_DATA + 2 + i));
//		buffer[i] = b;
//	}
//
//	PublishMqtt(topic, buffer, length, true);
//}


void callback(char* topic, byte * payload, unsigned int len) {

	Serial.print("message arrived: topic='");
	Serial.print(topic);
	Serial.print("', length=");
	Serial.print(len);
	Serial.print(", payload=");
	Serial.write(payload, len);
	Serial.println();

	if (strcmp(topic, "chac/lc/alive") == 0)
	{
		PublishAlive();
		return;
	}

	if (strcmp(topic, "chac/lc/gettime2") == 0)
	{
		PublishTime();
		return;
	}

	if (strcmp(topic, "chac/lc/refresh") == 0)
	{
		PublishAllStates();
		return;
	}

	if (len == 0)
		return;

	if (strncmp(topic, "chac/lc/state/", 14) == 0)
	{
		byte id = hexCharToByte(topic[14]);
		bool value = payload[0] != '0';
		//Serial.print("id=");
		//Serial.print(id);
		//Serial.print(", value=");
		//Serial.println(value);

		relaySet(id, value);
		return;
	}

	if (strcmp(topic, "chac/lc/mode") == 0)
	{
		setAutomaticMode(payload[0] == 'A');
		saveSettings(true);
		return;
	}

	if (strncmp(topic, "chac/lc/settings2/", 18) == 0)
	{
		byte id = hexCharToByte(topic[18]);
		char* p = (char*)payload;

		onOffSettings[id].isActive = *p != '0';
		p++;

		onOffSettings[id].onOffset = readHexInt16(p);
		p += 4;

		onOffSettings[id].offType = *p;
		p++;

		onOffSettings[id].offValue = readHexInt16(p);
		//p += 4;

		saveSettings(true);

		PublishLightState(id, isRelayOn(id));
		return;
	}

	if (strcmp(topic, "chac/lc/settime2") == 0)
	{
		char* data = (char*)payload;
		long tm = readHexInt32(data);


		setTime(tm + 4L * SECS_PER_HOUR);
		RTC.set(now());
		printDateTime(&Serial, now());
		Serial.println();
		return;
	}

	if (strcmp(topic, "chac/lc/settime") == 0)
	{
		char* data = (char*)payload;
		int yr, month, day;
		int hr, min, sec;

		yr = 2000 + (*data++ - '0') * 10;
		yr += (*data++ - '0');

		month = (*data++ - '0') * 10;
		month += (*data++ - '0');

		day = (*data++ - '0') * 10;
		day += (*data++ - '0');

		data++;

		hr = (*data++ - '0') * 10;
		hr += (*data++ - '0');
		min = (*data++ - '0') * 10;
		min += (*data++ - '0');
		sec = (*data++ - '0') * 10;
		sec += (*data++ - '0');

		setTime(hr, min, sec, day, month, yr);
		RTC.set(now());
		printDateTime(&Serial, now());
		Serial.println();
		return;
	}
}

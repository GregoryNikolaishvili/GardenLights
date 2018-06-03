// what is our longitude (west values negative) and latitude (south values negative)
float const LATITUDE = 41.822154;
float const LONGITUDE = 44.7340106;

Sunrise mySunrise(LATITUDE, LONGITUDE, 4);

void InitSun()
{
	// Time
	setSyncProvider(RTC.get);   // the function to get the time from the RTC
	unsigned long startMillis = millis();
	do
	{
		delay(100);
		if (timeStatus() == timeSet)
			break;
	} while (millis() - startMillis < 1000);

#ifdef _DEBUG
	printDateTime(&Serial, now());
	Serial.println();
#endif
	// Time

	mySunrise.Civil();

	//recalcSunriseSunset();
}

void recalcSunriseSunset()
{
	time_t _now = now();
	
	int mm = month(_now);
	int dd = day(_now);

	mySunrise.Rise(mm, dd); // (month,day) - january=1
	byte sunriseHH = mySunrise.Hour();
	byte sunriseMM = mySunrise.Minute();
	Serial.print("Sunrise: ");
	if (sunriseHH < 10)
		Serial.print('0');
	Serial.print(sunriseHH);
	Serial.print(":");
	if (sunriseMM < 10)
		Serial.print('0');
	Serial.println(sunriseMM);

	mySunrise.Set(mm, dd); // (month,day) - january=1
	byte sunsetHH = mySunrise.Hour();
	byte sunsetMM = mySunrise.Minute();
	Serial.print("Sunset: ");
	if (sunsetHH < 10)
		Serial.print('0');
	Serial.print(sunsetHH);
	Serial.print(":");
	if (sunsetMM < 10)
		Serial.print('0');
	Serial.println(sunsetMM);

	sunriseMin = sunriseHH * 60 + sunriseMM;
	sunsetMin = sunsetHH * 60 + sunsetMM;
}


#include "SimpleTimer.h"
#include "TinyGPS++.h"
#include <stdlib.h>
#include <SoftwareSerial.h>

static const int gpsRXPin = 3, gpsTXPin = 4;
static const uint32_t GPSBaudrate = 9600;

TinyGPSPlus gps_obj;
SoftwareSerial ss(gpsRXPin, gpsTXPin);
SoftwareSerial SIM900(7, 8);

String textForSMS;

double longitude;
double latitude;
char buff[10];
String mylong = "";
String mylati = "";
SimpleTimer timer;
int secs = 0;
int minutes = 0;
int mflag = 0;

void setup()
{
  SIM900.begin(19200);
  Serial.begin(9600);
  ss.begin(GPSBaudrate);
  delay(5000); 
  Serial.println(" Logging completed!");
  timer.setInterval(1000);
  Serial.println(gps_obj.satellites.value());
  delay(200);
  Serial.println("sim?");
  Serial.println(SIM900.print("AT+CSPN?\r"));
  delay(100);
  Serial.println(SIM900.print("AT+CREG?\r"));
  Serial.println("sim status is before this");
}

void sendSMS(String message)
{
  SIM900.println("AT+CMGF=1\r");
  delay(100);
  SIM900.println("AT+CMGS=\"6361152920\"");
  delay(100);
  SIM900.println(message);
  delay(100);
  SIM900.println((char)26);
  delay(100);
  SIM900.println();
  delay(5000);					
}

void loop()
{
  while (ss.available() > 0)
  {
    if (gps_obj.encode(ss.read()))
    {
 			displayInfo();
    }
  }
  if(millis() > 3500 && gps_obj.charsProcessed() < 10)
  {
    		Serial.println("No GPS detected: Check the circuit wiring.");
    	  while(true);
  }

  if(timer.isReady())
  {
    Secs_To_Mins();
    timer.reset();	
  }
  displayInfo();	
	if(mflag == 1) 
	{
    displayInfo();
		latitude = gps_obj.location.lat(), 6 ;    
 	  longitude = gps_obj.location.lng(), 6 ;
    mylati = dtostrf(latitude, 3, 6, buff);	
    mylong = dtostrf(longitude, 3, 6, buff);
    textForSMS = "";
    textForSMS = textForSMS + "Latitude:" + "," + mylati + "," + "Longitude:  "+ mylong + ","; 
    sendSMS(textForSMS);
    Serial.println(textForSMS);
    Serial.println("Message sent.");
    delay(3000);
    mflag = 0;
  }
}

void displayInfo()
{
  Serial.print("Location: ");
  if(gps_obj.location.isValid())	
  {
    Serial.print(gps_obj.location.lat(), 6);
    Serial.print(",");
    Serial.print(gps_obj.location.lng(), 6);
    Serial.print(" ");
    Serial.print("Speed:");
    Serial.print(gps_obj.speed.kmph());
  }
  else
  {
    Serial.print("INVALID");
  }
  Serial.print("  Date/Time: ");	
  if(gps_obj.date.isValid())
  {
    Serial.print(gps_obj.date.month());
    Serial.print("/");
   	Serial.print(gps_obj.date.day());
   	Serial.print("/");
    Serial.print(gps_obj.date.year());
  }
  else
  {
  	Serial.print("INVALID");
  }
  Serial.print(" ");
  if(gps_obj.time.isValid())	
  {
    	if (gps_obj.time.hour() < 10)
	    {
        Serial.print("0");
      }
    	Serial.print(gps_obj.time.hour());
    	Serial.print(" : ");
    	if(gps_obj.time.minute() < 10)
      {
        Serial.print("0");
      }
    	Serial.print(gps_obj.time.minute());
   	  Serial.print(" : ");
   	  if(gps_obj.time.second() < 10)
      {
        Serial.print("0");
      }
   	  Serial.print(gps_obj.time.second());
   	  Serial.print(".");
   	  if(gps_obj.time.centisecond() < 10)
      {
        Serial.print("0");
      }
   	  Serial.print(gps_obj.time.centisecond());
  }
  else
  {
    	Serial.print("INVALID");
  }
  Serial.println();
}

void Secs_To_Mins()
{
  secs += 1;
  Serial.println("Seconds:");
  Serial.println(secs);
  if ( secs >= 59 ) 
  {
      minutes += 1;
    	secs = 0;
    	Serial.println("Minutes:");
   	  Serial.println(minutes);
   	  if(minutes >= 2)
   	  {
   			minutes = 0;
 			  secs = 0;
 			  mflag = 1;
   	 	} 
  }
}

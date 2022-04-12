// Rayko Panteleev
// ESP8266 ESP-01 Internet real time clock
 
 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h> 
#include <NTPClient.h>               // Include NTPClient library
#include <Timezone.h>    // https://github.com/JChristensen/Timezone
#include <TM1637Display.h>       // Include LED Display library

const int CLK = 2; // pin D4 NodeMCU or GPIO 2 ESP-01
const int DIO = 0; // pin D3 NodeMCU or GPIO 0 ESP-01 
TM1637Display display(CLK, DIO); //init the display.
 
const char *ssid     = "YOUR_SSID"; //Replace with your Network ID
const char *password = "your_password"; //Replace with your Password

byte second_, minute_, hour_; 
byte last_second = 60;
int hours;
int minutes;
int delayPeriod = 59800; // Adjust the offline 1 minute delay duration and sync interval

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov", 0, 60000); 

// Time Change Rules
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 3, 120};     // 120 EU Summer Time (UTC +3 Hours);  -240 (UTC -4 Hours)
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 4, 60};       // 60 EU Standard Time (UTC +2 Hours); -300 (UTC -5 Hours)
Timezone CE(CEST, CET);

void setup() {
 
  Serial.begin(115200);
  display.clear();
  display.setBrightness(1); //set diplay maximum brightness (0-7)
 
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting...");
 if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    WiFi.reconnect();    
  }
  Serial.println("connected");
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true); 
  timeClient.begin();
}
 
void loop() {
 
ntpClock();  
}

void ntpClock(){
 
  timeClient.update();
  unsigned long unix_epoch = CE.toLocal(timeClient.getEpochTime()); // timeClient.getEpochTime() Get Unix epoch time from the NTP server // CE.toLocal - Timezone + daylight saving correction from Timezone.h
 
  second_ = second(unix_epoch);
  if (last_second != second_) { 
      
  delayPeriod = delayPeriod -(second_ * 1000); // Sync seconds   
    minute_ = minute(unix_epoch);
    hour_   = hour(unix_epoch);

    hours = hour_;
    minutes = minute_; 
    last_second = second_;       
        
  }
  // Prints time on the serial monitor
    Serial.print(hours);
    Serial.print(":");
    if (minutes < 10) {
      Serial.print("0");
      Serial.println(minutes);        
    }
    else Serial.println(minutes);
    
  // Prints time on the LED Display  
  display.showNumberDecEx(hours,  (0x80 >> 1), false, 2, 0); //Displays the hour value; (0x80 displays colon)
  display.showNumberDec(minutes, true, 2, 2); //Displays the minute value;  
  
  delay(delayPeriod); // Wait 1 min.
  
  // Offline time 
  minutes++;
  if (minutes == 60) {
    minutes = 0;
    hours++;
    if (hours == 24) hours = 0;
  } 
}

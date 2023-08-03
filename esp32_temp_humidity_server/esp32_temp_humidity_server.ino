

/*
    LCD_I2C - Arduino library to control a 16x2 LCD via an I2C adapter based on PCF8574

    Copyright(C) 2020 Blackhack <davidaristi.0504@gmail.com>

    This program is free software : you can redistribute it and /or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

/*
Board type:  uPesy ESP32 Wroom DevKit
115200 baud
*/
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <LCD_I2C.h>
#include <DHT.h>
#include <NTPClient.h>
#include <TimeLib.h>

const char* ntpServer = "ntp.nict.jp";
const long gmtOffsetInSeconds = 9 * 3600; // Tokyo is GMT+9
const int daylightOffsetInSeconds = 0;     // Tokyo does not have daylight saving time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffsetInSeconds, daylightOffsetInSeconds);


// Replace with your own network credentials
WebServer server(8080);
// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

const char* ssid = "tp-Link-xXx-239";
const char* password = "DontUseThis1234";

DHT dht(26, DHT22);

LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules, change according
float t = 0.0;
float h = 0.0;

void setup()
{
  dht.begin(); // start dht.
  delay(2000);
  Serial.begin(115200);
  lcd.begin(); // If you are using more I2C devices using the Wire library use lcd.begin(false)
                 // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();
    
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi Network ..");
 
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
 
  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  
  server.on("/",handle_OnConnect); 
  server.begin();
  Serial.println("HTTP server started");

  configTime(gmtOffsetInSeconds, daylightOffsetInSeconds, ntpServer);
  timeClient.begin();
}

void loop()
{
  lcd.backlight();
  timeClient.update(); 

  t = dht.readTemperature();
  h = dht.readHumidity();
  // Get current time
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  
  // Extract the year, month, and day
  int year = ptm->tm_year + 1900;
  int month = ptm->tm_mon + 1;
  int day = ptm->tm_mday;
  char date_value[9];
  snprintf(date_value, sizeof(date_value), "%02d.%02d.%02d", year % 100, month, day);

  String now = timeClient.getFormattedTime();
  String formattedTime = String(now).substring(0, 16);
  char time_value[20];
  formattedTime.toCharArray(time_value, 13);
  lcd.print(date_value);
  lcd.print("   ");
  lcd.print(time_value);
  lcd.setCursor(0, 1);


  lcd.print("T:");
  lcd.print(t);
  lcd.print("C ");  
  lcd.print("H:");
  lcd.print(h);
  lcd.setCursor(0, 0);
  
  server.handleClient();
  delay(1000);
  lcd.clear();
  
  
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(t, h)); 
}

String SendHTML(float t,float h){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Temp and Humidity</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  ptr +="<p>Temp: </p> \n";
  ptr += String(t);
  ptr += "C";
  ptr += "<p>Humidity: </p> \n";
  ptr += String(h);
  ptr += "%";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

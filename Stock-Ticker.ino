/*******************************************************************
 *  Written by Brian Lough and updated by Enzo Jimenez-Soto
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

#include "StockTicker.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Arduino.h"
#include <WiFiClientSecure.h>
#include "JsonStreamingParser.h"
#include <ArduinoJson.h>
#include "theSwedishMaker.h"

// ----------------------------
// Standard Libraries - Already Installed if you have ESP8266 set up
// ----------------------------

#include <ESP8266WiFi.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

// Define the typ of hardware and the pins used. 

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   12
#define DATA_PIN  15
#define CS_PIN    13

// Hardware SPI connection
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
char ssid[] = "************";         // your network SSID (name)
char password[] = "**********"; // your network key

String FINNHUB_ACCESS_TOKEN = "*************"; // FINNHUB API ACCESS TOKEN / API KEY
String STOCK_ID = "*******"; // STOCK ID ex. GME (TO THE MOON!)

WiFiClientSecure client;
StockTicker stockticker(client, FINNHUB_ACCESS_TOKEN, STOCK_ID);

unsigned long delayBetweenChecks = 600000; //mean time between api requests
unsigned long whenDueToCheck = 0;

void setup()
{
  P.begin();
  P.setFont(fontSubs);

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  //Set client insecure for https connections
  client.setInsecure();
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // If using ESP8266 Core 2.5 RC, uncomment the following
  // client.setInsecure();
}

void getStockCountForStock()
{
  Serial.println("Getting stock price for " + STOCK_ID);
  P.print("fetching");
  float stockCount = stockticker.getStock(STOCK_ID);

  

  Serial.println("Response:");
  Serial.print("Stock at: ");
  Serial.println(stockCount);
  P.print(stockCount); 
}


void loop()
{
  unsigned long timeNow = millis();
  if ((timeNow > whenDueToCheck))
  {
    getStockCountForStock();
    whenDueToCheck = timeNow + delayBetweenChecks;
  }
}

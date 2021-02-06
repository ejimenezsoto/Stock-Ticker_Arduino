
#include "StockTicker.h"

StockTicker::StockTicker(Client &client, String accessToken, String stockId = "") {
  this->client = &client;
  _accessToken = accessToken;
  _stockId = stockId;
}

String StockTicker::sendGetToIFinnHub(String command) {
  String body;
  String headers;
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  unsigned long now;
  bool avail;

  // Connect with facebook api over ssl
  if (client->connect(STOCK_HOST, STOCK_SSL_PORT)) {
    if (_debug) Serial.println(".... connected to server");
    String a="";
    char c;
    int ch_count=0;
    client->println("GET " + command +  " HTTP/1.1");
    client->println("Host: " STOCK_HOST);
    client->println("User-Agent: arduino/1.0.0");
    client->println("");
    now=millis();
    avail=false;
    while (millis() - now < STOCK_TIMEOUT) {
      while (client->available()) {

        // Allow body to be parsed before finishing
        avail = finishedHeaders;
        char c = client->read();
        // Serial.write(c);

        if(!finishedHeaders){
          if (currentLineIsBlank && c == '\n') {
            finishedHeaders = true;
            //Serial.println(headers);
          }
          else{
            headers = headers + c;

          }
        } else {
          if (ch_count < maxMessageLength)  {
            body=body+c;
            ch_count++;
          }
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
      if (avail) {
        // For some reason the body is coming back with a number on the outside of the start and end of it, we only want the JsonObject
        float firstJsonCharacterIndex = body.indexOf("{");
        float lastJsonCharacterIndex = body.lastIndexOf("}");
        if(firstJsonCharacterIndex != -1 && lastJsonCharacterIndex != -1){
          body = body.substring(firstJsonCharacterIndex, lastJsonCharacterIndex + 1);
        }
        // Serial.println("Body:");
        // Serial.println(body);
        // Serial.println("END");
        break;
      }
    }
  }
  else{
    Serial.println(".... error connecting to server");
  }
  return body;
}

float StockTicker::getStock(String stockId) {
  String command = "/api/v1/quote?symbol=" + _stockId + "&token=" + _accessToken;
  String response = sendGetToIFinnHub(command);

  if (_debug) Serial.println(response);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);

  if(root.success()) {
    if(root.containsKey("c")) {
      return root["c"].as<float>();
    } else {
      Serial.println("JSON response was not as expected");
    }
  } else {
    Serial.println("Failed to parse JSON");
  }
  return -1;
}

void StockTicker::closeClient() {
  if(client->connected()){
    client->stop();
  }
}

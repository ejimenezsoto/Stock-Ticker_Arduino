

#ifndef StockTicker_h
#define StockTicker_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define STOCK_HOST "finnhub.io"
#define STOCK_SSL_PORT 443
#define STOCK_TIMEOUT 1500


class StockTicker {
 public:
  StockTicker(Client &client, String accessToken, String stockId);
  float getStock(String stockId);
  String sendGetToIFinnHub(String command);
  bool _debug = false;

 private:
   Client *client;
   String _stockId;
   String _accessToken;
   const int maxMessageLength = 10000;
  void closeClient();
};
#endif

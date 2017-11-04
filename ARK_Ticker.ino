
/*
  ****************************************
    ARK_Ticker.ino
    sleepdeficit 11/2017
  ****************************************
*/

/* **************************** Configuration ***************************** */
/* ******************************* */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <DNSServer.h>

//------- Install From Library Manager -------
#include <ArduinoJson.h>
#include <CoinMarketCapApi.h>
#include <WiFiManager.h> 
/* ******************************* */

/* ******************************* */
WiFiClientSecure client;
CoinMarketCapApi api(client);

unsigned long api_mtbs = 10000; //10 seconds; mean time between api requests; 10req/min limit
unsigned long api_due_time = 0;

String HOSTNAME = "arkticker"; // http://arkticker
WiFiServer server(80);

String arkData;

bool isAP = true;
bool isSTA = false;
bool isServer = false;
/* ******************************* */
/* **************************** /Configuration ***************************** */



/* ***************************** Library Setup ***************************** */
/* **************** Setup mDNS *** */
void setupMDNS()
{
  if (!MDNS.begin("esp8266"))
  { 
    while(1)
    { delay(1000); }
  }
  server.begin();
  MDNS.addService("http", "tcp", 80);
}
/* ******************************* */
/* **************************** /Library Setup ***************************** */



/* ******************************* Functions ******************************* */
/* ********** Check Connection *** */
void checkConnection()
{
  if ( WiFi.status() != WL_CONNECTED )
  {
    WiFiManager wifiManager;
    if (!wifiManager.startConfigPortal("ARK: Ticker"))
    {
      delay(3000);
      ESP.reset();
      delay(5000);
    }
    isAP = false;
    isSTA = true;
  }
  else if ( (WiFi.status() == WL_CONNECTED) && (isSTA == true))
  {
    setupMDNS();
    isSTA = false;
    isServer = true;
  }
  else if ( (WiFi.status() == WL_CONNECTED) && (isServer == true) )
  {
    runServer();
    updateTicker(); 
  }
  else if ( (WiFi.status() == WL_DISCONNECTED) && (isServer == true) )
  {
    isServer = false;
  }
}
/* ******************************* */

/* **************** Run Server *** */
void runServer()
{
  WiFiClient client = server.available();
  if (!client) { return; }

  while(client.connected() && !client.available())
  { delay(1); }
  
  String req = client.readStringUntil('\r');
  
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1)
  { return; }
  
  req = req.substring(addr_start + 1, addr_end);
  client.flush();
  
  String s;
  if (req == "/")
  {
    IPAddress ip = WiFi.localIP();
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n";
    s += "<style>\r\n";
    s += "h1\r\n{\r\ncolor: #0f8;\r\nfont-size: 24px;\r\nfont-family: courier new, monospace;}\r\n";
    s += "body{\r\nbackground-color: #494949;\r\ncolor: #0f8;\r\nfont-size: 16px;\r\nfont-family: courier new, monospace;}\r\n";
    s +=  "</style>\r\n</head>\r\n";
    s += "<h1>Welcome Aboard!<br>\r\n</h1>\r\n<body>\r\n";
    s += arkData;
    s += "\r\n</body>\r\n</html>\r\n\r\n";
    Serial.println("Sending 200");
  }
  else
  {
    s = "HTTP/1.1 404 Not Found\r\n\r\n";
    Serial.println("Sending 404");
  }
  client.print(s);
}
/* ******************************* */

/* ************* Update Ticker *** */
void updateTicker()
{
  unsigned long timeNow = millis();
  if (timeNow > api_due_time)
  {
    refreshArkData();
    api_due_time = timeNow + api_mtbs;
  }
}
/* ******************************* */

/* ********** Refresh Ark Data *** */
void refreshArkData()
{
  arkData = "";
  arkData += "--------------- Ѧ ---------------- <br><br>";
  CMCTickerResponse response = api.GetTickerInfo("ark", "usd");
  if (response.error == "")
  {
    String name = "name: " + response.name + "<br>";
    String symbol = "symbol: " + response.symbol + "<br>";
    String rank = "rank: " + String(response.rank) + "<br><br>";
    String ark_usd = "ARK | USD: " + String(response.price_usd) + " USD <br>";
    String ark_btc = "ARK | BTC: " + String(response.price_btc, 8) + " BTC <br>";
    double price_btc = getBTCPrice();
    String btc_usd = "BTC | USD: " + String(price_btc) + " USD <br><br>";
    String twenty_four_hr_volume_usd = "24hr volume: " + String(response.volume_usd_24h) + " USD <br><br>";
    String market_cap = "market cap: " + String(response.market_cap_usd) + " USD <br><br>";
    String available_supply = "available supply: " + String(response.available_supply) + " ARK <br>";
    String total_supply = "total supply: " + String(response.total_supply) + " ARK <br><br>";
    String percent_change_1h = "percent change 1h: " + String(response.percent_change_1h) + "%<br>";
    String percent_change_24h = "percent change 24h: " + String(response.percent_change_24h) + "%<br>";
    String percent_change_7d = "percent change 7d: " + String(response.percent_change_7d) + "%<br><br>";
    String last_updated = "last updated: " + String(response.last_updated) + "<br>";
    arkData = name + symbol + rank;
    arkData += ark_usd + ark_btc + btc_usd;
    arkData += twenty_four_hr_volume_usd + market_cap;
    arkData += available_supply + total_supply;
    arkData += percent_change_1h + percent_change_24h + percent_change_7d + last_updated;
    arkData += "----------------------------------<br>";
  }
  else
  {
    String error = "error getting data: " + response.error + "\r\n";
    Serial.print(error);
  }
}
/* ******************************* */

/* ***************** BTC Price *** */
double getBTCPrice()
{
  CMCTickerResponse response = api.GetTickerInfo("bitcoin", "usd");
    if (response.error == "") {
    return response.price_usd;
  }
}
/* ******************************* */
/* ****************************** /Functions ******************************* */



/* ******************************* Board Setup ****************************** */
void setup()
{
  Serial.begin(115200);
  WiFi.hostname(HOSTNAME);
}
/* ****************************** /Board Setup ****************************** */



/* ********************************** Loop ********************************** */
void loop()
{
  if (millis() % 1000 == 0)
  {
  checkConnection();
  }
  
}
/* ********************************* /Loop ********************************** */

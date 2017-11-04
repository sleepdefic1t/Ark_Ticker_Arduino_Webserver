# Ark_Ticker_Arduino_Webserver

Ark: Ticker Arduino Webserver | Configure WiFi via AP |  http://arkticker

Using ESP8266

### What is this?

It is an Ark Price ticker that uses an Adafruit Huzzah ESP8266 Feather to make an AP-configurable Wireless Webserver.


### Required Libraries


Install using the Arduino IDE's Additional Boards Manager:  
*****Arduino IDE***** > ***Preferences*** > ***'Additional Boards Manager URL's'***

| | |
|:--- |:--- |
| **ESP8266 Library** | add the following line:<br>`http://arduino.esp8266.com/stable/package_esp8266com_index.json` |


Install these libraries' folders to the Arduino Library folder    
***~/Documents/Arduino/libraries***  

| | |
|:--- |:--- |
| | |
| **Arduino JSON Library** | https://github.com/bblanchon/ArduinoJson |
| | |
| **CoinMarketCap API Library** | https://github.com/bblanchon/ArduinoJson |
| | |
| **WiFiManager Library** | https://github.com/tzapu/WiFiManager |
| | |

-----

## How to Use it

1. Install the libraries, and flash the included ***'.ino'*** file.

2. First boot will make an access point named 'Ark: Ticker'

3. Upon connection to the AP, You will be greeted with a WifiManager Window.  
  Enter Scan or Enter your WiFi credentials accordingly.  
  The Window will close when the connection to your specified WiFi is established.  

4. Open your browser, and navigate to http://arkticker

5. Share this project and/or contribute it and make it even better! 




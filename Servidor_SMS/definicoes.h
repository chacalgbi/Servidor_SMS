// Sempre lembrar de definir o tamanho pra 4MB (FS:1Mb OTA:~1019)
#include "FS.h"
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <SSD1306Wire.h>

#define config_wifi 16
#define reset_sms   14

SSD1306Wire display1(0x3c, 4, 5);

WiFiClient client;
ESP8266WebServer server(80);
StaticJsonDocument<350> doc;

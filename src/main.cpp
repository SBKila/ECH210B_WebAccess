#include <Arduino.h>

#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

#include "config.h"

#include "webPages.h"
#include "modbusRegBank.h"

#define MAGIC 932

#define STATE_SETUP 0
#define STATE_RESTART 1
#define STATE_IDLE 2
#define STATE_READREGISTER 3

boolean modeSettings = false;
int state = STATE_IDLE;
boolean system_configured = false;
AsyncWebServer m_WiFiServer(80);


#include "settingMgr.h"
#include "echMgr.h"
#include "otaMgr.h"

#define DEBUG
#ifdef DEBUG
#define DEBUG_INIT(x) Serial.begin(x)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTLN_DEC(x) Serial.println(x, DEC)
#else
#define DEBUG_INIT(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTLN_DEC(x)
#endif

const char *http_username = "admin";
const char *http_password = "admin";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setupRunningMode()
{
  DEBUG_PRINTLN("setupRunningMode");

  WiFi.mode(WIFI_STA);
  WiFi.hostname(mySettings.hostname);
  DEBUG_PRINT("connecting wifi.");
  WiFi.begin(mySettings.ssid_name, mySettings.ssid_key);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(500);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("");

  IPAddress IP = WiFi.localIP();
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(IP);

  MDNS.begin(mySettings.hostname);

  SPIFFS.begin();
#ifdef ESP32
  m_WiFiServer.addHandler(new SPIFFSEditor(SPIFFS, http_username, http_password));
#elif defined(ESP8266)
  m_WiFiServer.addHandler(new SPIFFSEditor(http_username, http_password));
#endif

  m_WiFiServer.on("/register", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("GET /register");
    if (request->hasParam("addr"))
    {
      String value = request->getParam("addr", false)->value();
      uint16_t regiterContent;
      uint8_t result = readRegisters(value.toInt(), regiterContent);
      if (0 == result)
      {
        request->send(200, "text/plain", String(regiterContent));
      }
      else
      {
        request->send(404, "text/plain", String(result));
      }
    }
    else
    {
      request->send(SPIFFS, "/data.json", "application/json; charset=utf-8", false, registerProcessor);
    }
    DEBUG_PRINTLN("END GET /register");
  });

  m_WiFiServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("GET /");
    request->send(SPIFFS, "/board.html", "text/html; charset=utf-8", false, registerProcessor);
    DEBUG_PRINTLN("END GET /");
  });

  m_WiFiServer.onNotFound(notFound);
  m_WiFiServer.begin();

  wifi_station_set_hostname(mySettings.hostname);
  MDNS.addService("http", "tcp", 80);

  setupOTAUpdate();
  setupECH();

  DEBUG_PRINTLN("END_setupRunningMode");
}



void setupSettingMode()
{
  DEBUG_PRINTLN("setupSettingMode");
  mySettings.tag = MAGIC;

  // setup Access Point for 1 connection
  boolean result = WiFi.softAP(mySettings.hostname, "password", 8, false, 1);
  DEBUG_PRINT("Setup AP: ");
  DEBUG_PRINTLN(result);

  IPAddress IP = WiFi.softAPIP();
  DEBUG_PRINT("AP IP address: ");
  DEBUG_PRINTLN(IP);

  SPIFFS.begin();
#ifdef ESP32
  m_WiFiServer.addHandler(new SPIFFSEditor(SPIFFS, http_username, http_password));
#elif defined(ESP8266)
  m_WiFiServer.addHandler(new SPIFFSEditor(http_username, http_password));
#endif
  //Route for root / web page
  m_WiFiServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("GET /");
    request->send(SPIFFS, "/setup.html", "text/html; charset=utf-8", false, settingsProcessor);
  });
  m_WiFiServer.onNotFound(notFound);

  // Route to save settings / web page
  m_WiFiServer.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("GET /settings");
    if (request->hasParam("ssid-name", true))
    {
      String value = request->getParam("ssid-name", true)->value();
      if (value.length() != 0)
      {
        value.toCharArray(mySettings.ssid_name, 32);
        DEBUG_PRINT("ssid_name:");
        DEBUG_PRINTLN(mySettings.ssid_name);
      }
    }
    if (request->hasParam("ssid-pwd", true))
    {
      String value = request->getParam("ssid-pwd", true)->value();
      if (value.length() != 0)
      {
        value.toCharArray(mySettings.ssid_key, 63);
        DEBUG_PRINT("ssid_key:");
        DEBUG_PRINTLN(mySettings.ssid_key);
      }
    }

    if (request->hasParam("echModbusId", true))
    {
      String value = request->getParam("echModbusId", true)->value();
      if (value.length() != 0)
      {
        mySettings.ech_Modbus_Id = value.toInt();
        DEBUG_PRINT("ech_Modbus_Id:");
        DEBUG_PRINTLN(mySettings.ech_Modbus_Id);
      }
    }

    if (request->hasParam("echReadPeiod", true))
    {
      String value = request->getParam("echReadPeiod", true)->value();
      if (value.length() != 0)
      {
        mySettings.ech_Read_Period = value.toInt();
        DEBUG_PRINT("ech_Read_Period:");
        DEBUG_PRINTLN(mySettings.ech_Read_Period);
      }
    }

    if (request->hasParam("hostname", true))
    {
      String value = request->getParam("hostname", true)->value();
      if (value.length() != 0)
      {
        value.toCharArray(mySettings.hostname, 32);
        DEBUG_PRINT("hostmane:");
        DEBUG_PRINTLN(mySettings.hostname);
      }
    }
    request->send(SPIFFS, "/setupcompleted.html");
    storeSettings();
    state = STATE_RESTART;
  });
  m_WiFiServer.begin();
  DEBUG_PRINTLN("END_setupSettingMode");
}

void setup()
{
  DEBUG_INIT(9600);
  delay(3000);
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("setup");

  pinMode(SETUP_PIN, INPUT_PULLUP);
  // reload config
  loadSettings();
    
  if (digitalRead(SETUP_PIN) == LOW)
  {
    modeSettings = true;
  }
  else if (mySettings.tag != MAGIC)
  {
    mySettings.ech_Modbus_Id=1;
    mySettings.ech_Read_Period = 60;
    String("echmonitor").toCharArray(mySettings.hostname,32);
    modeSettings = true;
  }

  WiFi.reconnect();

  if (modeSettings)
  {
    setupSettingMode();
  }
  else
  {
    setupRunningMode();
  }

  DEBUG_PRINTLN("ENDsetup");
}

void loopSettingMode()
{
  if (state == STATE_RESTART)
  {
    DEBUG_PRINTLN("Reset..");
    delay(5000);
    ESP.restart();
  }
}

void loopRunningMode()
{
  if(!isOTAUpdate){
    loopECH();
  }
  loopOTAUpdate();
}

void loop()
{
  if (modeSettings)
  {
    loopSettingMode();
  }
  else
  {
    loopRunningMode();
  }
}
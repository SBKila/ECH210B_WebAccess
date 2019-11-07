#include <Arduino.h>
#include <EEPROM.h>
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
#include "settings.h"
#include "webPages.h"
#include "DataMgr.h"

#define MAGIC 932

#define SETUP_PIN 12

#define STATE_SETUP 0
#define STATE_RESTART 1
#define STATE_IDLE 2

boolean modeSettings = false;
int state = STATE_IDLE;
boolean system_configured = false;
AsyncWebServer m_WiFiServer(80);

settings mySettings;

#include "echMgr.h"


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

void loadSettings()
{
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, mySettings);
  EEPROM.end();
  DEBUG_PRINT("loaded:");
  DEBUG_PRINT(mySettings.tag);
  DEBUG_PRINT(",");
  DEBUG_PRINT(mySettings.ssid_name);
  DEBUG_PRINT(",");
  DEBUG_PRINTLN(mySettings.ssid_key);
  EEPROM.put(0, mySettings);
  DEBUG_PRINTLN("Settings loaded");
}
void storeSettings()
{
  EEPROM.begin(sizeof(struct settings));
  DEBUG_PRINT("store:");
  DEBUG_PRINT(mySettings.tag);
  DEBUG_PRINT(",");
  DEBUG_PRINT(mySettings.ssid_name);
  DEBUG_PRINT(",");
  DEBUG_PRINTLN(mySettings.ssid_key);
  EEPROM.put(0, mySettings);
  DEBUG_PRINTLN("Settings stored");
  EEPROM.end();
}
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String getRegisterProcessor(const String &var)
{
  if (var == "SD1")
    return (DataMgr::getSD1() != UNDEFINED) ? String(DataMgr::getSD1(), DEC) : "";
  if (var == "SD2")
    return (DataMgr::getSD2() != UNDEFINED) ? String(DataMgr::getSD2(), DEC) : "";
  if (var == "SD3")
    return (DataMgr::getSD3() != UNDEFINED) ? String(DataMgr::getSD3(), DEC) : "";
  if (var == "SD4")
    return (DataMgr::getSD4() != UNDEFINED) ? String(DataMgr::getSD4(), DEC) : "";
  if (var == "COMP")
    return (DataMgr::getDigitalOutput() & 0x01) ? "ON" : "OFF";
  if (var == "PUMP")
    return (DataMgr::getDigitalOutput() & 0x02) ? "ON" : "OFF";
  if (var == "VALVE")
    return (DataMgr::getDigitalOutput() & 0x04) ? "ON" : "OFF";
  if (var == "RDZ")
    return (DataMgr::getDigitalOutput() & 0x08) ? "ON" : "OFF";

  if (var == "HP")
    return (DataMgr::getDigitalInput() & 0x40) ? "ON" : "OFF";
  if (var == "BP")
    return (DataMgr::getDigitalInput() & 0x80) ? "ON" : "OFF";
  if (var == "FLUX")
    return (DataMgr::getDigitalInput() & 0x08) ? "ON" : "OFF";
  if (var == "HC")
    return (DataMgr::getDigitalInput() & 0x10) ? "ON" : "OFF";
  if (var == "OFS")
    return (DataMgr::getDigitalInput() & 0x20) ? "ON" : "OFF";
  if (var == "ECHCOM")
    return DataMgr::getEchComStatus() ? "OK" : "KO";
  return String();
}

void setupRunningMode()
{
  DEBUG_PRINTLN("setupRunningMode");

  WiFi.mode(WIFI_STA);
  WiFi.hostname("echmonitor");
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

  MDNS.begin("echmonitor");
  MDNS.addService("http", "tcp", 80);

  SPIFFS.begin();
#ifdef ESP32
  m_WiFiServer.addHandler(new SPIFFSEditor(SPIFFS, http_username, http_password));
#elif defined(ESP8266)
  m_WiFiServer.addHandler(new SPIFFSEditor(http_username, http_password));
#endif
  m_WiFiServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("GET /");
    request->send(SPIFFS, "/board.html", "text/html; charset=utf-8", false, getRegisterProcessor);
    DEBUG_PRINTLN("END GET /");
  });

  m_WiFiServer.on("/register", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("GET /register");
    if(request->hasParam("addr")){
      String value = request->getParam("ssid-name", true)->value();
      uint16_t regiterContent;
      if(0==readRegisters(value.toInt(), regiterContent)){
        request->send(200, "text/plain", String(regiterContent));
      } else {
        request->send(404);
      }
    } else {
    request->send(SPIFFS, "/data.json", "application/json; charset=utf-8", false, getRegisterProcessor);
    }
    DEBUG_PRINTLN("END GET /register");
  });

  m_WiFiServer.onNotFound(notFound);
  m_WiFiServer.begin();

  setupECH();

  DEBUG_PRINTLN("END_setupRunningMode");
}

void setupSettingMode()
{
  DEBUG_PRINTLN("setupSettingMode");
  mySettings.tag = MAGIC;

  // setup Access Point for 1 connection
  boolean result = WiFi.softAP("echmonitor-setup", "password", 8, false, 1);
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
    request->send(SPIFFS, "/setup.html");
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
    DEBUG_PRINTLN("1");
    // request->send(200, "text/html", setupcompleted_html);
    request->send(SPIFFS, "/setupcompleted.html");
    DEBUG_PRINTLN("2");
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
  if (digitalRead(SETUP_PIN) == LOW)
  {
    modeSettings = true;
  }
  else
  {
    // reload config
    loadSettings();
    if (mySettings.tag != MAGIC)
    {
      modeSettings = true;
    }
  }

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
  switch (state)
  {
  case STATE_IDLE:
    break;
  case STATE_SETUP:
    break;
  }

  loopECH();
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
  //  delay(10000); // wait for a second
}
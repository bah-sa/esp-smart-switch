#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
/*#include <DNSServer.h>*/
#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include <NTPClient.h>
//#include <WiFiUdp.h>
#include <ESP8266Ping.h>
#include <Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BMP280.h>
#include <SimpleDHT.h>
#include "FS.h"
#include "progmemConstants.h"
#include "StringExt.cpp"

/*
 * Устройство имеет три основных режима работы с нагрузкой:
 * 1. SWITCHER   - Переключатель
 * 2. RESETTER   - Сбрасыватель
 * 3. THERMOSTAT - Термостат
 * 
 * При первом включении устройства (если credentials для подключения к WiFi еще не установлены),
 * или если при включении была зажата кнопка "SETUP",
 * устройство переходит в режим точки доступа (AP) с открытой сетью.
 * Подключитесь к сети с именем "ESP8266_AP_XXXX", 
 * введите в браузере "http://192.168.10.1"
 * и выполните настройку устройства.
 */
#define debug true // вывод отладочных сообщений

#define array_count(array) sizeof(array)/sizeof(array[0])
#define firmware_version "0.9.5.7"

#define P0 1013.25 
BMP280 bmp; 

ADC_MODE(ADC_VCC);

char www_username[16];
char www_password[16];

/* Web Server Port */
int http_port = 80;

/* Tickers */ 
Ticker ticker;
Ticker resetticker;

/* setup button GPIO16 PIN=D0 !!! Вообще-то GPIO16 рекомендуют для использования только OUTPUT !!! */ 
const int setup_btn=16;
int setupButtonState=1;

/* Фотосенсор GPIO14 PIN=D5 */
int photosensor_pin=14;
/* Состояние нагрузки, управляемой фотосенсором */
int socket_state=0; /* 0-отключена, 1-включена, default=0 */

/* Нагрузка GPIO16 PIN=D0 */
int socket_pin=16; /* На этом же ПИН висит кнопка SETUP, но это допустимо, т.к. состояние кнопки проверяем только при старте устройства. */
int socket_mode=0;
boolean switcher_photosensor_enabled=false;
boolean switcher_timers_enabled=false;
boolean resetter_ping_checker_enabled=false;
boolean resetter_timers_enabled=false;
//const int reset_period = 10; // Продолжительность RESET'a в секундах

/* Narodmon Options */
boolean send2narodmon_enabled = true;
boolean send2NarodmonFlag;
int narodmonSendInterval = 5; // 5*60 = 300 sec.
//int narodmonSendIntervalCounter = -1; // Начальное значение счетчика = -1, т.к. firstInterruptInterval = 30 секунд, а не 60 сек.
int narodmonSendIntervalCounter = 0;

/* Шина OneWire (DS18B20) */
int onewire_pin; //=1; /* GPIO1  PIN=D10 */
int onewire_precision; //=10; /* 0.25C */
/* DHT22 */
int dht22_pin; //=2; /* GPIO2 PIN=D4 */

/* Soft AP */
char softAP_ssid[16];

/* Soft AP network parameters */
IPAddress apIP(192, 168, 10, 1);
IPAddress netMask(255, 255, 255, 0);

/* Thermostat Options */
int thermostat_mode; /* Режим работы термостата, 0 - нагрузка обогреваетель, 1 - нагрузка охладитель */
float thermostat_temperature; /* Желаемая температура */
float thermostat_deviation; /* Отклонение от желаемой температуры */
DeviceAddress thermostat_sensor; /* Адрес термодатчика */
//DeviceAddress nonselectedThermosensor =   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//DeviceAddress rtcThermosensor =   { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
//DeviceAddress dht22Thermosensor = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
//DeviceAddress bmp280Thermosensor = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };

DeviceAddress THERMOSENSOR_NON_SELECTED = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
DeviceAddress THERMOSENSOR_RTC          = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
DeviceAddress THERMOSENSOR_DHT22        = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
DeviceAddress THERMOSENSOR_BMP280       = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };

boolean thermostatFlag = false;

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
//const char *myHostname = "esp8266";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";
boolean   stDhcpFlag;
IPAddress stIP;
IPAddress stGateway;
IPAddress stSubnet;
IPAddress stDns1;
IPAddress stDns2;
//int eepromCredentialBlockSize = sizeof(ssid)+sizeof(password)+sizeof(stDhcpFlag)+sizeof(stIP)+sizeof(stGateway)+sizeof(stSubnet)+sizeof(stDns1)+sizeof(stDns2)+3;

// DNS server
/*
const byte DNS_PORT = 53;
DNSServer dnsServer;
*/
// Web server
//ESP8266WebServer server(80);
ESP8266WebServer server;

/* Should I connect to WLAN asap? */
boolean connect;

/* Last time I tried to connect to WLAN */
long lastConnectTry = 0;

/* Current WLAN status */
int status = WL_IDLE_STATUS;

/* WiFi status text */
//String status_t[7] = {"WL_IDLE_STATUS","WL_NO_SSID_AVAIL","WL_SCAN_COMPLETED","WL_CONNECTED","WL_CONNECT_FAILED","WL_CONNECTION_LOST","WL_DISCONNECTED"};
String status_t[7] = {FPSTR(WL_IDLE_STATUS_S),FPSTR(WL_NO_SSID_AVAIL_S),FPSTR(WL_SCAN_COMPLETED_S),FPSTR(WL_CONNECTED_S),FPSTR(WL_CONNECT_FAILED_S),FPSTR(WL_CONNECTION_LOST_S),FPSTR(WL_DISCONNECTED_S)};

/* RTC */
RtcDS3231<TwoWire> Rtc(Wire);

/* Интервал прерываний */
const byte nextInterruptInterval  = 60; // seconds

/* NTP parameters runtime values */
int ntpTimeZone;
char ntpServer[32];
int ntpSyncInterval;

int ntpSyncIntervalCounter = 999; // счетчик интервалов (нач.значение=max, чтобы при первом же после старта прерывании произошла синхронизация времени с NTP-сервером
boolean ntpUpdateFlag;

/* Ping Checker options */
IPAddress resetter_ping_IP;
boolean pingFlag;
int resetter_ping_fault_limit = RESETTER_PING_DEFAULT_FAULT_LIMIT;
int resetter_ping_fault_counter = 0;


void getSetupButtonState() {
  pinMode(setup_btn,INPUT);
  setupButtonState = digitalRead(setup_btn);
}

/* $$$$$$$$$$$$ */
int cnt=0;
void timerIsr()
{ cnt++;
  Serial.print("cnt=");
  Serial.println(cnt);
}


void setup() {
  Serial.begin(115200);
  Serial.println();

  /* Инициализация входа фотосенсора  */
  pinMode(photosensor_pin, INPUT_PULLUP);

  /* Инициализация модуля RTC */
  if (!RTC_setup()) { /* Если не удалось инициализировать модуль RTC */
    //ticker.attach(nextInterruptInterval, handleInterrupt); /* инициализируем тикер */
  }
  ticker.attach(nextInterruptInterval, handleInterrupt); /* инициализируем тикер */

  printCurrentDateTime(); 
  //Serial.println(" Starting...");
  Serial.println(FPSTR(APP_STARTING_S));

  /* Check for SETUP button pressed */
  getSetupButtonState();
  if (setupButtonState==LOW)
    Serial.println("SETUP BUTTON pressed.");

  /* Load Options from EEPROM */
  loadNtpBlock();
  loadSensorsBlock();
  loadSwitchBlock();

  /* OTA init */
  ArduinoOTA.begin();

  int webPort=80;
  if (loadCredentialsBlock() && setupButtonState==HIGH) { // Load WLAN credentials from EEPROM
    /* Start WiFi in station mode. */
    webPort = http_port;
    Serial.println("STATION mode.");
    WiFi.mode(WIFI_STA);
    connect = strlen(ssid) > 0; // Request WLAN connect if SETUP button not pressed and there is a SSID
  }
  else {
    /* Start WiFi in access point mode. */
    Serial.println("SOFT AP mode.");
    
    /* Get AP MAC-address */
    byte apmac[6];                     // the MAC address of your Wifi shield
    WiFi.softAPmacAddress(apmac);

    /* Concatenate Soft AP ssid "prefix + last 4 MAC hex digits" */
    //String softAP_ssid_string = softAP_ssid_prefix + String(apmac[1], HEX) + String(apmac[0], HEX) ;
    String softAP_ssid_string = FPSTR(AP_SSID_PREFIX_S); softAP_ssid_string += lpad(String(apmac[1], HEX)) + lpad(String(apmac[0], HEX)) ;
    
    softAP_ssid_string.toUpperCase();
    softAP_ssid_string.toCharArray(softAP_ssid, 16);

    WiFi.mode(WIFI_AP);
    //WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(apIP, apIP, netMask);
    WiFi.softAP(softAP_ssid);
    
    delay(500); // Without delay I've seen the IP address blank
    Serial.print("Soft AP "); Serial.print(softAP_ssid); Serial.print(" started. IP address: "); Serial.println(WiFi.softAPIP());
  }

  /* Setup the DNS server redirecting all the domains to the apIP */
/*  
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("DNSServer started.");
*/
  startWebServer(webPort);
}

/************************
 * Connect WLAN as Client
 */
boolean connectWifi() {
  printCurrentDateTime();
  Serial.print(" Connecting to ");
  Serial.print(ssid);
  Serial.println("...");
  WiFi.disconnect();
  WiFi.begin (ssid, password);
  int connectionResult = WiFi.waitForConnectResult();
  /* 
   * Function returns one of the following connection statuses: 
   * *  WL_CONNECTED  after successful connection is established 
   * *  WL_NO_SSID_AVAIL in case configured SSID cannot be reached 
   * *  WL_CONNECT_FAILED  if password is incorrect 
   * *  WL_IDLE_STATUS  when Wi-Fi is in process of changing between statuses 
   * *  WL_DISCONNECTED  if module is not configured in station mode
   */
  
  if (!stDhcpFlag) { /* Station static IP settings */
    WiFi.config(stIP, stGateway, stSubnet, stDns1, stDns2);
  }
  Serial.print ( "WiFi client status: " );
  Serial.println ( status_t[connectionResult] );
  return (connectionResult==WL_CONNECTED);
}


int photosensorOffValue = -10000; // OFF
int photosensorOnValue  = +10000;  // ON
int photosensorCounter = photosensorOffValue; // initial counter value = OFF value
/*********************************
 * Обработка состояния Фотосенсора
 */
void handlePhotosensorState() {
  if (socket_mode==SOCKET_MODE_SWITCH && switcher_photosensor_enabled) {
    int val = digitalRead(photosensor_pin);
    if (val==0) val=-1; // светло - счетчик уменьшается, темно - счетчик увеличивается
    photosensorCounter += val; // increment/decrement
    if (photosensorCounter<photosensorOffValue) photosensorCounter=photosensorOffValue; // min counter value
    if (photosensorCounter>photosensorOnValue) photosensorCounter=photosensorOnValue;   // max counter value
    if (socket_state==1 && photosensorCounter==photosensorOffValue) { // Нагрузка ВКЛЮЧЕНА, счетчик = min
      setSocketState(0, "СТАЛО СВЕТЛО."); // Отключаю нагрузку
    }
    if (socket_state==0 && photosensorCounter==photosensorOnValue) { // Нагрузка ОТКЛЮЧЕНА, счетчик = max
      setSocketState(1, "СТАЛО ТЕМНО."); // Включаю нагрузку
    }
    delay(1); /* ~20 сек. задержки перед переключением */
  }
}

/**********************************************************
 * Рабочий цикл
 */
void loop() {

  ArduinoOTA.handle();

  //Serial.println("loop"); 
  delay(1);

  /* Handle photosensor */
  if (photosensor_pin>=0) {
    handlePhotosensorState();
    //Serial.println("===> CP#1 handlePhotosensorState"); delay(1);
  }
  
  if (connect) {
    //Serial.println ( "Connect requested" );
    //Serial.println("===> CP#2 Connect requested"); delay(1);
    connect = false;
    connectWifi();
    lastConnectTry = millis();
  }
  {
    int s = WiFi.status();

    if (s == WL_IDLE_STATUS && millis() > (lastConnectTry + 60000) ) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = (setupButtonState==HIGH) && (strlen(ssid) > 0); // Request WLAN connect if there is a SSID
    }
    
    if (status != s) { // WLAN status changed
      printCurrentDateTime();
      Serial.print ( " WiFi Status: " );
      Serial.println ( status_t[s] );
      status = s; // Store current WLAN status
      
      if (s == WL_CONNECTED) {
        /* Just connected to WLAN */
        printCurrentDateTime();
        Serial.print ( " Connected to " );
        Serial.print ( ssid );
        Serial.print ( " [IP address: " );
        Serial.print ( WiFi.localIP() );
        Serial.println ( "]" );

        // Setup MDNS responder
        /*
        if (!MDNS.begin(myHostname)) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started.");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
        */

        /* start NTP client */
        /*
        timeClient.begin();
        Serial.println("NtpClient started.");
        */
      }
      else if (s == WL_NO_SSID_AVAIL) /* Configured SSID cannot be reached  */ 
      {
        WiFi.disconnect();
      }
    } // If status changed

    if (ntpUpdateFlag) {
      if (WiFi.status() == WL_CONNECTED) {
        if (syncTime()) { // если синхронизация удачна
          ntpSyncIntervalCounter = 0; // сбрасываем счетчик интервалов, иначе попытка синхронизации будет повторена спустя 1 интервал
        }
      }
      ntpUpdateFlag = false;
      delay(1);
    }
    
    if (pingFlag) {
      if (WiFi.status() == WL_CONNECTED) {
        testPING();
        pingFlag = false;
        delay(1);
       }
    }

    if (send2NarodmonFlag) {
      if (WiFi.status() == WL_CONNECTED) {
        send2NarodmonFlag = false; // Сбрасываем флаг до отправки
        narodmonSendIntervalCounter = 0; // Сбрасываем счетчик
        sendToNarodmon();
        delay(1);
      }
    }

    if (thermostatFlag) {
      checkThermostatState();
      thermostatFlag = false;
      delay(1);
    }
  }

  /* DNS */
  //Serial.println("===> CP#6"); delay(1);
  //dnsServer.processNextRequest();
  /* HTTP */
  //Serial.println("===> CP#7"); delay(1);
  server.handleClient();
  //Serial.println("===> CP#8 loop complete"); delay(1);
}

/* Restart device */
void rebootDevice() {
  Serial.println("Restarting...");
  delay(500);
  WiFi.forceSleepBegin(); 
  wdt_reset(); 
  ESP.restart(); 
  while(1) wdt_reset(); 
}


/*
 * Отправка чанка (Transfer-Encoding: chunked)
 */
void sendChunk(String chunk) {
  server.sendContent( String(chunk.length(),HEX)+"\r\n");
  server.sendContent( chunk+"\r\n" );
}
/**
 * Завершаем отправку chunked респонса
 */
void flushChunkedResponse() {
  /* Send empty chunk */
  sendChunk( "" );
  //server.client().flush();
  //delay(1); // Даем время, чтобы принять данные
  delay(5); // Даем время, чтобы принять данные
  server.client().stop(); // Stop is needed because we sent no content length
}

boolean authenticationRequired() {
  boolean result = true;
  
  if (www_username[0]==0 ||           // пустой username
      www_password[0]==0 ||           // или пустой пароль
      WiFi.softAPIP()!=INADDR_NONE || // или WiFi в режиме AP
      server.authenticate(www_username, www_password)) // или уже аутентифицированы
    result = false;
    
  return result;
}


/************
* Handle root
*/
void handleRoot() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleRoot started");
  #endif

/*  
  Serial.print("headers count=");
  Serial.println(server.headers());

  for (int i=0;i<server.headers();i++) {
    Serial.print(server.headerName(i));
    Serial.print(": ");
    Serial.println(server.header(0));
  }
*/  
/*

if (server.hasHeader("User-Agent")){
  Serial.print("USER-AGENT=");
  Serial.println(server.header("User-Agent"));
}
else
  Serial.println("User-Agent UNKNOWN");
*/  
  

  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();
  
  sendChunk(FPSTR(START_HEAD));
  sendChunk(FPSTR(META_TAGS));
  sendChunk(FPSTR(STYLE));
  sendChunk(FPSTR(FINISH_HEAD));
  sendChunk( StringExt(FPSTR(PAGE_HEADER)).replace("%s1","Welcome") );
  
  sendChunk( String("Welcome to smart device hotspot page!<br><br>") + "Select your choice from bottom navigation menu, please." );

  /* Navigation menu */
  sendChunk( FPSTR(NAVIGATION_MENU) );
  
  sendChunk( FPSTR(FINISH_DOC) );
  
  // Finish chunked response
  flushChunkedResponse();

  #if defined(DEBUG_MODE)
    Serial.print("handleRoot complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/****************************
* Handle "Device Information"
*/
void handleInfo() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();
    
  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleInfo started");
  #endif
  
  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();
  
//  Serial.println("handleInfo CP#1");

  sendChunk(FPSTR(START_HEAD));
  sendChunk(FPSTR(META_TAGS));
  sendChunk(FPSTR(STYLE));
  sendChunk(FPSTR(FINISH_HEAD));
  sendChunk( StringExt(FPSTR(PAGE_HEADER)).replace("%s1","Device Information") );

//  Serial.println("handleInfo CP#2");

  if (server.client().localIP() == apIP) {
    sendChunk( String("You are connected through the soft AP: ") + softAP_ssid );
  } else {
    sendChunk( String("You are connected through the wifi network: ") + ssid );
  }
  /* Soft AP Information */
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Soft AP") );
  sendChunk( StringExt(FPSTR(TR0)).replace("%s1","SSID:").replace("%s2",String(softAP_ssid)) );
  sendChunk( StringExt(FPSTR(TR1)).replace("%s1","IP Address:").replace("%s2",toStringIp(WiFi.softAPIP())) );
  sendChunk( FPSTR(TABLE_FOOTER) );
//  sendChunk( String() +
//    "</table>\n");
    
  /* WLAN Information */
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","WLAN") );
  sendChunk( StringExt(FPSTR(TR0)).replace("%s1","SSID:").replace("%s2",String(ssid)) );
  sendChunk( StringExt(FPSTR(TR1)).replace("%s1","IP Address:").replace("%s2",toStringIp(WiFi.localIP())) );
  sendChunk( StringExt(FPSTR(TR0)).replace("%s1","Gateway:").replace("%s2",toStringIp(WiFi.gatewayIP() )) );
  sendChunk( StringExt(FPSTR(TR1)).replace("%s1","Subnet mask:").replace("%s2",toStringIp(WiFi.subnetMask())) );

  unsigned char mac[6];
  WiFi.macAddress(mac);
  sendChunk( StringExt(FPSTR(TR0)).replace("%s1","MAC:").replace("%s2",macToStr(mac)) );
  sendChunk( FPSTR(TABLE_FOOTER) );

//  sendChunk(
//    String() +  "</table>\n");
    
//  Serial.println("handleInfo CP#6");

  /* System Information */
  //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); // Время компиляции скетча

  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","System Information") );
  sendChunk( StringExt(FPSTR(TR0)).replace("%s1","Firmware:").replace("%s2",String("ver.&nbsp;")+firmware_version) );
//  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Uptime:")+
//    "<span id='uptime'>"+getUptimeAsString()+"</span></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1)).replace("%s1","Uptime:").replace("%s2","<span id='uptime'>"+getUptimeAsString()+"</span>") );
  sendChunk( StringExt(FPSTR(TR0)).replace("%s1","V<sub>cc</sub>:").replace("%s2",getVccAsString()) );
  sendChunk( FPSTR(TABLE_FOOTER) );

  /* Navigation menu */
  sendChunk( FPSTR(NAVIGATION_MENU) );
  
//  Serial.println("handleInfo CP#8");

  /* script to increment onpage current datetime */
  sendChunk(
    String()+
    "<script>\n"
    "var seconds="+String(millis()/1000)+";\n"
    "var uptime=document.getElementById('uptime');\n"
    "function tick(){seconds++;\n"
    "var ss = seconds%60; var sec = Math.floor(seconds/60);\n"
    "var mi = sec%60; sec = Math.floor(sec/60);\n"
    "var hh = sec%24; sec = Math.floor(sec/24);\n"
    "var dd = sec; uptime.innerText=dd+' days, '+lpad(hh)+':'+lpad(mi)+':'+lpad(ss);\n"
    //"return ''+dd+' days, '+lpad(hh)+':'+lpad(mm)+':'+lpad(ss);\n"
    "}\n"
    "function lpad(v) {return ''+(v<10?'0'+v:v);}"
    "window.setInterval('tick()',1000);\n"
    "</script>\n"
  );
  
//  Serial.println("handleInfo CP#9");

  sendChunk( FPSTR(FINISH_DOC) );
  
//  Serial.println("handleInfo CP#10");

  /* Finish chunked response */
  flushChunkedResponse();
  
  #if defined(DEBUG_MODE)
    Serial.print("handleInfo complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/***********************************
* Set socket state & return an image
*/
void handleSw() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleSw started");
  #endif
  
  if (server.arg("st")=="on") {
    /* Включить нагрузку */    
    setSocketState(1, "РУЧНОЕ ДЕЙСТВИЕ.");
  }
  else if (server.arg("st")=="off") {
    /* Отключить нагрузку */    
    setSocketState(0, "РУЧНОЕ ДЕЙСТВИЕ.");
  }
  else if (server.arg("st")=="reset" && socket_state!=1) {
    /* Сбросить нагрузку по питанию */    
    setSocketState(1, "MANUAL RESET."); // В этом режиме нагрузка подключена к НЗ контактам реле, так что для сброса реле нужно включить
    // Теперь надо подождать "RESET_PERIOD" секунд и вновь включить нагрузку
    resetticker.once(RESET_PERIOD, restoreManualResetState);
  }
  else {
    /* Получить текущее состояние нагрузки */
  }
  
  // Redirect to switch page
  server.sendHeader("Location", "/switch", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length

  #if defined(DEBUG_MODE)
    Serial.print("handleSw complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

void restoreManualResetState() {
  setSocketState(0, "RESTORE STATE AFTER MANUAL RESET."); // Восстановление питания нагрузки
}


/*************
* Smart switch
*/
void handleSwitch() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();
  
  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleSwitch started");
  #endif

  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();

  sendChunk( FPSTR(START_HEAD) );
  sendChunk( FPSTR(META_TAGS) );
  sendChunk( FPSTR(STYLE) );

  /* MSIE не поддерживает SSE, поэтому принудительно обновляем страницу
   * каждые 30 секунд,
   * а если мы находимся в состоянии RESETTING - то через (RESET_PERIOD+1) секунд 
   */
  int refreshPeriod = (socket_mode==SOCKET_MODE_RESETTER && socket_state==1?RESET_PERIOD+1:30);
  /* Switch script */
  sendChunk(
    String() +
    "<script language='jscript'>\n"
    "  var socket_state="+socket_state+";\n"
    "  var power_is_on_s='"+FPSTR(POWER_IS_ON_S)+"';\n"
    "  var power_is_off_s='"+FPSTR(POWER_IS_OFF_S)+"';\n"
    "  var resetting_s='"+FPSTR(RESETTING_S)+"';\n"
    "  var switch_on_s='"+FPSTR(SWITCH_ON_S)+"';\n"
    "  var switch_off_s='"+FPSTR(SWITCH_OFF_S)+"';\n"
    "  var ua=navigator.userAgent; if (ua.search(/MSIE/)!=-1 || ua.search(/rv:11.0/)!=-1) { window.setTimeout(function(){location.reload(true)},"+String(refreshPeriod)+"000) }\n"
    "function sw(img) {\n"
    " if (document.all('socket_mode')[1].checked) {\n" /* resetter mode */
    "   location.href=(socket_state==1?'/sw?st=idle':'/sw?st=reset');\n"
    " } else {\n"
    "   location.href=(socket_state==1?'/sw?st=off':'/sw?st=on');\n"
    " }\n"
    " document.getElementById(\"comment\").style.visibility=\"hidden\";\n"
    " document.getElementById(\"ticker\").style.visibility=\"hidden\";\n"
    "}\n"
    );
  sendChunk(
    String() +
    "function socketPinChanged(val){\n"
    "document.getElementById(\"socket_mode_row\").style.display=(val<0?'none':'');\n"
    "document.getElementById(\"socket_status_row\").style.display=(val<0?'none':'');\n"
    "document.getElementById(\"sw_opt_table\").style.display =((val<0 || !document.all('socket_mode')[0].checked)?'none':'');\n"
    "document.getElementById(\"ps_opt_table\").style.display =((val<0 || !document.all('socket_mode')[0].checked || !document.all('switcher_photosensor_enabled').checked)?'none':'');\n"
    "document.getElementById(\"rst_opt_table\").style.display=((val<0 || !document.all('socket_mode')[1].checked)?'none':'');\n"
    "document.getElementById(\"ts_opt_table\").style.display =((val<0 || !document.all('socket_mode')[2].checked)?'none':'');\n"
    "}\n"
    );
  sendChunk(
    String() +
    "function socketModeChanged(val){\n"
    "document.getElementById(\"sw_opt_table\").style.display=(val==0?'':'none');\n"
    "document.getElementById(\"ps_opt_table\").style.display=(val==0 && document.all('switcher_photosensor_enabled').checked?'':'none');\n"
    "document.getElementById(\"rst_opt_table\").style.display=(val==1?'':'none');\n"
    "document.getElementById(\"ts_opt_table\").style.display=(val==2?'':'none');\n"
    "}\n"
    );
  sendChunk(
    String() +
    "function setSwitcherPhotosensorEnabled(enbl){\n"
    "document.getElementById(\"ps_opt_table\").style.display=(enbl?'':'none');\n"
    "}\n"
    "function setSwitcherTimersEnabled(enbl){\n"
    "}\n"
    "function setResetterTimersEnabled(enbl){\n"
    "}\n"
    "</script>\n"
    );
  sendChunk( FPSTR(FINISH_HEAD) );
  sendChunk( StringExt(FPSTR(PAGE_HEADER)).replace("%s1","Smart switch") );

//  sendChunk(
//    String() +
//    "<form method='POST' action='switchsave'>\n");
  sendChunk( StringExt(FPSTR(FORM_HEADER)).replace("%s1","switchsave") );
    
  sendChunk(
    String() +
    "<input type='hidden' name='h' value='1'/>\n");
  /* Socket options and current status */
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Socket") );
    
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Pin:")+
    "<select name='socket_pin' class='clsSelect' onchange='socketPinChanged(this.value);'>\n");
/*  
    "<option value='-1' selected "+getSelectedAttribute(socket_pin, -1)+">Not installed\n"
    "<option value='16' "+getSelectedAttribute(socket_pin, 16)+">D0\n" // GPIO16 только OUTPUT //+
    "<option  value='5' "+getSelectedAttribute(socket_pin,  5)+">D1\n" // SCL
    "<option  value='4' "+getSelectedAttribute(socket_pin,  4)+">D2\n" // SDA
    "<option  value='0' "+getSelectedAttribute(socket_pin,  0)+">D3\n" // +
    "<option  value='2' "+getSelectedAttribute(socket_pin,  2)+">D4\n" // +
    "<option value='14' "+getSelectedAttribute(socket_pin, 14)+">D5\n"
    "<option value='12' "+getSelectedAttribute(socket_pin, 12)+">D6\n" // +
    "<option value='13' "+getSelectedAttribute(socket_pin, 13)+">D7\n" // SWQ
    //"<option value='15' "+getSelectedAttribute(socket_pin, 15)+">D8\n" // --
    //"<option  value='3' "+getSelectedAttribute(socket_pin,  3)+">D9 (RX)\n" // --
    "<option  value='1' "+getSelectedAttribute(socket_pin,  1)+">D10 (TX)\n" //  
*/
  /* 
  Доступны все пины, кроме
  - D8
  - D9 (RX)
  и пина, используемого RTC для прерывания (если модуль RTC подключен) 
  */
  //int excludeGPIO1[] = {15, 3, rtcPresent?interruptPin:-999}; 
  int excludeGPIO1[] = {15, 3}; 
  sendChunk(
    getSelectPinOptions(excludeGPIO1, array_count(excludeGPIO1), socket_pin)
  );

  sendChunk(
    String()+
    "</select>\n"
    "</td></tr>\n");

  /* Socket Mode */
  sendChunk(
    String()+
    "<tr class='clsTR1' id='socket_mode_row' "+(socket_pin<0?"style='display:none'":"")+"><th class='clsTH1' style='vertical-align:middle;'>Mode:</th>"
    "<td class='clsTD1'>\n"
    "<input type='radio' name='socket_mode' onclick='socketModeChanged(this.value)' value='0' "+getCheckedAttribute(socket_mode==0)+">Switcher<br/>\n"
    "<input type='radio' name='socket_mode' onclick='socketModeChanged(this.value)' value='1' "+getCheckedAttribute(socket_mode==1)+">Resetter<br/>\n"
    "<input type='radio' name='socket_mode' onclick='socketModeChanged(this.value)' value='2' "+getCheckedAttribute(socket_mode==2)+">Thermostat\n"
    "</td></tr>\n");
    
  /* Socket Status */
  sendChunk(
    String() +
    "<tr class='clsTR0' id='socket_status_row' "+(socket_pin<0?"style='display:none'":"")+"><th class='clsTH1' style='vertical-align:middle;'>Status:</th>"
    "<td class='clsTD1' style='white-space:nowrap;'>\n"
    );

  sendChunk(
    String() +
    "<div id='sstate' class='"+
    (socket_mode==SOCKET_MODE_RESETTER?(socket_state==1?"clsOFF":"clsON"):(socket_state==1?"clsON":"clsOFF"))+
    "'>"+
    (socket_mode==SOCKET_MODE_RESETTER?(socket_state==1?FPSTR(RESETTING_S):FPSTR(POWER_IS_ON_S)):(socket_state==1?FPSTR(POWER_IS_ON_S):FPSTR(POWER_IS_OFF_S)))+
    "</div>"
    " <a href='#'><span id='saction'"
    " onclick='sw(this);'/>"+
    (socket_mode==SOCKET_MODE_RESETTER?(socket_state==1?FPSTR(EMPTY_S):FPSTR(RESET_S)):(socket_state==1?FPSTR(SWITCH_OFF_S):FPSTR(SWITCH_ON_S)))+
    "</span></a>\n"
    );
    
  sendChunk(
    String() +
    "<div id='ticker'  style='font:bold 9pt helvetica;width:32px;text-align:center;display:inline-block;color:#666666;;visibility:hidden;z-index:100;'>0</div>"
    "<div id='comment' style='font:bold 9pt helvetica;display:inline-block;color:#666666;visibility:hidden;'>sec. to ON</div>"
    "</td></tr>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );

  /* Thermostat Options */
  sendChunk( String() +
    "<table id='ts_opt_table' class='clsT1' "+(socket_pin<0 || socket_mode!=2?"style='display:none'":"")+"><caption>Thermostat Options</caption>\n");
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Thermosensor:")+
    "<select name='thermostat_sensor' class='clsSelect'>\n"
    "<option value='"+getAddressAsString(THERMOSENSOR_NON_SELECTED)+"' "+getSelectedAttribute(getAddressAsString(thermostat_sensor),  getAddressAsString(THERMOSENSOR_NON_SELECTED))+">Not selected\n"
    );

  /* Формируем список доступных термодатчиков */

  /* OneWire DS18B20 */
  if (onewire_pin>=0) {
    /* OneWire Setup */
    OneWire oneWire(onewire_pin);
    DallasTemperature sensors(&oneWire);
    sensors.begin();
    
    int n=sensors.getDeviceCount();
    for (int i=1; i<=n; i++) {
      DeviceAddress devAddr;
      sensors.getAddress(devAddr, i-1);
      sendChunk(
        String() +
        "<option value='"+getAddressAsString(devAddr)+"' "+getSelectedAttribute(getAddressAsString(thermostat_sensor),  getAddressAsString(devAddr))+">T"+String(i)+"&nbsp;["+getAddressAsString(devAddr)+"]\n"
        );
    }
  }
  /* RTC */
  if (rtcPresent) {
    sendChunk( String() +
      "<option value='"+getAddressAsString(THERMOSENSOR_RTC)+"' "+getSelectedAttribute(getAddressAsString(thermostat_sensor),  getAddressAsString(THERMOSENSOR_RTC))+">RTC\n"
      );
  }
  /* BMP280 */
  if (BMP280_setup()) /* init success */
  { 
    sendChunk( String() +
      "<option value='"+getAddressAsString(THERMOSENSOR_BMP280)+"' "+getSelectedAttribute(getAddressAsString(thermostat_sensor),  getAddressAsString(THERMOSENSOR_BMP280))+">BMP280\n"
      );
  }
  /* DHT 22 */
  if (dht22_pin>0) {
    sendChunk( String() +
      "<option value='"+getAddressAsString(THERMOSENSOR_DHT22)+"' "+getSelectedAttribute(getAddressAsString(thermostat_sensor),  getAddressAsString(THERMOSENSOR_DHT22))+">DHT</sub>\n"
      );
  }
    
  sendChunk( String() +
    "</select>"
    "</td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Desired T&#0176;:") );
  sendChunk( String() +
    "<select name='thermostat_temperature' class='clsSelect'>\n"
    "<option  value='4' "+getSelectedAttribute(thermostat_temperature,  4)+">+4&#0176;&nbsp;C\n"
    "<option  value='5' "+getSelectedAttribute(thermostat_temperature,  5)+">+5&#0176;&nbsp;C\n"
    "<option  value='6' "+getSelectedAttribute(thermostat_temperature,  6)+">+6&#0176;&nbsp;C\n"
    "<option  value='7' "+getSelectedAttribute(thermostat_temperature,  7)+">+7&#0176;&nbsp;C\n"
    "<option  value='8' "+getSelectedAttribute(thermostat_temperature,  8)+">+8&#0176;&nbsp;C\n"
    "<option  value='9' "+getSelectedAttribute(thermostat_temperature,  9)+">+9&#0176;&nbsp;C\n"
    "<option  value='10' "+getSelectedAttribute(thermostat_temperature, 10)+">+10&#0176;&nbsp;C\n"
    "<option  value='11' "+getSelectedAttribute(thermostat_temperature, 11)+">+11&#0176;&nbsp;C\n"
    "<option  value='12' "+getSelectedAttribute(thermostat_temperature, 12)+">+12&#0176;&nbsp;C\n"
    "<option  value='13' "+getSelectedAttribute(thermostat_temperature, 13)+">+13&#0176;&nbsp;C\n"
    "<option  value='14' "+getSelectedAttribute(thermostat_temperature, 14)+">+14&#0176;&nbsp;C\n"
    "<option  value='15' "+getSelectedAttribute(thermostat_temperature, 15)+">+15&#0176;&nbsp;C\n"
    );

  sendChunk( String() +
    "<option  value='16' "+getSelectedAttribute(thermostat_temperature, 16)+">+16&#0176;&nbsp;C\n"
    "<option  value='17' "+getSelectedAttribute(thermostat_temperature, 17)+">+17&#0176;&nbsp;C\n"
    "<option  value='18' "+getSelectedAttribute(thermostat_temperature, 18)+">+18&#0176;&nbsp;C\n"
    "<option  value='19' "+getSelectedAttribute(thermostat_temperature, 19)+">+19&#0176;&nbsp;C\n"
    "<option  value='20' "+getSelectedAttribute(thermostat_temperature, 20)+">+20&#0176;&nbsp;C\n"
    "<option  value='21' "+getSelectedAttribute(thermostat_temperature, 21)+">+21&#0176;&nbsp;C\n"
    "<option  value='22' "+getSelectedAttribute(thermostat_temperature, 22)+">+22&#0176;&nbsp;C\n"
    "<option  value='23' "+getSelectedAttribute(thermostat_temperature, 23)+">+23&#0176;&nbsp;C\n"
    "<option  value='24' "+getSelectedAttribute(thermostat_temperature, 24)+">+24&#0176;&nbsp;C\n"
    "<option  value='25' "+getSelectedAttribute(thermostat_temperature, 25)+">+25&#0176;&nbsp;C\n"
    "<option  value='26' "+getSelectedAttribute(thermostat_temperature, 26)+">+26&#0176;&nbsp;C\n"
    "<option  value='27' "+getSelectedAttribute(thermostat_temperature, 27)+">+27&#0176;&nbsp;C\n"
    "<option  value='28' "+getSelectedAttribute(thermostat_temperature, 28)+">+28&#0176;&nbsp;C\n"
    "<option  value='29' "+getSelectedAttribute(thermostat_temperature, 29)+">+29&#0176;&nbsp;C\n"
    "<option  value='30' "+getSelectedAttribute(thermostat_temperature, 30)+">+30&#0176;&nbsp;C\n"
    "</select>\n"
    "</td></tr>\n");
    
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Deviation:") );
  sendChunk( String() +
    "<select name='thermostat_deviation' class='clsSelect'>\n"
    "<option  value='0.125' "+getSelectedAttribute(thermostat_deviation, 0.125)+">&#0177;0.125&#0176;&nbsp;C\n"
    "<option  value='0.25' "+getSelectedAttribute(thermostat_deviation, 0.25)+">&#0177;0.25&#0176;&nbsp;C\n"
    "<option  value='0.5' "+getSelectedAttribute(thermostat_deviation, 0.5)+">&#0177;0.5&#0176;&nbsp;C\n"
    "<option  value='1' "+getSelectedAttribute(thermostat_deviation, 1)+">&#0177;1.0&#0176;&nbsp;C\n"
    "</select>\n"
    "</td></tr>\n");
    
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Mode:") );
  sendChunk( String() +
    "<select name='thermostat_mode' class='clsSelect'>\n"
    "<option value='0' "+getSelectedAttribute(thermostat_mode, 0)+">Heating\n"
    "<option value='1' "+getSelectedAttribute(thermostat_mode, 1)+">Cooling\n"
    "</select>\n"
    "</td></tr>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );
    
    

  /*
   * Switcher Options
   */
  sendChunk( String() +
    "<table id='sw_opt_table' class='clsT1' "+(socket_pin<0 || socket_mode!=0?"style='display:none'":"")+"><caption>Switcher Options</caption>\n");
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Photosensor:") );
  sendChunk( String() +
    "<input type='checkbox' name='switcher_photosensor_enabled' onclick='setSwitcherPhotosensorEnabled(this.checked)' "+getCheckedAttribute(switcher_photosensor_enabled)+">Enabled<br/>\n"
    "</td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Timers:") );
  sendChunk( String() +
    "<input type='checkbox' name='switcher_timers_enabled' onclick='setSwitcherTimersEnabled(this.checked)' "+getCheckedAttribute(switcher_timers_enabled)+">Enabled\n"
    "</td></tr>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );
    

  /*
   * Photo Sensor options and status
   */
  sendChunk(
    String() +
    "<table id='ps_opt_table' class='clsT1' "+(socket_pin<0 || socket_mode!=0 || !switcher_photosensor_enabled?"style='display:none'":"")+"><caption>Photo Sensor</caption>\n"
    "<tr><th class='clsTH1'>Pin:</th>"
    "<td class='clsTD1'>\n"
    "<select name='photosensor_pin' class='clsSelect'>\n"
  );
/*    
    "<option value='-1' selected "+getSelectedAttribute(photosensor_pin, -1)+">Not installed\n"
    //"<option value='16' "+getSelectedAttribute(photosensor_pin, 16)+">D0\n" // GPIO16 только OUTPUT
    "<option  value='5' "+getSelectedAttribute(photosensor_pin,  5)+">D1\n" // SCL
    "<option  value='4' "+getSelectedAttribute(photosensor_pin,  4)+">D2\n" // SDA
    //"<option  value='0' "+getSelectedAttribute(photosensor_pin,  0)+">D3\n" // модуль стартует только в темноте
    //"<option  value='2' "+getSelectedAttribute(photosensor_pin,  2)+">D4\n" // модуль стартует только в темноте
    "<option value='14' "+getSelectedAttribute(photosensor_pin, 14)+">D5\n"
    "<option value='12' "+getSelectedAttribute(photosensor_pin, 12)+">D6\n" 
    "<option value='13' "+getSelectedAttribute(photosensor_pin, 13)+">D7\n" // SQW
    //"<option value='15' "+getSelectedAttribute(photosensor_pin, 15)+">D8\n" // модуль стартует только на свету
    "<option  value='3' "+getSelectedAttribute(photosensor_pin,  3)+">D9 (RX)\n" // модуль прошивается только в темноте
    //"<option  value='1' "+getSelectedAttribute(photosensor_pin,  1)+">D10 (TX)\n" // модуль стартует только в темноте
*/    
  /* 
  Доступны пины, кроме
  - D0 (он только OUTPUT)
  - D3 (модуль стартует только в темноте)
  - D4 (модуль стартует только в темноте)
  - D8 (модуль стартует только на свету)
  - D9-RX (модуль прошивается только в темноте)
  - D10-TX (модуль стартует только в темноте)
  пина, используемого для управления розеткой
  и пина, используемого RTC для прерывания (если модуль RTC подключен) 
  */
  //int excludeGPIO2[] = {16,0,2,15,3,1,socket_pin,rtcPresent?interruptPin:-999}; 
  int excludeGPIO2[] = {16,0,2,15,3,1,socket_pin}; 
  sendChunk(
    getSelectPinOptions(excludeGPIO2, array_count(excludeGPIO2), photosensor_pin)
  );

  int ps_val = digitalRead(photosensor_pin); /* Считываем показание фотосенсора */
  /* Photosensor Status */
  sendChunk(
    String()+
    "</select>\n"
    "</td></tr>\n"
    "<tr class='clsTR1'><th class='clsTH1' style='vertical-align:middle;'>Status:</th>"
    "<td class='clsTD1' style='vertical-align:middle;'>\n"
    "<div id='pstext' class='"+(ps_val==1?"clsNIGHT":"clsDAY")+"'>"+
    (ps_val==1?"NIGHT":"DAY")+
    "</div>"
    "</td></tr>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );
    

  /* Resetter Options */ 
  sendChunk( String() +
    "<table id='rst_opt_table' class='clsT1' "+(socket_pin<0 || socket_mode!=1?"style='display:none'":"")+"><caption>Resetter Options</caption>\n");
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Ping Checker:")+
    "<input type='checkbox' name='resetter_ping_checker_enabled' onclick='setSwitcherMode(this.value)' "+getCheckedAttribute(resetter_ping_checker_enabled)+">Enabled<br/>\n"
    "</td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Host to ping:")+
    "<input class='clsInputText' name='resetter_ping_IP' value='"+toStringIp(resetter_ping_IP)+"' placeholder='0.0.0.0' maxlength='15'/>\n"
    "</td></tr>\n" );
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Ping faults limit:") );
    
  sendChunk( String() +
      "<select class='clsSelect' style='width:40%' name='resetter_ping_fault_limit'>\n"
      "<option value='1'"+getSelectedAttribute(resetter_ping_fault_limit,1)+">1\n"
      "<option value='2'"+getSelectedAttribute(resetter_ping_fault_limit,2)+">2\n"
      "<option value='3'"+getSelectedAttribute(resetter_ping_fault_limit,3)+">3\n"
      "<option value='4'"+getSelectedAttribute(resetter_ping_fault_limit,4)+">4\n"
      "<option value='5'"+getSelectedAttribute(resetter_ping_fault_limit,5)+">5\n"
      "<option value='6'"+getSelectedAttribute(resetter_ping_fault_limit,6)+">6\n"
      "<option value='7'"+getSelectedAttribute(resetter_ping_fault_limit,7)+">7\n"
      "<option value='8'"+getSelectedAttribute(resetter_ping_fault_limit,8)+">8\n"
      "<option value='9'"+getSelectedAttribute(resetter_ping_fault_limit,9)+">9\n"
      "<option value='10'"+getSelectedAttribute(resetter_ping_fault_limit,10)+">10\n"
      "</select>&nbsp;Ping interval = "+nextInterruptInterval+" sec.\n"
    "</td></tr>\n"
    );
  sendChunk(
    String() +
    "<tr class='clsTR1'><th class='clsTH1' rowspan='2'>Timers:</th>"
    "<td class='clsTD1'>\n"
    "<input type='checkbox' name='resetter_timers_enabled' onclick='setResetterTimersEnabled(this.checked)' "+getCheckedAttribute(resetter_timers_enabled)+">Enabled<br/>\n"
    "</td></tr>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );


  sendChunk(
    String() +
    "<input class='clsBtn' type='submit' onclick='ap.style.visibility=\"visible\";' value='Apply' />\n"
    "</form>\n"
    );

  /* Navigation menu */
  sendChunk( FPSTR(NAVIGATION_MENU) );
  /* Applying panel */
  sendChunk( FPSTR(APPLYING_PANEL) );
  /* SSE script */
  sendChunk( FPSTR(SWITCH_SSE_SCRIPT) );

  sendChunk( FPSTR(FINISH_DOC) );

  // Finish chunked response
  flushChunkedResponse();
  
  #if defined(DEBUG_MODE)
    Serial.print("handleSwitch complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/******************************************
* Handle Saving Switch Options
*******************************************/
void handleSwitchSave() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    Serial.begin(115200);
    int started = millis();
    Serial.println("handleSwitchSave started");
  #endif

  if (server.arg("h")=="1") {
    socket_pin = server.arg("socket_pin").toInt();
    int old_socket_mode = socket_mode; // Запоминаем текущий режим
    socket_mode = server.arg("socket_mode").toInt();
    switcher_photosensor_enabled = server.arg("switcher_photosensor_enabled")=="on";
    switcher_timers_enabled = server.arg("switcher_timers_enabled")=="on";
    photosensor_pin = server.arg("photosensor_pin").toInt();
    resetter_ping_checker_enabled = server.arg("resetter_ping_checker_enabled")=="on";
    if (server.arg("resetter_ping_IP")=="") { resetter_ping_IP = INADDR_NONE; } else { resetter_ping_IP.fromString(server.arg("resetter_ping_IP")); }
    resetter_ping_fault_limit = server.arg("resetter_ping_fault_limit").toInt();
    resetter_timers_enabled = server.arg("resetter_timers_enabled")=="on";
   
    char ts[18] = "";
    server.arg("thermostat_sensor").toCharArray(ts,18);
    convert(ts,thermostat_sensor);
    
    thermostat_temperature =  server.arg("thermostat_temperature").toFloat();
    thermostat_deviation =  server.arg("thermostat_deviation").toFloat();
    thermostat_mode =  server.arg("thermostat_mode").toInt();
    
    /** STORE TO EEPROM ********/
    struct switch_v1_t C;
    C.socket_pin = socket_pin;
    C.socket_mode = socket_mode;
    C.switcher_photosensor_enabled = switcher_photosensor_enabled;
    C.switcher_timers_enabled = switcher_timers_enabled;
    C.photosensor_pin = photosensor_pin;
    C.resetter_ping_checker_enabled = resetter_ping_checker_enabled;
    C.resetter_ping_IP = resetter_ping_IP;
    C.resetter_ping_fault_limit = resetter_ping_fault_limit;
    C.resetter_timers_enabled = resetter_timers_enabled;
    memcpy(C.thermostat_sensor, thermostat_sensor, sizeof(thermostat_sensor));
    C.thermostat_temperature = thermostat_temperature;
    C.thermostat_deviation = thermostat_deviation;
    C.thermostat_mode = thermostat_mode;
    
    storeSwitchBlock(C);
    //loadSwitchBlock();
    /**********************************/
    if (old_socket_mode != socket_mode // режим изменился
      && socket_state==1) { // нагрузка включена
        setSocketState(0, "ПРОИЗОШЛО ИЗМЕНЕНИЕ РЕЖИМА."); // Отключаю нагрузку
      }

    Serial.println("switchsave complete");
  }
  else {
    Serial.println("switchsave aborted. no data.");
  }
  // Redirect to "switch" page
  server.sendHeader("Location", "switch", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length

  #if defined(DEBUG_MODE)
    Serial.print("handleSwitchSave complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}


/*************************
* Wifi config page handler 
*/
void handleWifi() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleWifi started");
  #endif
  
  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();

  sendChunk( FPSTR(START_HEAD) );
  sendChunk( FPSTR(META_TAGS) );
  sendChunk( FPSTR(STYLE) );
  sendChunk( FPSTR(SET_DISPLAY_SCRIPT) );
  sendChunk( FPSTR(FINISH_HEAD) );
  
  sendChunk( StringExt(FPSTR(PAGE_HEADER)).replace("%s1","Wifi config") );

  /* Available network list */
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Available network list") );
  sendChunk( String() +
   "<tr><th class='clsTH2' style='width:40%'>SSID</th><th class='clsTH2' style='width:30%'>Enc.Type</th><th class='clsTH2' style='width:30%'>Signal Strength</th></tr>\n");
  //Serial.println("scan start");
  int n = WiFi.scanNetworks();
  //Serial.println("scan done");
  if (n > 0) {
    String enctype_t[9] = {"","","TKIP","","WPA","WEP","","NONE","AUTO"};
    for (int i = 0; i < n; i++) {
      sendChunk(String()+
        "<tr class='clsTR"+(i%2)+"'><td class='clsTD1'>"+WiFi.SSID(i)+"</td><td class='clsTD2'>"+enctype_t[WiFi.encryptionType(i)]+"</td><td class='clsTD2'>"+WiFi.RSSI(i)+"</td></tr>\n");
    }
  } else {
    sendChunk(String() + "<tr><td colspan='3'>No WLAN found</td></tr>");
  }
  sendChunk( FPSTR(TABLE_FOOTER) );
    
//  sendChunk( StringExt(FPSTR(FORM_HEADER)).replace("%s1","wifisave") );
  sendChunk( String()+
    "<form method='POST' action='wifisave' ");
    
  sendChunk( String()+
    "onsubmit='if (document.getElementById(\"wp1\").value!=document.getElementById(\"wp2\").value) "
    "{alert(\"Введенные пароли не совпадают!\");return false;} else {ap.style.visibility=\"visible\";return true;}' "
    " >\n"
    "<input type='hidden' name='h' value='1'/>\n" );
    
  /* WLAN Network credentials */  
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Connect to network") );
    
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","SSID:")+
    "<input class='clsInputText' type='text' placeholder='network'  name='n' value='"+ssid+"' maxlength='31'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Password:")+
    "<input class='clsInputText' type='password' placeholder='password' name='p' value='"+password+"' maxlength='31'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR0)).replace("%s1","Status:").replace("%s2",status_t[WiFi.status()]) );
  sendChunk( FPSTR(TABLE_FOOTER) );
    
  /* IP Settings */  
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","IP Settings") );
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Use DHCP:")+
    "<input style='width:18;margin-left:0;' type='checkbox' name='d' "+getCheckedAttribute(stDhcpFlag)+" onclick='setDisplay(this.checked)'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","IP&nbsp;Address:")+
    "<input id='ip1' "+getDisabled(stDhcpFlag)+" class='clsInputText' type='text' placeholder='0.0.0.0' name='i' value='"+toStringIp(stIP)+"'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Subnet&nbsp;mask:")+
    "<input id='ip2' "+getDisabled(stDhcpFlag)+" class='clsInputText' type='text' placeholder='0.0.0.0' name='m' value='"+toStringIp(stSubnet)+"'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Gateway:")+
    "<input id='ip3' "+getDisabled(stDhcpFlag)+" class='clsInputText' type='text' placeholder='0.0.0.0' name='g' value='"+toStringIp(stGateway)+"'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","DNS&nbsp;Server1:")+
    "<input id='ip4' "+getDisabled(stDhcpFlag)+" class='clsInputText' type='text' placeholder='0.0.0.0' name='d1' value='"+toStringIp(stDns1)+"'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","DNS&nbsp;Server2:")+
    "<input id='ip5' "+getDisabled(stDhcpFlag)+" class='clsInputText' type='text' placeholder='0.0.0.0' name='d2' value='"+toStringIp(stDns2)+"'/></td></tr>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );
    
    
  /* Web Server Settings */  
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Web Server Settings") );
    
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","HTTP&nbsp;Port:")+
    "<input class='clsInputText' type='text' placeholder='port number' name='http_port' value='"+http_port+"'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Username:")+
    "<input class='clsInputText' type='text' placeholder='' maxlength='15' name='wn' value='"+www_username+"'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Password:")+
    "<input class='clsInputText' type='password' placeholder='' maxlength='15' id='wp1' name='wp' value='"+www_password+"'/></td></tr>\n");
  sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Repeat password:")+
    "<input class='clsInputText' type='password' placeholder='' maxlength='15' id='wp2' value='"+www_password+"'/></td></tr>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );

  sendChunk( String() +
    "<input class='clsBtn' type='submit' "
    "on_click='alert(\"button click\"); alert(document.getElementById(\"wp1\").value); ap.style.visibility=\"visible\";' "
    "value='Apply' />\n"
    "</form>\n"
    );

  /* Navigation menu */
  sendChunk( FPSTR(NAVIGATION_MENU) );
  /* Applying panel */
  sendChunk( FPSTR(APPLYING_PANEL) );

  sendChunk( FPSTR(FINISH_DOC) );
  
  // Finish chunked response
  flushChunkedResponse();

  #if defined(DEBUG_MODE)
    Serial.print("handleWifi complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/*****************************************************************
* Handle the WLAN save form and redirect to WLAN config page again 
*/
void handleWifiSave() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();
  
  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleWifiSave started");
  #endif
  
  String newLocation = "wifi";
  
  if (server.arg("h")=="1") {
    server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
    server.arg("p").toCharArray(password, sizeof(password) - 1);
    stDhcpFlag = server.arg("d")=="on";
    if (server.arg("i")=="") { stIP = INADDR_NONE; } else { stIP.fromString(server.arg("i")); }
    if (server.arg("g")=="") { stGateway = INADDR_NONE; } else { stGateway.fromString(server.arg("g")); }
    if (server.arg("m")=="") { stSubnet = INADDR_NONE; } else { stSubnet.fromString(server.arg("m")); }
    if (server.arg("d1")=="") { stDns1 = INADDR_NONE; } else { stDns1.fromString(server.arg("d1")); }
    if (server.arg("d2")=="") { stDns2 = INADDR_NONE; } else { stDns2.fromString(server.arg("d2")); }
    int old_http_port = http_port;
    http_port = (int)server.arg("http_port").toInt();
    server.arg("wn").toCharArray(www_username, sizeof(www_username) - 1);
    server.arg("wp").toCharArray(www_password, sizeof(www_password) - 1);
    
    /* STORE OPTIONS TO EEPROM */
    struct credentials_v3_t C;
    strncpy(C.ssid, ssid, sizeof(ssid)); // dest, src, size
    strncpy(C.password, password, sizeof(password));
    C.dhcp_flag = stDhcpFlag;
    C.ip = stIP;
    C.gateway = stGateway;
    C.subnet = stSubnet;
    C.dns1 = stDns1;
    C.dns2 = stDns2;
    C.http_port = http_port;
    strncpy(C.www_username, www_username, sizeof(www_username)); // dest, src, size
    strncpy(C.www_password, www_password, sizeof(www_password));
    storeCredentialsBlock(C);
    /**********************************/
    if (http_port != old_http_port) {
      // Перезапускаем Web-сервер на новом порту
      restartWebServer(http_port);
      // Формируем полный URL для редиректа
      newLocation = "http://"+
        (WiFi.localIP()==INADDR_NONE?toStringIp(WiFi.softAPIP()):toStringIp(WiFi.localIP()))+
        (http_port==80?"/":":"+
        String(http_port)+"/")+newLocation;
      Serial.print("newLocation="); Serial.println(newLocation);
      //WiFi.softAPIP()
      //WiFi.localIP()
    }
  }
  else {
    Serial.println("wifi save abort. no data.");
  }
  
  // Redirect to "wifi" page
  server.sendHeader("Location", newLocation, true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  
  if (server.arg("h")=="1") {
    /* Если локальный IP-адрес = 0.0.0.0 */
    if (WiFi.localIP()==INADDR_NONE) { // Текущий режим = WIFI_AP или (WIFI_AP_STA и нет коннекта к WiFi)
      if (strlen(ssid) > 0) { // ssid задан
        WiFi.mode(WIFI_AP_STA); // переходим в режим AP-STATION
        connectWifi(); // коннектимся к WiFi как клиент
      }
    }
    else { // есть клиентский коннект к Wifi, значит режим либо WIFI_STA, либо WIFI_AP_STA
        // TODO: переконнект нужен только если изменились параметры сети
        WiFi.disconnect();
        if (strlen(ssid) > 0) { // ssid задан
          connectWifi(); // коннектимся к WiFi как клиент
        }

    }
  } 
/*
  server.sendHeader("Location", newLocation, true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
*/  
  #if defined(DEBUG_MODE)
    Serial.print("handleWifiSave complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/** Handle the WLAN clear form and restart */
/*
void handleWifiClear() {
  Serial.println("wifi clear");
  server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  server.arg("p").toCharArray(password, sizeof(password) - 1);
  server.sendHeader("Location", "wifi", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length

  WiFi.disconnect();
  clearCredentials();

  restartDevice();
}
*/

/*************************
* Time config page handler 
*/
void handleTime() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleTime started");
  #endif

  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();

  sendChunk( FPSTR(START_HEAD) );
  sendChunk( FPSTR(META_TAGS) );
  sendChunk( FPSTR(STYLE) );
  sendChunk( FPSTR(FINISH_HEAD) );

  sendChunk(
    String()+
    "<center style='z-index:0'>\n");
    
  sendChunk( StringExt(FPSTR(PAGE_HEADER)).replace("%s1","Time settings") );
  
  RtcDateTime dt;
  if (rtcPresent) {
    /* Current time */
    dt = Rtc.GetDateTime();
    sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Time") );
      
    sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Current time:")+
      "<span id='ct'>"+getFormattedDateTime(dt)+"</span></td></tr>\n");
    sendChunk( FPSTR(TABLE_FOOTER) );
      
    /* NTP Settings */
//    sendChunk( String()+
//      "<form method='POST' action='timesave'>\n");
    sendChunk( StringExt(FPSTR(FORM_HEADER)).replace("%s1","timesave") );
      
    sendChunk( String()+
      "<input type='hidden' name='h' value='1'/>\n");
      
    sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","NTP settings") );
      
    sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","NTP Server:")+
      "<input class='clsInputText' type='text' maxlength='31' placeholder='server name' name='ns' value='"+ntpServer+"' maxlength='31'/></td></tr>\n");
    sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Sync Interval:")+
      "<select class='clsSelect' style='width:40%' name='si'>\n"
      "<option value='60'"+getSelectedAttribute(ntpSyncInterval,60)+">1\n"
      "<option value='360'"+getSelectedAttribute(ntpSyncInterval,360)+">6\n"
      "<option value='720'"+getSelectedAttribute(ntpSyncInterval,720)+">12\n"
      "<option value='1440'"+getSelectedAttribute(ntpSyncInterval,1440)+">24\n"
      "</select>&nbsp;Hours\n"
      "</td></tr>\n");
    sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Time Zone:")+
      "<input class='clsInputText' type='text' placeholder='+0' name='tz' value='"+ntpTimeZone+"'/></td></tr>\n");
    sendChunk( FPSTR(TABLE_FOOTER) );
    
    /* Apply button */    
    sendChunk(
      String()+
      "<input class='clsBtn' type='submit' onclick='ap.style.visibility=\"visible\"' value='Apply'/>\n"
      "</form>\n"
    );
  } // if rtcPresent
  else {
    sendChunk(
      String()+
      "RTC module not installed.<br>\n"
    );
  }
  /* Navigation menu */
  sendChunk( FPSTR(NAVIGATION_MENU) );
  
  if (rtcPresent) {
    /* Applying panel */
    sendChunk( FPSTR(APPLYING_PANEL) );
    /* script to increment onpage current datetime */
    sendChunk(
      String()+
      "<script>\n"
      "function tick(){dt.setSeconds(dt.getSeconds()+1);ct.innerText=dt.toLocaleString();}\n"
      "var dt = new Date("+dt.Year()+","+(dt.Month()-1)+","+dt.Day()+","+dt.Hour()+","+dt.Minute()+","+dt.Second()+");\n"
      "ct.innerText=dt.toLocaleString();\n"
      "window.setInterval('tick()',1000);\n"
      "</script>\n"
    );
  }

  sendChunk(
    String()+
    "</center>");
    
  sendChunk( FPSTR(FINISH_DOC) );

  // Finish chunked response
  flushChunkedResponse();

  #if defined(DEBUG_MODE)
    Serial.print("handleTime complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/**********************************
 * Get string for checked attribute
 */
String getCheckedAttribute(boolean value) {
  String result="";
  if (value)
    result = " checked";
  return result;
}

/***********************************
 * Get string for selected attribute
 */
String getSelectedAttribute(float value, float attrValue) {
  String result="";
  if (value == attrValue)
    result = " selected";
  return result;
}
String getSelectedAttribute(String value, String attrValue) {
  String result="";
  if (value == attrValue)
    result = " selected";
  return result;
}

/***********************************
 * Get string for disabled attribute
 */
String getDisabled(boolean value) {
  String result="";
  if (value)
    result=" disabled";
  return result;
}

/*******************************************************************************
* Handle NTP & Time parmeters save form and redirect to Time settings page again 
*/
void handleTimeSave() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleTimeSave started");
  #endif

  if (server.arg("h")=="1") {

    server.arg("ns").toCharArray(ntpServer, sizeof(ntpServer) - 1);
    ntpSyncInterval = server.arg("si").toInt();
    int oldTimeZone = ntpTimeZone;
    ntpTimeZone = server.arg("tz").toInt();

    struct ntp_v1_t C;
    strncpy(C.server, ntpServer, sizeof(ntpServer)-1); // dest, src, size
    C.time_zone = ntpTimeZone;
    C.sync_interval = ntpSyncInterval;
    storeNtpBlock(C);
    
    
    //storeNtpBlock();
    //loadNtpOptionsBlock();
    //Serial.println("time saved.");

    if (ntpTimeZone != oldTimeZone) { /* Resync if time zone changed */
      Serial.println("TimeZone changed. Need to resync.");
      if (WiFi.status() == WL_CONNECTED) {
        if (syncTime()) { // если синхронизация удачна
          ntpSyncIntervalCounter = 0; // сбрасываем счетчик интервалов, чтобы синхронизация была повторена спустя ntpSyncInterval
        }
        else { // синхронизация не удалась
          ntpSyncIntervalCounter = ntpSyncInterval+1; // чтобы повторить синхронизацию в ближайший интервал
        }
      }
    }    
    else
      Serial.println("TimeZone not changed. Skip resync.");
    //  syncTime();
  }
  else {
    Serial.println("time save abort. no data.");
  }
  // Redirect to "time" page
  server.sendHeader("Location", "time", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length

  #if defined(DEBUG_MODE)
    Serial.print("handleTimeSave complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/*************************************
 * Handle "Rebooting in progress" page
 */
void handleReboot() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();
  
  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();

  sendChunk( FPSTR(START_HEAD) );
  sendChunk( FPSTR(META_TAGS) );
  sendChunk( FPSTR(STYLE) );
  sendChunk( FPSTR(TICK_SCRIPT) );
  sendChunk( FPSTR(FINISH_HEAD) );

  sendChunk(
    String()+
    "<h1 id='h1' class='clsH1'>Rebooting...</h1>\n"
    "<div style='width:100%;border:1px solid #03a9f4;text-align:center;z-index:0;padding:4px 0px 4px 0px;'>\n"
    "<span id='pt' style='vertical-align:bottom;background:transparent;color:#000000;'>0%</span>\n"
    "</div>"
    "<div id='pb' style='width:0%;height:24px;background:#03a9f4;color:#03a9f4;z-index:-1;position:relative;top:-25px;left:1px;'>&nbsp;</div>"
    "<script>\n"
    "window.setInterval(\"tick()\", 120);"
    "</script>\n"
    );
    
  // Finish chunked response
  flushChunkedResponse();
  
  delay(500);
  rebootDevice();
}

/**********************
 * Handle 404 Not found
 */
void handleNotFound() {

  /* Отсекаем запросы от ОС, бесполезно загружающие web-сервер */
/*  
  if (server.uri().indexOf("/")!=server.uri().lastIndexOf("/") // с вложенностью больше единицы
      || server.uri().indexOf("/api")>=0
      || server.uri().indexOf(".cgi")>=0
      || server.uri().indexOf("/get")>=0
      || server.uri().indexOf("/rtistatus")>=0
      || server.uri().indexOf("/__")>=0
      || server.uri().indexOf("/generate_204")>=0
      || server.uri().indexOf("/appwall")>=0
      || server.uri().indexOf("/images")>=0
      || server.uri().indexOf("/chat")>=0
      || server.uri().indexOf("/app.gif")>=0
      || server.uri().indexOf("/ncsi")>=0
      
  ) 
  {
    if (debug) {
      Serial.print(server.uri()); Serial.println(" REQUEST IGNORED"); 
    }
    // Send header
    server.sendContent( 
      "HTTP/1.1 404 Not found\r\n"
      "Content-Type: text/html;\r\n"
      "Content-Length: 13;\r\n"
      "Cache-Control: max-age=3600, must-revalidate\r\n"
      "\r\n"
    );
    //server.handleClient();
    // Send content
    server.sendContent("404 Not found");
    server.client().flush();
    server.client().stop();
    return;
  }
*/
  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.print("handleNotFound started "); Serial.println(server.uri());
  #endif
  
  if (handleSPIFFSfile()) { // If static file in SPIFFS was sent
    Serial.print(((server.method()==HTTP_GET)?"GET":"POST"));
    Serial.print(" ");
    Serial.println(server.uri());
  }
  else {

    server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
    //server.handleClient();
    
    sendChunk( FPSTR(START_HEAD) );
    sendChunk( FPSTR(META_TAGS) );
    sendChunk( FPSTR(STYLE) );
    sendChunk( FPSTR(FINISH_HEAD) );
  
    sendChunk(
      String()+
      "<h1 id='h1' class='clsH1'>404 Not found</h1>\n"
      "<div style='width:100%;text-align:left;padding:4px 0px 4px 0px;'>\n"
      "URI: "+server.uri()+"<br>\n"
      "Method: "+((server.method()==HTTP_GET)?"GET":"POST")+"<br>\n"
      "Arguments: "+server.args()+"<br>\n"
      );
      
      for ( uint8_t i = 0; i < server.args(); i++ ) {
        sendChunk(
          String()+
            server.argName ( i ) + ": " + server.arg ( i ) + "<br>\n");
      }
      
    sendChunk(
      String()+
      "</div>"
      );
    /* Navigation menu */
    sendChunk( FPSTR(NAVIGATION_MENU) );
  
    sendChunk( FPSTR(FINISH_DOC) );
    
    // Finish chunked response
    flushChunkedResponse();
  }

  #if defined(DEBUG_MODE)
    Serial.print("handleNotFound complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/*******************************
 * Stream standalone SPIFFS file 
 */
boolean handleSPIFFSfile() {
  boolean result = false;
  if (server.method() == HTTP_GET) {
    String path = server.uri();
    //Serial.print("handleSPIFFSfile: ");
    //Serial.println(path);
    SPIFFS.begin();
    if (SPIFFS.exists(path)) {
      File f = SPIFFS.open(path,"r");
      if (f) {
        String fileExt = getFileExtension(path);
        
        String mimeType = "text/plain"; // default
        if (fileExt == "jpg")
          mimeType = "image/jpeg";
        else if (fileExt == "png")
          mimeType = "image/png";
        else if (fileExt == "gif")
          mimeType = "image/gif";
        else if (fileExt == "ico")
          mimeType = "image/x-icon";
        else if (fileExt == "css")
          mimeType = "text/css";
        else if (fileExt == "htm")
          mimeType = "text/html";
        else
          mimeType = "text/plain";
/*          
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "-1");
*/        
        server.sendHeader("Cache-Control", "max-age=3600, must-revalidate");
        server.streamFile(f,mimeType);
/*
        char buf[1024];
        int siz = f.size();
        while(siz > 0) {
          size_t len = std::min((int)(sizeof(buf) - 1), siz);
          f.read((uint8_t *)buf, len);
          //server.client().write((const char*)buf, len);
          server.sendContent((const char*)buf);
          siz -= len; 
        }
*/        
        result = true;
      }
      f.close();
    }
  }
  return result;
}

/***********************************
 * Stream inpage SPIFFS file content
 */
/* НЕ УДАЛЯТЬ !!!
void sendFileContent(String path) {
  SPIFFS.begin();
  if (SPIFFS.exists(path)) {
    File f = SPIFFS.open(path,"r");
    if (f) {
      char buf[1024];
      int siz = f.size();
      while(siz > 0) {
        size_t len = std::min((int)(sizeof(buf) - 1), siz);
        f.read((uint8_t *)buf, len);
        //server.client().write((const char*)buf, len);
        server.sendContent((const char*)buf);
        siz -= len; 
      }
    }
  }
}
*/
/******************************
 * Send meta tags and CSS-style
 */
/* 
void sendStyle() {
  server.sendContent( FPSTR(META_TAGS) );
  //sendFileContent("/style.css"); // Stream SPIFFS file 
  server.sendContent( FPSTR(STYLE) );
}
*/


/********************
* Handle Sensors
*/
void handleSensors() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleSensors started");
  #endif
  
  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();
 
  sendChunk( FPSTR(START_HEAD) );
  sendChunk( FPSTR(META_TAGS) );
  sendChunk( FPSTR(STYLE) );
  sendChunk( FPSTR(ONEWIRE_PIN_CHANGED_SCRIPT) );
  sendChunk( FPSTR(FINISH_HEAD) );

  sendChunk( StringExt(FPSTR(PAGE_HEADER)).replace("%s1","Sensors") );

//  sendChunk(
//    String() +
//    "<form method='POST' action='sensorssave'>\n")
  sendChunk( StringExt(FPSTR(FORM_HEADER)).replace("%s1","sensorssave") );
    
  sendChunk(
    String() +
    "<input type='hidden' name='h' value='1'/>\n");
    
  if (rtcPresent) {
    /* DS3231 temperature sensor */
    RtcTemperature t1 = Rtc.GetTemperature();
    String sgn = "+"; if (t1.AsFloat() < 0) sgn="";

    sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","DS3231 RTC") );
      
    sendChunk(
      String()+
      "<tr><th class='clsTH1'>T&#0176;<sub>rtc</sub>:</th><td class='clsTD1' id='rtc_temp'>"+sgn+t1.AsFloat()+"&#0176;&nbsp;C</td></tr>\n");
    sendChunk( FPSTR(TABLE_FOOTER) );
  }
  else {
    sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","DS3231 RTC") );
    sendChunk( String()+
      "<tr><td class='clsTD2' colspan='2'>Module not installed.</td></tr>\n");
    sendChunk( FPSTR(TABLE_FOOTER) );
  }

  /* OneWire DS18B20 temperature sensors */
  sendChunk( String()+
    "<table class='clsT1' id='owt_table'><caption>DS18B20 OneWire</caption>");
    /* Setup OneWire PIN */
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Pin:")+
    "<select name='onewire_pin' class='clsSelect' onchange='onewirePinChanged(this.value);'>\n" );

  /* 
  Доступны все пины, кроме
  - D0 (он только OUTPUT), 
  - D8 (c подключенным к нему датчиком не загружается прошивка)
  и пина, используемого RTC для прерывания (если модуль RTC подключен)
  */
  //int excludeGPIO[] = {16,15,rtcPresent?interruptPin:-999}; 
  int excludeGPIO[] = {16,15}; 
  sendChunk(
    getSelectPinOptions(excludeGPIO, array_count(excludeGPIO), onewire_pin)
  );

  sendChunk(
    String()+
    "</select>\n"
    "</td></tr>\n"
    
    /* Setup T Sensors Precision */
    "<tr class='clsTR1' id='owp_r1' "+(onewire_pin<0?"style='display:none'":"")+"><th class='clsTH1'>Precision:</th>"
    "<td class='clsTD1'>\n"
    "<select name='onewire_precision' class='clsSelect' onchange='onewirePrecisionChanged(this.value);'>\n"
    "<option  value='9' "+getSelectedAttribute(onewire_precision,  9)+"> 9 bit (0.5&#0176;&nbsp;C)\n"
    "<option value='10' "+getSelectedAttribute(onewire_precision, 10)+">10 bit (0.25&#0176;&nbsp;C)\n"
    "<option value='11' "+getSelectedAttribute(onewire_precision, 11)+">11 bit (0.125&#0176;&nbsp;C)\n"
    "<option value='12' "+getSelectedAttribute(onewire_precision, 12)+">12 bit (0.06&#0176;&nbsp;C)\n"
    "</select>\n"
    "</td></tr>\n"
    );

  if (onewire_pin>=0) {
    /* OneWire Setup */
    OneWire oneWire(onewire_pin);
    DallasTemperature sensors(&oneWire);
    sensors.begin();
    sensors.setResolution(onewire_precision); // Set resolution globally for all sensors

    sensors.requestTemperatures();
    
    int n=sensors.getDeviceCount();
    for (int i=1; i<=n; i++) {
      DeviceAddress devAddr;
      sensors.getAddress(devAddr, i-1);

      float tempC = sensors.getTempC(devAddr);
      String sgn = "+"; if (tempC < 0) sgn="";

      sendChunk(
        String()+
        "<tr class='"+(i%2?"clsTR0":"clsTR1")+"' id='ow_t"+i+"'><th class='clsTH1'>T&#0176;<sub>"+(n>1?String(i):"")+"</sub> <span style='font-weight:normal'>["+getAddressAsString(devAddr)+"]</span>:</th><td class='clsTD1' id='ow_tv"+i+"'>"+sgn+tempC+"&#0176;&nbsp;C</td></tr>\n");
    }
    if (n==0) {
      sendChunk(
        String()+
        "<tr class='clsTR0'><td colspan='2' style='padding-top:6px;padding-bottom:6px;'>No temperature sensors found.</td></tr>\n");
    }
  }
  sendChunk( FPSTR(TABLE_FOOTER) );

  /* DHT22 */
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","DHT22") );
    
  /* Setup DHT22 PIN */
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Pin:")+
    "<select name='dht22_pin' class='clsSelect' onchange='dht22PinChanged(this.value);'>\n");

  //TODO - ПРОВЕРИТЬ будет ли заливаться прошивка при подключенном к D8 датчике !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  /* 
  Доступны все пины, кроме
  - D0 (он только OUTPUT)
  и пина, используемого RTC для прерывания (если модуль RTC подключен) 
  */
  //int excludeGPIO2[] = {16, rtcPresent?interruptPin:-999}; 
  int excludeGPIO2[] = {16}; 
  sendChunk(
    getSelectPinOptions(excludeGPIO2, array_count(excludeGPIO2), dht22_pin)
  );

  sendChunk(
    String()+
    "</select>\n"
    "</td></tr>\n"
  );

  if (dht22_pin>0) {
    SimpleDHT22 dht22;
    float dht22temp = 0;
    float dht22humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(dht22_pin, &dht22temp, &dht22humidity, NULL)) == SimpleDHTErrSuccess) {
      String sgn = "+"; if (dht22temp < 0) sgn="";
      sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","T&#0176;<sub>dht</sub>:")+
        "<span id='dht22temp'>"+sgn+dht22temp+"&#0176;&nbsp;C</span></td></tr>\n");
      sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Humidity:")+
        "<span id='dht22humidity'>&nbsp;"+dht22humidity+"&nbsp;%</span></td></tr>\n"
        );
    }
    else {
      Serial.print("Read DHT22 failed, err="); Serial.println(err);//delay(2000);
      //return;
      sendChunk(
        String()+
        "<tr class='clsTR0'><td colspan='2' style='padding-top:6px;padding-bottom:6px;'>No DHT22 sensor found.</td></tr>\n");
    }
    
  } // if

  sendChunk( FPSTR(TABLE_FOOTER) );

  /****** BMP280 ******/
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","BMP280") );
    
  if (BMP280_setup()) /* init */
  { /* Read BMP280 Sensor */
    double T,P,A;
    if (BMP280_read(T,P,A)) {
      String sgn = "+"; if (T < 0) sgn="";
      sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","T&#0176;<sub>bmp</sub>:")+
        "<span id='bmp280temp'>"+sgn+String(T,2)+"&#0176;&nbsp;C</span></td></tr>\n");
      sendChunk( StringExt(FPSTR(TR1_TH_OPEN_TD)).replace("%s1","Atmosphere pressure:")+
        "<span id='bmp280pres'>&nbsp;"+String(P,1)+"&nbsp;mmHg</span></td></tr>\n");
      sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Sea Level:")+
        "<span id='bmp280alt'>&nbsp;"+String((int)A)+"&nbsp;m</span></td></tr>\n" );
    }
    else
      sendChunk(
        String()+
        "<tr><td class='clsTD2' colspan='2'>Read failed.</td></tr>\n");
  }
  else
    sendChunk(
      String()+
      "<tr><td class='clsTD2' colspan='2'>Module not installed.</td></tr>\n");
      
  sendChunk( FPSTR(TABLE_FOOTER) );

  /****** REMOTE SENSORS ******/
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Remote sensors") );
    
  sendChunk( FPSTR(TABLE_FOOTER) );

  /* NARODMON */
  sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Online services") );
    
  sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Narodmon.Ru:")+
    "<input type='checkbox' name='send2narodmon_enabled' "+getCheckedAttribute(send2narodmon_enabled)+">Enabled<br/>\n");
  sendChunk( FPSTR(TABLE_FOOTER) );
    
  sendChunk(
    String()+
    "<input class='clsBtn' type='submit' onclick='ap.style.visibility=\"visible\"' value='Apply' />\n"
    "</form>\n");

  /* Navigation menu */
  sendChunk( FPSTR(NAVIGATION_MENU) );
  /* Applying panel */
  sendChunk( FPSTR(APPLYING_PANEL) );
  /* SSE script */
  sendChunk( FPSTR(SENSORS_SSE_SCRIPT) );

  sendChunk( FPSTR(FINISH_DOC) );
  
  // Finish chunked response
  flushChunkedResponse();

  #if defined(DEBUG_MODE)
    Serial.print("handleSensors complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/******************************************
* Handle Saving Wheather Sensors Options
*******************************************/
void handleSensorsSave() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    Serial.begin(115200);
    int started = millis();
    Serial.println("handleSensorsSave started");
  #endif

  if (server.arg("h")=="1") {
    onewire_pin = server.arg("onewire_pin").toInt();
    onewire_precision = server.arg("onewire_precision").toInt();
    dht22_pin = server.arg("dht22_pin").toInt();
    send2narodmon_enabled = server.arg("send2narodmon_enabled")=="on";

    /** STORE TO EEPROM ********/
    struct sensors_v1_t C;
    C.onewire_pin = onewire_pin;
    C.onewire_precision = onewire_precision;
    C.dht22_pin = dht22_pin;
    C.send2narodmon_enabled = send2narodmon_enabled;
    
    storeSensorsBlock(C);
    //loadSensorsBlock();
    /**********************************/
    //Serial.println("tempsave complete");
  }
  else {
    Serial.println("SensorsSave aborted. no data.");
  }
  // Redirect to "sensors" page
  server.sendHeader("Location", "sensors", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length

  #if defined(DEBUG_MODE)
    Serial.print("handleSensorsSave complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/**************************************
* Handle Switch Page Server-Send Events
*/
void handleSSE() {

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleSSE started");
  #endif
  
  String result;
//  String Header;
//  Header = "HTTP/1.1 200 OK\r\n";
//  Header += "Content-Type: text/event-stream;\r\n";
//  Header += "Cache-Control: no-cache\r\n";
//  Header += "\r\n";  
//  server.sendContent(Header);
  server.sendContent(FPSTR(EVENT_STREAM_HEADER));
  
  //server.handleClient();
  //int j = 100;
  //for (int i=1; i<=j; i++) {

    //yield();
    //delay(1000);

    /* Вычисляем время до переключения (в секундах) */
    int timeToSwitch = 0;
    
    if (socket_mode==SOCKET_MODE_SWITCH && switcher_photosensor_enabled) {
      if (socket_state==1 && photosensorCounter<photosensorOnValue) { // Нагрузка включена и счетчик в промежуточном положении
        timeToSwitch = (photosensorCounter-photosensorOffValue)/1000;
      } else if (socket_state==0 && photosensorCounter>photosensorOffValue) { // Нагрузка отключена и счетчик в промежуточном положении
        timeToSwitch = (photosensorOnValue-photosensorCounter)/1000;
      }
      //else 
      //  timeToSwitch = 0;
    }
    else
      photosensorCounter = (socket_state==1?photosensorOnValue:photosensorOffValue); // Сбрасываю счетчик фотосенсора

    if (timeToSwitch>0 || // Работает цикл задержки переключения
      socket_mode==SOCKET_MODE_RESETTER && socket_state==1) // Происходит RESET нагрузки
      result  = "retry: 100\n"; // реальный интервал обновления ~ 2-3 сек.
    else // Переключения не ожидается 
      result  = "retry: 3000\n"; // реальный интервал обновления ~ 5-6 сек.

    /* Состояние фотосенсора */
    result += "event: photosensorState\n";
    result += "data: ";
    result += digitalRead(photosensor_pin);
    result += "\n\n";
/*    
    result += "event: photosensorCounterChange\n";
    result += "data: ";
    result += photosensorCounter;
    result += "\n\n";
*/  
  
    /* Состояние розетки для подключения нагрузки */
    //if (socket_mode != SOCKET_MODE_RESETTER) { // Только для режимов != RESETTER
      result += "event: socketState\n";
      result += "data: ";
      result += socket_state;
      result += "\n\n";
    //}
/*    
    if (rtcPresent) { // Установлен модуль RTC
      result += "event: datetimeChange\n";
      result += "data: ";
      result += getFormattedDateTime(Rtc.GetDateTime());
      result += "\n\n";
    }
*/      
    /* Время, оставшееся до переключения (data.tts) и состояние в которое перейдет розетка (data.to) */
    if (socket_mode != SOCKET_MODE_RESETTER) { // Только для режимов != RESETTER
      result += "event: timeToSwitch\n";
      result += "data: {\"tts\":\"";
      result += timeToSwitch;
      result += "\",\"to\":\"";
      result += (socket_state==1?"OFF":"ON");
      result += "\"}";
      result += "\n\n";
    }
    
    server.sendContent(result);
    server.client().flush();
    delay(1);
    server.client().stop();
  //} // for

  #if defined(DEBUG_MODE)
    Serial.print("handleSSE complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/*****************************************************
* Handle "Wheather Sensors" Page Server-Send Events
*/
void handleSensorsSSE() {

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleSensorsSSE started");
  #endif
  
  String result;
//  String Header;
//  Header = "HTTP/1.1 200 OK\r\n";
//  Header += "Content-Type: text/event-stream;\r\n";
//  Header += "Cache-Control: no-cache\r\n";
//  Header += "\r\n";  
//  server.sendContent(Header);
  server.sendContent(FPSTR(EVENT_STREAM_HEADER));
  
  //server.handleClient();

  result  = "retry: 7000\n"; // реальный интервал обновления ~ 9-10 сек.
  if (rtcPresent) {
    // Считываем температуру с датчика модуля RTC
    RtcTemperature t1 = Rtc.GetTemperature();
    String sgn = (t1.AsFloat()<0?"":"+");
    result += "event: rtcTempState\n";
    result += "data: ";
    result += sgn + t1.AsFloat()+"&#0176;&nbsp;C";
    result += "\n\n";
  }    
  
  if (onewire_pin>=0) {
    // Считываем температуру с датчиков DS18B20 OneWire
    OneWire oneWire(onewire_pin);
    DallasTemperature sensors(&oneWire);
    sensors.begin();
    sensors.setResolution(onewire_precision); // Set resolution globally for all sensors
    sensors.requestTemperatures();
    result += "event: onewireTempState\n";
    result += "data: {";

    int n=sensors.getDeviceCount();
    for (int i=1; i<=n; i++) {
      DeviceAddress devAddr;
      sensors.getAddress(devAddr, i-1);

      float tempC = sensors.getTempC(devAddr);
      String sgn = (tempC<0?"":"+");

      result += "\"ow_tv"+String(i)+"\":\""+sgn+String(tempC)+"&#0176;&nbsp;C\""+(i==n?"":",");
    }
    result += "}\n\n";
  }

  if (dht22_pin>0) {
    // Считываем температуру и влажность с датчика DHT22
    SimpleDHT22 dht22;
    float dht22temp = 0;
    float dht22humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(dht22_pin, &dht22temp, &dht22humidity, NULL)) == SimpleDHTErrSuccess) {
      String sgn = (dht22temp<0?"":"+");
      
      result += "event: dht22TempState\n";
      result += "data: {\"dht22temp\":\""+sgn+dht22temp+"&#0176;&nbsp;C\",\"dht22humidity\":\"&nbsp;"+dht22humidity+"&nbsp;%\"}";
      result += "\n\n";
    }
  }

  /****** BMP280 ******/
  if (BMP280_setup()) /* init */
  { /* Read BMP280 Sensor */
    double T,P,A;
    if (BMP280_read(T,P,A)) {
      String sgn = "+"; if (T < 0) sgn="";
      result += "event: bmp280TempState\n";
      result += "data: {\"bmp280temp\":\""+sgn+String(T,2)+"&#0176;&nbsp;C\",\"bmp280pres\":\"&nbsp;"+String(P,1)+"&nbsp;mmHg\",\"bmp280alt\":\"&nbsp;"+String((int)A)+"&nbsp;m\"}";
      result += "\n\n";
    }
  }
  
//  Serial.println("=== RESULT===>");
//  Serial.println(result);
  
  server.sendContent(result);
  server.client().flush();
  delay(1);
  server.client().stop();

  #if defined(DEBUG_MODE)
    Serial.print("handleSensorsSSE complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/*************************
* Handle "Thermostat" Page
*/
void handleThermostat() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleThermostat started");
  #endif

  server.sendContent( FPSTR(RESPONSE_HTTP_200_HEADER_CHUNKED) );
  //server.handleClient();

  sendChunk( FPSTR(START_HEAD) );
  sendChunk( FPSTR(META_TAGS) );
  sendChunk( FPSTR(STYLE) );

  /* MSIE не поддерживает SSE, поэтому принудительно обновляем страницу
   * каждые 30 секунд
   */
  int refreshPeriod = 30;

  /* script */
  sendChunk(
    String() +
    "<script language='jscript'>\n"
    "  var socket_state="+socket_state+";\n"
    "  var power_is_on_s='"+FPSTR(POWER_IS_ON_S)+"';\n"
    "  var power_is_off_s='"+FPSTR(POWER_IS_OFF_S)+"';\n"
    "  var ua=navigator.userAgent; if (ua.search(/MSIE/)!=-1 || ua.search(/rv:11.0/)!=-1) { window.setTimeout(function(){location.reload(true)},"+String(refreshPeriod)+"000) }\n"
    "</script>");

  sendChunk( FPSTR(FINISH_HEAD) );
  sendChunk( StringExt(FPSTR(PAGE_HEADER)).replace("%s1","Thermostat") );

  if (socket_mode==SOCKET_MODE_THERMOSTAT) {
//    sendChunk(
//      String() +
//      "<form method='POST' action='thermostatsave'>\n")
    sendChunk( StringExt(FPSTR(FORM_HEADER)).replace("%s1","thermostatsave") );
      
    sendChunk(
      String() +
      "<input type='hidden' name='h' value='1'/>\n");
      
    /* Current thermostat status */
    sendChunk( StringExt(FPSTR(TABLE_HEADER)).replace("%s1","Current Status") );
      
    /* Current temperature */
    float tempC = getCurrentTemperature(thermostat_sensor);
       String sgn = "+"; if (tempC < 0) sgn="";
    if (tempC != DEVICE_DISCONNECTED_C) {
      /* Температура получена */
      sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Current T&#0176;:")+
        "<span id='currtemp'>"+sgn+tempC+"&#0176;&nbsp;C</span></td></tr>\n"
        );
    }
    else {
      /* Температуру получить не удалось */
      sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Current T&#0176;:")+
        "<span id='curtemp'>Unknown</span></td></tr>\n" );
    }
  
    /* Current socket status */
    sendChunk(
      String() +
      "<tr class='clsTR1' id='socket_status_row' "+(socket_pin<0?"style='display:none'":"")+"><th class='clsTH1' style='vertical-align:middle;p_adding-top:0px;'>Status:</th>"
      "<td class='clsTD1' style='white-space:nowrap;'>\n"
      "<div id='sstate' class='"+
      (socket_mode==SOCKET_MODE_RESETTER?(socket_state==1?"clsOFF":"clsON"):(socket_state==1?"clsON":"clsOFF"))+
      "'>"+
      (socket_mode==SOCKET_MODE_RESETTER?(socket_state==1?FPSTR(RESETTING_S):FPSTR(POWER_IS_ON_S)):(socket_state==1?FPSTR(POWER_IS_ON_S):FPSTR(POWER_IS_OFF_S)))+
      "</div>\n"
      "</td></tr>\n");
  
    /* Thermostat main options (desired temperature and deviation) */
    sendChunk( StringExt(FPSTR(TR0_TH_OPEN_TD)).replace("%s1","Desired T&#0176:") );
    sendChunk( String() +
      "<select name='thermostat_temperature' class='clsSelect'>\n"
      "<option  value='4' "+getSelectedAttribute(thermostat_temperature,  4)+">+4&#0176;&nbsp;C\n"
      "<option  value='5' "+getSelectedAttribute(thermostat_temperature,  5)+">+5&#0176;&nbsp;C\n"
      "<option  value='6' "+getSelectedAttribute(thermostat_temperature,  6)+">+6&#0176;&nbsp;C\n"
      "<option  value='7' "+getSelectedAttribute(thermostat_temperature,  7)+">+7&#0176;&nbsp;C\n"
      "<option  value='8' "+getSelectedAttribute(thermostat_temperature,  8)+">+8&#0176;&nbsp;C\n"
      "<option  value='9' "+getSelectedAttribute(thermostat_temperature,  9)+">+9&#0176;&nbsp;C\n"
      "<option  value='10' "+getSelectedAttribute(thermostat_temperature, 10)+">+10&#0176;&nbsp;C\n"
      "<option  value='11' "+getSelectedAttribute(thermostat_temperature, 11)+">+11&#0176;&nbsp;C\n"
      "<option  value='12' "+getSelectedAttribute(thermostat_temperature, 12)+">+12&#0176;&nbsp;C\n"
      "<option  value='13' "+getSelectedAttribute(thermostat_temperature, 13)+">+13&#0176;&nbsp;C\n"
      "<option  value='14' "+getSelectedAttribute(thermostat_temperature, 14)+">+14&#0176;&nbsp;C\n"
      "<option  value='15' "+getSelectedAttribute(thermostat_temperature, 15)+">+15&#0176;&nbsp;C\n"
      );
    sendChunk( String() +
      "<option  value='16' "+getSelectedAttribute(thermostat_temperature, 16)+">+16&#0176;&nbsp;C\n"
      "<option  value='17' "+getSelectedAttribute(thermostat_temperature, 17)+">+17&#0176;&nbsp;C\n"
      "<option  value='18' "+getSelectedAttribute(thermostat_temperature, 18)+">+18&#0176;&nbsp;C\n"
      "<option  value='19' "+getSelectedAttribute(thermostat_temperature, 19)+">+19&#0176;&nbsp;C\n"
      "<option  value='20' "+getSelectedAttribute(thermostat_temperature, 20)+">+20&#0176;&nbsp;C\n"
      "<option  value='21' "+getSelectedAttribute(thermostat_temperature, 21)+">+21&#0176;&nbsp;C\n"
      "<option  value='22' "+getSelectedAttribute(thermostat_temperature, 22)+">+22&#0176;&nbsp;C\n"
      "<option  value='23' "+getSelectedAttribute(thermostat_temperature, 23)+">+23&#0176;&nbsp;C\n"
      "<option  value='24' "+getSelectedAttribute(thermostat_temperature, 24)+">+24&#0176;&nbsp;C\n"
      "<option  value='25' "+getSelectedAttribute(thermostat_temperature, 25)+">+25&#0176;&nbsp;C\n"
      "<option  value='26' "+getSelectedAttribute(thermostat_temperature, 26)+">+26&#0176;&nbsp;C\n"
      "<option  value='27' "+getSelectedAttribute(thermostat_temperature, 27)+">+27&#0176;&nbsp;C\n"
      "<option  value='28' "+getSelectedAttribute(thermostat_temperature, 28)+">+28&#0176;&nbsp;C\n"
      "<option  value='29' "+getSelectedAttribute(thermostat_temperature, 29)+">+29&#0176;&nbsp;C\n"
      "<option  value='30' "+getSelectedAttribute(thermostat_temperature, 30)+">+30&#0176;&nbsp;C\n"
      "</select>\n");
    sendChunk( FPSTR(TABLE_FOOTER) );

    /* Apply Button */
    sendChunk(
      String() +
      "<input class='clsBtn' type='submit' onclick='ap.style.visibility=\"visible\";' value='Apply' />\n"
      "</form>\n"
      );
  } /* if socket_mode = THERMOSTAT */
  else {
    /* socket_mode != THERMOSTAT */
    sendChunk(
      String()+
      "Device is not in Thermostat mode now.<br>\n"
    );
    /* принудительно обновляем страницу каждые 30 секунд */
  sendChunk(
    String() +
    "<script language='jscript'>\n"
    "  window.setTimeout(function(){location.reload(true)},"+String(refreshPeriod)+"000)\n"
    "</script>");
  }

  /* Navigation menu */
  sendChunk( FPSTR(NAVIGATION_MENU) );
  /* Applying panel */
  sendChunk( FPSTR(APPLYING_PANEL) );
  
  if (socket_mode==SOCKET_MODE_THERMOSTAT) {
    /* SSE script */
    sendChunk( FPSTR(THERMOSTAT_SSE_SCRIPT) );
  }
  
  sendChunk( FPSTR(FINISH_DOC) );
  
  // Finish chunked response
  flushChunkedResponse();

  #if defined(DEBUG_MODE)
    Serial.print("handleThermostat complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/******************************************
* Handle Saving Thermostat Options
*******************************************/
void handleThermostatSave() {
  
  // Basic Аутентификация
  if (authenticationRequired())
    return server.requestAuthentication();

  #if defined(DEBUG_MODE)
    Serial.begin(115200);
    int started = millis();
    Serial.println("handleThermostatSave started");
  #endif

  if (server.arg("h")=="1") {
    /* Из блока настроек switch фактически меням только thermostat_temperature */
    thermostat_temperature = server.arg("thermostat_temperature").toFloat();
    /* Но сохраняем весь блок */
    /** STORE TO EEPROM ********/
    struct switch_v1_t C;
    C.socket_pin = socket_pin;
    C.socket_mode = socket_mode;
    C.switcher_photosensor_enabled = switcher_photosensor_enabled;
    C.switcher_timers_enabled = switcher_timers_enabled;
    C.photosensor_pin = photosensor_pin;
    C.resetter_ping_checker_enabled = resetter_ping_checker_enabled;
    C.resetter_ping_IP = resetter_ping_IP;
    C.resetter_ping_fault_limit = resetter_ping_fault_limit;
    C.resetter_timers_enabled = resetter_timers_enabled;
    memcpy(C.thermostat_sensor, thermostat_sensor, sizeof(thermostat_sensor));
    C.thermostat_temperature = thermostat_temperature;
    C.thermostat_deviation = thermostat_deviation;
    C.thermostat_mode = thermostat_mode;
    
    storeSwitchBlock(C);
    /**********************************/
    Serial.println("termostatsave complete");

    /* Форсируем вызов процесса термостата */
    thermostatFlag = true;
  }
  else {
    Serial.println("termostatsave aborted. no data.");
  }
  // Redirect to "thermostat" page
  server.sendHeader("Location", "thermostat", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length

  #if defined(DEBUG_MODE)
    Serial.print("handleThermostatSave complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}

/**************************************
* Handle Thermostat Page Server-Send Events
*/
void handleThermostatSSE() {

  #if defined(DEBUG_MODE)
    int started = millis();
    Serial.println("handleThermostatSSE started");
  #endif

  String result;
//  String Header, result;
//  Header = "HTTP/1.1 200 OK\r\n";
//  Header += "Content-Type: text/event-stream;\r\n";
//  Header += "Cache-Control: no-cache\r\n";
//  Header += "\r\n";  
//  server.sendContent(Header);
  server.sendContent(FPSTR(EVENT_STREAM_HEADER));
  
  //server.handleClient();
  

  if (socket_mode==SOCKET_MODE_THERMOSTAT) {
    result  = "retry: 8000\n"; // реальный интервал обновления ~ 10 сек.
    /* Состояние розетки для подключения нагрузки */
    result += "event: socketState\n";
    result += "data: ";
    result += socket_state;
    result += "\n\n";
  
    /* Считываем температуру с датчика термостата */
    float tempC = getCurrentTemperature(thermostat_sensor);
    if (tempC != DEVICE_DISCONNECTED_C) {
      String sgn = (tempC<0?"":"+");
      result += "event: currTemp\n";
      result += "data: ";
      result += sgn + tempC+"&#0176;&nbsp;C";
      result += "\n\n";
    }
    else {
      result += "event: currTemp\n";
      result += "data: Unknown";
      result += "\n\n";
    }
  }
  else {
    result += "event: invalidSocketMode\n";
    result += "data: empty";
    result += "\n\n";
  }
  
  server.sendContent(result);
  server.client().flush();
  delay(1);
  server.client().stop();

  #if defined(DEBUG_MODE)
    Serial.print("handleThermostatSSE complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  #endif
}


/*
 * Start head
 */
const char RESPONSE_HTTP_200_HEADER[] PROGMEM = 
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Cache-Control: no-cache, no-store, must-revalidate\r\n"
  "Pragma: no-cache\r\n"
  "Expires: -1\r\n"
  "Connection: close\r\n"
  "\r\n";
  
const char RESPONSE_HTTP_200_HEADER_CHUNKED[] PROGMEM = 
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Cache-Control: no-cache, no-store, must-revalidate\r\n"
  "Pragma: no-cache\r\n"
  "Expires: -1\r\n"
  "Transfer-Encoding: chunked\r\n"
  "Connection: close\r\n"
  "\r\n";

const char START_HEAD[] PROGMEM = 
  "<!DOCTYPE html>\r\n<html>\n<head>\n";
const char FINISH_HEAD[] PROGMEM = 
  "</head>\n<body>\n";
const char FINISH_DOC[] PROGMEM = 
  "</body>\n</html>";
const char PAGE_HEADER[] PROGMEM = 
  "<h1 class='clsH1'>%s1</h1>\n";
/* Четная строка таблицы */  
const char TR0[] PROGMEM = 
  "<tr class='clsTR0'><th class='clsTH1'>%s1</th><td class='clsTD1'>%s2</td></tr>\n";
/* Нечетная строка таблицы */  
const char TR1[] PROGMEM = 
  "<tr class='clsTR1'><th class='clsTH1'>%s1</th><td class='clsTD1'>%s2</td></tr>\n";
  
/*
 * "Applying changes..." float panel
 */
const char APPLYING_PANEL[] PROGMEM = 
  "<table id='ap' style='width:100%;height:100%;position:fixed;top:0;left:0;visibility:hidden;background:rgba(127,127,127,0.8);z-index:999;'>\n"
  "<tr><td style='text-align:center;vertical-align:middle;'>\n"
  "<span style='padding:30px 60px 30px 60px;height:80px;background:#FFFFFF;border:1px solid #777777;'>Applying changes...</span>\n"
  "</td></tr></table>\n";

/*
 * Meta tags    
 */
const char META_TAGS[] PROGMEM = 
  "<meta charset='utf-8'>\n"
  "<meta name='viewport' content='width=device-width, initial-scale=1'>\n";

/*
 * Meta tags    
 */
const char STYLE[] PROGMEM = 
  "<style>\n"
  "BODY {font:normal 10pt helvetica;text-align:center;margin-left:5%;margin-right:5%;}\n"
  "@media (orientation: landscape) and (min-width: 600px) {BODY {margin-left:10%;margin-right:10%} }\n"
  "@media (orientation: landscape) and (min-width: 800px) {BODY {margin-left:15%;margin-right:15%} }\n"
  "@media (orientation: landscape) and (min-width: 1000px){BODY {margin-left:20%;margin-right:20%} }\n"
  "CAPTION {font:bold 12pt helvetica;text-align:left;color:#333333;}\n"
  "HR {width:100%;border-color:#000000;height:0px;}\n"
  ".clsA1 {margin-right:12px;line-height:160%;}\n"
  ".clsH1 {font:bold 18pt helvetica;text-align:left;border-bottom:2px solid #000000;width:400;margin-top:24;}\n"
  ".clsT1 {border-width:1 0 0 0;border-top:1px solid #333322;border-collapse:separate;margin-top:24px;width:100%;}\n"
  ".clsTH1 {text-align:right;border:0;vertical-align:middle;padding:6px 6px 6px 6px;width:33%;}\n"
  ".clsTH2 {border-width:0 0 1 0;border-bottom:1px solid #000000;padding-bottom:2px;}\n"
  ".clsTD1 {text-align:left;border:0;vertical-align:middle;padding:6px 0px 6px 6px;width:60%;}\n"
  ".clsTD2 {text-align:center;border:0;padding-top:8px;}\n"
  ".clsTR0 {background-color:transparent;}\n" /* Четные строки таблицы */
  ".clsTR1 {background-color:#E0FFE0;}\n" /* Нечетные строки таблицы */
  ".clsInputText {font:normal 10pt helvetica;border-top:0px;border-left:0px;border-right:0px;border-bottom:1px solid #336699;width:92%;padding:6px 8px 4px 8px;background-color:transparent;}\n"
  ".clsBtn {font:normal 12pt helvetica;border:1px solid #999999;width:100%;padding:6px 8px 6px 8px;margin-top:24px;background:#CCFFCC;}\n"
  ".clsIcon1 {height:48px;width:48px;}\n"
  ".clsSelect {font:normal 10pt helvetica;border-top:0px;border-left:0px;border-right:0px;border-bottom:1px solid #336699;width:80%;padding:6px 8px 4px 8px;background-color:transparent;}\n"
  ".clsON  {background:#339933;padding:4px 8px 4px 8px;color:#FFFFFF;margin-right:8px;display:inline-block;}\n"
  ".clsOFF {background:#999999;padding:4px 8px 4px 8px;color:#FFFFFF;margin-right:8px;display:inline-block;}\n"
  ".clsDAY {background:#FFFF99;padding:4px 8px 4px 8px;color:#000000;display:inline-block;}\n"
  ".clsNIGHT {background:#333333;padding:4px 8px 4px 8px;color:#FFFFFF;;display:inline-block;}\n"
  "</style>\n";

/*   
 * Navigation menu
 */ 
const char NAVIGATION_MENU[] PROGMEM = 
    "<br><hr>"
    "<a class='clsA1' href='/info'>Info</a>\n"
    "<a class='clsA1' href='/wifi'>WiFi&nbsp;config</a>\n"
    "<a class='clsA1' href='/time'>Time&nbsp;settings</a>\n"
    "<a class='clsA1' href='/switch'>Smart&nbsp;switch</a>\n"
    //"<a class='clsA1' href='/sensors'>Wheather&nbsp;sensors</a>\n"
    "<a class='clsA1' href='/sensors'>Sensors</a>\n"
    "<a class='clsA1' href='/thermostat'>Thermostat</a>\n"
    "<a class='clsA1' href='/reboot'>Reboot&nbsp;device</a>\n"
    //"<a href='/wificlear'>Clear credentials</a>&nbsp;&nbsp;"
    ;

/*    
 * Set Display script
 */
const char SET_DISPLAY_SCRIPT[] PROGMEM = 
    "<script>\n"
    "function setDisplay(flag) {\n"
    "document.all('ip1').disabled=(flag?true:false);"
    "document.all('ip2').disabled=(flag?true:false);"
    "document.all('ip3').disabled=(flag?true:false);"
    "document.all('ip4').disabled=(flag?true:false);"
    "document.all('ip5').disabled=(flag?true:false);"
    "if (flag) {"
    "document.all('ip1').value='';"
    "document.all('ip2').value='';"
    "document.all('ip3').value='';"
    "document.all('ip4').value='';"
    "document.all('ip5').value='';"
    "}\n"
    "}\n"
    "</script>\n";

/*    
 * Tick script
 */
const char TICK_SCRIPT[] PROGMEM = 
    "<script language='jscript'>\n"
    "var n=1; function tick() {n++; if (n>100) {window.location.href='/';}\n"
    "else { document.all('pt').innerText=''+n+'%';\n"
    "document.all('pb').style.width=((n)+'%'); if(n==51){pt.style.color='#FFFFFF'} }}\n"
    "</script>\n";


/*    
 * Switch page SSE script
 */
const char SWITCH_SSE_SCRIPT[] PROGMEM = 
    "<script language='jscript'>\n"
    "if (typeof(EventSource) != 'undefined') {\n"
      "var source = new EventSource('/sse');\n"
      
      "var sstate = document.getElementById('sstate');\n"
      "var saction = document.getElementById('saction');\n"
      "source.addEventListener('socketState', function(e) {\n"
      "if (document.all('socket_mode')[1].checked) {\n" /* resetter */
      " sstate.innerText = (e.data==0?power_is_on_s:resetting_s);\n"
      " sstate.className = (e.data==0?'clsON':'clsOFF');\n"
      " saction.innerText = (e.data==0?reset_s:'');\n"
      "} else {\n" /* другие режимы */
      " sstate.innerText = (e.data==0?power_is_off_s:power_is_on_s);\n"
      " sstate.className = (e.data==0?'clsOFF':'clsON');\n"
      " saction.innerText = (e.data==0?switch_on_s:switch_off_s);\n"
      "}\n"
      "socket_state=e.data;\n"
      "});\n"
      
      "var pstext = document.getElementById('pstext');\n"
      "source.addEventListener('photosensorState', function(e) {\n"
      "pstext.innerText = (e.data==0?'DAY':'NIGHT');\n"
      "pstext.className = (e.data==0?'clsDAY':'NIGHT');\n"
      "});\n"

      "var tck = document.getElementById('ticker');\n"
      "var cmnt = document.getElementById('comment');\n"
      "var tim;\n"
      "var prevSrc;"
      "function tick() {\n"
      "  var n=tck.innerText;"
      "  if (n>0) { n--; tck.innerText=n; prevSrc=ssImg.src; }\n"
      "  else { var newSrc='/sw?st='+(prevSrc.indexOf('on')<0?'on':'off'); if(ssImg.src.indexOf(newSrc)<0) ssImg.src=newSrc;\n"
      "  tck.style.visibility='hidden';"
      "  cmnt.style.visibility='hidden';"
      "  window.clearInterval(tim); }\n"
      "}\n"
      "source.addEventListener('timeToSwitch', function(e) {\n"
      "var data = JSON.parse(e.data);\n"
      "var tts=data.tts; var to=data.to;\n"
      "if (tts==0) {"
      " tck.style.visibility='hidden';"
      " cmnt.style.visibility='hidden';"
      " window.clearInterval(tim);}\n"
      "else {"
      " tck.innerText=tts;cmnt.innerText='sec. to '+to;"
      " tck.style.visibility='visible';"
      " cmnt.style.visibility='visible';"
      " window.clearInterval(tim);tim=window.setInterval('tick()',1000);}"
      "});\n"
      
    "}\n"
    "</script>\n";

/*    
 * Thermostat page SSE script
 */
const char THERMOSTAT_SSE_SCRIPT[] PROGMEM = 
    "<script language='jscript'>\n"
    "if (typeof(EventSource) != 'undefined') {\n"
      "var source = new EventSource('/thermostatsse');\n"
      
      "var sstate = document.getElementById('sstate');\n"
      "source.addEventListener('socketState', function(e) {\n"
      " sstate.innerText = (e.data==0?power_is_off_s:power_is_on_s);\n"
      " sstate.className = (e.data==0?'clsOFF':'clsON');\n"
      "});\n"
      
      "var currtemp = document.getElementById('currtemp');\n"
      "window.setTimeout(function(){" /* таймаут, т.к. первое обращение часто выдает DEVICE_DISCONNECTED_C */
      "source.addEventListener('currTemp', function(e) {\n"
      " currtemp.innerHTML=e.data;\n"
      "});\n"
      "},5000);"

      "source.addEventListener('invalidSocketMode', function(e) {\n"
      " location.reload(true);\n"
      "});\n"
      
    "}\n"
    "</script>\n";

/*    
 * "Wheather sensors" page SSE script
 */
const char SENSORS_SSE_SCRIPT[] PROGMEM = 
    "<script language='jscript'>\n"
    "if (typeof(EventSource) != 'undefined') {\n"
      "var source = new EventSource('/sensorssse');\n"
      
      "var rtc_temp = document.getElementById('rtc_temp');\n"
      "source.addEventListener('rtcTempState', function(e) {\n"
      "rtc_temp.innerHTML=e.data;\n"
      "});\n"

      "function reviver(key,value) { var elt=document.getElementById(key); if(elt!=null) elt.innerHTML=value;}\n"
      "source.addEventListener('onewireTempState', function(e) {\n"
      "var data = JSON.parse(e.data,reviver);\n"
      "});\n"

      "source.addEventListener('dht22TempState', function(e) {\n"
      "var data = JSON.parse(e.data,reviver);\n"
      "});\n"

      "source.addEventListener('bmp280TempState', function(e) {\n"
      "var data = JSON.parse(e.data,reviver);\n"
      "});\n"
      
    "}\n"
    "</script>\n";
    

/*    
 * OneWire PIN changed Script
 */
const char ONEWIRE_PIN_CHANGED_SCRIPT[] PROGMEM = 
    "<script>\n"
    "function onewirePinChanged(pin) {\n"
    "document.all('owp_r1').style.display=(pin<0?'none':'');\n"
    "var table=(document.all('owt_table'));\n"
    "var rows=table.rows;\n"
    " for (var i=rows.length-1;i>1;i--) {\n"
    " table.deleteRow(i);\n"
    " }\n"
    "}\n"
    "</script>\n";

    
/* 
 * WiFi status text 
 */
const char WL_IDLE_STATUS_S[] PROGMEM = "WL_IDLE_STATUS";
const char WL_NO_SSID_AVAIL_S[] PROGMEM = "WL_NO_SSID_AVAIL";
const char WL_SCAN_COMPLETED_S[] PROGMEM = "WL_SCAN_COMPLETED";
const char WL_CONNECTED_S[] PROGMEM = "WL_CONNECTED";
const char WL_CONNECT_FAILED_S[] PROGMEM = "WL_CONNECT_FAILED";
const char WL_CONNECTION_LOST_S[] PROGMEM = "WL_CONNECTION_LOST";
const char WL_DISCONNECTED_S[] PROGMEM = "WL_DISCONNECTED";

const int SECONDS_PER_MINUTE PROGMEM = 60;
const int MINUTES_PER_HOUR   PROGMEM = 60;
const int HOURS_PER_DAY      PROGMEM = 24;

const int SOCKET_MODE_SWITCH     PROGMEM = 0;
const int SOCKET_MODE_RESETTER   PROGMEM = 1;
const int SOCKET_MODE_THERMOSTAT PROGMEM = 2;

const int THERMOSTAT_MODE_HEATER PROGMEM = 0;
const int THERMOSTAT_MODE_COOLER PROGMEM = 1;
const float THERMOSTAT_DEFAULT_TEMPERATURE PROGMEM = 5.0;
const float THERMOSTAT_DEFAULT_DEVIATION   PROGMEM = 0.5;

const int EEPROM_CREDENTIALS_BLOCK_ADDRESS  PROGMEM =   0;
const int EEPROM_NTP_BLOCK_ADDRESS          PROGMEM = 256;
const int EEPROM_SWITCH_BLOCK_ADDRESS       PROGMEM = 512;
const int EEPROM_SENSORS_BLOCK_ADDRESS      PROGMEM = 768;

const char NTP_DEFAULT_SERVER_S[] PROGMEM = "europe.pool.ntp.org";

const int NOT_INSTALLED PROGMEM = -1;

const int RESETTER_PING_DEFAULT_FAULT_LIMIT PROGMEM = 5;

const int ONEWIRE_PRECISION_9_BIT =   9; // 0.5 C
const int ONEWIRE_PRECISION_10_BIT = 10; // 0.25 C
const int ONEWIRE_PRECISION_11_BIT = 11; // 0.125 C
const int ONEWIRE_PRECISION_12_BIT = 12; // 0.06 C

const char AP_SSID_PREFIX_S[] PROGMEM = "ESP8266_AP_";

const char APP_STARTING_S[] PROGMEM = " Starting...";

const int RESET_PERIOD = 10; // sec

const char POWER_IS_ON_S[]  PROGMEM = "Power is On";
const char POWER_IS_OFF_S[] PROGMEM = "Power is Off";
const char SWITCH_ON_S[]  PROGMEM = "Switch On";
const char SWITCH_OFF_S[] PROGMEM = "Switch Off";
const char RESET_S[] PROGMEM = "Reset";
const char RESETTING_S[] PROGMEM = "Resetting";
const char EMPTY_S[] PROGMEM = "";


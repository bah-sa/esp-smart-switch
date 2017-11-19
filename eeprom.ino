/*
 * МОДУЛЬ СОХРАНЕНИЯ И ЧТЕНИЯ НАСТРОЕК ИЗ EEPROM.
 * ВЕРСИЯ 2.
 * **********************************************
 * Максимальный размер EEPROM = 4096 байт.
 * 
 * Разделение EEPROM на блоки:
 * 
 * Блок          Адрес   Размер   Занято
 * -----------------------------------------------
 * CREDENTAILS   0       256 байт      108 байт
 * NTP           256     256 байт      43 байт
 * SWITCH        512     256 байт      51 байт
 * SENSORS       768     256 байт      16 байт
 * FREE_BLOCK1   1024    256 байт
 * FREE_BLOCK2   1280    256 байт
 * FREE_BLOCK3   1536    256 байт
 * FREE_BLOCK4   1792    256 байт
 * TIMERS        2048   2048 байт
 * 
 * В каждый блок записываем структуру данных.
 * Первый байт в блоке содержит версию структуры,
 * Второй (для контроля) - длину этой структуры.
 * 
 * Совместимость сохраненных в EEPROM настроек достигается следующими принципами: 
 * - если возникает необходимость расширить существующую структуру хранящихся данных, 
 *   то создаем новую версию структуры, оставляя все старые версии для совместимости чтения сохраненных данных.
 * - необходимо следить, чтобы размер новой версии структуры не превысил максимальный размер блока данных (256 байт).
 * - запись всегда происходит структурой актуальной версии.
 * 
 *  
 */

/** CREDENTIALS ****************************************/
struct credentials_v1_t
{
    const byte ver = 1;         // structure version
    const byte len = 
      sizeof(credentials_v1_t); // structure size
    char      ssid[32];         // ssid
    char  password[32];         // password
    boolean  dhcp_flag;         // use DHCP
    IPAddress       ip;         // static ip
    IPAddress  gateway;         // static gateway
    IPAddress   subnet;         // subnet mask
    IPAddress     dns1;         // static dns1
    IPAddress     dns2;         // static dns2
};

/*  Вторая версия структуры, отличается от первой добавленным параметром http_port */
struct credentials_v2_t
{
    const byte ver = 2;         // structure version
    const byte len = 
      sizeof(credentials_v2_t); // structure size
    char      ssid[32];         // ssid
    char  password[32];         // password
    boolean  dhcp_flag;         // use DHCP
    IPAddress       ip;         // static ip
    IPAddress  gateway;         // static gateway
    IPAddress   subnet;         // subnet mask
    IPAddress     dns1;         // static dns1
    IPAddress     dns2;         // static dns2
    int      http_port;         // web server port
};

/*  3-я версия структуры, отличается от первой добавленными параметрами www_username и www_password */
struct credentials_v3_t
{
    const byte ver = 3;         // structure version
    const byte len = 
      sizeof(credentials_v3_t); // structure size
    char      ssid[32];         // ssid
    char  password[32];         // password
    boolean  dhcp_flag;         // use DHCP
    IPAddress       ip;         // static ip
    IPAddress  gateway;         // static gateway
    IPAddress   subnet;         // subnet mask
    IPAddress     dns1;         // static dns1
    IPAddress     dns2;         // static dns2
    int      http_port;         // web server port
    char www_username[16];      // www_username
    char www_password[16];      // www_password
};

union credentials_u {
  struct credentials_v1_t V1;
  struct credentials_v2_t V2;
  struct credentials_v3_t V3;
};


/*
 * Запись актуальной версии структуры
 */
void storeCredentialsBlock(struct credentials_v3_t C) {
  int p=0; /* Адрес блока CREDENTIALS */
  EEPROM.begin(4096);
  EEPROM.put(p, C);
  EEPROM.commit();
  EEPROM.end();
  
  Serial.print("CREDENTIALS Parameters stored to EEPROM. ver="); Serial.print(C.ver); Serial.print(" len="); Serial.println(C.len);
}

/*
 * Чтение должно поддерживать любую из версий записанных структур
*/
boolean loadCredentialsBlock() {
  boolean result = true;
  //int p=EEPROM_CREDENTIALS_BLOCK_ADDRESS; /* Адрес блока CREDENTIALS */
  union  credentials_u U = {};  // Объединение структур

  EEPROM.begin(4096);
  
  //Serial.println("*** LOAD CREDENTIALS ===>");
  EEPROM.get(EEPROM_CREDENTIALS_BLOCK_ADDRESS, U.V1); // Считываем в первую версию структуры

  // Можно сначала присвоить параметрам дефолтные значения,
  // а затем перекрывать их считанными данными.
  // Тогда новые параметры, которых нет в старых структурах,
  // останутся в дефолтных значениях.

  // Присваиваем параметрам дефолтные значения
  ssid[0] = 0;
  password[0] = 0;
  stDhcpFlag = true;
  stIP = INADDR_NONE;
  stGateway = INADDR_NONE;
  stSubnet = INADDR_NONE;
  stDns1 = INADDR_NONE;
  stDns2 = INADDR_NONE;
  http_port = 80;
  www_username[0] = 0;
  www_password[0] = 0;

  //Serial.print("CREDENTIALS ver="); Serial.println(U.V1.ver);
  //Serial.print("CREDENTIALS len="); Serial.println(U.V1.len);
  
  if (U.V1.ver==1 && U.V1.len==sizeof(credentials_v1_t)) {
    Serial.println("CREDENTIALS\tversion#1 in store.");
    //EEPROM.get(EEPROM_CREDENTIALS_BLOCK_ADDRESS, U.V1); // Данные в первую версию структуры уже считаны!
    // Разбор параметров структуры
    strncpy(ssid, U.V1.ssid, sizeof(U.V1.ssid)); // dest,src,size
    strncpy(password, U.V1.password, sizeof(U.V1.password));
    stDhcpFlag = U.V1.dhcp_flag;
    stIP = U.V1.ip;
    stGateway = U.V1.gateway;
    stSubnet = U.V1.subnet;
    stDns1 = U.V1.dns1;
    stDns2 = U.V1.dns2;
  }
  else if (U.V1.ver==2 && U.V1.len==sizeof(credentials_v2_t)) {
    Serial.println("CREDENTIALS\tversion#2 in store.");
    EEPROM.get(EEPROM_CREDENTIALS_BLOCK_ADDRESS, U.V2); // Считываем данные во 2-ю версию структуры
    // Разбор параметров структуры
    strncpy(ssid, U.V2.ssid, sizeof(U.V2.ssid)); // dest,src,size
    strncpy(password, U.V2.password, sizeof(U.V2.password));
    stDhcpFlag = U.V2.dhcp_flag;
    stIP = U.V2.ip;
    stGateway = U.V2.gateway;
    stSubnet = U.V2.subnet;
    stDns1 = U.V2.dns1;
    stDns2 = U.V2.dns2;
    http_port = U.V2.http_port; // Новый параметр в структуре
  }
  else if (U.V1.ver==3 && U.V1.len==sizeof(credentials_v3_t)) {
    Serial.println("CREDENTIALS\tversion#3 in store.");
    EEPROM.get(EEPROM_CREDENTIALS_BLOCK_ADDRESS, U.V3); // Считываем данные в 3-ю версию структуры
    // Разбор параметров структуры
    strncpy(ssid, U.V3.ssid, sizeof(U.V3.ssid)); // dest,src,size
    strncpy(password, U.V3.password, sizeof(U.V3.password));
    stDhcpFlag = U.V3.dhcp_flag;
    stIP = U.V3.ip;
    stGateway = U.V3.gateway;
    stSubnet = U.V3.subnet;
    stDns1 = U.V3.dns1;
    stDns2 = U.V3.dns2;
    http_port = U.V3.http_port;
    strncpy(www_username, U.V3.www_username, sizeof(U.V3.www_username)); // dest,src,size
    strncpy(www_password, U.V3.www_password, sizeof(U.V3.www_password));
  }
  else {
    Serial.println("Default CREDENTIALS Parameters applied.");
    result = false;
  }
/*
  Serial.print("ssid="); Serial.println(ssid);
  Serial.print("password="); Serial.println(password);
  Serial.print("stDhcpFlag="); Serial.println(stDhcpFlag);
  Serial.print("ip="); Serial.println(toStringIp(stIP));
  Serial.print("gateway="); Serial.println(toStringIp(stGateway));
  Serial.print("subnet="); Serial.println(toStringIp(stSubnet));
  Serial.print("dns1="); Serial.println(toStringIp(stDns1));
  Serial.print("dns2="); Serial.println(toStringIp(stDns2));
  Serial.print("www_username="); Serial.println(www_username);
  Serial.print("www_password="); Serial.println(www_password);
*/
  return result;
}

/** NTP ********************************************/
struct ntp_v1_t
{
    const byte ver = 1;         // structure version
    const byte len = sizeof(ntp_v1_t);         // structure size
    char    server[32];         // ntp server
    int      time_zone;         // ntp timezone
    int sync_interval;          // ntp sync interval
};

union ntp_u {
  struct ntp_v1_t V1;
  //struct ntp_v2_t V2;
};

/*
 * Запись актуальной версии структуры
 */
void storeNtpBlock(struct ntp_v1_t C) {
  //int p=256; /* Адрес блока NTP */
  EEPROM.begin(4096);
  EEPROM.put(EEPROM_NTP_BLOCK_ADDRESS, C);
  EEPROM.commit();
  EEPROM.end();
  
  Serial.print("NTP Parameters stored to EEPROM. DataSize="); Serial.println(C.len);
}

/*
 * Чтение должно поддерживать любую из версий записанных структур
*/
boolean loadNtpBlock() {
  boolean result = true;
  union  ntp_u U = {};  // Объединение структур

  EEPROM.begin(4096);
  
  //Serial.println("*** LOAD NTP ===>");
  EEPROM.get(EEPROM_NTP_BLOCK_ADDRESS, U.V1); // Считываем в первую версию структуры

  // Присваиваем параметрам дефолтные значения
  String defaultNtpServer = FPSTR(NTP_DEFAULT_SERVER_S);
  defaultNtpServer.toCharArray(ntpServer, 32);
  ntpTimeZone = 3; // MSK
  ntpSyncInterval = 60; // минут
  
  if (U.V1.ver==1 && U.V1.len==sizeof(ntp_v1_t)) {
    Serial.println("NTP\t\tversion#1 in store.");
    //EEPROM.get(EEPROM_NTP_BLOCK_ADDRESS, U.V1); // Данные в первую версию структуры уже считаны!
    // Разбор параметров структуры
    strncpy(ntpServer, U.V1.server, sizeof(U.V1.server)); // dest,src,size
    ntpTimeZone = U.V1.time_zone;
    ntpSyncInterval = U.V1.sync_interval;
  }
/*  
  else if (U.V1.ver==2 && U.V1.len==sizeof(ntp_v2_t)) {
    Serial.println("NTP\t\tversion#2 in store.");
    EEPROM.get(EEPROM_NTP_BLOCK_ADDRESS, U.V2); // Считываем данные во вторую версию структуры
    // Разбор параметров структуры
  }
*/  
  else {
    Serial.println("Default NTP Parameters applied.");
    result = false;
  }
/*
  Serial.print("ntpServer="); Serial.println(ntpServer);
  Serial.print("ntpTimeZone="); Serial.println(String(ntpTimeZone));
  Serial.print("ntpSyncInterval="); Serial.println(String(ntpSyncInterval));
*/
  return result;
}


/** SWITCH ********************************************/
struct switch_v1_t
{
    const byte ver = 1;         // structure version
    const byte len = sizeof(switch_v1_t);      // structure size
    int      socket_pin;        // socket pin
    int     socket_mode;        // socket mode [SOCKET_MODE_SWITCH, SOCKET_MODE_RESETTER, SOCKET_MODE_THERMOSTAT]
    boolean switcher_photosensor_enabled;
    boolean switcher_timers_enabled;
    int      photosensor_pin;        // photosensor pin
    boolean resetter_ping_checker_enabled;
    IPAddress resetter_ping_IP;
    int resetter_ping_fault_limit;
    boolean resetter_timers_enabled;
    DeviceAddress thermostat_sensor;  // thermostat sensor address
    float thermostat_temperature;     // thermostat desired temperature
    float thermostat_deviation;       // thermostat desired temperature deviation
    int thermostat_mode;              // termostat mode [TERMOSTAT_MODE_HEATER, TERMOSTAT_MODE_COOLER]
};

union switch_u {
  struct switch_v1_t V1;
  //struct switch_v2_t V2;
};

/*
 * Запись актуальной версии структуры
 */
void storeSwitchBlock(struct switch_v1_t C) {
  EEPROM.begin(4096);
  EEPROM.put(EEPROM_SWITCH_BLOCK_ADDRESS, C);
  EEPROM.commit();
  EEPROM.end();
  
  Serial.print("SWITCH Parameters stored to EEPROM. DataSize="); Serial.println(C.len);
}

/*
 * Чтение должно поддерживать любую из версий записанных структур
*/
boolean loadSwitchBlock() {
  boolean result = true;
  union  switch_u U = {};  // Объединение структур

  EEPROM.begin(4096);
  
  //Serial.println("*** LOAD SWITCH ===>");
  EEPROM.get(EEPROM_SWITCH_BLOCK_ADDRESS, U.V1); // Считываем в первую версию структуры

  // Присваиваем параметрам дефолтные значения
  socket_pin = NOT_INSTALLED;
  socket_mode = SOCKET_MODE_SWITCH;
  switcher_photosensor_enabled = false;
  switcher_timers_enabled = false;
  photosensor_pin = NOT_INSTALLED;
  resetter_ping_checker_enabled = false;
  resetter_ping_IP = INADDR_NONE;
  resetter_ping_fault_limit = RESETTER_PING_DEFAULT_FAULT_LIMIT;
  resetter_timers_enabled = false;
  thermostat_sensor[0] = 0x00;
  thermostat_sensor[1] = 0x00;
  thermostat_sensor[2] = 0x00;
  thermostat_sensor[3] = 0x00;
  thermostat_sensor[4] = 0x00;
  thermostat_sensor[5] = 0x00;
  thermostat_sensor[6] = 0x00;
  thermostat_sensor[7] = 0x00;
  thermostat_temperature = THERMOSTAT_DEFAULT_TEMPERATURE;
  thermostat_deviation = THERMOSTAT_DEFAULT_DEVIATION;
  thermostat_mode = THERMOSTAT_MODE_HEATER;
  
  if (U.V1.ver==1 && U.V1.len==sizeof(switch_v1_t)) {
    Serial.println("SWITCH\t\tversion#1 in store.");
    //EEPROM.get(EEPROM_SWITCH_BLOCK_ADDRESS, U.V1); // Данные в первую версию структуры уже считаны!
    // Разбор параметров структуры
    socket_pin = U.V1.socket_pin;
    socket_mode = U.V1.socket_mode;
    switcher_photosensor_enabled = U.V1.switcher_photosensor_enabled;
    switcher_timers_enabled = U.V1.switcher_timers_enabled;
    photosensor_pin = U.V1.photosensor_pin;
    resetter_ping_checker_enabled = U.V1.resetter_ping_checker_enabled;
    resetter_ping_IP = U.V1.resetter_ping_IP;
    resetter_ping_fault_limit = U.V1.resetter_ping_fault_limit;
    if (resetter_ping_fault_limit<1 || resetter_ping_fault_limit>10) resetter_ping_fault_limit = RESETTER_PING_DEFAULT_FAULT_LIMIT;
    resetter_timers_enabled = U.V1.resetter_timers_enabled;
    memcpy(thermostat_sensor, U.V1.thermostat_sensor, sizeof(U.V1.thermostat_sensor));
    thermostat_temperature = U.V1.thermostat_temperature;
    thermostat_deviation = U.V1.thermostat_deviation;
    thermostat_mode = U.V1.thermostat_mode;
    
    //strncpy(ntpServer, U.V1.server, sizeof(U.V1.server)); // dest,src,size
  }
/*  
  else if (U.V1.ver==2 && U.V1.len==sizeof(switch_v2_t)) {
    Serial.println("SWITCH\t\tversion#2 in store.");
    EEPROM.get(EEPROM_SWITCH_BLOCK_ADDRESS, U.V2); // Считываем данные во вторую версию структуры
    // Разбор параметров структуры
  }
*/  
  else {
    Serial.println("Default SWITCH Parameters applied.");
    result = false;
  }
/*    
    Serial.print("socket_pin=");
    Serial.println(socket_pin);
    Serial.print("socket_mode=");
    Serial.println(socket_mode);
    Serial.print("switcher_photosensor_enabled=");
    Serial.println(switcher_photosensor_enabled);
    Serial.print("switcher_timers_enabled=");
    Serial.println(switcher_timers_enabled);
    Serial.print("photosensor_pin=");
    Serial.println(photosensor_pin);
    Serial.print("resetter_ping_checker_enabled=");
    Serial.println(resetter_ping_checker_enabled);
    Serial.print("resetter_ping_IP=");
    Serial.println(toStringIp(resetter_ping_IP));
    Serial.print("resetter_ping_fault_limit=");
    Serial.println(resetter_ping_fault_limit);
    Serial.print("resetter_timers_enabled=");
    Serial.println(resetter_timers_enabled);
    Serial.print("thermostat_sensor=");
    Serial.println(getAddressAsString(thermostat_sensor));
    Serial.print("thermostat_temperature=");
    Serial.println(thermostat_temperature);
    Serial.print("thermostat_deviation=");
    Serial.println(thermostat_deviation);
    Serial.print("thermostat_mode=");
    Serial.println(thermostat_mode);
*/    
  return result;
}


/** SENSORS ********************************************/
struct sensors_v1_t
{
    const byte            ver = 1;         // structure version
    const byte len = sizeof(sensors_v1_t); // structure size
    int               onewire_pin;         // onewire pin
    int         onewire_precision;         // onewire sensors (ds18b20) precision
    int                 dht22_pin;         // dht22 pin
    boolean send2narodmon_enabled;         // send sensors data to narodmon.ru
};

union sensors_u {
  struct sensors_v1_t V1;
  //struct sensors_v2_t V2;
};

/*
 * Запись актуальной версии структуры
 */
void storeSensorsBlock(struct sensors_v1_t C) {
  EEPROM.begin(4096);
  EEPROM.put(EEPROM_SENSORS_BLOCK_ADDRESS, C);
  EEPROM.commit();
  EEPROM.end();
  
  Serial.print("SENSORS Parameters stored to EEPROM. DataSize="); Serial.println(C.len);
}

/*
 * Чтение должно поддерживать любую из версий записанных структур
*/
boolean loadSensorsBlock() {
  boolean result = true;
  union  sensors_u U = {};  // Объединение структур

  EEPROM.begin(4096);
  
  //Serial.println("*** LOAD SENSORS ===>");
  EEPROM.get(EEPROM_SENSORS_BLOCK_ADDRESS, U.V1); // Считываем в первую версию структуры

  // Присваиваем параметрам дефолтные значения
    onewire_pin = NOT_INSTALLED;
    onewire_precision = ONEWIRE_PRECISION_10_BIT;
    dht22_pin = NOT_INSTALLED;
    send2narodmon_enabled = false;
  
  if (U.V1.ver==1 && U.V1.len==sizeof(switch_v1_t)) {
    Serial.println("SENSORS\t\tversion#1 in store.");
    //EEPROM.get(EEPROM_SENSORS_BLOCK_ADDRESS, U.V1); // Данные в первую версию структуры уже считаны!
    // Разбор параметров структуры
    onewire_pin = U.V1.onewire_pin;
    onewire_precision = U.V1.onewire_precision;
    dht22_pin = U.V1.dht22_pin;
    send2narodmon_enabled = U.V1.send2narodmon_enabled;
  }
/*  
  else if (U.V1.ver==2 && U.V1.len==sizeof(switch_v2_t)) {
    Serial.println("SENSORS\t\tversion#2 in store.");
    EEPROM.get(EEPROM_SENSORS_BLOCK_ADDRESS, U.V2); // Считываем данные во вторую версию структуры
    // Разбор параметров структуры
  }
*/  
  else {
    Serial.println("Default SENSORS Parameters applied.");
    result = false;
  }
/*    
    Serial.print("onewire_pin=");
    Serial.println(onewire_pin);
    Serial.print("onewire_precision=");
    Serial.println(onewire_precision);
    Serial.print("dht22_pin=");
    Serial.println(dht22_pin);
    Serial.print("send2narodmon_enabled=");
    Serial.println(send2narodmon_enabled);
*/    
  return result;
}

/** REMOTE_SENSORS ********************************************/
/*
struct remote_sensor_v1_t
{
    const byte            ver = 1;        // structure version
    const byte len = sizeof(switch_v1_t); // structure size
    IPAddress ipaddr;  // ip-адрес удаленного устройства
    int port;  // порт
    DeviceAddress id;  // id сенсора
    char sensorType[3];  // тип сенсора (T,H,P,I,U,W,WH,WM,L,R,RX,TX,CO2,S,LAT,LNG,ELE)
    int localNumber;  // присваиваемый локальный номер (1-99)
};
*/
/*
union remote_sensors_u {
  struct sensors_v1_t V1;
  //struct sensors_v2_t V2;
};
*/
/*
 * Запись актуальной версии структуры
 */
/* 
void storeSensorsBlock(struct sensors_v1_t C) {
  EEPROM.begin(4096);
  EEPROM.put(EEPROM_SENSORS_BLOCK_ADDRESS, C);
  EEPROM.commit();
  EEPROM.end();
  
  Serial.print("SENSORS Parameters stored to EEPROM. DataSize="); Serial.println(C.len);
}
*/
/*
 * Чтение должно поддерживать любую из версий записанных структур
*/
/*
boolean loadSensorsBlock() {
  boolean result = true;
  union  sensors_u U = {};  // Объединение структур

  EEPROM.begin(4096);
  
  //Serial.println("*** LOAD SENSORS ===>");
  EEPROM.get(EEPROM_SENSORS_BLOCK_ADDRESS, U.V1); // Считываем в первую версию структуры

  // Присваиваем параметрам дефолтные значения
    onewire_pin = NOT_INSTALLED;
    onewire_precision = ONEWIRE_PRECISION_10_BIT;
    dht22_pin = NOT_INSTALLED;
    send2narodmon_enabled = false;
  
  if (U.V1.ver==1 && U.V1.len==sizeof(switch_v1_t)) {
    Serial.println("SENSORS\t\tversion#1 in store.");
    //EEPROM.get(EEPROM_SENSORS_BLOCK_ADDRESS, U.V1); // Данные в первую версию структуры уже считаны!
    // Разбор параметров структуры
    onewire_pin = U.V1.onewire_pin;
    onewire_precision = U.V1.onewire_precision;
    dht22_pin = U.V1.dht22_pin;
    send2narodmon_enabled = U.V1.send2narodmon_enabled;
  }

//  else if (U.V1.ver==2 && U.V1.len==sizeof(switch_v2_t)) {
//    Serial.println("SENSORS\t\tversion#2 in store.");
//    EEPROM.get(EEPROM_SENSORS_BLOCK_ADDRESS, U.V2); // Считываем данные во вторую версию структуры
//    // Разбор параметров структуры
//  }

  else {
    Serial.println("Default SENSORS Parameters applied.");
    result = false;
  }

//    Serial.print("onewire_pin=");
//    Serial.println(onewire_pin);
//    Serial.print("onewire_precision=");
//    Serial.println(onewire_precision);
//    Serial.print("dht22_pin=");
//    Serial.println(dht22_pin);
//    Serial.print("send2narodmon_enabled=");
//    Serial.println(send2narodmon_enabled);

  return result;
}
*/

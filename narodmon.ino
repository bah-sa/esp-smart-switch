/*
* Отправка данных на narodmon.ru по протоколу TCP.
* 
* Передача показаний по протоколам TCP / UDP на narodmon.ru:8283

Если Ваше устр-во допускает только ввод IP адреса или у Вас проблемы с DNS,
то используйте любой IP из списка nslookup narodmon.ru до которого у Вас наименьший ping,
но помните, что IP-адрес сервера может сменится, а DNS-имя - нет. 
TCP также можно использовать для массовой загрузки показаний на сервер "задним числом" 
путем передачи в одном пакете данных показаний датчика в разные моменты времени, 
но не более 4КБ на пакет для TCP и 512 байт для UDP. 
Предпочтительно отправлять показания всех датчиков в одном пакете данных 
для минимизации издержек на инициализацию соединения с сервером и импорт показаний датчиков.
Ответ сервера при успешной отправке будет OK (или текст ошибки или команда на исполнение), 
что рекомендуется использовать в качестве подтверждения получения данных и при его отсутствии повторить отправку через несколько секунд. 
Конец строки ответа сервера завершается спецсимволом \n код 10(0A). 
Для оперативного отслеживания состояния логических входов и выходов рекомендуется в ответ на получение команды от сервера 
возвращать новое состояние логических датчиков в уже открытый ранее сокет согласно протоколу, 
но только в случае изменений дабы не получить ошибку малого интервала показаний.
При использовании протокола UDP ответа сервера не последует.


Три датчика реалтайм:
 #47-6A-37-19-01-90
 #T1#-12.69
 #H1#80
 #P1#706
 ## Формат пакета данных:
 #MAC[#NAME][#LAT][#LNG][#ELE]\n
 #mac1#value1[#time1][#name1]\n
 ...
 #macN#valueN[#timeN][#nameN]\n## 
Загрузка истории показаний:
 #47-6A-37-19-01-90
 #T1#-12.69#1506937232
 #T1#21.08#1506936632
 #T1#29.46#1506936032
 ## C названием и GPS:
 #47-6A-37-19-01-90#Метео#56.2801#38.4965#123.5
 #T1#-12.69#Улица
 #T2#29.46#Дом
 #P1#706#Барометр
 ## 

\n - спецсимвол конца строки - код 10(0A).
## - признак окончания пакета данных (после него разрыв соединения), в [..] заключен необязательный параметр.
*/

bool sendToNarodmon() { // Собственно формирование пакета и отправка.

  WiFiClient client;
  DeviceAddress tempDeviceAddress;
  String clientMac = "";
  unsigned char mac[6];
  WiFi.macAddress(mac);
  clientMac += macToStr(mac);
  
  String buf;
  buf = "#" + clientMac + "\n"; // заголовок c указанием MAC-адреса устройства
  /* OneWire DS18B20 sensors */
  if (onewire_pin>=0) {
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
      buf = buf + "#T"+i+"#"+tempC+"#DS18B20_"+getAddressAsString(devAddr)+"\n";
    }
  }
  /****** BMP280 ******/
  if (BMP280_setup()) /* init */
  { /* Read BMP280 Sensor */
    double T,P,A;
    if (BMP280_read(T,P,A)) {
      buf = buf + "#T97#"+String(T,2)+"#BMP280_T\n";
      buf = buf + "#P97#"+String(P,1)+"#BMP280_P\n";
    }
  }
  /* DS3231 temperature sensor */
  if (rtcPresent) {
    RtcTemperature trtc = Rtc.GetTemperature();
    buf = buf + "#T98#"+trtc.AsFloat()+"#RTC3231\n";
  }
  /* DHT22 temperature & humidity sensors */
  if (dht22_pin>0) {
    SimpleDHT22 dht22;
    float dht22temp = 0;
    float dht22humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(dht22_pin, &dht22temp, &dht22humidity, NULL)) == SimpleDHTErrSuccess) {
      buf = buf + "#T99#"+dht22temp+"#DHT22_T\n";
      buf = buf + "#H99#"+dht22humidity+"#DHT22_H\n";
    }
  }
  buf = buf + "##\r\n"; // закрываем пакет

  printCurrentDateTime();
  Serial.println(" ===> sendToNarodmon ===>");
  if (debug) Serial.print(buf);

  if (!client.connect("narodmon.ru", 8283)) { // попытка подключения
    Serial.println("connection failed");
    return false; // не удалось;
  } else {
    client.print(buf); // и отправляем данные
    delay(10);
    while (client.available()) {
      String line = client.readStringUntil('\r'); // если что-то в ответ будет - все в Serial
      //Serial.print("response=[");      
      Serial.print(line);      
      //Serial.println("]");
    }
    printCurrentDateTime();
    Serial.println(" <=== finished");
  }

  return true; //ушло
}


String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    String d = String(mac[i], 16);
    result += (d.length()>1?d:"0"+d);
    if (i < 5)
      result += '-';
  }
  return result;
}


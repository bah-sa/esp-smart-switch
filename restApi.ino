/**
 * REST API
 */
 
/**
 * Обрабатываемые типы запросов:
 * GET /api/sensors
 * GET /api/sensors?sensorType={T,H,P,A,ALL}
 * GET /api/sensors?id=XXXXXXXXXXXXXXXXXXXX
 * GET /api/sensors?id=XXXXXXXXXXXXXXXXXXXX&sensorType={T,H,P,A,ALL}
 */
void handleApiSensors() {

  if ( server.method() != HTTP_GET )
    return server.send(405, "text/plain", "Method Not Allowed");

  int started; 
  if (debug) {
    started = millis();
    Serial.println("handleApiSensors started");
  }

  String sensorType = server.arg("sensorType")!="" ? server.arg("sensorType") : "ALL";
  String sensorId = server.arg("id")!="" ? server.arg("id") : "ALL";
  
  String Header, result;
  Header = "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: application/json;\r\n";
  Header += "Cache-Control: no-cache\r\n";
  Header += "Sensor-Type: "+sensorType+"\r\n";
  Header += "Sensor-Id: "+sensorId+"\r\n";
  Header += "\r\n";  
  server.sendContent(Header);
  server.handleClient();

  int cnt = 0; // счетчик датчиков
  result  = "{\n\"data\": [\n"; // data array started
  
  if ( (sensorType=="ALL"||sensorType=="T") && 
       (sensorId=="ALL"||sensorId==getAddressAsString(THERMOSENSOR_RTC)) &&
       rtcPresent
       ) {
    // Считываем температуру с датчика модуля RTC
    RtcTemperature t1 = Rtc.GetTemperature();
    String sgn = (t1.AsFloat()<0?"":"+");
    delay(1);      
    result += "{\n"; // sensor start
      result += "\"type\": \"sensors\",\n";
      result += "\"id\": \""+getAddressAsString(THERMOSENSOR_RTC)+"\",\n";
      result += "\"attributes\": {";
        result += "\"name\": \"RTC Thermosensor\",";
        result += "\"sensorType\": \"T\",";
        result += "\"value\": \""+String(t1.AsFloat())+"\",";
        result += "\"formattedValue\": \""+sgn + t1.AsFloat()+"&#0176;&nbsp;C\"";
      result += "}\n"; // attributes end
    result += "}"; // sensor end

    cnt++;  // инкремент счетчика датчиков
  }    
  
  if ((sensorType=="ALL"||sensorType=="T") && onewire_pin>=0) {
    // Считываем температуру с датчиков DS18B20 OneWire
    OneWire oneWire(onewire_pin);
    DallasTemperature sensors(&oneWire);
    sensors.begin();
    sensors.setResolution(onewire_precision); // Set resolution globally for all sensors
    sensors.requestTemperatures();

    int n=sensors.getDeviceCount();
    for (int i=1; i<=n; i++) {
      DeviceAddress devAddr;
      sensors.getAddress(devAddr, i-1);
      
      if (sensorId=="ALL"||sensorId==getAddressAsString(devAddr)) {
        float tempC = sensors.getTempC(devAddr);
        String sgn = (tempC<0?"":"+");
        
        delay(1);      
        result += (cnt>0?",\n":"");
        result += "{\n"; // sensor start
          result += "\"type\": \"sensors\",\n";
          result += "\"id\": \""+getAddressAsString(devAddr)+"\",\n";
          result += "\"attributes\": {";
            result += "\"name\": \"DS18B20\",";
            result += "\"sensorType\": \"T\",";
            result += "\"value\": \""+String(tempC)+"\",";
            result += "\"formattedValue\": \""+sgn+String(tempC)+"&#0176;&nbsp;C\"";
          result += "}\n"; // attributes end
        result += "}"; // sensor end
  
        cnt++;
      } // if
    } // for
  }

  if ( (sensorType=="ALL"||sensorType=="T"||sensorType=="H") &&
       (sensorId=="ALL"||sensorId==getAddressAsString(THERMOSENSOR_DHT22)) &&
       dht22_pin>0 ) {
    // Считываем температуру и влажность с датчика DHT22
    SimpleDHT22 dht22;
    float dht22temp = 0;
    float dht22humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(dht22_pin, &dht22temp, &dht22humidity, NULL)) == SimpleDHTErrSuccess) {
      String sgn = (dht22temp<0?"":"+");
      
      delay(1);
      if (sensorType=="ALL"||sensorType=="T") {
        result += (cnt>0?",\n":"");
          result += "{\n"; // sensor start
            result += "\"type\": \"sensors\",\n";
            result += "\"id\": \""+getAddressAsString(THERMOSENSOR_DHT22)+"\",\n";
            result += "\"attributes\": {";
              result += "\"name\": \"DHT22 Thermosensor\",";
              result += "\"sensorType\": \"T\",";
              result += "\"value\": \""+String(dht22temp)+"\",";
              result += "\"formattedValue\": \""+sgn+dht22temp+"&#0176;&nbsp;C\"";
            result += "}\n"; // attributes end
          result += "}"; // sensor end
          cnt++;
      }

      if (sensorType=="ALL"||sensorType=="H") {
        result += (cnt>0?",\n":"");
        result += "{\n"; // sensor start
          result += "\"type\": \"sensors\",\n";
          result += "\"id\": \""+getAddressAsString(THERMOSENSOR_DHT22)+"\",\n";
          result += "\"attributes\": {";
            result += "\"name\": \"DHT22 Humidity sensor\",";
            result += "\"sensorType\": \"H\",";
            result += "\"value\": \""+String(dht22humidity)+"\",";
            result += "\"formattedValue\": \""+String(dht22humidity)+"&nbsp;%\"";
          result += "}\n"; // attributes end
        result += "}"; // sensor end
        cnt++;
      }
    }
  }

  /****** BMP280 ******/
  if ( (sensorType=="ALL"||sensorType=="T"||sensorType=="P"||sensorType=="A") &&
       (sensorId=="ALL"||sensorId==getAddressAsString(THERMOSENSOR_BMP280)) &&
       BMP280_setup() ) /* init */
  { /* Read BMP280 Sensor */
    double T,P,A;
    if (BMP280_read(T,P,A)) {
      String sgn = "+"; if (T < 0) sgn="";

      delay(1);
      if (sensorType=="ALL"||sensorType=="T") {
        result += (cnt>0?",\n":"");
        result += "{\n"; // sensor start
          result += "\"type\": \"sensors\",\n";
          result += "\"id\": \""+getAddressAsString(THERMOSENSOR_BMP280)+"\",\n";
          result += "\"attributes\": {";
            result += "\"name\": \"BMP280 Thermosensor\",";
            result += "\"sensorType\": \"T\",";
            result += "\"value\": \""+String(T,2)+"\",";
            result += "\"formattedValue\": \""+sgn+String(T,2)+"&#0176;&nbsp;C\"";
          result += "}\n"; // attributes end
        result += "}"; // sensor end
        cnt++;
      }

      if (sensorType=="ALL"||sensorType=="P") {
        result += (cnt>0?",\n":"");
        result += "{\n"; // sensor start
          result += "\"type\": \"sensors\",\n";
          result += "\"id\": \""+getAddressAsString(THERMOSENSOR_BMP280)+"\",\n";
          result += "\"attributes\": {";
            result += "\"name\": \"BMP280 Pressure sensor\",";
            result += "\"sensorType\": \"P\",";
            result += "\"value\": \""+String(P,1)+"\",";
            result += "\"formattedValue\": \""+String(P,1)+"&nbsp;mmHg\"";
          result += "}\n"; // attributes end
        result += "}"; // sensor end
        cnt++;
      }

      if (sensorType=="ALL"||sensorType=="A") {
        result += (cnt>0?",\n":"");
        result += "{\n"; // sensor start
          result += "\"type\": \"sensors\",\n";
          result += "\"id\": \""+getAddressAsString(THERMOSENSOR_BMP280)+"\",\n";
          result += "\"attributes\": {";
            result += "\"name\": \"BMP280 Altitude sensor\",";
            result += "\"sensorType\": \"A\",";
            result += "\"value\": \""+String((int)A)+"\",";
            result += "\"formattedValue\": \""+String((int)A)+"&nbsp;m\"";
          result += "}\n"; // attributes end
        result += "}"; // sensor end
        cnt++;
      }
    }
  }
  result += "\n]}";
  
//  Serial.println("=== RESULT===>");
//  Serial.println(result);
  
  server.sendContent(result);
  server.client().flush();
  delay(1);
  server.client().stop();

  if (debug) {
    Serial.print("handleApiSensors complete ["); Serial.print(String(millis()-started)); Serial.println(" msec.]");
  }
}


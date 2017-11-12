/*
 * ТЕРМОСТАТ
*/
void checkThermostatState() {
  //if (CompareDeviceAddresses(thermostat_sensor, nonselectedThermosensor)) {
  if (CompareDeviceAddresses(thermostat_sensor, THERMOSENSOR_NON_SELECTED)) {
    Serial.println("===> checkThermostatState ERROR ===> NO SENSOR SELECTED");
    return; /* Не выбран датчик */
  }

  float tempC = getCurrentTemperature(thermostat_sensor);
  if (tempC != DEVICE_DISCONNECTED_C) {
    /* Температура получена */

    float lowThreshold = thermostat_temperature - thermostat_deviation;
    float highThreshold = thermostat_temperature + thermostat_deviation;

    if (socket_state==0) {
      /* Нагрузка отключена */
      if (thermostat_mode==0 && /* Режим отопления */
        tempC <= lowThreshold) { /* Температура упала */
        /* Включаем отопление */    
        setSocketState(1, String("ТЕРМОСТАТ НА ОТОПЛЕНИЕ. T=")+tempC+" <= "+lowThreshold+".");
      }
      else if (thermostat_mode==1 && /* Режим охлаждения */
        tempC >= highThreshold) { /* Температура выросла */
        /* Включаем охлаждение */    
        setSocketState(1, String("ТЕРМОСТАТ НА ОХЛАЖДЕНИЕ. T=")+tempC+" >= "+highThreshold+".");
      }
    }
    else {
      /* Нагрузка включена */
      if (thermostat_mode==0 && /* Режим отопления */
        tempC >= thermostat_temperature + thermostat_deviation) { /* Температура выросла */
        /* Отключаем отопление */    
        setSocketState(0, String("ТЕРМОСТАТ НА ОТОПЛЕНИЕ. T=")+tempC+" >= "+highThreshold+".");
      }
      else if (thermostat_mode==1 && /* Режим охлаждения */
        tempC <= lowThreshold) { /* Температура упала */
        /* Отключаем охлаждение */    
        setSocketState(0, String("ТЕРМОСТАТ НА ОХЛАЖДЕНИЕ. T=")+tempC+" <= "+lowThreshold+".");
      }
    }
  }
  else
    Serial.println("===> checkThermostatState ERROR ===> DEVICE_DISCONNECTED");
  
}

/*
 * Получаем актуальную температуру на датчике по его адресу.
 * В случае ошибки возвращается значение DEVICE_DISCONNECTED_C.
 */
float getCurrentTemperature(DeviceAddress devAddr) {
  float result = DEVICE_DISCONNECTED_C;
  if (CompareDeviceAddresses(devAddr, THERMOSENSOR_RTC)) {
    /* RTC */
    if (rtcPresent)
      result = Rtc.GetTemperature().AsFloat();
      
  } else if (CompareDeviceAddresses(devAddr, THERMOSENSOR_BMP280)) {
    if (BMP280_setup()) /* init */
    { /* Read BMP280 Sensor */
      double T,P,A;
      if (BMP280_read(T,P,A)) {
        result = (float)T;
      }
    }
  } else if (CompareDeviceAddresses(devAddr, THERMOSENSOR_DHT22)) {
    /* DHT22 */
    if (dht22_pin>0) {
      SimpleDHT22 dht22;
      float dht22temp = 0;
      float dht22humidity = 0;
      int err = SimpleDHTErrSuccess;
      if ((err = dht22.read2(dht22_pin, &dht22temp, &dht22humidity, NULL)) == SimpleDHTErrSuccess) {
        result = dht22temp;
      }
    }
  } else {
    /* OneWire DS18B20 */
    if (onewire_pin>=0) {
      OneWire oneWire(onewire_pin);
      DallasTemperature sensors(&oneWire);
      sensors.begin();
      sensors.setResolution(onewire_precision); // Set resolution globally for all sensors
      sensors.requestTemperaturesByAddress(devAddr);
      result = sensors.getTempC(devAddr);
    }
  }
  return result;  
}

/*
 * Сравниваем два адреса термодатчиков
 */
boolean CompareDeviceAddresses(const DeviceAddress addr1, const DeviceAddress addr2) {
  for (int i=0; i<=6 ;i++) {
    if (addr1[i]!=addr2[i])
      return false;
  }
  return true;
}



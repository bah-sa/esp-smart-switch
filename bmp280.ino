/*
// CONNECTIONS:
// BMP280 SDA --> SDA (NODEMCU PIN=D2 GPIO4)
// BMP280 SCL --> SCL (NODEMCU PIN=D1 GPIO5)
// BMP280 VCC --> 3.3v
// BMP280 GND --> GND
// BMP280 CSB --> 3.3v (необязательно)
*/

boolean BMP280_setup() {
  
  //if(!bmp.begin(sda,scl)){ 
  if(!bmp.begin()){ 
    //Serial.println("+++++++++>>>> BMP init failed!");
    return false;
  } 
  else {
    //Serial.println("+++++++++>>>> BMP init success!");
    return true;
  }
  bmp.setOversampling(4); 
}

boolean BMP280_read(double& T, double& P, double& A) {
  char result = bmp.startMeasurment(); 
  if (result!=0){ 
    delay(result); 
    result = bmp.getTemperatureAndPressure(T,P); 
    if (result!=0) 
    { 
      A = bmp.altitude(P,P0);
      P=(float)0.75*P; // пересчитываем давление в мм.рт.ст.
      return true;
    }
  } 
  return false;
}



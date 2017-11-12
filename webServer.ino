/**
 * Инициализируем и стартуем web-сервер
 */
void restartWebServer(int port) {
    Serial.println("Stopping WebServer...");
    server.stop();
    server.close();
    Serial.println("WebServer stopped...");
    startWebServer(port);
}

void startWebServer(int port) {
  server = ESP8266WebServer(port);
  
  /* Setup web pages: root, wifi config pages, etc. */
  server.on("/", handleRoot);
  server.on("/hotspot-detect.html", handleRoot);
  server.on("/info", handleInfo);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  //server.on("/wificlear", handleWifiClear);
  server.on("/time", handleTime);
  server.on("/timesave", handleTimeSave);
  server.on("/reboot", handleReboot);
  server.on("/switch", handleSwitch);
  server.on("/switchsave", handleSwitchSave);
  server.on("/sensors", handleSensors);
  server.on("/sensorssave", handleSensorsSave);
  server.on("/sensorssse", handleSensorsSSE);
  server.on("/sw", handleSw);
  server.on("/sse", handleSSE);
  server.on("/thermostat", handleThermostat);
  server.on("/thermostatsave", handleThermostatSave);
  server.on("/thermostatsse", handleThermostatSSE);
  server.onNotFound ( handleNotFound );
  //server.on("/chunktest", handleChunkTest);
  
  server.begin(); // Web server start
  Serial.print("WebServer started on port "); Serial.println(port);
}


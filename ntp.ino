/* Sync time */
boolean syncTime() {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, ntpServer, ntpTimeZone*3600);
  
  /* start NTP client */
  timeClient.begin();
  Serial.print("NTP Syncronizing to [");
  Serial.print(ntpServer);
  Serial.println("]...");
  
  if (timeClient.forceUpdate()) {
    Serial.println("NTP Sync complete.");

    RtcDateTime dt = RtcDateTime();
    dt.InitWithEpoch32Time(timeClient.getEpochTime());
    Rtc.SetDateTime(dt);
    
    Serial.print("RTC time updated to ");
    printCurrentDateTime();
    Serial.println("");
    return true;
  }
  else {
    Serial.println("NTP Sync failed.");
    return false;
  }
}


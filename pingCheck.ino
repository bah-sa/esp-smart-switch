//#define PING_REMOTE_IP      8, 8, 8, 8
//IPAddress pingAddr(PING_REMOTE_IP); // ip address to ping

void testPING()
{ 
  printCurrentDateTime();
  Serial.print(" Pinging host ");
  Serial.println(toStringIp(resetter_ping_IP));

  if (Ping.ping(resetter_ping_IP,1)) { // одна попытка
    printCurrentDateTime();
    Serial.println(" Ping Success!");
    resetter_ping_fault_counter = 0;
  } else {
    printCurrentDateTime();
    Serial.println(" Ping Error :(");
    resetter_ping_fault_counter++;
    //Serial.print("resetter_ping_fault_counter="); Serial.println(resetter_ping_fault_counter);
    //Serial.print("resetter_ping_fault_limit="); Serial.println(resetter_ping_fault_limit);

    // Так, как в режиме RESETTER нагрузка подключается к нормально замкнутым контактам реле,
    // то для сброса нагрузки реле необходимо включить!
    if (resetter_ping_fault_counter>=resetter_ping_fault_limit) {
      //printCurrentDateTime(); Serial.println(" RESET NEED.");
      setSocketState(1, "PING FAULT LIMIT EXCEEDED. RESET APPLYING."); // Сброс нагрузки
      resetter_ping_fault_counter = 0;
      // Теперь надо подождать "RESET_PERIOD" секунд и вновь включить нагрузку
      resetticker.once(RESET_PERIOD, restorePingResetState);
    }
  }
}

void restorePingResetState() {
  //printCurrentDateTime();
  setSocketState(0, "RESTORE STATE AFTER RESET BY PING FAULTS."); // Восстановление питания нагрузки
}


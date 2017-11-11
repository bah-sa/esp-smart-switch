/*
 * Управление розеткой (SOCKET)
 */

/****************************************
 * Включение/Выключение реле
 * newState = 1 (Включить), 0 (Отключить)
 */
void setSocketState(int newState, String comment) {
  if (socket_state!=newState) {
    printCurrentDateTime(); Serial.print(" "); Serial.print(comment); Serial.println ( newState==1?" ВКЛЮЧАЮ НАГРУЗКУ.":" ВЫКЛЮЧАЮ НАГРУЗКУ." );
    pinMode(socket_pin,OUTPUT);
    digitalWrite(socket_pin, (newState==0?HIGH:LOW));
    
    socket_state = newState; // Запоминаю состояние нагрузки
    
    // TODO - Сделать запоминание состояния нагрузки в EEPROM,
    // чтобы в случае пропадания питания состояние нагрузки восстанавливалось при инициализации устройства.
    // Актуально - для режима SWITCHER при ручном управлении (без фотосенсора) или при управлении по таймерам.
    
    photosensorCounter = (socket_state==1?photosensorOnValue:photosensorOffValue); // Сбрасываю счетчик фотосенсора (даже если изменение было вручную), чтобы отсчет задержки пошел заново
  } else {
    printCurrentDateTime(); Serial.println ( " СОСТОЯНИЕ НАГРУЗКИ НЕ ИЗМЕНИЛОСЬ." );
  }
}


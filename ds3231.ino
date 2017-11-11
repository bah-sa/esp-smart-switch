/*
// CONNECTIONS:
// DS3231 SDA --> SDA (NODEMCU PIN=D2 GPIO4)
// DS3231 SCL --> SCL (NODEMCU PIN=D1 GPIO5)
// DS3231 VCC --> 3.3v or 5v
// DS3231 GND --> GND
// DS3231 SQW --> NODEMCU PIN=D7 // GPIO13 // interrupt can use any gpio except GPIO16 (D0)
*/
#include <pgmspace.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

//const int interruptPin = 13; // GPIO13 (NODEMCU PIN=D7)
boolean rtcPresent; /* признак "Модуль RTC присутствует" */

/******************************************************
 * Инициализация модуля RTC
 * 
 * Возвращаемое значение: 
 * true  - модуль RTC успешно проинициализирован.
 * false - проинициализироовать модуль RTC не удалось.
 */
boolean RTC_setup() 
{

    Rtc.Begin();
    // Чтобы изменить дефолтные пины D1=GPIO5 и D2=GPIO4
    // Wire.begin(0, 2); // use pin 0 and 2 for SDA, SCL

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); // Время компиляции скетча

    if (!Rtc.IsDateTimeValid()) // В RTC установлено некорректное время
    {
      Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning()) // Часы в RTC не запущены
    {
      Rtc.SetIsRunning(true);  // Запускаем часы
      if (!Rtc.GetIsRunning()) { // Не удалось запустить часы (Модуль RTC не подключен или неисправен)
        rtcPresent = false;
        return rtcPresent; // Модуль RTC инициализировать не удалось
      }
    }

    RtcDateTime now = Rtc.GetDateTime(); // Получили текущее время из RTC
    if (now < compiled) // Если время меньше, чем время компиляции скетча
    {
      Rtc.SetDateTime(compiled); // Устанавливаем время в RTC, равное времени компиляции скетча
    }
/*
    // Инициализируем аппаратное прерывание
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
*/    

    Rtc.Enable32kHzPin(false); // Отключаем выход 32КГц
/*    
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne); // Режим работы SQW-пина = "Alarm One"
    // Инициализируем первое срабатывание будильника "Alarm One"
    RtcDateTime alarmTime = now + firstInterruptInterval; // Через firstInterruptInterval
    DS3231AlarmOne alarm1(0,0,
            alarmTime.Minute(), 
            alarmTime.Second(),
        DS3231AlarmOneControl_MinutesSecondsMatch);
    Rtc.SetAlarmOne(alarm1);

    // Сбрасываем все необработанные алармы, иначе будильник не сработает
    Rtc.LatchAlarmsTriggeredFlags();
*/
    rtcPresent = true;
    return rtcPresent; // Модуль RTC успешно проинициализирован
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Т.к. данный обработчик прерывает выполнение другого кода
// избегайте выполнения в нем операций, занимающих длительное время,
// особенно связанных с коммуникациями.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
void handleInterrupt()
{
/*    
    if (rtcPresent)
      Serial.println(" RTC INTERRUPT");
    else
*/

    printCurrentDateTime();
    Serial.println(" ===> TICKER INTERRUPT");

      
//    Serial.printf("===> Free heap size: %u\n", ESP.getFreeHeap());

    if (rtcPresent) { // Модуль RTC присутствует
      ntpSyncIntervalCounter++; // Инкремент счетчика интервалов
      if (ntpSyncIntervalCounter >= ntpSyncInterval) { // Счетчик интервалов достиг порога
        // Устанавливаем флаг "Необходима NTP-синхронизация"
        ntpUpdateFlag = true;
      }
      /*
      else {
        // Взводим будильник на следующее срабатывание
        rechargeAlarm();
      }
      */
    }

    /* Отправка показаний на Narodmon */
    if (send2narodmon_enabled) {
      narodmonSendIntervalCounter++;
      
      //Serial.print("narodmonSendIntervalCounter="); Serial.println(narodmonSendIntervalCounter);
      //Serial.print("narodmonSendInterval="); Serial.println(narodmonSendInterval);
      
      if ( narodmonSendIntervalCounter >= narodmonSendInterval) { // Счетчик интервалов достиг порога
        // Устанавливаем флаг "Необходима отправка показаний в narodmon"
        send2NarodmonFlag = true;
      }
    }

    /* Resetter Ping */
    if (socket_mode==SOCKET_MODE_RESETTER && socket_pin>=0 && resetter_ping_checker_enabled) {
      pingFlag = true;
      //Serial.println("===> Взведен флаг pingFlag");
    }
    //else
    //  Serial.println("===> Не Взведен флаг pingFlag");

    /* Resetter Ping */
    if (socket_mode==SOCKET_MODE_THERMOSTAT && socket_pin>=0)
      thermostatFlag = true;
}

/*********************************************
 * Взводим будильник на следующее срабатывание
 */
/* 
void rechargeAlarm() {
  if (rtcPresent) {
    RtcDateTime now = Rtc.GetDateTime(); // Текущее время
    RtcDateTime nextAlarmTime = now + nextInterruptInterval; // Следующее время срабатывания будильника - через nextInterruptInterval
    DS3231AlarmOne alarm1(0,0,
      nextAlarmTime.Minute(), 
      nextAlarmTime.Second(),
      DS3231AlarmOneControl_MinutesSecondsMatch);

    Rtc.SetAlarmOne(alarm1); // Установка будильника "Alarm One"
    Rtc.LatchAlarmsTriggeredFlags(); // Сбрасываем все необработанные алармы, иначе будильник не сработает
  }
}
*/

#define countof(a) (sizeof(a) / sizeof(a[0]))
/********************************************
 * Ф-ия возвращает отформатированную строку
 * с датой и временем
 */
String getFormattedDateTime(const RtcDateTime& dt) {
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    return datestring;
}

/*************************************
 * Ф-ия выводит на печать дату и время
 */
void printDateTime(const RtcDateTime& dt)
{
  Serial.print(getFormattedDateTime(dt));
}
/*********************************************
 * Ф-ия выводит на печать текущую дату и время
 */
void printCurrentDateTime()
{ if (rtcPresent) {
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
  }
}


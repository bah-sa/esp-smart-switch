/*
 * ТАЙМЕРЫ ДЛЯ УПРАВЛЕНИЯ НАГРУЗКОЙ
 * 
 * структура для хранения таймера в EEPROM
 * int      id 
 * char[32] Name
 * boolean  active
 * boolean  repeatable
 * int      dayofweek
 * ?        time
 * RtcDateTime next
 * int      action (0-выключить, 1-включить, 2-reset)
 */

typedef struct
{
    uint8_t  id;                              /**< unique timer identifier */
    char     name[12];                        /**< timer name */
    boolean  active;                          /**< timer is active */
    boolean  repeatable;                      /**< timer is repeatable */
    uint8_t  daysofweek;                      /**< tbit mask of days of week */
    uint8_t  time;                            /**< time in day */
    RtcDateTime next;                         /**< next datetime */
    int32_t  action;                          /**< action (0-выключить, 1-включить, 2-reset) */
} socket_timer_t;

/* 
class Timer 
{
  public:
  protected:
    uint8_t     _id;          // уникальный идентификатор
    char[32]    _name;        // имя
    boolean     _active;      // активен
    boolean     _repeatable;  // повторяемый
    byte        _daysofweek;  // дни недели (битовая маска XXXX XXXX)
    char[8]     _time;        // время
    uint8_t     _action;
    RtcDateTime _next;
}
*/

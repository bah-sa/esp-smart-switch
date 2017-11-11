/*
*/

const char ESP_PIN_NOT_INSTALLED[] PROGMEM = "Not Installed";
const char ESP_PIN_D0[] PROGMEM = "D0";
const char ESP_PIN_D1[] PROGMEM = "D1";
const char ESP_PIN_D2[] PROGMEM = "D2";
const char ESP_PIN_D3[] PROGMEM = "D3";
const char ESP_PIN_D4[] PROGMEM = "D4";
const char ESP_PIN_D5[] PROGMEM = "D5";
const char ESP_PIN_D6[] PROGMEM = "D6";
const char ESP_PIN_D7[] PROGMEM = "D7";
const char ESP_PIN_D8[] PROGMEM = "D8";
const char ESP_PIN_D9[] PROGMEM = "RX(D9)";
const char ESP_PIN_D10[] PROGMEM = "TX(D10)";

const char* const ESP_PIN_NAME_S[] = {ESP_PIN_D0,ESP_PIN_D1,ESP_PIN_D2,ESP_PIN_D3,ESP_PIN_D4,ESP_PIN_D5,ESP_PIN_D6,ESP_PIN_D7,ESP_PIN_D8,ESP_PIN_D9,ESP_PIN_D10};
const int ESP_PIN_GPIO_N[] = {16,5,4,0,2,14,12,13,15,3,1};
//int pins[11] = {0,1,2,3,4}

/*
 * Функция возвращает HTML-конструкцию из тэгов OPTIONS
 * для выбора пина в элементе SELECT
 * исключая пины из списка excludeGPIO и выбирая как "selected" пин, соответствующий selectedGPIO
 */
String getSelectPinOptions(int excludeGPIO[], const int excludeCount, const int selectedGPIO) {
  String result;

  int n = array_count(ESP_PIN_GPIO_N);

  if (!contains(excludeGPIO,excludeCount,-1))
    result += "<option  value='-1' " + getSelectedAttribute(selectedGPIO, -1) + ">" + FPSTR(ESP_PIN_NOT_INSTALLED) + "\n";
  for (int i=0; i < n; i++) {
    if (!contains(excludeGPIO,excludeCount,ESP_PIN_GPIO_N[i]))
      result += "<option  value='"+String(ESP_PIN_GPIO_N[i])+"' " + getSelectedAttribute(selectedGPIO,  ESP_PIN_GPIO_N[i]) + ">" + FPSTR(ESP_PIN_NAME_S[i]) + "\n";
  }

  return result;
}

/* Функция проверяет, содержится ли значение val в массиве arr */
boolean contains(int arr[], const int excludeCount, const int val) {
  //Serial.println("===> CONTAINS ===>");
  //int n = sizeof(arr)/sizeof(int);
  //Serial.print("===> n=");
  //Serial.println(n);
  for (int i=0; i < excludeCount; i++) {
    //Serial.print("===> i=");
    //Serial.println(i);
    if (arr[i]==val) {
      //Serial.println("===> RETURN TRUE");
      return true;
    }
  }
  //Serial.println("===> RETURN FALSE");
  return false;
}


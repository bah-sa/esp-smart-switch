/** Is this an IP? */
boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}



/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

/** Get file extension from path */
String getFileExtension(String path) {
  String res = "";
  int n = path.lastIndexOf(".");
  if (n > 0) {
    res = path.substring(n+1);
  }
  return res;
}

inline unsigned char toHex( char ch )
{
   return ( ( ch >= 'A' ) ? ( ch - 'A' + 0xA ) : ( ch - '0' ) ) & 0x0F;
}

// str   - указатель на массив символов
// bytes - выходной буфер
// функция возвращает кол-во байт
//
int convert( char* str, unsigned char* bytes ) 
{
   unsigned char Hi, Lo;

   int i = 0;
   while( ( Hi = *str++ ) && ( Lo = *str++ ) )
   {
      bytes[i++] = ( toHex( Hi ) << 4 ) | toHex( Lo ); 
   }

   return i;
}

String getUptimeAsString() {
  //String result;
  unsigned long runMillis= millis();
  unsigned long seconds=millis()/1000;
  int ss = seconds % SECONDS_PER_MINUTE; seconds /= SECONDS_PER_MINUTE;
  int mi = seconds % MINUTES_PER_HOUR;   seconds /= MINUTES_PER_HOUR;
  int hh = seconds % HOURS_PER_DAY;      seconds /= HOURS_PER_DAY;
  int dd = seconds;

  char uptimestring[42];
    snprintf_P(uptimestring, 
            countof(uptimestring),
            PSTR("%u days, %02u:%02u:%02u"),
            dd,
            hh,
            mi,
            ss );
  return uptimestring;
}

String getVccAsString() {
  float vcc = ESP.getVcc(); vcc /= 1000.0;
  return String(vcc)+" V";
  
}
/*
int convert( char* str, unsigned char* bytes )  
{ 
   unsigned char Hi, Lo; 
   int i = 0; 
   while( ( Hi = *str++ ) && ( Lo = *str++ ) ) 
   { 
      bytes[i++] = ( toHex( Hi ) << 4 ) | toHex( Lo );  
   } 
   return i; 
} 
*/

// function to get a device address as String
String getAddressAsString(DeviceAddress deviceAddress)
{ String result = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) result += "0";
    result += String(deviceAddress[i], HEX);
  }
  return result;
}

String lpad(String v) {
  return (v.length()<2?"0"+v:v);
}


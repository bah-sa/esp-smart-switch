#include <string.h>
#include "Arduino.h"

class StringExt : public String {
public:
  StringExt() : String()
  {}
  StringExt(const char* arg) : String(arg)
  {}
  StringExt(const __FlashStringHelper* arg) : String(arg)
  {}
  StringExt(String arg) : String(arg)
  {}
  
	StringExt replace(String s1, String s2)
	{ String v = String::c_str();
    v.replace(s1,s2);
    return StringExt(v);
	}
};

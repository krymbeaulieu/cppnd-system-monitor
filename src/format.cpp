#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  if (seconds == 0) {
    return "00:00:00";
  }
  string hours = std::to_string(seconds / 3600);
  string minutes = std::to_string((seconds % 3600) / 60);
  string seconds_str = std::to_string(((seconds % 3600) % 60));
  string out = std::string(2 - hours.length(), '0') + hours + ":" +
               std::string(2 - minutes.length(), '0') + minutes + ":" +
               std::string(2 - seconds_str.length(),'0') + seconds_str;
  return out;
}
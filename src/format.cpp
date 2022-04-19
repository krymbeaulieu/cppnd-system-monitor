#include <string>
#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
    if (seconds == 0) {return "00:00:00";}
    int hours = seconds/3600;
    int minutes = (seconds%3600)/60;
    int seconds_short = ((seconds%3600)%60); 
    string out;
    return std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds_short);}
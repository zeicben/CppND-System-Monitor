#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int ss;
    int mm;
    int hh;
    string hour;

    hh = seconds / 3600;
    seconds = seconds % 3600;
    mm = seconds / 60;
    seconds = seconds % 60;
    ss = seconds;
    // Build string with format "HH:MM:SS"
    hour = to_string(hh) + ":" + to_string(mm) + ":" + to_string(ss);

    return hour;
}
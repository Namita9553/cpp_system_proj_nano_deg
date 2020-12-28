#include <string>

#include "format.h"

using std::to_string;
using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long min, hrs;
  min = seconds / 60;
  hrs = min / 60;
  return to_string(hrs) + ":" + to_string(min % 60) + ":" +
         to_string(seconds % 60);
}
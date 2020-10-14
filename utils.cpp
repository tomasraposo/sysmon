#include <ncurses.h>
#include "utils.h"

float utils::transform(std::pair<float,float> r1, std::pair<float, float> r2, float val) {
  return (val - r1.first) / (r1.second - r1.first)
    * (r2.second - r2.first) + r2.first;
}

float utils::percent(float whole_val, float part_val) {
  return (whole_val * 100.0) / part_val;
}

void utils::lshift(std::vector<float> *list, float f) {
  for(size_t i = 0; i < list->size()-1; i++) 
    list->at(i) = list->at(i+1);  
  list->at(list->size()-1) = f;
}

void utils::rshift(std::vector<float> *list, float f) {
  for(int i = list->size()-1; i > 0; i--) 
    list->at(i) = list->at(i-1);
  list->at(0) = f;
}

std::string utils::parse_time(long seconds) {
  int hours, mins;
  seconds %= (24 * 3600);
  hours = seconds / 3600;
  seconds %= 3600;
  mins = seconds / 60;
  seconds %= 60;
  if (hours < 1)
    return std::to_string(mins)+":"+std::to_string(seconds);
  else 
    return std::to_string(hours)+" h"+std::to_string(mins)+" mins";
  return std::string("");
}

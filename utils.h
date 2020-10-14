#ifndef UTILS_H_
#define UTILS_H_
#include <ncurses.h>
#include <vector>
#include <string>
#include <utility>

namespace utils {
  float transform(std::pair<float,float> r1, std::pair<float, float> r2, float val);
  float percent(float whole_val, float part_val);
  void lshift(std::vector<float> *list, float f);
  void rshift(std::vector<float> *list, float f);
  std::string parse_time(long secs);
}
#endif

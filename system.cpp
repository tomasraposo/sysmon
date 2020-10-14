#include "system.h"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

void sys::set_os_info() {
  auto line_parser =
    [](std::string line) {
      return line.substr(line.find('"')+1);
    };
  std::ifstream fs("/etc/os-release");
  std::string line;
  if (fs.is_open()) {
    while (getline(fs, line)) {
      std::string token=line.substr(0,line.find("="));
      if (token=="NAME") {
	os+=line_parser(line);
      } else if (token=="VERSION") {
	os+=line_parser(line);
      }
    }
    std::replace(os.begin(), os.end(), '"', ' ');
  }
  fs.close();
}

void sys::set_kernel_info() {
  std::ifstream fs("/proc/version");
  std::string line;
  if (fs.is_open()) {
    getline(fs, line);
    kernel=line.substr(0, line.find("(")-1);
  }
  fs.close();
}

void sys::set_up_time() {
  std::ifstream fs("/proc/uptime");
  int days, hours, mins;
  long seconds;
  std::string line;
  if (fs.is_open()) {
    getline(fs, line, ' ');
    std::stringstream stream(line);
    stream >> seconds;
    days = seconds / (24 * 3600);
    seconds %= (24 * 3600);
    hours = seconds / 3600;
    seconds %= 3600;
    mins = seconds / 60;
    up_time = " "+std::to_string(days)+" days, "+
      std::to_string(hours)+" hours, "+std::to_string(mins)+" minutes";
  }
  fs.close();
}

void sys::set_procs() {
  std::ifstream fs("/proc/stat");
  std::string line, token, val;
  while (getline(fs, line)) {
    token = line.substr(0,line.find(' '));
    val = line.substr(line.find(' ')+1);
    if (token=="processes") {
      std::istringstream stream(val);
      stream >> total_procs;
    } else if (token=="procs_running") {
      std::istringstream stream(val);
      stream >> running_procs;
    }
  }
  fs.close();
}

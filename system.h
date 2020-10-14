#ifndef SYSTEM_H
#define SYSTEM_H
#include <string>
#include <vector>
#include "memory.h"
#include "cpu.h"

struct sys {
  std::string os;
  std::string kernel;
  std::string up_time;
  long total_procs;
  int running_procs;
  void set_os_info();
  void set_kernel_info();
  void set_up_time();
  void set_procs();
};

struct sys_meter {
  sys system;
  cpu_meter cpumeter;
  mem_meter memmeter;
};
#endif

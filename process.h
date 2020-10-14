#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include <vector>
#include <algorithm>
#include "system.h"
#include "memory.h"
#include <unistd.h>

enum cpu_metrics {utime=14, stime, cutime, cstime, starttime=22};

class proc {
private:
  int pid;
  int uid;
  std::string user;
  int pr;
  int ni;
  int virt;
  int res;
  int shr;
  std::string s;
  float cpu_pct;
  float mem_pct;
  std::string time;
  std::string cmd;
public:
  proc();
  proc(int _pid, mem memory);
  void set_pid(int pid);
  void set_uid();
  void set_user();
  void set_pr();
  void set_ni();
  void set_virt();
  void set_res();
  void set_shr();
  void set_s();
  void set_cpu_pct();
  void set_mem_pct(mem memory);
  void set_time();
  void set_cmd();
  int get_pid();
  int get_uid();
  std::string get_user();
  int get_pr();
  int get_ni();
  int get_virt();
  int get_res();
  int get_shr();
  std::string get_s();
  float get_cpu_pct();
  float get_mem_pct();
  std::string get_time();
  std::string get_cmd();
};

class procs_meter {
private:
  std::vector<proc> procs;
  std::vector<long> cpu_stats;
public:
  procs_meter();
  void read_procs_pids(mem memory);
  std::vector<long> read_cpu_stats(int pid);
  std::string parse_proc_pid_stat(int pid, int attr_id);
  long parse_proc_pid_status(int pid, std::string attr);
  std::vector<proc>& get_procs();
};
#endif


#include "process.h"
#include <experimental/filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include "utils.h"
#include <thread>
#include <chrono>
namespace exfs = std::experimental::filesystem;

proc::proc()=default;
proc::proc(int _pid, mem memory) {
  set_pid(_pid);
  set_uid();
  set_user();
  set_pr();
  set_ni();
  set_virt();
  set_res();
  set_shr();
  set_s();
  set_cpu_pct();
  set_mem_pct(memory);
  set_time();
  set_cmd(); 
}

void proc::set_pid(int pid) {
  this->pid=pid;
}

void proc::set_uid() {
  std::ifstream fs{"/proc/"+std::to_string(pid)+"/status"};
  std::string line;
  if (fs.is_open())
    while (std::getline(fs, line)) {
      std::string id=line.substr(0, line.find(":"));
      if (id=="Uid") {
	std::istringstream stream(line);
	std::istream_iterator<std::string> begin(stream), end;
	std::vector<std::string> vals(begin, end);
        if (!vals.empty()) std::stringstream(vals.at(1)) >> uid;
        fs.close();
	return;
      }
    }
}

void proc::set_user() {
  std::ifstream fs{"/etc/passwd"};
  std::string line;
  if (fs.is_open()) {
    while (getline(fs, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream stream(line);
      std::istream_iterator<std::string> begin(stream), end;
      std::vector<std::string> vec(begin,end);
      if (!vec.empty() && vec.at(2)==std::to_string(uid)) {
	std::stringstream(vec.at(0)) >> user;
	fs.close();
	return;
      }
    }
  }
}

void proc::set_pr() {
  const int attr_id = 18;
  std::string ret_stat = procs_meter{}.parse_proc_pid_stat(pid, attr_id);
  int val;
  if (ret_stat!=std::string("Failed to parse /proc/[pid]/stat"))
    if (std::stringstream(ret_stat) >> val) pr = val;
}

void proc::set_ni() {
  const int attr_id = 19;
  std::string ret_stat = procs_meter{}.parse_proc_pid_stat(pid, attr_id);
  int val;
  if (ret_stat!=std::string("Failed to parse /proc/[pid]/stat")) 
    if (std::stringstream(ret_stat) >> val) ni = val;
}

void proc::set_virt() {
  std::string vm_sz = "VmSize";
  long vm_val = procs_meter{}.parse_proc_pid_status(pid, vm_sz);
  if (vm_val!=-1) res = vm_val;
}
void proc::set_res() {
  std::string vm_rss = "VmRSS";
  long vm_val = procs_meter{}.parse_proc_pid_status(pid, vm_rss);
  if (vm_val!=-1) res = vm_val;
}

void proc::set_shr() {
  std::ifstream fs("/proc/"+std::to_string(pid)+"/statm");
  std::string line;
  long shared_mem{0l};
  int kb{1024};
  int page_sz(sysconf(_SC_PAGE_SIZE));
  int page_sz_kb{0};
  if (fs.is_open()) {
    int attr_index=0;
    while (std::getline(fs, line, ' ')) {
      if (attr_index==2) {
	if (std::stringstream(line) >> shared_mem) {
	  page_sz_kb = page_sz / kb;
	  shr = shared_mem*page_sz;
	  fs.close();
	  return;
	}
      }
      ++attr_index;
    }
  }
}

void proc::set_s() {
  const int attr_id = 3;
  std::string ret_stat = procs_meter{}.parse_proc_pid_stat(pid, attr_id); 
  if (ret_stat!=std::string("Failed to parse /proc/[pid]/stat")) s = ret_stat;    
}

void proc::set_cpu_pct() {
  auto cpu_stats = procs_meter{}.read_cpu_stats(pid); 
  std::ifstream fs("/proc/uptime");
  int clk_ticks(sysconf(_SC_CLK_TCK));
  std::string line;
  long uptime{0};
  long seconds{0};
  float total_time{0.0f};
  if (fs.is_open()) {
    std::getline(fs, line, ' ');
    if (std::stringstream(line) >> uptime) {
      for (size_t i=0; i<cpu_stats.size()-1; ++i)
	total_time += cpu_stats.at(i);
    }
    seconds = uptime - (cpu_stats.at(cpu_stats.size()-1) / clk_ticks);
    cpu_pct = static_cast<float>(100.0f * ((total_time / clk_ticks) / seconds));
    fs.close();
  }
}

void proc::set_mem_pct(mem memory) {
  mem_pct=static_cast<float>(res*100.0f/memory.mem_total);
}

void proc::set_time() {
  const int attr_id = 22;
  long secs=0;
  std::string ret_stat = procs_meter{}.parse_proc_pid_stat(pid, attr_id);
  if (ret_stat!=std::string("Failed to parse /proc/[pid]/stat")) {
    if (std::stringstream(ret_stat) >> secs) {
      secs/=sysconf(_SC_CLK_TCK);
      time = utils::parse_time(secs);
    }
  }
}
void proc::set_cmd() {
  std::ifstream fs{"/proc/"+std::to_string(pid)+"/cmdline"};
  if (fs.is_open()) {
    std::getline(fs, cmd);
    std::replace(cmd.begin(), cmd.end(), '\0', ' ');
    fs.close();
  }
}

int proc::get_pid() {return pid;}
int proc::get_uid() {return uid;}
std::string proc::get_user() {return user;}
int proc::get_pr() {return pr;}
int proc::get_ni() {return ni;}
int proc::get_virt() {return virt;}
int proc::get_res() {return res;}
int proc::get_shr() {return shr;}
std::string proc::get_s() {return s;}
float proc::get_cpu_pct() {return cpu_pct;}
float proc::get_mem_pct() {return mem_pct;}
std::string proc::get_time() {return time;}
std::string proc::get_cmd() {return cmd;}

procs_meter::procs_meter()=default;

void procs_meter::read_procs_pids(mem memory) {
  int pid;
  for (auto &entry : exfs::directory_iterator("/proc/")) {
    if (std::stringstream(entry.path().stem())>>pid) 
      procs.emplace_back(proc(pid, memory));
  }
}

std::vector<long> procs_meter::read_cpu_stats(int pid) {
  typedef cpu_metrics cm;
  std::string ret_str = "Failed to parse /proc/[pid]/stat";
  std::vector<int> indexes{cm::utime, cm::stime, cm::cutime, cm::cstime, cm::starttime};
  std::string stat;
  std::stringstream stream;
  long cpu_stat;

  for (size_t i=0; i<indexes.size(); ++i) {
    if (parse_proc_pid_stat(pid, indexes.at(i))!=ret_str) {
      stat = parse_proc_pid_stat(pid, indexes.at(i));
      stream << stat;
      if (stream >> cpu_stat) cpu_stats.push_back(cpu_stat); 
    }
    stream.str("");
  }
  return cpu_stats;
}

std::string procs_meter::parse_proc_pid_stat(int pid, int attr_id) {
  std::ifstream fs("/proc/"+std::to_string(pid)+"/stat");
  std::string line;
  if (fs.is_open()) {
    std::getline(fs, line);
    std::istringstream stream(line);
    std::istream_iterator<std::string> begin(stream), end;
    std::vector<std::string> tokens(begin, end);
    if (!tokens.empty()&&(size_t)(attr_id-1)<=tokens.size()) {
      fs.close();
      return tokens.at(attr_id-1);
    }
  }
  return std::string("Failed to parse /proc/[pid]/stat");
}

long procs_meter::parse_proc_pid_status(int pid, std::string attr) {
  std::ifstream fs("/proc/"+std::to_string(pid)+"/status");
  std::string line;
  long vm_val{0};
  if (fs.is_open()) {
    while (std::getline(fs, line)) {
      std::string term = line.substr(0, line.find(":"));
      if (term==attr) {
        std::istringstream stream(line);
	std::istream_iterator<std::string> begin(stream), end;
	std::vector<std::string> tokens(begin, end);
	if (!tokens.empty()) {
	  fs.close();
	  std::stringstream(tokens.at(1)) >> vm_val;
	  return vm_val;
	}
      }
    }
  }
  return long(-1);
}

std::vector<proc>& procs_meter::get_procs() {return procs;}

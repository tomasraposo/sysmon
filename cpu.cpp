#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/select.h>
#include "cpu.h"
#include "utils.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <chrono>
#include <thread>  

void cpu_meter::read_cpu_usage(std::vector<cpu> &cpus) {
  cpu *p_cpus = &cpus[0];
  char buffer[256];
  FILE *fp = popen("cat /proc/stat | grep \"cpu[0-8]\"","r");
  if (fp==NULL) throw "Unable to read from stdout";
  while (fgets(buffer,256,fp)!=NULL) {
    cpu cpu_n;
    int pos = std::string(buffer).find(" ");
    std::string line = std::string(buffer).substr(pos+1);   
    std::istringstream stream(line);
    std::istream_iterator<size_t> begin(stream), end;
    std::vector<size_t> times(begin, end);
    int index=0; size_t total = size_t(0);
    for (auto iter=times.begin(); iter!=times.end(); ++iter, ++index) {
      if (index==0) cpu_n.user=*iter;
      else if (index==1) cpu_n.nice=*iter;
      else if (index==2) cpu_n.system=*iter;
      else if (index==3) cpu_n.idle=*iter;
      total+=*iter;
    }
    stream.str("");
    cpu_n.total = total;
    *p_cpus = cpu_n;
    ++p_cpus;
  }
  pclose(fp);
}


void cpu_meter::set_cpu_usage_pct(std::vector<cpu> &prev, std::vector<cpu> &next) {
  try {
    cpu_meter::read_cpu_usage(prev);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cpu_meter::read_cpu_usage(next);
  } catch (const char *s) {std::cout << s << std::endl;}
  for(int i=0; i<8; ++i) {
    cpu_pct pct;
    float total = static_cast<float>(next.at(i).total - prev.at(i).total);
    pct.user = static_cast<float>(100.0f * (next.at(i).user - prev.at(i).user) / total);
    pct.nice = static_cast<float>(100.0f * (next.at(i).nice - prev.at(i).nice) / total);
    pct.system = static_cast<float>(100.0f * (next.at(i).system - prev.at(i).system) / total);
    pct.idle = static_cast<float>(100.0f * (next.at(i).idle - prev.at(i).idle) / total);
    pct.active = pct.user + pct.nice + pct.system;
    usage_pct.at(i) = pct;
  }
}

void cpu_meter::set_model_name() {
  std::ifstream fs("/proc/cpuinfo");
  std::string line;
  while (getline(fs, line)) {
    if (line.find("model name")!=std::string::npos) {
      std::stringstream stream(line);
      size_t index = line.find(":");
      if (index!=std::string::npos) {
	model_name=line.substr(index+1);
	return;
      }
    }
  }
}

void cpu_meter::set_num_cpus() {
  const int buffer_size = 256;
  std::string cmd{"lscpu"};
  FILE *stream = popen(cmd.c_str(), "r");
  std::array<char,buffer_size> buffer;
  int num;
  if (!stream) return;
  while (fgets(buffer.data(),buffer_size,stream)) {
    std::stringstream strstream{buffer.data()};
    std::string str{buffer.data()};
    if (strstr(str.c_str(), std::string("CPU(s)").c_str())) {
      for (std::string token; strstream >> token && !strstream.eof();) {
        if (std::stringstream{token} >> num) {
	  pclose(stream);
	  num_cpus = num;
	}
      }
    }
  }
  pclose(stream);
}

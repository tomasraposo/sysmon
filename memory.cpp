#include <fstream>
#include <sstream>
#include "memory.h"
#include <iostream>

void mem::read_mem_usage() {
  auto line_parser =
    [] (std::string line, size_t usage) {
      std::string token;
      std::stringstream stream(line);
      while (stream >> token) {
	if (std::stringstream(token) >> usage) {
	  return usage;
	}
      }
      return size_t(-1);
    };
  std::ifstream fp("/proc/meminfo");
  if (!fp) throw "Unable to read from stdout";
  std::string line{};
  size_t usage=size_t(0);
  while (getline(fp, line)) {
    std::string mem_type = line.substr(0, line.find(" ")-1);
    if (mem_type==std::string("MemTotal")) {
      mem_total = line_parser(line, usage);
    } else if (mem_type==std::string("MemFree")) {
      mem_free = line_parser(line,usage);
    } else if (mem_type==std::string("MemAvailable")) {
      mem_available=line_parser(line,usage);
    } else if (mem_type==std::string("Buffers")) {
      mem_buffered=line_parser(line,usage);
    } else if (mem_type==std::string("Cached")) {
      mem_cached=line_parser(line,usage);
    } else if (mem_type==std::string("SwapTotal")) {
      swap_total=line_parser(line,usage);
    } else if (mem_type==std::string("SwapFree")) {
      swap_free=line_parser(line,usage);
    }
  }
}

void mem_pct::set_mem_pct(mem &memory) {
  const size_t used = memory.mem_total - (memory.mem_free + memory.mem_buffered + memory.mem_cached);
  mem_used = static_cast<float>(100.0f * used / memory.mem_total);
}

void mem_pct::set_swap_pct(mem &memory) {
  const size_t _swap = memory.swap_total - memory.swap_free;
  mem_swap = static_cast<float>(100.0f * _swap / memory.swap_total);
}

void mem_meter::set_mem_usage_pct(mem &memory) {
  memory.read_mem_usage();
  mem_usage.set_mem_pct(memory);
  mem_usage.set_swap_pct(memory);
}


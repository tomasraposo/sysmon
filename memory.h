#ifndef MEMORY_H
#define MEMORY_H
#include <cstddef>

struct mem {
  size_t mem_total;
  size_t mem_free;
  size_t mem_available;
  size_t mem_buffered;
  size_t mem_cached;
  size_t swap_total;
  size_t swap_free;
  void read_mem_usage();
};

struct mem_pct {
  float mem_used;
  float mem_swap;
  void set_mem_pct(mem &memory);
  void set_swap_pct(mem &memory);
};

struct mem_meter {
  mem_pct mem_usage;
  void set_mem_usage_pct(mem &memory);
};
#endif /* MEMORY_H_ */

#ifndef CPU_H_
#define CPU_H_
#include <vector>
#include <string>

struct cpu {
  std::string num;
  size_t user;
  size_t system;
  size_t nice;
  size_t idle;
  size_t total;
};

struct cpu_pct {
  float user;
  float system;
  float nice;
  float idle;
  float active;
};

struct cpu_meter {
  std::string model_name;
  int num_cpus;
  std::vector<cpu_pct> usage_pct{8};
  void read_cpu_usage(std::vector<cpu> &cpus);
  void set_cpu_usage_pct(std::vector<cpu> &prev, std::vector<cpu> &next);
  void set_model_name();
  void set_num_cpus();
};
#endif /* CPU_H */

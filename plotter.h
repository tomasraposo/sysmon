#ifndef PLOTTER_H
#define PLOTTER_H
#include <string>
#include "window.h"
#include "net.h"
#include "system.h"

template <class E>
class plotter {
private:
  int num_vals;
  char graph_char;
  int max_val;
  std::vector<E> data;
public:
  plotter();
  plotter(int num_vals, char graph_char);
  void draw_cpu_usage_graph(window *win, int green, int yellow, int red);
  void draw_progress_bar(float value, std::string str, window *win, int x, int y, int blue);
  void draw_cpu_stats_block(window *win, cpu_meter cpumeter);
  void draw_legend_box(window *win, int green, int yellow, int red);
  void draw_net_stats(window *win, net netw);
  void draw_net_graph(window *win, int height, int green);
  void draw_net_rates(window *win, float rx_rate, float tx_rate);
  char get_graph_char(E val, int pos, int height);
  void lshift(E val);
  int get_max_val();
  void clear();
};
#endif /* PLOTTER_H */

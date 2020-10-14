#ifndef NET_H
#define NET_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <experimental/filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <ncurses.h>

namespace exfs = std::experimental::filesystem;

struct net {
  std::string path;
  unsigned long rx_bytes;
  unsigned long tx_bytes;
  unsigned long rx_packets;
  unsigned long tx_packets;
  unsigned long rx_errors;
  unsigned long tx_errors;
  unsigned long rx_dropped;
  unsigned long tx_dropped;
  unsigned long rx_fifo_errors;
  unsigned long tx_fifo_errors;
  unsigned long rx_compressed;
  unsigned long tx_compressed;
  unsigned long rx_crc_errors;
  unsigned long rx_frame_errors;
  unsigned long rx_length_errors;
  unsigned long rx_missed_errors;
  unsigned long rx_over_errors;
  unsigned long tx_carrier_errors;
  unsigned long tx_heartbeat_errors;
  unsigned long tx_window_errors;
  unsigned long tx_aborted_errors;
  unsigned long collisions;
  unsigned long multicast;
  time_t time_;
  net();
  net(std::string path);
  net(const net &netw);
  void set_iface(std::string path);
  void set_all_stats();
  void set_time();
};

struct net_rates {
  float rx_rate;
  float tx_rate;
  void set_rx_rate(unsigned long rx, unsigned long rxp, int time, int timep);
  void set_tx_rate(unsigned long tx, unsigned long txp, int time, int timep);
};

struct net_stats {
  unsigned long rx_bytes_min;
  unsigned long tx_bytes_min;
  time_t time_;
  net_stats();
  net_stats(net netw);
  void set_rx_bytes_min(net netw);
  void set_tx_bytes_min(net netw);
  void set_time();
};
#endif

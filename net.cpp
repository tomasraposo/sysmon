#include "net.h"
#include <sstream>
#include <ncurses.h>

net::net()=default;

net::net(std::string path) {
  set_iface(path);
  set_all_stats();
  time(&time_);
}

net::net(const net &netw) {
  this->rx_bytes = netw.rx_bytes;
  this->tx_bytes = netw.tx_bytes;
  this->rx_packets = netw.rx_packets;
  this->tx_packets = netw.tx_packets;
  this->rx_errors = netw.rx_errors;
  this->tx_errors = netw.tx_errors;
  this->rx_dropped = netw.rx_dropped;
  this->tx_dropped = netw.tx_dropped;
  this->rx_fifo_errors = netw.rx_fifo_errors;
  this->tx_fifo_errors = netw.tx_fifo_errors;
  this->rx_compressed = netw.rx_compressed;
  this->tx_compressed = netw.tx_compressed;
  this->rx_crc_errors = netw.rx_crc_errors;
  this->rx_frame_errors = netw.rx_frame_errors;
  this->rx_length_errors = netw.rx_length_errors;
  this->rx_missed_errors = netw.rx_missed_errors;
  this->rx_over_errors = netw.rx_over_errors;
  this->tx_carrier_errors = netw.tx_carrier_errors;
  this->tx_heartbeat_errors = netw.tx_heartbeat_errors;
  this->tx_window_errors = netw.tx_window_errors;
  this->tx_aborted_errors = netw.tx_aborted_errors;
  this->collisions = netw.collisions;
  this->multicast = netw.multicast;
  this->time_=netw.time_;
}

void net::set_iface(std::string iface) {
  this->path="/sys/class/net/"+iface+"/statistics";
}

void net::set_all_stats() {
  auto read_val =
    [] (std::string path, unsigned long *val) {
      std::ifstream fs(path);
      std::string line;
      if (fs.is_open()) {
	std::getline(fs, line);
	if (std::stringstream(line)>>*val) 
	  fs.close();
      }
    };
  for (auto &entry : exfs::directory_iterator(path)) {
    std::string fname = entry.path().stem();
    unsigned long val=0;
    read_val(entry.path(), &val);
    if (fname==std::string("rx_bytes")) {
      if (val!=ulong(-1)) rx_bytes=val;
    } else if (fname==std::string("tx_bytes")) {
      if (val!=ulong(-1)) tx_bytes=val;
    } else if (fname==std::string("rx_packets")) {
      if (val!=ulong(-1)) rx_packets=val;
    } else if (fname==std::string("tx_packets")) {
      if (val!=ulong(-1)) tx_packets=val;
    } else if (fname==std::string("rx_errors")) {
      if (val!=ulong(-1)) rx_errors=val;
    } else if (fname==std::string("tx_errors")) {
      if (val!=ulong(-1)) tx_errors=val;
    } else if (fname==std::string("rx_dropped")) {
      if (val!=ulong(-1)) rx_dropped=val;
    } else if (fname==std::string("tx_dropped")) {
      if (val!=ulong(-1)) tx_dropped=val;
    } else if (fname==std::string("rx_fifo_errors")) {
      if (val!=ulong(-1)) rx_fifo_errors=val;
    } else if (fname==std::string("tx_fifo_errors")) {
      if (val!=ulong(-1)) tx_fifo_errors=val;      
    } else if (fname==std::string("rx_compressed")) {
      if (val!=ulong(-1)) rx_compressed=val;      
    } else if (fname==std::string("tx_compressed")) {
      if (val!=ulong(-1)) tx_compressed=val;
    } else if (fname==std::string("rx_crc_errors")) {
      if (val!=ulong(-1)) rx_crc_errors=val;		
    } else if (fname==std::string("rx_frame_errors")) {
      if (val!=ulong(-1)) rx_frame_errors=val;
    } else if (fname==std::string("rx_length_errors")) {
      if (val!=ulong(-1)) rx_length_errors=val;
    } else if (fname==std::string("rx_missed_errors")) {
      if (val!=ulong(-1)) rx_missed_errors=val;
    } else if (fname==std::string("rx_over_errors")) {
      if (val!=ulong(-1)) rx_over_errors=val;		
    } else if (fname==std::string("tx_carrier_errors")) {
      if (val!=ulong(-1)) tx_carrier_errors=val;
    } else if (fname==std::string("tx_heartbeat_errors")) {
      if (val!=ulong(-1)) tx_heartbeat_errors=val;
    } else if (fname==std::string("tx_window_errors")) {
      if (val!=ulong(-1)) tx_window_errors=val;
    } else if (fname==std::string("tx_aborted_errors")) {
      if (val!=ulong(-1)) tx_aborted_errors=val;
    } else if (fname==std::string("collisions")) {
      if (val!=ulong(-1)) collisions=val;
    } else if (fname==std::string("multicast")) {
      if (val!=ulong(-1)) multicast=val;
    }
  }
  set_time();
}

void net::set_time() {
  time(&time_);
}

void net_rates::set_rx_rate(unsigned long rx, unsigned long rxp, int time, int timep) {
  int delta_t = time - timep;
  rx_rate = rx - rxp;
  rx_rate /= delta_t;
  rx_rate /= 1024;
}

void net_rates::set_tx_rate(unsigned long tx, unsigned long txp, int time, int timep) {
  int delta_t = time - timep;    
  tx_rate = tx - txp;
  tx_rate /= delta_t;
  tx_rate /= 1024;
}

net_stats::net_stats()=default;

net_stats::net_stats(net netw) {
  set_rx_bytes_min(netw);
  set_tx_bytes_min(netw);
  set_time();
}

void net_stats::set_rx_bytes_min(net netw) {
  const std::string rx_path = netw.path+"/rx_bytes";
  std::ifstream fs(rx_path);
  std::string line;
  if (fs.is_open()){
    std::getline(fs, line);
    if (std::stringstream(line) >> rx_bytes_min)
	return;
  }
}

void net_stats::set_tx_bytes_min(net netw) {
  const std::string rx_path = netw.path+"/tx_bytes";
  std::ifstream fs(rx_path);
  std::string line;
  if (fs.is_open()){
    std::getline(fs, line);
    if (std::stringstream(line) >> tx_bytes_min)
      return;
  }
}

void net_stats::set_time() {
  time(&time_);
}

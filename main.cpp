#include "window.h"
#include "system.h"
#include "memory.h"
#include "plotter.h"
#include "cpu.h"
#include "net.h"
#include "utils.h"
#include "process.h"
#include <stdlib.h>
#include <iostream>

void init_windows(std::vector<window*> &wins);
void display_systeminfo_win(window *topwin, int max_x, sys_meter sysmeter);
void resize_vec(std::vector<float> &data, int width);
void display_meminfo_win(window *bottomwin, int max_x, mem memory, float x1_val, float x2_val);
void display_net_rates(window *win, plotter<int> rx_plotter, plotter<int> tx_plotter);
void menu(window *win, std::vector<std::string> &ifaces, int sel_item);
void display_net_win(int rx_tx_choice, window *stats_win, window *graph_win,
		     plotter<int> &rx_plotter, plotter<int> &tx_plotter,
		     net &netiface, net_rates &netiface_rates);
void update_net_stats_win(window *stats_win, window *graph_win,
			  plotter<int> &rx_plotter, plotter<int> &tx_plotter);

int main() {
  initscr();
  // raw();
  noecho();
  curs_set(0);
  halfdelay(1);
  refresh();

  sys_meter sysmeter;
  cpu_meter cpumeter;
  mem_meter memmeter;
  
  sysmeter.system.set_os_info();
  sysmeter.system.set_kernel_info();
  sysmeter.system.set_up_time();
  sysmeter.system.set_procs();
  
  std::vector<window*> wins;

  mem memory;

  std::vector<cpu> prev(8);
  std::vector<cpu> next(8);
    
  init_windows(wins);

  keypad(stdscr,1);

  std::vector<float> prog_bars_data(static_cast<int>(wins.at(2)->get_frame().width),0);
  prog_bars_data.resize(wins.at(2)->get_frame().width);

  plotter<float> cpu_plotter(wins.at(1)->get_frame().width, '*');
  plotter<int> rx_plotter(wins.at(4)->get_frame().width, '*');
  plotter<int> tx_plotter(wins.at(4)->get_frame().width, '*');

  if (has_colors()) {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_GREEN, -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_RED, -1);
    init_pair(4, COLOR_BLUE, -1);
    init_pair(5, COLOR_CYAN, -1);
    init_pair(6, COLOR_MAGENTA, -1);
    init_pair(7, COLOR_WHITE, -1);
  }

  cpumeter.set_model_name();

  int rx_tx_choice=0;
  std::vector<std::string> ifaces;
  int sel_item= 0;

  int max_item = 4;

  auto cpu_it = cpumeter.usage_pct.begin();
  menu(wins.at(4), ifaces, sel_item);
  net netiface(ifaces.at(0));
  net_rates netiface_rates;
  netiface.set_all_stats();

  int ch =0;
  do {
    int ch = getch();
    switch(ch) {
    case KEY_LEFT:
      rx_tx_choice=0;
      wclear(wins.at(5)->get_win());
      wins.at(5)->refresh_window();
      break;
    case KEY_RIGHT:
      rx_tx_choice=1;
      wclear(wins.at(5)->get_win());
      wins.at(5)->refresh_window();
      break;
    case KEY_DOWN:
      if (++sel_item > max_item-1) {
	sel_item = 0;
	netiface.set_iface(ifaces.at(sel_item));
	update_net_stats_win(wins.at(4), wins.at(5), rx_plotter, tx_plotter);
	break;
      }
      netiface.set_iface(ifaces.at(sel_item));
      update_net_stats_win(wins.at(4), wins.at(5), rx_plotter, tx_plotter);
      break;
    case KEY_UP:
      if (--sel_item < 0) {
        sel_item = max_item-1;
	netiface.set_iface(ifaces.at(sel_item));
	update_net_stats_win(wins.at(4), wins.at(5), rx_plotter, tx_plotter);
        break;
      }
      netiface.set_iface(ifaces.at(sel_item));
      update_net_stats_win(wins.at(4), wins.at(5), rx_plotter, tx_plotter);
      break;
    case KEY_RESIZE:
      for (window *win : wins)
	delete win;
      clear();
      refresh();
      wins.clear();
      init_windows(wins);
      resize_vec(prog_bars_data, wins.at(2)->get_frame().width);
      cpu_plotter = plotter<float>(wins.at(1)->get_frame().width, '*');
      rx_plotter = plotter<int>(wins.at(4)->get_frame().width, '*');
      tx_plotter = plotter<int>(wins.at(4)->get_frame().width, '*');
      break;
    default:
      break;
    }
    int x, y;
    getmaxyx(stdscr,y,x);
    box(stdscr,ACS_VLINE,ACS_HLINE);
    refresh();
    
    net netiface_prev(netiface);
    netiface.set_all_stats();    
    netiface_rates.set_rx_rate(netiface.rx_bytes, netiface_prev.rx_bytes,
			       netiface.time_, netiface_prev.time_);
    
    netiface_rates.set_tx_rate(netiface.tx_bytes, netiface_prev.tx_bytes,
			       netiface.time_, netiface_prev.time_);

    rx_plotter.lshift(netiface_rates.rx_rate);
    tx_plotter.lshift(netiface_rates.tx_rate);
    
    display_systeminfo_win(wins.at(0), x, sysmeter);

    mvwprintw(wins.at(1)->get_win(), 2, 0, "%s", cpumeter.model_name.c_str());

    cpu_plotter.draw_cpu_usage_graph(wins.at(2), COLOR_PAIR(5), COLOR_PAIR(4), COLOR_PAIR(6));
    
    wins.at(2)->refresh_window();
    
    cpu_plotter.draw_progress_bar(prog_bars_data.at(0),std::string("CPU"),
				  wins.at(1),1, 3, COLOR_PAIR(4));    

    cpu_plotter.draw_cpu_stats_block(wins.at(1), cpumeter);
    
    cpu_plotter.draw_legend_box(wins.at(1), COLOR_PAIR(5), COLOR_PAIR(4), COLOR_PAIR(6));
    
    cpu_plotter.draw_progress_bar(memmeter.mem_usage.mem_used, std::string("MEM"),
			      wins.at(3),wins.at(3)->get_frame().width/2, 2, COLOR_PAIR(4));     
    cpu_plotter.draw_progress_bar(memmeter.mem_usage.mem_swap, std::string("SWAP"),
			      wins.at(3),wins.at(3)->get_frame().width/2, 4, COLOR_PAIR(4));
    
    display_meminfo_win(wins.at(3), x, memory, (wins.at(4)->get_frame().width/4)+1,
			(wins.at(4)->get_frame().width/2)+3);
    display_net_win(rx_tx_choice, wins.at(4), wins.at(5),
		    rx_plotter, tx_plotter, netiface, netiface_rates);
    
    menu(wins.at(4), ifaces, sel_item);
    mvwhline(wins.at(4)->get_win(), wins.at(4)->get_frame().height-1,
	     0, ACS_HLINE, wins.at(3)->get_frame().width/2);
    wrefresh(wins.at(4)->get_win());
    
    memmeter.set_mem_usage_pct(memory);
    cpumeter.set_cpu_usage_pct(prev, next);
    
    if (cpu_it!=cpumeter.usage_pct.end())
      cpu_it = cpumeter.usage_pct.begin();
    cpu_plotter.lshift(cpu_it->active);
    utils::rshift(&prog_bars_data, cpu_it->active);
    ++cpu_it;
  } while (ch !='\n');

  for (window *win : wins) delete win;
  endwin();
  return 0;
}
  
void init_windows(std::vector<window*> &wins) {
  int maxx, maxy;
  getmaxyx(stdscr,maxy,maxx);
  wins.emplace_back(new window(1,1,maxx-1,(maxy-1)/5)); //0
  wins.emplace_back(new window(1,wins.at(0)->get_frame().height,
			       (maxx-1)/2,((maxy-1)/5)*2)); //1
  wins.emplace_back(new window(wins.at(1)->get_frame().width+1,
			       (maxy-1)/5,(maxx-1)/2,((maxy-1)/5)*2)); //2
  wins.emplace_back(new window(1,wins.at(0)->get_frame().height*3,
			       maxx-1,wins.at(0)->get_frame().height)); //3
  wins.emplace_back(new window(1, wins.at(0)->get_frame().height*4,
			       (maxx-1)/2, (maxy-1)/4+1)); //4
  wins.emplace_back(new window(wins.at(4)->get_frame().width+1,
			       wins.at(0)->get_frame().height*4,
			       (maxx-1)/2, (maxy-1)/4+1));

  for (window *win : wins) win->refresh_window();
}

void display_systeminfo_win(window *topwin, int max_x, sys_meter sysmeter) {
  std::string system_info = "System Info";
  std::string cpu_util = "CPU Utilisation";
  wattron(topwin->get_win(), A_STANDOUT);
  mvwprintw(topwin->get_win(),topwin->get_frame().y, 0, "%s", system_info.c_str());
  wattroff(topwin->get_win(), A_STANDOUT);
  mvwhline(topwin->get_win(), 1, system_info.length(), ACS_HLINE, (max_x-13));
  
  int topwin_y = topwin->get_frame().y+2;
  mvwprintw(topwin->get_win(),topwin_y++,1, "Operating System Release: %s", sysmeter.system.os.c_str());
  mvwprintw(topwin->get_win(),topwin_y++,1, "Version of Linux Kernel: %s", sysmeter.system.kernel.c_str());
  mvwprintw(topwin->get_win(),topwin_y++,1, "Up-time: %s", sysmeter.system.up_time.c_str());
  mvwprintw(topwin->get_win(),topwin_y++,1, "Total Processes: %s", std::to_string(sysmeter.system.total_procs).c_str());
  mvwprintw(topwin->get_win(),topwin_y++,1, "Running Processes: %s", std::to_string(sysmeter.system.running_procs).c_str());
  
  wattron(topwin->get_win(), A_STANDOUT);
  mvwprintw(topwin->get_win(),topwin->get_frame().height-1, 0, "%s", cpu_util.c_str());
  wattroff(topwin->get_win(), A_STANDOUT);
  mvwhline(topwin->get_win(), topwin->get_frame().height-1, cpu_util.length(), ACS_HLINE, max_x-17);
  
  topwin->refresh_window();
}

void resize_vec(std::vector<float> &data, int width) {
  auto p_data = &data;
  int diff = width - data.size();  
  if(diff < 0) {
    for(int i = 0; i < std::abs(diff); i++) {
      utils::lshift(p_data, 0);
    }
  }
  data.resize(width);  
  if(diff > 0) {
    for(int i = 0; i < diff; i++) {
      utils::rshift(&p_data[0], 0);  
    }
  }
}

void display_meminfo_win(window *bottomwin, int max_x, mem memory, float x1_val, float x2_val) {  
  std::vector<std::string> mem_tags{"total", "free", "available", "buffered", "cached"};
  std::string mem_info="Memory Info";
  int y=1;
  wattron(bottomwin->get_win(), A_STANDOUT);
  mvwprintw(bottomwin->get_win(),y, 0, "%s", mem_info.c_str());
  wattroff(bottomwin->get_win(), A_STANDOUT);
  mvwhline(bottomwin->get_win(), y, mem_info.length(), ACS_HLINE, max_x-13);
  ++y;
  mvwprintw(bottomwin->get_win(), y, x1_val, "%s", std::string("RAM").c_str());
  mvwprintw(bottomwin->get_win(), y, x2_val, "%s", std::string("SWAP").c_str());
  y=3;
  mvwprintw(bottomwin->get_win(),y++,1, "%s", mem_tags.at(0).c_str());
  mvwprintw(bottomwin->get_win(),y++,1, "%s", mem_tags.at(1).c_str());
  mvwprintw(bottomwin->get_win(),y++,1, "%s", mem_tags.at(2).c_str());
  mvwprintw(bottomwin->get_win(),y++,1, "%s", mem_tags.at(3).c_str());
  mvwprintw(bottomwin->get_win(),y++,1, "%s", mem_tags.at(4).c_str());
  y=3;
  mvwprintw(bottomwin->get_win(),y++,x1_val, "%d", memory.mem_total);
  mvwprintw(bottomwin->get_win(),y++,x1_val, "%d", memory.mem_free);
  mvwprintw(bottomwin->get_win(),y++,x1_val, "%d", memory.mem_available);
  mvwprintw(bottomwin->get_win(),y++,x1_val, "%d", memory.mem_buffered);
  mvwprintw(bottomwin->get_win(),y++,x1_val, "%d", memory.mem_cached);
  y=3;
  mvwprintw(bottomwin->get_win(),y++,x2_val, "%d", memory.swap_total);
  mvwprintw(bottomwin->get_win(),y++,x2_val, "%d", memory.swap_free);
 
  for (int i=3; i<=7; ++i)
    mvwprintw(bottomwin->get_win(),i,
	      x2_val+std::string("SWAP").length()+7,
	      "%s", std::string("kB").c_str());
  
  wattron(bottomwin->get_win(), A_STANDOUT);
  mvwprintw(bottomwin->get_win(),bottomwin->get_frame().height-1, 0, "%s",
	    std::string("Network Stats").c_str());
  wattroff(bottomwin->get_win(), A_STANDOUT);
  mvwhline(bottomwin->get_win(), bottomwin->get_frame().height-1,
	   std::string("Network Stats").length(), ACS_HLINE, max_x-15);
  bottomwin->refresh_window();

  for (int y=3; y<6; y+=2)
      mvwhline(bottomwin->get_win(), y, bottomwin->get_frame().width/2, ACS_HLINE,
	       (bottomwin->get_frame().width/2)-1);
}

void display_net_rates(window *win, plotter<int> rx_plotter, plotter<int> tx_plotter) {
  wattron(win->get_win(), COLOR_PAIR(7));
  mvwprintw(win->get_win(), 10, 1, "max (KiB/s)");
  wattroff(win->get_win(), COLOR_PAIR(7));

  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(), 10, (win->get_frame().width/4)+1,
	    "%d", rx_plotter.get_max_val()); 
  wattroff(win->get_win(), COLOR_PAIR(1));

  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 10, (win->get_frame().width/2)+3, "%d",
	    tx_plotter.get_max_val());
  wattroff(win->get_win(), COLOR_PAIR(3));
}

void menu(window *win, std::vector<std::string> &ifaces, int sel_item) {
  std::string network_ifaces = "Interface: ";
  wattron(win->get_win(), A_STANDOUT);
  mvwprintw(win->get_win(), 12, 1, network_ifaces.c_str());
  wattroff(win->get_win(), A_STANDOUT);

  for (auto &entry : exfs::directory_iterator("/sys/class/net"))
    ifaces.push_back(entry.path().stem());

  int acc = network_ifaces.length();
  for (size_t i = 0; i<ifaces.size(); ++i) {
    int iface_len = ifaces.at(i).length();
    wattron(win->get_win(), A_INVIS);
    mvwaddstr(win->get_win(), 12, acc, ifaces.at(i).c_str());
    wattroff(win->get_win(),A_INVIS);
    if ((int)i==sel_item) {
      wattron(win->get_win(),A_STANDOUT);
      mvwaddstr(win->get_win(), 12, network_ifaces.length(), ifaces.at(i).c_str());
      wattroff(win->get_win(),A_STANDOUT);
    }
    acc+=i+iface_len;
  }
  wrefresh(win->get_win());
}

void display_net_win(int rx_tx_choice, window *stats_win, window *graph_win, plotter<int> &rx_plotter,
		     plotter<int> &tx_plotter, net &netiface, net_rates &netiface_rates) {
  wclear(stats_win->get_win());
  wrefresh(stats_win->get_win());
  rx_plotter.draw_net_stats(stats_win, netiface);
  rx_plotter.draw_net_rates(stats_win, netiface_rates.rx_rate, netiface_rates.rx_rate);
  stats_win->refresh_window();
  if (rx_tx_choice==1) 
    rx_plotter.draw_net_graph(graph_win, graph_win->get_frame().height-1, COLOR_PAIR(1));
  else
    tx_plotter.draw_net_graph(graph_win, graph_win->get_frame().height-1, COLOR_PAIR(3));
  graph_win->refresh_window();  
  display_net_rates(stats_win, rx_plotter, tx_plotter);
}

void update_net_stats_win(window *stats_win, window *graph_win,
			  plotter<int> &rx_plotter, plotter<int> &tx_plotter) {
  rx_plotter.clear();
  tx_plotter.clear();
  wclear(stats_win->get_win());
  wclear(graph_win->get_win());
  wrefresh(stats_win->get_win());
  wrefresh(graph_win->get_win());
}

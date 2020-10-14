#include "plotter.h"
#include <stdlib.h>
#include <ncurses.h>
#include "window.h"
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "utils.h"
#include <bits/stdc++.h>

template <class E>
plotter<E>::plotter()=default;

template <class E>
plotter<E>::plotter(int num_vals, char graph_char) {
  this->num_vals=num_vals;
  this->graph_char = graph_char;
  max_val = 0;
  data = std::vector<E>(num_vals, static_cast<E>(0));
  data.resize(num_vals);
}

template <class E>
void plotter<E>::draw_cpu_usage_graph(window *win, int cyan, int blue, int magenta) {
  std::vector<int> height_pct;
  int height = win->get_frame().height;
  int width = win->get_frame().width;
  WINDOW *window = win->get_win();

  for(int i=1; i<4; ++i)
    height_pct.push_back(static_cast<int>((float) height - (float) height * (25.0*i)/100.0));

  for (int j=1; j<10; ++j)
    mvwvline(win->get_win(), j, 0, ACS_HLINE, 10);

  for(int x = 1; x < width-1; ++x) {
    int data_point = std::abs(static_cast<int>((height * data.at(x)/100.0) - height));
    for(int y = 1; y < height-1; ++y) {
      if (y >= data_point) {
	if (data_point >= height_pct.at(0)) {
	  wattron(window,cyan);
	  mvwaddch(window, y, x, '*');
	  wattroff(window,cyan);
	} else if (data_point < height_pct.at(0) && data_point >= height_pct.at(1)) {
	  if (y < height_pct.at(0) && data_point >= height_pct.at(1)) {
	  wattron(window,blue);
	  mvwaddch(window, y, x, '*');
	  wattroff(window,blue);
	  } else {
	    wattron(window,cyan);
	    mvwaddch(window, y, x, '*');
	    wattroff(window,cyan);
	  }
	} else if (data_point < height_pct.at(1)) {
	  if (y < height_pct.at(1)) {
	    wattron(window,magenta);
	    mvwaddch(window, y, x, '*');
	    wattroff(window,magenta);
	  } else if (y >= height_pct.at(1) && y < height_pct.at(0)) {
	    wattron(window,blue);
	    mvwaddch(window, y, x, '*');
	    wattroff(window,blue);
          } else {
	    wattron(window,cyan);
	    mvwaddch(window, y, x, '*');
	    wattroff(window,cyan);
	  }
	}
      } else {
	mvwaddch(window, y, x, ' ');
      }
    }
  }
}


template <class E>
void plotter<E>::draw_progress_bar(float value, std::string str, window *win,
				   int x, int y, int blue) {
  int max_bars = win->get_frame().width/3;
  float data_point=value;
  std::stringstream stream{str+" [", std::ios_base::app | std::ios_base::out};
  stream << std::fixed << std::setprecision(2) << data_point;
  std::string output_bars=""; 
  mvwprintw(win->get_win(), y, x, "%s", stream.str().c_str());
  mvwprintw(win->get_win(), y, stream.str().length()+x,"%]");
  float bars = data_point * max_bars/100;
  for (int i=0; i<max_bars; ++i) {
    if (i<=bars) output_bars+='|';
    else output_bars+=' ';
  }
  
  wattron(win->get_win(), blue);
  mvwprintw(win->get_win(), y, stream.str().length()+x+std::string("%]").length(),
	    output_bars.c_str());
  wattroff(win->get_win(), blue);
}

template <class E>
void plotter<E>::draw_cpu_stats_block(window *win, cpu_meter cpumeter) {
  std::vector<std::string> cpu_tags = {"user", "sys", "nice","idle", "active"};
  int col1_pad = 5;
  int col2_pad = col1_pad+cpu_tags.at(0).length()+1;
  int col3_pad = col2_pad+cpu_tags.at(1).length()+1;
  int col4_pad = col3_pad+cpu_tags.at(2).length()+1;
  int col5_pad = col4_pad+cpu_tags.at(3).length()+1;
  for (int i=6; i<=12; ++i) {
    mvwprintw(win->get_win(),i,1,"%d",(i-4));
    mvwprintw(win->get_win(),i,col5_pad+6, "%s", "%");
  }

  mvwprintw(win->get_win(), 5,col1_pad, "%s", cpu_tags.at(0).c_str());
  mvwprintw(win->get_win(), 5,col2_pad, "%s", cpu_tags.at(1).c_str());
  mvwprintw(win->get_win(), 5,col3_pad, "%s", cpu_tags.at(2).c_str());
  mvwprintw(win->get_win(), 5,col4_pad, "%s", cpu_tags.at(3).c_str());
  mvwprintw(win->get_win(), 5,col5_pad, "%s", cpu_tags.at(4).c_str());
  
  for (size_t j=0; j<cpumeter.usage_pct.size(); ++j) {
    cpu_pct pct = cpumeter.usage_pct.at(j);
    mvwprintw(win->get_win(), 6+j,col1_pad, "%.1f ", pct.user);
    mvwprintw(win->get_win(), 6+j,col2_pad, "%.1f ", pct.system);
    mvwprintw(win->get_win(), 6+j,col3_pad, "%.1f ", pct.nice);
    mvwprintw(win->get_win(), 6+j,col5_pad, "%.1f ", pct.idle);
    mvwprintw(win->get_win(), 6+j,col4_pad, "%.1f ", pct.active);
  }

  mvwprintw(win->get_win(), 13, 1, "%s", "Avg");

  float avg_user, avg_system, avg_nice, avg_active, avg_idle;
  avg_user = avg_system = avg_nice =  avg_active = avg_idle = 0.0f;
  for (auto it=cpumeter.usage_pct.begin();
       it<cpumeter.usage_pct.end(); ++it) {
    avg_user+=(*it).user;
    avg_system+=(*it).system;
    avg_nice+=(*it).nice;
    avg_idle+=(*it).idle;
    avg_active+=(*it).active;
  }
  avg_user/=8; avg_system/=8; avg_nice/=8;
  avg_active/=8; avg_idle/=8;

  mvwprintw(win->get_win(), 13,col1_pad, "%.1f ", avg_user);
  mvwprintw(win->get_win(), 13,col2_pad, "%.1f ", avg_system);
  mvwprintw(win->get_win(), 13,col3_pad, "%.1f ", avg_nice);
  mvwprintw(win->get_win(), 13,col4_pad, "%.1f ", avg_active);
  mvwprintw(win->get_win(), 13,col5_pad, "%.1f ", avg_idle);
  mvwprintw(win->get_win(), 13,col5_pad+6, "%s", "%");
}

template <class E>
void plotter<E>::draw_legend_box(window *win, int cyan, int blue, int magenta) {
  std::string legend = "Legend";
  std::string str_cyan = "cyan";
  std::string str_blue = "blue";
  std::string str_magenta = "magenta";
  mvwprintw(win->get_win(), win->get_frame().height-4, 1, "%s", legend.c_str());
  wattron(win->get_win(), cyan);
  mvwprintw(win->get_win(), win->get_frame().height-3, 4, "%s", str_cyan.c_str());
  mvwprintw(win->get_win(), win->get_frame().height-2, 4, "%s", std::string("0-25%").c_str());
  wattroff(win->get_win(), cyan);
  wattron(win->get_win(), blue);
  mvwprintw(win->get_win(), win->get_frame().height-3, 10, "%s", str_blue.c_str());
  mvwprintw(win->get_win(), win->get_frame().height-2, 10, "%s", std::string("25-75%").c_str());
  wattroff(win->get_win(), blue);
  wattron(win->get_win(), magenta);
  mvwprintw(win->get_win(), win->get_frame().height-3, 17, "%s", str_magenta.c_str());
  mvwprintw(win->get_win(), win->get_frame().height-2, 17, "%s", std::string("75-100%").c_str());
  wattroff(win->get_win(), magenta);
  wrefresh(win->get_win());
}

template <class E>
void plotter<E>::draw_net_stats(window *win, net netw) {
  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(), 1, win->get_frame().width/4+1, "RX");
  wattroff(win->get_win(), COLOR_PAIR(1));

  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 1, (win->get_frame().width/2)+3, "TX");
  wattroff(win->get_win(), COLOR_PAIR(3));
  
  mvwprintw(win->get_win(), 2, 1, "bytes");
  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(), 2, win->get_frame().width/4+1,
	    std::to_string(netw.rx_bytes).c_str());
  wattroff(win->get_win(), COLOR_PAIR(1));

  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 2, (win->get_frame().width/2)+3,
	    std::to_string(netw.tx_bytes).c_str());
  wattroff(win->get_win(), COLOR_PAIR(3));

  mvwprintw(win->get_win(), 3, 1, "packets");

  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(),3, win->get_frame().width/4+1,
	    std::to_string(netw.rx_packets).c_str());
  wattroff(win->get_win(), COLOR_PAIR(1));

  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 3, (win->get_frame().width/2)+3,
	    std::to_string(netw.tx_packets).c_str());
  wattroff(win->get_win(), COLOR_PAIR(3));

  mvwprintw(win->get_win(), 4, 1, "errors");

  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(), 4, (win->get_frame().width/4)+1,
	    std::to_string(netw.rx_errors).c_str());
  wattroff(win->get_win(), COLOR_PAIR(1));

  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(),4, (win->get_frame().width/2)+3,
	    std::to_string(netw.rx_errors).c_str());
  wattroff(win->get_win(), COLOR_PAIR(3));

  mvwprintw(win->get_win(), 5, 1, "dropped");

  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(),5,(win->get_frame().width/4)+1,
	    std::to_string(netw.rx_dropped).c_str());
  wattroff(win->get_win(), COLOR_PAIR(1));

  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 5, (win->get_frame().width/2)+3,
	    std::to_string(netw.tx_dropped).c_str());
  wattroff(win->get_win(), COLOR_PAIR(3));

  mvwprintw(win->get_win(), 6, 1, "fifo errors");

  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(), 6, (win->get_frame().width/4)+1,
	    std::to_string(netw.rx_fifo_errors).c_str());
  wattroff(win->get_win(), COLOR_PAIR(1));

  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 6, (win->get_frame().width/2)+3,
	    std::to_string(netw.tx_fifo_errors).c_str());
  wattroff(win->get_win(), COLOR_PAIR(3));

  mvwprintw(win->get_win(), 7, 1, "compressed");

  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(), 7, (win->get_frame().width/4)+1,
	    std::to_string(netw.rx_compressed).c_str());
  wattroff(win->get_win(), COLOR_PAIR(1));
  
  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 7, (win->get_frame().width/2)+3,
	    std::to_string(netw.tx_compressed).c_str());
  wattroff(win->get_win(), COLOR_PAIR(3));  
}

template <class E>
void plotter<E>::draw_net_graph(window *win, int height, int green) {
  for (int j=1; j<win->get_frame().height; ++j)
    mvwvline(win->get_win(), j, 0, ACS_HLINE, win->get_frame().height);

  for (int j = 1; j < height; j++) {
    for (int i = 1; i < num_vals-1; i++) {
      char graph_char = get_graph_char(data.at(i), j, height);
      wattron(win->get_win(), green);
      mvwaddch(win->get_win(), height-j,
	       i, NCURSES_ACS(graph_char));
      wattroff(win->get_win(), green);
    }
  }
}

template <class E>
void plotter<E>::draw_net_rates(window *win, float rx_rate, float tx_rate) {
  mvwprintw(win->get_win(), 9, 1,  "rate");
  std::stringstream stream;
  std::string rx_r, tx_r;
  stream.precision(2);
  stream << std::fixed << rx_rate;
  stream >> rx_r;
  std::stringstream().swap(stream);
  stream.precision(2);
  stream << std::fixed << tx_rate;
  stream >> tx_r;
  wattron(win->get_win(), COLOR_PAIR(1));
  mvwprintw(win->get_win(),9,(win->get_frame().width/4)+1, "%s", rx_r.c_str());
  wattroff(win->get_win(), COLOR_PAIR(1));
  wattron(win->get_win(), COLOR_PAIR(3));
  mvwprintw(win->get_win(), 9, (win->get_frame().width/2)+3, "%s", tx_r.c_str());
  wattron(win->get_win(), COLOR_PAIR(3));
}

template <class E>
char plotter<E>::get_graph_char(E val, int vert_pos, int vert_height) {
  char result;
  float tick = ((float)vert_height) / max_val;
  float bar_height = val / tick;    
  if ((vert_pos - bar_height) > 0) 
    result = ' ';
  else 
    result = graph_char;
  return result;
}

template <class E>
void plotter<E>::lshift(E val) {
  data.at(num_vals-1) = val;    
  for (int i=0; i<num_vals-1; i++) 
    data.at(i)=data.at(i+1);    
  if (max_val < val) 
    max_val = val;
}

template <class E>
int plotter<E>::get_max_val() {
  return max_val;
}

template <class E>
void plotter<E>::clear() {
  std::fill(data.begin(), data.end(), static_cast<E>(0));
}

template class plotter<int>;
template class plotter<float>;

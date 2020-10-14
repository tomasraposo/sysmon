#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include "window.h"
#include "utils.h"

window::window()=default;
window::window(int x, int y, int maxx, int maxy) {
  frame w_frame{x, y, maxx, maxy};
  win = newwin(w_frame.height,w_frame.width,w_frame.y,w_frame.x);
  win_frame = w_frame;
}

window::~window(){
  delwin(win);
}

void window::resize_window(std::vector<float> &data) {
  auto p_data = &data;
  int maxx, maxy;
  getmaxyx(stdscr,maxy,maxx);
  maxx = (maxx-1)>>1; maxy = (maxy-1)>>1;
  frame w_frame{1, 1, maxx, maxy};
  
  int diff;
  wresize(win, w_frame.height, w_frame.width);
  mvwin(win, w_frame.y, w_frame.x);
  wclear(win);
  
   diff = w_frame.width - data.size();
  
   if(diff < 0)
     for(int i = 0; i < std::abs(diff); i++)
       utils::lshift(p_data, 0);
  
   data.resize(w_frame.width);
  
   if(diff > 0)
     for(int i = 0; i < diff; i++)
       utils::rshift(&p_data[0], 0);  
}

void window::refresh_window() {
  wrefresh(win);
}

WINDOW* window::get_win() const {
  return win;
}

frame window::get_frame() const {
  return win_frame;
}

// std::vector<float>* window::get_data() const {
//   return data;
// }

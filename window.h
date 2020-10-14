#ifndef WINDOW_H
#define WINDOW_H
#include "frame.h"
#include <ncurses.h>
#include <vector>

class window {
private:
  WINDOW* win;
  frame win_frame;
public:
  window();
  window(int x, int y, int maxx, int maxy);
  ~window();
  void resize_window(std::vector<float> &data);
  void refresh_window();
  WINDOW* get_win() const;
  frame get_frame() const;
};
#endif

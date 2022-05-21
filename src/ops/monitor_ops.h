//
// Created by castle on 3/24/22.
//

#ifndef CASTLE_DWM_MONITOR_OPS_H
#define CASTLE_DWM_MONITOR_OPS_H

#include "../state/dwm_types.h"

namespace ops::monitor {
  void arrange(monitor_t *m);
  void arrange_mon(monitor_t* monitor);
  void load_default_layout(monitor_t* monitor);
  void restack(monitor_t* monitor);
  void update_bar_pos(monitor_t* monitor);
  monitor_t *create_mon();
  void cleanup_mon(monitor_t *monitor);
  monitor_t *dir_to_mon(int dir);
  monitor_t *rect_to_mon(int x, int y, int w, int h);
  monitor_t *win_to_mon(Window w);
  
  namespace bar {
//    void draw_all_bars();
//    int draw_statusbar(monitor_t* monitor, int bar_height, char *stext);
//    void draw_bar(monitor_t* monitor);
  }
}


#endif //CASTLE_DWM_MONITOR_OPS_H

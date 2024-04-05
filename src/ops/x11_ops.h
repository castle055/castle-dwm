//
// Created by castle on 3/25/22.
//

#ifndef CASTLE_DWM_X11_OPS_H
#define CASTLE_DWM_X11_OPS_H

#include <string>
#include "X11/Xlib.h"

namespace ops::x11 {
  void check_other_wm();
  int x_error(Display *dpy, XErrorEvent *ee);
  int x_error_dummy(Display *dpy, XErrorEvent *ee);
  int x_error_start(Display *dpy, XErrorEvent *ee);
  int get_root_ptr(int *x, int *y);
  long get_state(Window w);
  int get_text_prop(Window w, Atom atom, std::string& text);
  //Window create_barwin(int x, int y, int w);
  //void destroy_barwin(Window barwin);
  void update_client_list();
  int update_geometry();
  void update_numlock_mask();
  //void update_status();
  void grab_keys();
  void scan();
}


#endif //CASTLE_DWM_X11_OPS_H

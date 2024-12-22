//
// Created by castle on 3/24/22.
//

module;
#include "../drw.h"
export module cyd_wm.state.misc;

import std;
import x11.types;
import cyd_wm.types;
// #include "dwm_types.h"

export namespace state {
  std::fstream log_file;
  std::fstream key_nav_file;
  
  std::string stext;
  
  Drw *drw;
  x11::window_t wmcheckwin;
  Clr **scheme;
  int sw, sh, bar_height, blw;
  int lrpad;
  unsigned int numlockmask;
  Cur *cursor[CurLast];
  
  bool running;
}

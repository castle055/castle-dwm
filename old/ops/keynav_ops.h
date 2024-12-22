//
// Created by castle on 5/19/22.
//

#ifndef CASTLE_DWM_KEYNAV_OPS_H
#define CASTLE_DWM_KEYNAV_OPS_H


#include <X11/Xlib.h>
#include "../state/dwm_types.h"

namespace ops::keynav {
  extern key_nav_target root;
  void reset_map();
  void reset();
  bool process(XKeyEvent* ev);
}


#endif //CASTLE_DWM_KEYNAV_OPS_H

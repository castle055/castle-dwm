/*! \file  focusin.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
export module cyd_wm.events.handlers.focusin;

import x11;
import cyd_wm.types;

import cyd_wm.state.monitors;

import cyd_wm.clients.focus;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void focusin(x11::event_t* e) {
    XFocusChangeEvent* ev = &e->xfocus;

    if (state::monitors::selected_monitor->sel && ev->window != state::monitors::selected_monitor->sel->win)
      clients::set_focus(state::monitors::selected_monitor->sel);
  }
}
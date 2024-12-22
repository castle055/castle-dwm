/*! \file  expose.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
export module cyd_wm.events.handlers.expose;

import x11;
import cyd_wm.types;

import cyd_wm.statusbar;
import cyd_wm.monitors.manager;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void expose(x11::event_t* e) {
    Monitor::sptr m;
    XExposeEvent* ev = &e->xexpose;

    if (ev->count == 0 && (m = monitors::from_window(ev->window)))
      statusbar::update_all();
  }
}
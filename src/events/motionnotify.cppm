/*! \file  motionnotify.cppm
 *! \brief 
 *!
 */

export module cyd_wm.events.handlers.motionnotify;

import x11;
import cyd_wm.types;

import cyd_wm.state.monitors;

import cyd_wm.monitors.manager;
import cyd_wm.clients.focus;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void motionnotify(x11::event_t* e) {
    static Monitor::sptr mon = nullptr;
    Monitor::sptr m;
    x11::XMotionEvent* ev = &e->xmotion;

    if (ev->window != x11::connection::root())
      return;
    if ((m = monitors::from_rectangle(ev->x_root, ev->y_root, 1, 1)) != mon && mon) {
      clients::unfocus(state::monitors::selected_monitor->sel, 1);
      state::monitors::selected_monitor = m;
      clients::focus(nullptr);
    }
    mon = m;
  }
}
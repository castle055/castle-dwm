/*! \file  enternotify.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
export module cyd_wm.events.handlers.enternotify;

import x11;
import cyd_wm.types;

import cyd_wm.state.monitors;

import cyd_wm.clients.manager;
import cyd_wm.clients.focus;
import cyd_wm.monitors.manager;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void enternotify(x11::event_t* e) {
    Client::sptr c;
    Monitor::sptr m;
    x11::XCrossingEvent* ev = &e->xcrossing;

    if ((ev->mode != NotifyNormal || ev->detail == NotifyInferior) && ev->window != x11::connection::root())
      return;
    c = clients::from_window(ev->window);
    m = c ? c->mon : monitors::from_window(ev->window);
    if (m != state::monitors::selected_monitor) {
      clients::unfocus(state::monitors::selected_monitor->sel, 1);
      state::monitors::selected_monitor = m;
    } else if (!c || c == state::monitors::selected_monitor->sel)
      return;
    clients::focus(c);
  }
}
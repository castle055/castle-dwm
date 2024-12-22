/*! \file  unmapnotify.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
export module cyd_wm.events.handlers.unmapnotify;

import x11;
import cyd_wm.types;

import cyd_wm.clients.manager;
import cyd_wm.clients.utils;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void unmapnotify(x11::event_t* e) {
    Client::sptr c;
    XUnmapEvent* ev = &e->xunmap;

    if ((c = clients::from_window(ev->window))) {
      if (ev->send_event)
        clients::set_state(c, WithdrawnState);
      else
        clients::unmanage(c, 0);
    }
  }
}
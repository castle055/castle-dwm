/*! \file  destroynotify.cppm
 *! \brief 
 *!
 */

export module cyd_wm.events.handlers.destroynotify;

import x11;
import cyd_wm.types;

import cyd_wm.clients.manager;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void destroynotify(x11::event_t* e) {
    Client::sptr c;
    x11::XDestroyWindowEvent* ev = &e->xdestroywindow;

    if ((c = clients::from_window(ev->window)))
      clients::unmanage(c, true);
  }
}
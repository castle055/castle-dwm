/*! \file  maprequest.cppm
 *! \brief 
 *!
 */

export module cyd_wm.events.handlers.maprequest;

import x11;
import cyd_wm.types;

import cyd_wm.clients.manager;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void maprequest(x11::event_t* e) {
    static x11::window_attrs_t wa;
    x11::XMapRequestEvent* ev = &e->xmaprequest;

    if (!x11::get_window_attributes(ev->window, &wa))
      return;
    if (wa.override_redirect)
      return;
    if (!clients::from_window(ev->window))
      clients::make_client(ev->window, &wa, 1);
  }
}
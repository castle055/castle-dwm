/*! \file  propertynotify.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
#include <X11/Xatom.h>
export module cyd_wm.events.handlers.propertynotify;

import x11;
import cyd_wm.types;

import cyd_wm.state.atoms;

import cyd_wm.clients.manager;
import cyd_wm.clients.updates;
import cyd_wm.layouts;
import cyd_wm.statusbar;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void propertynotify(x11::event_t* e) {
    Client::sptr c;
    x11::window_t trans;
    x11::XPropertyEvent* ev = &e->xproperty;

    if ((ev->window == x11::connection::root()) && (ev->atom == XA_WM_NAME))
      statusbar::update_all();
    else if (ev->state == PropertyDelete)
      return; /* ignore */
    else if ((c = clients::from_window(ev->window))) {
      switch (ev->atom) {
        default:
          break;
        case XA_WM_TRANSIENT_FOR:
          if (!c->ignoretransient && !c->isfloating && (x11::get_transient_for_hint(c->win, &trans)) &&
              (c->isfloating = (clients::from_window(trans)) != NULL))
            layouts::arrange(c->mon);
          break;
        case XA_WM_NORMAL_HINTS:
          clients::update_size_hints(c);
          break;
        case XA_WM_HINTS:
          clients::update_wm_hints(c);
          statusbar::update_all();
          break;
      }
      if (ev->atom == XA_WM_NAME || ev->atom == state::atoms::netatom[NetWMName]) {
        clients::update_title(c);
        if (c == c->mon->sel)
          statusbar::update_all();
      }
      if (ev->atom == state::atoms::netatom[NetWMWindowType])
        clients::update_window_type(c);
    }
  }
}
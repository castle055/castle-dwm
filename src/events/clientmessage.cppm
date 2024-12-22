/*! \file  clientmessage.cppm
 *! \brief 
 *!
 */

export module cyd_wm.events.handlers.clientmessage;

import x11;
import cyd_wm.types;

import cyd_wm.state.monitors;
import cyd_wm.state.atoms;

import cyd_wm.clients.manager;
import cyd_wm.clients.utils;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void clientmessage(x11::event_t* e) {
    x11::XClientMessageEvent* cme = &e->xclient;
    const Client::sptr c          = clients::from_window(cme->window);

    if (!c)
      return;
    if (cme->message_type == state::atoms::netatom[NetWMState]) {
      if (cme->data.l[1] == state::atoms::netatom[NetWMFullscreen]
          || cme->data.l[2] == state::atoms::netatom[NetWMFullscreen])
        c->set_fullscreen((cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
                           || (cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */ && !c->isfullscreen)));
    } else if (cme->message_type == state::atoms::netatom[NetActiveWindow]) {
      if (c != state::monitors::selected_monitor->sel && !c->isurgent)
        c->set_urgent(1);
    }
  }
}
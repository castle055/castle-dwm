/*! \file  configurenotify.cppm
 *! \brief 
 *!
 */

export module cyd_wm.events.handlers.configurenotify;

import x11;
import cyd_wm.types;

import cyd_wm.state.misc;
import cyd_wm.state.monitors;

import cyd_wm.clients.focus;
import cyd_wm.clients.utils;
import cyd_wm.layouts;
import cyd_wm.statusbar;
import cyd_wm.x11_utils;
import cyd_wm.drw;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void configurenotify(::x11::event_t* e) {
    Monitor::sptr m;
    Client::sptr c;
    ::x11::XConfigureEvent* ev = &e->xconfigure;
    int dirty;

    /* TODO: update_geometry handling sucks, needs to be simplified */
    if (ev->window == ::x11::connection::root()) {
      dirty     = (state::sw != ev->width || state::sh != ev->height);
      state::sw = ev->width;
      state::sh = ev->height;
      if (cyd_wm::x11::update_geometry() || dirty) {
        drw_resize(state::drw, state::sw, state::bar_height);
        statusbar::init_where_needed();
        for (const auto & monitor : state::monitors::monitors) {
          for (const auto & client : monitor->clients) {
            if (client->isfullscreen)
              clients::resize_client(c, m->mx, m->my, m->mw, m->mh);
          }
          ::x11::move_resize_window(m->barwin, m->wx, m->by, m->ww, state::bar_height);
        }
        clients::focus(nullptr);
        layouts::arrange(nullptr);
      }
    }
  }
}
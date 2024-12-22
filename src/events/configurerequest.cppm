/*! \file  configurerequest.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
#define ISVISIBLE(C)            (((C)->tags & (C)->mon->tagset[(C)->mon->seltags]))
export module cyd_wm.events.handlers.configurerequest;

import x11;
import cyd_wm.types;

import cyd_wm.state.monitors;

import cyd_wm.clients.manager;
import cyd_wm.clients.utils;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void configurerequest(x11::event_t* e) {
    Client::sptr c;
    Monitor::sptr m;
    XConfigureRequestEvent* ev = &e->xconfigurerequest;
    XWindowChanges wc;

    if ((c = clients::from_window(ev->window))) {
      if (ev->value_mask & CWBorderWidth)
        c->bw = ev->border_width;
      else if (c->isfloating || !state::monitors::selected_monitor->lt[state::monitors::selected_monitor->sellt]->arrange) {
        m = c->mon;
        if (ev->value_mask & CWX) {
          c->oldx = c->x;
          c->x    = m->mx + ev->x;
        }
        if (ev->value_mask & CWY) {
          c->oldy = c->y;
          c->y    = m->my + ev->y;
        }
        if (ev->value_mask & CWWidth) {
          c->oldw = c->w;
          c->w    = ev->width;
        }
        if (ev->value_mask & CWHeight) {
          c->oldh = c->h;
          c->h    = ev->height;
        }
        if ((c->x + c->w) > m->mx + m->mw && c->isfloating)
          c->x = m->mx + (m->mw / 2 - c->width() / 2); /* center in x direction */
        if ((c->y + c->h) > m->my + m->mh && c->isfloating)
          c->y = m->my + (m->mh / 2 - c->height() / 2); /* center in y direction */
        if ((ev->value_mask & (CWX | CWY)) && !(ev->value_mask & (CWWidth | CWHeight)))
          clients::configure(c);
        if (ISVISIBLE(c))
          x11::move_resize_window(c->win, c->x, c->y, c->w, c->h);
      } else
        clients::configure(c);
    } else {
      wc.x            = ev->x;
      wc.y            = ev->y;
      wc.width        = ev->width;
      wc.height       = ev->height;
      wc.border_width = ev->border_width;
      wc.sibling      = ev->above;
      wc.stack_mode   = ev->detail;
      x11::configure_window(ev->window, ev->value_mask, &wc);
    }
    x11::sync(false);
  }
}
/*! \file  clients.cppm
 *! \brief 
 *!
 */

module;
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
export module cyd_wm.clients.utils;

import std;

import cyd_wm.types;
import cyd_wm.state.monitors;
import cyd_wm.state.atoms;
import cyd_wm.state.config;
import cyd_wm.state.misc;
import cyd_wm.statusbar;
import cyd_wm.monitors.stack;
import cyd_wm.monitors.manager;
import cyd_wm.layouts;
import cyd_wm.clients.focus;

import x11;
import x11.api;

using namespace cyd_wm;

export namespace cyd_wm::clients {
  //===== client_t Ops implementations =====
  int apply_size_hints(Client::sptr client, int* X, int* Y, int* W, int* H, int interact) {
    int baseismin;
    Monitor::sptr m = client->mon;

    /* set minimum possible */
    *W = std::max(1, *W);
    *H = std::max(1, *H);
    if (interact) {
      if (*X > state::sw)
        *X = state::sw - client->width();
      if (*Y > state::sh)
        *Y = state::sh - client->height();
      if (*X + *W + 2 * client->bw < 0)
        *X = 0;
      if (*Y + *H + 2 * client->bw < 0)
        *Y = 0;
    } else {
      if (*X >= m->wx + m->ww)
        *X = m->wx + m->ww - client->width();
      if (*Y >= m->wy + m->wh)
        *Y = m->wy + m->wh - client->height();
      if (*X + *W + 2 * client->bw <= m->wx)
        *X = m->wx;
      if (*Y + *H + 2 * client->bw <= m->wy)
        *Y = m->wy;
    }
    if (*H < state::bar_height)
      *H = state::bar_height;
    if (*W < state::bar_height)
      *W = state::bar_height;
    if (state::config::resizehints || client->isfloating || !client->mon->lt[client->mon->sellt]->arrange) {
      /* see last two sentences in ICCCM 4.1.2.3 */
      baseismin = client->basew == client->minw && client->baseh == client->minh;
      if (!baseismin) {
        /* temporarily remove base dimensions */
        *W -= client->basew;
        *H -= client->baseh;
      }
      /* adjust for aspect limits */
      if (client->mina > 0 && client->maxa > 0) {
        if (client->maxa < (float)*W / (float)*H)
          *W = (int)std::lround((float)*H * client->maxa + 0.5);
        else if (client->mina < (float)*H / (float)*W)
          *H = (int)std::lround((float)*W * client->mina + 0.5);
      }
      if (baseismin) {
        /* increment calculation requires this */
        *W -= client->basew;
        *H -= client->baseh;
      }
      /* adjust for increment value */
      if (client->incw)
        *W -= *W % client->incw;
      if (client->inch)
        *H -= *H % client->inch;
      /* restore base dimensions */
      *W = std::max(*W + client->basew, client->minw);
      *H = std::max(*H + client->baseh, client->minh);
      if (client->maxw)
        *W = std::min(*W, client->maxw);
      if (client->maxh)
        *H = std::min(*H, client->maxh);
    }
    return *X != client->x || *Y != client->y || *W != client->w || *H != client->h;
  }

  void grab_buttons(Client::sptr client, int focused) {
    x11::update_numlock_mask(); {
      std::array<unsigned int, 4> modifiers = {0, LockMask, state::numlockmask, state::numlockmask | LockMask};
      x11::ungrab_button(AnyButton, AnyModifier, client->win);
      if (!focused)
        x11::grab_button(AnyButton, AnyModifier, client->win, false,
                         BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
      for (std::size_t i = 0; i < state::config::buttons.size(); i++)
        if (state::config::buttons[i].click == ClkClientWin)
          for (std::size_t j = 0; j < modifiers.size(); j++)
            x11::grab_button(state::config::buttons[i].button,
                             state::config::buttons[i].mask | modifiers[j],
                             client->win, false, BUTTONMASK,
                             GrabModeAsync, GrabModeSync, None, None);
    }
  }


  void pop(Client::sptr client) {
    // TODO - Before refactoring this used to 'bring to front' in the `clients` list, not the `stack` as it is now
    // I thought this made more sense and want to experiment on it!
    monitors::bring_to_front(client);
    clients::focus(client);
    layouts::arrange(client->mon);
  }

  void configure(Client::sptr client) {
    x11::configure_event_t ce {x11::connection::display()};

    ce.event             = client->win;
    ce.window            = client->win;
    ce.x                 = client->x;
    ce.y                 = client->y;
    ce.width             = client->w;
    ce.height            = client->h;
    ce.border_width      = client->bw;
    ce.above             = None;
    ce.override_redirect = false;
    x11::send_event(client->win, false, StructureNotifyMask, reinterpret_cast<x11::event_t*>(&ce));
  }

  void resize_client(Client::sptr client, int X, int Y, int W, int H) {
    x11::window_changes_t wc;

    client->oldx    = client->x;
    client->x       = wc.x = X;
    client->oldy    = client->y;
    client->y       = wc.y = Y;
    client->oldw    = client->w;
    client->w       = wc.width = W;
    client->oldh    = client->h;
    client->h       = wc.height = H;
    wc.border_width = client->bw;
    x11::configure_window(client->win, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &wc);
    configure(client);
    x11::sync();
  }

  void resize(Client::sptr client, int X, int Y, int W, int H, int interact) {
    if (apply_size_hints(client, &X, &Y, &W, &H, interact))
      resize_client(client, X, Y, W, H);
  }

  void send_mon(Client::sptr c, Monitor::sptr m) {
    if (c->mon == m)
      return;
    clients::unfocus(c, 1);
    monitors::detach_from_stack(c);
    monitors::detach_client(c->mon, c);
    c->mon  = m;
    c->tags = m->tagset[m->seltags]; /* assign TAGS of target monitor */
    monitors::attach_client(c->mon, c);
    monitors::attach_to_stack(c);
    clients::focus(nullptr);
    layouts::arrange(nullptr);
  }

  bool send_event(Client::sptr client, x11::atom_t proto) {
    int n;
    x11::atom_t* protocols;
    bool exists = false;
    x11::event_t ev;

    if (x11::get_wm_protocols(client->win, &protocols, &n)) {
      while (!exists && n--)
        exists = protocols[n] == proto;
      x11::free(protocols);
    }
    if (exists) {
      ev.type                 = ClientMessage;
      ev.xclient.window       = client->win;
      ev.xclient.message_type = state::atoms::wmatom[WMProtocols];
      ev.xclient.format       = 32;
      ev.xclient.data.l[0]    = proto;
      ev.xclient.data.l[1]    = CurrentTime;
      x11::send_event(client->win, false, NoEventMask, &ev);
    }
    return exists;
  }

  void set_state(Client::sptr client, long state) {
    long data[] = {state, None};

    x11::change_property(
      client->win,
      state::atoms::wmatom[WMState],
      state::atoms::wmatom[WMState],
      32,
      PropModeReplace,
      (unsigned char*)data,
      2
    );
  }

  x11::atom_t get_atom_prop(Client::sptr client, x11::atom_t prop) {
    int di;
    unsigned long dl;
    unsigned char* p     = nullptr;
    x11::atom_t da, atom = None;

    if (x11::get_window_property(client->win, prop, 0L, sizeof atom, false, XA_ATOM,
                                 &da, &di, &dl, &dl, &p) == Success && p) {
      atom = *(x11::atom_t*)p;
      x11::free(p);
    }
    return atom;
  }


  void set_fullscreen(Client::sptr client, int fullscreen) {
    if (fullscreen && !client->isfullscreen) {
      x11::change_property(client->win, state::atoms::netatom[NetWMState], XA_ATOM, 32,
                           PropModeReplace, (unsigned char*)&state::atoms::netatom[NetWMFullscreen], 1);
      client->isfullscreen = 1;
      client->oldstate     = client->isfloating;
      client->oldbw        = client->bw;
      client->bw           = 0;
      client->isfloating   = 1;
      resize_client(client, client->mon->mx, client->mon->my, client->mon->mw, client->mon->mh);
      x11::raise_window(client->win);
    } else if (!fullscreen && client->isfullscreen) {
      x11::change_property(client->win, state::atoms::netatom[NetWMState], XA_ATOM, 32,
                           PropModeReplace, (unsigned char*)0, 0);
      client->isfullscreen = 0;
      client->isfloating   = client->oldstate;
      client->bw           = client->oldbw;
      client->x            = client->oldx;
      client->y            = client->oldy;
      client->w            = client->oldw;
      client->h            = client->oldh;
      resize_client(client, client->x, client->y, client->w, client->h);
      layouts::arrange(client->mon);
    }
  }

  void set_urgent(Client::sptr client, int urg) {
    if (urg) {
      unsigned int i;
      /* === This focuses windows when urgent ===*/
      /* Useful when having rules for different clases to go to different TAGS */
      for (i = 0; i < state::config::TAGS.size() && !((unsigned int)(1 << i) & client->tags); i++);
      if (i < state::config::TAGS.size()) {
        const Arg a                       = {.ui = (unsigned int)(1 << i)};
        state::monitors::selected_monitor = client->mon;
        control::view(&a);
        focus(client);
        monitors::restack(state::monitors::selected_monitor);
      }
    }
    x11::wm_hints_t* wmh = x11::get_wm_hints(client->win);

    client->isurgent = urg;
    if (!wmh)
      return;
    wmh->flags = urg ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
    x11::set_wm_hints(client->win, wmh);
    x11::free(wmh);
  }

}

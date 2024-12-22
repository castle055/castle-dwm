//
// Created by castle on 3/24/22.
//

#include "event_ops.h"

#include "X11/Xatom.h"
#include "../state/dwm_types.h"
#include "../state/state.h"
#include "client_ops.h"
#include "monitor_ops.h"
#include "x11_ops.h"
#include "log_ops.h"
#include "bar_ops.h"
#include "keynav_ops.h"

using namespace ops;

void (*event::handler[])(XEvent *) = {
    nullptr,
    nullptr,
    handlers::keypress,
    nullptr, // TODO - this one is keyrelease
    handlers::buttonpress,
    nullptr,
    handlers::motionnotify,
    handlers::enternotify,
    nullptr,
    handlers::focusin,
    nullptr,nullptr,
    handlers::expose,
    nullptr,nullptr,nullptr,nullptr,
    handlers::destroynotify,
    handlers::unmapnotify,
    nullptr,
    handlers::maprequest,
    nullptr,
    handlers::configurenotify,
    handlers::configurerequest,
    nullptr,nullptr,nullptr,nullptr,
    handlers::propertynotify,
    nullptr,nullptr,nullptr,nullptr,
    handlers::clientmessage,
    handlers::mappingnotify
};
void ops::event::handlers::buttonpress(XEvent *e) {
  unsigned int i, x, click;
  Arg arg = {0};
  client_t *c;
  monitor_t *m;
  XButtonPressedEvent *ev = &e->xbutton;
  
  click = ClkRootWin;
  /* focus monitor if necessary */
  if ((m = monitor::win_to_mon(ev->window)) && m != state::selmon) {
    ops::client::unfocus(state::selmon->sel, 1);
    state::selmon = m;
    client::focus(nullptr);
  }
  if (ev->window == state::selmon->barwin) {
    i = x = 0;
    do
      x += TEXTW(state::config::TAGS[i]);
    while (ev->x >= x && ++i < state::config::TAGS.size());
    if (i < state::config::TAGS.size()) {
      click = ClkTagBar;
      arg.ui = 1 << i;
    } else if (ev->x < x + state::blw)
      click = ClkLtSymbol;
    else if (ev->x > state::selmon->ww - TEXTW(state::stext))
      click = ClkStatusText;
    else
      click = ClkWinTitle;
  } else if ((c = client::win_to_client(ev->window))) {
    ops::client::focus(c);
    ops::monitor::restack(state::selmon);
    XAllowEvents(state::dpy, ReplayPointer, CurrentTime);
    click = ClkClientWin;
  }
  for (i = 0; i < state::config::buttons.size(); i++)
    if (click == state::config::buttons[i].click && state::config::buttons[i].func && state::config::buttons[i].button == ev->button
        && CLEANMASK(state::config::buttons[i].mask) == CLEANMASK(ev->state))
      state::config::buttons[i].func(click == ClkTagBar && state::config::buttons[i].arg.i == 0 ? &arg : &state::config::buttons[i].arg);
}
void ops::event::handlers::clientmessage(XEvent *e) {
  XClientMessageEvent *cme = &e->xclient;
  client_t *c = client::win_to_client(cme->window);
  
  if (!c)
    return;
  if (cme->message_type == state::netatom[NetWMState]) {
    if (cme->data.l[1] == state::netatom[NetWMFullscreen]
        || cme->data.l[2] == state::netatom[NetWMFullscreen])
      ops::client::set_fullscreen(c, (cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
                                      || (cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */ && !c->isfullscreen)));
  } else if (cme->message_type == state::netatom[NetActiveWindow]) {
    if (c != state::selmon->sel && !c->isurgent)
      ops::client::set_urgent(c, 1);
  }
}
void ops::event::handlers::configurenotify(XEvent *e) {
  monitor_t *m;
  client_t *c;
  XConfigureEvent *ev = &e->xconfigure;
  int dirty;
  
  /* TODO: update_geometry handling sucks, needs to be simplified */
  if (ev->window == state::root) {
    dirty = (state::sw != ev->width || state::sh != ev->height);
    state::sw = ev->width;
    state::sh = ev->height;
    if (x11::update_geometry() || dirty) {
      drw_resize(state::drw, state::sw, state::bar_height);
      bar::init_where_needed();
      for (m = state::mons; m; m = m->next) {
        for (c = m->clients; c; c = c->next)
          if (c->isfullscreen)
            ops::client::resize_client(c, m->mx, m->my, m->mw, m->mh);
        XMoveResizeWindow(state::dpy, m->barwin, m->wx, m->by, m->ww, state::bar_height);
      }
      client::focus(nullptr);
      monitor::arrange(nullptr);
    }
  }
}
void ops::event::handlers::configurerequest(XEvent *e) {
  client_t *c;
  monitor_t *m;
  XConfigureRequestEvent *ev = &e->xconfigurerequest;
  XWindowChanges wc;
  
  if ((c = client::win_to_client(ev->window))) {
    if (ev->value_mask & CWBorderWidth)
      c->bw = ev->border_width;
    else if (c->isfloating || !state::selmon->lt[state::selmon->sellt]->arrange_) {
      m = c->mon;
      if (ev->value_mask & CWX) {
        c->oldx = c->x;
        c->x = m->mx + ev->x;
      }
      if (ev->value_mask & CWY) {
        c->oldy = c->y;
        c->y = m->my + ev->y;
      }
      if (ev->value_mask & CWWidth) {
        c->oldw = c->w;
        c->w = ev->width;
      }
      if (ev->value_mask & CWHeight) {
        c->oldh = c->h;
        c->h = ev->height;
      }
      if ((c->x + c->w) > m->mx + m->mw && c->isfloating)
        c->x = m->mx + (m->mw / 2 - WIDTH(c) / 2); /* center in x direction */
      if ((c->y + c->h) > m->my + m->mh && c->isfloating)
        c->y = m->my + (m->mh / 2 - HEIGHT(c) / 2); /* center in y direction */
      if ((ev->value_mask & (CWX | CWY)) && !(ev->value_mask & (CWWidth | CWHeight)))
        ops::client::configure_(c);
      if (ISVISIBLE(c))
        XMoveResizeWindow(state::dpy, c->win, c->x, c->y, c->w, c->h);
    } else
      ops::client::configure_(c);
  } else {
    wc.x = ev->x;
    wc.y = ev->y;
    wc.width = ev->width;
    wc.height = ev->height;
    wc.border_width = ev->border_width;
    wc.sibling = ev->above;
    wc.stack_mode = ev->detail;
    XConfigureWindow(state::dpy, ev->window, ev->value_mask, &wc);
  }
  XSync(state::dpy, False);
}
void ops::event::handlers::destroynotify(XEvent *e) {
  client_t *c;
  XDestroyWindowEvent *ev = &e->xdestroywindow;
  
  if ((c = client::win_to_client(ev->window)))
    ops::client::unmanage(c, 1);
}
void ops::event::handlers::enternotify(XEvent *e) {
  client_t *c;
  monitor_t *m;
  XCrossingEvent *ev = &e->xcrossing;
  
  if ((ev->mode != NotifyNormal || ev->detail == NotifyInferior) && ev->window != state::root)
    return;
  c = client::win_to_client(ev->window);
  m = c ? c->mon : monitor::win_to_mon(ev->window);
  if (m != state::selmon) {
    ops::client::unfocus(state::selmon->sel, 1);
    state::selmon = m;
  } else if (!c || c == state::selmon->sel)
    return;
  ops::client::focus(c);
}
void ops::event::handlers::expose(XEvent *e) {
  monitor_t *m;
  XExposeEvent *ev = &e->xexpose;
  
  if (ev->count == 0 && (m = monitor::win_to_mon(ev->window)))
    ops::bar::update_all();
}
void ops::event::handlers::focusin(XEvent *e) {
  XFocusChangeEvent *ev = &e->xfocus;
  
  if (state::selmon->sel && ev->window != state::selmon->sel->win)
    ops::client::set_focus(state::selmon->sel);
}
void ops::event::handlers::keypress(XEvent *e) {
  unsigned int i;
  
  KeySym keysym;
  XKeyEvent *ev;
  
  ev = &e->xkey;
  if (keynav::process(ev)) return;
  
  keysym = XKeycodeToKeysym(state::dpy, (KeyCode) ev->keycode, 0);
  
  // If not KeyNav, check normal shortcuts
  for (i = 0; i < state::config::keys.size(); i++)
    //if (((keysym == state::config::keys[i].keysym
    //    && CLEANMASK(state::config::keys[i].mod) == CLEANMASK(ev->state))
    //    || (state::config::keys[i].keysym == XK_Meta_L
    //    || state::config::keys[i].keysym == XK_Meta_R))
    if (keysym == state::config::keys[i].keysym
        && CLEANMASK(state::config::keys[i].mod) == CLEANMASK(ev->state)
        && state::config::keys[i].func)
      state::config::keys[i].func(&(state::config::keys[i].arg));
}
void ops::event::handlers::mappingnotify(XEvent *e) {
  XMappingEvent *ev = &e->xmapping;
  
  XRefreshKeyboardMapping(ev);
  if (ev->request == MappingKeyboard) {
    x11::grab_keys();
    keynav::reset();
  }
}
void ops::event::handlers::maprequest(XEvent *e) {
  log::debug("<maprequest>");
  static XWindowAttributes wa;
  XMapRequestEvent *ev = &e->xmaprequest;
  
  if (!XGetWindowAttributes(state::dpy, ev->window, &wa))
    return;
  if (wa.override_redirect)
    return;
  if (!client::win_to_client(ev->window))
    client::manage(ev->window, &wa, 1);
}
void ops::event::handlers::motionnotify(XEvent *e) {
  static monitor_t *mon = nullptr;
  monitor_t *m;
  XMotionEvent *ev = &e->xmotion;
  
  if (ev->window != state::root)
    return;
  if ((m = monitor::rect_to_mon(ev->x_root, ev->y_root, 1, 1)) != mon && mon) {
    ops::client::unfocus(state::selmon->sel, 1);
    state::selmon = m;
    client::focus(nullptr);
  }
  mon = m;
}
void ops::event::handlers::propertynotify(XEvent *e) {
  client_t *c;
  Window trans;
  XPropertyEvent *ev = &e->xproperty;
  
  if ((ev->window == state::root) && (ev->atom == XA_WM_NAME))
    bar::update_all();
  else if (ev->state == PropertyDelete)
    return; /* ignore */
  else if ((c = client::win_to_client(ev->window))) {
    switch (ev->atom) {
      default:
        break;
      case XA_WM_TRANSIENT_FOR:
        if (!c->ignoretransient && !c->isfloating && (XGetTransientForHint(state::dpy, c->win, &trans)) &&
            (c->isfloating = (client::win_to_client(trans)) != NULL))
          monitor::arrange(c->mon);
        break;
      case XA_WM_NORMAL_HINTS:
        ops::client::update_size_hints(c);
        break;
      case XA_WM_HINTS:
        ops::client::update_wm_hints(c);
        bar::update_all();
        break;
    }
    if (ev->atom == XA_WM_NAME || ev->atom == state::netatom[NetWMName]) {
      ops::client::update_title(c);
      if (c == c->mon->sel)
        bar::update_all();
    }
    if (ev->atom == state::netatom[NetWMWindowType])
      ops::client::update_window_type(c);
  }
}
void ops::event::handlers::unmapnotify(XEvent *e) {
  client_t *c;
  XUnmapEvent *ev = &e->xunmap;
  
  if ((c = client::win_to_client(ev->window))) {
    if (ev->send_event)
      client::set_client_state(c, WithdrawnState);
    else
      ops::client::unmanage(c, 0);
  }
}

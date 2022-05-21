//
// Created by castle on 3/24/22.
//

#include <cmath>
#include <cstring>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "../util.h"

#include "client_ops.h"
#include "monitor_ops.h"
#include "control_ops.h"
#include "log_ops.h"
#include "x11_ops.h"
#include "../state/state.h"
#include "bar_ops.h"

using namespace ops;

//===== client_t Ops implementations =====
void client::apply_rules(client_t* client) {
  const char *class_, *instance;
  unsigned int i;
  const Rule *r;
  monitor_t *m;
  XClassHint ch = {nullptr, nullptr};
  
  /* rule matching */
  client->isfloating = 0;
  client->tags = 0;
  XGetClassHint(state::dpy, client->win, &ch);
  class_ = ch.res_class ? ch.res_class : state::broken;
  instance = ch.res_name ? ch.res_name : state::broken;
  fprintf(stdout, "class %s - instance %s\n\r", class_, instance);
  
  for (i = 0; i < state::config::rules.size(); i++) {
    r = &state::config::rules[i];
    if ((!r->title || client->name.find(r->title) != std::string::npos)
        && (!r->class_ || strstr(class_, r->class_))
        && (!r->instance || strstr(instance, r->instance))) {
      client->isfloating = r->isfloating;
      client->ignoretransient = r->ignoretransient;
      client->tags |= r->tags;
      for (m = state::mons; m && m->num != r->monitor; m = m->next) {
        fprintf(stdout, "m->num: %d\n\r", m->num);
      }
      if (m)
        client->mon = m;
    }
  }
  if (ch.res_class)
    XFree(ch.res_class);
  if (ch.res_name)
    XFree(ch.res_name);
  client->tags = client->tags & TAGMASK ? client->tags & TAGMASK : client->mon->tagset[client->mon->seltags];
}
int client::apply_size_hints(client_t* client, int *X, int *Y, int *W, int *H, int interact) {
  int baseismin;
  monitor_t *m = client->mon;
  
  /* set minimum possible */
  *W = MAX(1, *W);
  *H = MAX(1, *H);
  if (interact) {
    if (*X > state::sw)
      *X = state::sw - WIDTH(client);
    if (*Y > state::sh)
      *Y = state::sh - HEIGHT(client);
    if (*X + *W + 2 * client->bw < 0)
      *X = 0;
    if (*Y + *H + 2 * client->bw < 0)
      *Y = 0;
  } else {
    if (*X >= m->wx + m->ww)
      *X = m->wx + m->ww - WIDTH(client);
    if (*Y >= m->wy + m->wh)
      *Y = m->wy + m->wh - HEIGHT(client);
    if (*X + *W + 2 * client->bw <= m->wx)
      *X = m->wx;
    if (*Y + *H + 2 * client->bw <= m->wy)
      *Y = m->wy;
  }
  if (*H < state::bar_height)
    *H = state::bar_height;
  if (*W < state::bar_height)
    *W = state::bar_height;
  if (state::config::resizehints || client->isfloating || !client->mon->lt[client->mon->sellt]->arrange_) {
    /* see last two sentences in ICCCM 4.1.2.3 */
    baseismin = client->basew == client->minw && client->baseh == client->minh;
    if (!baseismin) { /* temporarily remove base dimensions */
      *W -= client->basew;
      *H -= client->baseh;
    }
    /* adjust for aspect limits */
    if (client->mina > 0 && client->maxa > 0) {
      if (client->maxa < (float) *W / (float) *H)
        *W = (int)lround((float)*H * client->maxa + 0.5);
      else if (client->mina < (float) *H / (float) *W)
        *H = (int)lround((float)*W * client->mina + 0.5);
    }
    if (baseismin) { /* increment calculation requires this */
      *W -= client->basew;
      *H -= client->baseh;
    }
    /* adjust for increment value */
    if (client->incw)
      *W -= *W % client->incw;
    if (client->inch)
      *H -= *H % client->inch;
    /* restore base dimensions */
    *W = MAX(*W + client->basew, client->minw);
    *H = MAX(*H + client->baseh, client->minh);
    if (client->maxw)
      *W = MIN(*W, client->maxw);
    if (client->maxh)
      *H = MIN(*H, client->maxh);
  }
  return *X != client->x || *Y != client->y || *W != client->w || *H != client->h;
}
void client::attach(client_t* client) {
  client->next = client->mon->clients;
  client->mon->clients = client;
}
void client::attach_stack(client_t* client) {
  client->snext = client->mon->stack;
  client->mon->stack = client;
}
void client::configure_(client_t* client) {
  XConfigureEvent ce;
  
  ce.type = ConfigureNotify;
  ce.display = state::dpy;
  ce.event = client->win;
  ce.window = client->win;
  ce.x = client->x;
  ce.y = client->y;
  ce.width = client->w;
  ce.height = client->h;
  ce.border_width = client->bw;
  ce.above = None;
  ce.override_redirect = False;
  XSendEvent(state::dpy, client->win, False, StructureNotifyMask, (XEvent *) &ce);
}
void client::detach(client_t* client) {
  client_t **tc;
  
  for (tc = &client->mon->clients; *tc && *tc != client; tc = &(*tc)->next);
  *tc = client->next;
}
void client::detach_stack(client_t* client) {
  client_t **tc, *t;
  
  for (tc = &client->mon->stack; *tc && *tc != client; tc = &(*tc)->snext);
  *tc = client->snext;
  
  if (client == client->mon->sel) {
    for (t = client->mon->stack; t && !ISVISIBLE(t); t = t->snext);
    client->mon->sel = t;
  }
}
void client::focus(client_t* client) {
  client_t *c = client;
  
  if (!c || !ISVISIBLE(c))
    for (c = state::selmon->stack; c && !ISVISIBLE(c); c = c->snext);
  if (state::selmon->sel && state::selmon->sel != client)
    unfocus(state::selmon->sel, 0);
  if (c) {
    if (c->mon != state::selmon)
      state::selmon = c->mon;
    if (c->isurgent)
      set_urgent(c, 0);
    detach_stack(c);
    attach_stack(c);
    grab_buttons(c, 1);
    XSetWindowBorder(state::dpy, c->win, state::scheme[SchemeSel][ColBorder].pixel);
    set_focus(c);
  } else {
    XSetInputFocus(state::dpy, state::root, RevertToPointerRoot, CurrentTime);
    XDeleteProperty(state::dpy, state::root, state::netatom[NetActiveWindow]);
  }
  state::selmon->sel = c;
  bar::update_all();
}
Atom client::get_atom_prop(client_t* client, Atom prop) {
  int di;
  unsigned long dl;
  unsigned char *p = NULL;
  Atom da, atom = None;
  
  if (XGetWindowProperty(state::dpy, client->win, prop, 0L, sizeof atom, False, XA_ATOM,
                         &da, &di, &dl, &dl, &p) == Success && p) {
    atom = *(Atom *) p;
    XFree(p);
  }
  return atom;
}
void client::grab_buttons(client_t* client, int focused) {
  x11::update_numlock_mask();
  {
    unsigned int i, j;
    unsigned int modifiers[] = {0, LockMask, state::numlockmask, state::numlockmask | LockMask};
    XUngrabButton(state::dpy, AnyButton, AnyModifier, client->win);
    if (!focused)
      XGrabButton(state::dpy, AnyButton, AnyModifier, client->win, False,
                  BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
    for (i = 0; i < state::config::buttons.size(); i++)
      if (state::config::buttons[i].click == ClkClientWin)
        for (j = 0; j < LENGTH(modifiers); j++)
          XGrabButton(state::dpy, state::config::buttons[i].button,
                      state::config::buttons[i].mask | modifiers[j],
                      client->win, False, BUTTONMASK,
                      GrabModeAsync, GrabModeSync, None, None);
  }
}
void client::pop(client_t* client) {
  detach(client);
  attach(client);
  focus(client);
  monitor::arrange(client->mon);
}
void client::resize(client_t* client, int X, int Y, int W, int H, int interact) {
  if (apply_size_hints(client, &X, &Y, &W, &H, interact))
    resize_client(client, X, Y, W, H);
}
void client::resize_client(client_t* client, int X, int Y, int W, int H) {
  XWindowChanges wc;
  
  client->oldx = client->x;
  client->x = wc.x = X;
  client->oldy = client->y;
  client->y = wc.y = Y;
  client->oldw = client->w;
  client->w = wc.width = W;
  client->oldh = client->h;
  client->h = wc.height = H;
  wc.border_width = client->bw;
  XConfigureWindow(state::dpy, client->win, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &wc);
  configure_(client);
  XSync(state::dpy, False);
}
void client::set_focus(client_t* client) {
  if (!client->neverfocus) {
    XSetInputFocus(state::dpy, client->win, RevertToPointerRoot, CurrentTime);
    XChangeProperty(state::dpy, state::root, state::netatom[NetActiveWindow],
                    XA_WINDOW, 32, PropModeReplace,
                    (unsigned char *) &(client->win), 1);
  }
  send_event(client, state::wmatom[WMTakeFocus]);
}
void client::set_fullscreen(client_t* client, int fullscreen) {
  if (fullscreen && !client->isfullscreen) {
    XChangeProperty(state::dpy, client->win, state::netatom[NetWMState], XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &state::netatom[NetWMFullscreen], 1);
    client->isfullscreen = 1;
    client->oldstate = client->isfloating;
    client->oldbw = client->bw;
    client->bw = 0;
    client->isfloating = 1;
    resize_client(client, client->mon->mx, client->mon->my, client->mon->mw, client->mon->mh);
    XRaiseWindow(state::dpy, client->win);
  } else if (!fullscreen && client->isfullscreen) {
    XChangeProperty(state::dpy, client->win, state::netatom[NetWMState], XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) 0, 0);
    client->isfullscreen = 0;
    client->isfloating = client->oldstate;
    client->bw = client->oldbw;
    client->x = client->oldx;
    client->y = client->oldy;
    client->w = client->oldw;
    client->h = client->oldh;
    resize_client(client, client->x, client->y, client->w, client->h);
    monitor::arrange(client->mon);
  }
}
void client::set_urgent(client_t* client, int urg) {
  if (urg) {
    unsigned int i;
    /* === This focuses windows when urgent ===*/
    /* Useful when having rules for different clases to go to different TAGS */
    for (i = 0; i < state::config::TAGS.size() && !((unsigned int) (1 << i) & client->tags); i++);
    if (i < state::config::TAGS.size()) {
      const Arg a = {.ui = (unsigned int) (1 << i)};
      state::selmon = client->mon;
      control::view(&a);
      focus(client);
      monitor::restack(state::selmon);
    }
  }
  XWMHints *wmh;
  
  client->isurgent = urg;
  if (!(wmh = XGetWMHints(state::dpy, client->win)))
    return;
  wmh->flags = urg ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
  XSetWMHints(state::dpy, client->win, wmh);
  XFree(wmh);
}
void client::show_hide(client_t* client) {
  if (!client)
    return;
  if (ISVISIBLE(client)) {
    /* show clients top down */
    XMoveWindow(state::dpy, client->win, client->x, client->y);
    if ((!client->mon->lt[client->mon->sellt]->arrange_ || client->isfloating) && !client->isfullscreen)
      resize(client, client->x, client->y, client->w, client->h, 0);
    show_hide(client->snext);
  } else {
    /* hide clients bottom up */
    show_hide(client->snext);
    XMoveWindow(state::dpy, client->win, WIDTH(client) * -2, client->y);
  }
}
void client::unfocus(client_t* client, int set_focus) {
  if (!client)
    return;
  grab_buttons(client, 0);
  XSetWindowBorder(state::dpy, client->win, state::scheme[SchemeNorm][ColBorder].pixel);
  if (set_focus) {
    XSetInputFocus(state::dpy, state::root, RevertToPointerRoot, CurrentTime);
    XDeleteProperty(state::dpy, state::root, state::netatom[NetActiveWindow]);
  }
}
void client::unmanage(client_t* client, int destroyed) {
  monitor_t *m = client->mon;
  XWindowChanges wc;
  
  detach(client);
  detach_stack(client);
  if (!destroyed) {
    wc.border_width = client->oldbw;
    XGrabServer(state::dpy); /* avoid race conditions */
    XSetErrorHandler(x11::x_error_dummy);
    XConfigureWindow(state::dpy, client->win, CWBorderWidth, &wc); /* restore border */
    XUngrabButton(state::dpy, AnyButton, AnyModifier, client->win);
    set_client_state(client, WithdrawnState);
    XSync(state::dpy, False);
    XSetErrorHandler(x11::x_error);
    XUngrabServer(state::dpy);
  }
  delete client;
  client::focus(nullptr);
  x11::update_client_list();
  monitor::arrange(m);
}
void client::update_size_hints(client_t* client) {
  long msize;
  XSizeHints size;
  
  if (!XGetWMNormalHints(state::dpy, client->win, &size, &msize))
    /* size is uninitialized, ensure that size.flags aren't used */
    size.flags = PSize;
  if (size.flags & PBaseSize) {
    client->basew = size.base_width;
    client->baseh = size.base_height;
  } else if (size.flags & PMinSize) {
    client->basew = size.min_width;
    client->baseh = size.min_height;
  } else
    client->basew = client->baseh = 0;
  if (size.flags & PResizeInc) {
    client->incw = size.width_inc;
    client->inch = size.height_inc;
  } else
    client->incw = client->inch = 0;
  if (size.flags & PMaxSize) {
    client->maxw = size.max_width;
    client->maxh = size.max_height;
  } else
    client->maxw = client->maxh = 0;
  if (size.flags & PMinSize) {
    client->minw = size.min_width;
    client->minh = size.min_height;
  } else if (size.flags & PBaseSize) {
    client->minw = size.base_width;
    client->minh = size.base_height;
  } else
    client->minw = client->minh = 0;
  if (size.flags & PAspect) {
    client->mina = (float) size.min_aspect.y / size.min_aspect.x;
    client->maxa = (float) size.max_aspect.x / size.max_aspect.y;
  } else
    client->maxa = client->mina = 0.0;
  client->isfixed = (client->maxw && client->maxh && client->maxw == client->minw && client->maxh == client->minh);
}
void client::update_title(client_t* client) {
  log::debug("[update_title]");
  if (!x11::get_text_prop(client->win, state::netatom[NetWMName], client->name)) {
    x11::get_text_prop(client->win, XA_WM_NAME, client->name);
  }
  if (client->name[0] == '\0') { /* hack to mark broken clients */
    log::debug("[update_title] window [0x%X] has broken title", client->win);
    client->name = state::broken;
  }
}
void client::update_window_type(client_t* client) {
  Atom state = get_atom_prop(client, state::netatom[NetWMState]);
  Atom wtype = get_atom_prop(client, state::netatom[NetWMWindowType]);
  
  if (state == state::netatom[NetWMFullscreen])
    set_fullscreen(client, 1);
  if (wtype == state::netatom[NetWMWindowTypeDialog])
    client->isfloating = 1;
}
void client::update_wm_hints(client_t* client) {
  XWMHints *wmh;
  
  if ((wmh = XGetWMHints(state::dpy, client->win))) {
    if (client == state::selmon->sel && wmh->flags & XUrgencyHint) {
      wmh->flags &= ~XUrgencyHint;
      XSetWMHints(state::dpy, client->win, wmh);
    } else
      client->isurgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
    if (wmh->flags & InputHint)
      client->neverfocus = !wmh->input;
    else
      client->neverfocus = 0;
    XFree(wmh);
  }
}
client_t* client::next_tiled(client_t* client) {
  client_t* c = client;
  for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next);
  return c;
}
client_t* client::win_to_client(Window w) {
  log::debug("[win_to_client]");
  client_t *c;
  monitor_t *m;
  
  for (m = state::mons; m; m = m->next)
    for (c = m->clients; c; c = c->next)
      if (c->win == w)
        return c;
  return nullptr;
}
void client::send_mon(client_t *c, monitor_t *m) {
  if (c->mon == m)
    return;
  client::unfocus(c, 1);
  client::detach(c);
  client::detach_stack(c);
  c->mon = m;
  c->tags = m->tagset[m->seltags]; /* assign TAGS of target monitor */
  client::attach(c);
  client::attach_stack(c);
  client::focus(nullptr);
  monitor::arrange(NULL);
}
void client::set_client_state(client_t *c, long state) {
  long data[] = {state, None};
  
  XChangeProperty(state::dpy, c->win, state::wmatom[WMState], state::wmatom[WMState], 32,
                  PropModeReplace, (unsigned char *) data, 2);
}
int client::send_event(client_t *c, Atom proto) {
  int n;
  Atom *protocols;
  int exists = 0;
  XEvent ev;
  
  if (XGetWMProtocols(state::dpy, c->win, &protocols, &n)) {
    while (!exists && n--)
      exists = protocols[n] == proto;
    XFree(protocols);
  }
  if (exists) {
    ev.type = ClientMessage;
    ev.xclient.window = c->win;
    ev.xclient.message_type = state::wmatom[WMProtocols];
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = proto;
    ev.xclient.data.l[1] = CurrentTime;
    XSendEvent(state::dpy, c->win, False, NoEventMask, &ev);
  }
  return exists;
}
void client::manage(Window w, XWindowAttributes *wa, int urgent) {
  log::debug("[manage]");
  client_t *c, *t = nullptr;
  Window trans = None;
  XWindowChanges wc;
  
  c = new client_t; //(client_t *) ecalloc(1, sizeof(client_t));
  c->win = w;
  /* geometry */
  c->x = c->oldx = wa->x;
  c->y = c->oldy = wa->y;
  c->w = c->oldw = wa->width;
  c->h = c->oldh = wa->height;
  c->oldbw = wa->border_width;
  
  log::debug("[manage]");
  update_title(c);
  if (XGetTransientForHint(state::dpy, w, &trans) && (t = win_to_client(trans))) {
    c->mon = t->mon;
    c->tags = t->tags;
  } else {
    c->mon = state::selmon;
    ops::client::apply_rules(c);
  }
  
  log::debug("[manage]");
  if (c->x + WIDTH(c) > c->mon->mx + c->mon->mw)
    c->x = c->mon->mx + c->mon->mw - WIDTH(c);
  if (c->y + HEIGHT(c) > c->mon->my + c->mon->mh)
    c->y = c->mon->my + c->mon->mh - HEIGHT(c);
  c->x = MAX(c->x, c->mon->mx);
  /* only fix client y-offset, if the client center might cover the bar */
  c->y = MAX(c->y, ((c->mon->by == c->mon->my) && (c->x + (c->w / 2) >= c->mon->wx)
                    && (c->x + (c->w / 2) < c->mon->wx + c->mon->ww)) ? state::bar_height : c->mon->my);
  c->bw = state::config::borderpx;
  
  log::debug("[manage]");
  wc.border_width = c->bw;
  if (c->x == state::selmon->wx) c->x += (c->mon->ww - WIDTH(c)) / 2 - c->bw;
  if (c->y == state::selmon->wy) c->y += (c->mon->wh - HEIGHT(c)) / 2 - c->bw;
  XConfigureWindow(state::dpy, w, CWBorderWidth, &wc);
  XSetWindowBorder(state::dpy, w, state::scheme[SchemeNorm][ColBorder].pixel);
  configure_(c); /* propagates border_width, if size doesn't change */
  update_window_type(c);
  update_size_hints(c);
  update_wm_hints(c);
  XSelectInput(state::dpy, w, EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask);
  grab_buttons(c, 0);
  log::debug("[manage]");
  if (!c->isfloating)
    c->isfloating = c->oldstate = trans != None || c->isfixed;
  if (c->isfloating)
    XRaiseWindow(state::dpy, c->win);
  attach(c);
  attach_stack(c);
  XChangeProperty(state::dpy, state::root, state::netatom[NetClientList], XA_WINDOW, 32, PropModeAppend,
                  (unsigned char *) &(c->win), 1);
  XMoveResizeWindow(state::dpy, c->win, c->x + 2 * state::sw, c->y, c->w, c->h); /* some windows require this */
  set_client_state(c, NormalState);
  log::debug("[manage]");
  if (c->mon == state::selmon)
    unfocus(state::selmon->sel, 0);
  c->mon->sel = c;
  monitor::arrange(c->mon);
  XMapWindow(state::dpy, c->win);
  focus(nullptr);
  set_urgent(c, urgent);
}


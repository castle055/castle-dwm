//
// Created by castle on 3/24/22.
//

//#include <program_shell.h>
#include "control_ops.h"

#include "../state/state.h"
#include "client_ops.h"
#include "monitor_ops.h"
#include "x11_ops.h"
#include "event_ops.h"
#include "../util.h"
#include "log_ops.h"
#include "bar_ops.h"

#include <unistd.h>

using namespace ops;

void control::focusmon(const Arg *arg) {
  monitor_t *m;
  
  if (!state::mons->next)
    return;
  if ((m = monitor::dir_to_mon(arg->i)) == state::selmon)
    return;
  ops::client::unfocus(state::selmon->sel, 0);
  state::selmon = m;
  client::focus(nullptr);
}
void control::focusstack(const Arg *arg) {
  client_t *c = NULL, *i;
  
  if (!state::selmon->sel)
    return;
  if (arg->i > 0) {
    for (c = state::selmon->sel->next; c && !ISVISIBLE(c); c = c->next);
    if (!c)
      for (c = state::selmon->clients; c && !ISVISIBLE(c); c = c->next);
  } else {
    for (i = state::selmon->clients; i != state::selmon->sel; i = i->next)
      if (ISVISIBLE(i))
        c = i;
    if (!c)
      for (; i; i = i->next)
        if (ISVISIBLE(i))
          c = i;
  }
  if (c) {
    client::focus(c);
    monitor::restack(state::selmon);
  }
}
void control::incnmaster(const Arg *arg) {
  unsigned int i;
  state::selmon->nmaster = MAX(state::selmon->nmaster + arg->i, 0);
  for (i = 0; i <= state::config::TAGS.size(); ++i)
    if (state::selmon->tagset[state::selmon->seltags] & 1 << i)
      state::selmon->pertag->nmasters[(i + 1) % (state::config::TAGS.size() + 1)] = state::selmon->nmaster;
  monitor::arrange(state::selmon);
}
void control::killclient(const Arg *arg) {
  if (!state::selmon->sel)
    return;
  if (!client::send_event(state::selmon->sel, state::wmatom[WMDelete])) {
    XGrabServer(state::dpy);
    XSetErrorHandler(x11::x_error_dummy);
    XSetCloseDownMode(state::dpy, DestroyAll);
    XKillClient(state::dpy, state::selmon->sel->win);
    XSync(state::dpy, False);
    XSetErrorHandler(x11::x_error);
    XUngrabServer(state::dpy);
  }
}
void control::movemouse(const Arg *arg) {
  int x, y, ocx, ocy, nx, ny;
  client_t *c;
  monitor_t *m;
  XEvent ev;
  Time lasttime = 0;
  
  if (!(c = state::selmon->sel))
    return;
  if (c->isfullscreen) /* no support moving fullscreen windows by mouse */
    return;
  monitor::restack(state::selmon);
  ocx = c->x;
  ocy = c->y;
  if (XGrabPointer(state::dpy, state::root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
                   None, state::cursor[CurMove]->cursor, CurrentTime) != GrabSuccess)
    return;
  if (!x11::get_root_ptr(&x, &y))
    return;
  do {
    XMaskEvent(state::dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);
    switch (ev.type) {
      case ConfigureRequest:
      case Expose:
      case MapRequest:
        event::handler[ev.type](&ev);
        break;
      case MotionNotify:
        if ((ev.xmotion.time - lasttime) <= (1000 / 60))
          continue;
        lasttime = ev.xmotion.time;
        
        nx = ocx + (ev.xmotion.x - x);
        ny = ocy + (ev.xmotion.y - y);
        if (abs(state::selmon->wx - nx) < state::config::snap)
          nx = state::selmon->wx;
        else if (abs((state::selmon->wx + state::selmon->ww) - (nx + WIDTH(c))) < state::config::snap)
          nx = state::selmon->wx + state::selmon->ww - WIDTH(c);
        if (abs(state::selmon->wy - ny) < state::config::snap)
          ny = state::selmon->wy;
        else if (abs((state::selmon->wy + state::selmon->wh) - (ny + HEIGHT(c))) < state::config::snap)
          ny = state::selmon->wy + state::selmon->wh - HEIGHT(c);
        if (!c->isfloating && state::selmon->lt[state::selmon->sellt]->arrange_
            && (abs(nx - c->x) > state::config::snap || abs(ny - c->y) > state::config::snap))
          togglefloating(NULL);
        if (!state::selmon->lt[state::selmon->sellt]->arrange_ || c->isfloating)
          client::resize(c, nx, ny, c->w, c->h, 1);
        break;
    }
  } while (ev.type != ButtonRelease);
  XUngrabPointer(state::dpy, CurrentTime);
  if ((m = monitor::rect_to_mon(c->x, c->y, c->w, c->h)) != state::selmon) {
    client::send_mon(c, m);
    state::selmon = m;
    client::focus(nullptr);
  }
}
void control::quit(const Arg *arg) {
  state::running = 0;
}
void control::resizemouse(const Arg *arg) {
  int ocx, ocy, nw, nh;
  client_t *c;
  monitor_t *m;
  XEvent ev;
  Time lasttime = 0;
  
  if (!(c = state::selmon->sel))
    return;
  if (c->isfullscreen) /* no support resizing fullscreen windows by mouse */
    return;
  monitor::restack(state::selmon);
  ocx = c->x;
  ocy = c->y;
  if (XGrabPointer(state::dpy, state::root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
                   None, state::cursor[CurResize]->cursor, CurrentTime) != GrabSuccess)
    return;
  XWarpPointer(state::dpy, None, c->win, 0, 0, 0, 0, c->w + c->bw - 1, c->h + c->bw - 1);
  do {
    XMaskEvent(state::dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);
    switch (ev.type) {
      case ConfigureRequest:
      case Expose:
      case MapRequest:
        event::handler[ev.type](&ev);
        break;
      case MotionNotify:
        if ((ev.xmotion.time - lasttime) <= (1000 / 60))
          continue;
        lasttime = ev.xmotion.time;
        
        nw = MAX(ev.xmotion.x - ocx - 2 * c->bw + 1, 1);
        nh = MAX(ev.xmotion.y - ocy - 2 * c->bw + 1, 1);
        if (c->mon->wx + nw >= state::selmon->wx && c->mon->wx + nw <= state::selmon->wx + state::selmon->ww
            && c->mon->wy + nh >= state::selmon->wy && c->mon->wy + nh <= state::selmon->wy + state::selmon->wh) {
          if (!c->isfloating && state::selmon->lt[state::selmon->sellt]->arrange_
              && (abs(nw - c->w) > state::config::snap || abs(nh - c->h) > state::config::snap))
            togglefloating(NULL);
        }
        if (!state::selmon->lt[state::selmon->sellt]->arrange_ || c->isfloating)
          client::resize(c, c->x, c->y, nw, nh, 1);
        break;
    }
  } while (ev.type != ButtonRelease);
  XWarpPointer(state::dpy, None, c->win, 0, 0, 0, 0, c->w + c->bw - 1, c->h + c->bw - 1);
  XUngrabPointer(state::dpy, CurrentTime);
  while (XCheckMaskEvent(state::dpy, EnterWindowMask, &ev));
  if ((m = monitor::rect_to_mon(c->x, c->y, c->w, c->h)) != state::selmon) {
    client::send_mon(c, m);
    state::selmon = m;
    client::focus(nullptr);
  }
}
void control::setlayout(const Arg *arg) {
  unsigned int i;
  if (!arg || !arg->v || arg->v != state::selmon->lt[state::selmon->sellt])
    state::selmon->sellt ^= 1;
  if (arg && arg->v)
    state::selmon->lt[state::selmon->sellt] = (Layout *) arg->v;
  strncpy(state::selmon->ltsymbol, state::selmon->lt[state::selmon->sellt]->symbol, sizeof state::selmon->ltsymbol);
  
  for (i = 0; i <= state::config::TAGS.size(); ++i)
    if (state::selmon->tagset[state::selmon->seltags] & 1 << i) {
      state::selmon->pertag->ltidxs[(i + 1) % (state::config::TAGS.size() + 1)][state::selmon->sellt] = state::selmon->lt[state::selmon->sellt];
      state::selmon->pertag->sellts[(i + 1) % (state::config::TAGS.size() + 1)] = state::selmon->sellt;
    }
  
  if (state::selmon->sel)
    monitor::arrange(state::selmon);
  else
    bar::update_all();
}
/* arg > 1.0 will set mfact absolutely */
void control::setmfact(const Arg *arg) {
  float f;
  unsigned int i;
  
  if (!arg || !state::selmon->lt[state::selmon->sellt]->arrange_)
    return;
  f = arg->f < 1.0 ? arg->f + state::selmon->mfact : arg->f - 1.0;
  if (arg->f == 0.0)
    f = state::config::mfact;
  if (f < 0.05 || f > 0.95)
    return;
  state::selmon->mfact = f;
  for (i = 0; i <= state::config::TAGS.size(); ++i)
    if (state::selmon->tagset[state::selmon->seltags] & 1 << i)
      state::selmon->pertag->mfacts[(i + 1) % (state::config::TAGS.size() + 1)] = f;
  monitor::arrange(state::selmon);
}
void control::spawn_exec(const Arg *arg) {
  if (fork() == 0) {
    //program_shell::stop();
    if (state::dpy)
      close(ConnectionNumber(state::dpy));
    setsid();
    const char* cmd = (const char*)arg->v;
    log::debug("[spawn] running cmd: %s", cmd);
    std::system(cmd);
    log::debug("[spawn] running cmd: %s", cmd);
    fprintf(stderr, "dwm: execvp %s", cmd);
    perror(" failed");
    exit(EXIT_SUCCESS);
  }
}
void control::spawn(const Arg *arg) {
  if (state::config::cmds.find((char*)arg->v) == state::config::cmds.end()) {
    log::error("[spawn] No such command: %s", (char*)arg->v);
  }
  if (fork() == 0) {
    //program_shell::stop();
    if (state::dpy)
      close(ConnectionNumber(state::dpy));
    setsid();
    char** cmd = (char**)&state::config::cmds[(const char*)arg->v][0];
    if (strcmp((const char*)arg->v, "dmenucmd") == 0) {
      cmd[2] = (char*) std::to_string(state::selmon->num).c_str();
    }
    log::debug("[spawn] running cmd: %s", cmd[0]);
    execvp(cmd[0], cmd);
    perror(" failed");
    exit(EXIT_SUCCESS);
  }
}
void control::tag(const Arg *arg) {
  if (state::selmon->sel && arg->ui & TAGMASK) {
    state::selmon->sel->tags = arg->ui & TAGMASK;
    client::focus(nullptr);
    monitor::arrange(state::selmon);
  }
}
void control::tagmon(const Arg *arg) {
  if (!state::selmon->sel || !state::mons->next)
    return;
  client::send_mon(state::selmon->sel, monitor::dir_to_mon(arg->i));
}
void control::togglebar(const Arg *arg) {
  unsigned int i;
  state::selmon->showbar = !state::selmon->showbar;
  for (i = 0; i <= state::config::TAGS.size(); ++i)
    if (state::selmon->tagset[state::selmon->seltags] & 1 << i)
      state::selmon->pertag->showbars[(i + 1) % (state::config::TAGS.size() + 1)] = state::selmon->showbar;
  monitor::update_bar_pos(state::selmon);
  XMoveResizeWindow(state::dpy, state::selmon->barwin, state::selmon->wx+state::selmon->bar.wlen, state::selmon->by, state::selmon->ww-state::selmon->bar.wlen, state::bar_height);
  monitor::arrange(state::selmon);
}
void control::togglefloating(const Arg *arg) {
  if (!state::selmon->sel)
    return;
  if (state::selmon->sel->isfullscreen) /* no support for fullscreen windows */
    return;
  state::selmon->sel->isfloating = !state::selmon->sel->isfloating || state::selmon->sel->isfixed;
  if (state::selmon->sel->isfloating)
    client::resize(state::selmon->sel, state::selmon->sel->x, state::selmon->sel->y,
                        state::selmon->sel->w, state::selmon->sel->h, 0);
  monitor::arrange(state::selmon);
}
void control::toggletag(const Arg *arg) {
  unsigned int newtags;
  
  if (!state::selmon->sel)
    return;
  newtags = state::selmon->sel->tags ^ (arg->ui & TAGMASK);
  if (newtags) {
    state::selmon->sel->tags = newtags;
    client::focus(nullptr);
    monitor::arrange(state::selmon);
  }
}
void control::toggleview(const Arg *arg) {
  unsigned int newtagset = state::selmon->tagset[state::selmon->seltags] ^(arg->ui & TAGMASK);
  int i;
  
  if (newtagset) {
    state::selmon->tagset[state::selmon->seltags] = newtagset;
    
    if (newtagset == ~0) {
      state::selmon->pertag->prevtag = state::selmon->pertag->curtag;
      state::selmon->pertag->curtag = 0;
    }
    
    /* test if the user did not select the same tag */
    if (!(newtagset & 1 << (state::selmon->pertag->curtag - 1))) {
      state::selmon->pertag->prevtag = state::selmon->pertag->curtag;
      for (i = 0; !(newtagset & 1 << i); i++);
      state::selmon->pertag->curtag = i + 1;
    }
    
    /* apply settings for this view */
    state::selmon->nmaster = state::selmon->pertag->nmasters[state::selmon->pertag->curtag];
    state::selmon->mfact = state::selmon->pertag->mfacts[state::selmon->pertag->curtag];
    state::selmon->sellt = state::selmon->pertag->sellts[state::selmon->pertag->curtag];
    state::selmon->lt[state::selmon->sellt] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt];
    state::selmon->lt[state::selmon->sellt ^ 1] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt ^ 1];
    
    if (state::selmon->showbar != state::selmon->pertag->showbars[state::selmon->pertag->curtag])
      togglebar(NULL);
    
    client::focus(nullptr);
    monitor::arrange(state::selmon);
  }
}
static std::mutex view_mtx;
void control::view(const Arg *arg) {
  view_mtx.lock();
  int i;
  unsigned int tmptag;
  
  if ((arg->ui & TAGMASK) == state::selmon->tagset[state::selmon->seltags]) {
    view_mtx.unlock();
    return;
  }
  state::selmon->seltags ^= 1; /* toggle sel tagset */
  if (arg->ui & TAGMASK) {
    state::selmon->tagset[state::selmon->seltags] = arg->ui & TAGMASK;
    state::selmon->pertag->prevtag = state::selmon->pertag->curtag;
    
    if (arg->ui == ~0)
      state::selmon->pertag->curtag = 0;
    else {
      for (i = 0; !(arg->ui & 1 << i); i++);
      state::selmon->pertag->curtag = i + 1;
    }
  } else {
    tmptag = state::selmon->pertag->prevtag;
    state::selmon->pertag->prevtag = state::selmon->pertag->curtag;
    state::selmon->pertag->curtag = tmptag;
  }
  
  state::selmon->nmaster = state::selmon->pertag->nmasters[state::selmon->pertag->curtag];
  state::selmon->mfact = state::selmon->pertag->mfacts[state::selmon->pertag->curtag];
  state::selmon->sellt = state::selmon->pertag->sellts[state::selmon->pertag->curtag];
  state::selmon->lt[state::selmon->sellt] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt];
  state::selmon->lt[state::selmon->sellt ^ 1] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt ^ 1];
  view_mtx.unlock();
  
  if (state::selmon->showbar != state::selmon->pertag->showbars[state::selmon->pertag->curtag])
    togglebar(NULL);
  
  client::focus(nullptr);
  monitor::arrange(state::selmon);
  bar::update_all();
}
void control::zoom(const Arg *arg) {
  client_t *c = state::selmon->sel;
  
  if (!state::selmon->lt[state::selmon->sellt]->arrange_
      || (state::selmon->sel && state::selmon->sel->isfloating))
    return;
  if (c == client::next_tiled(state::selmon->clients))
    if (!c || !(c = client::next_tiled(c->next)))
      return;
  client::pop(c);
}

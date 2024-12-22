/*! \file  resizemouse.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.resizemouse;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void resizemouse(const Arg *arg) {
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
}
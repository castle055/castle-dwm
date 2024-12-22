/*! \file  movemouse.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.movemouse;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void movemouse(const Arg *arg) {
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
}
//
// Created by castle on 3/24/22.
//


export module layouts.tile;

import cyd_wm.types;
import cyd_wm.types;
import cyd_wm.clients.manager;
import cyd_wm.monitors.stack;

export void tile(cyd_wm::Monitor::sptr monitor) {
  using namespace cyd_wm;
  unsigned int i, n, h, mw, my, ty;
  Client::sptr c;


  for (n = 0, c = monitors::next_in_stack(monitor->clients.front()); c; c = ops::client::next_tiled(c->next), n++);
  if (n == 0)
    return;
  
  if (n > monitor->nmaster)
    mw = monitor->nmaster ? monitor->ww * monitor->mfact : 0;
  else
    mw = monitor->ww - state::config::gappx;
  for (i = 0, my = ty = state::config::gappx, c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(
      c->next), i++)
    if (i < monitor->nmaster) {
      h = (monitor->wh - my) / (MIN(n, monitor->nmaster) - i) - state::config::gappx;
      ops::client::resize(c, monitor->wx + state::config::gappx, monitor->wy + my, mw - (2 * c->bw) - state::config::gappx, h - (2 * c->bw), 0);
      my += HEIGHT(c) + state::config::gappx;
    } else {
      h = (monitor->wh - ty) / (n - i) - state::config::gappx;
      ops::client::resize(c, monitor->wx + mw + state::config::gappx, monitor->wy + ty, monitor->ww - mw - (2 * c->bw) - 2 * state::config::gappx, h - (2 * c->bw), 0);
      ty += HEIGHT(c) + state::config::gappx;
    }
}

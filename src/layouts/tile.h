//
// Created by castle on 3/24/22.
//

#ifndef CASTLE_DWM_TILE_H
#define CASTLE_DWM_TILE_H

#include <cstdio>

#include "../state/dwm_types.h"
#include "../state/state.h"
#include "../ops/client_ops.h"
#include "../util.h"

void tile(monitor_t* monitor) {
  unsigned int i, n, h, mw, my, ty;
  client_t *c;
  
  for (n = 0, c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(c->next), n++);
  if (n == 0)
    return;
  
  if (n > monitor->nmaster)
    mw = monitor->nmaster ? monitor->ww * monitor->mfact : 0;
    //mw = m->nmaster ? (m->ww - (g = gappx)) * m->mfact : 0;
  else
    mw = monitor->ww - state::config::gappx;
  for (i = 0, my = ty = state::config::gappx, c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(
      c->next), i++)
    if (i < monitor->nmaster) {
//<<<<<<< HEAD
      //h = (m->wh - my) / (MIN(n, m->nmaster) - i);
      h = (monitor->wh - my) / (MIN(n, monitor->nmaster) - i) - state::config::gappx;
      ops::client::resize(c, monitor->wx + state::config::gappx, monitor->wy + my, mw - (2 * c->bw) - state::config::gappx, h - (2 * c->bw), 0);
      my += HEIGHT(c) + state::config::gappx;
    } else {
      //h = (m->wh - ty) / (n - i);
      h = (monitor->wh - ty) / (n - i) - state::config::gappx;
      ops::client::resize(c, monitor->wx + mw + state::config::gappx, monitor->wy + ty, monitor->ww - mw - (2 * c->bw) - 2 * state::config::gappx, h - (2 * c->bw), 0);
      ty += HEIGHT(c) + state::config::gappx;
//=======
//			h = (m->wh - my) / (MIN(n, m->nmaster) - i);
//			resize(c, m->wx, m->wy + my, mw - (2*c->bw), h - (2*c->bw), 0);
//			if (my + HEIGHT(c) < m->wh)
//				my += HEIGHT(c);
//		} else {
//			h = (m->wh - ty) / (n - i);
//			resize(c, m->wx + mw, m->wy + ty, m->ww - mw - (2*c->bw), h - (2*c->bw), 0);
//			if (ty + HEIGHT(c) < m->wh)
//				ty += HEIGHT(c);
//>>>>>>> f09418bbb6651ab4c299cfefbe1d18de401f630e
    }
}

#endif //CASTLE_DWM_TILE_H

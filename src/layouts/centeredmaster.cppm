//
// Created by castle on 3/24/22.
//

export module layouts.centered_master;

import std.compat;

#include "../state/dwm_types.h"
#include "../ops/client_ops.h"
#include "../util.h"

export void centeredmaster(monitor_t* monitor) {
  unsigned int i, n, h, mw, mx, my, oty, ety, tw;
  client_t *c;
  
  /* count number of clients in the selected monitor */
  for (n = 0, c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(c->next), n++);
  if (n == 0)
    return;
  
  /* initialize areas */
  mw = monitor->ww;
  mx = 0;
  my = 0;
  tw = mw;
  
  if (n > monitor->nmaster) {
    /* go mfact box in the center if more than nmaster clients */
    mw = monitor->nmaster ? monitor->ww * monitor->mfact : 0;
    tw = monitor->ww - mw;
    
    if (n - monitor->nmaster > 1) {
      /* only one client */
      mx = (monitor->ww - mw) / 2;
      tw = (monitor->ww - mw) / 2;
    }
  }
  
  oty = 0;
  ety = 0;
  for (i = 0, c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(c->next), i++)
    if (i < monitor->nmaster) {
      /* nmaster clients are stacked vertically, in the center
       * of the screen */
      h = (monitor->wh - my) / (MIN(n, monitor->nmaster) - i);
      ops::client::resize(c, monitor->wx + mx, monitor->wy + my, mw - (2 * c->bw),
                     h - (2 * c->bw), 0);
      my += HEIGHT(c);
    } else {
      /* stack clients are stacked vertically */
      if ((i - monitor->nmaster) % 2) {
        h = (monitor->wh - ety) / ((1 + n - i) / 2);
        ops::client::resize(c, monitor->wx, monitor->wy + ety, tw - (2 * c->bw),
                       h - (2 * c->bw), 0);
        ety += HEIGHT(c);
      } else {
        h = (monitor->wh - oty) / ((1 + n - i) / 2);
        ops::client::resize(c, monitor->wx + mx + mw, monitor->wy + oty,
                       tw - (2 * c->bw), h - (2 * c->bw), 0);
        oty += HEIGHT(c);
      }
    }
}

void centeredfloatingmaster(monitor_t* monitor) {
  unsigned int i, n, w, mh, mw, mx, mxo, my, myo, tx;
  client_t *c;
  
  /* count number of clients in the selected monitor */
  for (n = 0, c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(c->next), n++);
  if (n == 0)
    return;
  
  /* initialize nmaster area */
  if (n > monitor->nmaster) {
    /* go mfact box in the center if more than nmaster clients */
    if (monitor->ww > monitor->wh) {
      mw = monitor->nmaster ? monitor->ww * monitor->mfact : 0;
      mh = monitor->nmaster ? monitor->wh * 0.9 : 0;
    } else {
      mh = monitor->nmaster ? monitor->wh * monitor->mfact : 0;
      mw = monitor->nmaster ? monitor->ww * 0.9 : 0;
    }
    mx = mxo = (monitor->ww - mw) / 2;
    my = myo = (monitor->wh - mh) / 2;
  } else {
    /* go fullscreen if all clients are in the master area */
    mh = monitor->wh;
    mw = monitor->ww;
    mx = mxo = 0;
    my = myo = 0;
  }
  
  for (i = tx = 0, c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(c->next), i++)
    if (i < monitor->nmaster) {
      /* nmaster clients are stacked horizontally, in the center
       * of the screen */
      w = (mw + mxo - mx) / (MIN(n, monitor->nmaster) - i);
      ops::client::resize(c, monitor->wx + mx, monitor->wy + my, w - (2 * c->bw),
                     mh - (2 * c->bw), 0);
      mx += WIDTH(c);
    } else {
      /* stack clients are stacked horizontally */
      w = (monitor->ww - tx) / (n - i);
      ops::client::resize(c, monitor->wx + tx, monitor->wy, w - (2 * c->bw),
                     monitor->wh - (2 * c->bw), 0);
      tx += WIDTH(c);
    }
}

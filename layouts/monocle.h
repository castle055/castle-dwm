//
// Created by castle on 3/24/22.
//

#ifndef CASTLE_DWM_MONOCLE_H
#define CASTLE_DWM_MONOCLE_H

#include <cstdio>

#include "../state/dwm_types.h"
#include "../ops/client_ops.h"

void monocle(monitor_t* monitor) {
  unsigned int n = 0;
  client_t* c;
  
  for (c = monitor->clients; c; c = c->next)
    if (ISVISIBLE(c))
      n++;
  if (n > 0) /* override layout symbol */
    snprintf(monitor->ltsymbol, sizeof monitor->ltsymbol, "[%d]", n);
  for (c = ops::client::next_tiled(monitor->clients); c; c = ops::client::next_tiled(c->next))
    ops::client::resize(c, monitor->wx, monitor->wy, monitor->ww - 2 * c->bw, monitor->wh - 2 * c->bw, 0);
}

#endif //CASTLE_DWM_MONOCLE_H

//
// Created by castle on 3/24/22.
//

export module layouts.monocle;

#include <cstdio>

#include "../state/dwm_types.h"
#include "../ops/client_ops.h"

module;
#define ISVISIBLE(C)            (((C)->tags & (C)->mon->tagset[(C)->mon->seltags]))
export void monocle(monitor_t* monitor) {
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

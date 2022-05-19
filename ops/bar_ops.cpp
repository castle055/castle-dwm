//
// Created by castle on 5/19/22.
//

#include "bar_ops.h"
#include "x11_ops.h"
#include "../state/state.h"

using namespace ops;

std::unordered_map<int, bar_t> bars;

void bar::init_where_needed() {
  x11::update_bars();
  monitor_t* m;
  for (m = state::mons; m; m = m->next) {
    if (m->barwin)
      continue;
    m->barwin = x11::create_barwin(m->wx, m->by, m->ww);
    bars[m->num] = { m->barwin };
  }
}

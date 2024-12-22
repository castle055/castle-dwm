/*! \file  zoom.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.zoom;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void zoom(const Arg *arg) {
    client_t *c = state::selmon->sel;

    if (!state::selmon->lt[state::selmon->sellt]->arrange_
        || (state::selmon->sel && state::selmon->sel->isfloating))
      return;
    if (c == client::next_tiled(state::selmon->clients))
      if (!c || !(c = client::next_tiled(c->next)))
        return;
    client::pop(c);
  }
}
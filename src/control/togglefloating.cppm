/*! \file  togglefloating.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.togglefloating;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void togglefloating(const Arg *arg) {
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
}
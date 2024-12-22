/*! \file  tagmon.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.tagmon;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void tagmon(const Arg *arg) {
    if (!state::selmon->sel || !state::mons->next)
      return;
    client::send_mon(state::selmon->sel, monitor::dir_to_mon(arg->i));
  }
}
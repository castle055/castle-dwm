/*! \file  tag.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.tag;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void tag(const Arg *arg) {
    if (state::selmon->sel && arg->ui & TAGMASK) {
      state::selmon->sel->tags = arg->ui & TAGMASK;
      client::focus(nullptr);
      monitor::arrange(state::selmon);
    }
  }
}
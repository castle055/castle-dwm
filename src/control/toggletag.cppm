/*! \file  toggletag.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.toggletag;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void toggletag(const Arg *arg) {
    unsigned int newtags;

    if (!state::selmon->sel)
      return;
    newtags = state::selmon->sel->tags ^ (arg->ui & TAGMASK);
    if (newtags) {
      state::selmon->sel->tags = newtags;
      client::focus(nullptr);
      monitor::arrange(state::selmon);
    }
  }
}
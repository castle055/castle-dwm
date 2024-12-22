/*! \file  focusstack.cppm
 *! \brief 
 *!
 */

module;
#define ISVISIBLE(C)            (((C)->tags & (C)->mon->tagset[(C)->mon->seltags]))
export module cyd_wm.control.focusstack;

import cyd_wm.types;

import cyd_wm.state.monitors;

import cyd_wm.clients.focus;
import cyd_wm.monitors.stack;

namespace cyd_wm::control {
  export
  void focusstack(const Arg *arg) {
    if (!state::monitors::selected_monitor->sel)
      return;

    Client::sptr c = arg->i > 0
                     ? monitors::next_in_stack(state::monitors::selected_monitor->sel, true)
                     : monitors::prev_in_stack(state::monitors::selected_monitor->sel, true);

    if (c) {
      clients::focus(c);
      monitors::restack(state::monitors::selected_monitor);
    }
  }
}
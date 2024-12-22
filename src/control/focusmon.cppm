/*! \file  focusmon.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.focusmon;

import cyd_wm.types;

import cyd_wm.state.monitors;

import cyd_wm.monitors.manager;
import cyd_wm.clients.focus;

namespace cyd_wm::control {
  export
  void focusmon(const Arg *arg) {
    if (state::monitors::monitors.size() <= 1)
      return;
    Monitor::sptr m = monitors::dir_to_mon(arg->i);
    if (m == state::monitors::selected_monitor)
      return;
    clients::unfocus(state::monitors::selected_monitor->sel, 0);
    state::monitors::selected_monitor = m;
    clients::focus(nullptr);
  }
}
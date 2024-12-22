/*! \file  killclient.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.killclient;

import cyd_wm.types;

import cyd_wm.state.monitors;
import cyd_wm.state.atoms;

import x11;
import cyd_wm.clients.utils;

namespace cyd_wm::control {
  export
  void killclient(const Arg *arg) {
    if (!state::monitors::selected_monitor->sel)
      return;
    if (!clients::send_event(state::monitors::selected_monitor->sel, state::atoms::wmatom[WMDelete])) {
      x11::scope_server_lock server_lock{};
      x11::scope_error_disabler error_disabler{};

      x11::set_close_down_mode(x11::close_mode::DESTROY_ALL);
      x11::kill_client(state::monitors::selected_monitor->sel->win);
      x11::sync(false);
    }
  }
}
/*! \file  quit.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.quit;

import cyd_wm.types;

import cyd_wm.state.misc;

namespace cyd_wm::control {
  export
  void quit(const Arg *arg) {
    state::running = false;
  }
}
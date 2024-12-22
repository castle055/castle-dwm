/*! \file  incmaster.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.incmaster;

import std;
import cyd_wm.types;

import cyd_wm.state.monitors;
import cyd_wm.state.config;

import cyd_wm.layouts;

namespace cyd_wm::control {
  export
  void incnmaster(const Arg *arg) {
    state::monitors::selected_monitor->nmaster = std::max(state::monitors::selected_monitor->nmaster + arg->i, 0);

    for (unsigned int i = 0; i <= state::config::TAGS.size(); ++i) {
      if (state::monitors::selected_monitor->tagset[state::monitors::selected_monitor->seltags] & 1 << i) {
        state::monitors::selected_monitor->tag_config[(i + 1) % (state::config::TAGS.size() + 1)].nmasters =
          state::monitors::selected_monitor->nmaster;
      }
    }

    layouts::arrange(state::monitors::selected_monitor);
  }
}
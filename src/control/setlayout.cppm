/*! \file  setlayout.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.setlayout;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void setlayout(const Arg *arg) {
    unsigned int i;
    if (!arg || !arg->v || arg->v != state::selmon->lt[state::selmon->sellt])
      state::selmon->sellt ^= 1;
    if (arg && arg->v)
      state::selmon->lt[state::selmon->sellt] = (Layout *) arg->v;
    strncpy(state::selmon->ltsymbol, state::selmon->lt[state::selmon->sellt]->symbol, sizeof state::selmon->ltsymbol);

    for (i = 0; i <= state::config::TAGS.size(); ++i)
      if (state::selmon->tagset[state::selmon->seltags] & 1 << i) {
        state::selmon->pertag->ltidxs[(i + 1) % (state::config::TAGS.size() + 1)][state::selmon->sellt] = state::selmon->lt[state::selmon->sellt];
        state::selmon->pertag->sellts[(i + 1) % (state::config::TAGS.size() + 1)] = state::selmon->sellt;
      }

    if (state::selmon->sel)
      monitor::arrange(state::selmon);
    else
      bar::update_all();
  }
}
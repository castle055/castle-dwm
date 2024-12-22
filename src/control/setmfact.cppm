/*! \file  setmfact.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.setmfact;

import cyd_wm.types;

namespace cyd_wm::control {
  /* arg > 1.0 will set mfact absolutely */
  export
  void setmfact(const Arg *arg) {
    float f;
    unsigned int i;

    if (!arg || !state::selmon->lt[state::selmon->sellt]->arrange_)
      return;
    f = arg->f < 1.0 ? arg->f + state::selmon->mfact : arg->f - 1.0;
    if (arg->f == 0.0)
      f = state::config::mfact;
    if (f < 0.05 || f > 0.95)
      return;
    state::selmon->mfact = f;
    for (i = 0; i <= state::config::TAGS.size(); ++i)
      if (state::selmon->tagset[state::selmon->seltags] & 1 << i)
        state::selmon->pertag->mfacts[(i + 1) % (state::config::TAGS.size() + 1)] = f;
    monitor::arrange(state::selmon);
  }
}
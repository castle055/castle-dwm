/*! \file  view.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.view;

import std;
import cyd_wm.types;

namespace cyd_wm::control {
  static std::mutex view_mtx;
  export
  void view(const Arg *arg) {
    std::lock_guard lk{view_mtx};
    int i;
    unsigned int tmptag;

    if ((arg->ui & TAGMASK) == state::selmon->tagset[state::selmon->seltags]) {
      return;
    }
    state::selmon->seltags ^= 1; /* toggle sel tagset */
    if (arg->ui & TAGMASK) {
      state::selmon->tagset[state::selmon->seltags] = arg->ui & TAGMASK;
      state::selmon->pertag->prevtag = state::selmon->pertag->curtag;

      if (arg->ui == ~0)
        state::selmon->pertag->curtag = 0;
      else {
        for (i = 0; !(arg->ui & 1 << i); i++);
        state::selmon->pertag->curtag = i + 1;
      }
    } else {
      tmptag = state::selmon->pertag->prevtag;
      state::selmon->pertag->prevtag = state::selmon->pertag->curtag;
      state::selmon->pertag->curtag = tmptag;
    }

    state::selmon->nmaster = state::selmon->pertag->nmasters[state::selmon->pertag->curtag];
    state::selmon->mfact = state::selmon->pertag->mfacts[state::selmon->pertag->curtag];
    state::selmon->sellt = state::selmon->pertag->sellts[state::selmon->pertag->curtag];
    state::selmon->lt[state::selmon->sellt] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt];
    state::selmon->lt[state::selmon->sellt ^ 1] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt ^ 1];

    if (state::selmon->showbar != state::selmon->pertag->showbars[state::selmon->pertag->curtag])
      togglebar(NULL);

    client::focus(nullptr);
    monitor::arrange(state::selmon);
    bar::update_all();
  }
}
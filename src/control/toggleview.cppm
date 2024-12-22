/*! \file  toggleview.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.toggleview;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void toggleview(const Arg *arg) {
    unsigned int newtagset = state::selmon->tagset[state::selmon->seltags] ^(arg->ui & TAGMASK);
    int i;

    if (newtagset) {
      state::selmon->tagset[state::selmon->seltags] = newtagset;

      if (newtagset == ~0) {
        state::selmon->pertag->prevtag = state::selmon->pertag->curtag;
        state::selmon->pertag->curtag = 0;
      }

      /* test if the user did not select the same tag */
      if (!(newtagset & 1 << (state::selmon->pertag->curtag - 1))) {
        state::selmon->pertag->prevtag = state::selmon->pertag->curtag;
        for (i = 0; !(newtagset & 1 << i); i++);
        state::selmon->pertag->curtag = i + 1;
      }

      /* apply settings for this view */
      state::selmon->nmaster = state::selmon->pertag->nmasters[state::selmon->pertag->curtag];
      state::selmon->mfact = state::selmon->pertag->mfacts[state::selmon->pertag->curtag];
      state::selmon->sellt = state::selmon->pertag->sellts[state::selmon->pertag->curtag];
      state::selmon->lt[state::selmon->sellt] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt];
      state::selmon->lt[state::selmon->sellt ^ 1] = state::selmon->pertag->ltidxs[state::selmon->pertag->curtag][state::selmon->sellt ^ 1];

      if (state::selmon->showbar != state::selmon->pertag->showbars[state::selmon->pertag->curtag])
        togglebar(NULL);

      client::focus(nullptr);
      monitor::arrange(state::selmon);
    }
  }
}
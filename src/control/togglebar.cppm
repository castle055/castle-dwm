/*! \file  togglebar.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.togglebar;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void togglebar(const Arg *arg) {
    //unsigned int i;
    //state::selmon->showbar = !state::selmon->showbar;
    //for (i = 0; i <= state::config::TAGS.size(); ++i)
    //  if (state::selmon->tagset[state::selmon->seltags] & 1 << i)
    //    state::selmon->pertag->showbars[(i + 1) % (state::config::TAGS.size() + 1)] = state::selmon->showbar;
    monitor::update_bar_pos(state::selmon);
    //XMoveResizeWindow(state::dpy, state::selmon->barwin, state::selmon->wx+state::selmon->bar.wlen, state::selmon->by, state::selmon->ww-state::selmon->bar.wlen, state::bar_height);
    monitor::arrange(state::selmon);
  }
}
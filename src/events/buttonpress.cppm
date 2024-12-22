/*! \file  buttonpress.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
#define CLEANMASK(mask)         (mask & ~(state::numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))

export module cyd_wm.events.handlers.buttonpress;

import x11;
import cyd_wm.types;

import cyd_wm.state.config;
import cyd_wm.state.monitors;
import cyd_wm.state.misc;

import cyd_wm.clients.manager;
import cyd_wm.clients.focus;
import cyd_wm.monitors.manager;
import cyd_wm.monitors.stack;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void buttonpress(x11::event_t* e) {
    const Arg arg = {0};
    x11::XButtonPressedEvent* ev = &e->xbutton;

    unsigned int click = ClkRootWin;
    /* focus monitor if necessary */
    if (Monitor::sptr m = monitors::from_window(ev->window); m && m != state::monitors::selected_monitor) {
      clients::unfocus(state::monitors::selected_monitor->sel, 1);
      state::monitors::selected_monitor = m;
      clients::focus(nullptr);
    }

    if (Client::sptr c = clients::from_window(ev->window)) {
      clients::focus(c);
      monitors::restack(state::monitors::selected_monitor);
      x11::allow_events(ReplayPointer, CurrentTime);
      click = ClkClientWin;
    }
    for (unsigned int i = 0; i < state::config::buttons.size(); i++)
      if (click == state::config::buttons[i].click && state::config::buttons[i].func && state::config::buttons[i].button
          == ev->button
          && CLEANMASK(state::config::buttons[i].mask) == CLEANMASK(ev->state))
        state::config::buttons[i].func(click == ClkTagBar && state::config::buttons[i].arg.i == 0
                                       ? &arg
                                       : &state::config::buttons[i].arg);
  }
}
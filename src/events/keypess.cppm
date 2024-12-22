/*! \file  keypess.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
#define CLEANMASK(mask)         (mask & ~(state::numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
export module cyd_wm.events.handlers.keypess;

import x11;
import cyd_wm.types;

import cyd_wm.state.misc;
import cyd_wm.state.config;

import keynav;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void keypress(x11::event_t* e) {
    XKeyEvent* ev = &e->xkey;
    if (keynav::process(ev)) return;

    KeySym keysym = x11::lookup_keysym(ev, 0);

    // If not KeyNav, check normal shortcuts
    for (unsigned int i = 0; i < state::config::keys.size(); i++) {
      //if (((keysym == state::config::keys[i].keysym
      //    && CLEANMASK(state::config::keys[i].mod) == CLEANMASK(ev->state))
      //    || (state::config::keys[i].keysym == XK_Meta_L
      //    || state::config::keys[i].keysym == XK_Meta_R))
      if (keysym == state::config::keys[i].keysym
          && CLEANMASK(state::config::keys[i].mod) == CLEANMASK(ev->state)
          && state::config::keys[i].func)
        state::config::keys[i].func(&(state::config::keys[i].arg));
    }
  }
}
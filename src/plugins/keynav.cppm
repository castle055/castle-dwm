/*! \file  keynav.cppm
 *! \brief 
 *!
 */

module;
#include <X11/keysymdef.h>

export module keynav;

import cyd_wm.types;
import :file;

#define IKT(NAME, DESC, FUNC) \
key_nav_target NAME = { \
    .description = #DESC, \
    .type = INTERNAL_VOID_BINDING, \
    .void_binding = FUNC, \
}

namespace keynav {
  key_nav_target dmenu_target = {
    .description = "dmenu",
    .type = APPLICATION,
    .func = control::spawn,
    .arg = {.v = "dmenucmd"},
  };

  IKT(int_reload_kn, reload keynav, file::reload_key_nav);
  key_nav_target internal = {
    .description = "internal",
    .type = NAV_DIR,
    .map = {
      {XK_r, int_reload_kn},
    }
  };

  key_nav_target keynav::root { };
}

export namespace keynav {
  void reset_map() {
    root.map.clear();
    root.map.emplace(XK_i, internal);
    root.map.emplace(XK_d, dmenu_target);
  }

  void reset() {
    state::key_nav::accepting = false;
    state::key_nav::current = &keynav::root;
    state::key_nav::current_path = "|>";
    XUngrabKeyboard(state::dpy, CurrentTime);
    x11::grab_keys();
    bar::update_all();
    log::info("[keypress] reset");
  }

  bool process(XKeyEvent* ev) {
    KeySym keysym = XKeycodeToKeysym(state::dpy, (KeyCode) ev->keycode, 0);
    // Navigate if KeyNav active
    if (state::key_nav::accepting) {
      //log::info("[key_nav] gotcha: %d", keysym);
      // Check if KeyNav trigger
      //log::info("[keypress] KeySym: %s", XKeysymToString(keysym));
      if ((keysym == state::config::key_nav::trigger.keysym
        && CLEANMASK(state::config::key_nav::trigger.mod) == CLEANMASK(ev->state))
        || keysym == XK_Escape) {
        reset();
        return true;
        }

      auto next_target = state::key_nav::current->map.find(keysym);
      if (next_target != state::key_nav::current->map.end()) {
        key_nav_target* target = &next_target->second;
        switch (target->type) {
          case NAV_DIR:
            state::key_nav::current = target;
          state::key_nav::current_path.append(" ");
          state::key_nav::current_path.append(XKeysymToString(keysym));
          ops::bar::update_all();
          break;
          case APPLICATION:
            log::info("[key_nav] running app");
          if (target->func) {
            target->func(&target->arg);
          }
          reset();
          break;
          case INTERNAL_VOID_BINDING:
            log::info("[key_nav] executing internal void binding");
          if (target->void_binding) {
            target->void_binding();
          }
          reset();
          break;
        }
      }
      return true;
    } else {
      // Check if KeyNav trigger
      //log::info("[keypress] KeySym: %s", XKeysymToString(keysym));
      if (keysym == state::config::key_nav::trigger.keysym
          && CLEANMASK(state::config::key_nav::trigger.mod) == CLEANMASK(ev->state)) {
        state::key_nav::accepting = true;
        log::info("[keypress] key_nav accepting");
        XUngrabKey(state::dpy, AnyKey, AnyModifier, state::root);
        XGrabKeyboard(state::dpy, state::root, True, GrabModeAsync, GrabModeAsync, CurrentTime);
        ops::bar::update_all();
        return true;
          }
    }

    return false;
  }
}

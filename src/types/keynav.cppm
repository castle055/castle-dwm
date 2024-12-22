/*! \file  keynav.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:keynav;

import std;
import :arg;
import x11.types;

export namespace keynav {
  enum KeyNavTargetType {
    NAV_DIR,
    APPLICATION,
    INTERNAL_VOID_BINDING,
  };

  struct key_nav_target {
    std::string description = "";

    KeyNavTargetType type;
    std::unordered_map<x11::keysym_t, key_nav_target> map;

    void (* func)(const cyd_wm::Arg*) = nullptr;

    const cyd_wm::Arg arg;

    void (* void_binding)() = nullptr;
  };
  using key_nav_map = std::unordered_map<x11::keysym_t, key_nav_target>;
}

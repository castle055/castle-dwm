/*! \file  mouse_button.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:mouse_button;

import :arg;

export namespace cyd_wm {
 struct MouseButton {
  unsigned int click{};
  unsigned int mask{};
  unsigned int button{};

  void (* func)(const Arg* arg){};

  const Arg arg{};
 };
}

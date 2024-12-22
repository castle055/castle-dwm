/*! \file  mappingnotify.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xutil.h>
export module cyd_wm.events.handlers.mappingnotify;

import x11;
import cyd_wm.types;

import cyd_wm.x11_utils;
import keynav;

namespace event_handlers {
  using namespace cyd_wm;

  export
  void mappingnotify(::x11::event_t* e) {
    ::x11::XMappingEvent* ev = &e->xmapping;

    XRefreshKeyboardMapping(ev);
    if (ev->request == MappingKeyboard) {
      cyd_wm::x11::grab_keys();
      keynav::reset();
    }
  }
}
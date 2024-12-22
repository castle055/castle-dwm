/*! \file  key_trigger.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:key_trigger;

import x11.types;

export namespace cyd_wm {
  struct KeyTrigger {
    unsigned int mod;
    x11::keysym_t keysym;
  };
}
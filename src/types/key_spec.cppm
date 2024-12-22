/*! \file  key_spec.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:key_spec;

import :arg;
import x11.types;

export namespace cyd_wm {
  struct KeySpec {
    unsigned int mod;
    x11::keysym_t keysym;

    void (* func)(const Arg*);

    const Arg arg;
  };
}

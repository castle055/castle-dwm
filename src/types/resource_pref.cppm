/*! \file  resource_pref.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:resource_pref;

import :enums;

export namespace cyd_wm {
  struct ResourcePref {
    char* name;
    resource_type type;
    void* dst;
  };
}
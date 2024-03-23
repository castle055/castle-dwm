//
// Created by castle on 2/1/24.
//

#ifndef CYD_WM_KEYNAV_TYPES_H
#define CYD_WM_KEYNAV_TYPES_H

#include "core_types.h"

enum KeyNavTargetType {
  NAV_DIR,
  APPLICATION,
  INTERNAL_VOID_BINDING,
};

struct key_nav_target {
  std::string description = "";
  
  KeyNavTargetType type;
  std::unordered_map<KeySym, key_nav_target*> map;
  
  void (*func)(const Arg *) = nullptr;
  const Arg arg;
  
  void (*void_binding)() = nullptr;
};
typedef std::unordered_map<KeySym, key_nav_target*> key_nav_map;

#endif //CYD_WM_KEYNAV_TYPES_H

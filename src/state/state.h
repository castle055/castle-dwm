//
// Created by castle on 3/24/22.
//

#ifndef CASTLE_DWM_STATE_H
#define CASTLE_DWM_STATE_H

#include <X11/Xlib.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "dwm_types.h"
#include "../drw.h"

namespace state {
  extern std::fstream log_file;
  extern std::fstream key_nav_file;
  
  extern const char broken[];
  
  extern std::string stext;
  
  extern Atom wmatom[WMLast], netatom[NetLast];
  extern Display* dpy;
  extern int screen;
  extern Drw *drw;
  extern monitor_t *mons, *selmon;
  extern Window root, wmcheckwin;
  extern Clr **scheme;
  extern int sw, sh, bar_height, blw;
  extern int lrpad;
  extern unsigned int numlockmask;
  extern Cur *cursor[CurLast];
  
  extern bool running;
  
  namespace key_nav {
    extern bool accepting;
    extern std::string current_path;
    extern key_nav_target* current;
  }
  namespace config {
    extern std::string log_file;
    extern std::string key_nav_file;
    extern unsigned short ctl_port;
    extern unsigned int borderpx;
    extern unsigned int snap;
    extern int showbar;
    extern int topbar;
    extern std::vector<const char*> fonts;
    extern std::string dmenufont;
    extern std::string normbgcolor;
    extern std::string selbgcolor;
    extern std::string normbordercolor;
    extern std::string selbordercolor;
    extern std::string normfgcolor;
    extern std::string selfgcolor;
    extern std::vector<std::vector<const char*>> colors;
    extern unsigned int gappx;
    extern const std::vector<std::string> TAGS;
    extern std::vector<Rule> rules;
    extern float mfact;
    extern int nmaster;
    extern int resizehints;
    extern std::vector<Layout> layouts;
    extern unsigned int default_layout;
    extern std::array<std::array<DefaultLayout, 9>, 3> default_layouts;
    extern void setup_default_layouts();
    extern unsigned int dmenumon;
    extern std::unordered_map<std::string, std::vector<const char*>> cmds;
    extern std::vector<Key>          keys;
    extern std::vector<MouseButton>  buttons;
    extern std::vector<ResourcePref> resources;
    
    namespace key_nav {
      extern long accepting_timespan;
      extern KeyTrigger trigger;
    }
  }
}


#endif //CASTLE_DWM_STATE_H

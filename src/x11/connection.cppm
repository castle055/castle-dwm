/*! \file  connection.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xlib.h>

export module x11.connection;

export import x11.types;

import fabric.logging;

namespace x11::connection {
 Display* display_ = nullptr;
 int screen_;
 Window root_;

 bool connection_established_ = false;

 void connect_if_needed() {
  if (!display_) {
   display_ = XOpenDisplay(nullptr);
   if (!display_) {
    LOG::print {FATAL}("Could not open display");
    std::exit(1);
   }
   screen_  = DefaultScreen(display_);
   root_ = DefaultRootWindow(display_);
  }
 }
}

export namespace x11::connection {
 display_t display() {
  connect_if_needed();
  return display_;
 }

 screen_t screen() {
  connect_if_needed();
  return screen_;
 }

 window_t root() {
  connect_if_needed();
  return root_;
 }
}

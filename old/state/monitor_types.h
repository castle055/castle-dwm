//
// Created by castle on 2/1/24.
//

#ifndef CYD_WM_MONITOR_TYPES_H
#define CYD_WM_MONITOR_TYPES_H

#include "../statusbar/types.h"

struct monitor_bar_t {
  bool init = false;
  int wlen = 290 + 360;

  // Workspaces
  cyd_wm::WorkspaceStatus wstatus;
  //cydui::layout::Layout* wlay = nullptr;
  cydui::window::CWindow* wwin = nullptr;
  //cydui::layout::Layout* tlay = nullptr;
  //cydui::window::CWindow* twin = nullptr;
};

struct monitor_t {
  char ltsymbol[16];
  float mfact = 0;
  int nmaster = 0;
  int num = 0;
  int by = 0;               /* bar geometry */
  int mx = 0, my = 0, mw = 0, mh = 0;   /* screen size */
  int wx = 0, wy = 0, ww = 0, wh = 0;   /* window area  */
  int gappx = 0;            /* gaps between windows */
  unsigned int seltags = 0;
  unsigned int sellt = 0;
  unsigned int tagset[2];
  int showbar = 0;
  int topbar = 0;
  client_t *clients = nullptr;
  client_t *sel = nullptr;
  client_t *stack = nullptr;
  monitor_t *next = nullptr;
  const Layout *lt[2];
  Pertag *pertag = nullptr;

  monitor_bar_t bar;
  unsigned long barwin = 0;
};

struct bar_t {
  int width;
  monitor_t* monitor;
  Window win;
  Window suggestion_window;
};

#endif //CYD_WM_MONITOR_TYPES_H

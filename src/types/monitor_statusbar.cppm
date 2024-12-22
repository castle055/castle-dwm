/*! \file  monitor_statusbar.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:monitor_statusbar;

import statusbar;

export namespace cyd_wm {
 class MonitorStatusBarConfig {
 public:
  bool show_bar{true};
  bool top_bar{true};
  int bar_height{26};
 };

 class MonitorStatusBar {
 public:
  bool init = false;
  int wlen = 290 + 360;

  // Workspaces
  cyd_wm::WorkspaceStatus wstatus;
  cydui::window::CWindow* wwin = nullptr;
 };
}
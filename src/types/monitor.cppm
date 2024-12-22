/*! \file  monitor.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:monitor;

import std;

import :tag;
import :action_history;
import :monitor_statusbar;

export namespace cyd_wm {
 class Layout;
 class Client;

 class Monitor {
 public:
  void update_statusbar_position(MonitorStatusBarConfig config) {
    wy = my;
    wh = mh;
    if (config.show_bar) {
      wh -= config.bar_height;
      by = config.top_bar ? wy : wy + wh;
      wy = config.top_bar ? wy + config.bar_height : wy;
    } else
      by = -config.bar_height;
  }

 public:
  using sptr = std::shared_ptr<Monitor>;

  std::string ltsymbol;
  float mfact          = 0;
  int nmaster          = 0;
  int num              = 0;
  int by               = 0;                         /* bar geometry */
  int mx               = 0, my = 0, mw = 0, mh = 0; /* screen size */
  int wx               = 0, wy = 0, ww = 0, wh = 0; /* window area  */
  int gappx            = 0;                         /* gaps between windows */
  unsigned int seltags = 0;
  unsigned int sellt   = 0;
  unsigned int tagset[2];
  int showbar = 0;
  int topbar  = 0;
  std::deque<std::shared_ptr<Client>> clients{};
  std::shared_ptr<Client> sel = nullptr;
  std::deque<std::shared_ptr<Client>> stack { };
  const Layout* lt[2];

  MonitorStatusBar bar;
  unsigned long barwin = 0;

  std::array<TagConfiguration, 9> tag_config{};
  TagSelection<9> tag_selection{};
  action_history<TagSelection<9>, 10> tag_selection_history{};
 };
}

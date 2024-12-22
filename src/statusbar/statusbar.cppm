/*! \file  statusbar.cppm
 *! \brief 
 *!
 */

export module cyd_wm.statusbar;

import std;

import cydui;
import cyd_wm.types;
import cyd_wm.state.monitors;

namespace statusbar {
  using namespace cyd_wm;

  struct bar_t {
    int width;
    Monitor::sptr monitor;
    x11::window_t win;
    x11::window_t suggestion_window;
  };

  std::unordered_map<int, bar_t> bars;

  void init_bar(Monitor::sptr mon) {
    MonitorStatusBar* bar = &(mon->bar);

    // Create the window that will display the layout
    bar->wwin = cyd::ui::window::create(
      cyd::ui::layout::create(cyd_wm::CydWMStatusBar {{.status = &bar->wstatus}}),
      "Workspace Selector",
      "castle-dwm-ui",
      mon->wx, mon->by + 0, // X, Y of the window
      mon->ww, 26,          // W, H of the window
      true
    );
    bars[mon->num] = {
      mon->ww,
      mon,
      0, //mon->barwin,
      0,
    };
    bar->init = true;

    bar->wwin->on_event<cyd_wm::WorkspaceEvent>(
      cyd::ui::async::Consumer<cyd_wm::WorkspaceEvent>(
        [mon](const cyd::ui::async::ParsedEvent<cyd_wm::WorkspaceEvent> &it) {
          if (it.data->start_menu) {
            const Arg a {.v = "startmenu"};
            //ops::control::spawn(&a);
          } else {
            const Arg a {.ui = (unsigned int)mon->bar.wstatus.selected_workspaces};
            //printf("=== selection: %09b\n", mon->bar.wstatus.selected_workspaces);
            monitors::switch_to_monitor(mon); // Select which ever monitor the selector was clicked on
            ops::control::view(&a); // Go to selected workspace
          }
        }));
  }

  void update_bar(Monitor::sptr monitor, bar_t bar) {
    if (monitor->sel == nullptr || !monitor->sel->isfullscreen) {
      x11::raise_window(get_id(monitor->bar.wwin->win_ref));
    }

    unsigned int i, occ = 0, urg = 0;

    for (const auto & c : monitor->clients) {
      occ |= c->tags;
      if (c->isurgent)
        urg |= c->tags;
    }

    monitor->bar.wwin->emit<cyd_wm::WindowStatusUpdate>({
      .win = get_id(monitor->bar.wwin->win_ref),
      .type = cyd_wm::win_status_update_t::WINDOW_TITLE,
      .str1 = monitor->sel ? monitor->sel->name : "",
    });
    monitor->bar.wwin->emit<cyd_wm::WindowBannerUpdate>({
      .monitor = monitor,
    });
    monitor->bar.wwin->emit<cyd_wm::ui::KeynavUpdate>({
      .win = get_id(monitor->bar.wwin->win_ref),
      .current = (monitor == state::selmon && state::key_nav::accepting)
                 ? std::optional {state::key_nav::current}
                 : std::nullopt,
    });
    monitor->bar.wwin->emit<RedrawEvent>({.win = get_id(monitor->bar.wwin->win_ref)});

    monitor->bar.wstatus.occupied_workspaces = (int)occ;
    monitor->bar.wstatus.selected_workspaces = (int)(monitor->tagset[monitor->seltags]);
    //cydui::events::emit<RedrawEvent>({.win = get_id(monitor->bar.wwin->win_ref)});
  }

}

export namespace statusbar {
  void init_where_needed() {
    for (const auto& monitor : state::monitors::monitors) {
      if (monitor->barwin)
        continue;
      if (monitor->bar.init)
        continue;

      init_bar(monitor);
    }
  }

  void update_all() {
    for (auto &item: bars) {
      auto &[_, bar] = item;
      update_bar(bar.monitor, bar);
    }
  }
}

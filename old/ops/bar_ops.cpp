//
// Created by castle on 5/19/22.
//

#include "bar_ops.h"
#include "control_ops.h"
#include "../state/state.h"
#include <cydui/cydui.hpp>
#include "../statusbar/CydWMStatusBar.h"

#include <string>

using namespace ops;

std::unordered_map<int, bar_t> bars;
logging::logger bar_log = {.name = "BAR_OPS", .on = false};

static void init_bar(monitor_t* mon) {
  monitor_bar_t* bar = &(mon->bar);
  bar_log.info("=== INITIALIZING BAR");

  // Create the window that will display the layout
  bar->wwin = cydui::window::create(
    cydui::layout::create(cyd_wm::CydWMStatusBar {{.status = &bar->wstatus}}),
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
    cydui::async::Consumer<cyd_wm::WorkspaceEvent>(
      [mon](const cydui::async::ParsedEvent<cyd_wm::WorkspaceEvent> &it) {
        if (it.data->start_menu) {
          const Arg a {.v = "startmenu"};
          //ops::control::spawn(&a);
        } else {
          const Arg a {.ui = (unsigned int)mon->bar.wstatus.selected_workspaces};
          //printf("=== selection: %09b\n", mon->bar.wstatus.selected_workspaces);
          state::selmon = mon;    // Select which ever monitor the selector was clicked on
          ops::control::view(&a); // Go to selected workspace
        }
      }));

  bar_log.info("=== BAR INITIALIZED");
}

void bar::init_where_needed() {
  monitor_t* m;
  for (m = state::mons; m; m = m->next) {
    if (m->barwin)
      continue;
    if (m->bar.init)
      continue;

    init_bar(m);
  }
}

void update_bar(monitor_t* monitor, bar_t bar) {
  bar_log.info("=== UPDATING BAR");
  if (monitor->sel == nullptr || !monitor->sel->isfullscreen) {
    XRaiseWindow(state::dpy, get_id(monitor->bar.wwin->win_ref));
  }

  unsigned int i, occ = 0, urg = 0;
  client_t* c;

  for (c = monitor->clients; c; c = c->next) {
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
  bar_log.info("=== BAR UPDATED");
}

void bar::update_all() {
  for (auto &item: bars) {
    auto &[_, bar] = item;
    update_bar(bar.monitor, bar);
  }
}

//
// Created by castle on 5/19/22.
//

#include "bar_ops.h"
#include "x11_ops.h"
#include "monitor_ops.h"
#include "control_ops.h"
#include "../state/state.h"
#include "../util.h"
#include <cydui/cydui.hpp>

#include <string>

using namespace ops;

std::unordered_map<int, bar_t> bars;
logging::logger bar_log = {.name = "BAR_OPS", .on = false};

static void init_bar(monitor_t* mon) {
  monitor_bar_t* bar = &(mon->bar);
  bar_log.info("=== INITIALIZING BAR");
  cydui::layout::Layout* layout = cydui::layout::create<cyd_wm::CydWMStatusBar>(cyd_wm::CydWMStatusBar{{.status = &bar->wstatus}});
  //cydui::layout::Layout* layout1 = cydui::layout::create<WinTitle>({});
  
  // Create the window that will display the layout
  cydui::window::CWindow* win = cydui::window::create(
    layout,
    "Workspace Selector",
    "castle-dwm-ui",
    mon->wx, mon->by + 0, // X, Y of the window
    mon->ww, 26, // W, H of the window
    true
  );
  bar->wwin         = win;
  //bar->wlay         = layout;
  
  cydui::events::on_event<cyd_wm::WorkspaceEvent>(
    cydui::events::Consumer<cyd_wm::WorkspaceEvent>(
      [mon,win](const cydui::events::ParsedEvent<cyd_wm::WorkspaceEvent>& it){
        //bar_log.info("=== ANSWERING TO WS EVENT");
        //printf("=== ANSWERING TO WS EVENT (win: %lX ?= %lX)\n", get_id(win->win_ref), it.data->win);
        if (it.data->win != get_id(win->win_ref)) return;
        if (it.data->start_menu) {
          const Arg a {.v = "startmenu"};
          ops::control::spawn(&a);
        } else {
          const Arg a {.ui = (unsigned int)mon->bar.wstatus.selected_workspaces};
          //printf("=== selection: %09b\n", mon->bar.wstatus.selected_workspaces);
          state::selmon = mon; // Select which ever monitor the selector was clicked on
          ops::control::view(&a); // Go to selected workspace
        }
  }));
  
  //mon->barwin = get_id(win->win_ref);//x11::create_barwin(m->wx, m->by, m->ww);
  //mon->barwin = x11::create_barwin(mon->wx+mon->bar.wlen, mon->by, mon->ww-mon->bar.wlen);
  bars[mon->num] = {
    mon->ww,
    0,//mon->barwin,
    0,
  };
  
  bar->init = true;
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
  
  int x = 0, w, tw = 0;
  int boxs = state::drw->fonts->h / 9;
  int boxw = state::drw->fonts->h / 6 + 2;
  unsigned int i, occ = 0, urg = 0;
  client_t *c;
  
  // Draws rectangle over all bar so that there are no glitches
  //XLockDisplay(state::dpy);
  ////drw_setscheme(state::drw, state::scheme[monitor == state::selmon ? SchemeSel : SchemeNorm]);
  ////drw_rect(state::drw, x, 0, monitor->ww-monitor->bar.wlen, state::bar_height, 1, 1);
  
  /* draw status first, so it can be overdrawn by TAGS later */
  //if (m == selmon || 1) { /* status is only drawn on selected monitor */
  
  ////tw = monitor->ww - draw_statusbar(monitor->ww - monitor->bar.wlen);
  
  //if (m == selmon) { /* status is only drawn on selected monitor */
  //	drw_setscheme(drw, scheme[SchemeNorm]);
  //	tw = TEXTW(stext) - lrpad + 2; /* 2px right padding */
  //	drw_text(drw, m->ww - tw, 0, tw, bar_height, 0, stext, 0);
  //}
  
  for (c = monitor->clients; c; c = c->next) {
    occ |= c->tags;
    if (c->isurgent)
      urg |= c->tags;
  }
  //for (i = 0; i < state::config::TAGS.size(); i++) {
  //  w = TEXTW(state::config::TAGS[i]);
  //  drw_setscheme(state::drw, state::scheme[monitor->tagset[monitor->seltags] & 1 << i ? SchemeSel : SchemeNorm]);
  //  drw_text(state::drw, x, 3, w - 4, state::bar_height - 6, state::lrpad / 2 - 2, state::config::TAGS[i].c_str(), urg & 1 << i);
  //  if (occ & 1 << i)
  //    drw_rect(state::drw, x + boxs, boxs, boxw, boxw,
  //             monitor == state::selmon && state::selmon->sel && state::selmon->sel->tags & 1 << i,
  //             urg & 1 << i);
  //  x += w;
  //}
  
  //cydui::events::emit<cyd_wm::WindowStatusUpdate>({
  //  .win = get_id(monitor->bar.wwin->win_ref),
  //  .type = cyd_wm::win_status_update_t::LAYOUT_SYMBOL,
  //  .str1 = monitor->ltsymbol,
  //});
  
  //w = state::blw = TEXTW_CSTR(monitor->ltsymbol);
  //drw_setscheme(state::drw, state::scheme[SchemeNorm]);
  //x = drw_text(state::drw, x, 3, w, state::bar_height - 6, state::lrpad / 2, monitor->ltsymbol, 0);
  
// //  if (monitor == state::selmon && state::key_nav::accepting) {
// //    unsigned int kn_width = TEXTW_CSTR(state::key_nav::current_path.c_str());
// //    x = drw_text(state::drw, x, 3, kn_width, state::bar_height - 6, state::lrpad / 2, state::key_nav::current_path.c_str(), 1);
// //
// //    for (const auto &target : state::key_nav::current->map) {
// //      std::string s;
// //      s.append(XKeysymToString(target.first));
// //      s.append("- ");
// //      s.append(target.second->description);
// //      kn_width = TEXTW_CSTR(s.c_str());
// //      x = drw_text(state::drw, x, 3, kn_width, state::bar_height - 6, state::lrpad / 2, s.c_str(), 0);
// //    }
// //
// //    if (bar.suggestion_window == 0) {
// //      int sw_height = state::key_nav::current->map.size() / 3 + 1;
// ////      bar.suggestion_window = x11::create_barwin(monitor->wx, sw_height, monitor->ww);
// //    }
// //  } else {
// //    if (bar.suggestion_window != 0) {
// ////      x11::destroy_barwin(bar.suggestion_window);
// //      bar.suggestion_window = 0;
// //    }
// //  }
  
  //if ((w = monitor->ww - tw - x) > state::bar_height) {
    cydui::events::emit<cyd_wm::WindowStatusUpdate>({
      .win = get_id(monitor->bar.wwin->win_ref),
      .type = cyd_wm::win_status_update_t::WINDOW_TITLE,
      .str1 = monitor->sel? monitor->sel->name : "",
    });
    cydui::events::emit<cyd_wm::ui::KeynavUpdate>({
      .win = get_id(monitor->bar.wwin->win_ref),
      .current = (monitor == state::selmon && state::key_nav::accepting)
                 ? std::optional{state::key_nav::current}
                 : std::nullopt,
    });
    cydui::events::emit<RedrawEvent>({.win = get_id(monitor->bar.wwin->win_ref)});
    //}
    
    monitor->bar.wstatus.occupied_workspaces = (int)occ;
    monitor->bar.wstatus.selected_workspaces = (int)(monitor->tagset[monitor->seltags]);
    //cydui::events::emit<RedrawEvent>({.win = get_id(monitor->bar.wwin->win_ref)});
    bar_log.info("=== BAR UPDATED");
}

void bar::update_all() {
  for (monitor_t* m = state::mons; m; m = m->next) {
    if (bars.contains(m->num)) {
      bar_t bar = bars[m->num];
      update_bar(m, bar);
    }
  }
}

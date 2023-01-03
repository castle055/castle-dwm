//
// Created by castle on 5/19/22.
//

#include "bar_ops.h"
#include "x11_ops.h"
#include "monitor_ops.h"
#include "control_ops.h"
#include "../state/state.h"
#include "../util.h"
#include <cyd_ui/cydui.hpp>
#include "../ui/status_bar/workspaces.hpp"

#include <string>

using namespace ops;

std::unordered_map<int, bar_t> bars;

static void init_bar(monitor_t* mon) {
  monitor_bar_t* bar = &(mon->bar);
  
  bar->wstate       = new WorkspacesState();
  auto* state       = bar->wstate;
  bar->workspaces   = new Workspaces(bar->wstate, { }, ___inner(Workspaces, t, { }));
  bar->wlay         = new cydui::layout::Layout(bar->workspaces);
  bar->wwin         = cydui::window::create(
    bar->wlay,
    "dwm", "dwm",
    mon->wx + 0, mon->by + 1,
    //1, 13,
    280, 24,
    true
  );
  
  bar->wstate->selected_workspaces.on_change([bar, mon]() {
    const Arg a {.ui = (unsigned int)bar->wstate->selected_workspaces.val()};
    ops::control::view(&a);
  });
  
  //m->barwin = win->win_ref->xwin;//x11::create_barwin(m->wx, m->by, m->ww);
  mon->barwin = x11::create_barwin(mon->wx+mon->bar.wlen, mon->by, mon->ww-mon->bar.wlen);
  bars[mon->num] = {
    mon->ww,
    mon->barwin,
    0,
  };
  
  bar->init = true;
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

int draw_statusbar(int width) {
  const char* stext = state::stext.c_str();
  int ret, i, w, x;
  unsigned int len;
  short isCode = 0;
  char *text;
  char *p;
  
  len = strlen(stext) + 1;
  if (!(text = (char *) malloc(sizeof(char) * len)))
    die("malloc");
  p = text;
  memcpy(text, stext, len);
  
  /* compute width of the status text */
  w = 0;
  i = -1;
  while (text[++i]) {
    if (text[i] == '^') {
      if (!isCode) {
        isCode = 1;
        text[i] = '\0';
        w += TEXTW_CSTR(text) - state::lrpad;
        text[i] = '^';
        if (text[++i] == 'f')
          w += atoi(text + ++i);
      } else {
        isCode = 0;
        text = text + i + 1;
        i = -1;
      }
    }
  }
  if (!isCode)
    w += TEXTW_CSTR(text) - state::lrpad;
  else
    isCode = 0;
  text = p;
  
  w += 2; /* 1px padding on both sides */
  ret = x = width - w;
  
  drw_setscheme(state::drw, state::scheme[state::config::colors.size()]);
  state::drw->scheme[ColFg] = state::scheme[SchemeNorm][ColFg];
  state::drw->scheme[ColBg] = state::scheme[SchemeNorm][ColBg];
  drw_rect(state::drw, x, 0, w, state::bar_height, 1, 1);
  x++;
  
  /* process status text */
  i = -1;
  while (text[++i]) {
    if (text[i] == '^' && !isCode) {
      isCode = 1;
      
      text[i] = '\0';
      w = TEXTW_CSTR(text) - state::lrpad;
      drw_text(state::drw, x, state::bar_height / 2, w, 0, 0, text, 0);
      
      x += w;
      
      /* process code */
      while (text[++i] != '^') {
        if (text[i] == 'c') {
          char buf[8];
          memcpy(buf, (char *) text + i + 1, 7);
          buf[7] = '\0';
          drw_clr_create(state::drw, &state::drw->scheme[ColFg], buf);
          i += 7;
        } else if (text[i] == 'b') {
          char buf[8];
          memcpy(buf, (char *) text + i + 1, 7);
          buf[7] = '\0';
          drw_clr_create(state::drw, &state::drw->scheme[ColBg], buf);
          i += 7;
        } else if (text[i] == 'd') {
          state::drw->scheme[ColFg] = state::scheme[SchemeNorm][ColFg];
          state::drw->scheme[ColBg] = state::scheme[SchemeNorm][ColBg];
        } else if (text[i] == 'r') {
          int rx = atoi(text + ++i);
          while (text[++i] != ',');
          int ry = atoi(text + ++i);
          while (text[++i] != ',');
          int rw = atoi(text + ++i);
          while (text[++i] != ',');
          int rh = atoi(text + ++i);
          
          drw_rect(state::drw, rx + x, ry, rw, rh, 1, 0);
        } else if (text[i] == 'a') {
          int rx = atoi(text + ++i);
          while (text[++i] != ',');
          int ry = atoi(text + ++i);
          while (text[++i] != ',');
          int rw = atoi(text + ++i);
          while (text[++i] != ',');
          int rh = atoi(text + ++i);
          while (text[++i] != ',');
          int rang1 = atoi(text + ++i);
          while (text[++i] != ',');
          int rang2 = atoi(text + ++i);
          
          drw_arc(state::drw, rx + x, ry, rw, rh, rang1 * 64, rang2 * 64, 0, 0);
        } else if (text[i] == 'A') {
          int rx = atoi(text + ++i);
          while (text[++i] != ',');
          int ry = atoi(text + ++i);
          while (text[++i] != ',');
          int rw = atoi(text + ++i);
          while (text[++i] != ',');
          int rh = atoi(text + ++i);
          while (text[++i] != ',');
          int rang1 = atoi(text + ++i);
          while (text[++i] != ',');
          int rang2 = atoi(text + ++i);
          
          drw_arc(state::drw, rx + x, ry, rw, rh, rang1 * 64, rang2 * 64, 1, 0);
        } else if (text[i] == 'l') {
          int rx1 = atoi(text + ++i);
          while (text[++i] != ',');
          int ry1 = atoi(text + ++i);
          while (text[++i] != ',');
          int rx2 = atoi(text + ++i);
          while (text[++i] != ',');
          int ry2 = atoi(text + ++i);
          
          drw_line(state::drw, rx1 + x, ry1, rx2 + x, ry2);
        } else if (text[i] == 'p') {
          int rx1 = atoi(text + ++i);
          while (text[++i] != ',');
          int ry1 = atoi(text + ++i);
          
          drw_rect(state::drw, rx1, ry1, 2, 2, 1, 0);
        } else if (text[i] == 'f') {
          x += atoi(text + ++i);
        }
      }
      
      text = text + i + 1;
      i = -1;
      isCode = 0;
    }
  }
  
  if (!isCode) {
    w = TEXTW_CSTR(text) - state::lrpad;
    drw_text(state::drw, x, 0, w, state::bar_height, 0, text, 0);
  }
  
  drw_setscheme(state::drw, state::scheme[SchemeNorm]);
  free(p);
  
  return ret;
}

void update_bar(monitor_t* monitor, bar_t bar) {
  int x = 0, w, tw = 0;
  int boxs = state::drw->fonts->h / 9;
  int boxw = state::drw->fonts->h / 6 + 2;
  unsigned int i, occ = 0, urg = 0;
  client_t *c;
  
  // Draws rectangle over all bar so that there are no glitches
  XLockDisplay(state::dpy);
  drw_setscheme(state::drw, state::scheme[monitor == state::selmon ? SchemeSel : SchemeNorm]);
  drw_rect(state::drw, x, 0, monitor->ww-monitor->bar.wlen, state::bar_height, 1, 1);
  
  /* draw status first so it can be overdrawn by TAGS later */
  //if (m == selmon || 1) { /* status is only drawn on selected monitor */
  tw = monitor->ww - draw_statusbar(monitor->ww - monitor->bar.wlen);
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
  w = state::blw = TEXTW_CSTR(monitor->ltsymbol);
  drw_setscheme(state::drw, state::scheme[SchemeNorm]);
  x = drw_text(state::drw, x, 3, w, state::bar_height - 6, state::lrpad / 2, monitor->ltsymbol, 0);
  
  if (monitor == state::selmon && state::key_nav::accepting) {
    unsigned int kn_width = TEXTW_CSTR(state::key_nav::current_path.c_str());
    x = drw_text(state::drw, x, 3, kn_width, state::bar_height - 6, state::lrpad / 2, state::key_nav::current_path.c_str(), 1);
    
    for (const auto &target : state::key_nav::current->map) {
      std::string s;
      s.append(XKeysymToString(target.first));
      s.append("- ");
      s.append(target.second->description);
      kn_width = TEXTW_CSTR(s.c_str());
      x = drw_text(state::drw, x, 3, kn_width, state::bar_height - 6, state::lrpad / 2, s.c_str(), 0);
    }
    
    if (bar.suggestion_window == 0) {
      int sw_height = state::key_nav::current->map.size() / 3 + 1;
//      bar.suggestion_window = x11::create_barwin(monitor->wx, sw_height, monitor->ww);
    }
  } else {
    if (bar.suggestion_window != 0) {
//      x11::destroy_barwin(bar.suggestion_window);
      bar.suggestion_window = 0;
    }
  }
  
  if ((w = monitor->ww - tw - x) > state::bar_height) {
    if (monitor->sel) {
      drw_setscheme(state::drw, state::scheme[monitor == state::selmon ? SchemeSel : SchemeNorm]);
      drw_text(state::drw, x, 0, w, state::bar_height, (state::lrpad + w - TEXTW(monitor->sel->name)) / 2, monitor->sel->name.c_str(), 0);
      if (monitor->sel->isfloating)
        drw_rect(state::drw, x + boxs + 4, 3, w - 8, state::bar_height - 6, monitor->sel->isfixed, 0);
    } else {
      drw_setscheme(state::drw, state::scheme[SchemeNorm]);
      drw_rect(state::drw, x, 0, w, state::bar_height, 1, 1);
    }
  }
  drw_map(state::drw, bar.win, 0, 0, bar.width, state::bar_height);
  
  XUnlockDisplay(state::dpy);
  monitor->bar.wstate->occupied_workspaces.set((unsigned int)monitor->bar.wwin->win_ref->xwin,(int)occ);
  monitor->bar.wstate->selected_workspaces.set((unsigned int)monitor->bar.wwin->win_ref->xwin,(int)(monitor->tagset[monitor->seltags]));
}

void bar::update_all() {
  for (monitor_t* m = state::mons; m; m = m->next) {
    auto bar_iter = bars.find(m->num);
    if (bar_iter == bars.end())
      continue;
    
    bar_t bar = bar_iter->second;
    update_bar(m, bar);
  }
}

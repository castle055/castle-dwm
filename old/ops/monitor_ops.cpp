//
// Created by castle on 3/24/22.
//

#include <cstdlib>
#include <X11/Xutil.h>
#include <cstring>
#include "../util.h"

#include "monitor_ops.h"
#include "client_ops.h"
#include "bar_ops.h"
#include "x11_ops.h"

#include "../state/state.h"
#include "log_ops.h"

using namespace ops;

//===== Monitor Ops implementations =====
void monitor::arrange(monitor_t *m) {
  if (m)
    client::show_hide(m->stack);
  else
    for (m = state::mons; m; m = m->next)
      client::show_hide(m->stack);
  if (m) {
    arrange_mon(m);
    restack(m);
  } else
    for (m = state::mons; m; m = m->next)
      arrange_mon(m);
}
void monitor::arrange_mon(monitor_t* monitor) {
  strncpy(monitor->ltsymbol, monitor->lt[monitor->sellt]->symbol, sizeof monitor->ltsymbol);
  if (monitor->lt[monitor->sellt]->arrange_)
    monitor->lt[monitor->sellt]->arrange_(monitor);
}
void monitor::load_default_layout(monitor_t* monitor) {
  if(!monitor) return;
  log::debug("Loading default layouts for monitor [%d]", monitor->num);
  state::config::setup_default_layouts();
  
  int l = state::config::default_layouts[monitor->num][monitor->pertag->curtag].ltidx;
  log::debug("[load_default_layout] monitor.num = %d", monitor->num);
  log::debug("[load_default_layout] curtag = %d", monitor->pertag->curtag);
  log::debug("[load_default_layout] l = %d", l);
  log::debug("[load_default_layout] monitor.sellt = %d",monitor->sellt);
  if (l < 0) l = state::config::default_layout;
  monitor->lt[monitor->sellt] = &state::config::layouts[l];
  log::debug("[load_default_layout] l = %d", l);
  monitor->nmaster = state::config::default_layouts[monitor->num][monitor->pertag->curtag].nmaster;
  log::debug("[load_default_layout] l = %d", l);
  strncpy(monitor->ltsymbol, monitor->lt[monitor->sellt]->symbol, sizeof monitor->ltsymbol);
  
  log::debug("[load_default_layout] loading tag layouts");
  monitor->pertag->ltidxs.resize(state::config::TAGS.size()+1);
  monitor->pertag->sellts.resize(state::config::TAGS.size()+1);
  monitor->pertag->nmasters.resize(state::config::TAGS.size()+1);
  for (size_t i = 0; i < state::config::TAGS.size(); ++i) {
//    if (tagset[seltags] & 1 << i) {
    int l1 = state::config::default_layouts[monitor->num][i].ltidx;
    if (l1 < 0) l1 = (int)state::config::default_layout;
    monitor->pertag->ltidxs[(i + 1) % (state::config::TAGS.size() + 1)][monitor->sellt] = &state::config::layouts[l1];
    monitor->pertag->sellts[(i + 1) % (state::config::TAGS.size() + 1)] = monitor->sellt;
    monitor->pertag->nmasters[(i+ 1) % (state::config::TAGS.size() + 1)] = state::config::default_layouts[monitor->num][i].nmaster;
  }
  log::debug("Done loading default layouts for monitor [%d]", monitor->num);
}
void monitor::cleanup_mon(monitor_t *monitor) {
  monitor_t *m;
  
  if (monitor == state::mons)
    state::mons = state::mons->next;
  else {
    for (m = state::mons; m && m->next != monitor; m = m->next);
    m->next = monitor->next;
  }
  XUnmapWindow(state::dpy, monitor->barwin);
  XDestroyWindow(state::dpy, monitor->barwin);
  free(monitor);
}
//void monitor::bar::draw_all_bars() {
//  for (monitor_t* m = state::mons; m; m = m->next)
//    draw_bar(m);
//}
//int monitor::bar::draw_statusbar(monitor_t* monitor, int bar_height, char *stext) {
//  int ret, i, w, x, len;
//  short isCode = 0;
//  char *text;
//  char *p;
//
//  len = strlen(stext) + 1;
//  if (!(text = (char *) malloc(sizeof(char) * len)))
//    die("malloc");
//  p = text;
//  memcpy(text, stext, len);
//
//  /* compute width of the status text */
//  w = 0;
//  i = -1;
//  while (text[++i]) {
//    if (text[i] == '^') {
//      if (!isCode) {
//        isCode = 1;
//        text[i] = '\0';
//        w += TEXTW_CSTR(text) - state::lrpad;
//        text[i] = '^';
//        if (text[++i] == 'f')
//          w += atoi(text + ++i);
//      } else {
//        isCode = 0;
//        text = text + i + 1;
//        i = -1;
//      }
//    }
//  }
//  if (!isCode)
//    w += TEXTW_CSTR(text) - state::lrpad;
//  else
//    isCode = 0;
//  text = p;
//
//  w += 2; /* 1px padding on both sides */
//  ret = x = monitor->ww - w;
//
//  drw_setscheme(state::drw, state::scheme[state::config::colors.size()]);
//  state::drw->scheme[ColFg] = state::scheme[SchemeNorm][ColFg];
//  state::drw->scheme[ColBg] = state::scheme[SchemeNorm][ColBg];
//  drw_rect(state::drw, x, 0, w, bar_height, 1, 1);
//  x++;
//
//  /* process status text */
//  i = -1;
//  while (text[++i]) {
//    if (text[i] == '^' && !isCode) {
//      isCode = 1;
//
//      text[i] = '\0';
//      w = TEXTW_CSTR(text) - state::lrpad;
//      drw_text(state::drw, x, bar_height / 2, w, 0, 0, text, 0);
//
//      x += w;
//
//      /* process code */
//      while (text[++i] != '^') {
//        if (text[i] == 'c') {
//          char buf[8];
//          memcpy(buf, (char *) text + i + 1, 7);
//          buf[7] = '\0';
//          drw_clr_create(state::drw, &state::drw->scheme[ColFg], buf);
//          i += 7;
//        } else if (text[i] == 'b') {
//          char buf[8];
//          memcpy(buf, (char *) text + i + 1, 7);
//          buf[7] = '\0';
//          drw_clr_create(state::drw, &state::drw->scheme[ColBg], buf);
//          i += 7;
//        } else if (text[i] == 'd') {
//          state::drw->scheme[ColFg] = state::scheme[SchemeNorm][ColFg];
//          state::drw->scheme[ColBg] = state::scheme[SchemeNorm][ColBg];
//        } else if (text[i] == 'r') {
//          int rx = atoi(text + ++i);
//          while (text[++i] != ',');
//          int ry = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rw = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rh = atoi(text + ++i);
//
//          drw_rect(state::drw, rx + x, ry, rw, rh, 1, 0);
//        } else if (text[i] == 'a') {
//          int rx = atoi(text + ++i);
//          while (text[++i] != ',');
//          int ry = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rw = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rh = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rang1 = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rang2 = atoi(text + ++i);
//
//          drw_arc(state::drw, rx + x, ry, rw, rh, rang1 * 64, rang2 * 64, 0, 0);
//        } else if (text[i] == 'A') {
//          int rx = atoi(text + ++i);
//          while (text[++i] != ',');
//          int ry = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rw = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rh = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rang1 = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rang2 = atoi(text + ++i);
//
//          drw_arc(state::drw, rx + x, ry, rw, rh, rang1 * 64, rang2 * 64, 1, 0);
//        } else if (text[i] == 'l') {
//          int rx1 = atoi(text + ++i);
//          while (text[++i] != ',');
//          int ry1 = atoi(text + ++i);
//          while (text[++i] != ',');
//          int rx2 = atoi(text + ++i);
//          while (text[++i] != ',');
//          int ry2 = atoi(text + ++i);
//
//          drw_line(state::drw, rx1 + x, ry1, rx2 + x, ry2);
//        } else if (text[i] == 'p') {
//          int rx1 = atoi(text + ++i);
//          while (text[++i] != ',');
//          int ry1 = atoi(text + ++i);
//
//          drw_rect(state::drw, rx1, ry1, 1, 1, 1, 0);
//        } else if (text[i] == 'f') {
//          x += atoi(text + ++i);
//        }
//      }
//
//      text = text + i + 1;
//      i = -1;
//      isCode = 0;
//    }
//  }
//
//  if (!isCode) {
//    w = TEXTW_CSTR(text) - state::lrpad;
//    drw_text(state::drw, x, 0, w, bar_height, 0, text, 0);
//  }
//
//  drw_setscheme(state::drw, state::scheme[SchemeNorm]);
//  free(p);
//
//  return ret;
//}
//void monitor::bar::draw_bar(monitor_t* monitor) {
//  int x, w, tw = 0;
//  int boxs = state::drw->fonts->h / 9;
//  int boxw = state::drw->fonts->h / 6 + 2;
//  unsigned int i, occ = 0, urg = 0;
//  client_t *c;
//
//  // Draws rectangle over all bar so that there are no glitches
//  drw_setscheme(state::drw, state::scheme[monitor == state::selmon ? SchemeSel : SchemeNorm]);
//  drw_rect(state::drw, 0, 0, monitor->ww, state::bar_height, 1, 1);
//
//  /* draw status first so it can be overdrawn by TAGS later */
//  //if (m == selmon || 1) { /* status is only drawn on selected monitor */
//  tw = monitor->ww - draw_statusbar(monitor, state::bar_height, (char*)state::stext.c_str());
//  //if (m == selmon) { /* status is only drawn on selected monitor */
//  //	drw_setscheme(drw, scheme[SchemeNorm]);
//  //	tw = TEXTW(stext) - lrpad + 2; /* 2px right padding */
//  //	drw_text(drw, m->ww - tw, 0, tw, bar_height, 0, stext, 0);
//  //}
//
//  for (c = monitor->clients; c; c = c->next) {
//    occ |= c->tags;
//    if (c->isurgent)
//      urg |= c->tags;
//  }
//
//  x = 0;
//  for (i = 0; i < state::config::TAGS.size(); i++) {
//    w = TEXTW(state::config::TAGS[i]);
//    drw_setscheme(state::drw, state::scheme[monitor->tagset[monitor->seltags] & 1 << i ? SchemeSel : SchemeNorm]);
//    drw_text(state::drw, x, 3, w - 4, state::bar_height - 6, state::lrpad / 2 - 2, state::config::TAGS[i].c_str(), urg & 1 << i);
//    if (occ & 1 << i)
//      drw_rect(state::drw, x + boxs, boxs, boxw, boxw,
//               monitor == state::selmon && state::selmon->sel && state::selmon->sel->tags & 1 << i,
//               urg & 1 << i);
//    x += w;
//  }
//  w = state::blw = TEXTW_CSTR(monitor->ltsymbol);
//  drw_setscheme(state::drw, state::scheme[SchemeNorm]);
//  x = drw_text(state::drw, x, 3, w, state::bar_height - 6, state::lrpad / 2, monitor->ltsymbol, 0);
//
//  if (state::key_nav::accepting) {
//    x = drw_text(state::drw, x, 3, w, state::bar_height - 6, state::lrpad / 2, state::key_nav::current_path.c_str(), 0);
//  }
//
//  if ((w = monitor->ww - tw - x) > state::bar_height) {
//    if (monitor->sel) {
//      drw_setscheme(state::drw, state::scheme[monitor == state::selmon ? SchemeSel : SchemeNorm]);
//      drw_text(state::drw, x, 0, w, state::bar_height, (state::lrpad + w - TEXTW(monitor->sel->name)) / 2, monitor->sel->name.c_str(), 0);
//      if (monitor->sel->isfloating)
//        drw_rect(state::drw, x + boxs + 4, 3, w - 8, state::bar_height - 6, monitor->sel->isfixed, 0);
//    } else {
//      drw_setscheme(state::drw, state::scheme[SchemeNorm]);
//      drw_rect(state::drw, x, 0, w, state::bar_height, 1, 1);
//    }
//  }
//  drw_map(state::drw, monitor->barwin, 0, 0, monitor->ww, state::bar_height);
//}
void monitor::restack(monitor_t* monitor) {
  client_t *c;
  XEvent ev;
  XWindowChanges wc;
  
  ops::bar::update_all();
  if (!monitor->sel)
    return;
  if (monitor->sel->isfloating || !monitor->lt[monitor->sellt]->arrange_)
    XRaiseWindow(state::dpy, monitor->sel->win);
  if (monitor->lt[monitor->sellt]->arrange_) {
    wc.stack_mode = Below;
    wc.sibling = monitor->barwin;
    for (c = monitor->stack; c; c = c->snext)
      if (!c->isfloating && ISVISIBLE(c)) {
        XConfigureWindow(state::dpy, c->win, CWSibling | CWStackMode, &wc);
        wc.sibling = c->win;
      }
  }
  XSync(state::dpy, False);
  while (XCheckMaskEvent(state::dpy, EnterWindowMask, &ev));
}
void monitor::update_bar_pos(monitor_t* monitor) {
  monitor->wy = monitor->my;
  monitor->wh = monitor->mh;
  if (state::config::showbar) {
    monitor->wh -= state::bar_height;
    monitor->by = state::config::topbar ? monitor->wy : monitor->wy + monitor->wh;
    monitor->wy = state::config::topbar ? monitor->wy + state::bar_height : monitor->wy;
  } else
    monitor->by = -state::bar_height;
}
monitor_t *monitor::create_mon() {
  log::debug("[create_mon] Creating new monitor");
  monitor_t *m;
  unsigned int i;
  
  log::debug("[create_mon] Allocating monitor");
  m = new monitor_t;//(monitor_t *) calloc(1, sizeof(monitor_t));
  m->num = 0;
  m->tagset[0] = m->tagset[1] = 1;
  m->mfact = state::config::mfact;
  m->nmaster = state::config::nmaster;
  m->showbar = state::config::showbar;
  m->topbar = state::config::topbar;
  m->gappx = state::config::gappx;
  m->lt[0] = &state::config::layouts[0];
  m->lt[1] = &state::config::layouts[1 % state::config::layouts.size()];
  //log::debug("[create_mon] Copy layout symbol into monitor struct");
  strncpy(m->ltsymbol, state::config::layouts[0].symbol, sizeof m->ltsymbol);
  //log::debug("[create_mon] Allocating monitor pertag");
  try {
    m->pertag = new Pertag;//(Pertag *) ecalloc(1, sizeof(Pertag));
    m->pertag->curtag = m->pertag->prevtag = 1;
    
    size_t tags_len = state::config::TAGS.size();
    log::debug("[create_mon] Configuring monitor tags");
    for (i = 0; i < tags_len; i++) {
      log::debug("[create_mon] Configuring monitor tag [%d]", i);
      m->pertag->nmasters.resize(tags_len);
      m->pertag->nmasters[i] = m->nmaster;
      m->pertag->mfacts.resize(tags_len);
      m->pertag->mfacts[i] = m->mfact;
      
      m->pertag->ltidxs.resize(tags_len);
      m->pertag->ltidxs[i][0] = m->lt[0];
      m->pertag->ltidxs[i][1] = m->lt[1];
      m->pertag->sellts.resize(tags_len);
      m->pertag->sellts[i] = m->sellt;
      
      m->pertag->showbars.resize(tags_len);
      m->pertag->showbars[i] = m->showbar;
    }
  } catch (const std::bad_alloc& e) {
    log::error("[create_mon] Bad alloc on Pertags");
  }
  
  return m;
}
monitor_t* monitor::dir_to_mon(int dir) {
  monitor_t *m = NULL;
  
  if (dir > 0) {
    if (!(m = state::selmon->next))
      m = state::mons;
  } else if (state::selmon == state::mons)
    for (m = state::mons; m->next; m = m->next);
  else
    for (m = state::mons; m->next != state::selmon; m = m->next);
  return m;
}
monitor_t* monitor::rect_to_mon(int x, int y, int w, int h) {
  monitor_t *m, *r = state::selmon;
  int a, area = 0;
  
  for (m = state::mons; m; m = m->next)
    if ((a = INTERSECT(x, y, w, h, m)) > area) {
      area = a;
      r = m;
    }
  return r;
}
monitor_t* monitor::win_to_mon(Window w) {
  int x, y;
  client_t *c;
  monitor_t *m;
  
  if (w == state::root && x11::get_root_ptr(&x, &y))
    return rect_to_mon(x, y, 1, 1);
  for (m = state::mons; m; m = m->next)
    if (w == m->barwin)
      return m;
  if ((c = client::win_to_client(w)))
    return c->mon;
  return state::selmon;
}

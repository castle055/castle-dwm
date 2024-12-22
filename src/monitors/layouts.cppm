/*! \file  layouts.cppm
 *! \brief 
 *!
 */

module;
#define ISVISIBLE(C)            (((C)->tags & (C)->mon->tagset[(C)->mon->seltags]))
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
export module cyd_wm.layouts;

import std;
import cyd_wm.state.monitors;
import cyd_wm.monitors.stack;

using namespace cyd_wm;

export namespace layouts {
  void show_hide(const std::deque<Client::sptr>& client) {
    for (const auto& client : client) {
      if (ISVISIBLE(client)) {
        x11::move_window(client->win, client->x, client->y);
        if ((!client->mon->lt[client->mon->sellt]->arrange || client->isfloating) && !client->isfullscreen)
          resize(client, client->x, client->y, client->w, client->h, 0);
      } else {
        /* hide clients bottom up */
        x11::move_window(client->win, WIDTH(client) * -2, client->y);
      }
    }
  }

  void arrange_mon(Monitor::sptr monitor) {
    monitor->ltsymbol = monitor->lt[monitor->sellt]->symbol;
    if (monitor->lt[monitor->sellt]->arrange)
      monitor->lt[monitor->sellt]->arrange(monitor);
  }
  void arrange(Monitor::sptr m) {
    if (m) {
      show_hide(m->stack);
      arrange_mon(m);
      monitors::restack(m);
    } else {
      for (const auto& mon: state::monitors::monitors) {
        show_hide(mon->stack);
        arrange_mon(m);
      }
    }
  }
  void load_default_layout(Monitor::sptr monitor) {
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
}
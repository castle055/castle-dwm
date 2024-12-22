/*! \file  monitor_manager.cppm
 *! \brief 
 *!
 */

module;
#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))
#define INTERSECT(x, y, w, h, m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                                    * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))

export module cyd_wm.monitors.manager;

import std;
import x11;
import cyd_wm.state.monitors;

export namespace cyd_wm::monitors {
  Monitor::sptr make_monitor() {
    log::debug("[create_mon] Creating new monitor");
    auto m = std::make_shared<Monitor>();
    unsigned int i;

    log::debug("[create_mon] Allocating monitor");
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

  Monitor::sptr from_rectangle(int x, int y, int width, int height) {
    Monitor::sptr r = state::monitors::selected_monitor;
    int a, area     = 0;

    for (const auto &monitor: state::monitors::monitors) {
      if ((a = INTERSECT(x, y, width, height, monitor)) > area) {
        area = a;
        r    = monitor;
      }
    }
    return r;
  }

  Monitor::sptr from_window(x11::window_t win) {
    int x, y;

    if (win == x11::connection::root() && x11::get_root_ptr(&x, &y))
      return from_rectangle(x, y, 1, 1);
    for (const auto &monitor: state::monitors::monitors) {
      if (win == monitor->barwin)
        return monitor;
    }
    for (const auto &monitor: state::monitors::monitors) {
      for (const auto &client: monitor->clients) {
        if (client->win == win)
          return monitor;
      }
    }
    return state::monitors::selected_monitor;
  }

  Monitor::sptr dir_to_mon(int dir) {
    Monitor::sptr m = nullptr;

    for (auto it = state::monitors::monitors.begin(); it != state::monitors::monitors.end(); ++it) {
      if (*it == state::monitors::selected_monitor) {
        if (dir > 0) {
          if (it == std::prev(state::monitors::monitors.end())) {
            m = state::monitors::selected_monitor;
          } else {
            m = *std::next(it);
          }
        } else {
          if (it == state::monitors::monitors.begin()) {
            m = state::monitors::selected_monitor;
          } else {
            m = *std::next(it);
          }
        }
      }
    }
    return m;
  }

  void switch_to_monitor(const Monitor::sptr& m) {
    state::monitors::selected_monitor = m;
  }

  bool unregister_monitor(Monitor::sptr monitor) {
    for (auto it = state::monitors::monitors.begin(); it != state::monitors::monitors.end(); ++ it) {
      if (*it == monitor) {
        state::monitors::monitors.erase(it);
        return true;
      }
    }
    return false;
  }

  void attach_client(Monitor::sptr monitor, Client::sptr client) {
    monitor->clients.push_front(client);
  }

  void detach_client(Monitor::sptr monitor, Client::sptr client) {
    for (auto it = monitor->clients.begin(); it != monitor->clients.end(); ++it) {
      if (*it == client) {
        monitor->clients.erase(it);
        return;
      }
    }
  }
}


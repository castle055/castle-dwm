/*! \file  monitor_stack.cppm
 *! \brief 
 *!
 */

module;
#define ISVISIBLE(C)            (((C)->tags & (C)->mon->tagset[(C)->mon->seltags]))
export module cyd_wm.monitors.stack;

import cyd_wm.types;
import cyd_wm.statusbar;

export namespace cyd_wm::monitors {
  //===== Monitor Ops implementations =====

  void restack(Monitor::sptr monitor) {
    Client::sptr c;
    x11::event_t ev;
    x11::window_changes_t wc;

    statusbar::update_all();

    if (!monitor->sel)
      return;
    if (monitor->sel->isfloating || !monitor->lt[monitor->sellt]->arrange)
      x11::raise_window(monitor->sel->win);
    if (monitor->lt[monitor->sellt]->arrange) {
      wc.stack_mode = Below;
      wc.sibling = monitor->barwin;
      for (const auto & client : monitor->stack) {
        if (!client->isfloating && ISVISIBLE(client)) {
          x11::configure_window(client->win, CWSibling | CWStackMode, &wc);
          wc.sibling = client->win;
        }
      }
    }
    x11::sync(false);
    while (XCheckMaskEvent(state::dpy, EnterWindowMask, &ev));
  }

  void attach_to_stack(Client::sptr client) {
    if (!client->mon) return;
    client->mon->stack.push_front(client);
  }
  void detach_from_stack(Client::sptr client) {
    if (!client->mon) return;
    for (auto it = client->mon->stack.begin(); it != client->mon->stack.end(); ++it) {
      if (*it == client) {
        client->mon->stack.erase(it);

        if (client == client->mon->sel) {
          for (it = client->mon->stack.begin(); !ISVISIBLE(*it) && it != client->mon->stack.end(); ++it);
          client->mon->sel = *it;
        }
        return;
      }
    }
  }
  void bring_to_front(const Client::sptr& client) {
    if (!client->mon) return;
    for (auto it = client->mon->stack.begin(); it != client->mon->stack.end(); ++it) {
      if (*it == client) {
        // Swap the instance within the deque
        std::swap(*it, client->mon->stack.front());
        return;
      }
    }
  }

  Client::sptr next_in_stack(Client::sptr client, bool include_floating = false) {
    for (auto it = client->mon->stack.begin(); it != client->mon->stack.end(); ++it) {
      if (*it == client) {
        if (include_floating) {
          for (; !ISVISIBLE(*it) && it != client->mon->stack.end(); ++it);
        } else {
          for (; ((*it)->isfloating || !ISVISIBLE(*it)) && it != client->mon->stack.end(); ++it);
        }
        return *it;
      }
    }
    return client;
  }

  Client::sptr prev_in_stack(Client::sptr client, bool include_floating = false) {
    for (auto it = client->mon->stack.rbegin(); it != client->mon->stack.rend(); ++it) {
      if (*it == client) {
        if (include_floating) {
          for (; !ISVISIBLE(*it) && it != client->mon->stack.rend(); ++it);
        } else {
          for (; ((*it)->isfloating || !ISVISIBLE(*it)) && it != client->mon->stack.rend(); ++it);
        }
        return *it;
      }
    }
    return client;
  }

}

/*! \file  client_manager.cppm
 *! \brief 
 *!
 */

module;
#define ISVISIBLE(C)            (((C)->tags & (C)->mon->tagset[(C)->mon->seltags]))
export module cyd_wm.clients.manager;

import std;
import cyd_wm.state.monitors;
import cyd_wm.monitors.manager;
import cyd_wm.monitors.stack;
import cyd_wm.layouts;

export namespace cyd_wm::clients {
  Client::sptr make_client(x11::window_t w, x11::window_attrs_t* wa, int urgent) {
    client_t *t = nullptr;
    x11::window_t trans = None;
    x11::window_changes_t wc;

    Client::sptr c = std::make_shared<Client>();
    c->win = w;
    /* geometry */
    c->x = c->oldx = wa->x;
    c->y = c->oldy = wa->y;
    c->w = c->oldw = wa->width;
    c->h = c->oldh = wa->height;
    c->oldbw = wa->border_width;

    update_title(c);
    if (XGetTransientForHint(state::dpy, w, &trans) && (t = win_to_client(trans))) {
      c->mon = t->mon;
      c->tags = t->tags;
    } else {
      c->mon = state::selmon;
      ops::client::apply_rules(c);
    }

    log::debug("[manage]");
    c->x += c->mon->mw;
    if (c->x + WIDTH(c) > c->mon->mx + c->mon->mw)
      c->x = c->mon->mx + c->mon->mw - WIDTH(c);
    if (c->y + HEIGHT(c) > c->mon->my + c->mon->mh)
      c->y = c->mon->my + c->mon->mh - HEIGHT(c);
    //c->x = MAX(c->x, c->mon->mx);
    /* only fix client y-offset, if the client center might cover the bar */
    c->y = MAX(c->y, ((c->mon->by == c->mon->my) && (c->x + (c->w / 2) >= c->mon->wx)
                      && (c->x + (c->w / 2) < c->mon->wx + c->mon->ww)) ? state::bar_height : c->mon->my);
    c->bw = state::config::borderpx;

    log::debug("[manage]");
    wc.border_width = c->bw;
    if (c->x == state::selmon->wx) c->x += (c->mon->ww - WIDTH(c)) / 2 - c->bw;
    if (c->y == state::selmon->wy) c->y += (c->mon->wh - HEIGHT(c)) / 2 - c->bw;
    XConfigureWindow(state::dpy, w, CWBorderWidth, &wc);
    XSetWindowBorder(state::dpy, w, state::scheme[SchemeNorm][ColBorder].pixel);
    configure_(c); /* propagates border_width, if size doesn't change */
    update_window_type(c);
    update_size_hints(c);
    update_wm_hints(c);
    XSelectInput(state::dpy, w, EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask);
    grab_buttons(c, 0);
    log::debug("[manage]");
    if (!c->isfloating)
      c->isfloating = c->oldstate = trans != None || c->isfixed;
    if (c->isfloating)
      XRaiseWindow(state::dpy, c->win);
    attach(c);
    attach_stack(c);
    XChangeProperty(state::dpy, state::root, state::netatom[NetClientList], XA_WINDOW, 32, PropModeAppend,
                    (unsigned char *) &(c->win), 1);
    XMoveResizeWindow(state::dpy, c->win, c->x + 2 * state::sw, c->y, c->w, c->h); /* some windows require this */
    set_client_state(c, NormalState);
    log::debug("[manage]");
    if (c->mon == state::selmon)
      unfocus(state::selmon->sel, 0);
    c->mon->sel = c;
    monitor::arrange(c->mon);
    XMapWindow(state::dpy, c->win);
    focus(nullptr);
    set_urgent(c, urgent);
  }

  Client::sptr from_window(x11::window_t w) {
    for (const auto &monitor: state::monitors::monitors) {
      for (const auto &client: monitor->clients) {
        if (client->win == w)
          return client;
      }
    }
    return nullptr;
  }

  void unmanage(Client::sptr client, bool destroyed) {
    Monitor::sptr m = client->mon;
    x11::window_changes_t wc;

    monitors::detach_client(m, client);
    monitors::detach_from_stack(client);
    if (!destroyed) {
      wc.border_width = client->oldbw;
      x11::with_server([&] { /* avoid race conditions */
        x11::with_no_errors([&] {
          x11::configure_window(client->win, CWBorderWidth, &wc); /* restore border */
          x11::ungrab_button(AnyButton, AnyModifier, client->win);
          client->set_state(WithdrawnState);
          x11::sync(false);
        });
      });
    }
    delete client;
    client::focus(nullptr);
    x11::update_client_list();
    layouts::arrange(m);
  }

}

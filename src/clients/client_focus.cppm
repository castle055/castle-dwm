/*! \file  client_focus.cppm
 *! \brief 
 *!
 */

export module cyd_wm.clients.focus;


export namespace cyd_wm::clients {
  void set_focus(Client::sptr client) {
    if (!client->neverfocus) {
      x11::set_input_focus(client->win, RevertToPointerRoot, CurrentTime);
      x11::change_property(x11::connection::root(), state::atoms::netatom[NetActiveWindow],
                           XA_WINDOW, 32, PropModeReplace,
                           (unsigned char*)&(client->win), 1);
    }
    send_event(client, state::atoms::wmatom[WMTakeFocus]);
  }

  void unfocus(Client::sptr client, int set_focus) {
    if (!client)
      return;
    grab_buttons(client, 0);
    x11::set_window_border(client->win, state::scheme[SchemeNorm][ColBorder].pixel);
    if (set_focus) {
      x11::set_input_focus(x11::connection::root(), RevertToPointerRoot, CurrentTime);
      x11::delete_property(x11::connection::root(), state::atoms::netatom[NetActiveWindow]);
    }
  }
  void focus(Client::sptr client) {
    if (!client || !ISVISIBLE(client)) {
      for (c = state::selmon->stack; c && !ISVISIBLE(c); c = c->snext);
    }
    if (state::monitors::selected_monitor->sel && state::monitors::selected_monitor->sel != client) {
      unfocus(state::monitors::selected_monitor->sel, 0);
    }
    if (client) {
      if (c->mon != state::selmon)
        state::selmon = c->mon;
      if (c->isurgent)
        set_urgent(c, 0);
      detach_stack(c);
      attach_stack(c);
      grab_buttons(c, 1);
      x11::set_window_border(c->win, state::scheme[SchemeSel][ColBorder].pixel);
      set_focus(c);
    } else {
      x11::set_input_focus(x11::connection::root(), RevertToPointerRoot, CurrentTime);
      x11::delete_property(x11::connection::root(), state::atoms::netatom[NetActiveWindow]);
    }
    state::monitors::selected_monitor->sel = client;
    statusbar::update_all();
  }
}
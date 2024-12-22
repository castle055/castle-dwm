/*! \file  client_updates.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xatom.h>
#include <X11/Xutil.h>
export module cyd_wm.clients.updates;

import std;
import x11;
import x11.types;
import cyd_wm.state.atoms;
import cyd_wm.state.monitors;

export namespace cyd_wm::clients {
  void update_size_hints(Client::sptr client) {
    long msize;
    x11::size_hints_t size;

    if (!x11::get_wm_normal_hints(client->win, &size, &msize))
      /* size is uninitialized, ensure that size.flags aren't used */
        size.flags = PSize;
    if (size.flags & PBaseSize) {
      client->basew = size.base_width;
      client->baseh = size.base_height;
    } else if (size.flags & PMinSize) {
      client->basew = size.min_width;
      client->baseh = size.min_height;
    } else
      client->basew = client->baseh = 0;
    if (size.flags & PResizeInc) {
      client->incw = size.width_inc;
      client->inch = size.height_inc;
    } else
      client->incw = client->inch = 0;
    if (size.flags & PMaxSize) {
      client->maxw = size.max_width;
      client->maxh = size.max_height;
    } else
      client->maxw = client->maxh = 0;
    if (size.flags & PMinSize) {
      client->minw = size.min_width;
      client->minh = size.min_height;
    } else if (size.flags & PBaseSize) {
      client->minw = size.base_width;
      client->minh = size.base_height;
    } else
      client->minw = client->minh = 0;
    if (size.flags & PAspect) {
      client->mina = (float) size.min_aspect.y / size.min_aspect.x;
      client->maxa = (float) size.max_aspect.x / size.max_aspect.y;
    } else
      client->maxa = client->mina = 0.0;
    client->isfixed = (client->maxw && client->maxh && client->maxw == client->minw && client->maxh == client->minh);
  }
  void update_title(Client::sptr client) {
    //log::debug("[update_title]");
    if (!x11::get_text_prop(client->win, state::atoms::netatom[NetWMName], client->name)) {
      x11::get_text_prop(client->win, XA_WM_NAME, client->name);
    }
    if (client->name[0] == '\0') { /* hack to mark broken clients */
      client->name = "broken";
    }
  }
  void update_window_type(const Client::sptr &client) {
    x11::atom_t state = client->get_atom_prop(state::atoms::netatom[NetWMState]);
    x11::atom_t wtype = client->get_atom_prop(state::atoms::netatom[NetWMWindowType]);

    if (state == state::atoms::netatom[NetWMFullscreen])
      client->set_fullscreen(1);
    if (wtype == state::atoms::netatom[NetWMWindowTypeDialog])
      client->isfloating = 1;
  }
  void update_wm_hints(Client::sptr client) {
    x11::wm_hints_t *wmh;

    if ((wmh = x11::get_wm_hints(client->win))) {
      if (client == state::monitors::selected_monitor->sel && wmh->flags & XUrgencyHint) {
        wmh->flags &= ~XUrgencyHint;
        x11::set_wm_hints(client->win, wmh);
      } else
        client->isurgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
      if (wmh->flags & InputHint)
        client->neverfocus = !wmh->input;
      else
        client->neverfocus = 0;
      x11::free(wmh);
    }
  }

}

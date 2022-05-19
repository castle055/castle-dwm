//
// Created by castle on 3/24/22.
//

#ifndef CASTLE_DWM_CLIENT_OPS_H
#define CASTLE_DWM_CLIENT_OPS_H

#include "../state/dwm_types.h"

namespace ops::client {
  void apply_rules(client_t* client);
  int apply_size_hints(client_t* client, int *X, int *Y, int *W, int *H, int interact);
  void attach(client_t* client);
  void attach_stack(client_t* client);
  void configure_(client_t* client);
  void detach(client_t* client);
  void detach_stack(client_t* client);
  void focus(client_t* client);
  Atom get_atom_prop(client_t* client, Atom prop);
  void grab_buttons(client_t* client, int focused);
  void pop(client_t* client);
  void resize(client_t* client, int x, int y, int w, int h, int interact);
  void resize_client(client_t* client, int x, int y, int w, int h);
  void set_fullscreen(client_t* client, int fullscreen);
  void set_urgent(client_t* client, int urg);
  void show_hide(client_t* client);
  void unfocus(client_t* client, int set_focus);
  void unmanage(client_t* client, int destroyed);
  void update_size_hints(client_t* client);
  void update_title(client_t* client);
  void update_window_type(client_t* client);
  void update_wm_hints(client_t* client);
  void set_focus(client_t* client);
  client_t* next_tiled(client_t* client);
  client_t *win_to_client(Window w);
  int send_event(client_t *c, Atom proto);
  void send_mon(client_t *c, monitor_t *m);
  void set_client_state(client_t *c, long state);
  void manage(Window w, XWindowAttributes *wa, int urgent);
}


#endif //CASTLE_DWM_CLIENT_OPS_H

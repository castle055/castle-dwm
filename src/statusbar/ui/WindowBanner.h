//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_WINDOWBANNER_H
#define CYD_WM_STATUS_BAR_WINDOWBANNER_H

#include "../state/monitor_types.h"
#include "types.h"
#include "font_icons.h"
#include "../ops/client_ops.h"

namespace cyd_wm::ui {
  COMPONENT(WindowController, {
            client_t* client = nullptr;
            } STATE {
            bool hover[3];
            bool pressed[3];
            }) {
    int size = 10;
    ON_REDRAW {
      WITH_THEME;
      $w(2 * (size + 10) + size);
      return {
        icons::dangerous {
          {
            .color = state->pressed[0] ? color::Black : color::Red,
            .size = size, .bold = true
          }
        }
        .x(0).y(($ch() / 2 - (size / 2)) + 1),
        icons::display_external_input {
          {
            .color = state->pressed[1]
                     ? theme.accent_bright
                     : state->hover[1]
                       ? theme.accent
                       : theme.accent_dim,
            .size = size, .bold = true
          }
        }
        .x(size + 10).y(($ch() / 2 - (size / 2)) + 1),
        icons::overview_key {
          {
            .color = state->pressed[2]
                     ? theme.accent_bright
                     : state->hover[2]
                       ? theme.accent
                       : theme.accent_dim,
            .size = size, .bold = true
          }
        }
        .x(2 * (size + 10)).y(($ch() / 2 - (size / 2)) + 1),
      };
    }

    FRAGMENT {
      if (state->pressed[0]) {
        fragment.append(vg::rect { }.w(size + 5).h($ch()).fill("#ff0000"_color));
      } else if (state->hover[0]) {
        fragment.append(vg::rect { }.w(size + 5).h($ch()).fill("#550000"_color));
      }
    }

    ON_BUTTON_PRESS {
      state->pressed[0] = false;
      state->pressed[1] = false;
      state->pressed[2] = false;
      if (button == Button::PRIMARY) {
        if (x < (size + 5)) {
          state->pressed[0] = true;
        } else if (x < (size + 10) + (size + 5)) {
          state->pressed[1] = true;
        } else if (x < 2 * (size + 10) + (size + 5)) {
          state->pressed[2] = true;
        }
      }
      state->mark_dirty();
    }

    ON_BUTTON_RELEASE {
      state->pressed[0] = false;
      state->pressed[1] = false;
      state->pressed[2] = false;
      constexpr Arg a {0};
      if (button == Button::PRIMARY) {
        if (x < (size + 5)) {
          ops::control::killclient(&a);
        } else if (x < (size + 10) + (size + 5)) {
          ops::control::togglefloating(&a);
        } else if (x < 2 * (size + 10) + (size + 5)) {
          ops::control::zoom(&a);
        }
      }
    }

    ON_MOUSE_ENTER {
      if (x < (size + 5)) {
        state->hover[0] = true;
      } else if (x < (size + 10) + (size + 5)) {
        state->hover[1] = true;
      } else if (x < 2 * (size + 10) + (size + 5)) {
        state->hover[2] = true;
      }
      state->mark_dirty();
    }

    ON_MOUSE_MOTION {
      state->hover[0] = false;
      state->hover[1] = false;
      state->hover[2] = false;
      if (x < (size + 5)) {
        state->hover[0] = true;
      } else if (x < (size + 10) + (size + 5)) {
        state->hover[1] = true;
      } else if (x < 2 * (size + 10) + (size + 5)) {
        state->hover[2] = true;
      }
      state->mark_dirty();
    }

    ON_MOUSE_EXIT {
      state->hover[0] = false;
      state->hover[1] = false;
      state->hover[2] = false;
      state->mark_dirty();
    }
  };

  COMPONENT(WindowListItem, {
            client_t* client = nullptr;
            bool selected = false;
            int item_size = 10;
            }) {
    ON_REDRAW {
      $w(props->item_size);
      $h(props->item_size);

      // Atom da;
      // int di;
      // unsigned long dr;
      // unsigned long dl;
      // unsigned char* p;
      // int ret = XGetWindowProperty(
      //       state::dpy,
      //       props->client->win,
      //       XA_WM_ICON_NAME,
      //       0L, 4,//sizeof XA_WM_ICON_NAME,
      //       False, XA_WM_ICON_NAME,
      //       &da, &di, &dr, &dl, &p);
      // if (ret == Success && p) {
      //   std::string str {(char*)p, dr};
      //   log.info("PROP: %s", str.c_str());
      // }

      return { };
    }

    FRAGMENT {
      WITH_THEME;
      fragment.append(
        vg::rect { }
        .w($cw()).h($ch())
        .stroke(props->selected ? theme.accent : theme.accent_dim)
        .stroke_width(1)
      );
    }

    ON_BUTTON_RELEASE {
      if (button == Button::PRIMARY) {
        ops::client::focus(props->client);
      }
    }
  };

  COMPONENT(WindowList, {
            std::vector<client_t*> clients;
            client_t* selected = nullptr;
            }) {
    ON_REDRAW {
      int padding         = 4;
      int spacing         = 10;
      int item_size       = 26 - 2 * padding;
      int item_offset     = item_size + spacing;
      int total_item_size = (item_offset * (int)props->clients.size()) - spacing;
      int item_y          = padding;
      auto item_x         = ($cw() - total_item_size) / 2;
      int index           = 0;

      return {
        with {props->clients}
        .map_to([&](client_t* client) -> map_to_result_t {
          map_to_result_t c {
            WindowListItem {{client, client == props->selected, item_size}}
            .x(item_x + (index * item_offset)).y(item_y),
          };
          index++;
          return c;
        }),
      };
    }
  };

  COMPONENT(WindowBanner, {
            } STATE {
            monitor_t* monitor = nullptr;
            std::string window_title;
            }) {
    ON_CUSTOM_EVENTS(
      ON_EVENT(WindowStatusUpdate, {
        if (ev->type == win_status_update_t::WINDOW_TITLE) {
        state->window_title = ev->str1;
        state->force_redraw();
        }
        }
      ),
      ON_EVENT(WindowBannerUpdate, {
        state->monitor = ev->monitor;
        state->force_redraw();
        }
      )
    )

    ON_REDRAW {
      std::vector<client_t*> clients { };
      client_t* selected = nullptr;
      if (nullptr != state->monitor) {
        selected    = state->monitor->sel;
        client_t* c = state->monitor->clients;
        while (c != nullptr) {
          if ((c->tags & state->monitor->tagset[state->monitor->seltags]) > 0) {
            clients.push_back(c);
          }
          c = c->next;
        }
      }

      return {
        with {clients.empty()}
        .or_else({
          WindowController {{state->monitor ? state->monitor->sel : nullptr}}
          .x(105).h($ch()),
        }),
        WindowList {{clients, selected}}
        .x(105 + 60).w($cw() - (105 + 60)).h($ch()),
      };
    }

    FRAGMENT {
      // ? Background
      fragment.append(
        vg::rect { }
        .w($cw().val())
        .h($ch().val())
        .fill(color::Transparent /*props->theme.background_secondary*/)
      );
      // fragment.append(
      //   vg::text {state->window_title}
      //   .x(105 + 60 + 5)
      //   .y(19)
      //   .font_family(props->theme->font)
      //   .font_family(props->theme->font2)
      //   .font_size(14)
      //   .font_weight(vg::font_weight_e::BOLD)
      //   .fill(props->theme->accent)
      // );
    }
  };
}

#endif //CYD_WM_STATUS_BAR_WINDOWBANNER_H

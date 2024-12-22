//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_WORKSPACERIBBON_H
#define CYD_WM_STATUS_BAR_WORKSPACERIBBON_H

#include "types.h"

#include "font_icons.h"

namespace cyd_wm::ui {
  COMPONENT(WorkspaceRibbon, { } STATE {
            bool hover = false;
            bool pressed = false;
            bool expanded = false;
            }) {
    ON_REDRAW {
      auto & [background_primary, background_secondary, accent_bright, accent, accent2, accent_dim, accent2_dim, font, font2] = theme_t::get();
      $w($h() + 75);
      return {
        icons::dialogs {
          {
            .color = state->hover ? accent_bright : background_primary,
            .size = 16,
            //.bold = true,
          }
        }
        .x(50)
        .y(5),
      };
    }

    FRAGMENT {
      auto &theme        = theme_t::get();
      Color accent_color = theme.accent;
      if (state->hover) accent_color = theme.accent_dim;
      if (state->pressed) accent_color = color::Black;
      // ? Background
      fragment.append(
        vg::rect { }.w($cw().val() - $ch().val())
                    .h($ch().val())
                    //.fill({.color = props->theme.background_primary})
                    .fill(accent_color),
        vg::polygon { }.points({
                         {$cw().val() - $ch().val(), 0},
                         {$cw().val(), 0},
                         {$cw().val() - $ch().val(), $ch().val()}
                       })
                       //.fill({.color = props->theme.background_primary})
                       .fill(accent_color)
      );
    }

    ON_MOUSE_ENTER {
      state->hover = true;
      state->mark_dirty();
    }

    ON_MOUSE_EXIT {
      state->hover = false;
      state->mark_dirty();
    }

    ON_BUTTON_PRESS {
      if (button == Button::PRIMARY) {
        state->pressed = true;
        state->mark_dirty();
      }
    }

    ON_BUTTON_RELEASE {
      if (button == Button::PRIMARY) {
        state->pressed = false;
        state->mark_dirty();
      }
    }
  };
}

#endif //CYD_WM_STATUS_BAR_WORKSPACERIBBON_H

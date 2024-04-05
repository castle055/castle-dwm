//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_WORKSPACERIBBON_H
#define CYD_WM_STATUS_BAR_WORKSPACERIBBON_H

#include "types.h"

#include "font_icons.h"

namespace cyd_wm::ui {
    COMPONENT(WorkspaceRibbon, {
      theme_t *theme = theme_t::default_theme();
    }) {
      ON_REDRAW {
        return {
          icons::dialogs {{
            .color = props->theme->background_primary,
            .size = 16,
            //.bold = true,
          }}
            .x(50)
            .y(5),
        };
      }
      
      FRAGMENT {
        // ? Background
        fragment.append(
          vg::rect {}
            .w($cw().val() - $ch().val())
            .h($ch().val())
              //.fill({.color = props->theme.background_primary})
            .fill(props->theme->accent),
          vg::polygon {}
            .points({
              {$cw().val() - $ch().val(), 0},
              {$cw().val(), 0},
              {$cw().val() - $ch().val(), $ch().val()}
            })
              //.fill({.color = props->theme.background_primary})
            .fill(props->theme->accent)
        );
      };
    };
}

#endif //CYD_WM_STATUS_BAR_WORKSPACERIBBON_H

//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_WINDOWBANNER_H
#define CYD_WM_STATUS_BAR_WINDOWBANNER_H


#include "types.h"

namespace cyd_wm::ui {
  COMPONENT(WindowBanner, {
    theme_t *theme = theme_t::default_theme();
  } STATE {
    std::string window_title;
  }) {
    ON_CUSTOM_EVENTS(ON_EVENT(WindowStatusUpdate, {
      if (ev->type == win_status_update_t::WINDOW_TITLE) {
        state->window_title = ev->str1;
        state->force_redraw();
      }
    }))
    
    ON_REDRAW {
      return {
      };
    }
    
    FRAGMENT {
      // ? Background
      fragment.append(
        vg::rect {}
          .w($cw().val())
          .h($ch().val())
          .fill(color::Transparent /*props->theme.background_secondary*/)
      );
      fragment.append(
        vg::text {state->window_title}
          .x(140)
          .y(19)
          .font_family(props->theme->font)
          .font_family(props->theme->font2)
          .font_size(14)
          .font_weight(vg::font_weight_e::BOLD)
          .fill(props->theme->accent)
      );
    };
  };
}

#endif //CYD_WM_STATUS_BAR_WINDOWBANNER_H

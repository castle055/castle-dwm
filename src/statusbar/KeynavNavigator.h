//
// Created by castle on 2/1/24.
//

#ifndef CYD_WM_KEYNAVNAVIGATOR_H
#define CYD_WM_KEYNAVNAVIGATOR_H

#include "types.h"
#include "../state/keynav_types.h"
#include "X11/Xlib.h"

namespace cyd_wm::ui {
  EVENT(KeynavUpdate, {
    unsigned long win = 0U;
    std::optional<key_nav_target*> current;
  })
  
  COMPONENT(KeynavTargetItem, {
    theme_t *theme = theme_t::default_theme();
    key_nav_target target;
    KeySym keysym;
  }) {
    ON_REDRAW { return { }; }
    FRAGMENT {
      //log.info("DESC: %s", props->target->description.c_str());
      auto [fg, bg] = get_color(props->target.type);
      str key = str{XKeysymToString(props->keysym)};
      std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      fragment.append(
        vg::rect {}
          .w($cw())
          .h($ch())
          .fill(bg),
        vg::rect {}
          .w(12)
          .h($ch())
          .fill(fg),
        vg::text {{key}}
          .x(2)
          .y(10)
          .fill(color::Black)
          .font_family(props->theme->font2)
          .font_weight(vg::font_weight_e ::BOLD)
          .font_size(11),
        vg::text {{props->target.description}}
          .x(16)
          .y(8)
          .fill(fg)
          .font_family(props->theme->font2)
          .font_weight(vg::font_weight_e ::BOLD)
          .font_size(9)
      );
    }
    
  private:
    static std::pair<Color, Color> get_color(KeyNavTargetType type) {
      switch (type) {
        case KeyNavTargetType::APPLICATION:
          return {"#FCAE1E"_color, "#663300"_color};
        case KeyNavTargetType::NAV_DIR:
          return {"#11AAFF"_color, "#062266"_color};
        case KeyNavTargetType::INTERNAL_VOID_BINDING:
          return {"#00FF00"_color, "#003300"_color};
      }
      return {"#FF0000"_color, "#440000"_color};
    }
  };
  
  COMPONENT(KeynavNavigator, {
    theme_t *theme = theme_t::default_theme();
  } STATE {
    std::optional<key_nav_target*> current = std::nullopt;
  }) {
    ON_CUSTOM_EVENTS(ON_EVENT(KeynavUpdate, {
      state->current = ev->current;
      state->force_redraw();
    }))
    ON_REDRAW {
      int _i = 0;
      if (state->current.has_value()) {
        return {
          stdui::Grid { }
            .rows(2)
            .cols((state->current.value()->map.size() / 2)
                    + (state->current.value()->map.size() % 2))
            .x_gap(4)
            .y_gap(2)
            .x($ch())
            .w($cw() - 2 * $ch())
            .h($ch())
              ({
                 with(state->current.value()->map)
                   .map_to(
                     [&](const std::pair<KeySym, key_nav_target> &it) -> map_to_result_t {
                       int i = _i++;
                       return {
                         KeynavTargetItem {{
                           props->theme,
                           it.second,
                           it.first
                         }}
                           .x(i / 2).y(i % 2).w(1).h(1),
                       };
                     }),
               }),
        };
      } else {
        return {};
      }
    }
    FRAGMENT {
      if (state->current.has_value()) {
        fragment.append(
          vg::rect {}
            .w($cw())
            .h($ch())
            .fill(props->theme->background_primary)
        );
      }
    };
  };
}

#endif //CYD_WM_KEYNAVNAVIGATOR_H

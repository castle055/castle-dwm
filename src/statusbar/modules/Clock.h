//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_CLOCK_H
#define CYD_WM_STATUS_BAR_CLOCK_H

#include "../types.h"

#include <ctime>

#include "./task_timer.h"

namespace cyd_wm::ui {
    COMPONENT(Clock, {
      theme_t &theme = *theme_t::default_theme();
    } STATE {
      std::string time_str = "--:--";
      
      time_t current_time;
      struct tm* lt;
      char t_str[8];
      task_timer_t _redraw_timer {
        {.delay = 5s, .loop = true},
        [&] {emit<RedrawEvent>({.component = this});}
      };
    }) {
      ON_REDRAW {
        $w(56);
        update_time();
        return {};
      }
      
      FRAGMENT {
        fragment.append(
          vg::rect {}
            .w($cw())
            .h($ch())
            .fill(get_background_color()),
          vg::text {state->time_str}
            .x(5)
            .y(14)
              //.font_family("Rampart One")
            .font_family(props->theme.font2)
            .font_size(14)
            .font_weight(vg::font_weight_e::BOLD)
            .fill("#000000"_color)
        );
      };
      
      void update_time() {
        time(&state->current_time);
        state->lt = localtime(&state->current_time);
        
        strftime(state->t_str, sizeof(state->t_str), "%H:%M", state->lt);
        
        state->time_str = std::string {state->t_str};
      }
      
      Color get_background_color() {
        return props->theme.accent;
      }
    };
}

#endif //CYD_WM_STATUS_BAR_CLOCK_H

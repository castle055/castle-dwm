//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_STATUSBANNER_H
#define CYD_WM_STATUS_BAR_STATUSBANNER_H


#include "types.h"

#include "modules/IconRibbon.h"
#include "modules/Clock.h"

#include "modules/storage_ribbon_ctl.h"
#include "modules/network_ribbon_ctl.h"
#include "modules/cpu_ribbon_ctl.h"
#include "modules/memory_ribbon_ctl.h"
#include "modules/temperature_ribbon_ctl.h"

namespace cyd_wm::ui {
    COMPONENT(StatusBanner, {
      theme_t *theme = &default_theme;
    }) {
      ON_REDRAW {
        return {
          Clock {}
            .x($cw() - 56 - 10)
            .h($ch())
            .padding_top(4)
            .padding_bottom(4),
          IconRibbon<icons::notifications> {}
            .x($cw() - 95 - 0 * (14 + 8))
            .h($ch()),
          IconRibbon<icons::cloudy_snowing> {}
            .x($cw() - 95 - 1 * (14 + 8))
            .h($ch()),
          IconRibbon<icons::graphic_eq> {}
            .x($cw() - 95 - 2 * (14 + 8))
            .h($ch()),
          IconRibbon<icons::language, network_icon_ribbon_ctl> {}
            .x($cw() - 115 - 3 * (14 + 8))
            .h($ch()),
          IconRibbon<icons::bluetooth> {}
            .x($cw() - 115 - 4 * (14 + 8))
            .h($ch()),
          IconRibbon<icons::hard_drive, storage_icon_ribbon_ctl> {}
            .x($cw() - 135 - 5 * (14 + 8))
            .h($ch()),
          IconRibbon<icons::developer_board, memory_icon_ribbon_ctl> {}
            .x($cw() - 135 - 7 * (14 + 8) - 6)
            .h($ch()),
          IconRibbon<icons::memory, cpu_icon_ribbon_ctl> {}
            .x($cw() - 135 - 9 * (14 + 8) - 12)
            .h($ch()),
          IconRibbon<icons::thermometer, temperature_icon_ribbon_ctl> {}
            .x($cw() - 135 - 11 * (14 + 8) - 18)
            .h($ch()),
        };
      }
      
      FRAGMENT {
        // ? Background
        fragment.append(
          vg::rect {}
            .x($ch())
            .w($cw() - $ch())
            .h($ch())
            .fill(props->theme->background_primary),
          vg::polygon {}
            .points({
              {0, 0},
              {$ch(), 0},
              {$ch(), $ch()}
            })
            .fill(props->theme->background_primary)
        );
        // ? Separators
        fragment.append(
          vg::line {}
            .stroke(props->theme->accent2)
            .stroke_width(1)
            .stroke_linecap(vg::stroke_linecap_e::ROUND)
            .x1($cw() - 95 - 2 * (14 + 8) - 14)
            .x2($cw() - 95 - 2 * (14 + 8) - 14)
            .y1(3)
            .y2(21),
          vg::line {}
            .stroke(props->theme->accent2)
            .stroke_width(1)
            .stroke_linecap(vg::stroke_linecap_e::ROUND)
            .x1($cw() - 115 - 4 * (14 + 8) - 11)
            .x2($cw() - 115 - 4 * (14 + 8) - 11)
            .y1(3)
            .y2(21)
        );
      };
    };
}

#endif //CYD_WM_STATUS_BAR_STATUSBANNER_H

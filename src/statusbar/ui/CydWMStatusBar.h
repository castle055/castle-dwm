//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_CYDWMSTATUSBAR_H
#define CYD_WM_STATUS_BAR_CYDWMSTATUSBAR_H

#include "types.h"

#include "HomeBanner.h"
#include "WorkspaceRibbon.h"
#include "WindowBanner.h"
#include "StatusBanner.h"

#include "KeynavNavigator.h"
#include "../ops/keynav_ops.h"

namespace cyd_wm {
  COMPONENT(CydWMStatusBar, {
            WorkspaceStatus* status = nullptr;
            } STATE { }) {
    ON_REDRAW {
      auto status_bar_w = 405;;

      return {
        ui::WindowBanner {{}}
        .x(280)
        .w($cw() - 280 - status_bar_w)
        .h($ch()), // - 4),
        ui::KeynavNavigator {{}}
        .x(370)
        .y(::state::key_nav::accepting ? 0 : -100)
        .w($cw() - 370 - 405)
        .h($ch()), // - 4),
        ui::WorkspaceRibbon {{}}
        .x(210 + 70)
        // .w(90 + $ch())
        .h($ch()),
        ui::StatusBanner {{}}
        .x($cw() - status_bar_w - $ch())
        .w(status_bar_w + $ch())
        .h($ch()),
        ui::HomeBanner {{props->status}}
        .w(300 + $ch())
        .h($ch()),
      };
    }
  };
}

#endif //CYD_WM_STATUS_BAR_CYDWMSTATUSBAR_H

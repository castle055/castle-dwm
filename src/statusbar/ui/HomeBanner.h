//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_HOMEBANNER_H
#define CYD_WM_STATUS_BAR_HOMEBANNER_H

#include "types.h"

#include "WorkspaceSelector.h"

static std::string to_roman(int n) {
  switch (n) {
    case 1:
      return "I";
    case 2:
      return "II";
    case 3:
      return "III";
    case 4:
      return "IV";
    case 5:
      return "V";
    case 6:
      return "VI";
    case 7:
      return "VII";
    case 8:
      return "VIII";
    case 9:
      return "IX";
    default:
      return "-";
  }
}

namespace cyd_wm::ui {
  static std::function<void(int button)> selector_action(
    WorkspaceStatus* status,
    int num,
    cydui::window::CWindow* win
  ) {
    return [status, win, num](int button) {
      //printf("win(%X)\n", win);
      if (button == 1) {
        status->selected_workspaces = (1 << num);
        win->emit<WorkspaceEvent>({ });
      } else if (button == 3) {
        status->selected_workspaces = (status->selected_workspaces ^ (1 << num));
        //printf("AFT: %09b\n", status->selected_workspaces);
        win->emit<WorkspaceEvent>({ });
      }
      win->emit<RedrawEvent>({ });
    };
  }

  static std::function<void(int button)> all_selector_action(WorkspaceStatus* status, cydui::window::CWindow* win) {
    return [status, win](int button) {
      if (button == 1) {
        status->selected_workspaces = 0b111111111;
        win->emit<WorkspaceEvent>({ });
      }
      win->emit<RedrawEvent>({ });
    };
  }

  COMPONENT(HomeBanner, {
            WorkspaceStatus* status;
            } STATE {
            bool is_hovering = false;
            }) {
    ON_REDRAW {
      auto &theme = theme_t::get();
      return {
        icons::api {
          {
            .color = theme.accent,
            .size = 20,
          }
        }.x(15)
         .y(($ch() / 2) - (20 / 2)),
        with {9}
        .for_each([&](int &i) -> map_to_result_t {
          return {
            WorkspaceSelector {
              {
                .label = to_roman(i + 1),
                .occupied = (props->status->occupied_workspaces & (1 << (i))) > 0,
                .selected = (props->status->selected_workspaces & (1 << (i))) > 0,
                .on_click = selector_action(props->status, i, window),
              }
            }
            .x(50 + i * 24)
            .w(24)
            .h($ch()),
          };
        }),
        WorkspaceSelector {
          {
            .label = "=",
            .select_all = true,
            .occupied = props->status->occupied_workspaces == 0b111111111,
            .selected = props->status->selected_workspaces == 0b111111111,
            .on_click = all_selector_action(props->status, window),
          }
        }
        .x(50 + 9 * 24)
        .w(24)
        .h($ch()),
      };
    }

    FRAGMENT {
      auto &theme = theme_t::get();
      // ? Background
      fragment.append(
        vg::rect { }
        .w($cw() - $ch())
        .h($ch())
        .fill(theme.background_primary),
        vg::polygon { }
        .points({
          {$cw() - $ch(), $ch()},
          {$cw(), 0},
          {$cw() + $ch(), 0},
          {$cw(), $ch()}
        })
        .fill({
          {$cw() - (2 * $ch() / 3), ($ch() / 3)},
          {$cw() - ($ch() / 3), (2 * $ch() / 3)},
          {
            {0.0, "#000000FF"_color},
            {1.0, "#00000000"_color},
          }
        }),
        vg::polygon { }
        .points({
          {$cw() - $ch(), 0},
          {$cw(), 0},
          {$cw() - $ch(), $ch()}
        })
        .fill(theme.background_primary)
      );
    };

    //ON_MOUSE_ENTER {
    //  state->is_hovering = true;
    //  state->mark_dirty();
    //};
    //ON_MOUSE_EXIT {
    //  state->is_hovering = false;
    //  state->mark_dirty();
    //};
    //ON_MOUSE_MOTION {
    //  state->mark_dirty();
    //};
  };
}


#endif //CYD_WM_STATUS_BAR_HOMEBANNER_H

//
// Created by castle on 11/7/23.
//

#ifndef CYD_WM_STATUS_BAR_WORKSPACESELECTOR_H
#define CYD_WM_STATUS_BAR_WORKSPACESELECTOR_H

#include "types.h"

#include "font_icons.h"

namespace cyd_wm::ui {
    COMPONENT(WorkspaceSelector, {
      theme_t *theme = theme_t::default_theme();
      std::string label = "-";
      bool select_all = false;
      bool occupied = false;
      bool selected = false;
      std::function<void(int)> on_click = [](int button) { };
    } STATE {
      bool is_hovering = false;
      bool is_pressed = false;
    }) {
      ON_REDRAW {
        std::vector<cydui::components::component_holder_t> components {};
        
        if (props->selected) {
          components.push_back(get_select_icon());
        }
        
        if (props->select_all) {
          components.push_back(get_icon<icons::atr>());
        } else {
          if (props->occupied) {
            components.push_back(get_icon<icons::adjust>());
          } else {
            components.push_back(get_icon<icons::circle>());
          }
        }
        
        return components;
      }
      template<icons::IconConcept Icon>
      cydui::components::component_holder_t get_icon() {
        return Icon {{
          .color = props->theme->accent,
          .size = 8,
          .bold = true,
        }}.x(($cw() - 8) / 2)
          .y(($ch() - 8) / 2);
      }
      cydui::components::component_holder_t get_select_icon() {
        //return icons::radio_button_checked {{
        return icons::circle {{
          .color = props->theme->accent2_dim,
          .size = 18,
        }}.x(($cw() - 18) / 2)
          .y(($ch() - 18) / 2 - 1);
      }
      
      FRAGMENT {
        if (state->is_hovering) {
          fragment.append(
            vg::line {}
              .x1($cw() / 2)
              .x2($cw() / 2)
              .y1(0)
              .y2($ch())
              .stroke_dasharray(props->selected
                                ? std::vector<double> {3, 17, 3}
                                : std::vector<double> {5, 13, 5})
              .stroke_width(1)
              .stroke(props->theme->accent)
            //vg::rect {}
            //  .w($cw())
            //  .h($ch())
            //  .fill(props->theme.accent)
            //  .fill({
            //    {0, -3}, {0, $ch() / 1}, {
            //      {0.0, props->theme.accent},
            //      {1.0, color::Transparent},
            //    }
            //  })
          );
          if (state->is_pressed) {
            fragment.append(
              vg::rect {}
                .w($cw())
                .h($ch())
                .fill(props->theme->accent)
                .fill({
                  {$cw() / 2, $ch() / 2}, 5,
                  {$cw() / 2, $ch() / 2}, $cw(),
                  {
                    {0.0, props->theme->accent2_dim},
                    {1.0, color::Transparent},
                  }
                })
            );
          }
        }
        //fragment.append(
        //  vg::text {props->label}
        //    .x(state->is_hovering ? 7 : 10)
        //    .y(state->is_hovering ? 18 : 16)
        //    .font_family(props->theme.font)
        //    .font_size(state->is_hovering ? 14 : 10)
        //      //.font_family("Hurmit Nerd Font")
        //    .font_weight(vg::font_weight_e::BOLD)
        //      //.font_weight(props->active ? vg::font_weight_e::BOLD : vg::font_weight_e::NORMAL)
        //      //.fill(state->is_hovering ? props->theme.background_primary : props->theme.accent)
        //      //.fill(props->theme.accent)
        //    .fill(props->active ? props->theme.accent : props->theme.accent_dim)
        //);
      }
      
      ON_MOUSE_ENTER {
        state->is_hovering = true;
        state->mark_dirty();
        //event_handler_t::on_mouse_enter(x, y);
      };
      ON_MOUSE_EXIT {
        state->is_hovering = false;
        state->is_pressed = false;
        state->mark_dirty();
        //event_handler_t::on_mouse_exit(x, y);
      };
      ON_BUTTON_PRESS {
        if (button == Button::PRIMARY) {
          state->is_pressed = true;
          state->mark_dirty();
        }
      };
      ON_BUTTON_RELEASE {
        props->on_click(static_cast<int>(button));
        if (button == Button::PRIMARY) {
          state->is_pressed = false;
        }
        state->mark_dirty();
      };
    };
}

#endif //CYD_WM_STATUS_BAR_WORKSPACESELECTOR_H

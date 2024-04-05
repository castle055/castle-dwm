//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_ICONRIBBON_H
#define CYD_WM_STATUS_BAR_ICONRIBBON_H

#include "../types.h"

#include "../font_icons.h"
#include "./task_timer.h"

namespace cyd_wm::ui {
    struct icon_ribbon_display_mode_blink_t {
      bool enabled = true;
      std::chrono::milliseconds step_duration = 1s;
      std::vector<uint8_t> steps = {0};
      
      void configure_timer(task_timer_t &timer) const {
        if (enabled) {
          timer.config.initial_delay = 0us;
          timer.config.delay = step_duration;
          timer.config.loop = true;
        }
      }
    };
    
    namespace blink_pattern {
        inline icon_ribbon_display_mode_blink_t even_pulse(std::chrono::milliseconds period = 1s) {
          return {
            .step_duration = period / 2,
            .steps = {2, 0}
          };
        }
        inline icon_ribbon_display_mode_blink_t short_pulse(std::chrono::milliseconds period = 1s) {
          return {
            .step_duration = period / 20,
            .steps = {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
          };
        }
        inline icon_ribbon_display_mode_blink_t burst_2(std::chrono::milliseconds period = 1s) {
          return {
            .step_duration = period / 20,
            .steps = {2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
          };
        }
        inline icon_ribbon_display_mode_blink_t burst_3(std::chrono::milliseconds period = 1s) {
          return {
            .step_duration = period / 20,
            .steps = {2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
          };
        }
        inline icon_ribbon_display_mode_blink_t burst_4(std::chrono::milliseconds period = 1s) {
          return {
            .step_duration = period / 20,
            .steps = {2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
          };
        }
        inline icon_ribbon_display_mode_blink_t burst_5(std::chrono::milliseconds period = 1s) {
          return {
            .step_duration = period / 20,
            .steps = {2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
          };
        }
        
        //!  Specials
        inline icon_ribbon_display_mode_blink_t critical(std::chrono::milliseconds period = 1s) {
          return {
            .step_duration = period / 20,
            .steps = {2, 1, 1, 2, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
          };
        }
    }
    
    struct icon_ribbon_display_mode_t {
      int background_color = 0;
      std::string badge;
      icon_ribbon_display_mode_blink_t blink {.enabled = false};
    };
    
    struct icon_ribbon_controller_i {
      cydui::components::component_state_t* icon_state = nullptr;
      icon_ribbon_display_mode_t mode;
    
    protected:
      void set_mode(const icon_ribbon_display_mode_t &_mode) {
        mode = _mode;
      }
    
    public:
      virtual void update() { };
    };
    
    template<typename T>
    concept IconRibbonControllerConcept = std::derived_from<T, icon_ribbon_controller_i>;
    
    struct default_icon_ribbon_ctl: public icon_ribbon_controller_i { };

#define SET_COMPONENT_TEMPLATE <icons::IconConcept Icon, IconRibbonControllerConcept Controller>
#undef SET_COMPONENT_TEMPLATE_DEFAULT
#define SET_COMPONENT_TEMPLATE_DEFAULT <icons::IconConcept Icon, IconRibbonControllerConcept Controller = default_icon_ribbon_ctl>
#define SET_COMPONENT_TEMPLATE_SHORT <Icon, Controller>
    
    STATE_TEMPLATE(IconRibbon)
    {
      Controller controller {};
      
      long blink_step = 0;
      task_timer_t _redraw_timer {
        {.enabled = true, .delay = 1s, .initial_delay = 0s, .loop = true},
        [&] {
          blink_step++;
          controller.update();
          controller.mode.blink.configure_timer(_redraw_timer);
          emit<RedrawEvent>({.component = this});
        }
      };
      
      bool is_hovering = false;
      bool is_pressed = false;
      
      explicit IconRibbonState(void* props) {
        controller.icon_state = this;
      }
    
    private:
      cydui::window::CWindow* modal_window = nullptr;
    public:
      void toggle_modal_window() {
        if (nullptr == modal_window) {
          //modal_window = cydui::window::create(
          //  cydui::layout::create<icons::team_dashboard>({
          //  }),
          //  "mon 2-1",
          //  "cyd-wm-ui",
          //  3340, 25,
          //  500, 325,
          //  true
          //);
        } else {
          modal_window->terminate();
          delete modal_window;
          modal_window = nullptr;
        }
      }
    };
    COMPONENT_TEMPLATE(IconRibbon, {
      theme_t *theme = theme_t::default_theme();
      int icon_size = 12;
    }) {
      ON_REDRAW {
        std::string badge = this->state->controller.mode.badge;
        int badge_size = 0;
        if (!badge.empty()) {
          badge_size = 9 * badge.size();
        }
        
        $w(this->props->icon_size + badge_size + 4);
        
        return {
          Icon {{
            .color = get_foreground_color(this->state->controller.mode.background_color),
            .size = this->props->icon_size,
            .bold = this->state->is_hovering,
          }}
            .x((($cw() - badge_size) - this->props->icon_size) / 2)
            .y(($ch() - this->props->icon_size) / 2),
        };
      }
      
      FRAGMENT {
        std::string badge = this->state->controller.mode.badge;
        
        fragment.append(
          vg::rect {}
            .w($cw())
            .h($ch())
            .fill(get_background_color(this->state->controller.mode.background_color))
        );
        if (!badge.empty()) {
          fragment.append(
            vg::text {this->state->controller.mode.badge}
              .x(4 + this->props->icon_size + 2)
              .y(($ch() / 2) + 4)
              .font_family(this->props->theme->font2)
              .font_size(12)
              .font_weight(this->state->is_hovering ? vg::font_weight_e::BOLD : vg::font_weight_e::NORMAL)
              .fill(get_foreground_color(this->state->controller.mode.background_color))
          );
        }
      };
      
      Color get_background_color(int i) {
        if (this->state->controller.mode.blink.enabled) {
          long step = this->state->blink_step % this->state->controller.mode.blink.steps.size();
          if (this->state->controller.mode.blink.steps[step] != 0) {
            i = this->state->controller.mode.blink.steps[step];
          }
        }
        switch (i) {
          case 0:
            return this->props->theme->background_primary;
          case 1:
            return "#330000"_color;
          case 2:
            return this->props->theme->accent;
          case 3:
            return this->props->theme->background_primary;
          case 4:
            return "#770000"_color;
          default:
            return this->props->theme->background_secondary;
        }
      }
      Color get_foreground_color(int i) {
        if (this->state->controller.mode.blink.enabled) {
          long step = this->state->blink_step % this->state->controller.mode.blink.steps.size();
          if (this->state->controller.mode.blink.steps[step] != 0) {
            i = this->state->controller.mode.blink.steps[step];
          }
        }
        switch (i) {
          case 0:
            if (this->state->is_hovering) {
              if (this->state->is_pressed) {
                return this->props->theme->accent_dim;
              } else {
                return this->props->theme->accent_bright;
              }
            } else {
              return this->props->theme->accent;
            }
          case 1:
            return "#FF0000"_color;//this->props->theme.accent_bright;
          case 2:
            return this->props->theme->background_secondary;
          case 3:
            return this->props->theme->accent_dim;
          case 4:
            return this->props->theme->background_primary;
          default:
            return this->props->theme->accent;
        }
      }
      
      ON_MOUSE_ENTER {
        this->state->is_hovering = true;
        this->state->mark_dirty();
      };
      ON_MOUSE_EXIT {
        this->state->is_hovering = false;
        this->state->is_pressed = false;
        this->state->mark_dirty();
      };
      ON_BUTTON_PRESS {
        if (button == Button::PRIMARY) {
          this->state->is_pressed = true;
          this->state->mark_dirty();
        }
      };
      ON_BUTTON_RELEASE {
        if (button == Button::PRIMARY) {
          this->state->is_pressed = false;
          this->state->toggle_modal_window();
          this->state->mark_dirty();
        }
      };
    };
}

#endif //CYD_WM_STATUS_BAR_ICONRIBBON_H

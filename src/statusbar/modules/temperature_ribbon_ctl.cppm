//
// Created by castle on 11/21/23.
//

module;
#include <sensors/sensors.h>

export module statusbar.modules.temp;

import std;
import statusbar.modules.icon_ribon;

export namespace cyd_wm::ui {
    struct temperature_icon_ribbon_ctl: public icon_ribbon_controller_i {
      temperature_icon_ribbon_ctl() {
        sensors_init(nullptr);
      }
      ~temperature_icon_ribbon_ctl() {
        sensors_cleanup();
      }
      void update() override {
        const sensors_chip_name* chip;
        int c = 0;
        
        double temperature = 0.0;
        //printf("=========================\n");
        while ((chip = sensors_get_detected_chips(nullptr, &c)) != nullptr) {
          if (std::strcmp(chip->prefix, "zenpower") != 0) continue;
          const sensors_feature* feat;
          int f = 0;
          
          //printf("T(%s <> %s): ", chip->prefix, chip->path);
          while ((feat = sensors_get_features(chip, &f)) != nullptr) {
            if (feat->type == SENSORS_FEATURE_TEMP) {
              sensors_get_value(chip, feat->number, &temperature);
              //printf("%f; ", temperature);
              goto outside_loops;
            }
          }
          //printf("\n");
        }
outside_loops:
        
        check_alarm(temperature);
      }
    
    private:
      void check_alarm(double used) {
        if (used >= 95) {
          set_mode({
            .background_color = 0,
            .badge = std::to_string((long) (used)),
            .blink = blink_pattern::critical(),
          });
        } else if (used >= 90) {
          set_mode({
            .background_color = 1,
            .badge = std::to_string((long) (used)),
            .blink = blink_pattern::short_pulse(3s),
          });
        } else if (used >= 80) {
          set_mode({
            .background_color = 1,
            .badge = std::to_string((long) (used)),
          });
        } else {
          set_mode({
            .badge = std::to_string((long) (used)),
          });
        }
      }
    };
}

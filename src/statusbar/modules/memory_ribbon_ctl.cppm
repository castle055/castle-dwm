//
// Created by castle on 11/21/23.
//

export module statusbar.modules.mem;

import std;
import statusbar.modules.icon_ribon;

export namespace cyd_wm::ui {
    struct memory_icon_ribbon_ctl: public icon_ribbon_controller_i {
      void update() override {
        std::ifstream f("/proc/meminfo");
        std::string line;
        
        long total, available;
        while (std::getline(f, line)) {
          std::istringstream iss {line};
          std::string key;
          long value;
          if (iss >> key >> value) {
            if (key == "MemTotal:") {
              // TOTAL
              total = value;
            } else if (key == "MemAvailable:") {
              // Available
              available = value;
            }
          }
        }
        auto percentage_used = static_cast<double>(total - available) / static_cast<double>(total);
        check_alarm(percentage_used);
      }
    
    private:
      void check_alarm(double used) {
        if (used >= 0.95) {
          set_mode({
            .background_color = 0,
            .badge = std::to_string((long) (used * 100.0)),
            .blink = blink_pattern::critical(),
          });
        } else if (used >= 0.90) {
          set_mode({
            .background_color = 1,
            .badge = std::to_string((long) (used * 100.0)),
            .blink = blink_pattern::short_pulse(3s),
          });
        } else if (used >= 0.80) {
          set_mode({
            .background_color = 1,
            .badge = std::to_string((long) (used * 100.0)),
          });
        } else {
          set_mode({
            .badge = std::to_string((long) (used * 100.0)),
          });
        }
      }
    };
}

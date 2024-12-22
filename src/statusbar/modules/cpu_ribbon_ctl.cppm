//
// Created by castle on 11/21/23.
//

export module statusbar.modules.cpu;

import std;
import statusbar.modules.icon_ribon;

export namespace cyd_wm::ui {
    struct cpu_icon_ribbon_ctl: public icon_ribbon_controller_i {
      long prev_total = 0;
      long prev_idle = 0;
      void update() override {
        std::ifstream f {"/proc/stat"};
        std::string line;
        std::getline(f, line);
        std::istringstream iss {line};
        
        std::string cpu_label;
        std::vector<long> cpu_times {};
        
        if (iss >> cpu_label) {
          if (cpu_label == "cpu") {
            long value;
            while (iss >> value) {
              cpu_times.push_back(value);
            }
          }
        }
        
        if (cpu_times.size() >= 7) {
          // Total CPU time
          long total = cpu_times[0]
            + cpu_times[1]
            + cpu_times[2]
            + cpu_times[3]
            + cpu_times[4]
            + cpu_times[5]
            + cpu_times[6];
          
          // Idle CPU time
          long idle = cpu_times[3];
          
          long d_total = total - prev_total;
          long d_idle = idle - prev_idle;
          prev_total = total;
          prev_idle = idle;
          
          auto percentage_used = static_cast<double>(d_total - d_idle) / static_cast<double>(d_total);
          check_alarm(percentage_used);
        } else {
          set_mode({
            .background_color = 3,
            .badge = "--",
          });
        }
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
            .blink = {.step_duration = 2s}
          });
        } else {
          set_mode({
            .badge = std::to_string((long) (used * 100.0)),
            .blink = {.step_duration = 2s}
          });
        }
      }
    };
}

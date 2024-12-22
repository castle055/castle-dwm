//
// Created by castle on 11/21/23.
//

export module statusbar.module.net;

import std;
import statusbar.modules.icon_ribon;

export namespace cyd_wm::ui {
    struct network_icon_ribbon_ctl: public icon_ribbon_controller_i {
      
      void update() override {
        std::ifstream f("/proc/net/route");
        if (f.good()) {
          std::string line;
          std::getline(f, line);
          if (std::getline(f, line)) {
            // There exists some route to the internet in this system, GREAT!
            set_mode({
              .background_color = 0,
            });
          } else {
            // There isn't any route to the internet in this system, SAD!
            set_mode({
              .background_color = 1,
            });
          }
        } else {
          // It must be the apocalypse
          set_mode({
            .background_color = 0,
            .blink = blink_pattern::burst_2(),
          });
        }
      }
    };
}

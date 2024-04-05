//
// Created by castle on 11/21/23.
//

#ifndef CYD_WM_STATUS_BAR_STORAGE_RIBBON_CTL_H
#define CYD_WM_STATUS_BAR_STORAGE_RIBBON_CTL_H

#include "IconRibbon.h"

#include "sys/statvfs.h"

namespace cyd_wm::ui {
    struct storage_icon_ribbon_ctl: public icon_ribbon_controller_i {
      struct statvfs stat;
      void update() override {
        set_mode({
          .background_color = 0,
        });
        
        unsigned long long free, total;
        double used = 0.0;
        if (statvfs("/", &stat) == 0) {
          total = stat.f_frsize * stat.f_blocks;
          free = stat.f_frsize * stat.f_bavail;
          used = std::max(used, 1.0 - ((double) free / (double) total));
          check_alarm(used);
        }
        if (statvfs("/home", &stat) == 0) {
          total = stat.f_frsize * stat.f_blocks;
          free = stat.f_frsize * stat.f_bavail;
          used = std::max(used, 1.0 - ((double) free / (double) total));
          check_alarm(used);
        }
        if (statvfs("/data", &stat) == 0) {
          total = stat.f_frsize * stat.f_blocks;
          free = stat.f_frsize * stat.f_bavail;
          used = std::max(used, 1.0 - ((double) free / (double) total));
          check_alarm(used);
        }
      }
    
    private:
      void check_alarm(double used) {
        if (used >= 0.98) {
          set_mode({
            .background_color = 0,
            .blink = blink_pattern::critical(),
          });
        } else if (used >= 0.95) {
          set_mode({
            .background_color = 1,
            .blink = blink_pattern::short_pulse(5s),
          });
        } else if (used >= 0.8) {
          set_mode({
            .background_color = 1,
          });
        }
      }
    };
}

#endif //CYD_WM_STATUS_BAR_STORAGE_RIBBON_CTL_H

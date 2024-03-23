//
// Created by castle on 11/21/23.
//

#ifndef CYD_WM_STATUS_BAR_TASK_TIMER_H
#define CYD_WM_STATUS_BAR_TASK_TIMER_H

#include "../types.h"

namespace cyd_wm::ui {
    struct task_timer_t {
      struct config_t {
        bool enabled = true;
        std::chrono::microseconds delay = 1s;
        std::chrono::microseconds initial_delay = delay;
        bool loop = false;
      };
      
      explicit task_timer_t(config_t _config, auto _action) {
        this->config = _config;
        this->action = _action;
        
        this->thd = std::thread {[&]() {
          std::this_thread::sleep_for(config.initial_delay);
          auto t0 = std::chrono::system_clock::now();
          do {
            t0 = std::chrono::system_clock::now();
            if (config.enabled) {
              action();
            }
            std::this_thread::sleep_until(t0 + config.delay);
          } while (running && config.loop);
        }};
        pthread_t pt = this->thd.native_handle();
        pthread_setname_np(pt, "task-timer");
      }
      
      void terminate() {
        running = false;
      }
      config_t config {};
    private:
      std::function<void()> action = [] { };
      
      std::atomic<bool> running = true;
      std::thread thd;
    };
}

#endif //CYD_WM_STATUS_BAR_TASK_TIMER_H

//
// Created by castle on 9/11/22.
//

#ifndef CYD_UI_CLOCK_MODULE_HPP
#define CYD_UI_CLOCK_MODULE_HPP

#include "cyd-ui/dist/include/cydui.hpp"
#include "./button.hpp"

#include <ctime>

STATE(ClockModule) {
  INIT_STATE(ClockModule) {
    
  }
};

COMPONENT(ClockModule) {
  PROPS({
  })
  
  INIT(ClockModule) {
  }
  
  REDRAW {
    std::time_t t = std::time(0);
    std::string time;
    std::tm* now = std::localtime(&t);
    
    if (now->tm_hour >= 10) {
      time.append(std::to_string(now->tm_hour));
    } else {
      time.append(" ");
      time.append(std::to_string(now->tm_hour));
    }
    time.append(":");
    if (now->tm_min >= 10) {
      time.append(std::to_string(now->tm_min));
    } else {
      time.append("0");
      time.append(std::to_string(now->tm_min));
    }
    
    add({
      COMP(Button)({
        .props = {
          .text = time,
        },
        .w = 50,
      }),
    });
  }
};

#endif //CYD_UI_CLOCK_MODULE_HPP

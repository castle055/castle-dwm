//
// Created by castle on 11/6/23.
//

#ifndef CYD_WM_STATUS_BAR_TYPES_H
#define CYD_WM_STATUS_BAR_TYPES_H

#include <cydui/cydui.hpp>
#include <cydui/stdui.h>
#include "../state/core_types.h"

namespace cyd_wm {
  using color::Color;
  struct theme_t {
    Color background_primary = "#06060a"_color;
    Color background_secondary = "#111326"_color;
    
    Color accent_bright = "#FCCE5E"_color;
    Color accent = "#FCAE1E"_color;
    Color accent2 = "#00afcc"_color;
    Color accent_dim = "#775f11"_color;
    Color accent2_dim = "#004f7f"_color;
    //Color accent2_dim = "#002f5c"_color;
    //Color accent2_dim = "#004f7c"_color;
    
    std::string font = "Poiret One";
    std::string font2 = "Hurmit Nerd Font";
  };
  
  static theme_t default_theme {};
  
  EVENT(WorkspaceEvent, {
    unsigned long win = 0U;
    bool start_menu = false;
  })
  
  enum class win_status_update_t {
    WINDOW_TITLE,
    LAYOUT_SYMBOL,
  };
  
  EVENT(WindowStatusUpdate, {
    unsigned long win = 0U;
    win_status_update_t type = win_status_update_t::WINDOW_TITLE;
    std::string str1;
  })
  
  struct WorkspaceStatus {
    int occupied_workspaces = 0;
    int selected_workspaces = 1;
  };
  
}

#endif //CYD_WM_STATUS_BAR_TYPES_H

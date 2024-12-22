//
// Created by castle on 11/6/23.
//

module;
#include "../state/core_types.h"
#include <cyd_fabric_modules/headers/macros/async_events.h>

export module statusbar.types;

export import cydui.std;

export namespace cyd_wm {
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
    
    static theme_t& get() {
      static theme_t t{};
      return t;
    }
  };

  inline theme_t& with_theme() {
    return theme_t::get();
  }
  
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

  EVENT(WindowBannerUpdate, {
    monitor_t* monitor = nullptr;
  })
  
  struct WorkspaceStatus {
    int occupied_workspaces = 0;
    int selected_workspaces = 1;
  };
  
}

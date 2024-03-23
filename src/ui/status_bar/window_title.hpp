//
// Created by castle on 10/2/22.
//

#ifndef CYD_UI_WINDOW_TITLE_HPP
#define CYD_UI_WINDOW_TITLE_HPP

#include "cydui/cydui.hpp"

using namespace primitives;

enum class win_status_update_t {
  WINDOW_TITLE,
  LAYOUT_SYMBOL,
};

EVENT(WindowStatusUpdate, {
  unsigned int win = 0U;
  win_status_update_t type;
  std::string str1 = "";
})

STATE(WinTitle) {
  color::Color c_bkg  = "#111326"_color;
  
  font::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  color::Color color = "#FCAE1E"_color;
  
  std::string layout_symbol = "X";
  std::string window_title = "test window - with long name";
  
  INIT_STATE(WinTitle) {
    listen(WindowStatusUpdate, {
      auto xwin = this->win? (unsigned int)(*this->win.unwrap())->xwin: 0;
      if (it.data->win == xwin) {
        switch (it.data->type) {
          case win_status_update_t::WINDOW_TITLE:
            this->window_title = it.data->str1;
            break;
          case win_status_update_t::LAYOUT_SYMBOL:
            this->layout_symbol = it.data->str1;
            break;
        }
      }
    });
  }
};

COMPONENT(WinTitle) {
  NO_PROPS
  
  INIT(WinTitle) {
  
  }
  
  REDRAW {
    auto [width,height] = graphics::get_text_size(&state->font, state->window_title);
    auto [width_ls,height_ls] = graphics::get_text_size(&state->font, state->layout_symbol);
    add({
      COMP(Rectangle)({
        .props = {
          .color = state->c_bkg,
          .filled = true
        },
        .w = 350,
        .h = 23,
      }),
      COMP(Text)({
        .props = {
          .color = state->color,
          .font = &state->font,
          .text = state->layout_symbol,
        },
        .x = 5,
        .y = height_ls,
      }),
      COMP(Text)({
        .props = {
          .color = state->color,
          .font = &state->font,
          .text = state->window_title,
        },
        .x = 40,
        .y = height,
      }),
    });
  }
};

#endif //CYD_UI_WINDOW_TITLE_HPP

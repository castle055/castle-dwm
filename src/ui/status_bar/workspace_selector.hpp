//
// Created by castle on 10/1/22.
//

#ifndef CYD_UI_WORKSPACE_SELECTOR_HPP
#define CYD_UI_WORKSPACE_SELECTOR_HPP

#include <cydui/cydui.hpp>
#include "../components/button.hpp"

STATE(WorkspaceSelector) {
  
  color::Color c_nominal  = "#111326"_color;
  color::Color c_selected = "#2d2310"_color;
  color::Color c_occupied = "#FCAE1E"_color;
  
  INIT_STATE(WorkspaceSelector) {
  
  }
};

COMPONENT(WorkspaceSelector) {
  PROPS({
    std::string text = "-";
    font::Font* font;
    bool         occupied = false;
    bool         selected = false;
    ButtonAction on_click;
  })
  
  INIT(WorkspaceSelector) { }
  
  REDRAW {
    add({
      COMP(Button)({
        .props = {
          .text = props.text,
          .font = props.font,
          .on_action = props.on_click,
          .c_dim = props.selected? state->c_selected : state->c_nominal
        },
        .w = 20,
        .h = 23,
      }),
      COMP(Rectangle)({
        .props = {
          .color = props.occupied? (state->c_occupied) : (props.selected? state->c_selected: state->c_nominal),
          .filled = true
        },
        .w = props.occupied? 3: 0,
        .h = props.occupied? 3: 0,
      })
    });
  }
};

#endif //CYD_UI_WORKSPACE_SELECTOR_HPP

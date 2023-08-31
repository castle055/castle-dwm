//
// Created by castle on 10/1/22.
//

#ifndef CYD_UI_WORKSPACE_SELECTOR_HPP
#define CYD_UI_WORKSPACE_SELECTOR_HPP

#include <cyd-ui/dist/include/cydui.hpp>
#include "../components/button.hpp"

STATE(WorkspaceSelector) {
  
  cydui::layout::color::Color* c_nominal  = new cydui::layout::color::Color("#111326");
  cydui::layout::color::Color* c_selected = new cydui::layout::color::Color("#2d2310");
  cydui::layout::color::Color* c_occupied = new cydui::layout::color::Color("#FCAE1E");
  
  INIT_STATE(WorkspaceSelector) {
  
  }
};

COMPONENT(WorkspaceSelector) {
  PROPS({
    std::string text = "-";
    cydui::layout::fonts::Font* font;
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

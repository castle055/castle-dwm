//
// Created by castle on 8/31/22.
//

#ifndef CYD_UI_BUTTON_HPP
#define CYD_UI_BUTTON_HPP

#include <functional>
#include "cyd-ui/dist/include/cydui.hpp"

STATE(Button) {
  bool hovering = false;
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  cydui::layout::color::Color* c     = new cydui::layout::color::Color("#FCAE1E");
  cydui::layout::color::Color* c_dim = new cydui::layout::color::Color("#2d2310");
  cydui::layout::color::Color* c1    = new cydui::layout::color::Color("#000000");
};

typedef std::function<void(int button)> ButtonAction;
#define action [&, this]

COMPONENT(Button) {
  PROPS({
    std::string text = "Text";
    cydui::layout::fonts::Font* font = nullptr;
    ButtonAction on_action = [](int) { };
    cydui::layout::color::Color* c     = nullptr;
    cydui::layout::color::Color* c_dim = nullptr;
    cydui::layout::color::Color* c1    = nullptr;
    
    bool toggled = false;
  })
  
  INIT(Button) {
    if (!this->props.font) this->props.font   = &state->font;
    if (!this->props.c) this->props.c         = state->c;
    if (!this->props.c_dim) this->props.c_dim = state->c_dim;
    if (!this->props.c1) this->props.c1       = state->c1;
  }
  
  REDRAW {
    auto content_size = graphics::get_text_size(&state->font, props.text);
    add({
      COMP(Text)({
        .props = {
          .color = state->hovering || props.toggled? props.c1 : props.c,
          .font = props.font,
          .text = props.text,
        },
        .x = 5,
        .y = ((dim->ch/2) + (state->font.size/2)) - content_size.second,
        //.init = [](auto* t){
        //t->state->dim.margin = 5;
        //},
      }),
      COMP(Rectangle)({
        .props = {
          .color = state->hovering || props.toggled? props.c : props.c_dim,
          .filled = true
        },
        .w = state->dim.cw,
        .h = state->dim.ch,
      }),
    }, true);
  }
  
  ON_MOUSE_ENTER(x,y) {
    state->hovering = true;
    state->dirty();
  }
  
  ON_MOUSE_EXIT(x,y) {
    state->hovering = false;
    state->dirty();
  }
  
  ON_CLICK(x,y,button) {
    state->hovering = false;
    props.on_action(button);
    state->dirty();
  }
};

#endif //CYD_UI_BUTTON_HPP

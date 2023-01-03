//
// Created by castle on 10/1/22.
//

#ifndef CYD_UI_WORKSPACE_SELECTOR_HPP
#define CYD_UI_WORKSPACE_SELECTOR_HPP

#include <cyd_ui/cydui.hpp>
#include "../components/button.hpp"

STATE(WorkspaceSelector)
  
  cydui::layout::color::Color* c_nominal  = new cydui::layout::color::Color("#111326");
  cydui::layout::color::Color* c_selected = new cydui::layout::color::Color("#2d2310");
  cydui::layout::color::Color* c_occupied = new cydui::layout::color::Color("#FCAE1E");
  
  INIT_STATE(WorkspaceSelector) {
  
  }
};

COMPONENT(WorkspaceSelector)
  PROPS({
    std::string text;
    cydui::layout::fonts::Font* font;
    bool         occupied = false;
    bool         selected = false;
    ButtonAction on_click;
  })
  
  INIT(WorkspaceSelector) DISABLE_LOG
  
  }
  
  REDRAW {
    WITH_STATE(WorkspaceSelector)
    
    using namespace primitives;
    ADD_TO(this, ({
      N(Button, ({
        .text = props.text,
        .font = props.font,
        .on_action = props.on_click,
        .c_dim = props.selected? state->c_selected : state->c_nominal
      }), ({ }), {
        thisButton->set_width(20);
      }),
      N(Rectangle, ({
        .color = props.occupied? (state->c_occupied) : (props.selected? state->c_selected: state->c_nominal),
        .filled = true
      }), ({ }), {
        thisRectangle->set_pos(this, 0, 0);
        if (props.occupied) {
          thisRectangle->set_size(3, 3);
        } else {
          thisRectangle->set_size(0, 0);
        }
      })
    }))
  }
};

#endif //CYD_UI_WORKSPACE_SELECTOR_HPP

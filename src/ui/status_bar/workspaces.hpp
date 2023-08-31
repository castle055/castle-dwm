// Created by castle on 9/27/22.
//

#ifndef CYD_UI_WORKSPACES_HPP
#define CYD_UI_WORKSPACES_HPP

#include "cyd-ui/dist/include/cydui.hpp"
#include "cyd-ui/dist/include/containers.hpp"
#include "../components/button.hpp"
#include "../components/clock_module.hpp"
#include "workspace_selector.hpp"

EVENT(WorkspaceEvent, {
  unsigned int win = 0U;
  bool start_menu = false;
})

struct WorkspaceStatus {
  int occupied_workspaces = 0;
  int selected_workspaces = 1;
};

STATE(Workspaces) {
  cydui::layout::color::Color* c_bkg  = new cydui::layout::color::Color("#111326");
  //IntProperty occupied_workspaces;
  //IntProperty selected_workspaces = 1;
  
  cydui::layout::fonts::Font font {
    .name = "Fira Code Retina",
    .size = 10
  };
  
  INIT_STATE(Workspaces) {
    //occupied_workspaces.bind(this);
    //selected_workspaces.bind(this);
    //selected_workspaces = (1 << 0);
  }
};

static ButtonAction selector_action(WorkspaceStatus* status, int num, unsigned int win) {
  return [status,win,num](int button) {
    if (button == 1) {
      status->selected_workspaces = (1 << num);
      events::emit<WorkspaceEvent>({win});
    } else if (button == 3) {
      status->selected_workspaces = (status->selected_workspaces ^ (1 << num));
      events::emit<WorkspaceEvent>({win});
    }
  };
}

static ButtonAction all_selector_action(WorkspaceStatus* status, unsigned int win) {
  return [status,win](int button) {
    if (button == 1) {
      status->selected_workspaces = 0b11111111;
      events::emit<WorkspaceEvent>({win});
    }
  };
}


#define SELECTOR(N) \
COMP(WorkspaceSelector)({ \
  .props = {        \
    .text = #N,     \
    .font = &state->font, \
    .occupied = (props.status->occupied_workspaces & (1 << (N-1))) > 0, \
    .selected = (props.status->selected_workspaces & (1 << (N-1))) > 0, \
    .on_click = selector_action(props.status, N-1, win) \
  }                 \
})
#define ALL_SELECTOR() \
COMP(WorkspaceSelector)({ \
  .props = {        \
    .text = "*",     \
    .font = &state->font, \
    .occupied = props.status->occupied_workspaces == 0b11111111, \
    .selected = props.status->selected_workspaces == 0b11111111, \
    .on_click = all_selector_action(props.status, win)           \
  }                 \
})



COMPONENT(Workspaces) {
  PROPS({
    WorkspaceStatus* status;
  })
  INIT(Workspaces) { }
  
  REDRAW {
    log.info("=== REDRAWING Workspaces");
    auto win = state->win? (unsigned int)(*state->win.unwrap())->xwin: 0;
    add({
      COMP(Rectangle)({
        .props = {
          .color = state->c_bkg,
          .filled = true
        },
        .w = 280,
        .h = 23,
      }),
      //COMP(Image)({
      //  .props = {
      //    .img = "/etc/start_logo.jpg",
      //    //.img = "/home/castle/.config/corium/startmenu/start_logo.jpg",
      //  },
      //  .x = 0,
      //  .w = 23,
      //}),
      COMP(HBox)({
        .props = {
          .spacing = 3,
        },
        .x = 36,
        .y = 0,
        .w = 270,
        .inner = {
          SELECTOR(1),
          SELECTOR(2),
          SELECTOR(3),
          SELECTOR(4),
          SELECTOR(5),
          SELECTOR(6),
          SELECTOR(7),
          SELECTOR(8),
          SELECTOR(9),
          ALL_SELECTOR(),
        },
      }),
    });
  }
  
  ON_CLICK(x, y, button) {
    log.info("=== START MENU CLICK");
    if (x < 23) {
      auto win = state->win? (unsigned int)(*state->win.unwrap())->xwin: 0;
      events::emit<WorkspaceEvent>({win, true});
    }
  }
};

#undef SELECTOR
#undef ALL_SELECTOR

#endif//CYD_UI_WORKSPACES_HPP

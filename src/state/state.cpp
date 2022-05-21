//
// Created by castle on 3/24/22.
//

#include <fstream>
#include "state.h"
#include "../ops/keynav_ops.h"

std::fstream state::log_file;
std::fstream state::key_nav_file;

const char state::broken[] = "broken";

std::string state::stext;

Atom state::wmatom[WMLast], state::netatom[NetLast];
Display* state::dpy;
int state::screen;
Drw *state::drw;
monitor_t *state::mons, *state::selmon;
Window state::root, state::wmcheckwin;
Clr **state::scheme;
int state::sw, state::sh, state::bar_height, state::blw;
int state::lrpad;
unsigned int state::numlockmask;
Cur *state::cursor[CurLast];
bool state::running = true;

bool state::key_nav::accepting = false;
std::string state::key_nav::current_path = "|>";
key_nav_target* state::key_nav::current = &ops::keynav::root;

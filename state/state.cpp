//
// Created by castle on 3/24/22.
//

#include <fstream>
#include "state.h"

std::fstream state::log_file;

const char state::broken[] = "broken";

std::string state::stext;

Atom state::wmatom[WMLast], state::netatom[NetLast];
Display* state::dpy;
int state::screen;
Drw *state::drw;
monitor_t *state::mons, *state::selmon;
Window state::root, state::wmcheckwin;
Clr **state::scheme;
int state::sw, state::sh, state::bh, state::blw;
int state::lrpad;
unsigned int state::numlockmask;
Cur *state::cursor[CurLast];
bool state::running = true;

bool state::key_nav::accepting = false;
std::string state::key_nav::current_path = XKeysymToString(state::config::key_nav::trigger.keysym);
key_nav_target state::key_nav::root;
key_nav_target* state::key_nav::current = &state::key_nav::root;

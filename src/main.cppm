
module;
// #include "config/files.h"

export module main;

import std;
import fabric.logging;

import cyd_wm.state.config;
import cyd_wm.state.misc;
import cyd_wm.types;

import cyd_wm.setup;
import cyd_wm.cleanup;
import cyd_wm.main_loop;


int main(int argc, char *argv[]) {
  using namespace cyd_wm;

  // config::load();
  // return 0;
  LOG::print {INFO}("Starting Cyd-WM...");

  state::log_file.open(state::config::log_file.c_str(), std::ios::app);

  setup();
  main_loop::run();
  cleanup();

  return 0;
}

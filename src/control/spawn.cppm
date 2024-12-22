/*! \file  spawn.cppm
 *! \brief 
 *!
 */

export module cyd_wm.control.spawn;

import cyd_wm.types;

namespace cyd_wm::control {
  export
  void spawn_exec(const Arg *arg) {
    XSync(state::dpy, false);
    if (fork() == 0) {
      //program_shell::stop();
      if (state::dpy)
        close(ConnectionNumber(state::dpy));
      setsid();
      const char* cmd = (const char*)arg->v;
      log::debug("[spawn] running cmd: %s", cmd);
      std::system(cmd);
      log::debug("[spawn] running cmd: %s", cmd);
      fprintf(stderr, "dwm: execvp %s", cmd);
      perror(" failed");
      exit(EXIT_SUCCESS);
    }
  }
  export
  void spawn(const Arg *arg) {
    if (state::config::cmds.find((char*)arg->v) == state::config::cmds.end()) {
      log::error("[spawn] No such command: %s", (char*)arg->v);
    }
    if (fork() == 0) {
      //program_shell::stop();
      if (state::dpy)
        close(ConnectionNumber(state::dpy));
      setsid();
      char** cmd = (char**)&state::config::cmds[(const char*)arg->v][0];
      if (strcmp((const char*)arg->v, "dmenucmd") == 0) {
        cmd[2] = (char*) std::to_string(state::selmon->num).c_str();
      }
      log::debug("[spawn] running cmd: %s", cmd[0]);
      execvp(cmd[0], cmd);
      perror(" failed");
      exit(EXIT_SUCCESS);
    }
  }
}
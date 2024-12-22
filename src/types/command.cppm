/*! \file  command.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:command;

import :arg;

export namespace cyd_wm {
  struct Command {
    const char* name;

    void (* func)(const Arg* arg);

    const Arg arg;
  };
}

/*! \file  atoms.cppm
 *! \brief 
 *!
 */

export module cyd_wm.state.keynav;

import std;
import cyd_wm.types;

export namespace state::keynav {
    bool accepting;
    std::string current_path;
    ::keynav::key_nav_target* current;
}
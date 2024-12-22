/*! \file  atoms.cppm
 *! \brief 
 *!
 */

export module cyd_wm.state.config;

import std;
import cyd_wm.types;

export namespace state::config {
    std::string log_file;
    std::string key_nav_file;
    unsigned short ctl_port;
    unsigned int borderpx;
    unsigned int snap;
    int showbar;
    int topbar;
    std::vector<const char*> fonts;
    std::string dmenufont;
    std::string normbgcolor;
    std::string selbgcolor;
    std::string normbordercolor;
    std::string selbordercolor;
    std::string normfgcolor;
    std::string selfgcolor;
    std::vector<std::vector<const char*>> colors;
    unsigned int gappx;
    const std::vector<std::string> TAGS;
    std::vector<cyd_wm::Rule> rules;
    float mfact;
    int nmaster;
    int resizehints;
    std::vector<cyd_wm::Layout> layouts;
    unsigned int default_layout;
    std::array<std::array<cyd_wm::DefaultLayout, 9>, 3> default_layouts;
    void setup_default_layouts();
    unsigned int dmenumon;
    std::unordered_map<std::string, std::vector<const char*>> cmds;
    std::vector<cyd_wm::KeySpec>          keys;
    std::vector<cyd_wm::MouseButton>  buttons;
    std::vector<cyd_wm::ResourcePref> resources;

    namespace key_nav {
      long accepting_timespan;
      cyd_wm::KeyTrigger trigger;
    }
}
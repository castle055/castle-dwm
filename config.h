#include <X11/XF86keysym.h>
#include <cstdlib>
#include "src/state/state.h"

#ifndef CDWM_CONFIG
#define CDWM_CONFIG

#include "src/layouts/monocle.h"
#include "src/layouts/tile.h"
#include "src/layouts/centeredmaster.h"

#include "src/ops/log_ops.h"

namespace state {

#define DEVEL false

#if DEVEL
#define MODKEY Mod4Mask
  unsigned short config::ctl_port= 33851;
  std::string config::log_file = "/dev/stdout";
#else
#define MODKEY Mod1Mask
  unsigned short config::ctl_port = 33850;
  std::string config::log_file = "/home/castle/.cache/castle-dwm.log";
#endif
  
  std::string config::key_nav_file = "/home/castle/.config/corium/keynav.yaml";
  
  /* appearance */
  unsigned int config::borderpx = 0;        /* border pixel of windows */
  unsigned int config::snap     = 30;       /* snap pixel */
  int          config::showbar  = 1;        /* 0 means no bar */
  int          config::topbar   = 1;        /* 0 means bottom bar */
  //static const char *fonts[]          = { "monospace:size=12" };
  //static const char *fonts[]	    = { "Iosevka Term:style=Bold:size=9:antialias=true:autohint:true" };
  std::vector<const char*>
                                        config::fonts           =
    {"Fira Code Retina:style=Bold:size=9:antialias=true:autohint:true"};
  std::string                           config::dmenufont       = "Fira Code Retina:size=14";
  //static const char dmenufont[]       = "Iosevka Term:size=14";
  std::string                           config::normbgcolor     = "#111326";
  std::string                           config::selbgcolor      = "#06060a";
  std::string                           config::normbordercolor = "#fcae1e";
  std::string                           config::selbordercolor  = "#fcae1e";
  std::string                           config::normfgcolor     = "#fcae1e";
  std::string                           config::selfgcolor      = "#fcae1e";
  //static char col_red[]	      = "#cc0000";
  //static char col_red2[]	      = "#150000";
  //static char col_red3[]	      = "#0a0000";
  std::vector<std::vector<const char*>> config::colors          = {
    /*               fg         bg         border   */
    {
      config::normfgcolor.c_str(),
      config::normbgcolor.c_str(),
      config::normbordercolor.c_str()
    },
    {
      config::selfgcolor.c_str(),
      config::selbgcolor.c_str(),
      config::selbordercolor.c_str()
    }
  };
  
  /* Gapps */
  unsigned int config::gappx = 20;
  
  /* tagging */
  const std::vector<std::string>config::TAGS = {
    "\uf269",
    "\uf121",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "\uf274"
  };
  
  std::vector<Rule> config::rules = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       TAGS mask     isfloating   monitor */
    { "Gimp", NULL, NULL, 0, 1, -1 },
    { "google-chrome", NULL, NULL, 1 << 7, 0, 1 },
    { "emacs", NULL, NULL, 1 << 1, 0, 1 },
    { "firefox", NULL, NULL, 1 << 0, 0, 0 },
    { "thunderbird", NULL, NULL, 1 << 8, 0, 0 },
    { "Thunderbird", NULL, NULL, 1 << 8, 0, 0 },
    { "jetbrains-idea", NULL, NULL, 1 << 4, 0, 1 },
    { "jetbrains-clion", NULL, NULL, 1 << 5, 0, 1 },
    { "jetbrains-webstorm", NULL, NULL, 1 << 6, 0, 1 },
    { "android-studio", NULL, NULL, 1 << 5, 0, 1 },
    { "st", NULL, NULL, 1 << 1, 0, -1 },
    { NULL, "zathura", NULL, 1 << 2, 0, 2 },
    { "scratch", NULL, NULL, 0, 1, -1 },
    //	{ "Main",     NULL,       NULL,       0,            1,           -1},
    { NULL, NULL, "Teensy", 0, 1, -1 },
    { "TelegramDesktop", NULL, NULL, 1 << 0, 0, 2 },
    { "discord", NULL, NULL, 1 << 0, 0, 2 },
    { "Slack", NULL, NULL, 1 << 0, 0, 2 },
    { "GitKraken", NULL, NULL, 1 << 3, 0, 0 },
    { "Inkscape", NULL, NULL, 1 << 7, 0, 1 },
    { "Binance", NULL, NULL, 1 << 2, 0, 0 },
  };
  
  /* layout(s) */
  float config::mfact       = 0.55; /* factor of master area size [0.05..0.95] */
  int   config::nmaster     = 1;    /* number of clients in master area */
  int   config::resizehints = 1;    /* 1 means respect size hints in tiled resizals */
  std::vector<Layout> config::layouts = {
    /* symbol     arrange function */
    { "[]=", tile },    /* first entry is default */
    { "><>", NULL },    /* no layout function means floating behavior */
    { "[M]", monocle },
    { "|M|", centeredmaster },
    { ">M>", centeredfloatingmaster },
  };
  unsigned int config::default_layout = 2;
  
  //        number of monitors ------+---+
  // UPDATE ASCII REPRESENTATION HERE
  std::array<std::array<DefaultLayout, 9>, 3> config::default_layouts;
  
  //    /* MON 0 */ {2,2,2,2,2,2,2,1,2},
  //    /* MON 1 */ {2,2,2,2,2,2,2,1,2},
  //    /* MON 2 */ {3,3,2,2,2,2,2,1,2},
  void config::setup_default_layouts() {
    static bool done = false;
    if (done) return;
    done = true;
    ops::log::debug("[setup_default_layouts]");
    
    //  default_layouts[0][1] = {.ltidx = 0, .nmaster = 1};
    //  default_layouts[1][1] = {.ltidx = 0, .nmaster = 1};
    
    // ADD LAYOUT DEFINITIONS HERE
    //  Workspace 8 on EVERY monitor is FLOATING LAYOUT
    default_layouts[0][7] = {.ltidx = 1, .nmaster = 1};
    default_layouts[1][7] = {.ltidx = 1, .nmaster = 1};
    default_layouts[2][7] = {.ltidx = 1, .nmaster = 1};
    //  Workspaces 1 & 2 on monitor 2 are CENTERED FLOATING MASTER LAYOUT with multiple masters
    default_layouts[2][0] = {.ltidx = 3, .nmaster = 4};
    default_layouts[2][1] = {.ltidx = 3, .nmaster = 7};
    ops::log::debug("![setup_default_layouts]");
  };
  
  /* key definitions */
#define TAGKEYS(KEY, TAG) \
  { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
  { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
  
  /* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
  
  /* commands */
  unsigned int config::dmenumon = 0; /* component of dmenucmd, manipulated in spawn() */
  std::unordered_map<std::string, std::vector<const char*>> config::cmds = {
    { "dmenucmd", { "dmenu_run", "-m", "0", "-fn", config::dmenufont.c_str(), "-nb", config::selbgcolor.c_str(), "-nf", config::normfgcolor.c_str(), "-sb", config::normfgcolor.c_str(), "-sf", config::selbgcolor.c_str(), nullptr }},
    { "termcmd", { "st", nullptr }},
    { "scratchterm", { "st", "-c", "scratch", "-t", "ScraTch", "-g", "110x27", nullptr }},
    { "webbrowsercmd", { "firefox", nullptr }},
    { "calc", { "calc", nullptr }},
    { "music", { "music", nullptr }},
    { "music_search", { "music-search", nullptr }},
    { "ytdl", { "yt-dl", nullptr }},
    { "xtogglemousemon", { "xtogglemousemon", nullptr }},
    { "bluemenu", { "st", "-c", "scratch", "-t", "ScraTch", "-g", "30x8-0+27", "-e", "bluemenu", nullptr }},
    { "mail", { "thunderbird", nullptr }},
    { "cecon", { "cecon-plot", nullptr }},
    { "mc_launch", { "mc-launch", nullptr }},
    { "screenshot", { "screenshot", nullptr }},
    { "gitkraken", { "gitkraken", nullptr }},
    { "fa4_iconlist", { "fa4_iconlist", nullptr }},
    { "quick_search", { "quick_search", nullptr }},
    { "upBrightness", { "brightup", nullptr }},
    { "downBrightness", { "brightdown", nullptr }},
    { "casysTog", { "casys", "toggle", nullptr }},
    { "casysUp", { "casys", "up", nullptr }},
    { "casysDown", { "casys", "down", nullptr }},
    { "exitmenu", { "exitmenu", nullptr }},
    { "dispmenu", { "cmonmgr", nullptr }},
    { "startmenu", { "startmenu", nullptr }},
  };
  
  KeyTrigger config::key_nav::trigger           = { MODKEY, XK_d };
  long      config::key_nav::accepting_timespan = -1;
  
  using namespace ops::control;
  std::vector<KeySpec> config::keys = {
    /* modifier                     key        function        argument */
    { 0, XK_Super_L, spawn, {.v = "startmenu"}},
    { MODKEY, XK_d, spawn, {.v = "dmenucmd"}},
    { MODKEY, XK_q, spawn, {.v = "termcmd"}},
    { MODKEY, XK_s, spawn, {.v = "scratchterm"}},
    { MODKEY, XK_w, spawn, {.v = "webbrowsercmd"}},
    { MODKEY, XK_c, spawn, {.v = "calc"}},
    { MODKEY, XK_n, spawn, {.v = "music"}},
    { MODKEY | ShiftMask, XK_n, spawn, {.v = "music_search"}},
    { MODKEY, XK_y, spawn, {.v = "ytdl"}},
    { MODKEY, XK_b, spawn, {.v = "bluemenu"}},
    { MODKEY, XK_m, spawn, {.v = "mail"}},
    { MODKEY, XK_a, spawn, {.v = "xtogglemousemon"}},
    { MODKEY, XK_v, spawn, {.v = "cecon"}},
    { MODKEY, XK_g, spawn, {.v = "gitkraken"}},
    { MODKEY | ShiftMask, XK_m, spawn, {.v = "mc_launch"}},
    { MODKEY | ShiftMask, XK_i, spawn, {.v = "fa4_iconlist"}},
    { MODKEY | ShiftMask, XK_l, spawn, {.v = "quick_search"}},
    { 0, XK_Print, spawn, {.v = "screenshot"}},
    { 0, XF86XK_MonBrightnessUp, spawn, {.v = "upBrightness"}},
    { 0, XF86XK_MonBrightnessDown, spawn, {.v = "downBrightness"}},
    { 0, XF86XK_AudioRaiseVolume, spawn, {.v = "casysUp"}},
    { 0, XF86XK_AudioLowerVolume, spawn, {.v = "casysDown"}},
    { 0, XF86XK_AudioMute, spawn, {.v = "casysTog"}},
    { MODKEY | ShiftMask, XK_b, togglebar, {0}},
    { MODKEY, XK_j, focusstack, {.i = +1}},
    { MODKEY, XK_k, focusstack, {.i = -1}},
    { MODKEY, XK_i, incnmaster, {.i = +1}},
    { MODKEY, XK_o, incnmaster, {.i = -1}},
    { MODKEY, XK_h, setmfact, {.f = -0.05}},
    { MODKEY, XK_l, setmfact, {.f = +0.05}},
    { MODKEY, XK_Return, zoom, {0}},
    { MODKEY, XK_Tab, view, {0}},
    { MODKEY | ShiftMask, XK_q, killclient, {0}},
    { MODKEY, XK_t, setlayout, {.v = &config::layouts[0]}},
    { MODKEY, XK_space, setlayout, {.v = &config::layouts[1]}},
    { MODKEY, XK_f, setlayout, {.v = &config::layouts[2]}},
    { MODKEY, XK_r, setlayout, {.v = &config::layouts[3]}},
    { MODKEY, XK_e, setlayout, {.v = &config::layouts[4]}},
    //	{ MODKEY,                       XK_space,  setlayout,      {0} },
    { MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    { MODKEY, XK_0, view, {.ui = (unsigned int)~0}},
    { MODKEY | ShiftMask, XK_0, tag, {.ui = (unsigned int)~0}},
    { MODKEY, XK_comma, focusmon, {.i = -1}},
    { MODKEY, XK_period, focusmon, {.i = +1}},
    { MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    { MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    TAGKEYS(XK_1, 0)
    TAGKEYS(XK_2, 1)
    TAGKEYS(XK_3, 2)
    TAGKEYS(XK_4, 3)
    TAGKEYS(XK_5, 4)
    TAGKEYS(XK_6, 5)
    TAGKEYS(XK_7, 6)
    TAGKEYS(XK_8, 7)
    TAGKEYS(XK_9, 8)
    //	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },
    { MODKEY | ShiftMask, XK_e, spawn, {.v = "exitmenu"}},
    { MODKEY | ShiftMask, XK_d, spawn, {.v = "dispmenu"}},
  };
  
  /* button definitions */
  /* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
  std::vector<MouseButton> config::buttons = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol, 0, Button1, setlayout, {0}},
    { ClkLtSymbol, 0, Button3, setlayout, {.v = &config::layouts[2]}},
    { ClkWinTitle, 0, Button2, zoom, {0}},
    { ClkStatusText, 0, Button2, spawn, {.v = "termcmd"}},
    { ClkClientWin, MODKEY, Button1, movemouse, {0}},
    { ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    { ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    { ClkTagBar, 0, Button1, view, {0}},
    { ClkTagBar, 0, Button3, toggleview, {0}},
    { ClkTagBar, MODKEY, Button1, tag, {0}},
    { ClkTagBar, MODKEY, Button3, toggletag, {0}},
  };
  
  /*
   * Xresources preferences to load at startup
   */
  std::vector<ResourcePref> config::resources = {
    { "normbgcolor", STRING, &config::normbgcolor },
    { "normbordercolor", STRING, &config::normbordercolor },
    { "normfgcolor", STRING, &config::normfgcolor },
    { "selbgcolor", STRING, &config::selbgcolor },
    { "selbordercolor", STRING, &config::selbordercolor },
    { "selfgcolor", STRING, &config::selfgcolor },
    { "borderpx", INTEGER, &config::borderpx },
    { "snap", INTEGER, &config::snap },
    { "showbar", INTEGER, &config::showbar },
    { "topbar", INTEGER, &config::topbar },
    { "nmaster", INTEGER, &config::nmaster },
    { "resizehints", INTEGER, &config::resizehints },
    { "mfact", FLOAT, &config::mfact },
  };
  
}

#endif // CDWM_CONFIG

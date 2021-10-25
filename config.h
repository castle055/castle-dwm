#include <X11/XF86keysym.h>
#include <cstdlib>
/* See LICENSE file for copyright and license details. */

static unsigned short ctl_port= 33850;

/* appearance */
static unsigned int borderpx  = 0;        /* border pixel of windows */
static unsigned int snap      = 30;       /* snap pixel */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
//static const char *fonts[]          = { "monospace:size=12" };
static const char *fonts[]	    = { "Iosevka Term:style=Bold:size=9:antialias=true:autohint:true" };
//static const char *fonts[]	    = { "Fira Code:style=Bold:size=9:antialias=true:autohint:true" };
static const char dmenufont[]       = "Iosevka Term:size=14";
static char normbgcolor[]     = "#111326";
static char selbgcolor[]      = "#06060a";
static char normbordercolor[] = "#fcae1e";
static char selbordercolor[]  = "#fcae1e";
static char normfgcolor[]     = "#fcae1e";
static char selfgcolor[]      = "#fcae1e";
//static char col_red[]	      = "#cc0000";
//static char col_red2[]	      = "#150000";
//static char col_red3[]	      = "#0a0000";
static char *colors[][3]      = {
	/*               fg         bg         border   */
	{ normfgcolor,   normbgcolor, normbordercolor},
	{ selfgcolor,   selbgcolor,    selbordercolor}
};

/* Gapps */
static const unsigned int gappx = 20;

/* tagging */
static const char *TAGS[] = {"\uf269", "\uf121", "3", "4", "5", "6", "7", "\uf008", "\uf274" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       TAGS mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1,        0 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       1,           -1,        0 },
	{ "st",       NULL,       NULL,       1 << 1,       0,           -1,        0 },
	{ "Thunderbird",NULL,     NULL,       1 << 8,       0,           -1,        0 },
	{ "jetbrains-idea",NULL,  NULL,       1 << 4,       0,           -1,        1 },
	{ NULL,       "zathura",  NULL,       1 << 2,       0,           -1,        0 },
	{ "scratch",  NULL,       NULL,       0,            1,           -1,        0 },
//	{ "Main",     NULL,       NULL,       0,            1,           -1,        0 },
	{ NULL,       NULL,       "Teensy",   0,            1,           -1,        0 }
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", selbgcolor, "-nf", normfgcolor, "-sb", normfgcolor, "-sf", selbgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *scratchterm[]  = { "st", "-c", "scratch", "-t", "ScraTch", "-g", "110x27", NULL };
static const char *webbrowsercmd[] = { "firefox", NULL };
static const char *calc[] = { "calc", NULL };
static const char *music[] = { "music", NULL };
static const char *music_search[] = { "music-search", NULL };
static const char *ytdl[] = { "yt-dl", NULL };
static const char *xtogglemousemon[] = { "xtogglemousemon", NULL };
static const char *bluemenu[] = { "st", "-c", "scratch", "-t", "ScraTch", "-g", "30x8-0+27", "-e", "bluemenu", NULL };
static const char *mail[] = { "thunderbird", NULL };
static const char *cecon[] = { "cecon-plot", NULL };
static const char *mc_launch[] = { "mc-launch", NULL };
static const char *screenshot[] = { "screenshot", NULL };
static const char *gitkraken[] = { "gitkraken", NULL };
static const char *fa4_iconlist[] = { "fa4_iconlist", NULL };
static const char *quick_search[] = { "quick_search", NULL };

// Brightness control
static const char *upBrightness[] = {"brightup", NULL};
static const char *downBrightness[] = {"brightdown", NULL};

// CASYS control
static const char *casysTog[] = {"casys", "toggle", NULL};
static const char *casysUp[] = {"casys", "up", NULL};
static const char *casysDown[] = {"casys", "down", NULL};

// MENUS
static const char *exitmenu[] = {"exitmenu", NULL};
static const char *dispmenu[] = {"cmonmgr", NULL};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,             		XK_q,	   spawn,          {.v = termcmd } },
	{ MODKEY,             		XK_s,	   spawn,          {.v = scratchterm } },
	{ MODKEY,			XK_w,	   spawn,	   {.v = webbrowsercmd } },
	{ MODKEY,			XK_c,	   spawn,	   {.v = calc } },
	{ MODKEY,			XK_n,	   spawn,	   {.v = music } },
	{ MODKEY|ShiftMask,		XK_n,	   spawn,	   {.v = music_search } },
	{ MODKEY,			XK_y,	   spawn,	   {.v = ytdl } },
	{ MODKEY,			XK_b,	   spawn,	   {.v = bluemenu } },
	{ MODKEY,			XK_m,	   spawn,	   {.v = mail } },
	{ MODKEY,			XK_a,	   spawn,	   {.v = xtogglemousemon } },
	{ MODKEY,			XK_v,	   spawn,	   {.v = cecon } },
	{ MODKEY,			XK_g,	   spawn,	   {.v = gitkraken } },
	{ MODKEY|ShiftMask,		XK_m,	   spawn,	   {.v = mc_launch } },
	{ MODKEY|ShiftMask,		XK_i,	   spawn,	   {.v = fa4_iconlist } },
	{ MODKEY|ShiftMask,		XK_l,	   spawn,	   {.v = quick_search } },
	{ 0,				XK_Print,  spawn,	   {.v = screenshot } },
	{ 0,			XF86XK_MonBrightnessUp,	spawn,	   {.v = upBrightness}},
	{ 0,			XF86XK_MonBrightnessDown, spawn,   {.v = downBrightness}},
	{ 0, 			XF86XK_AudioRaiseVolume, spawn,    {.v = casysUp}},
	{ 0, 			XF86XK_AudioLowerVolume, spawn,    {.v = casysDown}},
	{ 0,			XF86XK_AudioMute, spawn,	   {.v = casysTog}},
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_space,  setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_e,      setlayout,      {.v = &layouts[4]} },
//	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = (unsigned int)~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = (unsigned int)~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
//	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_e,      spawn,           {.v = exitmenu } },
	{ MODKEY|ShiftMask,             XK_d,      spawn,           {.v = dispmenu } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

//static const char *dwmfifo = std::strcat(std::getenv("HOME"), "/.local/utils/dwm.fifo");
static const char *dwmfifo = "/home/castle/.local/utils/dwm.fifo";
static Command commands[] = {
	{ "loadxrdb",        (void(*)(const Arg*))load_xresources,{0} },
	{ "dmenu",           spawn,          {.v = dmenucmd} },
	{ "term",            spawn,          {.v = termcmd} },
	{ "quit",            quit,           {0} },
	{ "togglebar",       togglebar,      {0} },
	{ "focusstack+",     focusstack,     {.i = +1} },
	{ "focusstack-",     focusstack,     {.i = -1} },
	{ "incnmaster+",     incnmaster,     {.i = +1} },
	{ "incnmaster-",     incnmaster,     {.i = -1} },
	{ "setmfact+",       setmfact,       {.f = +0.05} },
	{ "setmfact-",       setmfact,       {.f = -0.05} },
	{ "zoom",            zoom,           {0} },
	{ "view",            view,           {0} },
	{ "killclient",      killclient,     {0} },
	{ "setlayout-tiled", setlayout,      {.v = &layouts[0]} },
	{ "setlayout-float", setlayout,      {.v = &layouts[1]} },
	{ "setlayout-mono",  setlayout,      {.v = &layouts[2]} },
	{ "togglelayout",    setlayout,      {0} },
	{ "togglefloating",  togglefloating, {0} },
	{ "viewall",         view,           {.ui = (unsigned int)~0} },
	{ "tag",             tag,            {.ui = (unsigned int)~0} },
	{ "focusmon+",       focusmon,       {.i = +1} },
	{ "focusmon-",       focusmon,       {.i = -1} },
	{ "tagmon+",         tagmon,         {.i = +1} },
	{ "tagmon-",         tagmon,         {.i = -1} },
	{ "view1",           view,           {.ui = 1 << 0} },
	{ "view2",           view,           {.ui = 1 << 1} },
	{ "view3",           view,           {.ui = 1 << 2} },
	{ "view4",           view,           {.ui = 1 << 3} },
	{ "view5",           view,           {.ui = 1 << 4} },
	{ "view6",           view,           {.ui = 1 << 5} },
	{ "view7",           view,           {.ui = 1 << 6} },
	{ "view8",           view,           {.ui = 1 << 7} },
	{ "view9",           view,           {.ui = 1 << 8} },
	{ "toggleview1",     toggleview,     {.ui = 1 << 0} },
	{ "toggleview2",     toggleview,     {.ui = 1 << 1} },
	{ "toggleview3",     toggleview,     {.ui = 1 << 2} },
	{ "toggleview4",     toggleview,     {.ui = 1 << 3} },
	{ "toggleview5",     toggleview,     {.ui = 1 << 4} },
	{ "toggleview6",     toggleview,     {.ui = 1 << 5} },
	{ "toggleview7",     toggleview,     {.ui = 1 << 6} },
	{ "toggleview8",     toggleview,     {.ui = 1 << 7} },
	{ "toggleview9",     toggleview,     {.ui = 1 << 8} },
	{ "tag1",            tag,            {.ui = 1 << 0} },
	{ "tag2",            tag,            {.ui = 1 << 1} },
	{ "tag3",            tag,            {.ui = 1 << 2} },
	{ "tag4",            tag,            {.ui = 1 << 3} },
	{ "tag5",            tag,            {.ui = 1 << 4} },
	{ "tag6",            tag,            {.ui = 1 << 5} },
	{ "tag7",            tag,            {.ui = 1 << 6} },
	{ "tag8",            tag,            {.ui = 1 << 7} },
	{ "tag9",            tag,            {.ui = 1 << 8} },
	{ "toggletag1",      toggletag,      {.ui = 1 << 0} },
	{ "toggletag2",      toggletag,      {.ui = 1 << 1} },
	{ "toggletag3",      toggletag,      {.ui = 1 << 2} },
	{ "toggletag4",      toggletag,      {.ui = 1 << 3} },
	{ "toggletag5",      toggletag,      {.ui = 1 << 4} },
	{ "toggletag6",      toggletag,      {.ui = 1 << 5} },
	{ "toggletag7",      toggletag,      {.ui = 1 << 6} },
	{ "toggletag8",      toggletag,      {.ui = 1 << 7} },
	{ "toggletag9",      toggletag,      {.ui = 1 << 8} },
};

/*
 * Xresources preferences to load at startup
 */
static ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",      		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",     	 	FLOAT,   &mfact },
};

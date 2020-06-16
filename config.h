#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 30;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
//static const char *fonts[]          = { "monospace:size=12" };
static const char *fonts[]	    = { "Iosevka Term:style=Bold:size=12:antialias=true:autohint:true" };
static const char dmenufont[]       = "Iosevka Term:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_prim[]	    = "#06060a";
static const char col_prim2[]	    = "#111326";
static const char col_sec[]	    = "#fcae1e";
static const char col_red[]	    = "#cc0000";
static const char col_red2[]	    = "#150000";
static const char col_red3[]	    = "#0a0000";
static char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_sec,   col_prim2, col_sec },
	[SchemeSel]  = { col_sec,   col_prim,  col_sec  },
};
static int isRed = 0;
void
switchToRed(){
	colors[SchemeNorm][0] = col_red;
	colors[SchemeNorm][1] = col_red2;
	colors[SchemeNorm][2] = col_red;
	colors[SchemeSel][0]  = col_red;
	colors[SchemeSel][1]  = col_red3;
	colors[SchemeSel][2]  = col_red;
	settheme();
}
void
switchToNorm(){
	colors[SchemeNorm][0] = col_sec;
	colors[SchemeNorm][1] = col_prim2;
	colors[SchemeNorm][2] = col_sec;
	colors[SchemeSel][0]  = col_sec;
	colors[SchemeSel][1]  = col_prim;
	colors[SchemeSel][2]  = col_sec;
	settheme();
}

/* Gapps */
static const unsigned int gappx = 20;

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "PROD" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_prim, "-nf", col_sec, "-sb", col_sec, "-sf", col_prim, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *webbrowsercmd[] = { "firefox", NULL };
static const char *calc[] = { "calc", NULL };
static const char *music[] = { "music", NULL };

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
	{ MODKEY,			XK_w,	   spawn,	   {.v = webbrowsercmd } },
	{ MODKEY,			XK_c,	   spawn,	   {.v = calc } },
	{ MODKEY,			XK_n,	   spawn,	   {.v = music } },
	{ 0,			XF86XK_MonBrightnessUp,	spawn,	   {.v = upBrightness}},
	{ 0,			XF86XK_MonBrightnessDown, spawn,   {.v = downBrightness}},
	{ 0, 			XF86XK_AudioRaiseVolume, spawn,    {.v = casysUp}},
	{ 0, 			XF86XK_AudioLowerVolume, spawn,    {.v = casysDown}},
	{ 0,			XF86XK_AudioMute, spawn,	   {.v = casysTog}},
	{ MODKEY,                       XK_b,      togglebar,      {0} },
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
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
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

static const char *dwmfifo = "/home/victor/.local/utils/dwm.fifo";
static Command commands[] = {
	{ "NORMtheme",       switchToNorm,   {0} },
	{ "REDtheme",        switchToRed,    {0} },
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
	{ "viewall",         view,           {.ui = ~0} },
	{ "tag",             tag,            {.ui = ~0} },
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

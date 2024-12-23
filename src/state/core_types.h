//
// Created by castle on 2/1/24.
//

#ifndef CYD_WM_CORE_TYPES_H
#define CYD_WM_CORE_TYPES_H


#include <X11/X.h>
#include <vector>
#include <string>
#include <array>
#include <unordered_map>
//#include "../ui/status_bar/workspaces.hpp"
//#include "../ui/status_bar/window_title.hpp"

/* enums */
enum {
  CurNormal, CurResize, CurMove, CurLast
}; /* cursor */
enum {
  SchemeNorm, SchemeSel
}; /* color schemes */
enum {
  NetSupported, NetWMName, NetWMState, NetWMCheck,
  NetWMFullscreen, NetActiveWindow, NetWMWindowType,
  NetWMWindowTypeDialog, NetClientList, NetLast
}; /* EWMH atoms */
enum {
  WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast
}; /* default atoms */
enum {
  ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
  ClkClientWin, ClkRootWin, ClkLast
}; /* clicks */
enum resource_type {
  STRING = 0,
  INTEGER = 1,
  FLOAT = 2
}; /* Xresources preferences */

typedef union {
  int i;
  unsigned int ui;
  float f;
  const void *v;
} Arg;
typedef struct {
  unsigned int click;
  unsigned int mask;
  unsigned int button;
  
  void (*func)(const Arg *arg);
  
  const Arg arg;
} MouseButton;
typedef struct {
  unsigned int mod;
  KeySym keysym;
} KeyTrigger;
typedef struct {
  unsigned int mod;
  KeySym keysym;
  
  void (*func)(const Arg *);
  
  const Arg arg;
} KeySpec;
typedef struct {
  const char *class_;
  const char *instance;
  const char *title;
  unsigned int tags;
  int isfloating;
  int monitor;
  bool ignoretransient;
} Rule;
struct DefaultLayout {
  int ltidx = -1;
  int nmaster = 1;
};
typedef struct {
  const char *name;
  
  void (*func)(const Arg *arg);
  
  const Arg arg;
} Command;
typedef struct {
  char *name;
  enum resource_type type;
  void *dst;
} ResourcePref;

/* macros */
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)         (mask & ~(state::numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x, y, w, h, m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                                    * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
#define TAGMASK                 ((1 << state::config::TAGS.size()) - 1)
#define ISVISIBLE(C)            ((C->tags & C->mon->tagset[C->mon->seltags]))
#define LENGTH(X)               (sizeof X / sizeof X[0])
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)
#define TEXTW(X)                (drw_fontset_getwidth(state::drw, (X.c_str())) + state::lrpad)
#define TEXTW_CSTR(X)           (drw_fontset_getwidth(state::drw, (X)) + state::lrpad)

struct monitor_t;

typedef struct {
  const char symbol[16];
  
  void (*arrange_)(monitor_t *);
} Layout;

struct Pertag {
  unsigned int curtag, prevtag; /* current and previous tag */
  std::vector<int> nmasters; /* number of windows in master area */
  std::vector<float> mfacts; /* mfacts per tag */
  std::vector<unsigned int> sellts; /* selected layouts */
  std::vector<std::array<const Layout*, 2>> ltidxs; /* matrix of TAGS and layouts indexes  */
  std::vector<int> showbars; /* display bar for the current tag */
};

struct client_t {
  std::string name = "";
  float mina = 0.0f, maxa = 0.0f;
  int x = 0, y = 0, w = 0, h = 0;
  int oldx = 0, oldy = 0, oldw = 0, oldh = 0;
  int basew = 0, baseh = 0, incw = 0, inch = 0, maxw = 0, maxh = 0, minw = 0, minh = 0;
  int bw = 0, oldbw = 0;
  unsigned int tags = 0;
  int isfixed = 0, isfloating = 0, isurgent = 0, neverfocus = 0, oldstate = 0, isfullscreen = 0;
  bool ignoretransient = false;
  client_t *next = nullptr;
  client_t *snext = nullptr;
  monitor_t *mon = nullptr;
  Window win;
};

#endif //CYD_WM_CORE_TYPES_H

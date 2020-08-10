#include <X11/Xatom.h>
#include <X11/Xresource.h>

#include "drw.h"

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

/* classes */
class Monitor;
class Client;

/* macros */
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)         (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x, y, w, h, m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                                    * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define ISVISIBLE(C)            ((C->tags & C->mon->tagset[C->mon->seltags]))
#define LENGTH(X)               (sizeof X / sizeof X[0])
#define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)
#define TAGMASK                 ((1 << LENGTH(TAGS)) - 1)
#define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)

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
} Button;
typedef struct {
    unsigned int mod;
    KeySym keysym;

    void (*func)(const Arg *);

    const Arg arg;
} Key;
typedef struct {
    const char *symbol;

    void (*arrange_)(Monitor *);
} Layout;
typedef struct {
    const char *class_;
    const char *instance;
    const char *title;
    unsigned int tags;
    int isfloating;
    int monitor;
} Rule;
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

typedef struct Pertag Pertag;

class Client {
public:
    char name[256];
    float mina, maxa;
    int x, y, w, h;
    int oldx, oldy, oldw, oldh;
    int basew, baseh, incw, inch, maxw, maxh, minw, minh;
    int bw, oldbw;
    unsigned int tags;
    int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen;
    Client *next;
    Client *snext;
    Monitor *mon;
    Window win;

    void applyrules();
    int applysizehints(int *X, int *Y, int *W, int *H, int interact);
    void attach();
    void attachstack();
    void configure_();
    void detach();
    void detachstack();
    void focus();
    Atom getatomprop(Atom prop);
    void grabbuttons(int focused);
    void pop();
    void resize(int x, int y, int w, int h, int interact);
    void resizeclient(int x, int y, int w, int h);
    void setfullscreen(int fullscreen);
    void seturgent(int urg);
    void showhide();
    void unfocus(int setfocus);
    void unmanage(int destroyed);
    void updatesizehints();
    void updatetitle();
    void updatewindowtype();
    void updatewmhints();
    void setfocus();
    Client* nexttiled();
};
class Monitor {
public:
    char ltsymbol[16];
    float mfact;
    int nmaster;
    int num;
    int by;               /* bar geometry */
    int mx, my, mw, mh;   /* screen size */
    int wx, wy, ww, wh;   /* window area  */
    int gappx;            /* gaps between windows */
    unsigned int seltags;
    unsigned int sellt;
    unsigned int tagset[2];
    int showbar;
    int topbar;
    Client *clients;
    Client *sel;
    Client *stack;
    Monitor *next;
    Window barwin;
    const Layout *lt[2];
    Pertag *pertag;

    void arrangemon();
    int drawstatusbar(int bh, char *stext);
    void drawbar();
    void monocle();
    void restack();
    void tile();
    void updatebarpos();
    void centeredmaster();
    void centeredfloatingmaster();
};

/* function declarations */
extern void arrange(Monitor *m);
extern void buttonpress(XEvent *e);
extern void checkotherwm();
extern void cleanup();
extern void cleanupmon(Monitor *mon);
extern void clientmessage(XEvent *e);
extern void configurenotify(XEvent *e);
extern void configurerequest(XEvent *e);
extern Monitor *createmon();
extern void destroynotify(XEvent *e);
extern Monitor *dirtomon(int dir);
extern void dispatchcmd();
extern void drawbars();
extern void enternotify(XEvent *e);
extern Bool evpredicate();
extern void expose(XEvent *e);
extern void focusin(XEvent *e);
extern void focusmon(const Arg *arg);
extern void focusstack(const Arg *arg);
extern int getrootptr(int *x, int *y);
extern long getstate(Window w);
extern int gettextprop(Window w, Atom atom, char *text, unsigned int size);
extern void grabkeys();
extern void incnmaster(const Arg *arg);
extern void keypress(XEvent *e);
extern void killclient(const Arg *arg);
extern void manage(Window w, XWindowAttributes *wa, int urgent);
extern void mappingnotify(XEvent *e);
extern void maprequest(XEvent *e);
extern void monocle(Monitor *m);
extern void motionnotify(XEvent *e);
extern void movemouse(const Arg *arg);
extern void propertynotify(XEvent *e);
extern void quit(const Arg *arg);
extern Monitor *recttomon(int x, int y, int w, int h);
extern void resizemouse(const Arg *arg);
extern void run();
extern void scan();
extern int sendevent(Client *c, Atom proto);
extern void sendmon(Client *c, Monitor *m);
extern void setclientstate(Client *c, long state);
extern void focusNULL();
extern void setlayout(const Arg *arg);
extern void setmfact(const Arg *arg);
extern void settheme();    // This is mine for theme on the fly
extern void setup();
extern void sigchld(int unused);
extern void spawn(const Arg *arg);
extern void tag(const Arg *arg);
extern void tagmon(const Arg *arg);
extern void tile(Monitor *);
extern void togglebar(const Arg *arg);
extern void togglefloating(const Arg *arg);
extern void toggletag(const Arg *arg);
extern void toggleview(const Arg *arg);
extern void unmapnotify(XEvent *e);
extern void updatebars(void);
extern void updateclientlist(void);
extern int updategeom(void);
extern void updatenumlockmask(void);
extern void updatestatus(void);
extern void view(const Arg *arg);
extern Client *wintoclient(Window w);
extern Monitor *wintomon(Window w);
extern int xerror(Display *dpy, XErrorEvent *ee);
extern int xerrordummy(Display *dpy, XErrorEvent *ee);
extern int xerrorstart(Display *dpy, XErrorEvent *ee);
extern void zoom(const Arg *arg);
extern void centeredmaster(Monitor *m);
extern void centeredfloatingmaster(Monitor *m);
extern void load_xresources();
extern void resource_load(XrmDatabase db, char *name, enum resource_type rtype, void *dst);

#include "config.h"

struct Pertag {
    unsigned int curtag, prevtag; /* current and previous tag */
    int nmasters[LENGTH(TAGS) + 1]; /* number of windows in master area */
    float mfacts[LENGTH(TAGS) + 1]; /* mfacts per tag */
    unsigned int sellts[LENGTH(TAGS) + 1]; /* selected layouts */
    const Layout *ltidxs[LENGTH(TAGS) + 1][2]; /* matrix of TAGS and layouts indexes  */
    int showbars[LENGTH(TAGS) + 1]; /* display bar for the current tag */
};

/* variables */
static const char broken[] = "broken";

extern char stext[8096];

extern Atom wmatom[WMLast], netatom[NetLast];
extern Display* dpy;
extern Drw *drw;
extern Monitor *mons, *selmon;
extern Window root, wmcheckwin;
extern Clr **scheme;
extern int sw, sh, bh, blw;
extern int lrpad;
extern unsigned int numlockmask;
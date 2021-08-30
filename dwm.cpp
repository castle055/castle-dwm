/* See LICENSE file for copyright and license details.
 *
 * dynamic window manager is designed like any other X client as well. It is
 * driven through handling X events. In contrast to other X clients, a window
 * manager selects for SubstructureRedirectMask on the root window, to receive
 * events about window (dis-)appearance. Only one X connection at a time is
 * allowed to select for this event mask.
 *
 * The event handlers of dwm are organized in an array which is accessed
 * whenever a new event has been fetched. This allows event dispatching
 * in O(1) time.
 *
 * Each child of the root window is called a client, except windows which have
 * set the override_redirect flag. Clients are organized in a linked client
 * list on each monitor, the focus history is remembered through a stack list
 * on each monitor. Each client contains a bit array to indicate the TAGS of a
 * client.
 *
 * Keys and tagging rules are organized as arrays and defined in config.h.
 *
 * To understand everything else, start reading main().
 */

#include <fcntl.h>
#include <clocale>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <program_shell.h>

#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */

#include "castle-dwm.h"
#include "util.h"

/* variables */
char stext[8096];
static int screen;
int sw, sh;                  /* X display screen geometry width, height */
int bh, blw = 0;             /* bar geometry */
int lrpad;                   /* sum of left and right padding for text */

static int (*xerrorxlib)(Display *, XErrorEvent *);

unsigned int numlockmask = 0;

Display *dpy;
Atom wmatom[WMLast], netatom[NetLast];

static int running = 1;
static Cur *cursor[CurLast];
Clr **scheme;
Drw *drw;
Monitor *mons, *selmon;
Window root, wmcheckwin;
static int fifofd;

/*static void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = buttonpress,
	[ClientMessage] = clientmessage,
	[ConfigureRequest] = configurerequest,
	[ConfigureNotify] = configurenotify,
	[DestroyNotify] = destroynotify,
	[EnterNotify] = enternotify,
	[Expose] = expose,
	[FocusIn] = focusin,
	[KeyPress] = keypress,
	[MappingNotify] = mappingnotify,
	[MapRequest] = maprequest,
	[MotionNotify] = motionnotify,
	[PropertyNotify] = propertynotify,
	[UnmapNotify] = unmapnotify
};*/
static void (*handler[LASTEvent])(XEvent *) = {
            nullptr,nullptr,
        keypress,
            nullptr,
        buttonpress,
            nullptr,
        motionnotify,
        enternotify,
            nullptr,
        focusin,
            nullptr,nullptr,
        expose,
            nullptr,nullptr,nullptr,nullptr,
        destroynotify,
        unmapnotify,
            nullptr,
        maprequest,
            nullptr,
        configurenotify,
        configurerequest,
            nullptr,nullptr,nullptr,nullptr,
        propertynotify,
            nullptr,nullptr,nullptr,nullptr,
        clientmessage,
        mappingnotify
};

/* compile-time check if all TAGS fit into an unsigned int bit array. */
struct NumTags {
    char limitexceeded[LENGTH(TAGS) > 31 ? -1 : 1];
};

/* function implementations */
//===== Main functions =====
void arrange(Monitor *m) {
    if (m)
        m->stack->showhide();
    else
        for (m = mons; m; m = m->next)
            m->stack->showhide();
    if (m) {
        m->arrangemon();
        m->restack();
    } else
        for (m = mons; m; m = m->next)
            m->arrangemon();
}
void buttonpress(XEvent *e) {
    unsigned int i, x, click;
    Arg arg = {0};
    Client *c;
    Monitor *m;
    XButtonPressedEvent *ev = &e->xbutton;

    click = ClkRootWin;
    /* focus monitor if necessary */
    if ((m = wintomon(ev->window)) && m != selmon) {
        selmon->sel->unfocus(1);
        selmon = m;
        focusNULL();
    }
    if (ev->window == selmon->barwin) {
        i = x = 0;
        do
            x += TEXTW(TAGS[i]);
        while (ev->x >= x && ++i < LENGTH(TAGS));
        if (i < LENGTH(TAGS)) {
            click = ClkTagBar;
            arg.ui = 1 << i;
        } else if (ev->x < x + blw)
            click = ClkLtSymbol;
        else if (ev->x > selmon->ww - TEXTW(stext))
            click = ClkStatusText;
        else
            click = ClkWinTitle;
    } else if ((c = wintoclient(ev->window))) {
        c->focus();
        selmon->restack();
        XAllowEvents(dpy, ReplayPointer, CurrentTime);
        click = ClkClientWin;
    }
    for (i = 0; i < LENGTH(buttons); i++)
        if (click == buttons[i].click && buttons[i].func && buttons[i].button == ev->button
            && CLEANMASK(buttons[i].mask) == CLEANMASK(ev->state))
            buttons[i].func(click == ClkTagBar && buttons[i].arg.i == 0 ? &arg : &buttons[i].arg);
}
void checkotherwm(void) {
    xerrorxlib = XSetErrorHandler(xerrorstart);
    /* this causes an error if some other window manager is running */
    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
    XSync(dpy, False);
    XSetErrorHandler(xerror);
    XSync(dpy, False);
}
void cleanup(void) {
    Arg a = {.ui = (unsigned int) ~0};
    Layout foo = {"", NULL};
    Monitor *m;
    size_t i;

    view(&a);
    selmon->lt[selmon->sellt] = &foo;
    for (m = mons; m; m = m->next)
        while (m->stack)
            m->stack->unmanage(0);
    XUngrabKey(dpy, AnyKey, AnyModifier, root);
    while (mons)
        cleanupmon(mons);
    for (i = 0; i < CurLast; i++)
        drw_cur_free(drw, cursor[i]);
    for (i = 0; i < LENGTH(colors) + 1; i++)
        free(scheme[i]);
    XDestroyWindow(dpy, wmcheckwin);
    drw_free(drw);
    XSync(dpy, False);
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
    close(fifofd);
}
void cleanupmon(Monitor *mon) {
    Monitor *m;

    if (mon == mons)
        mons = mons->next;
    else {
        for (m = mons; m && m->next != mon; m = m->next);
        m->next = mon->next;
    }
    XUnmapWindow(dpy, mon->barwin);
    XDestroyWindow(dpy, mon->barwin);
    free(mon);
}
void clientmessage(XEvent *e) {
    XClientMessageEvent *cme = &e->xclient;
    Client *c = wintoclient(cme->window);

    if (!c)
        return;
    if (cme->message_type == netatom[NetWMState]) {
        if (cme->data.l[1] == netatom[NetWMFullscreen]
            || cme->data.l[2] == netatom[NetWMFullscreen])
            c->setfullscreen((cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
                              || (cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */ && !c->isfullscreen)));
    } else if (cme->message_type == netatom[NetActiveWindow]) {
        if (c != selmon->sel && !c->isurgent)
            c->seturgent(1);
    }
}
void configurenotify(XEvent *e) {
    Monitor *m;
    Client *c;
    XConfigureEvent *ev = &e->xconfigure;
    int dirty;

    /* TODO: updategeom handling sucks, needs to be simplified */
    if (ev->window == root) {
        dirty = (sw != ev->width || sh != ev->height);
        sw = ev->width;
        sh = ev->height;
        if (updategeom() || dirty) {
            drw_resize(drw, sw, bh);
            updatebars();
            for (m = mons; m; m = m->next) {
                for (c = m->clients; c; c = c->next)
                    if (c->isfullscreen)
                        c->resizeclient(m->mx, m->my, m->mw, m->mh);
                XMoveResizeWindow(dpy, m->barwin, m->wx, m->by, m->ww, bh);
            }
            focusNULL();
            arrange(nullptr);
        }
    }
}
void configurerequest(XEvent *e) {
    Client *c;
    Monitor *m;
    XConfigureRequestEvent *ev = &e->xconfigurerequest;
    XWindowChanges wc;

    if ((c = wintoclient(ev->window))) {
        if (ev->value_mask & CWBorderWidth)
            c->bw = ev->border_width;
        else if (c->isfloating || !selmon->lt[selmon->sellt]->arrange_) {
            m = c->mon;
            if (ev->value_mask & CWX) {
                c->oldx = c->x;
                c->x = m->mx + ev->x;
            }
            if (ev->value_mask & CWY) {
                c->oldy = c->y;
                c->y = m->my + ev->y;
            }
            if (ev->value_mask & CWWidth) {
                c->oldw = c->w;
                c->w = ev->width;
            }
            if (ev->value_mask & CWHeight) {
                c->oldh = c->h;
                c->h = ev->height;
            }
            if ((c->x + c->w) > m->mx + m->mw && c->isfloating)
                c->x = m->mx + (m->mw / 2 - WIDTH(c) / 2); /* center in x direction */
            if ((c->y + c->h) > m->my + m->mh && c->isfloating)
                c->y = m->my + (m->mh / 2 - HEIGHT(c) / 2); /* center in y direction */
            if ((ev->value_mask & (CWX | CWY)) && !(ev->value_mask & (CWWidth | CWHeight)))
                c->configure_();
            if (ISVISIBLE(c))
                XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
        } else
            c->configure_();
    } else {
        wc.x = ev->x;
        wc.y = ev->y;
        wc.width = ev->width;
        wc.height = ev->height;
        wc.border_width = ev->border_width;
        wc.sibling = ev->above;
        wc.stack_mode = ev->detail;
        XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
    }
    XSync(dpy, False);
}
Monitor *createmon() {
    Monitor *m;
    unsigned int i;

    m = (Monitor *) ecalloc(1, sizeof(Monitor));
    m->tagset[0] = m->tagset[1] = 1;
    m->mfact = mfact;
    m->nmaster = nmaster;
    m->showbar = showbar;
    m->topbar = topbar;
    m->gappx = gappx;
    m->lt[0] = &layouts[0];
    m->lt[1] = &layouts[1 % LENGTH(layouts)];
    strncpy(m->ltsymbol, layouts[0].symbol, sizeof m->ltsymbol);
    m->pertag = (Pertag *) ecalloc(1, sizeof(Pertag));
    m->pertag->curtag = m->pertag->prevtag = 1;

    for (i = 0; i <= LENGTH(TAGS); i++) {
        m->pertag->nmasters[i] = m->nmaster;
        m->pertag->mfacts[i] = m->mfact;

        m->pertag->ltidxs[i][0] = m->lt[0];
        m->pertag->ltidxs[i][1] = m->lt[1];
        m->pertag->sellts[i] = m->sellt;

        m->pertag->showbars[i] = m->showbar;
    }

    return m;
}
void destroynotify(XEvent *e) {
    Client *c;
    XDestroyWindowEvent *ev = &e->xdestroywindow;

    if ((c = wintoclient(ev->window)))
        c->unmanage(1);
}
Monitor* dirtomon(int dir) {
    Monitor *m = NULL;

    if (dir > 0) {
        if (!(m = selmon->next))
            m = mons;
    } else if (selmon == mons)
        for (m = mons; m->next; m = m->next);
    else
        for (m = mons; m->next != selmon; m = m->next);
    return m;
}
void dispatchcmd(void) {
    int i;
    char buf[BUFSIZ];
    ssize_t n;

    n = read(fifofd, buf, sizeof(buf) - 1);
    if (n == -1)
        die("Failed to read() from DWM fifo %s:", dwmfifo);
    buf[n] = '\0';
    buf[strcspn(buf, "\n")] = '\0';
    for (i = 0; i < LENGTH(commands); i++) {
        if (strcmp(commands[i].name, buf) == 0) {
            commands[i].func(&commands[i].arg);
            break;
        }
    }
}
void drawbars(void) {
    Monitor *m;

    for (m = mons; m; m = m->next)
        m->drawbar();
}
void enternotify(XEvent *e) {
    Client *c;
    Monitor *m;
    XCrossingEvent *ev = &e->xcrossing;

    if ((ev->mode != NotifyNormal || ev->detail == NotifyInferior) && ev->window != root)
        return;
    c = wintoclient(ev->window);
    m = c ? c->mon : wintomon(ev->window);
    if (m != selmon) {
        selmon->sel->unfocus(1);
        selmon = m;
    } else if (!c || c == selmon->sel)
        return;
    c->focus();
}
Bool evpredicate() {
    return True;
}
void expose(XEvent *e) {
    Monitor *m;
    XExposeEvent *ev = &e->xexpose;

    if (ev->count == 0 && (m = wintomon(ev->window)))
        m->drawbar();
}
void focusNULL() {
    Client *c;

    for (c = selmon->stack; c && !ISVISIBLE(c); c = c->snext);
    if (selmon->sel && selmon->sel != c)
        selmon->sel->unfocus(0);
    if (c) {
        if (c->mon != selmon)
            selmon = c->mon;
        if (c->isurgent)
            c->seturgent(0);
        c->detachstack();
        c->attachstack();
        c->grabbuttons(1);
        XSetWindowBorder(dpy, c->win, scheme[SchemeSel][ColBorder].pixel);
        c->setfocus();
    } else {
        XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
        XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
    }
    selmon->sel = c;
    drawbars();
}
/* there are some broken focus acquiring clients needing extra handling */
void focusin(XEvent *e) {
    XFocusChangeEvent *ev = &e->xfocus;

    if (selmon->sel && ev->window != selmon->sel->win)
        selmon->sel->setfocus();
}
void focusmon(const Arg *arg) {
    Monitor *m;

    if (!mons->next)
        return;
    if ((m = dirtomon(arg->i)) == selmon)
        return;
    selmon->sel->unfocus(0);
    selmon = m;
    focusNULL();
}
void focusstack(const Arg *arg) {
    Client *c = NULL, *i;

    if (!selmon->sel)
        return;
    if (arg->i > 0) {
        for (c = selmon->sel->next; c && !ISVISIBLE(c); c = c->next);
        if (!c)
            for (c = selmon->clients; c && !ISVISIBLE(c); c = c->next);
    } else {
        for (i = selmon->clients; i != selmon->sel; i = i->next)
            if (ISVISIBLE(i))
                c = i;
        if (!c)
            for (; i; i = i->next)
                if (ISVISIBLE(i))
                    c = i;
    }
    if (c) {
        c->focus();
        selmon->restack();
    }
}
int getrootptr(int *x, int *y) {
    int di;
    unsigned int dui;
    Window dummy;

    return XQueryPointer(dpy, root, &dummy, &dummy, x, y, &di, &di, &dui);
}
long getstate(Window w) {
    int format;
    long result = -1;
    unsigned char *p = NULL;
    unsigned long n, extra;
    Atom real;

    if (XGetWindowProperty(dpy, w, wmatom[WMState], 0L, 2L, False, wmatom[WMState],
                           &real, &format, &n, &extra, (unsigned char **) &p) != Success)
        return -1;
    if (n != 0)
        result = *p;
    XFree(p);
    return result;
}
int gettextprop(Window w, Atom atom, char *text, unsigned int size) {
    char **list = NULL;
    int n;
    XTextProperty name;

    if (!text || size == 0)
        return 0;
    text[0] = '\0';
    if (!XGetTextProperty(dpy, w, &name, atom) || !name.nitems)
        return 0;
    if (name.encoding == XA_STRING)
        strncpy(text, (char *) name.value, size - 1);
    else {
        if (XmbTextPropertyToTextList(dpy, &name, &list, &n) >= Success && n > 0 && *list) {
            strncpy(text, *list, size - 1);
            XFreeStringList(list);
        }
    }
    text[size - 1] = '\0';
    XFree(name.value);
    return 1;
}
void grabkeys(void) {
    updatenumlockmask();
    {
        unsigned int i, j;
        unsigned int modifiers[] = {0, LockMask, numlockmask, numlockmask | LockMask};
        KeyCode code;

        XUngrabKey(dpy, AnyKey, AnyModifier, root);
        for (i = 0; i < LENGTH(keys); i++)
            if ((code = XKeysymToKeycode(dpy, keys[i].keysym)))
                for (j = 0; j < LENGTH(modifiers); j++)
                    XGrabKey(dpy, code, keys[i].mod | modifiers[j], root,
                             True, GrabModeAsync, GrabModeAsync);
    }
}
void incnmaster(const Arg *arg) {
    unsigned int i;
    selmon->nmaster = MAX(selmon->nmaster + arg->i, 0);
    for (i = 0; i <= LENGTH(TAGS); ++i)
        if (selmon->tagset[selmon->seltags] & 1 << i)
            selmon->pertag->nmasters[(i + 1) % (LENGTH(TAGS) + 1)] = selmon->nmaster;
    arrange(selmon);
}
#ifdef XINERAMA
static int
isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info)
{
    while (n--)
        if (unique[n].x_org == info->x_org && unique[n].y_org == info->y_org
        && unique[n].width == info->width && unique[n].height == info->height)
            return 0;
    return 1;
}
#endif /* XINERAMA */
void keypress(XEvent *e) {
    unsigned int i;

    KeySym keysym;
    XKeyEvent *ev;

    ev = &e->xkey;
    keysym = XKeycodeToKeysym(dpy, (KeyCode) ev->keycode, 0);
    for (i = 0; i < LENGTH(keys); i++)
        if (keysym == keys[i].keysym
            && CLEANMASK(keys[i].mod) == CLEANMASK(ev->state)
            && keys[i].func)
            keys[i].func(&(keys[i].arg));
}
void killclient(const Arg *arg) {
    if (!selmon->sel)
        return;
    if (!sendevent(selmon->sel, wmatom[WMDelete])) {
        XGrabServer(dpy);
        XSetErrorHandler(xerrordummy);
        XSetCloseDownMode(dpy, DestroyAll);
        XKillClient(dpy, selmon->sel->win);
        XSync(dpy, False);
        XSetErrorHandler(xerror);
        XUngrabServer(dpy);
    }
}
void manage(Window w, XWindowAttributes *wa, int urgent) {
    Client *c, *t = NULL;
    Window trans = None;
    XWindowChanges wc;

    c = (Client *) ecalloc(1, sizeof(Client));
    c->win = w;
    /* geometry */
    c->x = c->oldx = wa->x;
    c->y = c->oldy = wa->y;
    c->w = c->oldw = wa->width;
    c->h = c->oldh = wa->height;
    c->oldbw = wa->border_width;

    c->updatetitle();
    if (XGetTransientForHint(dpy, w, &trans) && (t = wintoclient(trans))) {
        c->mon = t->mon;
        c->tags = t->tags;
    } else {
        c->mon = selmon;
        c->applyrules();
    }

    if (c->x + WIDTH(c) > c->mon->mx + c->mon->mw)
        c->x = c->mon->mx + c->mon->mw - WIDTH(c);
    if (c->y + HEIGHT(c) > c->mon->my + c->mon->mh)
        c->y = c->mon->my + c->mon->mh - HEIGHT(c);
    c->x = MAX(c->x, c->mon->mx);
    /* only fix client y-offset, if the client center might cover the bar */
    c->y = MAX(c->y, ((c->mon->by == c->mon->my) && (c->x + (c->w / 2) >= c->mon->wx)
                      && (c->x + (c->w / 2) < c->mon->wx + c->mon->ww)) ? bh : c->mon->my);
    c->bw = borderpx;

    wc.border_width = c->bw;
    if (c->x == selmon->wx) c->x += (c->mon->ww - WIDTH(c)) / 2 - c->bw;
    if (c->y == selmon->wy) c->y += (c->mon->wh - HEIGHT(c)) / 2 - c->bw;
    XConfigureWindow(dpy, w, CWBorderWidth, &wc);
    XSetWindowBorder(dpy, w, scheme[SchemeNorm][ColBorder].pixel);
    c->configure_(); /* propagates border_width, if size doesn't change */
    c->updatewindowtype();
    c->updatesizehints();
    c->updatewmhints();
    XSelectInput(dpy, w, EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask);
    c->grabbuttons(0);
    if (!c->isfloating)
        c->isfloating = c->oldstate = trans != None || c->isfixed;
    if (c->isfloating)
        XRaiseWindow(dpy, c->win);
    c->attach();
    c->attachstack();
    XChangeProperty(dpy, root, netatom[NetClientList], XA_WINDOW, 32, PropModeAppend,
                    (unsigned char *) &(c->win), 1);
    XMoveResizeWindow(dpy, c->win, c->x + 2 * sw, c->y, c->w, c->h); /* some windows require this */
    setclientstate(c, NormalState);
    if (c->mon == selmon)
        selmon->sel->unfocus(0);
    c->mon->sel = c;
    arrange(c->mon);
    XMapWindow(dpy, c->win);
    focusNULL();
    c->seturgent(urgent);
}
void mappingnotify(XEvent *e) {
    XMappingEvent *ev = &e->xmapping;

    XRefreshKeyboardMapping(ev);
    if (ev->request == MappingKeyboard)
        grabkeys();
}
void maprequest(XEvent *e) {
    static XWindowAttributes wa;
    XMapRequestEvent *ev = &e->xmaprequest;

    if (!XGetWindowAttributes(dpy, ev->window, &wa))
        return;
    if (wa.override_redirect)
        return;
    if (!wintoclient(ev->window))
        manage(ev->window, &wa, 1);
}
void monocle(Monitor *m) { m->monocle(); }
void motionnotify(XEvent *e) {
    static Monitor *mon = nullptr;
    Monitor *m;
    XMotionEvent *ev = &e->xmotion;

    if (ev->window != root)
        return;
    if ((m = recttomon(ev->x_root, ev->y_root, 1, 1)) != mon && mon) {
        selmon->sel->unfocus(1);
        selmon = m;
        focusNULL();
    }
    mon = m;
}
void movemouse(const Arg *arg) {
    int x, y, ocx, ocy, nx, ny;
    Client *c;
    Monitor *m;
    XEvent ev;
    Time lasttime = 0;

    if (!(c = selmon->sel))
        return;
    if (c->isfullscreen) /* no support moving fullscreen windows by mouse */
        return;
    selmon->restack();
    ocx = c->x;
    ocy = c->y;
    if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
                     None, cursor[CurMove]->cursor, CurrentTime) != GrabSuccess)
        return;
    if (!getrootptr(&x, &y))
        return;
    do {
        XMaskEvent(dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);
        switch (ev.type) {
            case ConfigureRequest:
            case Expose:
            case MapRequest:
                handler[ev.type](&ev);
                break;
            case MotionNotify:
                if ((ev.xmotion.time - lasttime) <= (1000 / 60))
                    continue;
                lasttime = ev.xmotion.time;

                nx = ocx + (ev.xmotion.x - x);
                ny = ocy + (ev.xmotion.y - y);
                if (abs(selmon->wx - nx) < snap)
                    nx = selmon->wx;
                else if (abs((selmon->wx + selmon->ww) - (nx + WIDTH(c))) < snap)
                    nx = selmon->wx + selmon->ww - WIDTH(c);
                if (abs(selmon->wy - ny) < snap)
                    ny = selmon->wy;
                else if (abs((selmon->wy + selmon->wh) - (ny + HEIGHT(c))) < snap)
                    ny = selmon->wy + selmon->wh - HEIGHT(c);
                if (!c->isfloating && selmon->lt[selmon->sellt]->arrange_
                    && (abs(nx - c->x) > snap || abs(ny - c->y) > snap))
                    togglefloating(NULL);
                if (!selmon->lt[selmon->sellt]->arrange_ || c->isfloating)
                    c->resize(nx, ny, c->w, c->h, 1);
                break;
        }
    } while (ev.type != ButtonRelease);
    XUngrabPointer(dpy, CurrentTime);
    if ((m = recttomon(c->x, c->y, c->w, c->h)) != selmon) {
        sendmon(c, m);
        selmon = m;
        focusNULL();
    }
}
void propertynotify(XEvent *e) {
    Client *c;
    Window trans;
    XPropertyEvent *ev = &e->xproperty;

    if ((ev->window == root) && (ev->atom == XA_WM_NAME))
        updatestatus();
    else if (ev->state == PropertyDelete)
        return; /* ignore */
    else if ((c = wintoclient(ev->window))) {
        switch (ev->atom) {
            default:
                break;
            case XA_WM_TRANSIENT_FOR:
                if (!c->ignoretransient && !c->isfloating && (XGetTransientForHint(dpy, c->win, &trans)) &&
                    (c->isfloating = (wintoclient(trans)) != NULL))
                    arrange(c->mon);
                break;
            case XA_WM_NORMAL_HINTS:
                c->updatesizehints();
                break;
            case XA_WM_HINTS:
                c->updatewmhints();
                drawbars();
                break;
        }
        if (ev->atom == XA_WM_NAME || ev->atom == netatom[NetWMName]) {
            c->updatetitle();
            if (c == c->mon->sel)
                c->mon->drawbar();
        }
        if (ev->atom == netatom[NetWMWindowType])
            c->updatewindowtype();
    }
}
void quit(const Arg *arg) {
    running = 0;
}
Monitor* recttomon(int x, int y, int w, int h) {
    Monitor *m, *r = selmon;
    int a, area = 0;

    for (m = mons; m; m = m->next)
        if ((a = INTERSECT(x, y, w, h, m)) > area) {
            area = a;
            r = m;
        }
    return r;
}
void resizemouse(const Arg *arg) {
    int ocx, ocy, nw, nh;
    Client *c;
    Monitor *m;
    XEvent ev;
    Time lasttime = 0;

    if (!(c = selmon->sel))
        return;
    if (c->isfullscreen) /* no support resizing fullscreen windows by mouse */
        return;
    selmon->restack();
    ocx = c->x;
    ocy = c->y;
    if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
                     None, cursor[CurResize]->cursor, CurrentTime) != GrabSuccess)
        return;
    XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w + c->bw - 1, c->h + c->bw - 1);
    do {
        XMaskEvent(dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);
        switch (ev.type) {
            case ConfigureRequest:
            case Expose:
            case MapRequest:
                handler[ev.type](&ev);
                break;
            case MotionNotify:
                if ((ev.xmotion.time - lasttime) <= (1000 / 60))
                    continue;
                lasttime = ev.xmotion.time;

                nw = MAX(ev.xmotion.x - ocx - 2 * c->bw + 1, 1);
                nh = MAX(ev.xmotion.y - ocy - 2 * c->bw + 1, 1);
                if (c->mon->wx + nw >= selmon->wx && c->mon->wx + nw <= selmon->wx + selmon->ww
                    && c->mon->wy + nh >= selmon->wy && c->mon->wy + nh <= selmon->wy + selmon->wh) {
                    if (!c->isfloating && selmon->lt[selmon->sellt]->arrange_
                        && (abs(nw - c->w) > snap || abs(nh - c->h) > snap))
                        togglefloating(NULL);
                }
                if (!selmon->lt[selmon->sellt]->arrange_ || c->isfloating)
                    c->resize(c->x, c->y, nw, nh, 1);
                break;
        }
    } while (ev.type != ButtonRelease);
    XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w + c->bw - 1, c->h + c->bw - 1);
    XUngrabPointer(dpy, CurrentTime);
    while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
    if ((m = recttomon(c->x, c->y, c->w, c->h)) != selmon) {
        sendmon(c, m);
        selmon = m;
        focusNULL();
    }
}
void run(void) {
    XEvent ev;
    fd_set rfds;
    int n;
    int dpyfd, maxfd;
    /* main event loop */
    XSync(dpy, False);
    dpyfd = ConnectionNumber(dpy);
    maxfd = fifofd;
    if (dpyfd > maxfd)
        maxfd = dpyfd;
    maxfd++;
    while (running) {
        FD_ZERO(&rfds);
        FD_SET(fifofd, &rfds);
        FD_SET(dpyfd, &rfds);
        n = select(maxfd, &rfds, NULL, NULL, NULL);
        if (n > 0) {
            if (FD_ISSET(fifofd, &rfds))
                dispatchcmd();
            if (FD_ISSET(dpyfd, &rfds))
                while (XCheckIfEvent(dpy, &ev, (int (*)(Display *, XEvent *, XPointer)) evpredicate, NULL)) {
                    //die("aye");
                    if (handler[ev.type])
                        handler[ev.type](&ev);
                    //int v = 0;
                    //for(Monitor* m = mons; m; m = m->next, v++)
                    //fprintf(stderr, "LE MONS: %d\n", v);
                } /* call handler */
        }
    }
}
void scan(void) {
    unsigned int i, num;
    Window d1, d2, *wins = NULL;
    XWindowAttributes wa;

    if (XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
        for (i = 0; i < num; i++) {
            if (!XGetWindowAttributes(dpy, wins[i], &wa)
                || wa.override_redirect || XGetTransientForHint(dpy, wins[i], &d1))
                continue;
            if (wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
                manage(wins[i], &wa, 0);
        }
        for (i = 0; i < num; i++) { /* now the transients */
            if (!XGetWindowAttributes(dpy, wins[i], &wa))
                continue;
            if (XGetTransientForHint(dpy, wins[i], &d1)
                && (wa.map_state == IsViewable || getstate(wins[i]) == IconicState))
                manage(wins[i], &wa, 0);
        }
        if (wins)
            XFree(wins);
    }
}
void sendmon(Client *c, Monitor *m) {
    if (c->mon == m)
        return;
    c->unfocus(1);
    c->detach();
    c->detachstack();
    c->mon = m;
    c->tags = m->tagset[m->seltags]; /* assign TAGS of target monitor */
    c->attach();
    c->attachstack();
    focusNULL();
    arrange(NULL);
}
void setclientstate(Client *c, long state) {
    long data[] = {state, None};

    XChangeProperty(dpy, c->win, wmatom[WMState], wmatom[WMState], 32,
                    PropModeReplace, (unsigned char *) data, 2);
}
int sendevent(Client *c, Atom proto) {
    int n;
    Atom *protocols;
    int exists = 0;
    XEvent ev;

    if (XGetWMProtocols(dpy, c->win, &protocols, &n)) {
        while (!exists && n--)
            exists = protocols[n] == proto;
        XFree(protocols);
    }
    if (exists) {
        ev.type = ClientMessage;
        ev.xclient.window = c->win;
        ev.xclient.message_type = wmatom[WMProtocols];
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = proto;
        ev.xclient.data.l[1] = CurrentTime;
        XSendEvent(dpy, c->win, False, NoEventMask, &ev);
    }
    return exists;
}
//void
//setgaps(const Arg *arg)
//{
//	if ((arg->i == 0) || (selmon->gappx + arg->i < 0))
//		selmon->gappx = 0;
//	else
//		selmon->gappx += arg->i;
//	arrange(selmon);
//}
void setlayout(const Arg *arg) {
    unsigned int i;
    if (!arg || !arg->v || arg->v != selmon->lt[selmon->sellt])
        selmon->sellt ^= 1;
    if (arg && arg->v)
        selmon->lt[selmon->sellt] = (Layout *) arg->v;
    strncpy(selmon->ltsymbol, selmon->lt[selmon->sellt]->symbol, sizeof selmon->ltsymbol);

    for (i = 0; i <= LENGTH(TAGS); ++i)
        if (selmon->tagset[selmon->seltags] & 1 << i) {
            selmon->pertag->ltidxs[(i + 1) % (LENGTH(TAGS) + 1)][selmon->sellt] = selmon->lt[selmon->sellt];
            selmon->pertag->sellts[(i + 1) % (LENGTH(TAGS) + 1)] = selmon->sellt;
        }

    if (selmon->sel)
        arrange(selmon);
    else
        selmon->drawbar();
}
/* arg > 1.0 will set mfact absolutely */
void setmfact(const Arg *arg) {
    float f;
    unsigned int i;

    if (!arg || !selmon->lt[selmon->sellt]->arrange_)
        return;
    f = arg->f < 1.0 ? arg->f + selmon->mfact : arg->f - 1.0;
    if (arg->f == 0.0)
        f = mfact;
    if (f < 0.05 || f > 0.95)
        return;
    selmon->mfact = f;
    for (i = 0; i <= LENGTH(TAGS); ++i)
        if (selmon->tagset[selmon->seltags] & 1 << i)
            selmon->pertag->mfacts[(i + 1) % (LENGTH(TAGS) + 1)] = f;
    arrange(selmon);
}
void settheme() {
    /* init appearance */
    scheme[LENGTH(colors)] = drw_scm_create(drw, colors[0], 3);
    for (int i = 0; i < LENGTH(colors); i++)
        scheme[i] = drw_scm_create(drw, colors[i], 3);
}
void setup() {
    XSetWindowAttributes wa;
    Atom utf8string;

    /* clean up any zombies immediately */
    sigchld(0);

    /* init screen */
    screen = DefaultScreen(dpy);
    sw = DisplayWidth(dpy, screen);
    sh = DisplayHeight(dpy, screen);
    root = RootWindow(dpy, screen);
    drw = drw_create(dpy, screen, root, sw, sh);
    if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
        die("no fonts could be loaded.");
    lrpad = drw->fonts->h;
    bh = drw->fonts->h + 10;
    updategeom();
    /* init atoms */
    utf8string =                XInternAtom(dpy, "UTF8_STRING", False);
    wmatom[WMProtocols] =       XInternAtom(dpy, "WM_PROTOCOLS", False);
    wmatom[WMDelete] =          XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    wmatom[WMState] =           XInternAtom(dpy, "WM_STATE", False);
    wmatom[WMTakeFocus] =       XInternAtom(dpy, "WM_TAKE_FOCUS", False);
    netatom[NetActiveWindow] =  XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    netatom[NetSupported] =     XInternAtom(dpy, "_NET_SUPPORTED", False);
    netatom[NetWMName] =        XInternAtom(dpy, "_NET_WM_NAME", False);
    netatom[NetWMState] =       XInternAtom(dpy, "_NET_WM_STATE", False);
    netatom[NetWMCheck] =       XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
    netatom[NetWMFullscreen] =  XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    netatom[NetWMWindowType] =  XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    netatom[NetClientList] =    XInternAtom(dpy, "_NET_CLIENT_LIST", False);
    /* init cursors */
    cursor[CurNormal] = drw_cur_create(drw, XC_left_ptr);
    cursor[CurResize] = drw_cur_create(drw, XC_sizing);
    cursor[CurMove] = drw_cur_create(drw, XC_fleur);

    settheme();

    /* init bars */
    updatebars();
    updatestatus();
    /* supporting window for NetWMCheck */
    wmcheckwin = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
    XChangeProperty(dpy, wmcheckwin, netatom[NetWMCheck], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char *) &wmcheckwin, 1);
    XChangeProperty(dpy, wmcheckwin, netatom[NetWMName], utf8string, 8,
                    PropModeReplace, (unsigned char *) "dwm", 3);
    XChangeProperty(dpy, root, netatom[NetWMCheck], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char *) &wmcheckwin, 1);
    /* EWMH support per view */
    XChangeProperty(dpy, root, netatom[NetSupported], XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) netatom, NetLast);
    XDeleteProperty(dpy, root, netatom[NetClientList]);
    /* select events */
    wa.cursor = cursor[CurNormal]->cursor;
    wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask
                    | ButtonPressMask | PointerMotionMask | EnterWindowMask
                    | LeaveWindowMask | StructureNotifyMask | PropertyChangeMask;
    XChangeWindowAttributes(dpy, root, CWEventMask | CWCursor, &wa);
    XSelectInput(dpy, root, wa.event_mask);
    grabkeys();
    focusNULL();

    fifofd = open(dwmfifo, O_RDWR | O_NONBLOCK);
    if (fifofd < 0)
        die("Failed to open() DWM fifo %s - %d:", dwmfifo, fifofd);
}
void sigchld(int unused) {
    if (signal(SIGCHLD, sigchld) == SIG_ERR)
        die("can't install SIGCHLD handler:");
    while (0 < waitpid(-1, NULL, WNOHANG));
}
void spawn(const Arg *arg) {
    if (arg->v == dmenucmd)
        dmenumon[0] = '0' + selmon->num;
    if (fork() == 0) {
        program_shell::stop();
        if (dpy)
            close(ConnectionNumber(dpy));
        setsid();
        execvp(((char **) arg->v)[0], (char **) arg->v);
        fprintf(stderr, "dwm: execvp %s", ((char **) arg->v)[0]);
        perror(" failed");
        exit(EXIT_SUCCESS);
    }
}
void tag(const Arg *arg) {
    if (selmon->sel && arg->ui & TAGMASK) {
        selmon->sel->tags = arg->ui & TAGMASK;
        focusNULL();
        arrange(selmon);
    }
}
void tagmon(const Arg *arg) {
    if (!selmon->sel || !mons->next)
        return;
    sendmon(selmon->sel, dirtomon(arg->i));
}
void tile(Monitor *m) { m->tile(); }
void togglebar(const Arg *arg) {
    unsigned int i;
    selmon->showbar = !selmon->showbar;
    for (i = 0; i <= LENGTH(TAGS); ++i)
        if (selmon->tagset[selmon->seltags] & 1 << i)
            selmon->pertag->showbars[(i + 1) % (LENGTH(TAGS) + 1)] = selmon->showbar;
    selmon->updatebarpos();
    XMoveResizeWindow(dpy, selmon->barwin, selmon->wx, selmon->by, selmon->ww, bh);
    arrange(selmon);
}
void togglefloating(const Arg *arg) {
    if (!selmon->sel)
        return;
    if (selmon->sel->isfullscreen) /* no support for fullscreen windows */
        return;
    selmon->sel->isfloating = !selmon->sel->isfloating || selmon->sel->isfixed;
    if (selmon->sel->isfloating)
        selmon->sel->resize(selmon->sel->x, selmon->sel->y,
                            selmon->sel->w, selmon->sel->h, 0);
    arrange(selmon);
}
void toggletag(const Arg *arg) {
    unsigned int newtags;

    if (!selmon->sel)
        return;
    newtags = selmon->sel->tags ^ (arg->ui & TAGMASK);
    if (newtags) {
        selmon->sel->tags = newtags;
        focusNULL();
        arrange(selmon);
    }
}
void toggleview(const Arg *arg) {
    unsigned int newtagset = selmon->tagset[selmon->seltags] ^(arg->ui & TAGMASK);
    int i;

    if (newtagset) {
        selmon->tagset[selmon->seltags] = newtagset;

        if (newtagset == ~0) {
            selmon->pertag->prevtag = selmon->pertag->curtag;
            selmon->pertag->curtag = 0;
        }

        /* test if the user did not select the same tag */
        if (!(newtagset & 1 << (selmon->pertag->curtag - 1))) {
            selmon->pertag->prevtag = selmon->pertag->curtag;
            for (i = 0; !(newtagset & 1 << i); i++);
            selmon->pertag->curtag = i + 1;
        }

        /* apply settings for this view */
        selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag];
        selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
        selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
        selmon->lt[selmon->sellt] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
        selmon->lt[selmon->sellt ^ 1] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt ^ 1];

        if (selmon->showbar != selmon->pertag->showbars[selmon->pertag->curtag])
            togglebar(NULL);

        focusNULL();
        arrange(selmon);
    }
}
void unmapnotify(XEvent *e) {
    Client *c;
    XUnmapEvent *ev = &e->xunmap;

    if ((c = wintoclient(ev->window))) {
        if (ev->send_event)
            setclientstate(c, WithdrawnState);
        else
            c->unmanage(0);
    }
}
void updatebars(void) {
    Monitor *m;
    XSetWindowAttributes wa = {
            .background_pixmap = ParentRelative,
            .event_mask = ButtonPressMask | ExposureMask,
            .override_redirect = True
    };
    XClassHint ch = {"dwm", "dwm"};
    for (m = mons; m; m = m->next) {
        if (m->barwin)
            continue;
        m->barwin = XCreateWindow(dpy, root, m->wx, m->by, m->ww, bh, 0, DefaultDepth(dpy, screen),
                                  CopyFromParent, DefaultVisual(dpy, screen),
                                  CWOverrideRedirect | CWBackPixmap | CWEventMask, &wa);
        XDefineCursor(dpy, m->barwin, cursor[CurNormal]->cursor);
        XMapRaised(dpy, m->barwin);
        XSetClassHint(dpy, m->barwin, &ch);
    }
}
void updateclientlist() {
    Client *c;
    Monitor *m;

    XDeleteProperty(dpy, root, netatom[NetClientList]);
    for (m = mons; m; m = m->next)
        for (c = m->clients; c; c = c->next)
            XChangeProperty(dpy, root, netatom[NetClientList],
                            XA_WINDOW, 32, PropModeAppend,
                            (unsigned char *) &(c->win), 1);
}
int updategeom(void) {
    int dirty = 0;

#ifdef XINERAMA
    if (XineramaIsActive(dpy)) {
        int i, j, n, nn;
        Client *c;
        Monitor *m;
        XineramaScreenInfo *info = XineramaQueryScreens(dpy, &nn);
        XineramaScreenInfo *unique = nullptr;

        for (n = 0, m = mons; m; m = m->next, n++);
        /* only consider unique geometries as separate screens */
        unique = (XineramaScreenInfo*) ecalloc(nn, sizeof(XineramaScreenInfo));
        for (i = 0, j = 0; i < nn; i++)
            if (isuniquegeom(unique, j, &info[i]))
                memcpy(&unique[j++], &info[i], sizeof(XineramaScreenInfo));
        XFree(info);
        nn = j;
        if (n <= nn) { /* new monitors available */
            for (i = 0; i < (nn - n); i++) {
                for (m = mons; m && m->next; m = m->next);
                if (m)
                    m->next = createmon();
                else
                    mons = createmon();
            }
            for (i = 0, m = mons; i < nn && m; m = m->next, i++)
                if (i >= n
                || unique[i].x_org != m->mx || unique[i].y_org != m->my
                || unique[i].width != m->mw || unique[i].height != m->mh)
                {
                    dirty = 1;
                    m->num = i;
                    m->mx = m->wx = unique[i].x_org;
                    m->my = m->wy = unique[i].y_org;
                    m->mw = m->ww = unique[i].width;
                    m->mh = m->wh = unique[i].height;
                    m->updatebarpos();
                }
        } else { /* less monitors available nn < n */
            for (i = nn; i < n; i++) {
                for (m = mons; m && m->next; m = m->next);
                while ((c = m->clients)) {
                    dirty = 1;
                    m->clients = c->next;
                    c->detachstack();
                    c->mon = mons;
                    c->attach();
                    c->attachstack();
                }
                if (m == selmon)
                    selmon = mons;
                cleanupmon(m);
            }
        }
        free(unique);
    } else
#endif /* XINERAMA */
    { /* default monitor setup */
        if (!mons)
            mons = createmon();
        if (mons->mw != sw || mons->mh != sh) {
            dirty = 1;
            mons->mw = mons->ww = sw;
            mons->mh = mons->wh = sh;
            mons->updatebarpos();
        }
    }
    if (dirty) {
        selmon = mons;
        selmon = wintomon(root);
    }
    return dirty;
}
void updatenumlockmask() {
    unsigned int i, j;
    XModifierKeymap *modmap;

    numlockmask = 0;
    modmap = XGetModifierMapping(dpy);
    for (i = 0; i < 8; i++)
        for (j = 0; j < modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j]
                == XKeysymToKeycode(dpy, XK_Num_Lock))
                numlockmask = (1 << i);
    XFreeModifiermap(modmap);
}
void updatestatus(void) {
    Monitor *m;
    if (!gettextprop(root, XA_WM_NAME, stext, sizeof(stext)))
        strcpy(stext, "dwm-VERSION");
    for (m = mons; m; m = m->next)
        m->drawbar();
}
void view(const Arg *arg) {
    int i;
    unsigned int tmptag;

    if ((arg->ui & TAGMASK) == selmon->tagset[selmon->seltags])
        return;
    selmon->seltags ^= 1; /* toggle sel tagset */
    if (arg->ui & TAGMASK) {
        selmon->tagset[selmon->seltags] = arg->ui & TAGMASK;
        selmon->pertag->prevtag = selmon->pertag->curtag;

        if (arg->ui == ~0)
            selmon->pertag->curtag = 0;
        else {
            for (i = 0; !(arg->ui & 1 << i); i++);
            selmon->pertag->curtag = i + 1;
        }
    } else {
        tmptag = selmon->pertag->prevtag;
        selmon->pertag->prevtag = selmon->pertag->curtag;
        selmon->pertag->curtag = tmptag;
    }

    selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag];
    selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
    selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
    selmon->lt[selmon->sellt] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
    selmon->lt[selmon->sellt ^ 1] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt ^ 1];

    if (selmon->showbar != selmon->pertag->showbars[selmon->pertag->curtag])
        togglebar(NULL);

    focusNULL();
    arrange(selmon);
}
Client* wintoclient(Window w) {
    Client *c;
    Monitor *m;

    for (m = mons; m; m = m->next)
        for (c = m->clients; c; c = c->next)
            if (c->win == w)
                return c;
    return nullptr;
}
Monitor* wintomon(Window w) {
    int x, y;
    Client *c;
    Monitor *m;

    if (w == root && getrootptr(&x, &y))
        return recttomon(x, y, 1, 1);
    for (m = mons; m; m = m->next)
        if (w == m->barwin)
            return m;
    if ((c = wintoclient(w)))
        return c->mon;
    return selmon;
}
/* There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's). Other types of errors call Xlibs
 * default error handler, which may call exit. */
int xerror(Display *dpy, XErrorEvent *ee) {
    if (ee->error_code == BadWindow
        || (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
        || (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
        || (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
        || (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
        || (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
        || (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
        || (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
        || (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
        return 0;
    fprintf(stderr, "dwm: fatal error: request code=%d, error code=%d\n",
            ee->request_code, ee->error_code);
    return xerrorxlib(dpy, ee); /* may call exit */
}
int xerrordummy(Display *dpy, XErrorEvent *ee) {
    return 0;
}
/* Startup Error handler to check if another window manager
 * is already running. */
int xerrorstart(Display *dpy, XErrorEvent *ee) {
    die("dwm: another window manager is already running");
    return -1;
}
void zoom(const Arg *arg) {
    Client *c = selmon->sel;

    if (!selmon->lt[selmon->sellt]->arrange_
        || (selmon->sel && selmon->sel->isfloating))
        return;
    if (c == selmon->clients->nexttiled())
        if (!c || !(c = c->next->nexttiled()))
            return;
    c->pop();
}
void resource_load(XrmDatabase db, char *name, enum resource_type rtype, void *dst) {
    char *sdst = nullptr;
    int *idst = nullptr;
    float *fdst = nullptr;

    sdst = (char *) dst;
    idst = (int *) dst;
    fdst = (float *) dst;

    char fullname[256];
    char *type;
    XrmValue ret;

    snprintf(fullname, sizeof(fullname), "%s.%s", "dwm", name);
    fullname[sizeof(fullname) - 1] = '\0';

    XrmGetResource(db, fullname, "*", &type, &ret);
    if (!(ret.addr == nullptr || strncmp("String", type, 64))) {
        switch (rtype) {
            case STRING:
                strcpy(sdst, ret.addr);
                break;
            case INTEGER:
                *idst = strtoul(ret.addr, nullptr, 10);
                break;
            case FLOAT:
                *fdst = strtof(ret.addr, nullptr);
                break;
        }
    }
}
void load_xresources() {
    Display *display;
    char *resm;
    XrmDatabase db;
    ResourcePref *p;

    display = XOpenDisplay(NULL);
    resm = XResourceManagerString(display);
    if (!resm)
        return;

    db = XrmGetStringDatabase(resm);
    for (p = resources; p < resources + LENGTH(resources); p++)
        resource_load(db, p->name, p->type, p->dst);
    XCloseDisplay(display);
    settheme();
}

int test(terminal::term_stream& stream, int argc, const char* argv[]) {
	stream << "hello world!";
	return 0;
}

int restart(terminal::term_stream& stream, int argc, const char* argv[]) {
    program_shell::stop();
	quit(nullptr);
	return 0;
}

int main(int argc, char *argv[]) {
	program_shell::set_var("TERM_HEADER", "[C-DWM Shell v0.1]");
	program_shell::set_var("TERM_PROMPT", "$ > ");
	program_shell::add_cmd("test", test);
	program_shell::add_cmd("restart", restart);
    int err = program_shell::init(ctl_port);
    if (err <0) {
        fprintf(stderr, "[program_shell::init] err: %d", err);
    }
    if (argc == 2 && !strcmp("-v", argv[1]))
        die("dwm-VERSION");
    else if (argc != 1)
        die("usage: dwm [-v]");
    if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
        fputs("warning: no locale support\n", stderr);
    if (!(dpy = XOpenDisplay(nullptr)))
        die("dwm: cannot open display");
    checkotherwm();
    XrmInitialize();
    scheme = (Clr **) ecalloc(LENGTH(colors) + 1, sizeof(Clr *));
    load_xresources();
    setup();
#ifdef __OpenBSD__
    if (pledge("stdio rpath proc exec", NULL) == -1)
        die("pledge");
#endif /* __OpenBSD__ */
    scan();
    run();
    cleanup();
    XCloseDisplay(dpy);
    return EXIT_SUCCESS;
}
void centeredmaster(Monitor *m) { m->centeredmaster(); }
void centeredfloatingmaster(Monitor *m) { m->centeredfloatingmaster(); }

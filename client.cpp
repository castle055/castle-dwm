#include <cmath>
#include <cstring>
#include <X11/Xutil.h>
#include "castle-dwm.h"
#include "util.h"

//===== Client Class implementations =====
void Client::applyrules() {
    const char *class_, *instance;
    unsigned int i;
    const Rule *r;
    Monitor *m;
    XClassHint ch = {nullptr, nullptr};

    /* rule matching */
    isfloating = 0;
    tags = 0;
    XGetClassHint(dpy, win, &ch);
    class_ = ch.res_class ? ch.res_class : broken;
    instance = ch.res_name ? ch.res_name : broken;

    for (i = 0; i < LENGTH(rules); i++) {
        r = &rules[i];
        if ((!r->title || strstr(name, r->title))
            && (!r->class_ || strstr(class_, r->class_))
            && (!r->instance || strstr(instance, r->instance))) {
            isfloating = r->isfloating;
            tags |= r->tags;
            for (m = mons; m && m->num != r->monitor; m = m->next);
            if (m)
                mon = m;
        }
    }
    if (ch.res_class)
        XFree(ch.res_class);
    if (ch.res_name)
        XFree(ch.res_name);
    tags = tags & TAGMASK ? tags & TAGMASK : mon->tagset[mon->seltags];
}
int Client::applysizehints(int *X, int *Y, int *W, int *H, int interact) {
    int baseismin;
    Monitor *m = mon;

    /* set minimum possible */
    *W = MAX(1, *W);
    *H = MAX(1, *H);
    if (interact) {
        if (*X > sw)
            *X = sw - WIDTH(this);
        if (*Y > sh)
            *Y = sh - HEIGHT(this);
        if (*X + *W + 2 * bw < 0)
            *X = 0;
        if (*Y + *H + 2 * bw < 0)
            *Y = 0;
    } else {
        if (*X >= m->wx + m->ww)
            *X = m->wx + m->ww - WIDTH(this);
        if (*Y >= m->wy + m->wh)
            *Y = m->wy + m->wh - HEIGHT(this);
        if (*X + *W + 2 * bw <= m->wx)
            *X = m->wx;
        if (*Y + *H + 2 * bw <= m->wy)
            *Y = m->wy;
    }
    if (*H < bh)
        *H = bh;
    if (*W < bh)
        *W = bh;
    if (resizehints || isfloating || !mon->lt[mon->sellt]->arrange_) {
        /* see last two sentences in ICCCM 4.1.2.3 */
        baseismin = basew == minw && baseh == minh;
        if (!baseismin) { /* temporarily remove base dimensions */
            *W -= basew;
            *H -= baseh;
        }
        /* adjust for aspect limits */
        if (mina > 0 && maxa > 0) {
            if (maxa < (float) *W / (float) *H)
                *W = (int)lround((float)*H * maxa + 0.5);
            else if (mina < (float) *H / (float) *W)
                *H = (int)lround((float)*W * mina + 0.5);
        }
        if (baseismin) { /* increment calculation requires this */
            *W -= basew;
            *H -= baseh;
        }
        /* adjust for increment value */
        if (incw)
            *W -= *W % incw;
        if (inch)
            *H -= *H % inch;
        /* restore base dimensions */
        *W = MAX(*W + basew, minw);
        *H = MAX(*H + baseh, minh);
        if (maxw)
            *W = MIN(*W, maxw);
        if (maxh)
            *H = MIN(*H, maxh);
    }
    return *X != x || *Y != y || *W != w || *H != h;
}
void Client::attach() {
    next = mon->clients;
    mon->clients = this;
}
void Client::attachstack() {
    snext = mon->stack;
    mon->stack = this;
}
void Client::configure_() {
    XConfigureEvent ce;

    ce.type = ConfigureNotify;
    ce.display = dpy;
    ce.event = win;
    ce.window = win;
    ce.x = x;
    ce.y = y;
    ce.width = w;
    ce.height = h;
    ce.border_width = bw;
    ce.above = None;
    ce.override_redirect = False;
    XSendEvent(dpy, win, False, StructureNotifyMask, (XEvent *) &ce);
}
void Client::detach() {
    Client **tc;

    for (tc = &mon->clients; *tc && *tc != this; tc = &(*tc)->next);
    *tc = next;
}
void Client::detachstack() {
    Client **tc, *t;

    for (tc = &mon->stack; *tc && *tc != this; tc = &(*tc)->snext);
    *tc = snext;

    if (this == mon->sel) {
        for (t = mon->stack; t && !ISVISIBLE(t); t = t->snext);
        mon->sel = t;
    }
}
void Client::focus() {
    Client *c = this;

    if (!c || !ISVISIBLE(c))
        for (c = selmon->stack; c && !ISVISIBLE(c); c = c->snext);
    if (selmon->sel && selmon->sel != this)
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
Atom Client::getatomprop(Atom prop) {
    int di;
    unsigned long dl;
    unsigned char *p = NULL;
    Atom da, atom = None;

    if (XGetWindowProperty(dpy, win, prop, 0L, sizeof atom, False, XA_ATOM,
                           &da, &di, &dl, &dl, &p) == Success && p) {
        atom = *(Atom *) p;
        XFree(p);
    }
    return atom;
}
void Client::grabbuttons(int focused) {
    updatenumlockmask();
    {
        unsigned int i, j;
        unsigned int modifiers[] = {0, LockMask, numlockmask, numlockmask | LockMask};
        XUngrabButton(dpy, AnyButton, AnyModifier, win);
        if (!focused)
            XGrabButton(dpy, AnyButton, AnyModifier, win, False,
                        BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
        for (i = 0; i < LENGTH(buttons); i++)
            if (buttons[i].click == ClkClientWin)
                for (j = 0; j < LENGTH(modifiers); j++)
                    XGrabButton(dpy, buttons[i].button,
                                buttons[i].mask | modifiers[j],
                                win, False, BUTTONMASK,
                                GrabModeAsync, GrabModeSync, None, None);
    }
}
void Client::pop() {
    detach();
    attach();
    focus();
    arrange(mon);
}
void Client::resize(int X, int Y, int W, int H, int interact) {
    if (applysizehints(&X, &Y, &W, &H, interact))
        resizeclient(X, Y, W, H);
}
void Client::resizeclient(int X, int Y, int W, int H) {
    XWindowChanges wc;

    oldx = x;
    x = wc.x = X;
    oldy = y;
    y = wc.y = Y;
    oldw = w;
    w = wc.width = W;
    oldh = h;
    h = wc.height = H;
    wc.border_width = bw;
    XConfigureWindow(dpy, win, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &wc);
    configure_();
    XSync(dpy, False);
}
void Client::setfocus() {
    if (!neverfocus) {
        XSetInputFocus(dpy, win, RevertToPointerRoot, CurrentTime);
        XChangeProperty(dpy, root, netatom[NetActiveWindow],
                        XA_WINDOW, 32, PropModeReplace,
                        (unsigned char *) &(win), 1);
    }
    sendevent(this, wmatom[WMTakeFocus]);
}
void Client::setfullscreen(int fullscreen) {
    if (fullscreen && !isfullscreen) {
        XChangeProperty(dpy, win, netatom[NetWMState], XA_ATOM, 32,
                        PropModeReplace, (unsigned char *) &netatom[NetWMFullscreen], 1);
        isfullscreen = 1;
        oldstate = isfloating;
        oldbw = bw;
        bw = 0;
        isfloating = 1;
        resizeclient(mon->mx, mon->my, mon->mw, mon->mh);
        XRaiseWindow(dpy, win);
    } else if (!fullscreen && isfullscreen) {
        XChangeProperty(dpy, win, netatom[NetWMState], XA_ATOM, 32,
                        PropModeReplace, (unsigned char *) 0, 0);
        isfullscreen = 0;
        isfloating = oldstate;
        bw = oldbw;
        x = oldx;
        y = oldy;
        w = oldw;
        h = oldh;
        resizeclient(x, y, w, h);
        arrange(mon);
    }
}
void Client::seturgent(int urg) {
    if (urg) {
        unsigned int i;
        /* === This focuses windows when urgent ===*/
        /* Useful when having rules for different clases to go to different TAGS */
        for (i = 0; i < LENGTH(TAGS) && !((unsigned int) (1 << i) & tags); i++);
        if (i < LENGTH(TAGS)) {
            const Arg a = {.ui = (unsigned int) (1 << i)};
            selmon = mon;
            view(&a);
            focus();
            selmon->restack();
        }
    }
    XWMHints *wmh;

    isurgent = urg;
    if (!(wmh = XGetWMHints(dpy, win)))
        return;
    wmh->flags = urg ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
    XSetWMHints(dpy, win, wmh);
    XFree(wmh);
}
void Client::showhide() {
    if (!this)
        return;
    if (ISVISIBLE(this)) {
        /* show clients top down */
        XMoveWindow(dpy, win, x, y);
        if ((!mon->lt[mon->sellt]->arrange_ || isfloating) && !isfullscreen)
            resize(x, y, w, h, 0);
        snext->showhide();
    } else {
        /* hide clients bottom up */
        snext->showhide();
        XMoveWindow(dpy, win, WIDTH(this) * -2, y);
    }
}
void Client::unfocus(int setfocus) {
    if (!this)
        return;
    grabbuttons(0);
    XSetWindowBorder(dpy, win, scheme[SchemeNorm][ColBorder].pixel);
    if (setfocus) {
        XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
        XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
    }
}
void Client::unmanage(int destroyed) {
    Monitor *m = mon;
    XWindowChanges wc;

    detach();
    detachstack();
    if (!destroyed) {
        wc.border_width = oldbw;
        XGrabServer(dpy); /* avoid race conditions */
        XSetErrorHandler(xerrordummy);
        XConfigureWindow(dpy, win, CWBorderWidth, &wc); /* restore border */
        XUngrabButton(dpy, AnyButton, AnyModifier, win);
        setclientstate(this, WithdrawnState);
        XSync(dpy, False);
        XSetErrorHandler(xerror);
        XUngrabServer(dpy);
    }
    free(this);
    focusNULL();
    updateclientlist();
    arrange(m);
}
void Client::updatesizehints() {
    long msize;
    XSizeHints size;

    if (!XGetWMNormalHints(dpy, win, &size, &msize))
        /* size is uninitialized, ensure that size.flags aren't used */
        size.flags = PSize;
    if (size.flags & PBaseSize) {
        basew = size.base_width;
        baseh = size.base_height;
    } else if (size.flags & PMinSize) {
        basew = size.min_width;
        baseh = size.min_height;
    } else
        basew = baseh = 0;
    if (size.flags & PResizeInc) {
        incw = size.width_inc;
        inch = size.height_inc;
    } else
        incw = inch = 0;
    if (size.flags & PMaxSize) {
        maxw = size.max_width;
        maxh = size.max_height;
    } else
        maxw = maxh = 0;
    if (size.flags & PMinSize) {
        minw = size.min_width;
        minh = size.min_height;
    } else if (size.flags & PBaseSize) {
        minw = size.base_width;
        minh = size.base_height;
    } else
        minw = minh = 0;
    if (size.flags & PAspect) {
        mina = (float) size.min_aspect.y / size.min_aspect.x;
        maxa = (float) size.max_aspect.x / size.max_aspect.y;
    } else
        maxa = mina = 0.0;
    isfixed = (maxw && maxh && maxw == minw && maxh == minh);
}
void Client::updatetitle() {
    if (!gettextprop(win, netatom[NetWMName], name, sizeof name))
        gettextprop(win, XA_WM_NAME, name, sizeof name);
    if (name[0] == '\0') /* hack to mark broken clients */
        strcpy(name, broken);
}
void Client::updatewindowtype() {
    Atom state = getatomprop(netatom[NetWMState]);
    Atom wtype = getatomprop(netatom[NetWMWindowType]);

    if (state == netatom[NetWMFullscreen])
        setfullscreen(1);
    if (wtype == netatom[NetWMWindowTypeDialog])
        isfloating = 1;
}
void Client::updatewmhints() {
    XWMHints *wmh;

    if ((wmh = XGetWMHints(dpy, win))) {
        if (this == selmon->sel && wmh->flags & XUrgencyHint) {
            wmh->flags &= ~XUrgencyHint;
            XSetWMHints(dpy, win, wmh);
        } else
            isurgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
        if (wmh->flags & InputHint)
            neverfocus = !wmh->input;
        else
            neverfocus = 0;
        XFree(wmh);
    }
}
Client* Client::nexttiled() {
    Client* c = this;
    for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next);
    return c;
}
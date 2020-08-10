#include <cstdio>
#include <cstdlib>
#include <X11/Xutil.h>
#include <cstring>
#include "castle-dwm.h"
#include "util.h"

//===== Monitor Class implementations =====
void Monitor::arrangemon() {
    strncpy(ltsymbol, lt[sellt]->symbol, sizeof ltsymbol);
    if (lt[sellt]->arrange_)
        lt[sellt]->arrange_(this);
}
int Monitor::drawstatusbar(int bh, char *stext) {
    int ret, i, w, x, len;
    short isCode = 0;
    char *text;
    char *p;

    len = strlen(stext) + 1;
    if (!(text = (char *) malloc(sizeof(char) * len)))
        die("malloc");
    p = text;
    memcpy(text, stext, len);

    /* compute width of the status text */
    w = 0;
    i = -1;
    while (text[++i]) {
        if (text[i] == '^') {
            if (!isCode) {
                isCode = 1;
                text[i] = '\0';
                w += TEXTW(text) - lrpad;
                text[i] = '^';
                if (text[++i] == 'f')
                    w += atoi(text + ++i);
            } else {
                isCode = 0;
                text = text + i + 1;
                i = -1;
            }
        }
    }
    if (!isCode)
        w += TEXTW(text) - lrpad;
    else
        isCode = 0;
    text = p;

    w += 2; /* 1px padding on both sides */
    ret = x = ww - w;

    drw_setscheme(drw, scheme[LENGTH(colors)]);
    drw->scheme[ColFg] = scheme[SchemeNorm][ColFg];
    drw->scheme[ColBg] = scheme[SchemeNorm][ColBg];
    drw_rect(drw, x, 0, w, bh, 1, 1);
    x++;

    /* process status text */
    i = -1;
    while (text[++i]) {
        if (text[i] == '^' && !isCode) {
            isCode = 1;

            text[i] = '\0';
            w = TEXTW(text) - lrpad;
            drw_text(drw, x, bh / 2, w, 0, 0, text, 0);

            x += w;

            /* process code */
            while (text[++i] != '^') {
                if (text[i] == 'c') {
                    char buf[8];
                    memcpy(buf, (char *) text + i + 1, 7);
                    buf[7] = '\0';
                    drw_clr_create(drw, &drw->scheme[ColFg], buf);
                    i += 7;
                } else if (text[i] == 'b') {
                    char buf[8];
                    memcpy(buf, (char *) text + i + 1, 7);
                    buf[7] = '\0';
                    drw_clr_create(drw, &drw->scheme[ColBg], buf);
                    i += 7;
                } else if (text[i] == 'd') {
                    drw->scheme[ColFg] = scheme[SchemeNorm][ColFg];
                    drw->scheme[ColBg] = scheme[SchemeNorm][ColBg];
                } else if (text[i] == 'r') {
                    int rx = atoi(text + ++i);
                    while (text[++i] != ',');
                    int ry = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rw = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rh = atoi(text + ++i);

                    drw_rect(drw, rx + x, ry, rw, rh, 1, 0);
                } else if (text[i] == 'a') {
                    int rx = atoi(text + ++i);
                    while (text[++i] != ',');
                    int ry = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rw = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rh = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rang1 = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rang2 = atoi(text + ++i);

                    drw_arc(drw, rx + x, ry, rw, rh, rang1 * 64, rang2 * 64, 0, 0);
                } else if (text[i] == 'A') {
                    int rx = atoi(text + ++i);
                    while (text[++i] != ',');
                    int ry = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rw = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rh = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rang1 = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rang2 = atoi(text + ++i);

                    drw_arc(drw, rx + x, ry, rw, rh, rang1 * 64, rang2 * 64, 1, 0);
                } else if (text[i] == 'l') {
                    int rx1 = atoi(text + ++i);
                    while (text[++i] != ',');
                    int ry1 = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rx2 = atoi(text + ++i);
                    while (text[++i] != ',');
                    int ry2 = atoi(text + ++i);

                    drw_line(drw, rx1 + x, ry1, rx2 + x, ry2);
                } else if (text[i] == 'p') {
                    int rx1 = atoi(text + ++i);
                    while (text[++i] != ',');
                    int ry1 = atoi(text + ++i);

                    drw_rect(drw, rx1, ry1, 1, 1, 1, 0);
                } else if (text[i] == 'f') {
                    x += atoi(text + ++i);
                }
            }

            text = text + i + 1;
            i = -1;
            isCode = 0;
        }
    }

    if (!isCode) {
        w = TEXTW(text) - lrpad;
        drw_text(drw, x, 0, w, bh, 0, text, 0);
    }

    drw_setscheme(drw, scheme[SchemeNorm]);
    free(p);

    return ret;
}
void Monitor::drawbar() {
    int x, w, tw = 0;
    int boxs = drw->fonts->h / 9;
    int boxw = drw->fonts->h / 6 + 2;
    unsigned int i, occ = 0, urg = 0;
    Client *c;

    // Draws rectangle over all bar so that there are no glitches
    drw_setscheme(drw, scheme[this == selmon ? SchemeSel : SchemeNorm]);
    drw_rect(drw, 0, 0, ww, bh, 1, 1);

    /* draw status first so it can be overdrawn by TAGS later */
    //if (m == selmon || 1) { /* status is only drawn on selected monitor */
    tw = ww - drawstatusbar(bh, stext);
    //if (m == selmon) { /* status is only drawn on selected monitor */
    //	drw_setscheme(drw, scheme[SchemeNorm]);
    //	tw = TEXTW(stext) - lrpad + 2; /* 2px right padding */
    //	drw_text(drw, m->ww - tw, 0, tw, bh, 0, stext, 0);
    //}

    for (c = clients; c; c = c->next) {
        occ |= c->tags;
        if (c->isurgent)
            urg |= c->tags;
    }

    x = 0;
    for (i = 0; i < LENGTH(TAGS); i++) {
        w = TEXTW(TAGS[i]);
        drw_setscheme(drw, scheme[tagset[seltags] & 1 << i ? SchemeSel : SchemeNorm]);
        drw_text(drw, x, 3, w - 4, bh - 6, lrpad / 2 - 2, TAGS[i], urg & 1 << i);
        if (occ & 1 << i)
            drw_rect(drw, x + boxs, boxs, boxw, boxw,
                     this == selmon && selmon->sel && selmon->sel->tags & 1 << i,
                     urg & 1 << i);
        x += w;
    }
    w = blw = TEXTW(ltsymbol);
    drw_setscheme(drw, scheme[SchemeNorm]);
    x = drw_text(drw, x, 3, w, bh - 6, lrpad / 2, ltsymbol, 0);

    if ((w = ww - tw - x) > bh) {
        if (sel) {
            drw_setscheme(drw, scheme[this == selmon ? SchemeSel : SchemeNorm]);
            drw_text(drw, x, 0, w, bh, (lrpad + w - TEXTW(sel->name)) / 2, sel->name, 0);
            if (sel->isfloating)
                drw_rect(drw, x + boxs + 4, 3, w - 8, bh - 6, sel->isfixed, 0);
        } else {
            drw_setscheme(drw, scheme[SchemeNorm]);
            drw_rect(drw, x, 0, w, bh, 1, 1);
        }
    }
    drw_map(drw, barwin, 0, 0, ww, bh);
}
void Monitor::monocle() {
    unsigned int n = 0;
    Client *c;

    for (c = clients; c; c = c->next)
        if (ISVISIBLE(c))
            n++;
    if (n > 0) /* override layout symbol */
        snprintf(ltsymbol, sizeof ltsymbol, "[%d]", n);
    for (c = clients->nexttiled(); c; c = c->next->nexttiled())
        c->resize(wx, wy, ww - 2 * c->bw, wh - 2 * c->bw, 0);
}
void Monitor::restack() {
    Client *c;
    XEvent ev;
    XWindowChanges wc;

    drawbar();
    if (!sel)
        return;
    if (sel->isfloating || !lt[sellt]->arrange_)
        XRaiseWindow(dpy, sel->win);
    if (lt[sellt]->arrange_) {
        wc.stack_mode = Below;
        wc.sibling = barwin;
        for (c = stack; c; c = c->snext)
            if (!c->isfloating && ISVISIBLE(c)) {
                XConfigureWindow(dpy, c->win, CWSibling | CWStackMode, &wc);
                wc.sibling = c->win;
            }
    }
    XSync(dpy, False);
    while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}
void Monitor::tile() {
    unsigned int i, n, h, mw, my, ty;
    Client *c;

    for (n = 0, c = clients->nexttiled(); c; c = c->next->nexttiled(), n++);
    if (n == 0)
        return;

    if (n > nmaster)
        mw = nmaster ? ww * mfact : 0;
        //mw = m->nmaster ? (m->ww - (g = gappx)) * m->mfact : 0;
    else
        mw = ww - gappx;
    for (i = 0, my = ty = gappx, c = clients->nexttiled(); c; c = c->next->nexttiled(), i++)
        if (i < nmaster) {
//<<<<<<< HEAD
            //h = (m->wh - my) / (MIN(n, m->nmaster) - i);
            h = (wh - my) / (MIN(n, nmaster) - i) - gappx;
            c->resize(wx + gappx, wy + my, mw - (2 * c->bw) - gappx, h - (2 * c->bw), 0);
            my += HEIGHT(c) + gappx;
        } else {
            //h = (m->wh - ty) / (n - i);
            h = (wh - ty) / (n - i) - gappx;
            c->resize(wx + mw + gappx, wy + ty, ww - mw - (2 * c->bw) - 2 * gappx, h - (2 * c->bw), 0);
            ty += HEIGHT(c) + gappx;
//=======
//			h = (m->wh - my) / (MIN(n, m->nmaster) - i);
//			resize(c, m->wx, m->wy + my, mw - (2*c->bw), h - (2*c->bw), 0);
//			if (my + HEIGHT(c) < m->wh)
//				my += HEIGHT(c);
//		} else {
//			h = (m->wh - ty) / (n - i);
//			resize(c, m->wx + mw, m->wy + ty, m->ww - mw - (2*c->bw), h - (2*c->bw), 0);
//			if (ty + HEIGHT(c) < m->wh)
//				ty += HEIGHT(c);
//>>>>>>> f09418bbb6651ab4c299cfefbe1d18de401f630e
        }
}
void Monitor::updatebarpos() {
    wy = my;
    wh = mh;
    if (showbar) {
        wh -= bh;
        by = topbar ? wy : wy + wh;
        wy = topbar ? wy + bh : wy;
    } else
        by = -bh;
}
void Monitor::centeredmaster() {
    unsigned int i, n, h, mw, mx, my, oty, ety, tw;
    Client *c;

    /* count number of clients in the selected monitor */
    for (n = 0, c = clients->nexttiled(); c; c = c->next->nexttiled(), n++);
    if (n == 0)
        return;

    /* initialize areas */
    mw = ww;
    mx = 0;
    my = 0;
    tw = mw;

    if (n > nmaster) {
        /* go mfact box in the center if more than nmaster clients */
        mw = nmaster ? ww * mfact : 0;
        tw = ww - mw;

        if (n - nmaster > 1) {
            /* only one client */
            mx = (ww - mw) / 2;
            tw = (ww - mw) / 2;
        }
    }

    oty = 0;
    ety = 0;
    for (i = 0, c = clients->nexttiled(); c; c = c->next->nexttiled(), i++)
        if (i < nmaster) {
            /* nmaster clients are stacked vertically, in the center
             * of the screen */
            h = (wh - my) / (MIN(n, nmaster) - i);
            c->resize(wx + mx, wy + my, mw - (2 * c->bw),
                      h - (2 * c->bw), 0);
            my += HEIGHT(c);
        } else {
            /* stack clients are stacked vertically */
            if ((i - nmaster) % 2) {
                h = (wh - ety) / ((1 + n - i) / 2);
                c->resize(wx, wy + ety, tw - (2 * c->bw),
                          h - (2 * c->bw), 0);
                ety += HEIGHT(c);
            } else {
                h = (wh - oty) / ((1 + n - i) / 2);
                c->resize(wx + mx + mw, wy + oty,
                          tw - (2 * c->bw), h - (2 * c->bw), 0);
                oty += HEIGHT(c);
            }
        }
}
void Monitor::centeredfloatingmaster() {
    unsigned int i, n, w, mh, mw, mx, mxo, my, myo, tx;
    Client *c;

    /* count number of clients in the selected monitor */
    for (n = 0, c = clients->nexttiled(); c; c = c->next->nexttiled(), n++);
    if (n == 0)
        return;

    /* initialize nmaster area */
    if (n > nmaster) {
        /* go mfact box in the center if more than nmaster clients */
        if (ww > wh) {
            mw = nmaster ? ww * mfact : 0;
            mh = nmaster ? wh * 0.9 : 0;
        } else {
            mh = nmaster ? wh * mfact : 0;
            mw = nmaster ? ww * 0.9 : 0;
        }
        mx = mxo = (ww - mw) / 2;
        my = myo = (wh - mh) / 2;
    } else {
        /* go fullscreen if all clients are in the master area */
        mh = wh;
        mw = ww;
        mx = mxo = 0;
        my = myo = 0;
    }

    for (i = tx = 0, c = clients->nexttiled(); c; c = c->next->nexttiled(), i++)
        if (i < nmaster) {
            /* nmaster clients are stacked horizontally, in the center
             * of the screen */
            w = (mw + mxo - mx) / (MIN(n, nmaster) - i);
            c->resize(wx + mx, wy + my, w - (2 * c->bw),
                      mh - (2 * c->bw), 0);
            mx += WIDTH(c);
        } else {
            /* stack clients are stacked horizontally */
            w = (ww - tx) / (n - i);
            c->resize(wx + tx, wy, w - (2 * c->bw),
                      wh - (2 * c->bw), 0);
            tx += WIDTH(c);
        }
}
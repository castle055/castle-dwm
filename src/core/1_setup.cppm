/*! \file  1_setup.cppm
 *! \brief 
 *!
 */

module;
#include <clocale>
#include <sys/select.h>
#include <sys/wait.h>

#include <X11/extensions/Xinerama.h>


export module cyd_wm.setup;

import fabric.logging;

import cyd_wm.xresources;
import cyd_wm.x11_utils;
import cyd_wm.state.init;
import cyd_wm.state.misc;
import cyd_wm.drw;

export namespace cyd_wm {
  void sigchld(int unused) {
    if (signal(SIGCHLD, sigchld) == SIG_ERR)
      die("can't install SIGCHLD handler:");
    while (0 < waitpid(-1, NULL, WNOHANG));
  }

  void setup() {
    ops::log::info("Setting up...");

    if (!setlocale(LC_CTYPE, "") || !XSupportsLocale()) {
      LOG::print {WARN}("No locale support");
    }

    x11::check_other_wm();
    state::scheme = (Clr**)ecalloc(state::config::colors.size() + 1, sizeof(Clr*));

    xresources::load_xresources();
    ops::file::reload_key_nav();

    XSetWindowAttributes wa;
    Atom utf8string;

    /* clean up any zombies immediately */
    sigchld(0);

    /* init screen */
    state::sw  = DisplayWidth(state::dpy, state::screen);
    state::sh  = DisplayHeight(state::dpy, state::screen);
    state::drw = drw_create(state::dpy, state::screen, state::root, state::sw, state::sh);
    if (!drw_fontset_create(state::drw, &state::config::fonts[0], state::config::fonts.size()))
      die("no fonts could be loaded.");
    state::lrpad      = state::drw->fonts->h;
    state::bar_height = state::drw->fonts->h + 10;
    x11::update_geometry();
    /* init atoms */
    ops::log::debug("[setup] dpy: %x, drw: %x", state::dpy, state::drw);
    ops::log::debug("[setup] Init intern atoms");
    utf8string = XInternAtom(state::dpy, "UTF8_STRING", False);

    state::init();

    /* init cursors */
    ops::log::debug("[setup] Init cursors");
    state::cursor[CurNormal] = drw_cur_create(state::drw, XC_left_ptr);
    state::cursor[CurResize] = drw_cur_create(state::drw, XC_sizing);
    state::cursor[CurMove]   = drw_cur_create(state::drw, XC_fleur);

    settheme();

    /* supporting window for NetWMCheck */
    state::wmcheckwin = XCreateSimpleWindow(state::dpy, state::root, 0, 0, 1, 1, 0, 0, 0);
    XChangeProperty(state::dpy, state::wmcheckwin, state::netatom[NetWMCheck], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char*)&state::wmcheckwin, 1);
    XChangeProperty(state::dpy, state::wmcheckwin, state::netatom[NetWMName], utf8string, 8,
                    PropModeReplace, (unsigned char*)"castle-dwm", 10);
    XChangeProperty(state::dpy, state::root, state::netatom[NetWMCheck], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char*)&state::wmcheckwin, 1);
    /* EWMH support per view */
    XChangeProperty(state::dpy, state::root, state::netatom[NetSupported], XA_ATOM, 32,
                    PropModeReplace, (unsigned char*)state::netatom, NetLast);
    XDeleteProperty(state::dpy, state::root, state::netatom[NetClientList]);
    /* select events */
    wa.cursor     = state::cursor[CurNormal]->cursor;
    wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask
                    | ButtonPressMask | PointerMotionMask | EnterWindowMask
                    | LeaveWindowMask | StructureNotifyMask | PropertyChangeMask;
    XChangeWindowAttributes(state::dpy, state::root, CWEventMask | CWCursor, &wa);
    XSelectInput(state::dpy, state::root, wa.event_mask);
    x11::grab_keys();
    ops::client::focus(nullptr);

    x11::scan();
    /* init bars */
    ops::bar::init_where_needed();
    ops::bar::update_all();

    ops::log::info("Done setting up.");
  }
}
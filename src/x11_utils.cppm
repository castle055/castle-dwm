/*! \file  x11_utils.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xatom.h>
#include <X11/Xproto.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xinerama.h>

export module cyd_wm.x11_utils;

import std;
import fabric.logging;
export import x11;


namespace cyd_wm::x11 {
  int is_unique_geom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info) {
    while (n--)
      if (unique[n].x_org == info->x_org && unique[n].y_org == info->y_org
          && unique[n].width == info->width && unique[n].height == info->height)
        return 0;
    return 1;
  }
  int (*xerrorxlib)(Display *, XErrorEvent *);
}

namespace cyd_wm::x11 {
/* There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's). Other types of errors call Xlibs
 * default error handler, which may call exit. */
  int x_error(Display* dpy, XErrorEvent* ee) {
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
    LOG::print {ERROR}("dwm: fatal error: request code={}, error code={}", ee->request_code, ee->error_code);
    return xerrorxlib(dpy, ee); /* may call exit */
  }

  int x_error_dummy(Display* dpy, XErrorEvent* ee) {
    return 0;
  }

/* Startup Error handler to check if another window manager
 * is already running. */
  int x_error_start(Display* dpy, XErrorEvent* ee) {
    LOG::print {ERROR}("dwm: another window manager is already running");
    std::exit(1);
    return -1;
  }
}

export namespace cyd_wm::x11 {
  void check_other_wm() {
    xerrorxlib = XSetErrorHandler(x_error_start);
    /* this causes an error if some other window manager is running */
    XSelectInput(connection::display(), connection::root(), SubstructureRedirectMask);
    XSync(connection::display(), False);
    XSetErrorHandler(x_error);
    XSync(connection::display(), False);
  }

  int get_root_ptr(int* x, int* y) {
    int di;
    unsigned int dui;
    Window dummy;

    return XQueryPointer(connection::display(), connection::root(), &dummy, &dummy, x, y, &di, &di, &dui);
  }

  long get_state(window_t w) {
    int format;
    long result      = -1;
    unsigned char* p = NULL;
    unsigned long n, extra;
    Atom real;

    if (XGetWindowProperty(connection::display(), w, state::wmatom[WMState], 0L, 2L, False, state::wmatom[WMState],
                           &real, &format, &n, &extra, (unsigned char**)&p) != Success)
      return -1;
    if (n != 0)
      result = *p;
    XFree(p);
    return result;
  }

  int get_text_prop(Window w, Atom atom, std::string &text) {
    //log::debug("[get_text_prop]");
    char** list = nullptr;
    int n;
    XTextProperty name;

    if (!XGetTextProperty(connection::display(), w, &name, atom) || !name.nitems) {
      log::debug("[get_text_prop] failed to get text prop for window [0x%X]", w);
      text.append("\0");
      return 0;
    }
    if (name.encoding == XA_STRING) {
      text.erase();
      text.append(reinterpret_cast<const char*>(name.value));
    } else {
      if (XmbTextPropertyToTextList(connection::display(), &name, &list, &n) >= Success && n > 0 && *list) {
        //log::debug("[get_text_prop] weird text list thing.");
        text = *list;
        XFreeStringList(list);
      }
    }
    text.append("\0");
    XFree(name.value);
    return 1;
  }

  void update_client_list() {
    client_t* c;
    monitor_t* m;

    XDeleteProperty(connection::display(), connection::root(), state::netatom[NetClientList]);
    for (m = state::mons; m; m = m->next)
      for (c = m->clients; c; c = c->next)
        XChangeProperty(connection::display(), connection::root(), state::netatom[NetClientList],
                        XA_WINDOW, 32, PropModeAppend,
                        (unsigned char*)&(c->win), 1);
  }

  int update_geometry() {
    //log::info("Updating geometry");
    int dirty = 0;

    if (XineramaIsActive(connection::display())) {
      log::info("Xinerama is active");
      int i, j, n, nn;
      client_t* c;
      monitor_t* m;
      XineramaScreenInfo* info   = XineramaQueryScreens(state::dpy, &nn);
      XineramaScreenInfo* unique = nullptr;

      for (n = 0, m = state::mons; m; m = m->next, n++);
      /* only consider unique geometries as separate screens */
      unique = (XineramaScreenInfo*)ecalloc(nn, sizeof(XineramaScreenInfo));
      for (i = 0, j = 0; i < nn; i++)
        if (is_unique_geom(unique, j, &info[i]))
          std::memcpy(&unique[j++], &info[i], sizeof(XineramaScreenInfo));
      XFree(info);
      nn = j;
      if (n <= nn) {
        /* new monitors available */
        for (i = 0; i < (nn - n); i++) {
          log::debug("[update_geom] adding monitor [%d]", i);
          for (m = state::mons; m && m->next; m = m->next);
          log::debug("[update_geom] found last monitor [0x%X]", m);
          if (m) {
            m->next = monitor::create_mon();
            log::debug("[update_geom] created monitor [0x%X] -> m->next", m->next);
            //monitor::load_default_layout(m->next);
          } else {
            state::mons = monitor::create_mon();
            log::debug("[update_geom] created monitor [0x%X] -> state::mons", state::mons);
            //monitor::load_default_layout(state::mons);
          }
        }
        log::debug("[update_geom] Fix dirty monitors");
        for (i = 0, m = state::mons; i < nn && m; m = m->next, i++)
          if (i >= n
              || unique[i].x_org != m->mx || unique[i].y_org != m->my
              || unique[i].width != m->mw || unique[i].height != m->mh) {
            dirty  = 1;
            m->num = i;
            m->mx  = m->wx = unique[i].x_org;
            m->my  = m->wy = unique[i].y_org;
            m->mw  = m->ww = unique[i].width;
            m->mh  = m->wh = unique[i].height;
            monitor::update_bar_pos(m);
            monitor::load_default_layout(m);
            log::debug("Created monitor [%d]", i);
//                    arrange(m);
          }
        log::debug("[update_geom] FIXED dirty monitors");
      } else {
        /* less monitors available nn < n */
        for (i = nn; i < n; i++) {
          for (m = state::mons; m && m->next; m = m->next);
          while ((c = m->clients)) {
            dirty      = 1;
            m->clients = c->next;
            client::detach_stack(c);
            c->mon = state::mons;
            client::attach(c);
            client::attach_stack(c);
          }
          if (m == state::selmon)
            state::selmon = state::mons;
          monitor::cleanup_mon(m);
        }
      }
      free(unique);
    } else {
      /* default monitor setup */
      log::info("Xinerama not active");
      if (!state::mons)
        state::mons = monitor::create_mon();
      if (state::mons->mw != state::sw || state::mons->mh != state::sh) {
        dirty           = 1;
        state::mons->mw = state::mons->ww = state::sw;
        state::mons->mh = state::mons->wh = state::sh;
        monitor::update_bar_pos(state::mons);
      }
    }
    if (dirty) {
      state::selmon = state::mons;
      state::selmon = monitor::win_to_mon(connection::root());
    }
    return dirty;
  }

  void update_numlock_mask() {
    unsigned int i, j;
    XModifierKeymap* modmap;

    state::numlockmask = 0;
    modmap             = XGetModifierMapping(connection::display());
    for (i = 0; i < 8; i++)
      for (j = 0; j < modmap->max_keypermod; j++)
        if (modmap->modifiermap[i * modmap->max_keypermod + j]
            == XKeysymToKeycode(connection::display(), XK_Num_Lock))
          state::numlockmask = (1 << i);
    XFreeModifiermap(modmap);
  }

//void x11::update_status() {
//  monitor_t *m;
//  //if (!x11::get_text_prop(state::root, XA_WM_NAME, state::stext))
//  //  state::stext = "dwm-VERSION";
//  bar::update_all();
//}

//Window create_window(
//    const char* name,
//    const char* clas,
//    int x,
//    int y,
//    int w,
//    int h
//) {
//  XSetWindowAttributes wa = {
//      .background_pixmap = ParentRelative,
//      .event_mask = ButtonPressMask | ExposureMask,
//      .override_redirect = True
//  };
//  XClassHint ch = {"dwm", "dwm"};
//  Window barwin = XCreateWindow(
//    state::dpy,
//    state::root,
//    x,
//    y,
//    w,
//    state::bar_height,
//    0,
//    DefaultDepth(state::dpy, state::screen),
//    CopyFromParent,
//    DefaultVisual(state::dpy, state::screen),
//    CWOverrideRedirect | CWBackPixmap | CWEventMask, &wa);
//  XDefineCursor(state::dpy, barwin, state::cursor[CurNormal]->cursor);
//  XMapRaised(state::dpy, barwin);
//  XSetClassHint(state::dpy, barwin, &ch);
//  return barwin;
//}
//
//Window x11::create_barwin(int x, int y, int w) {
//  return create_window("dwm", "bar", x, y, w, state::bar_height);
//}
//void x11::destroy_barwin(Window barwin) {
//  XDestroyWindow(state::dpy, barwin);
//}
  void grab_keys() {
    update_numlock_mask(); {
      unsigned int i, j;
      unsigned int modifiers[] = {0, LockMask, state::numlockmask, state::numlockmask | LockMask};
      KeyCode code;

      XUngrabKey(state::dpy, AnyKey, AnyModifier, connection::root());
      if ((code = XKeysymToKeycode(connection::display(), state::config::key_nav::trigger.keysym)))
        for (j = 0; j < LENGTH(modifiers); j++)
          XGrabKey(connection::display(), code, state::config::key_nav::trigger.mod | modifiers[j], connection::root(),
                   True, GrabModeAsync, GrabModeAsync);
      for (i = 0; i < state::config::keys.size(); i++)
        if ((code = XKeysymToKeycode(connection::display(), state::config::keys[i].keysym)))
          for (j = 0; j < LENGTH(modifiers); j++)
            XGrabKey(connection::display(), code, state::config::keys[i].mod | modifiers[j], connection::root(),
                     True, GrabModeAsync, GrabModeAsync);
    }
  }

  void scan() {
    unsigned int i, num;
    Window d1, d2,* wins = NULL;
    XWindowAttributes wa;

    if (XQueryTree(connection::display(), connection::root(), &d1, &d2, &wins, &num)) {
      for (i = 0; i < num; i++) {
        if (!XGetWindowAttributes(connection::display(), wins[i], &wa)
            || wa.override_redirect || XGetTransientForHint(connection::display(), wins[i], &d1))
          continue;
        if (wa.map_state == IsViewable || x11::get_state(wins[i]) == IconicState)
          client::manage(wins[i], &wa, 0);
      }
      for (i = 0; i < num; i++) {
        /* now the transients */
        if (!XGetWindowAttributes(connection::display(), wins[i], &wa))
          continue;
        if (XGetTransientForHint(connection::display(), wins[i], &d1)
            && (wa.map_state == IsViewable || x11::get_state(wins[i]) == IconicState))
          client::manage(wins[i], &wa, 0);
      }
      if (wins)
        XFree(wins);
    }
  }
}

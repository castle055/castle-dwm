
#include <clocale>
#include <sys/select.h>
#include <sys/wait.h>
#include <X11/cursorfont.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
//#include <program_shell.h>

#include <X11/extensions/Xinerama.h>


#include <fstream>

#include "castle-dwm.h"
#include "util.h"
#include "ops/log_ops.h"
#include "ops/file_ops.h"

/* function implementations */
//===== Main functions =====
void cleanup(void) {
  Arg a = {.ui = (unsigned int) ~0};
  Layout foo = {"", NULL};
  monitor_t *m;
  size_t i;
  
  ops::control::view(&a);
  state::selmon->lt[state::selmon->sellt] = &foo;
  for (m = state::mons; m; m = m->next)
    while (m->stack)
      ops::client::unmanage(m->stack, 0);
  XUngrabKey(state::dpy, AnyKey, AnyModifier, state::root);
  while (state::mons)
    ops::monitor::cleanup_mon(state::mons);
  for (i = 0; i < CurLast; i++)
    drw_cur_free(state::drw, state::cursor[i]);
  for (i = 0; i < config::colors.size() + 1; i++)
    free(state::scheme[i]);
  XDestroyWindow(state::dpy, state::wmcheckwin);
  drw_free(state::drw);
  XSync(state::dpy, False);
  XSetInputFocus(state::dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
  XDeleteProperty(state::dpy, state::root, state::netatom[NetActiveWindow]);
}
/* there are some broken focus acquiring clients needing extra handling */
Bool evpredicate() {
  return True;
}

void run() {
  ops::log::debug("Entering EVENT loop...");
  XEvent ev;
  /* main event loop */
  XSync(state::dpy, False);
  while (state::running) {
    while (XCheckIfEvent(state::dpy, &ev, (int (*)(Display *, XEvent *, XPointer)) evpredicate, NULL)) {
      if (ops::event::handler[ev.type]) {
        ops::log::debug("EVENT: %d", ev.type);
        ops::event::handler[ev.type](&ev);
      }
    } /* call handler */
  }
}

//void
//setgaps(const Arg *arg)
//{
//	if ((arg->i == 0) || (state::selmon->gappx + arg->i < 0))
//		state::selmon->gappx = 0;
//	else
//		state::selmon->gappx += arg->i;
//	arrange(state::selmon);
//}
void settheme() {
  /* init appearance */
  state::scheme[config::colors.size()] = drw_scm_create(state::drw, (char**)&(config::colors[0][0]), 3);
  for (int i = 0; i < config::colors.size(); i++)
    state::scheme[i] = drw_scm_create(state::drw, (char**)&(config::colors[i][0]), 3);
}
void setup() {
  ops::log::info("Setting up...");
  XSetWindowAttributes wa;
  Atom utf8string;
  
  /* clean up any zombies immediately */
  sigchld(0);
  
  /* init screen */
  state::screen = DefaultScreen(state::dpy);
  state::sw = DisplayWidth(state::dpy, state::screen);
  state::sh = DisplayHeight(state::dpy, state::screen);
  state::root = RootWindow(state::dpy, state::screen);
  state::drw = drw_create(state::dpy, state::screen, state::root, state::sw, state::sh);
  if (!drw_fontset_create(state::drw, &config::fonts[0], config::fonts.size()))
    die("no fonts could be loaded.");
  state::lrpad = state::drw->fonts->h;
  state::bar_height = state::drw->fonts->h + 10;
  ops::x11::update_geometry();
  /* init atoms */
  ops::log::debug("[setup] dpy: %x, drw: %x", state::dpy, state::drw);
  ops::log::debug("[setup] Init intern atoms");
  utf8string =                XInternAtom(state::dpy, "UTF8_STRING", False);
  state::wmatom[WMProtocols] =       XInternAtom(state::dpy, "WM_PROTOCOLS", False);
  state::wmatom[WMDelete] =          XInternAtom(state::dpy, "WM_DELETE_WINDOW", False);
  state::wmatom[WMState] =           XInternAtom(state::dpy, "WM_STATE", False);
  state::wmatom[WMTakeFocus] =       XInternAtom(state::dpy, "WM_TAKE_FOCUS", False);
  state::netatom[NetActiveWindow] =  XInternAtom(state::dpy, "_NET_ACTIVE_WINDOW", False);
  state::netatom[NetSupported] =     XInternAtom(state::dpy, "_NET_SUPPORTED", False);
  state::netatom[NetWMName] =        XInternAtom(state::dpy, "_NET_WM_NAME", False);
  state::netatom[NetWMState] =       XInternAtom(state::dpy, "_NET_WM_STATE", False);
  state::netatom[NetWMCheck] =       XInternAtom(state::dpy, "_NET_SUPPORTING_WM_CHECK", False);
  state::netatom[NetWMFullscreen] =  XInternAtom(state::dpy, "_NET_WM_STATE_FULLSCREEN", False);
  state::netatom[NetWMWindowType] =  XInternAtom(state::dpy, "_NET_WM_WINDOW_TYPE", False);
  state::netatom[NetWMWindowTypeDialog] = XInternAtom(state::dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
  state::netatom[NetClientList] =    XInternAtom(state::dpy, "_NET_CLIENT_LIST", False);
  /* init cursors */
  ops::log::debug("[setup] Init cursors");
  state::cursor[CurNormal] = drw_cur_create(state::drw, XC_left_ptr);
  state::cursor[CurResize] = drw_cur_create(state::drw, XC_sizing);
  state::cursor[CurMove] = drw_cur_create(state::drw, XC_fleur);
  
  settheme();
  
  /* init bars */
  ops::bar::init_where_needed();
  ops::x11::update_status();
  /* supporting window for NetWMCheck */
  state::wmcheckwin = XCreateSimpleWindow(state::dpy, state::root, 0, 0, 1, 1, 0, 0, 0);
  XChangeProperty(state::dpy, state::wmcheckwin, state::netatom[NetWMCheck], XA_WINDOW, 32,
                  PropModeReplace, (unsigned char *) &state::wmcheckwin, 1);
  XChangeProperty(state::dpy, state::wmcheckwin, state::netatom[NetWMName], utf8string, 8,
                  PropModeReplace, (unsigned char *) "dwm", 3);
  XChangeProperty(state::dpy, state::root, state::netatom[NetWMCheck], XA_WINDOW, 32,
                  PropModeReplace, (unsigned char *) &state::wmcheckwin, 1);
  /* EWMH support per view */
  XChangeProperty(state::dpy, state::root, state::netatom[NetSupported], XA_ATOM, 32,
                  PropModeReplace, (unsigned char *) state::netatom, NetLast);
  XDeleteProperty(state::dpy, state::root, state::netatom[NetClientList]);
  /* select events */
  wa.cursor = state::cursor[CurNormal]->cursor;
  wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask
                  | ButtonPressMask | PointerMotionMask | EnterWindowMask
                  | LeaveWindowMask | StructureNotifyMask | PropertyChangeMask;
  XChangeWindowAttributes(state::dpy, state::root, CWEventMask | CWCursor, &wa);
  XSelectInput(state::dpy, state::root, wa.event_mask);
  ops::x11::grab_keys();
  ops::client::focus(nullptr);
  ops::log::info("Done setting up.");
}
void sigchld(int unused) {
  if (signal(SIGCHLD, sigchld) == SIG_ERR)
    die("can't install SIGCHLD handler:");
  while (0 < waitpid(-1, NULL, WNOHANG));
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
  
  display = XOpenDisplay(nullptr);
  resm = XResourceManagerString(display);
  if (!resm)
    return;
  
  db = XrmGetStringDatabase(resm);
  for (const auto &p : state::config::resources)
    resource_load(db, p.name, p.type, p.dst);
  XCloseDisplay(display);
  settheme();
}

//int print_tree(terminal::term_stream& stream, int argc, const char* argv[]) {
  //int mi = 0;
  //for(monitor_t* m = state::mons; m; m = m->next, mi++) {
    //if (m == state::selmon) stream << "SELECTED ";
    //stream << "monitor_t " << std::to_string(mi) << ": bar: " << std::to_string(m->showbar) << "\n\r";
    //for (client_t* c = m->stack; c; c = c->snext) {
      //if (c == m->sel) stream << "* ";
      //stream << "\tStack Client " << c->name << "\n\r";
    //}
    //stream << "\n\r";
    //for (client_t* c = m->clients; c; c = c->next) {
      //if (c == m->sel) stream << "* ";
      //stream << "\tClient " << c->name << "\n\r";
    //}
  //}
  //return 0;
//}

//int restart(terminal::term_stream& stream, int argc, const char* argv[]) {
  //program_shell::stop();
  //ops::control::quit(nullptr);
  //return 0;
//}

int main(int argc, char *argv[]) {
  state::log_file.open(state::config::log_file.c_str(), std::ios::app);
  ops::file::reload_key_nav();
  
  ops::log::info("Starting CDWM...");
  //program_shell::set_var("TERM_HEADER", "[C-DWM Shell v0.1]");
  //program_shell::set_var("TERM_PROMPT", "$ > ");
  //program_shell::add_cmd("print_tree", print_tree);
  //program_shell::add_cmd("restart", restart);
  //int err = program_shell::init(config::ctl_port);
  //if (err <0) {
  //  fprintf(stderr, "[program_shell::init] err: %d", err);
  //}
  
  if (argc == 2 && !strcmp("-v", argv[1]))
    die("dwm-VERSION");
  else if (argc != 1)
    die("usage: dwm [-v]");
  
  if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
    fputs("warning: no locale support\n", stderr);
  if (!(state::dpy = XOpenDisplay(nullptr)))
    die("dwm: cannot open display");
  ops::x11::check_other_wm();
  XrmInitialize();
  state::scheme = (Clr **) ecalloc(config::colors.size() + 1, sizeof(Clr *));
  load_xresources();
  
  setup();
#ifdef __OpenBSD__
  if (pledge("stdio rpastd::iosproc exec", NULL) == -1)
        die("pledge");
#endif /* __OpenBSD__ */
  ops::x11::scan();
  run();
  cleanup();
  XCloseDisplay(state::dpy);
  return EXIT_SUCCESS;
}

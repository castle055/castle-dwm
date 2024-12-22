/*! \file  api.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xlib.h>
#include <X11/Xutil.h>

export module x11.api;

import std;
import fabric.logging;
export import x11.connection;

namespace {
  int (*xerrorxlib)(Display *, XErrorEvent *);

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
}

export namespace x11 {

 void free(void* data) {
  XFree(data);
 }

 void sync(bool discard_events = false) {
  XSync(connection::display(), discard_events? True: False);
 }

 Status send_event(window_t win, const bool propagate, const long event_mask, event_t* ev) {
  return XSendEvent(connection::display(), win, propagate? True: False, event_mask, ev);
 }

 Status set_wm_protocols(window_t win, atom_t* protocols, int count) {
  return XSetWMProtocols(connection::display(), win, protocols, count);
 }
 Status get_wm_protocols(window_t win, atom_t** protocols, int* count) {
  return XGetWMProtocols(connection::display(), win, protocols, count);
 }

 int change_property(window_t win, atom_t property, atom_t type, int format, int mode, _Xconst unsigned char* data, int nelements) {
  return XChangeProperty(connection::display(), win, property, type, format, mode, data, nelements);
 }

 int delete_property(window_t win, atom_t property) {
  return XDeleteProperty(connection::display(), win, property);
 }

 int get_window_property(
  window_t win,
  atom_t property,
  long long_offset,
  long long_length,
  bool delete_,
  atom_t req_type,
  atom_t* actual_type_return,
  int* actual_format_return,
  unsigned long* nitems_return,
  unsigned long* bytes_after_return,
  unsigned char** prop_return
 ) {
  return XGetWindowProperty(
   connection::display(),
   win,
   property,
   long_offset,
   long_length,
   delete_? True: False,
   req_type,
   actual_type_return,
   actual_format_return,
   nitems_return,
   bytes_after_return,
   prop_return
  );
 }

 int configure_window(window_t win, unsigned value_mask, window_changes_t* changes) {
  return XConfigureWindow(connection::display(), win, value_mask, changes);
 }
 int move_window(window_t win, unsigned x, unsigned y) {
  return XMoveWindow(connection::display(), win, x, y);
 }
 int resize_window(window_t win, unsigned width, unsigned height) {
  return XResizeWindow(connection::display(), win, width, height);
 }
 int move_resize_window(window_t win, unsigned x, unsigned y, unsigned width, unsigned height) {
  return XMoveResizeWindow(connection::display(), win, x, y, width, height);
 }
 int set_window_border_width(window_t win, unsigned width) {
  return XSetWindowBorderWidth(connection::display(), win, width);
 }

 int set_input_focus(window_t win, int revert_to, Time time) {
  return XSetInputFocus(connection::display(), win, revert_to, time);
 }
 int get_input_focus(window_t* win_return, int* revert_to_return) {
  return XGetInputFocus(connection::display(), win_return, revert_to_return);
 }

 int raise_window(window_t win) {
  return XRaiseWindow(connection::display(), win);
 }
 int lower_window(window_t win) {
  return XLowerWindow(connection::display(), win);
 }
 int circulate_subwindows_up(window_t win) {
  return XCirculateSubwindowsUp(connection::display(), win);
 }
 int circulate_subwindows_down(window_t win) {
  return XCirculateSubwindowsDown(connection::display(), win);
 }
 int circulate_subwindows(window_t win, int direction) {
  return XCirculateSubwindows(connection::display(), win, direction);
 }
 int restack_windows(window_t windows[], int nwindows) {
  return XRestackWindows(connection::display(), windows, nwindows);
 }

 atom_t intern_atom(_Xconst char* atom_name, bool only_if_exists) {
  return XInternAtom(connection::display(), atom_name, only_if_exists? True: False);
 }
 Status intern_atoms(char** names, int count, bool only_if_exists, atom_t* atoms_return) {
  return XInternAtoms(connection::display(), names, count, only_if_exists? True: False, atoms_return);
 }

 char* get_atom_name(atom_t atom) {
  return XGetAtomName(connection::display(), atom);
 }
 Status get_atom_names(atom_t* atoms, int count, char** names_return) {
  return XGetAtomNames(connection::display(), atoms, count, names_return);
 }

 XClassHint* alloc_class_hint() {
  return XAllocClassHint();
 }
 int set_class_hint(window_t win, XClassHint* hint) {
  return XSetClassHint(connection::display(), win, hint);
 }
 Status get_class_hint(window_t win, XClassHint* hint_return) {
  return XGetClassHint(connection::display(), win, hint_return);
 }

 int set_window_border(window_t win, unsigned long border_pixel) {
  return XSetWindowBorder(connection::display(), win, border_pixel);
 }

 int grab_button(unsigned int button, unsigned int modifiers, window_t grab_window, bool owner_events, unsigned int event_mask, int pointer_mode, int keyboard_mode, window_t confine_to, cursor_t cursor) {
  return XGrabButton(connection::display(), button, modifiers, grab_window, owner_events? True: False, event_mask, pointer_mode, keyboard_mode, confine_to, cursor);
 }
 int ungrab_button(unsigned int button, unsigned int modifiers, window_t grab_window) {
  return XUngrabButton(connection::display(), button, modifiers, grab_window);
 }

 size_hints_t* alloc_size_hints() {
  return XAllocSizeHints();
 }
 void set_wm_normal_hints(window_t win, size_hints_t* hints) {
  return XSetWMNormalHints(connection::display(), win, hints);
 }
 Status get_wm_normal_hints(window_t win, size_hints_t* hints_return, long* supplied_return) {
  return XGetWMNormalHints(connection::display(), win, hints_return, supplied_return);
 }
 void set_wm_size_hints(window_t win, size_hints_t* hints, atom_t property) {
  return XSetWMSizeHints(connection::display(), win, hints, property);
 }
 Status get_wm_size_hints(window_t win, size_hints_t* hints_return, long* supplied_return, atom_t property) {
  return XGetWMSizeHints(connection::display(), win, hints_return, supplied_return, property);
 }

 wm_hints_t* alloc_wm_hints() {
  return XAllocWMHints();
 }
 int set_wm_hints(window_t win, wm_hints_t* hints) {
  return XSetWMHints(connection::display(), win, hints);
 }
 wm_hints_t* get_wm_hints(window_t win) {
  return XGetWMHints(connection::display(), win);
 }

 struct scope_server_lock {
  scope_server_lock() {
   XGrabServer(connection::display());
  }
  ~scope_server_lock() {
   XUngrabServer(connection::display());
  }

  scope_server_lock(scope_server_lock&&) = delete;
  scope_server_lock& operator=(scope_server_lock&&) = delete;
  scope_server_lock(const scope_server_lock&) = delete;
  scope_server_lock& operator=(const scope_server_lock&) = delete;
 };

 void with_server(const std::function<void()>& block) {
  XGrabServer(connection::display());
  block();
  XUngrabServer(connection::display());
 }

 struct scope_error_disabler {
  scope_error_disabler() {
   XSetErrorHandler(x_error_dummy);
  }
  ~scope_error_disabler() {
   XSetErrorHandler(x_error);
  }

  scope_error_disabler(scope_error_disabler&&) = delete;
  scope_error_disabler& operator=(scope_error_disabler&&) = delete;
  scope_error_disabler(const scope_error_disabler&) = delete;
  scope_error_disabler& operator=(const scope_error_disabler&) = delete;
 };

 void with_no_errors(const std::function<void()>& block) {
  XSetErrorHandler(x_error_dummy);
  block();
  XSetErrorHandler(x_error);
 }

 int allow_events(int event_mode, Time time) {
  return XAllowEvents(connection::display(), event_mode, time);
 }

 keysym_t lookup_keysym(XKeyEvent* key_event, int index) {
  return XLookupKeysym(key_event, index);
 }

 Status get_window_attributes(window_t win, window_attrs_t* window_attrs_return) {
  return XGetWindowAttributes(connection::display(), win, window_attrs_return);
 }

 int set_transient_for_hint(window_t win, window_t prop_window) {
  return XSetTransientForHint(connection::display(), win, prop_window);
 }
 Status get_transient_for_hint(window_t win, window_t* prop_window_return) {
  return XGetTransientForHint(connection::display(), win, prop_window_return);
 }

 int if_event(event_t* event_return, int(*predicate)(display_t, event_t*, ptr_t), ptr_t arg) {
  return XIfEvent(connection::display(), event_return, predicate, arg);
 }

 bool check_if_event(event_t* event_return, int(*predicate)(display_t, event_t*, ptr_t), ptr_t arg) {
  return XCheckIfEvent(connection::display(), event_return, predicate, arg) == True;
 }

 int peek_if_event(event_t* event_return, int(*predicate)(display_t, event_t*, ptr_t), ptr_t arg) {
  return XPeekIfEvent(connection::display(), event_return, predicate, arg);
 }

 enum class close_mode {
  DESTROY_ALL,
  RETAIN_PEMANENT,
  RETAIN_TEMPORARY,
 };
 int set_close_down_mode(close_mode close_mode_) {
  switch (close_mode_) {
   case close_mode::DESTROY_ALL:
    return XSetCloseDownMode(connection::display(), DestroyAll);
   case close_mode::RETAIN_PEMANENT:
    return XSetCloseDownMode(connection::display(), RetainPermanent);
   case close_mode::RETAIN_TEMPORARY:
    return XSetCloseDownMode(connection::display(), RetainTemporary);
   default:
    return -1;
  }
 }
 int kill_client(XID resource) {
  return XKillClient(connection::display(), resource);
 }
}

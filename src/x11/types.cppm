/*! \file  types.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xlib.h>
#include <X11/Xutil.h>

export module x11.types;

export {
}

export namespace x11 {
 using display_t = Display*;
 using screen_t  = int;
 using window_t  = Window;
 using keysym_t  = KeySym;
 using event_t   = XEvent;
 using atom_t    = Atom;
 using cursor_t  = Cursor;
 using ptr_t     = XPointer;

 using window_changes_t = XWindowChanges;
 using window_attrs_t   = XWindowAttributes;
 using wm_hints_t       = XWMHints;
 using size_hints_t     = XSizeHints;

 using ::XClassHint;

 // Events
 struct configure_event_t: XConfigureEvent {
  explicit configure_event_t(display_t dpy) : XConfigureEvent() {
   this->type = ConfigureNotify;
   this->display = dpy;
  }
 };

 using ::XClientMessageEvent;
 using ::XDestroyWindowEvent;
 using ::XConfigureEvent;
 using ::XButtonPressedEvent;
 using ::XCrossingEvent;
 using ::XKeyEvent;
 using ::XMappingEvent;
 using ::XMapRequestEvent;
 using ::XMotionEvent;
 using ::XPropertyEvent;
}


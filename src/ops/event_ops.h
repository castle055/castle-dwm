//
// Created by castle on 3/24/22.
//

#ifndef CASTLE_DWM_EVENT_OPS_H
#define CASTLE_DWM_EVENT_OPS_H

#include "X11/X.h"
#include "X11/Xlib.h"

namespace ops::event {
  namespace handlers {
    void buttonpress(XEvent *e);
    void clientmessage(XEvent *e);
    void configurenotify(XEvent *e);
    void configurerequest(XEvent *e);
    void destroynotify(XEvent *e);
    void enternotify(XEvent *e);
    void expose(XEvent *e);
    void focusin(XEvent *e);
    void keypress(XEvent *e);
    void mappingnotify(XEvent *e);
    void maprequest(XEvent *e);
    void motionnotify(XEvent *e);
    void propertynotify(XEvent *e);
    void unmapnotify(XEvent *e);
  }
  
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
  extern void (*handler[LASTEvent])(XEvent *);
};


#endif //CASTLE_DWM_EVENT_OPS_H

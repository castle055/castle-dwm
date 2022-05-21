#include <X11/Xatom.h>
#include <X11/Xresource.h>

#include "ops/client_ops.h"
#include "ops/monitor_ops.h"
#include "ops/event_ops.h"
#include "ops/x11_ops.h"
#include "ops/control_ops.h"
#include "ops/bar_ops.h"
#include "state/state.h"

/* function declarations */
extern void cleanup();
extern void run();
extern void settheme();    // This is mine for theme on the fly
extern void setup();
extern void sigchld(int unused);
extern void load_xresources();
extern void resource_load(XrmDatabase db, char *name, enum resource_type rtype, void *dst);

#include "../config.h"

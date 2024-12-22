/*! \file  xresources.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xlib.h>
#include <X11/Xresource.h>
export module cyd_wm.xresources;

import std;
import cyd_wm.state.config;
import x11;

namespace {
  void init_if_needed() {
    static bool initialized = false;
    if (!initialized) {
      XrmInitialize();
      initialized = true;
    }
  }

  void resource_load(XrmDatabase db, char* name, enum resource_type rtype, void* dst) {
    char* sdst  = nullptr;
    int* idst   = nullptr;
    float* fdst = nullptr;

    sdst = (char*)dst;
    idst = (int*)dst;
    fdst = (float*)dst;

    char fullname[256];
    char* type;
    XrmValue ret;

    std::snprintf(fullname, sizeof(fullname), "%s.%s", "dwm", name);
    fullname[sizeof(fullname) - 1] = '\0';

    XrmGetResource(db, fullname, "*", &type, &ret);
    if (!(ret.addr == nullptr || std::strncmp("String", type, 64))) {
      switch (rtype) {
        case STRING:
          std::strcpy(sdst, ret.addr);
          break;
        case INTEGER:
          *idst = std::strtoul(ret.addr, nullptr, 10);
          break;
        case FLOAT:
          *fdst = std::strtof(ret.addr, nullptr);
          break;
      }
    }
  }
}

export namespace cyd_wm::xresources {
  void load_xresources() {
    init_if_needed();

    char* resm;
    XrmDatabase db;

    resm = XResourceManagerString(x11::connection::display());
    if (!resm)
      return;

    db = XrmGetStringDatabase(resm);
    for (const auto &p: state::config::resources)
      resource_load(db, p.name, p.type, p.dst);
    settheme();
  }
}
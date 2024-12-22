/*! \file  _event_handlers.cppm
 *! \brief 
 *!
 */

export module cyd_wm.events;

import x11;

import cyd_wm.events.handlers.buttonpress;
import cyd_wm.events.handlers.clientmessage;
import cyd_wm.events.handlers.configurenotify;
import cyd_wm.events.handlers.configurerequest;
import cyd_wm.events.handlers.destroynotify;
import cyd_wm.events.handlers.enternotify;
import cyd_wm.events.handlers.expose;
import cyd_wm.events.handlers.focusin;
import cyd_wm.events.handlers.keypess;
import cyd_wm.events.handlers.mappingnotify;
import cyd_wm.events.handlers.maprequest;
import cyd_wm.events.handlers.motionnotify;
import cyd_wm.events.handlers.propertynotify;
import cyd_wm.events.handlers.unmapnotify;

namespace events {
  void (* handlers[])(x11::event_t*) = {
    nullptr,
    nullptr,
    event_handlers::keypress,
    nullptr, // TODO - this one is keyrelease
    event_handlers::buttonpress,
    nullptr,
    event_handlers::motionnotify,
    event_handlers::enternotify,
    nullptr,
    event_handlers::focusin,
    nullptr, nullptr,
    event_handlers::expose,
    nullptr, nullptr, nullptr, nullptr,
    event_handlers::destroynotify,
    event_handlers::unmapnotify,
    nullptr,
    event_handlers::maprequest,
    nullptr,
    event_handlers::configurenotify,
    event_handlers::configurerequest,
    nullptr, nullptr, nullptr, nullptr,
    event_handlers::propertynotify,
    nullptr, nullptr, nullptr, nullptr,
    event_handlers::clientmessage,
    event_handlers::mappingnotify
  };

}

export namespace events {
  void dispatch(x11::event_t *event) {
    if (handlers[event->type]) {
      //ops::log::debug("EVENT: %d", ev.type);
      handlers[event->type](event);
    }
  }
}

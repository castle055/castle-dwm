/*! \file  2_main_loop.cppm
 *! \brief 
 *!
 */

export module cyd_wm.main_loop;

import std;

import cyd_wm.state.misc;
import x11.api;

import cyd_wm.events;

int eevpredicate(x11::display_t, x11::event_t*, x11::ptr_t) {
  return true;
}

export namespace cyd_wm::main_loop {
  void run() {
    x11::event_t ev;
    /* main event loop */
    x11::sync(false);
    while (state::running) {
      while (x11::check_if_event(&ev, eevpredicate, nullptr)) {
        events::dispatch(&ev);
      } /* call handler */

      using namespace std::chrono_literals;
      std::this_thread::sleep_for(8ms);
    }
  }

}
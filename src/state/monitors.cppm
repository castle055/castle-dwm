/*! \file  monitors.cppm
 *! \brief 
 *!
 */

export module cyd_wm.state.monitors;

export import cyd_wm.types;

export namespace state::monitors {
 std::deque<cyd_wm::Monitor::sptr> monitors{};

 cyd_wm::Monitor::sptr selected_monitor{};
}
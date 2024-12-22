/*! \file  _init.cppm
 *! \brief 
 *!
 */

export module cyd_wm.state.init;

import cyd_wm.state.atoms;

import x11.api;

export namespace state {
 void init() {
  /* init atoms */
  atoms::wmatom[WMProtocols]            = x11::intern_atom("WM_PROTOCOLS", false);
  atoms::wmatom[WMDelete]               = x11::intern_atom("WM_DELETE_WINDOW", false);
  atoms::wmatom[WMState]                = x11::intern_atom("WM_STATE", false);
  atoms::wmatom[WMTakeFocus]            = x11::intern_atom("WM_TAKE_FOCUS", false);
  atoms::netatom[NetActiveWindow]       = x11::intern_atom("_NET_ACTIVE_WINDOW", false);
  atoms::netatom[NetSupported]          = x11::intern_atom("_NET_SUPPORTED", false);
  atoms::netatom[NetWMName]             = x11::intern_atom("_NET_WM_NAME", false);
  atoms::netatom[NetWMState]            = x11::intern_atom("_NET_WM_STATE", false);
  atoms::netatom[NetWMCheck]            = x11::intern_atom("_NET_SUPPORTING_WM_CHECK", false);
  atoms::netatom[NetWMFullscreen]       = x11::intern_atom("_NET_WM_STATE_FULLSCREEN", false);
  atoms::netatom[NetWMWindowType]       = x11::intern_atom("_NET_WM_WINDOW_TYPE", false);
  atoms::netatom[NetWMWindowTypeDialog] = x11::intern_atom("_NET_WM_WINDOW_TYPE_DIALOG", false);
  atoms::netatom[NetClientList]         = x11::intern_atom("_NET_CLIENT_LIST", false);
 }
}
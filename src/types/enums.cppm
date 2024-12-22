/*! \file  enums.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:enums;

export
{
  enum {
    CurNormal, CurResize, CurMove, CurLast
  }; /* cursor */
  enum {
    SchemeNorm, SchemeSel
  }; /* color schemes */
  enum {
    NetSupported, NetWMName, NetWMState, NetWMCheck,
    NetWMFullscreen, NetActiveWindow, NetWMWindowType,
    NetWMWindowTypeDialog, NetClientList, NetLast
  }; /* EWMH atoms */
  enum {
    WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast
  }; /* default atoms */
  enum {
    ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
    ClkClientWin, ClkRootWin, ClkLast
  }; /* clicks */
  enum resource_type {
    STRING  = 0,
    INTEGER = 1,
    FLOAT   = 2
  }; /* Xresources preferences */
}
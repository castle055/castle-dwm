/*! \file  layout.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:layout;

import std;

export namespace cyd_wm {
 class Monitor;

 class Layout {
 public:
  std::string symbol{};
  std::function<void(std::shared_ptr<Monitor>)> arrange{};
 };

 struct DefaultLayout {
   int ltidx = -1;
   int nmaster = 1;
 };
}

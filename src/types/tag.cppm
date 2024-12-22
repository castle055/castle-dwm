/*! \file  tag.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:tag;

export namespace cyd_wm {
 class TagConfiguration {
 public:
  int nmasters { };        /* number of windows in master area */
  float mfacts { };        /* mfacts per tag */
  unsigned int sellts { }; /* selected layouts */
  int showbars { };        /* display bar for the current tag */
 };

 template <unsigned int N>
 class TagSelection {
 public:
 private:
  bool selected_[N] { };
 };
}

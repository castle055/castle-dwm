/*! \file  rule.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:rule;

export namespace cyd_wm {
  struct Rule {
    const char* class_;
    const char* instance;
    const char* title;
    unsigned int tags;
    int isfloating;
    int monitor;
    bool ignoretransient;
  };
}
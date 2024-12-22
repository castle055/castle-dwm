/*! \file  arg.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:arg;

export namespace cyd_wm {
  union Arg {
    int i;
    unsigned int ui;
    float f;
    const void *v;
  };
}
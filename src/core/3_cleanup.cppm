/*! \file  3_cleanup.cppm
 *! \brief 
 *!
 */

export module cyd_wm.cleanup;


export namespace cyd_wm {
  void cleanup(void) {
    Arg a      = {.ui = (unsigned int)~0};
    Layout foo = {"", NULL};
    monitor_t* m;
    size_t i;

    ops::control::view(&a);
    state::selmon->lt[state::selmon->sellt] = &foo;
    for (m = state::mons; m; m = m->next)
      while (m->stack)
        ops::client::unmanage(m->stack, 0);
    XUngrabKey(state::dpy, AnyKey, AnyModifier, state::root);
    while (state::mons)
      ops::monitor::cleanup_mon(state::mons);
    for (i = 0; i < CurLast; i++)
      drw_cur_free(state::drw, state::cursor[i]);
    for (i = 0; i < state::config::colors.size() + 1; i++)
      free(state::scheme[i]);
    XDestroyWindow(state::dpy, state::wmcheckwin);
    drw_free(state::drw);
    XSync(state::dpy, False);
    XSetInputFocus(state::dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    XDeleteProperty(state::dpy, state::root, state::netatom[NetActiveWindow]);

    XCloseDisplay(state::dpy);
  }
}
/*! \file  rules.cppm
 *! \brief 
 *!
 */

export module cyd_wm.clients.rules;

import std;
import fabric.logging;

import x11.api;
import cyd_wm.types;

export namespace cyd_wm::clients {
 void apply_rules_to(Client::sptr client) {
   unsigned int i;
   const Rule* r;
   Monitor::sptr m;
   x11::XClassHint ch = {nullptr, nullptr};

   /* rule matching */
   client->isfloating = 0;
   client->tags       = 0;
   x11::get_class_hint(client->win, &ch);
   const char* class_   = ch.res_class ? ch.res_class : "broken";
   const char* instance = ch.res_name ? ch.res_name : "broken";
   LOG::print {INFO}("class {} - instance {}", class_, instance)

   for (i = 0; i < state::config::rules.size(); i++) {
     r = &state::config::rules[i];
     if ((!r->title || client->name.find(r->title) != std::string::npos)
         && (!r->class_ || std::strstr(class_, r->class_))
         && (!r->instance || std::strstr(instance, r->instance))) {
       client->isfloating      = r->isfloating;
       client->ignoretransient = r->ignoretransient;
       client->tags |= r->tags;
       if (m)
         client->mon = m;
     }
   }
   if (ch.res_class)
     x11::free(ch.res_class);
   if (ch.res_name)
     x11::free(ch.res_name);
   client->tags = client->tags & TAGMASK ? client->tags & TAGMASK : client->mon->tagset[client->mon->seltags];
 }
}
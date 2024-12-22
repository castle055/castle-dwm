/*! \file  client.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:client;

import std;
export import x11.types;

export namespace cyd_wm {
 class Monitor;

 class Client {
 public:
   using sptr = std::shared_ptr<Client>;

   int width() const {
     return w + (2 * bw);
   }

   int height() const {
     return h + (2 * bw);
   }

   std::string name             = "";
   float mina                   = 0.0f, maxa = 0.0f;
   int x                        = 0, y       = 0, w    = 0, h    = 0;
   int oldx                     = 0, oldy    = 0, oldw = 0, oldh = 0;
   int basew                    = 0, baseh   = 0, incw = 0, inch = 0, maxw = 0, maxh = 0, minw = 0, minh = 0;
   int bw                       = 0, oldbw   = 0;
   unsigned int tags            = 0;
   int isfixed                  = 0, isfloating = 0, isurgent = 0, neverfocus = 0, oldstate = 0, isfullscreen = 0;
   bool ignoretransient         = false;
   Client* next                 = nullptr;
   Client* snext                = nullptr;
   std::shared_ptr<Monitor> mon = nullptr;
   x11::window_t win;
 };
}

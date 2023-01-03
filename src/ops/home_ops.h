//
// Created by castle on 7/31/22.
//

#ifndef CASTLE_DWM_HOME_OPS_H
#define CASTLE_DWM_HOME_OPS_H


namespace ops::home {
  
  void init();
  void destroy();
  
  

  namespace control {
    void show();
    void hide();
    void moveTo(int monId);
  }
}


#endif //CASTLE_DWM_HOME_OPS_H

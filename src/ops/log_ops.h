//
// Created by castle on 4/1/22.
//

#ifndef CASTLE_DWM_LOG_OPS_H
#define CASTLE_DWM_LOG_OPS_H


namespace ops::log {
  void debug  (const char* format, ...);
  void info   (const char* format, ...);
  void warn   (const char* format, ...);
  void error  (const char* format, ...);
}


#endif //CASTLE_DWM_LOG_OPS_H

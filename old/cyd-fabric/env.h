//
// Created by castle on 4/9/24.
//

#ifndef ENV_H
#define ENV_H

#include <string>

namespace env {
  inline std::string get(const std::string &name) {
    return std::getenv(name.c_str());
  }

#define ENV(NAME, ENV, ...)       \
  inline std::string NAME() {     \
    char* s = std::getenv(#ENV);  \
    return s == nullptr           \
            ? ( __VA_ARGS__ )     \
            : std::string{s};     \
  }

  //! Common Environment Variables

  ENV(USER, USER, "")
  ENV(PWD, PWD, ".")
  ENV(HOME, HOME, std::string{"/home/"} + USER())
}

#endif //ENV_H

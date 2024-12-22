//
// Created by castle on 4/9/24.
//

#ifndef CONFIG_ERRORS_H
#define CONFIG_ERRORS_H

#include <expected>
#include <string>

namespace config {
  struct error_t;

  template<typename R>
  using result_t = std::expected<R, error_t>;

  struct error_t {
    const char* msg = "";

    // ReSharper disable once CppNonExplicitConversionOperator
    template<typename R>
    operator result_t<R>() { // NOLINT(*-explicit-constructor)
      return std::unexpected {*this};
    }
  };
}

#endif //CONFIG_ERRORS_H

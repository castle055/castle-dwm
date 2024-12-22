//
// Created by castle on 4/9/24.
//

#ifndef INCLUDER_H
#define INCLUDER_H

#include <hocon/config_include_context.hpp>
#include <hocon/config_includer.hpp>
#include <hocon/config_includer_file.hpp>

namespace config {
  struct inc final: hocon::config_includer, hocon::config_includer_file {
  private:
    hocon::shared_includer fallback_;

    explicit inc(hocon::shared_includer fallback): fallback_(fallback) {
    }

  public:
    inc() = default;

    virtual ~inc() = default;

    [[nodiscard]] hocon::shared_includer with_fallback(hocon::shared_includer fallback) const override {
      if (fallback_.get() != nullptr) {
        return hocon::shared_includer(this);
      } else {
        return hocon::shared_includer(new inc(fallback));
      }
    }

    [[nodiscard]] hocon::shared_object include(hocon::shared_include_context context, std::string what) const override {
      // std::cout << "Not a file" << what << std::endl;
      // return fallback_->include(context, what.substr(0, what.find_first_of('(')));
      return fallback_->include(context, what);
    }

    [[nodiscard]] hocon::shared_object include_file(
      hocon::shared_include_context context,
      std::string what
    ) const override {
      // std::cout << what << std::endl;
      auto i = what.find_first_of("${HOME}");
      if (i != std::string::npos) {
        std::string tmp;
        tmp.append(what.substr(0, i));
        tmp.append(std::getenv("HOME"));
        tmp.append(what.substr(i + 7));
        what = tmp;
      }
      // std::cout << what << std::endl;

      hocon::shared_object obj = fallback_->include(context, what);
      hocon::config_resolve_options res_op { };
      res_op = res_op.set_allow_unresolved(true);
      return obj->to_config()->resolve(res_op)->root();
    }
  };
}

#endif //INCLUDER_H

//
// Created by castle on 4/9/24.
//

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <hocon/config.hpp>
#include <hocon/config_parse_options.hpp>

#include "config_errors.h"
#include "config_loader.h"
#include "variables_t.h"
#include "files.h"
#include "includer.h"

#include <hocon/parser/config_document.hpp>
#include <hocon/parser/config_document_factory.hpp>

namespace config {
  inline result_t<int> load() {
    hocon::config_parse_options op { };
    op = op.set_allow_missing(false)
           .append_includer(std::make_shared<const inc>(inc { }));
    hocon::shared_config conf = hocon::config::parse_file_any_syntax(locate_config_files()[0], op);

    hocon::config_resolve_options res_op { };
    res_op = res_op.set_allow_unresolved(true);
    while (!conf->is_resolved()) {
      conf = conf->resolve(res_op);
    }
    // hocon::shared_config conf = hocon::config::parse_string(R"()");
    // hocon::shared_config c;
    // hocon::config_parse_options op { };
    // op = op.set_allow_missing(false)
    //        .append_includer(std::make_shared<const inc>(inc { }));
    //
    // for (const auto &path: locate_config_files()) {
    //   std::cout << "PATH: " << path.string() << std::endl;
    //
    //   conf = std::dynamic_pointer_cast<const hocon::config>(
    //     conf->with_fallback(hocon::config::parse_file_any_syntax(path, op))
    //   );
    //   conf = conf->resolve();
    // }

    hocon::config_render_options o { };
    o = o.set_origin_comments(false);
    std::cout << conf->root()->render(o) << std::endl;

    // if (doc->has_path("kb.keyb3"))
    // std::cout << conf->get_string("key") << std::endl;

    return 0;
  }
}


#endif //CONFIG_LOADER_H

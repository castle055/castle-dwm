//
// Created by castle on 4/9/24.
//

#ifndef FILES_H
#define FILES_H

#include <vector>
#include <string>
#include <filesystem>
#include <ranges>

#include "../cyd-fabric/xdg.h"

namespace fs = std::filesystem;

namespace config {
  inline std::vector<fs::path> locate_config_files() {
    std::vector<fs::path> possible_locations {
      // xdg::CONFIG() + "/cyd-wm/main.conf",
      xdg::CONFIG() + "/cyd-wm/def.conf",
      // xdg::CONFIG() + "/cyd-wm/main.json",
      // "/etc/cyd-wm/main.conf",
    };

    std::vector<fs::path> locations { };
    for (fs::path &&possible_location: std::ranges::as_rvalue_view(possible_locations)
                                       // | std::views::reverse
                                       | std::views::filter([](const fs::path &p) { return exists(p); })) {
      locations.push_back(possible_location);
    }

    return locations;
  }
}

#endif //FILES_H

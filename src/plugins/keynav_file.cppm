/*! \file  keynav_file.cppm
 *! \brief 
 *!
 */

module;
#include <X11/Xlib.h>
#include <yaml-cpp/yaml.h>
export module keynav:file;

import std;
import cyd_wm.types;
import cyd_wm.state.keynav;

namespace keynav {
  export key_nav_target parse_map_node(YAML::Node node) {
    auto node_map = node["map"];
    auto kt       = key_nav_target {
      .description = node["desc"].as<std::string>(),
      .type = NAV_DIR
    };
    key_nav_map* map = &kt.map;

    for (YAML::const_iterator it = node_map.begin(); it != node_map.end(); ++it) {
      try {
        if (auto exec = it->second["exec"]) {
          auto* cmd = new std::vector<const char*>();
          if (exec.IsSequence()) {
            for (const auto &item: exec) {
              const char* s = (new std::string(item.as<std::string>()))->c_str();
              cmd->insert(cmd->end(), s);
            }
          } else {
            const char* s = (new std::string(exec.as<std::string>()))->c_str();
            cmd->insert(cmd->end(), s);
          }
          // IMPORTANT - move to heap so it doesn't disappear
          std::string desc = (*cmd)[0];
          if (auto desc_node = it->second["desc"]) desc = desc_node.as<std::string>();
          auto subkt = key_nav_target{
            .description = desc,
            .type = keynav::APPLICATION,
            .func = control::spawn_exec,
            .arg = {.v = (*cmd)[0]},
          };
          map->emplace(XStringToKeysym(it->first.as<std::string>().c_str()), subkt);
        } else if (auto submap = it->second["map"]) {
          map->emplace(XStringToKeysym(it->first.as<std::string>().c_str()), parse_map_node(it->second));
        }
      } catch (YAML::InvalidNode &e) {
      }
    }
    return kt;
  }

  export struct node_holder {
    YAML::Node node;

    explicit node_holder(const std::string &str): node(YAML::Load(str)) {
    }
  };

  void parse_key_nav() {
    try {
      std::ifstream file {state::config::key_nav_file};
      std::stringstream ss;
      const std::size_t buf_len = 8;
      char buf[buf_len];
      std::size_t n;
      while (!file.eof()) {
        for (n = 0; n < buf_len; n++) buf[n] = '\0';
        file.read(buf, buf_len);
        ss << std::string {buf};
      }
      std::string file_str = ss.str();
      //std::cout << file_str << std::endl;
      //YAML::Node key_nav_node = YAML::Load(file_str);
      node_holder* holder;

      std::thread {
        [&]() {
          holder = new node_holder {file_str};
        }
      }.join();

      YAML::Node key_nav_node = holder->node;

      for (YAML::const_iterator it = key_nav_node.begin(); it != key_nav_node.end(); ++it) {
        keynav::root.map.emplace(XStringToKeysym(it->first.as<std::string>().c_str()), parse_map_node(it->second));
      }
    } catch (YAML::BadFile &e) {
      // TODO - Log error
      return;
    }
  }

  export void reload_key_nav() {
    keynav::reset_map();
    parse_key_nav();
  }
}
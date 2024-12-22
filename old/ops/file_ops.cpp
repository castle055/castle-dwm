//
// Created by castle on 5/19/22.
//

#include "log_ops.h"
#include "keynav_ops.h"
#include "control_ops.h"
#include "file_ops.h"
#include "../state/state.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

using namespace ops;

key_nav_target parse_map_node(YAML::Node node) {
  log::info("[parse_map_node] parsing node");
  auto node_map = node["map"];
  auto kt       = key_nav_target {
    .description = node["desc"].as<std::string>(),
    .type = NAV_DIR
  };
  key_nav_map* map = &kt.map;

  for (YAML::const_iterator it = node_map.begin(); it != node_map.end(); ++it) {
    try {
      if (auto exec = it->second["exec"]) {
        log::info("[parse_map_node] parsing exec node");
        auto* cmd = new std::vector<const char*>();
        if (exec.IsSequence()) {
          for (const auto &item: exec) {
            const char* s = (new std::string(item.as<std::string>()))->c_str();
            log::info("seq: %s", s);
            cmd->insert(cmd->end(), s);
          }
        } else {
          const char* s = (new std::string(exec.as<std::string>()))->c_str();
          log::info(s);
          cmd->insert(cmd->end(), s);
        }
        // IMPORTANT - move to heap so it doesn't disappear
        std::string desc = (*cmd)[0];
        if (auto desc_node = it->second["desc"]) desc = desc_node.as<std::string>();
        auto subkt = key_nav_target({
          .description = desc,
          .type = APPLICATION,
          .func = control::spawn_exec,
          .arg = {.v = (*cmd)[0]},
        });
        map->emplace(XStringToKeysym(it->first.as<std::string>().c_str()), subkt);
      } else if (auto submap = it->second["map"]) {
        log::info("[parse_map_node] parsing map node: %s", it->first.as<std::string>().c_str());
        map->emplace(XStringToKeysym(it->first.as<std::string>().c_str()), parse_map_node(it->second));
      }
      log::info("[parse] item: %s - %s", it->first.as<std::string>().c_str(),
                it->second["desc"].as<std::string>().c_str());
    } catch (YAML::InvalidNode &e) {
    }
  }
  return kt;
}

struct node_holder {
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
      //log::info("[parse] item: %s - %s", it->first.as<std::string>().c_str(), it->second["desc"].as<std::string>().c_str());
    }
  } catch (YAML::BadFile &e) {
    log::error("[parse] bad file %s", state::config::key_nav_file.c_str());
    return;
  }
}

void file::reload_key_nav() {
  keynav::reset_map();
  parse_key_nav();
}

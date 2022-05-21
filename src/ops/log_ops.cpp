//
// Created by castle on 4/1/22.
//

#include "log_ops.h"
#include "../state/state.h"

#include <cstdarg>
#include <stdio.h>
#include <string>
#include <fstream>

using namespace ops;

void log::debug(const char *format, ...) {
  std::va_list va;
  time_t now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [DEBUG] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  if (state::log_file.is_open()) {
    const size_t buf_len = 1024;
    char buf[buf_len];
    vsnprintf(buf, buf_len, msg.c_str(), va);
    state::log_file << buf;
    state::log_file.flush();
  } else {
    vfprintf(stdout, msg.c_str(), va);
  }
  va_end(va);
  fflush(stdout);
}
void log::info(const char *format, ...) {
  va_list va;
  time_t now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [INFO] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  if (state::log_file.is_open()) {
    const size_t buf_len = 1024;
    char buf[buf_len];
    vsnprintf(buf, buf_len, msg.c_str(), va);
    state::log_file << buf;
    state::log_file.flush();
  } else {
    vfprintf(stdout, msg.c_str(), va);
  }
  va_end(va);
  fflush(stdout);
}
void log::warn(const char *format, ...) {
  va_list va;
  time_t now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [WARN] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  if (state::log_file.is_open()) {
    const size_t buf_len = 1024;
    char buf[buf_len];
    vsnprintf(buf, buf_len, msg.c_str(), va);
    state::log_file << buf;
    state::log_file.flush();
  } else {
    vfprintf(stdout, msg.c_str(), va);
  }
  va_end(va);
  fflush(stdout);
}

void log::error(const char *format, ...) {
  va_list va;
  time_t now = time(nullptr);
  
  std::string msg = ctime(&now);
  msg = msg.substr(0, msg.size() - 2);
  msg.append(": [ERROR] ");
  msg.append(format);
  msg.append("\n\r");
  
  va_start(va, format);
  if (state::log_file.is_open()) {
    const size_t buf_len = 1024;
    char buf[buf_len];
    vsnprintf(buf, buf_len, msg.c_str(), va);
    state::log_file << buf;
    state::log_file.flush();
  } else {
    vfprintf(stderr, msg.c_str(), va);
  }
  va_end(va);
  fflush(stderr);
}

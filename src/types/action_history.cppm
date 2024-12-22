/*! \file  action_history.cppm
 *! \brief 
 *!
 */

export module cyd_wm.types:action_history;

import std;
import fabric.memory.circular_buffer;

export namespace cyd_wm {
 template<typename T, std::size_t SIZE>
 class action_history {
 public:
  action_history()
   : data_(),
     current_action_offset_(0) {
  }
  void append(const T& action) {
   data_.pop_back(data_.size() - current_action_offset_);
   data_.push_back(action);
   current_action_offset_ = data_.size()-1;
  }

  bool undo() {
   if (current_action_offset_ > 0) {
    --current_action_offset_;
    return true;
   } else {
    return false;
   }
  }

  bool redo() {
   if (current_action_offset_ >= data_.size()) {
    return false;
   } else {
    ++current_action_offset_;
    return true;
   }
  }

  T& current() {
   return data_[current_action_offset_];
  }

  const T& current() const {
   return data_[current_action_offset_];
  }
 private:
  fabric::circular_buffer<T, SIZE> data_;
  std::size_t current_action_offset_;
 };
}
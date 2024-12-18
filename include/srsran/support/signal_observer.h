/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <functional>
namespace srsran {

class signal_observer;

class signal_subject
{
public:
  virtual ~signal_subject() {};
  virtual void attach(signal_observer* observer) = 0;
  virtual void detach(signal_observer* observer) = 0;
  virtual void notify_signal(int signal)         = 0;
};

using signal_callback = std::function<void()>;

class signal_observer
{
public:
  signal_observer(signal_callback callback_) : callback(callback_) {}

  virtual ~signal_observer()
  {
    if (current_subject) {
      current_subject->detach(this);
    }
  }

  void set_current_subject(signal_subject* subject)
  {
    if (current_subject) {
      current_subject->detach(this);
    }
    current_subject = subject;
  }

  void handle_signal(int signal) { callback(); };

private:
  signal_callback callback;
  signal_subject* current_subject = nullptr;
};

} // namespace srsran

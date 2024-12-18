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
#include <vector>

namespace srsran {

class signal_observer;

class signal_subject
{
public:
  void attach(signal_observer* observer);
  void detach(signal_observer* observer);
  void notify_signal(int signal);

private:
  std::vector<signal_observer*> observers;
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

void signal_subject::attach(signal_observer* observer)
{
  observers.push_back(observer);
  observer->set_current_subject(this);
}

void signal_subject::detach(signal_observer* observer)
{
  for (auto it = observers.begin(); it != observers.end();) {
    if (*it == observer) {
      it = observers.erase(it);
      continue;
    }
    ++it;
  }
  observer->set_current_subject(nullptr);
}

void signal_subject::notify_signal(int signal)
{
  for (auto observer : observers) {
    observer->handle_signal(signal);
  }
}

} // namespace srsran

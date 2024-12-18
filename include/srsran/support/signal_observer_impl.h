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

#include "srsran/support/signal_observer.h"
#include <vector>
namespace srsran {

class signal_subject_impl : public signal_subject
{
public:
  virtual ~signal_subject_impl() {}

  void attach(signal_observer* observer) override
  {
    observers.push_back(observer);
    observer->set_current_subject(this);
  }

  void detach(signal_observer* observer) override
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

  void notify_signal(int signal) override
  {
    for (auto observer : observers) {
      observer->handle_signal(signal);
    }
  }

private:
  std::vector<signal_observer*> observers;
};

} // namespace srsran

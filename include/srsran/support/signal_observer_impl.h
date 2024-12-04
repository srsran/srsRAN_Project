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
#include <list>
namespace srsran {

class signal_observable_impl : public signal_observable
{
public:
  virtual ~signal_observable_impl() {}

  void attach(signal_observer* observer) override
  {
    observers.push_back(observer);
    observer->set_current_observable(this);
  }

  void detach(signal_observer* observer) override
  {
    observers.remove(observer);
    observer->set_current_observable(nullptr);
  }

  void notify_signal(int signal) override
  {
    for (auto observer : observers) {
      observer->handle_signal(signal);
    }
  }

private:
  std::list<signal_observer*> observers;
};

} // namespace srsran

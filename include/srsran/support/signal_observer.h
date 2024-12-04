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

namespace srsran {

class signal_observer;

class signal_observable
{
public:
  virtual ~signal_observable(){};
  virtual void attach(signal_observer* observer) = 0;
  virtual void detach(signal_observer* observer) = 0;
  virtual void notify_signal(int signal)         = 0;
};

class signal_observer
{
public:
  virtual ~signal_observer()
  {
    if (current_observable) {
      current_observable->detach(this);
    }
  }
  void         set_current_observable(signal_observable* observable) { current_observable = observable; }
  virtual void handle_signal(int signal) = 0;

private:
  signal_observable* current_observable = nullptr;
};

} // namespace srsran

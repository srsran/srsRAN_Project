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

/// This class is a dispatcher for [posix] signals (e.g. SIGTERM,...) to which the signal observer can attach/detach in
/// order to subscribe for signal events.
///
/// The signal dispatcher must outlive all its subscribers, because the signal observer automatically unregister from
/// the signal dispatcher upon destruction.
class signal_dispatcher
{
public:
  /// \brief Attach a new observer to this signal dispatcher.
  /// \param observer The observer that subscribes for receiving signals.
  void attach(signal_observer* observer);
  /// \brief Detach an observer, i.e. end its subscription for receiving signals.
  /// \param observer The observer that unsubscribers from receiving signals.
  void detach(signal_observer* observer);
  /// \brief Notifies all its attached observers (i.e. subscribers) of the given signal. This function is to be called
  /// from the actual signal source.
  /// \param signal The signal that occured.
  void notify_signal(int signal);

private:
  /// Collection of subscribers attached to this dispatcher.
  std::vector<signal_observer*> observers;
};

using signal_callback = std::function<void()>;

/// This class is an observer that may be used to subscribe for [posix] signals (e.g. SIGTERM,...) from a signal
/// dispatcher. The dispatcher calls handle_signal to notify the observer of a signal event; the observer then invokes
/// the callback that was set upon construction.
///
/// Upon destruction, the observer automatically unsubscribes from the signal dispatcher.
class signal_observer
{
public:
  /// \brief Creates a signal observer with a given callback and attaches the observer to a signal dispatcher. The
  /// signal dispatcher must outlive the observer due to automatic detach on destruction of the observer.
  /// \param dispatcher_ The signal dispatcher to which this observer shall be attached.
  /// \param callback_ The callback to be invoked on a signal event.
  explicit signal_observer(signal_dispatcher& dispatcher_, signal_callback callback_) :
    dispatcher(dispatcher_), callback(std::move(callback_))
  {
    dispatcher.attach(this);
  }

  /// \brief Automatically unsubscribe from the current_subject (i.e. the signal dispatcher).
  virtual ~signal_observer() { dispatcher.detach(this); }

  /// \brief Handles a signal that was dispatched by the signal_dispatcher.
  /// \param signal The signal that occured.
  void handle_signal(int signal) { callback(); };

private:
  /// The signal dispatcher to which this observer is attached.
  signal_dispatcher& dispatcher;
  /// The callback to be invoked on a signal event.
  signal_callback callback;
};

void signal_dispatcher::attach(signal_observer* observer)
{
  observers.push_back(observer);
}

void signal_dispatcher::detach(signal_observer* observer)
{
  for (auto it = observers.begin(); it != observers.end();) {
    if (*it == observer) {
      it = observers.erase(it);
      continue;
    }
    ++it;
  }
}

void signal_dispatcher::notify_signal(int signal)
{
  for (auto observer : observers) {
    observer->handle_signal(signal);
  }
}

} // namespace srsran

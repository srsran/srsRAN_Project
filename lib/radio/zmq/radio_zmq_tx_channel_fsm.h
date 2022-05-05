
#ifndef SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_TX_CHANNEL_FSM_H
#define SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_TX_CHANNEL_FSM_H

#include <condition_variable>
#include <mutex>

namespace srsgnb {
class radio_zmq_tx_channel_fsm
{
private:
  /// Describes the possible states.
  enum class states {
    /// Indicates it failed to initialize.
    UNINITIALIZED = 0,
    /// Indicates it has been successfully initialized and it is waiting for request.
    WAIT_REQUEST,
    /// Indicates it has been successfully initialized and it is waiting for data to transmit.
    WAIT_DATA,
    /// Signals a stop to the asynchronous thread.
    WAIT_STOP,
    /// Indicates it is stopped.
    STOPPED
  };

  /// Indicates the current state.
  states state = states::UNINITIALIZED;
  /// Protects the state and transitions.
  mutable std::mutex mutex;
  /// Condition variable to wait for certain states.
  std::condition_variable cvar;

  /// Same than is_running().
  bool is_running_unprotected() const { return state == states::WAIT_REQUEST || state == states::WAIT_DATA; }

public:
  /// Notifies that the transmit stream has been initialized successfully.
  void init_successful()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::WAIT_REQUEST;
  }

  /// Notifies that the stream detected an error it cannot recover from.
  void on_error()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::STOPPED;
    cvar.notify_all();
  }

  /// Notifies that a request was received.
  void request_received()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::WAIT_REQUEST) {
      state = states::WAIT_DATA;
    }
  }

  /// Notifies a block of data was send.
  void data_sent()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::WAIT_DATA) {
      state = states::WAIT_REQUEST;
    }
  }

  /// Notifies a stop was signaled and wait for asynchronous executor to stop.
  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (is_running_unprotected()) {
      state = states::WAIT_STOP;
    }
  }

  /// Waits for the asynchronous tasks notifies that has stopped.
  void wait_stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (state != states::STOPPED) {
      cvar.wait(lock);
    }
  }

  /// Notifies the asynchronous task has stopped.
  void async_task_stopped()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::STOPPED;
    cvar.notify_all();
  }

  /// Indicates whether the current state is operational.
  bool is_running() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return is_running_unprotected();
  }

  /// Indicates whether it has a pending request.
  bool has_pending_request() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return state == states::WAIT_DATA;
  }
};
} // namespace srsgnb

#endif // SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_TX_CHANNEL_FSM_H

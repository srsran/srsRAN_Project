
#ifndef SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_RX_CHANNEL_FSM_H
#define SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_RX_CHANNEL_FSM_H

#include <condition_variable>
#include <mutex>

namespace srsgnb {
class radio_zmq_rx_channel_fsm
{
private:
  /// Describes the possible states.
  enum class states {
    /// Indicates it failed to initialise.
    UNINITIALISED = 0,
    /// Indicates it has been successfully initialised and it is waiting for request.
    SEND_REQUEST,
    /// Indicates it has been successfully initialised and it is waiting for data to transmit.
    RECEIVE_DATA,
    /// Signals a stop to the asynchronous thread.
    WAIT_STOP,
    /// Indicates it is stopped.
    STOPPED
  };

  /// Indicates the current state.
  states state = states::UNINITIALISED;
  /// Protects the state and transitions.
  mutable std::mutex mutex;
  /// Condition variable to wait for certain states.
  std::condition_variable cvar;

  /// Same than is_running().
  bool is_running_unprotected() const { return state == states::SEND_REQUEST || state == states::RECEIVE_DATA; }

public:
  /// Notifies that the transmit stream has been initialised successfully.
  void init_successful()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::SEND_REQUEST;
  }

  /// Notifies that the stream detected an error it cannot recover from.
  void on_error()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::STOPPED;
    cvar.notify_all();
  }

  /// Notifies that a request was received.
  void request_sent()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::SEND_REQUEST) {
      state = states::RECEIVE_DATA;
    }
  }

  /// Notifies a block of data was send.
  void data_received()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::RECEIVE_DATA) {
      state = states::SEND_REQUEST;
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
  bool has_pending_response() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return state == states::RECEIVE_DATA;
  }
};
} // namespace srsgnb

#endif // SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_RX_CHANNEL_FSM_H

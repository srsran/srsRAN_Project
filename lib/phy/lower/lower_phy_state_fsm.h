
#ifndef SRSGNB_PHY_LOWER_PHY_STATE_FSM_H
#define SRSGNB_PHY_LOWER_PHY_STATE_FSM_H

#include <condition_variable>
#include <mutex>

namespace srsgnb {

/// Describes lower physical layer state FSM.
class lower_phy_state_fsm
{
private:
  /// Describes the possible states.
  enum class states {
    /// Indicates the lower physical layer has not been initialised.
    UNINITIALISED,
    /// Indicates the lower physical layer has been successfully initialised and started the execution of asynchronous
    /// tasks.
    RUNNING,
    /// Indicates stop has been signaled and it is waiting for the execution of asynchronous tasks to finish.
    WAIT_STOP,
    /// Indicates the component was initialised successfully and no asynchronous task is performed.
    STOPPED
  };

  /// Indicates the current state.
  states state = states::UNINITIALISED;
  /// Protects the concurrent access to the instance.
  mutable std::mutex mutex;
  /// Used to wait for certain state transitions.
  std::condition_variable cvar;

  /// Same than is_running().
  bool is_running_unprotected() const { return state == states::RUNNING; }

public:
  /// Notifies the successful component initialisation.
  void on_successful_init()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::UNINITIALISED) {
      state = states::RUNNING;
    }
  }

  /// Notifies the asynchronous task executor stopped.
  void on_async_executor_stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::STOPPED;
    cvar.notify_all();
  }

  /// Signals a stop and join.
  void stop_and_join()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (is_running_unprotected()) {
      state = states::WAIT_STOP;
    }

    while (state != states::UNINITIALISED && state != states::STOPPED) {
      cvar.wait(lock);
    }
  }

  bool is_running() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return is_running_unprotected();
  }
};

} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_PHY_STATE_FSM_H

/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/intrusive_list.h"
#include "srsran/adt/unique_function.h"
#include "srsran/adt/variant.h"
#include "srsran/support/executors/task_executor.h"
#include <chrono>
#include <deque>
#include <mutex>

namespace srsran {

/// Type to represent a unique timer identifier.
enum timer_id_t : uint32_t { INVALID_TIMER_ID = std::numeric_limits<uint32_t>::max() };

/// Unit used for timer ticks
using timer_duration = std::chrono::milliseconds;

class unique_timer2;

/// Class that manages the creation/destruction/start/stop/timeout of unique_timers. Each unique_timer duration,
/// and callback can be set via the set(...) method. A timer can be started/stopped via run()/stop() methods.
/// Neither the unique_timer or timer_manager are thread-safe on their own. However, the communication between the two
/// entities can be across threads in single producer, single consumer fashion (SPSC).
/// Internal Data structures:
/// - timer_list - std::deque that stores timer objects via push_back() to keep pointer/reference validity.
///   The timer index in the timer_list matches the timer object id field.
///   This deque will only grow in size. Erased timers are just tagged in the deque as empty, and can be reused for the
///   creation of new timers. To avoid unnecessary runtime allocations, the user can set an initial capacity.
/// - free_list - intrusive forward linked list to keep track of the empty timers and speed up new timer creation.
/// - A large circular vector of size WHEEL_SIZE which works as a time wheel, storing and circularly indexing the
///   currently running timers by their respective timeout value.
///   For a number of running timers N, and uniform distribution of timeout values, the tick_all() complexity
///   should be O(N/WHEEL_SIZE). Thus, the performance should improve with a larger WHEEL_SIZE, at the expense of more
///   used memory.
class timer_manager2
{
  /// This type represents a tick.
  using timer_tick_t = uint32_t;

  using epoch_type = uint32_t;

  constexpr static timer_duration INVALID_DURATION = std::numeric_limits<timer_duration>::max();

  /// Possible states for a timer.
  enum class state_t { stopped, running, expired };

  /// Command sent from the timer frontend to the backend.
  struct cmd_t {
    enum action_t { start, stop, destroy };

    timer_id_t id;
    unsigned   epoch;
    action_t   action;
    unsigned   duration;
  };

  /// Data relative to a timer state that is safe to access from the frontend.
  struct timer_frontend {
    /// Reference to timer manager class.
    timer_manager2& parent;
    /// Timer identifier. This identifier should remain constant throughout the timer lifetime.
    const timer_id_t id;
    /// Task executor used to dispatch expiry callback. When set to nullptr, the timer is not allocated.
    task_executor* exec = nullptr;
    /// \brief Identifier of the last command sent to the timer backend. We use this value to keep track of cancelled
    /// timer runs.
    std::atomic<epoch_type> epoch{0};
    /// The current state of timer (e.g. running/expired/stopped) from the perspective of the timer frontend.
    state_t state = state_t::stopped;
    /// Duration of each timer run.
    timer_duration duration = INVALID_DURATION;
    /// Callback triggered when timer expires. Callback updates are protected by backend lock.
    unique_function<void(timer_id_t)> callback;

    timer_frontend(timer_manager2& parent_, timer_id_t id_);

    void destroy();

    void set(timer_duration duration);

    void set(timer_duration duration, unique_function<void(timer_id_t)> callback);

    void run();

    void stop();
  };

  /// \brief Timer context used solely by the back-end side of the timer manager.
  struct timer_backend_context {
    epoch_type epoch   = 0;
    state_t    state   = state_t::stopped;
    unsigned   timeout = 0;
  };

  /// \brief Object holding both the front-end and back-end contexts of the timers.
  struct timer_handle : public intrusive_double_linked_list_element<>, public intrusive_forward_list_element<> {
    std::unique_ptr<timer_frontend> frontend;
    timer_backend_context           backend;

    timer_handle() = default;
  };

public:
  /// Constructs a timer manager.
  /// \param capacity Number of timers to pre-reserve and speed up timer construction.
  explicit timer_manager2(size_t pre_reserve_capacity = 64);

  /// Advances one tick and triggers any running timer that just expired.
  void tick_all();

  /// Creates a new instance of a unique timer.
  unique_timer2 create_unique_timer(task_executor& exec);

  /// Returns the number of timers handled by this instance.
  size_t nof_timers() const;

  /// Returns the number of running timers handled by this instance.
  unsigned nof_running_timers() const;

private:
  friend class unique_timer2;

  /// \brief Create a new front-end context to be used by a newly created unique_timer.
  timer_frontend& create_frontend_timer(task_executor& exec);

  /// Push a new timer command (start, stop, destroy) from the front-end execution context to the backend.
  void push_timer_command(cmd_t cmd);

  /// \brief Create a new timer_handle object in the timer manager back-end side and associate it with the provided
  /// front-end timer.
  void create_timer_handle(std::unique_ptr<timer_frontend> timer);

  /// Start the ticking of a timer with a given duration.
  void start_timer_backend(timer_handle& timer, unsigned duration);

  /// \brief Stop a timer from ticking. If \c expiry_reason is set to true, the timer callback is dispatched to the
  /// frontend execution context.
  bool try_stop_timer_backend(timer_handle& timer, bool expiry_reason);
  void stop_timer_backend(timer_handle& timer, bool expiry_reason);

  /// Called to destroy a timer context and return it back to the free timer pool.
  void destroy_timer_backend(timer_handle& timer);

  /// Counter of the number of ticks elapsed. This counter gets incremented on every \c tick_all call.
  timer_tick_t cur_time = 0;

  /// Number of created timer_handle objects that are currently running.
  size_t nof_timers_running = 0;

  /// \brief List of timer_handle objects currently being managed by the timer_manager class. The index of the element
  /// in this container matches the id of the corresponding unique_timer object.
  /// This container will only grow in size. "Deleted" unique_timers in the front-end side are just tagged as empty
  /// in their respective timer_handle, in the back-end side. This allows already created timer_handle objects to be
  /// reused for the creation of new unique_timers in the front-end. The fact that the timer_handle is not deleted
  /// also avoids potential dangling pointer when the timer expiry callback is dispatched to the front-end execution
  /// context.
  /// Note: we use a deque to maintain reference validity.
  std::deque<timer_handle> timer_list;

  /// Free list of timer_handle objects in timer_list.
  mutable std::mutex           free_list_mutex;
  unsigned                     next_timer_id = 0;
  std::vector<timer_frontend*> free_list;

  /// Timer wheel, which is circularly indexed via a running timer timeout. Collisions are resolved via an intrusive
  /// list in timer_handle.
  std::vector<srsran::intrusive_double_linked_list<timer_handle>> time_wheel;

  /// Commands sent by the timer front-end to the backend.
  std::mutex                                                   cmd_mutex;
  std::vector<variant<cmd_t, std::unique_ptr<timer_frontend>>> pending_cmds, cmds_to_process;
};

/// This class represents a timer which invokes a user-provided callback upon timer expiration.
class unique_timer2
{
  using state_t = timer_manager2::state_t;

public:
  unique_timer2()                                = default;
  unique_timer2(const unique_timer2&)            = delete;
  unique_timer2& operator=(const unique_timer2&) = delete;

  unique_timer2(unique_timer2&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
  unique_timer2& operator=(unique_timer2&& other) noexcept
  {
    handle = std::exchange(other.handle, nullptr);
    return *this;
  }
  ~unique_timer2() { reset(); }

  /// Destroy current unique_timer2 context, cancelling any pending event.
  void reset()
  {
    if (is_valid()) {
      handle->destroy();
      handle = nullptr;
    }
  }

  /// Returns true if the timer is valid, otherwise returns false if already released.
  bool is_valid() const { return handle != nullptr; }

  /// Returns the unique timer identifier.
  timer_id_t id() const { return is_valid() ? handle->id : INVALID_TIMER_ID; }

  /// Returns true if the timer duration has been set, otherwise returns false.
  bool is_set() const { return is_valid() && handle->duration != timer_manager2::INVALID_DURATION; }

  /// Returns true if the timer is currently running, otherwise returns false.
  bool is_running() const { return is_valid() && handle->state == timer_manager2::state_t::running; }

  /// Returns true if the timer has expired, otherwise returns false.
  bool has_expired() const { return is_valid() && handle->state == timer_manager2::state_t::expired; }

  /// Returns the configured duration for this timer measured in ticks.
  timer_duration duration() const { return is_valid() ? handle->duration : timer_manager2::INVALID_DURATION; }

  /// Configures the duration of the timer calling the provided callback upon timer expiration.
  void set(timer_duration duration, unique_function<void(timer_id_t)> callback)
  {
    srsran_assert(is_valid(), "Trying to setup empty timer pimpl");
    handle->set(duration, std::move(callback));
  }

  /// Configures the duration of the timer.
  void set(timer_duration duration)
  {
    srsran_assert(is_valid(), "Trying to setup empty timer pimpl");
    handle->set(duration);
  }

  /// Activates the timer to start ticking.
  void run()
  {
    srsran_assert(is_valid(), "Starting invalid timer");
    handle->run();
  }

  /// Stops the timer from ticking.
  void stop()
  {
    if (is_running()) {
      handle->stop();
    }
  }

private:
  friend class timer_manager2;

  explicit unique_timer2(timer_manager2::timer_frontend& impl) : handle(&impl) {}

  timer_manager2::timer_frontend* handle = nullptr;
};

} // namespace srsran

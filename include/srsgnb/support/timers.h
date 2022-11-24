/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/intrusive_list.h"
#include "srsgnb/adt/unique_function.h"
#include <cinttypes>
#include <cstdint>
#include <deque>
#include <limits>
#include <mutex>

namespace srsgnb {

class unique_timer;

/// Type to represent a time difference measured in ticks.
using timer_tick_difference_t = uint32_t;

/// Type to represent a unique timer identifier.
using timer_id_t = uint32_t;

/// Class that manages the creation/destruction/start/stop/timeout of unique_timers. Each unique_timer duration,
/// and callback can be set via the set(...) method. A timer can be started/stopped via run()/stop() methods.
/// The timers access/alteration is thread-safe. Just beware non-atomic uses of its getters.
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
class timer_manager
{
  /// This type represents a tick.
  using timer_tick_t = uint32_t;

  /// Wheel configuration parameters.
  static constexpr size_t WHEEL_SHIFT = 16U;
  static constexpr size_t WHEEL_SIZE  = 1U << WHEEL_SHIFT;
  static constexpr size_t WHEEL_MASK  = WHEEL_SIZE - 1U;

  /// Timer state constant definitions.
  static constexpr uint64_t                STOPPED_FLAG       = 0U;
  static constexpr uint64_t                RUNNING_FLAG       = static_cast<uint64_t>(1U) << 63U;
  static constexpr uint64_t                EXPIRED_FLAG       = static_cast<uint64_t>(1U) << 62U;
  static constexpr timer_tick_difference_t MAX_TIMER_DURATION = 0x3fffffffU;

  /// Miscellaneous decoding helper functions.
  static bool                    decode_is_running_flag(uint64_t value) { return (value & RUNNING_FLAG) != 0; }
  static bool                    decode_is_expired_flag(uint64_t value) { return (value & EXPIRED_FLAG) != 0; }
  static timer_tick_difference_t decode_duration(uint64_t value) { return (value >> 32U) & MAX_TIMER_DURATION; }
  static timer_tick_t            decode_timeout(uint64_t value) { return static_cast<uint32_t>(value & 0xffffffffU); }
  static uint64_t encode_state(uint64_t mode_flag, timer_tick_difference_t duration, timer_tick_t timeout)
  {
    return mode_flag + (static_cast<uint64_t>(duration) << 32U) + timeout;
  }

  /// Internal timer implementation class.
  struct timer_handle : public intrusive_double_linked_list_element<>, public intrusive_forward_list_element<> {
    timer_manager& parent;
    /// Timer identifier. This identifier should remain constant throughout the timer lifetime.
    const timer_id_t id;
    /// Whether timer is being used. Writes are protected by backend lock.
    bool allocated = false;
    /// The current state of timer (e.g. running/expired/stopped) + duratin + timeout
    /// Read methods of timer_handle are not mutexed, thus its state updates must be atomic.
    std::atomic<uint64_t> state{0};
    /// Callback triggered when timer expires. Callback updates are protected by backend lock.
    unique_function<void(timer_id_t)> callback;

  public:
    explicit timer_handle(timer_manager& parent_, timer_id_t id_) : parent(parent_), id(id_) {}
    timer_handle(const timer_handle&)            = delete;
    timer_handle(timer_handle&&)                 = delete;
    timer_handle& operator=(const timer_handle&) = delete;
    timer_handle& operator=(timer_handle&&)      = delete;
    ~timer_handle()                              = default;

    /// Returns true if the timer is currently running, otherwise returns false.
    bool is_running() const { return decode_is_running_flag(state.load(std::memory_order_relaxed)); }

    /// Returns true if the timer has expired, otherwise returns false.
    bool has_expired() const { return decode_is_expired_flag(state.load(std::memory_order_relaxed)); }

    /// Returns the configured duration for this timer measured in ticks.
    timer_tick_difference_t duration() const { return decode_duration(state.load(std::memory_order_relaxed)); }

    /// Returns true if the timer duration has been set, otherwise returns false.
    bool is_set() const { return duration() > 0; }

    /// Returns the elapsed time in ticks.
    timer_tick_difference_t time_elapsed() const
    {
      uint64_t                state_snapshot = state.load(std::memory_order_relaxed);
      timer_tick_difference_t duration       = decode_duration(state_snapshot);
      timer_tick_t            timeout        = decode_timeout(state_snapshot);

      bool running = decode_is_running_flag(state_snapshot);
      bool expired = decode_is_expired_flag(state_snapshot);
      return running ? duration - (timeout - parent.cur_time) : (expired ? duration : 0);
    }

    /// Configures the duration of the timer.
    void set(timer_tick_difference_t duration)
    {
      srsgnb_assert(duration <= MAX_TIMER_DURATION,
                    "Invalid timer duration={}>{}",
                    duration,
                    (timer_tick_difference_t)MAX_TIMER_DURATION);
      std::lock_guard<std::mutex> lock(parent.mutex);
      set_impl(duration);
    }

    /// Configures the duration of the timer calling the provided callback upon timer expiration.
    void set(timer_tick_difference_t duration, unique_function<void(timer_id_t)> cb)
    {
      srsgnb_assert(duration <= MAX_TIMER_DURATION,
                    "Invalid timer duration={}>{}",
                    duration,
                    (timer_tick_difference_t)MAX_TIMER_DURATION);
      std::lock_guard<std::mutex> lock(parent.mutex);
      set_impl(duration);
      callback = std::move(cb);
    }

    /// Activate the timer to start ticking.
    void run() { parent.start_run(*this); }

    /// Stops the timer from ticking. The timer callback is not called.
    void stop() { parent.stop_timer(*this, false); }

    /// Deallocates the timer from its parent.
    void destroy() { parent.destroy_timer(*this); }

  private:
    void set_impl(timer_tick_difference_t duration)
    {
      // The next step will be one place ahead of current one.
      duration = std::max(duration, 1U);

      // Called in locked context.
      uint64_t old_state = state.load(std::memory_order_relaxed);
      if (decode_is_running_flag(old_state)) {
        // If already running, just extends timer lifetime.
        parent.start_run_nolock(*this, duration);
      } else {
        state.store(encode_state(STOPPED_FLAG, duration, 0), std::memory_order_relaxed);
      }
    }
  };

public:
  /// Default ctor
  /// \param capacity Number of timers to pre-reserve and speed up timer construction.
  explicit timer_manager(size_t capacity = 64);

  /// Advances one tick all running timers.
  void tick_all();

  /// Stops all running timers.
  void stop_all();

  /// Creates a new instance of a unique timer.
  unique_timer create_unique_timer();

  /// Returns the number of timers handled by this instance.
  size_t nof_timers() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return timer_list.size() - nof_free_timers;
  }

  /// Returns the number of running timers handled by this instance.
  size_t nof_running_timers() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return nof_timers_running;
  }

  /// Defer the call of the provided callback until duration ticks have elapsed.
  template <typename F>
  void defer_callback(timer_tick_difference_t duration, F f)
  {
    timer_handle&                     timer = create_timer();
    unique_function<void(timer_id_t)> c     = [func = std::move(f), &timer](timer_id_t tid) {
      func();
      // Auto-deletes timer.
      timer.destroy();
    };
    timer.set(duration, std::move(c));
    timer.run();
  }

  /// Returns the maximum allowed duration value.
  static constexpr timer_tick_difference_t max_timer_duration() { return MAX_TIMER_DURATION; }

  /// Returns the size of the internal timer wheel.
  static constexpr size_t get_wheel_size() { return WHEEL_SIZE; }

private:
  friend class unique_timer;

  timer_handle& create_timer();

  /// Called when timer is removed. The timer object gets registered in free list for reuse
  void destroy_timer(timer_handle& timer);
  void destroy_timer_nolock(timer_handle& timer);

  /// Called when timer is started.
  void start_run(timer_handle& timer, timer_tick_difference_t duration = 0);
  void start_run_nolock(timer_handle& timer, timer_tick_difference_t duration = 0);

  /// Called when user manually stops timer (as an alternative to expiry).
  void stop_timer(timer_handle& timer, bool expiry);
  void stop_timer_nolock(timer_handle& timer, bool expiry);

  /// Current timer absolute tick
  std::atomic<timer_tick_t> cur_time{0};

  /// Number of created timer_impl objects that are currently running.
  size_t nof_timers_running = 0;

  /// Number of created timer_impl objects that can be reused.
  size_t nof_free_timers = 0;

  /// List of created timer_impl objects.
  /// Note: Using a deque to maintain reference validity on emplace_back. Also, this deque will only grow.
  std::deque<timer_handle> timer_list;

  /// Free list of timer_impl objects in timer_list.
  srsgnb::intrusive_forward_list<timer_handle> free_list;

  /// Timer wheel, which is circularly indexed via a running timer timeout. Collisions are resolved via an intrusive
  /// list in timer_impl.
  std::vector<srsgnb::intrusive_double_linked_list<timer_handle>> time_wheel;

  /// Protects the addition/modification/removal of timers in timer_manager.
  mutable std::mutex mutex;
};

/// This class represents a timer which invokes a user-provided callback upon timer expiration.
class unique_timer
{
  static constexpr auto INVALID_TIMER_ID = std::numeric_limits<timer_id_t>::max();

  timer_manager::timer_handle* handle = nullptr;

public:
  unique_timer() = default;
  explicit unique_timer(timer_manager::timer_handle* pimpl) : handle(pimpl) {}

  unique_timer(const unique_timer&)            = delete;
  unique_timer& operator=(const unique_timer&) = delete;

  unique_timer(unique_timer&& other) noexcept : handle(other.handle) { other.handle = nullptr; }
  unique_timer& operator=(unique_timer&& other) noexcept
  {
    if (this != &other) {
      handle       = other.handle;
      other.handle = nullptr;
    }
    return *this;
  }

  ~unique_timer()
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
  bool is_set() const { return is_valid() && handle->is_set(); }

  /// Returns true if the timer is currently running, otherwise returns false.
  bool is_running() const { return is_valid() && handle->is_running(); }

  /// Returns true if the timer has expired, otherwise returns false.
  bool has_expired() const { return is_valid() && handle->has_expired(); }

  /// Returns the elapsed time in ticks.
  timer_tick_difference_t time_elapsed() const { return is_valid() ? handle->time_elapsed() : 0; }

  /// Returns the configured duration for this timer measured in ticks.
  timer_tick_difference_t duration() const { return is_valid() ? handle->duration() : 0; }

  /// Configures the duration of the timer calling the provided callback upon timer expiration.
  void set(timer_tick_difference_t duration, unique_function<void(timer_id_t)> callback)
  {
    srsgnb_assert(is_valid(), "Trying to setup empty timer pimpl");
    handle->set(duration, std::move(callback));
  }

  /// Configures the duration of the timer.
  void set(timer_tick_difference_t duration)
  {
    srsgnb_assert(is_valid(), "Trying to setup empty timer pimpl");
    handle->set(duration);
  }

  /// Activates the timer to start ticking.
  void run()
  {
    srsgnb_assert(is_valid(), "Starting invalid timer");
    handle->run();
  }

  /// Stops the timer from ticking.
  void stop()
  {
    if (is_valid()) {
      handle->stop();
    }
  }
};

} // namespace srsgnb

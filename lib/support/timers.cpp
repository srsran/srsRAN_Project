/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/support/timers.h"
#include "cameron314/concurrentqueue.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/adt/span.h"
#include "srsran/srslog/srslog.h"
#include <deque>
#include <utility>
#include <variant>

using namespace srsran;
using namespace timer_detail;

/// Timer Wheel configuration parameters.
static constexpr size_t WHEEL_SHIFT = 16U;
static constexpr size_t WHEEL_SIZE  = 1U << WHEEL_SHIFT;
static constexpr size_t WHEEL_MASK  = WHEEL_SIZE - 1U;

/// Maximum timeout duration supported for a given timer in ticks.
static constexpr timer_duration MAX_TIMER_DURATION = timer_duration{std::numeric_limits<unsigned>::max() / 2};

namespace {

/// Unambiguous identifier of the last command sent by a unique_timer to the timer_manager.
using cmd_id_t = uint32_t;

/// Command sent by the unique_timer (front-end) to the timer manager (back-end).
struct cmd_t {
  /// action type used when timer is started.
  struct start {
    /// Duration for the new run.
    timer_duration duration;
  };
  /// action type used when timer is stopped.
  struct stop {};
  /// action type used when timer is created.
  struct create {};
  /// action type used when timer is destroyed.
  struct destroy {};

  /// Unique identity of the timer.
  timer_id_t id;
  /// Identifier associated with this particular command.
  cmd_id_t cmd_id;
  /// Action Request type sent by the unique_timer.
  std::variant<start, stop, create, destroy> action;
};

/// Queue used by timer frontend to report that they have a new pending action to be processed by the backend.
class timer_update_signaller
{
  static constexpr size_t initial_batch_size            = 128;
  static constexpr size_t warn_on_nof_dequeues_per_tick = 4096U;
  static constexpr size_t default_queue_capacity        = 16384;

public:
  explicit timer_update_signaller(srslog::basic_logger& logger_, size_t initial_capacity = default_queue_capacity) :
    logger(logger_), commands(initial_capacity), cmd_dequeuer(commands), temp_cmd_buffer(initial_batch_size)
  {
  }

  /// Signal a timer state update (start, stop, create, destroy) from the front-end execution context to the backend.
  void notify_timer_update(timer_id_t tid) { commands.enqueue(tid); }

  /// Pop in batches the timers that have pending updates.
  span<timer_id_t> pop_updated_timers()
  {
    size_t           nof_popped = dequeue_events(commands, cmd_dequeuer, temp_cmd_buffer);
    span<timer_id_t> list{temp_cmd_buffer.data(), nof_popped};
    return list;
  }

private:
  size_t dequeue_events(moodycamel::ConcurrentQueue<timer_id_t>&                            q,
                        typename moodycamel::ConcurrentQueue<timer_id_t>::consumer_token_t& t,
                        std::vector<timer_id_t>&                                            buffer)
  {
    size_t nread = 0;
    for (size_t pos = 0; true;) {
      // pop whatever space we have left in temporary buffer.
      size_t max_n = buffer.size() - pos;
      size_t n     = q.try_dequeue_bulk(t, buffer.begin() + pos, max_n);
      nread += n;

      if (n < max_n) {
        // We drained the queue.
        break;
      }

      // Need to grow temp timer buffer and dequeue more objects.
      pos = buffer.size();
      buffer.resize(buffer.size() + initial_batch_size);
    }
    if (nread == warn_on_nof_dequeues_per_tick) {
      logger.warning("Number of timer events within one tick exceeded {}", warn_on_nof_dequeues_per_tick);
    }

    return nread;
  }

  srslog::basic_logger& logger;

  // Queue of timers with pending events.
  moodycamel::ConcurrentQueue<timer_id_t>                   commands;
  moodycamel::ConcurrentQueue<timer_id_t>::consumer_token_t cmd_dequeuer;
  std::vector<timer_id_t>                                   temp_cmd_buffer;
};

/// \brief Channel used by the timer frontend to communicate to the backend and forward new events.
///
/// This channel works as follows:
/// - it overwrites the latest timer event (e.g. stop, start, create, destroy) in an internal buffer. Older events
/// are discarded, even if they were not processed by the backend.
/// - it signals that the timer has a pending event to the backend via the \c backend_signaller IFF the timer has no
/// other events pending to be handled by the backend. This way the use of the backend_signaller is minimized.
class backend_channel
{
public:
  backend_channel(timer_update_signaller& backend_signaller_) : backend_signaller(backend_signaller_)
  {
    for (auto& cmd : buffers) {
      cmd.id = timer_id_t::invalid;
    }
  }

  /// Counter of the last command sent by this timer frontend.
  cmd_id_t current_cmd_id() const { return cmd_id.load(std::memory_order_relaxed); }

  /// Push a new action from the frontend to the backend.
  void push(timer_id_t tid, const std::variant<cmd_t::start, cmd_t::stop, cmd_t::create, cmd_t::destroy>& action)
  {
    srsran_sanity_check(tid != timer_id_t::invalid, "Invalid timer id");
    // increase the "epoch", which will invalidate other events for this timer.
    cmd_id_t new_cmd_id = cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
    // prepare new action.
    write_buffer->id     = tid;
    write_buffer->cmd_id = new_cmd_id;
    write_buffer->action = action;
    // push new action to middle buffer, so that it becomes visible by the backend.
    write_buffer = middle_buffer.exchange(write_buffer, std::memory_order_acq_rel);
    if (write_buffer->id == timer_id_t::invalid) {
      // during the exchange, we got back the previous middle_buffer. Two scenarios can occur:
      // - the action of the previous middle_buffer is marked as processed (id == invalid). This means that the backend
      // was able to process the previously dispatched action, and we need to notify the backend again of a new pending
      // action for this timer.
      // - the action of the previous middle_buffer was not marked as processed (id != invalid). This means that the
      // backend did not have time to process the previously dispatched event, however, it still knows it has
      // events to process for this timer. We avoid notifying the backend again in this case.
      backend_signaller.notify_timer_update(tid);
    }
    srsran_sanity_check(new_cmd_id == cmd_id.load(std::memory_order_relaxed),
                        "Detected race condition for timer {}",
                        fmt::underlying(tid));
  }

  /// Called by the backend to pop the pending action for this timer. Returns false if no action is pending.
  bool pop(cmd_t& elem)
  {
    read_buffer = middle_buffer.exchange(read_buffer, std::memory_order_acq_rel);
    if (read_buffer->id != timer_id_t::invalid) {
      // There is new action to process.
      elem = *read_buffer;
      // mark the action as processed.
      read_buffer->id = timer_id_t::invalid;
      return true;
    }
    return false;
  }

private:
  timer_update_signaller& backend_signaller;

  /// \brief Identifier of the last command sent from the front-end to the back-end side of the timer_manager. We use
  /// this value to track whether a timer run has been overwritten and there is no need to trigger a timeout.
  /// Atomic is used because we let the timer_manager back-end fetch this value to get a coarse estimation of the
  /// last cmd_id at the front-end side, and avoid an unnecessary timeout callback trigger.
  /// We only used memory_order_relaxed, because the real synchronization is done via the backend_signaller and
  /// middle_buffer.
  std::atomic<cmd_id_t> cmd_id{0};

  // TODO: Consider using alignas to avoid false sharing.
  std::array<cmd_t, 3> buffers;

  std::atomic<cmd_t*> middle_buffer{&buffers[1]};
  cmd_t*              read_buffer{&buffers[0]};
  cmd_t*              write_buffer{&buffers[2]};
};

/// Frontend side of the timer.
class frontend_handle : public frontend_state
{
public:
  /// Callback triggered when timer expires. Callback updates are protected by backend lock.
  unique_function<void(timer_id_t)> timeout_callback;
  /// Task executor used to dispatch expiry callback. When set to nullptr, the timer is not allocated.
  task_executor* exec = nullptr;
  /// Pending commands to be handled by the backend.
  backend_channel backend_ch;

  frontend_handle(timer_id_t id_, timer_update_signaller& ev_signaller, std::atomic<unsigned>& cur_time_) :
    frontend_state(id_), backend_ch(ev_signaller), cur_time(cur_time_)
  {
  }

  /// Initiate destruction of timer.
  void destroy()
  {
    state = state_t::stopped;
    backend_ch.push(id, cmd_t::destroy{});
  }

  /// Set timer duration. If it is running, restart timer.
  void set(timer_duration dur)
  {
    srsran_assert(dur <= MAX_TIMER_DURATION, "Invalid timer duration ({}>{})", dur.count(), MAX_TIMER_DURATION.count());
    duration = dur;
    if (state == state_t::running) {
      // If we are setting the timer when it is already running, force run restart.
      backend_ch.push(id, cmd_t::start{duration});
    }
  }

  void set(timer_duration dur, unique_function<void(timer_id_t)> callback_)
  {
    set(dur);
    // Note: we can set the timeout_callback after pushing the cmd to the backend, because the callback is going to be
    // run in the frontend (via the executor defer call).
    timeout_callback = std::move(callback_);
  }

  void run()
  {
    srsran_assert(duration != INVALID_DURATION, "Calling timer::run with invalid duration");
    state = state_t::running;
    backend_ch.push(id, cmd_t::start{duration});
  }

  void stop()
  {
    state = state_t::stopped;
    backend_ch.push(id, cmd_t::stop{});
  }

  tick_point_t now() const { return cur_time.load(std::memory_order_relaxed); }

private:
  std::atomic<unsigned>& cur_time;
};

} // namespace

/// Implementation of timer_manager.
class timer_manager::manager_impl
{
public:
  /// Timer context used solely by the back-end side of the timer manager.
  struct timer_backend_context {
    /// Last command to be handled in the timer backend.
    cmd_id_t cmd_id = 0;
    /// Current stand of the timer from the backend perspective.
    state_t state = state_t::stopped;
    /// Timeout set for the last timer run.
    unsigned timeout = 0;
  };

  /// Object holding both the front-end and back-end contexts of the timers.
  struct timer_handle : public intrusive_double_linked_list_element<>, public intrusive_forward_list_element<> {
    std::unique_ptr<frontend_handle> frontend;
    timer_backend_context            backend;

    timer_handle() = default;
  };

  manager_impl(size_t capacity) :
    logger(srslog::fetch_basic_logger("ALL")),
    timer_free_list(capacity),
    time_wheel(WHEEL_SIZE),
    pending_timers_to_create(capacity),
    timers_with_pending_events(logger)
  {
    // Pre-reserve timers.
    for (unsigned i = 0; i != capacity; ++i) {
      timers.emplace_back().frontend = std::make_unique<frontend_handle>(
          static_cast<timer_id_t>(next_timer_id.fetch_add(1, std::memory_order_relaxed)),
          timers_with_pending_events,
          cur_time);
    }

    // Push to free list in ascending id order.
    for (const auto& t : timers) {
      timer_free_list.enqueue(t.frontend.get());
    }
  }

  /// \brief Handle pending timer creation events.
  void handle_pending_timer_creations()
  {
    std::unique_ptr<frontend_handle> new_frontend;
    while (pending_timers_to_create.try_dequeue(new_frontend)) {
      auto timer_idx = static_cast<unsigned>(new_frontend->id);
      srsran_assert(timer_idx >= timers.size() or timers[timer_idx].frontend == nullptr,
                    "Duplicate timer id detection");

      if (timer_idx >= timers.size()) {
        // Resize timers list if needed.
        timers.resize(timer_idx + 1);
      }

      // Connect frontend to backend.
      timers[timer_idx].frontend = std::move(new_frontend);
    }
  }

  /// Handle in the timer manager backend side the command sent by the timer frontends
  void handle_timer_commands();

  /// Handle in the timer manager backend side, a single command sent by the respective timer frontend.
  void handle_timer_updates(timer_handle& timer);

  /// Stop a timer from ticking. If \c expiry_reason is set to true, the timer callback is dispatched to the frontend
  /// execution context.
  bool try_stop_timer_backend(timer_handle& timer, bool expiry_reason);
  void stop_timer_backend(timer_handle& timer, bool expiry_reason)
  {
    srsran_assert(timer.backend.state == state_t::running, "Stopping timer that was not running");
    try_stop_timer_backend(timer, expiry_reason);
  }

  /// Dispatch timeout handling to the front-end side.
  static bool trigger_timeout_handling(timer_handle& timer);

  /// Handle timers for which the timeout handling dispatch failed and was postponed.
  void handle_postponed_timeouts();

  /// Start the ticking of a timer with a given duration.
  void start_timer_backend(timer_handle& timer, timer_duration duration)
  {
    srsran_assert(timer.backend.state != state_t::running, "Invalid timer state");
    srsran_assert(timer.frontend != nullptr, "Invalid timer state");

    timer.backend.timeout = cur_time.load(std::memory_order_relaxed) + std::max((unsigned)duration.count(), 1U);
    timer.backend.state   = state_t::running;
    time_wheel[timer.backend.timeout & WHEEL_MASK].push_front(&timer);
    ++nof_timers_running;
  }

  /// Called to destroy a timer context and return it back to the free timer pool.
  void destroy_timer_backend(timer_handle& timer)
  {
    srsran_assert(timer.backend.state != state_t::running, "Destroying timer that is running not allowed");

    // Clear frontend (it is already released by unique_timer).
    timer.frontend->state            = state_t::stopped;
    timer.frontend->duration         = INVALID_DURATION;
    timer.frontend->timeout_callback = {};
    timer.frontend->exec             = nullptr;
    // Clear backend.
    timer.backend.state   = state_t::stopped;
    timer.backend.timeout = 0;
    // Add timer handle in free list.
    timer_free_list.enqueue(timer.frontend.get());
  }

  /// \brief Create a new front-end context to be used by a newly created unique_timer.
  frontend_handle& create_frontend_timer(task_executor& exec);

  srslog::basic_logger& logger;

  /// Counter of the number of ticks elapsed. This counter gets incremented on every \c tick call.
  std::atomic<tick_point_t> cur_time = 0;

  /// Number of created timer_handle objects that are currently running.
  size_t nof_timers_running = 0;

  /// \brief List of timer_handle objects currently being managed by the timer_manager class. The index of the element
  /// in this container matches the id of the corresponding unique_timer object.
  /// This container will only grow in size. "Deleted" unique_timers in the front-end side are just tagged as empty
  /// in their respective timer_handle, in the back-end side. This allows already created timer_handle objects to be
  /// reused for the creation of new unique_timers in the front-end. The fact that the timer_handle is not deleted
  /// also avoids potential dangling pointer when the timer expiry callback is dispatched to the front-end execution
  /// context.
  // Note: we use a deque to maintain reference validity.
  std::deque<timer_handle> timers;

  // Pool of timers that can be reused.
  moodycamel::ConcurrentQueue<frontend_handle*> timer_free_list;
  std::atomic<unsigned>                         next_timer_id{0};

  /// \brief Timer wheel, which is circularly indexed via a running timer timeout. Collisions are resolved via an
  /// intrusive linked list stored in the timer_handle objects.
  /// For a number of running timers N, and uniform distribution of timeout values, the tick() complexity
  /// should be O(N/WHEEL_SIZE). Thus, the performance should improve with a larger WHEEL_SIZE, at the expense of more
  /// used memory.
  std::vector<intrusive_double_linked_list<timer_handle>> time_wheel;

  /// \brief This is a list of timers that already timeouted, but for some unforeseen reason,
  /// it was not possible to dispatch their respective timeout handling to the timer front-end.
  /// The timeout handling will be re-triggered in the next slot.
  std::deque<std::pair<timer_id_t, cmd_id_t>> failed_to_trigger_timers;

  /// Pending timers to be created in the backend.
  moodycamel::ConcurrentQueue<std::unique_ptr<frontend_handle>> pending_timers_to_create;

  /// Commands sent by the timer front-end to the backend.
  timer_update_signaller timers_with_pending_events;
};

void timer_manager::manager_impl::handle_timer_commands()
{
  // Dequeue timers with new updates to be processed in this tick.
  span<timer_id_t> updated_timers = timers_with_pending_events.pop_updated_timers();
  for (timer_id_t tid : updated_timers) {
    const auto t_idx = static_cast<size_t>(tid);

    if (t_idx >= timers.size() or timers[t_idx].frontend == nullptr) {
      // Timer has not been created yet.
      handle_pending_timer_creations();

      if (t_idx >= timers.size() or timers[t_idx].frontend == nullptr) {
        // Unexpected - timer creation event is nowhere to be found.
        logger.warning("Discarding update for timer={}. Cause: Timer hasn't been created", fmt::underlying(tid));
        continue;
      }
    }
    timer_handle& timer = timers[t_idx];
    srsran_assert(timer.frontend != nullptr, "Invalid timer frontend");

    handle_timer_updates(timer);
  }
}

void timer_manager::manager_impl::handle_timer_updates(timer_handle& timer)
{
  srsran_assert(timer.frontend != nullptr, "Invalid timer frontend");

  cmd_t cmd;
  if (not timer.frontend->backend_ch.pop(cmd)) {
    // This shouldn't happen, but if it happens, it is not a big deal.
    logger.warning("New timer update signal for tid={} but no pending commands", fmt::underlying(timer.frontend->id));
    return;
  }

  // Update the timer backend cmd_id to match frontend.
  timer.backend.cmd_id = cmd.cmd_id;

  if (std::holds_alternative<cmd_t::create>(cmd.action)) {
    // In case of new timer creation, there are no more actions.
    return;
  }

  // Stop timer if it is currently running, no matter which action to apply.
  try_stop_timer_backend(timer, false);

  if (std::holds_alternative<cmd_t::start>(cmd.action)) {
    // Start new timer run.
    start_timer_backend(timer, std::get<cmd_t::start>(cmd.action).duration);
  } else if (std::holds_alternative<cmd_t::destroy>(cmd.action)) {
    destroy_timer_backend(timer);
  }
}

bool timer_manager::manager_impl::try_stop_timer_backend(timer_handle& timer, bool expiry_reason)
{
  if (timer.backend.state != state_t::running) {
    return false;
  }

  // Pop timer from the timer wheel.
  time_wheel[timer.backend.timeout & WHEEL_MASK].pop(&timer);
  --nof_timers_running;

  // Update timer state in the backend.
  timer.backend.state = expiry_reason ? state_t::expired : state_t::stopped;

  if (not expiry_reason) {
    // In case of stop command, no need to do any more actions. (stop is only set by timer frontend).
    return true;
  }

  // In case of expiry, and the backend and frontend (estimate) epoches match, dispatch the callback to frontend
  // executor.
  cmd_id_t current_cmd_id_estim = timer.frontend->backend_ch.current_cmd_id();
  if (timer.backend.cmd_id == current_cmd_id_estim) {
    bool success = trigger_timeout_handling(timer);

    // When it was not possible to dispatch timeout callback to executor, postpone the dispatch.
    if (not success) {
      logger.warning("Failed to dispatch timeout handling for timer={}. Re-scheduling the handling to the next slot",
                     fmt::underlying(timer.frontend->id));
      failed_to_trigger_timers.emplace_back(timer.frontend->id, timer.backend.cmd_id);
    }
  }
  return true;
}

bool timer_manager::manager_impl::trigger_timeout_handling(timer_handle& timer)
{
  return timer.frontend->exec->defer([frontend = timer.frontend.get(), expiry_epoch = timer.backend.cmd_id]() {
    // In case, the timer state has not been updated since the task was dispatched (epoches match).
    // Note: Now that we are in the same execution context as the timer frontend, the frontend cmd_id is precise.
    if (frontend->backend_ch.current_cmd_id() == expiry_epoch) {
      srsran_assert(frontend->state == state_t::running, "The timer can only expire if it was already running");
      // Update timer frontend state to expired.
      frontend->state = state_t::expired;

      // Run callback if configured.
      if (not frontend->timeout_callback.is_empty()) {
        frontend->timeout_callback(frontend->id);
      }
    }
  });
}

void timer_manager::manager_impl::handle_postponed_timeouts()
{
  while (not failed_to_trigger_timers.empty()) {
    timer_handle& timer       = timers[(size_t)failed_to_trigger_timers.front().first];
    cmd_id_t      prev_cmd_id = failed_to_trigger_timers.front().second;

    if (timer.backend.cmd_id == prev_cmd_id and timer.backend.cmd_id == timer.frontend->backend_ch.current_cmd_id() and
        not trigger_timeout_handling(timer)) {
      // Timeout handling dispatch failed again. No point in continuing loop.
      break;
    }

    // Either the cmd_id was updated or the timeout dispatch was successful. Remove element from the list.
    failed_to_trigger_timers.pop_front();
  }
}

frontend_handle& timer_manager::manager_impl::create_frontend_timer(task_executor& exec)
{
  // Pop cached timer from pool of unused timers.
  frontend_handle* cached_timer;
  if (timer_free_list.try_dequeue(cached_timer)) {
    srsran_assert(cached_timer != nullptr, "Invalid timer cached");
    srsran_assert(cached_timer->exec == nullptr, "Reassignment of timer detected");
    // Assign new executor to created timer.
    cached_timer->exec = &exec;
    return *cached_timer;
  }

  // In case it fails to reuse a cached timer frontend object, we create a new one.
  const auto id         = (timer_id_t)next_timer_id.fetch_add(1, std::memory_order_relaxed);
  auto       new_handle = std::make_unique<frontend_handle>(id, timers_with_pending_events, cur_time);
  new_handle->exec      = &exec;
  cached_timer          = new_handle.get();

  // Forward created timer handle to the backend.
  pending_timers_to_create.enqueue(std::move(new_handle));
  cached_timer->backend_ch.push(cached_timer->id, cmd_t::create{});

  return *cached_timer;
}

// timer_manager methods.

timer_manager::timer_manager(size_t capacity) : impl(std::make_unique<manager_impl>(capacity)) {}

timer_manager::~timer_manager() = default;

void timer_manager::tick()
{
  // Dequeue new commands from the timer front-ends to be processed in this tick.
  impl->handle_timer_commands();

  // Re-trigger timeout for timers that failed to be triggered in the previous slot.
  impl->handle_postponed_timeouts();

  // Advance time.
  unsigned cur_time = impl->cur_time.fetch_add(1, std::memory_order_relaxed) + 1;

  // Process the timer runs which expire in this tick.
  auto& wheel_list = impl->time_wheel[cur_time & WHEEL_MASK];

  // Iterate intrusive linked list of running timers with same wheel index.
  for (auto it = wheel_list.begin(); it != wheel_list.end();) {
    srsran_assert(it->frontend != nullptr, "invalid state of timer in timer wheel");
    manager_impl::timer_handle& timer = impl->timers[static_cast<size_t>(it->frontend->id)];
    // We move iterator already, in case, the current timer gets removed from the linked list.
    ++it;

    // If the timer does not expire yet, continue the iteration in the same wheel bucket.
    if (cur_time != timer.backend.timeout) {
      continue;
    }

    // Stop timer. Note: callback has to see the timer has already expired.
    impl->stop_timer_backend(timer, true);
  }
}

unique_timer timer_manager::create_unique_timer(task_executor& exec)
{
  return unique_timer(impl->create_frontend_timer(exec));
}

size_t timer_manager::nof_timers() const
{
  return impl->timers.size() - std::min(impl->timers.size(), impl->timer_free_list.size_approx());
}

size_t timer_manager::nof_running_timers() const
{
  return impl->nof_timers_running;
}

tick_point_t timer_manager::now() const
{
  return impl->cur_time.load(std::memory_order_relaxed);
}

// unique_timer methods

unique_timer& unique_timer::operator=(unique_timer&& other) noexcept
{
  reset();
  handle = std::exchange(other.handle, nullptr);
  return *this;
}

unique_timer::~unique_timer()
{
  reset();
}

void unique_timer::reset()
{
  if (is_valid()) {
    static_cast<frontend_handle*>(handle)->destroy();
    handle = nullptr;
  }
}

void unique_timer::set(timer_duration duration, unique_function<void(timer_id_t)> callback)
{
  srsran_assert(is_valid(), "Trying to setup empty timer pimpl");
  static_cast<frontend_handle*>(handle)->set(duration, std::move(callback));
}

void unique_timer::set(timer_duration duration)
{
  srsran_assert(is_valid(), "Trying to setup empty timer pimpl");
  static_cast<frontend_handle*>(handle)->set(duration);
}

void unique_timer::run()
{
  srsran_assert(is_valid(), "Starting invalid timer");
  static_cast<frontend_handle*>(handle)->run();
}

void unique_timer::stop()
{
  if (is_running()) {
    static_cast<frontend_handle*>(handle)->stop();
  }
}

tick_point_t unique_timer::now() const
{
  srsran_assert(is_valid(), "Getting tick from invalid timer");
  return static_cast<frontend_handle*>(handle)->now();
}

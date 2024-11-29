/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/timers.h"
#include "cameron314/concurrentqueue.h"
#include "srsran/adt/span.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

/// Timer Wheel configuration parameters.
static constexpr size_t WHEEL_SHIFT = 16U;
static constexpr size_t WHEEL_SIZE  = 1U << WHEEL_SHIFT;
static constexpr size_t WHEEL_MASK  = WHEEL_SIZE - 1U;

/// Maximum timeout duration supported for a given timer in ticks.
static constexpr timer_duration MAX_TIMER_DURATION = timer_duration{std::numeric_limits<unsigned>::max() / 2};

void timer_manager::timer_frontend::destroy()
{
  cmd_id_t new_cmd_id = cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
  state               = state_t::stopped;
  parent.push_timer_command(cmd_t{id, new_cmd_id, cmd_t::destroy{}});
}

void timer_manager::timer_frontend::set(timer_duration dur)
{
  srsran_assert(dur <= MAX_TIMER_DURATION, "Invalid timer duration ({}>{})", dur.count(), MAX_TIMER_DURATION.count());

  duration = dur;
  if (state == state_t::running) {
    cmd_id_t new_cmd_id = cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
    // If we are setting the timer when it is already running, force run restart.
    parent.push_timer_command(cmd_t{id, new_cmd_id, cmd_t::start{duration}});
  }
}

void timer_manager::timer_frontend::set(timer_duration dur, unique_function<void(timer_id_t)> callback_)
{
  set(dur);
  // Note: we can set the timeout_callback after pushing the cmd to the backend, because the callback is going to be
  // run in the frontend (via the executor defer call).
  timeout_callback = std::move(callback_);
}

void timer_manager::timer_frontend::run()
{
  srsran_assert(duration != INVALID_DURATION, "Calling timer::run with invalid duration");
  cmd_id_t new_cmd_id = cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
  state               = state_t::running;
  parent.push_timer_command(cmd_t{id, new_cmd_id, cmd_t::start{duration}});
}

void timer_manager::timer_frontend::stop()
{
  cmd_id_t new_cmd_id = cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
  state               = state_t::stopped;
  parent.push_timer_command(cmd_t{id, new_cmd_id, cmd_t::stop{}});
}

//

class timer_manager::unique_timer_pool
{
public:
  unique_timer_pool(timer_manager& parent, unsigned capacity) : free_list(capacity) {}

  void push(timer_manager::timer_frontend* obj) { free_list.enqueue(obj); }

  timer_manager::timer_frontend* pop()
  {
    timer_manager::timer_frontend* ret;
    if (free_list.try_dequeue(ret)) {
      return ret;
    }
    return nullptr;
  }

  size_t size_approx() const { return free_list.size_approx(); }

private:
  // List of timer_handle objects in timer_list that are currently not allocated.
  moodycamel::ConcurrentQueue<timer_manager::timer_frontend*> free_list;
};

//

class timer_manager::command_queue
{
  constexpr static size_t initial_batch_size            = 128;
  constexpr static size_t warn_on_nof_dequeues_per_tick = 4096U;
  constexpr static size_t default_queue_capacity        = 16384;

public:
  explicit command_queue(srslog::basic_logger& logger_, size_t initial_capacity = default_queue_capacity) :
    logger(logger_), commands(initial_capacity), cmd_dequeuer(commands), temp_cmd_buffer(initial_batch_size)
  {
  }

  void push_command(cmd_t cmd) { commands.enqueue(std::move(cmd)); }

  span<cmd_t> pop_commands()
  {
    size_t      nof_popped = dequeue_events(commands, cmd_dequeuer, temp_cmd_buffer);
    span<cmd_t> list{temp_cmd_buffer.data(), nof_popped};

    // Note: commands may arrive out-of-order due to the nature of this MPMC queue. We will order based on cmd_id, so
    // that it is easier to process by the backend.
    std::sort(list.begin(), list.end(), [](const cmd_t& lhs, const cmd_t& rhs) { return lhs.cmd_id < rhs.cmd_id; });
    return list;
  }

private:
  using timer_creation_queue = moodycamel::ConcurrentQueue<std::unique_ptr<timer_frontend>>;

  template <typename T>
  size_t dequeue_events(moodycamel::ConcurrentQueue<T>&                            q,
                        typename moodycamel::ConcurrentQueue<T>::consumer_token_t& t,
                        std::vector<T>&                                            buffer)
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

  // Queue of new commands.
  moodycamel::ConcurrentQueue<cmd_t>                   commands;
  moodycamel::ConcurrentQueue<cmd_t>::consumer_token_t cmd_dequeuer;
  std::vector<cmd_t>                                   temp_cmd_buffer;
};

//

timer_manager::timer_manager(size_t capacity) :
  logger(srslog::fetch_basic_logger("ALL")),
  timer_pool(std::make_unique<unique_timer_pool>(*this, capacity)),
  time_wheel(WHEEL_SIZE),
  pending_cmds(std::make_unique<command_queue>(logger))
{
  // Pre-reserve timers.
  while (timers.size() < capacity) {
    timers.emplace_back().frontend =
        std::make_unique<timer_frontend>(*this, (timer_id_t)next_timer_id.fetch_add(1, std::memory_order_relaxed));
  }

  // Push to free list in ascending id order.
  for (const auto& t : timers) {
    timer_pool->push(t.frontend.get());
  }
}

timer_manager::~timer_manager() {}

void timer_manager::tick()
{
  // Dequeue new commands from the timer front-ends to be processed in this tick.
  handle_timer_commands();

  // Re-trigger timeout for timers that failed to be triggered in the previous slot.
  handle_postponed_timeouts();

  // Advance time.
  ++cur_time;

  // Process the timer runs which expire in this tick.
  auto& wheel_list = time_wheel[cur_time & WHEEL_MASK];

  // Iterate intrusive linked list of running timers with same wheel index.
  for (auto it = wheel_list.begin(); it != wheel_list.end();) {
    srsran_assert(it->frontend != nullptr, "invalid state of timer in timer wheel");
    timer_handle& timer = timers[static_cast<size_t>(it->frontend->id)];
    // We move iterator already, in case, the current timer gets removed from the linked list.
    ++it;

    // If the timer does not expire yet, continue the iteration in the same wheel bucket.
    if (cur_time != timer.backend.timeout) {
      continue;
    }

    // Stop timer. Note: callback has to see the timer has already expired.
    stop_timer_backend(timer, true);
  }
}

void timer_manager::create_timer_handle(cmd_id_t cmd_id, std::unique_ptr<timer_frontend> timer)
{
  auto timer_idx = static_cast<unsigned>(timer->id);
  srsran_assert(timer_idx >= timers.size() or timers[timer_idx].frontend == nullptr, "Duplicate timer id detection");
  if (timer_idx >= timers.size()) {
    timers.resize(timer_idx + 1);
  }
  timers[timer_idx].frontend = std::move(timer);
  srsran_sanity_check(timers[timer_idx].backend.cmd_id + 1 == cmd_id, "Expected cmd_id to not change");
  timers[timer_idx].backend.cmd_id = cmd_id;
}

void timer_manager::handle_timer_commands()
{
  // Dequeue new commands from the timer front-ends to be processed in this tick.
  span<cmd_t> new_cmds = pending_cmds->pop_commands();
  for (cmd_t& cmd : new_cmds) {
    if (std::holds_alternative<cmd_t::create>(cmd.action)) {
      // Create new timer.
      create_timer_handle(cmd.cmd_id, std::move(std::get<cmd_t::create>(cmd.action).frontend));
      continue;
    }
    const auto t_idx = static_cast<size_t>(cmd.id);

    if (t_idx >= timers.size()) {
      // Second command ended up being processed before timer creation due to reordering.
      // Grow timers list to avoid bad accesses. The cmd will be added to tmp_skipped_cmds below in this iteration.
      if (t_idx > timers.size() + 1024) {
        logger.error("Detected corrupted timer_id={}. Current timer list size is {}", t_idx, timers.size());
      }
      timers.resize(t_idx + 1);
    }

    // The timer already exists.
    timer_handle& timer = timers[t_idx];

    if (cmd.cmd_id - timer.backend.cmd_id >= std::numeric_limits<cmd_id_t>::max() / 2) {
      // Note: This should not happen. It means that there was some corruption of the cmd_id.
      logger.warning(
          "Discarding cmd_id={} for timer={}. Cause: cmd_id is below the last processed cmd_id={} by the timer",
          cmd.cmd_id,
          fmt::underlying(cmd.id),
          timer.backend.cmd_id);
      continue;
    }
    if (cmd.cmd_id - timer.backend.cmd_id > 1) {
      // We detected a discontinuity in the cmd_id. It could be due to dequeue reordering.
      // Cache the command for later processing.
      tmp_skipped_cmds.emplace_back(cur_time, std::move(cmd));
      logger.debug("The processing of cmd_id={} for timer={} was postponed. Cause: There are commands in between "
                   "[{},{}) not yet processed",
                   cmd.cmd_id,
                   fmt::underlying(cmd.id),
                   timer.backend.cmd_id + 1,
                   cmd.cmd_id);
      continue;
    }

    // Handle timer command.
    handle_timer_command(timer, cmd);
  }

  constexpr static unsigned max_skip_slot_thres = 16;
  // Sort skipped cmds by cmd_id.
  std::sort(tmp_skipped_cmds.begin(), tmp_skipped_cmds.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.second.cmd_id < rhs.second.cmd_id;
  });
  for (auto& p : tmp_skipped_cmds) {
    cmd_t& cmd = p.second;
    if (cmd.id == timer_id_t::invalid) {
      // already processed.
      continue;
    }
    const auto t_idx = static_cast<size_t>(cmd.id);

    timer_handle& timer = timers[t_idx];

    if (cmd.cmd_id == timer.backend.cmd_id + 1) {
      // Now the command can be processed in order.
      handle_timer_command(timer, cmd);
      cmd.id = timer_id_t::invalid;
    } else if (cmd.cmd_id - timer.backend.cmd_id >= std::numeric_limits<cmd_id_t>::max() / 2) {
      // invalid cmd_id.
      logger.warning(
          "Discarding cmd_id={} for timer={}. Cause: cmd_id is below the last processed cmd_id={} by the timer",
          cmd.cmd_id,
          t_idx,
          timer.backend.cmd_id);
      cmd.id = timer_id_t::invalid;
    } else if (cur_time - p.first > max_skip_slot_thres) {
      logger.warning("Discarding cmd_ids=[{},{}) for timer={}. Cause: The cmd_ids went missing",
                     timer.backend.cmd_id + 1,
                     cmd.cmd_id - 1,
                     t_idx);
      if (timer.frontend != nullptr) {
        handle_timer_command(timer, cmd);
      }
      cmd.id = timer_id_t::invalid;
    }
  }
  // Remove commands that were initially skipped and have been processed since then.
  while (not tmp_skipped_cmds.empty() and tmp_skipped_cmds.front().second.id == timer_id_t::invalid) {
    tmp_skipped_cmds.pop_front();
  }
}

void timer_manager::handle_timer_command(timer_handle& timer, const cmd_t& cmd)
{
  srsran_sanity_check(not std::holds_alternative<cmd_t::create>(cmd.action), "Invalid action type");
  srsran_assert(timer.frontend != nullptr, "Invalid timer frontend");

  // Update the timer backend cmd_id to match frontend.
  timer.backend.cmd_id = cmd.cmd_id;

  // Stop timer if it is currently running, no matter which action.
  try_stop_timer_backend(timer, false);

  if (std::holds_alternative<cmd_t::start>(cmd.action)) {
    // Start new timer run.
    start_timer_backend(timer, std::get<cmd_t::start>(cmd.action).duration);
  } else if (std::holds_alternative<cmd_t::destroy>(cmd.action)) {
    destroy_timer_backend(timer);
  }
}

void timer_manager::start_timer_backend(timer_handle& timer, timer_duration duration)
{
  srsran_assert(timer.backend.state != state_t::running, "Invalid timer state");
  srsran_assert(timer.frontend != nullptr, "Invalid timer state");

  timer.backend.timeout = cur_time + std::max((unsigned)duration.count(), 1U);
  timer.backend.state   = state_t::running;
  time_wheel[timer.backend.timeout & WHEEL_MASK].push_front(&timer);
  ++nof_timers_running;
}

bool timer_manager::trigger_timeout_handling(timer_handle& timer)
{
  return timer.frontend->exec->defer([frontend = timer.frontend.get(), expiry_epoch = timer.backend.cmd_id]() {
    // In case, the timer state has not been updated since the task was dispatched (epoches match).
    // Note: Now that we are in the same execution context as the timer frontend, the frontend cmd_id is precise.
    if (frontend->cmd_id.load(std::memory_order_relaxed) == expiry_epoch) {
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

bool timer_manager::try_stop_timer_backend(timer_handle& timer, bool expiry_reason)
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
  cmd_id_t current_cmd_id_estim = timer.frontend->cmd_id.load(std::memory_order_relaxed);
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

void timer_manager::handle_postponed_timeouts()
{
  while (not failed_to_trigger_timers.empty()) {
    timer_handle& timer       = timers[(size_t)failed_to_trigger_timers.front().first];
    cmd_id_t      prev_cmd_id = failed_to_trigger_timers.front().second;

    if (timer.backend.cmd_id == prev_cmd_id and
        timer.backend.cmd_id == timer.frontend->cmd_id.load(std::memory_order_relaxed) and
        not trigger_timeout_handling(timer)) {
      // Timeout handling dispatch failed again. No point in continuing loop.
      break;
    }

    // Either the cmd_id was updated or the timeout dispatch was successful. Remove element from the list.
    failed_to_trigger_timers.pop_front();
  }
}

void timer_manager::stop_timer_backend(timer_handle& timer, bool expiry_reason)
{
  srsran_assert(timer.backend.state == state_t::running, "Stopping timer that was not running");
  try_stop_timer_backend(timer, expiry_reason);
}

void timer_manager::destroy_timer_backend(timer_handle& timer)
{
  srsran_assert(timer.backend.state != state_t::running, "Destroying timer that is running not allowed");

  // Clear frontend (it is already released by unique_timer).
  timer.frontend->state            = state_t::stopped;
  timer.frontend->duration         = timer_manager::INVALID_DURATION;
  timer.frontend->timeout_callback = {};
  timer.frontend->exec             = nullptr;
  // Clear backend.
  timer.backend.state   = state_t::stopped;
  timer.backend.timeout = 0;
  // Add timer handle in free list.
  timer_pool->push(timer.frontend.get());
}

timer_manager::timer_frontend& timer_manager::create_frontend_timer(task_executor& exec)
{
  // Pop cached timer from pool.
  timer_frontend* cached_timer = timer_pool->pop();
  if (cached_timer != nullptr) {
    srsran_assert(cached_timer->exec == nullptr, "Reassignment of timer detected");
    // Assign new executor to created timer.
    cached_timer->exec = &exec;
    return *cached_timer;
  }

  // In case it fails to reuse a cached timer frontend object. Need to create a new one.
  const auto id         = (timer_id_t)next_timer_id.fetch_add(1, std::memory_order_relaxed);
  auto       new_handle = std::make_unique<timer_frontend>(*this, id);
  new_handle->exec      = &exec;
  cached_timer          = new_handle.get();

  // Forward created timer handle to the backend.
  // Note: This cannot fail, otherwise the created timer "id" cannot be reused.
  auto next_cmd_id = cached_timer->cmd_id.fetch_add(1, std::memory_order_relaxed) + 1;
  pending_cmds->push_command(cmd_t{cached_timer->id, next_cmd_id, cmd_t::create{std::move(new_handle)}});

  return *cached_timer;
}

void timer_manager::push_timer_command(cmd_t cmd)
{
  pending_cmds->push_command(std::move(cmd));
}

unique_timer timer_manager::create_unique_timer(task_executor& exec)
{
  return unique_timer(create_frontend_timer(exec));
}

size_t timer_manager::nof_timers() const
{
  return timers.size() - std::min(timers.size(), timer_pool->size_approx());
}

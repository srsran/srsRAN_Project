/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/strong_type.h"
#include "srsran/adt/unique_function.h"
#include <string>
#include <thread>

namespace srsran {

/// Computes the number of threads that are usable in the given host.
size_t compute_host_nof_hardware_threads();

/// OS thread RT scheduling priority.
/// Note: posix defines a minimum spread between sched_get_priority_max() and sched_get_priority_min() of 32.
struct os_thread_realtime_priority_tag {};
class os_thread_realtime_priority
  : public strong_type<int, struct os_thread_realtime_priority_tag, strong_equality, strong_comparison>
{
  static int max_posix_prio()
  {
    // Subtract one to the priority offset to avoid scheduling threads with the highest priority that could contend with
    // OS critical tasks.
    static int value = sched_get_priority_max(SCHED_FIFO) - 1;
    return value;
  }
  static int min_posix_prio()
  {
    static int value = sched_get_priority_min(SCHED_FIFO);
    return value;
  }

public:
  /// Creates non-realtime thread priority
  os_thread_realtime_priority() : strong_type(0) {}

  /// Returns a strong-type for OS thread priority set to non-realtime.
  static os_thread_realtime_priority no_realtime() { return os_thread_realtime_priority{}; }

  /// Returns a strong-type for OS thread priority set to max priority.
  static os_thread_realtime_priority max() { return os_thread_realtime_priority{max_posix_prio()}; }

  /// Returns a strong-type for OS thread priority set to min priority.
  static os_thread_realtime_priority min() { return os_thread_realtime_priority{min_posix_prio()}; }

  /// Increase thread priority by \c prio_diff.
  os_thread_realtime_priority& operator+=(int prio_diff)
  {
    *this = os_thread_realtime_priority{value() + prio_diff};
    return *this;
  }
  os_thread_realtime_priority operator+(int prio_diff) const
  {
    os_thread_realtime_priority ret{*this};
    return ret += prio_diff;
  }

  /// Decrease thread priority by \c prio_diff.
  os_thread_realtime_priority& operator-=(int prio_diff)
  {
    *this = os_thread_realtime_priority{value() - prio_diff};
    return *this;
  }
  os_thread_realtime_priority operator-(int prio_diff) const
  {
    os_thread_realtime_priority ret{*this};
    return ret -= prio_diff;
  }

  /// Returns Posix OS thread priority value within {0, ..., sched_get_priority_max(SCHED_FIFO)}.
  int native() const { return strong_type::value(); }

  int native_sched_policy() const { return *this == no_realtime() ? SCHED_OTHER : SCHED_FIFO; }

private:
  /// Creates real-time thread priority value based on POSIX priority.
  /// \param[in] posix_prio Posix priority value between sched_get_priority_min(SCHED_FIFO) and
  /// sched_get_priority_max(SCHED_FIFO).
  explicit os_thread_realtime_priority(int posix_prio) :
    strong_type(std::max(min_posix_prio(), std::min(max_posix_prio(), posix_prio)))
  {
  }
};

/// CPU affinity bitmap.
struct os_sched_affinity_bitmask {
public:
  os_sched_affinity_bitmask() : cpu_bitset(compute_host_nof_hardware_threads()) {}

  explicit os_sched_affinity_bitmask(size_t cpu_idx) : cpu_bitset(compute_host_nof_hardware_threads()) { set(cpu_idx); }

  size_t size() const { return cpu_bitset.size(); }

  void set(size_t cpu_idx) { cpu_bitset.set(cpu_idx); }

  bool test(size_t cpu_idx) const { return cpu_bitset.test(cpu_idx); }

  bool any() const { return cpu_bitset.any(); }

  uint64_t to_uint64() const { return cpu_bitset.to_uint64(); }

private:
  bounded_bitset<1024> cpu_bitset;
};

/// Unique thread wrapper that ensures the thread is joined on destruction and provides an interface to set/get
/// the thread name, priority, cpu mask.
class unique_thread
{
public:
  /// Creates a unique_thread object with no associated OS thread.
  unique_thread() = default;

  /// Creates a unique_thread object with an associated OS thread with custom attributes.
  template <typename Callable>
  unique_thread(std::string                      name_,
                os_thread_realtime_priority      prio,
                const os_sched_affinity_bitmask& cpu_mask,
                Callable&&                       c) :
    name(std::move(name_)), thread_handle(make_thread(name, std::forward<Callable>(c), prio, cpu_mask))
  {
  }

  /// Creates a unique_thread object with an associated OS thread with custom scheduling priority.
  template <typename Callable>
  unique_thread(std::string name_, os_thread_realtime_priority prio, Callable&& c) :
    name(std::move(name_)), thread_handle(make_thread(name, std::forward<Callable>(c), prio))
  {
  }

  /// Creates a unique_thread object with an associated OS thread with custom cpu affinity.
  template <typename Callable>
  unique_thread(std::string name_, const os_sched_affinity_bitmask& cpu_mask, Callable&& c) :
    name(std::move(name_)),
    thread_handle(make_thread(name, std::forward<Callable>(c), os_thread_realtime_priority::no_realtime(), cpu_mask))
  {
  }

  /// Creates a unique_thread object with an associated OS thread with default attributes.
  template <typename Callable>
  unique_thread(std::string name_, Callable&& c) :
    name(std::move(name_)), thread_handle(make_thread(name, std::forward<Callable>(c)))
  {
  }

  unique_thread(const unique_thread&) = delete;

  unique_thread(unique_thread&& other) noexcept = default;

  ~unique_thread() { join(); }

  unique_thread& operator=(const unique_thread&) = delete;

  unique_thread& operator=(unique_thread&& other) noexcept = default;

  /// Joins thread if it is running.
  void join()
  {
    if (thread_handle.joinable()) {
      thread_handle.join();
    }
  }

  /// Gets thread name.
  const char* get_name() const { return name.c_str(); }

  /// Gets C++ thread id.
  std::thread::id get_id() const { return thread_handle.get_id(); }

  /// Checks whether thread is running.
  bool running() const { return thread_handle.joinable(); }

  /// Print to console the current thread priority.
  void print_priority();

private:
  /// Starts thread with provided name and attributes.
  static std::thread make_thread(const std::string&               name,
                                 unique_function<void()>          callable,
                                 os_thread_realtime_priority      prio     = os_thread_realtime_priority::no_realtime(),
                                 const os_sched_affinity_bitmask& cpu_mask = {});

  /// Thread name.
  std::string name;

  std::thread thread_handle;
};

/// Get caller thread name.
const char* this_thread_name();

/// Print caller thread priority.
void print_this_thread_priority();

} // namespace srsran

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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/unique_function.h"
#include <string>
#include <thread>

namespace srsgnb {

/// Computes the number of threads that are usable in the given host.
size_t compute_host_nof_hardware_threads();

/// OS thread RT scheduling priority.
/// Note: posix defines a minimum spread between sched_get_priority_max() and sched_get_priority_min() of 32.
enum class os_thread_realtime_priority : int { NO_REALTIME = 0, MIN_PRIO = 1, MAX_PRIO = 32 };

/// CPU affinity bitmap.
struct os_sched_affinity_bitmask {
public:
  os_sched_affinity_bitmask() : cpu_bitset(compute_host_nof_hardware_threads()) {}

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
    thread_handle(make_thread(name, std::forward<Callable>(c), os_thread_realtime_priority::NO_REALTIME, cpu_mask))
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
                                 os_thread_realtime_priority      prio     = os_thread_realtime_priority::NO_REALTIME,
                                 const os_sched_affinity_bitmask& cpu_mask = {});

  /// Thread name.
  std::string name;

  std::thread thread_handle;
};

/// Get caller thread name.
const char* this_thread_name();

/// Print caller thread priority.
void print_this_thread_priority();

} // namespace srsgnb

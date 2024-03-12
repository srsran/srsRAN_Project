/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cpu_architecture_info.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/strong_type.h"
#include "srsran/adt/unique_function.h"
#include <string>
#include <thread>

namespace srsran {

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
  static constexpr size_t MAX_CPUS = 1024;

  os_sched_affinity_bitmask() : cpu_bitset(cpu_architecture_info::get().get_host_total_nof_cpus()) {}

  explicit os_sched_affinity_bitmask(size_t cpu_idx) :
    cpu_bitset(cpu_architecture_info::get().get_host_total_nof_cpus())
  {
    set(cpu_idx);
  }

  os_sched_affinity_bitmask(size_t bitset_size, size_t cpu_idx) : cpu_bitset(bitset_size) { set(cpu_idx); }

  size_t size() const { return cpu_bitset.size(); }

  void set(size_t cpu_idx) { cpu_bitset.set(cpu_idx); }

  /// Returns a bitmap of the CPUs available to the application.
  static const os_sched_affinity_bitmask& available_cpus();

  /// \brief Finds, within a range of CPU indexes, the lowest CPU enabled.
  /// \param[in] start_cpu_index Starting CPU index for the search.
  /// \param[in] end_cpu_index End CPU index for the search.
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  int find_lowest(size_t start_cpu_index, size_t end_cpu_index)
  {
    return cpu_bitset.find_lowest(start_cpu_index, end_cpu_index);
  }

  /// Returns true if no CPU is enabled.
  bool none() const { return cpu_bitset.none(); }

  /// \brief Fills range of bits to true.
  /// \param[in] start Starting bit index that will be set.
  /// \param[in] end End bit index (excluding) where the bits stop being set.
  void fill(size_t start, size_t end) { cpu_bitset.fill(start, end); }

  bool test(size_t cpu_idx) const { return cpu_bitset.test(cpu_idx); }

  bool any() const { return cpu_bitset.any(); }

  bool all() const { return cpu_bitset.all(); }

  uint64_t to_uint64() const { return cpu_bitset.to_uint64(); }

  /// \brief Number of CPUs enabled in the bitmask.
  size_t count() const { return cpu_bitset.count(); }

  /// \brief Returns the list of CPU Ids contained in \c this bitmask but not in the \c rhs bitmask.
  static_vector<size_t, os_sched_affinity_bitmask::MAX_CPUS> subtract(const os_sched_affinity_bitmask& rhs) const;

  /// \brief Get CPU ids of the affinity mask.
  static_vector<size_t, os_sched_affinity_bitmask::MAX_CPUS> get_cpu_ids() const
  {
    return cpu_bitset.get_bit_positions();
  }

private:
  bounded_bitset<MAX_CPUS> cpu_bitset;
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

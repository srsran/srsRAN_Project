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

#include "srsran/support/executors/unique_thread.h"
#include "srsran/adt/mpmc_queue.h"
#include "fmt/std.h"
#include <cstdio>
#include <mutex>
#include <pthread.h>
#include <sys/types.h>

using namespace srsran;

/// Sets thread OS scheduling real-time priority.
static bool thread_set_param(::pthread_t t, os_thread_realtime_priority prio)
{
  sched_param param{};

  param.sched_priority = prio.native();
  if (::pthread_setschedparam(t, prio.native_sched_policy(), &param) != 0) {
    fmt::println(stderr,
                 "Warning: Scheduling priority of thread \"{}\" not changed. Cause: Not enough privileges.",
                 this_thread_name());
    return false;
  }

  return true;
}

static bool thread_set_affinity(::pthread_t t, const os_sched_affinity_bitmask& bitmap, const std::string& name)
{
  auto invalid_ids = bitmap.subtract(os_sched_affinity_bitmask::available_cpus());
  if (!invalid_ids.empty()) {
    fmt::println("Warning: The CPU affinity of thread \"{}\" contains the following invalid CPU ids: {}",
                 name,
                 span<const size_t>(invalid_ids));
  }

  ::cpu_set_t* cpusetp     = CPU_ALLOC(bitmap.size());
  size_t       cpuset_size = CPU_ALLOC_SIZE(bitmap.size());
  CPU_ZERO_S(cpuset_size, cpusetp);

  for (size_t i = 0, e = bitmap.size(); i != e; ++i) {
    if (bitmap.test(i)) {
      CPU_SET_S(i, cpuset_size, cpusetp);
    }
  }

  int ret;
  if ((ret = ::pthread_setaffinity_np(t, cpuset_size, cpusetp)) != 0) {
    fmt::print("Couldn't set affinity for {} thread. Cause: '{}'\n", name, ::strerror(ret));
    CPU_FREE(cpusetp);
    return false;
  }

  CPU_FREE(cpusetp);
  return true;
}

static std::string compute_this_thread_name()
{
  // See Posix pthread_setname_np.
  const uint32_t    MAX_THREAD_NAME_LEN       = 16;
  char              name[MAX_THREAD_NAME_LEN] = {};
  const ::pthread_t tid                       = ::pthread_self();
  if (::pthread_getname_np(tid, name, MAX_THREAD_NAME_LEN)) {
    ::perror("Could not get pthread name");
  }

  return name;
}

static void print_thread_priority(::pthread_t t, const char* tname, std::thread::id tid)
{
  if (t == 0) {
    fmt::println("Error: Trying to print priority of invalid thread handle");
    return;
  }

  ::cpu_set_t cpuset;

  int s;

  s = ::pthread_getaffinity_np(t, sizeof(::cpu_set_t), &cpuset);
  if (s != 0) {
    fmt::println("error pthread_getaffinity_np: {}", ::strerror(s));
  }

  fmt::println("Set returned by pthread_getaffinity_np() contained:");
  for (unsigned j = 0; j != CPU_SETSIZE; ++j) {
    if (CPU_ISSET(j, &cpuset)) {
      fmt::println("    CPU {}", j);
    }
  }

  int           policy;
  ::sched_param param;
  s = ::pthread_getschedparam(t, &policy, &param);
  if (s != 0) {
    fmt::println("error pthread_getaffinity_np: {}", ::strerror(s));
  }

  const char* p;
  switch (policy) {
    case SCHED_FIFO:
      p = "SCHED_FIFO";
      break;
    case SCHED_RR:
      p = "SCHED_RR";
      break;
    default:
      p = "Other";
      break;
  }

  fmt::println("Thread [{}:{}]: Sched policy is \"{}\". Priority is {}.", tname, tid, p, param.sched_priority);
}

namespace {

/// List of observers of thread creation/deletion. This list may only grow in size.
class unique_thread_observer_list
{
public:
  void add(std::unique_ptr<unique_thread::observer> observer)
  {
    std::lock_guard<std::mutex> lock(mutex);
    observers.emplace_back(std::move(observer));
  }

  /// Called on every thread creation.
  void on_thread_creation()
  {
    // Note: we use index-based loop because list of observers may increase (never decrease) throughout the loop.
    std::unique_lock<std::mutex> lock(mutex);
    for (unsigned i = 0; i < observers.size(); ++i) {
      unique_thread::observer* observer = observers[i].get();
      lock.unlock();
      // Call observer without holding the mutex.
      observer->on_thread_creation();
      lock.lock();
    }
  }

  /// Called on every thread destruction.
  void on_thread_destruction()
  {
    std::unique_lock<std::mutex> lock(mutex);
    for (unsigned i = 0; i < observers.size(); ++i) {
      unique_thread::observer* observer = observers[i].get();
      lock.unlock();
      observer->on_thread_destruction();
      lock.lock();
    }
  }

private:
  std::mutex                                            mutex;
  std::vector<std::unique_ptr<unique_thread::observer>> observers;
};

/// Class maintaining the unique thread indexes.
class unique_thread_index_manager
{
  /// Queue type used for storing identifiers.
  using queue_type =
      concurrent_queue<unsigned, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  /// Maximum number of threads supported by the application.
  static constexpr unsigned MAX_NOF_THREADS = 256;

public:
  unique_thread_index_manager() : free_list(MAX_NOF_THREADS)
  {
    for (unsigned i = 0; i != MAX_NOF_THREADS; ++i) {
      bool discard = free_list.try_push(i);
      report_error_if_not(discard, "Failed to fill free list of unique thread identifiers");
    }
  }

  /// Returns maximum number of threads supported by the application.
  static unsigned get_max_nof_supported_threads() { return MAX_NOF_THREADS; }

  /// Get free identifier.
  unsigned get_free_identifier()
  {
    unsigned idx;
    report_error_if_not(free_list.try_pop(idx), "Failed to get a free unique thread identifier");
    return idx;
  }

  /// Release used identifier.
  void release_identifier(unsigned id)
  {
    srsran_sanity_check(id < MAX_NOF_THREADS, "Invalid unique thread identifier being released");

    bool discard = free_list.try_push(id);
    report_error_if_not(discard, "Failed to release used unique thread identifier");
  }

private:
  queue_type free_list;
};

} // namespace

static unique_thread_index_manager thread_index_manager;
thread_local unsigned              unique_thread_index;

/// Global unique list of thread lifetime observers.
static unique_thread_observer_list thread_observers;

const os_sched_affinity_bitmask& os_sched_affinity_bitmask::available_cpus()
{
  static os_sched_affinity_bitmask available_cpus_mask = []() {
    os_sched_affinity_bitmask bitmask;
    ::cpu_set_t               cpuset = cpu_architecture_info::get().get_available_cpuset();
    for (size_t i = 0, e = bitmask.size(); i != e; ++i) {
      if (CPU_ISSET(i, &cpuset)) {
        bitmask.cpu_bitset.set(i);
      }
    }
    return bitmask;
  }();

  return available_cpus_mask;
}

static_vector<size_t, os_sched_affinity_bitmask::MAX_CPUS>
os_sched_affinity_bitmask::subtract(const os_sched_affinity_bitmask& rhs) const
{
  auto invalid_bitmap = (~rhs.cpu_bitset) & cpu_bitset;
  return invalid_bitmap.get_bit_positions();
}

///////////////////////////////////////

std::thread unique_thread::make_thread(const std::string&               name,
                                       unique_function<void()>          callable,
                                       os_thread_realtime_priority      prio,
                                       const os_sched_affinity_bitmask& cpu_mask)
{
  // Launch thread.
  return std::thread([name, prio, cpu_mask, callable = std::move(callable)]() {
    std::string fixed_name = name;

    // Truncate the thread name if it exceeds the maximum length.
    static const unsigned MAX_THREADNAME_LEN = 15;
    if (fixed_name.size() > MAX_THREADNAME_LEN) {
      fixed_name.erase(MAX_THREADNAME_LEN, std::string::npos);
      fmt::println("Thread [{}]: Thread name '{}' exceeds {} characters, truncating to '{}'",
                   std::this_thread::get_id(),
                   name,
                   MAX_THREADNAME_LEN,
                   fixed_name);
    }

    ::pthread_t tself = ::pthread_self();
    if (::pthread_setname_np(tself, fixed_name.c_str()) != 0) {
      ::perror("pthread_setname_np");
      fmt::println("Thread [{}]: Error while setting thread name to {}.", std::this_thread::get_id(), name);
    }

    // Set thread OS priority and affinity.
    // Note: TSAN seems to have issues with thread attributes when running as normal user, disable them in that case.
#ifndef HAVE_TSAN
    if (prio != os_thread_realtime_priority::no_realtime()) {
      thread_set_param(tself, prio);
    }
    if (cpu_mask.any()) {
      thread_set_affinity(tself, cpu_mask, name);
    }
#endif

    // Initialize unique thread index.
    unique_thread_index = thread_index_manager.get_free_identifier();

    // Trigger observers.
    thread_observers.on_thread_creation();

    // Run task.
    callable();

    // Trigger observers.
    thread_observers.on_thread_destruction();

    // Release unique thread identifier.
    thread_index_manager.release_identifier(get_thread_index());
  });
}

unsigned srsran::get_thread_index()
{
  return unique_thread_index;
}

const char* srsran::this_thread_name()
{
  // Storage of current thread name, set via unique_thread.
  thread_local std::string this_thread_name_val = compute_this_thread_name();
  return this_thread_name_val.c_str();
}

void srsran::print_this_thread_priority()
{
  print_thread_priority(::pthread_self(), this_thread_name(), std::this_thread::get_id());
}

void unique_thread::print_priority()
{
  print_thread_priority(thread_handle.native_handle(), name.c_str(), thread_handle.get_id());
}

void unique_thread::add_observer(std::unique_ptr<observer> observer)
{
  thread_observers.add(std::move(observer));
}

unsigned unique_thread::get_max_nof_supported_threads()
{
  return unique_thread_index_manager::get_max_nof_supported_threads();
}

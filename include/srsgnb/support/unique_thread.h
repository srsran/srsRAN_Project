/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_THREADS_H
#define SRSRAN_THREADS_H

#include "srsgnb/adt/unique_function.h"
#include <string>
#include <thread>

namespace srsgnb {

/// Unique thread wrapper that ensures the thread is joined on destruction and provides an interface to set/get
/// the thread name, priority, cpu mask.
class unique_thread
{
public:
  explicit unique_thread(std::string name_) : name(std::move(name_)) {}

  unique_thread(const unique_thread&) = delete;

  unique_thread(unique_thread&& other) noexcept = default;

  ~unique_thread() { join(); }

  unique_thread& operator=(const unique_thread&) = delete;

  unique_thread& operator=(unique_thread&& other) noexcept = default;

  bool start(unique_function<void()> callable) { return start_impl(-1, -1, std::move(callable)); }

  bool start(int prio, unique_function<void()> callable) { return start_impl(-1, prio, std::move(callable)); }

  bool start_cpu(int prio, int cpu, unique_function<void()> callable)
  {
    return start_impl(cpu, prio, std::move(callable));
  }

  bool start_cpu_mask(int prio, int mask, unique_function<void()> callable)
  {
    // we multiply mask by 100 to distinguish it from a single cpu core id.
    return start_impl(mask * 100, prio, std::move(callable));
  }

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

  void print_priority();

private:
  /// Start thread. This function blocks until the thread has started and thread_id has been set.
  bool start_impl(int cpu, int prio_offset, unique_function<void()> callable);

  /// Thread name.
  std::string name;

  std::thread thread_handle;
};

/// Get caller thread name.
const char* this_thread_name();

/// Print caller thread priority.
void print_this_thread_priority();

} // namespace srsgnb

#endif // SRSRAN_THREADS_H

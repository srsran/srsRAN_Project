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

#include "srsgnb_test.h"

#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsgnb {

/// This custom sink intercepts log messages to count error and warning log entries.
class log_sink_spy : public srslog::sink
{
public:
  explicit log_sink_spy(std::unique_ptr<srslog::log_formatter> f) :
    srslog::sink(std::move(f)), s(srslog::get_default_sink())
  {
    error_counter.store(0);
    warning_counter.store(0);
  }

  /// Identifier of this custom sink.
  static const char* name() { return "log_sink_spy"; }

  /// Returns the number of log entries tagged as errors.
  unsigned get_error_counter() const
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    return error_counter.load();
  }

  /// Returns the number of log entries tagged as warnings.
  unsigned get_warning_counter() const
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    return warning_counter.load();
  }

  /// Resets the counters back to 0.
  void reset_counters()
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    error_counter.store(0);
    warning_counter.store(0);
  }

  srslog::detail::error_string write(srslog::detail::memory_buffer buffer) override
  {
    std::string entry(buffer.data(), buffer.size());
    if (entry.find("[E]") != std::string::npos) {
      error_counter.fetch_add(1);
    } else if (entry.find("[W]") != std::string::npos) {
      warning_counter.fetch_add(1);
    }

    return s.write(buffer);
  }

  srslog::detail::error_string flush() override { return s.flush(); }

private:
  srslog::sink&         s;
  std::atomic<unsigned> error_counter;
  std::atomic<unsigned> warning_counter;
};

/// This custom sink intercepts log messages allowing users to check if a certain log entry has been generated.
/// Calling spy.has_message("something") will return true if any log entries generated so far contain the string
/// "something".
/// The log entries history can be cleared with reset so old entries can be discarded.
class log_sink_message_spy : public srslog::sink
{
public:
  explicit log_sink_message_spy(std::unique_ptr<srslog::log_formatter> f) :
    srslog::sink(std::move(f)), s(srslog::get_default_sink())
  {
  }

  /// Identifier of this custom sink.
  static const char* name() { return "log_sink_message_spy"; }

  /// Discards all registered log entries.
  void reset()
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    entries.clear();
  }

  /// Returns true if the string in msg is found in the registered log entries.
  bool has_message(const std::string& msg) const
  {
    srslog::flush();
    return std::find_if(entries.cbegin(), entries.cend(), [&](const std::string& entry) {
             return entry.find(msg) != std::string::npos;
           }) != entries.cend();
  }

  srslog::detail::error_string write(srslog::detail::memory_buffer buffer) override
  {
    entries.emplace_back(buffer.data(), buffer.size());

    return s.write(buffer);
  }

  srslog::detail::error_string flush() override { return s.flush(); }

private:
  srslog::sink&            s;
  std::vector<std::string> entries;
};

/**
 * Delimits beginning/ending of a test with the following console output:
 * ============= [Test <Name of the Test>] ===============
 * <test log>
 * =======================================================
 */
class test_delimit_logger
{
  const size_t delimiter_length = 128;

public:
  template <typename... Args>
  explicit test_delimit_logger(const char* test_name_fmt, Args&&... args)
  {
    test_name               = fmt::format(test_name_fmt, std::forward<Args>(args)...);
    std::string name_str    = fmt::format("[ Test \"{}\" ]", test_name);
    double      nof_repeats = (delimiter_length - name_str.size()) / 2.0;
    fmt::print("{0:=>{1}}{2}{0:=>{3}}\n", "", (int)floor(nof_repeats), name_str, (int)ceil(nof_repeats));
  }
  test_delimit_logger(const test_delimit_logger&)            = delete;
  test_delimit_logger(test_delimit_logger&&)                 = delete;
  test_delimit_logger& operator=(const test_delimit_logger&) = delete;
  test_delimit_logger& operator=(test_delimit_logger&&)      = delete;
  ~test_delimit_logger()
  {
    srslog::flush();
    fmt::print("{:=>{}}\n", "", delimiter_length);
  }

private:
  std::string test_name;
};

/// Test object to verify correct move ctor/assignment logic
struct moveonly_test_object {
  moveonly_test_object() : val_ptr(std::make_unique<int>(object_count_impl())) { object_count_impl()++; }
  moveonly_test_object(int v) : val_ptr(std::make_unique<int>(v)) { object_count_impl()++; }
  ~moveonly_test_object() { object_count_impl()--; }
  moveonly_test_object(moveonly_test_object&& other) noexcept : val_ptr(std::move(other.val_ptr))
  {
    object_count_impl()++;
  }
  moveonly_test_object(const moveonly_test_object& other)            = delete;
  moveonly_test_object& operator=(moveonly_test_object&&) noexcept   = default;
  moveonly_test_object& operator=(const moveonly_test_object& other) = delete;

  bool has_value() const { return val_ptr != nullptr; }
  int  value() const
  {
    srsgnb_assert(has_value(), "Invalid access");
    return *val_ptr;
  }
  bool operator==(const moveonly_test_object& other) const
  {
    return has_value() == other.has_value() and (!has_value() || value() == other.value());
  }
  bool operator!=(const moveonly_test_object& other) const { return !(*this == other); }

  static size_t object_count() { return object_count_impl(); }

private:
  static size_t& object_count_impl()
  {
    static size_t count = 0;
    return count;
  }

  std::unique_ptr<int> val_ptr;
};

/// Test object to verify correct copy ctor/assignment logic
struct copyonly_test_object {
  copyonly_test_object() : val(object_count_impl()) { object_count_impl()++; }
  explicit copyonly_test_object(int v) : val(v) { object_count_impl()++; }
  ~copyonly_test_object() { object_count_impl()--; }
  copyonly_test_object(const copyonly_test_object& other) noexcept : val(other.val) { object_count_impl()++; }
  copyonly_test_object(copyonly_test_object&& other)                    = delete;
  copyonly_test_object& operator=(const copyonly_test_object&) noexcept = default;
  copyonly_test_object& operator=(copyonly_test_object&&)               = delete;

  int  value() const { return val; }
  bool operator==(const copyonly_test_object& other) const { return val == other.val; }
  bool operator!=(const copyonly_test_object& other) const { return val != other.val; }

  static size_t object_count() { return object_count_impl(); }

private:
  static size_t& object_count_impl()
  {
    static size_t count = 0;
    return count;
  }
  int val;
};

#define CONDERROR(cond, fmt, ...) srsgnb_assert(not(cond), fmt, ##__VA_ARGS__)

} // namespace srsgnb

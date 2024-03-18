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

#include "srsran_test.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/error_handling.h"
#include <cstdio>
#include <random>

namespace srsran {

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
  moveonly_test_object() : val_ptr(std::make_unique<int>(object_count_impl()))
  {
    object_count_impl().fetch_add(1, std::memory_order_relaxed);
  }
  moveonly_test_object(int v) : val_ptr(std::make_unique<int>(v))
  {
    object_count_impl().fetch_add(1, std::memory_order_relaxed);
  }
  ~moveonly_test_object() { object_count_impl().fetch_sub(1, std::memory_order_relaxed); }
  moveonly_test_object(moveonly_test_object&& other) noexcept : val_ptr(std::move(other.val_ptr))
  {
    object_count_impl().fetch_add(1, std::memory_order_relaxed);
  }
  moveonly_test_object(const moveonly_test_object& other)            = delete;
  moveonly_test_object& operator=(moveonly_test_object&&) noexcept   = default;
  moveonly_test_object& operator=(const moveonly_test_object& other) = delete;

  bool has_value() const { return val_ptr != nullptr; }
  int  value() const
  {
    srsran_assert(has_value(), "Invalid access");
    return *val_ptr;
  }
  bool operator==(const moveonly_test_object& other) const
  {
    return has_value() == other.has_value() and (!has_value() || value() == other.value());
  }
  bool operator!=(const moveonly_test_object& other) const { return !(*this == other); }

  static size_t object_count() { return object_count_impl().load(std::memory_order_relaxed); }

private:
  static std::atomic<size_t>& object_count_impl()
  {
    static std::atomic<size_t> count{0};
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

struct nondefault_ctor_test_object {
  nondefault_ctor_test_object(int val_) : val(val_) {}
  int  value() const { return val; }
  bool operator==(const nondefault_ctor_test_object& other) const { return val == other.val; }
  bool operator!=(const nondefault_ctor_test_object& other) const { return val != other.val; }

private:
  int val;
};

/// \brief This class creates a random generation interface that is suitable for unit tests. The user has the
/// ability to set a random seed to reproduce tests.
class test_rgen
{
public:
  /// \brief Get test pseudo-random generator.
  static std::mt19937& get()
  {
    thread_local std::mt19937& random_generator = init(std::random_device{}());
    return random_generator;
  }

  /// \brief Set random seed for the test pseudo-random generator.
  /// If this function is never called, the pseudo-random generator will be initialized with a random seed using
  /// std::random_device. If it is called once, the seed will be the one passed as an argument. If it is called more
  /// than once, the test will abort. Thus, there should not be race conditions in seed initialization.
  static void set_seed(uint32_t seed)
  {
    init(seed);
    if (get_seed() != seed) {
      report_fatal_error("Trying to set a random seed multiple times (not thread-safe)");
    }
  }

  /// \brief Return a random integer with uniform distribution within the specified bounds.
  template <typename Integer>
  static Integer uniform_int(Integer min, Integer max)
  {
    return std::uniform_int_distribution<Integer>{min, max}(get());
  }

  template <typename Integer>
  static Integer uniform_int()
  {
    return uniform_int(std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::max());
  }

  static bool bernoulli(double p) { return std::bernoulli_distribution(p)(get()); }

  /// \brief Return a vector of integers with specified size filled with random values.
  template <typename Integer>
  static std::vector<Integer> random_vector(size_t sz)
  {
    std::vector<Integer> vec(sz);
    for (unsigned i = 0; i != sz; ++i) {
      vec[i] = test_rgen::uniform_int<Integer>();
    }
    return vec;
  }

private:
  static std::mt19937& init(uint32_t seed)
  {
    thread_local std::mt19937 random_generator(get_seed(seed));
    return random_generator;
  }

  static uint32_t get_seed(uint32_t seed = 0)
  {
    static uint32_t locked_seed = [](uint32_t seed_) {
      fmt::print("-- TEST random generator seed: {}\n", seed_);
      return seed_;
    }(seed);
    return locked_seed;
  }
};

/// \brief Helper macro to set test seed without being in a class/function scope.
#define TEST_RGEN_SET_SEED(seed)                                                                                       \
  static const bool dummy = []() {                                                                                     \
    srsran::test_rgen::set_seed(seed);                                                                                 \
    return dummy;                                                                                                      \
  }();

} // namespace srsran

namespace fmt {

/// \brief Formatter for moveonly_test_object.
template <>
struct formatter<srsran::moveonly_test_object> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(const srsran::moveonly_test_object& obj, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", obj.value());
  }
};

} // namespace fmt

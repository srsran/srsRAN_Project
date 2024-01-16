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

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

namespace srsran {

/// This function forbids the compiler from optimizing away expressions without side-effects.
template <typename T>
inline std::enable_if_t<std::is_trivially_copyable<T>::value && (sizeof(T) <= sizeof(T*))>
do_not_optimize(T const& value)
{
  asm volatile("" : : "r,m"(value) : "memory");
}
template <typename T>
inline std::enable_if_t<!std::is_trivially_copyable<T>::value || (sizeof(T) > sizeof(T*))>
do_not_optimize(T const& value)
{
  asm volatile("" : : "m"(value) : "memory");
}
template <typename T>
inline std::enable_if_t<std::is_trivially_copyable<T>::value && (sizeof(T) <= sizeof(T*))> do_not_optimize(T& value)
{
  asm volatile("" : "+m,r"(value) : : "memory");
}
template <typename T>
inline std::enable_if_t<!std::is_trivially_copyable<T>::value || (sizeof(T) > sizeof(T*))> do_not_optimize(T& value)
{
  asm volatile("" : "+m"(value) : : "memory");
}

/// \brief Describes a class for performing benchmarks.
class benchmarker
{
private:
  /// Benchmark main title.
  std::string title;
  /// Number of repetitions.
  uint64_t nof_repetitions;

  /// Collects a benchmark result line.
  struct benchmark_result {
    /// Measurement description.
    std::string description;
    /// Number of elements computed in every measurement. It is used to calculate the throughput.
    uint64_t size;
    /// Actual measurements in nanoseconds.
    std::vector<uint64_t> measurements;
  };
  /// Collects different measurements.
  std::vector<benchmark_result> benchmark_results;

  /// \brief Get the maximum measured execution time.
  /// \return A pair of values containing the maximum execution time and the number of elements in order.
  std::pair<uint64_t, uint64_t> get_max_meas_time_ns() const
  {
    auto it = std::max_element(
        benchmark_results.begin(), benchmark_results.end(), [](const benchmark_result& a, const benchmark_result& b) {
          return a.measurements.back() < b.measurements.back();
        });
    return {it->measurements.back(), it->size};
  }

  /// Get percentile column width for execution time, including one decimal point.
  unsigned get_percentile_width_time(double scaling = 1.0) const
  {
    uint64_t max_meas_ns = get_max_meas_time_ns().first;
    unsigned percentile_width =
        static_cast<unsigned>(std::ceil(std::log10(static_cast<double>(max_meas_ns) * scaling)));
    percentile_width = std::max(percentile_width, 6U);
    return percentile_width + 2;
  }

  /// Get percentile column width for execution throughput.
  unsigned get_percentile_width_throughput(double scaling) const
  {
    std::pair<uint64_t, uint64_t> max_meas = get_max_meas_time_ns();
    max_meas.first                         = std::max(max_meas.first, 1UL);
    double throughput_max                  = convert_to_throughput(max_meas.first, max_meas.second);

    unsigned percentile_width = static_cast<unsigned>(std::ceil(std::log10(throughput_max * scaling))) + 2U;
    percentile_width          = std::max(percentile_width, 8U);
    return percentile_width;
  }

  /// Get description column width.
  unsigned get_description_width() const
  {
    unsigned descr_width = std::max_element(benchmark_results.begin(),
                                            benchmark_results.end(),
                                            [](const benchmark_result& a, const benchmark_result& b) {
                                              return a.description.size() < b.description.size();
                                            })
                               ->description.size();
    descr_width = std::max(descr_width, 13U);
    return descr_width;
  }

  /// \brief Prints the percentile header.
  /// \param[in] descr_width      Description column width.
  /// \param[in] percentile_width Percentiles column width.
  /// \param[in] units            Units displayed in the header.
  void print_percentile_header(unsigned descr_width, unsigned percentile_width, const std::string& units) const
  {
    fmt::print("\"{}\" performance for {} repetitions. All values are in {}.\n"
               " {:<{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|\n",
               title,
               nof_repetitions,
               units,
               "Percentiles:",
               descr_width,
               "50th",
               percentile_width,
               "75th",
               percentile_width,
               "90th",
               percentile_width,
               "99th",
               percentile_width,
               "99.9th",
               percentile_width,
               "99.99th",
               percentile_width,
               "Worst",
               percentile_width);
  }

  /// \brief Converts a measurement into throughput in millions of elements per second.
  /// \param[in] time_ns Execution time in nanoseconds.
  /// \param[in] size    Number of elements processed during the measurement.
  /// \return The calculated throughput.
  static inline double convert_to_throughput(uint64_t time_ns, size_t size)
  {
    time_ns = std::max(time_ns, 1UL);

    // To get a more precise result with one decimal place, the throughput is calculated in hundreds of thousands of
    // elements per second and then divided by ten when it is converted to a floating point number.
    uint64_t throughput = (size * 10000UL) / time_ns;

    // Converts to floating point.
    return static_cast<double>(throughput) * 0.1;
  }

  struct do_nothing_functor {
    void operator()() {}
  };

public:
  /// \brief Creates a bench marker.
  ///
  /// \param[in] title_           Indicates the benchmark title for the percentile report.
  /// \param[in] nof_repetitions_ Indicates the number of repetitions of the function to measure the performance.
  benchmarker(std::string title_, uint64_t nof_repetitions_) :
    title(std::move(title_)), nof_repetitions(nof_repetitions_)
  {
#if ASSERTS_ENABLED
    fmt::print(stderr, "Warning: Assertions are enabled. Performance might be negatively affected.\n");
#endif
  }

  /// Prints the time execution measurements in nanoseconds.
  void print_percentiles_time(std::string units = "nanoseconds", double scaling = 1.0) const
  {
    if (benchmark_results.empty()) {
      return;
    }

    // Get width plus 2 characters for 1 decimal position.
    unsigned percentile_width = get_percentile_width_time(scaling);
    unsigned descr_width      = get_description_width();

    print_percentile_header(descr_width, percentile_width, units);
    for (const benchmark_result& result : benchmark_results) {
      fmt::print(" {:{}}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|\n",
                 result.description,
                 descr_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.5)] * scaling,
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.75)] * scaling,
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.9)] * scaling,
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.99)] * scaling,
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.999)] * scaling,
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.9999)] * scaling,
                 percentile_width,
                 result.measurements.back() * scaling,
                 percentile_width);
    }
  }

  /// \brief Prints the throughput measurements in millions of elements per seconds.
  /// \param[in] units Units counted in the throughput (ie. bits, symbols, etc.).
  void print_percentiles_throughput(const std::string& units, double scaling = 1.0) const
  {
    if (benchmark_results.empty()) {
      fmt::print("No benchmark results for {}.\n", title);
      return;
    }

    unsigned percentile_width = get_percentile_width_throughput(scaling);
    unsigned descr_width      = get_description_width();

    print_percentile_header(descr_width, percentile_width, "mega" + units + " per second");
    for (const benchmark_result& result : benchmark_results) {
      fmt::print(
          " {:{}}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|\n",
          result.description,
          descr_width,
          convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.5)], result.size) * scaling,
          percentile_width,
          convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.75)], result.size) *
              scaling,
          percentile_width,
          convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.9)], result.size) * scaling,
          percentile_width,
          convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.99)], result.size) *
              scaling,
          percentile_width,
          convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.999)], result.size) *
              scaling,
          percentile_width,
          convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.9999)], result.size) *
              scaling,
          percentile_width,
          convert_to_throughput(result.measurements.back(), result.size) * scaling,
          percentile_width);
    }
  }

  /// \brief Performs a new performance measurement.
  /// \tparam Func             Lambda type to perform the benchmark.
  /// \tparam PostFunc     Lambda type to call after each Func call.
  /// \param[in] description   Measurement description for later reporting.
  /// \param[in] size          Number of elements processed in the measurement.
  /// \param[in] function      Lambda function to call repeatedly.
  /// \param[in] post_func Lambda function to call repeatedly, after \c function, but without being accounted for
  /// in the benchmark results.
  template <typename Func, typename PostFunc = do_nothing_functor>
  void new_measure(const std::string& description,
                   uint64_t           size,
                   Func&&             function,
                   PostFunc&&         post_func = do_nothing_functor{})
  {
    benchmark_result result;
    result.description = description;
    result.size        = size;
    result.measurements.reserve(nof_repetitions);

    for (uint64_t rep = 0; rep != nof_repetitions; ++rep) {
      auto start = std::chrono::high_resolution_clock::now();
      function();
      auto end = std::chrono::high_resolution_clock::now();
      result.measurements.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
      post_func();
    }

    std::sort(result.measurements.begin(), result.measurements.end());

    benchmark_results.push_back(std::move(result));
  }

  /// \brief Performs a new performance measurement with some context setup.
  /// \tparam Func           Lambda type to perform the benchmark.
  /// \param[in] description Measurement description for later reporting.
  /// \param[in] size        Number of elements processed in the measurement.
  /// \param[in] function    Lambda function to call repeatedly.
  template <typename Context, typename Func>
  void
  new_measure_with_context(const std::string& description, uint64_t size, Context&& context_function, Func&& function)
  {
    benchmark_result result;
    result.description = description;
    result.size        = size;
    result.measurements.reserve(nof_repetitions);

    for (uint64_t rep = 0; rep != nof_repetitions; ++rep) {
      context_function();
      auto start = std::chrono::high_resolution_clock::now();
      function();
      auto end = std::chrono::high_resolution_clock::now();
      result.measurements.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    std::sort(result.measurements.begin(), result.measurements.end());

    benchmark_results.push_back(std::move(result));
  }
};

} // namespace srsran

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

#include <chrono>
#include <string>
#include <vector>

namespace srsgnb {

/// \brief Describes a class for performing benchmarks.
class benchmarker
{
private:
  std::string title;
  unsigned    nof_repetitions;
  struct benchmark_result {
    std::string           description;
    std::vector<unsigned> measurements;
  };
  std::vector<benchmark_result> benchmark_results;

public:
  /// \brief Creates a bench marker.
  ///
  /// \param[in] title_           Indicates the benchmark title for the percentile report.
  /// \param[in] nof_repetitions_ Indicates the number of repetitions of the function to measure the performance.
  benchmarker(const std::string& title_, unsigned nof_repetitions_) : title(title_), nof_repetitions(nof_repetitions_)
  {
    // Do nothing.
  }

  /// Prints the stored performance measurements.
  void print_percentiles() const
  {
    unsigned max_meas_ns = std::max_element(benchmark_results.begin(),
                                            benchmark_results.end(),
                                            [](const benchmark_result& a, const benchmark_result& b) {
                                              return a.measurements.back() < b.measurements.back();
                                            })
                               ->measurements.back();
    unsigned percentile_width = static_cast<unsigned>(std::ceil(std::log10(static_cast<float>(max_meas_ns))));
    percentile_width          = std::max(percentile_width, 8U);

    unsigned descr_with = std::max_element(benchmark_results.begin(),
                                           benchmark_results.end(),
                                           [](const benchmark_result& a, const benchmark_result& b) {
                                             return a.description.size() < b.description.size();
                                           })
                              ->description.size();
    descr_with = std::max(descr_with, 13U);

    fmt::print("{} performance. All values in nanoseconds.\n"
               " {:<{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|\n",
               title,
               "Percentiles:",
               descr_with,
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
               "Worst",
               percentile_width);
    for (const benchmark_result& result : benchmark_results) {
      fmt::print(" {:{}}|{:{}}|{:{}}|{:{}}|{:{}}|{:{}}|{:{}}|\n",
                 result.description,
                 descr_with,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.5)],
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.75)],
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.9)],
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.99)],
                 percentile_width,
                 result.measurements[static_cast<size_t>(nof_repetitions * 0.999)],
                 percentile_width,
                 result.measurements.back(),
                 percentile_width);
    }
  }

  /// \brief Performs a new performance measurement.
  /// \tparam Func       Lambda type to perform the benchmark.
  /// \param description Measurement description for later reporting.
  /// \param function    Lambda function to call repeatedly.
  template <typename Func>
  void new_measure(const std::string& description, Func&& function)
  {
    benchmark_result result;
    result.description = description;
    result.measurements.reserve(nof_repetitions);

    for (unsigned rep = 0; rep != nof_repetitions; ++rep) {
      auto start = std::chrono::high_resolution_clock::now();
      function();
      auto end = std::chrono::high_resolution_clock::now();
      result.measurements.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    std::sort(result.measurements.begin(), result.measurements.end());

    benchmark_results.push_back(std::move(result));
  }
};

} // namespace srsgnb

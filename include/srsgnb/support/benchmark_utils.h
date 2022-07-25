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
  /// Benchmark main title.
  std::string title;
  /// Number of repetitions.
  unsigned nof_repetitions;

  /// Collects a benchmark result line.
  struct benchmark_result {
    /// Measurement description.
    std::string description;
    /// Number of elements computed in every measurement. It is used to calculate the throughput.
    unsigned size;
    /// Actual measurements in nanoseconds.
    std::vector<unsigned> measurements;
  };
  /// Collects different measurements.
  std::vector<benchmark_result> benchmark_results;

  /// \brief Get the maximum measured execution time.
  /// \return A pair of values containing the maximum execution time and the number of elements in order.
  std::pair<unsigned, unsigned> get_max_meas_time_ns() const
  {
    auto it = std::max_element(
        benchmark_results.begin(), benchmark_results.end(), [](const benchmark_result& a, const benchmark_result& b) {
          return a.measurements.back() < b.measurements.back();
        });
    return {it->measurements.back(), it->size};
  }

  /// Get percentile column width for execution time.
  unsigned get_percentile_width_time() const
  {
    unsigned max_meas_ns      = get_max_meas_time_ns().first;
    unsigned percentile_width = static_cast<unsigned>(std::ceil(std::log10(static_cast<float>(max_meas_ns))));
    percentile_width          = std::max(percentile_width, 8U);
    return percentile_width;
  }

  /// Get percentile column width for execution throughput.
  unsigned get_percentile_width_throughput() const
  {
    std::pair<unsigned, unsigned> max_meas = get_max_meas_time_ns();
    max_meas.first                         = std::max(max_meas.first, 1U);
    double throughput_max                  = convert_to_throughput(max_meas.first, max_meas.second);

    unsigned percentile_width = static_cast<unsigned>(std::ceil(std::log10(static_cast<float>(throughput_max)))) + 2U;
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
    fmt::print("\"{}\" performance for {} repetitions. All values in {}.\n"
               " {:<{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|{:^{}}|\n",
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
               "Worst",
               percentile_width);
  }

  /// \brief Converts a measurement into throughput in millions of elements per second.
  /// \param[in] time_ns Execution time in nanoseconds.
  /// \param[in] size    Number of elements processed during the measurement.
  /// \return The calculated throughput.
  static inline double convert_to_throughput(unsigned time_ns, unsigned size)
  {
    time_ns = std::max(time_ns, 1U);
    return 1e3 * static_cast<double>(size) / static_cast<double>(time_ns);
  }

public:
  /// \brief Creates a bench marker.
  ///
  /// \param[in] title_           Indicates the benchmark title for the percentile report.
  /// \param[in] nof_repetitions_ Indicates the number of repetitions of the function to measure the performance.
  benchmarker(const std::string& title_, unsigned nof_repetitions_) : title(title_), nof_repetitions(nof_repetitions_)
  {
    // Do nothing.
  }

  /// Prints the time execution measurements in nanoseconds.
  void print_percentiles_time() const
  {
    unsigned percentile_width = get_percentile_width_time();
    unsigned descr_width      = get_description_width();

    print_percentile_header(descr_width, percentile_width, "nanoseconds");
    for (const benchmark_result& result : benchmark_results) {
      fmt::print(" {:{}}|{:{}}|{:{}}|{:{}}|{:{}}|{:{}}|{:{}}|\n",
                 result.description,
                 descr_width,
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

  /// \brief Prints the throughput measurements in millions of elements per seconds.
  /// \param[in] units Units counted in the throughput (ie. bits, symbols, etc.).
  void print_percentiles_throughput(const std::string& units) const
  {
    unsigned percentile_width = get_percentile_width_throughput();
    unsigned descr_with       = get_description_width();

    print_percentile_header(descr_with, percentile_width, "millions of " + units + " per second");
    for (const benchmark_result& result : benchmark_results) {
      fmt::print(" {:{}}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|{:{}.1f}|\n",
                 result.description,
                 descr_with,
                 convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.5)], result.size),
                 percentile_width,
                 convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.75)], result.size),
                 percentile_width,
                 convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.9)], result.size),
                 percentile_width,
                 convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.99)], result.size),
                 percentile_width,
                 convert_to_throughput(result.measurements[static_cast<size_t>(nof_repetitions * 0.999)], result.size),
                 percentile_width,
                 convert_to_throughput(result.measurements.back(), result.size),
                 percentile_width);
    }
  }

  /// \brief Performs a new performance measurement.
  /// \tparam Func           Lambda type to perform the benchmark.
  /// \param[in] description Measurement description for later reporting.
  /// \param[in] size        Number of elements processed in the measurement.
  /// \param[in] function    Lambda function to call repeatedly.
  template <typename Func>
  void new_measure(const std::string& description, unsigned size, Func&& function)
  {
    benchmark_result result;
    result.description = description;
    result.size        = size;
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

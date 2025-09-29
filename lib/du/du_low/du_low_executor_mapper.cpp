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

#include "srsran/du/du_low/du_low_executor_mapper.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/phy/upper/upper_phy_execution_configuration.h"
#include "srsran/support/executors/concurrent_metrics_executor.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/executors/task_fork_limiter.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_du;

namespace {

class du_low_executor_mapper_impl : public du_low_executor_mapper
{
public:
  du_low_executor_mapper_impl(const du_low_executor_mapper_config& config)
  {
    if (std::holds_alternative<du_low_executor_mapper_single_exec_config>(config.executors)) {
      const auto& single = std::get<du_low_executor_mapper_single_exec_config>(config.executors);

      srsran_assert(single.common_executor.executor != nullptr, "Invalid common executor.");
      srsran_assert(single.common_executor.max_concurrency != 0, "Invalid common executor maximum concurrency.");

      phy_config.pdcch_executor   = single.common_executor;
      phy_config.pdsch_executor   = single.common_executor;
      phy_config.ssb_executor     = single.common_executor;
      phy_config.csi_rs_executor  = single.common_executor;
      phy_config.prs_executor     = single.common_executor;
      phy_config.dl_grid_executor = single.common_executor;
      phy_config.prach_executor   = single.common_executor;
      phy_config.pusch_executor   = single.common_executor;
      phy_config.pucch_executor   = single.common_executor;
      phy_config.srs_executor     = single.common_executor;

      phy_config.pusch_ch_estimator_executor = create_inline_task_executor();

      // The PDSCH processor and the PUSCH decoder shall work synchronously.
      phy_config.pdsch_codeblock_executor = {};
      phy_config.pusch_decoder_executor   = {};
    } else if (std::holds_alternative<du_low_executor_mapper_flexible_exec_config>(config.executors)) {
      const unsigned                           max_pucch_batch_size = 16;
      const unsigned                           max_pusch_batch_size = 1;
      const unsigned                           max_pdsch_batch_size = 16;
      static constexpr std::array<unsigned, 1> pdsch_queue_sizes{default_queue_size};
      static constexpr std::array<unsigned, 1> pucch_queue_sizes{default_queue_size};
      static constexpr std::array<unsigned, 3> pusch_srs_queue_sizes{
          default_queue_size, default_queue_size, default_queue_size};

      const auto& flexible = std::get<du_low_executor_mapper_flexible_exec_config>(config.executors);

      auto pdsch_executors = create_task_fork_limiter(
          flexible.rt_hi_prio_exec, flexible.max_pdsch_concurrency, pdsch_queue_sizes, max_pdsch_batch_size);

      phy_config.pdcch_executor           = flexible.rt_hi_prio_exec;
      phy_config.pdsch_executor           = pdsch_executors[0];
      phy_config.ssb_executor             = flexible.rt_hi_prio_exec;
      phy_config.csi_rs_executor          = flexible.rt_hi_prio_exec;
      phy_config.prs_executor             = flexible.rt_hi_prio_exec;
      phy_config.dl_grid_executor         = flexible.non_rt_hi_prio_exec;
      phy_config.pdsch_codeblock_executor = pdsch_executors[0];
      phy_config.prach_executor           = flexible.non_rt_hi_prio_exec;
      phy_config.pucch_executor =
          create_task_fork_limiter(
              flexible.non_rt_hi_prio_exec, flexible.max_pucch_concurrency, pucch_queue_sizes, max_pucch_batch_size)
              .front();
      auto pusch_srs_execs                   = create_task_fork_limiter(flexible.non_rt_medium_prio_exec,
                                                      flexible.max_pusch_and_srs_concurrency,
                                                      pusch_srs_queue_sizes,
                                                      max_pusch_batch_size);
      phy_config.pusch_ch_estimator_executor = pusch_srs_execs[0];
      phy_config.pusch_executor              = pusch_srs_execs[1];
      phy_config.pusch_decoder_executor      = pusch_srs_execs[2];
      phy_config.srs_executor                = pusch_srs_execs[2];
    }

    srsran_assert(phy_config.pdcch_executor.is_valid(), "Invalid PDCCH executor.");
    srsran_assert(phy_config.pdsch_executor.is_valid(), "Invalid PDSCH executor.");
    srsran_assert(phy_config.ssb_executor.is_valid(), "Invalid SSB executor.");
    srsran_assert(phy_config.csi_rs_executor.is_valid(), "Invalid NZP-CSI-RS executor.");
    srsran_assert(phy_config.prs_executor.is_valid(), "Invalid PRS executor.");
    srsran_assert(phy_config.dl_grid_executor.is_valid(), "Invalid DL grid pool executor.");
    srsran_assert(phy_config.pucch_executor.is_valid(), "Invalid PUCCH executor.");
    srsran_assert(phy_config.pusch_executor.is_valid(), "Invalid PUSCH executor.");
    srsran_assert(phy_config.pusch_ch_estimator_executor.is_valid(), "Invalid PUSCH channel estimator executor.");
    srsran_assert(phy_config.prach_executor.is_valid(), "Invalid PRACH executor.");
    srsran_assert(phy_config.srs_executor.is_valid(), "Invalid SRS executor.");

    if (config.metrics.has_value()) {
      const du_low_executor_mapper_metric_config& metrics_config = *config.metrics;
      phy_config.pdcch_executor  = wrap_executor_with_metric(phy_config.pdcch_executor, "pdcch_exec", metrics_config);
      phy_config.pdsch_executor  = wrap_executor_with_metric(phy_config.pdsch_executor, "pdsch_exec", metrics_config);
      phy_config.ssb_executor    = wrap_executor_with_metric(phy_config.ssb_executor, "ssb_exec", metrics_config);
      phy_config.csi_rs_executor = wrap_executor_with_metric(phy_config.csi_rs_executor, "csi_rs_exec", metrics_config);
      phy_config.prs_executor    = wrap_executor_with_metric(phy_config.prs_executor, "prs_exec", metrics_config);
      if (phy_config.pdsch_codeblock_executor.is_valid()) {
        phy_config.pdsch_codeblock_executor =
            wrap_executor_with_metric(phy_config.pdsch_codeblock_executor, "pdsch_codeblock_exec", metrics_config);
      }
      phy_config.prach_executor = wrap_executor_with_metric(phy_config.prach_executor, "prach_exec", metrics_config);
      phy_config.pusch_executor = wrap_executor_with_metric(phy_config.pusch_executor, "pusch_exec", metrics_config);
      phy_config.pusch_ch_estimator_executor =
          wrap_executor_with_metric(phy_config.pusch_ch_estimator_executor, "pusch_ch_est_exec", metrics_config);
      if (phy_config.pusch_decoder_executor.is_valid()) {
        phy_config.pusch_decoder_executor =
            wrap_executor_with_metric(phy_config.pusch_decoder_executor, "pusch_dec_exec", metrics_config);
      }
      phy_config.pucch_executor = wrap_executor_with_metric(phy_config.pucch_executor, "pucch_exec", metrics_config);
      phy_config.srs_executor   = wrap_executor_with_metric(phy_config.srs_executor, "srs_exec", metrics_config);
    }
  }

  // See interface for documentation.
  const upper_phy_execution_configuration& get_upper_phy_execution_config() const override { return phy_config; }

private:
  /// Default queue size for DU low executors.
  static constexpr unsigned default_queue_size = 2048;

  /// Creates a strand on the top of a given executor.
  std::vector<upper_phy_executor>
  create_strand(upper_phy_executor base_executor, span<const unsigned> queue_sizes, unsigned max_batch)
  {
    srsran_assert(base_executor.is_valid(), "Invalid executor.");
    srsran_assert(!queue_sizes.empty(), "Queue sizes must not be empty.");

    // Only one priority level is requested.
    if ((queue_sizes.size() == 1)) {
      // Skip creating a strand if the base executor maximum concurrency is one.
      if (base_executor.max_concurrency == 1) {
        return {base_executor};
      }

      // Create strand without priority queues.
      executors.emplace_back(make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(
          *base_executor.executor, queue_sizes[0], max_batch));
      return {{executors.back().get(), 1}};
    }

    // Instantiate a priority task strand if several priority levels are requested.
    std::vector<concurrent_queue_params> qparams;
    qparams.reserve(queue_sizes.size());
    for (auto& qsize : queue_sizes) {
      qparams.emplace_back(concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, qsize});
    }
    auto prio_strand_ptr = make_priority_task_strand_ptr(*base_executor.executor, qparams);

    // Save executors.
    auto exec_list = prio_strand_ptr->get_executors();
    adaptors.emplace_back(std::move(prio_strand_ptr));

    std::vector<upper_phy_executor> ret;
    for (task_executor& exec : exec_list) {
      ret.emplace_back(upper_phy_executor{.executor = &exec, .max_concurrency = 1});
    }
    return ret;
  }

  /// Creates a task fork limiter on the top of a given executor.
  std::vector<upper_phy_executor> create_task_fork_limiter(upper_phy_executor   base_executor,
                                                           unsigned             max_nof_threads,
                                                           span<const unsigned> queue_sizes,
                                                           unsigned             max_batch)
  {
    srsran_assert(base_executor.is_valid(), "Invalid executor.");
    srsran_assert(!queue_sizes.empty(), "Queue sizes must not be empty.");

    // Limit maximum number of threads to the base executor maximum concurrency.
    if ((max_nof_threads == 0) || (max_nof_threads > base_executor.max_concurrency)) {
      max_nof_threads = base_executor.max_concurrency;
    }

    // If the resultant maximum number of threads is one, create a strand.
    if (max_nof_threads <= 1) {
      return create_strand(base_executor, queue_sizes, max_batch);
    }

    // Skip creation of the fork limiter if there is only one queue and the maximum number of threads is equal to the
    // base executor maximum concurrency.
    if ((queue_sizes.size() == 1) && (max_nof_threads == base_executor.max_concurrency)) {
      return {base_executor};
    }

    // Create fork limiter.
    auto fork_limiter = std::make_unique<task_fork_limiter<task_executor&>>(
        *base_executor.executor, max_nof_threads, queue_sizes, max_batch);
    auto execs = fork_limiter->get_executors();
    adaptors.emplace_back(std::move(fork_limiter));

    std::vector<upper_phy_executor> ret;
    for (task_executor& exec : execs) {
      ret.emplace_back(upper_phy_executor{.executor = &exec, .max_concurrency = max_nof_threads});
    }
    return ret;
  }

  /// Creates an inline task executor.
  upper_phy_executor create_inline_task_executor()
  {
    executors.emplace_back(std::make_unique<inline_task_executor>());
    return {executors.back().get(), 1};
  }

  /// Wraps an executor with a metric decorator.
  upper_phy_executor wrap_executor_with_metric(upper_phy_executor                          base_executor,
                                               std::string                                 exec_name,
                                               const du_low_executor_mapper_metric_config& config)
  {
    srsran_assert(base_executor.is_valid(), "Invalid executor.");
    executors.emplace_back(make_concurrent_metrics_executor_ptr(
        exec_name, *base_executor.executor, config.sequential_executor, config.logger, config.period));
    return {executors.back().get(), base_executor.max_concurrency};
  }

  /// Upper physical layer executor configuration.
  upper_phy_execution_configuration phy_config;
  /// List of internal executor instances.
  std::vector<std::unique_ptr<task_executor>> executors;
  /// Strands and task fork limiters used by the mapper.
  std::vector<std::variant<std::unique_ptr<priority_task_strand<task_executor&>>,
                           std::unique_ptr<task_fork_limiter<task_executor&>>>>
      adaptors;
};

} // namespace

std::unique_ptr<du_low_executor_mapper>
srsran::srs_du::create_du_low_executor_mapper(const du_low_executor_mapper_config& config)
{
  return std::make_unique<du_low_executor_mapper_impl>(config);
}

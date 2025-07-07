/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/du_low/du_low_executor_mapper.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/support/executors/concurrent_metrics_executor.h"
#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_du;

namespace {

class du_low_cell_executor_mapper_impl : public du_low_cell_executor_mapper
{
public:
  du_low_cell_executor_mapper_impl(task_executor& dl_exec_,
                                   task_executor& pdsch_exec_,
                                   task_executor& prach_exec_,
                                   task_executor& pusch_exec_,
                                   task_executor& pusch_decoder_exec_,
                                   task_executor& pucch_exec_,
                                   task_executor& srs_exec_) :
    dl_exec(dl_exec_),
    pdsch_exec(pdsch_exec_),
    prach_exec(prach_exec_),
    pusch_exec(pusch_exec_),
    pusch_decoder_exec(pusch_decoder_exec_),
    pucch_exec(pucch_exec_),
    srs_exec(srs_exec_)
  {
  }

  // See interface for documentation.
  task_executor& downlink_executor() override { return dl_exec; }

  // See interface for documentation.
  task_executor& pdsch_executor() override { return pdsch_exec; }

  // See interface for documentation.
  task_executor& prach_executor() override { return prach_exec; }

  // See interface for documentation.
  task_executor& pusch_executor() override { return pusch_exec; }

  // See interface for documentation.
  task_executor& pusch_decoder_executor() override { return pusch_decoder_exec; }

  // See interface for documentation.
  task_executor& pucch_executor() override { return pucch_exec; }

  // See interface for documentation.
  task_executor& srs_executor() override { return srs_exec; }

private:
  task_executor& dl_exec;
  task_executor& pdsch_exec;
  task_executor& prach_exec;
  task_executor& pusch_exec;
  task_executor& pusch_decoder_exec;
  task_executor& pucch_exec;
  task_executor& srs_exec;
};

class du_low_executor_mapper_impl : public du_low_executor_mapper
{
public:
  du_low_executor_mapper_impl(const du_low_executor_mapper_config& config)
  {
    srsran_assert(!config.cells.empty(), "Cells in the executor mapper configuration must not be empty.");
    for (unsigned cell_idx = 0, end = config.cells.size(); cell_idx != end; ++cell_idx) {
      const auto& cell_config = config.cells[cell_idx];

      task_executor* dl_exec        = nullptr;
      task_executor* pdsch_exec     = nullptr;
      task_executor* prach_exec     = nullptr;
      task_executor* pusch_exec     = nullptr;
      task_executor* pusch_dec_exec = nullptr;
      task_executor* pucch_exec     = nullptr;
      task_executor* srs_exec       = nullptr;

      if (std::holds_alternative<du_low_executor_mapper_single_exec_config>(cell_config)) {
        const auto& single = std::get<du_low_executor_mapper_single_exec_config>(cell_config);

        srsran_assert(single.common_executor != nullptr, "Invalid common executor.");

        dl_exec        = single.common_executor;
        pdsch_exec     = single.common_executor;
        prach_exec     = single.common_executor;
        pusch_exec     = single.common_executor;
        pusch_dec_exec = single.common_executor;
        pucch_exec     = single.common_executor;
        srs_exec       = single.common_executor;
      } else if (std::holds_alternative<du_low_executor_mapper_manual_exec_config>(cell_config)) {
        const auto& manual = std::get<du_low_executor_mapper_manual_exec_config>(cell_config);
        dl_exec            = manual.dl_executor;
        pdsch_exec         = manual.pdsch_executor;
        prach_exec         = create_strand(manual.high_priority_executor);
        pusch_exec         = manual.pusch_executor;
        pusch_dec_exec     = manual.pusch_decoder_executor;
        pucch_exec         = manual.pucch_executor;
        srs_exec           = manual.srs_executor;
      }

      srsran_assert(dl_exec != nullptr, "Invalid DL executor.");
      srsran_assert(pdsch_exec != nullptr, "Invalid PDSCH executor.");
      srsran_assert(prach_exec != nullptr, "Invalid PRACH executor.");
      srsran_assert(pusch_exec != nullptr, "Invalid PUSCH executor.");
      srsran_assert(pusch_dec_exec != nullptr, "Invalid PUSCH decoder executor.");
      srsran_assert(pucch_exec != nullptr, "Invalid PUCCH executor.");
      srsran_assert(srs_exec != nullptr, "Invalid SRS executor.");

      if (config.metrics.has_value()) {
        const du_low_executor_mapper_metric_config& metrics_config = *config.metrics;
        dl_exec    = wrap_executor_with_metric(dl_exec, fmt::format("dl_exec#{}", cell_idx), metrics_config);
        pdsch_exec = wrap_executor_with_metric(pdsch_exec, fmt::format("pdsch_exec#{}", cell_idx), metrics_config);
        prach_exec = wrap_executor_with_metric(prach_exec, fmt::format("prach_exec#{}", cell_idx), metrics_config);
        pusch_exec = wrap_executor_with_metric(pusch_exec, fmt::format("pusch_exec#{}", cell_idx), metrics_config);
        pusch_dec_exec =
            wrap_executor_with_metric(pusch_dec_exec, fmt::format("pusch_dec_exec#{}", cell_idx), metrics_config);
        pucch_exec = wrap_executor_with_metric(pucch_exec, fmt::format("pucch_exec#{}", cell_idx), metrics_config);
        srs_exec   = wrap_executor_with_metric(srs_exec, fmt::format("srs_exec#{}", cell_idx), metrics_config);
      }

      cell_mappers.emplace_back(std::make_unique<du_low_cell_executor_mapper_impl>(
          *dl_exec, *pdsch_exec, *prach_exec, *pusch_exec, *pusch_dec_exec, *pucch_exec, *srs_exec));
    }
  }

  // See interface for documentation.
  du_low_cell_executor_mapper& get_cell_mapper(unsigned cell_index) override
  {
    srsran_assert(cell_index < cell_mappers.size(),
                  "The cell index {} exceeds the number of cells {}",
                  cell_index,
                  cell_mappers.size());
    return *cell_mappers[cell_index];
  }

private:
  /// Creates a strand on the top of a given executor.
  task_executor* create_strand(task_executor* base_executor)
  {
    srsran_assert(base_executor != nullptr, "Invalid executor.");
    executors.emplace_back(make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(*base_executor, 2048));
    return executors.back().get();
  }

  /// Wraps an executor with a metric decorator.
  task_executor* wrap_executor_with_metric(task_executor*                              base_executor,
                                           std::string                                 exec_name,
                                           const du_low_executor_mapper_metric_config& config)
  {
    srsran_assert(base_executor != nullptr, "Invalid executor.");
    executors.emplace_back(make_concurrent_metrics_executor_ptr(
        exec_name, *base_executor, config.sequential_executor, config.logger, config.period));
    return executors.back().get();
  }

  /// Actual executor map indexed per cell basis.
  std::vector<std::unique_ptr<du_low_cell_executor_mapper>> cell_mappers;
  /// List of internal executor instances.
  std::vector<std::unique_ptr<task_executor>> executors;
};

} // namespace

std::unique_ptr<du_low_executor_mapper>
srsran::srs_du::create_du_low_executor_mapper(const du_low_executor_mapper_config& config)
{
  return std::make_unique<du_low_executor_mapper_impl>(config);
}

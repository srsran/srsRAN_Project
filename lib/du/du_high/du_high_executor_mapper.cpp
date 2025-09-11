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

#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/support/executors/executor_decoration_factory.h"
#include "srsran/support/executors/strand_executor.h"

using namespace srsran;
using namespace srs_du;

namespace {

/// Helper class to decorate executors with extra functionalities.
struct executor_decorator {
  template <typename Exec>
  task_executor& decorate(Exec&&                                          exec,
                          bool                                            is_sync,
                          bool                                            tracing_enabled,
                          std::optional<unsigned>                         throttle_thres,
                          const std::optional<std::chrono::milliseconds>& metrics_period,
                          const std::string&                              exec_name = "")
  {
    if (not is_sync and not tracing_enabled and not metrics_period and not throttle_thres) {
      // No decoration needed, return the original executor.
      return exec;
    }

    execution_decoration_config cfg;
    if (is_sync) {
      cfg.sync = execution_decoration_config::sync_option{};
    }
    if (throttle_thres.has_value()) {
      cfg.throttle = execution_decoration_config::throttle_option{*throttle_thres};
    }
    if (tracing_enabled) {
      cfg.trace = execution_decoration_config::trace_option{exec_name};
    }
    if (metrics_period) {
      cfg.metrics = execution_decoration_config::metrics_option{exec_name, *metrics_period};
    }
    decorators.push_back(decorate_executor(std::forward<Exec>(exec), cfg));

    return *decorators.back();
  }

private:
  std::vector<std::unique_ptr<task_executor>> decorators;
};

/// Cell executor mapper that uses dedicated serialized workers, one per cell.
class dedicated_cell_worker_executor_mapper final : public du_high_cell_executor_mapper
{
public:
  dedicated_cell_worker_executor_mapper(const du_high_executor_config::dedicated_cell_worker_list& cell_workers,
                                        bool                                                       rt_mode_enabled,
                                        bool                                                       trace_enabled,
                                        const std::optional<std::chrono::milliseconds>&            metrics_period)
  {
    bool is_sync = not rt_mode_enabled;
    cell_execs.reserve(cell_workers.size());
    for (const auto& cell_worker : cell_workers) {
      std::string slot_exec_name =
          trace_enabled or metrics_period ? fmt::format("slot_ind_exec#{}", cell_execs.size()) : "";
      std::string cell_exec_name =
          trace_enabled or metrics_period ? fmt::format("cell_exec#{}", cell_execs.size()) : "";
      cell_execs.push_back(du_high_executor_config::dedicated_cell_worker{
          &decorator.decorate(
              *cell_worker.high_prio_executor, is_sync, trace_enabled, std::nullopt, metrics_period, slot_exec_name),
          &decorator.decorate(
              *cell_worker.low_prio_executor, is_sync, trace_enabled, std::nullopt, metrics_period, cell_exec_name)});
    }
  }

  task_executor& mac_cell_executor(du_cell_index_t cell_index) override
  {
    return *cell_execs[cell_index % cell_execs.size()].low_prio_executor;
  }

  task_executor& rlc_lower_executor(du_cell_index_t cell_index) override
  {
    return *cell_execs[cell_index % cell_execs.size()].low_prio_executor;
  }

  task_executor& slot_ind_executor(du_cell_index_t cell_index) override
  {
    return *cell_execs[cell_index % cell_execs.size()].high_prio_executor;
  }

private:
  du_high_executor_config::dedicated_cell_worker_list cell_execs;
  executor_decorator                                  decorator;
};

/// Cell executor mapper that uses a general thread pool, and creates one strand instance per cell.
class strand_cell_worker_executor_mapper final : public du_high_cell_executor_mapper
{
public:
  strand_cell_worker_executor_mapper(const du_high_executor_config::strand_based_worker_pool& cfg,
                                     bool                                                     rt_mode_enabled,
                                     bool                                                     trace_enabled,
                                     const std::optional<std::chrono::milliseconds>&          metrics_period)
  {
    srsran_assert(cfg.nof_cells > 0, "Invalid number of cells");
    concurrent_queue_params slot_qparams{concurrent_queue_policy::lockfree_spsc, 4};
    concurrent_queue_params other_qparams{concurrent_queue_policy::lockfree_mpmc, cfg.default_task_queue_size};
    bool                    is_sync = not rt_mode_enabled;

    cell_strands.resize(cfg.nof_cells);
    for (unsigned i = 0, e = cfg.nof_cells; i != e; ++i) {
      cell_strands[i].strand =
          std::make_unique<cell_strand_type>(cfg.pool_executors[i % cfg.pool_executors.size()],
                                             std::array<concurrent_queue_params, 2>{slot_qparams, other_qparams});
      auto execs = cell_strands[i].strand->get_executors();

      std::string exec_name = trace_enabled or metrics_period ? fmt::format("slot_ind_exec#{}", i) : "";
      cell_strands[i].slot_ind_exec =
          &decorator.decorate(execs[0], is_sync, trace_enabled, std::nullopt, metrics_period, exec_name);
      exec_name = trace_enabled or metrics_period ? fmt::format("mac_cell_exec#{}", i) : "";
      cell_strands[i].mac_cell_exec =
          &decorator.decorate(execs[1], is_sync, trace_enabled, std::nullopt, metrics_period, exec_name);
      exec_name = trace_enabled or metrics_period ? fmt::format("rlc_lower_exec#{}", i) : "";
      cell_strands[i].rlc_lower_exec =
          &decorator.decorate(execs[1], is_sync, trace_enabled, std::nullopt, metrics_period, exec_name);
    }
  }

  task_executor& mac_cell_executor(du_cell_index_t cell_index) override
  {
    return *cell_strands[cell_index % cell_strands.size()].mac_cell_exec;
  }

  task_executor& rlc_lower_executor(du_cell_index_t cell_index) override
  {
    return *cell_strands[cell_index % cell_strands.size()].rlc_lower_exec;
  }

  task_executor& slot_ind_executor(du_cell_index_t cell_index) override
  {
    return *cell_strands[cell_index % cell_strands.size()].slot_ind_exec;
  }

private:
  using cell_strand_type = priority_task_strand<task_executor*, force_dispatch_strand_lock<enqueue_priority::max>>;
  struct strand_context {
    std::unique_ptr<cell_strand_type> strand;
    task_executor*                    slot_ind_exec;
    task_executor*                    mac_cell_exec;
    task_executor*                    rlc_lower_exec;
  };

  std::vector<strand_context> cell_strands;
  executor_decorator          decorator;
};

static std::unique_ptr<du_high_cell_executor_mapper>
create_du_high_cell_executor_mapper(const du_high_executor_config& config)
{
  std::unique_ptr<du_high_cell_executor_mapper> cell_mapper;
  if (const auto* ded_workers =
          std::get_if<du_high_executor_config::dedicated_cell_worker_list>(&config.cell_executors)) {
    cell_mapper = std::make_unique<dedicated_cell_worker_executor_mapper>(
        *ded_workers, config.is_rt_mode_enabled, config.trace_exec_tasks, config.metrics_period);
  } else {
    cell_mapper = std::make_unique<strand_cell_worker_executor_mapper>(
        std::get<du_high_executor_config::strand_based_worker_pool>(config.cell_executors),
        config.is_rt_mode_enabled,
        config.trace_exec_tasks,
        config.metrics_period);
  }
  return cell_mapper;
}

//--- common_ue_executor_mapper ---//

/// Common base class for different types of ue_executor_mappers.
class common_ue_executor_mapper : public du_high_ue_executor_mapper
{
protected:
  using strand_type = priority_task_strand<task_executor*>;
  struct strand_context {
    std::unique_ptr<strand_type> strand;
    task_executor*               ctrl_exec;
    task_executor*               ul_exec;
    task_executor*               dl_exec;
  };

  common_ue_executor_mapper(unsigned initial_capacity)
  {
    srsran_assert(initial_capacity > 0, "Invalid number of max strands");

    strands.reserve(initial_capacity);
  }

  void add_strand(task_executor&                                  pool_exec,
                  unsigned                                        ctrl_queue_size,
                  unsigned                                        pdu_queue_size,
                  bool                                            trace_enabled,
                  const std::optional<std::chrono::milliseconds>& metrics_period)
  {
    auto& strand_ctxt  = strands.emplace_back();
    strand_ctxt.strand = std::make_unique<strand_type>(
        &pool_exec,
        std::array<concurrent_queue_params, 3>{
            concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, ctrl_queue_size},
            concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, pdu_queue_size},
            concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, pdu_queue_size}});
    auto execs            = strands.back().strand->get_executors();
    strand_ctxt.ctrl_exec = &execs[0];
    strand_ctxt.ul_exec   = &execs[1];
    strand_ctxt.dl_exec   = &execs[2];

    if (trace_enabled or metrics_period) {
      // If tracing is enabled, decorate the executors.
      unsigned idx          = strands.size() - 1;
      strand_ctxt.ctrl_exec = &decorator.decorate(*strand_ctxt.ctrl_exec,
                                                  false,
                                                  trace_enabled,
                                                  std::nullopt,
                                                  metrics_period,
                                                  fmt::format("ue_ctrl_exec#{}", idx));
      strand_ctxt.ul_exec   = &decorator.decorate(
          *strand_ctxt.ul_exec, false, trace_enabled, std::nullopt, metrics_period, fmt::format("ue_ul_exec#{}", idx));
      strand_ctxt.dl_exec = &decorator.decorate(
          *strand_ctxt.dl_exec, false, trace_enabled, std::nullopt, metrics_period, fmt::format("ue_dl_exec#{}", idx));
    }
  }

  std::vector<strand_context> strands;
  executor_decorator          decorator;
};

/// L2 UL executor mapper that maps UEs based on their RNTI.
class index_based_ue_executor_mapper final : public common_ue_executor_mapper
{
public:
  index_based_ue_executor_mapper(task_executor&                                  pool_executor,
                                 unsigned                                        max_strands,
                                 unsigned                                        ctrl_queue_size,
                                 unsigned                                        pdu_queue_size,
                                 bool                                            trace_enabled,
                                 const std::optional<std::chrono::milliseconds>& metrics_period) :
    common_ue_executor_mapper(max_strands)
  {
    for (unsigned i = 0; i != max_strands; ++i) {
      add_strand(pool_executor, ctrl_queue_size, pdu_queue_size, trace_enabled, metrics_period);
    }
  }

  void rebind_executors(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    // Do nothing.
  }

  task_executor& ctrl_executor(du_ue_index_t ue_index) override
  {
    ue_index = ue_index < MAX_NOF_DU_UES ? ue_index : to_du_ue_index(0);
    return *strands[ue_index % strands.size()].ctrl_exec;
  }

  task_executor& f1u_dl_pdu_executor(du_ue_index_t ue_index) override
  {
    ue_index = ue_index < MAX_NOF_DU_UES ? ue_index : to_du_ue_index(0);
    return *strands[ue_index % strands.size()].dl_exec;
  }

  task_executor& mac_ul_pdu_executor(du_ue_index_t ue_index) override
  {
    ue_index = ue_index < MAX_NOF_DU_UES ? ue_index : to_du_ue_index(0);
    return *strands[ue_index % strands.size()].ul_exec;
  }
};

/// L2 UL executor mapper that maps UEs based on their PCell.
class pcell_ue_executor_mapper final : public common_ue_executor_mapper
{
public:
  explicit pcell_ue_executor_mapper(task_executor&                                  pool_executor,
                                    unsigned                                        max_strands_,
                                    unsigned                                        ctrl_queue_size_,
                                    unsigned                                        pdu_queue_size_,
                                    bool                                            trace_enabled_,
                                    const std::optional<std::chrono::milliseconds>& metrics_period_) :
    common_ue_executor_mapper(max_strands_),
    pool_exec(pool_executor),
    max_strands(max_strands_),
    ctrl_queue_size(ctrl_queue_size_),
    pdu_queue_size(pdu_queue_size_),
    trace_enabled(trace_enabled_),
    metrics_period(metrics_period_)
  {
    // Create base strand.
    add_strand(pool_executor, ctrl_queue_size, pdu_queue_size, trace_enabled, metrics_period);

    // Initialize UE executor lookup.
    for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
      ue_idx_to_exec_index[i] = 0;
    }
  }

  void rebind_executors(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue id={}", fmt::underlying(ue_index));
    ue_idx_to_exec_index[ue_index] = pcell_index % max_strands;
    while (strands.size() <= ue_idx_to_exec_index[ue_index]) {
      add_strand(pool_exec, ctrl_queue_size, pdu_queue_size, trace_enabled, metrics_period);
    }
  }

  task_executor& ctrl_executor(du_ue_index_t ue_index) override
  {
    size_t idx = ue_index < MAX_NOF_DU_UES ? ue_idx_to_exec_index[ue_index] : 0;
    return *strands[idx].ctrl_exec;
  }

  task_executor& f1u_dl_pdu_executor(du_ue_index_t ue_index) override
  {
    size_t idx = ue_index < MAX_NOF_DU_UES ? ue_idx_to_exec_index[ue_index] : 0;
    return *strands[idx].dl_exec;
  }

  task_executor& mac_ul_pdu_executor(du_ue_index_t ue_index) override
  {
    size_t idx = ue_index < MAX_NOF_DU_UES ? ue_idx_to_exec_index[ue_index] : 0;
    return *strands[idx].ul_exec;
  }

private:
  task_executor&                                 pool_exec;
  const unsigned                                 max_strands;
  const unsigned                                 ctrl_queue_size;
  const unsigned                                 pdu_queue_size;
  const bool                                     trace_enabled;
  const std::optional<std::chrono::milliseconds> metrics_period;

  /// Map of ue indexes to executors. The last position is used when the UE has no ue_index yet assigned.
  std::array<unsigned, MAX_NOF_DU_UES> ue_idx_to_exec_index;
};

static std::unique_ptr<du_high_ue_executor_mapper>
create_du_high_ue_executor_mapper(const du_high_executor_config& config)
{
  std::unique_ptr<du_high_ue_executor_mapper> ue_mapper;
  if (config.ue_executors.policy == du_high_executor_config::ue_executor_config::map_policy::per_cell) {
    ue_mapper = std::make_unique<pcell_ue_executor_mapper>(*config.ue_executors.pool_executor,
                                                           config.ue_executors.max_nof_strands,
                                                           config.ue_executors.ctrl_queue_size,
                                                           config.ue_executors.pdu_queue_size,
                                                           config.trace_exec_tasks,
                                                           config.metrics_period);
  } else {
    ue_mapper = std::make_unique<index_based_ue_executor_mapper>(*config.ue_executors.pool_executor,
                                                                 config.ue_executors.max_nof_strands,
                                                                 config.ue_executors.ctrl_queue_size,
                                                                 config.ue_executors.pdu_queue_size,
                                                                 config.trace_exec_tasks,
                                                                 config.metrics_period);
  }
  return ue_mapper;
}

//--- ctrl_executor_mapper ---//

/// \brief Executor mapper for control-plane tasks.
///
/// It creates one strand with two priority levels. The highest priority is used for timer tick tasks. The second level
/// is used for all other control-plane tasks.
/// In case of non-RT operation, we make the timer_exec synchronous. This will have the effect of stopping
/// the lower layers from running faster than this strand.
class ctrl_executor_mapper
{
  using ctrl_strand_type = task_strand<task_executor*, concurrent_queue_policy::lockfree_mpmc>;

public:
  ctrl_executor_mapper(const du_high_executor_config::control_executor_config& cfg,
                       bool                                                    trace_enabled,
                       const std::optional<std::chrono::milliseconds>&         metrics_period) :
    strand(cfg.pool_executor, cfg.task_queue_size),
    ctrl_exec(decorator.decorate(strand,
                                 false,
                                 trace_enabled,
                                 std::nullopt,
                                 metrics_period,
                                 trace_enabled or metrics_period ? "du_ctrl_exec" : "")),
    e2_exec(decorator.decorate(strand,
                               false,
                               trace_enabled,
                               std::nullopt,
                               metrics_period,
                               trace_enabled or metrics_period ? "du_e2_exec" : ""))
  {
  }

  ctrl_strand_type   strand;
  executor_decorator decorator;

  task_executor& ctrl_exec;
  task_executor& e2_exec;
};

//

class du_high_executor_mapper_impl final : public du_high_executor_mapper
{
  using tick_strand_type = task_strand<task_executor*, concurrent_queue_policy::lockfree_mpmc>;

public:
  explicit du_high_executor_mapper_impl(const du_high_executor_config& config) :
    raw_non_rt_hi_prio_exec(*config.ctrl_executors.pool_executor),
    raw_low_prio_exec(*config.ue_executors.f1u_reader_executor),
    cell_mapper_ptr(create_du_high_cell_executor_mapper(config)),
    ue_mapper_ptr(create_du_high_ue_executor_mapper(config)),
    ctrl_mapper(config.ctrl_executors, config.trace_exec_tasks, config.metrics_period)
  {
  }

  du_high_cell_executor_mapper& cell_mapper() override { return *cell_mapper_ptr; }
  du_high_ue_executor_mapper&   ue_mapper() override { return *ue_mapper_ptr; }
  task_executor&                du_control_executor() override { return ctrl_mapper.ctrl_exec; }
  task_executor&                du_e2_executor() override { return ctrl_mapper.e2_exec; }
  task_executor&                f1c_rx_executor() override { return raw_non_rt_hi_prio_exec; }
  task_executor&                e2_rx_executor() override { return raw_non_rt_hi_prio_exec; }
  task_executor&                f1u_rx_executor() override { return raw_low_prio_exec; }

private:
  /// Raw control-plane executor.
  task_executor& raw_non_rt_hi_prio_exec;
  task_executor& raw_low_prio_exec;

  std::unique_ptr<du_high_cell_executor_mapper> cell_mapper_ptr;
  std::unique_ptr<du_high_ue_executor_mapper>   ue_mapper_ptr;
  ctrl_executor_mapper                          ctrl_mapper;
};

} // namespace

std::unique_ptr<du_high_executor_mapper>
srsran::srs_du::create_du_high_executor_mapper(const du_high_executor_config& config)
{
  return std::make_unique<du_high_executor_mapper_impl>(config);
}

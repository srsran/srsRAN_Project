/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_low_unit_factory.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "du_low_config.h"
#include "du_low_config_translator.h"
#include "du_low_hal_factory.h"
#include "srsran/du/du_low/o_du_low_factory.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"

using namespace srsran;

static void generate_dl_processor_config(downlink_processor_factory_sw_config&                  out_cfg,
                                         const du_low_unit_config&                              unit_cfg,
                                         const upper_phy_execution_configuration&               exec_config,
                                         std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory)
{
  out_cfg.ldpc_encoder_type   = "auto";
  out_cfg.crc_calculator_type = "auto";

  const du_low_unit_expert_threads_config& upper_phy_threads_cfg = unit_cfg.expert_execution_cfg.threads;

  // Check hardware acceleration usage.
  bool hwacc_pdsch_processor = hw_encoder_factory != nullptr;
  if (hwacc_pdsch_processor) {
    out_cfg.hw_encoder_factory = hw_encoder_factory;
  }

  // The flexible PDSCH processor implementation will be used by default.
  if ((upper_phy_threads_cfg.pdsch_processor_type == "auto") ||
      (upper_phy_threads_cfg.pdsch_processor_type == "flexible")) {
    // Setup parameters for synchronous operation:
    // - the batch size must be the maximum to avoid more than one batch; and
    // - the maximum number of simultaneous PDSCH equals to the number of DL processing threads.
    unsigned cb_batch_length            = pdsch_processor_flexible_configuration::synchronous_cb_batch_length;
    unsigned max_nof_simultaneous_pdsch = upper_phy_threads_cfg.nof_dl_threads;

    // Override default parameters if the CB batch length is set for asynchronous concurrent operation.
    if (upper_phy_threads_cfg.pdsch_cb_batch_length != du_low_unit_expert_threads_config::synchronous_cb_batch_length) {
      // For asynchronous operation:
      // - Use the given CB batch length;
      // - The number of simultaneous PDSCH is equal to the maximum number of PDSCH per slot times the maximum allowed
      //   processing time.
      cb_batch_length            = upper_phy_threads_cfg.pdsch_cb_batch_length;
      max_nof_simultaneous_pdsch = (MAX_UE_PDUS_PER_SLOT + 1) * unit_cfg.expert_phy_cfg.max_processing_delay_slots;
    }

    // Emplace configuration parameters.
    pdsch_processor_flexible_configuration pdsch_proc_config = {
        .nof_pdsch_codeblock_threads   = exec_config.pdsch_codeblock_executor.max_concurrency,
        .cb_batch_length               = cb_batch_length,
        .max_nof_simultaneous_pdsch    = max_nof_simultaneous_pdsch,
        .pdsch_codeblock_task_executor = *exec_config.pdsch_codeblock_executor.executor};
    out_cfg.pdsch_processor.emplace<pdsch_processor_flexible_configuration>(pdsch_proc_config);
  } else if (upper_phy_threads_cfg.pdsch_processor_type == "generic") {
    out_cfg.pdsch_processor.emplace<pdsch_processor_generic_configuration>();
  } else {
    srsran_assert(false,
                  "Invalid {}PDSCH processor type {}.",
                  hwacc_pdsch_processor ? "hardware-accelerated " : "",
                  upper_phy_threads_cfg.pdsch_processor_type);
  }

  out_cfg.pdcch_executor  = exec_config.pdcch_executor;
  out_cfg.pdsch_executor  = exec_config.pdsch_executor;
  out_cfg.ssb_executor    = exec_config.ssb_executor;
  out_cfg.csi_rs_executor = exec_config.csi_rs_executor;
  out_cfg.prs_executor    = exec_config.prs_executor;
}

o_du_low_unit_factory::o_du_low_unit_factory(const std::optional<du_low_unit_hal_config>& hal_config,
                                             unsigned                                     nof_cells) :
  hal_dependencies(make_du_low_hal_dependencies(hal_config, nof_cells))
{
}

o_du_low_unit o_du_low_unit_factory::create(const o_du_low_unit_config&       params,
                                            const o_du_low_unit_dependencies& dependencies)
{
  srs_du::o_du_low_config o_du_low_cfg;

  // Copy FAPI configuration.
  o_du_low_cfg.fapi_cfg = params.fapi_cfg;

  // Configure the metrics.
  o_du_low_cfg.enable_metrics = params.du_low_unit_cfg.metrics_cfg.enable_du_low;

  generate_o_du_low_config(o_du_low_cfg, params.du_low_unit_cfg, params.cells);

  // Fill the workers information.
  for (unsigned i = 0, e = o_du_low_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    srs_du::du_low_cell_executor_mapper& cell_exec_map = dependencies.workers[i];

    srs_du::du_low_cell_config& cell = o_du_low_cfg.du_low_cfg.cells[i];

    generate_dl_processor_config(cell.dl_proc_cfg,
                                 params.du_low_unit_cfg,
                                 cell_exec_map.get_upper_phy_execution_config(),
                                 hal_dependencies.hw_encoder_factory);

    upper_phy_config& upper          = cell.upper_phy_cfg;
    upper.rg_gateway                 = &dependencies.rg_gateway;
    upper.rx_symbol_request_notifier = &dependencies.rx_symbol_request_notifier;
    upper.executors                  = cell_exec_map.get_upper_phy_execution_config();
    if (hal_dependencies.hw_decoder_factory) {
      upper.hw_decoder_factory = hal_dependencies.hw_decoder_factory;
    }
  }

  o_du_low_unit unit;
  unit.o_du_lo = srs_du::make_o_du_low(o_du_low_cfg);
  report_error_if_not(unit.o_du_lo, "Invalid O-DU low");

  return unit;
}

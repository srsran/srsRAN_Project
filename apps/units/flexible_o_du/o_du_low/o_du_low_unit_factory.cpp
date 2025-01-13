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

#include "o_du_low_unit_factory.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "du_low_config.h"
#include "du_low_config_translator.h"
#include "du_low_hal_factory.h"
#include "srsran/du/du_low/o_du_low_factory.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"

using namespace srsran;

static void generate_dl_processor_config(downlink_processor_factory_sw_config& out_cfg,
                                         const du_low_unit_config&             unit_cfg,
                                         task_executor&                        pdsch_codeblock_executor,
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

  // Hardware-acceleration is currently supported for 'generic' PDSCH processor types only.
  if ((!hwacc_pdsch_processor) &&
      ((upper_phy_threads_cfg.pdsch_processor_type == "lite") ||
       ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads == 1)))) {
    out_cfg.pdsch_processor.emplace<pdsch_processor_lite_configuration>();
  } else if ((!hwacc_pdsch_processor) &&
             ((upper_phy_threads_cfg.pdsch_processor_type == "concurrent") ||
              ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads > 1)))) {
    pdsch_processor_concurrent_configuration pdsch_proc_config;
    pdsch_proc_config.nof_pdsch_codeblock_threads = upper_phy_threads_cfg.nof_dl_threads;
    pdsch_proc_config.max_nof_simultaneous_pdsch =
        (MAX_UE_PDUS_PER_SLOT + 1) * unit_cfg.expert_phy_cfg.max_processing_delay_slots;
    pdsch_proc_config.pdsch_codeblock_task_executor = &pdsch_codeblock_executor;
    out_cfg.pdsch_processor.emplace<pdsch_processor_concurrent_configuration>(pdsch_proc_config);
  } else if ((hwacc_pdsch_processor) || (upper_phy_threads_cfg.pdsch_processor_type == "generic")) {
    out_cfg.pdsch_processor.emplace<pdsch_processor_generic_configuration>();
  } else {
    srsran_assert(false,
                  "Invalid {}PDSCH processor type {}.",
                  hwacc_pdsch_processor ? "hardware-accelerated " : "",
                  upper_phy_threads_cfg.pdsch_processor_type);
  }
  out_cfg.nof_concurrent_threads = upper_phy_threads_cfg.nof_dl_threads;
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
  o_du_low_cfg.du_low_cfg.logger = &srslog::fetch_basic_logger("DU");

  generate_o_du_low_config(o_du_low_cfg, params.du_low_unit_cfg, params.du_cells, params.max_puschs_per_slot);

  // Fill the PRACH ports.
  o_du_low_cfg.prach_ports = params.prach_ports;

  // Fill the workers information.
  for (unsigned i = 0, e = o_du_low_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    srs_du::du_low_cell_config& cell = o_du_low_cfg.du_low_cfg.cells[i];

    generate_dl_processor_config(cell.dl_proc_cfg,
                                 params.du_low_unit_cfg,
                                 *dependencies.workers.upper_pdsch_exec[i],
                                 hal_dependencies.hw_encoder_factory);

    upper_phy_config& upper          = cell.upper_phy_cfg;
    upper.rg_gateway                 = &dependencies.rg_gateway;
    upper.rx_symbol_request_notifier = &dependencies.rx_symbol_request_notifier;
    upper.pucch_executor             = dependencies.workers.upper_pucch_exec[i];
    upper.pusch_executor             = dependencies.workers.upper_pusch_exec[i];
    upper.pusch_decoder_executor     = dependencies.workers.upper_pusch_decoder_exec[i];
    upper.prach_executor             = dependencies.workers.upper_prach_exec[i];
    upper.srs_executor               = dependencies.workers.upper_srs_exec[i];
    if (hal_dependencies.hw_decoder_factory) {
      upper.hw_decoder_factory = hal_dependencies.hw_decoder_factory;
    }
    dependencies.workers.get_du_low_dl_executors(upper.dl_executors, i);
  }

  o_du_low_unit unit;
  unit.o_du_lo = srs_du::make_o_du_low(o_du_low_cfg, params.du_cells);
  report_error_if_not(unit.o_du_lo, "Invalid O-DU low");

  return unit;
}

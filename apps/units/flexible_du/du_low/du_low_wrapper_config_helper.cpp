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

#include "du_low_wrapper_config_helper.h"
#include "apps/services/worker_manager.h"
#include "du_low_config_translator.h"
#include "srsran/du_low/du_low_wrapper_factory.h"

using namespace srsran;

static void generate_dl_processor_config(downlink_processor_factory_sw_config& out_cfg,
                                         const du_low_unit_config&             unit_cfg,
                                         task_executor&                        pdsch_codeblock_executor)
{
  out_cfg.ldpc_encoder_type   = "auto";
  out_cfg.crc_calculator_type = "auto";

  const du_low_unit_expert_threads_config& upper_phy_threads_cfg = unit_cfg.expert_execution_cfg.threads;

  if ((upper_phy_threads_cfg.pdsch_processor_type == "lite") ||
      ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads == 1))) {
    out_cfg.pdsch_processor.emplace<pdsch_processor_lite_configuration>();
  } else if ((upper_phy_threads_cfg.pdsch_processor_type == "concurrent") ||
             ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads > 1))) {
    pdsch_processor_concurrent_configuration pdsch_proc_config;
    pdsch_proc_config.nof_pdsch_codeblock_threads = upper_phy_threads_cfg.nof_dl_threads;
    pdsch_proc_config.max_nof_simultaneous_pdsch =
        (MAX_UE_PDUS_PER_SLOT + 1) * unit_cfg.expert_phy_cfg.max_processing_delay_slots;
    pdsch_proc_config.pdsch_codeblock_task_executor = &pdsch_codeblock_executor;
    out_cfg.pdsch_processor.emplace<pdsch_processor_concurrent_configuration>(pdsch_proc_config);
  } else if (upper_phy_threads_cfg.pdsch_processor_type == "generic") {
    out_cfg.pdsch_processor.emplace<pdsch_processor_generic_configuration>();
  } else {
    srsran_assert(false, "Invalid PDSCH processor type {}.", upper_phy_threads_cfg.pdsch_processor_type);
  }
  out_cfg.nof_concurrent_threads = upper_phy_threads_cfg.nof_dl_threads;
}

void srsran::make_du_low_wrapper_config_and_dependencies(
    du_low_wrapper_config&                out_cfg,
    const du_low_unit_config&             du_low_unit_cfg,
    std::vector<cell_prach_ports_entry>   prach_ports,
    span<const du_cell_config>            du_cells,
    span<const unsigned>                  max_puschs_per_slot,
    upper_phy_rg_gateway&                 rg_gateway,
    upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier,
    worker_manager&                       workers,
    unsigned                              du_id)
{
  out_cfg.du_low_cfg.logger = &srslog::fetch_basic_logger("DU");

  generate_du_low_wrapper_config(
      out_cfg, du_low_unit_cfg, std::move(prach_ports), du_cells, max_puschs_per_slot, du_id);

  // Fill the workers information.
  for (unsigned i = 0, e = out_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    du_low_cell_config& cell = out_cfg.du_low_cfg.cells[i];

    generate_dl_processor_config(cell.dl_proc_cfg, du_low_unit_cfg, *workers.upper_pdsch_exec[i + du_id]);

    upper_phy_config& upper          = cell.upper_phy_cfg;
    upper.rg_gateway                 = &rg_gateway;
    upper.rx_symbol_request_notifier = &rx_symbol_request_notifier;
    upper.pucch_executor             = workers.upper_pucch_exec[i + du_id];
    upper.pusch_executor             = workers.upper_pusch_exec[i + du_id];
    upper.pusch_decoder_executor     = workers.upper_pusch_decoder_exec[i + du_id];
    upper.prach_executor             = workers.upper_prach_exec[i + du_id];
    upper.srs_executor               = workers.upper_srs_exec[i + du_id];
    workers.get_du_low_dl_executors(upper.dl_executors, i + du_id);
  }
}
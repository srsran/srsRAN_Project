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
#include "du_low_config.h"
#include "du_low_config_translator.h"
#include "srsran/du/du_low/du_low_wrapper_factory.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#endif // DPDK_FOUND

using namespace srsran;

static void generate_dl_processor_config(downlink_processor_factory_sw_config& out_cfg,
                                         const du_low_unit_config&             unit_cfg,
                                         task_executor&                        pdsch_codeblock_executor,
                                         bool                                  hwacc_pdsch_processor)
{
  out_cfg.ldpc_encoder_type   = "auto";
  out_cfg.crc_calculator_type = "auto";

  const du_low_unit_expert_threads_config& upper_phy_threads_cfg = unit_cfg.expert_execution_cfg.threads;

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

void srsran::make_du_low_wrapper_config_and_dependencies(
    srs_du::du_low_wrapper_config&              out_cfg,
    const du_low_unit_config&                   du_low_unit_cfg,
    std::vector<srs_du::cell_prach_ports_entry> prach_ports,
    span<const srs_du::du_cell_config>          du_cells,
    span<const unsigned>                        max_puschs_per_slot,
    upper_phy_rg_gateway&                       rg_gateway,
    upper_phy_rx_symbol_request_notifier&       rx_symbol_request_notifier,
    worker_manager&                             workers,
    unsigned                                    du_id)
{
  out_cfg.du_low_cfg.logger = &srslog::fetch_basic_logger("DU");

  // Initialize hardware-accelerator (only if needed).
  hal_upper_phy_config hal_config  = {};
  hal_config.hwacc_pdsch_processor = false;
  hal_config.hwacc_pusch_processor = false;
#ifdef DPDK_FOUND
  if (du_low_unit_cfg.hal_config && du_low_unit_cfg.hal_config->bbdev_hwacc &&
      !du_low_unit_cfg.hal_config->bbdev_hwacc->hwacc_type.empty()) {
    const bbdev_appconfig& bbdev_app_cfg = du_low_unit_cfg.hal_config->bbdev_hwacc.value();
    srslog::basic_logger&  hwacc_logger  = srslog::fetch_basic_logger("HWACC", false);
    hwacc_logger.set_level(du_low_unit_cfg.loggers.hal_level);

    hal::bbdev_hwacc_pdsch_enc_factory_configuration         hwacc_pdsch_enc_cfg = {};
    hal::bbdev_hwacc_pusch_dec_factory_configuration         hwacc_pusch_dec_cfg = {};
    std::shared_ptr<hal::ext_harq_buffer_context_repository> harq_buffer_context = nullptr;
    unsigned                                                 nof_hwacc_dus       = du_cells.size();

    // Create a bbdev accelerator factory.
    std::unique_ptr<dpdk::bbdev_acc_factory> bbdev_acc_factory =
        dpdk::create_bbdev_acc_factory(bbdev_app_cfg.bbdev_acc_type);
    report_error_if_not(bbdev_acc_factory,
                        "Unable to create the {} bbdev hardware-accelerator interface factory.",
                        bbdev_app_cfg.bbdev_acc_type);

    // Intefacing to the bbdev-based hardware-accelerator.
    dpdk::bbdev_acc_configuration bbdev_config;
    bbdev_config.id = bbdev_app_cfg.id;
    if (bbdev_app_cfg.pdsch_enc && bbdev_app_cfg.pdsch_enc->nof_hwacc > 0) {
      bbdev_config.nof_ldpc_enc_lcores = nof_hwacc_dus * bbdev_app_cfg.pdsch_enc->nof_hwacc;
    }
    if (bbdev_app_cfg.pusch_dec && bbdev_app_cfg.pusch_dec->nof_hwacc > 0) {
      bbdev_config.nof_ldpc_dec_lcores = nof_hwacc_dus * bbdev_app_cfg.pusch_dec->nof_hwacc;
    }
    // If no msg_mbuf size is defined, a worst-case value will be used.
    bbdev_config.msg_mbuf_size = bbdev_app_cfg.msg_mbuf_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
    // If no rm_mbuf size is defined, a worst-case value will be used.
    bbdev_config.rm_mbuf_size = bbdev_app_cfg.rm_mbuf_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
    // If no number of mbufs is defined, a worst-case value will be used.
    bbdev_config.nof_mbuf = bbdev_app_cfg.nof_mbuf.value_or(static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS))));
    std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = bbdev_acc_factory->create(bbdev_config, hwacc_logger);
    report_error_if_not(bbdev_accelerator, "Unable to open the {} hardware-accelerator.", bbdev_app_cfg.hwacc_type);

    // Configure the hardware-accelerated PDSCH encoding factory (only if needed).
    if (bbdev_app_cfg.pdsch_enc && bbdev_app_cfg.pdsch_enc->nof_hwacc > 0) {
      hwacc_pdsch_enc_cfg.acc_type          = bbdev_app_cfg.hwacc_type;
      hwacc_pdsch_enc_cfg.bbdev_accelerator = bbdev_accelerator;
      hwacc_pdsch_enc_cfg.cb_mode           = bbdev_app_cfg.pdsch_enc->cb_mode;
      // If no maximum buffer size is defined, a worst-case value will be used.
      hwacc_pdsch_enc_cfg.max_tb_size = bbdev_app_cfg.pdsch_enc->max_buffer_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
      hwacc_pdsch_enc_cfg.dedicated_queue = bbdev_app_cfg.pdsch_enc->dedicated_queue;
      hal_config.hwacc_pdsch_processor    = true;
      hal_config.hwacc_pdsch_enc_cfg      = hwacc_pdsch_enc_cfg;
    }

    // Configure the hardware-accelerated PUSCH decoding factory (only if needed).
    if (bbdev_app_cfg.pusch_dec && bbdev_app_cfg.pusch_dec->nof_hwacc > 0) {
      hwacc_pusch_dec_cfg.acc_type          = bbdev_app_cfg.hwacc_type;
      hwacc_pusch_dec_cfg.bbdev_accelerator = bbdev_accelerator;
      hwacc_pusch_dec_cfg.ext_softbuffer    = bbdev_app_cfg.pusch_dec->ext_softbuffer;
      if (hwacc_pusch_dec_cfg.ext_softbuffer) {
        // Set up an external HARQ buffer context repository.
        unsigned nof_cbs            = bbdev_app_cfg.pusch_dec->harq_context_size.value_or(MAX_NOF_SEGMENTS);
        uint64_t ext_harq_buff_size = bbdev_accelerator->get_harq_buff_size_bytes();
        harq_buffer_context = hal::create_ext_harq_buffer_context_repository(nof_cbs, ext_harq_buff_size, false);
        report_error_if_not(harq_buffer_context,
                            "Unable to create the external HARQ buffer context for the {} hardware-accelerator.",
                            bbdev_app_cfg.hwacc_type);
        hwacc_pusch_dec_cfg.harq_buffer_context = harq_buffer_context;
      }
      hwacc_pusch_dec_cfg.dedicated_queue = bbdev_app_cfg.pusch_dec->dedicated_queue;
      hal_config.hwacc_pusch_processor    = true;
      hal_config.hwacc_pusch_dec_cfg      = hwacc_pusch_dec_cfg;
    }
  }
#endif // DPDK_FOUND

  generate_du_low_wrapper_config(out_cfg, du_low_unit_cfg, du_cells, max_puschs_per_slot, du_id);

  // Fill the hal config.
  for (auto& cell : out_cfg.du_low_cfg.cells) {
    cell.upper_phy_cfg.hal_config = hal_config;
  }

  // Fill the PRACH ports.
  out_cfg.prach_ports = std::move(prach_ports);

  // Fill the workers information.
  for (unsigned i = 0, e = out_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    srs_du::du_low_cell_config& cell = out_cfg.du_low_cfg.cells[i];

    generate_dl_processor_config(cell.dl_proc_cfg,
                                 du_low_unit_cfg,
                                 *workers.upper_pdsch_exec[i + du_id],
                                 cell.upper_phy_cfg.hal_config.hwacc_pdsch_processor);

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

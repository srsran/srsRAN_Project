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

#include "phy_factory.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include "srsran/ran/band_helper.h"

using namespace srsran;

std::unique_ptr<upper_phy> srsran::create_upper_phy(const upper_phy_params&               params,
                                                    upper_phy_rg_gateway*                 rg_gateway,
                                                    span<task_executor*>                  dl_executors,
                                                    task_executor*                        ul_executor,
                                                    upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier)
{
  downlink_processor_factory_sw_config dl_proc_config;
  dl_proc_config.ldpc_encoder_type      = "auto";
  dl_proc_config.crc_calculator_type    = "auto";
  dl_proc_config.nof_concurrent_threads = 1;

  // Create downlink processor factory.
  std::shared_ptr<downlink_processor_factory> dl_proc_factory = create_downlink_processor_factory_sw(dl_proc_config);
  report_fatal_error_if_not(dl_proc_factory, "Invalid DL processor factory.");

  // Create channel precoder factory.
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  report_fatal_error_if_not(precoding_factory, "Invalid channel precoder factory.");

  // Create resource grid factory.
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory(precoding_factory);
  report_fatal_error_if_not(rg_factory, "Invalid resource grid factory.");

  // Create upper PHY factory.
  std::unique_ptr<upper_phy_factory> upper_phy_factory = create_upper_phy_factory(dl_proc_factory, rg_factory);
  report_fatal_error_if_not(upper_phy_factory, "Invalid upper PHY factory.");

  static constexpr unsigned dl_pipeline_depth    = 8;
  static constexpr unsigned ul_pipeline_depth    = 8;
  static constexpr unsigned prach_pipeline_depth = 1;

  upper_phy_config upper_config;
  upper_config.log_level                = params.log_level;
  upper_config.enable_logging_broadcast = (params.log_level != srslog::basic_levels::none);
  upper_config.rx_symbol_printer_filename.clear();
  upper_config.logger_max_hex_size     = 64;
  upper_config.pusch_sinr_calc_method  = channel_state_information::sinr_type::channel_estimator;
  upper_config.sector_id               = 0;
  upper_config.nof_tx_ports            = 1;
  upper_config.nof_rx_ports            = 1;
  upper_config.ldpc_decoder_iterations = 6;
  upper_config.ldpc_decoder_early_stop = true;

  unsigned nof_slots_per_subframe         = get_nof_slots_per_subframe(params.scs);
  upper_config.nof_dl_rg                  = dl_pipeline_depth * nof_slots_per_subframe;
  upper_config.dl_rg_expire_timeout_slots = upper_config.nof_dl_rg - 2;
  upper_config.nof_dl_processors          = upper_config.nof_dl_rg;
  upper_config.nof_ul_rg                  = ul_pipeline_depth;
  upper_config.max_ul_thread_concurrency  = 4;
  upper_config.max_pusch_concurrency      = 1;
  upper_config.nof_pusch_decoder_threads  = 1;
  upper_config.nof_prach_buffer           = prach_pipeline_depth * nof_slots_per_subframe;
  upper_config.max_nof_td_prach_occasions = 1;
  upper_config.max_nof_fd_prach_occasions = 1;
  upper_config.is_prach_long_format       = true;

  upper_config.active_scs                                  = {};
  upper_config.active_scs[to_numerology_value(params.scs)] = true;

  unsigned bw_rb        = band_helper::get_n_rbs_from_bw(params.channel_bw_mhz, params.scs, frequency_range::FR1);
  upper_config.dl_bw_rb = bw_rb;
  upper_config.ul_bw_rb = bw_rb;

  upper_config.tx_buffer_config.nof_buffers          = upper_config.nof_ul_rg;
  upper_config.tx_buffer_config.nof_codeblocks       = 128;
  upper_config.tx_buffer_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
  upper_config.tx_buffer_config.expire_timeout_slots = 100 * nof_slots_per_subframe;
  upper_config.tx_buffer_config.external_soft_bits   = false;

  upper_config.rx_buffer_config.nof_buffers          = upper_config.nof_dl_rg;
  upper_config.rx_buffer_config.max_nof_codeblocks   = 128;
  upper_config.rx_buffer_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
  upper_config.rx_buffer_config.expire_timeout_slots = 100 * nof_slots_per_subframe;
  upper_config.rx_buffer_config.external_soft_bits   = false;

  upper_config.dl_executors               = dl_executors;
  upper_config.rg_gateway                 = rg_gateway;
  upper_config.pucch_executor             = ul_executor;
  upper_config.pusch_executor             = ul_executor;
  upper_config.pusch_decoder_executor     = nullptr;
  upper_config.prach_executor             = ul_executor;
  upper_config.rx_symbol_request_notifier = rx_symbol_request_notifier;
  upper_config.crc_calculator_type        = "auto";
  upper_config.ldpc_rate_dematcher_type   = "auto";
  upper_config.ldpc_decoder_type          = "auto";

  return upper_phy_factory->create(upper_config);
}

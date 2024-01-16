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

#include "srsran/ru/ru_ofh_factory.h"
#include "ru_ofh_impl.h"
#include "ru_ofh_rx_symbol_handler_impl.h"
#include "srsran/ofh/ofh_factories.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

/// Generates the OFH sector configuration from the common Open FrontHaul configuration.
static ofh::sector_configuration generate_sector_configuration(const ru_ofh_configuration&        config,
                                                               const ru_ofh_sector_configuration& sector_cfg)
{
  // Prepare sector configuration.
  ofh::sector_configuration ofh_sector_config;

  ofh_sector_config.interface                   = sector_cfg.interface;
  ofh_sector_config.is_promiscuous_mode_enabled = sector_cfg.is_promiscuous_mode_enabled;
  ofh_sector_config.mtu_size                    = sector_cfg.mtu_size;
  ofh_sector_config.mac_dst_address             = sector_cfg.mac_dst_address;
  ofh_sector_config.mac_src_address             = sector_cfg.mac_src_address;
  ofh_sector_config.tci                         = sector_cfg.tci;
  ofh_sector_config.tx_window_timing_params     = sector_cfg.tx_window_timing_params;
  ofh_sector_config.rx_window_timing_params     = sector_cfg.rx_window_timing_params;
  ofh_sector_config.cp                          = sector_cfg.cp;
  ofh_sector_config.scs                         = sector_cfg.scs;
  ofh_sector_config.bw                          = sector_cfg.bw;
  ofh_sector_config.nof_antennas_ul             = sector_cfg.nof_antennas_ul;
  ofh_sector_config.ru_operating_bw = sector_cfg.ru_operating_bw ? sector_cfg.ru_operating_bw.value() : sector_cfg.bw;
  ofh_sector_config.prach_eaxc      = sector_cfg.prach_eaxc;
  ofh_sector_config.dl_eaxc         = sector_cfg.dl_eaxc;
  ofh_sector_config.ul_eaxc         = sector_cfg.ul_eaxc;
  ofh_sector_config.is_prach_control_plane_enabled       = sector_cfg.is_prach_control_plane_enabled;
  ofh_sector_config.is_downlink_broadcast_enabled        = sector_cfg.is_downlink_broadcast_enabled;
  ofh_sector_config.ignore_ecpri_payload_size_field      = sector_cfg.ignore_ecpri_payload_size_field;
  ofh_sector_config.ignore_ecpri_seq_id_field            = sector_cfg.ignore_ecpri_seq_id_field;
  ofh_sector_config.ul_compression_params                = sector_cfg.ul_compression_params;
  ofh_sector_config.dl_compression_params                = sector_cfg.dl_compression_params;
  ofh_sector_config.prach_compression_params             = sector_cfg.prach_compression_params;
  ofh_sector_config.iq_scaling                           = sector_cfg.iq_scaling;
  ofh_sector_config.max_processing_delay_slots           = config.max_processing_delay_slots;
  ofh_sector_config.dl_processing_time                   = config.dl_processing_time;
  ofh_sector_config.is_uplink_static_compr_hdr_enabled   = sector_cfg.is_uplink_static_comp_hdr_enabled;
  ofh_sector_config.is_downlink_static_compr_hdr_enabled = sector_cfg.is_downlink_static_comp_hdr_enabled;
  ofh_sector_config.uses_dpdk                            = config.uses_dpdk;
  ofh_sector_config.tdd_config                           = sector_cfg.tdd_config;

  return ofh_sector_config;
}

/// Generates the OFH sector dependencies from the common Open FrontHaul dependencies.
static ofh::sector_dependencies generate_sector_dependencies(ru_ofh_sector_dependencies&& dependencies,
                                                             std::shared_ptr<ofh::uplane_rx_symbol_notifier> notifier)
{
  // Prepare sector configuration.
  ofh::sector_dependencies ofh_sector_dependencies;
  ofh_sector_dependencies.logger               = dependencies.logger;
  ofh_sector_dependencies.receiver_executor    = dependencies.receiver_executor;
  ofh_sector_dependencies.transmitter_executor = dependencies.transmitter_executor;
  ofh_sector_dependencies.downlink_executor    = dependencies.downlink_executor;
  ofh_sector_dependencies.notifier             = notifier;
  ofh_sector_dependencies.eth_gateway          = std::move(dependencies.eth_gateway);

  return ofh_sector_dependencies;
}

std::unique_ptr<radio_unit> srsran::create_ofh_ru(const ru_ofh_configuration& config,
                                                  ru_ofh_dependencies&&       dependencies)
{
  report_fatal_error_if_not(config.max_processing_delay_slots >= 1,
                            "max_processing_delay_slots option should be greater than or equal to 1");
  report_fatal_error_if_not(dependencies.timing_notifier, "Invalid timing notifier");

  ru_ofh_impl_dependencies ofh_dependencies;
  ofh_dependencies.logger          = dependencies.logger;
  ofh_dependencies.timing_notifier = dependencies.timing_notifier;

  // Create UL Rx symbol notifier.
  auto ul_data_notifier = std::make_shared<ru_ofh_rx_symbol_handler_impl>(*dependencies.rx_symbol_notifier);

  // Create sectors.
  for (unsigned i = 0, e = config.sector_configs.size(); i != e; ++i) {
    const auto& sector_cfg = config.sector_configs[i];

    if (sector_cfg.ru_operating_bw) {
      report_fatal_error_if_not(
          sector_cfg.ru_operating_bw.value() >= sector_cfg.bw,
          "The RU operating bandwidth should be greater than or equal to the bandwidth of the cell");
    }
    if (sector_cfg.is_downlink_broadcast_enabled) {
      report_fatal_error_if_not(
          sector_cfg.dl_eaxc.size() > 1,
          "The downlink broadcast option is only available when the number of downlink ports is greater than one");
    }

    // Create OFH sector.
    auto sector = ofh::create_ofh_sector(
        generate_sector_configuration(config, sector_cfg),
        generate_sector_dependencies(std::move(dependencies.sector_dependencies[i]), ul_data_notifier));
    report_fatal_error_if_not(sector, "Unable to create OFH sector");
    ofh_dependencies.sectors.emplace_back(std::move(sector));

    fmt::print("Initializing the Open FrontHaul Interface for sector#{}: ul_compr=[{},{}], dl_compr=[{},{}], "
               "prach_compr=[{},{}] prach_cp_enabled={}, downlink_broadcast={}.{}\n",
               i,
               to_string(sector_cfg.ul_compression_params.type),
               sector_cfg.ul_compression_params.data_width,
               to_string(sector_cfg.dl_compression_params.type),
               sector_cfg.dl_compression_params.data_width,
               to_string(sector_cfg.prach_compression_params.type),
               sector_cfg.prach_compression_params.data_width,
               sector_cfg.is_prach_control_plane_enabled,
               sector_cfg.is_downlink_broadcast_enabled,
               (sector_cfg.ru_operating_bw && sector_cfg.bw != *sector_cfg.ru_operating_bw)
                   ? fmt::format("\nOperating a {}MHz cell over a RU with instantaneous bandwidth of {}MHz.",
                                 sector_cfg.bw,
                                 *sector_cfg.ru_operating_bw)
                   : fmt::format(""));
  }

  // Prepare OFH controller configuration.
  ofh::controller_config controller_cfg;
  controller_cfg.cp        = config.sector_configs.back().cp;
  controller_cfg.scs       = config.sector_configs.back().scs;
  controller_cfg.gps_Alpha = config.gps_Alpha;
  controller_cfg.gps_Beta  = config.gps_Beta;

  // Create OFH timing controller.
  ofh_dependencies.timing_mngr =
      ofh::create_ofh_timing_manager(controller_cfg, *dependencies.logger, *dependencies.rt_timing_executor);
  report_fatal_error_if_not(ofh_dependencies.timing_mngr, "Unable to create OFH timing manager");

  ru_ofh_impl_config ru_config;
  ru_config.nof_slot_offset_du_ru = config.max_processing_delay_slots;
  ru_config.nof_symbols_per_slot  = get_nsymb_per_slot(config.sector_configs.back().cp);

  return std::make_unique<ru_ofh_impl>(ru_config, std::move(ofh_dependencies));
}

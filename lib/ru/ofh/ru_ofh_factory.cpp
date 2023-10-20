/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "ru_ofh_timing_handler.h"
#include "srsran/ofh/ofh_factories.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

/// Generates the OFH sector configuration from the common Open FrontHaul configuration.
static ofh::sector_configuration generate_sector_configuration(const ru_ofh_configuration&        config,
                                                               const ru_ofh_sector_configuration& sector_cfg,
                                                               ofh::uplane_rx_symbol_notifier*    notifier)
{
  // Prepare sector configuration.
  ofh::sector_configuration ofh_sector_config;

  ofh_sector_config.logger                      = config.logger;
  ofh_sector_config.transmitter_executor        = sector_cfg.transmitter_executor;
  ofh_sector_config.receiver_executor           = sector_cfg.receiver_executor;
  ofh_sector_config.downlink_executors          = sector_cfg.downlink_executors;
  ofh_sector_config.notifier                    = notifier;
  ofh_sector_config.interface                   = sector_cfg.interface;
  ofh_sector_config.is_promiscuous_mode_enabled = sector_cfg.is_promiscuous_mode_enabled;
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

std::unique_ptr<radio_unit> srsran::create_ofh_ru(const ru_ofh_configuration& config)
{
  report_fatal_error_if_not(config.max_processing_delay_slots >= 1,
                            "max_processing_delay_slots option should be greater or equal to 1");

  ru_ofh_impl_dependencies ofh_deps;

  // Create UL Rx symbol notifier.
  ofh_deps.ul_data_notifier = std::make_unique<ru_ofh_rx_symbol_handler_impl>(*config.rx_symbol_notifier);

  // Create sectors.
  std::vector<ofh::ota_symbol_handler*> symbol_handlers;
  for (unsigned i = 0, e = config.sector_configs.size(); i != e; ++i) {
    const auto& sector_cfg = config.sector_configs[i];

    if (sector_cfg.ru_operating_bw) {
      report_fatal_error_if_not(sector_cfg.ru_operating_bw.value() >= sector_cfg.bw,
                                "The RU operating bandwidth should be greater or equal to the bandwidth of the cell");
    }
    if (sector_cfg.is_downlink_broadcast_enabled) {
      report_fatal_error_if_not(
          sector_cfg.dl_eaxc.size() > 1,
          "Downlink broadcast option only available when the number of downlink ports is more than one");
    }

    fmt::print("Initializing Open Fronthaul Interface sector={}: ul_comp=[{},{}], dl_comp=[{},{}], prach_comp=[{},{}] "
               "prach_cp_enabled={}, downlink_broadcast={}.{}\n",
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

    // Create OFH sector.
    auto sector =
        ofh::create_ofh_sector(generate_sector_configuration(config, sector_cfg, ofh_deps.ul_data_notifier.get()));
    report_fatal_error_if_not(sector, "Unable to create OFH sector");
    ofh_deps.sectors.emplace_back(std::move(sector));

    // Add the symbol handlers to the list of handlers.
    symbol_handlers.push_back(&ofh_deps.sectors.back()->get_transmitter().get_ota_symbol_handler());
    symbol_handlers.push_back(&ofh_deps.sectors.back()->get_receiver().get_ota_symbol_handler());
  }

  // Create OFH OTA symbol notifier.
  ofh_deps.symbol_notifier =
      ofh::create_ofh_ota_symbol_notifier(config.max_processing_delay_slots,
                                          get_nsymb_per_slot(config.sector_configs.back().cp),
                                          std::make_unique<ru_ofh_timing_handler>(*config.timing_notifier),
                                          symbol_handlers);
  report_fatal_error_if_not(ofh_deps.symbol_notifier, "Unable to create OFH OTA symbol notifier");

  // Prepare OFH controller configuration.
  ofh::controller_config controller_cfg;
  controller_cfg.logger    = config.logger;
  controller_cfg.notifier  = ofh_deps.symbol_notifier.get();
  controller_cfg.executor  = config.rt_timing_executor;
  controller_cfg.cp        = config.sector_configs.back().cp;
  controller_cfg.scs       = config.sector_configs.back().scs;
  controller_cfg.gps_Alpha = config.gps_Alpha;
  controller_cfg.gps_Beta  = config.gps_Beta;

  // Create OFH timing controller.
  ofh_deps.timing_controller = ofh::create_ofh_timing_controller(controller_cfg);
  report_fatal_error_if_not(ofh_deps.timing_controller, "Unable to create OFH timing controller");

  return std::make_unique<ru_ofh_impl>(*config.logger, std::move(ofh_deps));
}

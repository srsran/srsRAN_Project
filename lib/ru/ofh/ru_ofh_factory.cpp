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
#include "srsran/ofh/ofh_receiver.h"
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

  ofh_sector_config.logger                  = config.logger;
  ofh_sector_config.receiver_executor       = sector_cfg.receiver_executor;
  ofh_sector_config.notifier                = notifier;
  ofh_sector_config.interface               = sector_cfg.interface;
  ofh_sector_config.mac_dst_address         = sector_cfg.mac_dst_address;
  ofh_sector_config.mac_src_address         = sector_cfg.mac_src_address;
  ofh_sector_config.tci                     = sector_cfg.tci;
  ofh_sector_config.tx_window_timing_params = config.tx_window_timing_params;
  ofh_sector_config.cp                      = config.cp;
  ofh_sector_config.scs                     = config.scs;
  ofh_sector_config.bw                      = config.bw;
  ofh_sector_config.ru_operating_bw         = config.ru_operating_bw ? config.ru_operating_bw.value() : config.bw;
  ofh_sector_config.ul_prach_eaxc           = sector_cfg.ul_prach_eaxc;
  ofh_sector_config.dl_eaxc                 = sector_cfg.dl_eaxc;
  ofh_sector_config.ul_eaxc                 = sector_cfg.ul_eaxc;
  ofh_sector_config.is_prach_control_plane_enabled = config.is_prach_control_plane_enabled;
  ofh_sector_config.is_downlink_broadcast_enabled  = config.is_downlink_broadcast_enabled;
  ofh_sector_config.ul_compression_params          = config.ul_compression_params;
  ofh_sector_config.dl_compression_params          = config.dl_compression_params;
  ofh_sector_config.iq_scaling                     = config.iq_scaling;
  ofh_sector_config.max_processing_delay_slots     = config.max_processing_delay_slots;

  return ofh_sector_config;
}

std::unique_ptr<radio_unit> srsran::create_ofh_ru(const ru_ofh_configuration& config)
{
  report_fatal_error_if_not(config.sector_configs.size() == 1,
                            "Open FrontHaul interface currently only supports a single sector");
  report_fatal_error_if_not(config.max_processing_delay_slots >= 1,
                            "max_processing_delay_slots option should be greater or equal to 1");
  if (config.ru_operating_bw) {
    report_fatal_error_if_not(config.ru_operating_bw.value() >= config.bw,
                              "The RU operating bandwidth should be greater or equal to the bandwidth of the cell");
  }

  fmt::print("Initializing Open Fronthaul Interface with ul_comp=[{},{}], dl_comp=[{},{}], prach_cp_enabled={}, "
             "downlink_broadcast={}.{}\n",
             to_string(config.ul_compression_params.type),
             config.ul_compression_params.data_width,
             to_string(config.dl_compression_params.type),
             config.dl_compression_params.data_width,
             config.is_prach_control_plane_enabled,
             config.is_downlink_broadcast_enabled,
             (config.ru_operating_bw && config.bw != *config.ru_operating_bw)
                 ? fmt::format("\nOperating a {}MHz cell over a RU with instantaneous bandwidth of {}MHz.",
                               config.bw,
                               *config.ru_operating_bw)
                 : fmt::format(""));

  ru_ofh_impl_dependencies ofh_deps;

  // Create UL Rx symbol notifier.
  ofh_deps.ul_data_notifier = std::make_unique<ru_ofh_rx_symbol_handler_impl>(*config.rx_symbol_notifier);

  // Create sectors.
  std::vector<ofh::symbol_handler*>               symbol_handlers;
  std::vector<ofh::ota_symbol_boundary_notifier*> ota_symbol_notifiers;
  for (const auto& sector_cfg : config.sector_configs) {
    if (config.is_downlink_broadcast_enabled) {
      report_fatal_error_if_not(
          sector_cfg.dl_eaxc.size() > 1,
          "Downlink broadcast option only available when the number of downlink ports is more than one");
    }

    // Create OFH sector.
    auto sector =
        ofh::create_ofh_sector(generate_sector_configuration(config, sector_cfg, ofh_deps.ul_data_notifier.get()));
    report_fatal_error_if_not(sector, "Unable to create OFH sector");
    ofh_deps.sectors.emplace_back(std::move(sector), sector_cfg.transmitter_executor);

    // Add the symbol handlers to the list of handlers.
    symbol_handlers.push_back(&ofh_deps.sectors.back().first->get_transmitter().get_symbol_handler());
    ota_symbol_notifiers.push_back(&ofh_deps.sectors.back().first->get_receiver().get_ota_symbol_notifier());
  }

  // Create OFH OTA symbol notifier.
  ofh_deps.symbol_notifier =
      ofh::create_ofh_ota_symbol_notifier(config.max_processing_delay_slots,
                                          get_nsymb_per_slot(config.cp),
                                          *config.logger,
                                          std::make_unique<ru_ofh_timing_handler>(*config.timing_notifier),
                                          symbol_handlers,
                                          ota_symbol_notifiers);
  report_fatal_error_if_not(ofh_deps.symbol_notifier, "Unable to create OFH OTA symbol notifier");

  // Prepare OFH controller configuration.
  ofh::controller_config controller_cfg;
  controller_cfg.logger    = config.logger;
  controller_cfg.notifier  = ofh_deps.symbol_notifier.get();
  controller_cfg.executor  = config.rt_timing_executor;
  controller_cfg.cp        = config.cp;
  controller_cfg.scs       = config.scs;
  controller_cfg.gps_Alpha = config.gps_Alpha;
  controller_cfg.gps_Beta  = config.gps_Beta;

  // Create OFH timing controller.
  ofh_deps.timing_controller = ofh::create_ofh_timing_controller(controller_cfg);
  report_fatal_error_if_not(ofh_deps.timing_controller, "Unable to create OFH timing controller");

  return std::make_unique<ru_ofh_impl>(*config.logger, std::move(ofh_deps));
}

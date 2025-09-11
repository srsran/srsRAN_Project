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

#include "srsran/ru/ofh/ru_ofh_factory.h"
#include "ru_ofh_impl.h"
#include "ru_ofh_rx_symbol_handler_impl.h"
#include "srsran/ofh/ofh_factories.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_ofh_ru(const ru_ofh_configuration& config,
                                                  ru_ofh_dependencies&&       dependencies)
{
  report_fatal_error_if_not(dependencies.timing_notifier, "Invalid timing notifier");

  ru_ofh_impl_dependencies ofh_dependencies;
  ofh_dependencies.logger             = dependencies.logger;
  ofh_dependencies.timing_notifier    = dependencies.timing_notifier;
  ofh_dependencies.error_notifier     = dependencies.error_notifier;
  ofh_dependencies.rx_symbol_notifier = dependencies.rx_symbol_notifier;

  // Prepare OFH controller configuration.
  ofh::controller_config controller_cfg;
  controller_cfg.cp                            = config.sector_configs.back().cp;
  controller_cfg.scs                           = config.sector_configs.back().scs;
  controller_cfg.gps_Alpha                     = config.gps_Alpha;
  controller_cfg.gps_Beta                      = config.gps_Beta;
  controller_cfg.enable_log_warnings_for_lates = config.sector_configs.back().enable_log_warnings_for_lates;

  // Create OFH timing controller.
  ofh_dependencies.timing_mngr =
      ofh::create_ofh_timing_manager(controller_cfg, *dependencies.logger, *dependencies.rt_timing_executor);
  report_fatal_error_if_not(ofh_dependencies.timing_mngr, "Unable to create OFH timing manager");

  ru_ofh_impl_config ru_config;
  ru_config.nof_slot_offset_du_ru = config.sector_configs.back().max_processing_delay_slots;
  ru_config.nof_symbols_per_slot  = get_nsymb_per_slot(config.sector_configs.back().cp);
  ru_config.scs                   = config.sector_configs.back().scs;

  auto ru_ofh = std::make_unique<ru_ofh_impl>(ru_config, std::move(ofh_dependencies));

  std::vector<std::unique_ptr<ofh::sector>> sectors;
  // Create sectors.
  for (unsigned i = 0, e = config.sector_configs.size(); i != e; ++i) {
    const auto& sector_cfg = config.sector_configs[i];

    report_fatal_error_if_not(sector_cfg.max_processing_delay_slots >= 1,
                              "max_processing_delay_slots option should be greater than or equal to 1");

    report_fatal_error_if_not(
        sector_cfg.ru_operating_bw >= sector_cfg.bw,
        "The RU operating bandwidth should be greater than or equal to the bandwidth of the cell");

    // Fill the notifier in the dependencies.
    dependencies.sector_dependencies[i].notifier     = &ru_ofh->get_uplane_rx_symbol_notifier();
    dependencies.sector_dependencies[i].err_notifier = &ru_ofh->get_error_notifier();

    // Create OFH sector.
    auto sector = ofh::create_ofh_sector(sector_cfg, std::move(dependencies.sector_dependencies[i]));
    report_fatal_error_if_not(sector, "Unable to create OFH sector");
    sectors.emplace_back(std::move(sector));

    fmt::println("Initializing the Open Fronthaul Interface for sector#{}: ul_compr=[{},{}], dl_compr=[{},{}], "
                 "prach_compr=[{},{}], prach_cp_enabled={}{}",
                 i,
                 to_string(sector_cfg.ul_compression_params.type),
                 sector_cfg.ul_compression_params.data_width,
                 to_string(sector_cfg.dl_compression_params.type),
                 sector_cfg.dl_compression_params.data_width,
                 to_string(sector_cfg.prach_compression_params.type),
                 sector_cfg.prach_compression_params.data_width,
                 sector_cfg.is_prach_control_plane_enabled,
                 (sector_cfg.bw != sector_cfg.ru_operating_bw)
                     ? fmt::format(".\nOperating a {}MHz cell over a RU with instantaneous bandwidth of {}MHz",
                                   fmt::underlying(sector_cfg.bw),
                                   fmt::underlying(sector_cfg.ru_operating_bw))
                     : fmt::format(""));
  }

  ru_ofh->set_ofh_sectors(std::move(sectors));

  return ru_ofh;
}

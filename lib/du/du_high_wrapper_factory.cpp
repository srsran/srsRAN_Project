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

#include "srsran/du/du_high_wrapper_factory.h"
#include "adapters/fapi_factory.h"
#include "du_high_wrapper_impl.h"
#include "srsran/du_high/du_high_factory.h"
#include "srsran/fapi/buffered_decorator_factories.h"
#include "srsran/fapi/logging_decorator_factories.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"

using namespace srsran;

static std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>
build_fapi_adaptors(const du_cell_config& du_cell, du_high_wrapper_sector_dependencies& dependencies, unsigned sector)
{
  srsran_assert(dependencies.gateway, "Invalid gateway");
  srsran_assert(dependencies.last_msg_notifier, "Invalid last message notifier");

  const subcarrier_spacing scs             = du_cell.scs_common;
  auto                     pm_tools        = fapi_adaptor::generate_precoding_matrix_tables(du_cell.dl_carrier.nof_ant);
  auto                     uci_part2_tools = fapi_adaptor::generate_uci_part2_correspondence(1);

  return build_mac_fapi_adaptor(
      sector,
      scs,
      *dependencies.gateway,
      *dependencies.last_msg_notifier,
      std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(pm_tools)),
      std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(uci_part2_tools)),
      get_max_Nprb(du_cell.dl_carrier.carrier_bw_mhz, scs, frequency_range::FR1));
}

std::unique_ptr<du_high_wrapper> srsran::make_du_high_wrapper(const du_high_wrapper_config&  config,
                                                              du_high_wrapper_dependencies&& wrapper_dependencies)
{
  srsran_assert(config.du_hi.cells.size() == wrapper_dependencies.sectors.size(),
                "DU high number of cells '{}' does not match number sectors '{}' in wrapper dependencies",
                config.du_hi.cells.size(),
                wrapper_dependencies.sectors.size());

  du_high_wrapper_impl_dependencies dependencies;

  for (unsigned i = 0, e = wrapper_dependencies.sectors.size(); i != e; ++i) {
    dependencies.du_high_adaptor.push_back(
        build_fapi_adaptors(config.du_hi.cells[i], wrapper_dependencies.sectors[i], i));
  }

  auto& logger        = srslog::fetch_basic_logger("DU");
  dependencies.logger = &logger;

  logger.debug("FAPI adaptors created successfully");

  auto wrapper_du = std::make_unique<du_high_wrapper_impl>(std::move(dependencies));

  // Instantiate DU-high.
  srs_du::du_high_configuration du_hi_cfg = config.du_hi;
  du_hi_cfg.phy_adapter                   = &wrapper_du->get_mac_result_notifier();
  wrapper_du->set_du_high(make_du_high(du_hi_cfg));

  logger.info("DU created successfully");

  return wrapper_du;
}
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

#include "srsran/du/du_low/o_du_low_factory.h"
#include "o_du_low_impl.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_low/du_low_factory.h"
#include "srsran/du/du_low/o_du_low_config.h"
#include "srsran/fapi/messages/config_request_tlvs.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"

using namespace srsran;
using namespace srs_du;

static fapi_adaptor::phy_fapi_adaptor_config generate_fapi_adaptor_config(const o_du_low_config&     odu_low_cfg,
                                                                          span<const du_cell_config> du_cell)
{
  fapi_adaptor::phy_fapi_adaptor_config out_config;

  for (unsigned i = 0, e = du_cell.size(); i != e; ++i) {
    const upper_phy_config& upper_cfg = odu_low_cfg.du_low_cfg.cells[i].upper_phy_cfg;
    const du_cell_config&   cell_cfg  = du_cell[i];
    out_config.sectors.push_back(
        {i,
         upper_cfg.nof_slots_request_headroom,
         upper_cfg.allow_request_on_empty_uplink_slot,
         cell_cfg,
         std::vector<uint8_t>(odu_low_cfg.prach_ports[i].begin(), odu_low_cfg.prach_ports[i].end())});
  }

  return out_config;
}

static fapi_adaptor::phy_fapi_adaptor_dependencies
generate_fapi_adaptor_dependencies(du_low& du_low, span<const du_cell_config> du_cell)
{
  fapi_adaptor::phy_fapi_adaptor_dependencies out_dependencies;

  for (unsigned i = 0, e = du_cell.size(); i != e; ++i) {
    auto&      dependencies = out_dependencies.sectors.emplace_back();
    upper_phy& upper        = du_low.get_upper_phy(i);

    dependencies.logger               = &srslog::fetch_basic_logger("FAPI");
    dependencies.dl_processor_pool    = &upper.get_downlink_processor_pool();
    dependencies.dl_rg_pool           = &upper.get_downlink_resource_grid_pool();
    dependencies.dl_pdu_validator     = &upper.get_downlink_pdu_validator();
    dependencies.ul_request_processor = &upper.get_uplink_request_processor();
    dependencies.ul_rg_pool           = &upper.get_uplink_resource_grid_pool();
    dependencies.ul_pdu_repository    = &upper.get_uplink_pdu_slot_repository();
    dependencies.ul_pdu_validator     = &upper.get_uplink_pdu_validator();
    dependencies.pm_repo              = std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_repository>>(
        fapi_adaptor::generate_precoding_matrix_tables(du_cell[i].dl_carrier.nof_ant, i)));
    dependencies.part2_repo = std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_repository>>(
        fapi_adaptor::generate_uci_part2_correspondence(1)));
  }

  return out_dependencies;
}

std::unique_ptr<o_du_low> srsran::srs_du::make_o_du_low(const o_du_low_config&     config,
                                                        span<const du_cell_config> du_cells)
{
  srsran_assert(config.du_low_cfg.cells.size() == du_cells.size(),
                "Number of cells mismatch between upper PHY '{}' and DU high '{}'",
                config.du_low_cfg.cells.size(),
                du_cells.size());
  srsran_assert(config.du_low_cfg.cells.size() == config.prach_ports.size(),
                "Number of cells mismatch between upper PHY '{}' and cell PRACH ports '{}'",
                config.du_low_cfg.cells.size(),
                config.prach_ports.size());

  auto& logger = srslog::fetch_basic_logger("DU");

  du_low_dependencies du_low_deps;
  du_low_deps.logger = &logger;

  auto du_lo = make_du_low(config.du_low_cfg, std::move(du_low_deps));

  report_error_if_not(du_lo != nullptr, "Unable to create DU low.");
  logger.debug("DU low created successfully");

  auto fapi = fapi_adaptor::create_phy_fapi_adaptor_factory()->create(
      generate_fapi_adaptor_config(config, du_cells), generate_fapi_adaptor_dependencies(*du_lo, du_cells));
  report_error_if_not(fapi != nullptr, "Unable to create FAPI-PHY adpators.");

  auto o_du_lo = std::make_unique<o_du_low_impl>(std::move(du_lo), std::move(fapi), du_cells.size());
  report_error_if_not(o_du_lo != nullptr, "Unable to create O-DU low.");

  return o_du_lo;
}

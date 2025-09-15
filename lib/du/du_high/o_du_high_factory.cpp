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

#include "srsran/du/du_high/o_du_high_factory.h"
#include "o_du_high_impl.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/du/du_high/du_high_factory.h"
#include "srsran/du/du_high/o_du_high_config.h"
#include "srsran/e2/e2_du_factory.h"
#include "srsran/fapi/decorator_factory.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor_factory.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "srsran/ran/band_helper.h"

using namespace srsran;
using namespace srs_du;

static fapi_adaptor::mac_fapi_adaptor_config generate_fapi_adaptor_config(const o_du_high_config& config)
{
  fapi_adaptor::mac_fapi_adaptor_config out_config;

  for (unsigned i = 0, e = config.du_hi.ran.cells.size(); i != e; ++i) {
    const auto& du_cell = config.du_hi.ran.cells[i];
    unsigned    nof_prb = get_max_Nprb(
        du_cell.dl_carrier.carrier_bw_mhz, du_cell.scs_common, band_helper::get_freq_range(du_cell.dl_carrier.band));
    out_config.sectors.push_back(
        {i, nof_prb, du_cell.scs_common, config.fapi.log_level, config.fapi.l2_nof_slots_ahead});
  }

  return out_config;
}

static fapi_adaptor::mac_fapi_adaptor_dependencies
generate_fapi_adaptor_dependencies(const o_du_high_config& config, o_du_high_dependencies& odu_dependencies)
{
  fapi_adaptor::mac_fapi_adaptor_dependencies out_dependencies;

  for (unsigned i = 0, e = config.du_hi.ran.cells.size(); i != e; ++i) {
    auto& sector_dependencies = odu_dependencies.sectors[i];
    out_dependencies.sectors.push_back(
        {sector_dependencies.gateway,
         sector_dependencies.last_msg_notifier,
         std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(
             fapi_adaptor::generate_precoding_matrix_tables(config.du_hi.ran.cells[i].dl_carrier.nof_ant, i))),
         std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(
             fapi_adaptor::generate_uci_part2_correspondence(1))),
         sector_dependencies.fapi_executor});
  }

  return out_dependencies;
}

std::unique_ptr<o_du_high> srsran::srs_du::make_o_du_high(const o_du_high_config&  config,
                                                          o_du_high_dependencies&& odu_dependencies)
{
  o_du_high_impl_dependencies dependencies;
  srslog::basic_logger*       logger = &srslog::fetch_basic_logger("DU");
  dependencies.logger                = logger;
  dependencies.du_high_adaptor       = fapi_adaptor::create_mac_fapi_adaptor_factory()->create(
      generate_fapi_adaptor_config(config), generate_fapi_adaptor_dependencies(config, odu_dependencies));
  dependencies.metrics_notifier = odu_dependencies.du_hi.du_notifier;

  logger->debug("FAPI adaptors created successfully");

  srs_du::du_high_configuration du_hi_cfg = config.du_hi;

  auto odu = std::make_unique<o_du_high_impl>(config.du_hi.ran.cells.size(), std::move(dependencies));

  // Resolve dependencies for DU-high.
  odu_dependencies.du_hi.phy_adapter = &odu->get_mac_result_notifier();
  odu_dependencies.du_hi.du_notifier = &odu->get_du_metrics_notifier();

  if (!odu_dependencies.e2_client) {
    odu->set_du_high(make_du_high(du_hi_cfg, odu_dependencies.du_hi));
    logger->info("DU created successfully");

    return odu;
  }

  auto du_hi = make_du_high(du_hi_cfg, odu_dependencies.du_hi);

  auto e2agent = create_e2_du_agent(config.e2ap_config,
                                    *odu_dependencies.e2_client,
                                    odu_dependencies.e2_du_metric_iface,
                                    &du_hi->get_f1ap_du(),
                                    &du_hi->get_du_configurator(),
                                    timer_factory{odu_dependencies.du_hi.timer_ctrl->get_timer_manager(),
                                                  odu_dependencies.du_hi.exec_mapper->du_e2_executor()},
                                    odu_dependencies.du_hi.exec_mapper->du_e2_executor());

  odu->set_e2_agent(std::move(e2agent));
  odu->set_du_high(std::move(du_hi));
  logger->info("DU created successfully");

  return odu;
}

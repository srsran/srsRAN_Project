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

#include "phy_fapi_adaptor_factory_impl.h"
#include "phy_fapi_adaptor_impl.h"
#include "phy_fapi_sector_adaptor_impl.h"
#include "srsran/du/du_cell_config.h"

using namespace srsran;
using namespace fapi_adaptor;

static fapi::prach_config generate_prach_config_tlv(const srs_du::du_cell_config& cell_cfg)
{
  fapi::prach_config config     = {};
  config.prach_res_config_index = 0;
  config.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  config.prach_scs              = prach_subcarrier_spacing::kHz1_25;
  config.prach_ul_bwp_pusch_scs = cell_cfg.scs_common;
  config.restricted_set         = restricted_set_config::UNRESTRICTED;
  config.num_prach_fd_occasions = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm;
  config.prach_config_index =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index;
  config.prach_format           = prach_format_type::zero;
  config.num_prach_td_occasions = 1;
  config.num_preambles          = 1;
  config.start_preamble_index   = 0;

  // Add FD occasion info.
  fapi::prach_fd_occasion_config& fd_occasion = config.fd_occasions.emplace_back();
  fd_occasion.prach_root_sequence_index =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().prach_root_seq_index;
  fd_occasion.prach_freq_offset =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start;
  fd_occasion.prach_zero_corr_conf =
      cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config;

  return config;
}

static fapi::carrier_config generate_carrier_config_tlv(const srs_du::du_cell_config& du_cell)
{
  // Deduce common numerology and grid size for DL and UL.
  unsigned numerology = to_numerology_value(du_cell.scs_common);
  unsigned grid_size_bw_prb =
      band_helper::get_n_rbs_from_bw(MHz_to_bs_channel_bandwidth(du_cell.dl_carrier.carrier_bw_mhz),
                                     du_cell.scs_common,
                                     band_helper::get_freq_range(du_cell.dl_carrier.band));

  fapi::carrier_config fapi_config = {};

  // NOTE; for now we only need to fill the nof_prb_ul_grid and nof_prb_dl_grid for the common SCS.
  fapi_config.dl_grid_size             = {};
  fapi_config.dl_grid_size[numerology] = grid_size_bw_prb;
  fapi_config.ul_grid_size             = {};
  fapi_config.ul_grid_size[numerology] = grid_size_bw_prb;

  // Number of transmit and receive antenna ports.
  fapi_config.num_tx_ant = du_cell.dl_carrier.nof_ant;
  fapi_config.num_rx_ant = du_cell.ul_carrier.nof_ant;

  return fapi_config;
}

std::unique_ptr<phy_fapi_adaptor> phy_fapi_adaptor_factory_impl::create(const phy_fapi_adaptor_config&  config,
                                                                        phy_fapi_adaptor_dependencies&& dependencies)
{
  std::vector<std::unique_ptr<phy_fapi_sector_adaptor>> sectors;
  for (unsigned i = 0, e = config.sectors.size(); i != e; ++i) {
    const auto& sector_cfg          = config.sectors[i];
    auto&       sector_dependencies = dependencies.sectors[i];

    phy_fapi_sector_adaptor_impl_config adaptor_config;
    adaptor_config.sector_id                     = sector_cfg.sector_id;
    adaptor_config.nof_slots_request_headroom    = sector_cfg.nof_slots_request_headroom;
    adaptor_config.allow_request_on_empty_ul_tti = sector_cfg.allow_request_on_empty_ul_tti;
    adaptor_config.scs                           = sector_cfg.du_cell.scs_common;
    adaptor_config.scs_common                    = sector_cfg.du_cell.scs_common;
    adaptor_config.prach_cfg                     = generate_prach_config_tlv(sector_cfg.du_cell);
    adaptor_config.carrier_cfg                   = generate_carrier_config_tlv(sector_cfg.du_cell);
    adaptor_config.prach_ports                   = sector_cfg.prach_ports;

    phy_fapi_sector_adaptor_impl_dependencies adaptor_dependencies;
    adaptor_dependencies.logger               = sector_dependencies.logger;
    adaptor_dependencies.dl_processor_pool    = sector_dependencies.dl_processor_pool;
    adaptor_dependencies.dl_rg_pool           = sector_dependencies.dl_rg_pool;
    adaptor_dependencies.dl_pdu_validator     = sector_dependencies.dl_pdu_validator;
    adaptor_dependencies.ul_request_processor = sector_dependencies.ul_request_processor;
    adaptor_dependencies.ul_rg_pool           = sector_dependencies.ul_rg_pool;
    adaptor_dependencies.ul_pdu_repository    = sector_dependencies.ul_pdu_repository;
    adaptor_dependencies.ul_pdu_validator     = sector_dependencies.ul_pdu_validator;
    adaptor_dependencies.pm_repo              = std::move(sector_dependencies.pm_repo);
    adaptor_dependencies.part2_repo           = std::move(sector_dependencies.part2_repo);

    sectors.push_back(std::make_unique<phy_fapi_sector_adaptor_impl>(adaptor_config, std::move(adaptor_dependencies)));
  }

  return std::make_unique<phy_fapi_adaptor_impl>(std::move(sectors));
}

std::unique_ptr<phy_fapi_adaptor_factory> srsran::fapi_adaptor::create_phy_fapi_adaptor_factory()
{
  return std::make_unique<phy_fapi_adaptor_factory_impl>();
}

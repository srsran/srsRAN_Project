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

#include "srsran/du_low/du_low_wrapper_factory.h"
#include "du_low_wrapper_impl.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du_low/du_low_factory.h"
#include "srsran/du_low/du_low_wrapper_config.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"

using namespace srsran;

static fapi::prach_config generate_prach_config_tlv(const du_cell_config& cell_cfg)
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

static fapi::carrier_config generate_carrier_config_tlv(const du_cell_config& du_cell)
{
  // Deduce common numerology and grid size for DL and UL.
  unsigned numerology       = to_numerology_value(du_cell.scs_common);
  unsigned grid_size_bw_prb = band_helper::get_n_rbs_from_bw(
      MHz_to_bs_channel_bandwidth(du_cell.dl_carrier.carrier_bw_mhz),
      du_cell.scs_common,
      band_helper::get_freq_range(band_helper::get_band_from_dl_arfcn(du_cell.dl_carrier.arfcn)));

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

static std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> create_phy_fapi_adaptor(upper_phy&              upper,
                                                                               const upper_phy_config& upper_cfg,
                                                                               const du_cell_config&   du_cell,
                                                                               span<const uint8_t>     prach_ports)
{
  const subcarrier_spacing scs             = du_cell.scs_common;
  auto                     pm_tools        = fapi_adaptor::generate_precoding_matrix_tables(du_cell.dl_carrier.nof_ant);
  auto                     uci_part2_tools = fapi_adaptor::generate_uci_part2_correspondence(1);

  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor_factory> adaptor_factory =
      fapi_adaptor::create_phy_fapi_adaptor_factory();
  report_error_if_not(adaptor_factory, "Invalid PHY adaptor factory.");

  auto prach_tlv   = generate_prach_config_tlv(du_cell);
  auto carrier_tlv = generate_carrier_config_tlv(du_cell);

  fapi_adaptor::phy_fapi_adaptor_factory_config phy_fapi_config;
  phy_fapi_config.sector_id                  = upper_cfg.sector_id;
  phy_fapi_config.nof_slots_request_headroom = upper_cfg.nof_slots_request_headroom;
  phy_fapi_config.scs                        = scs;
  phy_fapi_config.scs_common                 = scs;
  phy_fapi_config.prach_cfg                  = &prach_tlv;
  phy_fapi_config.carrier_cfg                = &carrier_tlv;
  phy_fapi_config.prach_ports                = std::vector<uint8_t>(prach_ports.begin(), prach_ports.end());

  fapi_adaptor::phy_fapi_adaptor_factory_dependencies phy_fapi_dependencies;
  phy_fapi_dependencies.logger               = &srslog::fetch_basic_logger("FAPI");
  phy_fapi_dependencies.dl_processor_pool    = &upper.get_downlink_processor_pool();
  phy_fapi_dependencies.dl_rg_pool           = &upper.get_downlink_resource_grid_pool();
  phy_fapi_dependencies.dl_pdu_validator     = &upper.get_downlink_pdu_validator();
  phy_fapi_dependencies.ul_request_processor = &upper.get_uplink_request_processor();
  phy_fapi_dependencies.ul_rg_pool           = &upper.get_uplink_resource_grid_pool();
  phy_fapi_dependencies.ul_pdu_repository    = &upper.get_uplink_slot_pdu_repository();
  phy_fapi_dependencies.ul_pdu_validator     = &upper.get_uplink_pdu_validator();
  phy_fapi_dependencies.pm_repo =
      std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_repository>>(pm_tools));
  phy_fapi_dependencies.part2_repo =
      std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_repository>>(uci_part2_tools));

  return adaptor_factory->create(phy_fapi_config, std::move(phy_fapi_dependencies));
}

std::unique_ptr<du_low_wrapper> srsran::make_du_low_wrapper(const du_low_wrapper_config& config,
                                                            span<const du_cell_config>   du_cells)
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
  auto  du_lo  = make_du_low(config.du_low_cfg);

  report_error_if_not(du_lo != nullptr, "Unable to create DU low.");
  logger.debug("DU low created successfully");

  // Instantiate adaptor of FAPI to DU-low.
  std::vector<std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>> fapi_adaptors(config.du_low_cfg.cells.size());
  for (unsigned i = 0, e = config.du_low_cfg.cells.size(); i != e; ++i) {
    fapi_adaptors[i] = create_phy_fapi_adaptor(
        du_lo->get_upper_phy(i), config.du_low_cfg.cells[i].upper_phy_cfg, du_cells[i], config.prach_ports[i]);

    report_error_if_not(fapi_adaptors.back(), "Unable to create PHY adaptor for cell '{}'", i);
  }

  logger.debug("DU-low wrapper created successfully");

  return std::make_unique<du_low_wrapper_impl>(std::move(du_lo), std::move(fapi_adaptors));
}

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

#include "du_impl.h"
#include "adapters/fapi_factory.h"
#include "srsran/du_high/du_high_factory.h"
#include "srsran/fapi/logging_decorator_factories.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"

using namespace srsran;

namespace {

/// Dummy implementation of the mac_result_notifier.
class phy_dummy : public mac_result_notifier
{
  mac_cell_result_notifier& cell;

public:
  explicit phy_dummy(mac_cell_result_notifier& cell_) : cell(cell_) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
};

class fapi_slot_last_message_dummy : public fapi::slot_last_message_notifier
{
public:
  void on_last_message(slot_point slot) override {}
};

} // namespace

static fapi::prach_config generate_prach_config_tlv(const std::vector<du_cell_config>& cell_cfg)
{
  srsran_assert(cell_cfg.size() == 1, "Currently supporting one cell");

  const du_cell_config& cell = cell_cfg.front();

  fapi::prach_config config     = {};
  config.prach_res_config_index = 0;
  config.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  config.prach_scs              = prach_subcarrier_spacing::kHz1_25;
  config.prach_ul_bwp_pusch_scs = cell.scs_common;
  config.restricted_set         = restricted_set_config::UNRESTRICTED;
  config.num_prach_fd_occasions = cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm;
  config.prach_config_index =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index;
  config.prach_format           = prach_format_type::zero;
  config.num_prach_td_occasions = 1;
  config.num_preambles          = 1;
  config.start_preamble_index   = 0;

  // Add FD occasion info.
  fapi::prach_fd_occasion_config& fd_occasion = config.fd_occasions.emplace_back();
  fd_occasion.prach_root_sequence_index = cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().prach_root_seq_index;
  fd_occasion.prach_freq_offset =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start;
  fd_occasion.prach_zero_corr_conf =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config;

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

du_impl::du_impl(const du_config& du_cfg) :
  logger(srslog::fetch_basic_logger("DU")),
  du_lo(std::make_unique<du_low_impl>(du_cfg.du_lo)),
  last_msg_notifier(std::make_unique<fapi_slot_last_message_dummy>())
{
  report_error_if_not(du_lo != nullptr, "Unable to instantiate DU-low.");
  report_error_if_not(du_cfg.du_hi.cells.size() == 1, "Only one cell per DU is supported");

  // Create FAPI adaptors.
  const du_cell_config&    du_cell         = du_cfg.du_hi.cells.front();
  const upper_phy_config&  upper_phy_cfg   = du_cfg.du_lo.upper_phy.front();
  const unsigned           sector          = du_cfg.fapi.sector;
  const subcarrier_spacing scs             = du_cell.scs_common;
  auto                     pm_tools        = fapi_adaptor::generate_precoding_matrix_tables(du_cell.dl_carrier.nof_ant);
  auto                     uci_part2_tools = fapi_adaptor::generate_uci_part2_correspondence(1);

  // Instantiate adaptor of FAPI to DU-low.
  du_low_adaptor = build_phy_fapi_adaptor(
      sector,
      upper_phy_cfg.nof_slots_request_headroom,
      scs,
      scs,
      du_lo->get_downlink_processor_pool(),
      du_lo->get_downlink_resource_grid_pool(),
      du_lo->get_uplink_request_processor(),
      du_lo->get_uplink_resource_grid_pool(),
      du_lo->get_uplink_slot_pdu_repository(),
      du_lo->get_downlink_pdu_validator(),
      du_lo->get_uplink_pdu_validator(),
      generate_prach_config_tlv(du_cfg.du_hi.cells),
      generate_carrier_config_tlv(du_cell),
      std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_repository>>(pm_tools)),
      std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_repository>>(uci_part2_tools)),
      du_lo->get_tx_buffer_pool(),
      du_cfg.fapi.prach_ports);

  report_error_if_not(du_low_adaptor, "Unable to create PHY adaptor.");
  du_lo->set_rx_results_notifier(du_low_adaptor->get_rx_results_notifier());
  du_lo->set_timing_notifier(du_low_adaptor->get_timing_notifier());

  if (du_cfg.fapi.log_level == "debug") {
    // Create gateway loggers and intercept MAC adaptor calls.
    logging_slot_gateway = fapi::create_logging_slot_gateway(du_low_adaptor->get_slot_message_gateway());
    report_error_if_not(logging_slot_gateway, "Unable to create logger for slot data notifications.");
    du_high_adaptor = build_mac_fapi_adaptor(
        sector,
        scs,
        *logging_slot_gateway,
        *last_msg_notifier,
        std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(pm_tools)),
        std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(uci_part2_tools)),
        get_max_Nprb(du_cell.dl_carrier.carrier_bw_mhz, scs, srsran::frequency_range::FR1));

    // Create notification loggers.
    logging_slot_time_notifier = fapi::create_logging_slot_time_notifier(du_high_adaptor->get_slot_time_notifier());
    report_error_if_not(logging_slot_time_notifier, "Unable to create logger for slot time notifications.");
    logging_slot_error_notifier = fapi::create_logging_slot_error_notifier(du_high_adaptor->get_slot_error_notifier());
    report_error_if_not(logging_slot_error_notifier, "Unable to create logger for slot error notifications.");
    logging_slot_data_notifier = fapi::create_logging_slot_data_notifier(du_high_adaptor->get_slot_data_notifier());
    report_error_if_not(logging_slot_data_notifier, "Unable to create logger for slot data notifications.");

    // Connect the PHY adaptor with the loggers to intercept PHY notifications.
    du_low_adaptor->set_slot_time_message_notifier(*logging_slot_time_notifier);
    du_low_adaptor->set_slot_error_message_notifier(*logging_slot_error_notifier);
    du_low_adaptor->set_slot_data_message_notifier(*logging_slot_data_notifier);
  } else {
    du_high_adaptor = build_mac_fapi_adaptor(
        0,
        scs,
        du_low_adaptor->get_slot_message_gateway(),
        *last_msg_notifier,
        std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(pm_tools)),
        std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(uci_part2_tools)),
        get_max_Nprb(du_cell.dl_carrier.carrier_bw_mhz, scs, frequency_range::FR1));
    report_error_if_not(du_high_adaptor, "Unable to create MAC adaptor.");
    du_low_adaptor->set_slot_time_message_notifier(du_high_adaptor->get_slot_time_notifier());
    du_low_adaptor->set_slot_error_message_notifier(du_high_adaptor->get_slot_error_notifier());
    du_low_adaptor->set_slot_data_message_notifier(du_high_adaptor->get_slot_data_notifier());
  }
  logger.debug("FAPI adaptors created successfully");

  du_high_result_notifier = std::make_unique<phy_dummy>(du_high_adaptor->get_cell_result_notifier());

  // Instantiate DU-high.
  srs_du::du_high_configuration du_hi_cfg = du_cfg.du_hi;
  du_hi_cfg.phy_adapter                   = du_high_result_notifier.get();
  du_hi                                   = make_du_high(du_hi_cfg);
  report_error_if_not(du_hi != nullptr, "Unable to instantiate DU-high.");

  logger.info("DU created successfully");
}

void du_impl::start()
{
  logger.info("Starting DU...");
  du_hi->start();

  // Configure the FAPI -> DU interface.
  du_cell_index_t cell_id = to_du_cell_index(0);
  du_high_adaptor->set_cell_slot_handler(du_hi->get_slot_handler(cell_id));
  du_high_adaptor->set_cell_rach_handler(du_hi->get_rach_handler(cell_id));
  du_high_adaptor->set_cell_pdu_handler(du_hi->get_pdu_handler());
  du_high_adaptor->set_cell_crc_handler(du_hi->get_control_info_handler(cell_id));

  logger.info("DU started successfully");
}

void du_impl::stop()
{
  logger.info("Stopping DU...");
  du_hi->stop();
  logger.info("DU stopped successfully");
}

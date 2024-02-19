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

#pragma once

#include "fapi_to_phy_translator.h"
#include "phy_to_fapi_error_event_translator.h"
#include "phy_to_fapi_results_event_translator.h"
#include "phy_to_fapi_time_event_translator.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"

namespace srsran {
class uplink_request_processor;

namespace fapi {
struct carrier_config;
struct prach_config;
} // namespace fapi

namespace fapi_adaptor {

/// PHY/FAPI adaptor implementation configuration.
struct phy_fapi_adaptor_impl_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Request headroom size in slots.
  unsigned nof_slots_request_headroom;
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  subcarrier_spacing scs;
  /// Common subcarrier spacing, as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// FAPI PRACH configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::prach_config* prach_cfg;
  /// FAPI carrier configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::carrier_config* carrier_cfg;
  /// PRACH port list.
  std::vector<uint8_t> prach_ports;
};

/// PHY/FAPI adaptor implementation dependencies.
struct phy_fapi_adaptor_impl_dependencies {
  /// Logger.
  srslog::basic_logger* logger;
  /// Downlink processor pool.
  downlink_processor_pool* dl_processor_pool;
  /// Downlink resource grid pool.
  resource_grid_pool* dl_rg_pool;
  /// Downlink PDU validator.
  const downlink_pdu_validator* dl_pdu_validator;
  /// Uplink request processor.
  uplink_request_processor* ul_request_processor;
  /// Uplink resource grid pool.
  resource_grid_pool* ul_rg_pool;
  /// Uplink slot PDU repository.
  uplink_slot_pdu_repository* ul_pdu_repository;
  /// Uplink PDU validator.
  const uplink_pdu_validator* ul_pdu_validator;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
  /// UCI Part2 correspondence repository.
  std::unique_ptr<uci_part2_correspondence_repository> part2_repo;
};

/// \brief PHY&ndash;FAPI bidirectional adaptor implementation.
class phy_fapi_adaptor_impl : public phy_fapi_adaptor
{
public:
  /// Constructor for the PHY&ndash;FAPI bidirectional adaptor.
  phy_fapi_adaptor_impl(const phy_fapi_adaptor_impl_config& config, phy_fapi_adaptor_impl_dependencies&& dependencies);

  upper_phy_error_notifier& get_error_notifier() override;

  // See interface for documentation.
  upper_phy_timing_notifier& get_timing_notifier() override;

  // See interface for documentation.
  fapi::slot_message_gateway& get_slot_message_gateway() override;

  // See interface for documentation.
  upper_phy_rx_results_notifier& get_rx_results_notifier() override;

  // See interface for documentation.
  void set_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier) override;

  // See interface for documentation.
  void set_slot_error_message_notifier(fapi::slot_error_message_notifier& fapi_error_notifier) override;

  // See interface for documentation.
  void set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier) override;

private:
  /// PHY-to-FAPI uplink results events translator.
  phy_to_fapi_results_event_translator results_translator;
  /// FAPI-to-PHY message translator.
  fapi_to_phy_translator fapi_translator;
  /// PHY-to-FAPI time events translator.
  phy_to_fapi_time_event_translator time_translator;
  /// PHY-to-FAPI error events translator.
  phy_to_fapi_error_event_translator error_translator;
};

} // namespace fapi_adaptor
} // namespace srsran

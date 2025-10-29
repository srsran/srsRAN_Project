/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/messages/config_request_tlvs.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_repository.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class downlink_pdu_validator;
class downlink_processor_pool;
class resource_grid_pool;
class uplink_pdu_validator;
class uplink_request_processor;
class uplink_pdu_slot_repository_pool;

namespace srs_du {
struct du_cell_config;
} // namespace srs_du

namespace fapi_adaptor {

/// PHY-FAPI P7 sector fastpath adaptor configuration.
struct phy_fapi_p7_sector_fastpath_adaptor_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Request headroom size in slots.
  unsigned nof_slots_request_headroom;
  /// Allow uplink requests on empty UL_TTI.requests.
  bool allow_request_on_empty_ul_tti = false;
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  subcarrier_spacing scs;
  /// Common subcarrier spacing, as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// Carrier cell configuration.
  fapi::carrier_config carrier_cfg;
  /// PRACH cell configuration.
  fapi::prach_config prach_cfg;
  /// PRACH port list.
  std::vector<uint8_t> prach_ports;
  /// dBFS calibration value.
  float dBFS_calibration_value;
};

/// PHY-FAPI P7 sector fastpath adaptor dependencies.
struct phy_fapi_p7_sector_fastpath_adaptor_dependencies {
  /// Logger.
  srslog::basic_logger& logger;
  /// Downlink processor pool.
  downlink_processor_pool& dl_processor_pool;
  /// Downlink resource grid pool.
  resource_grid_pool& dl_rg_pool;
  /// Downlink PDU validator.
  const downlink_pdu_validator& dl_pdu_validator;
  /// Uplink request processor.
  uplink_request_processor& ul_request_processor;
  /// Uplink slot PDU repository.
  uplink_pdu_slot_repository_pool& ul_pdu_repository;
  /// Uplink PDU validator.
  const uplink_pdu_validator& ul_pdu_validator;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
  /// UCI Part2 correspondence repository.
  std::unique_ptr<uci_part2_correspondence_repository> part2_repo;
};

} // namespace fapi_adaptor
} // namespace srsran

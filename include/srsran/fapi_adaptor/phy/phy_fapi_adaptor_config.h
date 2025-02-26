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

#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_repository.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/srslog/logger.h"

namespace srsran {
class downlink_pdu_validator;
class downlink_processor_pool;
class resource_grid_pool;
class uplink_pdu_validator;
class uplink_request_processor;
class uplink_pdu_slot_repository;

namespace srs_du {
struct du_cell_config;
}

namespace fapi_adaptor {
/// PHY&ndash;FAPI sector adaptor configuration.
struct phy_fapi_sector_adaptor_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Request headroom size in slots.
  unsigned nof_slots_request_headroom;
  /// Allow uplink requests on empty UL_TTI.requests.
  bool allow_request_on_empty_ul_tti = false;
  /// DU cell configuration.
  const srs_du::du_cell_config& du_cell;
  /// PRACH port list.
  std::vector<uint8_t> prach_ports;
};

/// PHY&ndash;FAPI sector adaptor dependencies.
struct phy_fapi_sector_adaptor_dependencies {
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
  uplink_pdu_slot_repository_pool* ul_pdu_repository;
  /// Uplink PDU validator.
  const uplink_pdu_validator* ul_pdu_validator;
  /// Precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> pm_repo;
  /// UCI Part2 correspondence repository.
  std::unique_ptr<uci_part2_correspondence_repository> part2_repo;
};

/// PHY&ndash;FAPI adaptor configuration.
struct phy_fapi_adaptor_config {
  std::vector<phy_fapi_sector_adaptor_config> sectors;
};

/// PHY&ndash;FAPI adaptor dependencies.
struct phy_fapi_adaptor_dependencies {
  std::vector<phy_fapi_sector_adaptor_dependencies> sectors;
};
} // namespace fapi_adaptor
} // namespace srsran

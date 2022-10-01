/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "fapi_to_phy_translator.h"
#include "phy_to_fapi_results_event_translator.h"
#include "phy_to_fapi_time_event_translator.h"
#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor.h"

namespace srsgnb {
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
  /// Subcarrier spacing as per TS38.211 Section 4.2.
  subcarrier_spacing scs;
  /// Downlink processor pool.
  downlink_processor_pool* dl_processor_pool;
  /// Downlink resource grid pool.
  resource_grid_pool* dl_rg_pool;
  /// Uplink request processor.
  uplink_request_processor* ul_request_processor;
  /// Uplink resource grid pool.
  resource_grid_pool* ul_rg_pool;
  /// Uplink slot PDU repository.
  uplink_slot_pdu_repository* ul_pdu_repository;
  /// Common subcarrier spacing, as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// FAPI PRACH configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::prach_config* prach_cfg;
  /// FAPI carrier configuration TLV as per SCF-222 v4.0 section 3.3.2.4.
  const fapi::carrier_config* carrier_cfg;
};

/// \brief PHY&ndash;FAPI bidirectional adaptor implementation.
class phy_fapi_adaptor_impl : public phy_fapi_adaptor
{
public:
  /// Constructor for the PHY&ndash;FAPI bidirectional adaptor.
  explicit phy_fapi_adaptor_impl(const phy_fapi_adaptor_impl_config& config);

  // See interface for documentation.
  upper_phy_timing_notifier& get_timing_notifier() override;

  // See interface for documentation.
  fapi::slot_message_gateway& get_slot_message_gateway() override;

  // See interface for documentation.
  upper_phy_rx_results_notifier& get_rx_results_notifier() override;

  // See interface for documentation.
  void set_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier) override;

  // See interface for documentation.
  void set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier) override;

private:
  /// FAPI-to-PHY message translator.
  fapi_to_phy_translator fapi_translator;
  /// PHY-to-FAPI time events translator.
  phy_to_fapi_time_event_translator time_translator;
  /// PHY-to-FAPI uplink results events translator.
  phy_to_fapi_results_event_translator results_translator;
};

} // namespace fapi_adaptor
} // namespace srsgnb

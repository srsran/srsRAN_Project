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

/// Defines the configuration for the PHY-FAPI adaptor implementation.
struct phy_fapi_adaptor_impl_config {
  unsigned                    sector_id;
  downlink_processor_pool*    dl_processor_pool;
  resource_grid_pool*         dl_rg_pool;
  uplink_request_processor*   ul_request_processor;
  resource_grid_pool*         ul_rg_pool;
  uplink_slot_pdu_repository* ul_pdu_repository;
  subcarrier_spacing          scs_common;
  const fapi::prach_config*   prach_cfg;
  const fapi::carrier_config* carrier_cfg;
};

/// \brief PHY&ndash;FAPI bidirectional adaptor implementation.
class phy_fapi_adaptor_impl : public phy_fapi_adaptor
{
public:
  /// \brief Constructor for the PHY&ndash;FAPI bidirectional adaptor.
  ///
  /// \param[in] sector_id Sector identifier.
  /// \param[in] dl_processor_pool Downlink processor pool that will be used to process PDUs.
  /// \param[in] rg_pool Resource grid pool that will be used to process PDUs.
  /// \param[in] ul_request_processor Uplink request processor.
  /// \param[in] scs_common Common subcarrier spacing, as per TS38.331 Section 6.2.2.
  /// \param[in] prach_cfg PRACH configuration.
  /// \param[in] carrier_cfg Carrier configuration.
  phy_fapi_adaptor_impl(const phy_fapi_adaptor_impl_config& config);

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

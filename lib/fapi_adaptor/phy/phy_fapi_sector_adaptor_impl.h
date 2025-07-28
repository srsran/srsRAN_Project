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

#pragma once

#include "fapi_to_phy_translator.h"
#include "phy_to_fapi_error_event_translator.h"
#include "phy_to_fapi_results_event_translator.h"
#include "phy_to_fapi_time_event_translator.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_config.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"

namespace srsran {
class uplink_request_processor;

namespace fapi_adaptor {

/// \brief PHY&ndash;FAPI bidirectional adaptor sector implementation.
class phy_fapi_sector_adaptor_impl : public phy_fapi_sector_adaptor
{
public:
  /// Constructor for the PHY&ndash;FAPI bidirectional sector adaptor.
  phy_fapi_sector_adaptor_impl(const phy_fapi_sector_adaptor_config&  config,
                               phy_fapi_sector_adaptor_dependencies&& dependencies);

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
  void set_error_message_notifier(fapi::error_message_notifier& fapi_error_notifier) override;

  // See interface for documentation.
  void set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier) override;

  // See interface for documentation.
  fapi::slot_last_message_notifier& get_slot_last_message_notifier() override;

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

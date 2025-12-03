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

#include "configuration_procedure.h"
#include "srsran/fapi_adaptor/phy/p5/phy_fapi_p5_sector_adaptor.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief PHY-FAPI P5 sector operation request adaptor.
///
/// This P5 sector adaptor allows to configure a cell operation request notifier, that will be notified in every
/// START.request and STOP.request FAPI messages.
class phy_fapi_p5_sector_operation_request_adaptor : public phy_fapi_p5_sector_adaptor
{
  fapi::configuration_procedure gateway;

public:
  explicit phy_fapi_p5_sector_operation_request_adaptor(srslog::basic_logger& logger) : gateway(logger) {}

  // See interface for documentation.
  fapi::config_message_gateway& get_config_message_gateway() override;

  // See interface for documentation.
  void set_error_message_notifier(fapi::error_message_notifier& err_notifier) override;

  // See interface for documentation.
  void set_config_message_notifier(fapi::config_message_notifier& config_notifier) override;

  // Sets the FAPI cell operation request notifier to the given one.
  void set_cell_operation_request_notifier(fapi::cell_operation_request_notifier& cell_notifier);
};

} // namespace fapi_adaptor
} // namespace srsran

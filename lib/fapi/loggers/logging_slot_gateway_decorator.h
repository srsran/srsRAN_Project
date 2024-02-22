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

#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace fapi {

/// Adds logging information over the implemented interface.
class logging_slot_gateway_decorator : public slot_message_gateway
{
public:
  logging_slot_gateway_decorator(srslog::basic_logger& logger_, slot_message_gateway& gateway_) :
    logger(logger_), gateway(gateway_)
  {
  }

  // See interface for documentation.
  void dl_tti_request(const dl_tti_request_message& msg) override;

  // See interface for documentation.
  void ul_tti_request(const ul_tti_request_message& msg) override;

  // See interface for documentation.
  void ul_dci_request(const ul_dci_request_message& msg) override;

  // See interface for documentation.
  void tx_data_request(const tx_data_request_message& msg) override;

private:
  /// FAPI logger.
  srslog::basic_logger& logger;
  slot_message_gateway& gateway;
};

} // namespace fapi
} // namespace srsran

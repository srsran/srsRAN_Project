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

#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ether {

/// Implementation for the VLAN Ethernet frame decoder.
class vlan_frame_decoder_impl : public vlan_frame_decoder
{
public:
  explicit vlan_frame_decoder_impl(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  span<const uint8_t> decode(span<const uint8_t> frame, vlan_frame_params& eth_params) override;

private:
  srslog::basic_logger& logger;
};

} // namespace ether
} // namespace srsran

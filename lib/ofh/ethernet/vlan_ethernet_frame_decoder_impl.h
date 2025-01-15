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

#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ether {

/// Implementation for the VLAN Ethernet frame decoder.
class vlan_frame_decoder_impl : public vlan_frame_decoder
{
public:
  vlan_frame_decoder_impl(srslog::basic_logger& logger_, unsigned sector_id_) : logger(logger_), sector_id(sector_id_)
  {
  }

  // See interface for documentation.
  span<const uint8_t> decode(span<const uint8_t> frame, vlan_frame_params& eth_params) override;

private:
  srslog::basic_logger& logger;
  const unsigned        sector_id;
};

} // namespace ether
} // namespace srsran

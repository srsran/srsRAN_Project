/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUCCH detector declaration.

#pragma once

#include "srsgnb/phy/upper/channel_processors/pucch_detector.h"

namespace srsgnb {

/// PUCCH detector implementation for Formats 0 and 1.
class pucch_detector_impl : public pucch_detector
{
  // See interface for documentation.
  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader& /*grid*/,
                                                                 const format0_configuration& /*config*/) override
  {
    srsgnb_assert(false, "PUCCH Format 0 not implemented yet.");
    return {};
  }

  // See interface for documentation.
  pucch_uci_message detect(const resource_grid_reader&  grid,
                           const channel_estimate&      estimates,
                           const format1_configuration& config) override
  {
    return pucch_uci_message();
  }
};

} // namespace srsgnb

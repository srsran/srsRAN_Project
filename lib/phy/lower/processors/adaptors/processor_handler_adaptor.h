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

#include "srsran/phy/lower/lower_phy_downlink_handler.h"
#include "srsran/phy/lower/lower_phy_uplink_request_handler.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_request_handler.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_request_handler.h"

namespace srsran {

class shared_resource_grid;

/// Adapts the lower physical layer handlers to the internal processors request handlers.
class processor_handler_adaptor
{
public:
  /// Constructs the lower physical layer handler adaptor from the internal processor request handlers.
  processor_handler_adaptor(pdxch_processor_request_handler& pdxch_handler,
                            prach_processor_request_handler& prach_handler,
                            puxch_processor_request_handler& puxch_handler) :
    dl_handler(pdxch_handler), ul_request_handler(prach_handler, puxch_handler)
  {
  }

  /// Gets resource grid handler.
  lower_phy_downlink_handler& get_downlink_handler() { return dl_handler; }

  /// Gets demodulate request handler.
  lower_phy_uplink_request_handler& get_uplink_request_handler() { return ul_request_handler; }

private:
  /// Implements the lower physical layer downlink handler.
  class downlink_handler_adaptor : public lower_phy_downlink_handler
  {
  public:
    /// Connects the adaptor with the PDxCH processor handler.
    explicit downlink_handler_adaptor(pdxch_processor_request_handler& pdxch_handler_) : pdxch_handler(pdxch_handler_)
    {
    }

    // See interface for documentation.
    void handle_resource_grid(const resource_grid_context& context, const shared_resource_grid& grid) override;

  private:
    /// Actual PDxCH processor request handler.
    pdxch_processor_request_handler& pdxch_handler;
  };

  /// Implements the lower physical layer uplink request handler.
  class uplink_request_handler_adaptor : public lower_phy_uplink_request_handler
  {
  public:
    /// Connects the adaptor with the PRACH and PUxCH processor handlers.
    uplink_request_handler_adaptor(prach_processor_request_handler& prach_handler_,
                                   puxch_processor_request_handler& puxch_handler_) :
      prach_handler(prach_handler_), puxch_handler(puxch_handler_)
    {
    }

    // See interface for documentation.
    void request_prach_window(const prach_buffer_context& context, prach_buffer& buffer) override;

    // See interface for documentation.
    void request_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;

  private:
    /// Actual PRACH processor request handler.
    prach_processor_request_handler& prach_handler;
    /// Actual PUxCH processor request handler.
    puxch_processor_request_handler& puxch_handler;
  };

  downlink_handler_adaptor       dl_handler;
  uplink_request_handler_adaptor ul_request_handler;
};

} // namespace srsran

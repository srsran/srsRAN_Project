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

#include "srsgnb/fapi/messages.h"
#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <memory>

namespace srsgnb {

class downlink_processor_pool;
class resource_grid_pool;
class uplink_request_processor;

namespace fapi_adaptor {

/// Configuration parameters for the PHY&ndash;FAPI adaptor factory.
struct phy_fapi_adaptor_factory_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Common subcarrier spacing as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs_common;
  /// PRACH configuration as per SCF-222 v4.0 Section 3.3.2.4 TLV 0x1031.
  fapi::prach_config prach_cfg;
  /// Carrier configuration per SCF-222 v4.0 Section 3.3.2.4 TLV 0x102d.
  fapi::carrier_config carrier_cfg;
};

/// Factory to create \c phy_fapi_adaptor objects.
class phy_fapi_adaptor_factory
{
public:
  virtual ~phy_fapi_adaptor_factory() = default;

  /// Creates a \c phy_fapi_adaptor object using the given configuration.
  virtual std::unique_ptr<phy_fapi_adaptor> create(const phy_fapi_adaptor_factory_config& config) = 0;
};

/// Creates a \c phy_fapi_adaptor_factory object.
std::unique_ptr<phy_fapi_adaptor_factory>
create_phy_fapi_adaptor_factory(downlink_processor_pool&  dl_processor_pool,
                                resource_grid_pool&       rg_pool,
                                uplink_request_processor& ul_request_processor);

} // namespace fapi_adaptor
} // namespace srsgnb

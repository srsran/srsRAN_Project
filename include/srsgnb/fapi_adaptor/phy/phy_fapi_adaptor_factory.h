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

#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <memory>

namespace srsgnb {

class downlink_processor_pool;
class resource_grid_pool;

namespace fapi_adaptor {

struct phy_fapi_adaptor_factory_config {
  unsigned sector_id;
  /// Subcarrier spacing common, as per TS 331, Section 6.2.2,
  subcarrier_spacing scs_common;
};

/// Factory that creates phy_fapi adaptors.
class phy_fapi_adaptor_factory
{
public:
  virtual ~phy_fapi_adaptor_factory() = default;

  /// Creates and returns a phy_fapi_adaptor using the given configuration.
  virtual std::unique_ptr<phy_fapi_adaptor> create(phy_fapi_adaptor_factory_config config) = 0;
};

/// Creates and returns a phy_fapi_adaptor_factory;
std::unique_ptr<phy_fapi_adaptor_factory> create_phy_fapi_adaptor_factory(downlink_processor_pool& dl_processor_pool,
                                                                          resource_grid_pool&      rg_pool);

} // namespace fapi_adaptor
} // namespace srsgnb

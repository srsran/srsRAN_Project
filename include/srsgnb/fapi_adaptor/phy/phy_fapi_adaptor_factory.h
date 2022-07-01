/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_FACTORY_H
#define SRSGNB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_FACTORY_H

#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>

namespace srsgnb {

class downlink_processor_pool;
class resource_grid_pool;

namespace fapi_adaptor {

struct phy_fapi_adaptor_factory_config {
  unsigned sector_id;
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

#endif // SRSGNB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_FACTORY_H

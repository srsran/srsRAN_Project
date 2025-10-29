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

#include "phy_fapi_fastpath_adaptor.h"
#include "phy_fapi_fastpath_adaptor_config.h"

namespace srsran {
namespace fapi_adaptor {

/// Factory to create PHY-FAPI fastpath adaptors.
class phy_fapi_fastpath_adaptor_factory
{
public:
  virtual ~phy_fapi_fastpath_adaptor_factory() = default;

  /// Creates a PHY-FAPI fastpath adaptor using the given configuration and dependencies.
  virtual std::unique_ptr<phy_fapi_fastpath_adaptor> create(const phy_fapi_fastpath_adaptor_config& config,
                                                            phy_fapi_fastpath_adaptor_dependencies  dependencies) = 0;
};

/// Creates a PHY-FAPI fastpath adaptor factory.
std::unique_ptr<phy_fapi_fastpath_adaptor_factory> create_phy_fapi_fastpath_adaptor_factory();

} // namespace fapi_adaptor
} // namespace srsran

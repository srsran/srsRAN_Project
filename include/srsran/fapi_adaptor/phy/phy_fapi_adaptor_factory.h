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

#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_config.h"

namespace srsran {
namespace fapi_adaptor {

/// Factory to create \c phy_fapi_adaptor objects.
class phy_fapi_adaptor_factory
{
public:
  virtual ~phy_fapi_adaptor_factory() = default;

  /// Creates a \c phy_fapi_adaptor object using the given configuration and dependencies.
  virtual std::unique_ptr<phy_fapi_adaptor> create(const phy_fapi_adaptor_config&  config,
                                                   phy_fapi_adaptor_dependencies&& dependencies) = 0;
};

/// Creates a \c phy_fapi_adaptor_factory object.
std::unique_ptr<phy_fapi_adaptor_factory> create_phy_fapi_adaptor_factory();

} // namespace fapi_adaptor
} // namespace srsran

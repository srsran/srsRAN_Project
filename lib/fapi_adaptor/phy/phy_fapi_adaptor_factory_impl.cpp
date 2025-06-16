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

#include "phy_fapi_adaptor_factory_impl.h"
#include "phy_fapi_adaptor_impl.h"
#include "phy_fapi_sector_adaptor_impl.h"

using namespace srsran;
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_adaptor> phy_fapi_adaptor_factory_impl::create(const phy_fapi_adaptor_config&  config,
                                                                        phy_fapi_adaptor_dependencies&& dependencies)
{
  std::vector<std::unique_ptr<phy_fapi_sector_adaptor>> sectors;
  for (unsigned i = 0, e = config.sectors.size(); i != e; ++i) {
    const auto& sector_cfg          = config.sectors[i];
    auto&       sector_dependencies = dependencies.sectors[i];

    sectors.push_back(std::make_unique<phy_fapi_sector_adaptor_impl>(sector_cfg, std::move(sector_dependencies)));
  }

  return std::make_unique<phy_fapi_adaptor_impl>(std::move(sectors));
}

std::unique_ptr<phy_fapi_adaptor_factory> srsran::fapi_adaptor::create_phy_fapi_adaptor_factory()
{
  return std::make_unique<phy_fapi_adaptor_factory_impl>();
}

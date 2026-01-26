/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "phy_fapi_fastpath_adaptor_factory_impl.h"
#include "p7/phy_fapi_p7_sector_fastpath_adaptor_impl.h"
#include "phy_fapi_fastpath_adaptor_impl.h"
#include "phy_fapi_sector_fastpath_adaptor_impl.h"

using namespace srsran;
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_fastpath_adaptor>
phy_fapi_fastpath_adaptor_factory_impl::create(const phy_fapi_fastpath_adaptor_config& config,
                                               phy_fapi_fastpath_adaptor_dependencies  dependencies)
{
  std::vector<std::unique_ptr<phy_fapi_sector_fastpath_adaptor>> sectors;
  for (unsigned i = 0, e = config.sectors.size(); i != e; ++i) {
    const auto& p7_sector_cfg          = config.sectors[i].p7_config;
    auto&       p7_sector_dependencies = dependencies.sectors[i].p7_dependencies;

    sectors.push_back(std::make_unique<phy_fapi_sector_fastpath_adaptor_impl>(
        std::make_unique<phy_fapi_p7_sector_fastpath_adaptor_impl>(p7_sector_cfg, std::move(p7_sector_dependencies))));
  }

  return std::make_unique<phy_fapi_fastpath_adaptor_impl>(std::move(sectors));
}

std::unique_ptr<phy_fapi_fastpath_adaptor_factory> srsran::fapi_adaptor::create_phy_fapi_fastpath_adaptor_factory()
{
  return std::make_unique<phy_fapi_fastpath_adaptor_factory_impl>();
}

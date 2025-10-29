/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

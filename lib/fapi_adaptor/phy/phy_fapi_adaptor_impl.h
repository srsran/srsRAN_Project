/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {
namespace fapi_adaptor {

class phy_fapi_adaptor_impl : public phy_fapi_adaptor
{
public:
  explicit phy_fapi_adaptor_impl(std::vector<std::unique_ptr<phy_fapi_sector_adaptor>> sector_adaptors_);

  phy_fapi_sector_adaptor& get_sector_adaptor(unsigned cell_id) override;

private:
  std::vector<std::unique_ptr<phy_fapi_sector_adaptor>> sector_adaptors;
};

} // namespace fapi_adaptor
} // namespace srsran

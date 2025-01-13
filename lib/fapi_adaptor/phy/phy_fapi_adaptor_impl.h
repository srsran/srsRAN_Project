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

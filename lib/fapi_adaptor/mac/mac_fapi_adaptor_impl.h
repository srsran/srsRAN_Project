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

#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_sector_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {
namespace fapi_adaptor {

/// MAC/FAPI adaptor implementation.
class mac_fapi_adaptor_impl : public mac_fapi_adaptor
{
public:
  explicit mac_fapi_adaptor_impl(std::vector<std::unique_ptr<mac_fapi_sector_adaptor>> sector_adaptors_);

  // See interface for documentation.
  mac_fapi_sector_adaptor& get_sector_adaptor(unsigned cell_id) override;

private:
  std::vector<std::unique_ptr<mac_fapi_sector_adaptor>> sector_adaptors;
};

} // namespace fapi_adaptor
} // namespace srsran

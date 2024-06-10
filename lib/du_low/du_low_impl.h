/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/du_low/du_low.h"
#include "srsran/phy/upper/upper_phy.h"

namespace srsran {

/// DU low implementation.
class du_low_impl final : public du_low
{
public:
  explicit du_low_impl(std::vector<std::unique_ptr<upper_phy>> upper_);

  // See interface for documentation.
  upper_phy& get_upper_phy(unsigned cell_id) override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  span<upper_phy*> get_all_upper_phys() override;

private:
  std::vector<std::unique_ptr<upper_phy>> upper;
  std::vector<upper_phy*>                 upper_ptrs;
};

} // namespace srsran

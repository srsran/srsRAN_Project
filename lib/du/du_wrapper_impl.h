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

#include "srsran/du/du_high_wrapper.h"
#include "srsran/du/du_wrapper.h"
#include "srsran/du_low/du_low_wrapper.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>

namespace srsran {

/// DU wrapper implementation dependencies.
struct du_wrapper_impl_dependencies {
  std::unique_ptr<du_low_wrapper>  du_lo;
  std::unique_ptr<du_high_wrapper> du_hi;
};

/// DU wrapper implementation.
class du_wrapper_impl final : public du_wrapper
{
public:
  explicit du_wrapper_impl(du_wrapper_impl_dependencies&& du_cfg);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  du_high_wrapper& get_du_high_wrapper() override;

  // See interface for documentation.
  du_low_wrapper& get_du_low_wrapper() override;

private:
  std::unique_ptr<du_low_wrapper>  du_lo;
  std::unique_ptr<du_high_wrapper> du_hi;
};

} // namespace srsran

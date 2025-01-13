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

#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_low/o_du_low.h"
#include "srsran/du/du_power_controller.h"
#include "srsran/du/o_du.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// O-RAN DU implementation dependencies.
struct o_du_impl_dependencies {
  std::unique_ptr<o_du_low>  du_lo;
  std::unique_ptr<o_du_high> du_hi;
};

/// O-RAN DU implementation.
class o_du_impl final : public o_du, public du_power_controller
{
public:
  explicit o_du_impl(o_du_impl_dependencies&& du_cfg);

  // See interface for documentation.
  du_power_controller& get_power_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  o_du_high& get_o_du_high() override;

  // See interface for documentation.
  o_du_low& get_o_du_low() override;

private:
  std::unique_ptr<o_du_high> du_hi;
  std::unique_ptr<o_du_low>  du_lo;
};

} // namespace srs_du
} // namespace srsran

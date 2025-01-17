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

#include "srsran/du/du.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi_adaptor/fapi_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {

class radio_unit;

/// Split 6 O-RAN DU implementation.
class split6_o_du_impl : public srs_du::du, public du_operation_controller
{
public:
  explicit split6_o_du_impl(std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors_,
                            std::unique_ptr<srs_du::o_du_high>               odu_hi_);

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<srs_du::o_du_high>               odu_hi;
  std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors;
};

} // namespace srsran

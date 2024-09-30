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

#include "srsran/du/du.h"
#include "srsran/du/du_high_wrapper.h"
#include "srsran/du/du_power_controller.h"
#include "srsran/fapi_adaptor/fapi_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {

class radio_unit;

class split6_du_impl : public srs_du::du, public du_power_controller
{
public:
  explicit split6_du_impl(std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors_,
                          std::vector<std::unique_ptr<du_high_wrapper>>    du_list_);

  // See interface for documentation.
  du_power_controller& get_power_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::vector<std::unique_ptr<du_high_wrapper>>    du_list;
  std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors;
};

} // namespace srsran

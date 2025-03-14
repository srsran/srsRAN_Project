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

#include "du_low_metrics_collector_impl.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/phy/upper/upper_phy.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// DU low implementation.
class du_low_impl final : public du_low, public du_operation_controller
{
public:
  explicit du_low_impl(std::vector<std::unique_ptr<upper_phy>> upper_);

  // See interface for documentation.
  upper_phy& get_upper_phy(unsigned cell_id) override;

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  du_low_metrics_collector* get_metrics_collector() override;

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::vector<std::unique_ptr<upper_phy>> upper;
  std::vector<upper_phy*>                 upper_ptrs;
  du_low_metrics_collector_impl           metrics_collector;
};

} // namespace srs_du
} // namespace srsran

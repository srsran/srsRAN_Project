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

#include "metrics/o_du_low_metrics_collector_impl.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/du/du_low/o_du_low.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// O-RAN DU low implementation dependencies.
struct o_du_low_impl_dependencies {
  std::unique_ptr<du_low>                         du_lo;
  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> fapi_adaptor;
};

class o_du_low_impl final : public o_du_low, public du_operation_controller
{
public:
  o_du_low_impl(bool enable_metrics, unsigned nof_cells);

  // See interface for documentation.
  du_low& get_du_low() override;

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  fapi_adaptor::phy_fapi_adaptor& get_phy_fapi_adaptor() override;

  // See interface for documentation.
  o_du_low_metrics_collector* get_metrics_collector() override;

  /// Returns the upper phy metrics notifier.
  upper_phy_metrics_notifiers* get_upper_phy_metrics_notifier();

  /// Sets the DU high and the PHY-FAPI adapter to the given ones.
  void set_o_du_low_depedencies(o_du_low_impl_dependencies&& dependencies);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  const unsigned                                  nof_cells;
  const bool                                      metrics_enabled;
  o_du_low_metrics_collector_impl                 metrics_collector;
  std::unique_ptr<du_low>                         du_lo;
  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> fapi_adaptor;
};

} // namespace srs_du
} // namespace srsran

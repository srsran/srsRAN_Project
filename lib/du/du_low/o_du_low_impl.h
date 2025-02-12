/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

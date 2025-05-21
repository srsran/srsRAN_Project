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

#include "o_du_high_metrics_notifier_proxy.h"
#include "srsran/du/du_high/du_high.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/du/o_du_config.h"
#include "srsran/e2/e2.h"

namespace srsran {

class mac_result_notifier;

namespace srs_du {

/// O-RAN DU high implementation dependencies.
struct o_du_high_impl_dependencies {
  srslog::basic_logger*                           logger;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor> du_high_adaptor;
  du_metrics_notifier*                            metrics_notifier;
};

/// O-RAN DU high implementation.
class o_du_high_impl : public o_du_high, public du_operation_controller
{
public:
  o_du_high_impl(unsigned nof_cells_, o_du_high_impl_dependencies&& du_dependencies);

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  fapi_adaptor::mac_fapi_adaptor& get_mac_fapi_adaptor() override;

  // See interface for documentation.
  du_high& get_du_high() override;

  // See interface for documentation.
  void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier) override;

  /// Sets the DU high to the given one.
  void set_du_high(std::unique_ptr<du_high> updated_du_high);

  /// Sets the E2 agent to the given one.
  void set_e2_agent(std::unique_ptr<e2_agent> agent);

  /// Returns the MAC result notifier of this O-RAN DU high.
  mac_result_notifier& get_mac_result_notifier() { return *du_high_result_notifier; }

  /// Returns the DU metrics notifier of this O-DU high implementation.
  du_metrics_notifier& get_du_metrics_notifier() { return metrics_notifier_poxy; }

private:
  const unsigned                                  nof_cells;
  srslog::basic_logger&                           logger;
  o_du_high_metrics_notifier_proxy                metrics_notifier_poxy;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor> du_high_adaptor;
  std::unique_ptr<mac_result_notifier>            du_high_result_notifier;
  std::unique_ptr<du_high>                        du_hi;
  std::unique_ptr<e2_agent>                       e2agent;
};

} // namespace srs_du
} // namespace srsran

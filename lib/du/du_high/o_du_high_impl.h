/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/du_high.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_power_controller.h"
#include "srsran/du/o_du_config.h"
#include "srsran/e2/e2.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"

namespace srsran {
namespace srs_du {

/// O-RAN DU high implementation dependencies.
struct o_du_high_impl_dependencies {
  srslog::basic_logger*                                        logger;
  std::vector<std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>> du_high_adaptor;
};

/// O-RAN DU high implementation.
class o_du_high_impl : public o_du_high, public du_power_controller
{
public:
  explicit o_du_high_impl(o_du_high_impl_dependencies&& du_dependencies);

  // See interface for documentation.
  du_power_controller& get_power_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  fapi::slot_data_message_notifier& get_slot_data_message_notifier(unsigned cell_id) override;

  // See interface for documentation.
  fapi::slot_error_message_notifier& get_slot_error_message_notifier(unsigned cell_id) override;

  // See interface for documentation.
  fapi::slot_time_message_notifier& get_slot_time_message_notifier(unsigned cell_id) override;

  // See interface for documentation.
  du_high& get_du_high() override;

  /// Sets the DU high to the given one.
  void set_du_high(std::unique_ptr<du_high> updated_du_high);

  /// Sets the E2 agent to the given one.
  void set_e2_agent(std::unique_ptr<e2_agent> agent);

  /// Returns the MAC result notifier of this O-RAN DU high.
  mac_result_notifier& get_mac_result_notifier() { return *du_high_result_notifier; }

private:
  srslog::basic_logger&                                        logger;
  std::vector<std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>> du_high_adaptor;
  std::unique_ptr<mac_result_notifier>                         du_high_result_notifier;
  std::unique_ptr<du_high>                                     du_hi;
  std::unique_ptr<e2_agent>                                    e2agent;
};

} // namespace srs_du
} // namespace srsran

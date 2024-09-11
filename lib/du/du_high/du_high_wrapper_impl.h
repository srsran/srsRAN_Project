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
#include "srsran/du/du_high/du_high_wrapper.h"
#include "srsran/du/du_wrapper_config.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"

namespace srsran {
namespace srs_du {

/// DU high wrapper implementation dependencies.
struct du_high_wrapper_impl_dependencies {
  srslog::basic_logger*                                        logger;
  std::vector<std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>> du_high_adaptor;
};

/// DU high wrapper implementation.
class du_high_wrapper_impl : public du_high_wrapper
{
public:
  explicit du_high_wrapper_impl(du_high_wrapper_impl_dependencies&& du_dependencies);

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

  /// Returns the MAC result notifier of this DU high wrapper.
  mac_result_notifier& get_mac_result_notifier() { return *du_high_result_notifier; }

private:
  srslog::basic_logger&                                        logger;
  std::vector<std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>> du_high_adaptor;
  std::unique_ptr<mac_result_notifier>                         du_high_result_notifier;
  std::unique_ptr<du_high>                                     du_hi;
};

} // namespace srs_du
} // namespace srsran
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
#include "srsran/du/du_wrapper_config.h"
#include "srsran/du_high/du_high.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"

namespace srsran {

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

} // namespace srsran

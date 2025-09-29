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

#include "split6_plugin_dummy.h"
#include "srsran/du/du_high/du_high_configuration.h"
#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi_adaptor/fapi_operation_controller.h"

namespace srsran::fapi {
struct dl_tti_request_message;
struct ul_tti_request_message;
struct ul_dci_request_message;
struct tx_data_request_message;
} // namespace srsran::fapi
using namespace srsran;

namespace {

/// Dummy FAPI adaptor implementation.
class fapi_adaptor_dummy : public fapi::fapi_adaptor,
                           fapi::fapi_message_interface_collection,
                           fapi::operation_controller
{
  /// Dummy slot message gateway implementation.
  class slot_message_gateway_dummy : public fapi::slot_message_gateway
  {
  public:
    // See interface for documentation.
    void dl_tti_request(const fapi::dl_tti_request_message& msg) override {}
    // See interface for documentation.
    void ul_tti_request(const fapi::ul_tti_request_message& msg) override {}
    // See interface for documentation.
    void ul_dci_request(const fapi::ul_dci_request_message& msg) override {}
    // See interface for documentation.
    void tx_data_request(const fapi::tx_data_request_message& msg) override {}
  };

  /// Dummy slot last message notifier implementation.
  class slot_last_message_notifier_dummy : public fapi::slot_last_message_notifier
  {
  public:
    // See interface for documentation.
    void on_last_message(slot_point slot) override {}
  };

  std::unique_ptr<slot_message_gateway_dummy>       gateway  = std::make_unique<slot_message_gateway_dummy>();
  std::unique_ptr<slot_last_message_notifier_dummy> notifier = std::make_unique<slot_last_message_notifier_dummy>();

public:
  // See interface for documentation.
  fapi::operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  fapi_message_interface_collection& get_message_interface_collection() override { return *this; }

  // See interface for documentation.
  bool start() override { return false; }

  // See interface for documentation.
  bool stop() override { return false; }

  // See interface for documentation.
  fapi::slot_message_gateway& get_slot_message_gateway() override { return *gateway; }

  // See interface for documentation.
  fapi::slot_last_message_notifier& get_slot_last_message_notifier() override { return *notifier; }

  // See interface for documentation.
  void set_slot_time_message_notifier(fapi::slot_time_message_notifier& notifier_) override {}

  // See interface for documentation.
  void set_slot_data_message_notifier(fapi::slot_data_message_notifier& notifier_) override {}

  // See interface for documentation.
  void set_error_message_notifier(fapi::error_message_notifier& notifier_) override {}
};

} // namespace

std::vector<std::unique_ptr<fapi::fapi_adaptor>>
split6_plugin_dummy::create_fapi_adaptor(const srs_du::du_high_configuration& du_high_cfg,
                                         const o_du_unit_dependencies&        dependencies)
{
  std::vector<std::unique_ptr<fapi::fapi_adaptor>> output(du_high_cfg.ran.cells.size());
  for (auto& cell : output) {
    cell = std::make_unique<fapi_adaptor_dummy>();
  }

  return output;
}

#ifndef SRSRAN_HAS_ENTERPRISE
std::unique_ptr<split6_plugin> srsran::create_split6_plugin(std::string_view app_name)
{
  return std::make_unique<split6_plugin_dummy>();
}
#endif

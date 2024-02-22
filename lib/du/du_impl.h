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

#include "du_low_impl.h"
#include "srsran/du/du.h"
#include "srsran/du/du_config.h"
#include "srsran/du_high/du_high.h"
#include "srsran/fapi/buffered_decorator_factories.h"
#include "srsran/fapi/slot_data_message_notifier.h"
#include "srsran/fapi/slot_error_message_notifier.h"
#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"

namespace srsran {

namespace fapi_adaptor {

class precoding_matrix_mapper;
class uci_part2_correspondence_mapper;

} // namespace fapi_adaptor

/// Helper adaptor that allows to create the objects first and later resolve the dependencies.
class du_impl_slot_time_message_notifier_adaptor : public fapi::slot_time_message_notifier
{
  std::reference_wrapper<fapi::slot_time_message_notifier> notifier;

public:
  du_impl_slot_time_message_notifier_adaptor();

  void on_slot_indication(const fapi::slot_indication_message& msg) override;

  void set_notifier(fapi::slot_time_message_notifier& notif);
};

class du_impl final : public du
{
public:
  du_impl(const du_config& du_cfg);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  f1ap_message_handler& get_f1ap_message_handler() override { return du_hi->get_f1ap_message_handler(); }

  // See interface for documentation.
  upper_phy_rx_symbol_handler& get_rx_symbol_handler() override { return du_lo->get_rx_symbol_handler(); }

  // See interface for documentation.
  upper_phy_timing_handler& get_timing_handler() override { return du_lo->get_timing_handler(); }

  // See interface for documentation.
  upper_phy_error_handler& get_error_handler() override { return du_lo->get_error_handler(); }

private:
  void build_fapi_adaptors(const du_config&                                               du_cfg,
                           std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>         pm_mapper,
                           std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper> part2_mapper);

  void build_fapi_adaptors_with_loggers(const du_config&                                               du_cfg,
                                        std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>         pm_mapper,
                                        std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper> part2_mapper);

  void build_fapi_adaptors_with_message_buffering(
      const du_config&                                               du_cfg,
      std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>         pm_mapper,
      std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper> part2_mapper,
      task_executor&                                                 executor);

  void build_fapi_adaptors_with_message_buffering_and_loggers(
      const du_config&                                               du_cfg,
      std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>         pm_mapper,
      std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper> part2_mapper,
      task_executor&                                                 executor);

private:
  srslog::basic_logger& logger;

  // DU-low instantiation.
  std::unique_ptr<du_low_impl> du_lo;

  // FAPI adapters.
  std::unique_ptr<fapi::slot_last_message_notifier>  last_msg_notifier;
  std::unique_ptr<fapi::slot_message_gateway>        logging_slot_gateway;
  std::unique_ptr<fapi::slot_time_message_notifier>  logging_slot_time_notifier;
  std::unique_ptr<fapi::slot_error_message_notifier> logging_slot_error_notifier;
  std::unique_ptr<fapi::slot_data_message_notifier>  logging_slot_data_notifier;
  du_impl_slot_time_message_notifier_adaptor         time_notifier_adaptor;
  std::unique_ptr<fapi::buffered_decorator_modules>  buffered_modules;
  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>    du_low_adaptor;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>    du_high_adaptor;
  std::unique_ptr<mac_result_notifier>               du_high_result_notifier;

  // DU-high instantiation.
  std::unique_ptr<du_high> du_hi;
};

} // namespace srsran

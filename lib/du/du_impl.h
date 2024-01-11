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

#include "du_low_impl.h"
#include "srsran/du/du.h"
#include "srsran/du/du_config.h"
#include "srsran/du_high/du_high.h"
#include "srsran/fapi/slot_data_message_notifier.h"
#include "srsran/fapi/slot_error_message_notifier.h"
#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"

namespace srsran {

class du_impl final : public du
{
public:
  du_impl(const du_config& du_cfg);

  void start() override;

  void stop() override;

  f1ap_message_handler& get_f1ap_message_handler() override { return du_hi->get_f1ap_message_handler(); }

  upper_phy_rx_symbol_handler& get_rx_symbol_handler() override { return du_lo->get_rx_symbol_handler(); }

  upper_phy_timing_handler& get_timing_handler() override { return du_lo->get_timing_handler(); }

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
  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>    du_low_adaptor;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>    du_high_adaptor;
  std::unique_ptr<mac_result_notifier>               du_high_result_notifier;

  // DU-high instantiation.
  std::unique_ptr<du_high> du_hi;
};

} // namespace srsran

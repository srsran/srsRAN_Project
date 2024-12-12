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

#include "fapi_to_mac_data_msg_translator.h"
#include "fapi_to_mac_error_msg_translator.h"
#include "fapi_to_mac_time_msg_translator.h"
#include "mac_to_fapi_translator.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor_config.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief MAC&ndash;FAPI bidirectional adaptor implementation.
class mac_fapi_adaptor_impl : public mac_fapi_adaptor
{
public:
  /// Constructor for the MAC&ndash;FAPI bidirectional adaptor.
  mac_fapi_adaptor_impl(const mac_fapi_adaptor_config& config, mac_fapi_adaptor_dependencies&& dependencies);

  // See interface for documentation.
  fapi::slot_time_message_notifier& get_slot_time_message_notifier() override;

  // See interface for documentation.
  fapi::slot_error_message_notifier& get_slot_error_message_notifier() override;

  // See interface for documentation.
  fapi::slot_data_message_notifier& get_slot_data_message_notifier() override;

  // See interface for documentation.
  mac_cell_result_notifier& get_cell_result_notifier() override;

  // See interface for documentation.
  void set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override;

  // See interface for documentation.
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) override;

  // See interface for documentation.
  void set_cell_pdu_handler(mac_pdu_handler& handler) override;

  // See interface for documentation.
  void set_cell_crc_handler(mac_cell_control_information_handler& handler) override;

private:
  /// MAC-to-FAPI data translator.
  mac_to_fapi_translator mac_translator;
  /// FAPI-to-MAC data-specific message translator.
  fapi_to_mac_data_msg_translator fapi_data_translator;
  /// FAPI-to-MAC time-specific message translator.
  fapi_to_mac_time_msg_translator fapi_time_translator;
  /// FAPI-to-MAC error-specific message translator.
  fapi_to_mac_error_msg_translator fapi_error_translator;
};

} // namespace fapi_adaptor
} // namespace srsran

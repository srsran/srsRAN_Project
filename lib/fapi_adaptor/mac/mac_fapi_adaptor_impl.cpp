/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_fapi_adaptor_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace fapi_adaptor;

mac_fapi_adaptor_impl::mac_fapi_adaptor_impl(fapi::slot_message_gateway&                      msg_gw,
                                             fapi::slot_last_message_notifier&                last_msg_notifier,
                                             std::unique_ptr<precoding_matrix_mapper>         pm_mapper,
                                             std::unique_ptr<uci_part2_correspondence_mapper> part2_mapper,
                                             unsigned                                         cell_nof_prbs,
                                             subcarrier_spacing                               scs) :
  mac_translator(srslog::fetch_basic_logger("FAPI"),
                 msg_gw,
                 last_msg_notifier,
                 std::move(pm_mapper),
                 std::move(part2_mapper),
                 cell_nof_prbs),
  fapi_data_translator(scs),
  fapi_time_translator(scs),
  fapi_error_translator(scs)
{
}

fapi::slot_time_message_notifier& mac_fapi_adaptor_impl::get_slot_time_notifier()
{
  return fapi_time_translator;
}

fapi::slot_error_message_notifier& mac_fapi_adaptor_impl::get_slot_error_notifier()
{
  return fapi_error_translator;
}

fapi::slot_data_message_notifier& mac_fapi_adaptor_impl::get_slot_data_notifier()
{
  return fapi_data_translator;
}

mac_cell_result_notifier& mac_fapi_adaptor_impl::get_cell_result_notifier()
{
  return mac_translator;
}

void mac_fapi_adaptor_impl::set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler)
{
  fapi_time_translator.set_cell_slot_handler(mac_slot_handler);
  fapi_error_translator.set_cell_slot_handler(mac_slot_handler);
}

void mac_fapi_adaptor_impl::set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler)
{
  fapi_data_translator.set_cell_rach_handler(mac_rach_handler);
}

void mac_fapi_adaptor_impl::set_cell_pdu_handler(mac_pdu_handler& handler)
{
  fapi_data_translator.set_cell_pdu_handler(handler);
}

void mac_fapi_adaptor_impl::set_cell_crc_handler(mac_cell_control_information_handler& handler)
{
  fapi_data_translator.set_cell_crc_handler(handler);
}

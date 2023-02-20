/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_fapi_adaptor_impl.h"

using namespace srsran;
using namespace fapi_adaptor;

mac_fapi_adaptor_impl::mac_fapi_adaptor_impl(fapi::slot_message_gateway&       msg_gw,
                                             fapi::slot_last_message_notifier& last_msg_notifier,
                                             unsigned                          sector_id_,
                                             subcarrier_spacing                scs) :
  sector_id(sector_id_), mac_translator(msg_gw, last_msg_notifier), fapi_data_translator(scs), fapi_time_translator(scs)
{
  // :TODO: remove this when sector id is used (in logging)
  (void)(sector_id);
}

fapi::slot_data_message_notifier& mac_fapi_adaptor_impl::get_slot_data_notifier()
{
  return fapi_data_translator;
}

fapi::slot_time_message_notifier& mac_fapi_adaptor_impl::get_slot_time_notifier()
{
  return fapi_time_translator;
}

mac_cell_result_notifier& mac_fapi_adaptor_impl::get_cell_result_notifier()
{
  return mac_translator;
}

void mac_fapi_adaptor_impl::set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler)
{
  fapi_time_translator.set_cell_slot_handler(mac_slot_handler);
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

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

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

mac_fapi_adaptor_impl::mac_fapi_adaptor_impl(slot_message_gateway& gateway,
                                             unsigned              sector_id_,
                                             subcarrier_spacing    scs) :
  sector_id(sector_id_), mac_translator(gateway), slot_dispatcher(mac_translator), fapi_translator(scs, slot_dispatcher)
{
  // :TODO: remove this when sector id is used (in logging)
  (void)(sector_id);
}

slot_message_notifier& mac_fapi_adaptor_impl::get_slot_notifier()
{
  return fapi_translator;
}

void mac_fapi_adaptor_impl::set_mac_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler)
{
  slot_dispatcher.set_mac_cell_slot_handler(mac_slot_handler);
}

mac_cell_result_notifier& mac_fapi_adaptor_impl::get_cell_result_notifier()
{
  return mac_translator;
}

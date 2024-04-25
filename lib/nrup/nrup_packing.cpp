/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/nrup/nrup_packing.h"
#include "srsran/support/bit_encoding.h"

namespace srsran {

nrup_pdu_type nrup_packing::get_pdu_type(byte_buffer_view container) const
{
  srsran_assert(!container.empty(), "Cannot get PDU type of empty PDU");
  uint8_t field_value = container[0] >> 4;
  return uint_to_nrup_pdu_type(field_value);
}

bool nrup_packing::unpack(nru_dl_user_data& dl_user_data, byte_buffer_view container) const
{
  return false;
};

bool nrup_packing::pack(byte_buffer& out_buf, const nru_dl_user_data& dl_user_data) const
{
  return false;
};

bool nrup_packing::unpack(nru_dl_data_delivery_status& dl_data_delivery_status, byte_buffer_view container) const
{
  return false;
};

bool nrup_packing::pack(byte_buffer& out_buf, const nru_dl_data_delivery_status& dl_data_delivery_status) const
{
  return false;
};

} // namespace srsran

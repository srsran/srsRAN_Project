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

#include "ecpri_packet_builder_impl.h"
#include "../support/network_order_binary_serializer.h"
#include "srsran/ofh/ecpri/ecpri_constants.h"
#include "srsran/ofh/ecpri/ecpri_packet_properties.h"

using namespace srsran;
using namespace ecpri;

units::bytes packet_builder_impl::get_header_size(message_type msg_type) const
{
  switch (msg_type) {
    case message_type::rt_control_data:
      return ECPRI_COMMON_HEADER_SIZE + ECPRI_REALTIME_CONTROL_PACKET_FIELDS_SIZE;
    case message_type::iq_data:
      return ECPRI_COMMON_HEADER_SIZE + ECPRI_IQ_DATA_PACKET_FIELDS_SIZE;
  }

  return ECPRI_COMMON_HEADER_SIZE;
}

/// Returns the first byte in the eCPRI Common Header.
static uint8_t get_first_byte_in_common_header(bool concatenate = false)
{
  srsran_assert(!concatenate, "eCPRI concatenation is currently not supported");

  uint8_t header = 0;
  // Protocol revision (4 highest bits).
  header |= uint8_t(ECPRI_PROTOCOL_REVISION) << 4;
  // Reserved 3 bits.
  // Concatenation field (last bit).
  header |= uint8_t(concatenate ? 1U : 0U);

  return header;
}

void packet_builder_impl::build_control_packet(span<uint8_t> buffer, const realtime_control_parameters& msg_params)
{
  // The payload is already appended in the buffer, so payload size equals buffer size - eCPRI header size.
  units::bytes payload_size(buffer.size());
  payload_size -= ECPRI_COMMON_HEADER_SIZE;
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Write eCPRI version and concatenation fields (1 Byte).
  serializer.write(get_first_byte_in_common_header());

  // Write eCPRI message type (1 Byte).
  serializer.write(static_cast<uint8_t>(message_type::rt_control_data));

  // Write the payload size (2 Bytes).
  serializer.write(static_cast<uint16_t>(payload_size.value()));

  // Write RTC_ID (2 Bytes).
  serializer.write(msg_params.rtc_id);

  // Write SEQ_ID (2 Byte).
  serializer.write(msg_params.seq_id);
}

void packet_builder_impl::build_data_packet(span<uint8_t> buffer, const iq_data_parameters& msg_params)
{
  // The payload is already appended in the buffer, so payload size equals buffer size - eCPRI header size.
  units::bytes payload_size(buffer.size());
  payload_size -= ECPRI_COMMON_HEADER_SIZE;
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Write eCPRI version and concatenation fields (1 Byte).
  serializer.write(get_first_byte_in_common_header());

  // Write eCPRI message type (1 Byte).
  serializer.write(static_cast<uint8_t>(message_type::iq_data));

  // Write the payload size (2 Bytes).
  serializer.write(static_cast<uint16_t>(payload_size.value()));

  // Write PC_ID (2 Bytes).
  serializer.write(msg_params.pc_id);

  // Write SEQ_ID (2 Bytes).
  serializer.write(msg_params.seq_id);
}

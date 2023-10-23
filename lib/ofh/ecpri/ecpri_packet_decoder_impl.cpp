/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ecpri_packet_decoder_impl.h"
#include "../support/network_order_binary_deserializer.h"
#include "ecpri_constants.h"
#include "srsran/ofh/ecpri/ecpri_packet_properties.h"

using namespace srsran;
using namespace ecpri;

/// Extracts an eCPRI common header from the deserializer.
static void deserialize_header(ofh::network_order_binary_deserializer& deserializer, common_header& header)
{
  uint8_t value         = deserializer.read<uint8_t>();
  header.revision       = value >> 4;
  header.is_last_packet = (value & 1U) == 0;

  header.msg_type     = static_cast<message_type>(deserializer.read<uint8_t>());
  header.payload_size = units::bytes(deserializer.read<uint16_t>());
}

/// Checks if the given eCPRI common header is valid.
static bool is_header_valid(const common_header& header, srslog::basic_logger& logger)
{
  if (header.revision != ECPRI_PROTOCOL_REVISION) {
    logger.debug("Dropping incoming eCPRI packet as the detected eCPRI protocol revision '{}' is not supported",
                 header.revision);

    return false;
  }

  if (!header.is_last_packet) {
    logger.debug("Dropping incoming eCPRI packet as the current implementation does not support concatenation");

    return false;
  }
  return true;
}

/// Extracts and returns the eCPRI IQ data parameters from the given deserializer.
static iq_data_parameters deserialize_iq_data_parameters(ofh::network_order_binary_deserializer& deserializer)
{
  iq_data_parameters params;
  params.pc_id  = deserializer.read<uint16_t>();
  params.seq_id = deserializer.read<uint16_t>();

  return params;
}

/// Extracts and returns the eCPRI realtime control parameters from the given deserializer.
static realtime_control_parameters
deserialize_rt_control_parameters(ofh::network_order_binary_deserializer& deserializer)
{
  realtime_control_parameters params;
  params.rtc_id = deserializer.read<uint16_t>();
  params.seq_id = deserializer.read<uint16_t>();

  return params;
}

span<const uint8_t> packet_decoder_impl::decode(span<const uint8_t> packet, packet_parameters& params)
{
  span<const uint8_t> payload = decode_header(packet, params);

  if (payload.empty()) {
    return {};
  }

  return decode_payload(payload, params);
}

span<const uint8_t> packet_decoder_impl::decode_header(span<const uint8_t>               packet,
                                                       srsran::ecpri::packet_parameters& params)
{
  // Sanity size check.
  if (units::bytes(packet.size()) < ECPRI_COMMON_HEADER_SIZE) {
    logger.debug("Dropping incoming eCPRI packet as its size is {} bytes which is smaller than the eCPRI common header "
                 "size ({})",
                 packet.size(),
                 ECPRI_COMMON_HEADER_SIZE);

    return {};
  }

  ofh::network_order_binary_deserializer deserializer(packet);

  deserialize_header(deserializer, params.header);
  if (!is_header_valid(params.header, logger)) {
    return {};
  }

  return packet.subspan(deserializer.get_offset(), deserializer.remaining_bytes());
}

span<const uint8_t> packet_decoder_use_header_payload_size::decode_payload(span<const uint8_t> packet,
                                                                           packet_parameters&  params)
{
  if (params.header.payload_size > units::bytes(packet.size())) {
    logger.debug("Dropping incoming eCPRI packet as its size is {} bytes while the payload size field in the header is "
                 "set to {} bytes",
                 packet.size(),
                 params.header.payload_size);

    return {};
  }

  ofh::network_order_binary_deserializer deserializer(packet);

  switch (params.header.msg_type) {
    case message_type::iq_data:
      params.type_params = deserialize_iq_data_parameters(deserializer);
      return packet.subspan(deserializer.get_offset(),
                            (params.header.payload_size - ECPRI_IQ_DATA_PACKET_FIELDS_SIZE).value());
    case message_type::rt_control_data:
      params.type_params = deserialize_rt_control_parameters(deserializer);
      return packet.subspan(deserializer.get_offset(),
                            (params.header.payload_size - ECPRI_REALTIME_CONTROL_PACKET_FIELDS_SIZE).value());
    default:
      logger.debug("Dropping incoming eCPRI packet as type value '{}' is not supported",
                   static_cast<unsigned>(params.header.msg_type));
      break;
  }

  return {};
}

span<const uint8_t> packet_decoder_ignore_header_payload_size::decode_payload(span<const uint8_t> packet,
                                                                              packet_parameters&  params)
{
  ofh::network_order_binary_deserializer deserializer(packet);

  switch (params.header.msg_type) {
    case message_type::iq_data:
      params.type_params = deserialize_iq_data_parameters(deserializer);
      return packet.subspan(deserializer.get_offset(), deserializer.remaining_bytes());
    case message_type::rt_control_data:
      params.type_params = deserialize_rt_control_parameters(deserializer);
      return packet.subspan(deserializer.get_offset(), deserializer.remaining_bytes());
    default:
      logger.debug("Dropping incoming eCPRI packet as type value '{}' is not supported",
                   static_cast<unsigned>(params.header.msg_type));
      break;
  }

  return {};
}

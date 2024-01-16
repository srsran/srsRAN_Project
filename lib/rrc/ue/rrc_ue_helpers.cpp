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

#include "rrc_ue_helpers.h"
#include "srsran/asn1/rrc_nr/msg_common.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"

using namespace srsran;
using namespace srs_cu_cp;

template <class T>
void srsran::srs_cu_cp::log_rrc_message(rrc_ue_logger&    logger,
                                        const direction_t dir,
                                        byte_buffer_view  pdu,
                                        const T&          msg,
                                        const char*       msg_type)
{
  if (logger.get_basic_logger().debug.enabled()) {
    asn1::json_writer js;
    msg.to_json(js);
    logger.log_debug(pdu.begin(),
                     pdu.end(),
                     "{} {} {} ({} B)",
                     (dir == Rx) ? "Rx" : "Tx",
                     msg_type,
                     msg.msg.c1().type().to_string(),
                     pdu.length());
    logger.log_debug("Containerized {}: {}", msg.msg.c1().type().to_string(), js.to_string());
  } else if (logger.get_basic_logger().info.enabled()) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    logger.log_info(pdu.begin(), pdu.end(), "{} {}", msg_type, msg.msg.c1().type().to_string());
  }
}

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::ul_ccch_msg_s>(rrc_ue_logger&                     logger,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::ul_ccch_msg_s& msg,
                                                                              const char* msg_type);

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::ul_dcch_msg_s>(rrc_ue_logger&                     logger,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::ul_dcch_msg_s& msg,
                                                                              const char* msg_type);

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::dl_ccch_msg_s>(rrc_ue_logger&                     logger,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::dl_ccch_msg_s& msg,
                                                                              const char* msg_type);

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::dl_dcch_msg_s>(rrc_ue_logger&                     logger,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::dl_dcch_msg_s& msg,
                                                                              const char* msg_type);

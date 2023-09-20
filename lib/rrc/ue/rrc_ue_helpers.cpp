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

#include "rrc_ue_helpers.h"
#include "srsran/asn1/rrc_nr/msg_common.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"

using namespace srsran;
using namespace srs_cu_cp;

template <class T>
void srsran::srs_cu_cp::log_rrc_message(const char*       source,
                                        const direction_t dir,
                                        byte_buffer_view  pdu,
                                        const T&          msg,
                                        const char*       msg_type)
{
  if (srslog::fetch_basic_logger("RRC").debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    // TODO: remove serialization
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    srslog::fetch_basic_logger("RRC").debug(
        bytes.data(), bytes.size(), "{} - {} {} ({} B)", source, (dir == Rx) ? "Rx" : "Tx", msg_type, pdu.length());
    srslog::fetch_basic_logger("RRC").debug("Content: {}", json_writer.to_string().c_str());
  } else if (srslog::fetch_basic_logger("RRC").info.enabled()) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    srslog::fetch_basic_logger("RRC").info(bytes.data(), bytes.size(), "{} {}", source, msg_type);
  }
}

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::ul_ccch_msg_s>(const char*                        source,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::ul_ccch_msg_s& msg,
                                                                              const char* msg_type);

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::ul_dcch_msg_s>(const char*                        source,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::ul_dcch_msg_s& msg,
                                                                              const char* msg_type);

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::dl_ccch_msg_s>(const char*                        source,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::dl_ccch_msg_s& msg,
                                                                              const char* msg_type);

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::dl_dcch_msg_s>(const char*                        source,
                                                                              const direction_t                  dir,
                                                                              byte_buffer_view                   pdu,
                                                                              const asn1::rrc_nr::dl_dcch_msg_s& msg,
                                                                              const char* msg_type);
template void
srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::cell_group_cfg_s>(const char*                           source,
                                                                   const direction_t                     dir,
                                                                   byte_buffer_view                      pdu,
                                                                   const asn1::rrc_nr::cell_group_cfg_s& msg,
                                                                   const char*                           msg_type);

template void
srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::radio_bearer_cfg_s>(const char*                             source,
                                                                     const direction_t                       dir,
                                                                     byte_buffer_view                        pdu,
                                                                     const asn1::rrc_nr::radio_bearer_cfg_s& msg,
                                                                     const char*                             msg_type);

template void srsran::srs_cu_cp::log_rrc_message<asn1::rrc_nr::ho_cmd_s>(const char*                   source,
                                                                         const direction_t             dir,
                                                                         byte_buffer_view              pdu,
                                                                         const asn1::rrc_nr::ho_cmd_s& msg,
                                                                         const char*                   msg_type);

void srsran::srs_cu_cp::log_rx_pdu_fail(ue_index_t       ue_index,
                                        const char*      source,
                                        byte_buffer_view pdu,
                                        const char*      cause_str,
                                        bool             log_hex)
{
  if (log_hex) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    srslog::fetch_basic_logger("RRC").error(
        bytes.data(), bytes.size(), "ue={} Rx {} PDU - discarding cause={}", ue_index, source, cause_str);
  } else {
    srslog::fetch_basic_logger("RRC").error("ue={} Rx {} PDU - discarding cause={}", ue_index, source, cause_str);
  }
}

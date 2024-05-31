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

#include "log_helpers.h"
#include "../common/e1ap_asn1_utils.h"
#include "srsran/support/format_utils.h"

using namespace srsran;

namespace fmt {

template <>
struct formatter<asn1::e1ap::e1ap_pdu_c> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const asn1::e1ap::e1ap_pdu_c& p, FormatContext& ctx)
  {
    asn1::json_writer js;
    p.to_json(js);
    return fmt::format_to(ctx.out(), "{}", js.to_string());
  }
};

} // namespace fmt

static void log_common_message(srslog::basic_logger& logger,
                               bool                  is_rx,
                               uint8_t               transaction_id,
                               const e1ap_message&   e1ap_msg,
                               bool                  json_enabled)
{
  const char* msg_name = get_message_type_str(e1ap_msg.pdu);
  const char* rx_str   = is_rx ? "Rx" : "Tx";

  if (json_enabled) {
    logger.info("{} PDU tid={}: {}\n{}", rx_str, transaction_id, msg_name, e1ap_msg.pdu);
  } else {
    logger.info("{} PDU tid={}: {}", rx_str, transaction_id, msg_name);
  }
}

template <typename UeIndex>
void srsran::log_e1ap_pdu(srslog::basic_logger&         logger,
                          bool                          is_rx,
                          const std::optional<UeIndex>& ue_id,
                          const e1ap_message&           e1ap_msg,
                          bool                          json_enabled)
{
  if (not logger.info.enabled()) {
    return;
  }

  // Determine if it is a UE-dedicated message or common message.
  std::optional<uint8_t> transaction_id = get_transaction_id(e1ap_msg.pdu);
  if (transaction_id.has_value()) {
    log_common_message(logger, is_rx, transaction_id.value(), e1ap_msg, json_enabled);
    return;
  }

  std::optional<gnb_cu_cp_ue_e1ap_id_t> cp_ue_id = get_gnb_cu_cp_ue_e1ap_id(e1ap_msg.pdu);
  std::optional<gnb_cu_up_ue_e1ap_id_t> up_ue_id = get_gnb_cu_up_ue_e1ap_id(e1ap_msg.pdu);
  const char*                           msg_name = get_message_type_str(e1ap_msg.pdu);

  // Create PDU formatter that runs in log backend.
  // Note: msg_name is a string literal and therefore it is ok to pass by pointer.
  auto pdu_description = make_formattable([is_rx, cp_ue_id, up_ue_id, ue_id, msg_name = msg_name](auto& ctx) {
    return fmt::format_to(ctx.out(),
                          "{} PDU{}{}{}: {}",
                          is_rx ? "Rx" : "Tx",
                          add_prefix_if_set(" ue=", ue_id),
                          add_prefix_if_set(" cu_cp_ue=", cp_ue_id),
                          add_prefix_if_set(" cu_up_ue=", up_ue_id),
                          msg_name);
  });

  if (json_enabled) {
    logger.info("{}\n{}", pdu_description, e1ap_msg.pdu);
  } else {
    logger.info("{}", pdu_description);
  }
}

template void srsran::log_e1ap_pdu<srs_cu_cp::ue_index_t>(srslog::basic_logger&                       logger,
                                                          bool                                        is_rx,
                                                          const std::optional<srs_cu_cp::ue_index_t>& ue_id,
                                                          const e1ap_message&                         e1ap_msg,
                                                          bool                                        json_enabled);
template void srsran::log_e1ap_pdu<srs_cu_up::ue_index_t>(srslog::basic_logger&                       logger,
                                                          bool                                        is_rx,
                                                          const std::optional<srs_cu_up::ue_index_t>& ue_id,
                                                          const e1ap_message&                         e1ap_msg,
                                                          bool                                        json_enabled);

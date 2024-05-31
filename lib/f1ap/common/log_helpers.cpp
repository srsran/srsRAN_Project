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
#include "f1ap_asn1_utils.h"
#include "srsran/support/format_utils.h"

using namespace srsran;

namespace fmt {

template <>
struct formatter<asn1::f1ap::f1ap_pdu_c> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const asn1::f1ap::f1ap_pdu_c& p, FormatContext& ctx)
  {
    asn1::json_writer js;
    p.to_json(js);
    return fmt::format_to(ctx.out(), "{}", js.to_string());
  }
};

} // namespace fmt

template <typename UeIndex>
void srsran::log_f1ap_pdu(srslog::basic_logger&         logger,
                          bool                          is_rx,
                          gnb_du_id_t                   du_id,
                          const std::optional<UeIndex>& ue_id,
                          const f1ap_message&           msg,
                          bool                          json_enabled)
{
  if (not logger.info.enabled()) {
    return;
  }

  // Determine if it is a UE-dedicated message or common message.
  std::optional<uint8_t>             transaction_id = get_transaction_id(msg.pdu);
  std::optional<gnb_cu_ue_f1ap_id_t> cu_ue_id       = get_gnb_cu_ue_f1ap_id(msg.pdu);
  std::optional<gnb_du_ue_f1ap_id_t> du_ue_id       = get_gnb_du_ue_f1ap_id(msg.pdu);
  const char*                        msg_name       = get_message_type_str(msg.pdu);

  // Create PDU formatter that runs in log backend.
  // Note: msg_name is a string literal and therefore it is ok to pass by pointer.
  auto pdu_description =
      make_formattable([is_rx, du_id, cu_ue_id, du_ue_id, ue_id, transaction_id, msg_name = msg_name](auto& ctx) {
        return fmt::format_to(ctx.out(),
                              "{} PDU du={}{}{}{}{}: {}",
                              is_rx ? "Rx" : "Tx",
                              du_id,
                              add_prefix_if_set(" tid=", transaction_id),
                              add_prefix_if_set(" ue=", ue_id),
                              add_prefix_if_set(" cu_ue=", cu_ue_id),
                              add_prefix_if_set(" du_ue=", du_ue_id),
                              msg_name);
      });

  if (json_enabled) {
    logger.info("{}\n{}", pdu_description, msg.pdu);
  } else {
    logger.info("{}", pdu_description);
  }
}

template void srsran::log_f1ap_pdu<srs_cu_cp::ue_index_t>(srslog::basic_logger&                       logger,
                                                          bool                                        is_rx,
                                                          gnb_du_id_t                                 du_id,
                                                          const std::optional<srs_cu_cp::ue_index_t>& ue_id,
                                                          const f1ap_message&                         msg,
                                                          bool                                        json_enabled);
template void srsran::log_f1ap_pdu<du_ue_index_t>(srslog::basic_logger&               logger,
                                                  bool                                is_rx,
                                                  gnb_du_id_t                         du_id,
                                                  const std::optional<du_ue_index_t>& ue_id,
                                                  const f1ap_message&                 msg,
                                                  bool                                json_enabled);

/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "ngap_asn1_utils.h"
#include "srsran/support/format/custom_formattable.h"

using namespace srsran;

namespace fmt {

template <>
struct formatter<asn1::ngap::ngap_pdu_c> : public basic_parser {
  template <typename FormatContext>
  auto format(const asn1::ngap::ngap_pdu_c& p, FormatContext& ctx) const
  {
    asn1::json_writer js;
    p.to_json(js);
    return fmt::format_to(ctx.out(), "{}", js.to_string());
  }
};

} // namespace fmt

void srsran::srs_cu_cp::log_ngap_pdu(srslog::basic_logger&            logger,
                                     bool                             json_log,
                                     bool                             is_rx,
                                     const std::optional<ue_index_t>& ue_idx,
                                     const asn1::ngap::ngap_pdu_c&    pdu)
{
  if (not logger.info.enabled()) {
    return;
  }

  std::optional<ran_ue_id_t> ran_ue_id = asn1_utils::get_ran_ue_id(pdu);
  std::optional<amf_ue_id_t> amf_ue_id = asn1_utils::get_amf_ue_id(pdu);

  // Custom formattable object whose formatting function will run in the log backend.
  auto pdu_log_entry = make_formattable(
      [is_rx, ran_ue_id, amf_ue_id, ue_idx, msg_name = asn1_utils::get_message_type_str(pdu)](auto& ctx) {
        return fmt::format_to(ctx.out(),
                              "{} PDU{}{}{}: {}",
                              is_rx ? "Rx" : "Tx",
                              add_prefix_if_set(" ue=", ue_idx),
                              add_prefix_if_set(" ran_ue=", ran_ue_id),
                              add_prefix_if_set(" amf_ue=", amf_ue_id),
                              msg_name);
      });

  if (json_log) {
    logger.info("{}\n{}", pdu_log_entry, pdu);
  } else {
    logger.info("{}", pdu_log_entry);
  }
}

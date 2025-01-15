/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  std::optional<ran_ue_id_t> ran_ue_id = get_ran_ue_id(pdu);
  std::optional<amf_ue_id_t> amf_ue_id = get_amf_ue_id(pdu);

  // Custom formattable object whose formatting function will run in the log backend.
  auto pdu_log_entry =
      make_formattable([is_rx, ran_ue_id, amf_ue_id, ue_idx, msg_name = get_message_type_str(pdu)](auto& ctx) {
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

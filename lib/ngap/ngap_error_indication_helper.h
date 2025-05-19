/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ngap_asn1_converters.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/ngap/ngap_types.h"

namespace srsran {
namespace srs_cu_cp {

struct error_indication_request_t {
  ngap_cause_t               cause;
  std::optional<ran_ue_id_t> ran_ue_id;
  std::optional<amf_ue_id_t> amf_ue_id;
};

/// \brief Send an Error Indication message to the AMF.
/// \param[in] ngap_notifier The message notifier to send the message to the AMF.
/// \param[in] logger The logger to log the message.
/// \param[in] ran_ue_id The RAN UE ID (optional).
/// \param[in] amf_ue_id The AMF UE ID (optional).
/// \param[in] cause The cause of the Error Indication (optional).

inline void send_error_indication(ngap_message_notifier&      ngap_notifier,
                                  srslog::basic_logger&       logger,
                                  std::optional<ran_ue_id_t>  ran_ue_id = {},
                                  std::optional<amf_ue_id_t>  amf_ue_id = {},
                                  std::optional<ngap_cause_t> cause     = {})
{
  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_ERROR_IND);
  auto& error_ind = ngap_msg.pdu.init_msg().value.error_ind();

  // Set optionally provided RAN UE ID.
  if (ran_ue_id.has_value()) {
    error_ind->ran_ue_ngap_id_present = true;
    error_ind->ran_ue_ngap_id         = ran_ue_id_to_uint(ran_ue_id.value());
  }

  // Set optionally provided AMF UE ID.
  if (amf_ue_id.has_value()) {
    error_ind->amf_ue_ngap_id_present = true;
    error_ind->amf_ue_ngap_id         = amf_ue_id_to_uint(amf_ue_id.value());
  }

  if (cause.has_value()) {
    error_ind->cause_present = true;
    error_ind->cause         = cause_to_asn1(cause.value());
  }

  // TODO: Add missing values.

  // Forward message to AMF.
  if (!ngap_notifier.on_new_message(ngap_msg)) {
    logger.warning("AMF notifier is not set. Cannot send ErrorIndication");
    return;
  }
}

} // namespace srs_cu_cp
} // namespace srsran

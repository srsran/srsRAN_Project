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

#pragma once

#include "ngap_asn1_converters.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/ngap/ngap_types.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Send an Error Indication message to the AMF.
/// \param[in] ngap_notifier The message notifier to send the message to the AMF.
/// \param[in] logger The logger to log the message.
/// \param[in] ran_ue_id The RAN UE ID (optional).
/// \param[in] amf_ue_id The AMF UE ID (optional).
/// \param[in] cause The cause of the Error Indication (optional).

inline void send_error_indication(ngap_message_notifier& ngap_notifier,
                                  srslog::basic_logger&  logger,
                                  optional<ran_ue_id_t>  ran_ue_id = {},
                                  optional<amf_ue_id_t>  amf_ue_id = {},
                                  optional<ngap_cause_t> cause     = {})
{
  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_ERROR_IND);
  auto& error_ind = ngap_msg.pdu.init_msg().value.error_ind();

  // Set optionally provided RAN UE ID
  if (ran_ue_id.has_value()) {
    error_ind->ran_ue_ngap_id_present = true;
    error_ind->ran_ue_ngap_id         = ran_ue_id_to_uint(ran_ue_id.value());
  }

  // Set optionally provided AMF UE ID
  if (amf_ue_id.has_value()) {
    error_ind->amf_ue_ngap_id_present = true;
    error_ind->amf_ue_ngap_id         = amf_ue_id_to_uint(amf_ue_id.value());
  }

  if (cause.has_value()) {
    error_ind->cause_present = true;
    error_ind->cause         = cause_to_asn1(cause.value());
  }

  // TODO: Add missing values

  // Forward message to AMF
  logger.info("{}{}{}Sending ErrorIndication",
              error_ind->ran_ue_ngap_id_present ? fmt::format(" ran_ue_id={}", error_ind->ran_ue_ngap_id) : "",
              error_ind->amf_ue_ngap_id_present ? fmt::format(" amf_ue_id={}", error_ind->amf_ue_ngap_id) : "",
              error_ind->ran_ue_ngap_id_present || error_ind->amf_ue_ngap_id_present ? ": " : "");
  ngap_notifier.on_new_message(ngap_msg);
}

} // namespace srs_cu_cp
} // namespace srsran
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

#include "srsran/adt/optional.h"
#include "srsran/asn1/e1ap/common.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// Get string with E1AP error cause.
inline const char* get_cause_str(const asn1::e1ap::cause_c& cause)
{
  using namespace asn1::e1ap;
  switch (cause.type()) {
    case cause_c::types_opts::radio_network:
      return cause.radio_network().to_string();
    case cause_c::types_opts::transport:
      return cause.transport().to_string();
    case cause_c::types_opts::protocol:
      return cause.protocol().to_string();
    case cause_c::types_opts::misc:
      return cause.misc().to_string();
    default:
      break;
  }
  return "unknown";
}

/// Extracts string literal with the message type name.
inline const char* get_message_type_str(const asn1::e1ap::e1ap_pdu_c& pdu)
{
  switch (pdu.type().value) {
    case asn1::e1ap::e1ap_pdu_c::types_opts::init_msg:
      return pdu.init_msg().value.type().to_string();
    case asn1::e1ap::e1ap_pdu_c::types_opts::successful_outcome:
      return pdu.successful_outcome().value.type().to_string();
    case asn1::e1ap::e1ap_pdu_c::types_opts::unsuccessful_outcome:
      return pdu.unsuccessful_outcome().value.type().to_string();
    default:
      break;
  }
  report_fatal_error("Invalid E1AP PDU type \"{}\"", pdu.type().to_string());
}

/// Extracts transaction id of Initiating message.
inline std::optional<uint8_t> get_transaction_id(const asn1::e1ap::init_msg_s& out)
{
  using namespace asn1::e1ap;
  using init_type = e1ap_elem_procs_o::init_msg_c::types_opts;
  switch (out.value.type().value) {
    case init_type::reset:
      return out.value.reset()->transaction_id;
    case init_type::gnb_cu_up_e1_setup_request:
      return out.value.gnb_cu_up_e1_setup_request()->transaction_id;
    case init_type::gnb_cu_cp_e1_setup_request:
      return out.value.gnb_cu_cp_e1_setup_request()->transaction_id;
    case init_type::gnb_cu_up_cfg_upd:
      return out.value.gnb_cu_up_cfg_upd()->transaction_id;
    case init_type::gnb_cu_cp_cfg_upd:
      return out.value.gnb_cu_cp_cfg_upd()->transaction_id;
    case init_type::e1_release_request:
      return out.value.e1_release_request()->transaction_id;
    case init_type::res_status_request:
      return out.value.res_status_request()->transaction_id;
    case init_type::iab_up_tnl_address_upd:
      return out.value.iab_up_tnl_address_upd()->transaction_id;
    case init_type::error_ind:
      return out.value.error_ind()->transaction_id;
      // TODO: Remaining cases.
    default:
      break;
  }
  return std::nullopt;
}

/// Extracts transaction id of Successful Outcome message.
inline std::optional<uint8_t> get_transaction_id(const asn1::e1ap::successful_outcome_s& out)
{
  using namespace asn1::e1ap;
  using success_type = e1ap_elem_procs_o::successful_outcome_c::types_opts;
  switch (out.value.type().value) {
    case success_type::reset_ack:
      return out.value.reset_ack()->transaction_id;
    case success_type::gnb_cu_up_e1_setup_resp:
      return out.value.gnb_cu_up_e1_setup_resp()->transaction_id;
    case success_type::gnb_cu_cp_e1_setup_resp:
      return out.value.gnb_cu_cp_e1_setup_resp()->transaction_id;
    case success_type::gnb_cu_up_cfg_upd_ack:
      return out.value.gnb_cu_up_cfg_upd_ack()->transaction_id;
    case success_type::gnb_cu_cp_cfg_upd_ack:
      return out.value.gnb_cu_cp_cfg_upd_ack()->transaction_id;
    case success_type::res_status_resp:
      return out.value.res_status_resp()->transaction_id;
    case success_type::iab_up_tnl_address_upd_ack:
      return out.value.iab_up_tnl_address_upd_ack()->transaction_id;
    default:
      break;
  }
  return std::nullopt;
}

/// Extracts transaction id of Unsuccessful Outcome message.
inline std::optional<uint8_t> get_transaction_id(const asn1::e1ap::unsuccessful_outcome_s& out)
{
  using namespace asn1::e1ap;
  switch (out.value.type().value) {
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail:
      return out.value.gnb_cu_up_e1_setup_fail()->transaction_id;
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_e1_setup_fail:
      return out.value.gnb_cu_cp_e1_setup_fail()->transaction_id;
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_cfg_upd_fail:
      return out.value.gnb_cu_cp_cfg_upd_fail()->transaction_id;
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_cfg_upd_fail:
      return out.value.gnb_cu_up_cfg_upd_fail()->transaction_id;
      // TODO: Remaining cases.
    default:
      break;
  }
  return std::nullopt;
}

/// Extracts transaction id of E1AP PDU.
inline std::optional<uint8_t> get_transaction_id(const asn1::e1ap::e1ap_pdu_c& pdu)
{
  using namespace asn1::e1ap;
  switch (pdu.type().value) {
    case e1ap_pdu_c::types_opts::init_msg:
      return get_transaction_id(pdu.init_msg());
    case e1ap_pdu_c::types_opts::successful_outcome:
      return get_transaction_id(pdu.successful_outcome());
    case e1ap_pdu_c::types_opts::unsuccessful_outcome:
      return get_transaction_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return std::nullopt;
}

inline std::optional<gnb_cu_cp_ue_e1ap_id_t> get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::init_msg_s& init_msg)
{
  using init_msg_type = asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts;
  switch (init_msg.value.type()) {
    case init_msg_type::bearer_context_setup_request:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id;
    case init_msg_type::bearer_context_release_cmd:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_release_cmd()->gnb_cu_cp_ue_e1ap_id;
    case init_msg_type::bearer_context_mod_request:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id;
    case init_msg_type::bearer_context_mod_required:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_mod_required()->gnb_cu_cp_ue_e1ap_id;
    case init_msg_type::bearer_context_release_request:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_release_request()->gnb_cu_cp_ue_e1ap_id;
    default:
      break;
  }
  return std::nullopt;
}

inline std::optional<gnb_cu_cp_ue_e1ap_id_t>
get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::successful_outcome_s& success_outcome)
{
  using namespace asn1::e1ap;
  using success_msg_type = e1ap_elem_procs_o::successful_outcome_c::types_opts;
  switch (success_outcome.value.type().value) {
    case success_msg_type::bearer_context_setup_resp:
      return (gnb_cu_cp_ue_e1ap_id_t)success_outcome.value.bearer_context_setup_resp()->gnb_cu_cp_ue_e1ap_id;
    case success_msg_type::bearer_context_mod_resp:
      return (gnb_cu_cp_ue_e1ap_id_t)success_outcome.value.bearer_context_mod_resp()->gnb_cu_cp_ue_e1ap_id;
    case success_msg_type::bearer_context_mod_confirm:
      return (gnb_cu_cp_ue_e1ap_id_t)success_outcome.value.bearer_context_mod_confirm()->gnb_cu_cp_ue_e1ap_id;
    case success_msg_type::bearer_context_release_complete:
      return (gnb_cu_cp_ue_e1ap_id_t)success_outcome.value.bearer_context_release_complete()->gnb_cu_cp_ue_e1ap_id;
    default:
      break;
  }
  return std::nullopt;
}

inline std::optional<gnb_cu_cp_ue_e1ap_id_t>
get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  using namespace asn1::e1ap;
  using unsuccess_msg_type = e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts;
  switch (unsuccessful_outcome.value.type().value) {
    case unsuccess_msg_type::bearer_context_setup_fail:
      return (gnb_cu_cp_ue_e1ap_id_t)unsuccessful_outcome.value.bearer_context_setup_fail()->gnb_cu_cp_ue_e1ap_id;
    case unsuccess_msg_type::bearer_context_mod_fail:
      return (gnb_cu_cp_ue_e1ap_id_t)unsuccessful_outcome.value.bearer_context_mod_fail()->gnb_cu_cp_ue_e1ap_id;
    default:
      break;
  }
  return std::nullopt;
}

inline std::optional<gnb_cu_cp_ue_e1ap_id_t> get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::e1ap_pdu_c& pdu)
{
  using namespace asn1::e1ap;
  switch (pdu.type().value) {
    case e1ap_pdu_c::types_opts::init_msg:
      return get_gnb_cu_cp_ue_e1ap_id(pdu.init_msg());
    case e1ap_pdu_c::types_opts::successful_outcome:
      return get_gnb_cu_cp_ue_e1ap_id(pdu.successful_outcome());
    case e1ap_pdu_c::types_opts::unsuccessful_outcome:
      return get_gnb_cu_cp_ue_e1ap_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return std::nullopt;
}

//////////////////////////////////

inline std::optional<gnb_cu_up_ue_e1ap_id_t> get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::init_msg_s& init_msg)
{
  using init_msg_type = asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts;
  switch (init_msg.value.type()) {
    case init_msg_type::bearer_context_release_cmd:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_release_cmd()->gnb_cu_up_ue_e1ap_id;
    case init_msg_type::bearer_context_mod_request:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_mod_request()->gnb_cu_up_ue_e1ap_id;
    case init_msg_type::bearer_context_mod_required:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_mod_required()->gnb_cu_up_ue_e1ap_id;
    case init_msg_type::bearer_context_release_request:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_release_request()->gnb_cu_up_ue_e1ap_id;
    default:
      break;
  }
  return std::nullopt;
}

inline std::optional<gnb_cu_up_ue_e1ap_id_t>
get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::successful_outcome_s& success_outcome)
{
  using namespace asn1::e1ap;
  using success_msg_type = e1ap_elem_procs_o::successful_outcome_c::types_opts;
  switch (success_outcome.value.type().value) {
    case success_msg_type::bearer_context_setup_resp:
      return (gnb_cu_up_ue_e1ap_id_t)success_outcome.value.bearer_context_setup_resp()->gnb_cu_up_ue_e1ap_id;
    case success_msg_type::bearer_context_mod_resp:
      return (gnb_cu_up_ue_e1ap_id_t)success_outcome.value.bearer_context_mod_resp()->gnb_cu_up_ue_e1ap_id;
    case success_msg_type::bearer_context_mod_confirm:
      return (gnb_cu_up_ue_e1ap_id_t)success_outcome.value.bearer_context_mod_confirm()->gnb_cu_up_ue_e1ap_id;
    case success_msg_type::bearer_context_release_complete:
      return (gnb_cu_up_ue_e1ap_id_t)success_outcome.value.bearer_context_release_complete()->gnb_cu_up_ue_e1ap_id;
    default:
      break;
  }
  return std::nullopt;
}

inline std::optional<gnb_cu_up_ue_e1ap_id_t>
get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  using namespace asn1::e1ap;
  using unsuccess_msg_type = e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts;
  switch (unsuccessful_outcome.value.type().value) {
    case unsuccess_msg_type::bearer_context_setup_fail:
      return (gnb_cu_up_ue_e1ap_id_t)unsuccessful_outcome.value.bearer_context_setup_fail()->gnb_cu_up_ue_e1ap_id;
    case unsuccess_msg_type::bearer_context_mod_fail:
      return (gnb_cu_up_ue_e1ap_id_t)unsuccessful_outcome.value.bearer_context_mod_fail()->gnb_cu_up_ue_e1ap_id;
    default:
      break;
  }
  return std::nullopt;
}

inline std::optional<gnb_cu_up_ue_e1ap_id_t> get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::e1ap_pdu_c& pdu)
{
  using namespace asn1::e1ap;
  switch (pdu.type().value) {
    case e1ap_pdu_c::types_opts::init_msg:
      return get_gnb_cu_up_ue_e1ap_id(pdu.init_msg());
    case e1ap_pdu_c::types_opts::successful_outcome:
      return get_gnb_cu_up_ue_e1ap_id(pdu.successful_outcome());
    case e1ap_pdu_c::types_opts::unsuccessful_outcome:
      return get_gnb_cu_up_ue_e1ap_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return std::nullopt;
}

} // namespace srsran

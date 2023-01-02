/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/e1/common/e1_types.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

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

/// Extracts message type.
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
inline expected<uint8_t> get_transaction_id(const asn1::e1ap::init_msg_s& out)
{
  using namespace asn1::e1ap;
  switch (out.value.type().value) {
    case e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request:
      return out.value.gnb_cu_cp_e1_setup_request()->transaction_id->value;
    case e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_up_e1_setup_request:
      return out.value.gnb_cu_up_e1_setup_request()->transaction_id->value;
    case e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_cfg_upd:
      return out.value.gnb_cu_cp_cfg_upd()->transaction_id->value;
    case e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_up_cfg_upd:
      return out.value.gnb_cu_up_cfg_upd()->transaction_id->value;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Successful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::e1ap::successful_outcome_s& out)
{
  using namespace asn1::e1ap;
  switch (out.value.type().value) {
    case e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_cp_e1_setup_resp:
      return out.value.gnb_cu_cp_e1_setup_resp()->transaction_id->value;
    case e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp:
      return out.value.gnb_cu_up_e1_setup_resp()->transaction_id->value;
    case e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_cp_cfg_upd_ack:
      return out.value.gnb_cu_cp_cfg_upd_ack()->transaction_id->value;
    case e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_cfg_upd_ack:
      return out.value.gnb_cu_up_cfg_upd_ack()->transaction_id->value;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Unsuccessful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::e1ap::unsuccessful_outcome_s& out)
{
  using namespace asn1::e1ap;
  switch (out.value.type().value) {
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_e1_setup_fail:
      return out.value.gnb_cu_cp_e1_setup_fail()->transaction_id->value;
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail:
      return out.value.gnb_cu_up_e1_setup_fail()->transaction_id->value;
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_cfg_upd_fail:
      return out.value.gnb_cu_cp_cfg_upd_fail()->transaction_id->value;
    case e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_cfg_upd_fail:
      return out.value.gnb_cu_up_cfg_upd_fail()->transaction_id->value;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of E1AP PDU.
inline expected<uint8_t> get_transaction_id(const asn1::e1ap::e1ap_pdu_c& pdu)
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
  return {default_error_t{}};
}

inline expected<gnb_cu_cp_ue_e1ap_id_t> get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::init_msg_s& init_msg)
{
  using init_msg_type = asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts;
  switch (init_msg.value.type()) {
    case init_msg_type::bearer_context_setup_request:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_release_cmd:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_release_cmd()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_mod_request:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_mod_required:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_mod_required()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_release_request:
      return (gnb_cu_cp_ue_e1ap_id_t)init_msg.value.bearer_context_release_request()->gnb_cu_cp_ue_e1ap_id->value;
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<gnb_cu_cp_ue_e1ap_id_t>
get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::successful_outcome_s& success_outcome)
{
  return {default_error_t{}};
}

inline expected<gnb_cu_cp_ue_e1ap_id_t>
get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  return {default_error_t{}};
}

inline expected<gnb_cu_cp_ue_e1ap_id_t> get_gnb_cu_cp_ue_e1ap_id(const asn1::e1ap::e1ap_pdu_c& pdu)
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
  return {default_error_t{}};
}

//////////////////////////////////

inline expected<gnb_cu_up_ue_e1ap_id_t> get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::init_msg_s& init_msg)
{
  using init_msg_type = asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts;
  switch (init_msg.value.type()) {
    case init_msg_type::bearer_context_setup_request:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_release_cmd:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_release_cmd()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_mod_request:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_mod_required:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_mod_required()->gnb_cu_cp_ue_e1ap_id->value;
    case init_msg_type::bearer_context_release_request:
      return (gnb_cu_up_ue_e1ap_id_t)init_msg.value.bearer_context_release_request()->gnb_cu_cp_ue_e1ap_id->value;
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<gnb_cu_up_ue_e1ap_id_t>
get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::successful_outcome_s& success_outcome)
{
  return {default_error_t{}};
}

inline expected<gnb_cu_up_ue_e1ap_id_t>
get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  return {default_error_t{}};
}

inline expected<gnb_cu_up_ue_e1ap_id_t> get_gnb_cu_up_ue_e1ap_id(const asn1::e1ap::e1ap_pdu_c& pdu)
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
  return {default_error_t{}};
}

} // namespace srsgnb

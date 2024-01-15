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

#include "srsran/adt/expected.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/ran/paging_information.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// Get string with F1AP error cause.
inline const char* get_cause_str(const asn1::f1ap::cause_c& cause)
{
  using namespace asn1::f1ap;
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
inline const char* get_message_type_str(const asn1::f1ap::f1ap_pdu_c& pdu)
{
  switch (pdu.type().value) {
    case asn1::f1ap::f1ap_pdu_c::types_opts::init_msg:
      return pdu.init_msg().value.type().to_string();
    case asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome:
      return pdu.successful_outcome().value.type().to_string();
    case asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome:
      return pdu.unsuccessful_outcome().value.type().to_string();
    default:
      break;
  }
  report_fatal_error("Invalid F1AP PDU type \"{}\"", pdu.type().to_string());
}

/// Extracts transaction id of Initiating message.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::init_msg_s& out)
{
  using namespace asn1::f1ap;
  switch (out.value.type().value) {
    case f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request:
      return out.value.f1_setup_request()->transaction_id;
    case f1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cfg_upd:
      return out.value.gnb_cu_cfg_upd()->transaction_id;
    case f1ap_elem_procs_o::init_msg_c::types_opts::gnb_du_cfg_upd:
      return out.value.gnb_du_cfg_upd()->transaction_id;
    case f1ap_elem_procs_o::init_msg_c::types_opts::f1_removal_request:
      return (*out.value.f1_removal_request())[0]->transaction_id();
    case f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer:
      return (*out.value.init_ul_rrc_msg_transfer()).transaction_id;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Successful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::successful_outcome_s& out)
{
  using namespace asn1::f1ap;
  switch (out.value.type().value) {
    case f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp:
      return out.value.f1_setup_resp()->transaction_id;
    case f1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_cfg_upd_ack:
      return out.value.gnb_cu_cfg_upd_ack()->transaction_id;
    case f1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_du_cfg_upd_ack:
      return out.value.gnb_du_cfg_upd_ack()->transaction_id;
    case f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_removal_resp:
      return out.value.f1_removal_resp()->transaction_id;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Unsuccessful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::unsuccessful_outcome_s& out)
{
  using namespace asn1::f1ap;
  switch (out.value.type().value) {
    case f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail:
      return out.value.f1_setup_fail()->transaction_id;
    case f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cfg_upd_fail:
      return out.value.gnb_cu_cfg_upd_fail()->transaction_id;
    case f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_du_cfg_upd_fail:
      return out.value.gnb_du_cfg_upd_fail()->transaction_id;
    case f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_removal_fail:
      return out.value.f1_removal_fail()->transaction_id;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of F1AP PDU.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::f1ap_pdu_c& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu.type().value) {
    case f1ap_pdu_c::types_opts::init_msg:
      return get_transaction_id(pdu.init_msg());
    case f1ap_pdu_c::types_opts::successful_outcome:
      return get_transaction_id(pdu.successful_outcome());
    case f1ap_pdu_c::types_opts::unsuccessful_outcome:
      return get_transaction_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<gnb_du_ue_f1ap_id_t> get_gnb_du_ue_f1ap_id(const asn1::f1ap::init_msg_s& init_msg)
{
  using init_msg_type = asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts;
  switch (init_msg.value.type()) {
    case init_msg_type::ue_context_setup_request:
      if (init_msg.value.ue_context_setup_request()->gnb_du_ue_f1ap_id_present) {
        return (gnb_du_ue_f1ap_id_t)init_msg.value.ue_context_setup_request()->gnb_du_ue_f1ap_id;
      }
      return {default_error_t{}};
    case init_msg_type::ue_context_release_cmd:
      return (gnb_du_ue_f1ap_id_t)init_msg.value.ue_context_release_cmd()->gnb_du_ue_f1ap_id;
    case init_msg_type::ue_context_mod_request:
      return (gnb_du_ue_f1ap_id_t)init_msg.value.ue_context_mod_request()->gnb_du_ue_f1ap_id;
    case init_msg_type::ue_context_mod_required:
      return (gnb_du_ue_f1ap_id_t)init_msg.value.ue_context_mod_required()->gnb_du_ue_f1ap_id;
    case init_msg_type::ue_context_release_request:
      return (gnb_du_ue_f1ap_id_t)init_msg.value.ue_context_release_request()->gnb_du_ue_f1ap_id;
    case init_msg_type::dl_rrc_msg_transfer:
      return (gnb_du_ue_f1ap_id_t)init_msg.value.dl_rrc_msg_transfer()->gnb_du_ue_f1ap_id;
    case init_msg_type::ul_rrc_msg_transfer:
      return (gnb_du_ue_f1ap_id_t)init_msg.value.ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id;
    case init_msg_type::init_ul_rrc_msg_transfer:
      return (gnb_du_ue_f1ap_id_t)init_msg.value.init_ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id;
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<gnb_du_ue_f1ap_id_t> get_gnb_du_ue_f1ap_id(const asn1::f1ap::successful_outcome_s& success_outcome)
{
  return {default_error_t{}};
}

inline expected<gnb_du_ue_f1ap_id_t>
get_gnb_du_ue_f1ap_id(const asn1::f1ap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  return {default_error_t{}};
}

inline expected<gnb_du_ue_f1ap_id_t> get_gnb_du_ue_f1ap_id(const asn1::f1ap::f1ap_pdu_c& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu.type().value) {
    case f1ap_pdu_c::types_opts::init_msg:
      return get_gnb_du_ue_f1ap_id(pdu.init_msg());
    case f1ap_pdu_c::types_opts::successful_outcome:
      return get_gnb_du_ue_f1ap_id(pdu.successful_outcome());
    case f1ap_pdu_c::types_opts::unsuccessful_outcome:
      return get_gnb_du_ue_f1ap_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<unsigned> get_paging_ue_identity_index_value(const asn1::f1ap::paging_s& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu->ue_id_idx_value.type()) {
    case ue_id_idx_value_c::types_opts::idx_len10:
      return pdu->ue_id_idx_value.idx_len10().to_number();
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<uint64_t> get_paging_identity(const asn1::f1ap::paging_s& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu->paging_id.type()) {
    case paging_id_c::types_opts::ran_ue_paging_id: {
      return pdu->paging_id.ran_ue_paging_id().irnti.to_number();
    }
    case paging_id_c::types_opts::cn_ue_paging_id: {
      return pdu->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number();
    }
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<paging_identity_type> get_paging_identity_type(const asn1::f1ap::paging_s& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu->paging_id.type()) {
    case paging_id_c::types_opts::ran_ue_paging_id: {
      return paging_identity_type::ran_ue_paging_identity;
    }
    case paging_id_c::types_opts::cn_ue_paging_id: {
      return paging_identity_type::cn_ue_paging_identity;
    }
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<unsigned> get_paging_drx_in_nof_rf(const asn1::f1ap::paging_s& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu->paging_drx.value) {
    case paging_drx_opts::v32:
      return 32;
    case paging_drx_opts::v64:
      return 64;
    case paging_drx_opts::v128:
      return 128;
    case paging_drx_opts::v256:
      return 256;
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<unsigned> get_paging_priority(const asn1::f1ap::paging_s& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu->paging_prio.value) {
    case paging_prio_opts::priolevel1:
      return 1;
    case paging_prio_opts::priolevel2:
      return 2;
    case paging_prio_opts::priolevel3:
      return 3;
    case paging_prio_opts::priolevel4:
      return 4;
    case paging_prio_opts::priolevel5:
      return 5;
    case paging_prio_opts::priolevel6:
      return 6;
    case paging_prio_opts::priolevel7:
      return 7;
    case paging_prio_opts::priolevel8:
      return 8;
    default:
      break;
  }
  return {default_error_t{}};
}

} // namespace srsran

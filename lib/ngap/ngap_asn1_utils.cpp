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

#include "ngap_asn1_utils.h"
#include "srsran/asn1/ngap/ngap.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

const char* srsran::srs_cu_cp::get_cause_str(const asn1::ngap::cause_c& cause)
{
  using namespace asn1::ngap;
  switch (cause.type()) {
    case cause_c::types_opts::radio_network:
      return cause.radio_network().to_string();
    case cause_c::types_opts::transport:
      return cause.transport().to_string();
    case cause_c::types_opts::nas:
      return cause.nas().to_string();
    case cause_c::types_opts::protocol:
      return cause.protocol().to_string();
    case cause_c::types_opts::misc:
      return cause.misc().to_string();
    default:
      break;
  }
  return "unknown";
}

const char* srsran::srs_cu_cp::get_message_type_str(const asn1::ngap::ngap_pdu_c& pdu)
{
  using namespace asn1::ngap;
  switch (pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return pdu.init_msg().value.type().to_string();
    case ngap_pdu_c::types_opts::successful_outcome:
      return pdu.successful_outcome().value.type().to_string();
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return pdu.unsuccessful_outcome().value.type().to_string();
    default:
      break;
  }
  report_fatal_error("Invalid NGAP PDU type \"{}\"", pdu.type().to_string());
}

std::optional<ran_ue_id_t> srsran::srs_cu_cp::get_ran_ue_id(const asn1::ngap::init_msg_s& init_msg)
{
  using namespace asn1::ngap;
  using init_types = ngap_elem_procs_o::init_msg_c::types_opts;

  switch (init_msg.value.type()) {
    case init_types::ho_cancel:
      return uint_to_ran_ue_id(init_msg.value.ho_cancel()->ran_ue_ngap_id);
    case init_types::ho_required:
      return uint_to_ran_ue_id(init_msg.value.ho_required()->ran_ue_ngap_id);
    case init_types::init_context_setup_request:
      return uint_to_ran_ue_id(init_msg.value.init_context_setup_request()->ran_ue_ngap_id);
    case init_types::pdu_session_res_modify_request:
      return uint_to_ran_ue_id(init_msg.value.pdu_session_res_modify_request()->ran_ue_ngap_id);
    case init_types::pdu_session_res_modify_ind:
      return uint_to_ran_ue_id(init_msg.value.pdu_session_res_modify_ind()->ran_ue_ngap_id);
    case init_types::pdu_session_res_release_cmd:
      return uint_to_ran_ue_id(init_msg.value.pdu_session_res_release_cmd()->ran_ue_ngap_id);
    case init_types::pdu_session_res_setup_request:
      return uint_to_ran_ue_id(init_msg.value.pdu_session_res_setup_request()->ran_ue_ngap_id);
    case init_types::ue_context_mod_request:
      return uint_to_ran_ue_id(init_msg.value.ue_context_mod_request()->ran_ue_ngap_id);
    case init_types::ue_context_release_cmd:
      if (init_msg.value.ue_context_release_cmd()->ue_ngap_ids.type().value ==
          ue_ngap_ids_c::types_opts::ue_ngap_id_pair) {
        return uint_to_ran_ue_id(init_msg.value.ue_context_release_cmd()->ue_ngap_ids.ue_ngap_id_pair().ran_ue_ngap_id);
      }
      return std::nullopt;
    case init_types::ue_context_resume_request:
      return uint_to_ran_ue_id(init_msg.value.ue_context_resume_request()->ran_ue_ngap_id);
    case init_types::ue_context_suspend_request:
      return uint_to_ran_ue_id(init_msg.value.ue_context_suspend_request()->ran_ue_ngap_id);
    case init_types::dl_nas_transport:
      return uint_to_ran_ue_id(init_msg.value.dl_nas_transport()->ran_ue_ngap_id);
    case init_types::error_ind:
      if (init_msg.value.error_ind()->ran_ue_ngap_id_present) {
        return uint_to_ran_ue_id(init_msg.value.error_ind()->ran_ue_ngap_id);
      }
      break;
    case init_types::init_ue_msg:
      return uint_to_ran_ue_id(init_msg.value.init_ue_msg()->ran_ue_ngap_id);
    case init_types::ul_nas_transport:
      return uint_to_ran_ue_id(init_msg.value.ul_nas_transport()->ran_ue_ngap_id);
    default:
      break;
  }
  return std::nullopt;
}

std::optional<ran_ue_id_t> srsran::srs_cu_cp::get_ran_ue_id(const asn1::ngap::successful_outcome_s& success_outcome)
{
  using namespace asn1::ngap;
  using success_types = ngap_elem_procs_o::successful_outcome_c::types_opts;

  switch (success_outcome.value.type().value) {
    case success_types::ho_cancel_ack:
      return uint_to_ran_ue_id(success_outcome.value.ho_cancel_ack()->ran_ue_ngap_id);
    case success_types::ho_cmd:
      return uint_to_ran_ue_id(success_outcome.value.ho_cmd()->ran_ue_ngap_id);
    case success_types::ho_request_ack:
      return uint_to_ran_ue_id(success_outcome.value.ho_request_ack()->ran_ue_ngap_id);
    case success_types::init_context_setup_resp:
      return uint_to_ran_ue_id(success_outcome.value.init_context_setup_resp()->ran_ue_ngap_id);
    case success_types::pdu_session_res_modify_resp:
      return uint_to_ran_ue_id(success_outcome.value.pdu_session_res_modify_resp()->ran_ue_ngap_id);
    case success_types::pdu_session_res_modify_confirm:
      return uint_to_ran_ue_id(success_outcome.value.pdu_session_res_modify_confirm()->ran_ue_ngap_id);
    case success_types::pdu_session_res_release_resp:
      return uint_to_ran_ue_id(success_outcome.value.pdu_session_res_release_resp()->ran_ue_ngap_id);
    case success_types::pdu_session_res_setup_resp:
      return uint_to_ran_ue_id(success_outcome.value.pdu_session_res_setup_resp()->ran_ue_ngap_id);
    case success_types::ue_context_mod_resp:
      return uint_to_ran_ue_id(success_outcome.value.ue_context_mod_resp()->ran_ue_ngap_id);
    case success_types::ue_context_release_complete:
      return uint_to_ran_ue_id(success_outcome.value.ue_context_release_complete()->ran_ue_ngap_id);
    case success_types::ue_context_resume_resp:
      return uint_to_ran_ue_id(success_outcome.value.ue_context_resume_resp()->ran_ue_ngap_id);
    case success_types::ue_context_suspend_resp:
      return uint_to_ran_ue_id(success_outcome.value.ue_context_suspend_resp()->ran_ue_ngap_id);
    default:
      break;
  }

  return std::nullopt;
}

std::optional<ran_ue_id_t>
srsran::srs_cu_cp::get_ran_ue_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  using namespace asn1::ngap;
  using unsuccess_types = ngap_elem_procs_o::unsuccessful_outcome_c::types_opts;

  switch (unsuccessful_outcome.value.type().value) {
    case unsuccess_types::ho_prep_fail:
      return uint_to_ran_ue_id(unsuccessful_outcome.value.ho_prep_fail()->ran_ue_ngap_id);
    case unsuccess_types::ue_context_mod_fail:
      return uint_to_ran_ue_id(unsuccessful_outcome.value.ue_context_mod_fail()->ran_ue_ngap_id);
    case unsuccess_types::ue_context_resume_fail:
      return uint_to_ran_ue_id(unsuccessful_outcome.value.ue_context_resume_fail()->ran_ue_ngap_id);
    case unsuccess_types::ue_context_suspend_fail:
      return uint_to_ran_ue_id(unsuccessful_outcome.value.ue_context_suspend_fail()->ran_ue_ngap_id);
    default:
      break;
  }

  return std::nullopt;
}

std::optional<ran_ue_id_t> srsran::srs_cu_cp::get_ran_ue_id(const asn1::ngap::ngap_pdu_c& pdu)
{
  using namespace asn1::ngap;
  switch (pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return get_ran_ue_id(pdu.init_msg());
    case ngap_pdu_c::types_opts::successful_outcome:
      return get_ran_ue_id(pdu.successful_outcome());
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return get_ran_ue_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return std::nullopt;
}

std::optional<amf_ue_id_t> srsran::srs_cu_cp::get_amf_ue_id(const asn1::ngap::init_msg_s& init_msg)
{
  using namespace asn1::ngap;
  using init_types = ngap_elem_procs_o::init_msg_c::types_opts;

  switch (init_msg.value.type()) {
    case init_types::ho_cancel:
      return uint_to_amf_ue_id(init_msg.value.ho_cancel()->amf_ue_ngap_id);
    case init_types::ho_required:
      return uint_to_amf_ue_id(init_msg.value.ho_required()->amf_ue_ngap_id);
    case init_types::ho_request:
      return uint_to_amf_ue_id(init_msg.value.ho_request()->amf_ue_ngap_id);
    case init_types::init_context_setup_request:
      return uint_to_amf_ue_id(init_msg.value.init_context_setup_request()->amf_ue_ngap_id);
    case init_types::pdu_session_res_modify_request:
      return uint_to_amf_ue_id(init_msg.value.pdu_session_res_modify_request()->amf_ue_ngap_id);
    case init_types::pdu_session_res_modify_ind:
      return uint_to_amf_ue_id(init_msg.value.pdu_session_res_modify_ind()->amf_ue_ngap_id);
    case init_types::pdu_session_res_release_cmd:
      return uint_to_amf_ue_id(init_msg.value.pdu_session_res_release_cmd()->amf_ue_ngap_id);
    case init_types::pdu_session_res_setup_request:
      return uint_to_amf_ue_id(init_msg.value.pdu_session_res_setup_request()->amf_ue_ngap_id);
    case init_types::ue_context_mod_request:
      return uint_to_amf_ue_id(init_msg.value.ue_context_mod_request()->amf_ue_ngap_id);
    case init_types::ue_context_release_cmd:
      switch (init_msg.value.ue_context_release_cmd()->ue_ngap_ids.type().value) {
        case asn1::ngap::ue_ngap_ids_c::types_opts::ue_ngap_id_pair:
          return uint_to_amf_ue_id(
              init_msg.value.ue_context_release_cmd()->ue_ngap_ids.ue_ngap_id_pair().amf_ue_ngap_id);
        case asn1::ngap::ue_ngap_ids_c::types_opts::amf_ue_ngap_id:
          return uint_to_amf_ue_id(init_msg.value.ue_context_release_cmd()->ue_ngap_ids.amf_ue_ngap_id());
        default:
          break;
      }
      return std::nullopt;
    case init_types::dl_nas_transport:
      return uint_to_amf_ue_id(init_msg.value.dl_nas_transport()->amf_ue_ngap_id);
    case init_types::error_ind:
      if (init_msg.value.error_ind()->amf_ue_ngap_id_present) {
        return uint_to_amf_ue_id(init_msg.value.error_ind()->amf_ue_ngap_id);
      }
      break;
    case init_types::init_ue_msg:
      return std::nullopt;
    case init_types::ul_nas_transport:
      return uint_to_amf_ue_id(init_msg.value.ul_nas_transport()->amf_ue_ngap_id);
    default:
      break;
  }

  return std::nullopt;
}

std::optional<amf_ue_id_t> srsran::srs_cu_cp::get_amf_ue_id(const asn1::ngap::successful_outcome_s& success_outcome)
{
  using namespace asn1::ngap;
  using success_types = ngap_elem_procs_o::successful_outcome_c::types_opts;

  switch (success_outcome.value.type()) {
    case success_types::ho_cancel_ack:
      return uint_to_amf_ue_id(success_outcome.value.ho_cancel_ack()->amf_ue_ngap_id);
    case success_types::ho_cmd:
      return uint_to_amf_ue_id(success_outcome.value.ho_cmd()->amf_ue_ngap_id);
    case success_types::ho_request_ack:
      return uint_to_amf_ue_id(success_outcome.value.ho_request_ack()->amf_ue_ngap_id);
    case success_types::init_context_setup_resp:
      return uint_to_amf_ue_id(success_outcome.value.init_context_setup_resp()->amf_ue_ngap_id);
    case success_types::pdu_session_res_modify_resp:
      return uint_to_amf_ue_id(success_outcome.value.pdu_session_res_modify_resp()->amf_ue_ngap_id);
    case success_types::pdu_session_res_modify_confirm:
      return uint_to_amf_ue_id(success_outcome.value.pdu_session_res_modify_confirm()->amf_ue_ngap_id);
    case success_types::pdu_session_res_release_resp:
      return uint_to_amf_ue_id(success_outcome.value.pdu_session_res_release_resp()->amf_ue_ngap_id);
    case success_types::pdu_session_res_setup_resp:
      return uint_to_amf_ue_id(success_outcome.value.pdu_session_res_setup_resp()->amf_ue_ngap_id);
    case success_types::ue_context_mod_resp:
      return uint_to_amf_ue_id(success_outcome.value.ue_context_mod_resp()->amf_ue_ngap_id);
    case success_types::ue_context_release_complete:
      return uint_to_amf_ue_id(success_outcome.value.ue_context_release_complete()->amf_ue_ngap_id);
    case success_types::ue_context_resume_resp:
      return uint_to_amf_ue_id(success_outcome.value.ue_context_resume_resp()->amf_ue_ngap_id);
    case success_types::ue_context_suspend_resp:
      return uint_to_amf_ue_id(success_outcome.value.ue_context_suspend_resp()->amf_ue_ngap_id);
    case success_types::ue_radio_cap_check_resp:
      return uint_to_amf_ue_id(success_outcome.value.ue_radio_cap_check_resp()->amf_ue_ngap_id);
    default:
      break;
  }

  return std::nullopt;
}

std::optional<amf_ue_id_t>
srsran::srs_cu_cp::get_amf_ue_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  using namespace asn1::ngap;
  using unsuccess_types = ngap_elem_procs_o::unsuccessful_outcome_c::types_opts;

  switch (unsuccessful_outcome.value.type()) {
    case unsuccess_types::ho_prep_fail:
      return uint_to_amf_ue_id(unsuccessful_outcome.value.ho_prep_fail()->amf_ue_ngap_id);
    case unsuccess_types::ho_fail:
      return uint_to_amf_ue_id(unsuccessful_outcome.value.ho_fail()->amf_ue_ngap_id);
    case unsuccess_types::init_context_setup_fail:
      return uint_to_amf_ue_id(unsuccessful_outcome.value.init_context_setup_fail()->amf_ue_ngap_id);
    case unsuccess_types::ue_context_mod_fail:
      return uint_to_amf_ue_id(unsuccessful_outcome.value.ue_context_mod_fail()->amf_ue_ngap_id);
    case unsuccess_types::ue_context_resume_fail:
      return uint_to_amf_ue_id(unsuccessful_outcome.value.ue_context_resume_fail()->amf_ue_ngap_id);
    case unsuccess_types::ue_context_suspend_fail:
      return uint_to_amf_ue_id(unsuccessful_outcome.value.ue_context_suspend_fail()->amf_ue_ngap_id);
    default:
      break;
  }

  return std::nullopt;
}

std::optional<amf_ue_id_t> srsran::srs_cu_cp::get_amf_ue_id(const asn1::ngap::ngap_pdu_c& pdu)
{
  using namespace asn1::ngap;
  switch (pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return get_amf_ue_id(pdu.init_msg());
    case ngap_pdu_c::types_opts::successful_outcome:
      return get_amf_ue_id(pdu.successful_outcome());
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return get_amf_ue_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return std::nullopt;
}

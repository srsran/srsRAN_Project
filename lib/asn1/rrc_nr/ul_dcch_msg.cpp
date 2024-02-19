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

#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// UL-DCCH-MessageType ::= CHOICE
void ul_dcch_msg_type_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    case types::msg_class_ext:
      c.destroy<msg_class_ext_c_>();
      break;
    default:
      break;
  }
}
void ul_dcch_msg_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      c.init<msg_class_ext_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }
}
ul_dcch_msg_type_c::ul_dcch_msg_type_c(const ul_dcch_msg_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.init(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }
}
ul_dcch_msg_type_c& ul_dcch_msg_type_c::operator=(const ul_dcch_msg_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.set(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }

  return *this;
}
ul_dcch_msg_type_c::c1_c_& ul_dcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c.get<c1_c_>();
}
ul_dcch_msg_type_c::msg_class_ext_c_& ul_dcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
  return c.get<msg_class_ext_c_>();
}
void ul_dcch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      j.write_fieldname("messageClassExtension");
      c.get<msg_class_ext_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_dcch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::meas_report:
      c = meas_report_s{};
      break;
    case types::rrc_recfg_complete:
      c = rrc_recfg_complete_s{};
      break;
    case types::rrc_setup_complete:
      c = rrc_setup_complete_s{};
      break;
    case types::rrc_reest_complete:
      c = rrc_reest_complete_s{};
      break;
    case types::rrc_resume_complete:
      c = rrc_resume_complete_s{};
      break;
    case types::security_mode_complete:
      c = security_mode_complete_s{};
      break;
    case types::security_mode_fail:
      c = security_mode_fail_s{};
      break;
    case types::ul_info_transfer:
      c = ul_info_transfer_s{};
      break;
    case types::location_meas_ind:
      c = location_meas_ind_s{};
      break;
    case types::ue_cap_info:
      c = ue_cap_info_s{};
      break;
    case types::counter_check_resp:
      c = counter_check_resp_s{};
      break;
    case types::ue_assist_info:
      c = ue_assist_info_s{};
      break;
    case types::fail_info:
      c = fail_info_s{};
      break;
    case types::ul_info_transfer_mrdc:
      c = ul_info_transfer_mrdc_s{};
      break;
    case types::scg_fail_info:
      c = scg_fail_info_s{};
      break;
    case types::scg_fail_info_eutra:
      c = scg_fail_info_eutra_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
}
meas_report_s& ul_dcch_msg_type_c::c1_c_::set_meas_report()
{
  set(types::meas_report);
  return c.get<meas_report_s>();
}
rrc_recfg_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_recfg_complete()
{
  set(types::rrc_recfg_complete);
  return c.get<rrc_recfg_complete_s>();
}
rrc_setup_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_setup_complete()
{
  set(types::rrc_setup_complete);
  return c.get<rrc_setup_complete_s>();
}
rrc_reest_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_reest_complete()
{
  set(types::rrc_reest_complete);
  return c.get<rrc_reest_complete_s>();
}
rrc_resume_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_resume_complete()
{
  set(types::rrc_resume_complete);
  return c.get<rrc_resume_complete_s>();
}
security_mode_complete_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_complete()
{
  set(types::security_mode_complete);
  return c.get<security_mode_complete_s>();
}
security_mode_fail_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_fail()
{
  set(types::security_mode_fail);
  return c.get<security_mode_fail_s>();
}
ul_info_transfer_s& ul_dcch_msg_type_c::c1_c_::set_ul_info_transfer()
{
  set(types::ul_info_transfer);
  return c.get<ul_info_transfer_s>();
}
location_meas_ind_s& ul_dcch_msg_type_c::c1_c_::set_location_meas_ind()
{
  set(types::location_meas_ind);
  return c.get<location_meas_ind_s>();
}
ue_cap_info_s& ul_dcch_msg_type_c::c1_c_::set_ue_cap_info()
{
  set(types::ue_cap_info);
  return c.get<ue_cap_info_s>();
}
counter_check_resp_s& ul_dcch_msg_type_c::c1_c_::set_counter_check_resp()
{
  set(types::counter_check_resp);
  return c.get<counter_check_resp_s>();
}
ue_assist_info_s& ul_dcch_msg_type_c::c1_c_::set_ue_assist_info()
{
  set(types::ue_assist_info);
  return c.get<ue_assist_info_s>();
}
fail_info_s& ul_dcch_msg_type_c::c1_c_::set_fail_info()
{
  set(types::fail_info);
  return c.get<fail_info_s>();
}
ul_info_transfer_mrdc_s& ul_dcch_msg_type_c::c1_c_::set_ul_info_transfer_mrdc()
{
  set(types::ul_info_transfer_mrdc);
  return c.get<ul_info_transfer_mrdc_s>();
}
scg_fail_info_s& ul_dcch_msg_type_c::c1_c_::set_scg_fail_info()
{
  set(types::scg_fail_info);
  return c.get<scg_fail_info_s>();
}
scg_fail_info_eutra_s& ul_dcch_msg_type_c::c1_c_::set_scg_fail_info_eutra()
{
  set(types::scg_fail_info_eutra);
  return c.get<scg_fail_info_eutra_s>();
}
void ul_dcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_report:
      j.write_fieldname("measurementReport");
      c.get<meas_report_s>().to_json(j);
      break;
    case types::rrc_recfg_complete:
      j.write_fieldname("rrcReconfigurationComplete");
      c.get<rrc_recfg_complete_s>().to_json(j);
      break;
    case types::rrc_setup_complete:
      j.write_fieldname("rrcSetupComplete");
      c.get<rrc_setup_complete_s>().to_json(j);
      break;
    case types::rrc_reest_complete:
      j.write_fieldname("rrcReestablishmentComplete");
      c.get<rrc_reest_complete_s>().to_json(j);
      break;
    case types::rrc_resume_complete:
      j.write_fieldname("rrcResumeComplete");
      c.get<rrc_resume_complete_s>().to_json(j);
      break;
    case types::security_mode_complete:
      j.write_fieldname("securityModeComplete");
      c.get<security_mode_complete_s>().to_json(j);
      break;
    case types::security_mode_fail:
      j.write_fieldname("securityModeFailure");
      c.get<security_mode_fail_s>().to_json(j);
      break;
    case types::ul_info_transfer:
      j.write_fieldname("ulInformationTransfer");
      c.get<ul_info_transfer_s>().to_json(j);
      break;
    case types::location_meas_ind:
      j.write_fieldname("locationMeasurementIndication");
      c.get<location_meas_ind_s>().to_json(j);
      break;
    case types::ue_cap_info:
      j.write_fieldname("ueCapabilityInformation");
      c.get<ue_cap_info_s>().to_json(j);
      break;
    case types::counter_check_resp:
      j.write_fieldname("counterCheckResponse");
      c.get<counter_check_resp_s>().to_json(j);
      break;
    case types::ue_assist_info:
      j.write_fieldname("ueAssistanceInformation");
      c.get<ue_assist_info_s>().to_json(j);
      break;
    case types::fail_info:
      j.write_fieldname("failureInformation");
      c.get<fail_info_s>().to_json(j);
      break;
    case types::ul_info_transfer_mrdc:
      j.write_fieldname("ulInformationTransferMRDC");
      c.get<ul_info_transfer_mrdc_s>().to_json(j);
      break;
    case types::scg_fail_info:
      j.write_fieldname("scgFailureInformation");
      c.get<scg_fail_info_s>().to_json(j);
      break;
    case types::scg_fail_info_eutra:
      j.write_fieldname("scgFailureInformationEUTRA");
      c.get<scg_fail_info_eutra_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().pack(bref));
      break;
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.get<rrc_recfg_complete_s>().pack(bref));
      break;
    case types::rrc_setup_complete:
      HANDLE_CODE(c.get<rrc_setup_complete_s>().pack(bref));
      break;
    case types::rrc_reest_complete:
      HANDLE_CODE(c.get<rrc_reest_complete_s>().pack(bref));
      break;
    case types::rrc_resume_complete:
      HANDLE_CODE(c.get<rrc_resume_complete_s>().pack(bref));
      break;
    case types::security_mode_complete:
      HANDLE_CODE(c.get<security_mode_complete_s>().pack(bref));
      break;
    case types::security_mode_fail:
      HANDLE_CODE(c.get<security_mode_fail_s>().pack(bref));
      break;
    case types::ul_info_transfer:
      HANDLE_CODE(c.get<ul_info_transfer_s>().pack(bref));
      break;
    case types::location_meas_ind:
      HANDLE_CODE(c.get<location_meas_ind_s>().pack(bref));
      break;
    case types::ue_cap_info:
      HANDLE_CODE(c.get<ue_cap_info_s>().pack(bref));
      break;
    case types::counter_check_resp:
      HANDLE_CODE(c.get<counter_check_resp_s>().pack(bref));
      break;
    case types::ue_assist_info:
      HANDLE_CODE(c.get<ue_assist_info_s>().pack(bref));
      break;
    case types::fail_info:
      HANDLE_CODE(c.get<fail_info_s>().pack(bref));
      break;
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.get<ul_info_transfer_mrdc_s>().pack(bref));
      break;
    case types::scg_fail_info:
      HANDLE_CODE(c.get<scg_fail_info_s>().pack(bref));
      break;
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.get<scg_fail_info_eutra_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().unpack(bref));
      break;
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.get<rrc_recfg_complete_s>().unpack(bref));
      break;
    case types::rrc_setup_complete:
      HANDLE_CODE(c.get<rrc_setup_complete_s>().unpack(bref));
      break;
    case types::rrc_reest_complete:
      HANDLE_CODE(c.get<rrc_reest_complete_s>().unpack(bref));
      break;
    case types::rrc_resume_complete:
      HANDLE_CODE(c.get<rrc_resume_complete_s>().unpack(bref));
      break;
    case types::security_mode_complete:
      HANDLE_CODE(c.get<security_mode_complete_s>().unpack(bref));
      break;
    case types::security_mode_fail:
      HANDLE_CODE(c.get<security_mode_fail_s>().unpack(bref));
      break;
    case types::ul_info_transfer:
      HANDLE_CODE(c.get<ul_info_transfer_s>().unpack(bref));
      break;
    case types::location_meas_ind:
      HANDLE_CODE(c.get<location_meas_ind_s>().unpack(bref));
      break;
    case types::ue_cap_info:
      HANDLE_CODE(c.get<ue_cap_info_s>().unpack(bref));
      break;
    case types::counter_check_resp:
      HANDLE_CODE(c.get<counter_check_resp_s>().unpack(bref));
      break;
    case types::ue_assist_info:
      HANDLE_CODE(c.get<ue_assist_info_s>().unpack(bref));
      break;
    case types::fail_info:
      HANDLE_CODE(c.get<fail_info_s>().unpack(bref));
      break;
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.get<ul_info_transfer_mrdc_s>().unpack(bref));
      break;
    case types::scg_fail_info:
      HANDLE_CODE(c.get<scg_fail_info_s>().unpack(bref));
      break;
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.get<scg_fail_info_eutra_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"measurementReport",
                                "rrcReconfigurationComplete",
                                "rrcSetupComplete",
                                "rrcReestablishmentComplete",
                                "rrcResumeComplete",
                                "securityModeComplete",
                                "securityModeFailure",
                                "ulInformationTransfer",
                                "locationMeasurementIndication",
                                "ueCapabilityInformation",
                                "counterCheckResponse",
                                "ueAssistanceInformation",
                                "failureInformation",
                                "ulInformationTransferMRDC",
                                "scgFailureInformation",
                                "scgFailureInformationEUTRA"};
  return convert_enum_idx(names, 16, value, "ul_dcch_msg_type_c::c1_c_::types");
}

void ul_dcch_msg_type_c::msg_class_ext_c_::set(types::options e)
{
  type_ = e;
}
ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_& ul_dcch_msg_type_c::msg_class_ext_c_::set_c2()
{
  set(types::c2);
  return c;
}
void ul_dcch_msg_type_c::msg_class_ext_c_::set_msg_class_ext_future_r16()
{
  set(types::msg_class_ext_future_r16);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c2:
      j.write_fieldname("c2");
      c.to_json(j);
      break;
    case types::msg_class_ext_future_r16:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext_future_r16:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext_future_r16:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ul_ded_msg_segment_r16:
      c = ul_ded_msg_segment_r16_s{};
      break;
    case types::ded_sib_request_r16:
      c = ded_sib_request_r16_s{};
      break;
    case types::mcg_fail_info_r16:
      c = mcg_fail_info_r16_s{};
      break;
    case types::ue_info_resp_r16:
      c = ue_info_resp_r16_s{};
      break;
    case types::sidelink_ue_info_nr_r16:
      c = sidelink_ue_info_nr_r16_s{};
      break;
    case types::ul_info_transfer_irat_r16:
      c = ul_info_transfer_irat_r16_s{};
      break;
    case types::iab_other_info_r16:
      c = iab_other_info_r16_s{};
      break;
    case types::mbs_interest_ind_r17:
      c = mb_si_nterest_ind_r17_s{};
      break;
    case types::ue_positioning_assist_info_r17:
      c = ue_positioning_assist_info_r17_s{};
      break;
    case types::meas_report_app_layer_r17:
      c = meas_report_app_layer_r17_s{};
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
  }
}
ul_ded_msg_segment_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ul_ded_msg_segment_r16()
{
  set(types::ul_ded_msg_segment_r16);
  return c.get<ul_ded_msg_segment_r16_s>();
}
ded_sib_request_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ded_sib_request_r16()
{
  set(types::ded_sib_request_r16);
  return c.get<ded_sib_request_r16_s>();
}
mcg_fail_info_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_mcg_fail_info_r16()
{
  set(types::mcg_fail_info_r16);
  return c.get<mcg_fail_info_r16_s>();
}
ue_info_resp_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_info_resp_r16()
{
  set(types::ue_info_resp_r16);
  return c.get<ue_info_resp_r16_s>();
}
sidelink_ue_info_nr_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_sidelink_ue_info_nr_r16()
{
  set(types::sidelink_ue_info_nr_r16);
  return c.get<sidelink_ue_info_nr_r16_s>();
}
ul_info_transfer_irat_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ul_info_transfer_irat_r16()
{
  set(types::ul_info_transfer_irat_r16);
  return c.get<ul_info_transfer_irat_r16_s>();
}
iab_other_info_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_iab_other_info_r16()
{
  set(types::iab_other_info_r16);
  return c.get<iab_other_info_r16_s>();
}
mb_si_nterest_ind_r17_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_mbs_interest_ind_r17()
{
  set(types::mbs_interest_ind_r17);
  return c.get<mb_si_nterest_ind_r17_s>();
}
ue_positioning_assist_info_r17_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_positioning_assist_info_r17()
{
  set(types::ue_positioning_assist_info_r17);
  return c.get<ue_positioning_assist_info_r17_s>();
}
meas_report_app_layer_r17_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_meas_report_app_layer_r17()
{
  set(types::meas_report_app_layer_r17);
  return c.get<meas_report_app_layer_r17_s>();
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare6()
{
  set(types::spare6);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare5()
{
  set(types::spare5);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare4()
{
  set(types::spare4);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare3()
{
  set(types::spare3);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare2()
{
  set(types::spare2);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare1()
{
  set(types::spare1);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_ded_msg_segment_r16:
      j.write_fieldname("ulDedicatedMessageSegment-r16");
      c.get<ul_ded_msg_segment_r16_s>().to_json(j);
      break;
    case types::ded_sib_request_r16:
      j.write_fieldname("dedicatedSIBRequest-r16");
      c.get<ded_sib_request_r16_s>().to_json(j);
      break;
    case types::mcg_fail_info_r16:
      j.write_fieldname("mcgFailureInformation-r16");
      c.get<mcg_fail_info_r16_s>().to_json(j);
      break;
    case types::ue_info_resp_r16:
      j.write_fieldname("ueInformationResponse-r16");
      c.get<ue_info_resp_r16_s>().to_json(j);
      break;
    case types::sidelink_ue_info_nr_r16:
      j.write_fieldname("sidelinkUEInformationNR-r16");
      c.get<sidelink_ue_info_nr_r16_s>().to_json(j);
      break;
    case types::ul_info_transfer_irat_r16:
      j.write_fieldname("ulInformationTransferIRAT-r16");
      c.get<ul_info_transfer_irat_r16_s>().to_json(j);
      break;
    case types::iab_other_info_r16:
      j.write_fieldname("iabOtherInformation-r16");
      c.get<iab_other_info_r16_s>().to_json(j);
      break;
    case types::mbs_interest_ind_r17:
      j.write_fieldname("mbsInterestIndication-r17");
      c.get<mb_si_nterest_ind_r17_s>().to_json(j);
      break;
    case types::ue_positioning_assist_info_r17:
      j.write_fieldname("uePositioningAssistanceInfo-r17");
      c.get<ue_positioning_assist_info_r17_s>().to_json(j);
      break;
    case types::meas_report_app_layer_r17:
      j.write_fieldname("measurementReportAppLayer-r17");
      c.get<meas_report_app_layer_r17_s>().to_json(j);
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_ded_msg_segment_r16:
      HANDLE_CODE(c.get<ul_ded_msg_segment_r16_s>().pack(bref));
      break;
    case types::ded_sib_request_r16:
      HANDLE_CODE(c.get<ded_sib_request_r16_s>().pack(bref));
      break;
    case types::mcg_fail_info_r16:
      HANDLE_CODE(c.get<mcg_fail_info_r16_s>().pack(bref));
      break;
    case types::ue_info_resp_r16:
      HANDLE_CODE(c.get<ue_info_resp_r16_s>().pack(bref));
      break;
    case types::sidelink_ue_info_nr_r16:
      HANDLE_CODE(c.get<sidelink_ue_info_nr_r16_s>().pack(bref));
      break;
    case types::ul_info_transfer_irat_r16:
      HANDLE_CODE(c.get<ul_info_transfer_irat_r16_s>().pack(bref));
      break;
    case types::iab_other_info_r16:
      HANDLE_CODE(c.get<iab_other_info_r16_s>().pack(bref));
      break;
    case types::mbs_interest_ind_r17:
      HANDLE_CODE(c.get<mb_si_nterest_ind_r17_s>().pack(bref));
      break;
    case types::ue_positioning_assist_info_r17:
      HANDLE_CODE(c.get<ue_positioning_assist_info_r17_s>().pack(bref));
      break;
    case types::meas_report_app_layer_r17:
      HANDLE_CODE(c.get<meas_report_app_layer_r17_s>().pack(bref));
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_ded_msg_segment_r16:
      HANDLE_CODE(c.get<ul_ded_msg_segment_r16_s>().unpack(bref));
      break;
    case types::ded_sib_request_r16:
      HANDLE_CODE(c.get<ded_sib_request_r16_s>().unpack(bref));
      break;
    case types::mcg_fail_info_r16:
      HANDLE_CODE(c.get<mcg_fail_info_r16_s>().unpack(bref));
      break;
    case types::ue_info_resp_r16:
      HANDLE_CODE(c.get<ue_info_resp_r16_s>().unpack(bref));
      break;
    case types::sidelink_ue_info_nr_r16:
      HANDLE_CODE(c.get<sidelink_ue_info_nr_r16_s>().unpack(bref));
      break;
    case types::ul_info_transfer_irat_r16:
      HANDLE_CODE(c.get<ul_info_transfer_irat_r16_s>().unpack(bref));
      break;
    case types::iab_other_info_r16:
      HANDLE_CODE(c.get<iab_other_info_r16_s>().unpack(bref));
      break;
    case types::mbs_interest_ind_r17:
      HANDLE_CODE(c.get<mb_si_nterest_ind_r17_s>().unpack(bref));
      break;
    case types::ue_positioning_assist_info_r17:
      HANDLE_CODE(c.get<ue_positioning_assist_info_r17_s>().unpack(bref));
      break;
    case types::meas_report_app_layer_r17:
      HANDLE_CODE(c.get<meas_report_app_layer_r17_s>().unpack(bref));
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulDedicatedMessageSegment-r16",
                                "dedicatedSIBRequest-r16",
                                "mcgFailureInformation-r16",
                                "ueInformationResponse-r16",
                                "sidelinkUEInformationNR-r16",
                                "ulInformationTransferIRAT-r16",
                                "iabOtherInformation-r16",
                                "mbsInterestIndication-r17",
                                "uePositioningAssistanceInfo-r17",
                                "measurementReportAppLayer-r17",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types");
}

const char* ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* names[] = {"c2", "messageClassExtensionFuture-r16"};
  return convert_enum_idx(names, 2, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}

const char* ul_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "ul_dcch_msg_type_c::types");
}
uint8_t ul_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_dcch_msg_type_c::types");
}

// UL-DCCH-Message ::= SEQUENCE
SRSASN_CODE ul_dcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_dcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-DCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

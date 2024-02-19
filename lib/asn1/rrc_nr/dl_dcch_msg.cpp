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

#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg_ies.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// DL-DCCH-MessageType ::= CHOICE
void dl_dcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
dl_dcch_msg_type_c::c1_c_& dl_dcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void dl_dcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void dl_dcch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_dcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_dcch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::rrc_recfg:
      c = rrc_recfg_s{};
      break;
    case types::rrc_resume:
      c = rrc_resume_s{};
      break;
    case types::rrc_release:
      c = rrc_release_s{};
      break;
    case types::rrc_reest:
      c = rrc_reest_s{};
      break;
    case types::security_mode_cmd:
      c = security_mode_cmd_s{};
      break;
    case types::dl_info_transfer:
      c = dl_info_transfer_s{};
      break;
    case types::ue_cap_enquiry:
      c = ue_cap_enquiry_s{};
      break;
    case types::counter_check:
      c = counter_check_s{};
      break;
    case types::mob_from_nr_cmd:
      c = mob_from_nr_cmd_s{};
      break;
    case types::dl_ded_msg_segment_r16:
      c = dl_ded_msg_segment_r16_s{};
      break;
    case types::ue_info_request_r16:
      c = ue_info_request_r16_s{};
      break;
    case types::dl_info_transfer_mrdc_r16:
      c = dl_info_transfer_mrdc_r16_s{};
      break;
    case types::logged_meas_cfg_r16:
      c = logged_meas_cfg_r16_s{};
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
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
  }
}
rrc_recfg_s& dl_dcch_msg_type_c::c1_c_::set_rrc_recfg()
{
  set(types::rrc_recfg);
  return c.get<rrc_recfg_s>();
}
rrc_resume_s& dl_dcch_msg_type_c::c1_c_::set_rrc_resume()
{
  set(types::rrc_resume);
  return c.get<rrc_resume_s>();
}
rrc_release_s& dl_dcch_msg_type_c::c1_c_::set_rrc_release()
{
  set(types::rrc_release);
  return c.get<rrc_release_s>();
}
rrc_reest_s& dl_dcch_msg_type_c::c1_c_::set_rrc_reest()
{
  set(types::rrc_reest);
  return c.get<rrc_reest_s>();
}
security_mode_cmd_s& dl_dcch_msg_type_c::c1_c_::set_security_mode_cmd()
{
  set(types::security_mode_cmd);
  return c.get<security_mode_cmd_s>();
}
dl_info_transfer_s& dl_dcch_msg_type_c::c1_c_::set_dl_info_transfer()
{
  set(types::dl_info_transfer);
  return c.get<dl_info_transfer_s>();
}
ue_cap_enquiry_s& dl_dcch_msg_type_c::c1_c_::set_ue_cap_enquiry()
{
  set(types::ue_cap_enquiry);
  return c.get<ue_cap_enquiry_s>();
}
counter_check_s& dl_dcch_msg_type_c::c1_c_::set_counter_check()
{
  set(types::counter_check);
  return c.get<counter_check_s>();
}
mob_from_nr_cmd_s& dl_dcch_msg_type_c::c1_c_::set_mob_from_nr_cmd()
{
  set(types::mob_from_nr_cmd);
  return c.get<mob_from_nr_cmd_s>();
}
dl_ded_msg_segment_r16_s& dl_dcch_msg_type_c::c1_c_::set_dl_ded_msg_segment_r16()
{
  set(types::dl_ded_msg_segment_r16);
  return c.get<dl_ded_msg_segment_r16_s>();
}
ue_info_request_r16_s& dl_dcch_msg_type_c::c1_c_::set_ue_info_request_r16()
{
  set(types::ue_info_request_r16);
  return c.get<ue_info_request_r16_s>();
}
dl_info_transfer_mrdc_r16_s& dl_dcch_msg_type_c::c1_c_::set_dl_info_transfer_mrdc_r16()
{
  set(types::dl_info_transfer_mrdc_r16);
  return c.get<dl_info_transfer_mrdc_r16_s>();
}
logged_meas_cfg_r16_s& dl_dcch_msg_type_c::c1_c_::set_logged_meas_cfg_r16()
{
  set(types::logged_meas_cfg_r16);
  return c.get<logged_meas_cfg_r16_s>();
}
void dl_dcch_msg_type_c::c1_c_::set_spare3()
{
  set(types::spare3);
}
void dl_dcch_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_dcch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void dl_dcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_recfg:
      j.write_fieldname("rrcReconfiguration");
      c.get<rrc_recfg_s>().to_json(j);
      break;
    case types::rrc_resume:
      j.write_fieldname("rrcResume");
      c.get<rrc_resume_s>().to_json(j);
      break;
    case types::rrc_release:
      j.write_fieldname("rrcRelease");
      c.get<rrc_release_s>().to_json(j);
      break;
    case types::rrc_reest:
      j.write_fieldname("rrcReestablishment");
      c.get<rrc_reest_s>().to_json(j);
      break;
    case types::security_mode_cmd:
      j.write_fieldname("securityModeCommand");
      c.get<security_mode_cmd_s>().to_json(j);
      break;
    case types::dl_info_transfer:
      j.write_fieldname("dlInformationTransfer");
      c.get<dl_info_transfer_s>().to_json(j);
      break;
    case types::ue_cap_enquiry:
      j.write_fieldname("ueCapabilityEnquiry");
      c.get<ue_cap_enquiry_s>().to_json(j);
      break;
    case types::counter_check:
      j.write_fieldname("counterCheck");
      c.get<counter_check_s>().to_json(j);
      break;
    case types::mob_from_nr_cmd:
      j.write_fieldname("mobilityFromNRCommand");
      c.get<mob_from_nr_cmd_s>().to_json(j);
      break;
    case types::dl_ded_msg_segment_r16:
      j.write_fieldname("dlDedicatedMessageSegment-r16");
      c.get<dl_ded_msg_segment_r16_s>().to_json(j);
      break;
    case types::ue_info_request_r16:
      j.write_fieldname("ueInformationRequest-r16");
      c.get<ue_info_request_r16_s>().to_json(j);
      break;
    case types::dl_info_transfer_mrdc_r16:
      j.write_fieldname("dlInformationTransferMRDC-r16");
      c.get<dl_info_transfer_mrdc_r16_s>().to_json(j);
      break;
    case types::logged_meas_cfg_r16:
      j.write_fieldname("loggedMeasurementConfiguration-r16");
      c.get<logged_meas_cfg_r16_s>().to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_dcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_recfg:
      HANDLE_CODE(c.get<rrc_recfg_s>().pack(bref));
      break;
    case types::rrc_resume:
      HANDLE_CODE(c.get<rrc_resume_s>().pack(bref));
      break;
    case types::rrc_release:
      HANDLE_CODE(c.get<rrc_release_s>().pack(bref));
      break;
    case types::rrc_reest:
      HANDLE_CODE(c.get<rrc_reest_s>().pack(bref));
      break;
    case types::security_mode_cmd:
      HANDLE_CODE(c.get<security_mode_cmd_s>().pack(bref));
      break;
    case types::dl_info_transfer:
      HANDLE_CODE(c.get<dl_info_transfer_s>().pack(bref));
      break;
    case types::ue_cap_enquiry:
      HANDLE_CODE(c.get<ue_cap_enquiry_s>().pack(bref));
      break;
    case types::counter_check:
      HANDLE_CODE(c.get<counter_check_s>().pack(bref));
      break;
    case types::mob_from_nr_cmd:
      HANDLE_CODE(c.get<mob_from_nr_cmd_s>().pack(bref));
      break;
    case types::dl_ded_msg_segment_r16:
      HANDLE_CODE(c.get<dl_ded_msg_segment_r16_s>().pack(bref));
      break;
    case types::ue_info_request_r16:
      HANDLE_CODE(c.get<ue_info_request_r16_s>().pack(bref));
      break;
    case types::dl_info_transfer_mrdc_r16:
      HANDLE_CODE(c.get<dl_info_transfer_mrdc_r16_s>().pack(bref));
      break;
    case types::logged_meas_cfg_r16:
      HANDLE_CODE(c.get<logged_meas_cfg_r16_s>().pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_recfg:
      HANDLE_CODE(c.get<rrc_recfg_s>().unpack(bref));
      break;
    case types::rrc_resume:
      HANDLE_CODE(c.get<rrc_resume_s>().unpack(bref));
      break;
    case types::rrc_release:
      HANDLE_CODE(c.get<rrc_release_s>().unpack(bref));
      break;
    case types::rrc_reest:
      HANDLE_CODE(c.get<rrc_reest_s>().unpack(bref));
      break;
    case types::security_mode_cmd:
      HANDLE_CODE(c.get<security_mode_cmd_s>().unpack(bref));
      break;
    case types::dl_info_transfer:
      HANDLE_CODE(c.get<dl_info_transfer_s>().unpack(bref));
      break;
    case types::ue_cap_enquiry:
      HANDLE_CODE(c.get<ue_cap_enquiry_s>().unpack(bref));
      break;
    case types::counter_check:
      HANDLE_CODE(c.get<counter_check_s>().unpack(bref));
      break;
    case types::mob_from_nr_cmd:
      HANDLE_CODE(c.get<mob_from_nr_cmd_s>().unpack(bref));
      break;
    case types::dl_ded_msg_segment_r16:
      HANDLE_CODE(c.get<dl_ded_msg_segment_r16_s>().unpack(bref));
      break;
    case types::ue_info_request_r16:
      HANDLE_CODE(c.get<ue_info_request_r16_s>().unpack(bref));
      break;
    case types::dl_info_transfer_mrdc_r16:
      HANDLE_CODE(c.get<dl_info_transfer_mrdc_r16_s>().unpack(bref));
      break;
    case types::logged_meas_cfg_r16:
      HANDLE_CODE(c.get<logged_meas_cfg_r16_s>().unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReconfiguration",
                                "rrcResume",
                                "rrcRelease",
                                "rrcReestablishment",
                                "securityModeCommand",
                                "dlInformationTransfer",
                                "ueCapabilityEnquiry",
                                "counterCheck",
                                "mobilityFromNRCommand",
                                "dlDedicatedMessageSegment-r16",
                                "ueInformationRequest-r16",
                                "dlInformationTransferMRDC-r16",
                                "loggedMeasurementConfiguration-r16",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "dl_dcch_msg_type_c::c1_c_::types");
}

const char* dl_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "dl_dcch_msg_type_c::types");
}
uint8_t dl_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "dl_dcch_msg_type_c::types");
}

// DL-DCCH-Message ::= SEQUENCE
SRSASN_CODE dl_dcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_dcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void dl_dcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("DL-DCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

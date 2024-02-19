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

#include "srsran/asn1/rrc_nr/nr_ue_variables.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// CLI-TriggeredList-r16 ::= CHOICE
void cli_triggered_list_r16_c::destroy_()
{
  switch (type_) {
    case types::srs_rsrp_triggered_list_r16:
      c.destroy<srs_rsrp_triggered_list_r16_l>();
      break;
    case types::cli_rssi_triggered_list_r16:
      c.destroy<cli_rssi_triggered_list_r16_l>();
      break;
    default:
      break;
  }
}
void cli_triggered_list_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::srs_rsrp_triggered_list_r16:
      c.init<srs_rsrp_triggered_list_r16_l>();
      break;
    case types::cli_rssi_triggered_list_r16:
      c.init<cli_rssi_triggered_list_r16_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cli_triggered_list_r16_c");
  }
}
cli_triggered_list_r16_c::cli_triggered_list_r16_c(const cli_triggered_list_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::srs_rsrp_triggered_list_r16:
      c.init(other.c.get<srs_rsrp_triggered_list_r16_l>());
      break;
    case types::cli_rssi_triggered_list_r16:
      c.init(other.c.get<cli_rssi_triggered_list_r16_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cli_triggered_list_r16_c");
  }
}
cli_triggered_list_r16_c& cli_triggered_list_r16_c::operator=(const cli_triggered_list_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::srs_rsrp_triggered_list_r16:
      c.set(other.c.get<srs_rsrp_triggered_list_r16_l>());
      break;
    case types::cli_rssi_triggered_list_r16:
      c.set(other.c.get<cli_rssi_triggered_list_r16_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cli_triggered_list_r16_c");
  }

  return *this;
}
srs_rsrp_triggered_list_r16_l& cli_triggered_list_r16_c::set_srs_rsrp_triggered_list_r16()
{
  set(types::srs_rsrp_triggered_list_r16);
  return c.get<srs_rsrp_triggered_list_r16_l>();
}
cli_rssi_triggered_list_r16_l& cli_triggered_list_r16_c::set_cli_rssi_triggered_list_r16()
{
  set(types::cli_rssi_triggered_list_r16);
  return c.get<cli_rssi_triggered_list_r16_l>();
}
void cli_triggered_list_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::srs_rsrp_triggered_list_r16:
      j.start_array("srs-RSRP-TriggeredList-r16");
      for (const auto& e1 : c.get<srs_rsrp_triggered_list_r16_l>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::cli_rssi_triggered_list_r16:
      j.start_array("cli-RSSI-TriggeredList-r16");
      for (const auto& e1 : c.get<cli_rssi_triggered_list_r16_l>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "cli_triggered_list_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE cli_triggered_list_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::srs_rsrp_triggered_list_r16:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srs_rsrp_triggered_list_r16_l>(), 1, 32, integer_packer<uint8_t>(0, 63)));
      break;
    case types::cli_rssi_triggered_list_r16:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cli_rssi_triggered_list_r16_l>(), 1, 64, integer_packer<uint8_t>(0, 63)));
      break;
    default:
      log_invalid_choice_id(type_, "cli_triggered_list_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cli_triggered_list_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::srs_rsrp_triggered_list_r16:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<srs_rsrp_triggered_list_r16_l>(), bref, 1, 32, integer_packer<uint8_t>(0, 63)));
      break;
    case types::cli_rssi_triggered_list_r16:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<cli_rssi_triggered_list_r16_l>(), bref, 1, 64, integer_packer<uint8_t>(0, 63)));
      break;
    default:
      log_invalid_choice_id(type_, "cli_triggered_list_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cli_triggered_list_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"srs-RSRP-TriggeredList-r16", "cli-RSSI-TriggeredList-r16"};
  return convert_enum_idx(names, 2, value, "cli_triggered_list_r16_c::types");
}

void cells_triggered_list_item_c_::destroy_() {}
void cells_triggered_list_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
cells_triggered_list_item_c_::cells_triggered_list_item_c_(const cells_triggered_list_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pci:
      c.init(other.c.get<uint16_t>());
      break;
    case types::pci_eutra:
      c.init(other.c.get<uint16_t>());
      break;
    case types::pci_utra_fdd_r16:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }
}
cells_triggered_list_item_c_& cells_triggered_list_item_c_::operator=(const cells_triggered_list_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pci:
      c.set(other.c.get<uint16_t>());
      break;
    case types::pci_eutra:
      c.set(other.c.get<uint16_t>());
      break;
    case types::pci_utra_fdd_r16:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }

  return *this;
}
uint16_t& cells_triggered_list_item_c_::set_pci()
{
  set(types::pci);
  return c.get<uint16_t>();
}
uint16_t& cells_triggered_list_item_c_::set_pci_eutra()
{
  set(types::pci_eutra);
  return c.get<uint16_t>();
}
uint16_t& cells_triggered_list_item_c_::set_pci_utra_fdd_r16()
{
  set(types::pci_utra_fdd_r16);
  return c.get<uint16_t>();
}
void cells_triggered_list_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pci:
      j.write_int("physCellId", c.get<uint16_t>());
      break;
    case types::pci_eutra:
      j.write_int("physCellIdEUTRA", c.get<uint16_t>());
      break;
    case types::pci_utra_fdd_r16:
      j.write_int("physCellIdUTRA-FDD-r16", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE cells_triggered_list_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pci:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1007u));
      break;
    case types::pci_eutra:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)503u));
      break;
    case types::pci_utra_fdd_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_triggered_list_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pci:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1007u));
      break;
    case types::pci_eutra:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)503u));
      break;
    case types::pci_utra_fdd_r16:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cells_triggered_list_item_c_::types_opts::to_string() const
{
  static const char* names[] = {"physCellId", "physCellIdEUTRA", "physCellIdUTRA-FDD-r16"};
  return convert_enum_idx(names, 3, value, "cells_triggered_list_item_c_::types");
}

// VarConditionalReconfig ::= SEQUENCE
SRSASN_CODE var_conditional_recfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cond_recfg_list.size() > 0, 1));

  if (cond_recfg_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cond_recfg_list, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_conditional_recfg_s::unpack(cbit_ref& bref)
{
  bool cond_recfg_list_present;
  HANDLE_CODE(bref.unpack(cond_recfg_list_present, 1));

  if (cond_recfg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cond_recfg_list, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void var_conditional_recfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cond_recfg_list.size() > 0) {
    j.start_array("condReconfigList");
    for (const auto& e1 : cond_recfg_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// VarConnEstFailReport-r16 ::= SEQUENCE
SRSASN_CODE var_conn_est_fail_report_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(conn_est_fail_report_r16.pack(bref));
  HANDLE_CODE(plmn_id_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_conn_est_fail_report_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(conn_est_fail_report_r16.unpack(bref));
  HANDLE_CODE(plmn_id_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_conn_est_fail_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("connEstFailReport-r16");
  conn_est_fail_report_r16.to_json(j);
  j.write_fieldname("plmn-Identity-r16");
  plmn_id_r16.to_json(j);
  j.end_obj();
}

// VarConnEstFailReportList-r17 ::= SEQUENCE
SRSASN_CODE var_conn_est_fail_report_list_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, conn_est_fail_report_list_r17, 1, 4));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_conn_est_fail_report_list_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(conn_est_fail_report_list_r17, bref, 1, 4));

  return SRSASN_SUCCESS;
}
void var_conn_est_fail_report_list_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("connEstFailReportList-r17");
  for (const auto& e1 : conn_est_fail_report_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// VarLogMeasConfig-r16-IEs ::= SEQUENCE
SRSASN_CODE var_log_meas_cfg_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(bt_name_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(wlan_name_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sensor_name_list_r16_present, 1));
  HANDLE_CODE(bref.pack(early_meas_ind_r17_present, 1));
  HANDLE_CODE(bref.pack(area_cfg_v1700_present, 1));

  if (area_cfg_r16_present) {
    HANDLE_CODE(area_cfg_r16.pack(bref));
  }
  if (bt_name_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bt_name_list_r16, 1, 4));
  }
  if (wlan_name_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_name_list_r16, 1, 4));
  }
  if (sensor_name_list_r16_present) {
    HANDLE_CODE(sensor_name_list_r16.pack(bref));
  }
  HANDLE_CODE(logging_dur_r16.pack(bref));
  HANDLE_CODE(report_type.pack(bref));
  if (area_cfg_v1700_present) {
    HANDLE_CODE(area_cfg_v1700.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_cfg_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r16_present, 1));
  bool bt_name_list_r16_present;
  HANDLE_CODE(bref.unpack(bt_name_list_r16_present, 1));
  bool wlan_name_list_r16_present;
  HANDLE_CODE(bref.unpack(wlan_name_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sensor_name_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(early_meas_ind_r17_present, 1));
  HANDLE_CODE(bref.unpack(area_cfg_v1700_present, 1));

  if (area_cfg_r16_present) {
    HANDLE_CODE(area_cfg_r16.unpack(bref));
  }
  if (bt_name_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bt_name_list_r16, bref, 1, 4));
  }
  if (wlan_name_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_name_list_r16, bref, 1, 4));
  }
  if (sensor_name_list_r16_present) {
    HANDLE_CODE(sensor_name_list_r16.unpack(bref));
  }
  HANDLE_CODE(logging_dur_r16.unpack(bref));
  HANDLE_CODE(report_type.unpack(bref));
  if (area_cfg_v1700_present) {
    HANDLE_CODE(area_cfg_v1700.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void var_log_meas_cfg_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (area_cfg_r16_present) {
    j.write_fieldname("areaConfiguration-r16");
    area_cfg_r16.to_json(j);
  }
  if (bt_name_list_r16.size() > 0) {
    j.start_array("bt-NameList-r16");
    for (const auto& e1 : bt_name_list_r16) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (wlan_name_list_r16.size() > 0) {
    j.start_array("wlan-NameList-r16");
    for (const auto& e1 : wlan_name_list_r16) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (sensor_name_list_r16_present) {
    j.write_fieldname("sensor-NameList-r16");
    sensor_name_list_r16.to_json(j);
  }
  j.write_str("loggingDuration-r16", logging_dur_r16.to_string());
  j.write_fieldname("reportType");
  report_type.to_json(j);
  if (early_meas_ind_r17_present) {
    j.write_str("earlyMeasIndication-r17", "true");
  }
  if (area_cfg_v1700_present) {
    j.write_fieldname("areaConfiguration-v1700");
    area_cfg_v1700.to_json(j);
  }
  j.end_obj();
}

void var_log_meas_cfg_r16_ies_s::report_type_c_::destroy_()
{
  switch (type_) {
    case types::periodical:
      c.destroy<logged_periodical_report_cfg_r16_s>();
      break;
    case types::event_triggered:
      c.destroy<logged_event_trigger_cfg_r16_s>();
      break;
    default:
      break;
  }
}
void var_log_meas_cfg_r16_ies_s::report_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::periodical:
      c.init<logged_periodical_report_cfg_r16_s>();
      break;
    case types::event_triggered:
      c.init<logged_event_trigger_cfg_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "var_log_meas_cfg_r16_ies_s::report_type_c_");
  }
}
var_log_meas_cfg_r16_ies_s::report_type_c_::report_type_c_(const var_log_meas_cfg_r16_ies_s::report_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::periodical:
      c.init(other.c.get<logged_periodical_report_cfg_r16_s>());
      break;
    case types::event_triggered:
      c.init(other.c.get<logged_event_trigger_cfg_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "var_log_meas_cfg_r16_ies_s::report_type_c_");
  }
}
var_log_meas_cfg_r16_ies_s::report_type_c_&
var_log_meas_cfg_r16_ies_s::report_type_c_::operator=(const var_log_meas_cfg_r16_ies_s::report_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::periodical:
      c.set(other.c.get<logged_periodical_report_cfg_r16_s>());
      break;
    case types::event_triggered:
      c.set(other.c.get<logged_event_trigger_cfg_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "var_log_meas_cfg_r16_ies_s::report_type_c_");
  }

  return *this;
}
logged_periodical_report_cfg_r16_s& var_log_meas_cfg_r16_ies_s::report_type_c_::set_periodical()
{
  set(types::periodical);
  return c.get<logged_periodical_report_cfg_r16_s>();
}
logged_event_trigger_cfg_r16_s& var_log_meas_cfg_r16_ies_s::report_type_c_::set_event_triggered()
{
  set(types::event_triggered);
  return c.get<logged_event_trigger_cfg_r16_s>();
}
void var_log_meas_cfg_r16_ies_s::report_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::periodical:
      j.write_fieldname("periodical");
      c.get<logged_periodical_report_cfg_r16_s>().to_json(j);
      break;
    case types::event_triggered:
      j.write_fieldname("eventTriggered");
      c.get<logged_event_trigger_cfg_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "var_log_meas_cfg_r16_ies_s::report_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE var_log_meas_cfg_r16_ies_s::report_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::periodical:
      HANDLE_CODE(c.get<logged_periodical_report_cfg_r16_s>().pack(bref));
      break;
    case types::event_triggered:
      HANDLE_CODE(c.get<logged_event_trigger_cfg_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "var_log_meas_cfg_r16_ies_s::report_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_cfg_r16_ies_s::report_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::periodical:
      HANDLE_CODE(c.get<logged_periodical_report_cfg_r16_s>().unpack(bref));
      break;
    case types::event_triggered:
      HANDLE_CODE(c.get<logged_event_trigger_cfg_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "var_log_meas_cfg_r16_ies_s::report_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* var_log_meas_cfg_r16_ies_s::report_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical", "eventTriggered"};
  return convert_enum_idx(names, 2, value, "var_log_meas_cfg_r16_ies_s::report_type_c_::types");
}

// VarLogMeasReport-r16 ::= SEQUENCE
SRSASN_CODE var_log_meas_report_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(absolute_time_info_r16.pack(bref));
  HANDLE_CODE(trace_ref_r16.pack(bref));
  HANDLE_CODE(trace_recording_session_ref_r16.pack(bref));
  HANDLE_CODE(tce_id_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, log_meas_info_list_r16, 1, 520));
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r16, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_report_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(absolute_time_info_r16.unpack(bref));
  HANDLE_CODE(trace_ref_r16.unpack(bref));
  HANDLE_CODE(trace_recording_session_ref_r16.unpack(bref));
  HANDLE_CODE(tce_id_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(log_meas_info_list_r16, bref, 1, 520));
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r16, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void var_log_meas_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("absoluteTimeInfo-r16", absolute_time_info_r16.to_string());
  j.write_fieldname("traceReference-r16");
  trace_ref_r16.to_json(j);
  j.write_str("traceRecordingSessionRef-r16", trace_recording_session_ref_r16.to_string());
  j.write_str("tce-Id-r16", tce_id_r16.to_string());
  j.start_array("logMeasInfoList-r16");
  for (const auto& e1 : log_meas_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("plmn-IdentityList-r16");
  for (const auto& e1 : plmn_id_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("sigLoggedMeasType-r17", "true");
  j.end_obj();
}

// VarMeasConfig ::= SEQUENCE
SRSASN_CODE var_meas_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_id_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_obj_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(report_cfg_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(quant_cfg_present, 1));
  HANDLE_CODE(bref.pack(s_measure_cfg_present, 1));

  if (meas_id_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_list, 1, 64));
  }
  if (meas_obj_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_list, 1, 64));
  }
  if (report_cfg_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, report_cfg_list, 1, 64));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.pack(bref));
  }
  if (s_measure_cfg_present) {
    HANDLE_CODE(s_measure_cfg.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_cfg_s::unpack(cbit_ref& bref)
{
  bool meas_id_list_present;
  HANDLE_CODE(bref.unpack(meas_id_list_present, 1));
  bool meas_obj_list_present;
  HANDLE_CODE(bref.unpack(meas_obj_list_present, 1));
  bool report_cfg_list_present;
  HANDLE_CODE(bref.unpack(report_cfg_list_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_present, 1));
  HANDLE_CODE(bref.unpack(s_measure_cfg_present, 1));

  if (meas_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_list, bref, 1, 64));
  }
  if (meas_obj_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_list, bref, 1, 64));
  }
  if (report_cfg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(report_cfg_list, bref, 1, 64));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.unpack(bref));
  }
  if (s_measure_cfg_present) {
    HANDLE_CODE(s_measure_cfg.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void var_meas_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_id_list.size() > 0) {
    j.start_array("measIdList");
    for (const auto& e1 : meas_id_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_obj_list.size() > 0) {
    j.start_array("measObjectList");
    for (const auto& e1 : meas_obj_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (report_cfg_list.size() > 0) {
    j.start_array("reportConfigList");
    for (const auto& e1 : report_cfg_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (quant_cfg_present) {
    j.write_fieldname("quantityConfig");
    quant_cfg.to_json(j);
  }
  if (s_measure_cfg_present) {
    j.write_fieldname("s-MeasureConfig");
    s_measure_cfg.to_json(j);
  }
  j.end_obj();
}

void var_meas_cfg_s::s_measure_cfg_c_::destroy_() {}
void var_meas_cfg_s::s_measure_cfg_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
var_meas_cfg_s::s_measure_cfg_c_::s_measure_cfg_c_(const var_meas_cfg_s::s_measure_cfg_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_rsrp:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rsrp:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
  }
}
var_meas_cfg_s::s_measure_cfg_c_&
var_meas_cfg_s::s_measure_cfg_c_::operator=(const var_meas_cfg_s::s_measure_cfg_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_rsrp:
      c.set(other.c.get<uint8_t>());
      break;
    case types::csi_rsrp:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
  }

  return *this;
}
uint8_t& var_meas_cfg_s::s_measure_cfg_c_::set_ssb_rsrp()
{
  set(types::ssb_rsrp);
  return c.get<uint8_t>();
}
uint8_t& var_meas_cfg_s::s_measure_cfg_c_::set_csi_rsrp()
{
  set(types::csi_rsrp);
  return c.get<uint8_t>();
}
void var_meas_cfg_s::s_measure_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_rsrp:
      j.write_int("ssb-RSRP", c.get<uint8_t>());
      break;
    case types::csi_rsrp:
      j.write_int("csi-RSRP", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE var_meas_cfg_s::s_measure_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_rsrp:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::csi_rsrp:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_cfg_s::s_measure_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_rsrp:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::csi_rsrp:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* var_meas_cfg_s::s_measure_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-RSRP", "csi-RSRP"};
  return convert_enum_idx(names, 2, value, "var_meas_cfg_s::s_measure_cfg_c_::types");
}

// VarMeasConfigSL-r16 ::= SEQUENCE
SRSASN_CODE var_meas_cfg_sl_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_meas_id_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_obj_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_report_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_quant_cfg_r16_present, 1));

  if (sl_meas_id_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_id_list_r16, 1, 64));
  }
  if (sl_meas_obj_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_obj_list_r16, 1, 64));
  }
  if (sl_report_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_report_cfg_list_r16, 1, 64));
  }
  if (sl_quant_cfg_r16_present) {
    HANDLE_CODE(sl_quant_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_cfg_sl_r16_s::unpack(cbit_ref& bref)
{
  bool sl_meas_id_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_id_list_r16_present, 1));
  bool sl_meas_obj_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_obj_list_r16_present, 1));
  bool sl_report_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_report_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_quant_cfg_r16_present, 1));

  if (sl_meas_id_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_id_list_r16, bref, 1, 64));
  }
  if (sl_meas_obj_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_obj_list_r16, bref, 1, 64));
  }
  if (sl_report_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_report_cfg_list_r16, bref, 1, 64));
  }
  if (sl_quant_cfg_r16_present) {
    HANDLE_CODE(sl_quant_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void var_meas_cfg_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_meas_id_list_r16.size() > 0) {
    j.start_array("sl-MeasIdList-r16");
    for (const auto& e1 : sl_meas_id_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_meas_obj_list_r16.size() > 0) {
    j.start_array("sl-MeasObjectList-r16");
    for (const auto& e1 : sl_meas_obj_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_report_cfg_list_r16.size() > 0) {
    j.start_array("sl-reportConfigList-r16");
    for (const auto& e1 : sl_report_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_quant_cfg_r16_present) {
    j.write_fieldname("sl-QuantityConfig-r16");
    sl_quant_cfg_r16.to_json(j);
  }
  j.end_obj();
}

// VarMeasIdleConfig-r16 ::= SEQUENCE
SRSASN_CODE var_meas_idle_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(validity_area_list_r16.size() > 0, 1));

  if (meas_idle_carrier_list_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_nr_r16, 1, 8));
  }
  if (meas_idle_carrier_list_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_eutra_r16, 1, 8));
  }
  HANDLE_CODE(meas_idle_dur_r16.pack(bref));
  if (validity_area_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, validity_area_list_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_idle_cfg_r16_s::unpack(cbit_ref& bref)
{
  bool meas_idle_carrier_list_nr_r16_present;
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_nr_r16_present, 1));
  bool meas_idle_carrier_list_eutra_r16_present;
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_eutra_r16_present, 1));
  bool validity_area_list_r16_present;
  HANDLE_CODE(bref.unpack(validity_area_list_r16_present, 1));

  if (meas_idle_carrier_list_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_nr_r16, bref, 1, 8));
  }
  if (meas_idle_carrier_list_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_eutra_r16, bref, 1, 8));
  }
  HANDLE_CODE(meas_idle_dur_r16.unpack(bref));
  if (validity_area_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(validity_area_list_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void var_meas_idle_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_idle_carrier_list_nr_r16.size() > 0) {
    j.start_array("measIdleCarrierListNR-r16");
    for (const auto& e1 : meas_idle_carrier_list_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_idle_carrier_list_eutra_r16.size() > 0) {
    j.start_array("measIdleCarrierListEUTRA-r16");
    for (const auto& e1 : meas_idle_carrier_list_eutra_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("measIdleDuration-r16", meas_idle_dur_r16.to_string());
  if (validity_area_list_r16.size() > 0) {
    j.start_array("validityAreaList-r16");
    for (const auto& e1 : validity_area_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* var_meas_idle_cfg_r16_s::meas_idle_dur_r16_opts::to_string() const
{
  static const char* names[] = {"sec10", "sec30", "sec60", "sec120", "sec180", "sec240", "sec300", "spare"};
  return convert_enum_idx(names, 8, value, "var_meas_idle_cfg_r16_s::meas_idle_dur_r16_e_");
}
uint16_t var_meas_idle_cfg_r16_s::meas_idle_dur_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 30, 60, 120, 180, 240, 300};
  return map_enum_number(numbers, 7, value, "var_meas_idle_cfg_r16_s::meas_idle_dur_r16_e_");
}

// VarMeasIdleReport-r16 ::= SEQUENCE
SRSASN_CODE var_meas_idle_report_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_report_idle_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_report_idle_eutra_r16_present, 1));

  if (meas_report_idle_nr_r16_present) {
    HANDLE_CODE(meas_report_idle_nr_r16.pack(bref));
  }
  if (meas_report_idle_eutra_r16_present) {
    HANDLE_CODE(meas_report_idle_eutra_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_idle_report_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_report_idle_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_report_idle_eutra_r16_present, 1));

  if (meas_report_idle_nr_r16_present) {
    HANDLE_CODE(meas_report_idle_nr_r16.unpack(bref));
  }
  if (meas_report_idle_eutra_r16_present) {
    HANDLE_CODE(meas_report_idle_eutra_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void var_meas_idle_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_report_idle_nr_r16_present) {
    j.write_fieldname("measReportIdleNR-r16");
    meas_report_idle_nr_r16.to_json(j);
  }
  if (meas_report_idle_eutra_r16_present) {
    j.write_fieldname("measReportIdleEUTRA-r16");
    meas_report_idle_eutra_r16.to_json(j);
  }
  j.end_obj();
}

// VarMeasReport ::= SEQUENCE
SRSASN_CODE var_meas_report_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cells_triggered_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(cli_triggered_list_r16_present, 1));
  HANDLE_CODE(bref.pack(tx_pool_meas_to_add_mod_list_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(relays_triggered_list_r17.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)64u));
  if (cells_triggered_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_triggered_list, 1, 32));
  }
  HANDLE_CODE(pack_unconstrained_integer(bref, nof_reports_sent));
  if (cli_triggered_list_r16_present) {
    HANDLE_CODE(cli_triggered_list_r16.pack(bref));
  }
  if (tx_pool_meas_to_add_mod_list_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tx_pool_meas_to_add_mod_list_nr_r16, 1, 8, integer_packer<uint8_t>(1, 16)));
  }
  if (relays_triggered_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, relays_triggered_list_r17, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_report_s::unpack(cbit_ref& bref)
{
  bool cells_triggered_list_present;
  HANDLE_CODE(bref.unpack(cells_triggered_list_present, 1));
  HANDLE_CODE(bref.unpack(cli_triggered_list_r16_present, 1));
  bool tx_pool_meas_to_add_mod_list_nr_r16_present;
  HANDLE_CODE(bref.unpack(tx_pool_meas_to_add_mod_list_nr_r16_present, 1));
  bool relays_triggered_list_r17_present;
  HANDLE_CODE(bref.unpack(relays_triggered_list_r17_present, 1));

  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)64u));
  if (cells_triggered_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_triggered_list, bref, 1, 32));
  }
  HANDLE_CODE(unpack_unconstrained_integer(nof_reports_sent, bref));
  if (cli_triggered_list_r16_present) {
    HANDLE_CODE(cli_triggered_list_r16.unpack(bref));
  }
  if (tx_pool_meas_to_add_mod_list_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tx_pool_meas_to_add_mod_list_nr_r16, bref, 1, 8, integer_packer<uint8_t>(1, 16)));
  }
  if (relays_triggered_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(relays_triggered_list_r17, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void var_meas_report_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  if (cells_triggered_list.size() > 0) {
    j.start_array("cellsTriggeredList");
    for (const auto& e1 : cells_triggered_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("numberOfReportsSent", nof_reports_sent);
  if (cli_triggered_list_r16_present) {
    j.write_fieldname("cli-TriggeredList-r16");
    cli_triggered_list_r16.to_json(j);
  }
  if (tx_pool_meas_to_add_mod_list_nr_r16.size() > 0) {
    j.start_array("tx-PoolMeasToAddModListNR-r16");
    for (const auto& e1 : tx_pool_meas_to_add_mod_list_nr_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (relays_triggered_list_r17.size() > 0) {
    j.start_array("relaysTriggeredList-r17");
    for (const auto& e1 : relays_triggered_list_r17) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// VarMeasReportSL-r16 ::= SEQUENCE
SRSASN_CODE var_meas_report_sl_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_freq_triggered_list_r16.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, sl_meas_id_r16, (uint8_t)1u, (uint8_t)64u));
  if (sl_freq_triggered_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_freq_triggered_list_r16, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  HANDLE_CODE(pack_unconstrained_integer(bref, sl_nof_reports_sent_r16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_report_sl_r16_s::unpack(cbit_ref& bref)
{
  bool sl_freq_triggered_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_freq_triggered_list_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_meas_id_r16, bref, (uint8_t)1u, (uint8_t)64u));
  if (sl_freq_triggered_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_freq_triggered_list_r16, bref, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  HANDLE_CODE(unpack_unconstrained_integer(sl_nof_reports_sent_r16, bref));

  return SRSASN_SUCCESS;
}
void var_meas_report_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-MeasId-r16", sl_meas_id_r16);
  if (sl_freq_triggered_list_r16.size() > 0) {
    j.start_array("sl-FrequencyTriggeredList-r16");
    for (const auto& e1 : sl_freq_triggered_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_int("sl-NumberOfReportsSent-r16", sl_nof_reports_sent_r16);
  j.end_obj();
}

// VarMobilityHistoryReport-r17 ::= SEQUENCE
SRSASN_CODE var_mob_history_report_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(visited_pscell_info_list_report_r17.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, visited_cell_info_list_r16, 1, 16));
  if (visited_pscell_info_list_report_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, visited_pscell_info_list_report_r17, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_mob_history_report_r17_s::unpack(cbit_ref& bref)
{
  bool visited_pscell_info_list_report_r17_present;
  HANDLE_CODE(bref.unpack(visited_pscell_info_list_report_r17_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(visited_cell_info_list_r16, bref, 1, 16));
  if (visited_pscell_info_list_report_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(visited_pscell_info_list_report_r17, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void var_mob_history_report_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("visitedCellInfoList-r16");
  for (const auto& e1 : visited_cell_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (visited_pscell_info_list_report_r17.size() > 0) {
    j.start_array("visitedPSCellInfoListReport-r17");
    for (const auto& e1 : visited_pscell_info_list_report_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// VarRA-Report-r16 ::= SEQUENCE
SRSASN_CODE var_ra_report_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, ra_report_list_r16, 1, 8));
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r16, 1, 12));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_ra_report_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(ra_report_list_r16, bref, 1, 8));
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r16, bref, 1, 12));

  return SRSASN_SUCCESS;
}
void var_ra_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ra-ReportList-r16");
  for (const auto& e1 : ra_report_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("plmn-IdentityList-r16");
  for (const auto& e1 : plmn_id_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// VarRLF-Report-r16 ::= SEQUENCE
SRSASN_CODE var_rlf_report_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rlf_report_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r16, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_rlf_report_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rlf_report_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r16, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void var_rlf_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rlf-Report-r16");
  rlf_report_r16.to_json(j);
  j.start_array("plmn-IdentityList-r16");
  for (const auto& e1 : plmn_id_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// VarResumeMAC-Input ::= SEQUENCE
SRSASN_CODE var_resume_mac_input_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, source_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, source_c_rnti, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_resume_mac_input_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(source_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.unpack(bref));
  HANDLE_CODE(unpack_integer(source_c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
void var_resume_mac_input_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId", source_pci);
  j.write_str("targetCellIdentity", target_cell_id.to_string());
  j.write_int("source-c-RNTI", source_c_rnti);
  j.end_obj();
}

// VarShortMAC-Input ::= SEQUENCE
SRSASN_CODE var_short_mac_input_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, source_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, source_c_rnti, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_short_mac_input_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(source_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.unpack(bref));
  HANDLE_CODE(unpack_integer(source_c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
void var_short_mac_input_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId", source_pci);
  j.write_str("targetCellIdentity", target_cell_id.to_string());
  j.write_int("source-c-RNTI", source_c_rnti);
  j.end_obj();
}

// VarSuccessHO-Report-r17-IEs ::= SEQUENCE
SRSASN_CODE var_success_ho_report_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(success_ho_report_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r17, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_success_ho_report_r17_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(success_ho_report_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r17, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void var_success_ho_report_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("successHO-Report-r17");
  success_ho_report_r17.to_json(j);
  j.start_array("plmn-IdentityList-r17");
  for (const auto& e1 : plmn_id_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

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

#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PLMN-Identity-EUTRA-5GC ::= CHOICE
void plmn_id_eutra_5_gc_c::destroy_()
{
  switch (type_) {
    case types::plmn_id_eutra_5_gc:
      c.destroy<plmn_id_s>();
      break;
    default:
      break;
  }
}
void plmn_id_eutra_5_gc_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_id_eutra_5_gc:
      c.init<plmn_id_s>();
      break;
    case types::plmn_idx:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_5_gc_c");
  }
}
plmn_id_eutra_5_gc_c::plmn_id_eutra_5_gc_c(const plmn_id_eutra_5_gc_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_id_eutra_5_gc:
      c.init(other.c.get<plmn_id_s>());
      break;
    case types::plmn_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_5_gc_c");
  }
}
plmn_id_eutra_5_gc_c& plmn_id_eutra_5_gc_c::operator=(const plmn_id_eutra_5_gc_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_id_eutra_5_gc:
      c.set(other.c.get<plmn_id_s>());
      break;
    case types::plmn_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_5_gc_c");
  }

  return *this;
}
plmn_id_s& plmn_id_eutra_5_gc_c::set_plmn_id_eutra_5_gc()
{
  set(types::plmn_id_eutra_5_gc);
  return c.get<plmn_id_s>();
}
uint8_t& plmn_id_eutra_5_gc_c::set_plmn_idx()
{
  set(types::plmn_idx);
  return c.get<uint8_t>();
}
void plmn_id_eutra_5_gc_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_id_eutra_5_gc:
      j.write_fieldname("plmn-Identity-EUTRA-5GC");
      c.get<plmn_id_s>().to_json(j);
      break;
    case types::plmn_idx:
      j.write_int("plmn-index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_5_gc_c");
  }
  j.end_obj();
}
SRSASN_CODE plmn_id_eutra_5_gc_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_id_eutra_5_gc:
      HANDLE_CODE(c.get<plmn_id_s>().pack(bref));
      break;
    case types::plmn_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_5_gc_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_eutra_5_gc_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_id_eutra_5_gc:
      HANDLE_CODE(c.get<plmn_id_s>().unpack(bref));
      break;
    case types::plmn_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_5_gc_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* plmn_id_eutra_5_gc_c::types_opts::to_string() const
{
  static const char* names[] = {"plmn-Identity-EUTRA-5GC", "plmn-index"};
  return convert_enum_idx(names, 2, value, "plmn_id_eutra_5_gc_c::types");
}
int8_t plmn_id_eutra_5_gc_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-5};
  return map_enum_number(numbers, 1, value, "plmn_id_eutra_5_gc_c::types");
}

// CellIdentity-EUTRA-5GC ::= CHOICE
void cell_id_eutra_5_gc_c::destroy_()
{
  switch (type_) {
    case types::cell_id_eutra:
      c.destroy<fixed_bitstring<28>>();
      break;
    default:
      break;
  }
}
void cell_id_eutra_5_gc_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_id_eutra:
      c.init<fixed_bitstring<28>>();
      break;
    case types::cell_id_idx:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_5_gc_c");
  }
}
cell_id_eutra_5_gc_c::cell_id_eutra_5_gc_c(const cell_id_eutra_5_gc_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_id_eutra:
      c.init(other.c.get<fixed_bitstring<28>>());
      break;
    case types::cell_id_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_5_gc_c");
  }
}
cell_id_eutra_5_gc_c& cell_id_eutra_5_gc_c::operator=(const cell_id_eutra_5_gc_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_id_eutra:
      c.set(other.c.get<fixed_bitstring<28>>());
      break;
    case types::cell_id_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_5_gc_c");
  }

  return *this;
}
fixed_bitstring<28>& cell_id_eutra_5_gc_c::set_cell_id_eutra()
{
  set(types::cell_id_eutra);
  return c.get<fixed_bitstring<28>>();
}
uint8_t& cell_id_eutra_5_gc_c::set_cell_id_idx()
{
  set(types::cell_id_idx);
  return c.get<uint8_t>();
}
void cell_id_eutra_5_gc_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_id_eutra:
      j.write_str("cellIdentity-EUTRA", c.get<fixed_bitstring<28>>().to_string());
      break;
    case types::cell_id_idx:
      j.write_int("cellId-index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_5_gc_c");
  }
  j.end_obj();
}
SRSASN_CODE cell_id_eutra_5_gc_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_id_eutra:
      HANDLE_CODE(c.get<fixed_bitstring<28>>().pack(bref));
      break;
    case types::cell_id_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_5_gc_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_id_eutra_5_gc_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_id_eutra:
      HANDLE_CODE(c.get<fixed_bitstring<28>>().unpack(bref));
      break;
    case types::cell_id_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_5_gc_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cell_id_eutra_5_gc_c::types_opts::to_string() const
{
  static const char* names[] = {"cellIdentity-EUTRA", "cellId-index"};
  return convert_enum_idx(names, 2, value, "cell_id_eutra_5_gc_c::types");
}

// FrequencyComponent-r17 ::= ENUMERATED
const char* freq_component_r17_opts::to_string() const
{
  static const char* names[] = {"activeCarrier", "configuredCarrier", "activeBWP", "configuredBWP"};
  return convert_enum_idx(names, 4, value, "freq_component_r17_e");
}

// MeasQuantityResults ::= SEQUENCE
SRSASN_CODE meas_quant_results_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_present, 1));
  HANDLE_CODE(bref.pack(rsrq_present, 1));
  HANDLE_CODE(bref.pack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(pack_integer(bref, rsrp, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrq_present) {
    HANDLE_CODE(pack_integer(bref, rsrq, (uint8_t)0u, (uint8_t)127u));
  }
  if (sinr_present) {
    HANDLE_CODE(pack_integer(bref, sinr, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_quant_results_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_present, 1));
  HANDLE_CODE(bref.unpack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(unpack_integer(rsrp, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrq_present) {
    HANDLE_CODE(unpack_integer(rsrq, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (sinr_present) {
    HANDLE_CODE(unpack_integer(sinr, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_quant_results_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_present) {
    j.write_int("rsrp", rsrp);
  }
  if (rsrq_present) {
    j.write_int("rsrq", rsrq);
  }
  if (sinr_present) {
    j.write_int("sinr", sinr);
  }
  j.end_obj();
}

// CellAccessRelatedInfo-EUTRA-5GC ::= SEQUENCE
SRSASN_CODE cell_access_related_info_eutra_5_gc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ranac_5gc_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_eutra_5gc, 1, 12));
  HANDLE_CODE(tac_eutra_5gc.pack(bref));
  if (ranac_5gc_present) {
    HANDLE_CODE(pack_integer(bref, ranac_5gc, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_eutra_5gc.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_eutra_5_gc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ranac_5gc_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_eutra_5gc, bref, 1, 12));
  HANDLE_CODE(tac_eutra_5gc.unpack(bref));
  if (ranac_5gc_present) {
    HANDLE_CODE(unpack_integer(ranac_5gc, bref, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_eutra_5gc.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_access_related_info_eutra_5_gc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList-eutra-5gc");
  for (const auto& e1 : plmn_id_list_eutra_5gc) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-eutra-5gc", tac_eutra_5gc.to_string());
  if (ranac_5gc_present) {
    j.write_int("ranac-5gc", ranac_5gc);
  }
  j.write_fieldname("cellIdentity-eutra-5gc");
  cell_id_eutra_5gc.to_json(j);
  j.end_obj();
}

// CellAccessRelatedInfo-EUTRA-EPC ::= SEQUENCE
SRSASN_CODE cell_access_related_info_eutra_epc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_eutra_epc, 1, 12));
  HANDLE_CODE(tac_eutra_epc.pack(bref));
  HANDLE_CODE(cell_id_eutra_epc.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_eutra_epc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_eutra_epc, bref, 1, 12));
  HANDLE_CODE(tac_eutra_epc.unpack(bref));
  HANDLE_CODE(cell_id_eutra_epc.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_access_related_info_eutra_epc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList-eutra-epc");
  for (const auto& e1 : plmn_id_list_eutra_epc) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-eutra-epc", tac_eutra_epc.to_string());
  j.write_str("cellIdentity-eutra-epc", cell_id_eutra_epc.to_string());
  j.end_obj();
}

// DefaultDC-Location-r17 ::= CHOICE
void default_dc_location_r17_c::destroy_() {}
void default_dc_location_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
default_dc_location_r17_c::default_dc_location_r17_c(const default_dc_location_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ul:
      c.init(other.c.get<freq_component_r17_e>());
      break;
    case types::dl:
      c.init(other.c.get<freq_component_r17_e>());
      break;
    case types::ul_and_dl:
      c.init(other.c.get<freq_component_r17_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "default_dc_location_r17_c");
  }
}
default_dc_location_r17_c& default_dc_location_r17_c::operator=(const default_dc_location_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ul:
      c.set(other.c.get<freq_component_r17_e>());
      break;
    case types::dl:
      c.set(other.c.get<freq_component_r17_e>());
      break;
    case types::ul_and_dl:
      c.set(other.c.get<freq_component_r17_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "default_dc_location_r17_c");
  }

  return *this;
}
freq_component_r17_e& default_dc_location_r17_c::set_ul()
{
  set(types::ul);
  return c.get<freq_component_r17_e>();
}
freq_component_r17_e& default_dc_location_r17_c::set_dl()
{
  set(types::dl);
  return c.get<freq_component_r17_e>();
}
freq_component_r17_e& default_dc_location_r17_c::set_ul_and_dl()
{
  set(types::ul_and_dl);
  return c.get<freq_component_r17_e>();
}
void default_dc_location_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul:
      j.write_str("ul", c.get<freq_component_r17_e>().to_string());
      break;
    case types::dl:
      j.write_str("dl", c.get<freq_component_r17_e>().to_string());
      break;
    case types::ul_and_dl:
      j.write_str("ulAndDL", c.get<freq_component_r17_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "default_dc_location_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE default_dc_location_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul:
      HANDLE_CODE(c.get<freq_component_r17_e>().pack(bref));
      break;
    case types::dl:
      HANDLE_CODE(c.get<freq_component_r17_e>().pack(bref));
      break;
    case types::ul_and_dl:
      HANDLE_CODE(c.get<freq_component_r17_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "default_dc_location_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE default_dc_location_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul:
      HANDLE_CODE(c.get<freq_component_r17_e>().unpack(bref));
      break;
    case types::dl:
      HANDLE_CODE(c.get<freq_component_r17_e>().unpack(bref));
      break;
    case types::ul_and_dl:
      HANDLE_CODE(c.get<freq_component_r17_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "default_dc_location_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* default_dc_location_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"ul", "dl", "ulAndDL"};
  return convert_enum_idx(names, 3, value, "default_dc_location_r17_c::types");
}

// OffsetValue-r17 ::= SEQUENCE
SRSASN_CODE offset_value_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, offset_value_r17, (int16_t)-20000, (int16_t)20000));
  HANDLE_CODE(bref.pack(shift7dot5k_hz_r17, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE offset_value_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(offset_value_r17, bref, (int16_t)-20000, (int16_t)20000));
  HANDLE_CODE(bref.unpack(shift7dot5k_hz_r17, 1));

  return SRSASN_SUCCESS;
}
void offset_value_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("offsetValue-r17", offset_value_r17);
  j.write_bool("shift7dot5kHz-r17", shift7dot5k_hz_r17);
  j.end_obj();
}

// ResultsPerCSI-RS-Index ::= SEQUENCE
SRSASN_CODE results_per_csi_rs_idx_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_results_present, 1));

  HANDLE_CODE(pack_integer(bref, csi_rs_idx, (uint8_t)0u, (uint8_t)95u));
  if (csi_rs_results_present) {
    HANDLE_CODE(csi_rs_results.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE results_per_csi_rs_idx_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_results_present, 1));

  HANDLE_CODE(unpack_integer(csi_rs_idx, bref, (uint8_t)0u, (uint8_t)95u));
  if (csi_rs_results_present) {
    HANDLE_CODE(csi_rs_results.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void results_per_csi_rs_idx_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS-Index", csi_rs_idx);
  if (csi_rs_results_present) {
    j.write_fieldname("csi-RS-Results");
    csi_rs_results.to_json(j);
  }
  j.end_obj();
}

// ResultsPerSSB-Index ::= SEQUENCE
SRSASN_CODE results_per_ssb_idx_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_results_present, 1));

  HANDLE_CODE(pack_integer(bref, ssb_idx, (uint8_t)0u, (uint8_t)63u));
  if (ssb_results_present) {
    HANDLE_CODE(ssb_results.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE results_per_ssb_idx_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_results_present, 1));

  HANDLE_CODE(unpack_integer(ssb_idx, bref, (uint8_t)0u, (uint8_t)63u));
  if (ssb_results_present) {
    HANDLE_CODE(ssb_results.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void results_per_ssb_idx_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb-Index", ssb_idx);
  if (ssb_results_present) {
    j.write_fieldname("ssb-Results");
    ssb_results.to_json(j);
  }
  j.end_obj();
}

// CC-Group-r17 ::= SEQUENCE
SRSASN_CODE cc_group_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_cell_idx_higher_r17_present, 1));
  HANDLE_CODE(bref.pack(offset_to_default_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_idx_lower_r17, (uint8_t)0u, (uint8_t)31u));
  if (serv_cell_idx_higher_r17_present) {
    HANDLE_CODE(pack_integer(bref, serv_cell_idx_higher_r17, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(default_dc_location_r17.pack(bref));
  if (offset_to_default_r17_present) {
    HANDLE_CODE(offset_to_default_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cc_group_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(serv_cell_idx_higher_r17_present, 1));
  HANDLE_CODE(bref.unpack(offset_to_default_r17_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_idx_lower_r17, bref, (uint8_t)0u, (uint8_t)31u));
  if (serv_cell_idx_higher_r17_present) {
    HANDLE_CODE(unpack_integer(serv_cell_idx_higher_r17, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(default_dc_location_r17.unpack(bref));
  if (offset_to_default_r17_present) {
    HANDLE_CODE(offset_to_default_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cc_group_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndexLower-r17", serv_cell_idx_lower_r17);
  if (serv_cell_idx_higher_r17_present) {
    j.write_int("servCellIndexHigher-r17", serv_cell_idx_higher_r17);
  }
  j.write_fieldname("defaultDC-Location-r17");
  default_dc_location_r17.to_json(j);
  if (offset_to_default_r17_present) {
    j.write_fieldname("offsetToDefault-r17");
    offset_to_default_r17.to_json(j);
  }
  j.end_obj();
}

void cc_group_r17_s::offset_to_default_r17_c_::destroy_()
{
  switch (type_) {
    case types::offset_value:
      c.destroy<offset_value_r17_s>();
      break;
    case types::offsetlist:
      c.destroy<offsetlist_l_>();
      break;
    default:
      break;
  }
}
void cc_group_r17_s::offset_to_default_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::offset_value:
      c.init<offset_value_r17_s>();
      break;
    case types::offsetlist:
      c.init<offsetlist_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cc_group_r17_s::offset_to_default_r17_c_");
  }
}
cc_group_r17_s::offset_to_default_r17_c_::offset_to_default_r17_c_(
    const cc_group_r17_s::offset_to_default_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::offset_value:
      c.init(other.c.get<offset_value_r17_s>());
      break;
    case types::offsetlist:
      c.init(other.c.get<offsetlist_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cc_group_r17_s::offset_to_default_r17_c_");
  }
}
cc_group_r17_s::offset_to_default_r17_c_&
cc_group_r17_s::offset_to_default_r17_c_::operator=(const cc_group_r17_s::offset_to_default_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::offset_value:
      c.set(other.c.get<offset_value_r17_s>());
      break;
    case types::offsetlist:
      c.set(other.c.get<offsetlist_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cc_group_r17_s::offset_to_default_r17_c_");
  }

  return *this;
}
offset_value_r17_s& cc_group_r17_s::offset_to_default_r17_c_::set_offset_value()
{
  set(types::offset_value);
  return c.get<offset_value_r17_s>();
}
cc_group_r17_s::offset_to_default_r17_c_::offsetlist_l_& cc_group_r17_s::offset_to_default_r17_c_::set_offsetlist()
{
  set(types::offsetlist);
  return c.get<offsetlist_l_>();
}
void cc_group_r17_s::offset_to_default_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::offset_value:
      j.write_fieldname("offsetValue");
      c.get<offset_value_r17_s>().to_json(j);
      break;
    case types::offsetlist:
      j.start_array("offsetlist");
      for (const auto& e1 : c.get<offsetlist_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "cc_group_r17_s::offset_to_default_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE cc_group_r17_s::offset_to_default_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::offset_value:
      HANDLE_CODE(c.get<offset_value_r17_s>().pack(bref));
      break;
    case types::offsetlist:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<offsetlist_l_>(), 1, 128));
      break;
    default:
      log_invalid_choice_id(type_, "cc_group_r17_s::offset_to_default_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cc_group_r17_s::offset_to_default_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::offset_value:
      HANDLE_CODE(c.get<offset_value_r17_s>().unpack(bref));
      break;
    case types::offsetlist:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<offsetlist_l_>(), bref, 1, 128));
      break;
    default:
      log_invalid_choice_id(type_, "cc_group_r17_s::offset_to_default_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cc_group_r17_s::offset_to_default_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"offsetValue", "offsetlist"};
  return convert_enum_idx(names, 2, value, "cc_group_r17_s::offset_to_default_r17_c_::types");
}

// CGI-InfoEUTRA ::= SEQUENCE
SRSASN_CODE cgi_info_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cgi_info_epc_present, 1));
  HANDLE_CODE(bref.pack(cgi_info_5_gc.size() > 0, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_ind_prio_present, 1));

  if (cgi_info_epc_present) {
    HANDLE_CODE(bref.pack(cgi_info_epc.cgi_info_epc_list.size() > 0, 1));
    HANDLE_CODE(cgi_info_epc.cgi_info_epc_legacy.pack(bref));
    if (cgi_info_epc.cgi_info_epc_list.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, cgi_info_epc.cgi_info_epc_list, 1, 12));
    }
  }
  if (cgi_info_5_gc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cgi_info_5_gc, 1, 12));
  }
  HANDLE_CODE(pack_integer(bref, freq_band_ind, (uint16_t)1u, (uint16_t)256u));
  if (multi_band_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_info_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cgi_info_epc_present, 1));
  bool cgi_info_5_gc_present;
  HANDLE_CODE(bref.unpack(cgi_info_5_gc_present, 1));
  bool multi_band_info_list_present;
  HANDLE_CODE(bref.unpack(multi_band_info_list_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_ind_prio_present, 1));

  if (cgi_info_epc_present) {
    bool cgi_info_epc_list_present;
    HANDLE_CODE(bref.unpack(cgi_info_epc_list_present, 1));
    HANDLE_CODE(cgi_info_epc.cgi_info_epc_legacy.unpack(bref));
    if (cgi_info_epc_list_present) {
      HANDLE_CODE(unpack_dyn_seq_of(cgi_info_epc.cgi_info_epc_list, bref, 1, 12));
    }
  }
  if (cgi_info_5_gc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cgi_info_5_gc, bref, 1, 12));
  }
  HANDLE_CODE(unpack_integer(freq_band_ind, bref, (uint16_t)1u, (uint16_t)256u));
  if (multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list, bref, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
void cgi_info_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cgi_info_epc_present) {
    j.write_fieldname("cgi-info-EPC");
    j.start_obj();
    j.write_fieldname("cgi-info-EPC-legacy");
    cgi_info_epc.cgi_info_epc_legacy.to_json(j);
    if (cgi_info_epc.cgi_info_epc_list.size() > 0) {
      j.start_array("cgi-info-EPC-list");
      for (const auto& e1 : cgi_info_epc.cgi_info_epc_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (cgi_info_5_gc.size() > 0) {
    j.start_array("cgi-info-5GC");
    for (const auto& e1 : cgi_info_5_gc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("freqBandIndicator", freq_band_ind);
  if (multi_band_info_list.size() > 0) {
    j.start_array("multiBandInfoList");
    for (const auto& e1 : multi_band_info_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (freq_band_ind_prio_present) {
    j.write_str("freqBandIndicatorPriority", "true");
  }
  j.end_obj();
}

// CGI-InfoNR ::= SEQUENCE
SRSASN_CODE cgi_info_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(plmn_id_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(no_sib1_present, 1));

  if (plmn_id_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_info_list, 1, 12));
  }
  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (no_sib1_present) {
    HANDLE_CODE(pack_integer(bref, no_sib1.ssb_subcarrier_offset, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(no_sib1.pdcch_cfg_sib1.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= npn_id_info_list_r16.is_present();
    group_flags[1] |= cell_reserved_for_other_use_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(npn_id_info_list_r16.is_present(), 1));
      if (npn_id_info_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *npn_id_info_list_r16, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_reserved_for_other_use_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_info_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool plmn_id_info_list_present;
  HANDLE_CODE(bref.unpack(plmn_id_info_list_present, 1));
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  HANDLE_CODE(bref.unpack(no_sib1_present, 1));

  if (plmn_id_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_info_list, bref, 1, 12));
  }
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (no_sib1_present) {
    HANDLE_CODE(unpack_integer(no_sib1.ssb_subcarrier_offset, bref, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(no_sib1.pdcch_cfg_sib1.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(npn_id_info_list_r16, bref);
      if (npn_id_info_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*npn_id_info_list_r16, bref, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cell_reserved_for_other_use_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void cgi_info_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_info_list.size() > 0) {
    j.start_array("plmn-IdentityInfoList");
    for (const auto& e1 : plmn_id_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (no_sib1_present) {
    j.write_fieldname("noSIB1");
    j.start_obj();
    j.write_int("ssb-SubcarrierOffset", no_sib1.ssb_subcarrier_offset);
    j.write_fieldname("pdcch-ConfigSIB1");
    no_sib1.pdcch_cfg_sib1.to_json(j);
    j.end_obj();
  }
  if (ext) {
    if (npn_id_info_list_r16.is_present()) {
      j.start_array("npn-IdentityInfoList-r16");
      for (const auto& e1 : *npn_id_info_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (cell_reserved_for_other_use_r16_present) {
      j.write_str("cellReservedForOtherUse-r16", "true");
    }
  }
  j.end_obj();
}

// MeasQuantityResultsEUTRA ::= SEQUENCE
SRSASN_CODE meas_quant_results_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_present, 1));
  HANDLE_CODE(bref.pack(rsrq_present, 1));
  HANDLE_CODE(bref.pack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(pack_integer(bref, rsrp, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_present) {
    HANDLE_CODE(pack_integer(bref, rsrq, (uint8_t)0u, (uint8_t)34u));
  }
  if (sinr_present) {
    HANDLE_CODE(pack_integer(bref, sinr, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_quant_results_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_present, 1));
  HANDLE_CODE(bref.unpack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(unpack_integer(rsrp, bref, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_present) {
    HANDLE_CODE(unpack_integer(rsrq, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (sinr_present) {
    HANDLE_CODE(unpack_integer(sinr, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_quant_results_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_present) {
    j.write_int("rsrp", rsrp);
  }
  if (rsrq_present) {
    j.write_int("rsrq", rsrq);
  }
  if (sinr_present) {
    j.write_int("sinr", sinr);
  }
  j.end_obj();
}

// NeedForNCSG-IntraFreq-r17 ::= SEQUENCE
SRSASN_CODE need_for_ncsg_intra_freq_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_id_r17, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(gap_ind_intra_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_ncsg_intra_freq_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_id_r17, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(gap_ind_intra_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void need_for_ncsg_intra_freq_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId-r17", serv_cell_id_r17);
  j.write_str("gapIndicationIntra-r17", gap_ind_intra_r17.to_string());
  j.end_obj();
}

const char* need_for_ncsg_intra_freq_r17_s::gap_ind_intra_r17_opts::to_string() const
{
  static const char* names[] = {"gap", "ncsg", "nogap-noncsg"};
  return convert_enum_idx(names, 3, value, "need_for_ncsg_intra_freq_r17_s::gap_ind_intra_r17_e_");
}

// NeedForNCSG-NR-r17 ::= SEQUENCE
SRSASN_CODE need_for_ncsg_nr_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_nr_r17, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(gap_ind_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_ncsg_nr_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_nr_r17, bref, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(gap_ind_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void need_for_ncsg_nr_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandNR-r17", band_nr_r17);
  j.write_str("gapIndication-r17", gap_ind_r17.to_string());
  j.end_obj();
}

const char* need_for_ncsg_nr_r17_s::gap_ind_r17_opts::to_string() const
{
  static const char* names[] = {"gap", "ncsg", "nogap-noncsg"};
  return convert_enum_idx(names, 3, value, "need_for_ncsg_nr_r17_s::gap_ind_r17_e_");
}

// PerRAAttemptInfo-r16 ::= SEQUENCE
SRSASN_CODE per_ra_attempt_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(contention_detected_r16_present, 1));
  HANDLE_CODE(bref.pack(dl_rsrp_above_thres_r16_present, 1));

  if (contention_detected_r16_present) {
    HANDLE_CODE(bref.pack(contention_detected_r16, 1));
  }
  if (dl_rsrp_above_thres_r16_present) {
    HANDLE_CODE(bref.pack(dl_rsrp_above_thres_r16, 1));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= fallback_to_four_step_ra_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(fallback_to_four_step_ra_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE per_ra_attempt_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(contention_detected_r16_present, 1));
  HANDLE_CODE(bref.unpack(dl_rsrp_above_thres_r16_present, 1));

  if (contention_detected_r16_present) {
    HANDLE_CODE(bref.unpack(contention_detected_r16, 1));
  }
  if (dl_rsrp_above_thres_r16_present) {
    HANDLE_CODE(bref.unpack(dl_rsrp_above_thres_r16, 1));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(fallback_to_four_step_ra_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void per_ra_attempt_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (contention_detected_r16_present) {
    j.write_bool("contentionDetected-r16", contention_detected_r16);
  }
  if (dl_rsrp_above_thres_r16_present) {
    j.write_bool("dlRSRPAboveThreshold-r16", dl_rsrp_above_thres_r16);
  }
  if (ext) {
    if (fallback_to_four_step_ra_r17_present) {
      j.write_str("fallbackToFourStepRA-r17", "true");
    }
  }
  j.end_obj();
}

// MeasResultEUTRA ::= SEQUENCE
SRSASN_CODE meas_result_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cgi_info_present, 1));

  HANDLE_CODE(pack_integer(bref, eutra_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(meas_result.pack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(cgi_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cgi_info_present, 1));

  HANDLE_CODE(unpack_integer(eutra_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(meas_result.unpack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(cgi_info.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-PhysCellId", eutra_pci);
  j.write_fieldname("measResult");
  meas_result.to_json(j);
  if (cgi_info_present) {
    j.write_fieldname("cgi-Info");
    cgi_info.to_json(j);
  }
  j.end_obj();
}

// MeasResultLoggingNR-r16 ::= SEQUENCE
SRSASN_CODE meas_result_logging_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nof_good_ssb_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r16, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(results_ssb_cell_r16.pack(bref));
  if (nof_good_ssb_r16_present) {
    HANDLE_CODE(pack_integer(bref, nof_good_ssb_r16, (uint8_t)1u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_logging_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nof_good_ssb_r16_present, 1));

  HANDLE_CODE(unpack_integer(pci_r16, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(results_ssb_cell_r16.unpack(bref));
  if (nof_good_ssb_r16_present) {
    HANDLE_CODE(unpack_integer(nof_good_ssb_r16, bref, (uint8_t)1u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_logging_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r16", pci_r16);
  j.write_fieldname("resultsSSB-Cell-r16");
  results_ssb_cell_r16.to_json(j);
  if (nof_good_ssb_r16_present) {
    j.write_int("numberOfGoodSSB-r16", nof_good_ssb_r16);
  }
  j.end_obj();
}

// MeasResultNR ::= SEQUENCE
SRSASN_CODE meas_result_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pci_present, 1));

  if (pci_present) {
    HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  }
  HANDLE_CODE(bref.pack(meas_result.rs_idx_results_present, 1));
  HANDLE_CODE(bref.pack(meas_result.cell_results.results_ssb_cell_present, 1));
  HANDLE_CODE(bref.pack(meas_result.cell_results.results_csi_rs_cell_present, 1));
  if (meas_result.cell_results.results_ssb_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_ssb_cell.pack(bref));
  }
  if (meas_result.cell_results.results_csi_rs_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_csi_rs_cell.pack(bref));
  }
  if (meas_result.rs_idx_results_present) {
    HANDLE_CODE(bref.pack(meas_result.rs_idx_results.results_ssb_idxes.size() > 0, 1));
    HANDLE_CODE(bref.pack(meas_result.rs_idx_results.results_csi_rs_idxes.size() > 0, 1));
    if (meas_result.rs_idx_results.results_ssb_idxes.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result.rs_idx_results.results_ssb_idxes, 1, 64));
    }
    if (meas_result.rs_idx_results.results_csi_rs_idxes.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result.rs_idx_results.results_csi_rs_idxes, 1, 64));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cgi_info.is_present();
    group_flags[1] |= cho_candidate_r17_present;
    group_flags[1] |= cho_cfg_r17.is_present();
    group_flags[1] |= triggered_event_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cgi_info.is_present(), 1));
      if (cgi_info.is_present()) {
        HANDLE_CODE(cgi_info->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cho_candidate_r17_present, 1));
      HANDLE_CODE(bref.pack(cho_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(triggered_event_r17.is_present(), 1));
      if (cho_cfg_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cho_cfg_r17, 1, 2));
      }
      if (triggered_event_r17.is_present()) {
        HANDLE_CODE(bref.pack(triggered_event_r17->time_between_events_r17_present, 1));
        HANDLE_CODE(bref.pack(triggered_event_r17->first_triggered_event_present, 1));
        if (triggered_event_r17->time_between_events_r17_present) {
          HANDLE_CODE(pack_integer(bref, triggered_event_r17->time_between_events_r17, (uint16_t)0u, (uint16_t)1023u));
        }
        if (triggered_event_r17->first_triggered_event_present) {
          HANDLE_CODE(triggered_event_r17->first_triggered_event.pack(bref));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pci_present, 1));

  if (pci_present) {
    HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  }
  HANDLE_CODE(bref.unpack(meas_result.rs_idx_results_present, 1));
  HANDLE_CODE(bref.unpack(meas_result.cell_results.results_ssb_cell_present, 1));
  HANDLE_CODE(bref.unpack(meas_result.cell_results.results_csi_rs_cell_present, 1));
  if (meas_result.cell_results.results_ssb_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_ssb_cell.unpack(bref));
  }
  if (meas_result.cell_results.results_csi_rs_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_csi_rs_cell.unpack(bref));
  }
  if (meas_result.rs_idx_results_present) {
    bool results_ssb_idxes_present;
    HANDLE_CODE(bref.unpack(results_ssb_idxes_present, 1));
    bool results_csi_rs_idxes_present;
    HANDLE_CODE(bref.unpack(results_csi_rs_idxes_present, 1));
    if (results_ssb_idxes_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result.rs_idx_results.results_ssb_idxes, bref, 1, 64));
    }
    if (results_csi_rs_idxes_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result.rs_idx_results.results_csi_rs_idxes, bref, 1, 64));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(cgi_info, bref);
      if (cgi_info.is_present()) {
        HANDLE_CODE(cgi_info->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cho_candidate_r17_present, 1));
      unpack_presence_flag(cho_cfg_r17, bref);
      unpack_presence_flag(triggered_event_r17, bref);
      if (cho_cfg_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cho_cfg_r17, bref, 1, 2));
      }
      if (triggered_event_r17.is_present()) {
        HANDLE_CODE(bref.unpack(triggered_event_r17->time_between_events_r17_present, 1));
        HANDLE_CODE(bref.unpack(triggered_event_r17->first_triggered_event_present, 1));
        if (triggered_event_r17->time_between_events_r17_present) {
          HANDLE_CODE(
              unpack_integer(triggered_event_r17->time_between_events_r17, bref, (uint16_t)0u, (uint16_t)1023u));
        }
        if (triggered_event_r17->first_triggered_event_present) {
          HANDLE_CODE(triggered_event_r17->first_triggered_event.unpack(bref));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_present) {
    j.write_int("physCellId", pci);
  }
  j.write_fieldname("measResult");
  j.start_obj();
  j.write_fieldname("cellResults");
  j.start_obj();
  if (meas_result.cell_results.results_ssb_cell_present) {
    j.write_fieldname("resultsSSB-Cell");
    meas_result.cell_results.results_ssb_cell.to_json(j);
  }
  if (meas_result.cell_results.results_csi_rs_cell_present) {
    j.write_fieldname("resultsCSI-RS-Cell");
    meas_result.cell_results.results_csi_rs_cell.to_json(j);
  }
  j.end_obj();
  if (meas_result.rs_idx_results_present) {
    j.write_fieldname("rsIndexResults");
    j.start_obj();
    if (meas_result.rs_idx_results.results_ssb_idxes.size() > 0) {
      j.start_array("resultsSSB-Indexes");
      for (const auto& e1 : meas_result.rs_idx_results.results_ssb_idxes) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result.rs_idx_results.results_csi_rs_idxes.size() > 0) {
      j.start_array("resultsCSI-RS-Indexes");
      for (const auto& e1 : meas_result.rs_idx_results.results_csi_rs_idxes) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  j.end_obj();
  if (ext) {
    if (cgi_info.is_present()) {
      j.write_fieldname("cgi-Info");
      cgi_info->to_json(j);
    }
    if (cho_candidate_r17_present) {
      j.write_str("choCandidate-r17", "true");
    }
    if (cho_cfg_r17.is_present()) {
      j.start_array("choConfig-r17");
      for (const auto& e1 : *cho_cfg_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (triggered_event_r17.is_present()) {
      j.write_fieldname("triggeredEvent-r17");
      j.start_obj();
      if (triggered_event_r17->time_between_events_r17_present) {
        j.write_int("timeBetweenEvents-r17", triggered_event_r17->time_between_events_r17);
      }
      if (triggered_event_r17->first_triggered_event_present) {
        j.write_str("firstTriggeredEvent", triggered_event_r17->first_triggered_event.to_string());
      }
      j.end_obj();
    }
  }
  j.end_obj();
}

const char* meas_result_nr_s::triggered_event_r17_s_::first_triggered_event_opts::to_string() const
{
  static const char* names[] = {"condFirstEvent", "condSecondEvent"};
  return convert_enum_idx(names, 2, value, "meas_result_nr_s::triggered_event_r17_s_::first_triggered_event_e_");
}

// NeedForNCSG-EUTRA-r17 ::= SEQUENCE
SRSASN_CODE need_for_ncsg_eutra_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_eutra_r17, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(gap_ind_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_ncsg_eutra_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_eutra_r17, bref, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(gap_ind_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void need_for_ncsg_eutra_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandEUTRA-r17", band_eutra_r17);
  j.write_str("gapIndication-r17", gap_ind_r17.to_string());
  j.end_obj();
}

const char* need_for_ncsg_eutra_r17_s::gap_ind_r17_opts::to_string() const
{
  static const char* names[] = {"gap", "ncsg", "nogap-noncsg"};
  return convert_enum_idx(names, 3, value, "need_for_ncsg_eutra_r17_s::gap_ind_r17_e_");
}

// SL-QoS-Info-r16 ::= SEQUENCE
SRSASN_CODE sl_qos_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_qos_profile_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_qos_flow_id_r16, (uint16_t)1u, (uint16_t)2048u));
  if (sl_qos_profile_r16_present) {
    HANDLE_CODE(sl_qos_profile_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_qos_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_qos_profile_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_qos_flow_id_r16, bref, (uint16_t)1u, (uint16_t)2048u));
  if (sl_qos_profile_r16_present) {
    HANDLE_CODE(sl_qos_profile_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_qos_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-QoS-FlowIdentity-r16", sl_qos_flow_id_r16);
  if (sl_qos_profile_r16_present) {
    j.write_fieldname("sl-QoS-Profile-r16");
    sl_qos_profile_r16.to_json(j);
  }
  j.end_obj();
}

// UplinkTxDirectCurrentCarrierInfo-r16 ::= SEQUENCE
SRSASN_CODE ul_tx_direct_current_carrier_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_idx_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(serv_cell_info_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_carrier_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_idx_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(serv_cell_info_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_tx_direct_current_carrier_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex-r16", serv_cell_idx_r16);
  j.write_fieldname("servCellInfo-r16");
  serv_cell_info_r16.to_json(j);
  j.end_obj();
}

void ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::set(types::options e)
{
  type_ = e;
}
uint8_t& ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::set_bwp_id_r16()
{
  set(types::bwp_id_r16);
  return c;
}
void ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::set_deactiv_carrier_r16()
{
  set(types::deactiv_carrier_r16);
}
void ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::bwp_id_r16:
      j.write_int("bwp-Id-r16", c);
      break;
    case types::deactiv_carrier_r16:
      break;
    default:
      log_invalid_choice_id(type_, "ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::bwp_id_r16:
      HANDLE_CODE(pack_integer(bref, c, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::deactiv_carrier_r16:
      break;
    default:
      log_invalid_choice_id(type_, "ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::bwp_id_r16:
      HANDLE_CODE(unpack_integer(c, bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::deactiv_carrier_r16:
      break;
    default:
      log_invalid_choice_id(type_, "ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"bwp-Id-r16", "deactivatedCarrier-r16"};
  return convert_enum_idx(names, 2, value, "ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::types");
}

// UplinkTxDirectCurrentTwoCarrierInfo-r16 ::= SEQUENCE
SRSASN_CODE ul_tx_direct_current_two_carrier_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ref_carrier_idx_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.pack(shift7dot5k_hz_r16, 1));
  HANDLE_CODE(pack_integer(bref, tx_direct_current_location_r16, (uint16_t)0u, (uint16_t)3301u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_two_carrier_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ref_carrier_idx_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(bref.unpack(shift7dot5k_hz_r16, 1));
  HANDLE_CODE(unpack_integer(tx_direct_current_location_r16, bref, (uint16_t)0u, (uint16_t)3301u));

  return SRSASN_SUCCESS;
}
void ul_tx_direct_current_two_carrier_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("referenceCarrierIndex-r16", ref_carrier_idx_r16);
  j.write_bool("shift7dot5kHz-r16", shift7dot5k_hz_r16);
  j.write_int("txDirectCurrentLocation-r16", tx_direct_current_location_r16);
  j.end_obj();
}

// WLAN-Identifiers-r16 ::= SEQUENCE
SRSASN_CODE wlan_ids_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ssid_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(bssid_r16_present, 1));
  HANDLE_CODE(bref.pack(hessid_r16_present, 1));

  if (ssid_r16.size() > 0) {
    HANDLE_CODE(ssid_r16.pack(bref));
  }
  if (bssid_r16_present) {
    HANDLE_CODE(bssid_r16.pack(bref));
  }
  if (hessid_r16_present) {
    HANDLE_CODE(hessid_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_ids_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ssid_r16_present;
  HANDLE_CODE(bref.unpack(ssid_r16_present, 1));
  HANDLE_CODE(bref.unpack(bssid_r16_present, 1));
  HANDLE_CODE(bref.unpack(hessid_r16_present, 1));

  if (ssid_r16_present) {
    HANDLE_CODE(ssid_r16.unpack(bref));
  }
  if (bssid_r16_present) {
    HANDLE_CODE(bssid_r16.unpack(bref));
  }
  if (hessid_r16_present) {
    HANDLE_CODE(hessid_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void wlan_ids_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssid_r16.size() > 0) {
    j.write_str("ssid-r16", ssid_r16.to_string());
  }
  if (bssid_r16_present) {
    j.write_str("bssid-r16", bssid_r16.to_string());
  }
  if (hessid_r16_present) {
    j.write_str("hessid-r16", hessid_r16.to_string());
  }
  j.end_obj();
}

// WLAN-RTT-r16 ::= SEQUENCE
SRSASN_CODE wlan_rtt_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rtt_accuracy_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, rtt_value_r16, (uint32_t)0u, (uint32_t)16777215u));
  HANDLE_CODE(rtt_units_r16.pack(bref));
  if (rtt_accuracy_r16_present) {
    HANDLE_CODE(pack_integer(bref, rtt_accuracy_r16, (uint16_t)0u, (uint16_t)255u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wlan_rtt_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rtt_accuracy_r16_present, 1));

  HANDLE_CODE(unpack_integer(rtt_value_r16, bref, (uint32_t)0u, (uint32_t)16777215u));
  HANDLE_CODE(rtt_units_r16.unpack(bref));
  if (rtt_accuracy_r16_present) {
    HANDLE_CODE(unpack_integer(rtt_accuracy_r16, bref, (uint16_t)0u, (uint16_t)255u));
  }

  return SRSASN_SUCCESS;
}
void wlan_rtt_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rttValue-r16", rtt_value_r16);
  j.write_str("rttUnits-r16", rtt_units_r16.to_string());
  if (rtt_accuracy_r16_present) {
    j.write_int("rttAccuracy-r16", rtt_accuracy_r16);
  }
  j.end_obj();
}

const char* wlan_rtt_r16_s::rtt_units_r16_opts::to_string() const
{
  static const char* names[] = {
      "microseconds", "hundredsofnanoseconds", "tensofnanoseconds", "nanoseconds", "tenthsofnanoseconds"};
  return convert_enum_idx(names, 5, value, "wlan_rtt_r16_s::rtt_units_r16_e_");
}

// CGI-InfoEUTRALogging ::= SEQUENCE
SRSASN_CODE cgi_info_eutra_logging_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_eutra_5gc_present, 1));
  HANDLE_CODE(bref.pack(tac_eutra_5gc_present, 1));
  HANDLE_CODE(bref.pack(cell_id_eutra_5gc_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_eutra_epc_present, 1));
  HANDLE_CODE(bref.pack(tac_eutra_epc_present, 1));
  HANDLE_CODE(bref.pack(cell_id_eutra_epc_present, 1));

  if (plmn_id_eutra_5gc_present) {
    HANDLE_CODE(plmn_id_eutra_5gc.pack(bref));
  }
  if (tac_eutra_5gc_present) {
    HANDLE_CODE(tac_eutra_5gc.pack(bref));
  }
  if (cell_id_eutra_5gc_present) {
    HANDLE_CODE(cell_id_eutra_5gc.pack(bref));
  }
  if (plmn_id_eutra_epc_present) {
    HANDLE_CODE(plmn_id_eutra_epc.pack(bref));
  }
  if (tac_eutra_epc_present) {
    HANDLE_CODE(tac_eutra_epc.pack(bref));
  }
  if (cell_id_eutra_epc_present) {
    HANDLE_CODE(cell_id_eutra_epc.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_info_eutra_logging_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_eutra_5gc_present, 1));
  HANDLE_CODE(bref.unpack(tac_eutra_5gc_present, 1));
  HANDLE_CODE(bref.unpack(cell_id_eutra_5gc_present, 1));
  HANDLE_CODE(bref.unpack(plmn_id_eutra_epc_present, 1));
  HANDLE_CODE(bref.unpack(tac_eutra_epc_present, 1));
  HANDLE_CODE(bref.unpack(cell_id_eutra_epc_present, 1));

  if (plmn_id_eutra_5gc_present) {
    HANDLE_CODE(plmn_id_eutra_5gc.unpack(bref));
  }
  if (tac_eutra_5gc_present) {
    HANDLE_CODE(tac_eutra_5gc.unpack(bref));
  }
  if (cell_id_eutra_5gc_present) {
    HANDLE_CODE(cell_id_eutra_5gc.unpack(bref));
  }
  if (plmn_id_eutra_epc_present) {
    HANDLE_CODE(plmn_id_eutra_epc.unpack(bref));
  }
  if (tac_eutra_epc_present) {
    HANDLE_CODE(tac_eutra_epc.unpack(bref));
  }
  if (cell_id_eutra_epc_present) {
    HANDLE_CODE(cell_id_eutra_epc.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cgi_info_eutra_logging_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_eutra_5gc_present) {
    j.write_fieldname("plmn-Identity-eutra-5gc");
    plmn_id_eutra_5gc.to_json(j);
  }
  if (tac_eutra_5gc_present) {
    j.write_str("trackingAreaCode-eutra-5gc", tac_eutra_5gc.to_string());
  }
  if (cell_id_eutra_5gc_present) {
    j.write_str("cellIdentity-eutra-5gc", cell_id_eutra_5gc.to_string());
  }
  if (plmn_id_eutra_epc_present) {
    j.write_fieldname("plmn-Identity-eutra-epc");
    plmn_id_eutra_epc.to_json(j);
  }
  if (tac_eutra_epc_present) {
    j.write_str("trackingAreaCode-eutra-epc", tac_eutra_epc.to_string());
  }
  if (cell_id_eutra_epc_present) {
    j.write_str("cellIdentity-eutra-epc", cell_id_eutra_epc.to_string());
  }
  j.end_obj();
}

// LogMeasResultBT-r16 ::= SEQUENCE
SRSASN_CODE log_meas_result_bt_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rssi_bt_r16_present, 1));

  HANDLE_CODE(bt_addr_r16.pack(bref));
  if (rssi_bt_r16_present) {
    HANDLE_CODE(pack_integer(bref, rssi_bt_r16, (int16_t)-128, (int16_t)127));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_result_bt_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rssi_bt_r16_present, 1));

  HANDLE_CODE(bt_addr_r16.unpack(bref));
  if (rssi_bt_r16_present) {
    HANDLE_CODE(unpack_integer(rssi_bt_r16, bref, (int16_t)-128, (int16_t)127));
  }

  return SRSASN_SUCCESS;
}
void log_meas_result_bt_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bt-Addr-r16", bt_addr_r16.to_string());
  if (rssi_bt_r16_present) {
    j.write_int("rssi-BT-r16", rssi_bt_r16);
  }
  j.end_obj();
}

// LogMeasResultWLAN-r16 ::= SEQUENCE
SRSASN_CODE log_meas_result_wlan_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rssi_wlan_r16_present, 1));
  HANDLE_CODE(bref.pack(rtt_wlan_r16_present, 1));

  HANDLE_CODE(wlan_ids_r16.pack(bref));
  if (rssi_wlan_r16_present) {
    HANDLE_CODE(pack_integer(bref, rssi_wlan_r16, (uint8_t)0u, (uint8_t)141u));
  }
  if (rtt_wlan_r16_present) {
    HANDLE_CODE(rtt_wlan_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_result_wlan_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rssi_wlan_r16_present, 1));
  HANDLE_CODE(bref.unpack(rtt_wlan_r16_present, 1));

  HANDLE_CODE(wlan_ids_r16.unpack(bref));
  if (rssi_wlan_r16_present) {
    HANDLE_CODE(unpack_integer(rssi_wlan_r16, bref, (uint8_t)0u, (uint8_t)141u));
  }
  if (rtt_wlan_r16_present) {
    HANDLE_CODE(rtt_wlan_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void log_meas_result_wlan_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("wlan-Identifiers-r16");
  wlan_ids_r16.to_json(j);
  if (rssi_wlan_r16_present) {
    j.write_int("rssiWLAN-r16", rssi_wlan_r16);
  }
  if (rtt_wlan_r16_present) {
    j.write_fieldname("rtt-WLAN-r16");
    rtt_wlan_r16.to_json(j);
  }
  j.end_obj();
}

// NeedForGapNCSG-InfoEUTRA-r17 ::= SEQUENCE
SRSASN_CODE need_for_gap_ncsg_info_eutra_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, need_for_ncsg_eutra_r17, 1, 256));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gap_ncsg_info_eutra_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(need_for_ncsg_eutra_r17, bref, 1, 256));

  return SRSASN_SUCCESS;
}
void need_for_gap_ncsg_info_eutra_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("needForNCSG-EUTRA-r17");
  for (const auto& e1 : need_for_ncsg_eutra_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// NeedForGapNCSG-InfoNR-r17 ::= SEQUENCE
SRSASN_CODE need_for_gap_ncsg_info_nr_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_need_for_ncsg_r17, 1, 32));
  HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_need_for_ncsg_r17, 1, 1024));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gap_ncsg_info_nr_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(intra_freq_need_for_ncsg_r17, bref, 1, 32));
  HANDLE_CODE(unpack_dyn_seq_of(inter_freq_need_for_ncsg_r17, bref, 1, 1024));

  return SRSASN_SUCCESS;
}
void need_for_gap_ncsg_info_nr_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("intraFreq-needForNCSG-r17");
  for (const auto& e1 : intra_freq_need_for_ncsg_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("interFreq-needForNCSG-r17");
  for (const auto& e1 : inter_freq_need_for_ncsg_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// NeedForGapsIntraFreq-r16 ::= SEQUENCE
SRSASN_CODE need_for_gaps_intra_freq_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_id_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(gap_ind_intra_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gaps_intra_freq_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_id_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(gap_ind_intra_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void need_for_gaps_intra_freq_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId-r16", serv_cell_id_r16);
  j.write_str("gapIndicationIntra-r16", gap_ind_intra_r16.to_string());
  j.end_obj();
}

const char* need_for_gaps_intra_freq_r16_s::gap_ind_intra_r16_opts::to_string() const
{
  static const char* names[] = {"gap", "no-gap"};
  return convert_enum_idx(names, 2, value, "need_for_gaps_intra_freq_r16_s::gap_ind_intra_r16_e_");
}

// NeedForGapsNR-r16 ::= SEQUENCE
SRSASN_CODE need_for_gaps_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_nr_r16, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(gap_ind_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gaps_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_nr_r16, bref, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(gap_ind_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void need_for_gaps_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandNR-r16", band_nr_r16);
  j.write_str("gapIndication-r16", gap_ind_r16.to_string());
  j.end_obj();
}

const char* need_for_gaps_nr_r16_s::gap_ind_r16_opts::to_string() const
{
  static const char* names[] = {"gap", "no-gap"};
  return convert_enum_idx(names, 2, value, "need_for_gaps_nr_r16_s::gap_ind_r16_e_");
}

// PCI-ARFCN-EUTRA-r16 ::= SEQUENCE
SRSASN_CODE pci_arfcn_eutra_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r16, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_arfcn_eutra_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r16, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)262143u));

  return SRSASN_SUCCESS;
}
void pci_arfcn_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r16", pci_r16);
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.end_obj();
}

// PCI-ARFCN-NR-r16 ::= SEQUENCE
SRSASN_CODE pci_arfcn_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r16, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_arfcn_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r16, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
void pci_arfcn_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r16", pci_r16);
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.end_obj();
}

// PerRACSI-RSInfo-r16 ::= SEQUENCE
SRSASN_CODE per_ra_csi_r_si_nfo_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, csi_rs_idx_r16, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(pack_integer(bref, nof_preambs_sent_on_csi_rs_r16, (uint8_t)1u, (uint8_t)200u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE per_ra_csi_r_si_nfo_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(csi_rs_idx_r16, bref, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(unpack_integer(nof_preambs_sent_on_csi_rs_r16, bref, (uint8_t)1u, (uint8_t)200u));

  return SRSASN_SUCCESS;
}
void per_ra_csi_r_si_nfo_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS-Index-r16", csi_rs_idx_r16);
  j.write_int("numberOfPreamblesSentOnCSI-RS-r16", nof_preambs_sent_on_csi_rs_r16);
  j.end_obj();
}

// PerRASSBInfo-r16 ::= SEQUENCE
SRSASN_CODE per_ra_ssb_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ssb_idx_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, nof_preambs_sent_on_ssb_r16, (uint8_t)1u, (uint8_t)200u));
  HANDLE_CODE(pack_dyn_seq_of(bref, per_ra_attempt_info_list_r16, 1, 200));

  return SRSASN_SUCCESS;
}
SRSASN_CODE per_ra_ssb_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ssb_idx_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(nof_preambs_sent_on_ssb_r16, bref, (uint8_t)1u, (uint8_t)200u));
  HANDLE_CODE(unpack_dyn_seq_of(per_ra_attempt_info_list_r16, bref, 1, 200));

  return SRSASN_SUCCESS;
}
void per_ra_ssb_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb-Index-r16", ssb_idx_r16);
  j.write_int("numberOfPreamblesSentOnSSB-r16", nof_preambs_sent_on_ssb_r16);
  j.start_array("perRAAttemptInfoList-r16");
  for (const auto& e1 : per_ra_attempt_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RRCReconfigurationComplete-v1720-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1720_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_tx_direct_current_more_carrier_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_more_carrier_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_more_carrier_list_r17, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1720_ies_s::unpack(cbit_ref& bref)
{
  bool ul_tx_direct_current_more_carrier_list_r17_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_more_carrier_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_more_carrier_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_more_carrier_list_r17, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1720_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_tx_direct_current_more_carrier_list_r17.size() > 0) {
    j.start_array("uplinkTxDirectCurrentMoreCarrierList-r17");
    for (const auto& e1 : ul_tx_direct_current_more_carrier_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// ResultsPerSSB-IndexIdle-r16 ::= SEQUENCE
SRSASN_CODE results_per_ssb_idx_idle_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_results_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, ssb_idx_r16, (uint8_t)0u, (uint8_t)63u));
  if (ssb_results_r16_present) {
    HANDLE_CODE(bref.pack(ssb_results_r16.ssb_rsrp_result_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_results_r16.ssb_rsrq_result_r16_present, 1));
    if (ssb_results_r16.ssb_rsrp_result_r16_present) {
      HANDLE_CODE(pack_integer(bref, ssb_results_r16.ssb_rsrp_result_r16, (uint8_t)0u, (uint8_t)127u));
    }
    if (ssb_results_r16.ssb_rsrq_result_r16_present) {
      HANDLE_CODE(pack_integer(bref, ssb_results_r16.ssb_rsrq_result_r16, (uint8_t)0u, (uint8_t)127u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE results_per_ssb_idx_idle_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_results_r16_present, 1));

  HANDLE_CODE(unpack_integer(ssb_idx_r16, bref, (uint8_t)0u, (uint8_t)63u));
  if (ssb_results_r16_present) {
    HANDLE_CODE(bref.unpack(ssb_results_r16.ssb_rsrp_result_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_results_r16.ssb_rsrq_result_r16_present, 1));
    if (ssb_results_r16.ssb_rsrp_result_r16_present) {
      HANDLE_CODE(unpack_integer(ssb_results_r16.ssb_rsrp_result_r16, bref, (uint8_t)0u, (uint8_t)127u));
    }
    if (ssb_results_r16.ssb_rsrq_result_r16_present) {
      HANDLE_CODE(unpack_integer(ssb_results_r16.ssb_rsrq_result_r16, bref, (uint8_t)0u, (uint8_t)127u));
    }
  }

  return SRSASN_SUCCESS;
}
void results_per_ssb_idx_idle_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb-Index-r16", ssb_idx_r16);
  if (ssb_results_r16_present) {
    j.write_fieldname("ssb-Results-r16");
    j.start_obj();
    if (ssb_results_r16.ssb_rsrp_result_r16_present) {
      j.write_int("ssb-RSRP-Result-r16", ssb_results_r16.ssb_rsrp_result_r16);
    }
    if (ssb_results_r16.ssb_rsrq_result_r16_present) {
      j.write_int("ssb-RSRQ-Result-r16", ssb_results_r16.ssb_rsrq_result_r16);
    }
    j.end_obj();
  }
  j.end_obj();
}

// SL-PagingIdentityRemoteUE-r17 ::= SEQUENCE
SRSASN_CODE sl_paging_id_remote_ue_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(full_i_rnti_r17_present, 1));

  HANDLE_CODE(ng_5_g_s_tmsi_r17.pack(bref));
  if (full_i_rnti_r17_present) {
    HANDLE_CODE(full_i_rnti_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_paging_id_remote_ue_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(full_i_rnti_r17_present, 1));

  HANDLE_CODE(ng_5_g_s_tmsi_r17.unpack(bref));
  if (full_i_rnti_r17_present) {
    HANDLE_CODE(full_i_rnti_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_paging_id_remote_ue_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ng-5G-S-TMSI-r17", ng_5_g_s_tmsi_r17.to_string());
  if (full_i_rnti_r17_present) {
    j.write_str("fullI-RNTI-r17", full_i_rnti_r17.to_string());
  }
  j.end_obj();
}

// SL-RLC-ModeIndication-r16 ::= SEQUENCE
SRSASN_CODE sl_rlc_mode_ind_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_mode_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_qos_info_list_r16, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_rlc_mode_ind_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_mode_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sl_qos_info_list_r16, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void sl_rlc_mode_ind_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-Mode-r16");
  sl_mode_r16.to_json(j);
  j.start_array("sl-QoS-InfoList-r16");
  for (const auto& e1 : sl_qos_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

void sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::set(types::options e)
{
  type_ = e;
}
void sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::set_sl_am_mode_r16()
{
  set(types::sl_am_mode_r16);
}
void sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::set_sl_um_mode_r16()
{
  set(types::sl_um_mode_r16);
}
void sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  return SRSASN_SUCCESS;
}

const char* sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"sl-AM-Mode-r16", "sl-UM-Mode-r16"};
  return convert_enum_idx(names, 2, value, "sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::types");
}

// UplinkTxDirectCurrentTwoCarrier-r16 ::= SEQUENCE
SRSASN_CODE ul_tx_direct_current_two_carrier_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(second_pa_tx_direct_current_r16_present, 1));

  HANDLE_CODE(carrier_one_info_r16.pack(bref));
  HANDLE_CODE(carrier_two_info_r16.pack(bref));
  HANDLE_CODE(single_pa_tx_direct_current_r16.pack(bref));
  if (second_pa_tx_direct_current_r16_present) {
    HANDLE_CODE(second_pa_tx_direct_current_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_two_carrier_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(second_pa_tx_direct_current_r16_present, 1));

  HANDLE_CODE(carrier_one_info_r16.unpack(bref));
  HANDLE_CODE(carrier_two_info_r16.unpack(bref));
  HANDLE_CODE(single_pa_tx_direct_current_r16.unpack(bref));
  if (second_pa_tx_direct_current_r16_present) {
    HANDLE_CODE(second_pa_tx_direct_current_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_tx_direct_current_two_carrier_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("carrierOneInfo-r16");
  carrier_one_info_r16.to_json(j);
  j.write_fieldname("carrierTwoInfo-r16");
  carrier_two_info_r16.to_json(j);
  j.write_fieldname("singlePA-TxDirectCurrent-r16");
  single_pa_tx_direct_current_r16.to_json(j);
  if (second_pa_tx_direct_current_r16_present) {
    j.write_fieldname("secondPA-TxDirectCurrent-r16");
    second_pa_tx_direct_current_r16.to_json(j);
  }
  j.end_obj();
}

// VictimSystemType-r16 ::= SEQUENCE
SRSASN_CODE victim_sys_type_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gps_r16_present, 1));
  HANDLE_CODE(bref.pack(glonass_r16_present, 1));
  HANDLE_CODE(bref.pack(bds_r16_present, 1));
  HANDLE_CODE(bref.pack(galileo_r16_present, 1));
  HANDLE_CODE(bref.pack(nav_ic_r16_present, 1));
  HANDLE_CODE(bref.pack(wlan_r16_present, 1));
  HANDLE_CODE(bref.pack(bluetooth_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE victim_sys_type_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gps_r16_present, 1));
  HANDLE_CODE(bref.unpack(glonass_r16_present, 1));
  HANDLE_CODE(bref.unpack(bds_r16_present, 1));
  HANDLE_CODE(bref.unpack(galileo_r16_present, 1));
  HANDLE_CODE(bref.unpack(nav_ic_r16_present, 1));
  HANDLE_CODE(bref.unpack(wlan_r16_present, 1));
  HANDLE_CODE(bref.unpack(bluetooth_r16_present, 1));

  return SRSASN_SUCCESS;
}
void victim_sys_type_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gps_r16_present) {
    j.write_str("gps-r16", "true");
  }
  if (glonass_r16_present) {
    j.write_str("glonass-r16", "true");
  }
  if (bds_r16_present) {
    j.write_str("bds-r16", "true");
  }
  if (galileo_r16_present) {
    j.write_str("galileo-r16", "true");
  }
  if (nav_ic_r16_present) {
    j.write_str("navIC-r16", "true");
  }
  if (wlan_r16_present) {
    j.write_str("wlan-r16", "true");
  }
  if (bluetooth_r16_present) {
    j.write_str("bluetooth-r16", "true");
  }
  j.end_obj();
}

// AffectedCarrierFreq-r16 ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(interference_direction_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(interference_direction_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.write_str("interferenceDirection-r16", interference_direction_r16.to_string());
  j.end_obj();
}

const char* affected_carrier_freq_r16_s::interference_direction_r16_opts::to_string() const
{
  static const char* names[] = {"nr", "other", "both", "spare"};
  return convert_enum_idx(names, 4, value, "affected_carrier_freq_r16_s::interference_direction_r16_e_");
}

// AffectedCarrierFreqComb-r16 ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_comb_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(affected_carrier_freq_comb_r16.size() > 0, 1));

  if (affected_carrier_freq_comb_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_comb_r16, 2, 32, integer_packer<uint32_t>(0, 3279165)));
  }
  HANDLE_CODE(victim_sys_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_comb_r16_s::unpack(cbit_ref& bref)
{
  bool affected_carrier_freq_comb_r16_present;
  HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_r16_present, 1));

  if (affected_carrier_freq_comb_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_r16, bref, 2, 32, integer_packer<uint32_t>(0, 3279165)));
  }
  HANDLE_CODE(victim_sys_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_comb_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (affected_carrier_freq_comb_r16.size() > 0) {
    j.start_array("affectedCarrierFreqComb-r16");
    for (const auto& e1 : affected_carrier_freq_comb_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_fieldname("victimSystemType-r16");
  victim_sys_type_r16.to_json(j);
  j.end_obj();
}

// CommonLocationInfo-r16 ::= SEQUENCE
SRSASN_CODE common_location_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gnss_tod_msec_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(location_timestamp_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(location_coordinate_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(location_error_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(location_source_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(velocity_estimate_r16.size() > 0, 1));

  if (gnss_tod_msec_r16.size() > 0) {
    HANDLE_CODE(gnss_tod_msec_r16.pack(bref));
  }
  if (location_timestamp_r16.size() > 0) {
    HANDLE_CODE(location_timestamp_r16.pack(bref));
  }
  if (location_coordinate_r16.size() > 0) {
    HANDLE_CODE(location_coordinate_r16.pack(bref));
  }
  if (location_error_r16.size() > 0) {
    HANDLE_CODE(location_error_r16.pack(bref));
  }
  if (location_source_r16.size() > 0) {
    HANDLE_CODE(location_source_r16.pack(bref));
  }
  if (velocity_estimate_r16.size() > 0) {
    HANDLE_CODE(velocity_estimate_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE common_location_info_r16_s::unpack(cbit_ref& bref)
{
  bool gnss_tod_msec_r16_present;
  HANDLE_CODE(bref.unpack(gnss_tod_msec_r16_present, 1));
  bool location_timestamp_r16_present;
  HANDLE_CODE(bref.unpack(location_timestamp_r16_present, 1));
  bool location_coordinate_r16_present;
  HANDLE_CODE(bref.unpack(location_coordinate_r16_present, 1));
  bool location_error_r16_present;
  HANDLE_CODE(bref.unpack(location_error_r16_present, 1));
  bool location_source_r16_present;
  HANDLE_CODE(bref.unpack(location_source_r16_present, 1));
  bool velocity_estimate_r16_present;
  HANDLE_CODE(bref.unpack(velocity_estimate_r16_present, 1));

  if (gnss_tod_msec_r16_present) {
    HANDLE_CODE(gnss_tod_msec_r16.unpack(bref));
  }
  if (location_timestamp_r16_present) {
    HANDLE_CODE(location_timestamp_r16.unpack(bref));
  }
  if (location_coordinate_r16_present) {
    HANDLE_CODE(location_coordinate_r16.unpack(bref));
  }
  if (location_error_r16_present) {
    HANDLE_CODE(location_error_r16.unpack(bref));
  }
  if (location_source_r16_present) {
    HANDLE_CODE(location_source_r16.unpack(bref));
  }
  if (velocity_estimate_r16_present) {
    HANDLE_CODE(velocity_estimate_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void common_location_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnss_tod_msec_r16.size() > 0) {
    j.write_str("gnss-TOD-msec-r16", gnss_tod_msec_r16.to_string());
  }
  if (location_timestamp_r16.size() > 0) {
    j.write_str("locationTimestamp-r16", location_timestamp_r16.to_string());
  }
  if (location_coordinate_r16.size() > 0) {
    j.write_str("locationCoordinate-r16", location_coordinate_r16.to_string());
  }
  if (location_error_r16.size() > 0) {
    j.write_str("locationError-r16", location_error_r16.to_string());
  }
  if (location_source_r16.size() > 0) {
    j.write_str("locationSource-r16", location_source_r16.to_string());
  }
  if (velocity_estimate_r16.size() > 0) {
    j.write_str("velocityEstimate-r16", velocity_estimate_r16.to_string());
  }
  j.end_obj();
}

// MeasResult2EUTRA-r16 ::= SEQUENCE
SRSASN_CODE meas_result2_eutra_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_eutra_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result2_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.start_array("measResultList-r16");
  for (const auto& e1 : meas_result_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResult2NR-r16 ::= SEQUENCE
SRSASN_CODE meas_result2_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_freq_r16_present, 1));
  HANDLE_CODE(bref.pack(ref_freq_csi_rs_r16_present, 1));

  if (ssb_freq_r16_present) {
    HANDLE_CODE(pack_integer(bref, ssb_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ref_freq_csi_rs_r16_present) {
    HANDLE_CODE(pack_integer(bref, ref_freq_csi_rs_r16, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_freq_r16_present, 1));
  HANDLE_CODE(bref.unpack(ref_freq_csi_rs_r16_present, 1));

  if (ssb_freq_r16_present) {
    HANDLE_CODE(unpack_integer(ssb_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ref_freq_csi_rs_r16_present) {
    HANDLE_CODE(unpack_integer(ref_freq_csi_rs_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result2_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_freq_r16_present) {
    j.write_int("ssbFrequency-r16", ssb_freq_r16);
  }
  if (ref_freq_csi_rs_r16_present) {
    j.write_int("refFreqCSI-RS-r16", ref_freq_csi_rs_r16);
  }
  j.start_array("measResultList-r16");
  for (const auto& e1 : meas_result_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResultLogging2NR-r16 ::= SEQUENCE
SRSASN_CODE meas_result_logging2_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_logging_nr_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_logging2_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_logging_nr_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result_logging2_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.start_array("measResultListLoggingNR-r16");
  for (const auto& e1 : meas_result_list_logging_nr_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResultUTRA-FDD-r16 ::= SEQUENCE
SRSASN_CODE meas_result_utra_fdd_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r16, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(bref.pack(meas_result_r16.utra_fdd_rs_cp_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_r16.utra_fdd_ec_n0_r16_present, 1));
  if (meas_result_r16.utra_fdd_rs_cp_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_r16.utra_fdd_rs_cp_r16, (int8_t)-5, (int8_t)91));
  }
  if (meas_result_r16.utra_fdd_ec_n0_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_r16.utra_fdd_ec_n0_r16, (uint8_t)0u, (uint8_t)49u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_utra_fdd_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r16, bref, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(bref.unpack(meas_result_r16.utra_fdd_rs_cp_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_r16.utra_fdd_ec_n0_r16_present, 1));
  if (meas_result_r16.utra_fdd_rs_cp_r16_present) {
    HANDLE_CODE(unpack_integer(meas_result_r16.utra_fdd_rs_cp_r16, bref, (int8_t)-5, (int8_t)91));
  }
  if (meas_result_r16.utra_fdd_ec_n0_r16_present) {
    HANDLE_CODE(unpack_integer(meas_result_r16.utra_fdd_ec_n0_r16, bref, (uint8_t)0u, (uint8_t)49u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_utra_fdd_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r16", pci_r16);
  j.write_fieldname("measResult-r16");
  j.start_obj();
  if (meas_result_r16.utra_fdd_rs_cp_r16_present) {
    j.write_int("utra-FDD-RSCP-r16", meas_result_r16.utra_fdd_rs_cp_r16);
  }
  if (meas_result_r16.utra_fdd_ec_n0_r16_present) {
    j.write_int("utra-FDD-EcN0-r16", meas_result_r16.utra_fdd_ec_n0_r16);
  }
  j.end_obj();
  j.end_obj();
}

// PerRACSI-RSInfo-v1660 ::= SEQUENCE
SRSASN_CODE per_ra_csi_r_si_nfo_v1660_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_idx_v1660_present, 1));

  if (csi_rs_idx_v1660_present) {
    HANDLE_CODE(pack_integer(bref, csi_rs_idx_v1660, (uint8_t)1u, (uint8_t)96u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE per_ra_csi_r_si_nfo_v1660_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_idx_v1660_present, 1));

  if (csi_rs_idx_v1660_present) {
    HANDLE_CODE(unpack_integer(csi_rs_idx_v1660, bref, (uint8_t)1u, (uint8_t)96u));
  }

  return SRSASN_SUCCESS;
}
void per_ra_csi_r_si_nfo_v1660_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_idx_v1660_present) {
    j.write_int("csi-RS-Index-v1660", csi_rs_idx_v1660);
  }
  j.end_obj();
}

// PerRAInfo-r16 ::= CHOICE
void per_ra_info_r16_c::destroy_()
{
  switch (type_) {
    case types::per_ra_ssb_info_list_r16:
      c.destroy<per_ra_ssb_info_r16_s>();
      break;
    case types::per_ra_csi_r_si_nfo_list_r16:
      c.destroy<per_ra_csi_r_si_nfo_r16_s>();
      break;
    default:
      break;
  }
}
void per_ra_info_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::per_ra_ssb_info_list_r16:
      c.init<per_ra_ssb_info_r16_s>();
      break;
    case types::per_ra_csi_r_si_nfo_list_r16:
      c.init<per_ra_csi_r_si_nfo_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "per_ra_info_r16_c");
  }
}
per_ra_info_r16_c::per_ra_info_r16_c(const per_ra_info_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::per_ra_ssb_info_list_r16:
      c.init(other.c.get<per_ra_ssb_info_r16_s>());
      break;
    case types::per_ra_csi_r_si_nfo_list_r16:
      c.init(other.c.get<per_ra_csi_r_si_nfo_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "per_ra_info_r16_c");
  }
}
per_ra_info_r16_c& per_ra_info_r16_c::operator=(const per_ra_info_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::per_ra_ssb_info_list_r16:
      c.set(other.c.get<per_ra_ssb_info_r16_s>());
      break;
    case types::per_ra_csi_r_si_nfo_list_r16:
      c.set(other.c.get<per_ra_csi_r_si_nfo_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "per_ra_info_r16_c");
  }

  return *this;
}
per_ra_ssb_info_r16_s& per_ra_info_r16_c::set_per_ra_ssb_info_list_r16()
{
  set(types::per_ra_ssb_info_list_r16);
  return c.get<per_ra_ssb_info_r16_s>();
}
per_ra_csi_r_si_nfo_r16_s& per_ra_info_r16_c::set_per_ra_csi_r_si_nfo_list_r16()
{
  set(types::per_ra_csi_r_si_nfo_list_r16);
  return c.get<per_ra_csi_r_si_nfo_r16_s>();
}
void per_ra_info_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::per_ra_ssb_info_list_r16:
      j.write_fieldname("perRASSBInfoList-r16");
      c.get<per_ra_ssb_info_r16_s>().to_json(j);
      break;
    case types::per_ra_csi_r_si_nfo_list_r16:
      j.write_fieldname("perRACSI-RSInfoList-r16");
      c.get<per_ra_csi_r_si_nfo_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "per_ra_info_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE per_ra_info_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::per_ra_ssb_info_list_r16:
      HANDLE_CODE(c.get<per_ra_ssb_info_r16_s>().pack(bref));
      break;
    case types::per_ra_csi_r_si_nfo_list_r16:
      HANDLE_CODE(c.get<per_ra_csi_r_si_nfo_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "per_ra_info_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE per_ra_info_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::per_ra_ssb_info_list_r16:
      HANDLE_CODE(c.get<per_ra_ssb_info_r16_s>().unpack(bref));
      break;
    case types::per_ra_csi_r_si_nfo_list_r16:
      HANDLE_CODE(c.get<per_ra_csi_r_si_nfo_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "per_ra_info_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* per_ra_info_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"perRASSBInfoList-r16", "perRACSI-RSInfoList-r16"};
  return convert_enum_idx(names, 2, value, "per_ra_info_r16_c::types");
}

// RRCReconfigurationComplete-v1700-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_nr_r17_present, 1));
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_eutra_r17_present, 1));
  HANDLE_CODE(bref.pack(sel_cond_rrc_recfg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (need_for_gap_ncsg_info_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_nr_r17.pack(bref));
  }
  if (need_for_gap_ncsg_info_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17.pack(bref));
  }
  if (sel_cond_rrc_recfg_r17_present) {
    HANDLE_CODE(pack_integer(bref, sel_cond_rrc_recfg_r17, (uint8_t)1u, (uint8_t)8u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_info_nr_r17_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_info_eutra_r17_present, 1));
  HANDLE_CODE(bref.unpack(sel_cond_rrc_recfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (need_for_gap_ncsg_info_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_nr_r17.unpack(bref));
  }
  if (need_for_gap_ncsg_info_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17.unpack(bref));
  }
  if (sel_cond_rrc_recfg_r17_present) {
    HANDLE_CODE(unpack_integer(sel_cond_rrc_recfg_r17, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (need_for_gap_ncsg_info_nr_r17_present) {
    j.write_fieldname("needForGapNCSG-InfoNR-r17");
    need_for_gap_ncsg_info_nr_r17.to_json(j);
  }
  if (need_for_gap_ncsg_info_eutra_r17_present) {
    j.write_fieldname("needForGapNCSG-InfoEUTRA-r17");
    need_for_gap_ncsg_info_eutra_r17.to_json(j);
  }
  if (sel_cond_rrc_recfg_r17_present) {
    j.write_int("selectedCondRRCReconfig-r17", sel_cond_rrc_recfg_r17);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ReducedAggregatedBandwidth ::= ENUMERATED
const char* reduced_aggr_bw_opts::to_string() const
{
  static const char* names[] = {
      "mhz0", "mhz10", "mhz20", "mhz30", "mhz40", "mhz50", "mhz60", "mhz80", "mhz100", "mhz200", "mhz300", "mhz400"};
  return convert_enum_idx(names, 12, value, "reduced_aggr_bw_e");
}
uint16_t reduced_aggr_bw_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 10, 20, 30, 40, 50, 60, 80, 100, 200, 300, 400};
  return map_enum_number(numbers, 12, value, "reduced_aggr_bw_e");
}

// ReducedAggregatedBandwidth-r17 ::= ENUMERATED
const char* reduced_aggr_bw_r17_opts::to_string() const
{
  static const char* names[] = {"mhz0", "mhz100", "mhz200", "mhz400", "mhz800", "mhz1200", "mhz1600", "mhz2000"};
  return convert_enum_idx(names, 8, value, "reduced_aggr_bw_r17_e");
}
uint16_t reduced_aggr_bw_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 100, 200, 400, 800, 1200, 1600, 2000};
  return map_enum_number(numbers, 8, value, "reduced_aggr_bw_r17_e");
}

// SL-DRX-ConfigUC-SemiStatic-r17 ::= SEQUENCE
SRSASN_CODE sl_drx_cfg_uc_semi_static_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_drx_on_dur_timer_r17.pack(bref));
  HANDLE_CODE(sl_drx_cycle_start_offset_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, sl_drx_slot_offset_r17, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_uc_semi_static_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_drx_on_dur_timer_r17.unpack(bref));
  HANDLE_CODE(sl_drx_cycle_start_offset_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(sl_drx_slot_offset_r17, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void sl_drx_cfg_uc_semi_static_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-drx-onDurationTimer-r17");
  sl_drx_on_dur_timer_r17.to_json(j);
  j.write_fieldname("sl-drx-CycleStartOffset-r17");
  sl_drx_cycle_start_offset_r17.to_json(j);
  j.write_int("sl-drx-SlotOffset-r17", sl_drx_slot_offset_r17);
  j.end_obj();
}

void sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::destroy_() {}
void sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::sl_drx_on_dur_timer_r17_c_(
    const sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sub_milli_seconds:
      c.init(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.init(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_");
  }
}
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_&
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::operator=(
    const sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sub_milli_seconds:
      c.set(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.set(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_");
  }

  return *this;
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_&
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sub_milli_seconds:
      j.write_int("subMilliSeconds", c.get<uint8_t>());
      break;
    case types::milli_seconds:
      j.write_str("milliSeconds", c.get<milli_seconds_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      names, 32, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_");
}
uint16_t sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(
      numbers, 24, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_");
}

const char* sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::types");
}

void sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::destroy_() {}
void sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::sl_drx_cycle_start_offset_r17_c_(
    const sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_");
  }
}
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_&
sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::operator=(
    const sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_");
  }

  return *this;
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
void sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms10:
      j.write_int("ms10", c.get<uint8_t>());
      break;
    case types::ms20:
      j.write_int("ms20", c.get<uint8_t>());
      break;
    case types::ms32:
      j.write_int("ms32", c.get<uint8_t>());
      break;
    case types::ms40:
      j.write_int("ms40", c.get<uint8_t>());
      break;
    case types::ms60:
      j.write_int("ms60", c.get<uint8_t>());
      break;
    case types::ms64:
      j.write_int("ms64", c.get<uint8_t>());
      break;
    case types::ms70:
      j.write_int("ms70", c.get<uint8_t>());
      break;
    case types::ms80:
      j.write_int("ms80", c.get<uint8_t>());
      break;
    case types::ms128:
      j.write_int("ms128", c.get<uint8_t>());
      break;
    case types::ms160:
      j.write_int("ms160", c.get<uint8_t>());
      break;
    case types::ms256:
      j.write_int("ms256", c.get<uint16_t>());
      break;
    case types::ms320:
      j.write_int("ms320", c.get<uint16_t>());
      break;
    case types::ms512:
      j.write_int("ms512", c.get<uint16_t>());
      break;
    case types::ms640:
      j.write_int("ms640", c.get<uint16_t>());
      break;
    case types::ms1024:
      j.write_int("ms1024", c.get<uint16_t>());
      break;
    case types::ms1280:
      j.write_int("ms1280", c.get<uint16_t>());
      break;
    case types::ms2048:
      j.write_int("ms2048", c.get<uint16_t>());
      break;
    case types::ms2560:
      j.write_int("ms2560", c.get<uint16_t>());
      break;
    case types::ms5120:
      j.write_int("ms5120", c.get<uint16_t>());
      break;
    case types::ms10240:
      j.write_int("ms10240", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types");
}
uint16_t sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(
      numbers, 20, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types");
}

// SL-TxResourceReq-r16 ::= SEQUENCE
SRSASN_CODE sl_tx_res_req_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_rlc_mode_ind_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_qos_info_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_type_tx_sync_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_tx_interested_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_cap_info_sidelink_r16.size() > 0, 1));

  HANDLE_CODE(sl_dest_id_r16.pack(bref));
  HANDLE_CODE(sl_cast_type_r16.pack(bref));
  if (sl_rlc_mode_ind_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_mode_ind_list_r16, 1, 512));
  }
  if (sl_qos_info_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_qos_info_list_r16, 1, 64));
  }
  if (sl_type_tx_sync_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_type_tx_sync_list_r16, 1, 8));
  }
  if (sl_tx_interested_freq_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_interested_freq_list_r16, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_cap_info_sidelink_r16.size() > 0) {
    HANDLE_CODE(sl_cap_info_sidelink_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_res_req_r16_s::unpack(cbit_ref& bref)
{
  bool sl_rlc_mode_ind_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_rlc_mode_ind_list_r16_present, 1));
  bool sl_qos_info_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_qos_info_list_r16_present, 1));
  bool sl_type_tx_sync_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_type_tx_sync_list_r16_present, 1));
  bool sl_tx_interested_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_tx_interested_freq_list_r16_present, 1));
  bool sl_cap_info_sidelink_r16_present;
  HANDLE_CODE(bref.unpack(sl_cap_info_sidelink_r16_present, 1));

  HANDLE_CODE(sl_dest_id_r16.unpack(bref));
  HANDLE_CODE(sl_cast_type_r16.unpack(bref));
  if (sl_rlc_mode_ind_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_mode_ind_list_r16, bref, 1, 512));
  }
  if (sl_qos_info_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_qos_info_list_r16, bref, 1, 64));
  }
  if (sl_type_tx_sync_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_type_tx_sync_list_r16, bref, 1, 8));
  }
  if (sl_tx_interested_freq_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_tx_interested_freq_list_r16, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_cap_info_sidelink_r16_present) {
    HANDLE_CODE(sl_cap_info_sidelink_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_tx_res_req_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-DestinationIdentity-r16", sl_dest_id_r16.to_string());
  j.write_str("sl-CastType-r16", sl_cast_type_r16.to_string());
  if (sl_rlc_mode_ind_list_r16.size() > 0) {
    j.start_array("sl-RLC-ModeIndicationList-r16");
    for (const auto& e1 : sl_rlc_mode_ind_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_qos_info_list_r16.size() > 0) {
    j.start_array("sl-QoS-InfoList-r16");
    for (const auto& e1 : sl_qos_info_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_type_tx_sync_list_r16.size() > 0) {
    j.start_array("sl-TypeTxSyncList-r16");
    for (const auto& e1 : sl_type_tx_sync_list_r16) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (sl_tx_interested_freq_list_r16.size() > 0) {
    j.start_array("sl-TxInterestedFreqList-r16");
    for (const auto& e1 : sl_tx_interested_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_cap_info_sidelink_r16.size() > 0) {
    j.write_str("sl-CapabilityInformationSidelink-r16", sl_cap_info_sidelink_r16.to_string());
  }
  j.end_obj();
}

const char* sl_tx_res_req_r16_s::sl_cast_type_r16_opts::to_string() const
{
  static const char* names[] = {"broadcast", "groupcast", "unicast", "spare1"};
  return convert_enum_idx(names, 4, value, "sl_tx_res_req_r16_s::sl_cast_type_r16_e_");
}

// SL-TxResourceReqL2U2N-Relay-r17 ::= SEQUENCE
SRSASN_CODE sl_tx_res_req_l2_u2_n_relay_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_dest_id_l2_u2_n_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_local_id_request_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_paging_id_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_cap_info_sidelink_r17.size() > 0, 1));

  if (sl_dest_id_l2_u2_n_r17_present) {
    HANDLE_CODE(sl_dest_id_l2_u2_n_r17.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_interested_freq_list_l2_u2_n_r17, 1, 8, integer_packer<uint8_t>(1, 8)));
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_type_tx_sync_list_l2_u2_n_r17, 1, 8));
  if (sl_paging_id_remote_ue_r17_present) {
    HANDLE_CODE(sl_paging_id_remote_ue_r17.pack(bref));
  }
  if (sl_cap_info_sidelink_r17.size() > 0) {
    HANDLE_CODE(sl_cap_info_sidelink_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_res_req_l2_u2_n_relay_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_dest_id_l2_u2_n_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_local_id_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_paging_id_remote_ue_r17_present, 1));
  bool sl_cap_info_sidelink_r17_present;
  HANDLE_CODE(bref.unpack(sl_cap_info_sidelink_r17_present, 1));

  if (sl_dest_id_l2_u2_n_r17_present) {
    HANDLE_CODE(sl_dest_id_l2_u2_n_r17.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(sl_tx_interested_freq_list_l2_u2_n_r17, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  HANDLE_CODE(unpack_dyn_seq_of(sl_type_tx_sync_list_l2_u2_n_r17, bref, 1, 8));
  if (sl_paging_id_remote_ue_r17_present) {
    HANDLE_CODE(sl_paging_id_remote_ue_r17.unpack(bref));
  }
  if (sl_cap_info_sidelink_r17_present) {
    HANDLE_CODE(sl_cap_info_sidelink_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_tx_res_req_l2_u2_n_relay_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_dest_id_l2_u2_n_r17_present) {
    j.write_str("sl-DestinationIdentityL2U2N-r17", sl_dest_id_l2_u2_n_r17.to_string());
  }
  j.start_array("sl-TxInterestedFreqListL2U2N-r17");
  for (const auto& e1 : sl_tx_interested_freq_list_l2_u2_n_r17) {
    j.write_int(e1);
  }
  j.end_array();
  j.start_array("sl-TypeTxSyncListL2U2N-r17");
  for (const auto& e1 : sl_type_tx_sync_list_l2_u2_n_r17) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  if (sl_local_id_request_r17_present) {
    j.write_str("sl-LocalID-Request-r17", "true");
  }
  if (sl_paging_id_remote_ue_r17_present) {
    j.write_fieldname("sl-PagingIdentityRemoteUE-r17");
    sl_paging_id_remote_ue_r17.to_json(j);
  }
  if (sl_cap_info_sidelink_r17.size() > 0) {
    j.write_str("sl-CapabilityInformationSidelink-r17", sl_cap_info_sidelink_r17.to_string());
  }
  j.end_obj();
}

// Sensor-LocationInfo-r16 ::= SEQUENCE
SRSASN_CODE sensor_location_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sensor_meas_info_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sensor_motion_info_r16.size() > 0, 1));

  if (sensor_meas_info_r16.size() > 0) {
    HANDLE_CODE(sensor_meas_info_r16.pack(bref));
  }
  if (sensor_motion_info_r16.size() > 0) {
    HANDLE_CODE(sensor_motion_info_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sensor_location_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sensor_meas_info_r16_present;
  HANDLE_CODE(bref.unpack(sensor_meas_info_r16_present, 1));
  bool sensor_motion_info_r16_present;
  HANDLE_CODE(bref.unpack(sensor_motion_info_r16_present, 1));

  if (sensor_meas_info_r16_present) {
    HANDLE_CODE(sensor_meas_info_r16.unpack(bref));
  }
  if (sensor_motion_info_r16_present) {
    HANDLE_CODE(sensor_motion_info_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sensor_location_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sensor_meas_info_r16.size() > 0) {
    j.write_str("sensor-MeasurementInformation-r16", sensor_meas_info_r16.to_string());
  }
  if (sensor_motion_info_r16.size() > 0) {
    j.write_str("sensor-MotionInformation-r16", sensor_motion_info_r16.to_string());
  }
  j.end_obj();
}

// VisitedPSCellInfo-r17 ::= SEQUENCE
SRSASN_CODE visited_pscell_info_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(visited_cell_id_r17_present, 1));

  if (visited_cell_id_r17_present) {
    HANDLE_CODE(visited_cell_id_r17.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, time_spent_r17, (uint16_t)0u, (uint16_t)4095u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_pscell_info_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(visited_cell_id_r17_present, 1));

  if (visited_cell_id_r17_present) {
    HANDLE_CODE(visited_cell_id_r17.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(time_spent_r17, bref, (uint16_t)0u, (uint16_t)4095u));

  return SRSASN_SUCCESS;
}
void visited_pscell_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (visited_cell_id_r17_present) {
    j.write_fieldname("visitedCellId-r17");
    visited_cell_id_r17.to_json(j);
  }
  j.write_int("timeSpent-r17", time_spent_r17);
  j.end_obj();
}

void visited_pscell_info_r17_s::visited_cell_id_r17_c_::destroy_()
{
  switch (type_) {
    case types::nr_cell_id_r17:
      c.destroy<nr_cell_id_r17_c_>();
      break;
    case types::eutra_cell_id_r17:
      c.destroy<eutra_cell_id_r17_c_>();
      break;
    default:
      break;
  }
}
void visited_pscell_info_r17_s::visited_cell_id_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_cell_id_r17:
      c.init<nr_cell_id_r17_c_>();
      break;
    case types::eutra_cell_id_r17:
      c.init<eutra_cell_id_r17_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_");
  }
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_::visited_cell_id_r17_c_(
    const visited_pscell_info_r17_s::visited_cell_id_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_cell_id_r17:
      c.init(other.c.get<nr_cell_id_r17_c_>());
      break;
    case types::eutra_cell_id_r17:
      c.init(other.c.get<eutra_cell_id_r17_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_");
  }
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_& visited_pscell_info_r17_s::visited_cell_id_r17_c_::operator=(
    const visited_pscell_info_r17_s::visited_cell_id_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_cell_id_r17:
      c.set(other.c.get<nr_cell_id_r17_c_>());
      break;
    case types::eutra_cell_id_r17:
      c.set(other.c.get<eutra_cell_id_r17_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_");
  }

  return *this;
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_&
visited_pscell_info_r17_s::visited_cell_id_r17_c_::set_nr_cell_id_r17()
{
  set(types::nr_cell_id_r17);
  return c.get<nr_cell_id_r17_c_>();
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_&
visited_pscell_info_r17_s::visited_cell_id_r17_c_::set_eutra_cell_id_r17()
{
  set(types::eutra_cell_id_r17);
  return c.get<eutra_cell_id_r17_c_>();
}
void visited_pscell_info_r17_s::visited_cell_id_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_cell_id_r17:
      j.write_fieldname("nr-CellId-r17");
      c.get<nr_cell_id_r17_c_>().to_json(j);
      break;
    case types::eutra_cell_id_r17:
      j.write_fieldname("eutra-CellId-r17");
      c.get<eutra_cell_id_r17_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE visited_pscell_info_r17_s::visited_cell_id_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_cell_id_r17:
      HANDLE_CODE(c.get<nr_cell_id_r17_c_>().pack(bref));
      break;
    case types::eutra_cell_id_r17:
      HANDLE_CODE(c.get<eutra_cell_id_r17_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_pscell_info_r17_s::visited_cell_id_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_cell_id_r17:
      HANDLE_CODE(c.get<nr_cell_id_r17_c_>().unpack(bref));
      break;
    case types::eutra_cell_id_r17:
      HANDLE_CODE(c.get<eutra_cell_id_r17_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::destroy_()
{
  switch (type_) {
    case types::cgi_info_r17:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r17:
      c.destroy<pci_arfcn_nr_r16_s>();
      break;
    default:
      break;
  }
}
void visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cgi_info_r17:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r17:
      c.init<pci_arfcn_nr_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_");
  }
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::nr_cell_id_r17_c_(
    const visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cgi_info_r17:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r17:
      c.init(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_");
  }
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_&
visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::operator=(
    const visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cgi_info_r17:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r17:
      c.set(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_");
  }

  return *this;
}
cgi_info_logging_r16_s& visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::set_cgi_info_r17()
{
  set(types::cgi_info_r17);
  return c.get<cgi_info_logging_r16_s>();
}
pci_arfcn_nr_r16_s& visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::set_pci_arfcn_r17()
{
  set(types::pci_arfcn_r17);
  return c.get<pci_arfcn_nr_r16_s>();
}
void visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cgi_info_r17:
      j.write_fieldname("cgi-Info-r17");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::pci_arfcn_r17:
      j.write_fieldname("pci-arfcn-r17");
      c.get<pci_arfcn_nr_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cgi_info_r17:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cgi_info_r17:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"cgi-Info-r17", "pci-arfcn-r17"};
  return convert_enum_idx(
      names, 2, value, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::types");
}

void visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r17:
      c.destroy<cgi_info_eutra_logging_s>();
      break;
    case types::pci_arfcn_r17:
      c.destroy<pci_arfcn_eutra_r16_s>();
      break;
    default:
      break;
  }
}
void visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r17:
      c.init<cgi_info_eutra_logging_s>();
      break;
    case types::pci_arfcn_r17:
      c.init<pci_arfcn_eutra_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_");
  }
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::eutra_cell_id_r17_c_(
    const visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r17:
      c.init(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::pci_arfcn_r17:
      c.init(other.c.get<pci_arfcn_eutra_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_");
  }
}
visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_&
visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::operator=(
    const visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r17:
      c.set(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::pci_arfcn_r17:
      c.set(other.c.get<pci_arfcn_eutra_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_");
  }

  return *this;
}
cgi_info_eutra_logging_s&
visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::set_cell_global_id_r17()
{
  set(types::cell_global_id_r17);
  return c.get<cgi_info_eutra_logging_s>();
}
pci_arfcn_eutra_r16_s& visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::set_pci_arfcn_r17()
{
  set(types::pci_arfcn_r17);
  return c.get<pci_arfcn_eutra_r16_s>();
}
void visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r17:
      j.write_fieldname("cellGlobalId-r17");
      c.get<cgi_info_eutra_logging_s>().to_json(j);
      break;
    case types::pci_arfcn_r17:
      j.write_fieldname("pci-arfcn-r17");
      c.get<pci_arfcn_eutra_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r17:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().pack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_eutra_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r17:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().unpack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_eutra_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r17", "pci-arfcn-r17"};
  return convert_enum_idx(
      names, 2, value, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::types");
}

const char* visited_pscell_info_r17_s::visited_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-CellId-r17", "eutra-CellId-r17"};
  return convert_enum_idx(names, 2, value, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::types");
}

// LocationInfo-r16 ::= SEQUENCE
SRSASN_CODE location_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(common_location_info_r16_present, 1));
  HANDLE_CODE(bref.pack(bt_location_info_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(wlan_location_info_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sensor_location_info_r16_present, 1));

  if (common_location_info_r16_present) {
    HANDLE_CODE(common_location_info_r16.pack(bref));
  }
  if (bt_location_info_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bt_location_info_r16, 1, 32));
  }
  if (wlan_location_info_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_location_info_r16, 1, 32));
  }
  if (sensor_location_info_r16_present) {
    HANDLE_CODE(sensor_location_info_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE location_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(common_location_info_r16_present, 1));
  bool bt_location_info_r16_present;
  HANDLE_CODE(bref.unpack(bt_location_info_r16_present, 1));
  bool wlan_location_info_r16_present;
  HANDLE_CODE(bref.unpack(wlan_location_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(sensor_location_info_r16_present, 1));

  if (common_location_info_r16_present) {
    HANDLE_CODE(common_location_info_r16.unpack(bref));
  }
  if (bt_location_info_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bt_location_info_r16, bref, 1, 32));
  }
  if (wlan_location_info_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_location_info_r16, bref, 1, 32));
  }
  if (sensor_location_info_r16_present) {
    HANDLE_CODE(sensor_location_info_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void location_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (common_location_info_r16_present) {
    j.write_fieldname("commonLocationInfo-r16");
    common_location_info_r16.to_json(j);
  }
  if (bt_location_info_r16.size() > 0) {
    j.start_array("bt-LocationInfo-r16");
    for (const auto& e1 : bt_location_info_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (wlan_location_info_r16.size() > 0) {
    j.start_array("wlan-LocationInfo-r16");
    for (const auto& e1 : wlan_location_info_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sensor_location_info_r16_present) {
    j.write_fieldname("sensor-LocationInfo-r16");
    sensor_location_info_r16.to_json(j);
  }
  j.end_obj();
}

// MUSIM-Assistance-r17 ::= SEQUENCE
SRSASN_CODE mu_si_m_assist_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(musim_preferred_rrc_state_r17_present, 1));
  HANDLE_CODE(bref.pack(musim_gap_pref_list_r17.size() > 0, 1));

  if (musim_preferred_rrc_state_r17_present) {
    HANDLE_CODE(musim_preferred_rrc_state_r17.pack(bref));
  }
  if (musim_gap_pref_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, musim_gap_pref_list_r17, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_assist_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(musim_preferred_rrc_state_r17_present, 1));
  bool musim_gap_pref_list_r17_present;
  HANDLE_CODE(bref.unpack(musim_gap_pref_list_r17_present, 1));

  if (musim_preferred_rrc_state_r17_present) {
    HANDLE_CODE(musim_preferred_rrc_state_r17.unpack(bref));
  }
  if (musim_gap_pref_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(musim_gap_pref_list_r17, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void mu_si_m_assist_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (musim_preferred_rrc_state_r17_present) {
    j.write_str("musim-PreferredRRC-State-r17", musim_preferred_rrc_state_r17.to_string());
  }
  if (musim_gap_pref_list_r17.size() > 0) {
    j.start_array("musim-GapPreferenceList-r17");
    for (const auto& e1 : musim_gap_pref_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* mu_si_m_assist_r17_s::musim_preferred_rrc_state_r17_opts::to_string() const
{
  static const char* names[] = {"idle", "inactive", "outOfConnected"};
  return convert_enum_idx(names, 3, value, "mu_si_m_assist_r17_s::musim_preferred_rrc_state_r17_e_");
}

// MaxBW-PreferenceFR2-2-r17 ::= SEQUENCE
SRSASN_CODE max_bw_pref_fr2_2_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_bw_fr2_2_r17_present, 1));

  if (reduced_max_bw_fr2_2_r17_present) {
    HANDLE_CODE(bref.pack(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17_present, 1));
    HANDLE_CODE(bref.pack(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17_present, 1));
    if (reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17_present) {
      HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17.pack(bref));
    }
    if (reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17_present) {
      HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17.pack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_bw_pref_fr2_2_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr2_2_r17_present, 1));

  if (reduced_max_bw_fr2_2_r17_present) {
    HANDLE_CODE(bref.unpack(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17_present, 1));
    HANDLE_CODE(bref.unpack(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17_present, 1));
    if (reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17_present) {
      HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17.unpack(bref));
    }
    if (reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17_present) {
      HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17.unpack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
void max_bw_pref_fr2_2_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_bw_fr2_2_r17_present) {
    j.write_fieldname("reducedMaxBW-FR2-2-r17");
    j.start_obj();
    if (reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17_present) {
      j.write_str("reducedBW-FR2-2-DL-r17", reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17.to_string());
    }
    if (reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17_present) {
      j.write_str("reducedBW-FR2-2-UL-r17", reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17.to_string());
    }
    j.end_obj();
  }
  j.end_obj();
}

// MaxMIMO-LayerPreferenceFR2-2-r17 ::= SEQUENCE
SRSASN_CODE max_mimo_layer_pref_fr2_2_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr2_2_r17_present, 1));

  if (reduced_max_mimo_layers_fr2_2_r17_present) {
    HANDLE_CODE(pack_integer(
        bref, reduced_max_mimo_layers_fr2_2_r17.reduced_mimo_layers_fr2_2_dl_r17, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(pack_integer(
        bref, reduced_max_mimo_layers_fr2_2_r17.reduced_mimo_layers_fr2_2_ul_r17, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_mimo_layer_pref_fr2_2_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr2_2_r17_present, 1));

  if (reduced_max_mimo_layers_fr2_2_r17_present) {
    HANDLE_CODE(unpack_integer(
        reduced_max_mimo_layers_fr2_2_r17.reduced_mimo_layers_fr2_2_dl_r17, bref, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(unpack_integer(
        reduced_max_mimo_layers_fr2_2_r17.reduced_mimo_layers_fr2_2_ul_r17, bref, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
void max_mimo_layer_pref_fr2_2_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_mimo_layers_fr2_2_r17_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR2-2-r17");
    j.start_obj();
    j.write_int("reducedMIMO-LayersFR2-2-DL-r17", reduced_max_mimo_layers_fr2_2_r17.reduced_mimo_layers_fr2_2_dl_r17);
    j.write_int("reducedMIMO-LayersFR2-2-UL-r17", reduced_max_mimo_layers_fr2_2_r17.reduced_mimo_layers_fr2_2_ul_r17);
    j.end_obj();
  }
  j.end_obj();
}

// MeasResultCBR-NR-r16 ::= SEQUENCE
SRSASN_CODE meas_result_cbr_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, sl_pool_report_id_r16, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, sl_cbr_results_nr_r16, (uint8_t)0u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cbr_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(sl_pool_report_id_r16, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(sl_cbr_results_nr_r16, bref, (uint8_t)0u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
void meas_result_cbr_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-poolReportIdentity-r16", sl_pool_report_id_r16);
  j.write_int("sl-CBR-ResultsNR-r16", sl_cbr_results_nr_r16);
  j.end_obj();
}

// MeasResultCLI-RSSI-r16 ::= SEQUENCE
SRSASN_CODE meas_result_cli_rssi_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rssi_res_id_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, cli_rssi_result_r16, (uint8_t)0u, (uint8_t)76u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cli_rssi_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rssi_res_id_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(cli_rssi_result_r16, bref, (uint8_t)0u, (uint8_t)76u));

  return SRSASN_SUCCESS;
}
void meas_result_cli_rssi_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rssi-ResourceId-r16", rssi_res_id_r16);
  j.write_int("cli-RSSI-Result-r16", cli_rssi_result_r16);
  j.end_obj();
}

// MeasResultFailedCell-r16 ::= SEQUENCE
SRSASN_CODE meas_result_failed_cell_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cgi_info.pack(bref));
  HANDLE_CODE(meas_result_r16.cell_results_r16.results_ssb_cell_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_failed_cell_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cgi_info.unpack(bref));
  HANDLE_CODE(meas_result_r16.cell_results_r16.results_ssb_cell_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void meas_result_failed_cell_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cgi-Info");
  cgi_info.to_json(j);
  j.write_fieldname("measResult-r16");
  j.start_obj();
  j.write_fieldname("cellResults-r16");
  j.start_obj();
  j.write_fieldname("resultsSSB-Cell-r16");
  meas_result_r16.cell_results_r16.results_ssb_cell_r16.to_json(j);
  j.end_obj();
  j.write_fieldname("rsIndexResults-r16");
  j.start_obj();
  j.start_array("resultsSSB-Indexes-r16");
  for (const auto& e1 : meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
  j.end_obj();
  j.end_obj();
}

// MeasResultSRS-RSRP-r16 ::= SEQUENCE
SRSASN_CODE meas_result_srs_rsrp_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, srs_res_id_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, srs_rsrp_result_r16, (uint8_t)0u, (uint8_t)98u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_srs_rsrp_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(srs_res_id_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(srs_rsrp_result_r16, bref, (uint8_t)0u, (uint8_t)98u));

  return SRSASN_SUCCESS;
}
void meas_result_srs_rsrp_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srs-ResourceId-r16", srs_res_id_r16);
  j.write_int("srs-RSRP-Result-r16", srs_rsrp_result_r16);
  j.end_obj();
}

// MeasResultServingCell-r16 ::= SEQUENCE
SRSASN_CODE meas_result_serving_cell_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(results_ssb_present, 1));

  HANDLE_CODE(results_ssb_cell.pack(bref));
  if (results_ssb_present) {
    HANDLE_CODE(pack_integer(bref, results_ssb.best_ssb_idx, (uint8_t)0u, (uint8_t)63u));
    HANDLE_CODE(results_ssb.best_ssb_results.pack(bref));
    HANDLE_CODE(pack_integer(bref, results_ssb.nof_good_ssb, (uint8_t)1u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serving_cell_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(results_ssb_present, 1));

  HANDLE_CODE(results_ssb_cell.unpack(bref));
  if (results_ssb_present) {
    HANDLE_CODE(unpack_integer(results_ssb.best_ssb_idx, bref, (uint8_t)0u, (uint8_t)63u));
    HANDLE_CODE(results_ssb.best_ssb_results.unpack(bref));
    HANDLE_CODE(unpack_integer(results_ssb.nof_good_ssb, bref, (uint8_t)1u, (uint8_t)64u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_serving_cell_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("resultsSSB-Cell");
  results_ssb_cell.to_json(j);
  if (results_ssb_present) {
    j.write_fieldname("resultsSSB");
    j.start_obj();
    j.write_int("best-ssb-Index", results_ssb.best_ssb_idx);
    j.write_fieldname("best-ssb-Results");
    results_ssb.best_ssb_results.to_json(j);
    j.write_int("numberOfGoodSSB", results_ssb.nof_good_ssb);
    j.end_obj();
  }
  j.end_obj();
}

// MeasResultsPerCellIdleEUTRA-r16 ::= SEQUENCE
SRSASN_CODE meas_results_per_cell_idle_eutra_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, eutra_pci_r16, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(bref.pack(meas_idle_result_eutra_r16.rsrp_result_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_idle_result_eutra_r16.rsrq_result_eutra_r16_present, 1));
  if (meas_idle_result_eutra_r16.rsrp_result_eutra_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_idle_result_eutra_r16.rsrp_result_eutra_r16, (uint8_t)0u, (uint8_t)97u));
  }
  if (meas_idle_result_eutra_r16.rsrq_result_eutra_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_idle_result_eutra_r16.rsrq_result_eutra_r16, (int8_t)-30, (int8_t)46));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_per_cell_idle_eutra_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(eutra_pci_r16, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(bref.unpack(meas_idle_result_eutra_r16.rsrp_result_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_idle_result_eutra_r16.rsrq_result_eutra_r16_present, 1));
  if (meas_idle_result_eutra_r16.rsrp_result_eutra_r16_present) {
    HANDLE_CODE(unpack_integer(meas_idle_result_eutra_r16.rsrp_result_eutra_r16, bref, (uint8_t)0u, (uint8_t)97u));
  }
  if (meas_idle_result_eutra_r16.rsrq_result_eutra_r16_present) {
    HANDLE_CODE(unpack_integer(meas_idle_result_eutra_r16.rsrq_result_eutra_r16, bref, (int8_t)-30, (int8_t)46));
  }

  return SRSASN_SUCCESS;
}
void meas_results_per_cell_idle_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-PhysCellId-r16", eutra_pci_r16);
  j.write_fieldname("measIdleResultEUTRA-r16");
  j.start_obj();
  if (meas_idle_result_eutra_r16.rsrp_result_eutra_r16_present) {
    j.write_int("rsrp-ResultEUTRA-r16", meas_idle_result_eutra_r16.rsrp_result_eutra_r16);
  }
  if (meas_idle_result_eutra_r16.rsrq_result_eutra_r16_present) {
    j.write_int("rsrq-ResultEUTRA-r16", meas_idle_result_eutra_r16.rsrq_result_eutra_r16);
  }
  j.end_obj();
  j.end_obj();
}

// MeasResultsPerCellIdleNR-r16 ::= SEQUENCE
SRSASN_CODE meas_results_per_cell_idle_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, pci_r16, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(bref.pack(meas_idle_result_nr_r16.rsrp_result_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_idle_result_nr_r16.rsrq_result_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_idle_result_nr_r16.results_ssb_idxes_r16.size() > 0, 1));
  if (meas_idle_result_nr_r16.rsrp_result_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_idle_result_nr_r16.rsrp_result_r16, (uint8_t)0u, (uint8_t)127u));
  }
  if (meas_idle_result_nr_r16.rsrq_result_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_idle_result_nr_r16.rsrq_result_r16, (uint8_t)0u, (uint8_t)127u));
  }
  if (meas_idle_result_nr_r16.results_ssb_idxes_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_result_nr_r16.results_ssb_idxes_r16, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_per_cell_idle_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(pci_r16, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(bref.unpack(meas_idle_result_nr_r16.rsrp_result_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_idle_result_nr_r16.rsrq_result_r16_present, 1));
  bool results_ssb_idxes_r16_present;
  HANDLE_CODE(bref.unpack(results_ssb_idxes_r16_present, 1));
  if (meas_idle_result_nr_r16.rsrp_result_r16_present) {
    HANDLE_CODE(unpack_integer(meas_idle_result_nr_r16.rsrp_result_r16, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (meas_idle_result_nr_r16.rsrq_result_r16_present) {
    HANDLE_CODE(unpack_integer(meas_idle_result_nr_r16.rsrq_result_r16, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (results_ssb_idxes_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_result_nr_r16.results_ssb_idxes_r16, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void meas_results_per_cell_idle_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r16", pci_r16);
  j.write_fieldname("measIdleResultNR-r16");
  j.start_obj();
  if (meas_idle_result_nr_r16.rsrp_result_r16_present) {
    j.write_int("rsrp-Result-r16", meas_idle_result_nr_r16.rsrp_result_r16);
  }
  if (meas_idle_result_nr_r16.rsrq_result_r16_present) {
    j.write_int("rsrq-Result-r16", meas_idle_result_nr_r16.rsrq_result_r16);
  }
  if (meas_idle_result_nr_r16.results_ssb_idxes_r16.size() > 0) {
    j.start_array("resultsSSB-Indexes-r16");
    for (const auto& e1 : meas_idle_result_nr_r16.results_ssb_idxes_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
  j.end_obj();
}

// MinSchedulingOffsetPreferenceExt-r17 ::= SEQUENCE
SRSASN_CODE min_sched_offset_pref_ext_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(preferred_k0_r17_present, 1));
  HANDLE_CODE(bref.pack(preferred_k2_r17_present, 1));

  if (preferred_k0_r17_present) {
    HANDLE_CODE(bref.pack(preferred_k0_r17.preferred_k0_scs_480k_hz_r17_present, 1));
    HANDLE_CODE(bref.pack(preferred_k0_r17.preferred_k0_scs_960k_hz_r17_present, 1));
    if (preferred_k0_r17.preferred_k0_scs_480k_hz_r17_present) {
      HANDLE_CODE(preferred_k0_r17.preferred_k0_scs_480k_hz_r17.pack(bref));
    }
    if (preferred_k0_r17.preferred_k0_scs_960k_hz_r17_present) {
      HANDLE_CODE(preferred_k0_r17.preferred_k0_scs_960k_hz_r17.pack(bref));
    }
  }
  if (preferred_k2_r17_present) {
    HANDLE_CODE(bref.pack(preferred_k2_r17.preferred_k2_scs_480k_hz_r17_present, 1));
    HANDLE_CODE(bref.pack(preferred_k2_r17.preferred_k2_scs_960k_hz_r17_present, 1));
    if (preferred_k2_r17.preferred_k2_scs_480k_hz_r17_present) {
      HANDLE_CODE(preferred_k2_r17.preferred_k2_scs_480k_hz_r17.pack(bref));
    }
    if (preferred_k2_r17.preferred_k2_scs_960k_hz_r17_present) {
      HANDLE_CODE(preferred_k2_r17.preferred_k2_scs_960k_hz_r17.pack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE min_sched_offset_pref_ext_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(preferred_k0_r17_present, 1));
  HANDLE_CODE(bref.unpack(preferred_k2_r17_present, 1));

  if (preferred_k0_r17_present) {
    HANDLE_CODE(bref.unpack(preferred_k0_r17.preferred_k0_scs_480k_hz_r17_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k0_r17.preferred_k0_scs_960k_hz_r17_present, 1));
    if (preferred_k0_r17.preferred_k0_scs_480k_hz_r17_present) {
      HANDLE_CODE(preferred_k0_r17.preferred_k0_scs_480k_hz_r17.unpack(bref));
    }
    if (preferred_k0_r17.preferred_k0_scs_960k_hz_r17_present) {
      HANDLE_CODE(preferred_k0_r17.preferred_k0_scs_960k_hz_r17.unpack(bref));
    }
  }
  if (preferred_k2_r17_present) {
    HANDLE_CODE(bref.unpack(preferred_k2_r17.preferred_k2_scs_480k_hz_r17_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k2_r17.preferred_k2_scs_960k_hz_r17_present, 1));
    if (preferred_k2_r17.preferred_k2_scs_480k_hz_r17_present) {
      HANDLE_CODE(preferred_k2_r17.preferred_k2_scs_480k_hz_r17.unpack(bref));
    }
    if (preferred_k2_r17.preferred_k2_scs_960k_hz_r17_present) {
      HANDLE_CODE(preferred_k2_r17.preferred_k2_scs_960k_hz_r17.unpack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
void min_sched_offset_pref_ext_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (preferred_k0_r17_present) {
    j.write_fieldname("preferredK0-r17");
    j.start_obj();
    if (preferred_k0_r17.preferred_k0_scs_480k_hz_r17_present) {
      j.write_str("preferredK0-SCS-480kHz-r17", preferred_k0_r17.preferred_k0_scs_480k_hz_r17.to_string());
    }
    if (preferred_k0_r17.preferred_k0_scs_960k_hz_r17_present) {
      j.write_str("preferredK0-SCS-960kHz-r17", preferred_k0_r17.preferred_k0_scs_960k_hz_r17.to_string());
    }
    j.end_obj();
  }
  if (preferred_k2_r17_present) {
    j.write_fieldname("preferredK2-r17");
    j.start_obj();
    if (preferred_k2_r17.preferred_k2_scs_480k_hz_r17_present) {
      j.write_str("preferredK2-SCS-480kHz-r17", preferred_k2_r17.preferred_k2_scs_480k_hz_r17.to_string());
    }
    if (preferred_k2_r17.preferred_k2_scs_960k_hz_r17_present) {
      j.write_str("preferredK2-SCS-960kHz-r17", preferred_k2_r17.preferred_k2_scs_960k_hz_r17.to_string());
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_480k_hz_r17_opts::to_string() const
{
  static const char* names[] = {"sl8", "sl16", "sl32", "sl48"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_480k_hz_r17_e_");
}
uint8_t min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_480k_hz_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 16, 32, 48};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_480k_hz_r17_e_");
}

const char* min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_960k_hz_r17_opts::to_string() const
{
  static const char* names[] = {"sl8", "sl16", "sl32", "sl48"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_960k_hz_r17_e_");
}
uint8_t min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_960k_hz_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 16, 32, 48};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k0_r17_s_::preferred_k0_scs_960k_hz_r17_e_");
}

const char* min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_480k_hz_r17_opts::to_string() const
{
  static const char* names[] = {"sl8", "sl16", "sl32", "sl48"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_480k_hz_r17_e_");
}
uint8_t min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_480k_hz_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 16, 32, 48};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_480k_hz_r17_e_");
}

const char* min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_960k_hz_r17_opts::to_string() const
{
  static const char* names[] = {"sl8", "sl16", "sl32", "sl48"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_960k_hz_r17_e_");
}
uint8_t min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_960k_hz_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 16, 32, 48};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_ext_r17_s::preferred_k2_r17_s_::preferred_k2_scs_960k_hz_r17_e_");
}

// NeedForGapsInfoNR-r16 ::= SEQUENCE
SRSASN_CODE need_for_gaps_info_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_need_for_gap_r16, 1, 32));
  HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_need_for_gap_r16, 1, 1024));

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gaps_info_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(intra_freq_need_for_gap_r16, bref, 1, 32));
  HANDLE_CODE(unpack_dyn_seq_of(inter_freq_need_for_gap_r16, bref, 1, 1024));

  return SRSASN_SUCCESS;
}
void need_for_gaps_info_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("intraFreq-needForGap-r16");
  for (const auto& e1 : intra_freq_need_for_gap_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("interFreq-needForGap-r16");
  for (const auto& e1 : inter_freq_need_for_gap_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// OverheatingAssistance-r17 ::= SEQUENCE
SRSASN_CODE overheat_assist_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_bw_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr2_2_present, 1));

  if (reduced_max_bw_fr2_2_r17_present) {
    HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17.pack(bref));
    HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17.pack(bref));
  }
  if (reduced_max_mimo_layers_fr2_2_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr2_2.reduced_mimo_layers_fr2_2_dl.pack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr2_2.reduced_mimo_layers_fr2_2_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE overheat_assist_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr2_2_present, 1));

  if (reduced_max_bw_fr2_2_r17_present) {
    HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17.unpack(bref));
    HANDLE_CODE(reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17.unpack(bref));
  }
  if (reduced_max_mimo_layers_fr2_2_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr2_2.reduced_mimo_layers_fr2_2_dl.unpack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr2_2.reduced_mimo_layers_fr2_2_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void overheat_assist_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_bw_fr2_2_r17_present) {
    j.write_fieldname("reducedMaxBW-FR2-2-r17");
    j.start_obj();
    j.write_str("reducedBW-FR2-2-DL-r17", reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_dl_r17.to_string());
    j.write_str("reducedBW-FR2-2-UL-r17", reduced_max_bw_fr2_2_r17.reduced_bw_fr2_2_ul_r17.to_string());
    j.end_obj();
  }
  if (reduced_max_mimo_layers_fr2_2_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR2-2");
    j.start_obj();
    j.write_str("reducedMIMO-LayersFR2-2-DL", reduced_max_mimo_layers_fr2_2.reduced_mimo_layers_fr2_2_dl.to_string());
    j.write_str("reducedMIMO-LayersFR2-2-UL", reduced_max_mimo_layers_fr2_2.reduced_mimo_layers_fr2_2_ul.to_string());
    j.end_obj();
  }
  j.end_obj();
}

// RRCReconfigurationComplete-v1640-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1640_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_tx_direct_current_two_carrier_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_two_carrier_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_two_carrier_list_r16, 1, 64));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1640_ies_s::unpack(cbit_ref& bref)
{
  bool ul_tx_direct_current_two_carrier_list_r16_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_two_carrier_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_two_carrier_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_two_carrier_list_r16, bref, 1, 64));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1640_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_tx_direct_current_two_carrier_list_r16.size() > 0) {
    j.start_array("uplinkTxDirectCurrentTwoCarrierList-r16");
    for (const auto& e1 : ul_tx_direct_current_two_carrier_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCResumeComplete-v1720-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_v1720_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_tx_direct_current_more_carrier_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_more_carrier_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_more_carrier_list_r17, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_v1720_ies_s::unpack(cbit_ref& bref)
{
  bool ul_tx_direct_current_more_carrier_list_r17_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_more_carrier_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_more_carrier_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_more_carrier_list_r17, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_v1720_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_tx_direct_current_more_carrier_list_r17.size() > 0) {
    j.start_array("uplinkTxDirectCurrentMoreCarrierList-r17");
    for (const auto& e1 : ul_tx_direct_current_more_carrier_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// ReducedMaxBW-FRx-r16 ::= SEQUENCE
SRSASN_CODE reduced_max_bw_f_rx_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(reduced_bw_dl_r16.pack(bref));
  HANDLE_CODE(reduced_bw_ul_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reduced_max_bw_f_rx_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(reduced_bw_dl_r16.unpack(bref));
  HANDLE_CODE(reduced_bw_ul_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void reduced_max_bw_f_rx_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reducedBW-DL-r16", reduced_bw_dl_r16.to_string());
  j.write_str("reducedBW-UL-r16", reduced_bw_ul_r16.to_string());
  j.end_obj();
}

// ReducedMaxCCs-r16 ::= SEQUENCE
SRSASN_CODE reduced_max_ccs_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, reduced_ccs_dl_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, reduced_ccs_ul_r16, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reduced_max_ccs_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(reduced_ccs_dl_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(reduced_ccs_ul_r16, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void reduced_max_ccs_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reducedCCsDL-r16", reduced_ccs_dl_r16);
  j.write_int("reducedCCsUL-r16", reduced_ccs_ul_r16);
  j.end_obj();
}

// SIB-Type-r17 ::= ENUMERATED
const char* sib_type_r17_opts::to_string() const
{
  static const char* names[] = {"sibType2",
                                "sibType3",
                                "sibType4",
                                "sibType5",
                                "sibType9",
                                "sibType10-v1610",
                                "sibType11-v1610",
                                "sibType12-v1610",
                                "sibType13-v1610",
                                "sibType14-v1610",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sib_type_r17_e");
}
uint8_t sib_type_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 5, 9, 10, 11, 12, 13, 14};
  return map_enum_number(numbers, 10, value, "sib_type_r17_e");
}

// SL-TrafficPatternInfo-r16 ::= SEQUENCE
SRSASN_CODE sl_traffic_pattern_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(traffic_periodicity_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, timing_offset_r16, (uint16_t)0u, (uint16_t)10239u));
  HANDLE_CODE(msg_size_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, sl_qos_flow_id_r16, (uint16_t)1u, (uint16_t)2048u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_traffic_pattern_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(traffic_periodicity_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(timing_offset_r16, bref, (uint16_t)0u, (uint16_t)10239u));
  HANDLE_CODE(msg_size_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(sl_qos_flow_id_r16, bref, (uint16_t)1u, (uint16_t)2048u));

  return SRSASN_SUCCESS;
}
void sl_traffic_pattern_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("trafficPeriodicity-r16", traffic_periodicity_r16.to_string());
  j.write_int("timingOffset-r16", timing_offset_r16);
  j.write_str("messageSize-r16", msg_size_r16.to_string());
  j.write_int("sl-QoS-FlowIdentity-r16", sl_qos_flow_id_r16);
  j.end_obj();
}

const char* sl_traffic_pattern_info_r16_s::traffic_periodicity_r16_opts::to_string() const
{
  static const char* names[] = {
      "ms20", "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms700", "ms800", "ms900", "ms1000"};
  return convert_enum_idx(names, 12, value, "sl_traffic_pattern_info_r16_s::traffic_periodicity_r16_e_");
}
uint16_t sl_traffic_pattern_info_r16_s::traffic_periodicity_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(numbers, 12, value, "sl_traffic_pattern_info_r16_s::traffic_periodicity_r16_e_");
}

// SL-TxResourceReq-v1700 ::= SEQUENCE
SRSASN_CODE sl_tx_res_req_v1700_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_drx_info_from_rx_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_drx_ind_r17_present, 1));

  if (sl_drx_info_from_rx_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drx_info_from_rx_list_r17, 1, 4));
  }
  if (sl_drx_ind_r17_present) {
    HANDLE_CODE(sl_drx_ind_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_res_req_v1700_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_drx_info_from_rx_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_drx_info_from_rx_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_drx_ind_r17_present, 1));

  if (sl_drx_info_from_rx_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_drx_info_from_rx_list_r17, bref, 1, 4));
  }
  if (sl_drx_ind_r17_present) {
    HANDLE_CODE(sl_drx_ind_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_tx_res_req_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_drx_info_from_rx_list_r17.size() > 0) {
    j.start_array("sl-DRX-InfoFromRxList-r17");
    for (const auto& e1 : sl_drx_info_from_rx_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drx_ind_r17_present) {
    j.write_str("sl-DRX-Indication-r17", sl_drx_ind_r17.to_string());
  }
  j.end_obj();
}

const char* sl_tx_res_req_v1700_s::sl_drx_ind_r17_opts::to_string() const
{
  static const char* names[] = {"on", "off"};
  return convert_enum_idx(names, 2, value, "sl_tx_res_req_v1700_s::sl_drx_ind_r17_e_");
}

// SL-TxResourceReqCommRelay-r17 ::= CHOICE
void sl_tx_res_req_comm_relay_r17_c::destroy_()
{
  switch (type_) {
    case types::sl_tx_res_req_l2_u2_n_relay_r17:
      c.destroy<sl_tx_res_req_l2_u2_n_relay_r17_s>();
      break;
    case types::sl_tx_res_req_l3_u2_n_relay_r17:
      c.destroy<sl_tx_res_req_r16_s>();
      break;
    default:
      break;
  }
}
void sl_tx_res_req_comm_relay_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sl_tx_res_req_l2_u2_n_relay_r17:
      c.init<sl_tx_res_req_l2_u2_n_relay_r17_s>();
      break;
    case types::sl_tx_res_req_l3_u2_n_relay_r17:
      c.init<sl_tx_res_req_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_res_req_comm_relay_r17_c");
  }
}
sl_tx_res_req_comm_relay_r17_c::sl_tx_res_req_comm_relay_r17_c(const sl_tx_res_req_comm_relay_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl_tx_res_req_l2_u2_n_relay_r17:
      c.init(other.c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>());
      break;
    case types::sl_tx_res_req_l3_u2_n_relay_r17:
      c.init(other.c.get<sl_tx_res_req_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_res_req_comm_relay_r17_c");
  }
}
sl_tx_res_req_comm_relay_r17_c& sl_tx_res_req_comm_relay_r17_c::operator=(const sl_tx_res_req_comm_relay_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl_tx_res_req_l2_u2_n_relay_r17:
      c.set(other.c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>());
      break;
    case types::sl_tx_res_req_l3_u2_n_relay_r17:
      c.set(other.c.get<sl_tx_res_req_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_res_req_comm_relay_r17_c");
  }

  return *this;
}
sl_tx_res_req_l2_u2_n_relay_r17_s& sl_tx_res_req_comm_relay_r17_c::set_sl_tx_res_req_l2_u2_n_relay_r17()
{
  set(types::sl_tx_res_req_l2_u2_n_relay_r17);
  return c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>();
}
sl_tx_res_req_r16_s& sl_tx_res_req_comm_relay_r17_c::set_sl_tx_res_req_l3_u2_n_relay_r17()
{
  set(types::sl_tx_res_req_l3_u2_n_relay_r17);
  return c.get<sl_tx_res_req_r16_s>();
}
void sl_tx_res_req_comm_relay_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl_tx_res_req_l2_u2_n_relay_r17:
      j.write_fieldname("sl-TxResourceReqL2U2N-Relay-r17");
      c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>().to_json(j);
      break;
    case types::sl_tx_res_req_l3_u2_n_relay_r17:
      j.write_fieldname("sl-TxResourceReqL3U2N-Relay-r17");
      c.get<sl_tx_res_req_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_res_req_comm_relay_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_tx_res_req_comm_relay_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl_tx_res_req_l2_u2_n_relay_r17:
      HANDLE_CODE(c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>().pack(bref));
      break;
    case types::sl_tx_res_req_l3_u2_n_relay_r17:
      HANDLE_CODE(c.get<sl_tx_res_req_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_res_req_comm_relay_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_res_req_comm_relay_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl_tx_res_req_l2_u2_n_relay_r17:
      HANDLE_CODE(c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>().unpack(bref));
      break;
    case types::sl_tx_res_req_l3_u2_n_relay_r17:
      HANDLE_CODE(c.get<sl_tx_res_req_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_res_req_comm_relay_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_tx_res_req_comm_relay_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-TxResourceReqL2U2N-Relay-r17", "sl-TxResourceReqL3U2N-Relay-r17"};
  return convert_enum_idx(names, 2, value, "sl_tx_res_req_comm_relay_r17_c::types");
}
uint8_t sl_tx_res_req_comm_relay_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_tx_res_req_comm_relay_r17_c::types");
}

// UE-MeasurementsAvailable-r16 ::= SEQUENCE
SRSASN_CODE ue_meass_available_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(log_meas_available_r16_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_bt_r16_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_wlan_r16_present, 1));
  HANDLE_CODE(bref.pack(conn_est_fail_info_available_r16_present, 1));
  HANDLE_CODE(bref.pack(rlf_info_available_r16_present, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= success_ho_info_available_r17_present;
    group_flags[0] |= sig_log_meas_cfg_available_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(success_ho_info_available_r17_present, 1));
      HANDLE_CODE(bref.pack(sig_log_meas_cfg_available_r17_present, 1));
      if (sig_log_meas_cfg_available_r17_present) {
        HANDLE_CODE(bref.pack(sig_log_meas_cfg_available_r17, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_meass_available_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(log_meas_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_bt_r16_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_wlan_r16_present, 1));
  HANDLE_CODE(bref.unpack(conn_est_fail_info_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(rlf_info_available_r16_present, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(success_ho_info_available_r17_present, 1));
      HANDLE_CODE(bref.unpack(sig_log_meas_cfg_available_r17_present, 1));
      if (sig_log_meas_cfg_available_r17_present) {
        HANDLE_CODE(bref.unpack(sig_log_meas_cfg_available_r17, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ue_meass_available_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (log_meas_available_r16_present) {
    j.write_str("logMeasAvailable-r16", "true");
  }
  if (log_meas_available_bt_r16_present) {
    j.write_str("logMeasAvailableBT-r16", "true");
  }
  if (log_meas_available_wlan_r16_present) {
    j.write_str("logMeasAvailableWLAN-r16", "true");
  }
  if (conn_est_fail_info_available_r16_present) {
    j.write_str("connEstFailInfoAvailable-r16", "true");
  }
  if (rlf_info_available_r16_present) {
    j.write_str("rlf-InfoAvailable-r16", "true");
  }
  if (ext) {
    if (success_ho_info_available_r17_present) {
      j.write_str("successHO-InfoAvailable-r17", "true");
    }
    if (sig_log_meas_cfg_available_r17_present) {
      j.write_bool("sigLogMeasConfigAvailable-r17", sig_log_meas_cfg_available_r17);
    }
  }
  j.end_obj();
}

// UL-GapFR2-Preference-r17 ::= SEQUENCE
SRSASN_CODE ul_gap_fr2_pref_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_gap_fr2_pattern_pref_r17_present, 1));

  if (ul_gap_fr2_pattern_pref_r17_present) {
    HANDLE_CODE(pack_integer(bref, ul_gap_fr2_pattern_pref_r17, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_gap_fr2_pref_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_gap_fr2_pattern_pref_r17_present, 1));

  if (ul_gap_fr2_pattern_pref_r17_present) {
    HANDLE_CODE(unpack_integer(ul_gap_fr2_pattern_pref_r17, bref, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
void ul_gap_fr2_pref_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_gap_fr2_pattern_pref_r17_present) {
    j.write_int("ul-GapFR2-PatternPreference-r17", ul_gap_fr2_pattern_pref_r17);
  }
  j.end_obj();
}

// UplinkTxDirectCurrentBWP ::= SEQUENCE
SRSASN_CODE ul_tx_direct_current_bwp_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, bwp_id, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(bref.pack(shift7dot5k_hz, 1));
  HANDLE_CODE(pack_integer(bref, tx_direct_current_location, (uint16_t)0u, (uint16_t)3301u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_bwp_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(bref.unpack(shift7dot5k_hz, 1));
  HANDLE_CODE(unpack_integer(tx_direct_current_location, bref, (uint16_t)0u, (uint16_t)3301u));

  return SRSASN_SUCCESS;
}
void ul_tx_direct_current_bwp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bwp-Id", bwp_id);
  j.write_bool("shift7dot5kHz", shift7dot5k_hz);
  j.write_int("txDirectCurrentLocation", tx_direct_current_location);
  j.end_obj();
}

// ChoCandidateCell-r17 ::= CHOICE
void cho_candidate_cell_r17_c::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r17:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r17:
      c.destroy<pci_arfcn_nr_r16_s>();
      break;
    default:
      break;
  }
}
void cho_candidate_cell_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r17:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r17:
      c.init<pci_arfcn_nr_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cho_candidate_cell_r17_c");
  }
}
cho_candidate_cell_r17_c::cho_candidate_cell_r17_c(const cho_candidate_cell_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r17:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r17:
      c.init(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cho_candidate_cell_r17_c");
  }
}
cho_candidate_cell_r17_c& cho_candidate_cell_r17_c::operator=(const cho_candidate_cell_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r17:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r17:
      c.set(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cho_candidate_cell_r17_c");
  }

  return *this;
}
cgi_info_logging_r16_s& cho_candidate_cell_r17_c::set_cell_global_id_r17()
{
  set(types::cell_global_id_r17);
  return c.get<cgi_info_logging_r16_s>();
}
pci_arfcn_nr_r16_s& cho_candidate_cell_r17_c::set_pci_arfcn_r17()
{
  set(types::pci_arfcn_r17);
  return c.get<pci_arfcn_nr_r16_s>();
}
void cho_candidate_cell_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r17:
      j.write_fieldname("cellGlobalId-r17");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::pci_arfcn_r17:
      j.write_fieldname("pci-arfcn-r17");
      c.get<pci_arfcn_nr_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "cho_candidate_cell_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE cho_candidate_cell_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r17:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cho_candidate_cell_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cho_candidate_cell_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r17:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cho_candidate_cell_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cho_candidate_cell_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r17", "pci-arfcn-r17"};
  return convert_enum_idx(names, 2, value, "cho_candidate_cell_r17_c::types");
}

// ConnEstFailReport-r16 ::= SEQUENCE
SRSASN_CODE conn_est_fail_report_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(location_info_r16_present, 1));

  HANDLE_CODE(meas_result_failed_cell_r16.pack(bref));
  if (location_info_r16_present) {
    HANDLE_CODE(location_info_r16.pack(bref));
  }
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra.size() > 0, 1));
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr, 1, 8));
  }
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra, 1, 8));
  }
  HANDLE_CODE(pack_integer(bref, nof_conn_fail_r16, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_dyn_seq_of(bref, per_ra_info_list_r16, 1, 200));
  HANDLE_CODE(pack_integer(bref, time_since_fail_r16, (uint32_t)0u, (uint32_t)172800u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE conn_est_fail_report_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(location_info_r16_present, 1));

  HANDLE_CODE(meas_result_failed_cell_r16.unpack(bref));
  if (location_info_r16_present) {
    HANDLE_CODE(location_info_r16.unpack(bref));
  }
  bool meas_result_neigh_cell_list_nr_present;
  HANDLE_CODE(bref.unpack(meas_result_neigh_cell_list_nr_present, 1));
  bool meas_result_neigh_cell_list_eutra_present;
  HANDLE_CODE(bref.unpack(meas_result_neigh_cell_list_eutra_present, 1));
  if (meas_result_neigh_cell_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr, bref, 1, 8));
  }
  if (meas_result_neigh_cell_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra, bref, 1, 8));
  }
  HANDLE_CODE(unpack_integer(nof_conn_fail_r16, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_dyn_seq_of(per_ra_info_list_r16, bref, 1, 200));
  HANDLE_CODE(unpack_integer(time_since_fail_r16, bref, (uint32_t)0u, (uint32_t)172800u));

  return SRSASN_SUCCESS;
}
void conn_est_fail_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResultFailedCell-r16");
  meas_result_failed_cell_r16.to_json(j);
  if (location_info_r16_present) {
    j.write_fieldname("locationInfo-r16");
    location_info_r16.to_json(j);
  }
  j.write_fieldname("measResultNeighCells-r16");
  j.start_obj();
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr.size() > 0) {
    j.start_array("measResultNeighCellListNR");
    for (const auto& e1 : meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra.size() > 0) {
    j.start_array("measResultNeighCellListEUTRA");
    for (const auto& e1 : meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
  j.write_int("numberOfConnFail-r16", nof_conn_fail_r16);
  j.start_array("perRAInfoList-r16");
  for (const auto& e1 : per_ra_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("timeSinceFailure-r16", time_since_fail_r16);
  j.end_obj();
}

// DRX-Preference-r16 ::= SEQUENCE
SRSASN_CODE drx_pref_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(preferred_drx_inactivity_timer_r16_present, 1));
  HANDLE_CODE(bref.pack(preferred_drx_long_cycle_r16_present, 1));
  HANDLE_CODE(bref.pack(preferred_drx_short_cycle_r16_present, 1));
  HANDLE_CODE(bref.pack(preferred_drx_short_cycle_timer_r16_present, 1));

  if (preferred_drx_inactivity_timer_r16_present) {
    HANDLE_CODE(preferred_drx_inactivity_timer_r16.pack(bref));
  }
  if (preferred_drx_long_cycle_r16_present) {
    HANDLE_CODE(preferred_drx_long_cycle_r16.pack(bref));
  }
  if (preferred_drx_short_cycle_r16_present) {
    HANDLE_CODE(preferred_drx_short_cycle_r16.pack(bref));
  }
  if (preferred_drx_short_cycle_timer_r16_present) {
    HANDLE_CODE(pack_integer(bref, preferred_drx_short_cycle_timer_r16, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_pref_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(preferred_drx_inactivity_timer_r16_present, 1));
  HANDLE_CODE(bref.unpack(preferred_drx_long_cycle_r16_present, 1));
  HANDLE_CODE(bref.unpack(preferred_drx_short_cycle_r16_present, 1));
  HANDLE_CODE(bref.unpack(preferred_drx_short_cycle_timer_r16_present, 1));

  if (preferred_drx_inactivity_timer_r16_present) {
    HANDLE_CODE(preferred_drx_inactivity_timer_r16.unpack(bref));
  }
  if (preferred_drx_long_cycle_r16_present) {
    HANDLE_CODE(preferred_drx_long_cycle_r16.unpack(bref));
  }
  if (preferred_drx_short_cycle_r16_present) {
    HANDLE_CODE(preferred_drx_short_cycle_r16.unpack(bref));
  }
  if (preferred_drx_short_cycle_timer_r16_present) {
    HANDLE_CODE(unpack_integer(preferred_drx_short_cycle_timer_r16, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void drx_pref_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (preferred_drx_inactivity_timer_r16_present) {
    j.write_str("preferredDRX-InactivityTimer-r16", preferred_drx_inactivity_timer_r16.to_string());
  }
  if (preferred_drx_long_cycle_r16_present) {
    j.write_str("preferredDRX-LongCycle-r16", preferred_drx_long_cycle_r16.to_string());
  }
  if (preferred_drx_short_cycle_r16_present) {
    j.write_str("preferredDRX-ShortCycle-r16", preferred_drx_short_cycle_r16.to_string());
  }
  if (preferred_drx_short_cycle_timer_r16_present) {
    j.write_int("preferredDRX-ShortCycleTimer-r16", preferred_drx_short_cycle_timer_r16);
  }
  j.end_obj();
}

const char* drx_pref_r16_s::preferred_drx_inactivity_timer_r16_opts::to_string() const
{
  static const char* names[] = {"ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",
                                "ms10",   "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",
                                "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_pref_r16_s::preferred_drx_inactivity_timer_r16_e_");
}
uint16_t drx_pref_r16_s::preferred_drx_inactivity_timer_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "drx_pref_r16_s::preferred_drx_inactivity_timer_r16_e_");
}

const char* drx_pref_r16_s::preferred_drx_long_cycle_r16_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",    "ms60",    "ms64",    "ms70",    "ms80",
                                "ms128",  "ms160",  "ms256",  "ms320",   "ms512",   "ms640",   "ms1024",  "ms1280",
                                "ms2048", "ms2560", "ms5120", "ms10240", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7", "spare6", "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "drx_pref_r16_s::preferred_drx_long_cycle_r16_e_");
}
uint16_t drx_pref_r16_s::preferred_drx_long_cycle_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "drx_pref_r16_s::preferred_drx_long_cycle_r16_e_");
}

const char* drx_pref_r16_s::preferred_drx_short_cycle_r16_opts::to_string() const
{
  static const char* names[] = {"ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms7",    "ms8",    "ms10",
                                "ms14",   "ms16",   "ms20",   "ms30",   "ms32",   "ms35",   "ms40",   "ms64",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",  "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_pref_r16_s::preferred_drx_short_cycle_r16_e_");
}
uint16_t drx_pref_r16_s::preferred_drx_short_cycle_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {2,  3,  4,  5,  6,  7,   8,   10,  14,  16,  20, 30,
                                     32, 35, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(numbers, 23, value, "drx_pref_r16_s::preferred_drx_short_cycle_r16_e_");
}

// EUTRA-RSTD-Info ::= SEQUENCE
SRSASN_CODE eutra_rstd_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, meas_prs_offset, (uint8_t)0u, (uint8_t)39u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_rstd_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(meas_prs_offset, bref, (uint8_t)0u, (uint8_t)39u));

  return SRSASN_SUCCESS;
}
void eutra_rstd_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_int("measPRS-Offset", meas_prs_offset);
  j.end_obj();
}

// IDC-Assistance-r16 ::= SEQUENCE
SRSASN_CODE idc_assist_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(affected_carrier_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(affected_carrier_freq_comb_list_r16.size() > 0, 1));

  if (affected_carrier_freq_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_list_r16, 1, 128));
  }
  if (affected_carrier_freq_comb_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_comb_list_r16, 1, 128));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE idc_assist_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool affected_carrier_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(affected_carrier_freq_list_r16_present, 1));
  bool affected_carrier_freq_comb_list_r16_present;
  HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_list_r16_present, 1));

  if (affected_carrier_freq_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_list_r16, bref, 1, 128));
  }
  if (affected_carrier_freq_comb_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_list_r16, bref, 1, 128));
  }

  return SRSASN_SUCCESS;
}
void idc_assist_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (affected_carrier_freq_list_r16.size() > 0) {
    j.start_array("affectedCarrierFreqList-r16");
    for (const auto& e1 : affected_carrier_freq_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (affected_carrier_freq_comb_list_r16.size() > 0) {
    j.start_array("affectedCarrierFreqCombList-r16");
    for (const auto& e1 : affected_carrier_freq_comb_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// LogMeasInfo-r16 ::= SEQUENCE
SRSASN_CODE log_meas_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(location_info_r16_present, 1));
  HANDLE_CODE(bref.pack(serv_cell_id_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_serving_cell_r16_present, 1));
  HANDLE_CODE(bref.pack(any_cell_sel_detected_r16_present, 1));

  if (location_info_r16_present) {
    HANDLE_CODE(location_info_r16.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, relative_time_stamp_r16, (uint16_t)0u, (uint16_t)7200u));
  if (serv_cell_id_r16_present) {
    HANDLE_CODE(serv_cell_id_r16.pack(bref));
  }
  if (meas_result_serving_cell_r16_present) {
    HANDLE_CODE(meas_result_serving_cell_r16.pack(bref));
  }
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra.size() > 0, 1));
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr, 1, 8));
  }
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= in_dev_coex_detected_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(in_dev_coex_detected_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(location_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(serv_cell_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_serving_cell_r16_present, 1));
  HANDLE_CODE(bref.unpack(any_cell_sel_detected_r16_present, 1));

  if (location_info_r16_present) {
    HANDLE_CODE(location_info_r16.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(relative_time_stamp_r16, bref, (uint16_t)0u, (uint16_t)7200u));
  if (serv_cell_id_r16_present) {
    HANDLE_CODE(serv_cell_id_r16.unpack(bref));
  }
  if (meas_result_serving_cell_r16_present) {
    HANDLE_CODE(meas_result_serving_cell_r16.unpack(bref));
  }
  bool meas_result_neigh_cell_list_nr_present;
  HANDLE_CODE(bref.unpack(meas_result_neigh_cell_list_nr_present, 1));
  bool meas_result_neigh_cell_list_eutra_present;
  HANDLE_CODE(bref.unpack(meas_result_neigh_cell_list_eutra_present, 1));
  if (meas_result_neigh_cell_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr, bref, 1, 8));
  }
  if (meas_result_neigh_cell_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(in_dev_coex_detected_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void log_meas_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (location_info_r16_present) {
    j.write_fieldname("locationInfo-r16");
    location_info_r16.to_json(j);
  }
  j.write_int("relativeTimeStamp-r16", relative_time_stamp_r16);
  if (serv_cell_id_r16_present) {
    j.write_fieldname("servCellIdentity-r16");
    serv_cell_id_r16.to_json(j);
  }
  if (meas_result_serving_cell_r16_present) {
    j.write_fieldname("measResultServingCell-r16");
    meas_result_serving_cell_r16.to_json(j);
  }
  j.write_fieldname("measResultNeighCells-r16");
  j.start_obj();
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr.size() > 0) {
    j.start_array("measResultNeighCellListNR");
    for (const auto& e1 : meas_result_neigh_cells_r16.meas_result_neigh_cell_list_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra.size() > 0) {
    j.start_array("measResultNeighCellListEUTRA");
    for (const auto& e1 : meas_result_neigh_cells_r16.meas_result_neigh_cell_list_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
  if (any_cell_sel_detected_r16_present) {
    j.write_str("anyCellSelectionDetected-r16", "true");
  }
  if (ext) {
    if (in_dev_coex_detected_r17_present) {
      j.write_str("inDeviceCoexDetected-r17", "true");
    }
  }
  j.end_obj();
}

// MaxBW-Preference-r16 ::= SEQUENCE
SRSASN_CODE max_bw_pref_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_bw_fr1_r16_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_bw_fr2_r16_present, 1));

  if (reduced_max_bw_fr1_r16_present) {
    HANDLE_CODE(reduced_max_bw_fr1_r16.pack(bref));
  }
  if (reduced_max_bw_fr2_r16_present) {
    HANDLE_CODE(reduced_max_bw_fr2_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_bw_pref_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr1_r16_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr2_r16_present, 1));

  if (reduced_max_bw_fr1_r16_present) {
    HANDLE_CODE(reduced_max_bw_fr1_r16.unpack(bref));
  }
  if (reduced_max_bw_fr2_r16_present) {
    HANDLE_CODE(reduced_max_bw_fr2_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void max_bw_pref_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_bw_fr1_r16_present) {
    j.write_fieldname("reducedMaxBW-FR1-r16");
    reduced_max_bw_fr1_r16.to_json(j);
  }
  if (reduced_max_bw_fr2_r16_present) {
    j.write_fieldname("reducedMaxBW-FR2-r16");
    reduced_max_bw_fr2_r16.to_json(j);
  }
  j.end_obj();
}

// MaxCC-Preference-r16 ::= SEQUENCE
SRSASN_CODE max_cc_pref_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_ccs_r16_present, 1));

  if (reduced_max_ccs_r16_present) {
    HANDLE_CODE(reduced_max_ccs_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_cc_pref_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_ccs_r16_present, 1));

  if (reduced_max_ccs_r16_present) {
    HANDLE_CODE(reduced_max_ccs_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void max_cc_pref_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_ccs_r16_present) {
    j.write_fieldname("reducedMaxCCs-r16");
    reduced_max_ccs_r16.to_json(j);
  }
  j.end_obj();
}

// MaxMIMO-LayerPreference-r16 ::= SEQUENCE
SRSASN_CODE max_mimo_layer_pref_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr1_r16_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr2_r16_present, 1));

  if (reduced_max_mimo_layers_fr1_r16_present) {
    HANDLE_CODE(
        pack_integer(bref, reduced_max_mimo_layers_fr1_r16.reduced_mimo_layers_fr1_dl_r16, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(
        pack_integer(bref, reduced_max_mimo_layers_fr1_r16.reduced_mimo_layers_fr1_ul_r16, (uint8_t)1u, (uint8_t)4u));
  }
  if (reduced_max_mimo_layers_fr2_r16_present) {
    HANDLE_CODE(
        pack_integer(bref, reduced_max_mimo_layers_fr2_r16.reduced_mimo_layers_fr2_dl_r16, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(
        pack_integer(bref, reduced_max_mimo_layers_fr2_r16.reduced_mimo_layers_fr2_ul_r16, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_mimo_layer_pref_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr1_r16_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr2_r16_present, 1));

  if (reduced_max_mimo_layers_fr1_r16_present) {
    HANDLE_CODE(
        unpack_integer(reduced_max_mimo_layers_fr1_r16.reduced_mimo_layers_fr1_dl_r16, bref, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(
        unpack_integer(reduced_max_mimo_layers_fr1_r16.reduced_mimo_layers_fr1_ul_r16, bref, (uint8_t)1u, (uint8_t)4u));
  }
  if (reduced_max_mimo_layers_fr2_r16_present) {
    HANDLE_CODE(
        unpack_integer(reduced_max_mimo_layers_fr2_r16.reduced_mimo_layers_fr2_dl_r16, bref, (uint8_t)1u, (uint8_t)8u));
    HANDLE_CODE(
        unpack_integer(reduced_max_mimo_layers_fr2_r16.reduced_mimo_layers_fr2_ul_r16, bref, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
void max_mimo_layer_pref_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_mimo_layers_fr1_r16_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR1-r16");
    j.start_obj();
    j.write_int("reducedMIMO-LayersFR1-DL-r16", reduced_max_mimo_layers_fr1_r16.reduced_mimo_layers_fr1_dl_r16);
    j.write_int("reducedMIMO-LayersFR1-UL-r16", reduced_max_mimo_layers_fr1_r16.reduced_mimo_layers_fr1_ul_r16);
    j.end_obj();
  }
  if (reduced_max_mimo_layers_fr2_r16_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR2-r16");
    j.start_obj();
    j.write_int("reducedMIMO-LayersFR2-DL-r16", reduced_max_mimo_layers_fr2_r16.reduced_mimo_layers_fr2_dl_r16);
    j.write_int("reducedMIMO-LayersFR2-UL-r16", reduced_max_mimo_layers_fr2_r16.reduced_mimo_layers_fr2_ul_r16);
    j.end_obj();
  }
  j.end_obj();
}

// MeasResult2EUTRA ::= SEQUENCE
SRSASN_CODE meas_result2_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_serving_cell_present, 1));
  HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.pack(bref));
  }
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_serving_cell_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.unpack(bref));
  }
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result2_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  if (meas_result_serving_cell_present) {
    j.write_fieldname("measResultServingCell");
    meas_result_serving_cell.to_json(j);
  }
  if (meas_result_best_neigh_cell_present) {
    j.write_fieldname("measResultBestNeighCell");
    meas_result_best_neigh_cell.to_json(j);
  }
  j.end_obj();
}

// MeasResult2NR ::= SEQUENCE
SRSASN_CODE meas_result2_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ssb_freq_present, 1));
  HANDLE_CODE(bref.pack(ref_freq_csi_rs_present, 1));
  HANDLE_CODE(bref.pack(meas_result_serving_cell_present, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cell_list_nr.size() > 0, 1));

  if (ssb_freq_present) {
    HANDLE_CODE(pack_integer(bref, ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ref_freq_csi_rs_present) {
    HANDLE_CODE(pack_integer(bref, ref_freq_csi_rs, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.pack(bref));
  }
  if (meas_result_neigh_cell_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cell_list_nr, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ssb_freq_present, 1));
  HANDLE_CODE(bref.unpack(ref_freq_csi_rs_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_serving_cell_present, 1));
  bool meas_result_neigh_cell_list_nr_present;
  HANDLE_CODE(bref.unpack(meas_result_neigh_cell_list_nr_present, 1));

  if (ssb_freq_present) {
    HANDLE_CODE(unpack_integer(ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ref_freq_csi_rs_present) {
    HANDLE_CODE(unpack_integer(ref_freq_csi_rs, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.unpack(bref));
  }
  if (meas_result_neigh_cell_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cell_list_nr, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_result2_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_freq_present) {
    j.write_int("ssbFrequency", ssb_freq);
  }
  if (ref_freq_csi_rs_present) {
    j.write_int("refFreqCSI-RS", ref_freq_csi_rs);
  }
  if (meas_result_serving_cell_present) {
    j.write_fieldname("measResultServingCell");
    meas_result_serving_cell.to_json(j);
  }
  if (meas_result_neigh_cell_list_nr.size() > 0) {
    j.start_array("measResultNeighCellListNR");
    for (const auto& e1 : meas_result_neigh_cell_list_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MeasResult2UTRA-FDD-r16 ::= SEQUENCE
SRSASN_CODE meas_result2_utra_fdd_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cell_list_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_utra_fdd_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cell_list_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result2_utra_fdd_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.start_array("measResultNeighCellList-r16");
  for (const auto& e1 : meas_result_neigh_cell_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResultCellSFTD-NR ::= SEQUENCE
SRSASN_CODE meas_result_cell_sftd_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_result_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(pack_integer(bref, sfn_offset_result, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, frame_boundary_offset_result, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cell_sftd_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_result_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(unpack_integer(sfn_offset_result, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(frame_boundary_offset_result, bref, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(unpack_integer(rsrp_result, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_cell_sftd_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_int("sfn-OffsetResult", sfn_offset_result);
  j.write_int("frameBoundaryOffsetResult", frame_boundary_offset_result);
  if (rsrp_result_present) {
    j.write_int("rsrp-Result", rsrp_result);
  }
  j.end_obj();
}

// MeasResultNR-SL-r16 ::= SEQUENCE
SRSASN_CODE meas_result_nr_sl_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_cbr_nr_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_nr_sl_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_cbr_nr_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result_nr_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measResultListCBR-NR-r16");
  for (const auto& e1 : meas_result_list_cbr_nr_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResultServMO ::= SEQUENCE
SRSASN_CODE meas_result_serv_mo_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_id, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_result_serving_cell.pack(bref));
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serv_mo_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_result_serving_cell.unpack(bref));
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_serv_mo_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId", serv_cell_id);
  j.write_fieldname("measResultServingCell");
  meas_result_serving_cell.to_json(j);
  if (meas_result_best_neigh_cell_present) {
    j.write_fieldname("measResultBestNeighCell");
    meas_result_best_neigh_cell.to_json(j);
  }
  j.end_obj();
}

// MeasResultSuccessHONR-r17 ::= SEQUENCE
SRSASN_CODE meas_result_success_ho_nr_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_r17.cell_results_r17.results_ssb_cell_r17_present, 1));
  HANDLE_CODE(bref.pack(meas_result_r17.cell_results_r17.results_csi_rs_cell_r17_present, 1));
  if (meas_result_r17.cell_results_r17.results_ssb_cell_r17_present) {
    HANDLE_CODE(meas_result_r17.cell_results_r17.results_ssb_cell_r17.pack(bref));
  }
  if (meas_result_r17.cell_results_r17.results_csi_rs_cell_r17_present) {
    HANDLE_CODE(meas_result_r17.cell_results_r17.results_csi_rs_cell_r17.pack(bref));
  }
  HANDLE_CODE(bref.pack(meas_result_r17.rs_idx_results_r17.results_ssb_idxes_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_r17.rs_idx_results_r17.results_csi_rs_idxes_r17.size() > 0, 1));
  if (meas_result_r17.rs_idx_results_r17.results_ssb_idxes_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_r17.rs_idx_results_r17.results_ssb_idxes_r17, 1, 64));
  }
  if (meas_result_r17.rs_idx_results_r17.results_csi_rs_idxes_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_r17.rs_idx_results_r17.results_csi_rs_idxes_r17, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_success_ho_nr_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_r17.cell_results_r17.results_ssb_cell_r17_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_r17.cell_results_r17.results_csi_rs_cell_r17_present, 1));
  if (meas_result_r17.cell_results_r17.results_ssb_cell_r17_present) {
    HANDLE_CODE(meas_result_r17.cell_results_r17.results_ssb_cell_r17.unpack(bref));
  }
  if (meas_result_r17.cell_results_r17.results_csi_rs_cell_r17_present) {
    HANDLE_CODE(meas_result_r17.cell_results_r17.results_csi_rs_cell_r17.unpack(bref));
  }
  bool results_ssb_idxes_r17_present;
  HANDLE_CODE(bref.unpack(results_ssb_idxes_r17_present, 1));
  bool results_csi_rs_idxes_r17_present;
  HANDLE_CODE(bref.unpack(results_csi_rs_idxes_r17_present, 1));
  if (results_ssb_idxes_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_r17.rs_idx_results_r17.results_ssb_idxes_r17, bref, 1, 64));
  }
  if (results_csi_rs_idxes_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_r17.rs_idx_results_r17.results_csi_rs_idxes_r17, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void meas_result_success_ho_nr_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResult-r17");
  j.start_obj();
  j.write_fieldname("cellResults-r17");
  j.start_obj();
  if (meas_result_r17.cell_results_r17.results_ssb_cell_r17_present) {
    j.write_fieldname("resultsSSB-Cell-r17");
    meas_result_r17.cell_results_r17.results_ssb_cell_r17.to_json(j);
  }
  if (meas_result_r17.cell_results_r17.results_csi_rs_cell_r17_present) {
    j.write_fieldname("resultsCSI-RS-Cell-r17");
    meas_result_r17.cell_results_r17.results_csi_rs_cell_r17.to_json(j);
  }
  j.end_obj();
  j.write_fieldname("rsIndexResults-r17");
  j.start_obj();
  if (meas_result_r17.rs_idx_results_r17.results_ssb_idxes_r17.size() > 0) {
    j.start_array("resultsSSB-Indexes-r17");
    for (const auto& e1 : meas_result_r17.rs_idx_results_r17.results_ssb_idxes_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_r17.rs_idx_results_r17.results_csi_rs_idxes_r17.size() > 0) {
    j.start_array("resultsCSI-RS-Indexes-r17");
    for (const auto& e1 : meas_result_r17.rs_idx_results_r17.results_csi_rs_idxes_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
  j.end_obj();
  j.end_obj();
}

// MeasResultsPerCarrierIdleEUTRA-r16 ::= SEQUENCE
SRSASN_CODE meas_results_per_carrier_idle_eutra_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, carrier_freq_eutra_r16, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_results_per_cell_list_idle_eutra_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_per_carrier_idle_eutra_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(carrier_freq_eutra_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_results_per_cell_list_idle_eutra_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_results_per_carrier_idle_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreqEUTRA-r16", carrier_freq_eutra_r16);
  j.start_array("measResultsPerCellListIdleEUTRA-r16");
  for (const auto& e1 : meas_results_per_cell_list_idle_eutra_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResultsPerCarrierIdleNR-r16 ::= SEQUENCE
SRSASN_CODE meas_results_per_carrier_idle_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_results_per_cell_list_idle_nr_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_per_carrier_idle_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_results_per_cell_list_idle_nr_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_results_per_carrier_idle_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.start_array("measResultsPerCellListIdleNR-r16");
  for (const auto& e1 : meas_results_per_cell_list_idle_nr_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MinSchedulingOffsetPreference-r16 ::= SEQUENCE
SRSASN_CODE min_sched_offset_pref_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(preferred_k0_r16_present, 1));
  HANDLE_CODE(bref.pack(preferred_k2_r16_present, 1));

  if (preferred_k0_r16_present) {
    HANDLE_CODE(bref.pack(preferred_k0_r16.preferred_k0_scs_15k_hz_r16_present, 1));
    HANDLE_CODE(bref.pack(preferred_k0_r16.preferred_k0_scs_30k_hz_r16_present, 1));
    HANDLE_CODE(bref.pack(preferred_k0_r16.preferred_k0_scs_60k_hz_r16_present, 1));
    HANDLE_CODE(bref.pack(preferred_k0_r16.preferred_k0_scs_120k_hz_r16_present, 1));
    if (preferred_k0_r16.preferred_k0_scs_15k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_15k_hz_r16.pack(bref));
    }
    if (preferred_k0_r16.preferred_k0_scs_30k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_30k_hz_r16.pack(bref));
    }
    if (preferred_k0_r16.preferred_k0_scs_60k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_60k_hz_r16.pack(bref));
    }
    if (preferred_k0_r16.preferred_k0_scs_120k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_120k_hz_r16.pack(bref));
    }
  }
  if (preferred_k2_r16_present) {
    HANDLE_CODE(bref.pack(preferred_k2_r16.preferred_k2_scs_15k_hz_r16_present, 1));
    HANDLE_CODE(bref.pack(preferred_k2_r16.preferred_k2_scs_30k_hz_r16_present, 1));
    HANDLE_CODE(bref.pack(preferred_k2_r16.preferred_k2_scs_60k_hz_r16_present, 1));
    HANDLE_CODE(bref.pack(preferred_k2_r16.preferred_k2_scs_120k_hz_r16_present, 1));
    if (preferred_k2_r16.preferred_k2_scs_15k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_15k_hz_r16.pack(bref));
    }
    if (preferred_k2_r16.preferred_k2_scs_30k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_30k_hz_r16.pack(bref));
    }
    if (preferred_k2_r16.preferred_k2_scs_60k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_60k_hz_r16.pack(bref));
    }
    if (preferred_k2_r16.preferred_k2_scs_120k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_120k_hz_r16.pack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE min_sched_offset_pref_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(preferred_k0_r16_present, 1));
  HANDLE_CODE(bref.unpack(preferred_k2_r16_present, 1));

  if (preferred_k0_r16_present) {
    HANDLE_CODE(bref.unpack(preferred_k0_r16.preferred_k0_scs_15k_hz_r16_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k0_r16.preferred_k0_scs_30k_hz_r16_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k0_r16.preferred_k0_scs_60k_hz_r16_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k0_r16.preferred_k0_scs_120k_hz_r16_present, 1));
    if (preferred_k0_r16.preferred_k0_scs_15k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_15k_hz_r16.unpack(bref));
    }
    if (preferred_k0_r16.preferred_k0_scs_30k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_30k_hz_r16.unpack(bref));
    }
    if (preferred_k0_r16.preferred_k0_scs_60k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_60k_hz_r16.unpack(bref));
    }
    if (preferred_k0_r16.preferred_k0_scs_120k_hz_r16_present) {
      HANDLE_CODE(preferred_k0_r16.preferred_k0_scs_120k_hz_r16.unpack(bref));
    }
  }
  if (preferred_k2_r16_present) {
    HANDLE_CODE(bref.unpack(preferred_k2_r16.preferred_k2_scs_15k_hz_r16_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k2_r16.preferred_k2_scs_30k_hz_r16_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k2_r16.preferred_k2_scs_60k_hz_r16_present, 1));
    HANDLE_CODE(bref.unpack(preferred_k2_r16.preferred_k2_scs_120k_hz_r16_present, 1));
    if (preferred_k2_r16.preferred_k2_scs_15k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_15k_hz_r16.unpack(bref));
    }
    if (preferred_k2_r16.preferred_k2_scs_30k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_30k_hz_r16.unpack(bref));
    }
    if (preferred_k2_r16.preferred_k2_scs_60k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_60k_hz_r16.unpack(bref));
    }
    if (preferred_k2_r16.preferred_k2_scs_120k_hz_r16_present) {
      HANDLE_CODE(preferred_k2_r16.preferred_k2_scs_120k_hz_r16.unpack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
void min_sched_offset_pref_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (preferred_k0_r16_present) {
    j.write_fieldname("preferredK0-r16");
    j.start_obj();
    if (preferred_k0_r16.preferred_k0_scs_15k_hz_r16_present) {
      j.write_str("preferredK0-SCS-15kHz-r16", preferred_k0_r16.preferred_k0_scs_15k_hz_r16.to_string());
    }
    if (preferred_k0_r16.preferred_k0_scs_30k_hz_r16_present) {
      j.write_str("preferredK0-SCS-30kHz-r16", preferred_k0_r16.preferred_k0_scs_30k_hz_r16.to_string());
    }
    if (preferred_k0_r16.preferred_k0_scs_60k_hz_r16_present) {
      j.write_str("preferredK0-SCS-60kHz-r16", preferred_k0_r16.preferred_k0_scs_60k_hz_r16.to_string());
    }
    if (preferred_k0_r16.preferred_k0_scs_120k_hz_r16_present) {
      j.write_str("preferredK0-SCS-120kHz-r16", preferred_k0_r16.preferred_k0_scs_120k_hz_r16.to_string());
    }
    j.end_obj();
  }
  if (preferred_k2_r16_present) {
    j.write_fieldname("preferredK2-r16");
    j.start_obj();
    if (preferred_k2_r16.preferred_k2_scs_15k_hz_r16_present) {
      j.write_str("preferredK2-SCS-15kHz-r16", preferred_k2_r16.preferred_k2_scs_15k_hz_r16.to_string());
    }
    if (preferred_k2_r16.preferred_k2_scs_30k_hz_r16_present) {
      j.write_str("preferredK2-SCS-30kHz-r16", preferred_k2_r16.preferred_k2_scs_30k_hz_r16.to_string());
    }
    if (preferred_k2_r16.preferred_k2_scs_60k_hz_r16_present) {
      j.write_str("preferredK2-SCS-60kHz-r16", preferred_k2_r16.preferred_k2_scs_60k_hz_r16.to_string());
    }
    if (preferred_k2_r16.preferred_k2_scs_120k_hz_r16_present) {
      j.write_str("preferredK2-SCS-120kHz-r16", preferred_k2_r16.preferred_k2_scs_120k_hz_r16.to_string());
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_15k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl1", "sl2", "sl4", "sl6"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_15k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_15k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_15k_hz_r16_e_");
}

const char* min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_30k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl1", "sl2", "sl4", "sl6"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_30k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_30k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_30k_hz_r16_e_");
}

const char* min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_60k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl2", "sl4", "sl8", "sl12"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_60k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_60k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 12};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_60k_hz_r16_e_");
}

const char* min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_120k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl2", "sl4", "sl8", "sl12"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_120k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_120k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 12};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k0_r16_s_::preferred_k0_scs_120k_hz_r16_e_");
}

const char* min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_15k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl1", "sl2", "sl4", "sl6"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_15k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_15k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_15k_hz_r16_e_");
}

const char* min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_30k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl1", "sl2", "sl4", "sl6"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_30k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_30k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_30k_hz_r16_e_");
}

const char* min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_60k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl2", "sl4", "sl8", "sl12"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_60k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_60k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 12};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_60k_hz_r16_e_");
}

const char* min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_120k_hz_r16_opts::to_string() const
{
  static const char* names[] = {"sl2", "sl4", "sl8", "sl12"};
  return convert_enum_idx(
      names, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_120k_hz_r16_e_");
}
uint8_t min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_120k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 12};
  return map_enum_number(
      numbers, 4, value, "min_sched_offset_pref_r16_s::preferred_k2_r16_s_::preferred_k2_scs_120k_hz_r16_e_");
}

// NR-PRS-MeasurementInfo-r16 ::= SEQUENCE
SRSASN_CODE nr_prs_meas_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, dl_prs_point_a_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(nr_meas_prs_repeat_and_offset_r16.pack(bref));
  HANDLE_CODE(nr_meas_prs_len_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_prs_meas_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(dl_prs_point_a_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(nr_meas_prs_repeat_and_offset_r16.unpack(bref));
  HANDLE_CODE(nr_meas_prs_len_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void nr_prs_meas_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-PRS-PointA-r16", dl_prs_point_a_r16);
  j.write_fieldname("nr-MeasPRS-RepetitionAndOffset-r16");
  nr_meas_prs_repeat_and_offset_r16.to_json(j);
  j.write_str("nr-MeasPRS-length-r16", nr_meas_prs_len_r16.to_string());
  j.end_obj();
}

void nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::destroy_() {}
void nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::nr_meas_prs_repeat_and_offset_r16_c_(
    const nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms20_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms40_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms80_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms160_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_");
  }
}
nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_&
nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::operator=(
    const nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms20_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms40_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms80_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms160_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_");
  }

  return *this;
}
uint8_t& nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::set_ms20_r16()
{
  set(types::ms20_r16);
  return c.get<uint8_t>();
}
uint8_t& nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::set_ms40_r16()
{
  set(types::ms40_r16);
  return c.get<uint8_t>();
}
uint8_t& nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::set_ms80_r16()
{
  set(types::ms80_r16);
  return c.get<uint8_t>();
}
uint8_t& nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::set_ms160_r16()
{
  set(types::ms160_r16);
  return c.get<uint8_t>();
}
void nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms20_r16:
      j.write_int("ms20-r16", c.get<uint8_t>());
      break;
    case types::ms40_r16:
      j.write_int("ms40-r16", c.get<uint8_t>());
      break;
    case types::ms80_r16:
      j.write_int("ms80-r16", c.get<uint8_t>());
      break;
    case types::ms160_r16:
      j.write_int("ms160-r16", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms20_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms40_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms80_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms160_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms20_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms40_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms80_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms160_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms20-r16", "ms40-r16", "ms80-r16", "ms160-r16"};
  return convert_enum_idx(names, 4, value, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types");
}
uint8_t nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 40, 80, 160};
  return map_enum_number(numbers, 4, value, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types");
}

const char* nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_opts::to_string() const
{
  static const char* names[] = {"ms1dot5", "ms3", "ms3dot5", "ms4", "ms5dot5", "ms6", "ms10", "ms20"};
  return convert_enum_idx(names, 8, value, "nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_e_");
}
float nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_opts::to_number() const
{
  static const float numbers[] = {1.5, 3.0, 3.5, 4.0, 5.5, 6.0, 10.0, 20.0};
  return map_enum_number(numbers, 8, value, "nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_e_");
}
const char* nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"1.5", "3", "3.5", "4", "5.5", "6", "10", "20"};
  return convert_enum_idx(number_strs, 8, value, "nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_e_");
}

// NR-TimeStamp-r17 ::= SEQUENCE
SRSASN_CODE nr_time_stamp_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, nr_sfn_r17, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(nr_slot_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_time_stamp_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(nr_sfn_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(nr_slot_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void nr_time_stamp_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nr-SFN-r17", nr_sfn_r17);
  j.write_fieldname("nr-Slot-r17");
  nr_slot_r17.to_json(j);
  j.end_obj();
}

void nr_time_stamp_r17_s::nr_slot_r17_c_::destroy_() {}
void nr_time_stamp_r17_s::nr_slot_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
nr_time_stamp_r17_s::nr_slot_r17_c_::nr_slot_r17_c_(const nr_time_stamp_r17_s::nr_slot_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::scs15_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::scs30_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::scs60_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::scs120_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_time_stamp_r17_s::nr_slot_r17_c_");
  }
}
nr_time_stamp_r17_s::nr_slot_r17_c_&
nr_time_stamp_r17_s::nr_slot_r17_c_::operator=(const nr_time_stamp_r17_s::nr_slot_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::scs15_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::scs30_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::scs60_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::scs120_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_time_stamp_r17_s::nr_slot_r17_c_");
  }

  return *this;
}
uint8_t& nr_time_stamp_r17_s::nr_slot_r17_c_::set_scs15_r17()
{
  set(types::scs15_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_time_stamp_r17_s::nr_slot_r17_c_::set_scs30_r17()
{
  set(types::scs30_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_time_stamp_r17_s::nr_slot_r17_c_::set_scs60_r17()
{
  set(types::scs60_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_time_stamp_r17_s::nr_slot_r17_c_::set_scs120_r17()
{
  set(types::scs120_r17);
  return c.get<uint8_t>();
}
void nr_time_stamp_r17_s::nr_slot_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scs15_r17:
      j.write_int("scs15-r17", c.get<uint8_t>());
      break;
    case types::scs30_r17:
      j.write_int("scs30-r17", c.get<uint8_t>());
      break;
    case types::scs60_r17:
      j.write_int("scs60-r17", c.get<uint8_t>());
      break;
    case types::scs120_r17:
      j.write_int("scs120-r17", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "nr_time_stamp_r17_s::nr_slot_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE nr_time_stamp_r17_s::nr_slot_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scs15_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::scs30_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::scs60_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::scs120_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_time_stamp_r17_s::nr_slot_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_time_stamp_r17_s::nr_slot_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scs15_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::scs30_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::scs60_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::scs120_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_time_stamp_r17_s::nr_slot_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* nr_time_stamp_r17_s::nr_slot_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"scs15-r17", "scs30-r17", "scs60-r17", "scs120-r17"};
  return convert_enum_idx(names, 4, value, "nr_time_stamp_r17_s::nr_slot_r17_c_::types");
}
uint8_t nr_time_stamp_r17_s::nr_slot_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "nr_time_stamp_r17_s::nr_slot_r17_c_::types");
}

// RA-InformationCommon-r16 ::= SEQUENCE
SRSASN_CODE ra_info_common_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(msg1_freq_start_r16_present, 1));
  HANDLE_CODE(bref.pack(msg1_freq_start_cfra_r16_present, 1));
  HANDLE_CODE(bref.pack(msg1_subcarrier_spacing_r16_present, 1));
  HANDLE_CODE(bref.pack(msg1_subcarrier_spacing_cfra_r16_present, 1));
  HANDLE_CODE(bref.pack(msg1_fdm_r16_present, 1));
  HANDLE_CODE(bref.pack(msg1_fdmcfra_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, absolute_freq_point_a_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_integer(bref, location_and_bw_r16, (uint16_t)0u, (uint16_t)37949u));
  HANDLE_CODE(subcarrier_spacing_r16.pack(bref));
  if (msg1_freq_start_r16_present) {
    HANDLE_CODE(pack_integer(bref, msg1_freq_start_r16, (uint16_t)0u, (uint16_t)274u));
  }
  if (msg1_freq_start_cfra_r16_present) {
    HANDLE_CODE(pack_integer(bref, msg1_freq_start_cfra_r16, (uint16_t)0u, (uint16_t)274u));
  }
  if (msg1_subcarrier_spacing_r16_present) {
    HANDLE_CODE(msg1_subcarrier_spacing_r16.pack(bref));
  }
  if (msg1_subcarrier_spacing_cfra_r16_present) {
    HANDLE_CODE(msg1_subcarrier_spacing_cfra_r16.pack(bref));
  }
  if (msg1_fdm_r16_present) {
    HANDLE_CODE(msg1_fdm_r16.pack(bref));
  }
  if (msg1_fdmcfra_r16_present) {
    HANDLE_CODE(msg1_fdmcfra_r16.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, per_ra_info_list_r16, 1, 200));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= per_ra_info_list_v1660.is_present();
    group_flags[1] |= msg1_scs_from_prach_cfg_idx_r16_present;
    group_flags[2] |= msg1_scs_from_prach_cfg_idx_cfra_r16_present;
    group_flags[3] |= msg_a_ro_freq_start_r17_present;
    group_flags[3] |= msg_a_ro_freq_start_cfra_r17_present;
    group_flags[3] |= msg_a_subcarrier_spacing_r17_present;
    group_flags[3] |= msg_a_ro_fdm_r17_present;
    group_flags[3] |= msg_a_ro_fdmcfra_r17_present;
    group_flags[3] |= msg_a_scs_from_prach_cfg_idx_r17_present;
    group_flags[3] |= msg_a_trans_max_r17_present;
    group_flags[3] |= msg_a_mcs_r17_present;
    group_flags[3] |= nrof_prbs_per_msg_a_po_r17_present;
    group_flags[3] |= msg_a_pusch_time_domain_alloc_r17_present;
    group_flags[3] |= freq_start_msg_a_pusch_r17_present;
    group_flags[3] |= nrof_msg_a_po_fdm_r17_present;
    group_flags[3] |= dl_pathloss_rsrp_r17_present;
    group_flags[3] |= intended_sibs_r17.is_present();
    group_flags[3] |= ssbs_for_si_acquisition_r17.is_present();
    group_flags[3] |= msg_a_pusch_payload_size_r17_present;
    group_flags[3] |= on_demand_si_success_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(per_ra_info_list_v1660.is_present(), 1));
      if (per_ra_info_list_v1660.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *per_ra_info_list_v1660, 1, 200));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(msg1_scs_from_prach_cfg_idx_r16_present, 1));
      if (msg1_scs_from_prach_cfg_idx_r16_present) {
        HANDLE_CODE(msg1_scs_from_prach_cfg_idx_r16.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(msg1_scs_from_prach_cfg_idx_cfra_r16_present, 1));
      if (msg1_scs_from_prach_cfg_idx_cfra_r16_present) {
        HANDLE_CODE(msg1_scs_from_prach_cfg_idx_cfra_r16.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(msg_a_ro_freq_start_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_ro_freq_start_cfra_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_subcarrier_spacing_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_ro_fdm_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_ro_fdmcfra_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_scs_from_prach_cfg_idx_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_trans_max_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_mcs_r17_present, 1));
      HANDLE_CODE(bref.pack(nrof_prbs_per_msg_a_po_r17_present, 1));
      HANDLE_CODE(bref.pack(msg_a_pusch_time_domain_alloc_r17_present, 1));
      HANDLE_CODE(bref.pack(freq_start_msg_a_pusch_r17_present, 1));
      HANDLE_CODE(bref.pack(nrof_msg_a_po_fdm_r17_present, 1));
      HANDLE_CODE(bref.pack(dl_pathloss_rsrp_r17_present, 1));
      HANDLE_CODE(bref.pack(intended_sibs_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(ssbs_for_si_acquisition_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(msg_a_pusch_payload_size_r17_present, 1));
      HANDLE_CODE(bref.pack(on_demand_si_success_r17_present, 1));
      if (msg_a_ro_freq_start_r17_present) {
        HANDLE_CODE(pack_integer(bref, msg_a_ro_freq_start_r17, (uint16_t)0u, (uint16_t)274u));
      }
      if (msg_a_ro_freq_start_cfra_r17_present) {
        HANDLE_CODE(pack_integer(bref, msg_a_ro_freq_start_cfra_r17, (uint16_t)0u, (uint16_t)274u));
      }
      if (msg_a_subcarrier_spacing_r17_present) {
        HANDLE_CODE(msg_a_subcarrier_spacing_r17.pack(bref));
      }
      if (msg_a_ro_fdm_r17_present) {
        HANDLE_CODE(msg_a_ro_fdm_r17.pack(bref));
      }
      if (msg_a_ro_fdmcfra_r17_present) {
        HANDLE_CODE(msg_a_ro_fdmcfra_r17.pack(bref));
      }
      if (msg_a_scs_from_prach_cfg_idx_r17_present) {
        HANDLE_CODE(msg_a_scs_from_prach_cfg_idx_r17.pack(bref));
      }
      if (msg_a_trans_max_r17_present) {
        HANDLE_CODE(msg_a_trans_max_r17.pack(bref));
      }
      if (msg_a_mcs_r17_present) {
        HANDLE_CODE(pack_integer(bref, msg_a_mcs_r17, (uint8_t)0u, (uint8_t)15u));
      }
      if (nrof_prbs_per_msg_a_po_r17_present) {
        HANDLE_CODE(pack_integer(bref, nrof_prbs_per_msg_a_po_r17, (uint8_t)1u, (uint8_t)32u));
      }
      if (msg_a_pusch_time_domain_alloc_r17_present) {
        HANDLE_CODE(pack_integer(bref, msg_a_pusch_time_domain_alloc_r17, (uint8_t)1u, (uint8_t)16u));
      }
      if (freq_start_msg_a_pusch_r17_present) {
        HANDLE_CODE(pack_integer(bref, freq_start_msg_a_pusch_r17, (uint16_t)0u, (uint16_t)274u));
      }
      if (nrof_msg_a_po_fdm_r17_present) {
        HANDLE_CODE(nrof_msg_a_po_fdm_r17.pack(bref));
      }
      if (dl_pathloss_rsrp_r17_present) {
        HANDLE_CODE(pack_integer(bref, dl_pathloss_rsrp_r17, (uint8_t)0u, (uint8_t)127u));
      }
      if (intended_sibs_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intended_sibs_r17, 1, 32));
      }
      if (ssbs_for_si_acquisition_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ssbs_for_si_acquisition_r17, 1, 64, integer_packer<uint8_t>(0, 63)));
      }
      if (msg_a_pusch_payload_size_r17_present) {
        HANDLE_CODE(msg_a_pusch_payload_size_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_info_common_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(msg1_freq_start_r16_present, 1));
  HANDLE_CODE(bref.unpack(msg1_freq_start_cfra_r16_present, 1));
  HANDLE_CODE(bref.unpack(msg1_subcarrier_spacing_r16_present, 1));
  HANDLE_CODE(bref.unpack(msg1_subcarrier_spacing_cfra_r16_present, 1));
  HANDLE_CODE(bref.unpack(msg1_fdm_r16_present, 1));
  HANDLE_CODE(bref.unpack(msg1_fdmcfra_r16_present, 1));

  HANDLE_CODE(unpack_integer(absolute_freq_point_a_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_integer(location_and_bw_r16, bref, (uint16_t)0u, (uint16_t)37949u));
  HANDLE_CODE(subcarrier_spacing_r16.unpack(bref));
  if (msg1_freq_start_r16_present) {
    HANDLE_CODE(unpack_integer(msg1_freq_start_r16, bref, (uint16_t)0u, (uint16_t)274u));
  }
  if (msg1_freq_start_cfra_r16_present) {
    HANDLE_CODE(unpack_integer(msg1_freq_start_cfra_r16, bref, (uint16_t)0u, (uint16_t)274u));
  }
  if (msg1_subcarrier_spacing_r16_present) {
    HANDLE_CODE(msg1_subcarrier_spacing_r16.unpack(bref));
  }
  if (msg1_subcarrier_spacing_cfra_r16_present) {
    HANDLE_CODE(msg1_subcarrier_spacing_cfra_r16.unpack(bref));
  }
  if (msg1_fdm_r16_present) {
    HANDLE_CODE(msg1_fdm_r16.unpack(bref));
  }
  if (msg1_fdmcfra_r16_present) {
    HANDLE_CODE(msg1_fdmcfra_r16.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(per_ra_info_list_r16, bref, 1, 200));

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(per_ra_info_list_v1660, bref);
      if (per_ra_info_list_v1660.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*per_ra_info_list_v1660, bref, 1, 200));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(msg1_scs_from_prach_cfg_idx_r16_present, 1));
      if (msg1_scs_from_prach_cfg_idx_r16_present) {
        HANDLE_CODE(msg1_scs_from_prach_cfg_idx_r16.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(msg1_scs_from_prach_cfg_idx_cfra_r16_present, 1));
      if (msg1_scs_from_prach_cfg_idx_cfra_r16_present) {
        HANDLE_CODE(msg1_scs_from_prach_cfg_idx_cfra_r16.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(msg_a_ro_freq_start_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_ro_freq_start_cfra_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_subcarrier_spacing_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_ro_fdm_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_ro_fdmcfra_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_scs_from_prach_cfg_idx_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_trans_max_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_mcs_r17_present, 1));
      HANDLE_CODE(bref.unpack(nrof_prbs_per_msg_a_po_r17_present, 1));
      HANDLE_CODE(bref.unpack(msg_a_pusch_time_domain_alloc_r17_present, 1));
      HANDLE_CODE(bref.unpack(freq_start_msg_a_pusch_r17_present, 1));
      HANDLE_CODE(bref.unpack(nrof_msg_a_po_fdm_r17_present, 1));
      HANDLE_CODE(bref.unpack(dl_pathloss_rsrp_r17_present, 1));
      unpack_presence_flag(intended_sibs_r17, bref);
      unpack_presence_flag(ssbs_for_si_acquisition_r17, bref);
      HANDLE_CODE(bref.unpack(msg_a_pusch_payload_size_r17_present, 1));
      HANDLE_CODE(bref.unpack(on_demand_si_success_r17_present, 1));
      if (msg_a_ro_freq_start_r17_present) {
        HANDLE_CODE(unpack_integer(msg_a_ro_freq_start_r17, bref, (uint16_t)0u, (uint16_t)274u));
      }
      if (msg_a_ro_freq_start_cfra_r17_present) {
        HANDLE_CODE(unpack_integer(msg_a_ro_freq_start_cfra_r17, bref, (uint16_t)0u, (uint16_t)274u));
      }
      if (msg_a_subcarrier_spacing_r17_present) {
        HANDLE_CODE(msg_a_subcarrier_spacing_r17.unpack(bref));
      }
      if (msg_a_ro_fdm_r17_present) {
        HANDLE_CODE(msg_a_ro_fdm_r17.unpack(bref));
      }
      if (msg_a_ro_fdmcfra_r17_present) {
        HANDLE_CODE(msg_a_ro_fdmcfra_r17.unpack(bref));
      }
      if (msg_a_scs_from_prach_cfg_idx_r17_present) {
        HANDLE_CODE(msg_a_scs_from_prach_cfg_idx_r17.unpack(bref));
      }
      if (msg_a_trans_max_r17_present) {
        HANDLE_CODE(msg_a_trans_max_r17.unpack(bref));
      }
      if (msg_a_mcs_r17_present) {
        HANDLE_CODE(unpack_integer(msg_a_mcs_r17, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (nrof_prbs_per_msg_a_po_r17_present) {
        HANDLE_CODE(unpack_integer(nrof_prbs_per_msg_a_po_r17, bref, (uint8_t)1u, (uint8_t)32u));
      }
      if (msg_a_pusch_time_domain_alloc_r17_present) {
        HANDLE_CODE(unpack_integer(msg_a_pusch_time_domain_alloc_r17, bref, (uint8_t)1u, (uint8_t)16u));
      }
      if (freq_start_msg_a_pusch_r17_present) {
        HANDLE_CODE(unpack_integer(freq_start_msg_a_pusch_r17, bref, (uint16_t)0u, (uint16_t)274u));
      }
      if (nrof_msg_a_po_fdm_r17_present) {
        HANDLE_CODE(nrof_msg_a_po_fdm_r17.unpack(bref));
      }
      if (dl_pathloss_rsrp_r17_present) {
        HANDLE_CODE(unpack_integer(dl_pathloss_rsrp_r17, bref, (uint8_t)0u, (uint8_t)127u));
      }
      if (intended_sibs_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intended_sibs_r17, bref, 1, 32));
      }
      if (ssbs_for_si_acquisition_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ssbs_for_si_acquisition_r17, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
      }
      if (msg_a_pusch_payload_size_r17_present) {
        HANDLE_CODE(msg_a_pusch_payload_size_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ra_info_common_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("absoluteFrequencyPointA-r16", absolute_freq_point_a_r16);
  j.write_int("locationAndBandwidth-r16", location_and_bw_r16);
  j.write_str("subcarrierSpacing-r16", subcarrier_spacing_r16.to_string());
  if (msg1_freq_start_r16_present) {
    j.write_int("msg1-FrequencyStart-r16", msg1_freq_start_r16);
  }
  if (msg1_freq_start_cfra_r16_present) {
    j.write_int("msg1-FrequencyStartCFRA-r16", msg1_freq_start_cfra_r16);
  }
  if (msg1_subcarrier_spacing_r16_present) {
    j.write_str("msg1-SubcarrierSpacing-r16", msg1_subcarrier_spacing_r16.to_string());
  }
  if (msg1_subcarrier_spacing_cfra_r16_present) {
    j.write_str("msg1-SubcarrierSpacingCFRA-r16", msg1_subcarrier_spacing_cfra_r16.to_string());
  }
  if (msg1_fdm_r16_present) {
    j.write_str("msg1-FDM-r16", msg1_fdm_r16.to_string());
  }
  if (msg1_fdmcfra_r16_present) {
    j.write_str("msg1-FDMCFRA-r16", msg1_fdmcfra_r16.to_string());
  }
  j.start_array("perRAInfoList-r16");
  for (const auto& e1 : per_ra_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (per_ra_info_list_v1660.is_present()) {
      j.start_array("perRAInfoList-v1660");
      for (const auto& e1 : *per_ra_info_list_v1660) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (msg1_scs_from_prach_cfg_idx_r16_present) {
      j.write_str("msg1-SCS-From-prach-ConfigurationIndex-r16", msg1_scs_from_prach_cfg_idx_r16.to_string());
    }
    if (msg1_scs_from_prach_cfg_idx_cfra_r16_present) {
      j.write_str("msg1-SCS-From-prach-ConfigurationIndexCFRA-r16", msg1_scs_from_prach_cfg_idx_cfra_r16.to_string());
    }
    if (msg_a_ro_freq_start_r17_present) {
      j.write_int("msgA-RO-FrequencyStart-r17", msg_a_ro_freq_start_r17);
    }
    if (msg_a_ro_freq_start_cfra_r17_present) {
      j.write_int("msgA-RO-FrequencyStartCFRA-r17", msg_a_ro_freq_start_cfra_r17);
    }
    if (msg_a_subcarrier_spacing_r17_present) {
      j.write_str("msgA-SubcarrierSpacing-r17", msg_a_subcarrier_spacing_r17.to_string());
    }
    if (msg_a_ro_fdm_r17_present) {
      j.write_str("msgA-RO-FDM-r17", msg_a_ro_fdm_r17.to_string());
    }
    if (msg_a_ro_fdmcfra_r17_present) {
      j.write_str("msgA-RO-FDMCFRA-r17", msg_a_ro_fdmcfra_r17.to_string());
    }
    if (msg_a_scs_from_prach_cfg_idx_r17_present) {
      j.write_str("msgA-SCS-From-prach-ConfigurationIndex-r17", msg_a_scs_from_prach_cfg_idx_r17.to_string());
    }
    if (msg_a_trans_max_r17_present) {
      j.write_str("msgA-TransMax-r17", msg_a_trans_max_r17.to_string());
    }
    if (msg_a_mcs_r17_present) {
      j.write_int("msgA-MCS-r17", msg_a_mcs_r17);
    }
    if (nrof_prbs_per_msg_a_po_r17_present) {
      j.write_int("nrofPRBs-PerMsgA-PO-r17", nrof_prbs_per_msg_a_po_r17);
    }
    if (msg_a_pusch_time_domain_alloc_r17_present) {
      j.write_int("msgA-PUSCH-TimeDomainAllocation-r17", msg_a_pusch_time_domain_alloc_r17);
    }
    if (freq_start_msg_a_pusch_r17_present) {
      j.write_int("frequencyStartMsgA-PUSCH-r17", freq_start_msg_a_pusch_r17);
    }
    if (nrof_msg_a_po_fdm_r17_present) {
      j.write_str("nrofMsgA-PO-FDM-r17", nrof_msg_a_po_fdm_r17.to_string());
    }
    if (dl_pathloss_rsrp_r17_present) {
      j.write_int("dlPathlossRSRP-r17", dl_pathloss_rsrp_r17);
    }
    if (intended_sibs_r17.is_present()) {
      j.start_array("intendedSIBs-r17");
      for (const auto& e1 : *intended_sibs_r17) {
        j.write_str(e1.to_string());
      }
      j.end_array();
    }
    if (ssbs_for_si_acquisition_r17.is_present()) {
      j.start_array("ssbsForSI-Acquisition-r17");
      for (const auto& e1 : *ssbs_for_si_acquisition_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (msg_a_pusch_payload_size_r17_present) {
      j.write_str("msgA-PUSCH-PayloadSize-r17", msg_a_pusch_payload_size_r17.to_string());
    }
    if (on_demand_si_success_r17_present) {
      j.write_str("onDemandSISuccess-r17", "true");
    }
  }
  j.end_obj();
}

const char* ra_info_common_r16_s::msg1_fdm_r16_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "eight"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::msg1_fdm_r16_e_");
}
uint8_t ra_info_common_r16_s::msg1_fdm_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "ra_info_common_r16_s::msg1_fdm_r16_e_");
}

const char* ra_info_common_r16_s::msg1_fdmcfra_r16_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "eight"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::msg1_fdmcfra_r16_e_");
}
uint8_t ra_info_common_r16_s::msg1_fdmcfra_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "ra_info_common_r16_s::msg1_fdmcfra_r16_e_");
}

const char* ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_r16_opts::to_string() const
{
  static const char* names[] = {"kHz1dot25", "kHz5", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_r16_e_");
}
float ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_r16_opts::to_number() const
{
  static const float numbers[] = {1.25, 5.0};
  return map_enum_number(numbers, 2, value, "ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_r16_e_");
}
const char* ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"1.25", "5"};
  return convert_enum_idx(number_strs, 4, value, "ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_r16_e_");
}

const char* ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_cfra_r16_opts::to_string() const
{
  static const char* names[] = {"kHz1dot25", "kHz5", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_cfra_r16_e_");
}
float ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_cfra_r16_opts::to_number() const
{
  static const float numbers[] = {1.25, 5.0};
  return map_enum_number(numbers, 2, value, "ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_cfra_r16_e_");
}
const char* ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_cfra_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"1.25", "5"};
  return convert_enum_idx(number_strs, 4, value, "ra_info_common_r16_s::msg1_scs_from_prach_cfg_idx_cfra_r16_e_");
}

const char* ra_info_common_r16_s::msg_a_ro_fdm_r17_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "eight"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::msg_a_ro_fdm_r17_e_");
}
uint8_t ra_info_common_r16_s::msg_a_ro_fdm_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "ra_info_common_r16_s::msg_a_ro_fdm_r17_e_");
}

const char* ra_info_common_r16_s::msg_a_ro_fdmcfra_r17_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "eight"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::msg_a_ro_fdmcfra_r17_e_");
}
uint8_t ra_info_common_r16_s::msg_a_ro_fdmcfra_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "ra_info_common_r16_s::msg_a_ro_fdmcfra_r17_e_");
}

const char* ra_info_common_r16_s::msg_a_scs_from_prach_cfg_idx_r17_opts::to_string() const
{
  static const char* names[] = {"kHz1dot25", "kHz5", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::msg_a_scs_from_prach_cfg_idx_r17_e_");
}
float ra_info_common_r16_s::msg_a_scs_from_prach_cfg_idx_r17_opts::to_number() const
{
  static const float numbers[] = {1.25, 5.0};
  return map_enum_number(numbers, 2, value, "ra_info_common_r16_s::msg_a_scs_from_prach_cfg_idx_r17_e_");
}
const char* ra_info_common_r16_s::msg_a_scs_from_prach_cfg_idx_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"1.25", "5"};
  return convert_enum_idx(number_strs, 4, value, "ra_info_common_r16_s::msg_a_scs_from_prach_cfg_idx_r17_e_");
}

const char* ra_info_common_r16_s::msg_a_trans_max_r17_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n6", "n8", "n10", "n20", "n50", "n100", "n200"};
  return convert_enum_idx(names, 10, value, "ra_info_common_r16_s::msg_a_trans_max_r17_e_");
}
uint8_t ra_info_common_r16_s::msg_a_trans_max_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6, 8, 10, 20, 50, 100, 200};
  return map_enum_number(numbers, 10, value, "ra_info_common_r16_s::msg_a_trans_max_r17_e_");
}

const char* ra_info_common_r16_s::nrof_msg_a_po_fdm_r17_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "eight"};
  return convert_enum_idx(names, 4, value, "ra_info_common_r16_s::nrof_msg_a_po_fdm_r17_e_");
}
uint8_t ra_info_common_r16_s::nrof_msg_a_po_fdm_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "ra_info_common_r16_s::nrof_msg_a_po_fdm_r17_e_");
}

// RAN-VisibleMeasurements-r17 ::= SEQUENCE
SRSASN_CODE ran_visible_meass_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(app_layer_buffer_level_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(playout_delay_for_media_startup_r17_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_id_list_r17.size() > 0, 1));

  if (app_layer_buffer_level_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, app_layer_buffer_level_list_r17, 1, 8, integer_packer<uint16_t>(0, 30000)));
  }
  if (playout_delay_for_media_startup_r17_present) {
    HANDLE_CODE(pack_integer(bref, playout_delay_for_media_startup_r17, (uint16_t)0u, (uint16_t)30000u));
  }
  if (pdu_session_id_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_id_list_r17, 1, 256, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_visible_meass_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool app_layer_buffer_level_list_r17_present;
  HANDLE_CODE(bref.unpack(app_layer_buffer_level_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(playout_delay_for_media_startup_r17_present, 1));
  bool pdu_session_id_list_r17_present;
  HANDLE_CODE(bref.unpack(pdu_session_id_list_r17_present, 1));

  if (app_layer_buffer_level_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(app_layer_buffer_level_list_r17, bref, 1, 8, integer_packer<uint16_t>(0, 30000)));
  }
  if (playout_delay_for_media_startup_r17_present) {
    HANDLE_CODE(unpack_integer(playout_delay_for_media_startup_r17, bref, (uint16_t)0u, (uint16_t)30000u));
  }
  if (pdu_session_id_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pdu_session_id_list_r17, bref, 1, 256, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
void ran_visible_meass_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (app_layer_buffer_level_list_r17.size() > 0) {
    j.start_array("appLayerBufferLevelList-r17");
    for (const auto& e1 : app_layer_buffer_level_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (playout_delay_for_media_startup_r17_present) {
    j.write_int("playoutDelayForMediaStartup-r17", playout_delay_for_media_startup_r17);
  }
  if (pdu_session_id_list_r17.size() > 0) {
    j.start_array("pdu-SessionIdList-r17");
    for (const auto& e1 : pdu_session_id_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// RRCReconfigurationComplete-v1610-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.pack(need_for_gaps_info_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.pack(bref));
  }
  if (need_for_gaps_info_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_info_nr_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gaps_info_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.unpack(bref));
  }
  if (need_for_gaps_info_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_info_nr_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_meass_available_r16_present) {
    j.write_fieldname("ue-MeasurementsAvailable-r16");
    ue_meass_available_r16.to_json(j);
  }
  if (need_for_gaps_info_nr_r16_present) {
    j.write_fieldname("needForGapsInfoNR-r16");
    need_for_gaps_info_nr_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCResumeComplete-v1700-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_nr_r17_present, 1));
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_eutra_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (need_for_gap_ncsg_info_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_nr_r17.pack(bref));
  }
  if (need_for_gap_ncsg_info_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_info_nr_r17_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_info_eutra_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (need_for_gap_ncsg_info_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_nr_r17.unpack(bref));
  }
  if (need_for_gap_ncsg_info_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (need_for_gap_ncsg_info_nr_r17_present) {
    j.write_fieldname("needForGapNCSG-InfoNR-r17");
    need_for_gap_ncsg_info_nr_r17.to_json(j);
  }
  if (need_for_gap_ncsg_info_eutra_r17_present) {
    j.write_fieldname("needForGapNCSG-InfoEUTRA-r17");
    need_for_gap_ncsg_info_eutra_r17.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCSetupComplete-v1700-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_complete_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(onboarding_request_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(onboarding_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_setup_complete_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (onboarding_request_r17_present) {
    j.write_str("onboardingRequest-r17", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// ReleasePreference-r16 ::= SEQUENCE
SRSASN_CODE release_pref_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(preferred_rrc_state_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE release_pref_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(preferred_rrc_state_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void release_pref_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("preferredRRC-State-r16", preferred_rrc_state_r16.to_string());
  j.end_obj();
}

const char* release_pref_r16_s::preferred_rrc_state_r16_opts::to_string() const
{
  static const char* names[] = {"idle", "inactive", "connected", "outOfConnected"};
  return convert_enum_idx(names, 4, value, "release_pref_r16_s::preferred_rrc_state_r16_e_");
}

// SHR-Cause-r17 ::= SEQUENCE
SRSASN_CODE shr_cause_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(t304_cause_r17_present, 1));
  HANDLE_CODE(bref.pack(t310_cause_r17_present, 1));
  HANDLE_CODE(bref.pack(t312_cause_r17_present, 1));
  HANDLE_CODE(bref.pack(source_daps_fail_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE shr_cause_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(t304_cause_r17_present, 1));
  HANDLE_CODE(bref.unpack(t310_cause_r17_present, 1));
  HANDLE_CODE(bref.unpack(t312_cause_r17_present, 1));
  HANDLE_CODE(bref.unpack(source_daps_fail_r17_present, 1));

  return SRSASN_SUCCESS;
}
void shr_cause_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (t304_cause_r17_present) {
    j.write_str("t304-cause-r17", "true");
  }
  if (t310_cause_r17_present) {
    j.write_str("t310-cause-r17", "true");
  }
  if (t312_cause_r17_present) {
    j.write_str("t312-cause-r17", "true");
  }
  if (source_daps_fail_r17_present) {
    j.write_str("sourceDAPS-Failure-r17", "true");
  }
  j.end_obj();
}

// SL-RxDRX-Report-v1700 ::= SEQUENCE
SRSASN_CODE sl_rx_drx_report_v1700_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sl_drx_cfg_from_tx_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_rx_drx_report_v1700_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sl_drx_cfg_from_tx_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_rx_drx_report_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-DRX-ConfigFromTx-r17");
  sl_drx_cfg_from_tx_r17.to_json(j);
  j.end_obj();
}

// SL-RxInterestedGC-BC-Dest-r17 ::= SEQUENCE
SRSASN_CODE sl_rx_interested_gc_bc_dest_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_rx_interested_qos_info_list_r17, 1, 64));
  HANDLE_CODE(sl_dest_id_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_rx_interested_gc_bc_dest_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(sl_rx_interested_qos_info_list_r17, bref, 1, 64));
  HANDLE_CODE(sl_dest_id_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_rx_interested_gc_bc_dest_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sl-RxInterestedQoS-InfoList-r17");
  for (const auto& e1 : sl_rx_interested_qos_info_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("sl-DestinationIdentity-r16", sl_dest_id_r16.to_string());
  j.end_obj();
}

// SL-TxResourceReqCommRelayInfo-r17 ::= SEQUENCE
SRSASN_CODE sl_tx_res_req_comm_relay_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_relay_drx_cfg_r17_present, 1));

  if (sl_relay_drx_cfg_r17_present) {
    HANDLE_CODE(sl_relay_drx_cfg_r17.pack(bref));
  }
  HANDLE_CODE(sl_tx_res_req_comm_relay_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_res_req_comm_relay_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_relay_drx_cfg_r17_present, 1));

  if (sl_relay_drx_cfg_r17_present) {
    HANDLE_CODE(sl_relay_drx_cfg_r17.unpack(bref));
  }
  HANDLE_CODE(sl_tx_res_req_comm_relay_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_tx_res_req_comm_relay_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_relay_drx_cfg_r17_present) {
    j.write_fieldname("sl-RelayDRXConfig-r17");
    sl_relay_drx_cfg_r17.to_json(j);
  }
  j.write_fieldname("sl-TxResourceReqCommRelay-r17");
  sl_tx_res_req_comm_relay_r17.to_json(j);
  j.end_obj();
}

// SL-TxResourceReqDisc-r17 ::= SEQUENCE
SRSASN_CODE sl_tx_res_req_disc_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_source_id_relay_ue_r17_present, 1));

  HANDLE_CODE(sl_dest_id_disc_r17.pack(bref));
  if (sl_source_id_relay_ue_r17_present) {
    HANDLE_CODE(sl_source_id_relay_ue_r17.pack(bref));
  }
  HANDLE_CODE(sl_cast_type_disc_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_interested_freq_list_disc_r17, 1, 8, integer_packer<uint8_t>(1, 8)));
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_type_tx_sync_list_disc_r17, 1, 8));
  HANDLE_CODE(sl_discovery_type_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_res_req_disc_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_source_id_relay_ue_r17_present, 1));

  HANDLE_CODE(sl_dest_id_disc_r17.unpack(bref));
  if (sl_source_id_relay_ue_r17_present) {
    HANDLE_CODE(sl_source_id_relay_ue_r17.unpack(bref));
  }
  HANDLE_CODE(sl_cast_type_disc_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sl_tx_interested_freq_list_disc_r17, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  HANDLE_CODE(unpack_dyn_seq_of(sl_type_tx_sync_list_disc_r17, bref, 1, 8));
  HANDLE_CODE(sl_discovery_type_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_tx_res_req_disc_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-DestinationIdentityDisc-r17", sl_dest_id_disc_r17.to_string());
  if (sl_source_id_relay_ue_r17_present) {
    j.write_str("sl-SourceIdentityRelayUE-r17", sl_source_id_relay_ue_r17.to_string());
  }
  j.write_str("sl-CastTypeDisc-r17", sl_cast_type_disc_r17.to_string());
  j.start_array("sl-TxInterestedFreqListDisc-r17");
  for (const auto& e1 : sl_tx_interested_freq_list_disc_r17) {
    j.write_int(e1);
  }
  j.end_array();
  j.start_array("sl-TypeTxSyncListDisc-r17");
  for (const auto& e1 : sl_type_tx_sync_list_disc_r17) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.write_str("sl-DiscoveryType-r17", sl_discovery_type_r17.to_string());
  j.end_obj();
}

const char* sl_tx_res_req_disc_r17_s::sl_cast_type_disc_r17_opts::to_string() const
{
  static const char* names[] = {"broadcast", "groupcast", "unicast", "spare1"};
  return convert_enum_idx(names, 4, value, "sl_tx_res_req_disc_r17_s::sl_cast_type_disc_r17_e_");
}

const char* sl_tx_res_req_disc_r17_s::sl_discovery_type_r17_opts::to_string() const
{
  static const char* names[] = {"relay", "non-Relay"};
  return convert_enum_idx(names, 2, value, "sl_tx_res_req_disc_r17_s::sl_discovery_type_r17_e_");
}

// UEAssistanceInformation-v1700-IEs ::= SEQUENCE
SRSASN_CODE ue_assist_info_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_gap_fr2_pref_r17_present, 1));
  HANDLE_CODE(bref.pack(musim_assist_r17_present, 1));
  HANDLE_CODE(bref.pack(overheat_assist_r17_present, 1));
  HANDLE_CODE(bref.pack(max_bw_pref_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.pack(max_mimo_layer_pref_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.pack(min_sched_offset_pref_ext_r17_present, 1));
  HANDLE_CODE(bref.pack(rlm_meas_relaxation_state_r17_present, 1));
  HANDLE_CODE(bref.pack(bfd_meas_relaxation_state_r17_present, 1));
  HANDLE_CODE(bref.pack(non_sdt_data_ind_r17_present, 1));
  HANDLE_CODE(bref.pack(scg_deactivation_pref_r17_present, 1));
  HANDLE_CODE(bref.pack(ul_data_r17_present, 1));
  HANDLE_CODE(bref.pack(rrm_meas_relaxation_fulfilment_r17_present, 1));
  HANDLE_CODE(bref.pack(propagation_delay_difference_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_gap_fr2_pref_r17_present) {
    HANDLE_CODE(ul_gap_fr2_pref_r17.pack(bref));
  }
  if (musim_assist_r17_present) {
    HANDLE_CODE(musim_assist_r17.pack(bref));
  }
  if (overheat_assist_r17_present) {
    HANDLE_CODE(overheat_assist_r17.pack(bref));
  }
  if (max_bw_pref_fr2_2_r17_present) {
    HANDLE_CODE(max_bw_pref_fr2_2_r17.pack(bref));
  }
  if (max_mimo_layer_pref_fr2_2_r17_present) {
    HANDLE_CODE(max_mimo_layer_pref_fr2_2_r17.pack(bref));
  }
  if (min_sched_offset_pref_ext_r17_present) {
    HANDLE_CODE(min_sched_offset_pref_ext_r17.pack(bref));
  }
  if (rlm_meas_relaxation_state_r17_present) {
    HANDLE_CODE(bref.pack(rlm_meas_relaxation_state_r17, 1));
  }
  if (bfd_meas_relaxation_state_r17_present) {
    HANDLE_CODE(bfd_meas_relaxation_state_r17.pack(bref));
  }
  if (non_sdt_data_ind_r17_present) {
    HANDLE_CODE(bref.pack(non_sdt_data_ind_r17.resume_cause_r17_present, 1));
    if (non_sdt_data_ind_r17.resume_cause_r17_present) {
      HANDLE_CODE(non_sdt_data_ind_r17.resume_cause_r17.pack(bref));
    }
  }
  if (scg_deactivation_pref_r17_present) {
    HANDLE_CODE(scg_deactivation_pref_r17.pack(bref));
  }
  if (rrm_meas_relaxation_fulfilment_r17_present) {
    HANDLE_CODE(bref.pack(rrm_meas_relaxation_fulfilment_r17, 1));
  }
  if (propagation_delay_difference_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, propagation_delay_difference_r17, 1, 4, integer_packer<int16_t>(-270, 270)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_assist_info_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_gap_fr2_pref_r17_present, 1));
  HANDLE_CODE(bref.unpack(musim_assist_r17_present, 1));
  HANDLE_CODE(bref.unpack(overheat_assist_r17_present, 1));
  HANDLE_CODE(bref.unpack(max_bw_pref_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.unpack(max_mimo_layer_pref_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.unpack(min_sched_offset_pref_ext_r17_present, 1));
  HANDLE_CODE(bref.unpack(rlm_meas_relaxation_state_r17_present, 1));
  HANDLE_CODE(bref.unpack(bfd_meas_relaxation_state_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_sdt_data_ind_r17_present, 1));
  HANDLE_CODE(bref.unpack(scg_deactivation_pref_r17_present, 1));
  HANDLE_CODE(bref.unpack(ul_data_r17_present, 1));
  HANDLE_CODE(bref.unpack(rrm_meas_relaxation_fulfilment_r17_present, 1));
  bool propagation_delay_difference_r17_present;
  HANDLE_CODE(bref.unpack(propagation_delay_difference_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_gap_fr2_pref_r17_present) {
    HANDLE_CODE(ul_gap_fr2_pref_r17.unpack(bref));
  }
  if (musim_assist_r17_present) {
    HANDLE_CODE(musim_assist_r17.unpack(bref));
  }
  if (overheat_assist_r17_present) {
    HANDLE_CODE(overheat_assist_r17.unpack(bref));
  }
  if (max_bw_pref_fr2_2_r17_present) {
    HANDLE_CODE(max_bw_pref_fr2_2_r17.unpack(bref));
  }
  if (max_mimo_layer_pref_fr2_2_r17_present) {
    HANDLE_CODE(max_mimo_layer_pref_fr2_2_r17.unpack(bref));
  }
  if (min_sched_offset_pref_ext_r17_present) {
    HANDLE_CODE(min_sched_offset_pref_ext_r17.unpack(bref));
  }
  if (rlm_meas_relaxation_state_r17_present) {
    HANDLE_CODE(bref.unpack(rlm_meas_relaxation_state_r17, 1));
  }
  if (bfd_meas_relaxation_state_r17_present) {
    HANDLE_CODE(bfd_meas_relaxation_state_r17.unpack(bref));
  }
  if (non_sdt_data_ind_r17_present) {
    HANDLE_CODE(bref.unpack(non_sdt_data_ind_r17.resume_cause_r17_present, 1));
    if (non_sdt_data_ind_r17.resume_cause_r17_present) {
      HANDLE_CODE(non_sdt_data_ind_r17.resume_cause_r17.unpack(bref));
    }
  }
  if (scg_deactivation_pref_r17_present) {
    HANDLE_CODE(scg_deactivation_pref_r17.unpack(bref));
  }
  if (rrm_meas_relaxation_fulfilment_r17_present) {
    HANDLE_CODE(bref.unpack(rrm_meas_relaxation_fulfilment_r17, 1));
  }
  if (propagation_delay_difference_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(propagation_delay_difference_r17, bref, 1, 4, integer_packer<int16_t>(-270, 270)));
  }

  return SRSASN_SUCCESS;
}
void ue_assist_info_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_gap_fr2_pref_r17_present) {
    j.write_fieldname("ul-GapFR2-Preference-r17");
    ul_gap_fr2_pref_r17.to_json(j);
  }
  if (musim_assist_r17_present) {
    j.write_fieldname("musim-Assistance-r17");
    musim_assist_r17.to_json(j);
  }
  if (overheat_assist_r17_present) {
    j.write_fieldname("overheatingAssistance-r17");
    overheat_assist_r17.to_json(j);
  }
  if (max_bw_pref_fr2_2_r17_present) {
    j.write_fieldname("maxBW-PreferenceFR2-2-r17");
    max_bw_pref_fr2_2_r17.to_json(j);
  }
  if (max_mimo_layer_pref_fr2_2_r17_present) {
    j.write_fieldname("maxMIMO-LayerPreferenceFR2-2-r17");
    max_mimo_layer_pref_fr2_2_r17.to_json(j);
  }
  if (min_sched_offset_pref_ext_r17_present) {
    j.write_fieldname("minSchedulingOffsetPreferenceExt-r17");
    min_sched_offset_pref_ext_r17.to_json(j);
  }
  if (rlm_meas_relaxation_state_r17_present) {
    j.write_bool("rlm-MeasRelaxationState-r17", rlm_meas_relaxation_state_r17);
  }
  if (bfd_meas_relaxation_state_r17_present) {
    j.write_str("bfd-MeasRelaxationState-r17", bfd_meas_relaxation_state_r17.to_string());
  }
  if (non_sdt_data_ind_r17_present) {
    j.write_fieldname("nonSDT-DataIndication-r17");
    j.start_obj();
    if (non_sdt_data_ind_r17.resume_cause_r17_present) {
      j.write_str("resumeCause-r17", non_sdt_data_ind_r17.resume_cause_r17.to_string());
    }
    j.end_obj();
  }
  if (scg_deactivation_pref_r17_present) {
    j.write_str("scg-DeactivationPreference-r17", scg_deactivation_pref_r17.to_string());
  }
  if (ul_data_r17_present) {
    j.write_str("uplinkData-r17", "true");
  }
  if (rrm_meas_relaxation_fulfilment_r17_present) {
    j.write_bool("rrm-MeasRelaxationFulfilment-r17", rrm_meas_relaxation_fulfilment_r17);
  }
  if (propagation_delay_difference_r17.size() > 0) {
    j.start_array("propagationDelayDifference-r17");
    for (const auto& e1 : propagation_delay_difference_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* ue_assist_info_v1700_ies_s::scg_deactivation_pref_r17_opts::to_string() const
{
  static const char* names[] = {"scgDeactivationPreferred", "noPreference"};
  return convert_enum_idx(names, 2, value, "ue_assist_info_v1700_ies_s::scg_deactivation_pref_r17_e_");
}

// UL-PDCP-DelayValueResult-r16 ::= SEQUENCE
SRSASN_CODE ul_pdcp_delay_value_result_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, drb_id_r16, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, average_delay_r16, (uint16_t)0u, (uint16_t)10000u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pdcp_delay_value_result_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(drb_id_r16, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(average_delay_r16, bref, (uint16_t)0u, (uint16_t)10000u));

  return SRSASN_SUCCESS;
}
void ul_pdcp_delay_value_result_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Id-r16", drb_id_r16);
  j.write_int("averageDelay-r16", average_delay_r16);
  j.end_obj();
}

// UL-PDCP-ExcessDelayResult-r17 ::= SEQUENCE
SRSASN_CODE ul_pdcp_excess_delay_result_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, drb_id_r17, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, excess_delay_r17, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pdcp_excess_delay_result_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(drb_id_r17, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(excess_delay_r17, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void ul_pdcp_excess_delay_result_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Id-r17", drb_id_r17);
  j.write_int("excessDelay-r17", excess_delay_r17);
  j.end_obj();
}

// UplinkTxDirectCurrentCell ::= SEQUENCE
SRSASN_CODE ul_tx_direct_current_cell_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_direct_current_bwp, 1, 4));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ul_direct_current_bwp_sul.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_direct_current_bwp_sul.is_present(), 1));
      if (ul_direct_current_bwp_sul.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ul_direct_current_bwp_sul, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_cell_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_dyn_seq_of(ul_direct_current_bwp, bref, 1, 4));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ul_direct_current_bwp_sul, bref);
      if (ul_direct_current_bwp_sul.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_direct_current_bwp_sul, bref, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ul_tx_direct_current_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.start_array("uplinkDirectCurrentBWP");
  for (const auto& e1 : ul_direct_current_bwp) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (ul_direct_current_bwp_sul.is_present()) {
      j.start_array("uplinkDirectCurrentBWP-SUL");
      for (const auto& e1 : *ul_direct_current_bwp_sul) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// VisitedCellInfo-r16 ::= SEQUENCE
SRSASN_CODE visited_cell_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(visited_cell_id_r16_present, 1));

  if (visited_cell_id_r16_present) {
    HANDLE_CODE(visited_cell_id_r16.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, time_spent_r16, (uint16_t)0u, (uint16_t)4095u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= visited_pscell_info_list_report_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(visited_pscell_info_list_report_r17.is_present(), 1));
      if (visited_pscell_info_list_report_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *visited_pscell_info_list_report_r17, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_cell_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(visited_cell_id_r16_present, 1));

  if (visited_cell_id_r16_present) {
    HANDLE_CODE(visited_cell_id_r16.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(time_spent_r16, bref, (uint16_t)0u, (uint16_t)4095u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(visited_pscell_info_list_report_r17, bref);
      if (visited_pscell_info_list_report_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*visited_pscell_info_list_report_r17, bref, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void visited_cell_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (visited_cell_id_r16_present) {
    j.write_fieldname("visitedCellId-r16");
    visited_cell_id_r16.to_json(j);
  }
  j.write_int("timeSpent-r16", time_spent_r16);
  if (ext) {
    if (visited_pscell_info_list_report_r17.is_present()) {
      j.start_array("visitedPSCellInfoListReport-r17");
      for (const auto& e1 : *visited_pscell_info_list_report_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

void visited_cell_info_r16_s::visited_cell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::nr_cell_id_r16:
      c.destroy<nr_cell_id_r16_c_>();
      break;
    case types::eutra_cell_id_r16:
      c.destroy<eutra_cell_id_r16_c_>();
      break;
    default:
      break;
  }
}
void visited_cell_info_r16_s::visited_cell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_cell_id_r16:
      c.init<nr_cell_id_r16_c_>();
      break;
    case types::eutra_cell_id_r16:
      c.init<eutra_cell_id_r16_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_");
  }
}
visited_cell_info_r16_s::visited_cell_id_r16_c_::visited_cell_id_r16_c_(
    const visited_cell_info_r16_s::visited_cell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_cell_id_r16:
      c.init(other.c.get<nr_cell_id_r16_c_>());
      break;
    case types::eutra_cell_id_r16:
      c.init(other.c.get<eutra_cell_id_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_");
  }
}
visited_cell_info_r16_s::visited_cell_id_r16_c_&
visited_cell_info_r16_s::visited_cell_id_r16_c_::operator=(const visited_cell_info_r16_s::visited_cell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_cell_id_r16:
      c.set(other.c.get<nr_cell_id_r16_c_>());
      break;
    case types::eutra_cell_id_r16:
      c.set(other.c.get<eutra_cell_id_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_");
  }

  return *this;
}
visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_&
visited_cell_info_r16_s::visited_cell_id_r16_c_::set_nr_cell_id_r16()
{
  set(types::nr_cell_id_r16);
  return c.get<nr_cell_id_r16_c_>();
}
visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_&
visited_cell_info_r16_s::visited_cell_id_r16_c_::set_eutra_cell_id_r16()
{
  set(types::eutra_cell_id_r16);
  return c.get<eutra_cell_id_r16_c_>();
}
void visited_cell_info_r16_s::visited_cell_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_cell_id_r16:
      j.write_fieldname("nr-CellId-r16");
      c.get<nr_cell_id_r16_c_>().to_json(j);
      break;
    case types::eutra_cell_id_r16:
      j.write_fieldname("eutra-CellId-r16");
      c.get<eutra_cell_id_r16_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE visited_cell_info_r16_s::visited_cell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_cell_id_r16:
      HANDLE_CODE(c.get<nr_cell_id_r16_c_>().pack(bref));
      break;
    case types::eutra_cell_id_r16:
      HANDLE_CODE(c.get<eutra_cell_id_r16_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_cell_info_r16_s::visited_cell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_cell_id_r16:
      HANDLE_CODE(c.get<nr_cell_id_r16_c_>().unpack(bref));
      break;
    case types::eutra_cell_id_r16:
      HANDLE_CODE(c.get<eutra_cell_id_r16_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::cgi_info:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r16:
      c.destroy<pci_arfcn_nr_r16_s>();
      break;
    default:
      break;
  }
}
void visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cgi_info:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r16:
      c.init<pci_arfcn_nr_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_");
  }
}
visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::nr_cell_id_r16_c_(
    const visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cgi_info:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r16:
      c.init(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_");
  }
}
visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_&
visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::operator=(
    const visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cgi_info:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r16:
      c.set(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_");
  }

  return *this;
}
cgi_info_logging_r16_s& visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::set_cgi_info()
{
  set(types::cgi_info);
  return c.get<cgi_info_logging_r16_s>();
}
pci_arfcn_nr_r16_s& visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::set_pci_arfcn_r16()
{
  set(types::pci_arfcn_r16);
  return c.get<pci_arfcn_nr_r16_s>();
}
void visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cgi_info:
      j.write_fieldname("cgi-Info");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::pci_arfcn_r16:
      j.write_fieldname("pci-arfcn-r16");
      c.get<pci_arfcn_nr_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cgi_info:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cgi_info:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cgi-Info", "pci-arfcn-r16"};
  return convert_enum_idx(names, 2, value, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::types");
}

void visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r16:
      c.destroy<cgi_info_eutra_s>();
      break;
    case types::pci_arfcn_r16:
      c.destroy<pci_arfcn_eutra_r16_s>();
      break;
    default:
      break;
  }
}
void visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r16:
      c.init<cgi_info_eutra_s>();
      break;
    case types::pci_arfcn_r16:
      c.init<pci_arfcn_eutra_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_");
  }
}
visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::eutra_cell_id_r16_c_(
    const visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r16:
      c.init(other.c.get<cgi_info_eutra_s>());
      break;
    case types::pci_arfcn_r16:
      c.init(other.c.get<pci_arfcn_eutra_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_");
  }
}
visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_&
visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::operator=(
    const visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r16:
      c.set(other.c.get<cgi_info_eutra_s>());
      break;
    case types::pci_arfcn_r16:
      c.set(other.c.get<pci_arfcn_eutra_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_");
  }

  return *this;
}
cgi_info_eutra_s& visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::set_cell_global_id_r16()
{
  set(types::cell_global_id_r16);
  return c.get<cgi_info_eutra_s>();
}
pci_arfcn_eutra_r16_s& visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::set_pci_arfcn_r16()
{
  set(types::pci_arfcn_r16);
  return c.get<pci_arfcn_eutra_r16_s>();
}
void visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r16:
      j.write_fieldname("cellGlobalId-r16");
      c.get<cgi_info_eutra_s>().to_json(j);
      break;
    case types::pci_arfcn_r16:
      j.write_fieldname("pci-arfcn-r16");
      c.get<pci_arfcn_eutra_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_s>().pack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_eutra_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_s>().unpack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_eutra_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(
      names, 2, value, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::types");
}

const char* visited_cell_info_r16_s::visited_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-CellId-r16", "eutra-CellId-r16"};
  return convert_enum_idx(names, 2, value, "visited_cell_info_r16_s::visited_cell_id_r16_c_::types");
}

// DRB-CountInfo ::= SEQUENCE
SRSASN_CODE drb_count_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, count_ul, (uint64_t)0u, (uint64_t)4294967295u));
  HANDLE_CODE(pack_integer(bref, count_dl, (uint64_t)0u, (uint64_t)4294967295u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_count_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(count_ul, bref, (uint64_t)0u, (uint64_t)4294967295u));
  HANDLE_CODE(unpack_integer(count_dl, bref, (uint64_t)0u, (uint64_t)4294967295u));

  return SRSASN_SUCCESS;
}
void drb_count_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Identity", drb_id);
  j.write_int("count-Uplink", count_ul);
  j.write_int("count-Downlink", count_dl);
  j.end_obj();
}

// FailureInfoDAPS-r16 ::= SEQUENCE
SRSASN_CODE fail_info_daps_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(fail_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_daps_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(fail_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void fail_info_daps_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("failureType-r16", fail_type_r16.to_string());
  j.end_obj();
}

const char* fail_info_daps_r16_s::fail_type_r16_opts::to_string() const
{
  static const char* names[] = {"daps-failure", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "fail_info_daps_r16_s::fail_type_r16_e_");
}

// MBS-ServiceInfo-r17 ::= SEQUENCE
SRSASN_CODE mbs_service_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(tmgi_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_service_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(tmgi_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbs_service_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tmgi-r17");
  tmgi_r17.to_json(j);
  j.end_obj();
}

// MeasReportAppLayer-r17 ::= SEQUENCE
SRSASN_CODE meas_report_app_layer_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_report_app_layer_container_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(app_layer_session_status_r17_present, 1));
  HANDLE_CODE(bref.pack(ran_visible_meass_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_cfg_app_layer_id_r17, (uint8_t)0u, (uint8_t)15u));
  if (meas_report_app_layer_container_r17.size() > 0) {
    HANDLE_CODE(meas_report_app_layer_container_r17.pack(bref));
  }
  if (app_layer_session_status_r17_present) {
    HANDLE_CODE(app_layer_session_status_r17.pack(bref));
  }
  if (ran_visible_meass_r17_present) {
    HANDLE_CODE(ran_visible_meass_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_app_layer_r17_s::unpack(cbit_ref& bref)
{
  bool meas_report_app_layer_container_r17_present;
  HANDLE_CODE(bref.unpack(meas_report_app_layer_container_r17_present, 1));
  HANDLE_CODE(bref.unpack(app_layer_session_status_r17_present, 1));
  HANDLE_CODE(bref.unpack(ran_visible_meass_r17_present, 1));

  HANDLE_CODE(unpack_integer(meas_cfg_app_layer_id_r17, bref, (uint8_t)0u, (uint8_t)15u));
  if (meas_report_app_layer_container_r17_present) {
    HANDLE_CODE(meas_report_app_layer_container_r17.unpack(bref));
  }
  if (app_layer_session_status_r17_present) {
    HANDLE_CODE(app_layer_session_status_r17.unpack(bref));
  }
  if (ran_visible_meass_r17_present) {
    HANDLE_CODE(ran_visible_meass_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_app_layer_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measConfigAppLayerId-r17", meas_cfg_app_layer_id_r17);
  if (meas_report_app_layer_container_r17.size() > 0) {
    j.write_str("measReportAppLayerContainer-r17", meas_report_app_layer_container_r17.to_string());
  }
  if (app_layer_session_status_r17_present) {
    j.write_str("appLayerSessionStatus-r17", app_layer_session_status_r17.to_string());
  }
  if (ran_visible_meass_r17_present) {
    j.write_fieldname("ran-VisibleMeasurements-r17");
    ran_visible_meass_r17.to_json(j);
  }
  j.end_obj();
}

const char* meas_report_app_layer_r17_s::app_layer_session_status_r17_opts::to_string() const
{
  static const char* names[] = {"started", "stopped"};
  return convert_enum_idx(names, 2, value, "meas_report_app_layer_r17_s::app_layer_session_status_r17_e_");
}

// MeasResultCLI-r16 ::= SEQUENCE
SRSASN_CODE meas_result_cli_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_list_srs_rsrp_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_list_cli_rssi_r16.size() > 0, 1));

  if (meas_result_list_srs_rsrp_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_srs_rsrp_r16, 1, 8));
  }
  if (meas_result_list_cli_rssi_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_cli_rssi_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cli_r16_s::unpack(cbit_ref& bref)
{
  bool meas_result_list_srs_rsrp_r16_present;
  HANDLE_CODE(bref.unpack(meas_result_list_srs_rsrp_r16_present, 1));
  bool meas_result_list_cli_rssi_r16_present;
  HANDLE_CODE(bref.unpack(meas_result_list_cli_rssi_r16_present, 1));

  if (meas_result_list_srs_rsrp_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_srs_rsrp_r16, bref, 1, 8));
  }
  if (meas_result_list_cli_rssi_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_cli_rssi_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_result_cli_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_list_srs_rsrp_r16.size() > 0) {
    j.start_array("measResultListSRS-RSRP-r16");
    for (const auto& e1 : meas_result_list_srs_rsrp_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_list_cli_rssi_r16.size() > 0) {
    j.start_array("measResultListCLI-RSSI-r16");
    for (const auto& e1 : meas_result_list_cli_rssi_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MeasResultForRSSI-r16 ::= SEQUENCE
SRSASN_CODE meas_result_for_rssi_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rssi_result_r16, (uint8_t)0u, (uint8_t)76u));
  HANDLE_CODE(pack_integer(bref, ch_occupancy_r16, (uint8_t)0u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_for_rssi_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rssi_result_r16, bref, (uint8_t)0u, (uint8_t)76u));
  HANDLE_CODE(unpack_integer(ch_occupancy_r16, bref, (uint8_t)0u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
void meas_result_for_rssi_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rssi-Result-r16", rssi_result_r16);
  j.write_int("channelOccupancy-r16", ch_occupancy_r16);
  j.end_obj();
}

// MeasResultIdleEUTRA-r16 ::= SEQUENCE
SRSASN_CODE meas_result_idle_eutra_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_results_per_carrier_list_idle_eutra_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_idle_eutra_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_results_per_carrier_list_idle_eutra_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result_idle_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measResultsPerCarrierListIdleEUTRA-r16");
  for (const auto& e1 : meas_results_per_carrier_list_idle_eutra_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasResultIdleNR-r16 ::= SEQUENCE
SRSASN_CODE meas_result_idle_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_results_per_carrier_list_idle_nr_r16.size() > 0, 1));

  HANDLE_CODE(bref.pack(meas_result_serving_cell_r16.rsrp_result_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_serving_cell_r16.rsrq_result_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_serving_cell_r16.results_ssb_idxes_r16.size() > 0, 1));
  if (meas_result_serving_cell_r16.rsrp_result_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_serving_cell_r16.rsrp_result_r16, (uint8_t)0u, (uint8_t)127u));
  }
  if (meas_result_serving_cell_r16.rsrq_result_r16_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_serving_cell_r16.rsrq_result_r16, (uint8_t)0u, (uint8_t)127u));
  }
  if (meas_result_serving_cell_r16.results_ssb_idxes_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_serving_cell_r16.results_ssb_idxes_r16, 1, 32));
  }
  if (meas_results_per_carrier_list_idle_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_results_per_carrier_list_idle_nr_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_idle_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_results_per_carrier_list_idle_nr_r16_present;
  HANDLE_CODE(bref.unpack(meas_results_per_carrier_list_idle_nr_r16_present, 1));

  HANDLE_CODE(bref.unpack(meas_result_serving_cell_r16.rsrp_result_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_serving_cell_r16.rsrq_result_r16_present, 1));
  bool results_ssb_idxes_r16_present;
  HANDLE_CODE(bref.unpack(results_ssb_idxes_r16_present, 1));
  if (meas_result_serving_cell_r16.rsrp_result_r16_present) {
    HANDLE_CODE(unpack_integer(meas_result_serving_cell_r16.rsrp_result_r16, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (meas_result_serving_cell_r16.rsrq_result_r16_present) {
    HANDLE_CODE(unpack_integer(meas_result_serving_cell_r16.rsrq_result_r16, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (results_ssb_idxes_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_serving_cell_r16.results_ssb_idxes_r16, bref, 1, 32));
  }
  if (meas_results_per_carrier_list_idle_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_results_per_carrier_list_idle_nr_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_result_idle_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResultServingCell-r16");
  j.start_obj();
  if (meas_result_serving_cell_r16.rsrp_result_r16_present) {
    j.write_int("rsrp-Result-r16", meas_result_serving_cell_r16.rsrp_result_r16);
  }
  if (meas_result_serving_cell_r16.rsrq_result_r16_present) {
    j.write_int("rsrq-Result-r16", meas_result_serving_cell_r16.rsrq_result_r16);
  }
  if (meas_result_serving_cell_r16.results_ssb_idxes_r16.size() > 0) {
    j.start_array("resultsSSB-Indexes-r16");
    for (const auto& e1 : meas_result_serving_cell_r16.results_ssb_idxes_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
  if (meas_results_per_carrier_list_idle_nr_r16.size() > 0) {
    j.start_array("measResultsPerCarrierListIdleNR-r16");
    for (const auto& e1 : meas_results_per_carrier_list_idle_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MeasResultRLFNR-r16 ::= SEQUENCE
SRSASN_CODE meas_result_rlf_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_r16.rs_idx_results_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_r16.cell_results_r16.results_ssb_cell_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_r16.cell_results_r16.results_csi_rs_cell_r16_present, 1));
  if (meas_result_r16.cell_results_r16.results_ssb_cell_r16_present) {
    HANDLE_CODE(meas_result_r16.cell_results_r16.results_ssb_cell_r16.pack(bref));
  }
  if (meas_result_r16.cell_results_r16.results_csi_rs_cell_r16_present) {
    HANDLE_CODE(meas_result_r16.cell_results_r16.results_csi_rs_cell_r16.pack(bref));
  }
  if (meas_result_r16.rs_idx_results_r16_present) {
    HANDLE_CODE(bref.pack(meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16.size() > 0, 1));
    HANDLE_CODE(bref.pack(meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16_present, 1));
    HANDLE_CODE(bref.pack(meas_result_r16.rs_idx_results_r16.results_csi_rs_idxes_r16.size() > 0, 1));
    HANDLE_CODE(bref.pack(meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16_present, 1));
    if (meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16, 1, 64));
    }
    if (meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16_present) {
      HANDLE_CODE(meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16.pack(bref));
    }
    if (meas_result_r16.rs_idx_results_r16.results_csi_rs_idxes_r16.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_r16.rs_idx_results_r16.results_csi_rs_idxes_r16, 1, 64));
    }
    if (meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16_present) {
      HANDLE_CODE(meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16.pack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_rlf_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_r16.rs_idx_results_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_r16.cell_results_r16.results_ssb_cell_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_r16.cell_results_r16.results_csi_rs_cell_r16_present, 1));
  if (meas_result_r16.cell_results_r16.results_ssb_cell_r16_present) {
    HANDLE_CODE(meas_result_r16.cell_results_r16.results_ssb_cell_r16.unpack(bref));
  }
  if (meas_result_r16.cell_results_r16.results_csi_rs_cell_r16_present) {
    HANDLE_CODE(meas_result_r16.cell_results_r16.results_csi_rs_cell_r16.unpack(bref));
  }
  if (meas_result_r16.rs_idx_results_r16_present) {
    bool results_ssb_idxes_r16_present;
    HANDLE_CODE(bref.unpack(results_ssb_idxes_r16_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16_present, 1));
    bool results_csi_rs_idxes_r16_present;
    HANDLE_CODE(bref.unpack(results_csi_rs_idxes_r16_present, 1));
    HANDLE_CODE(bref.unpack(meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16_present, 1));
    if (results_ssb_idxes_r16_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16, bref, 1, 64));
    }
    if (meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16_present) {
      HANDLE_CODE(meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16.unpack(bref));
    }
    if (results_csi_rs_idxes_r16_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_r16.rs_idx_results_r16.results_csi_rs_idxes_r16, bref, 1, 64));
    }
    if (meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16_present) {
      HANDLE_CODE(meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16.unpack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
void meas_result_rlf_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResult-r16");
  j.start_obj();
  j.write_fieldname("cellResults-r16");
  j.start_obj();
  if (meas_result_r16.cell_results_r16.results_ssb_cell_r16_present) {
    j.write_fieldname("resultsSSB-Cell-r16");
    meas_result_r16.cell_results_r16.results_ssb_cell_r16.to_json(j);
  }
  if (meas_result_r16.cell_results_r16.results_csi_rs_cell_r16_present) {
    j.write_fieldname("resultsCSI-RS-Cell-r16");
    meas_result_r16.cell_results_r16.results_csi_rs_cell_r16.to_json(j);
  }
  j.end_obj();
  if (meas_result_r16.rs_idx_results_r16_present) {
    j.write_fieldname("rsIndexResults-r16");
    j.start_obj();
    if (meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16.size() > 0) {
      j.start_array("resultsSSB-Indexes-r16");
      for (const auto& e1 : meas_result_r16.rs_idx_results_r16.results_ssb_idxes_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16_present) {
      j.write_str("ssbRLMConfigBitmap-r16", meas_result_r16.rs_idx_results_r16.ssb_rlm_cfg_bitmap_r16.to_string());
    }
    if (meas_result_r16.rs_idx_results_r16.results_csi_rs_idxes_r16.size() > 0) {
      j.start_array("resultsCSI-RS-Indexes-r16");
      for (const auto& e1 : meas_result_r16.rs_idx_results_r16.results_csi_rs_idxes_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16_present) {
      j.write_str("csi-rsRLMConfigBitmap-r16",
                  meas_result_r16.rs_idx_results_r16.csi_rs_rlm_cfg_bitmap_r16.to_string());
    }
    j.end_obj();
  }
  j.end_obj();
  j.end_obj();
}

// MeasResultRxTxTimeDiff-r17 ::= SEQUENCE
SRSASN_CODE meas_result_rx_tx_time_diff_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rx_tx_time_diff_ue_r17_present, 1));

  if (rx_tx_time_diff_ue_r17_present) {
    HANDLE_CODE(rx_tx_time_diff_ue_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_rx_tx_time_diff_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rx_tx_time_diff_ue_r17_present, 1));

  if (rx_tx_time_diff_ue_r17_present) {
    HANDLE_CODE(rx_tx_time_diff_ue_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_rx_tx_time_diff_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rx_tx_time_diff_ue_r17_present) {
    j.write_fieldname("rxTxTimeDiff-ue-r17");
    rx_tx_time_diff_ue_r17.to_json(j);
  }
  j.end_obj();
}

// MeasResultSFTD-EUTRA ::= SEQUENCE
SRSASN_CODE meas_result_sftd_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_result_present, 1));

  HANDLE_CODE(pack_integer(bref, eutra_pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, sfn_offset_result, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, frame_boundary_offset_result, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_sftd_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_result_present, 1));

  HANDLE_CODE(unpack_integer(eutra_pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(sfn_offset_result, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(frame_boundary_offset_result, bref, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(unpack_integer(rsrp_result, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_sftd_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-PhysCellId", eutra_pci);
  j.write_int("sfn-OffsetResult", sfn_offset_result);
  j.write_int("frameBoundaryOffsetResult", frame_boundary_offset_result);
  if (rsrp_result_present) {
    j.write_int("rsrp-Result", rsrp_result);
  }
  j.end_obj();
}

// MeasResultsSL-r16 ::= SEQUENCE
SRSASN_CODE meas_results_sl_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_results_list_sl_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_sl_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_results_list_sl_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_results_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResultsListSL-r16");
  meas_results_list_sl_r16.to_json(j);
  j.end_obj();
}

void meas_results_sl_r16_s::meas_results_list_sl_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResultNR-SL-r16");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE meas_results_sl_r16_s::meas_results_list_sl_r16_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_sl_r16_s::meas_results_list_sl_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "meas_results_sl_r16_s::meas_results_list_sl_r16_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* meas_results_sl_r16_s::meas_results_list_sl_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"measResultNR-SL-r16"};
  return convert_enum_idx(names, 1, value, "meas_results_sl_r16_s::meas_results_list_sl_r16_c_::types");
}

// OverheatingAssistance ::= SEQUENCE
SRSASN_CODE overheat_assist_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_ccs_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_bw_fr1_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_bw_fr2_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr1_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr2_present, 1));

  if (reduced_max_ccs_present) {
    HANDLE_CODE(reduced_max_ccs.pack(bref));
  }
  if (reduced_max_bw_fr1_present) {
    HANDLE_CODE(reduced_max_bw_fr1.pack(bref));
  }
  if (reduced_max_bw_fr2_present) {
    HANDLE_CODE(reduced_max_bw_fr2.pack(bref));
  }
  if (reduced_max_mimo_layers_fr1_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_dl.pack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_ul.pack(bref));
  }
  if (reduced_max_mimo_layers_fr2_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_dl.pack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE overheat_assist_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_ccs_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr1_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr2_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr1_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr2_present, 1));

  if (reduced_max_ccs_present) {
    HANDLE_CODE(reduced_max_ccs.unpack(bref));
  }
  if (reduced_max_bw_fr1_present) {
    HANDLE_CODE(reduced_max_bw_fr1.unpack(bref));
  }
  if (reduced_max_bw_fr2_present) {
    HANDLE_CODE(reduced_max_bw_fr2.unpack(bref));
  }
  if (reduced_max_mimo_layers_fr1_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_dl.unpack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_ul.unpack(bref));
  }
  if (reduced_max_mimo_layers_fr2_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_dl.unpack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void overheat_assist_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_ccs_present) {
    j.write_fieldname("reducedMaxCCs");
    reduced_max_ccs.to_json(j);
  }
  if (reduced_max_bw_fr1_present) {
    j.write_fieldname("reducedMaxBW-FR1");
    reduced_max_bw_fr1.to_json(j);
  }
  if (reduced_max_bw_fr2_present) {
    j.write_fieldname("reducedMaxBW-FR2");
    reduced_max_bw_fr2.to_json(j);
  }
  if (reduced_max_mimo_layers_fr1_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR1");
    j.start_obj();
    j.write_str("reducedMIMO-LayersFR1-DL", reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_dl.to_string());
    j.write_str("reducedMIMO-LayersFR1-UL", reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_ul.to_string());
    j.end_obj();
  }
  if (reduced_max_mimo_layers_fr2_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR2");
    j.start_obj();
    j.write_str("reducedMIMO-LayersFR2-DL", reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_dl.to_string());
    j.write_str("reducedMIMO-LayersFR2-UL", reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_ul.to_string());
    j.end_obj();
  }
  j.end_obj();
}

// RA-Report-r16 ::= SEQUENCE
SRSASN_CODE ra_report_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ra_info_common_r16_present, 1));

  HANDLE_CODE(cell_id_r16.pack(bref));
  if (ra_info_common_r16_present) {
    HANDLE_CODE(ra_info_common_r16.pack(bref));
  }
  HANDLE_CODE(ra_purpose_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sp_cell_id_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sp_cell_id_r17.is_present(), 1));
      if (sp_cell_id_r17.is_present()) {
        HANDLE_CODE(sp_cell_id_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_report_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ra_info_common_r16_present, 1));

  HANDLE_CODE(cell_id_r16.unpack(bref));
  if (ra_info_common_r16_present) {
    HANDLE_CODE(ra_info_common_r16.unpack(bref));
  }
  HANDLE_CODE(ra_purpose_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sp_cell_id_r17, bref);
      if (sp_cell_id_r17.is_present()) {
        HANDLE_CODE(sp_cell_id_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ra_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellId-r16");
  cell_id_r16.to_json(j);
  if (ra_info_common_r16_present) {
    j.write_fieldname("ra-InformationCommon-r16");
    ra_info_common_r16.to_json(j);
  }
  j.write_str("raPurpose-r16", ra_purpose_r16.to_string());
  if (ext) {
    if (sp_cell_id_r17.is_present()) {
      j.write_fieldname("spCellID-r17");
      sp_cell_id_r17->to_json(j);
    }
  }
  j.end_obj();
}

void ra_report_r16_s::cell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r16:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r16:
      c.destroy<pci_arfcn_nr_r16_s>();
      break;
    default:
      break;
  }
}
void ra_report_r16_s::cell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r16:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r16:
      c.init<pci_arfcn_nr_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ra_report_r16_s::cell_id_r16_c_");
  }
}
ra_report_r16_s::cell_id_r16_c_::cell_id_r16_c_(const ra_report_r16_s::cell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r16:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r16:
      c.init(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ra_report_r16_s::cell_id_r16_c_");
  }
}
ra_report_r16_s::cell_id_r16_c_&
ra_report_r16_s::cell_id_r16_c_::operator=(const ra_report_r16_s::cell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r16:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r16:
      c.set(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ra_report_r16_s::cell_id_r16_c_");
  }

  return *this;
}
cgi_info_logging_r16_s& ra_report_r16_s::cell_id_r16_c_::set_cell_global_id_r16()
{
  set(types::cell_global_id_r16);
  return c.get<cgi_info_logging_r16_s>();
}
pci_arfcn_nr_r16_s& ra_report_r16_s::cell_id_r16_c_::set_pci_arfcn_r16()
{
  set(types::pci_arfcn_r16);
  return c.get<pci_arfcn_nr_r16_s>();
}
void ra_report_r16_s::cell_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r16:
      j.write_fieldname("cellGlobalId-r16");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::pci_arfcn_r16:
      j.write_fieldname("pci-arfcn-r16");
      c.get<pci_arfcn_nr_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ra_report_r16_s::cell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE ra_report_r16_s::cell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ra_report_r16_s::cell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_report_r16_s::cell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ra_report_r16_s::cell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ra_report_r16_s::cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(names, 2, value, "ra_report_r16_s::cell_id_r16_c_::types");
}

const char* ra_report_r16_s::ra_purpose_r16_opts::to_string() const
{
  static const char* names[] = {"accessRelated",
                                "beamFailureRecovery",
                                "reconfigurationWithSync",
                                "ulUnSynchronized",
                                "schedulingRequestFailure",
                                "noPUCCHResourceAvailable",
                                "requestForOtherSI",
                                "msg3RequestForOtherSI-r17",
                                "spare8",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "ra_report_r16_s::ra_purpose_r16_e_");
}
uint8_t ra_report_r16_s::ra_purpose_r16_opts::to_number() const
{
  if (value == msg3_request_for_other_si_r17) {
    return 3;
  }
  invalid_enum_number(value, "ra_report_r16_s::ra_purpose_r16_e_");
  return 0;
}

// RRCReconfigurationComplete-v1560-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_resp_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_resp_present) {
    HANDLE_CODE(scg_resp.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_resp_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_resp_present) {
    HANDLE_CODE(scg_resp.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_resp_present) {
    j.write_fieldname("scg-Response");
    scg_resp.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void rrc_recfg_complete_v1560_ies_s::scg_resp_c_::destroy_()
{
  switch (type_) {
    case types::nr_scg_resp:
      c.destroy<dyn_octstring>();
      break;
    case types::eutra_scg_resp:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void rrc_recfg_complete_v1560_ies_s::scg_resp_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_scg_resp:
      c.init<dyn_octstring>();
      break;
    case types::eutra_scg_resp:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }
}
rrc_recfg_complete_v1560_ies_s::scg_resp_c_::scg_resp_c_(const rrc_recfg_complete_v1560_ies_s::scg_resp_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_scg_resp:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_resp:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }
}
rrc_recfg_complete_v1560_ies_s::scg_resp_c_&
rrc_recfg_complete_v1560_ies_s::scg_resp_c_::operator=(const rrc_recfg_complete_v1560_ies_s::scg_resp_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_scg_resp:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_resp:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }

  return *this;
}
dyn_octstring& rrc_recfg_complete_v1560_ies_s::scg_resp_c_::set_nr_scg_resp()
{
  set(types::nr_scg_resp);
  return c.get<dyn_octstring>();
}
dyn_octstring& rrc_recfg_complete_v1560_ies_s::scg_resp_c_::set_eutra_scg_resp()
{
  set(types::eutra_scg_resp);
  return c.get<dyn_octstring>();
}
void rrc_recfg_complete_v1560_ies_s::scg_resp_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_scg_resp:
      j.write_str("nr-SCG-Response", c.get<dyn_octstring>().to_string());
      break;
    case types::eutra_scg_resp:
      j.write_str("eutra-SCG-Response", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::scg_resp_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::eutra_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::scg_resp_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::eutra_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_recfg_complete_v1560_ies_s::scg_resp_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG-Response", "eutra-SCG-Response"};
  return convert_enum_idx(names, 2, value, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_::types");
}

// RRCResumeComplete-v1640-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_v1640_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_tx_direct_current_two_carrier_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_two_carrier_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_two_carrier_list_r16, 1, 64));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_v1640_ies_s::unpack(cbit_ref& bref)
{
  bool ul_tx_direct_current_two_carrier_list_r16_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_two_carrier_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_two_carrier_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_two_carrier_list_r16, bref, 1, 64));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_v1640_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_tx_direct_current_two_carrier_list_r16.size() > 0) {
    j.start_array("uplinkTxDirectCurrentTwoCarrierList-r16");
    for (const auto& e1 : ul_tx_direct_current_two_carrier_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCSetupComplete-v1690-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_complete_v1690_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_rrc_segmentation_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_v1690_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_rrc_segmentation_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_setup_complete_v1690_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_rrc_segmentation_r16_present) {
    j.write_str("ul-RRC-Segmentation-r16", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SL-Failure-r16 ::= SEQUENCE
SRSASN_CODE sl_fail_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_dest_id_r16.pack(bref));
  HANDLE_CODE(sl_fail_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_fail_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_dest_id_r16.unpack(bref));
  HANDLE_CODE(sl_fail_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_fail_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-DestinationIdentity-r16", sl_dest_id_r16.to_string());
  j.write_str("sl-Failure-r16", sl_fail_r16.to_string());
  j.end_obj();
}

const char* sl_fail_r16_s::sl_fail_r16_opts::to_string() const
{
  static const char* names[] = {
      "rlf", "configFailure", "drxReject-v1710", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_fail_r16_s::sl_fail_r16_e_");
}

// SuccessHO-Report-r17 ::= SEQUENCE
SRSASN_CODE success_ho_report_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r17_present, 1));
  HANDLE_CODE(bref.pack(location_info_r17_present, 1));
  HANDLE_CODE(bref.pack(time_since_cho_recfg_r17_present, 1));
  HANDLE_CODE(bref.pack(shr_cause_r17_present, 1));
  HANDLE_CODE(bref.pack(ra_info_common_r17_present, 1));
  HANDLE_CODE(bref.pack(up_interruption_time_at_ho_r17_present, 1));
  HANDLE_CODE(bref.pack(c_rnti_r17_present, 1));

  HANDLE_CODE(bref.pack(source_cell_info_r17.source_cell_meas_r17_present, 1));
  HANDLE_CODE(bref.pack(source_cell_info_r17.rlf_in_source_daps_r17_present, 1));
  HANDLE_CODE(source_cell_info_r17.source_pcell_id_r17.pack(bref));
  if (source_cell_info_r17.source_cell_meas_r17_present) {
    HANDLE_CODE(source_cell_info_r17.source_cell_meas_r17.pack(bref));
  }
  HANDLE_CODE(bref.pack(target_cell_info_r17.target_cell_meas_r17_present, 1));
  HANDLE_CODE(target_cell_info_r17.target_pcell_id_r17.pack(bref));
  if (target_cell_info_r17.target_cell_meas_r17_present) {
    HANDLE_CODE(target_cell_info_r17.target_cell_meas_r17.pack(bref));
  }
  if (meas_result_neigh_cells_r17_present) {
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r17.meas_result_list_nr_r17.size() > 0, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r17.meas_result_list_eutra_r17.size() > 0, 1));
    if (meas_result_neigh_cells_r17.meas_result_list_nr_r17.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r17.meas_result_list_nr_r17, 1, 8));
    }
    if (meas_result_neigh_cells_r17.meas_result_list_eutra_r17.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r17.meas_result_list_eutra_r17, 1, 8));
    }
  }
  if (location_info_r17_present) {
    HANDLE_CODE(location_info_r17.pack(bref));
  }
  if (time_since_cho_recfg_r17_present) {
    HANDLE_CODE(pack_integer(bref, time_since_cho_recfg_r17, (uint16_t)0u, (uint16_t)1023u));
  }
  if (shr_cause_r17_present) {
    HANDLE_CODE(shr_cause_r17.pack(bref));
  }
  if (ra_info_common_r17_present) {
    HANDLE_CODE(ra_info_common_r17.pack(bref));
  }
  if (up_interruption_time_at_ho_r17_present) {
    HANDLE_CODE(pack_integer(bref, up_interruption_time_at_ho_r17, (uint16_t)0u, (uint16_t)1023u));
  }
  if (c_rnti_r17_present) {
    HANDLE_CODE(pack_integer(bref, c_rnti_r17, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE success_ho_report_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r17_present, 1));
  HANDLE_CODE(bref.unpack(location_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(time_since_cho_recfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(shr_cause_r17_present, 1));
  HANDLE_CODE(bref.unpack(ra_info_common_r17_present, 1));
  HANDLE_CODE(bref.unpack(up_interruption_time_at_ho_r17_present, 1));
  HANDLE_CODE(bref.unpack(c_rnti_r17_present, 1));

  HANDLE_CODE(bref.unpack(source_cell_info_r17.source_cell_meas_r17_present, 1));
  HANDLE_CODE(bref.unpack(source_cell_info_r17.rlf_in_source_daps_r17_present, 1));
  HANDLE_CODE(source_cell_info_r17.source_pcell_id_r17.unpack(bref));
  if (source_cell_info_r17.source_cell_meas_r17_present) {
    HANDLE_CODE(source_cell_info_r17.source_cell_meas_r17.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(target_cell_info_r17.target_cell_meas_r17_present, 1));
  HANDLE_CODE(target_cell_info_r17.target_pcell_id_r17.unpack(bref));
  if (target_cell_info_r17.target_cell_meas_r17_present) {
    HANDLE_CODE(target_cell_info_r17.target_cell_meas_r17.unpack(bref));
  }
  if (meas_result_neigh_cells_r17_present) {
    bool meas_result_list_nr_r17_present;
    HANDLE_CODE(bref.unpack(meas_result_list_nr_r17_present, 1));
    bool meas_result_list_eutra_r17_present;
    HANDLE_CODE(bref.unpack(meas_result_list_eutra_r17_present, 1));
    if (meas_result_list_nr_r17_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r17.meas_result_list_nr_r17, bref, 1, 8));
    }
    if (meas_result_list_eutra_r17_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r17.meas_result_list_eutra_r17, bref, 1, 8));
    }
  }
  if (location_info_r17_present) {
    HANDLE_CODE(location_info_r17.unpack(bref));
  }
  if (time_since_cho_recfg_r17_present) {
    HANDLE_CODE(unpack_integer(time_since_cho_recfg_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (shr_cause_r17_present) {
    HANDLE_CODE(shr_cause_r17.unpack(bref));
  }
  if (ra_info_common_r17_present) {
    HANDLE_CODE(ra_info_common_r17.unpack(bref));
  }
  if (up_interruption_time_at_ho_r17_present) {
    HANDLE_CODE(unpack_integer(up_interruption_time_at_ho_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (c_rnti_r17_present) {
    HANDLE_CODE(unpack_integer(c_rnti_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
void success_ho_report_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sourceCellInfo-r17");
  j.start_obj();
  j.write_fieldname("sourcePCellId-r17");
  source_cell_info_r17.source_pcell_id_r17.to_json(j);
  if (source_cell_info_r17.source_cell_meas_r17_present) {
    j.write_fieldname("sourceCellMeas-r17");
    source_cell_info_r17.source_cell_meas_r17.to_json(j);
  }
  if (source_cell_info_r17.rlf_in_source_daps_r17_present) {
    j.write_str("rlf-InSourceDAPS-r17", "true");
  }
  j.end_obj();
  j.write_fieldname("targetCellInfo-r17");
  j.start_obj();
  j.write_fieldname("targetPCellId-r17");
  target_cell_info_r17.target_pcell_id_r17.to_json(j);
  if (target_cell_info_r17.target_cell_meas_r17_present) {
    j.write_fieldname("targetCellMeas-r17");
    target_cell_info_r17.target_cell_meas_r17.to_json(j);
  }
  j.end_obj();
  if (meas_result_neigh_cells_r17_present) {
    j.write_fieldname("measResultNeighCells-r17");
    j.start_obj();
    if (meas_result_neigh_cells_r17.meas_result_list_nr_r17.size() > 0) {
      j.start_array("measResultListNR-r17");
      for (const auto& e1 : meas_result_neigh_cells_r17.meas_result_list_nr_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r17.meas_result_list_eutra_r17.size() > 0) {
      j.start_array("measResultListEUTRA-r17");
      for (const auto& e1 : meas_result_neigh_cells_r17.meas_result_list_eutra_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (location_info_r17_present) {
    j.write_fieldname("locationInfo-r17");
    location_info_r17.to_json(j);
  }
  if (time_since_cho_recfg_r17_present) {
    j.write_int("timeSinceCHO-Reconfig-r17", time_since_cho_recfg_r17);
  }
  if (shr_cause_r17_present) {
    j.write_fieldname("shr-Cause-r17");
    shr_cause_r17.to_json(j);
  }
  if (ra_info_common_r17_present) {
    j.write_fieldname("ra-InformationCommon-r17");
    ra_info_common_r17.to_json(j);
  }
  if (up_interruption_time_at_ho_r17_present) {
    j.write_int("upInterruptionTimeAtHO-r17", up_interruption_time_at_ho_r17);
  }
  if (c_rnti_r17_present) {
    j.write_int("c-RNTI-r17", c_rnti_r17);
  }
  j.end_obj();
}

// UE-CapabilityRAT-Container ::= SEQUENCE
SRSASN_CODE ue_cap_rat_container_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rat_type.pack(bref));
  HANDLE_CODE(ue_cap_rat_container.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_rat_container_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rat_type.unpack(bref));
  HANDLE_CODE(ue_cap_rat_container.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_rat_container_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rat-Type", rat_type.to_string());
  j.write_str("ue-CapabilityRAT-Container", ue_cap_rat_container.to_string());
  j.end_obj();
}

// UE-TxTEG-Association-r17 ::= SEQUENCE
SRSASN_CODE ue_tx_teg_assoc_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_cell_id_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, ue_tx_teg_id_r17, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(nr_time_stamp_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, associated_srs_pos_res_id_list_r17, 1, 64, integer_packer<uint8_t>(0, 63)));
  if (serv_cell_id_r17_present) {
    HANDLE_CODE(pack_integer(bref, serv_cell_id_r17, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_tx_teg_assoc_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(serv_cell_id_r17_present, 1));

  HANDLE_CODE(unpack_integer(ue_tx_teg_id_r17, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(nr_time_stamp_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(associated_srs_pos_res_id_list_r17, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  if (serv_cell_id_r17_present) {
    HANDLE_CODE(unpack_integer(serv_cell_id_r17, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void ue_tx_teg_assoc_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ue-TxTEG-ID-r17", ue_tx_teg_id_r17);
  j.write_fieldname("nr-TimeStamp-r17");
  nr_time_stamp_r17.to_json(j);
  j.start_array("associatedSRS-PosResourceIdList-r17");
  for (const auto& e1 : associated_srs_pos_res_id_list_r17) {
    j.write_int(e1);
  }
  j.end_array();
  if (serv_cell_id_r17_present) {
    j.write_int("servCellId-r17", serv_cell_id_r17);
  }
  j.end_obj();
}

// UEAssistanceInformation-v1610-IEs ::= SEQUENCE
SRSASN_CODE ue_assist_info_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idc_assist_r16_present, 1));
  HANDLE_CODE(bref.pack(drx_pref_r16_present, 1));
  HANDLE_CODE(bref.pack(max_bw_pref_r16_present, 1));
  HANDLE_CODE(bref.pack(max_cc_pref_r16_present, 1));
  HANDLE_CODE(bref.pack(max_mimo_layer_pref_r16_present, 1));
  HANDLE_CODE(bref.pack(min_sched_offset_pref_r16_present, 1));
  HANDLE_CODE(bref.pack(release_pref_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ue_assist_info_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(ref_time_info_pref_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (idc_assist_r16_present) {
    HANDLE_CODE(idc_assist_r16.pack(bref));
  }
  if (drx_pref_r16_present) {
    HANDLE_CODE(drx_pref_r16.pack(bref));
  }
  if (max_bw_pref_r16_present) {
    HANDLE_CODE(max_bw_pref_r16.pack(bref));
  }
  if (max_cc_pref_r16_present) {
    HANDLE_CODE(max_cc_pref_r16.pack(bref));
  }
  if (max_mimo_layer_pref_r16_present) {
    HANDLE_CODE(max_mimo_layer_pref_r16.pack(bref));
  }
  if (min_sched_offset_pref_r16_present) {
    HANDLE_CODE(min_sched_offset_pref_r16.pack(bref));
  }
  if (release_pref_r16_present) {
    HANDLE_CODE(release_pref_r16.pack(bref));
  }
  if (sl_ue_assist_info_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_ue_assist_info_nr_r16, 1, 8));
  }
  if (ref_time_info_pref_r16_present) {
    HANDLE_CODE(bref.pack(ref_time_info_pref_r16, 1));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_assist_info_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idc_assist_r16_present, 1));
  HANDLE_CODE(bref.unpack(drx_pref_r16_present, 1));
  HANDLE_CODE(bref.unpack(max_bw_pref_r16_present, 1));
  HANDLE_CODE(bref.unpack(max_cc_pref_r16_present, 1));
  HANDLE_CODE(bref.unpack(max_mimo_layer_pref_r16_present, 1));
  HANDLE_CODE(bref.unpack(min_sched_offset_pref_r16_present, 1));
  HANDLE_CODE(bref.unpack(release_pref_r16_present, 1));
  bool sl_ue_assist_info_nr_r16_present;
  HANDLE_CODE(bref.unpack(sl_ue_assist_info_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(ref_time_info_pref_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (idc_assist_r16_present) {
    HANDLE_CODE(idc_assist_r16.unpack(bref));
  }
  if (drx_pref_r16_present) {
    HANDLE_CODE(drx_pref_r16.unpack(bref));
  }
  if (max_bw_pref_r16_present) {
    HANDLE_CODE(max_bw_pref_r16.unpack(bref));
  }
  if (max_cc_pref_r16_present) {
    HANDLE_CODE(max_cc_pref_r16.unpack(bref));
  }
  if (max_mimo_layer_pref_r16_present) {
    HANDLE_CODE(max_mimo_layer_pref_r16.unpack(bref));
  }
  if (min_sched_offset_pref_r16_present) {
    HANDLE_CODE(min_sched_offset_pref_r16.unpack(bref));
  }
  if (release_pref_r16_present) {
    HANDLE_CODE(release_pref_r16.unpack(bref));
  }
  if (sl_ue_assist_info_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_ue_assist_info_nr_r16, bref, 1, 8));
  }
  if (ref_time_info_pref_r16_present) {
    HANDLE_CODE(bref.unpack(ref_time_info_pref_r16, 1));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_assist_info_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idc_assist_r16_present) {
    j.write_fieldname("idc-Assistance-r16");
    idc_assist_r16.to_json(j);
  }
  if (drx_pref_r16_present) {
    j.write_fieldname("drx-Preference-r16");
    drx_pref_r16.to_json(j);
  }
  if (max_bw_pref_r16_present) {
    j.write_fieldname("maxBW-Preference-r16");
    max_bw_pref_r16.to_json(j);
  }
  if (max_cc_pref_r16_present) {
    j.write_fieldname("maxCC-Preference-r16");
    max_cc_pref_r16.to_json(j);
  }
  if (max_mimo_layer_pref_r16_present) {
    j.write_fieldname("maxMIMO-LayerPreference-r16");
    max_mimo_layer_pref_r16.to_json(j);
  }
  if (min_sched_offset_pref_r16_present) {
    j.write_fieldname("minSchedulingOffsetPreference-r16");
    min_sched_offset_pref_r16.to_json(j);
  }
  if (release_pref_r16_present) {
    j.write_fieldname("releasePreference-r16");
    release_pref_r16.to_json(j);
  }
  if (sl_ue_assist_info_nr_r16.size() > 0) {
    j.start_array("sl-UE-AssistanceInformationNR-r16");
    for (const auto& e1 : sl_ue_assist_info_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ref_time_info_pref_r16_present) {
    j.write_bool("referenceTimeInfoPreference-r16", ref_time_info_pref_r16);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// DelayBudgetReport ::= CHOICE
void delay_budget_report_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("type1", c.to_string());
  j.end_obj();
}
SRSASN_CODE delay_budget_report_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE delay_budget_report_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "delay_budget_report_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* delay_budget_report_c::type1_opts::to_string() const
{
  static const char* names[] = {"msMinus1280",
                                "msMinus640",
                                "msMinus320",
                                "msMinus160",
                                "msMinus80",
                                "msMinus60",
                                "msMinus40",
                                "msMinus20",
                                "ms0",
                                "ms20",
                                "ms40",
                                "ms60",
                                "ms80",
                                "ms160",
                                "ms320",
                                "ms640",
                                "ms1280"};
  return convert_enum_idx(names, 17, value, "delay_budget_report_c::type1_e_");
}
int16_t delay_budget_report_c::type1_opts::to_number() const
{
  static const int16_t numbers[] = {
      -1280, -640, -320, -160, -80, -60, -40, -20, 0, 20, 40, 60, 80, 160, 320, 640, 1280};
  return map_enum_number(numbers, 17, value, "delay_budget_report_c::type1_e_");
}

const char* delay_budget_report_c::types_opts::to_string() const
{
  static const char* names[] = {"type1"};
  return convert_enum_idx(names, 1, value, "delay_budget_report_c::types");
}
uint8_t delay_budget_report_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "delay_budget_report_c::types");
}

// FailureInfoRLC-Bearer ::= SEQUENCE
SRSASN_CODE fail_info_rlc_bearer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_group_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, lc_ch_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(fail_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_rlc_bearer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_group_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(lc_ch_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(fail_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void fail_info_rlc_bearer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellGroupId", cell_group_id);
  j.write_int("logicalChannelIdentity", lc_ch_id);
  j.write_str("failureType", fail_type.to_string());
  j.end_obj();
}

const char* fail_info_rlc_bearer_s::fail_type_opts::to_string() const
{
  static const char* names[] = {"rlc-failure", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "fail_info_rlc_bearer_s::fail_type_e_");
}

// FailureInformation-v1610-IEs ::= SEQUENCE
SRSASN_CODE fail_info_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_info_daps_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_info_daps_r16_present) {
    HANDLE_CODE(fail_info_daps_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_info_daps_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_info_daps_r16_present) {
    HANDLE_CODE(fail_info_daps_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void fail_info_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_info_daps_r16_present) {
    j.write_fieldname("failureInfoDAPS-r16");
    fail_info_daps_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// FailureReportMCG-r16 ::= SEQUENCE
SRSASN_CODE fail_report_mcg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(fail_type_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_freq_list_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_scg_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_scg_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_freq_list_utra_fdd_r16.size() > 0, 1));

  if (fail_type_r16_present) {
    HANDLE_CODE(fail_type_r16.pack(bref));
  }
  if (meas_result_freq_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list_r16, 1, 8));
  }
  if (meas_result_freq_list_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list_eutra_r16, 1, 8));
  }
  if (meas_result_scg_r16.size() > 0) {
    HANDLE_CODE(meas_result_scg_r16.pack(bref));
  }
  if (meas_result_scg_eutra_r16.size() > 0) {
    HANDLE_CODE(meas_result_scg_eutra_r16.pack(bref));
  }
  if (meas_result_freq_list_utra_fdd_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list_utra_fdd_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_mcg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(fail_type_r16_present, 1));
  bool meas_result_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(meas_result_freq_list_r16_present, 1));
  bool meas_result_freq_list_eutra_r16_present;
  HANDLE_CODE(bref.unpack(meas_result_freq_list_eutra_r16_present, 1));
  bool meas_result_scg_r16_present;
  HANDLE_CODE(bref.unpack(meas_result_scg_r16_present, 1));
  bool meas_result_scg_eutra_r16_present;
  HANDLE_CODE(bref.unpack(meas_result_scg_eutra_r16_present, 1));
  bool meas_result_freq_list_utra_fdd_r16_present;
  HANDLE_CODE(bref.unpack(meas_result_freq_list_utra_fdd_r16_present, 1));

  if (fail_type_r16_present) {
    HANDLE_CODE(fail_type_r16.unpack(bref));
  }
  if (meas_result_freq_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list_r16, bref, 1, 8));
  }
  if (meas_result_freq_list_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list_eutra_r16, bref, 1, 8));
  }
  if (meas_result_scg_r16_present) {
    HANDLE_CODE(meas_result_scg_r16.unpack(bref));
  }
  if (meas_result_scg_eutra_r16_present) {
    HANDLE_CODE(meas_result_scg_eutra_r16.unpack(bref));
  }
  if (meas_result_freq_list_utra_fdd_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list_utra_fdd_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void fail_report_mcg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_type_r16_present) {
    j.write_str("failureType-r16", fail_type_r16.to_string());
  }
  if (meas_result_freq_list_r16.size() > 0) {
    j.start_array("measResultFreqList-r16");
    for (const auto& e1 : meas_result_freq_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_freq_list_eutra_r16.size() > 0) {
    j.start_array("measResultFreqListEUTRA-r16");
    for (const auto& e1 : meas_result_freq_list_eutra_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_scg_r16.size() > 0) {
    j.write_str("measResultSCG-r16", meas_result_scg_r16.to_string());
  }
  if (meas_result_scg_eutra_r16.size() > 0) {
    j.write_str("measResultSCG-EUTRA-r16", meas_result_scg_eutra_r16.to_string());
  }
  if (meas_result_freq_list_utra_fdd_r16.size() > 0) {
    j.start_array("measResultFreqListUTRA-FDD-r16");
    for (const auto& e1 : meas_result_freq_list_utra_fdd_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* fail_report_mcg_r16_s::fail_type_r16_opts::to_string() const
{
  static const char* names[] = {"t310-Expiry",
                                "randomAccessProblem",
                                "rlc-MaxNumRetx",
                                "t312-Expiry-r16",
                                "lbt-Failure-r16",
                                "beamFailureRecoveryFailure-r16",
                                "bh-RLF-r16",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "fail_report_mcg_r16_s::fail_type_r16_e_");
}
uint16_t fail_report_mcg_r16_s::fail_type_r16_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case t312_expiry_r16:
      return 312;
    default:
      invalid_enum_number(value, "fail_report_mcg_r16_s::fail_type_r16_e_");
  }
  return 0;
}

// FailureReportSCG ::= SEQUENCE
SRSASN_CODE fail_report_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_freq_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_scg_fail.size() > 0, 1));

  HANDLE_CODE(fail_type.pack(bref));
  if (meas_result_freq_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list, 1, 8));
  }
  if (meas_result_scg_fail.size() > 0) {
    HANDLE_CODE(meas_result_scg_fail.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= location_info_r16.is_present();
    group_flags[0] |= fail_type_v1610_present;
    group_flags[1] |= prev_pscell_id_r17.is_present();
    group_flags[1] |= failed_pscell_id_r17.is_present();
    group_flags[1] |= time_scg_fail_r17_present;
    group_flags[1] |= per_ra_info_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(location_info_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(fail_type_v1610_present, 1));
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->pack(bref));
      }
      if (fail_type_v1610_present) {
        HANDLE_CODE(fail_type_v1610.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(prev_pscell_id_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(failed_pscell_id_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(time_scg_fail_r17_present, 1));
      HANDLE_CODE(bref.pack(per_ra_info_list_r17.is_present(), 1));
      if (prev_pscell_id_r17.is_present()) {
        HANDLE_CODE(pack_integer(bref, prev_pscell_id_r17->pci_r17, (uint16_t)0u, (uint16_t)1007u));
        HANDLE_CODE(pack_integer(bref, prev_pscell_id_r17->carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
      }
      if (failed_pscell_id_r17.is_present()) {
        HANDLE_CODE(pack_integer(bref, failed_pscell_id_r17->pci_r17, (uint16_t)0u, (uint16_t)1007u));
        HANDLE_CODE(pack_integer(bref, failed_pscell_id_r17->carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
      }
      if (time_scg_fail_r17_present) {
        HANDLE_CODE(pack_integer(bref, time_scg_fail_r17, (uint16_t)0u, (uint16_t)1023u));
      }
      if (per_ra_info_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *per_ra_info_list_r17, 1, 200));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_result_freq_list_present;
  HANDLE_CODE(bref.unpack(meas_result_freq_list_present, 1));
  bool meas_result_scg_fail_present;
  HANDLE_CODE(bref.unpack(meas_result_scg_fail_present, 1));

  HANDLE_CODE(fail_type.unpack(bref));
  if (meas_result_freq_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list, bref, 1, 8));
  }
  if (meas_result_scg_fail_present) {
    HANDLE_CODE(meas_result_scg_fail.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(location_info_r16, bref);
      HANDLE_CODE(bref.unpack(fail_type_v1610_present, 1));
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->unpack(bref));
      }
      if (fail_type_v1610_present) {
        HANDLE_CODE(fail_type_v1610.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(prev_pscell_id_r17, bref);
      unpack_presence_flag(failed_pscell_id_r17, bref);
      HANDLE_CODE(bref.unpack(time_scg_fail_r17_present, 1));
      unpack_presence_flag(per_ra_info_list_r17, bref);
      if (prev_pscell_id_r17.is_present()) {
        HANDLE_CODE(unpack_integer(prev_pscell_id_r17->pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
        HANDLE_CODE(unpack_integer(prev_pscell_id_r17->carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
      }
      if (failed_pscell_id_r17.is_present()) {
        HANDLE_CODE(unpack_integer(failed_pscell_id_r17->pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
        HANDLE_CODE(unpack_integer(failed_pscell_id_r17->carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
      }
      if (time_scg_fail_r17_present) {
        HANDLE_CODE(unpack_integer(time_scg_fail_r17, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (per_ra_info_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*per_ra_info_list_r17, bref, 1, 200));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void fail_report_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("failureType", fail_type.to_string());
  if (meas_result_freq_list.size() > 0) {
    j.start_array("measResultFreqList");
    for (const auto& e1 : meas_result_freq_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_scg_fail.size() > 0) {
    j.write_str("measResultSCG-Failure", meas_result_scg_fail.to_string());
  }
  if (ext) {
    if (location_info_r16.is_present()) {
      j.write_fieldname("locationInfo-r16");
      location_info_r16->to_json(j);
    }
    if (fail_type_v1610_present) {
      j.write_str("failureType-v1610", fail_type_v1610.to_string());
    }
    if (prev_pscell_id_r17.is_present()) {
      j.write_fieldname("previousPSCellId-r17");
      j.start_obj();
      j.write_int("physCellId-r17", prev_pscell_id_r17->pci_r17);
      j.write_int("carrierFreq-r17", prev_pscell_id_r17->carrier_freq_r17);
      j.end_obj();
    }
    if (failed_pscell_id_r17.is_present()) {
      j.write_fieldname("failedPSCellId-r17");
      j.start_obj();
      j.write_int("physCellId-r17", failed_pscell_id_r17->pci_r17);
      j.write_int("carrierFreq-r17", failed_pscell_id_r17->carrier_freq_r17);
      j.end_obj();
    }
    if (time_scg_fail_r17_present) {
      j.write_int("timeSCGFailure-r17", time_scg_fail_r17);
    }
    if (per_ra_info_list_r17.is_present()) {
      j.start_array("perRAInfoList-r17");
      for (const auto& e1 : *per_ra_info_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* fail_report_scg_s::fail_type_opts::to_string() const
{
  static const char* names[] = {"t310-Expiry",
                                "randomAccessProblem",
                                "rlc-MaxNumRetx",
                                "synchReconfigFailureSCG",
                                "scg-ReconfigFailure",
                                "srb3-IntegrityFailure",
                                "other-r16",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "fail_report_scg_s::fail_type_e_");
}
uint16_t fail_report_scg_s::fail_type_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case srb3_integrity_fail:
      return 3;
    default:
      invalid_enum_number(value, "fail_report_scg_s::fail_type_e_");
  }
  return 0;
}

const char* fail_report_scg_s::fail_type_v1610_opts::to_string() const
{
  static const char* names[] = {"scg-lbtFailure-r16",
                                "beamFailureRecoveryFailure-r16",
                                "t312-Expiry-r16",
                                "bh-RLF-r16",
                                "beamFailure-r17",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "fail_report_scg_s::fail_type_v1610_e_");
}
uint16_t fail_report_scg_s::fail_type_v1610_opts::to_number() const
{
  if (value == t312_expiry_r16) {
    return 312;
  }
  invalid_enum_number(value, "fail_report_scg_s::fail_type_v1610_e_");
  return 0;
}

// FailureReportSCG-EUTRA ::= SEQUENCE
SRSASN_CODE fail_report_scg_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_freq_list_mrdc.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_scg_fail_mrdc.size() > 0, 1));

  HANDLE_CODE(fail_type.pack(bref));
  if (meas_result_freq_list_mrdc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list_mrdc, 1, 8));
  }
  if (meas_result_scg_fail_mrdc.size() > 0) {
    HANDLE_CODE(meas_result_scg_fail_mrdc.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= location_info_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(location_info_r16.is_present(), 1));
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_scg_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_result_freq_list_mrdc_present;
  HANDLE_CODE(bref.unpack(meas_result_freq_list_mrdc_present, 1));
  bool meas_result_scg_fail_mrdc_present;
  HANDLE_CODE(bref.unpack(meas_result_scg_fail_mrdc_present, 1));

  HANDLE_CODE(fail_type.unpack(bref));
  if (meas_result_freq_list_mrdc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list_mrdc, bref, 1, 8));
  }
  if (meas_result_scg_fail_mrdc_present) {
    HANDLE_CODE(meas_result_scg_fail_mrdc.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(location_info_r16, bref);
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void fail_report_scg_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("failureType", fail_type.to_string());
  if (meas_result_freq_list_mrdc.size() > 0) {
    j.start_array("measResultFreqListMRDC");
    for (const auto& e1 : meas_result_freq_list_mrdc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_scg_fail_mrdc.size() > 0) {
    j.write_str("measResultSCG-FailureMRDC", meas_result_scg_fail_mrdc.to_string());
  }
  if (ext) {
    if (location_info_r16.is_present()) {
      j.write_fieldname("locationInfo-r16");
      location_info_r16->to_json(j);
    }
  }
  j.end_obj();
}

const char* fail_report_scg_eutra_s::fail_type_opts::to_string() const
{
  static const char* names[] = {"t313-Expiry",
                                "randomAccessProblem",
                                "rlc-MaxNumRetx",
                                "scg-ChangeFailure",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "fail_report_scg_eutra_s::fail_type_e_");
}
uint16_t fail_report_scg_eutra_s::fail_type_opts::to_number() const
{
  static const uint16_t numbers[] = {313};
  return map_enum_number(numbers, 1, value, "fail_report_scg_eutra_s::fail_type_e_");
}

// IAB-IP-AddressAndTraffic-r16 ::= SEQUENCE
SRSASN_CODE iab_ip_address_and_traffic_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(all_traffic_iab_ip_address_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(f1_c_traffic_ip_address_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(f1_u_traffic_ip_address_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_f1_traffic_ip_address_r16.size() > 0, 1));

  if (all_traffic_iab_ip_address_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, all_traffic_iab_ip_address_r16, 1, 8));
  }
  if (f1_c_traffic_ip_address_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, f1_c_traffic_ip_address_r16, 1, 8));
  }
  if (f1_u_traffic_ip_address_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, f1_u_traffic_ip_address_r16, 1, 8));
  }
  if (non_f1_traffic_ip_address_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, non_f1_traffic_ip_address_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_ip_address_and_traffic_r16_s::unpack(cbit_ref& bref)
{
  bool all_traffic_iab_ip_address_r16_present;
  HANDLE_CODE(bref.unpack(all_traffic_iab_ip_address_r16_present, 1));
  bool f1_c_traffic_ip_address_r16_present;
  HANDLE_CODE(bref.unpack(f1_c_traffic_ip_address_r16_present, 1));
  bool f1_u_traffic_ip_address_r16_present;
  HANDLE_CODE(bref.unpack(f1_u_traffic_ip_address_r16_present, 1));
  bool non_f1_traffic_ip_address_r16_present;
  HANDLE_CODE(bref.unpack(non_f1_traffic_ip_address_r16_present, 1));

  if (all_traffic_iab_ip_address_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(all_traffic_iab_ip_address_r16, bref, 1, 8));
  }
  if (f1_c_traffic_ip_address_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(f1_c_traffic_ip_address_r16, bref, 1, 8));
  }
  if (f1_u_traffic_ip_address_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(f1_u_traffic_ip_address_r16, bref, 1, 8));
  }
  if (non_f1_traffic_ip_address_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(non_f1_traffic_ip_address_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void iab_ip_address_and_traffic_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (all_traffic_iab_ip_address_r16.size() > 0) {
    j.start_array("all-Traffic-IAB-IP-Address-r16");
    for (const auto& e1 : all_traffic_iab_ip_address_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (f1_c_traffic_ip_address_r16.size() > 0) {
    j.start_array("f1-C-Traffic-IP-Address-r16");
    for (const auto& e1 : f1_c_traffic_ip_address_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (f1_u_traffic_ip_address_r16.size() > 0) {
    j.start_array("f1-U-Traffic-IP-Address-r16");
    for (const auto& e1 : f1_u_traffic_ip_address_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_f1_traffic_ip_address_r16.size() > 0) {
    j.start_array("non-F1-Traffic-IP-Address-r16");
    for (const auto& e1 : non_f1_traffic_ip_address_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// IAB-IP-AddressNumReq-r16 ::= SEQUENCE
SRSASN_CODE iab_ip_address_num_req_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(all_traffic_num_req_r16_present, 1));
  HANDLE_CODE(bref.pack(f1_c_traffic_num_req_r16_present, 1));
  HANDLE_CODE(bref.pack(f1_u_traffic_num_req_r16_present, 1));
  HANDLE_CODE(bref.pack(non_f1_traffic_num_req_r16_present, 1));

  if (all_traffic_num_req_r16_present) {
    HANDLE_CODE(pack_integer(bref, all_traffic_num_req_r16, (uint8_t)1u, (uint8_t)8u));
  }
  if (f1_c_traffic_num_req_r16_present) {
    HANDLE_CODE(pack_integer(bref, f1_c_traffic_num_req_r16, (uint8_t)1u, (uint8_t)8u));
  }
  if (f1_u_traffic_num_req_r16_present) {
    HANDLE_CODE(pack_integer(bref, f1_u_traffic_num_req_r16, (uint8_t)1u, (uint8_t)8u));
  }
  if (non_f1_traffic_num_req_r16_present) {
    HANDLE_CODE(pack_integer(bref, non_f1_traffic_num_req_r16, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_ip_address_num_req_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(all_traffic_num_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(f1_c_traffic_num_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(f1_u_traffic_num_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_f1_traffic_num_req_r16_present, 1));

  if (all_traffic_num_req_r16_present) {
    HANDLE_CODE(unpack_integer(all_traffic_num_req_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (f1_c_traffic_num_req_r16_present) {
    HANDLE_CODE(unpack_integer(f1_c_traffic_num_req_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (f1_u_traffic_num_req_r16_present) {
    HANDLE_CODE(unpack_integer(f1_u_traffic_num_req_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (non_f1_traffic_num_req_r16_present) {
    HANDLE_CODE(unpack_integer(non_f1_traffic_num_req_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void iab_ip_address_num_req_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (all_traffic_num_req_r16_present) {
    j.write_int("all-Traffic-NumReq-r16", all_traffic_num_req_r16);
  }
  if (f1_c_traffic_num_req_r16_present) {
    j.write_int("f1-C-Traffic-NumReq-r16", f1_c_traffic_num_req_r16);
  }
  if (f1_u_traffic_num_req_r16_present) {
    j.write_int("f1-U-Traffic-NumReq-r16", f1_u_traffic_num_req_r16);
  }
  if (non_f1_traffic_num_req_r16_present) {
    j.write_int("non-F1-Traffic-NumReq-r16", non_f1_traffic_num_req_r16);
  }
  j.end_obj();
}

// IAB-IP-AddressPrefixReq-r16 ::= SEQUENCE
SRSASN_CODE iab_ip_address_prefix_req_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(all_traffic_prefix_req_r16_present, 1));
  HANDLE_CODE(bref.pack(f1_c_traffic_prefix_req_r16_present, 1));
  HANDLE_CODE(bref.pack(f1_u_traffic_prefix_req_r16_present, 1));
  HANDLE_CODE(bref.pack(non_f1_traffic_prefix_req_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_ip_address_prefix_req_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(all_traffic_prefix_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(f1_c_traffic_prefix_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(f1_u_traffic_prefix_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_f1_traffic_prefix_req_r16_present, 1));

  return SRSASN_SUCCESS;
}
void iab_ip_address_prefix_req_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (all_traffic_prefix_req_r16_present) {
    j.write_str("all-Traffic-PrefixReq-r16", "true");
  }
  if (f1_c_traffic_prefix_req_r16_present) {
    j.write_str("f1-C-Traffic-PrefixReq-r16", "true");
  }
  if (f1_u_traffic_prefix_req_r16_present) {
    j.write_str("f1-U-Traffic-PrefixReq-r16", "true");
  }
  if (non_f1_traffic_prefix_req_r16_present) {
    j.write_str("non-F1-Traffic-PrefixReq-r16", "true");
  }
  j.end_obj();
}

// IAB-IP-PrefixAndTraffic-r16 ::= SEQUENCE
SRSASN_CODE iab_ip_prefix_and_traffic_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(all_traffic_iab_ip_address_r16_present, 1));
  HANDLE_CODE(bref.pack(f1_c_traffic_ip_address_r16_present, 1));
  HANDLE_CODE(bref.pack(f1_u_traffic_ip_address_r16_present, 1));
  HANDLE_CODE(bref.pack(non_f1_traffic_ip_address_r16_present, 1));

  if (all_traffic_iab_ip_address_r16_present) {
    HANDLE_CODE(all_traffic_iab_ip_address_r16.pack(bref));
  }
  if (f1_c_traffic_ip_address_r16_present) {
    HANDLE_CODE(f1_c_traffic_ip_address_r16.pack(bref));
  }
  if (f1_u_traffic_ip_address_r16_present) {
    HANDLE_CODE(f1_u_traffic_ip_address_r16.pack(bref));
  }
  if (non_f1_traffic_ip_address_r16_present) {
    HANDLE_CODE(non_f1_traffic_ip_address_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_ip_prefix_and_traffic_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(all_traffic_iab_ip_address_r16_present, 1));
  HANDLE_CODE(bref.unpack(f1_c_traffic_ip_address_r16_present, 1));
  HANDLE_CODE(bref.unpack(f1_u_traffic_ip_address_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_f1_traffic_ip_address_r16_present, 1));

  if (all_traffic_iab_ip_address_r16_present) {
    HANDLE_CODE(all_traffic_iab_ip_address_r16.unpack(bref));
  }
  if (f1_c_traffic_ip_address_r16_present) {
    HANDLE_CODE(f1_c_traffic_ip_address_r16.unpack(bref));
  }
  if (f1_u_traffic_ip_address_r16_present) {
    HANDLE_CODE(f1_u_traffic_ip_address_r16.unpack(bref));
  }
  if (non_f1_traffic_ip_address_r16_present) {
    HANDLE_CODE(non_f1_traffic_ip_address_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void iab_ip_prefix_and_traffic_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (all_traffic_iab_ip_address_r16_present) {
    j.write_fieldname("all-Traffic-IAB-IP-Address-r16");
    all_traffic_iab_ip_address_r16.to_json(j);
  }
  if (f1_c_traffic_ip_address_r16_present) {
    j.write_fieldname("f1-C-Traffic-IP-Address-r16");
    f1_c_traffic_ip_address_r16.to_json(j);
  }
  if (f1_u_traffic_ip_address_r16_present) {
    j.write_fieldname("f1-U-Traffic-IP-Address-r16");
    f1_u_traffic_ip_address_r16.to_json(j);
  }
  if (non_f1_traffic_ip_address_r16_present) {
    j.write_fieldname("non-F1-Traffic-IP-Address-r16");
    non_f1_traffic_ip_address_r16.to_json(j);
  }
  j.end_obj();
}

// LocationMeasurementInfo ::= CHOICE
void location_meas_info_c::destroy_()
{
  switch (type_) {
    case types::eutra_rstd:
      c.destroy<eutra_rstd_info_list_l>();
      break;
    case types::nr_prs_meas_r16:
      c.destroy<nr_prs_meas_info_list_r16_l>();
      break;
    default:
      break;
  }
}
void location_meas_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eutra_rstd:
      c.init<eutra_rstd_info_list_l>();
      break;
    case types::eutra_fine_timing_detection:
      break;
    case types::nr_prs_meas_r16:
      c.init<nr_prs_meas_info_list_r16_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
  }
}
location_meas_info_c::location_meas_info_c(const location_meas_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eutra_rstd:
      c.init(other.c.get<eutra_rstd_info_list_l>());
      break;
    case types::eutra_fine_timing_detection:
      break;
    case types::nr_prs_meas_r16:
      c.init(other.c.get<nr_prs_meas_info_list_r16_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
  }
}
location_meas_info_c& location_meas_info_c::operator=(const location_meas_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eutra_rstd:
      c.set(other.c.get<eutra_rstd_info_list_l>());
      break;
    case types::eutra_fine_timing_detection:
      break;
    case types::nr_prs_meas_r16:
      c.set(other.c.get<nr_prs_meas_info_list_r16_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
  }

  return *this;
}
eutra_rstd_info_list_l& location_meas_info_c::set_eutra_rstd()
{
  set(types::eutra_rstd);
  return c.get<eutra_rstd_info_list_l>();
}
void location_meas_info_c::set_eutra_fine_timing_detection()
{
  set(types::eutra_fine_timing_detection);
}
nr_prs_meas_info_list_r16_l& location_meas_info_c::set_nr_prs_meas_r16()
{
  set(types::nr_prs_meas_r16);
  return c.get<nr_prs_meas_info_list_r16_l>();
}
void location_meas_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eutra_rstd:
      j.start_array("eutra-RSTD");
      for (const auto& e1 : c.get<eutra_rstd_info_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::eutra_fine_timing_detection:
      break;
    case types::nr_prs_meas_r16:
      j.start_array("nr-PRS-Measurement-r16");
      for (const auto& e1 : c.get<nr_prs_meas_info_list_r16_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
  }
  j.end_obj();
}
SRSASN_CODE location_meas_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eutra_rstd:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<eutra_rstd_info_list_l>(), 1, 3));
      break;
    case types::eutra_fine_timing_detection:
      break;
    case types::nr_prs_meas_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<nr_prs_meas_info_list_r16_l>(), 1, 4));
    } break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eutra_rstd:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<eutra_rstd_info_list_l>(), bref, 1, 3));
      break;
    case types::eutra_fine_timing_detection:
      break;
    case types::nr_prs_meas_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_dyn_seq_of(c.get<nr_prs_meas_info_list_r16_l>(), bref, 1, 4));
    } break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* location_meas_info_c::types_opts::to_string() const
{
  static const char* names[] = {"eutra-RSTD", "eutra-FineTimingDetection", "nr-PRS-Measurement-r16"};
  return convert_enum_idx(names, 3, value, "location_meas_info_c::types");
}

// LogMeasReport-r16 ::= SEQUENCE
SRSASN_CODE log_meas_report_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(log_meas_available_r16_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_bt_r16_present, 1));
  HANDLE_CODE(bref.pack(log_meas_available_wlan_r16_present, 1));

  HANDLE_CODE(absolute_time_stamp_r16.pack(bref));
  HANDLE_CODE(trace_ref_r16.pack(bref));
  HANDLE_CODE(trace_recording_session_ref_r16.pack(bref));
  HANDLE_CODE(tce_id_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, log_meas_info_list_r16, 1, 520));

  return SRSASN_SUCCESS;
}
SRSASN_CODE log_meas_report_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(log_meas_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_bt_r16_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_available_wlan_r16_present, 1));

  HANDLE_CODE(absolute_time_stamp_r16.unpack(bref));
  HANDLE_CODE(trace_ref_r16.unpack(bref));
  HANDLE_CODE(trace_recording_session_ref_r16.unpack(bref));
  HANDLE_CODE(tce_id_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(log_meas_info_list_r16, bref, 1, 520));

  return SRSASN_SUCCESS;
}
void log_meas_report_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("absoluteTimeStamp-r16", absolute_time_stamp_r16.to_string());
  j.write_fieldname("traceReference-r16");
  trace_ref_r16.to_json(j);
  j.write_str("traceRecordingSessionRef-r16", trace_recording_session_ref_r16.to_string());
  j.write_str("tce-Id-r16", tce_id_r16.to_string());
  j.start_array("logMeasInfoList-r16");
  for (const auto& e1 : log_meas_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (log_meas_available_r16_present) {
    j.write_str("logMeasAvailable-r16", "true");
  }
  if (log_meas_available_bt_r16_present) {
    j.write_str("logMeasAvailableBT-r16", "true");
  }
  if (log_meas_available_wlan_r16_present) {
    j.write_str("logMeasAvailableWLAN-r16", "true");
  }
  j.end_obj();
}

// MeasResults ::= SEQUENCE
SRSASN_CODE meas_results_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_serving_mo_list, 1, 32));
  if (meas_result_neigh_cells_present) {
    HANDLE_CODE(meas_result_neigh_cells.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_serv_freq_list_eutra_scg.is_present();
    group_flags[0] |= meas_result_serv_freq_list_nr_scg.is_present();
    group_flags[0] |= meas_result_sftd_eutra.is_present();
    group_flags[0] |= meas_result_sftd_nr.is_present();
    group_flags[1] |= meas_result_cell_list_sftd_nr.is_present();
    group_flags[2] |= meas_result_for_rssi_r16.is_present();
    group_flags[2] |= location_info_r16.is_present();
    group_flags[2] |= ul_pdcp_delay_value_result_list_r16.is_present();
    group_flags[2] |= meas_results_sl_r16.is_present();
    group_flags[2] |= meas_result_cli_r16.is_present();
    group_flags[3] |= meas_result_rx_tx_time_diff_r17.is_present();
    group_flags[3] |= sl_meas_result_serving_relay_r17.size() > 0;
    group_flags[3] |= ul_pdcp_excess_delay_result_list_r17.is_present();
    group_flags[3] |= coarse_location_info_r17.size() > 0;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_eutra_scg.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_nr_scg.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_sftd_eutra.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_sftd_nr.is_present(), 1));
      if (meas_result_serv_freq_list_eutra_scg.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_eutra_scg, 1, 32));
      }
      if (meas_result_serv_freq_list_nr_scg.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_nr_scg, 1, 32));
      }
      if (meas_result_sftd_eutra.is_present()) {
        HANDLE_CODE(meas_result_sftd_eutra->pack(bref));
      }
      if (meas_result_sftd_nr.is_present()) {
        HANDLE_CODE(meas_result_sftd_nr->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_nr.is_present(), 1));
      if (meas_result_cell_list_sftd_nr.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_cell_list_sftd_nr, 1, 3));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_for_rssi_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(location_info_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pdcp_delay_value_result_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_results_sl_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_cli_r16.is_present(), 1));
      if (meas_result_for_rssi_r16.is_present()) {
        HANDLE_CODE(meas_result_for_rssi_r16->pack(bref));
      }
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->pack(bref));
      }
      if (ul_pdcp_delay_value_result_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ul_pdcp_delay_value_result_list_r16, 1, 29));
      }
      if (meas_results_sl_r16.is_present()) {
        HANDLE_CODE(meas_results_sl_r16->pack(bref));
      }
      if (meas_result_cli_r16.is_present()) {
        HANDLE_CODE(meas_result_cli_r16->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_rx_tx_time_diff_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_meas_result_serving_relay_r17.size() > 0, 1));
      HANDLE_CODE(bref.pack(ul_pdcp_excess_delay_result_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(coarse_location_info_r17.size() > 0, 1));
      if (meas_result_rx_tx_time_diff_r17.is_present()) {
        HANDLE_CODE(meas_result_rx_tx_time_diff_r17->pack(bref));
      }
      if (sl_meas_result_serving_relay_r17.size() > 0) {
        HANDLE_CODE(sl_meas_result_serving_relay_r17.pack(bref));
      }
      if (ul_pdcp_excess_delay_result_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ul_pdcp_excess_delay_result_list_r17, 1, 29));
      }
      if (coarse_location_info_r17.size() > 0) {
        HANDLE_CODE(coarse_location_info_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_present, 1));

  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_serving_mo_list, bref, 1, 32));
  if (meas_result_neigh_cells_present) {
    HANDLE_CODE(meas_result_neigh_cells.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_result_serv_freq_list_eutra_scg, bref);
      unpack_presence_flag(meas_result_serv_freq_list_nr_scg, bref);
      unpack_presence_flag(meas_result_sftd_eutra, bref);
      unpack_presence_flag(meas_result_sftd_nr, bref);
      if (meas_result_serv_freq_list_eutra_scg.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_eutra_scg, bref, 1, 32));
      }
      if (meas_result_serv_freq_list_nr_scg.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_nr_scg, bref, 1, 32));
      }
      if (meas_result_sftd_eutra.is_present()) {
        HANDLE_CODE(meas_result_sftd_eutra->unpack(bref));
      }
      if (meas_result_sftd_nr.is_present()) {
        HANDLE_CODE(meas_result_sftd_nr->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_result_cell_list_sftd_nr, bref);
      if (meas_result_cell_list_sftd_nr.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_cell_list_sftd_nr, bref, 1, 3));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_result_for_rssi_r16, bref);
      unpack_presence_flag(location_info_r16, bref);
      unpack_presence_flag(ul_pdcp_delay_value_result_list_r16, bref);
      unpack_presence_flag(meas_results_sl_r16, bref);
      unpack_presence_flag(meas_result_cli_r16, bref);
      if (meas_result_for_rssi_r16.is_present()) {
        HANDLE_CODE(meas_result_for_rssi_r16->unpack(bref));
      }
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->unpack(bref));
      }
      if (ul_pdcp_delay_value_result_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_pdcp_delay_value_result_list_r16, bref, 1, 29));
      }
      if (meas_results_sl_r16.is_present()) {
        HANDLE_CODE(meas_results_sl_r16->unpack(bref));
      }
      if (meas_result_cli_r16.is_present()) {
        HANDLE_CODE(meas_result_cli_r16->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_result_rx_tx_time_diff_r17, bref);
      bool sl_meas_result_serving_relay_r17_present;
      HANDLE_CODE(bref.unpack(sl_meas_result_serving_relay_r17_present, 1));
      unpack_presence_flag(ul_pdcp_excess_delay_result_list_r17, bref);
      bool coarse_location_info_r17_present;
      HANDLE_CODE(bref.unpack(coarse_location_info_r17_present, 1));
      if (meas_result_rx_tx_time_diff_r17.is_present()) {
        HANDLE_CODE(meas_result_rx_tx_time_diff_r17->unpack(bref));
      }
      if (sl_meas_result_serving_relay_r17_present) {
        HANDLE_CODE(sl_meas_result_serving_relay_r17.unpack(bref));
      }
      if (ul_pdcp_excess_delay_result_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_pdcp_excess_delay_result_list_r17, bref, 1, 29));
      }
      if (coarse_location_info_r17_present) {
        HANDLE_CODE(coarse_location_info_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_results_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  j.start_array("measResultServingMOList");
  for (const auto& e1 : meas_result_serving_mo_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (meas_result_neigh_cells_present) {
    j.write_fieldname("measResultNeighCells");
    meas_result_neigh_cells.to_json(j);
  }
  if (ext) {
    if (meas_result_serv_freq_list_eutra_scg.is_present()) {
      j.start_array("measResultServFreqListEUTRA-SCG");
      for (const auto& e1 : *meas_result_serv_freq_list_eutra_scg) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_serv_freq_list_nr_scg.is_present()) {
      j.start_array("measResultServFreqListNR-SCG");
      for (const auto& e1 : *meas_result_serv_freq_list_nr_scg) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_sftd_eutra.is_present()) {
      j.write_fieldname("measResultSFTD-EUTRA");
      meas_result_sftd_eutra->to_json(j);
    }
    if (meas_result_sftd_nr.is_present()) {
      j.write_fieldname("measResultSFTD-NR");
      meas_result_sftd_nr->to_json(j);
    }
    if (meas_result_cell_list_sftd_nr.is_present()) {
      j.start_array("measResultCellListSFTD-NR");
      for (const auto& e1 : *meas_result_cell_list_sftd_nr) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_for_rssi_r16.is_present()) {
      j.write_fieldname("measResultForRSSI-r16");
      meas_result_for_rssi_r16->to_json(j);
    }
    if (location_info_r16.is_present()) {
      j.write_fieldname("locationInfo-r16");
      location_info_r16->to_json(j);
    }
    if (ul_pdcp_delay_value_result_list_r16.is_present()) {
      j.start_array("ul-PDCP-DelayValueResultList-r16");
      for (const auto& e1 : *ul_pdcp_delay_value_result_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_results_sl_r16.is_present()) {
      j.write_fieldname("measResultsSL-r16");
      meas_results_sl_r16->to_json(j);
    }
    if (meas_result_cli_r16.is_present()) {
      j.write_fieldname("measResultCLI-r16");
      meas_result_cli_r16->to_json(j);
    }
    if (meas_result_rx_tx_time_diff_r17.is_present()) {
      j.write_fieldname("measResultRxTxTimeDiff-r17");
      meas_result_rx_tx_time_diff_r17->to_json(j);
    }
    if (sl_meas_result_serving_relay_r17.size() > 0) {
      j.write_str("sl-MeasResultServingRelay-r17", sl_meas_result_serving_relay_r17.to_string());
    }
    if (ul_pdcp_excess_delay_result_list_r17.is_present()) {
      j.start_array("ul-PDCP-ExcessDelayResultList-r17");
      for (const auto& e1 : *ul_pdcp_excess_delay_result_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (coarse_location_info_r17.size() > 0) {
      j.write_str("coarseLocationInfo-r17", coarse_location_info_r17.to_string());
    }
  }
  j.end_obj();
}

void meas_results_s::meas_result_neigh_cells_c_::destroy_()
{
  switch (type_) {
    case types::meas_result_list_nr:
      c.destroy<meas_result_list_nr_l>();
      break;
    case types::meas_result_list_eutra:
      c.destroy<meas_result_list_eutra_l>();
      break;
    case types::meas_result_list_utra_fdd_r16:
      c.destroy<meas_result_list_utra_fdd_r16_l>();
      break;
    case types::sl_meas_results_cand_relay_r17:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void meas_results_s::meas_result_neigh_cells_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_result_list_nr:
      c.init<meas_result_list_nr_l>();
      break;
    case types::meas_result_list_eutra:
      c.init<meas_result_list_eutra_l>();
      break;
    case types::meas_result_list_utra_fdd_r16:
      c.init<meas_result_list_utra_fdd_r16_l>();
      break;
    case types::sl_meas_results_cand_relay_r17:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
}
meas_results_s::meas_result_neigh_cells_c_::meas_result_neigh_cells_c_(
    const meas_results_s::meas_result_neigh_cells_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_result_list_nr:
      c.init(other.c.get<meas_result_list_nr_l>());
      break;
    case types::meas_result_list_eutra:
      c.init(other.c.get<meas_result_list_eutra_l>());
      break;
    case types::meas_result_list_utra_fdd_r16:
      c.init(other.c.get<meas_result_list_utra_fdd_r16_l>());
      break;
    case types::sl_meas_results_cand_relay_r17:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
}
meas_results_s::meas_result_neigh_cells_c_&
meas_results_s::meas_result_neigh_cells_c_::operator=(const meas_results_s::meas_result_neigh_cells_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_result_list_nr:
      c.set(other.c.get<meas_result_list_nr_l>());
      break;
    case types::meas_result_list_eutra:
      c.set(other.c.get<meas_result_list_eutra_l>());
      break;
    case types::meas_result_list_utra_fdd_r16:
      c.set(other.c.get<meas_result_list_utra_fdd_r16_l>());
      break;
    case types::sl_meas_results_cand_relay_r17:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }

  return *this;
}
meas_result_list_nr_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_nr()
{
  set(types::meas_result_list_nr);
  return c.get<meas_result_list_nr_l>();
}
meas_result_list_eutra_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_eutra()
{
  set(types::meas_result_list_eutra);
  return c.get<meas_result_list_eutra_l>();
}
meas_result_list_utra_fdd_r16_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_utra_fdd_r16()
{
  set(types::meas_result_list_utra_fdd_r16);
  return c.get<meas_result_list_utra_fdd_r16_l>();
}
dyn_octstring& meas_results_s::meas_result_neigh_cells_c_::set_sl_meas_results_cand_relay_r17()
{
  set(types::sl_meas_results_cand_relay_r17);
  return c.get<dyn_octstring>();
}
void meas_results_s::meas_result_neigh_cells_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_result_list_nr:
      j.start_array("measResultListNR");
      for (const auto& e1 : c.get<meas_result_list_nr_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::meas_result_list_eutra:
      j.start_array("measResultListEUTRA");
      for (const auto& e1 : c.get<meas_result_list_eutra_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::meas_result_list_utra_fdd_r16:
      j.start_array("measResultListUTRA-FDD-r16");
      for (const auto& e1 : c.get<meas_result_list_utra_fdd_r16_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_meas_results_cand_relay_r17:
      j.write_str("sl-MeasResultsCandRelay-r17", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_results_s::meas_result_neigh_cells_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_result_list_nr:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_nr_l>(), 1, 8));
      break;
    case types::meas_result_list_eutra: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_eutra_l>(), 1, 8));
    } break;
    case types::meas_result_list_utra_fdd_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_utra_fdd_r16_l>(), 1, 8));
    } break;
    case types::sl_meas_results_cand_relay_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_s::meas_result_neigh_cells_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_result_list_nr:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_nr_l>(), bref, 1, 8));
      break;
    case types::meas_result_list_eutra: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_eutra_l>(), bref, 1, 8));
    } break;
    case types::meas_result_list_utra_fdd_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_utra_fdd_r16_l>(), bref, 1, 8));
    } break;
    case types::sl_meas_results_cand_relay_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_results_s::meas_result_neigh_cells_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "measResultListNR", "measResultListEUTRA", "measResultListUTRA-FDD-r16", "sl-MeasResultsCandRelay-r17"};
  return convert_enum_idx(names, 4, value, "meas_results_s::meas_result_neigh_cells_c_::types");
}

// PosSIB-ReqInfo-r16 ::= SEQUENCE
SRSASN_CODE pos_sib_req_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gnss_id_r16_present, 1));
  HANDLE_CODE(bref.pack(sbas_id_r16_present, 1));

  if (gnss_id_r16_present) {
    HANDLE_CODE(gnss_id_r16.pack(bref));
  }
  if (sbas_id_r16_present) {
    HANDLE_CODE(sbas_id_r16.pack(bref));
  }
  HANDLE_CODE(pos_sib_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sib_req_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gnss_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(sbas_id_r16_present, 1));

  if (gnss_id_r16_present) {
    HANDLE_CODE(gnss_id_r16.unpack(bref));
  }
  if (sbas_id_r16_present) {
    HANDLE_CODE(sbas_id_r16.unpack(bref));
  }
  HANDLE_CODE(pos_sib_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void pos_sib_req_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnss_id_r16_present) {
    j.write_fieldname("gnss-id-r16");
    gnss_id_r16.to_json(j);
  }
  if (sbas_id_r16_present) {
    j.write_fieldname("sbas-id-r16");
    sbas_id_r16.to_json(j);
  }
  j.write_str("posSibType-r16", pos_sib_type_r16.to_string());
  j.end_obj();
}

const char* pos_sib_req_info_r16_s::pos_sib_type_r16_opts::to_string() const
{
  static const char* names[] = {
      "posSibType1-1",        "posSibType1-2",       "posSibType1-3",        "posSibType1-4",
      "posSibType1-5",        "posSibType1-6",       "posSibType1-7",        "posSibType1-8",
      "posSibType2-1",        "posSibType2-2",       "posSibType2-3",        "posSibType2-4",
      "posSibType2-5",        "posSibType2-6",       "posSibType2-7",        "posSibType2-8",
      "posSibType2-9",        "posSibType2-10",      "posSibType2-11",       "posSibType2-12",
      "posSibType2-13",       "posSibType2-14",      "posSibType2-15",       "posSibType2-16",
      "posSibType2-17",       "posSibType2-18",      "posSibType2-19",       "posSibType2-20",
      "posSibType2-21",       "posSibType2-22",      "posSibType2-23",       "posSibType3-1",
      "posSibType4-1",        "posSibType5-1",       "posSibType6-1",        "posSibType6-2",
      "posSibType6-3",        "posSibType1-9-v1710", "posSibType1-10-v1710", "posSibType2-24-v1710",
      "posSibType2-25-v1710", "posSibType6-4-v1710", "posSibType6-5-v1710",  "posSibType6-6-v1710"};
  return convert_enum_idx(names, 44, value, "pos_sib_req_info_r16_s::pos_sib_type_r16_e_");
}

// RLF-Report-r16 ::= CHOICE
void rlf_report_r16_c::destroy_()
{
  switch (type_) {
    case types::nr_rlf_report_r16:
      c.destroy<nr_rlf_report_r16_s_>();
      break;
    case types::eutra_rlf_report_r16:
      c.destroy<eutra_rlf_report_r16_s_>();
      break;
    default:
      break;
  }
}
void rlf_report_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_rlf_report_r16:
      c.init<nr_rlf_report_r16_s_>();
      break;
    case types::eutra_rlf_report_r16:
      c.init<eutra_rlf_report_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c");
  }
}
rlf_report_r16_c::rlf_report_r16_c(const rlf_report_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_rlf_report_r16:
      c.init(other.c.get<nr_rlf_report_r16_s_>());
      break;
    case types::eutra_rlf_report_r16:
      c.init(other.c.get<eutra_rlf_report_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c");
  }
}
rlf_report_r16_c& rlf_report_r16_c::operator=(const rlf_report_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_rlf_report_r16:
      c.set(other.c.get<nr_rlf_report_r16_s_>());
      break;
    case types::eutra_rlf_report_r16:
      c.set(other.c.get<eutra_rlf_report_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c");
  }

  return *this;
}
rlf_report_r16_c::nr_rlf_report_r16_s_& rlf_report_r16_c::set_nr_rlf_report_r16()
{
  set(types::nr_rlf_report_r16);
  return c.get<nr_rlf_report_r16_s_>();
}
rlf_report_r16_c::eutra_rlf_report_r16_s_& rlf_report_r16_c::set_eutra_rlf_report_r16()
{
  set(types::eutra_rlf_report_r16);
  return c.get<eutra_rlf_report_r16_s_>();
}
void rlf_report_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_rlf_report_r16:
      j.write_fieldname("nr-RLF-Report-r16");
      c.get<nr_rlf_report_r16_s_>().to_json(j);
      break;
    case types::eutra_rlf_report_r16:
      j.write_fieldname("eutra-RLF-Report-r16");
      c.get<eutra_rlf_report_r16_s_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_rlf_report_r16:
      HANDLE_CODE(c.get<nr_rlf_report_r16_s_>().pack(bref));
      break;
    case types::eutra_rlf_report_r16:
      HANDLE_CODE(c.get<eutra_rlf_report_r16_s_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_rlf_report_r16:
      HANDLE_CODE(c.get<nr_rlf_report_r16_s_>().unpack(bref));
      break;
    case types::eutra_rlf_report_r16:
      HANDLE_CODE(c.get<eutra_rlf_report_r16_s_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_r16_present, 1));
  HANDLE_CODE(bref.pack(prev_pcell_id_r16_present, 1));
  HANDLE_CODE(bref.pack(reconnect_cell_id_r16_present, 1));
  HANDLE_CODE(bref.pack(time_until_reconn_r16_present, 1));
  HANDLE_CODE(bref.pack(reest_cell_id_r16_present, 1));
  HANDLE_CODE(bref.pack(time_conn_fail_r16_present, 1));
  HANDLE_CODE(bref.pack(location_info_r16_present, 1));
  HANDLE_CODE(bref.pack(no_suitable_cell_found_r16_present, 1));
  HANDLE_CODE(bref.pack(ra_info_common_r16_present, 1));

  HANDLE_CODE(meas_result_last_serv_cell_r16.pack(bref));
  if (meas_result_neigh_cells_r16_present) {
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r16.meas_result_list_nr_r16.size() > 0, 1));
    HANDLE_CODE(bref.pack(meas_result_neigh_cells_r16.meas_result_list_eutra_r16.size() > 0, 1));
    if (meas_result_neigh_cells_r16.meas_result_list_nr_r16.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r16.meas_result_list_nr_r16, 1, 8));
    }
    if (meas_result_neigh_cells_r16.meas_result_list_eutra_r16.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_r16.meas_result_list_eutra_r16, 1, 8));
    }
  }
  HANDLE_CODE(pack_integer(bref, c_rnti_r16, (uint32_t)0u, (uint32_t)65535u));
  if (prev_pcell_id_r16_present) {
    HANDLE_CODE(prev_pcell_id_r16.pack(bref));
  }
  HANDLE_CODE(failed_pcell_id_r16.pack(bref));
  if (reconnect_cell_id_r16_present) {
    HANDLE_CODE(reconnect_cell_id_r16.pack(bref));
  }
  if (time_until_reconn_r16_present) {
    HANDLE_CODE(pack_integer(bref, time_until_reconn_r16, (uint32_t)0u, (uint32_t)172800u));
  }
  if (reest_cell_id_r16_present) {
    HANDLE_CODE(reest_cell_id_r16.pack(bref));
  }
  if (time_conn_fail_r16_present) {
    HANDLE_CODE(pack_integer(bref, time_conn_fail_r16, (uint16_t)0u, (uint16_t)1023u));
  }
  HANDLE_CODE(pack_integer(bref, time_since_fail_r16, (uint32_t)0u, (uint32_t)172800u));
  HANDLE_CODE(conn_fail_type_r16.pack(bref));
  HANDLE_CODE(rlf_cause_r16.pack(bref));
  if (location_info_r16_present) {
    HANDLE_CODE(location_info_r16.pack(bref));
  }
  if (ra_info_common_r16_present) {
    HANDLE_CODE(ra_info_common_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_rs_rlm_cfg_bitmap_v1650_present;
    group_flags[1] |= last_ho_type_r17_present;
    group_flags[1] |= time_conn_source_daps_fail_r17_present;
    group_flags[1] |= time_since_cho_recfg_r17_present;
    group_flags[1] |= cho_cell_id_r17.is_present();
    group_flags[1] |= cho_candidate_cell_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_rlm_cfg_bitmap_v1650_present, 1));
      if (csi_rs_rlm_cfg_bitmap_v1650_present) {
        HANDLE_CODE(csi_rs_rlm_cfg_bitmap_v1650.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(last_ho_type_r17_present, 1));
      HANDLE_CODE(bref.pack(time_conn_source_daps_fail_r17_present, 1));
      HANDLE_CODE(bref.pack(time_since_cho_recfg_r17_present, 1));
      HANDLE_CODE(bref.pack(cho_cell_id_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(cho_candidate_cell_list_r17.is_present(), 1));
      if (last_ho_type_r17_present) {
        HANDLE_CODE(last_ho_type_r17.pack(bref));
      }
      if (time_conn_source_daps_fail_r17_present) {
        HANDLE_CODE(pack_integer(bref, time_conn_source_daps_fail_r17, (uint16_t)0u, (uint16_t)1023u));
      }
      if (time_since_cho_recfg_r17_present) {
        HANDLE_CODE(pack_integer(bref, time_since_cho_recfg_r17, (uint16_t)0u, (uint16_t)1023u));
      }
      if (cho_cell_id_r17.is_present()) {
        HANDLE_CODE(cho_cell_id_r17->pack(bref));
      }
      if (cho_candidate_cell_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cho_candidate_cell_list_r17, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_r16_present, 1));
  HANDLE_CODE(bref.unpack(prev_pcell_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(reconnect_cell_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(time_until_reconn_r16_present, 1));
  HANDLE_CODE(bref.unpack(reest_cell_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(time_conn_fail_r16_present, 1));
  HANDLE_CODE(bref.unpack(location_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(no_suitable_cell_found_r16_present, 1));
  HANDLE_CODE(bref.unpack(ra_info_common_r16_present, 1));

  HANDLE_CODE(meas_result_last_serv_cell_r16.unpack(bref));
  if (meas_result_neigh_cells_r16_present) {
    bool meas_result_list_nr_r16_present;
    HANDLE_CODE(bref.unpack(meas_result_list_nr_r16_present, 1));
    bool meas_result_list_eutra_r16_present;
    HANDLE_CODE(bref.unpack(meas_result_list_eutra_r16_present, 1));
    if (meas_result_list_nr_r16_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r16.meas_result_list_nr_r16, bref, 1, 8));
    }
    if (meas_result_list_eutra_r16_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_r16.meas_result_list_eutra_r16, bref, 1, 8));
    }
  }
  HANDLE_CODE(unpack_integer(c_rnti_r16, bref, (uint32_t)0u, (uint32_t)65535u));
  if (prev_pcell_id_r16_present) {
    HANDLE_CODE(prev_pcell_id_r16.unpack(bref));
  }
  HANDLE_CODE(failed_pcell_id_r16.unpack(bref));
  if (reconnect_cell_id_r16_present) {
    HANDLE_CODE(reconnect_cell_id_r16.unpack(bref));
  }
  if (time_until_reconn_r16_present) {
    HANDLE_CODE(unpack_integer(time_until_reconn_r16, bref, (uint32_t)0u, (uint32_t)172800u));
  }
  if (reest_cell_id_r16_present) {
    HANDLE_CODE(reest_cell_id_r16.unpack(bref));
  }
  if (time_conn_fail_r16_present) {
    HANDLE_CODE(unpack_integer(time_conn_fail_r16, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  HANDLE_CODE(unpack_integer(time_since_fail_r16, bref, (uint32_t)0u, (uint32_t)172800u));
  HANDLE_CODE(conn_fail_type_r16.unpack(bref));
  HANDLE_CODE(rlf_cause_r16.unpack(bref));
  if (location_info_r16_present) {
    HANDLE_CODE(location_info_r16.unpack(bref));
  }
  if (ra_info_common_r16_present) {
    HANDLE_CODE(ra_info_common_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(csi_rs_rlm_cfg_bitmap_v1650_present, 1));
      if (csi_rs_rlm_cfg_bitmap_v1650_present) {
        HANDLE_CODE(csi_rs_rlm_cfg_bitmap_v1650.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(last_ho_type_r17_present, 1));
      HANDLE_CODE(bref.unpack(time_conn_source_daps_fail_r17_present, 1));
      HANDLE_CODE(bref.unpack(time_since_cho_recfg_r17_present, 1));
      unpack_presence_flag(cho_cell_id_r17, bref);
      unpack_presence_flag(cho_candidate_cell_list_r17, bref);
      if (last_ho_type_r17_present) {
        HANDLE_CODE(last_ho_type_r17.unpack(bref));
      }
      if (time_conn_source_daps_fail_r17_present) {
        HANDLE_CODE(unpack_integer(time_conn_source_daps_fail_r17, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (time_since_cho_recfg_r17_present) {
        HANDLE_CODE(unpack_integer(time_since_cho_recfg_r17, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (cho_cell_id_r17.is_present()) {
        HANDLE_CODE(cho_cell_id_r17->unpack(bref));
      }
      if (cho_candidate_cell_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cho_candidate_cell_list_r17, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResultLastServCell-r16");
  meas_result_last_serv_cell_r16.to_json(j);
  if (meas_result_neigh_cells_r16_present) {
    j.write_fieldname("measResultNeighCells-r16");
    j.start_obj();
    if (meas_result_neigh_cells_r16.meas_result_list_nr_r16.size() > 0) {
      j.start_array("measResultListNR-r16");
      for (const auto& e1 : meas_result_neigh_cells_r16.meas_result_list_nr_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_neigh_cells_r16.meas_result_list_eutra_r16.size() > 0) {
      j.start_array("measResultListEUTRA-r16");
      for (const auto& e1 : meas_result_neigh_cells_r16.meas_result_list_eutra_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  j.write_int("c-RNTI-r16", c_rnti_r16);
  if (prev_pcell_id_r16_present) {
    j.write_fieldname("previousPCellId-r16");
    prev_pcell_id_r16.to_json(j);
  }
  j.write_fieldname("failedPCellId-r16");
  failed_pcell_id_r16.to_json(j);
  if (reconnect_cell_id_r16_present) {
    j.write_fieldname("reconnectCellId-r16");
    reconnect_cell_id_r16.to_json(j);
  }
  if (time_until_reconn_r16_present) {
    j.write_int("timeUntilReconnection-r16", time_until_reconn_r16);
  }
  if (reest_cell_id_r16_present) {
    j.write_fieldname("reestablishmentCellId-r16");
    reest_cell_id_r16.to_json(j);
  }
  if (time_conn_fail_r16_present) {
    j.write_int("timeConnFailure-r16", time_conn_fail_r16);
  }
  j.write_int("timeSinceFailure-r16", time_since_fail_r16);
  j.write_str("connectionFailureType-r16", conn_fail_type_r16.to_string());
  j.write_str("rlf-Cause-r16", rlf_cause_r16.to_string());
  if (location_info_r16_present) {
    j.write_fieldname("locationInfo-r16");
    location_info_r16.to_json(j);
  }
  if (no_suitable_cell_found_r16_present) {
    j.write_str("noSuitableCellFound-r16", "true");
  }
  if (ra_info_common_r16_present) {
    j.write_fieldname("ra-InformationCommon-r16");
    ra_info_common_r16.to_json(j);
  }
  if (ext) {
    if (csi_rs_rlm_cfg_bitmap_v1650_present) {
      j.write_str("csi-rsRLMConfigBitmap-v1650", csi_rs_rlm_cfg_bitmap_v1650.to_string());
    }
    if (last_ho_type_r17_present) {
      j.write_str("lastHO-Type-r17", last_ho_type_r17.to_string());
    }
    if (time_conn_source_daps_fail_r17_present) {
      j.write_int("timeConnSourceDAPS-Failure-r17", time_conn_source_daps_fail_r17);
    }
    if (time_since_cho_recfg_r17_present) {
      j.write_int("timeSinceCHO-Reconfig-r17", time_since_cho_recfg_r17);
    }
    if (cho_cell_id_r17.is_present()) {
      j.write_fieldname("choCellId-r17");
      cho_cell_id_r17->to_json(j);
    }
    if (cho_candidate_cell_list_r17.is_present()) {
      j.start_array("choCandidateCellList-r17");
      for (const auto& e1 : *cho_candidate_cell_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

void rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::nr_prev_cell_r16:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::eutra_prev_cell_r16:
      c.destroy<cgi_info_eutra_logging_s>();
      break;
    default:
      break;
  }
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_prev_cell_r16:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::eutra_prev_cell_r16:
      c.init<cgi_info_eutra_logging_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::prev_pcell_id_r16_c_(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_prev_cell_r16:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::eutra_prev_cell_r16:
      c.init(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::operator=(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_prev_cell_r16:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::eutra_prev_cell_r16:
      c.set(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_");
  }

  return *this;
}
cgi_info_logging_r16_s& rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::set_nr_prev_cell_r16()
{
  set(types::nr_prev_cell_r16);
  return c.get<cgi_info_logging_r16_s>();
}
cgi_info_eutra_logging_s& rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::set_eutra_prev_cell_r16()
{
  set(types::eutra_prev_cell_r16);
  return c.get<cgi_info_eutra_logging_s>();
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_prev_cell_r16:
      j.write_fieldname("nrPreviousCell-r16");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::eutra_prev_cell_r16:
      j.write_fieldname("eutraPreviousCell-r16");
      c.get<cgi_info_eutra_logging_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_prev_cell_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::eutra_prev_cell_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_prev_cell_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::eutra_prev_cell_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nrPreviousCell-r16", "eutraPreviousCell-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::types");
}

void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::nr_failed_pcell_id_r16:
      c.destroy<nr_failed_pcell_id_r16_c_>();
      break;
    case types::eutra_failed_pcell_id_r16:
      c.destroy<eutra_failed_pcell_id_r16_c_>();
      break;
    default:
      break;
  }
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_failed_pcell_id_r16:
      c.init<nr_failed_pcell_id_r16_c_>();
      break;
    case types::eutra_failed_pcell_id_r16:
      c.init<eutra_failed_pcell_id_r16_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::failed_pcell_id_r16_c_(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_failed_pcell_id_r16:
      c.init(other.c.get<nr_failed_pcell_id_r16_c_>());
      break;
    case types::eutra_failed_pcell_id_r16:
      c.init(other.c.get<eutra_failed_pcell_id_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::operator=(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_failed_pcell_id_r16:
      c.set(other.c.get<nr_failed_pcell_id_r16_c_>());
      break;
    case types::eutra_failed_pcell_id_r16:
      c.set(other.c.get<eutra_failed_pcell_id_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_");
  }

  return *this;
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::set_nr_failed_pcell_id_r16()
{
  set(types::nr_failed_pcell_id_r16);
  return c.get<nr_failed_pcell_id_r16_c_>();
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::set_eutra_failed_pcell_id_r16()
{
  set(types::eutra_failed_pcell_id_r16);
  return c.get<eutra_failed_pcell_id_r16_c_>();
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_failed_pcell_id_r16:
      j.write_fieldname("nrFailedPCellId-r16");
      c.get<nr_failed_pcell_id_r16_c_>().to_json(j);
      break;
    case types::eutra_failed_pcell_id_r16:
      j.write_fieldname("eutraFailedPCellId-r16");
      c.get<eutra_failed_pcell_id_r16_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_failed_pcell_id_r16:
      HANDLE_CODE(c.get<nr_failed_pcell_id_r16_c_>().pack(bref));
      break;
    case types::eutra_failed_pcell_id_r16:
      HANDLE_CODE(c.get<eutra_failed_pcell_id_r16_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_failed_pcell_id_r16:
      HANDLE_CODE(c.get<nr_failed_pcell_id_r16_c_>().unpack(bref));
      break;
    case types::eutra_failed_pcell_id_r16:
      HANDLE_CODE(c.get<eutra_failed_pcell_id_r16_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r16:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r16:
      c.destroy<pci_arfcn_nr_r16_s>();
      break;
    default:
      break;
  }
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r16:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r16:
      c.init<pci_arfcn_nr_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r16:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r16:
      c.init(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::operator=(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r16:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r16:
      c.set(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_");
  }

  return *this;
}
cgi_info_logging_r16_s&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::set_cell_global_id_r16()
{
  set(types::cell_global_id_r16);
  return c.get<cgi_info_logging_r16_s>();
}
pci_arfcn_nr_r16_s&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::set_pci_arfcn_r16()
{
  set(types::pci_arfcn_r16);
  return c.get<pci_arfcn_nr_r16_s>();
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r16:
      j.write_fieldname("cellGlobalId-r16");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::pci_arfcn_r16:
      j.write_fieldname("pci-arfcn-r16");
      c.get<pci_arfcn_nr_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::types");
}

void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r16:
      c.destroy<cgi_info_eutra_logging_s>();
      break;
    case types::pci_arfcn_r16:
      c.destroy<pci_arfcn_eutra_r16_s>();
      break;
    default:
      break;
  }
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r16:
      c.init<cgi_info_eutra_logging_s>();
      break;
    case types::pci_arfcn_r16:
      c.init<pci_arfcn_eutra_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::
    eutra_failed_pcell_id_r16_c_(
        const rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r16:
      c.init(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::pci_arfcn_r16:
      c.init(other.c.get<pci_arfcn_eutra_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::operator=(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r16:
      c.set(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::pci_arfcn_r16:
      c.set(other.c.get<pci_arfcn_eutra_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_");
  }

  return *this;
}
cgi_info_eutra_logging_s&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::set_cell_global_id_r16()
{
  set(types::cell_global_id_r16);
  return c.get<cgi_info_eutra_logging_s>();
}
pci_arfcn_eutra_r16_s&
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::set_pci_arfcn_r16()
{
  set(types::pci_arfcn_r16);
  return c.get<pci_arfcn_eutra_r16_s>();
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r16:
      j.write_fieldname("cellGlobalId-r16");
      c.get<cgi_info_eutra_logging_s>().to_json(j);
      break;
    case types::pci_arfcn_r16:
      j.write_fieldname("pci-arfcn-r16");
      c.get<pci_arfcn_eutra_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().pack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_eutra_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().unpack(bref));
      break;
    case types::pci_arfcn_r16:
      HANDLE_CODE(c.get<pci_arfcn_eutra_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::types_opts::to_string()
    const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::types");
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nrFailedPCellId-r16", "eutraFailedPCellId-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::types");
}

void rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::nr_reconnect_cell_id_r16:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::eutra_reconnect_cell_id_r16:
      c.destroy<cgi_info_eutra_logging_s>();
      break;
    default:
      break;
  }
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_reconnect_cell_id_r16:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::eutra_reconnect_cell_id_r16:
      c.init<cgi_info_eutra_logging_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::reconnect_cell_id_r16_c_(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_reconnect_cell_id_r16:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::eutra_reconnect_cell_id_r16:
      c.init(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::operator=(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_reconnect_cell_id_r16:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::eutra_reconnect_cell_id_r16:
      c.set(other.c.get<cgi_info_eutra_logging_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_");
  }

  return *this;
}
cgi_info_logging_r16_s& rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::set_nr_reconnect_cell_id_r16()
{
  set(types::nr_reconnect_cell_id_r16);
  return c.get<cgi_info_logging_r16_s>();
}
cgi_info_eutra_logging_s&
rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::set_eutra_reconnect_cell_id_r16()
{
  set(types::eutra_reconnect_cell_id_r16);
  return c.get<cgi_info_eutra_logging_s>();
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_reconnect_cell_id_r16:
      j.write_fieldname("nrReconnectCellId-r16");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::eutra_reconnect_cell_id_r16:
      j.write_fieldname("eutraReconnectCellId-r16");
      c.get<cgi_info_eutra_logging_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_reconnect_cell_id_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::eutra_reconnect_cell_id_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_reconnect_cell_id_r16:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::eutra_reconnect_cell_id_r16:
      HANDLE_CODE(c.get<cgi_info_eutra_logging_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nrReconnectCellId-r16", "eutraReconnectCellId-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::types");
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::conn_fail_type_r16_opts::to_string() const
{
  static const char* names[] = {"rlf", "hof"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::conn_fail_type_r16_e_");
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::rlf_cause_r16_opts::to_string() const
{
  static const char* names[] = {"t310-Expiry",
                                "randomAccessProblem",
                                "rlc-MaxNumRetx",
                                "beamFailureRecoveryFailure",
                                "lbtFailure-r16",
                                "bh-rlfRecoveryFailure",
                                "t312-expiry-r17",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::rlf_cause_r16_e_");
}
uint16_t rlf_report_r16_c::nr_rlf_report_r16_s_::rlf_cause_r16_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case t312_expiry_r17:
      return 312;
    default:
      invalid_enum_number(value, "rlf_report_r16_c::nr_rlf_report_r16_s_::rlf_cause_r16_e_");
  }
  return 0;
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::last_ho_type_r17_opts::to_string() const
{
  static const char* names[] = {"cho", "daps", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::last_ho_type_r17_e_");
}

void rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::destroy_()
{
  switch (type_) {
    case types::cell_global_id_r17:
      c.destroy<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r17:
      c.destroy<pci_arfcn_nr_r16_s>();
      break;
    default:
      break;
  }
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_r17:
      c.init<cgi_info_logging_r16_s>();
      break;
    case types::pci_arfcn_r17:
      c.init<pci_arfcn_nr_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::cho_cell_id_r17_c_(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_r17:
      c.init(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r17:
      c.init(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_");
  }
}
rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_&
rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::operator=(
    const rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_r17:
      c.set(other.c.get<cgi_info_logging_r16_s>());
      break;
    case types::pci_arfcn_r17:
      c.set(other.c.get<pci_arfcn_nr_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_");
  }

  return *this;
}
cgi_info_logging_r16_s& rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::set_cell_global_id_r17()
{
  set(types::cell_global_id_r17);
  return c.get<cgi_info_logging_r16_s>();
}
pci_arfcn_nr_r16_s& rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::set_pci_arfcn_r17()
{
  set(types::pci_arfcn_r17);
  return c.get<pci_arfcn_nr_r16_s>();
}
void rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_r17:
      j.write_fieldname("cellGlobalId-r17");
      c.get<cgi_info_logging_r16_s>().to_json(j);
      break;
    case types::pci_arfcn_r17:
      j.write_fieldname("pci-arfcn-r17");
      c.get<pci_arfcn_nr_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_r17:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().pack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_r17:
      HANDLE_CODE(c.get<cgi_info_logging_r16_s>().unpack(bref));
      break;
    case types::pci_arfcn_r17:
      HANDLE_CODE(c.get<pci_arfcn_nr_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r17", "pci-arfcn-r17"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::types");
}

SRSASN_CODE rlf_report_r16_c::eutra_rlf_report_r16_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(failed_pcell_id_eutra.pack(bref));
  HANDLE_CODE(meas_result_rlf_report_eutra_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_rlf_report_eutra_v1690.size() > 0;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_rlf_report_eutra_v1690.size() > 0, 1));
      if (meas_result_rlf_report_eutra_v1690.size() > 0) {
        HANDLE_CODE(meas_result_rlf_report_eutra_v1690.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_report_r16_c::eutra_rlf_report_r16_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(failed_pcell_id_eutra.unpack(bref));
  HANDLE_CODE(meas_result_rlf_report_eutra_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_rlf_report_eutra_v1690_present;
      HANDLE_CODE(bref.unpack(meas_result_rlf_report_eutra_v1690_present, 1));
      if (meas_result_rlf_report_eutra_v1690_present) {
        HANDLE_CODE(meas_result_rlf_report_eutra_v1690.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rlf_report_r16_c::eutra_rlf_report_r16_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("failedPCellId-EUTRA");
  failed_pcell_id_eutra.to_json(j);
  j.write_str("measResult-RLF-Report-EUTRA-r16", meas_result_rlf_report_eutra_r16.to_string());
  if (ext) {
    if (meas_result_rlf_report_eutra_v1690.size() > 0) {
      j.write_str("measResult-RLF-Report-EUTRA-v1690", meas_result_rlf_report_eutra_v1690.to_string());
    }
  }
  j.end_obj();
}

const char* rlf_report_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"nr-RLF-Report-r16", "eutra-RLF-Report-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::types");
}

// RRCReconfigurationComplete-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_tx_direct_current_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_list, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1530_ies_s::unpack(cbit_ref& bref)
{
  bool ul_tx_direct_current_list_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_list_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_list, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_tx_direct_current_list.size() > 0) {
    j.start_array("uplinkTxDirectCurrentList");
    for (const auto& e1 : ul_tx_direct_current_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCReestablishmentComplete-v1610-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_complete_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_complete_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_reest_complete_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_meass_available_r16_present) {
    j.write_fieldname("ue-MeasurementsAvailable-r16");
    ue_meass_available_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCResumeComplete-v1610-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idle_meas_available_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_idle_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_idle_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(scg_resp_r16_present, 1));
  HANDLE_CODE(bref.pack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.pack(mob_history_avail_r16_present, 1));
  HANDLE_CODE(bref.pack(mob_state_r16_present, 1));
  HANDLE_CODE(bref.pack(need_for_gaps_info_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_idle_eutra_r16_present) {
    HANDLE_CODE(meas_result_idle_eutra_r16.pack(bref));
  }
  if (meas_result_idle_nr_r16_present) {
    HANDLE_CODE(meas_result_idle_nr_r16.pack(bref));
  }
  if (scg_resp_r16_present) {
    HANDLE_CODE(scg_resp_r16.pack(bref));
  }
  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.pack(bref));
  }
  if (mob_state_r16_present) {
    HANDLE_CODE(mob_state_r16.pack(bref));
  }
  if (need_for_gaps_info_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_info_nr_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idle_meas_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_idle_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_idle_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(scg_resp_r16_present, 1));
  HANDLE_CODE(bref.unpack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(mob_history_avail_r16_present, 1));
  HANDLE_CODE(bref.unpack(mob_state_r16_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gaps_info_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_idle_eutra_r16_present) {
    HANDLE_CODE(meas_result_idle_eutra_r16.unpack(bref));
  }
  if (meas_result_idle_nr_r16_present) {
    HANDLE_CODE(meas_result_idle_nr_r16.unpack(bref));
  }
  if (scg_resp_r16_present) {
    HANDLE_CODE(scg_resp_r16.unpack(bref));
  }
  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.unpack(bref));
  }
  if (mob_state_r16_present) {
    HANDLE_CODE(mob_state_r16.unpack(bref));
  }
  if (need_for_gaps_info_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_info_nr_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idle_meas_available_r16_present) {
    j.write_str("idleMeasAvailable-r16", "true");
  }
  if (meas_result_idle_eutra_r16_present) {
    j.write_fieldname("measResultIdleEUTRA-r16");
    meas_result_idle_eutra_r16.to_json(j);
  }
  if (meas_result_idle_nr_r16_present) {
    j.write_fieldname("measResultIdleNR-r16");
    meas_result_idle_nr_r16.to_json(j);
  }
  if (scg_resp_r16_present) {
    j.write_fieldname("scg-Response-r16");
    scg_resp_r16.to_json(j);
  }
  if (ue_meass_available_r16_present) {
    j.write_fieldname("ue-MeasurementsAvailable-r16");
    ue_meass_available_r16.to_json(j);
  }
  if (mob_history_avail_r16_present) {
    j.write_str("mobilityHistoryAvail-r16", "true");
  }
  if (mob_state_r16_present) {
    j.write_str("mobilityState-r16", mob_state_r16.to_string());
  }
  if (need_for_gaps_info_nr_r16_present) {
    j.write_fieldname("needForGapsInfoNR-r16");
    need_for_gaps_info_nr_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::destroy_()
{
  switch (type_) {
    case types::nr_scg_resp:
      c.destroy<dyn_octstring>();
      break;
    case types::eutra_scg_resp:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_scg_resp:
      c.init<dyn_octstring>();
      break;
    case types::eutra_scg_resp:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_");
  }
}
rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::scg_resp_r16_c_(
    const rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_scg_resp:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_resp:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_");
  }
}
rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_& rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::operator=(
    const rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_scg_resp:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_resp:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_");
  }

  return *this;
}
dyn_octstring& rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::set_nr_scg_resp()
{
  set(types::nr_scg_resp);
  return c.get<dyn_octstring>();
}
dyn_octstring& rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::set_eutra_scg_resp()
{
  set(types::eutra_scg_resp);
  return c.get<dyn_octstring>();
}
void rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_scg_resp:
      j.write_str("nr-SCG-Response", c.get<dyn_octstring>().to_string());
      break;
    case types::eutra_scg_resp:
      j.write_str("eutra-SCG-Response", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::eutra_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::eutra_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG-Response", "eutra-SCG-Response"};
  return convert_enum_idx(names, 2, value, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::types");
}

const char* rrc_resume_complete_v1610_ies_s::mob_state_r16_opts::to_string() const
{
  static const char* names[] = {"normal", "medium", "high", "spare"};
  return convert_enum_idx(names, 4, value, "rrc_resume_complete_v1610_ies_s::mob_state_r16_e_");
}

// RRCSetupComplete-v1610-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_complete_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(iab_node_ind_r16_present, 1));
  HANDLE_CODE(bref.pack(idle_meas_available_r16_present, 1));
  HANDLE_CODE(bref.pack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.pack(mob_history_avail_r16_present, 1));
  HANDLE_CODE(bref.pack(mob_state_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.pack(bref));
  }
  if (mob_state_r16_present) {
    HANDLE_CODE(mob_state_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(iab_node_ind_r16_present, 1));
  HANDLE_CODE(bref.unpack(idle_meas_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(ue_meass_available_r16_present, 1));
  HANDLE_CODE(bref.unpack(mob_history_avail_r16_present, 1));
  HANDLE_CODE(bref.unpack(mob_state_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_meass_available_r16_present) {
    HANDLE_CODE(ue_meass_available_r16.unpack(bref));
  }
  if (mob_state_r16_present) {
    HANDLE_CODE(mob_state_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_setup_complete_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (iab_node_ind_r16_present) {
    j.write_str("iab-NodeIndication-r16", "true");
  }
  if (idle_meas_available_r16_present) {
    j.write_str("idleMeasAvailable-r16", "true");
  }
  if (ue_meass_available_r16_present) {
    j.write_fieldname("ue-MeasurementsAvailable-r16");
    ue_meass_available_r16.to_json(j);
  }
  if (mob_history_avail_r16_present) {
    j.write_str("mobilityHistoryAvail-r16", "true");
  }
  if (mob_state_r16_present) {
    j.write_str("mobilityState-r16", mob_state_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* rrc_setup_complete_v1610_ies_s::mob_state_r16_opts::to_string() const
{
  static const char* names[] = {"normal", "medium", "high", "spare"};
  return convert_enum_idx(names, 4, value, "rrc_setup_complete_v1610_ies_s::mob_state_r16_e_");
}

// RegisteredAMF ::= SEQUENCE
SRSASN_CODE registered_amf_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  HANDLE_CODE(amf_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE registered_amf_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  HANDLE_CODE(amf_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void registered_amf_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_present) {
    j.write_fieldname("plmn-Identity");
    plmn_id.to_json(j);
  }
  j.write_str("amf-Identifier", amf_id.to_string());
  j.end_obj();
}

// S-NSSAI ::= CHOICE
void s_nssai_c::destroy_()
{
  switch (type_) {
    case types::sst:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::sst_sd:
      c.destroy<fixed_bitstring<32>>();
      break;
    default:
      break;
  }
}
void s_nssai_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sst:
      c.init<fixed_bitstring<8>>();
      break;
    case types::sst_sd:
      c.init<fixed_bitstring<32>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }
}
s_nssai_c::s_nssai_c(const s_nssai_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sst:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::sst_sd:
      c.init(other.c.get<fixed_bitstring<32>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }
}
s_nssai_c& s_nssai_c::operator=(const s_nssai_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sst:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::sst_sd:
      c.set(other.c.get<fixed_bitstring<32>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }

  return *this;
}
fixed_bitstring<8>& s_nssai_c::set_sst()
{
  set(types::sst);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<32>& s_nssai_c::set_sst_sd()
{
  set(types::sst_sd);
  return c.get<fixed_bitstring<32>>();
}
void s_nssai_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sst:
      j.write_str("sst", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::sst_sd:
      j.write_str("sst-SD", c.get<fixed_bitstring<32>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }
  j.end_obj();
}
SRSASN_CODE s_nssai_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sst:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::sst_sd:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE s_nssai_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sst:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::sst_sd:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* s_nssai_c::types_opts::to_string() const
{
  static const char* names[] = {"sst", "sst-SD"};
  return convert_enum_idx(names, 2, value, "s_nssai_c::types");
}

// SCGFailureInformation-v1590-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SCGFailureInformationEUTRA-v1590-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_eutra_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_eutra_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SIB-ReqInfo-r16 ::= ENUMERATED
const char* sib_req_info_r16_opts::to_string() const
{
  static const char* names[] = {"sib12", "sib13", "sib14", "sib20-v1700", "sib21-v1700", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sib_req_info_r16_e");
}
uint8_t sib_req_info_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 13, 14, 20, 21};
  return map_enum_number(numbers, 5, value, "sib_req_info_r16_e");
}

// SidelinkUEInformationNR-v1700-IEs ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_nr_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_tx_res_req_list_v1700.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rx_drx_report_list_v1700.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rx_interested_gc_bc_dest_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rx_interested_freq_list_disc_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_tx_res_req_list_disc_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_tx_res_req_list_comm_relay_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(ue_type_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_source_id_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sl_tx_res_req_list_v1700.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_res_req_list_v1700, 1, 32));
  }
  if (sl_rx_drx_report_list_v1700.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rx_drx_report_list_v1700, 1, 32));
  }
  if (sl_rx_interested_gc_bc_dest_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rx_interested_gc_bc_dest_list_r17, 1, 32));
  }
  if (sl_rx_interested_freq_list_disc_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rx_interested_freq_list_disc_r17, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_tx_res_req_list_disc_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_res_req_list_disc_r17, 1, 32));
  }
  if (sl_tx_res_req_list_comm_relay_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_res_req_list_comm_relay_r17, 1, 32));
  }
  if (ue_type_r17_present) {
    HANDLE_CODE(ue_type_r17.pack(bref));
  }
  if (sl_source_id_remote_ue_r17_present) {
    HANDLE_CODE(sl_source_id_remote_ue_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_nr_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool sl_tx_res_req_list_v1700_present;
  HANDLE_CODE(bref.unpack(sl_tx_res_req_list_v1700_present, 1));
  bool sl_rx_drx_report_list_v1700_present;
  HANDLE_CODE(bref.unpack(sl_rx_drx_report_list_v1700_present, 1));
  bool sl_rx_interested_gc_bc_dest_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_rx_interested_gc_bc_dest_list_r17_present, 1));
  bool sl_rx_interested_freq_list_disc_r17_present;
  HANDLE_CODE(bref.unpack(sl_rx_interested_freq_list_disc_r17_present, 1));
  bool sl_tx_res_req_list_disc_r17_present;
  HANDLE_CODE(bref.unpack(sl_tx_res_req_list_disc_r17_present, 1));
  bool sl_tx_res_req_list_comm_relay_r17_present;
  HANDLE_CODE(bref.unpack(sl_tx_res_req_list_comm_relay_r17_present, 1));
  HANDLE_CODE(bref.unpack(ue_type_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_source_id_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sl_tx_res_req_list_v1700_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_tx_res_req_list_v1700, bref, 1, 32));
  }
  if (sl_rx_drx_report_list_v1700_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rx_drx_report_list_v1700, bref, 1, 32));
  }
  if (sl_rx_interested_gc_bc_dest_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rx_interested_gc_bc_dest_list_r17, bref, 1, 32));
  }
  if (sl_rx_interested_freq_list_disc_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rx_interested_freq_list_disc_r17, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_tx_res_req_list_disc_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_tx_res_req_list_disc_r17, bref, 1, 32));
  }
  if (sl_tx_res_req_list_comm_relay_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_tx_res_req_list_comm_relay_r17, bref, 1, 32));
  }
  if (ue_type_r17_present) {
    HANDLE_CODE(ue_type_r17.unpack(bref));
  }
  if (sl_source_id_remote_ue_r17_present) {
    HANDLE_CODE(sl_source_id_remote_ue_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_nr_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_tx_res_req_list_v1700.size() > 0) {
    j.start_array("sl-TxResourceReqList-v1700");
    for (const auto& e1 : sl_tx_res_req_list_v1700) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_rx_drx_report_list_v1700.size() > 0) {
    j.start_array("sl-RxDRX-ReportList-v1700");
    for (const auto& e1 : sl_rx_drx_report_list_v1700) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_rx_interested_gc_bc_dest_list_r17.size() > 0) {
    j.start_array("sl-RxInterestedGC-BC-DestList-r17");
    for (const auto& e1 : sl_rx_interested_gc_bc_dest_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_rx_interested_freq_list_disc_r17.size() > 0) {
    j.start_array("sl-RxInterestedFreqListDisc-r17");
    for (const auto& e1 : sl_rx_interested_freq_list_disc_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_tx_res_req_list_disc_r17.size() > 0) {
    j.start_array("sl-TxResourceReqListDisc-r17");
    for (const auto& e1 : sl_tx_res_req_list_disc_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_tx_res_req_list_comm_relay_r17.size() > 0) {
    j.start_array("sl-TxResourceReqListCommRelay-r17");
    for (const auto& e1 : sl_tx_res_req_list_comm_relay_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ue_type_r17_present) {
    j.write_str("ue-Type-r17", ue_type_r17.to_string());
  }
  if (sl_source_id_remote_ue_r17_present) {
    j.write_str("sl-SourceIdentityRemoteUE-r17", sl_source_id_remote_ue_r17.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* sidelink_ue_info_nr_v1700_ies_s::ue_type_r17_opts::to_string() const
{
  static const char* names[] = {"relayUE", "remoteUE"};
  return convert_enum_idx(names, 2, value, "sidelink_ue_info_nr_v1700_ies_s::ue_type_r17_e_");
}

// UEAssistanceInformation-v1540-IEs ::= SEQUENCE
SRSASN_CODE ue_assist_info_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(overheat_assist_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (overheat_assist_present) {
    HANDLE_CODE(overheat_assist.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_assist_info_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(overheat_assist_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (overheat_assist_present) {
    HANDLE_CODE(overheat_assist.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_assist_info_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (overheat_assist_present) {
    j.write_fieldname("overheatingAssistance");
    overheat_assist.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UEInformationResponse-v1700-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(success_ho_report_r17_present, 1));
  HANDLE_CODE(bref.pack(conn_est_fail_report_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(coarse_location_info_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (success_ho_report_r17_present) {
    HANDLE_CODE(success_ho_report_r17.pack(bref));
  }
  if (conn_est_fail_report_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, conn_est_fail_report_list_r17, 1, 4));
  }
  if (coarse_location_info_r17.size() > 0) {
    HANDLE_CODE(coarse_location_info_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(success_ho_report_r17_present, 1));
  bool conn_est_fail_report_list_r17_present;
  HANDLE_CODE(bref.unpack(conn_est_fail_report_list_r17_present, 1));
  bool coarse_location_info_r17_present;
  HANDLE_CODE(bref.unpack(coarse_location_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (success_ho_report_r17_present) {
    HANDLE_CODE(success_ho_report_r17.unpack(bref));
  }
  if (conn_est_fail_report_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(conn_est_fail_report_list_r17, bref, 1, 4));
  }
  if (coarse_location_info_r17_present) {
    HANDLE_CODE(coarse_location_info_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (success_ho_report_r17_present) {
    j.write_fieldname("successHO-Report-r17");
    success_ho_report_r17.to_json(j);
  }
  if (conn_est_fail_report_list_r17.size() > 0) {
    j.start_array("connEstFailReportList-r17");
    for (const auto& e1 : conn_est_fail_report_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (coarse_location_info_r17.size() > 0) {
    j.write_str("coarseLocationInfo-r17", coarse_location_info_r17.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UEPositioningAssistanceInfo-v1720-IEs ::= SEQUENCE
SRSASN_CODE ue_positioning_assist_info_v1720_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_tx_teg_timing_error_margin_value_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_tx_teg_timing_error_margin_value_r17_present) {
    HANDLE_CODE(ue_tx_teg_timing_error_margin_value_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_positioning_assist_info_v1720_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_tx_teg_timing_error_margin_value_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_tx_teg_timing_error_margin_value_r17_present) {
    HANDLE_CODE(ue_tx_teg_timing_error_margin_value_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_positioning_assist_info_v1720_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_tx_teg_timing_error_margin_value_r17_present) {
    j.write_str("ue-TxTEG-TimingErrorMarginValue-r17", ue_tx_teg_timing_error_margin_value_r17.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* ue_positioning_assist_info_v1720_ies_s::ue_tx_teg_timing_error_margin_value_r17_opts::to_string() const
{
  static const char* names[] = {"tc0",
                                "tc2",
                                "tc4",
                                "tc6",
                                "tc8",
                                "tc12",
                                "tc16",
                                "tc20",
                                "tc24",
                                "tc32",
                                "tc40",
                                "tc48",
                                "tc56",
                                "tc64",
                                "tc72",
                                "tc80"};
  return convert_enum_idx(
      names, 16, value, "ue_positioning_assist_info_v1720_ies_s::ue_tx_teg_timing_error_margin_value_r17_e_");
}
uint8_t ue_positioning_assist_info_v1720_ies_s::ue_tx_teg_timing_error_margin_value_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6, 8, 12, 16, 20, 24, 32, 40, 48, 56, 64, 72, 80};
  return map_enum_number(
      numbers, 16, value, "ue_positioning_assist_info_v1720_ies_s::ue_tx_teg_timing_error_margin_value_r17_e_");
}

// ULInformationTransfer-v1700-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_info_f1c_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_info_f1c_r17.size() > 0) {
    HANDLE_CODE(ded_info_f1c_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool ded_info_f1c_r17_present;
  HANDLE_CODE(bref.unpack(ded_info_f1c_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_info_f1c_r17_present) {
    HANDLE_CODE(ded_info_f1c_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_info_f1c_r17.size() > 0) {
    j.write_str("dedicatedInfoF1c-r17", ded_info_f1c_r17.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CounterCheckResponse-IEs ::= SEQUENCE
SRSASN_CODE counter_check_resp_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, drb_count_info_list, 0, 29));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(drb_count_info_list, bref, 0, 29));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void counter_check_resp_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("drb-CountInfoList");
  for (const auto& e1 : drb_count_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// DedicatedSIBRequest-r16-IEs ::= SEQUENCE
SRSASN_CODE ded_sib_request_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(on_demand_sib_request_list_r16_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (on_demand_sib_request_list_r16_present) {
    HANDLE_CODE(bref.pack(on_demand_sib_request_list_r16.requested_sib_list_r16.size() > 0, 1));
    HANDLE_CODE(bref.pack(on_demand_sib_request_list_r16.requested_pos_sib_list_r16.size() > 0, 1));
    if (on_demand_sib_request_list_r16.requested_sib_list_r16.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, on_demand_sib_request_list_r16.requested_sib_list_r16, 1, 8));
    }
    if (on_demand_sib_request_list_r16.requested_pos_sib_list_r16.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, on_demand_sib_request_list_r16.requested_pos_sib_list_r16, 1, 32));
    }
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ded_sib_request_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(on_demand_sib_request_list_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (on_demand_sib_request_list_r16_present) {
    bool requested_sib_list_r16_present;
    HANDLE_CODE(bref.unpack(requested_sib_list_r16_present, 1));
    bool requested_pos_sib_list_r16_present;
    HANDLE_CODE(bref.unpack(requested_pos_sib_list_r16_present, 1));
    if (requested_sib_list_r16_present) {
      HANDLE_CODE(unpack_dyn_seq_of(on_demand_sib_request_list_r16.requested_sib_list_r16, bref, 1, 8));
    }
    if (requested_pos_sib_list_r16_present) {
      HANDLE_CODE(unpack_dyn_seq_of(on_demand_sib_request_list_r16.requested_pos_sib_list_r16, bref, 1, 32));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ded_sib_request_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (on_demand_sib_request_list_r16_present) {
    j.write_fieldname("onDemandSIB-RequestList-r16");
    j.start_obj();
    if (on_demand_sib_request_list_r16.requested_sib_list_r16.size() > 0) {
      j.start_array("requestedSIB-List-r16");
      for (const auto& e1 : on_demand_sib_request_list_r16.requested_sib_list_r16) {
        j.write_str(e1.to_string());
      }
      j.end_array();
    }
    if (on_demand_sib_request_list_r16.requested_pos_sib_list_r16.size() > 0) {
      j.start_array("requestedPosSIB-List-r16");
      for (const auto& e1 : on_demand_sib_request_list_r16.requested_pos_sib_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// FailureInformation-IEs ::= SEQUENCE
SRSASN_CODE fail_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_info_rlc_bearer_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_info_rlc_bearer_present) {
    HANDLE_CODE(fail_info_rlc_bearer.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_info_rlc_bearer_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_info_rlc_bearer_present) {
    HANDLE_CODE(fail_info_rlc_bearer.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void fail_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_info_rlc_bearer_present) {
    j.write_fieldname("failureInfoRLC-Bearer");
    fail_info_rlc_bearer.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// IABOtherInformation-r16-IEs ::= SEQUENCE
SRSASN_CODE iab_other_info_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ip_info_type_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_other_info_r16_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ip_info_type_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void iab_other_info_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ip-InfoType-r16");
  ip_info_type_r16.to_json(j);
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

void iab_other_info_r16_ies_s::ip_info_type_r16_c_::destroy_()
{
  switch (type_) {
    case types::iab_ip_request_r16:
      c.destroy<iab_ip_request_r16_s_>();
      break;
    case types::iab_ip_report_r16:
      c.destroy<iab_ip_report_r16_s_>();
      break;
    default:
      break;
  }
}
void iab_other_info_r16_ies_s::ip_info_type_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::iab_ip_request_r16:
      c.init<iab_ip_request_r16_s_>();
      break;
    case types::iab_ip_report_r16:
      c.init<iab_ip_report_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_");
  }
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_::ip_info_type_r16_c_(
    const iab_other_info_r16_ies_s::ip_info_type_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::iab_ip_request_r16:
      c.init(other.c.get<iab_ip_request_r16_s_>());
      break;
    case types::iab_ip_report_r16:
      c.init(other.c.get<iab_ip_report_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_");
  }
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_&
iab_other_info_r16_ies_s::ip_info_type_r16_c_::operator=(const iab_other_info_r16_ies_s::ip_info_type_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::iab_ip_request_r16:
      c.set(other.c.get<iab_ip_request_r16_s_>());
      break;
    case types::iab_ip_report_r16:
      c.set(other.c.get<iab_ip_report_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_");
  }

  return *this;
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_&
iab_other_info_r16_ies_s::ip_info_type_r16_c_::set_iab_ip_request_r16()
{
  set(types::iab_ip_request_r16);
  return c.get<iab_ip_request_r16_s_>();
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_&
iab_other_info_r16_ies_s::ip_info_type_r16_c_::set_iab_ip_report_r16()
{
  set(types::iab_ip_report_r16);
  return c.get<iab_ip_report_r16_s_>();
}
void iab_other_info_r16_ies_s::ip_info_type_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::iab_ip_request_r16:
      j.write_fieldname("iab-IP-Request-r16");
      j.start_obj();
      if (c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16_present) {
        j.write_fieldname("iab-IPv4-AddressNumReq-r16");
        c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16.to_json(j);
      }
      if (c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16_present) {
        j.write_fieldname("iab-IPv6-AddressReq-r16");
        c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16.to_json(j);
      }
      j.end_obj();
      break;
    case types::iab_ip_report_r16:
      j.write_fieldname("iab-IP-Report-r16");
      j.start_obj();
      if (c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16_present) {
        j.write_fieldname("iab-IPv4-AddressReport-r16");
        c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16.to_json(j);
      }
      if (c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16_present) {
        j.write_fieldname("iab-IPv6-Report-r16");
        c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE iab_other_info_r16_ies_s::ip_info_type_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::iab_ip_request_r16:
      HANDLE_CODE(bref.pack(c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16_present, 1));
      if (c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16_present) {
        HANDLE_CODE(c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16.pack(bref));
      }
      if (c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16_present) {
        HANDLE_CODE(c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16.pack(bref));
      }
      break;
    case types::iab_ip_report_r16:
      HANDLE_CODE(bref.pack(c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16_present, 1));
      if (c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16_present) {
        HANDLE_CODE(c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16.pack(bref));
      }
      if (c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16_present) {
        HANDLE_CODE(c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_other_info_r16_ies_s::ip_info_type_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::iab_ip_request_r16:
      HANDLE_CODE(bref.unpack(c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16_present, 1));
      if (c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16_present) {
        HANDLE_CODE(c.get<iab_ip_request_r16_s_>().iab_ip_v4_address_num_req_r16.unpack(bref));
      }
      if (c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16_present) {
        HANDLE_CODE(c.get<iab_ip_request_r16_s_>().iab_ip_v6_address_req_r16.unpack(bref));
      }
      break;
    case types::iab_ip_report_r16:
      HANDLE_CODE(bref.unpack(c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16_present, 1));
      if (c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16_present) {
        HANDLE_CODE(c.get<iab_ip_report_r16_s_>().iab_ip_v4_address_report_r16.unpack(bref));
      }
      if (c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16_present) {
        HANDLE_CODE(c.get<iab_ip_report_r16_s_>().iab_ip_v6_report_r16.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::destroy_()
{
  switch (type_) {
    case types::iab_ip_v6_address_num_req_r16:
      c.destroy<iab_ip_address_num_req_r16_s>();
      break;
    case types::iab_ip_v6_address_prefix_req_r16:
      c.destroy<iab_ip_address_prefix_req_r16_s>();
      break;
    default:
      break;
  }
}
void iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::set(
    types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::iab_ip_v6_address_num_req_r16:
      c.init<iab_ip_address_num_req_r16_s>();
      break;
    case types::iab_ip_v6_address_prefix_req_r16:
      c.init<iab_ip_address_prefix_req_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_");
  }
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::
    iab_ip_v6_address_req_r16_c_(
        const iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::iab_ip_v6_address_num_req_r16:
      c.init(other.c.get<iab_ip_address_num_req_r16_s>());
      break;
    case types::iab_ip_v6_address_prefix_req_r16:
      c.init(other.c.get<iab_ip_address_prefix_req_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_");
  }
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_&
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::operator=(
    const iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::iab_ip_v6_address_num_req_r16:
      c.set(other.c.get<iab_ip_address_num_req_r16_s>());
      break;
    case types::iab_ip_v6_address_prefix_req_r16:
      c.set(other.c.get<iab_ip_address_prefix_req_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_");
  }

  return *this;
}
iab_ip_address_num_req_r16_s& iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::
    iab_ip_v6_address_req_r16_c_::set_iab_ip_v6_address_num_req_r16()
{
  set(types::iab_ip_v6_address_num_req_r16);
  return c.get<iab_ip_address_num_req_r16_s>();
}
iab_ip_address_prefix_req_r16_s& iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::
    iab_ip_v6_address_req_r16_c_::set_iab_ip_v6_address_prefix_req_r16()
{
  set(types::iab_ip_v6_address_prefix_req_r16);
  return c.get<iab_ip_address_prefix_req_r16_s>();
}
void iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::iab_ip_v6_address_num_req_r16:
      j.write_fieldname("iab-IPv6-AddressNumReq-r16");
      c.get<iab_ip_address_num_req_r16_s>().to_json(j);
      break;
    case types::iab_ip_v6_address_prefix_req_r16:
      j.write_fieldname("iab-IPv6-AddressPrefixReq-r16");
      c.get<iab_ip_address_prefix_req_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::pack(
    bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::iab_ip_v6_address_num_req_r16:
      HANDLE_CODE(c.get<iab_ip_address_num_req_r16_s>().pack(bref));
      break;
    case types::iab_ip_v6_address_prefix_req_r16:
      HANDLE_CODE(c.get<iab_ip_address_prefix_req_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::unpack(
    cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::iab_ip_v6_address_num_req_r16:
      HANDLE_CODE(c.get<iab_ip_address_num_req_r16_s>().unpack(bref));
      break;
    case types::iab_ip_v6_address_prefix_req_r16:
      HANDLE_CODE(c.get<iab_ip_address_prefix_req_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::
    types_opts::to_string() const
{
  static const char* names[] = {"iab-IPv6-AddressNumReq-r16", "iab-IPv6-AddressPrefixReq-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::types");
}

void iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::destroy_()
{
  switch (type_) {
    case types::iab_ip_v6_address_report_r16:
      c.destroy<iab_ip_address_and_traffic_r16_s>();
      break;
    case types::iab_ip_v6_prefix_report_r16:
      c.destroy<iab_ip_prefix_and_traffic_r16_s>();
      break;
    default:
      break;
  }
}
void iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::iab_ip_v6_address_report_r16:
      c.init<iab_ip_address_and_traffic_r16_s>();
      break;
    case types::iab_ip_v6_prefix_report_r16:
      c.init<iab_ip_prefix_and_traffic_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_");
  }
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::iab_ip_v6_report_r16_c_(
    const iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::iab_ip_v6_address_report_r16:
      c.init(other.c.get<iab_ip_address_and_traffic_r16_s>());
      break;
    case types::iab_ip_v6_prefix_report_r16:
      c.init(other.c.get<iab_ip_prefix_and_traffic_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_");
  }
}
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_&
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::operator=(
    const iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::iab_ip_v6_address_report_r16:
      c.set(other.c.get<iab_ip_address_and_traffic_r16_s>());
      break;
    case types::iab_ip_v6_prefix_report_r16:
      c.set(other.c.get<iab_ip_prefix_and_traffic_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_");
  }

  return *this;
}
iab_ip_address_and_traffic_r16_s& iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::
    iab_ip_v6_report_r16_c_::set_iab_ip_v6_address_report_r16()
{
  set(types::iab_ip_v6_address_report_r16);
  return c.get<iab_ip_address_and_traffic_r16_s>();
}
iab_ip_prefix_and_traffic_r16_s& iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::
    iab_ip_v6_report_r16_c_::set_iab_ip_v6_prefix_report_r16()
{
  set(types::iab_ip_v6_prefix_report_r16);
  return c.get<iab_ip_prefix_and_traffic_r16_s>();
}
void iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::iab_ip_v6_address_report_r16:
      j.write_fieldname("iab-IPv6-AddressReport-r16");
      c.get<iab_ip_address_and_traffic_r16_s>().to_json(j);
      break;
    case types::iab_ip_v6_prefix_report_r16:
      j.write_fieldname("iab-IPv6-PrefixReport-r16");
      c.get<iab_ip_prefix_and_traffic_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::iab_ip_v6_address_report_r16:
      HANDLE_CODE(c.get<iab_ip_address_and_traffic_r16_s>().pack(bref));
      break;
    case types::iab_ip_v6_prefix_report_r16:
      HANDLE_CODE(c.get<iab_ip_prefix_and_traffic_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::iab_ip_v6_address_report_r16:
      HANDLE_CODE(c.get<iab_ip_address_and_traffic_r16_s>().unpack(bref));
      break;
    case types::iab_ip_v6_prefix_report_r16:
      HANDLE_CODE(c.get<iab_ip_prefix_and_traffic_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::types_opts::to_string()
    const
{
  static const char* names[] = {"iab-IPv6-AddressReport-r16", "iab-IPv6-PrefixReport-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::types");
}

const char* iab_other_info_r16_ies_s::ip_info_type_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"iab-IP-Request-r16", "iab-IP-Report-r16"};
  return convert_enum_idx(names, 2, value, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::types");
}

// LocationMeasurementIndication-IEs ::= SEQUENCE
SRSASN_CODE location_meas_ind_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_ind.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_ind_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_ind.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void location_meas_ind_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measurementIndication");
  meas_ind.to_json(j);
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MBSInterestIndication-r17-IEs ::= SEQUENCE
SRSASN_CODE mb_si_nterest_ind_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbs_freq_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(mbs_prio_r17_present, 1));
  HANDLE_CODE(bref.pack(mbs_service_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbs_freq_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_freq_list_r17, 1, 16, integer_packer<uint32_t>(0, 3279165)));
  }
  if (mbs_service_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_service_list_r17, 1, 16));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mb_si_nterest_ind_r17_ies_s::unpack(cbit_ref& bref)
{
  bool mbs_freq_list_r17_present;
  HANDLE_CODE(bref.unpack(mbs_freq_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(mbs_prio_r17_present, 1));
  bool mbs_service_list_r17_present;
  HANDLE_CODE(bref.unpack(mbs_service_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbs_freq_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_freq_list_r17, bref, 1, 16, integer_packer<uint32_t>(0, 3279165)));
  }
  if (mbs_service_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_service_list_r17, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mb_si_nterest_ind_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbs_freq_list_r17.size() > 0) {
    j.start_array("mbs-FreqList-r17");
    for (const auto& e1 : mbs_freq_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mbs_prio_r17_present) {
    j.write_str("mbs-Priority-r17", "true");
  }
  if (mbs_service_list_r17.size() > 0) {
    j.start_array("mbs-ServiceList-r17");
    for (const auto& e1 : mbs_service_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MCGFailureInformation-r16-IEs ::= SEQUENCE
SRSASN_CODE mcg_fail_info_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_mcg_r16_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_mcg_r16_present) {
    HANDLE_CODE(fail_report_mcg_r16.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcg_fail_info_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_mcg_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_mcg_r16_present) {
    HANDLE_CODE(fail_report_mcg_r16.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mcg_fail_info_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_mcg_r16_present) {
    j.write_fieldname("failureReportMCG-r16");
    fail_report_mcg_r16.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MeasurementReport-IEs ::= SEQUENCE
SRSASN_CODE meas_report_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_results.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_results.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResults");
  meas_results.to_json(j);
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MeasurementReportAppLayer-r17-IEs ::= SEQUENCE
SRSASN_CODE meas_report_app_layer_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_report_app_layer_list_r17, 1, 16));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_app_layer_r17_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_report_app_layer_list_r17, bref, 1, 16));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_app_layer_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measurementReportAppLayerList-r17");
  for (const auto& e1 : meas_report_app_layer_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RRCReconfigurationComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCReestablishmentComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_complete_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_reest_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCResumeComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_nas_msg.size() > 0, 1));
  HANDLE_CODE(bref.pack(sel_plmn_id_present, 1));
  HANDLE_CODE(bref.pack(ul_tx_direct_current_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_nas_msg.size() > 0) {
    HANDLE_CODE(ded_nas_msg.pack(bref));
  }
  if (sel_plmn_id_present) {
    HANDLE_CODE(pack_integer(bref, sel_plmn_id, (uint8_t)1u, (uint8_t)12u));
  }
  if (ul_tx_direct_current_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_list, 1, 32));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_ies_s::unpack(cbit_ref& bref)
{
  bool ded_nas_msg_present;
  HANDLE_CODE(bref.unpack(ded_nas_msg_present, 1));
  HANDLE_CODE(bref.unpack(sel_plmn_id_present, 1));
  bool ul_tx_direct_current_list_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_nas_msg_present) {
    HANDLE_CODE(ded_nas_msg.unpack(bref));
  }
  if (sel_plmn_id_present) {
    HANDLE_CODE(unpack_integer(sel_plmn_id, bref, (uint8_t)1u, (uint8_t)12u));
  }
  if (ul_tx_direct_current_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_list, bref, 1, 32));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_nas_msg.size() > 0) {
    j.write_str("dedicatedNAS-Message", ded_nas_msg.to_string());
  }
  if (sel_plmn_id_present) {
    j.write_int("selectedPLMN-Identity", sel_plmn_id);
  }
  if (ul_tx_direct_current_list.size() > 0) {
    j.start_array("uplinkTxDirectCurrentList");
    for (const auto& e1 : ul_tx_direct_current_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCSetupComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(registered_amf_present, 1));
  HANDLE_CODE(bref.pack(guami_type_present, 1));
  HANDLE_CODE(bref.pack(s_nssai_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ng_5_g_s_tmsi_value_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, sel_plmn_id, (uint8_t)1u, (uint8_t)12u));
  if (registered_amf_present) {
    HANDLE_CODE(registered_amf.pack(bref));
  }
  if (guami_type_present) {
    HANDLE_CODE(guami_type.pack(bref));
  }
  if (s_nssai_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, s_nssai_list, 1, 8));
  }
  HANDLE_CODE(ded_nas_msg.pack(bref));
  if (ng_5_g_s_tmsi_value_present) {
    HANDLE_CODE(ng_5_g_s_tmsi_value.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(registered_amf_present, 1));
  HANDLE_CODE(bref.unpack(guami_type_present, 1));
  bool s_nssai_list_present;
  HANDLE_CODE(bref.unpack(s_nssai_list_present, 1));
  HANDLE_CODE(bref.unpack(ng_5_g_s_tmsi_value_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(sel_plmn_id, bref, (uint8_t)1u, (uint8_t)12u));
  if (registered_amf_present) {
    HANDLE_CODE(registered_amf.unpack(bref));
  }
  if (guami_type_present) {
    HANDLE_CODE(guami_type.unpack(bref));
  }
  if (s_nssai_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(s_nssai_list, bref, 1, 8));
  }
  HANDLE_CODE(ded_nas_msg.unpack(bref));
  if (ng_5_g_s_tmsi_value_present) {
    HANDLE_CODE(ng_5_g_s_tmsi_value.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_setup_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("selectedPLMN-Identity", sel_plmn_id);
  if (registered_amf_present) {
    j.write_fieldname("registeredAMF");
    registered_amf.to_json(j);
  }
  if (guami_type_present) {
    j.write_str("guami-Type", guami_type.to_string());
  }
  if (s_nssai_list.size() > 0) {
    j.start_array("s-NSSAI-List");
    for (const auto& e1 : s_nssai_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("dedicatedNAS-Message", ded_nas_msg.to_string());
  if (ng_5_g_s_tmsi_value_present) {
    j.write_fieldname("ng-5G-S-TMSI-Value");
    ng_5_g_s_tmsi_value.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* rrc_setup_complete_ies_s::guami_type_opts::to_string() const
{
  static const char* names[] = {"native", "mapped"};
  return convert_enum_idx(names, 2, value, "rrc_setup_complete_ies_s::guami_type_e_");
}

void rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::destroy_()
{
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.destroy<fixed_bitstring<48>>();
      break;
    case types::ng_5_g_s_tmsi_part2:
      c.destroy<fixed_bitstring<9>>();
      break;
    default:
      break;
  }
}
void rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.init<fixed_bitstring<48>>();
      break;
    case types::ng_5_g_s_tmsi_part2:
      c.init<fixed_bitstring<9>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_");
  }
}
rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::ng_5_g_s_tmsi_value_c_(
    const rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.init(other.c.get<fixed_bitstring<48>>());
      break;
    case types::ng_5_g_s_tmsi_part2:
      c.init(other.c.get<fixed_bitstring<9>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_");
  }
}
rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_& rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::operator=(
    const rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.set(other.c.get<fixed_bitstring<48>>());
      break;
    case types::ng_5_g_s_tmsi_part2:
      c.set(other.c.get<fixed_bitstring<9>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_");
  }

  return *this;
}
fixed_bitstring<48>& rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::set_ng_5_g_s_tmsi()
{
  set(types::ng_5_g_s_tmsi);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<9>& rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::set_ng_5_g_s_tmsi_part2()
{
  set(types::ng_5_g_s_tmsi_part2);
  return c.get<fixed_bitstring<9>>();
}
void rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      j.write_str("ng-5G-S-TMSI", c.get<fixed_bitstring<48>>().to_string());
      break;
    case types::ng_5_g_s_tmsi_part2:
      j.write_str("ng-5G-S-TMSI-Part2", c.get<fixed_bitstring<9>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().pack(bref));
      break;
    case types::ng_5_g_s_tmsi_part2:
      HANDLE_CODE(c.get<fixed_bitstring<9>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().unpack(bref));
      break;
    case types::ng_5_g_s_tmsi_part2:
      HANDLE_CODE(c.get<fixed_bitstring<9>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::types_opts::to_string() const
{
  static const char* names[] = {"ng-5G-S-TMSI", "ng-5G-S-TMSI-Part2"};
  return convert_enum_idx(names, 2, value, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::types");
}

// SCGFailureInformation-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_scg_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_scg_present) {
    HANDLE_CODE(fail_report_scg.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_scg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_scg_present) {
    HANDLE_CODE(fail_report_scg.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_scg_present) {
    j.write_fieldname("failureReportSCG");
    fail_report_scg.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCGFailureInformationEUTRA-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_eutra_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_scg_eutra_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_scg_eutra_present) {
    HANDLE_CODE(fail_report_scg_eutra.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_scg_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_scg_eutra_present) {
    HANDLE_CODE(fail_report_scg_eutra.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_eutra_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_scg_eutra_present) {
    j.write_fieldname("failureReportSCG-EUTRA");
    fail_report_scg_eutra.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SecurityModeComplete-IEs ::= SEQUENCE
SRSASN_CODE security_mode_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SecurityModeFailure-IEs ::= SEQUENCE
SRSASN_CODE security_mode_fail_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_fail_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SidelinkUEInformationNR-r16-IEs ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_nr_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_rx_interested_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_tx_res_req_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_fail_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sl_rx_interested_freq_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rx_interested_freq_list_r16, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_tx_res_req_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_res_req_list_r16, 1, 32));
  }
  if (sl_fail_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_fail_list_r16, 1, 32));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_nr_r16_ies_s::unpack(cbit_ref& bref)
{
  bool sl_rx_interested_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_rx_interested_freq_list_r16_present, 1));
  bool sl_tx_res_req_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_tx_res_req_list_r16_present, 1));
  bool sl_fail_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_fail_list_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sl_rx_interested_freq_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rx_interested_freq_list_r16, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_tx_res_req_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_tx_res_req_list_r16, bref, 1, 32));
  }
  if (sl_fail_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_fail_list_r16, bref, 1, 32));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_nr_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_rx_interested_freq_list_r16.size() > 0) {
    j.start_array("sl-RxInterestedFreqList-r16");
    for (const auto& e1 : sl_rx_interested_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_tx_res_req_list_r16.size() > 0) {
    j.start_array("sl-TxResourceReqList-r16");
    for (const auto& e1 : sl_tx_res_req_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_fail_list_r16.size() > 0) {
    j.start_array("sl-FailureList-r16");
    for (const auto& e1 : sl_fail_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UEAssistanceInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_assist_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delay_budget_report_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (delay_budget_report_present) {
    HANDLE_CODE(delay_budget_report.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_assist_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delay_budget_report_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (delay_budget_report_present) {
    HANDLE_CODE(delay_budget_report.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_assist_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (delay_budget_report_present) {
    j.write_fieldname("delayBudgetReport");
    delay_budget_report.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_cap_rat_container_list_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_cap_rat_container_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_cap_rat_container_list, 0, 8));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_cap_rat_container_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_cap_rat_container_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ue_cap_rat_container_list, bref, 0, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_cap_rat_container_list_present) {
    j.start_array("ue-CapabilityRAT-ContainerList");
    for (const auto& e1 : ue_cap_rat_container_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UEInformationResponse-r16-IEs ::= SEQUENCE
SRSASN_CODE ue_info_resp_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_idle_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_result_idle_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(log_meas_report_r16_present, 1));
  HANDLE_CODE(bref.pack(conn_est_fail_report_r16_present, 1));
  HANDLE_CODE(bref.pack(ra_report_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(rlf_report_r16_present, 1));
  HANDLE_CODE(bref.pack(mob_history_report_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_idle_eutra_r16_present) {
    HANDLE_CODE(meas_result_idle_eutra_r16.pack(bref));
  }
  if (meas_result_idle_nr_r16_present) {
    HANDLE_CODE(meas_result_idle_nr_r16.pack(bref));
  }
  if (log_meas_report_r16_present) {
    HANDLE_CODE(log_meas_report_r16.pack(bref));
  }
  if (conn_est_fail_report_r16_present) {
    HANDLE_CODE(conn_est_fail_report_r16.pack(bref));
  }
  if (ra_report_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ra_report_list_r16, 1, 8));
  }
  if (rlf_report_r16_present) {
    HANDLE_CODE(rlf_report_r16.pack(bref));
  }
  if (mob_history_report_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mob_history_report_r16, 1, 16));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_idle_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_idle_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_report_r16_present, 1));
  HANDLE_CODE(bref.unpack(conn_est_fail_report_r16_present, 1));
  bool ra_report_list_r16_present;
  HANDLE_CODE(bref.unpack(ra_report_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(rlf_report_r16_present, 1));
  bool mob_history_report_r16_present;
  HANDLE_CODE(bref.unpack(mob_history_report_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_idle_eutra_r16_present) {
    HANDLE_CODE(meas_result_idle_eutra_r16.unpack(bref));
  }
  if (meas_result_idle_nr_r16_present) {
    HANDLE_CODE(meas_result_idle_nr_r16.unpack(bref));
  }
  if (log_meas_report_r16_present) {
    HANDLE_CODE(log_meas_report_r16.unpack(bref));
  }
  if (conn_est_fail_report_r16_present) {
    HANDLE_CODE(conn_est_fail_report_r16.unpack(bref));
  }
  if (ra_report_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ra_report_list_r16, bref, 1, 8));
  }
  if (rlf_report_r16_present) {
    HANDLE_CODE(rlf_report_r16.unpack(bref));
  }
  if (mob_history_report_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mob_history_report_r16, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_info_resp_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_idle_eutra_r16_present) {
    j.write_fieldname("measResultIdleEUTRA-r16");
    meas_result_idle_eutra_r16.to_json(j);
  }
  if (meas_result_idle_nr_r16_present) {
    j.write_fieldname("measResultIdleNR-r16");
    meas_result_idle_nr_r16.to_json(j);
  }
  if (log_meas_report_r16_present) {
    j.write_fieldname("logMeasReport-r16");
    log_meas_report_r16.to_json(j);
  }
  if (conn_est_fail_report_r16_present) {
    j.write_fieldname("connEstFailReport-r16");
    conn_est_fail_report_r16.to_json(j);
  }
  if (ra_report_list_r16.size() > 0) {
    j.start_array("ra-ReportList-r16");
    for (const auto& e1 : ra_report_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rlf_report_r16_present) {
    j.write_fieldname("rlf-Report-r16");
    rlf_report_r16.to_json(j);
  }
  if (mob_history_report_r16.size() > 0) {
    j.start_array("mobilityHistoryReport-r16");
    for (const auto& e1 : mob_history_report_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UEPositioningAssistanceInfo-r17-IEs ::= SEQUENCE
SRSASN_CODE ue_positioning_assist_info_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_tx_teg_assoc_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_tx_teg_assoc_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_tx_teg_assoc_list_r17, 1, 256));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_positioning_assist_info_r17_ies_s::unpack(cbit_ref& bref)
{
  bool ue_tx_teg_assoc_list_r17_present;
  HANDLE_CODE(bref.unpack(ue_tx_teg_assoc_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_tx_teg_assoc_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ue_tx_teg_assoc_list_r17, bref, 1, 256));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_positioning_assist_info_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_tx_teg_assoc_list_r17.size() > 0) {
    j.start_array("ue-TxTEG-AssociationList-r17");
    for (const auto& e1 : ue_tx_teg_assoc_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ULDedicatedMessageSegment-r16-IEs ::= SEQUENCE
SRSASN_CODE ul_ded_msg_segment_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, segment_num_r16, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(rrc_msg_segment_container_r16.pack(bref));
  HANDLE_CODE(rrc_msg_segment_type_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ded_msg_segment_r16_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(segment_num_r16, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(rrc_msg_segment_container_r16.unpack(bref));
  HANDLE_CODE(rrc_msg_segment_type_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_ded_msg_segment_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("segmentNumber-r16", segment_num_r16);
  j.write_str("rrc-MessageSegmentContainer-r16", rrc_msg_segment_container_r16.to_string());
  j.write_str("rrc-MessageSegmentType-r16", rrc_msg_segment_type_r16.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* ul_ded_msg_segment_r16_ies_s::rrc_msg_segment_type_r16_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "ul_ded_msg_segment_r16_ies_s::rrc_msg_segment_type_r16_e_");
}

// ULInformationTransfer-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_nas_msg.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_nas_msg.size() > 0) {
    HANDLE_CODE(ded_nas_msg.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_ies_s::unpack(cbit_ref& bref)
{
  bool ded_nas_msg_present;
  HANDLE_CODE(bref.unpack(ded_nas_msg_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_nas_msg_present) {
    HANDLE_CODE(ded_nas_msg.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_nas_msg.size() > 0) {
    j.write_str("dedicatedNAS-Message", ded_nas_msg.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ULInformationTransferIRAT-r16-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_irat_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_dcch_msg_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_dcch_msg_eutra_r16.size() > 0) {
    HANDLE_CODE(ul_dcch_msg_eutra_r16.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_irat_r16_ies_s::unpack(cbit_ref& bref)
{
  bool ul_dcch_msg_eutra_r16_present;
  HANDLE_CODE(bref.unpack(ul_dcch_msg_eutra_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_dcch_msg_eutra_r16_present) {
    HANDLE_CODE(ul_dcch_msg_eutra_r16.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_irat_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_dcch_msg_eutra_r16.size() > 0) {
    j.write_str("ul-DCCH-MessageEUTRA-r16", ul_dcch_msg_eutra_r16.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// ULInformationTransferMRDC-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_mrdc_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_dcch_msg_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(ul_dcch_msg_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_dcch_msg_nr.size() > 0) {
    HANDLE_CODE(ul_dcch_msg_nr.pack(bref));
  }
  if (ul_dcch_msg_eutra.size() > 0) {
    HANDLE_CODE(ul_dcch_msg_eutra.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_ies_s::unpack(cbit_ref& bref)
{
  bool ul_dcch_msg_nr_present;
  HANDLE_CODE(bref.unpack(ul_dcch_msg_nr_present, 1));
  bool ul_dcch_msg_eutra_present;
  HANDLE_CODE(bref.unpack(ul_dcch_msg_eutra_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_dcch_msg_nr_present) {
    HANDLE_CODE(ul_dcch_msg_nr.unpack(bref));
  }
  if (ul_dcch_msg_eutra_present) {
    HANDLE_CODE(ul_dcch_msg_eutra.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_mrdc_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_dcch_msg_nr.size() > 0) {
    j.write_str("ul-DCCH-MessageNR", ul_dcch_msg_nr.to_string());
  }
  if (ul_dcch_msg_eutra.size() > 0) {
    j.write_str("ul-DCCH-MessageEUTRA", ul_dcch_msg_eutra.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CounterCheckResponse ::= SEQUENCE
SRSASN_CODE counter_check_resp_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void counter_check_resp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void counter_check_resp_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
counter_check_resp_ies_s& counter_check_resp_s::crit_exts_c_::set_counter_check_resp()
{
  set(types::counter_check_resp);
  return c;
}
void counter_check_resp_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void counter_check_resp_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::counter_check_resp:
      j.write_fieldname("counterCheckResponse");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE counter_check_resp_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::counter_check_resp:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::counter_check_resp:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* counter_check_resp_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"counterCheckResponse", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "counter_check_resp_s::crit_exts_c_::types");
}

// DedicatedSIBRequest-r16 ::= SEQUENCE
SRSASN_CODE ded_sib_request_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ded_sib_request_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ded_sib_request_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ded_sib_request_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ded_sib_request_r16_ies_s& ded_sib_request_r16_s::crit_exts_c_::set_ded_sib_request_r16()
{
  set(types::ded_sib_request_r16);
  return c;
}
void ded_sib_request_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ded_sib_request_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ded_sib_request_r16:
      j.write_fieldname("dedicatedSIBRequest-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ded_sib_request_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ded_sib_request_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ded_sib_request_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ded_sib_request_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ded_sib_request_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ded_sib_request_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ded_sib_request_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ded_sib_request_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"dedicatedSIBRequest-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ded_sib_request_r16_s::crit_exts_c_::types");
}

// FailureInformation ::= SEQUENCE
SRSASN_CODE fail_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void fail_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void fail_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
fail_info_ies_s& fail_info_s::crit_exts_c_::set_fail_info()
{
  set(types::fail_info);
  return c;
}
void fail_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void fail_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fail_info:
      j.write_fieldname("failureInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "fail_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE fail_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fail_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fail_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* fail_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"failureInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "fail_info_s::crit_exts_c_::types");
}

// IABOtherInformation-r16 ::= SEQUENCE
SRSASN_CODE iab_other_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_other_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void iab_other_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void iab_other_info_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
iab_other_info_r16_ies_s& iab_other_info_r16_s::crit_exts_c_::set_iab_other_info_r16()
{
  set(types::iab_other_info_r16);
  return c;
}
void iab_other_info_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void iab_other_info_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::iab_other_info_r16:
      j.write_fieldname("iabOtherInformation-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE iab_other_info_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::iab_other_info_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_other_info_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::iab_other_info_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "iab_other_info_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* iab_other_info_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"iabOtherInformation-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "iab_other_info_r16_s::crit_exts_c_::types");
}

// LocationMeasurementIndication ::= SEQUENCE
SRSASN_CODE location_meas_ind_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_ind_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void location_meas_ind_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void location_meas_ind_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
location_meas_ind_ies_s& location_meas_ind_s::crit_exts_c_::set_location_meas_ind()
{
  set(types::location_meas_ind);
  return c;
}
void location_meas_ind_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void location_meas_ind_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::location_meas_ind:
      j.write_fieldname("locationMeasurementIndication");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_ind_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE location_meas_ind_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::location_meas_ind:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_ind_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_ind_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::location_meas_ind:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_ind_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* location_meas_ind_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"locationMeasurementIndication", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "location_meas_ind_s::crit_exts_c_::types");
}

// MBSInterestIndication-r17 ::= SEQUENCE
SRSASN_CODE mb_si_nterest_ind_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mb_si_nterest_ind_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mb_si_nterest_ind_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mb_si_nterest_ind_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mb_si_nterest_ind_r17_ies_s& mb_si_nterest_ind_r17_s::crit_exts_c_::set_mbs_interest_ind_r17()
{
  set(types::mbs_interest_ind_r17);
  return c;
}
void mb_si_nterest_ind_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mb_si_nterest_ind_r17_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mbs_interest_ind_r17:
      j.write_fieldname("mbsInterestIndication-r17");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mb_si_nterest_ind_r17_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mb_si_nterest_ind_r17_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mbs_interest_ind_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mb_si_nterest_ind_r17_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mb_si_nterest_ind_r17_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mbs_interest_ind_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mb_si_nterest_ind_r17_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mb_si_nterest_ind_r17_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mbsInterestIndication-r17", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mb_si_nterest_ind_r17_s::crit_exts_c_::types");
}

// MCGFailureInformation-r16 ::= SEQUENCE
SRSASN_CODE mcg_fail_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcg_fail_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mcg_fail_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mcg_fail_info_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mcg_fail_info_r16_ies_s& mcg_fail_info_r16_s::crit_exts_c_::set_mcg_fail_info_r16()
{
  set(types::mcg_fail_info_r16);
  return c;
}
void mcg_fail_info_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mcg_fail_info_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mcg_fail_info_r16:
      j.write_fieldname("mcgFailureInformation-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mcg_fail_info_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mcg_fail_info_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mcg_fail_info_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mcg_fail_info_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcg_fail_info_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mcg_fail_info_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mcg_fail_info_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mcg_fail_info_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mcgFailureInformation-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mcg_fail_info_r16_s::crit_exts_c_::types");
}

// MeasurementReport ::= SEQUENCE
SRSASN_CODE meas_report_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_report_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void meas_report_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_report_ies_s& meas_report_s::crit_exts_c_::set_meas_report()
{
  set(types::meas_report);
  return c;
}
void meas_report_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void meas_report_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_report:
      j.write_fieldname("measurementReport");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_report_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_report_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"measurementReport", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "meas_report_s::crit_exts_c_::types");
}

// RRCReconfigurationComplete ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_recfg_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_recfg_complete_ies_s& rrc_recfg_complete_s::crit_exts_c_::set_rrc_recfg_complete()
{
  set(types::rrc_recfg_complete);
  return c;
}
void rrc_recfg_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_recfg_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_recfg_complete:
      j.write_fieldname("rrcReconfigurationComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_recfg_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_recfg_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReconfigurationComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_recfg_complete_s::crit_exts_c_::types");
}

// RRCReestablishmentComplete ::= SEQUENCE
SRSASN_CODE rrc_reest_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_reest_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_reest_complete_ies_s& rrc_reest_complete_s::crit_exts_c_::set_rrc_reest_complete()
{
  set(types::rrc_reest_complete);
  return c;
}
void rrc_reest_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_reest_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_reest_complete:
      j.write_fieldname("rrcReestablishmentComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_reest_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_reest_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_reest_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_reest_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReestablishmentComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_reest_complete_s::crit_exts_c_::types");
}

// RRCResumeComplete ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_resume_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_resume_complete_ies_s& rrc_resume_complete_s::crit_exts_c_::set_rrc_resume_complete()
{
  set(types::rrc_resume_complete);
  return c;
}
void rrc_resume_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_resume_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_resume_complete:
      j.write_fieldname("rrcResumeComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_resume_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_resume_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_resume_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_resume_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcResumeComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_resume_complete_s::crit_exts_c_::types");
}

// RRCSetupComplete ::= SEQUENCE
SRSASN_CODE rrc_setup_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_setup_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_setup_complete_ies_s& rrc_setup_complete_s::crit_exts_c_::set_rrc_setup_complete()
{
  set(types::rrc_setup_complete);
  return c;
}
void rrc_setup_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_setup_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_setup_complete:
      j.write_fieldname("rrcSetupComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_setup_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_setup_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_setup_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_setup_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcSetupComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_setup_complete_s::crit_exts_c_::types");
}

// SCGFailureInformation ::= SEQUENCE
SRSASN_CODE scg_fail_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_fail_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_fail_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_ies_s& scg_fail_info_s::crit_exts_c_::set_scg_fail_info()
{
  set(types::scg_fail_info);
  return c;
}
void scg_fail_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_fail_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_fail_info:
      j.write_fieldname("scgFailureInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_fail_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_fail_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* scg_fail_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"scgFailureInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "scg_fail_info_s::crit_exts_c_::types");
}

// SCGFailureInformationEUTRA ::= SEQUENCE
SRSASN_CODE scg_fail_info_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_fail_info_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_fail_info_eutra_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_eutra_ies_s& scg_fail_info_eutra_s::crit_exts_c_::set_scg_fail_info_eutra()
{
  set(types::scg_fail_info_eutra);
  return c;
}
void scg_fail_info_eutra_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_fail_info_eutra_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_fail_info_eutra:
      j.write_fieldname("scgFailureInformationEUTRA");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_eutra_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_eutra_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_eutra_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_eutra_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* scg_fail_info_eutra_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"scgFailureInformationEUTRA", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "scg_fail_info_eutra_s::crit_exts_c_::types");
}

// SecurityModeComplete ::= SEQUENCE
SRSASN_CODE security_mode_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_complete_ies_s& security_mode_complete_s::crit_exts_c_::set_security_mode_complete()
{
  set(types::security_mode_complete);
  return c;
}
void security_mode_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_complete:
      j.write_fieldname("securityModeComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* security_mode_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"securityModeComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "security_mode_complete_s::crit_exts_c_::types");
}

// SecurityModeFailure ::= SEQUENCE
SRSASN_CODE security_mode_fail_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_fail_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_fail_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_fail_ies_s& security_mode_fail_s::crit_exts_c_::set_security_mode_fail()
{
  set(types::security_mode_fail);
  return c;
}
void security_mode_fail_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_fail_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_fail:
      j.write_fieldname("securityModeFailure");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_fail_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_fail:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_fail:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* security_mode_fail_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"securityModeFailure", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "security_mode_fail_s::crit_exts_c_::types");
}

// SidelinkUEInformationNR-r16 ::= SEQUENCE
SRSASN_CODE sidelink_ue_info_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void sidelink_ue_info_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void sidelink_ue_info_nr_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
sidelink_ue_info_nr_r16_ies_s& sidelink_ue_info_nr_r16_s::crit_exts_c_::set_sidelink_ue_info_nr_r16()
{
  set(types::sidelink_ue_info_nr_r16);
  return c;
}
void sidelink_ue_info_nr_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void sidelink_ue_info_nr_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sidelink_ue_info_nr_r16:
      j.write_fieldname("sidelinkUEInformationNR-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sidelink_ue_info_nr_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE sidelink_ue_info_nr_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sidelink_ue_info_nr_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sidelink_ue_info_nr_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_ue_info_nr_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sidelink_ue_info_nr_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sidelink_ue_info_nr_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sidelink_ue_info_nr_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"sidelinkUEInformationNR-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "sidelink_ue_info_nr_r16_s::crit_exts_c_::types");
}

// UEAssistanceInformation ::= SEQUENCE
SRSASN_CODE ue_assist_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_assist_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_assist_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_assist_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_assist_info_ies_s& ue_assist_info_s::crit_exts_c_::set_ue_assist_info()
{
  set(types::ue_assist_info);
  return c;
}
void ue_assist_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_assist_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_assist_info:
      j.write_fieldname("ueAssistanceInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_assist_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_assist_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_assist_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_assist_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_assist_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_assist_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_assist_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_assist_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueAssistanceInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_assist_info_s::crit_exts_c_::types");
}

// UECapabilityInformation ::= SEQUENCE
SRSASN_CODE ue_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_cap_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_info_ies_s& ue_cap_info_s::crit_exts_c_::set_ue_cap_info()
{
  set(types::ue_cap_info);
  return c;
}
void ue_cap_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_cap_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_cap_info:
      j.write_fieldname("ueCapabilityInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_cap_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_cap_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_cap_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueCapabilityInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_cap_info_s::crit_exts_c_::types");
}

// UEInformationResponse-r16 ::= SEQUENCE
SRSASN_CODE ue_info_resp_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_info_resp_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_info_resp_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_info_resp_r16_ies_s& ue_info_resp_r16_s::crit_exts_c_::set_ue_info_resp_r16()
{
  set(types::ue_info_resp_r16);
  return c;
}
void ue_info_resp_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_info_resp_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_info_resp_r16:
      j.write_fieldname("ueInformationResponse-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_resp_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_info_resp_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_info_resp_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_resp_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_resp_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_info_resp_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_resp_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_info_resp_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueInformationResponse-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_info_resp_r16_s::crit_exts_c_::types");
}

// UEPositioningAssistanceInfo-r17 ::= SEQUENCE
SRSASN_CODE ue_positioning_assist_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_positioning_assist_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_positioning_assist_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_positioning_assist_info_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_positioning_assist_info_r17_ies_s&
ue_positioning_assist_info_r17_s::crit_exts_c_::set_ue_positioning_assist_info_r17()
{
  set(types::ue_positioning_assist_info_r17);
  return c;
}
void ue_positioning_assist_info_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_positioning_assist_info_r17_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_positioning_assist_info_r17:
      j.write_fieldname("uePositioningAssistanceInfo-r17");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_positioning_assist_info_r17_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_positioning_assist_info_r17_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_positioning_assist_info_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_positioning_assist_info_r17_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_positioning_assist_info_r17_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_positioning_assist_info_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_positioning_assist_info_r17_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_positioning_assist_info_r17_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"uePositioningAssistanceInfo-r17", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_positioning_assist_info_r17_s::crit_exts_c_::types");
}

// ULDedicatedMessageSegment-r16 ::= SEQUENCE
SRSASN_CODE ul_ded_msg_segment_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ded_msg_segment_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_ded_msg_segment_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_ded_msg_segment_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_ded_msg_segment_r16_ies_s& ul_ded_msg_segment_r16_s::crit_exts_c_::set_ul_ded_msg_segment_r16()
{
  set(types::ul_ded_msg_segment_r16);
  return c;
}
void ul_ded_msg_segment_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_ded_msg_segment_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_ded_msg_segment_r16:
      j.write_fieldname("ulDedicatedMessageSegment-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ded_msg_segment_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ded_msg_segment_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_ded_msg_segment_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ded_msg_segment_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ded_msg_segment_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_ded_msg_segment_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ded_msg_segment_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_ded_msg_segment_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulDedicatedMessageSegment-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_ded_msg_segment_r16_s::crit_exts_c_::types");
}

// ULInformationTransfer ::= SEQUENCE
SRSASN_CODE ul_info_transfer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_ies_s& ul_info_transfer_s::crit_exts_c_::set_ul_info_transfer()
{
  set(types::ul_info_transfer);
  return c;
}
void ul_info_transfer_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_info_transfer_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer:
      j.write_fieldname("ulInformationTransfer");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_info_transfer_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulInformationTransfer", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_info_transfer_s::crit_exts_c_::types");
}

// ULInformationTransferIRAT-r16 ::= SEQUENCE
SRSASN_CODE ul_info_transfer_irat_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_irat_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_irat_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_irat_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_& ul_info_transfer_irat_r16_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ul_info_transfer_irat_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_info_transfer_irat_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_irat_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_irat_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_irat_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_irat_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_irat_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_irat_r16_ies_s& ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::set_ul_info_transfer_irat_r16()
{
  set(types::ul_info_transfer_irat_r16);
  return c;
}
void ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer_irat_r16:
      j.write_fieldname("ulInformationTransferIRAT-r16");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer_irat_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer_irat_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulInformationTransferIRAT-r16", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::types");
}

const char* ul_info_transfer_irat_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_info_transfer_irat_r16_s::crit_exts_c_::types");
}
uint8_t ul_info_transfer_irat_r16_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_info_transfer_irat_r16_s::crit_exts_c_::types");
}

// ULInformationTransferMRDC ::= SEQUENCE
SRSASN_CODE ul_info_transfer_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_mrdc_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_& ul_info_transfer_mrdc_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ul_info_transfer_mrdc_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_mrdc_ies_s& ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_ul_info_transfer_mrdc()
{
  set(types::ul_info_transfer_mrdc);
  return c;
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer_mrdc:
      j.write_fieldname("ulInformationTransferMRDC");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulInformationTransferMRDC", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::types");
}

const char* ul_info_transfer_mrdc_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_info_transfer_mrdc_s::crit_exts_c_::types");
}
uint8_t ul_info_transfer_mrdc_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_info_transfer_mrdc_s::crit_exts_c_::types");
}

// DRX-Config ::= SEQUENCE
SRSASN_CODE drx_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(short_drx_present, 1));

  HANDLE_CODE(drx_on_dur_timer.pack(bref));
  HANDLE_CODE(drx_inactivity_timer.pack(bref));
  HANDLE_CODE(pack_integer(bref, drx_harq_rtt_timer_dl, (uint8_t)0u, (uint8_t)56u));
  HANDLE_CODE(pack_integer(bref, drx_harq_rtt_timer_ul, (uint8_t)0u, (uint8_t)56u));
  HANDLE_CODE(drx_retx_timer_dl.pack(bref));
  HANDLE_CODE(drx_retx_timer_ul.pack(bref));
  HANDLE_CODE(drx_long_cycle_start_offset.pack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.pack(bref));
    HANDLE_CODE(pack_integer(bref, short_drx.drx_short_cycle_timer, (uint8_t)1u, (uint8_t)16u));
  }
  HANDLE_CODE(pack_integer(bref, drx_slot_offset, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(short_drx_present, 1));

  HANDLE_CODE(drx_on_dur_timer.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer.unpack(bref));
  HANDLE_CODE(unpack_integer(drx_harq_rtt_timer_dl, bref, (uint8_t)0u, (uint8_t)56u));
  HANDLE_CODE(unpack_integer(drx_harq_rtt_timer_ul, bref, (uint8_t)0u, (uint8_t)56u));
  HANDLE_CODE(drx_retx_timer_dl.unpack(bref));
  HANDLE_CODE(drx_retx_timer_ul.unpack(bref));
  HANDLE_CODE(drx_long_cycle_start_offset.unpack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.unpack(bref));
    HANDLE_CODE(unpack_integer(short_drx.drx_short_cycle_timer, bref, (uint8_t)1u, (uint8_t)16u));
  }
  HANDLE_CODE(unpack_integer(drx_slot_offset, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void drx_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-onDurationTimer");
  drx_on_dur_timer.to_json(j);
  j.write_str("drx-InactivityTimer", drx_inactivity_timer.to_string());
  j.write_int("drx-HARQ-RTT-TimerDL", drx_harq_rtt_timer_dl);
  j.write_int("drx-HARQ-RTT-TimerUL", drx_harq_rtt_timer_ul);
  j.write_str("drx-RetransmissionTimerDL", drx_retx_timer_dl.to_string());
  j.write_str("drx-RetransmissionTimerUL", drx_retx_timer_ul.to_string());
  j.write_fieldname("drx-LongCycleStartOffset");
  drx_long_cycle_start_offset.to_json(j);
  if (short_drx_present) {
    j.write_fieldname("shortDRX");
    j.start_obj();
    j.write_str("drx-ShortCycle", short_drx.drx_short_cycle.to_string());
    j.write_int("drx-ShortCycleTimer", short_drx.drx_short_cycle_timer);
    j.end_obj();
  }
  j.write_int("drx-SlotOffset", drx_slot_offset);
  j.end_obj();
}

void drx_cfg_s::drx_on_dur_timer_c_::destroy_() {}
void drx_cfg_s::drx_on_dur_timer_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_cfg_s::drx_on_dur_timer_c_::drx_on_dur_timer_c_(const drx_cfg_s::drx_on_dur_timer_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sub_milli_seconds:
      c.init(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.init(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_on_dur_timer_c_");
  }
}
drx_cfg_s::drx_on_dur_timer_c_& drx_cfg_s::drx_on_dur_timer_c_::operator=(const drx_cfg_s::drx_on_dur_timer_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sub_milli_seconds:
      c.set(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.set(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_on_dur_timer_c_");
  }

  return *this;
}
uint8_t& drx_cfg_s::drx_on_dur_timer_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_e_& drx_cfg_s::drx_on_dur_timer_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void drx_cfg_s::drx_on_dur_timer_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sub_milli_seconds:
      j.write_int("subMilliSeconds", c.get<uint8_t>());
      break;
    case types::milli_seconds:
      j.write_str("milliSeconds", c.get<milli_seconds_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_on_dur_timer_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_s::drx_on_dur_timer_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_on_dur_timer_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_s::drx_on_dur_timer_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_on_dur_timer_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_e_");
}
uint16_t drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 24, value, "drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_e_");
}

const char* drx_cfg_s::drx_on_dur_timer_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "drx_cfg_s::drx_on_dur_timer_c_::types");
}

const char* drx_cfg_s::drx_inactivity_timer_opts::to_string() const
{
  static const char* names[] = {"ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",
                                "ms10",   "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",
                                "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_s::drx_inactivity_timer_e_");
}
uint16_t drx_cfg_s::drx_inactivity_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "drx_cfg_s::drx_inactivity_timer_e_");
}

const char* drx_cfg_s::drx_retx_timer_dl_opts::to_string() const
{
  static const char* names[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_s::drx_retx_timer_dl_e_");
}
uint16_t drx_cfg_s::drx_retx_timer_dl_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "drx_cfg_s::drx_retx_timer_dl_e_");
}

const char* drx_cfg_s::drx_retx_timer_ul_opts::to_string() const
{
  static const char* names[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_s::drx_retx_timer_ul_e_");
}
uint16_t drx_cfg_s::drx_retx_timer_ul_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "drx_cfg_s::drx_retx_timer_ul_e_");
}

void drx_cfg_s::drx_long_cycle_start_offset_c_::destroy_() {}
void drx_cfg_s::drx_long_cycle_start_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_cfg_s::drx_long_cycle_start_offset_c_::drx_long_cycle_start_offset_c_(
    const drx_cfg_s::drx_long_cycle_start_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_long_cycle_start_offset_c_");
  }
}
drx_cfg_s::drx_long_cycle_start_offset_c_&
drx_cfg_s::drx_long_cycle_start_offset_c_::operator=(const drx_cfg_s::drx_long_cycle_start_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_long_cycle_start_offset_c_");
  }

  return *this;
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_s::drx_long_cycle_start_offset_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
void drx_cfg_s::drx_long_cycle_start_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms10:
      j.write_int("ms10", c.get<uint8_t>());
      break;
    case types::ms20:
      j.write_int("ms20", c.get<uint8_t>());
      break;
    case types::ms32:
      j.write_int("ms32", c.get<uint8_t>());
      break;
    case types::ms40:
      j.write_int("ms40", c.get<uint8_t>());
      break;
    case types::ms60:
      j.write_int("ms60", c.get<uint8_t>());
      break;
    case types::ms64:
      j.write_int("ms64", c.get<uint8_t>());
      break;
    case types::ms70:
      j.write_int("ms70", c.get<uint8_t>());
      break;
    case types::ms80:
      j.write_int("ms80", c.get<uint8_t>());
      break;
    case types::ms128:
      j.write_int("ms128", c.get<uint8_t>());
      break;
    case types::ms160:
      j.write_int("ms160", c.get<uint8_t>());
      break;
    case types::ms256:
      j.write_int("ms256", c.get<uint16_t>());
      break;
    case types::ms320:
      j.write_int("ms320", c.get<uint16_t>());
      break;
    case types::ms512:
      j.write_int("ms512", c.get<uint16_t>());
      break;
    case types::ms640:
      j.write_int("ms640", c.get<uint16_t>());
      break;
    case types::ms1024:
      j.write_int("ms1024", c.get<uint16_t>());
      break;
    case types::ms1280:
      j.write_int("ms1280", c.get<uint16_t>());
      break;
    case types::ms2048:
      j.write_int("ms2048", c.get<uint16_t>());
      break;
    case types::ms2560:
      j.write_int("ms2560", c.get<uint16_t>());
      break;
    case types::ms5120:
      j.write_int("ms5120", c.get<uint16_t>());
      break;
    case types::ms10240:
      j.write_int("ms10240", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_long_cycle_start_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_s::drx_long_cycle_start_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_s::drx_long_cycle_start_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_cfg_s::drx_long_cycle_start_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "drx_cfg_s::drx_long_cycle_start_offset_c_::types");
}
uint16_t drx_cfg_s::drx_long_cycle_start_offset_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "drx_cfg_s::drx_long_cycle_start_offset_c_::types");
}

const char* drx_cfg_s::short_drx_s_::drx_short_cycle_opts::to_string() const
{
  static const char* names[] = {"ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms7",    "ms8",    "ms10",
                                "ms14",   "ms16",   "ms20",   "ms30",   "ms32",   "ms35",   "ms40",   "ms64",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",  "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_s::short_drx_s_::drx_short_cycle_e_");
}
uint16_t drx_cfg_s::short_drx_s_::drx_short_cycle_opts::to_number() const
{
  static const uint16_t numbers[] = {2,  3,  4,  5,  6,  7,   8,   10,  14,  16,  20, 30,
                                     32, 35, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(numbers, 23, value, "drx_cfg_s::short_drx_s_::drx_short_cycle_e_");
}

// BandCombinationInfo ::= SEQUENCE
SRSASN_CODE band_combination_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_combination_idx, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(pack_dyn_seq_of(bref, allowed_feature_sets_list, 1, 128, integer_packer<uint8_t>(1, 128)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_combination_idx, bref, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(unpack_dyn_seq_of(allowed_feature_sets_list, bref, 1, 128, integer_packer<uint8_t>(1, 128)));

  return SRSASN_SUCCESS;
}
void band_combination_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandCombinationIndex", band_combination_idx);
  j.start_array("allowedFeatureSetsList");
  for (const auto& e1 : allowed_feature_sets_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// T-Offset-r16 ::= ENUMERATED
const char* t_offset_r16_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms0dot75", "ms1", "ms1dot5", "ms2", "ms2dot5", "ms3", "spare1"};
  return convert_enum_idx(names, 8, value, "t_offset_r16_e");
}
float t_offset_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0};
  return map_enum_number(numbers, 7, value, "t_offset_r16_e");
}
const char* t_offset_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "0.75", "1", "1.5", "2", "2.5", "3"};
  return convert_enum_idx(number_strs, 8, value, "t_offset_r16_e");
}

// ConfigRestrictInfoSCG ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(allowed_bc_list_mrdc.size() > 0, 1));
  HANDLE_CODE(bref.pack(pwr_coordination_fr1_present, 1));
  HANDLE_CODE(bref.pack(serv_cell_idx_range_scg_present, 1));
  HANDLE_CODE(bref.pack(max_meas_freqs_scg_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));

  if (allowed_bc_list_mrdc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_bc_list_mrdc, 1, 65536));
  }
  if (pwr_coordination_fr1_present) {
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_nr_fr1_present, 1));
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_eutra_present, 1));
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_ue_fr1_present, 1));
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_nr_fr1, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_eutra, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_ue_fr1, (int8_t)-30, (int8_t)33));
    }
  }
  if (serv_cell_idx_range_scg_present) {
    HANDLE_CODE(pack_integer(bref, serv_cell_idx_range_scg.low_bound, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, serv_cell_idx_range_scg.up_bound, (uint8_t)0u, (uint8_t)31u));
  }
  if (max_meas_freqs_scg_present) {
    HANDLE_CODE(pack_integer(bref, max_meas_freqs_scg, (uint8_t)1u, (uint8_t)32u));
  }
  if (dummy_present) {
    HANDLE_CODE(pack_integer(bref, dummy, (uint8_t)1u, (uint8_t)62u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sel_band_entries_mn_list.is_present();
    group_flags[0] |= pdcch_blind_detection_scg_present;
    group_flags[0] |= max_num_rohc_context_sessions_sn_present;
    group_flags[1] |= max_intra_freq_meas_identities_scg_present;
    group_flags[1] |= max_inter_freq_meas_identities_scg_present;
    group_flags[2] |= p_max_nr_fr1_mcg_r16_present;
    group_flags[2] |= pwr_coordination_fr2_r16.is_present();
    group_flags[2] |= nrdc_pc_mode_fr1_r16_present;
    group_flags[2] |= nrdc_pc_mode_fr2_r16_present;
    group_flags[2] |= max_meas_srs_res_scg_r16_present;
    group_flags[2] |= max_meas_cli_res_scg_r16_present;
    group_flags[2] |= max_num_ehc_contexts_sn_r16_present;
    group_flags[2] |= allowed_reduced_cfg_for_overheat_r16.is_present();
    group_flags[2] |= max_toffset_r16_present;
    group_flags[3] |= allowed_reduced_cfg_for_overheat_r17.is_present();
    group_flags[3] |= max_num_udc_drb_r17_present;
    group_flags[3] |= max_num_cp_cc_andidates_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sel_band_entries_mn_list.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.pack(max_num_rohc_context_sessions_sn_present, 1));
      if (sel_band_entries_mn_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref,
                                    *sel_band_entries_mn_list,
                                    1,
                                    65536,
                                    SeqOfPacker<integer_packer<uint8_t>>(1, 32, integer_packer<uint8_t>(0, 32))));
      }
      if (pdcch_blind_detection_scg_present) {
        HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_scg, (uint8_t)1u, (uint8_t)15u));
      }
      if (max_num_rohc_context_sessions_sn_present) {
        HANDLE_CODE(pack_integer(bref, max_num_rohc_context_sessions_sn, (uint16_t)0u, (uint16_t)16384u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_intra_freq_meas_identities_scg_present, 1));
      HANDLE_CODE(bref.pack(max_inter_freq_meas_identities_scg_present, 1));
      if (max_intra_freq_meas_identities_scg_present) {
        HANDLE_CODE(pack_integer(bref, max_intra_freq_meas_identities_scg, (uint8_t)1u, (uint8_t)62u));
      }
      if (max_inter_freq_meas_identities_scg_present) {
        HANDLE_CODE(pack_integer(bref, max_inter_freq_meas_identities_scg, (uint8_t)1u, (uint8_t)62u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(p_max_nr_fr1_mcg_r16_present, 1));
      HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(nrdc_pc_mode_fr1_r16_present, 1));
      HANDLE_CODE(bref.pack(nrdc_pc_mode_fr2_r16_present, 1));
      HANDLE_CODE(bref.pack(max_meas_srs_res_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(max_meas_cli_res_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(max_num_ehc_contexts_sn_r16_present, 1));
      HANDLE_CODE(bref.pack(allowed_reduced_cfg_for_overheat_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(max_toffset_r16_present, 1));
      if (p_max_nr_fr1_mcg_r16_present) {
        HANDLE_CODE(pack_integer(bref, p_max_nr_fr1_mcg_r16, (int8_t)-30, (int8_t)33));
      }
      if (pwr_coordination_fr2_r16.is_present()) {
        HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present, 1));
        HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present, 1));
        HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present, 1));
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present) {
          HANDLE_CODE(pack_integer(bref, pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present) {
          HANDLE_CODE(pack_integer(bref, pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present) {
          HANDLE_CODE(pack_integer(bref, pwr_coordination_fr2_r16->p_max_ue_fr2_r16, (int8_t)-30, (int8_t)33));
        }
      }
      if (nrdc_pc_mode_fr1_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr1_r16.pack(bref));
      }
      if (nrdc_pc_mode_fr2_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr2_r16.pack(bref));
      }
      if (max_meas_srs_res_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, max_meas_srs_res_scg_r16, (uint8_t)0u, (uint8_t)32u));
      }
      if (max_meas_cli_res_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, max_meas_cli_res_scg_r16, (uint8_t)0u, (uint8_t)64u));
      }
      if (max_num_ehc_contexts_sn_r16_present) {
        HANDLE_CODE(pack_integer(bref, max_num_ehc_contexts_sn_r16, (uint32_t)0u, (uint32_t)65536u));
      }
      if (allowed_reduced_cfg_for_overheat_r16.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r16->pack(bref));
      }
      if (max_toffset_r16_present) {
        HANDLE_CODE(max_toffset_r16.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(allowed_reduced_cfg_for_overheat_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(max_num_udc_drb_r17_present, 1));
      HANDLE_CODE(bref.pack(max_num_cp_cc_andidates_r17_present, 1));
      if (allowed_reduced_cfg_for_overheat_r17.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r17->pack(bref));
      }
      if (max_num_udc_drb_r17_present) {
        HANDLE_CODE(pack_integer(bref, max_num_udc_drb_r17, (uint8_t)0u, (uint8_t)2u));
      }
      if (max_num_cp_cc_andidates_r17_present) {
        HANDLE_CODE(pack_integer(bref, max_num_cp_cc_andidates_r17, (uint8_t)0u, (uint8_t)7u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool allowed_bc_list_mrdc_present;
  HANDLE_CODE(bref.unpack(allowed_bc_list_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(pwr_coordination_fr1_present, 1));
  HANDLE_CODE(bref.unpack(serv_cell_idx_range_scg_present, 1));
  HANDLE_CODE(bref.unpack(max_meas_freqs_scg_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  if (allowed_bc_list_mrdc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_bc_list_mrdc, bref, 1, 65536));
  }
  if (pwr_coordination_fr1_present) {
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_nr_fr1_present, 1));
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_eutra_present, 1));
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_ue_fr1_present, 1));
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_nr_fr1, bref, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_eutra, bref, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_ue_fr1, bref, (int8_t)-30, (int8_t)33));
    }
  }
  if (serv_cell_idx_range_scg_present) {
    HANDLE_CODE(unpack_integer(serv_cell_idx_range_scg.low_bound, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(serv_cell_idx_range_scg.up_bound, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (max_meas_freqs_scg_present) {
    HANDLE_CODE(unpack_integer(max_meas_freqs_scg, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (dummy_present) {
    HANDLE_CODE(unpack_integer(dummy, bref, (uint8_t)1u, (uint8_t)62u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sel_band_entries_mn_list, bref);
      HANDLE_CODE(bref.unpack(pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.unpack(max_num_rohc_context_sessions_sn_present, 1));
      if (sel_band_entries_mn_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sel_band_entries_mn_list,
                                      bref,
                                      1,
                                      65536,
                                      SeqOfPacker<integer_packer<uint8_t>>(1, 32, integer_packer<uint8_t>(0, 32))));
      }
      if (pdcch_blind_detection_scg_present) {
        HANDLE_CODE(unpack_integer(pdcch_blind_detection_scg, bref, (uint8_t)1u, (uint8_t)15u));
      }
      if (max_num_rohc_context_sessions_sn_present) {
        HANDLE_CODE(unpack_integer(max_num_rohc_context_sessions_sn, bref, (uint16_t)0u, (uint16_t)16384u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_intra_freq_meas_identities_scg_present, 1));
      HANDLE_CODE(bref.unpack(max_inter_freq_meas_identities_scg_present, 1));
      if (max_intra_freq_meas_identities_scg_present) {
        HANDLE_CODE(unpack_integer(max_intra_freq_meas_identities_scg, bref, (uint8_t)1u, (uint8_t)62u));
      }
      if (max_inter_freq_meas_identities_scg_present) {
        HANDLE_CODE(unpack_integer(max_inter_freq_meas_identities_scg, bref, (uint8_t)1u, (uint8_t)62u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(p_max_nr_fr1_mcg_r16_present, 1));
      unpack_presence_flag(pwr_coordination_fr2_r16, bref);
      HANDLE_CODE(bref.unpack(nrdc_pc_mode_fr1_r16_present, 1));
      HANDLE_CODE(bref.unpack(nrdc_pc_mode_fr2_r16_present, 1));
      HANDLE_CODE(bref.unpack(max_meas_srs_res_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(max_meas_cli_res_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(max_num_ehc_contexts_sn_r16_present, 1));
      unpack_presence_flag(allowed_reduced_cfg_for_overheat_r16, bref);
      HANDLE_CODE(bref.unpack(max_toffset_r16_present, 1));
      if (p_max_nr_fr1_mcg_r16_present) {
        HANDLE_CODE(unpack_integer(p_max_nr_fr1_mcg_r16, bref, (int8_t)-30, (int8_t)33));
      }
      if (pwr_coordination_fr2_r16.is_present()) {
        HANDLE_CODE(bref.unpack(pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present, 1));
        HANDLE_CODE(bref.unpack(pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present, 1));
        HANDLE_CODE(bref.unpack(pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present, 1));
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present) {
          HANDLE_CODE(unpack_integer(pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16, bref, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present) {
          HANDLE_CODE(unpack_integer(pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16, bref, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present) {
          HANDLE_CODE(unpack_integer(pwr_coordination_fr2_r16->p_max_ue_fr2_r16, bref, (int8_t)-30, (int8_t)33));
        }
      }
      if (nrdc_pc_mode_fr1_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr1_r16.unpack(bref));
      }
      if (nrdc_pc_mode_fr2_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr2_r16.unpack(bref));
      }
      if (max_meas_srs_res_scg_r16_present) {
        HANDLE_CODE(unpack_integer(max_meas_srs_res_scg_r16, bref, (uint8_t)0u, (uint8_t)32u));
      }
      if (max_meas_cli_res_scg_r16_present) {
        HANDLE_CODE(unpack_integer(max_meas_cli_res_scg_r16, bref, (uint8_t)0u, (uint8_t)64u));
      }
      if (max_num_ehc_contexts_sn_r16_present) {
        HANDLE_CODE(unpack_integer(max_num_ehc_contexts_sn_r16, bref, (uint32_t)0u, (uint32_t)65536u));
      }
      if (allowed_reduced_cfg_for_overheat_r16.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r16->unpack(bref));
      }
      if (max_toffset_r16_present) {
        HANDLE_CODE(max_toffset_r16.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(allowed_reduced_cfg_for_overheat_r17, bref);
      HANDLE_CODE(bref.unpack(max_num_udc_drb_r17_present, 1));
      HANDLE_CODE(bref.unpack(max_num_cp_cc_andidates_r17_present, 1));
      if (allowed_reduced_cfg_for_overheat_r17.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r17->unpack(bref));
      }
      if (max_num_udc_drb_r17_present) {
        HANDLE_CODE(unpack_integer(max_num_udc_drb_r17, bref, (uint8_t)0u, (uint8_t)2u));
      }
      if (max_num_cp_cc_andidates_r17_present) {
        HANDLE_CODE(unpack_integer(max_num_cp_cc_andidates_r17, bref, (uint8_t)0u, (uint8_t)7u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cfg_restrict_info_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (allowed_bc_list_mrdc.size() > 0) {
    j.start_array("allowedBC-ListMRDC");
    for (const auto& e1 : allowed_bc_list_mrdc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pwr_coordination_fr1_present) {
    j.write_fieldname("powerCoordination-FR1");
    j.start_obj();
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      j.write_int("p-maxNR-FR1", pwr_coordination_fr1.p_max_nr_fr1);
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      j.write_int("p-maxEUTRA", pwr_coordination_fr1.p_max_eutra);
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      j.write_int("p-maxUE-FR1", pwr_coordination_fr1.p_max_ue_fr1);
    }
    j.end_obj();
  }
  if (serv_cell_idx_range_scg_present) {
    j.write_fieldname("servCellIndexRangeSCG");
    j.start_obj();
    j.write_int("lowBound", serv_cell_idx_range_scg.low_bound);
    j.write_int("upBound", serv_cell_idx_range_scg.up_bound);
    j.end_obj();
  }
  if (max_meas_freqs_scg_present) {
    j.write_int("maxMeasFreqsSCG", max_meas_freqs_scg);
  }
  if (dummy_present) {
    j.write_int("dummy", dummy);
  }
  if (ext) {
    if (sel_band_entries_mn_list.is_present()) {
      j.start_array("selectedBandEntriesMNList");
      for (const auto& e1 : *sel_band_entries_mn_list) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
    }
    if (pdcch_blind_detection_scg_present) {
      j.write_int("pdcch-BlindDetectionSCG", pdcch_blind_detection_scg);
    }
    if (max_num_rohc_context_sessions_sn_present) {
      j.write_int("maxNumberROHC-ContextSessionsSN", max_num_rohc_context_sessions_sn);
    }
    if (max_intra_freq_meas_identities_scg_present) {
      j.write_int("maxIntraFreqMeasIdentitiesSCG", max_intra_freq_meas_identities_scg);
    }
    if (max_inter_freq_meas_identities_scg_present) {
      j.write_int("maxInterFreqMeasIdentitiesSCG", max_inter_freq_meas_identities_scg);
    }
    if (p_max_nr_fr1_mcg_r16_present) {
      j.write_int("p-maxNR-FR1-MCG-r16", p_max_nr_fr1_mcg_r16);
    }
    if (pwr_coordination_fr2_r16.is_present()) {
      j.write_fieldname("powerCoordination-FR2-r16");
      j.start_obj();
      if (pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present) {
        j.write_int("p-maxNR-FR2-MCG-r16", pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16);
      }
      if (pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present) {
        j.write_int("p-maxNR-FR2-SCG-r16", pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16);
      }
      if (pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present) {
        j.write_int("p-maxUE-FR2-r16", pwr_coordination_fr2_r16->p_max_ue_fr2_r16);
      }
      j.end_obj();
    }
    if (nrdc_pc_mode_fr1_r16_present) {
      j.write_str("nrdc-PC-mode-FR1-r16", nrdc_pc_mode_fr1_r16.to_string());
    }
    if (nrdc_pc_mode_fr2_r16_present) {
      j.write_str("nrdc-PC-mode-FR2-r16", nrdc_pc_mode_fr2_r16.to_string());
    }
    if (max_meas_srs_res_scg_r16_present) {
      j.write_int("maxMeasSRS-ResourceSCG-r16", max_meas_srs_res_scg_r16);
    }
    if (max_meas_cli_res_scg_r16_present) {
      j.write_int("maxMeasCLI-ResourceSCG-r16", max_meas_cli_res_scg_r16);
    }
    if (max_num_ehc_contexts_sn_r16_present) {
      j.write_int("maxNumberEHC-ContextsSN-r16", max_num_ehc_contexts_sn_r16);
    }
    if (allowed_reduced_cfg_for_overheat_r16.is_present()) {
      j.write_fieldname("allowedReducedConfigForOverheating-r16");
      allowed_reduced_cfg_for_overheat_r16->to_json(j);
    }
    if (max_toffset_r16_present) {
      j.write_str("maxToffset-r16", max_toffset_r16.to_string());
    }
    if (allowed_reduced_cfg_for_overheat_r17.is_present()) {
      j.write_fieldname("allowedReducedConfigForOverheating-r17");
      allowed_reduced_cfg_for_overheat_r17->to_json(j);
    }
    if (max_num_udc_drb_r17_present) {
      j.write_int("maxNumberUDC-DRB-r17", max_num_udc_drb_r17);
    }
    if (max_num_cp_cc_andidates_r17_present) {
      j.write_int("maxNumberCPCCandidates-r17", max_num_cp_cc_andidates_r17);
    }
  }
  j.end_obj();
}

const char* cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(names, 3, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_e_");
}
uint8_t cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_e_");
}

const char* cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(names, 3, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_e_");
}
uint8_t cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_e_");
}

// VictimSystemType ::= SEQUENCE
SRSASN_CODE victim_sys_type_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gps_present, 1));
  HANDLE_CODE(bref.pack(glonass_present, 1));
  HANDLE_CODE(bref.pack(bds_present, 1));
  HANDLE_CODE(bref.pack(galileo_present, 1));
  HANDLE_CODE(bref.pack(wlan_present, 1));
  HANDLE_CODE(bref.pack(bluetooth_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE victim_sys_type_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gps_present, 1));
  HANDLE_CODE(bref.unpack(glonass_present, 1));
  HANDLE_CODE(bref.unpack(bds_present, 1));
  HANDLE_CODE(bref.unpack(galileo_present, 1));
  HANDLE_CODE(bref.unpack(wlan_present, 1));
  HANDLE_CODE(bref.unpack(bluetooth_present, 1));

  return SRSASN_SUCCESS;
}
void victim_sys_type_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gps_present) {
    j.write_str("gps", "true");
  }
  if (glonass_present) {
    j.write_str("glonass", "true");
  }
  if (bds_present) {
    j.write_str("bds", "true");
  }
  if (galileo_present) {
    j.write_str("galileo", "true");
  }
  if (wlan_present) {
    j.write_str("wlan", "true");
  }
  if (bluetooth_present) {
    j.write_str("bluetooth", "true");
  }
  j.end_obj();
}

// AffectedCarrierFreqCombInfoMRDC ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc_present, 1));

  HANDLE_CODE(victim_sys_type.pack(bref));
  HANDLE_CODE(interference_direction_mrdc.pack(bref));
  if (affected_carrier_freq_comb_mrdc_present) {
    HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0, 1));
    if (affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra,
                                  1,
                                  32,
                                  integer_packer<uint32_t>(0, 262143)));
    }
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr,
                                1,
                                32,
                                integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_mrdc_present, 1));

  HANDLE_CODE(victim_sys_type.unpack(bref));
  HANDLE_CODE(interference_direction_mrdc.unpack(bref));
  if (affected_carrier_freq_comb_mrdc_present) {
    bool affected_carrier_freq_comb_eutra_present;
    HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_eutra_present, 1));
    if (affected_carrier_freq_comb_eutra_present) {
      HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra,
                                    bref,
                                    1,
                                    32,
                                    integer_packer<uint32_t>(0, 262143)));
    }
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr,
                                  bref,
                                  1,
                                  32,
                                  integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_comb_info_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("victimSystemType");
  victim_sys_type.to_json(j);
  j.write_str("interferenceDirectionMRDC", interference_direction_mrdc.to_string());
  if (affected_carrier_freq_comb_mrdc_present) {
    j.write_fieldname("affectedCarrierFreqCombMRDC");
    j.start_obj();
    if (affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0) {
      j.start_array("affectedCarrierFreqCombEUTRA");
      for (const auto& e1 : affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra) {
        j.write_int(e1);
      }
      j.end_array();
    }
    j.start_array("affectedCarrierFreqCombNR");
    for (const auto& e1 : affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr) {
      j.write_int(e1);
    }
    j.end_array();
    j.end_obj();
  }
  j.end_obj();
}

const char* affected_carrier_freq_comb_info_mrdc_s::interference_direction_mrdc_opts::to_string() const
{
  static const char* names[] = {"eutra-nr", "nr", "other", "utra-nr-other", "nr-other", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "affected_carrier_freq_comb_info_mrdc_s::interference_direction_mrdc_e_");
}

// FrequencyConfig-NR-r16 ::= SEQUENCE
SRSASN_CODE freq_cfg_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, freq_band_ind_nr_r16, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(pack_integer(bref, carrier_center_freq_nr_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_integer(bref, carrier_bw_nr_r16, (uint16_t)1u, (uint16_t)275u));
  HANDLE_CODE(subcarrier_spacing_nr_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_cfg_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(freq_band_ind_nr_r16, bref, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(unpack_integer(carrier_center_freq_nr_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_integer(carrier_bw_nr_r16, bref, (uint16_t)1u, (uint16_t)275u));
  HANDLE_CODE(subcarrier_spacing_nr_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void freq_cfg_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("freqBandIndicatorNR-r16", freq_band_ind_nr_r16);
  j.write_int("carrierCenterFreq-NR-r16", carrier_center_freq_nr_r16);
  j.write_int("carrierBandwidth-NR-r16", carrier_bw_nr_r16);
  j.write_str("subcarrierSpacing-NR-r16", subcarrier_spacing_nr_r16.to_string());
  j.end_obj();
}

// TransmissionBandwidth-EUTRA-r16 ::= ENUMERATED
const char* tx_bw_eutra_r16_opts::to_string() const
{
  static const char* names[] = {"rb6", "rb15", "rb25", "rb50", "rb75", "rb100"};
  return convert_enum_idx(names, 6, value, "tx_bw_eutra_r16_e");
}
uint8_t tx_bw_eutra_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "tx_bw_eutra_r16_e");
}

// ServCellInfoXCG-EUTRA-r16 ::= SEQUENCE
SRSASN_CODE serv_cell_info_xcg_eutra_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(ul_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(tx_bw_eutra_r16_present, 1));

  if (dl_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(pack_integer(bref, dl_carrier_freq_eutra_r16, (uint32_t)0u, (uint32_t)262143u));
  }
  if (ul_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(pack_integer(bref, ul_carrier_freq_eutra_r16, (uint32_t)0u, (uint32_t)262143u));
  }
  if (tx_bw_eutra_r16_present) {
    HANDLE_CODE(tx_bw_eutra_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE serv_cell_info_xcg_eutra_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(ul_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(tx_bw_eutra_r16_present, 1));

  if (dl_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(unpack_integer(dl_carrier_freq_eutra_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (ul_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(unpack_integer(ul_carrier_freq_eutra_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (tx_bw_eutra_r16_present) {
    HANDLE_CODE(tx_bw_eutra_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void serv_cell_info_xcg_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_carrier_freq_eutra_r16_present) {
    j.write_int("dl-CarrierFreq-EUTRA-r16", dl_carrier_freq_eutra_r16);
  }
  if (ul_carrier_freq_eutra_r16_present) {
    j.write_int("ul-CarrierFreq-EUTRA-r16", ul_carrier_freq_eutra_r16);
  }
  if (tx_bw_eutra_r16_present) {
    j.write_str("transmissionBandwidth-EUTRA-r16", tx_bw_eutra_r16.to_string());
  }
  j.end_obj();
}

// ServCellInfoXCG-NR-r16 ::= SEQUENCE
SRSASN_CODE serv_cell_info_xcg_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_freq_info_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(ul_freq_info_nr_r16_present, 1));

  if (dl_freq_info_nr_r16_present) {
    HANDLE_CODE(dl_freq_info_nr_r16.pack(bref));
  }
  if (ul_freq_info_nr_r16_present) {
    HANDLE_CODE(ul_freq_info_nr_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE serv_cell_info_xcg_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_freq_info_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(ul_freq_info_nr_r16_present, 1));

  if (dl_freq_info_nr_r16_present) {
    HANDLE_CODE(dl_freq_info_nr_r16.unpack(bref));
  }
  if (ul_freq_info_nr_r16_present) {
    HANDLE_CODE(ul_freq_info_nr_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void serv_cell_info_xcg_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_freq_info_nr_r16_present) {
    j.write_fieldname("dl-FreqInfo-NR-r16");
    dl_freq_info_nr_r16.to_json(j);
  }
  if (ul_freq_info_nr_r16_present) {
    j.write_fieldname("ul-FreqInfo-NR-r16");
    ul_freq_info_nr_r16.to_json(j);
  }
  j.end_obj();
}

// DRX-Info2 ::= SEQUENCE
SRSASN_CODE drx_info2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(drx_on_dur_timer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(drx_on_dur_timer.unpack(bref));

  return SRSASN_SUCCESS;
}
void drx_info2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-onDurationTimer");
  drx_on_dur_timer.to_json(j);
  j.end_obj();
}

void drx_info2_s::drx_on_dur_timer_c_::destroy_() {}
void drx_info2_s::drx_on_dur_timer_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_info2_s::drx_on_dur_timer_c_::drx_on_dur_timer_c_(const drx_info2_s::drx_on_dur_timer_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sub_milli_seconds:
      c.init(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.init(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
  }
}
drx_info2_s::drx_on_dur_timer_c_&
drx_info2_s::drx_on_dur_timer_c_::operator=(const drx_info2_s::drx_on_dur_timer_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sub_milli_seconds:
      c.set(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.set(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
  }

  return *this;
}
uint8_t& drx_info2_s::drx_on_dur_timer_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
drx_info2_s::drx_on_dur_timer_c_::milli_seconds_e_& drx_info2_s::drx_on_dur_timer_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void drx_info2_s::drx_on_dur_timer_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sub_milli_seconds:
      j.write_int("subMilliSeconds", c.get<uint8_t>());
      break;
    case types::milli_seconds:
      j.write_str("milliSeconds", c.get<milli_seconds_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_info2_s::drx_on_dur_timer_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info2_s::drx_on_dur_timer_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_info2_s::drx_on_dur_timer_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_info2_s::drx_on_dur_timer_c_::milli_seconds_e_");
}
uint16_t drx_info2_s::drx_on_dur_timer_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 24, value, "drx_info2_s::drx_on_dur_timer_c_::milli_seconds_e_");
}

const char* drx_info2_s::drx_on_dur_timer_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "drx_info2_s::drx_on_dur_timer_c_::types");
}

// FR-Info ::= SEQUENCE
SRSASN_CODE fr_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(fr_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fr_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(fr_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void fr_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_str("fr-Type", fr_type.to_string());
  j.end_obj();
}

const char* fr_info_s::fr_type_opts::to_string() const
{
  static const char* names[] = {"fr1", "fr2"};
  return convert_enum_idx(names, 2, value, "fr_info_s::fr_type_e_");
}
uint8_t fr_info_s::fr_type_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "fr_info_s::fr_type_e_");
}

// NR-FreqInfo ::= SEQUENCE
SRSASN_CODE nr_freq_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_freq_present, 1));

  if (measured_freq_present) {
    HANDLE_CODE(pack_integer(bref, measured_freq, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_freq_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(measured_freq_present, 1));

  if (measured_freq_present) {
    HANDLE_CODE(unpack_integer(measured_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
void nr_freq_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_freq_present) {
    j.write_int("measuredFrequency", measured_freq);
  }
  j.end_obj();
}

// DRX-Info ::= SEQUENCE
SRSASN_CODE drx_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(short_drx_present, 1));

  HANDLE_CODE(drx_long_cycle_start_offset.pack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.pack(bref));
    HANDLE_CODE(pack_integer(bref, short_drx.drx_short_cycle_timer, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(short_drx_present, 1));

  HANDLE_CODE(drx_long_cycle_start_offset.unpack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.unpack(bref));
    HANDLE_CODE(unpack_integer(short_drx.drx_short_cycle_timer, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void drx_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-LongCycleStartOffset");
  drx_long_cycle_start_offset.to_json(j);
  if (short_drx_present) {
    j.write_fieldname("shortDRX");
    j.start_obj();
    j.write_str("drx-ShortCycle", short_drx.drx_short_cycle.to_string());
    j.write_int("drx-ShortCycleTimer", short_drx.drx_short_cycle_timer);
    j.end_obj();
  }
  j.end_obj();
}

void drx_info_s::drx_long_cycle_start_offset_c_::destroy_() {}
void drx_info_s::drx_long_cycle_start_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_info_s::drx_long_cycle_start_offset_c_::drx_long_cycle_start_offset_c_(
    const drx_info_s::drx_long_cycle_start_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }
}
drx_info_s::drx_long_cycle_start_offset_c_&
drx_info_s::drx_long_cycle_start_offset_c_::operator=(const drx_info_s::drx_long_cycle_start_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }

  return *this;
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
void drx_info_s::drx_long_cycle_start_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms10:
      j.write_int("ms10", c.get<uint8_t>());
      break;
    case types::ms20:
      j.write_int("ms20", c.get<uint8_t>());
      break;
    case types::ms32:
      j.write_int("ms32", c.get<uint8_t>());
      break;
    case types::ms40:
      j.write_int("ms40", c.get<uint8_t>());
      break;
    case types::ms60:
      j.write_int("ms60", c.get<uint8_t>());
      break;
    case types::ms64:
      j.write_int("ms64", c.get<uint8_t>());
      break;
    case types::ms70:
      j.write_int("ms70", c.get<uint8_t>());
      break;
    case types::ms80:
      j.write_int("ms80", c.get<uint8_t>());
      break;
    case types::ms128:
      j.write_int("ms128", c.get<uint8_t>());
      break;
    case types::ms160:
      j.write_int("ms160", c.get<uint8_t>());
      break;
    case types::ms256:
      j.write_int("ms256", c.get<uint16_t>());
      break;
    case types::ms320:
      j.write_int("ms320", c.get<uint16_t>());
      break;
    case types::ms512:
      j.write_int("ms512", c.get<uint16_t>());
      break;
    case types::ms640:
      j.write_int("ms640", c.get<uint16_t>());
      break;
    case types::ms1024:
      j.write_int("ms1024", c.get<uint16_t>());
      break;
    case types::ms1280:
      j.write_int("ms1280", c.get<uint16_t>());
      break;
    case types::ms2048:
      j.write_int("ms2048", c.get<uint16_t>());
      break;
    case types::ms2560:
      j.write_int("ms2560", c.get<uint16_t>());
      break;
    case types::ms5120:
      j.write_int("ms5120", c.get<uint16_t>());
      break;
    case types::ms10240:
      j.write_int("ms10240", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_info_s::drx_long_cycle_start_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info_s::drx_long_cycle_start_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_info_s::drx_long_cycle_start_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "drx_info_s::drx_long_cycle_start_offset_c_::types");
}
uint16_t drx_info_s::drx_long_cycle_start_offset_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "drx_info_s::drx_long_cycle_start_offset_c_::types");
}

const char* drx_info_s::short_drx_s_::drx_short_cycle_opts::to_string() const
{
  static const char* names[] = {"ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms7",    "ms8",    "ms10",
                                "ms14",   "ms16",   "ms20",   "ms30",   "ms32",   "ms35",   "ms40",   "ms64",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",  "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_info_s::short_drx_s_::drx_short_cycle_e_");
}
uint16_t drx_info_s::short_drx_s_::drx_short_cycle_opts::to_number() const
{
  static const uint16_t numbers[] = {2,  3,  4,  5,  6,  7,   8,   10,  14,  16,  20, 30,
                                     32, 35, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(numbers, 23, value, "drx_info_s::short_drx_s_::drx_short_cycle_e_");
}

// CandidateCellCPC-r17 ::= SEQUENCE
SRSASN_CODE candidate_cell_cp_c_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ssb_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_list_r17, 1, 8, integer_packer<uint16_t>(0, 1007)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_cell_cp_c_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ssb_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_list_r17, bref, 1, 8, integer_packer<uint16_t>(0, 1007)));

  return SRSASN_SUCCESS;
}
void candidate_cell_cp_c_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssbFrequency-r17", ssb_freq_r17);
  j.start_array("candidateCellList-r17");
  for (const auto& e1 : candidate_cell_list_r17) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// CG-ConfigInfo-v1730-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1730_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fr1_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.pack(fr2_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fr1_carriers_mcg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr1_carriers_mcg_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_mcg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr2_carriers_mcg_r17, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1730_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fr1_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.unpack(fr2_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fr1_carriers_mcg_r17_present) {
    HANDLE_CODE(unpack_integer(fr1_carriers_mcg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_mcg_r17_present) {
    HANDLE_CODE(unpack_integer(fr2_carriers_mcg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1730_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fr1_carriers_mcg_r17_present) {
    j.write_int("fr1-Carriers-MCG-r17", fr1_carriers_mcg_r17);
  }
  if (fr2_carriers_mcg_r17_present) {
    j.write_int("fr2-Carriers-MCG-r17", fr2_carriers_mcg_r17);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CG-ConfigInfo-v1700-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(candidate_cell_list_cp_c_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(two_phr_mode_mcg_r17_present, 1));
  HANDLE_CODE(bref.pack(low_mob_eval_connected_in_pcell_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (candidate_cell_list_cp_c_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_list_cp_c_r17, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool candidate_cell_list_cp_c_r17_present;
  HANDLE_CODE(bref.unpack(candidate_cell_list_cp_c_r17_present, 1));
  HANDLE_CODE(bref.unpack(two_phr_mode_mcg_r17_present, 1));
  HANDLE_CODE(bref.unpack(low_mob_eval_connected_in_pcell_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (candidate_cell_list_cp_c_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_list_cp_c_r17, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_cell_list_cp_c_r17.size() > 0) {
    j.start_array("candidateCellListCPC-r17");
    for (const auto& e1 : candidate_cell_list_cp_c_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (two_phr_mode_mcg_r17_present) {
    j.write_str("twoPHRModeMCG-r17", "enabled");
  }
  if (low_mob_eval_connected_in_pcell_r17_present) {
    j.write_str("lowMobilityEvaluationConnectedInPCell-r17", "enabled");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-ConfigInfo-v1640-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1640_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_cell_info_list_mcg_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(serv_cell_info_list_mcg_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (serv_cell_info_list_mcg_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_mcg_nr_r16, 1, 32));
  }
  if (serv_cell_info_list_mcg_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_mcg_eutra_r16, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1640_ies_s::unpack(cbit_ref& bref)
{
  bool serv_cell_info_list_mcg_nr_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_mcg_nr_r16_present, 1));
  bool serv_cell_info_list_mcg_eutra_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_mcg_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (serv_cell_info_list_mcg_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_mcg_nr_r16, bref, 1, 32));
  }
  if (serv_cell_info_list_mcg_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_mcg_eutra_r16, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1640_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_cell_info_list_mcg_nr_r16.size() > 0) {
    j.start_array("servCellInfoListMCG-NR-r16");
    for (const auto& e1 : serv_cell_info_list_mcg_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (serv_cell_info_list_mcg_eutra_r16.size() > 0) {
    j.start_array("servCellInfoListMCG-EUTRA-r16");
    for (const auto& e1 : serv_cell_info_list_mcg_eutra_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-ConfigInfo-v1620-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1620_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_assist_info_source_scg_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_assist_info_source_scg_r16.size() > 0) {
    HANDLE_CODE(ue_assist_info_source_scg_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1620_ies_s::unpack(cbit_ref& bref)
{
  bool ue_assist_info_source_scg_r16_present;
  HANDLE_CODE(bref.unpack(ue_assist_info_source_scg_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_assist_info_source_scg_r16_present) {
    HANDLE_CODE(ue_assist_info_source_scg_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1620_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_assist_info_source_scg_r16.size() > 0) {
    j.write_str("ueAssistanceInformationSourceSCG-r16", ue_assist_info_source_scg_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-ConfigInfo-v1610-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_info_mcg2_present, 1));
  HANDLE_CODE(bref.pack(aligned_drx_ind_present, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_r16_present, 1));
  HANDLE_CODE(bref.pack(dummy1_present, 1));
  HANDLE_CODE(bref.pack(sidelink_ue_info_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sidelink_ue_info_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (drx_info_mcg2_present) {
    HANDLE_CODE(drx_info_mcg2.pack(bref));
  }
  if (scg_fail_info_r16_present) {
    HANDLE_CODE(scg_fail_info_r16.fail_type_r16.pack(bref));
    HANDLE_CODE(scg_fail_info_r16.meas_result_scg_r16.pack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.fail_type_eutra_r16.pack(bref));
    HANDLE_CODE(dummy1.meas_result_scg_eutra_r16.pack(bref));
  }
  if (sidelink_ue_info_nr_r16.size() > 0) {
    HANDLE_CODE(sidelink_ue_info_nr_r16.pack(bref));
  }
  if (sidelink_ue_info_eutra_r16.size() > 0) {
    HANDLE_CODE(sidelink_ue_info_eutra_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_info_mcg2_present, 1));
  HANDLE_CODE(bref.unpack(aligned_drx_ind_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(dummy1_present, 1));
  bool sidelink_ue_info_nr_r16_present;
  HANDLE_CODE(bref.unpack(sidelink_ue_info_nr_r16_present, 1));
  bool sidelink_ue_info_eutra_r16_present;
  HANDLE_CODE(bref.unpack(sidelink_ue_info_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (drx_info_mcg2_present) {
    HANDLE_CODE(drx_info_mcg2.unpack(bref));
  }
  if (scg_fail_info_r16_present) {
    HANDLE_CODE(scg_fail_info_r16.fail_type_r16.unpack(bref));
    HANDLE_CODE(scg_fail_info_r16.meas_result_scg_r16.unpack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.fail_type_eutra_r16.unpack(bref));
    HANDLE_CODE(dummy1.meas_result_scg_eutra_r16.unpack(bref));
  }
  if (sidelink_ue_info_nr_r16_present) {
    HANDLE_CODE(sidelink_ue_info_nr_r16.unpack(bref));
  }
  if (sidelink_ue_info_eutra_r16_present) {
    HANDLE_CODE(sidelink_ue_info_eutra_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drx_info_mcg2_present) {
    j.write_fieldname("drx-InfoMCG2");
    drx_info_mcg2.to_json(j);
  }
  if (aligned_drx_ind_present) {
    j.write_str("alignedDRX-Indication", "true");
  }
  if (scg_fail_info_r16_present) {
    j.write_fieldname("scgFailureInfo-r16");
    j.start_obj();
    j.write_str("failureType-r16", scg_fail_info_r16.fail_type_r16.to_string());
    j.write_str("measResultSCG-r16", scg_fail_info_r16.meas_result_scg_r16.to_string());
    j.end_obj();
  }
  if (dummy1_present) {
    j.write_fieldname("dummy1");
    j.start_obj();
    j.write_str("failureTypeEUTRA-r16", dummy1.fail_type_eutra_r16.to_string());
    j.write_str("measResultSCG-EUTRA-r16", dummy1.meas_result_scg_eutra_r16.to_string());
    j.end_obj();
  }
  if (sidelink_ue_info_nr_r16.size() > 0) {
    j.write_str("sidelinkUEInformationNR-r16", sidelink_ue_info_nr_r16.to_string());
  }
  if (sidelink_ue_info_eutra_r16.size() > 0) {
    j.write_str("sidelinkUEInformationEUTRA-r16", sidelink_ue_info_eutra_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_opts::to_string() const
{
  static const char* names[] = {"scg-lbtFailure-r16",
                                "beamFailureRecoveryFailure-r16",
                                "t312-Expiry-r16",
                                "bh-RLF-r16",
                                "beamFailure-r17",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_e_");
}
uint16_t cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_opts::to_number() const
{
  if (value == t312_expiry_r16) {
    return 312;
  }
  invalid_enum_number(value, "cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_e_");
  return 0;
}

const char* cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_opts::to_string() const
{
  static const char* names[] = {"scg-lbtFailure-r16",
                                "beamFailureRecoveryFailure-r16",
                                "t312-Expiry-r16",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_e_");
}
uint16_t cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_opts::to_number() const
{
  if (value == t312_expiry_r16) {
    return 312;
  }
  invalid_enum_number(value, "cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_e_");
  return 0;
}

// CG-ConfigInfo-v1590-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_frequencies_mn_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (serv_frequencies_mn_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_frequencies_mn_nr, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool serv_frequencies_mn_nr_present;
  HANDLE_CODE(bref.unpack(serv_frequencies_mn_nr_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (serv_frequencies_mn_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_frequencies_mn_nr, bref, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_frequencies_mn_nr.size() > 0) {
    j.start_array("servFrequenciesMN-NR");
    for (const auto& e1 : serv_frequencies_mn_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-UplinkCarrierMCG ::= SEQUENCE
SRSASN_CODE ph_ul_carrier_mcg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ph_type1or3.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_ul_carrier_mcg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ph_type1or3.unpack(bref));

  return SRSASN_SUCCESS;
}
void ph_ul_carrier_mcg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ph-Type1or3", ph_type1or3.to_string());
  j.end_obj();
}

const char* ph_ul_carrier_mcg_s::ph_type1or3_opts::to_string() const
{
  static const char* names[] = {"type1", "type3"};
  return convert_enum_idx(names, 2, value, "ph_ul_carrier_mcg_s::ph_type1or3_e_");
}
uint8_t ph_ul_carrier_mcg_s::ph_type1or3_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 3};
  return map_enum_number(numbers, 2, value, "ph_ul_carrier_mcg_s::ph_type1or3_e_");
}

// CG-ConfigInfo-v1570-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1570_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sftd_freq_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(sftd_freq_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sftd_freq_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sftd_freq_list_nr, 1, 3, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sftd_freq_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sftd_freq_list_eutra, 1, 3, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1570_ies_s::unpack(cbit_ref& bref)
{
  bool sftd_freq_list_nr_present;
  HANDLE_CODE(bref.unpack(sftd_freq_list_nr_present, 1));
  bool sftd_freq_list_eutra_present;
  HANDLE_CODE(bref.unpack(sftd_freq_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sftd_freq_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sftd_freq_list_nr, bref, 1, 3, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sftd_freq_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sftd_freq_list_eutra, bref, 1, 3, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1570_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sftd_freq_list_nr.size() > 0) {
    j.start_array("sftdFrequencyList-NR");
    for (const auto& e1 : sftd_freq_list_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sftd_freq_list_eutra.size() > 0) {
    j.start_array("sftdFrequencyList-EUTRA");
    for (const auto& e1 : sftd_freq_list_eutra) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-InfoMCG ::= SEQUENCE
SRSASN_CODE ph_info_mcg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.pack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= two_srs_pusch_repeat_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_info_mcg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.unpack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void ph_info_mcg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_fieldname("ph-Uplink");
  ph_ul.to_json(j);
  if (ph_supplementary_ul_present) {
    j.write_fieldname("ph-SupplementaryUplink");
    ph_supplementary_ul.to_json(j);
  }
  if (ext) {
    if (two_srs_pusch_repeat_r17_present) {
      j.write_str("twoSRS-PUSCH-Repetition-r17", "enabled");
    }
  }
  j.end_obj();
}

// CG-ConfigInfo-v1560-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(candidate_cell_info_list_mn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(source_cfg_scg_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_eutra_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_mcg_present, 1));
  HANDLE_CODE(bref.pack(meas_result_report_cgi_eutra_present, 1));
  HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(fr_info_list_mcg.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_mn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_mn_eutra.pack(bref));
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.pack(bref));
  }
  if (source_cfg_scg_eutra.size() > 0) {
    HANDLE_CODE(source_cfg_scg_eutra.pack(bref));
  }
  if (scg_fail_info_eutra_present) {
    HANDLE_CODE(scg_fail_info_eutra.fail_type_eutra.pack(bref));
    HANDLE_CODE(scg_fail_info_eutra.meas_result_scg_eutra.pack(bref));
  }
  if (drx_cfg_mcg_present) {
    HANDLE_CODE(drx_cfg_mcg.pack(bref));
  }
  if (meas_result_report_cgi_eutra_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi_eutra.eutra_freq, (uint32_t)0u, (uint32_t)262143u));
    HANDLE_CODE(pack_integer(
        bref, meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(meas_result_report_cgi_eutra.cgi_info_eutra.pack(bref));
  }
  if (meas_result_cell_list_sftd_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_cell_list_sftd_eutra, 1, 3));
  }
  if (fr_info_list_mcg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, fr_info_list_mcg, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1560_ies_s::unpack(cbit_ref& bref)
{
  bool candidate_cell_info_list_mn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_mn_eutra_present, 1));
  bool candidate_cell_info_list_sn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_eutra_present, 1));
  bool source_cfg_scg_eutra_present;
  HANDLE_CODE(bref.unpack(source_cfg_scg_eutra_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_eutra_present, 1));
  HANDLE_CODE(bref.unpack(drx_cfg_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_report_cgi_eutra_present, 1));
  bool meas_result_cell_list_sftd_eutra_present;
  HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_eutra_present, 1));
  bool fr_info_list_mcg_present;
  HANDLE_CODE(bref.unpack(fr_info_list_mcg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_mn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_mn_eutra.unpack(bref));
  }
  if (candidate_cell_info_list_sn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.unpack(bref));
  }
  if (source_cfg_scg_eutra_present) {
    HANDLE_CODE(source_cfg_scg_eutra.unpack(bref));
  }
  if (scg_fail_info_eutra_present) {
    HANDLE_CODE(scg_fail_info_eutra.fail_type_eutra.unpack(bref));
    HANDLE_CODE(scg_fail_info_eutra.meas_result_scg_eutra.unpack(bref));
  }
  if (drx_cfg_mcg_present) {
    HANDLE_CODE(drx_cfg_mcg.unpack(bref));
  }
  if (meas_result_report_cgi_eutra_present) {
    HANDLE_CODE(unpack_integer(meas_result_report_cgi_eutra.eutra_freq, bref, (uint32_t)0u, (uint32_t)262143u));
    HANDLE_CODE(unpack_integer(
        meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(meas_result_report_cgi_eutra.cgi_info_eutra.unpack(bref));
  }
  if (meas_result_cell_list_sftd_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_cell_list_sftd_eutra, bref, 1, 3));
  }
  if (fr_info_list_mcg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(fr_info_list_mcg, bref, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_cell_info_list_mn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListMN-EUTRA", candidate_cell_info_list_mn_eutra.to_string());
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListSN-EUTRA", candidate_cell_info_list_sn_eutra.to_string());
  }
  if (source_cfg_scg_eutra.size() > 0) {
    j.write_str("sourceConfigSCG-EUTRA", source_cfg_scg_eutra.to_string());
  }
  if (scg_fail_info_eutra_present) {
    j.write_fieldname("scgFailureInfoEUTRA");
    j.start_obj();
    j.write_str("failureTypeEUTRA", scg_fail_info_eutra.fail_type_eutra.to_string());
    j.write_str("measResultSCG-EUTRA", scg_fail_info_eutra.meas_result_scg_eutra.to_string());
    j.end_obj();
  }
  if (drx_cfg_mcg_present) {
    j.write_fieldname("drx-ConfigMCG");
    drx_cfg_mcg.to_json(j);
  }
  if (meas_result_report_cgi_eutra_present) {
    j.write_fieldname("measResultReportCGI-EUTRA");
    j.start_obj();
    j.write_int("eutraFrequency", meas_result_report_cgi_eutra.eutra_freq);
    j.write_int("cellForWhichToReportCGI-EUTRA", meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra);
    j.write_fieldname("cgi-InfoEUTRA");
    meas_result_report_cgi_eutra.cgi_info_eutra.to_json(j);
    j.end_obj();
  }
  if (meas_result_cell_list_sftd_eutra.size() > 0) {
    j.start_array("measResultCellListSFTD-EUTRA");
    for (const auto& e1 : meas_result_cell_list_sftd_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (fr_info_list_mcg.size() > 0) {
    j.start_array("fr-InfoListMCG");
    for (const auto& e1 : fr_info_list_mcg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_opts::to_string() const
{
  static const char* names[] = {"t313-Expiry", "randomAccessProblem", "rlc-MaxNumRetx", "scg-ChangeFailure"};
  return convert_enum_idx(names, 4, value, "cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_e_");
}
uint16_t cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_opts::to_number() const
{
  static const uint16_t numbers[] = {313};
  return map_enum_number(numbers, 1, value, "cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_e_");
}

// CG-ConfigInfo-v1540-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ph_info_mcg.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_report_cgi_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ph_info_mcg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ph_info_mcg, 1, 32));
  }
  if (meas_result_report_cgi_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi.ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi.cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)1007u));
    HANDLE_CODE(meas_result_report_cgi.cgi_info.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1540_ies_s::unpack(cbit_ref& bref)
{
  bool ph_info_mcg_present;
  HANDLE_CODE(bref.unpack(ph_info_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_report_cgi_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ph_info_mcg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ph_info_mcg, bref, 1, 32));
  }
  if (meas_result_report_cgi_present) {
    HANDLE_CODE(unpack_integer(meas_result_report_cgi.ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(
        unpack_integer(meas_result_report_cgi.cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)1007u));
    HANDLE_CODE(meas_result_report_cgi.cgi_info.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ph_info_mcg.size() > 0) {
    j.start_array("ph-InfoMCG");
    for (const auto& e1 : ph_info_mcg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_report_cgi_present) {
    j.write_fieldname("measResultReportCGI");
    j.start_obj();
    j.write_int("ssbFrequency", meas_result_report_cgi.ssb_freq);
    j.write_int("cellForWhichToReportCGI", meas_result_report_cgi.cell_for_which_to_report_cgi);
    j.write_fieldname("cgi-Info");
    meas_result_report_cgi.cgi_info.to_json(j);
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MRDC-AssistanceInfo ::= SEQUENCE
SRSASN_CODE mrdc_assist_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_comb_info_list_mrdc, 1, 128));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= overheat_assist_scg_r16.size() > 0;
    group_flags[1] |= overheat_assist_scg_fr2_2_r17.size() > 0;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(overheat_assist_scg_r16.size() > 0, 1));
      if (overheat_assist_scg_r16.size() > 0) {
        HANDLE_CODE(overheat_assist_scg_r16.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(overheat_assist_scg_fr2_2_r17.size() > 0, 1));
      if (overheat_assist_scg_fr2_2_r17.size() > 0) {
        HANDLE_CODE(overheat_assist_scg_fr2_2_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_assist_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_info_list_mrdc, bref, 1, 128));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool overheat_assist_scg_r16_present;
      HANDLE_CODE(bref.unpack(overheat_assist_scg_r16_present, 1));
      if (overheat_assist_scg_r16_present) {
        HANDLE_CODE(overheat_assist_scg_r16.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool overheat_assist_scg_fr2_2_r17_present;
      HANDLE_CODE(bref.unpack(overheat_assist_scg_fr2_2_r17_present, 1));
      if (overheat_assist_scg_fr2_2_r17_present) {
        HANDLE_CODE(overheat_assist_scg_fr2_2_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mrdc_assist_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("affectedCarrierFreqCombInfoListMRDC");
  for (const auto& e1 : affected_carrier_freq_comb_info_list_mrdc) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (overheat_assist_scg_r16.size() > 0) {
      j.write_str("overheatingAssistanceSCG-r16", overheat_assist_scg_r16.to_string());
    }
    if (overheat_assist_scg_fr2_2_r17.size() > 0) {
      j.write_str("overheatingAssistanceSCG-FR2-2-r17", overheat_assist_scg_fr2_2_r17.to_string());
    }
  }
  j.end_obj();
}

// MeasConfigMN ::= SEQUENCE
SRSASN_CODE meas_cfg_mn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_frequencies_mn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.pack(gap_purpose_present, 1));

  if (measured_frequencies_mn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, measured_frequencies_mn, 1, 32));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.pack(bref));
  }
  if (gap_purpose_present) {
    HANDLE_CODE(gap_purpose.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_gap_cfg_fr2.is_present();
    group_flags[1] |= inter_freq_no_gap_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_gap_cfg_fr2.is_present(), 1));
      if (meas_gap_cfg_fr2.is_present()) {
        HANDLE_CODE(meas_gap_cfg_fr2->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_no_gap_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_mn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool measured_frequencies_mn_present;
  HANDLE_CODE(bref.unpack(measured_frequencies_mn_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.unpack(gap_purpose_present, 1));

  if (measured_frequencies_mn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(measured_frequencies_mn, bref, 1, 32));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.unpack(bref));
  }
  if (gap_purpose_present) {
    HANDLE_CODE(gap_purpose.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_gap_cfg_fr2, bref);
      if (meas_gap_cfg_fr2.is_present()) {
        HANDLE_CODE(meas_gap_cfg_fr2->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(inter_freq_no_gap_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cfg_mn_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_frequencies_mn.size() > 0) {
    j.start_array("measuredFrequenciesMN");
    for (const auto& e1 : measured_frequencies_mn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_gap_cfg_present) {
    j.write_fieldname("measGapConfig");
    meas_gap_cfg.to_json(j);
  }
  if (gap_purpose_present) {
    j.write_str("gapPurpose", gap_purpose.to_string());
  }
  if (ext) {
    if (meas_gap_cfg_fr2.is_present()) {
      j.write_fieldname("measGapConfigFR2");
      meas_gap_cfg_fr2->to_json(j);
    }
    if (inter_freq_no_gap_r16_present) {
      j.write_str("interFreqNoGap-r16", "true");
    }
  }
  j.end_obj();
}

const char* meas_cfg_mn_s::gap_purpose_opts::to_string() const
{
  static const char* names[] = {"perUE", "perFR1"};
  return convert_enum_idx(names, 2, value, "meas_cfg_mn_s::gap_purpose_e_");
}
uint8_t meas_cfg_mn_s::gap_purpose_opts::to_number() const
{
  if (value == per_fr1) {
    return 1;
  }
  invalid_enum_number(value, "meas_cfg_mn_s::gap_purpose_e_");
  return 0;
}

// CG-ConfigInfo-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_cap_info.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_mn.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_present, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_info_present, 1));
  HANDLE_CODE(bref.pack(drx_info_mcg_present, 1));
  HANDLE_CODE(bref.pack(meas_cfg_mn_present, 1));
  HANDLE_CODE(bref.pack(source_cfg_scg.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(mcg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(mrdc_assist_info_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_cap_info.size() > 0) {
    HANDLE_CODE(ue_cap_info.pack(bref));
  }
  if (candidate_cell_info_list_mn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_info_list_mn, 1, 8));
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn.pack(bref));
  }
  if (meas_result_cell_list_sftd_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_cell_list_sftd_nr, 1, 3));
  }
  if (scg_fail_info_present) {
    HANDLE_CODE(scg_fail_info.fail_type.pack(bref));
    HANDLE_CODE(scg_fail_info.meas_result_scg.pack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.pack(bref));
  }
  if (drx_info_mcg_present) {
    HANDLE_CODE(drx_info_mcg.pack(bref));
  }
  if (meas_cfg_mn_present) {
    HANDLE_CODE(meas_cfg_mn.pack(bref));
  }
  if (source_cfg_scg.size() > 0) {
    HANDLE_CODE(source_cfg_scg.pack(bref));
  }
  if (scg_rb_cfg.size() > 0) {
    HANDLE_CODE(scg_rb_cfg.pack(bref));
  }
  if (mcg_rb_cfg.size() > 0) {
    HANDLE_CODE(mcg_rb_cfg.pack(bref));
  }
  if (mrdc_assist_info_present) {
    HANDLE_CODE(mrdc_assist_info.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_ies_s::unpack(cbit_ref& bref)
{
  bool ue_cap_info_present;
  HANDLE_CODE(bref.unpack(ue_cap_info_present, 1));
  bool candidate_cell_info_list_mn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_mn_present, 1));
  bool candidate_cell_info_list_sn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_present, 1));
  bool meas_result_cell_list_sftd_nr_present;
  HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_nr_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_info_present, 1));
  HANDLE_CODE(bref.unpack(drx_info_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_mn_present, 1));
  bool source_cfg_scg_present;
  HANDLE_CODE(bref.unpack(source_cfg_scg_present, 1));
  bool scg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(scg_rb_cfg_present, 1));
  bool mcg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(mcg_rb_cfg_present, 1));
  HANDLE_CODE(bref.unpack(mrdc_assist_info_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_cap_info_present) {
    HANDLE_CODE(ue_cap_info.unpack(bref));
  }
  if (candidate_cell_info_list_mn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_info_list_mn, bref, 1, 8));
  }
  if (candidate_cell_info_list_sn_present) {
    HANDLE_CODE(candidate_cell_info_list_sn.unpack(bref));
  }
  if (meas_result_cell_list_sftd_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_cell_list_sftd_nr, bref, 1, 3));
  }
  if (scg_fail_info_present) {
    HANDLE_CODE(scg_fail_info.fail_type.unpack(bref));
    HANDLE_CODE(scg_fail_info.meas_result_scg.unpack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.unpack(bref));
  }
  if (drx_info_mcg_present) {
    HANDLE_CODE(drx_info_mcg.unpack(bref));
  }
  if (meas_cfg_mn_present) {
    HANDLE_CODE(meas_cfg_mn.unpack(bref));
  }
  if (source_cfg_scg_present) {
    HANDLE_CODE(source_cfg_scg.unpack(bref));
  }
  if (scg_rb_cfg_present) {
    HANDLE_CODE(scg_rb_cfg.unpack(bref));
  }
  if (mcg_rb_cfg_present) {
    HANDLE_CODE(mcg_rb_cfg.unpack(bref));
  }
  if (mrdc_assist_info_present) {
    HANDLE_CODE(mrdc_assist_info.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_cap_info.size() > 0) {
    j.write_str("ue-CapabilityInfo", ue_cap_info.to_string());
  }
  if (candidate_cell_info_list_mn.size() > 0) {
    j.start_array("candidateCellInfoListMN");
    for (const auto& e1 : candidate_cell_info_list_mn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    j.write_str("candidateCellInfoListSN", candidate_cell_info_list_sn.to_string());
  }
  if (meas_result_cell_list_sftd_nr.size() > 0) {
    j.start_array("measResultCellListSFTD-NR");
    for (const auto& e1 : meas_result_cell_list_sftd_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scg_fail_info_present) {
    j.write_fieldname("scgFailureInfo");
    j.start_obj();
    j.write_str("failureType", scg_fail_info.fail_type.to_string());
    j.write_str("measResultSCG", scg_fail_info.meas_result_scg.to_string());
    j.end_obj();
  }
  if (cfg_restrict_info_present) {
    j.write_fieldname("configRestrictInfo");
    cfg_restrict_info.to_json(j);
  }
  if (drx_info_mcg_present) {
    j.write_fieldname("drx-InfoMCG");
    drx_info_mcg.to_json(j);
  }
  if (meas_cfg_mn_present) {
    j.write_fieldname("measConfigMN");
    meas_cfg_mn.to_json(j);
  }
  if (source_cfg_scg.size() > 0) {
    j.write_str("sourceConfigSCG", source_cfg_scg.to_string());
  }
  if (scg_rb_cfg.size() > 0) {
    j.write_str("scg-RB-Config", scg_rb_cfg.to_string());
  }
  if (mcg_rb_cfg.size() > 0) {
    j.write_str("mcg-RB-Config", mcg_rb_cfg.to_string());
  }
  if (mrdc_assist_info_present) {
    j.write_fieldname("mrdc-AssistanceInfo");
    mrdc_assist_info.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_opts::to_string() const
{
  static const char* names[] = {"t310-Expiry",
                                "randomAccessProblem",
                                "rlc-MaxNumRetx",
                                "synchReconfigFailure-SCG",
                                "scg-reconfigFailure",
                                "srb3-IntegrityFailure"};
  return convert_enum_idx(names, 6, value, "cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_e_");
}
uint16_t cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case srb3_integrity_fail:
      return 3;
    default:
      invalid_enum_number(value, "cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_e_");
  }
  return 0;
}

// CG-ConfigInfo ::= SEQUENCE
SRSASN_CODE cg_cfg_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_cfg_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void cg_cfg_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_info_s::crit_exts_c_::c1_c_& cg_cfg_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_cfg_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void cg_cfg_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cg_cfg_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_info_ies_s& cg_cfg_info_s::crit_exts_c_::c1_c_::set_cg_cfg_info()
{
  set(types::cg_cfg_info);
  return c;
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cg_cfg_info:
      j.write_fieldname("cg-ConfigInfo");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cg_cfg_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cg_cfg_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cg_cfg_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"cg-ConfigInfo", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "cg_cfg_info_s::crit_exts_c_::c1_c_::types");
}

const char* cg_cfg_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "cg_cfg_info_s::crit_exts_c_::types");
}
uint8_t cg_cfg_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_cfg_info_s::crit_exts_c_::types");
}

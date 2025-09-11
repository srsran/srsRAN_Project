/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/asn1/e2sm/e2sm_ccc.h"

using namespace asn1;
using namespace asn1::e2sm_ccc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PlmnId ::= SEQUENCE
SRSASN_CODE plmn_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcc_present, 1));
  HANDLE_CODE(bref.pack(mnc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(mcc.pack(bref));
  }
  if (mnc_present) {
    HANDLE_CODE(mnc.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcc_present, 1));
  HANDLE_CODE(bref.unpack(mnc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(mcc.unpack(bref));
  }
  if (mnc_present) {
    HANDLE_CODE(mnc.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void plmn_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mcc_present) {
    j.write_str("mcc", mcc.to_string());
  }
  if (mnc_present) {
    j.write_str("mnc", mnc.to_string());
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const plmn_id_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.mcc_present) {
    j["mcc"] = obj.mcc.to_string();
  }
  if (obj.mnc_present) {
    j["mnc"] = obj.mnc.to_string();
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, plmn_id_s& obj)
{
  if (in_j.contains("mcc")) {
    obj.mcc.from_string(in_j["mcc"].get<std::string>());
    obj.mcc_present = true;
  }
  if (in_j.contains("mnc")) {
    obj.mnc.from_string(in_j["mnc"].get<std::string>());
    obj.mnc_present = true;
  }
}

// Snssai ::= SEQUENCE
SRSASN_CODE snssai_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sst_present, 1));
  HANDLE_CODE(bref.pack(sd_present, 1));

  if (sst_present) {
    HANDLE_CODE(pack_integer(bref, sst, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (sd_present) {
    HANDLE_CODE(sd.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE snssai_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sst_present, 1));
  HANDLE_CODE(bref.unpack(sd_present, 1));

  if (sst_present) {
    HANDLE_CODE(unpack_integer(sst, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (sd_present) {
    HANDLE_CODE(sd.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void snssai_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sst_present) {
    j.write_int("sst", sst);
  }
  if (sd_present) {
    j.write_str("sd", sd.to_string());
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const snssai_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.sst_present) {
    j["sst"] = obj.sst;
  }
  if (obj.sd_present) {
    j["sd"] = obj.sd.to_string();
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, snssai_s& obj)
{
  if (in_j.contains("sst")) {
    in_j["sst"].get_to(obj.sst);
    obj.sst_present = true;
  }
  if (in_j.contains("sd")) {
    obj.sd.from_string(in_j["sd"].get<std::string>());
    obj.sd_present = true;
  }
}

SRSASN_CODE partition_flow_list_item_s_::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(snssai_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_present, 1));
  HANDLE_CODE(bref.pack(n5qi_list.size() > 0, 1));

  if (snssai_present) {
    HANDLE_CODE(snssai.pack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  if (n5qi_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, n5qi_list, 1, 64, integer_packer<uint16_t>(0, 255, false, true)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE partition_flow_list_item_s_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(snssai_present, 1));
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));
  bool n5qi_list_present;
  HANDLE_CODE(bref.unpack(n5qi_list_present, 1));

  if (snssai_present) {
    HANDLE_CODE(snssai.unpack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  if (n5qi_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(n5qi_list, bref, 1, 64, integer_packer<uint16_t>(0, 255, false, true)));
  }

  return SRSASN_SUCCESS;
}
void partition_flow_list_item_s_::to_json(json_writer& j) const
{
  j.start_obj();
  if (snssai_present) {
    j.write_fieldname("snssai");
    snssai.to_json(j);
  }
  if (plmn_id_present) {
    j.write_fieldname("plmnId");
    plmn_id.to_json(j);
  }
  if (n5qi_list.size() > 0) {
    j.start_array("n5qiList");
    for (const auto& e1 : n5qi_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const partition_flow_list_item_s_& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.snssai_present) {
    j["snssai"] = obj.snssai;
  }
  if (obj.plmn_id_present) {
    j["plmnId"] = obj.plmn_id;
  }
  if (obj.n5qi_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.n5qi_list) {
        j_array.push_back(e1);
      }
      j["n5qiList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, partition_flow_list_item_s_& obj)
{
  if (in_j.contains("snssai")) {
    in_j["snssai"].get_to(obj.snssai);
    obj.snssai_present = true;
  }
  if (in_j.contains("plmnId")) {
    in_j["plmnId"].get_to(obj.plmn_id);
    obj.plmn_id_present = true;
  }
  if (in_j.contains("n5qiList")) {
    {
      const auto& in_arr  = in_j.at("n5qiList");
      auto&       out_arr = obj.n5qi_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<uint16_t>();
      }
    }
  }
}

// PlmnInfo ::= SEQUENCE
SRSASN_CODE plmn_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_present, 1));
  HANDLE_CODE(bref.pack(snssai_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  if (snssai_present) {
    HANDLE_CODE(snssai.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));
  HANDLE_CODE(bref.unpack(snssai_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  if (snssai_present) {
    HANDLE_CODE(snssai.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void plmn_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_present) {
    j.write_fieldname("plmnId");
    plmn_id.to_json(j);
  }
  if (snssai_present) {
    j.write_fieldname("snssai");
    snssai.to_json(j);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const plmn_info_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.plmn_id_present) {
    j["plmnId"] = obj.plmn_id;
  }
  if (obj.snssai_present) {
    j["snssai"] = obj.snssai;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, plmn_info_s& obj)
{
  if (in_j.contains("plmnId")) {
    in_j["plmnId"].get_to(obj.plmn_id);
    obj.plmn_id_present = true;
  }
  if (in_j.contains("snssai")) {
    in_j["snssai"].get_to(obj.snssai);
    obj.snssai_present = true;
  }
}

// RPolarizationItem ::= SEQUENCE
SRSASN_CODE r_polarization_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rpolarization_idx, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(rpolarization_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE r_polarization_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rpolarization_idx, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(rpolarization_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void r_polarization_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rPolarizationIndex", rpolarization_idx);
  j.write_str("rPolarizationType", rpolarization_type.to_string());
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const r_polarization_item_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["rPolarizationIndex"]   = obj.rpolarization_idx;
  j["rPolarizationType"]    = obj.rpolarization_type.to_string();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, r_polarization_item_s& obj)
{
  in_j["rPolarizationIndex"].get_to(obj.rpolarization_idx);
  obj.rpolarization_type.from_string(in_j["rPolarizationType"].get<std::string>());
}

const char* r_polarization_item_s::rpolarization_type_opts::to_string() const
{
  static const char* names[] = {"MINUS_45", "ZERO", "PLUS_45", "PLUS_90"};
  return convert_enum_idx(names, 4, value, "r_polarization_item_s::rpolarization_type_e_");
}
bool r_polarization_item_s::rpolarization_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"MINUS_45", "ZERO", "PLUS_45", "PLUS_90"};
  uint32_t           idx;
  if (convert_enum_str(names, 4, str.c_str(), idx, "r_polarization_item_s::rpolarization_type_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// SleepMode ::= SEQUENCE
SRSASN_CODE sleep_mode_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sleep_mode_type.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, wakeup_dur, false, true));
  HANDLE_CODE(bref.pack(wakeup_dur_guaranteed, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sleep_mode_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sleep_mode_type.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(wakeup_dur, bref, false, true));
  HANDLE_CODE(bref.unpack(wakeup_dur_guaranteed, 1));

  return SRSASN_SUCCESS;
}
void sleep_mode_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sleepModeType", sleep_mode_type.to_string());
  j.write_int("wakeupDuration", wakeup_dur);
  j.write_bool("wakeupDurationGuaranteed", wakeup_dur_guaranteed);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const sleep_mode_s& obj)
{
  out_j["sleepModeType"]            = obj.sleep_mode_type.to_string();
  out_j["wakeupDuration"]           = obj.wakeup_dur;
  out_j["wakeupDurationGuaranteed"] = obj.wakeup_dur_guaranteed;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, sleep_mode_s& obj)
{
  obj.sleep_mode_type.from_string(in_j["sleepModeType"].get<std::string>());
  in_j["wakeupDuration"].get_to(obj.wakeup_dur);
  in_j["wakeupDurationGuaranteed"].get_to(obj.wakeup_dur_guaranteed);
}

const char* sleep_mode_s::sleep_mode_type_opts::to_string() const
{
  static const char* names[] = {"SLEEP_MODE0", "SLEEP_MODE1", "SLEEP_MODE3", "SLEEP_MODE4"};
  return convert_enum_idx(names, 4, value, "sleep_mode_s::sleep_mode_type_e_");
}
bool sleep_mode_s::sleep_mode_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"SLEEP_MODE0", "SLEEP_MODE1", "SLEEP_MODE3", "SLEEP_MODE4"};
  uint32_t           idx;
  if (convert_enum_str(names, 4, str.c_str(), idx, "sleep_mode_s::sleep_mode_type_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t sleep_mode_s::sleep_mode_type_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "sleep_mode_s::sleep_mode_type_e_");
}

// SupportedTrxControlMask ::= SEQUENCE
SRSASN_CODE supported_trx_ctrl_mask_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mask_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, ant_mask, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_trx_ctrl_mask_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mask_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(ant_mask, bref, false, true));

  return SRSASN_SUCCESS;
}
void supported_trx_ctrl_mask_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maskName", mask_name.to_string());
  j.write_int("antennaMask", ant_mask);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const supported_trx_ctrl_mask_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["maskName"]             = obj.mask_name.to_string();
  j["antennaMask"]          = obj.ant_mask;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, supported_trx_ctrl_mask_s& obj)
{
  obj.mask_name.from_string(in_j["maskName"].get<std::string>());
  in_j["antennaMask"].get_to(obj.ant_mask);
}

// TPolarizationItem ::= SEQUENCE
SRSASN_CODE t_polarization_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, tpolarization_idx, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(tpolarization_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE t_polarization_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(tpolarization_idx, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(tpolarization_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void t_polarization_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("tPolarizationIndex", tpolarization_idx);
  j.write_str("tPolarizationType", tpolarization_type.to_string());
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const t_polarization_item_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["tPolarizationIndex"]   = obj.tpolarization_idx;
  j["tPolarizationType"]    = obj.tpolarization_type.to_string();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, t_polarization_item_s& obj)
{
  in_j["tPolarizationIndex"].get_to(obj.tpolarization_idx);
  obj.tpolarization_type.from_string(in_j["tPolarizationType"].get<std::string>());
}

const char* t_polarization_item_s::tpolarization_type_opts::to_string() const
{
  static const char* names[] = {"MINUS_45", "ZERO", "PLUS_45", "PLUS_90"};
  return convert_enum_idx(names, 4, value, "t_polarization_item_s::tpolarization_type_e_");
}
bool t_polarization_item_s::tpolarization_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"MINUS_45", "ZERO", "PLUS_45", "PLUS_90"};
  uint32_t           idx;
  if (convert_enum_str(names, 4, str.c_str(), idx, "t_polarization_item_s::tpolarization_type_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// RxArrayItem ::= SEQUENCE
SRSASN_CODE rx_array_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rhorizontal_spacing_present, 1));
  HANDLE_CODE(bref.pack(rvertical_spacing_present, 1));
  HANDLE_CODE(bref.pack(rnormal_vector_direction_present, 1));
  HANDLE_CODE(bref.pack(rleftmost_bottom_array_elem_position_present, 1));

  HANDLE_CODE(rname.pack(bref));
  HANDLE_CODE(pack_integer(bref, rnof_rows, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(pack_integer(bref, rnof_columns, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(pack_integer(bref, rnof_array_layers, (uint16_t)1u, (uint16_t)255u, false, true));
  if (rhorizontal_spacing_present) {
    HANDLE_CODE(rhorizontal_spacing.pack(bref));
  }
  if (rvertical_spacing_present) {
    HANDLE_CODE(rvertical_spacing.pack(bref));
  }
  if (rnormal_vector_direction_present) {
    HANDLE_CODE(bref.pack(rnormal_vector_direction.razimuth_angle_present, 1));
    HANDLE_CODE(bref.pack(rnormal_vector_direction.rzenith_angle_present, 1));
    if (rnormal_vector_direction.razimuth_angle_present) {
      HANDLE_CODE(rnormal_vector_direction.razimuth_angle.pack(bref));
    }
    if (rnormal_vector_direction.rzenith_angle_present) {
      HANDLE_CODE(rnormal_vector_direction.rzenith_angle.pack(bref));
    }
  }
  if (rleftmost_bottom_array_elem_position_present) {
    HANDLE_CODE(bref.pack(rleftmost_bottom_array_elem_position.rx_axis_present, 1));
    HANDLE_CODE(bref.pack(rleftmost_bottom_array_elem_position.ry_axis_present, 1));
    HANDLE_CODE(bref.pack(rleftmost_bottom_array_elem_position.rz_axis_present, 1));
    if (rleftmost_bottom_array_elem_position.rx_axis_present) {
      HANDLE_CODE(rleftmost_bottom_array_elem_position.rx_axis.pack(bref));
    }
    if (rleftmost_bottom_array_elem_position.ry_axis_present) {
      HANDLE_CODE(rleftmost_bottom_array_elem_position.ry_axis.pack(bref));
    }
    if (rleftmost_bottom_array_elem_position.rz_axis_present) {
      HANDLE_CODE(rleftmost_bottom_array_elem_position.rz_axis.pack(bref));
    }
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, rpolarization_list, 1, 64, true));
  HANDLE_CODE(pack_integer(bref, rband_num, (uint16_t)1u, (uint16_t)1024u, false, true));
  HANDLE_CODE(bref.pack(rgain_correction_range.rmax_present, 1));
  HANDLE_CODE(bref.pack(rgain_correction_range.rmin_present, 1));
  if (rgain_correction_range.rmax_present) {
    HANDLE_CODE(rgain_correction_range.rmax.pack(bref));
  }
  if (rgain_correction_range.rmin_present) {
    HANDLE_CODE(rgain_correction_range.rmin.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rx_array_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rhorizontal_spacing_present, 1));
  HANDLE_CODE(bref.unpack(rvertical_spacing_present, 1));
  HANDLE_CODE(bref.unpack(rnormal_vector_direction_present, 1));
  HANDLE_CODE(bref.unpack(rleftmost_bottom_array_elem_position_present, 1));

  HANDLE_CODE(rname.unpack(bref));
  HANDLE_CODE(unpack_integer(rnof_rows, bref, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(unpack_integer(rnof_columns, bref, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(unpack_integer(rnof_array_layers, bref, (uint16_t)1u, (uint16_t)255u, false, true));
  if (rhorizontal_spacing_present) {
    HANDLE_CODE(rhorizontal_spacing.unpack(bref));
  }
  if (rvertical_spacing_present) {
    HANDLE_CODE(rvertical_spacing.unpack(bref));
  }
  if (rnormal_vector_direction_present) {
    HANDLE_CODE(bref.unpack(rnormal_vector_direction.razimuth_angle_present, 1));
    HANDLE_CODE(bref.unpack(rnormal_vector_direction.rzenith_angle_present, 1));
    if (rnormal_vector_direction.razimuth_angle_present) {
      HANDLE_CODE(rnormal_vector_direction.razimuth_angle.unpack(bref));
    }
    if (rnormal_vector_direction.rzenith_angle_present) {
      HANDLE_CODE(rnormal_vector_direction.rzenith_angle.unpack(bref));
    }
  }
  if (rleftmost_bottom_array_elem_position_present) {
    HANDLE_CODE(bref.unpack(rleftmost_bottom_array_elem_position.rx_axis_present, 1));
    HANDLE_CODE(bref.unpack(rleftmost_bottom_array_elem_position.ry_axis_present, 1));
    HANDLE_CODE(bref.unpack(rleftmost_bottom_array_elem_position.rz_axis_present, 1));
    if (rleftmost_bottom_array_elem_position.rx_axis_present) {
      HANDLE_CODE(rleftmost_bottom_array_elem_position.rx_axis.unpack(bref));
    }
    if (rleftmost_bottom_array_elem_position.ry_axis_present) {
      HANDLE_CODE(rleftmost_bottom_array_elem_position.ry_axis.unpack(bref));
    }
    if (rleftmost_bottom_array_elem_position.rz_axis_present) {
      HANDLE_CODE(rleftmost_bottom_array_elem_position.rz_axis.unpack(bref));
    }
  }
  HANDLE_CODE(unpack_dyn_seq_of(rpolarization_list, bref, 1, 64, true));
  HANDLE_CODE(unpack_integer(rband_num, bref, (uint16_t)1u, (uint16_t)1024u, false, true));
  HANDLE_CODE(bref.unpack(rgain_correction_range.rmax_present, 1));
  HANDLE_CODE(bref.unpack(rgain_correction_range.rmin_present, 1));
  if (rgain_correction_range.rmax_present) {
    HANDLE_CODE(rgain_correction_range.rmax.unpack(bref));
  }
  if (rgain_correction_range.rmin_present) {
    HANDLE_CODE(rgain_correction_range.rmin.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rx_array_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rName", rname.to_string());
  j.write_int("rNumberOfRows", rnof_rows);
  j.write_int("rNumberOfColumns", rnof_columns);
  j.write_int("rNumberOfArrayLayers", rnof_array_layers);
  if (rhorizontal_spacing_present) {
    j.write_fieldname("rHorizontalSpacing");
    rhorizontal_spacing.to_json(j);
  }
  if (rvertical_spacing_present) {
    j.write_fieldname("rVerticalSpacing");
    rvertical_spacing.to_json(j);
  }
  if (rnormal_vector_direction_present) {
    j.write_fieldname("rNormalVectorDirection");
    j.start_obj();
    if (rnormal_vector_direction.razimuth_angle_present) {
      j.write_fieldname("rAzimuthAngle");
      rnormal_vector_direction.razimuth_angle.to_json(j);
    }
    if (rnormal_vector_direction.rzenith_angle_present) {
      j.write_fieldname("rZenithAngle");
      rnormal_vector_direction.rzenith_angle.to_json(j);
    }
    j.end_obj();
  }
  if (rleftmost_bottom_array_elem_position_present) {
    j.write_fieldname("rLeftmostBottomArrayElementPosition");
    j.start_obj();
    if (rleftmost_bottom_array_elem_position.rx_axis_present) {
      j.write_fieldname("rXAxis");
      rleftmost_bottom_array_elem_position.rx_axis.to_json(j);
    }
    if (rleftmost_bottom_array_elem_position.ry_axis_present) {
      j.write_fieldname("rYAxis");
      rleftmost_bottom_array_elem_position.ry_axis.to_json(j);
    }
    if (rleftmost_bottom_array_elem_position.rz_axis_present) {
      j.write_fieldname("rZAxis");
      rleftmost_bottom_array_elem_position.rz_axis.to_json(j);
    }
    j.end_obj();
  }
  j.start_array("rPolarizationList");
  for (const auto& e1 : rpolarization_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("rBandNumber", rband_num);
  j.write_fieldname("rGainCorrectionRange");
  j.start_obj();
  if (rgain_correction_range.rmax_present) {
    j.write_fieldname("rMax");
    rgain_correction_range.rmax.to_json(j);
  }
  if (rgain_correction_range.rmin_present) {
    j.write_fieldname("rMin");
    rgain_correction_range.rmin.to_json(j);
  }
  j.end_obj();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const rx_array_item_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["rName"]                = obj.rname.to_string();
  j["rNumberOfRows"]        = obj.rnof_rows;
  j["rNumberOfColumns"]     = obj.rnof_columns;
  j["rNumberOfArrayLayers"] = obj.rnof_array_layers;
  if (obj.rhorizontal_spacing_present) {
    j["rHorizontalSpacing"] = obj.rhorizontal_spacing;
  }
  if (obj.rvertical_spacing_present) {
    j["rVerticalSpacing"] = obj.rvertical_spacing;
  }
  if (obj.rnormal_vector_direction_present) {
    j["rNormalVectorDirection"] = nlohmann::ordered_json::object();
    if (obj.rnormal_vector_direction.razimuth_angle_present) {
      j["rAzimuthAngle"] = obj.rnormal_vector_direction.razimuth_angle;
    }
    if (obj.rnormal_vector_direction.rzenith_angle_present) {
      j["rZenithAngle"] = obj.rnormal_vector_direction.rzenith_angle;
    }
  }
  if (obj.rleftmost_bottom_array_elem_position_present) {
    j["rLeftmostBottomArrayElementPosition"] = nlohmann::ordered_json::object();
    if (obj.rleftmost_bottom_array_elem_position.rx_axis_present) {
      j["rXAxis"] = obj.rleftmost_bottom_array_elem_position.rx_axis;
    }
    if (obj.rleftmost_bottom_array_elem_position.ry_axis_present) {
      j["rYAxis"] = obj.rleftmost_bottom_array_elem_position.ry_axis;
    }
    if (obj.rleftmost_bottom_array_elem_position.rz_axis_present) {
      j["rZAxis"] = obj.rleftmost_bottom_array_elem_position.rz_axis;
    }
  }
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.rpolarization_list) {
      j_array.push_back(e1);
    }
    j["rPolarizationList"] = j_array;
  }
  j["rBandNumber"]          = obj.rband_num;
  j["rGainCorrectionRange"] = nlohmann::ordered_json::object();
  if (obj.rgain_correction_range.rmax_present) {
    j["rMax"] = obj.rgain_correction_range.rmax;
  }
  if (obj.rgain_correction_range.rmin_present) {
    j["rMin"] = obj.rgain_correction_range.rmin;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, rx_array_item_s& obj)
{
  obj.rname.from_string(in_j["rName"].get<std::string>());
  in_j["rNumberOfRows"].get_to(obj.rnof_rows);
  in_j["rNumberOfColumns"].get_to(obj.rnof_columns);
  in_j["rNumberOfArrayLayers"].get_to(obj.rnof_array_layers);
  if (in_j.contains("rHorizontalSpacing")) {
    in_j["rHorizontalSpacing"].get_to(obj.rhorizontal_spacing);
    obj.rhorizontal_spacing_present = true;
  }
  if (in_j.contains("rVerticalSpacing")) {
    in_j["rVerticalSpacing"].get_to(obj.rvertical_spacing);
    obj.rvertical_spacing_present = true;
  }
  if (in_j.contains("rNormalVectorDirection")) {
    if (obj.rnormal_vector_direction.razimuth_angle_present) {
      in_j["rAzimuthAngle"].get_to(obj.rnormal_vector_direction.razimuth_angle);

      obj.rnormal_vector_direction.razimuth_angle_present = true;
    }
    if (obj.rnormal_vector_direction.rzenith_angle_present) {
      in_j["rZenithAngle"].get_to(obj.rnormal_vector_direction.rzenith_angle);

      obj.rnormal_vector_direction.rzenith_angle_present = true;
    }
    obj.rnormal_vector_direction_present = true;
  }
  if (in_j.contains("rLeftmostBottomArrayElementPosition")) {
    if (obj.rleftmost_bottom_array_elem_position.rx_axis_present) {
      in_j["rXAxis"].get_to(obj.rleftmost_bottom_array_elem_position.rx_axis);

      obj.rleftmost_bottom_array_elem_position.rx_axis_present = true;
    }
    if (obj.rleftmost_bottom_array_elem_position.ry_axis_present) {
      in_j["rYAxis"].get_to(obj.rleftmost_bottom_array_elem_position.ry_axis);

      obj.rleftmost_bottom_array_elem_position.ry_axis_present = true;
    }
    if (obj.rleftmost_bottom_array_elem_position.rz_axis_present) {
      in_j["rZAxis"].get_to(obj.rleftmost_bottom_array_elem_position.rz_axis);

      obj.rleftmost_bottom_array_elem_position.rz_axis_present = true;
    }
    obj.rleftmost_bottom_array_elem_position_present = true;
  }
  {
    const auto& in_arr  = in_j.at("rPolarizationList");
    auto&       out_arr = obj.rpolarization_list;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<r_polarization_item_s>();
    }
  }
  in_j["rBandNumber"].get_to(obj.rband_num);
  if (obj.rgain_correction_range.rmax_present) {
    in_j["rMax"].get_to(obj.rgain_correction_range.rmax);

    obj.rgain_correction_range.rmax_present = true;
  }
  if (obj.rgain_correction_range.rmin_present) {
    in_j["rMin"].get_to(obj.rgain_correction_range.rmin);

    obj.rgain_correction_range.rmin_present = true;
  }
}

// TxArrayItem ::= SEQUENCE
SRSASN_CODE tx_array_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thorizontal_spacing_present, 1));
  HANDLE_CODE(bref.pack(tvertical_spacing_present, 1));
  HANDLE_CODE(bref.pack(tnormal_vector_direction_present, 1));
  HANDLE_CODE(bref.pack(tleftmost_bottom_array_elem_position_present, 1));
  HANDLE_CODE(bref.pack(tmin_gain_present, 1));

  HANDLE_CODE(tname.pack(bref));
  HANDLE_CODE(pack_integer(bref, tnof_rows, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(pack_integer(bref, tnof_columns, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(pack_integer(bref, tnof_array_layers, (uint16_t)1u, (uint16_t)255u, false, true));
  if (thorizontal_spacing_present) {
    HANDLE_CODE(thorizontal_spacing.pack(bref));
  }
  if (tvertical_spacing_present) {
    HANDLE_CODE(tvertical_spacing.pack(bref));
  }
  if (tnormal_vector_direction_present) {
    HANDLE_CODE(bref.pack(tnormal_vector_direction.tazimuth_angle_present, 1));
    HANDLE_CODE(bref.pack(tnormal_vector_direction.tzenith_angle_present, 1));
    if (tnormal_vector_direction.tazimuth_angle_present) {
      HANDLE_CODE(tnormal_vector_direction.tazimuth_angle.pack(bref));
    }
    if (tnormal_vector_direction.tzenith_angle_present) {
      HANDLE_CODE(tnormal_vector_direction.tzenith_angle.pack(bref));
    }
  }
  if (tleftmost_bottom_array_elem_position_present) {
    HANDLE_CODE(bref.pack(tleftmost_bottom_array_elem_position.tx_axis_present, 1));
    HANDLE_CODE(bref.pack(tleftmost_bottom_array_elem_position.ty_axis_present, 1));
    HANDLE_CODE(bref.pack(tleftmost_bottom_array_elem_position.tz_axis_present, 1));
    if (tleftmost_bottom_array_elem_position.tx_axis_present) {
      HANDLE_CODE(tleftmost_bottom_array_elem_position.tx_axis.pack(bref));
    }
    if (tleftmost_bottom_array_elem_position.ty_axis_present) {
      HANDLE_CODE(tleftmost_bottom_array_elem_position.ty_axis.pack(bref));
    }
    if (tleftmost_bottom_array_elem_position.tz_axis_present) {
      HANDLE_CODE(tleftmost_bottom_array_elem_position.tz_axis.pack(bref));
    }
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, tpolarization_list, 1, 64, true));
  HANDLE_CODE(pack_integer(bref, tband_num, (uint16_t)1u, (uint16_t)1024u, false, true));
  HANDLE_CODE(tmax_gain.pack(bref));
  if (tmin_gain_present) {
    HANDLE_CODE(tmin_gain.pack(bref));
  }
  HANDLE_CODE(bref.pack(tindependent_pwr_budget, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tx_array_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thorizontal_spacing_present, 1));
  HANDLE_CODE(bref.unpack(tvertical_spacing_present, 1));
  HANDLE_CODE(bref.unpack(tnormal_vector_direction_present, 1));
  HANDLE_CODE(bref.unpack(tleftmost_bottom_array_elem_position_present, 1));
  HANDLE_CODE(bref.unpack(tmin_gain_present, 1));

  HANDLE_CODE(tname.unpack(bref));
  HANDLE_CODE(unpack_integer(tnof_rows, bref, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(unpack_integer(tnof_columns, bref, (uint32_t)1u, (uint32_t)65535u, false, true));
  HANDLE_CODE(unpack_integer(tnof_array_layers, bref, (uint16_t)1u, (uint16_t)255u, false, true));
  if (thorizontal_spacing_present) {
    HANDLE_CODE(thorizontal_spacing.unpack(bref));
  }
  if (tvertical_spacing_present) {
    HANDLE_CODE(tvertical_spacing.unpack(bref));
  }
  if (tnormal_vector_direction_present) {
    HANDLE_CODE(bref.unpack(tnormal_vector_direction.tazimuth_angle_present, 1));
    HANDLE_CODE(bref.unpack(tnormal_vector_direction.tzenith_angle_present, 1));
    if (tnormal_vector_direction.tazimuth_angle_present) {
      HANDLE_CODE(tnormal_vector_direction.tazimuth_angle.unpack(bref));
    }
    if (tnormal_vector_direction.tzenith_angle_present) {
      HANDLE_CODE(tnormal_vector_direction.tzenith_angle.unpack(bref));
    }
  }
  if (tleftmost_bottom_array_elem_position_present) {
    HANDLE_CODE(bref.unpack(tleftmost_bottom_array_elem_position.tx_axis_present, 1));
    HANDLE_CODE(bref.unpack(tleftmost_bottom_array_elem_position.ty_axis_present, 1));
    HANDLE_CODE(bref.unpack(tleftmost_bottom_array_elem_position.tz_axis_present, 1));
    if (tleftmost_bottom_array_elem_position.tx_axis_present) {
      HANDLE_CODE(tleftmost_bottom_array_elem_position.tx_axis.unpack(bref));
    }
    if (tleftmost_bottom_array_elem_position.ty_axis_present) {
      HANDLE_CODE(tleftmost_bottom_array_elem_position.ty_axis.unpack(bref));
    }
    if (tleftmost_bottom_array_elem_position.tz_axis_present) {
      HANDLE_CODE(tleftmost_bottom_array_elem_position.tz_axis.unpack(bref));
    }
  }
  HANDLE_CODE(unpack_dyn_seq_of(tpolarization_list, bref, 1, 64, true));
  HANDLE_CODE(unpack_integer(tband_num, bref, (uint16_t)1u, (uint16_t)1024u, false, true));
  HANDLE_CODE(tmax_gain.unpack(bref));
  if (tmin_gain_present) {
    HANDLE_CODE(tmin_gain.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(tindependent_pwr_budget, 1));

  return SRSASN_SUCCESS;
}
void tx_array_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("tName", tname.to_string());
  j.write_int("tNumberOfRows", tnof_rows);
  j.write_int("tNumberOfColumns", tnof_columns);
  j.write_int("tNumberOfArrayLayers", tnof_array_layers);
  if (thorizontal_spacing_present) {
    j.write_fieldname("tHorizontalSpacing");
    thorizontal_spacing.to_json(j);
  }
  if (tvertical_spacing_present) {
    j.write_fieldname("tVerticalSpacing");
    tvertical_spacing.to_json(j);
  }
  if (tnormal_vector_direction_present) {
    j.write_fieldname("tNormalVectorDirection");
    j.start_obj();
    if (tnormal_vector_direction.tazimuth_angle_present) {
      j.write_fieldname("tAzimuthAngle");
      tnormal_vector_direction.tazimuth_angle.to_json(j);
    }
    if (tnormal_vector_direction.tzenith_angle_present) {
      j.write_fieldname("tZenithAngle");
      tnormal_vector_direction.tzenith_angle.to_json(j);
    }
    j.end_obj();
  }
  if (tleftmost_bottom_array_elem_position_present) {
    j.write_fieldname("tLeftmostBottomArrayElementPosition");
    j.start_obj();
    if (tleftmost_bottom_array_elem_position.tx_axis_present) {
      j.write_fieldname("tXAxis");
      tleftmost_bottom_array_elem_position.tx_axis.to_json(j);
    }
    if (tleftmost_bottom_array_elem_position.ty_axis_present) {
      j.write_fieldname("tYAxis");
      tleftmost_bottom_array_elem_position.ty_axis.to_json(j);
    }
    if (tleftmost_bottom_array_elem_position.tz_axis_present) {
      j.write_fieldname("tZAxis");
      tleftmost_bottom_array_elem_position.tz_axis.to_json(j);
    }
    j.end_obj();
  }
  j.start_array("tPolarizationList");
  for (const auto& e1 : tpolarization_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("tBandNumber", tband_num);
  j.write_fieldname("tMaxGain");
  tmax_gain.to_json(j);
  if (tmin_gain_present) {
    j.write_fieldname("tMinGain");
    tmin_gain.to_json(j);
  }
  j.write_bool("tIndependentPowerBudget", tindependent_pwr_budget);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const tx_array_item_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["tName"]                = obj.tname.to_string();
  j["tNumberOfRows"]        = obj.tnof_rows;
  j["tNumberOfColumns"]     = obj.tnof_columns;
  j["tNumberOfArrayLayers"] = obj.tnof_array_layers;
  if (obj.thorizontal_spacing_present) {
    j["tHorizontalSpacing"] = obj.thorizontal_spacing;
  }
  if (obj.tvertical_spacing_present) {
    j["tVerticalSpacing"] = obj.tvertical_spacing;
  }
  if (obj.tnormal_vector_direction_present) {
    j["tNormalVectorDirection"] = nlohmann::ordered_json::object();
    if (obj.tnormal_vector_direction.tazimuth_angle_present) {
      j["tAzimuthAngle"] = obj.tnormal_vector_direction.tazimuth_angle;
    }
    if (obj.tnormal_vector_direction.tzenith_angle_present) {
      j["tZenithAngle"] = obj.tnormal_vector_direction.tzenith_angle;
    }
  }
  if (obj.tleftmost_bottom_array_elem_position_present) {
    j["tLeftmostBottomArrayElementPosition"] = nlohmann::ordered_json::object();
    if (obj.tleftmost_bottom_array_elem_position.tx_axis_present) {
      j["tXAxis"] = obj.tleftmost_bottom_array_elem_position.tx_axis;
    }
    if (obj.tleftmost_bottom_array_elem_position.ty_axis_present) {
      j["tYAxis"] = obj.tleftmost_bottom_array_elem_position.ty_axis;
    }
    if (obj.tleftmost_bottom_array_elem_position.tz_axis_present) {
      j["tZAxis"] = obj.tleftmost_bottom_array_elem_position.tz_axis;
    }
  }
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.tpolarization_list) {
      j_array.push_back(e1);
    }
    j["tPolarizationList"] = j_array;
  }
  j["tBandNumber"] = obj.tband_num;
  j["tMaxGain"]    = obj.tmax_gain;
  if (obj.tmin_gain_present) {
    j["tMinGain"] = obj.tmin_gain;
  }
  j["tIndependentPowerBudget"] = obj.tindependent_pwr_budget;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, tx_array_item_s& obj)
{
  obj.tname.from_string(in_j["tName"].get<std::string>());
  in_j["tNumberOfRows"].get_to(obj.tnof_rows);
  in_j["tNumberOfColumns"].get_to(obj.tnof_columns);
  in_j["tNumberOfArrayLayers"].get_to(obj.tnof_array_layers);
  if (in_j.contains("tHorizontalSpacing")) {
    in_j["tHorizontalSpacing"].get_to(obj.thorizontal_spacing);
    obj.thorizontal_spacing_present = true;
  }
  if (in_j.contains("tVerticalSpacing")) {
    in_j["tVerticalSpacing"].get_to(obj.tvertical_spacing);
    obj.tvertical_spacing_present = true;
  }
  if (in_j.contains("tNormalVectorDirection")) {
    if (obj.tnormal_vector_direction.tazimuth_angle_present) {
      in_j["tAzimuthAngle"].get_to(obj.tnormal_vector_direction.tazimuth_angle);

      obj.tnormal_vector_direction.tazimuth_angle_present = true;
    }
    if (obj.tnormal_vector_direction.tzenith_angle_present) {
      in_j["tZenithAngle"].get_to(obj.tnormal_vector_direction.tzenith_angle);

      obj.tnormal_vector_direction.tzenith_angle_present = true;
    }
    obj.tnormal_vector_direction_present = true;
  }
  if (in_j.contains("tLeftmostBottomArrayElementPosition")) {
    if (obj.tleftmost_bottom_array_elem_position.tx_axis_present) {
      in_j["tXAxis"].get_to(obj.tleftmost_bottom_array_elem_position.tx_axis);

      obj.tleftmost_bottom_array_elem_position.tx_axis_present = true;
    }
    if (obj.tleftmost_bottom_array_elem_position.ty_axis_present) {
      in_j["tYAxis"].get_to(obj.tleftmost_bottom_array_elem_position.ty_axis);

      obj.tleftmost_bottom_array_elem_position.ty_axis_present = true;
    }
    if (obj.tleftmost_bottom_array_elem_position.tz_axis_present) {
      in_j["tZAxis"].get_to(obj.tleftmost_bottom_array_elem_position.tz_axis);

      obj.tleftmost_bottom_array_elem_position.tz_axis_present = true;
    }
    obj.tleftmost_bottom_array_elem_position_present = true;
  }
  {
    const auto& in_arr  = in_j.at("tPolarizationList");
    auto&       out_arr = obj.tpolarization_list;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<t_polarization_item_s>();
    }
  }
  in_j["tBandNumber"].get_to(obj.tband_num);
  in_j["tMaxGain"].get_to(obj.tmax_gain);
  if (in_j.contains("tMinGain")) {
    in_j["tMinGain"].get_to(obj.tmin_gain);
    obj.tmin_gain_present = true;
  }
  in_j["tIndependentPowerBudget"].get_to(obj.tindependent_pwr_budget);
}

// AsmCapabilityInfo ::= SEQUENCE
SRSASN_CODE asm_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, sleep_modes, 1, 64, true));
  HANDLE_CODE(bref.pack(defined_dur_sleep_supported, 1));
  HANDLE_CODE(bref.pack(undefined_dur_sleep_supported, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE asm_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(sleep_modes, bref, 1, 64, true));
  HANDLE_CODE(bref.unpack(defined_dur_sleep_supported, 1));
  HANDLE_CODE(bref.unpack(undefined_dur_sleep_supported, 1));

  return SRSASN_SUCCESS;
}
void asm_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sleepModes");
  for (const auto& e1 : sleep_modes) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_bool("definedDurationSleepSupported", defined_dur_sleep_supported);
  j.write_bool("undefinedDurationSleepSupported", undefined_dur_sleep_supported);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const asm_cap_info_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.sleep_modes) {
      j_array.push_back(e1);
    }
    j["sleepModes"] = j_array;
  }
  j["definedDurationSleepSupported"]   = obj.defined_dur_sleep_supported;
  j["undefinedDurationSleepSupported"] = obj.undefined_dur_sleep_supported;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, asm_cap_info_s& obj)
{
  {
    const auto& in_arr  = in_j.at("sleepModes");
    auto&       out_arr = obj.sleep_modes;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<sleep_mode_s>();
    }
  }
  in_j["definedDurationSleepSupported"].get_to(obj.defined_dur_sleep_supported);
  in_j["undefinedDurationSleepSupported"].get_to(obj.undefined_dur_sleep_supported);
}

// BwpContext ::= ENUMERATED
const char* bwp_context_opts::to_string() const
{
  static const char* names[] = {"DL", "UL", "SUL"};
  return convert_enum_idx(names, 3, value, "bwp_context_e");
}
bool bwp_context_opts::from_string(const std::string& str)
{
  static const char* names[] = {"DL", "UL", "SUL"};
  uint32_t           idx;
  if (convert_enum_str(names, 3, str.c_str(), idx, "bwp_context_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// CyclicPrefix ::= ENUMERATED
const char* cp_opts::to_string() const
{
  static const char* names[] = {"n15", "n30", "n60", "n120"};
  return convert_enum_idx(names, 4, value, "cp_e");
}
bool cp_opts::from_string(const std::string& str)
{
  static const char* names[] = {"n15", "n30", "n60", "n120"};
  uint32_t           idx;
  if (convert_enum_str(names, 4, str.c_str(), idx, "cp_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t cp_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "cp_e");
}

// EnergySavingCapabilityCommonInfo ::= SEQUENCE
SRSASN_CODE energy_saving_cap_common_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sleep_dur_ext_supported_present, 1));

  HANDLE_CODE(bref.pack(st8_ready_msg_supported, 1));
  if (sleep_dur_ext_supported_present) {
    HANDLE_CODE(bref.pack(sleep_dur_ext_supported, 1));
  }
  HANDLE_CODE(bref.pack(emergency_wake_up_cmd_supported, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE energy_saving_cap_common_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sleep_dur_ext_supported_present, 1));

  HANDLE_CODE(bref.unpack(st8_ready_msg_supported, 1));
  if (sleep_dur_ext_supported_present) {
    HANDLE_CODE(bref.unpack(sleep_dur_ext_supported, 1));
  }
  HANDLE_CODE(bref.unpack(emergency_wake_up_cmd_supported, 1));

  return SRSASN_SUCCESS;
}
void energy_saving_cap_common_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("sT8-ready-message-supported", st8_ready_msg_supported);
  if (sleep_dur_ext_supported_present) {
    j.write_bool("sleep-duration-extension-supported", sleep_dur_ext_supported);
  }
  j.write_bool("emergency-wake-up-command-supported", emergency_wake_up_cmd_supported);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const energy_saving_cap_common_info_s& obj)
{
  nlohmann::ordered_json& j        = out_j;
  j["sT8-ready-message-supported"] = obj.st8_ready_msg_supported;
  if (obj.sleep_dur_ext_supported_present) {
    j["sleep-duration-extension-supported"] = obj.sleep_dur_ext_supported;
  }
  j["emergency-wake-up-command-supported"] = obj.emergency_wake_up_cmd_supported;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, energy_saving_cap_common_info_s& obj)
{
  in_j["sT8-ready-message-supported"].get_to(obj.st8_ready_msg_supported);
  if (in_j.contains("sleep-duration-extension-supported")) {
    in_j["sleep-duration-extension-supported"].get_to(obj.sleep_dur_ext_supported);
    obj.sleep_dur_ext_supported_present = true;
  }
  in_j["emergency-wake-up-command-supported"].get_to(obj.emergency_wake_up_cmd_supported);
}

// EventTriggerStyleType ::= SEQUENCE
SRSASN_CODE event_trigger_style_type_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, event_trigger_style_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_style_type_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(event_trigger_style_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void event_trigger_style_type_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eventTriggerStyleType", event_trigger_style_type);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const event_trigger_style_type_s& obj)
{
  nlohmann::ordered_json& j  = out_j;
  j["eventTriggerStyleType"] = obj.event_trigger_style_type;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, event_trigger_style_type_s& obj)
{
  in_j["eventTriggerStyleType"].get_to(obj.event_trigger_style_type);
}

// IsInitialBwp ::= ENUMERATED
const char* is_init_bwp_opts::to_string() const
{
  static const char* names[] = {"INITIAL", "OTHER", "SUL"};
  return convert_enum_idx(names, 3, value, "is_init_bwp_e");
}
bool is_init_bwp_opts::from_string(const std::string& str)
{
  static const char* names[] = {"INITIAL", "OTHER", "SUL"};
  uint32_t           idx;
  if (convert_enum_str(names, 3, str.c_str(), idx, "is_init_bwp_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

SRSASN_CODE partition_list_item_s_::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(poffset_to_point_a_present, 1));
  HANDLE_CODE(bref.pack(pnof_rbs_present, 1));
  HANDLE_CODE(bref.pack(partition_flow_list.size() > 0, 1));

  if (poffset_to_point_a_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, poffset_to_point_a, false, true));
  }
  if (pnof_rbs_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, pnof_rbs, false, true));
  }
  if (partition_flow_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, partition_flow_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE partition_list_item_s_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(poffset_to_point_a_present, 1));
  HANDLE_CODE(bref.unpack(pnof_rbs_present, 1));
  bool partition_flow_list_present;
  HANDLE_CODE(bref.unpack(partition_flow_list_present, 1));

  if (poffset_to_point_a_present) {
    HANDLE_CODE(unpack_unconstrained_integer(poffset_to_point_a, bref, false, true));
  }
  if (pnof_rbs_present) {
    HANDLE_CODE(unpack_unconstrained_integer(pnof_rbs, bref, false, true));
  }
  if (partition_flow_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(partition_flow_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void partition_list_item_s_::to_json(json_writer& j) const
{
  j.start_obj();
  if (poffset_to_point_a_present) {
    j.write_int("pOffsetToPointA", poffset_to_point_a);
  }
  if (pnof_rbs_present) {
    j.write_int("pNumberOfRBs", pnof_rbs);
  }
  if (partition_flow_list.size() > 0) {
    j.start_array("partitionFlowList");
    for (const auto& e1 : partition_flow_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const partition_list_item_s_& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.poffset_to_point_a_present) {
    j["pOffsetToPointA"] = obj.poffset_to_point_a;
  }
  if (obj.pnof_rbs_present) {
    j["pNumberOfRBs"] = obj.pnof_rbs;
  }
  if (obj.partition_flow_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.partition_flow_list) {
        j_array.push_back(e1);
      }
      j["partitionFlowList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, partition_list_item_s_& obj)
{
  if (in_j.contains("pOffsetToPointA")) {
    in_j["pOffsetToPointA"].get_to(obj.poffset_to_point_a);
    obj.poffset_to_point_a_present = true;
  }
  if (in_j.contains("pNumberOfRBs")) {
    in_j["pNumberOfRBs"].get_to(obj.pnof_rbs);
    obj.pnof_rbs_present = true;
  }
  if (in_j.contains("partitionFlowList")) {
    {
      const auto& in_arr  = in_j.at("partitionFlowList");
      auto&       out_arr = obj.partition_flow_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<partition_flow_list_item_s_>();
      }
    }
  }
}

// PerfObjective ::= SEQUENCE
SRSASN_CODE perf_objive_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_ngbr_flow_br_present, 1));
  HANDLE_CODE(bref.pack(flow_br_avg_win_present, 1));
  HANDLE_CODE(bref.pack(max_pd_present, 1));
  HANDLE_CODE(bref.pack(target_pd_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_info_list, 1, 64, true));
  HANDLE_CODE(pack_integer(bref, five_qi_value, (uint16_t)0u, (uint16_t)255u, false, true));
  if (max_ngbr_flow_br_present) {
    HANDLE_CODE(max_ngbr_flow_br.pack(bref));
  }
  if (flow_br_avg_win_present) {
    HANDLE_CODE(pack_integer(bref, flow_br_avg_win, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (max_pd_present) {
    HANDLE_CODE(pack_integer(bref, max_pd, (uint16_t)0u, (uint16_t)1023u, false, true));
  }
  if (target_pd_present) {
    HANDLE_CODE(pack_integer(bref, target_pd, (uint16_t)0u, (uint16_t)1023u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE perf_objive_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_ngbr_flow_br_present, 1));
  HANDLE_CODE(bref.unpack(flow_br_avg_win_present, 1));
  HANDLE_CODE(bref.unpack(max_pd_present, 1));
  HANDLE_CODE(bref.unpack(target_pd_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_info_list, bref, 1, 64, true));
  HANDLE_CODE(unpack_integer(five_qi_value, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (max_ngbr_flow_br_present) {
    HANDLE_CODE(max_ngbr_flow_br.unpack(bref));
  }
  if (flow_br_avg_win_present) {
    HANDLE_CODE(unpack_integer(flow_br_avg_win, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (max_pd_present) {
    HANDLE_CODE(unpack_integer(max_pd, bref, (uint16_t)0u, (uint16_t)1023u, false, true));
  }
  if (target_pd_present) {
    HANDLE_CODE(unpack_integer(target_pd, bref, (uint16_t)0u, (uint16_t)1023u, false, true));
  }

  return SRSASN_SUCCESS;
}
void perf_objive_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmnInfoList");
  for (const auto& e1 : plmn_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("fiveQIValue", five_qi_value);
  if (max_ngbr_flow_br_present) {
    j.write_str("maxNgbrFlowBr", max_ngbr_flow_br.to_string());
  }
  if (flow_br_avg_win_present) {
    j.write_int("flowBrAvgWindow", flow_br_avg_win);
  }
  if (max_pd_present) {
    j.write_int("maxPd", max_pd);
  }
  if (target_pd_present) {
    j.write_int("targetPd", target_pd);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const perf_objive_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.plmn_info_list) {
      j_array.push_back(e1);
    }
    j["plmnInfoList"] = j_array;
  }
  j["fiveQIValue"] = obj.five_qi_value;
  if (obj.max_ngbr_flow_br_present) {
    j["maxNgbrFlowBr"] = obj.max_ngbr_flow_br.to_string();
  }
  if (obj.flow_br_avg_win_present) {
    j["flowBrAvgWindow"] = obj.flow_br_avg_win;
  }
  if (obj.max_pd_present) {
    j["maxPd"] = obj.max_pd;
  }
  if (obj.target_pd_present) {
    j["targetPd"] = obj.target_pd;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, perf_objive_s& obj)
{
  {
    const auto& in_arr  = in_j.at("plmnInfoList");
    auto&       out_arr = obj.plmn_info_list;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<plmn_info_s>();
    }
  }
  in_j["fiveQIValue"].get_to(obj.five_qi_value);
  if (in_j.contains("maxNgbrFlowBr")) {
    obj.max_ngbr_flow_br.from_string(in_j["maxNgbrFlowBr"].get<std::string>());
    obj.max_ngbr_flow_br_present = true;
  }
  if (in_j.contains("flowBrAvgWindow")) {
    in_j["flowBrAvgWindow"].get_to(obj.flow_br_avg_win);
    obj.flow_br_avg_win_present = true;
  }
  if (in_j.contains("maxPd")) {
    in_j["maxPd"].get_to(obj.max_pd);
    obj.max_pd_present = true;
  }
  if (in_j.contains("targetPd")) {
    in_j["targetPd"].get_to(obj.target_pd);
    obj.target_pd_present = true;
  }
}

// PrbBlankingConfigItem ::= SEQUENCE
SRSASN_CODE prb_blanking_cfg_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(bwp_idx_present, 1));
  HANDLE_CODE(bref.pack(frame_num_present, 1));
  HANDLE_CODE(bref.pack(slot_num_present, 1));
  HANDLE_CODE(bref.pack(prb_idx_present, 1));

  if (bwp_idx_present) {
    HANDLE_CODE(pack_integer(bref, bwp_idx, (uint8_t)0u, (uint8_t)3u, false, true));
  }
  if (frame_num_present) {
    HANDLE_CODE(pack_integer(bref, frame_num, (uint16_t)0u, (uint16_t)10240u, false, true));
  }
  if (slot_num_present) {
    HANDLE_CODE(pack_integer(bref, slot_num, (uint16_t)0u, (uint16_t)320u, false, true));
  }
  if (prb_idx_present) {
    HANDLE_CODE(pack_integer(bref, prb_idx, (uint16_t)0u, (uint16_t)275u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE prb_blanking_cfg_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(bwp_idx_present, 1));
  HANDLE_CODE(bref.unpack(frame_num_present, 1));
  HANDLE_CODE(bref.unpack(slot_num_present, 1));
  HANDLE_CODE(bref.unpack(prb_idx_present, 1));

  if (bwp_idx_present) {
    HANDLE_CODE(unpack_integer(bwp_idx, bref, (uint8_t)0u, (uint8_t)3u, false, true));
  }
  if (frame_num_present) {
    HANDLE_CODE(unpack_integer(frame_num, bref, (uint16_t)0u, (uint16_t)10240u, false, true));
  }
  if (slot_num_present) {
    HANDLE_CODE(unpack_integer(slot_num, bref, (uint16_t)0u, (uint16_t)320u, false, true));
  }
  if (prb_idx_present) {
    HANDLE_CODE(unpack_integer(prb_idx, bref, (uint16_t)0u, (uint16_t)275u, false, true));
  }

  return SRSASN_SUCCESS;
}
void prb_blanking_cfg_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bwp_idx_present) {
    j.write_int("bwpIndex", bwp_idx);
  }
  if (frame_num_present) {
    j.write_int("frameNumber", frame_num);
  }
  if (slot_num_present) {
    j.write_int("slotNumber", slot_num);
  }
  if (prb_idx_present) {
    j.write_int("prbIndex", prb_idx);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const prb_blanking_cfg_item_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.bwp_idx_present) {
    j["bwpIndex"] = obj.bwp_idx;
  }
  if (obj.frame_num_present) {
    j["frameNumber"] = obj.frame_num;
  }
  if (obj.slot_num_present) {
    j["slotNumber"] = obj.slot_num;
  }
  if (obj.prb_idx_present) {
    j["prbIndex"] = obj.prb_idx;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, prb_blanking_cfg_item_s& obj)
{
  if (in_j.contains("bwpIndex")) {
    in_j["bwpIndex"].get_to(obj.bwp_idx);
    obj.bwp_idx_present = true;
  }
  if (in_j.contains("frameNumber")) {
    in_j["frameNumber"].get_to(obj.frame_num);
    obj.frame_num_present = true;
  }
  if (in_j.contains("slotNumber")) {
    in_j["slotNumber"].get_to(obj.slot_num);
    obj.slot_num_present = true;
  }
  if (in_j.contains("prbIndex")) {
    in_j["prbIndex"].get_to(obj.prb_idx);
    obj.prb_idx_present = true;
  }
}

// RrmPolicyMember ::= SEQUENCE
SRSASN_CODE rrm_policy_member_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_present, 1));
  HANDLE_CODE(bref.pack(snssai_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  if (snssai_present) {
    HANDLE_CODE(snssai.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrm_policy_member_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));
  HANDLE_CODE(bref.unpack(snssai_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  if (snssai_present) {
    HANDLE_CODE(snssai.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrm_policy_member_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_present) {
    j.write_fieldname("plmnId");
    plmn_id.to_json(j);
  }
  if (snssai_present) {
    j.write_fieldname("snssai");
    snssai.to_json(j);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const rrm_policy_member_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.plmn_id_present) {
    j["plmnId"] = obj.plmn_id;
  }
  if (obj.snssai_present) {
    j["snssai"] = obj.snssai;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, rrm_policy_member_s& obj)
{
  if (in_j.contains("plmnId")) {
    in_j["plmnId"].get_to(obj.plmn_id);
    obj.plmn_id_present = true;
  }
  if (in_j.contains("snssai")) {
    in_j["snssai"].get_to(obj.snssai);
    obj.snssai_present = true;
  }
}

// TrxControlCapabilityInfo ::= SEQUENCE
SRSASN_CODE trx_ctrl_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_trx_ctrl_masks, 1, 64, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, sleep_modes, 1, 64, true));
  HANDLE_CODE(bref.pack(defined_dur_sleep_supported, 1));
  HANDLE_CODE(bref.pack(undefined_dur_sleep_supported, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE trx_ctrl_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_trx_ctrl_masks, bref, 1, 64, true));
  HANDLE_CODE(unpack_dyn_seq_of(sleep_modes, bref, 1, 64, true));
  HANDLE_CODE(bref.unpack(defined_dur_sleep_supported, 1));
  HANDLE_CODE(bref.unpack(undefined_dur_sleep_supported, 1));

  return SRSASN_SUCCESS;
}
void trx_ctrl_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedTrxControlMasks");
  for (const auto& e1 : supported_trx_ctrl_masks) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("sleepModes");
  for (const auto& e1 : sleep_modes) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_bool("definedDurationSleepSupported", defined_dur_sleep_supported);
  j.write_bool("undefinedDurationSleepSupported", undefined_dur_sleep_supported);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const trx_ctrl_cap_info_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.supported_trx_ctrl_masks) {
      j_array.push_back(e1);
    }
    j["supportedTrxControlMasks"] = j_array;
  }
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.sleep_modes) {
      j_array.push_back(e1);
    }
    j["sleepModes"] = j_array;
  }
  j["definedDurationSleepSupported"]   = obj.defined_dur_sleep_supported;
  j["undefinedDurationSleepSupported"] = obj.undefined_dur_sleep_supported;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, trx_ctrl_cap_info_s& obj)
{
  {
    const auto& in_arr  = in_j.at("supportedTrxControlMasks");
    auto&       out_arr = obj.supported_trx_ctrl_masks;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<supported_trx_ctrl_mask_s>();
    }
  }
  {
    const auto& in_arr  = in_j.at("sleepModes");
    auto&       out_arr = obj.sleep_modes;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<sleep_mode_s>();
    }
  }
  in_j["definedDurationSleepSupported"].get_to(obj.defined_dur_sleep_supported);
  in_j["undefinedDurationSleepSupported"].get_to(obj.undefined_dur_sleep_supported);
}

// AdministrativeState ::= ENUMERATED
const char* administrative_state_opts::to_string() const
{
  static const char* names[] = {"LOCKED", "SHUTTINGDOWN", "UNLOCKED"};
  return convert_enum_idx(names, 3, value, "administrative_state_e");
}
bool administrative_state_opts::from_string(const std::string& str)
{
  static const char* names[] = {"LOCKED", "SHUTTINGDOWN", "UNLOCKED"};
  uint32_t           idx;
  if (convert_enum_str(names, 3, str.c_str(), idx, "administrative_state_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// CellState ::= ENUMERATED
const char* cell_state_opts::to_string() const
{
  static const char* names[] = {"IDLE", "INACTIVE", "ACTIVE"};
  return convert_enum_idx(names, 3, value, "cell_state_e");
}
bool cell_state_opts::from_string(const std::string& str)
{
  static const char* names[] = {"IDLE", "INACTIVE", "ACTIVE"};
  uint32_t           idx;
  if (convert_enum_str(names, 3, str.c_str(), idx, "cell_state_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// ControlStyle ::= SEQUENCE
SRSASN_CODE ctrl_style_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ric_call_process_id_format_type_present, 1));

  HANDLE_CODE(pack_unconstrained_integer(bref, ctrl_service_style_type, false, true));
  HANDLE_CODE(ctrl_service_style_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, ctrl_service_hdr_format_type, false, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, ctrl_service_msg_format_type, false, true));
  if (ric_call_process_id_format_type_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, ric_call_process_id_format_type, false, true));
  }
  HANDLE_CODE(pack_unconstrained_integer(bref, ctrl_service_ctrl_outcome_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ctrl_style_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ric_call_process_id_format_type_present, 1));

  HANDLE_CODE(unpack_unconstrained_integer(ctrl_service_style_type, bref, false, true));
  HANDLE_CODE(ctrl_service_style_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(ctrl_service_hdr_format_type, bref, false, true));
  HANDLE_CODE(unpack_unconstrained_integer(ctrl_service_msg_format_type, bref, false, true));
  if (ric_call_process_id_format_type_present) {
    HANDLE_CODE(unpack_unconstrained_integer(ric_call_process_id_format_type, bref, false, true));
  }
  HANDLE_CODE(unpack_unconstrained_integer(ctrl_service_ctrl_outcome_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void ctrl_style_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("controlServiceStyleType", ctrl_service_style_type);
  j.write_str("controlServiceStyleName", ctrl_service_style_name.to_string());
  j.write_int("controlServiceHeaderFormatType", ctrl_service_hdr_format_type);
  j.write_int("controlServiceMessageFormatType", ctrl_service_msg_format_type);
  if (ric_call_process_id_format_type_present) {
    j.write_int("ricCallProcessIDFormatType", ric_call_process_id_format_type);
  }
  j.write_int("controlServiceControlOutcomeFormatType", ctrl_service_ctrl_outcome_format_type);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ctrl_style_s& obj)
{
  nlohmann::ordered_json& j            = out_j;
  j["controlServiceStyleType"]         = obj.ctrl_service_style_type;
  j["controlServiceStyleName"]         = obj.ctrl_service_style_name.to_string();
  j["controlServiceHeaderFormatType"]  = obj.ctrl_service_hdr_format_type;
  j["controlServiceMessageFormatType"] = obj.ctrl_service_msg_format_type;
  if (obj.ric_call_process_id_format_type_present) {
    j["ricCallProcessIDFormatType"] = obj.ric_call_process_id_format_type;
  }
  j["controlServiceControlOutcomeFormatType"] = obj.ctrl_service_ctrl_outcome_format_type;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ctrl_style_s& obj)
{
  in_j["controlServiceStyleType"].get_to(obj.ctrl_service_style_type);
  obj.ctrl_service_style_name.from_string(in_j["controlServiceStyleName"].get<std::string>());
  in_j["controlServiceHeaderFormatType"].get_to(obj.ctrl_service_hdr_format_type);
  in_j["controlServiceMessageFormatType"].get_to(obj.ctrl_service_msg_format_type);
  if (in_j.contains("ricCallProcessIDFormatType")) {
    in_j["ricCallProcessIDFormatType"].get_to(obj.ric_call_process_id_format_type);
    obj.ric_call_process_id_format_type_present = true;
  }
  in_j["controlServiceControlOutcomeFormatType"].get_to(obj.ctrl_service_ctrl_outcome_format_type);
}

// CycleStartOffset ::= SEQUENCE
SRSASN_CODE cycle_start_offset_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(periodicity_present, 1));
  HANDLE_CODE(bref.pack(offset_present, 1));

  if (periodicity_present) {
    HANDLE_CODE(periodicity.pack(bref));
  }
  if (offset_present) {
    HANDLE_CODE(pack_integer(bref, offset, (uint16_t)0u, (uint16_t)10239u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cycle_start_offset_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(periodicity_present, 1));
  HANDLE_CODE(bref.unpack(offset_present, 1));

  if (periodicity_present) {
    HANDLE_CODE(periodicity.unpack(bref));
  }
  if (offset_present) {
    HANDLE_CODE(unpack_integer(offset, bref, (uint16_t)0u, (uint16_t)10239u, false, true));
  }

  return SRSASN_SUCCESS;
}
void cycle_start_offset_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (periodicity_present) {
    j.write_str("periodicity", periodicity.to_string());
  }
  if (offset_present) {
    j.write_int("offset", offset);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cycle_start_offset_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.periodicity_present) {
    j["periodicity"] = obj.periodicity.to_string();
  }
  if (obj.offset_present) {
    j["offset"] = obj.offset;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cycle_start_offset_s& obj)
{
  if (in_j.contains("periodicity")) {
    obj.periodicity.from_string(in_j["periodicity"].get<std::string>());
    obj.periodicity_present = true;
  }
  if (in_j.contains("offset")) {
    in_j["offset"].get_to(obj.offset);
    obj.offset_present = true;
  }
}

const char* cycle_start_offset_s::periodicity_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "cycle_start_offset_s::periodicity_e_");
}
bool cycle_start_offset_s::periodicity_opts::from_string(const std::string& str)
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  uint32_t           idx;
  if (convert_enum_str(names, 20, str.c_str(), idx, "cycle_start_offset_s::periodicity_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint16_t cycle_start_offset_s::periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "cycle_start_offset_s::periodicity_e_");
}

// EsObjective ::= CHOICE
void es_objive_c::destroy_()
{
  switch (type_) {
    case types::target_ec:
      c.destroy<target_ec_s_>();
      break;
    case types::es_percentage:
      c.destroy<es_percentage_s_>();
      break;
    default:
      break;
  }
}
void es_objive_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::target_ec:
      c.init<target_ec_s_>();
      break;
    case types::es_percentage:
      c.init<es_percentage_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "es_objive_c");
  }
}
es_objive_c::es_objive_c(const es_objive_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::target_ec:
      c.init(other.c.get<target_ec_s_>());
      break;
    case types::es_percentage:
      c.init(other.c.get<es_percentage_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "es_objive_c");
  }
}
es_objive_c& es_objive_c::operator=(const es_objive_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::target_ec:
      c.set(other.c.get<target_ec_s_>());
      break;
    case types::es_percentage:
      c.set(other.c.get<es_percentage_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "es_objive_c");
  }

  return *this;
}
es_objive_c::target_ec_s_& es_objive_c::set_target_ec()
{
  set(types::target_ec);
  return c.get<target_ec_s_>();
}
es_objive_c::es_percentage_s_& es_objive_c::set_es_percentage()
{
  set(types::es_percentage);
  return c.get<es_percentage_s_>();
}
void es_objive_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::target_ec:
      j.write_fieldname("target-Ec");
      j.start_obj();
      if (c.get<target_ec_s_>().target_ec_present) {
        j.write_fieldname("targetEc");
        c.get<target_ec_s_>().target_ec.to_json(j);
      }
      j.end_obj();
      break;
    case types::es_percentage:
      j.write_fieldname("es-Percentage");
      j.start_obj();
      if (c.get<es_percentage_s_>().es_percentage_present) {
        j.write_int("esPercentage", c.get<es_percentage_s_>().es_percentage);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "es_objive_c");
  }
  j.end_obj();
}
SRSASN_CODE es_objive_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::target_ec:
      HANDLE_CODE(bref.pack(c.get<target_ec_s_>().target_ec_present, 1));
      if (c.get<target_ec_s_>().target_ec_present) {
        HANDLE_CODE(c.get<target_ec_s_>().target_ec.pack(bref));
      }
      break;
    case types::es_percentage:
      HANDLE_CODE(bref.pack(c.get<es_percentage_s_>().es_percentage_present, 1));
      if (c.get<es_percentage_s_>().es_percentage_present) {
        HANDLE_CODE(
            pack_integer(bref, c.get<es_percentage_s_>().es_percentage, (uint8_t)0u, (uint8_t)100u, false, true));
      }
      break;
    default:
      log_invalid_choice_id(type_, "es_objive_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE es_objive_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::target_ec:
      HANDLE_CODE(bref.unpack(c.get<target_ec_s_>().target_ec_present, 1));
      if (c.get<target_ec_s_>().target_ec_present) {
        HANDLE_CODE(c.get<target_ec_s_>().target_ec.unpack(bref));
      }
      break;
    case types::es_percentage:
      HANDLE_CODE(bref.unpack(c.get<es_percentage_s_>().es_percentage_present, 1));
      if (c.get<es_percentage_s_>().es_percentage_present) {
        HANDLE_CODE(
            unpack_integer(c.get<es_percentage_s_>().es_percentage, bref, (uint8_t)0u, (uint8_t)100u, false, true));
      }
      break;
    default:
      log_invalid_choice_id(type_, "es_objive_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const es_objive_c& obj)
{
  nlohmann::ordered_json j = nlohmann::ordered_json::object();
  switch (obj.type()) {
    case es_objive_c::types::target_ec:
      if (obj.target_ec().target_ec_present) {
        j["targetEc"] = obj.target_ec().target_ec;
      }
      out_j = j;
      break;
    case es_objive_c::types::es_percentage:
      if (obj.es_percentage().es_percentage_present) {
        j["esPercentage"] = obj.es_percentage().es_percentage;
      }
      out_j = j;
      break;
    default:
      log_invalid_choice_id(obj.type(), "es_objive_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, es_objive_c& obj)
{
  if (in_j.find("targetEc") != in_j.end()) {
    obj.set_es_percentage();
    obj.es_percentage().es_percentage_present = true;
    auto& format                              = obj.es_percentage().es_percentage;
    in_j.get_to(format);
  } else if (in_j.find("esPercentage") != in_j.end()) {
    obj.set_target_ec();
    obj.target_ec().target_ec_present = true;
    auto& format                      = obj.target_ec().target_ec;
    in_j.get_to(format);
  }
}

const char* es_objive_c::types_opts::to_string() const
{
  static const char* names[] = {"target-Ec", "es-Percentage"};
  return convert_enum_idx(names, 2, value, "es_objive_c::types");
}
bool es_objive_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"target-Ec", "es-Percentage"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "es_objive_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// EventTriggerStyle ::= SEQUENCE
SRSASN_CODE event_trigger_style_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, event_trigger_style_type, false, true));
  HANDLE_CODE(event_trigger_style_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, event_trigger_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_style_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(event_trigger_style_type, bref, false, true));
  HANDLE_CODE(event_trigger_style_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(event_trigger_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void event_trigger_style_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eventTriggerStyleType", event_trigger_style_type);
  j.write_str("eventTriggerStyleName", event_trigger_style_name.to_string());
  j.write_int("eventTriggerFormatType", event_trigger_format_type);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const event_trigger_style_s& obj)
{
  nlohmann::ordered_json& j   = out_j;
  j["eventTriggerStyleType"]  = obj.event_trigger_style_type;
  j["eventTriggerStyleName"]  = obj.event_trigger_style_name.to_string();
  j["eventTriggerFormatType"] = obj.event_trigger_format_type;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, event_trigger_style_s& obj)
{
  in_j["eventTriggerStyleType"].get_to(obj.event_trigger_style_type);
  obj.event_trigger_style_name.from_string(in_j["eventTriggerStyleName"].get<std::string>());
  in_j["eventTriggerFormatType"].get_to(obj.event_trigger_format_type);
}

// O-Bwp ::= SEQUENCE
SRSASN_CODE o_bwp_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(bwp_context_present, 1));
  HANDLE_CODE(bref.pack(is_init_bwp_present, 1));
  HANDLE_CODE(bref.pack(sub_carrier_spacing_present, 1));
  HANDLE_CODE(bref.pack(cp_present, 1));
  HANDLE_CODE(bref.pack(start_rb_present, 1));
  HANDLE_CODE(bref.pack(nof_rbs_present, 1));

  if (bwp_context_present) {
    HANDLE_CODE(bwp_context.pack(bref));
  }
  if (is_init_bwp_present) {
    HANDLE_CODE(is_init_bwp.pack(bref));
  }
  if (sub_carrier_spacing_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, sub_carrier_spacing, false, true));
  }
  if (cp_present) {
    HANDLE_CODE(cp.pack(bref));
  }
  if (start_rb_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, start_rb, false, true));
  }
  if (nof_rbs_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, nof_rbs, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_bwp_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(bwp_context_present, 1));
  HANDLE_CODE(bref.unpack(is_init_bwp_present, 1));
  HANDLE_CODE(bref.unpack(sub_carrier_spacing_present, 1));
  HANDLE_CODE(bref.unpack(cp_present, 1));
  HANDLE_CODE(bref.unpack(start_rb_present, 1));
  HANDLE_CODE(bref.unpack(nof_rbs_present, 1));

  if (bwp_context_present) {
    HANDLE_CODE(bwp_context.unpack(bref));
  }
  if (is_init_bwp_present) {
    HANDLE_CODE(is_init_bwp.unpack(bref));
  }
  if (sub_carrier_spacing_present) {
    HANDLE_CODE(unpack_unconstrained_integer(sub_carrier_spacing, bref, false, true));
  }
  if (cp_present) {
    HANDLE_CODE(cp.unpack(bref));
  }
  if (start_rb_present) {
    HANDLE_CODE(unpack_unconstrained_integer(start_rb, bref, false, true));
  }
  if (nof_rbs_present) {
    HANDLE_CODE(unpack_unconstrained_integer(nof_rbs, bref, false, true));
  }

  return SRSASN_SUCCESS;
}
void o_bwp_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bwp_context_present) {
    j.write_str("bwpContext", bwp_context.to_string());
  }
  if (is_init_bwp_present) {
    j.write_str("isInitialBwp", is_init_bwp.to_string());
  }
  if (sub_carrier_spacing_present) {
    j.write_int("subCarrierSpacing", sub_carrier_spacing);
  }
  if (cp_present) {
    j.write_str("cyclicPrefix", cp.to_string());
  }
  if (start_rb_present) {
    j.write_int("startRB", start_rb);
  }
  if (nof_rbs_present) {
    j.write_int("numberOfRBs", nof_rbs);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_bwp_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.bwp_context_present) {
    j["bwpContext"] = obj.bwp_context.to_string();
  }
  if (obj.is_init_bwp_present) {
    j["isInitialBwp"] = obj.is_init_bwp.to_string();
  }
  if (obj.sub_carrier_spacing_present) {
    j["subCarrierSpacing"] = obj.sub_carrier_spacing;
  }
  if (obj.cp_present) {
    j["cyclicPrefix"] = obj.cp.to_string();
  }
  if (obj.start_rb_present) {
    j["startRB"] = obj.start_rb;
  }
  if (obj.nof_rbs_present) {
    j["numberOfRBs"] = obj.nof_rbs;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_bwp_s& obj)
{
  if (in_j.contains("bwpContext")) {
    obj.bwp_context.from_string(in_j["bwpContext"].get<std::string>());
    obj.bwp_context_present = true;
  }
  if (in_j.contains("isInitialBwp")) {
    obj.is_init_bwp.from_string(in_j["isInitialBwp"].get<std::string>());
    obj.is_init_bwp_present = true;
  }
  if (in_j.contains("subCarrierSpacing")) {
    in_j["subCarrierSpacing"].get_to(obj.sub_carrier_spacing);
    obj.sub_carrier_spacing_present = true;
  }
  if (in_j.contains("cyclicPrefix")) {
    obj.cp.from_string(in_j["cyclicPrefix"].get<std::string>());
    obj.cp_present = true;
  }
  if (in_j.contains("startRB")) {
    in_j["startRB"].get_to(obj.start_rb);
    obj.start_rb_present = true;
  }
  if (in_j.contains("numberOfRBs")) {
    in_j["numberOfRBs"].get_to(obj.nof_rbs);
    obj.nof_rbs_present = true;
  }
}

// OnDurationTimer ::= CHOICE
void on_dur_timer_c::destroy_() {}
void on_dur_timer_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
on_dur_timer_c::on_dur_timer_c(const on_dur_timer_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::integer:
      c.init(other.c.get<uint8_t>());
      break;
    case types::timeout:
      c.init(other.c.get<timeout_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "on_dur_timer_c");
  }
}
on_dur_timer_c& on_dur_timer_c::operator=(const on_dur_timer_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::integer:
      c.set(other.c.get<uint8_t>());
      break;
    case types::timeout:
      c.set(other.c.get<timeout_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "on_dur_timer_c");
  }

  return *this;
}
uint8_t& on_dur_timer_c::set_integer()
{
  set(types::integer);
  return c.get<uint8_t>();
}
on_dur_timer_c::timeout_e_& on_dur_timer_c::set_timeout()
{
  set(types::timeout);
  return c.get<timeout_e_>();
}
void on_dur_timer_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::integer:
      j.write_int("integer", c.get<uint8_t>());
      break;
    case types::timeout:
      j.write_str("timeout", c.get<timeout_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "on_dur_timer_c");
  }
  j.end_obj();
}
SRSASN_CODE on_dur_timer_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::integer:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)31u, false, true));
      break;
    case types::timeout:
      HANDLE_CODE(c.get<timeout_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "on_dur_timer_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE on_dur_timer_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::integer:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)31u, false, true));
      break;
    case types::timeout:
      HANDLE_CODE(c.get<timeout_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "on_dur_timer_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const on_dur_timer_c& obj)
{
  switch (obj.type()) {
    case on_dur_timer_c::types::integer:
      out_j = obj.integer();
      break;
    case on_dur_timer_c::types::timeout:
      out_j = obj.timeout().to_string();
      break;
    default:
      log_invalid_choice_id(obj.type(), "on_dur_timer_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, on_dur_timer_c& obj)
{
  if (in_j.is_number()) {
    obj.set_integer();
    obj.integer() = in_j.get<std::uint8_t>();
  } else if (in_j.is_string()) {
    obj.set_timeout();
    obj.timeout().from_string(in_j.get<std::string>());
  }
}

const char* on_dur_timer_c::timeout_opts::to_string() const
{
  static const char* names[] = {"ms1",   "ms2",   "ms3",   "ms4",   "ms5",    "ms6",    "ms8",   "ms10",
                                "ms20",  "ms40",  "ms50",  "ms60",  "ms80",   "ms100",  "ms200", "ms300",
                                "ms400", "ms500", "ms600", "ms800", "ms1000", "ms1200", "ms1600"};
  return convert_enum_idx(names, 23, value, "on_dur_timer_c::timeout_e_");
}
bool on_dur_timer_c::timeout_opts::from_string(const std::string& str)
{
  static const char* names[] = {"ms1",   "ms2",   "ms3",   "ms4",   "ms5",    "ms6",    "ms8",   "ms10",
                                "ms20",  "ms40",  "ms50",  "ms60",  "ms80",   "ms100",  "ms200", "ms300",
                                "ms400", "ms500", "ms600", "ms800", "ms1000", "ms1200", "ms1600"};
  uint32_t           idx;
  if (convert_enum_str(names, 23, str.c_str(), idx, "on_dur_timer_c::timeout_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint16_t on_dur_timer_c::timeout_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,   3,   4,   5,   6,   8,   10,  20,   40,   50,  60,
                                     80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 23, value, "on_dur_timer_c::timeout_e_");
}

const char* on_dur_timer_c::types_opts::to_string() const
{
  static const char* names[] = {"integer", "timeout"};
  return convert_enum_idx(names, 2, value, "on_dur_timer_c::types");
}
bool on_dur_timer_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"integer", "timeout"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "on_dur_timer_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// OperationalState ::= ENUMERATED
const char* operational_state_opts::to_string() const
{
  static const char* names[] = {"ENABLED", "DISABLED"};
  return convert_enum_idx(names, 2, value, "operational_state_e");
}
bool operational_state_opts::from_string(const std::string& str)
{
  static const char* names[] = {"ENABLED", "DISABLED"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "operational_state_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// OruCapabilities ::= SEQUENCE
SRSASN_CODE oru_cap_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(energy_saving_cap_common_info_present, 1));
  HANDLE_CODE(bref.pack(asm_cap_info_present, 1));
  HANDLE_CODE(bref.pack(trx_ctrl_cap_info_present, 1));

  if (energy_saving_cap_common_info_present) {
    HANDLE_CODE(energy_saving_cap_common_info.pack(bref));
  }
  if (asm_cap_info_present) {
    HANDLE_CODE(asm_cap_info.pack(bref));
  }
  if (trx_ctrl_cap_info_present) {
    HANDLE_CODE(trx_ctrl_cap_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE oru_cap_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(energy_saving_cap_common_info_present, 1));
  HANDLE_CODE(bref.unpack(asm_cap_info_present, 1));
  HANDLE_CODE(bref.unpack(trx_ctrl_cap_info_present, 1));

  if (energy_saving_cap_common_info_present) {
    HANDLE_CODE(energy_saving_cap_common_info.unpack(bref));
  }
  if (asm_cap_info_present) {
    HANDLE_CODE(asm_cap_info.unpack(bref));
  }
  if (trx_ctrl_cap_info_present) {
    HANDLE_CODE(trx_ctrl_cap_info.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void oru_cap_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (energy_saving_cap_common_info_present) {
    j.write_fieldname("energySavingCapabilityCommonInfo");
    energy_saving_cap_common_info.to_json(j);
  }
  if (asm_cap_info_present) {
    j.write_fieldname("asmCapabilityInfo");
    asm_cap_info.to_json(j);
  }
  if (trx_ctrl_cap_info_present) {
    j.write_fieldname("trxControlCapabilityInfo");
    trx_ctrl_cap_info.to_json(j);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const oru_cap_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.energy_saving_cap_common_info_present) {
    j["energySavingCapabilityCommonInfo"] = obj.energy_saving_cap_common_info;
  }
  if (obj.asm_cap_info_present) {
    j["asmCapabilityInfo"] = obj.asm_cap_info;
  }
  if (obj.trx_ctrl_cap_info_present) {
    j["trxControlCapabilityInfo"] = obj.trx_ctrl_cap_info;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, oru_cap_s& obj)
{
  if (in_j.contains("energySavingCapabilityCommonInfo")) {
    in_j["energySavingCapabilityCommonInfo"].get_to(obj.energy_saving_cap_common_info);
    obj.energy_saving_cap_common_info_present = true;
  }
  if (in_j.contains("asmCapabilityInfo")) {
    in_j["asmCapabilityInfo"].get_to(obj.asm_cap_info);
    obj.asm_cap_info_present = true;
  }
  if (in_j.contains("trxControlCapabilityInfo")) {
    in_j["trxControlCapabilityInfo"].get_to(obj.trx_ctrl_cap_info);
    obj.trx_ctrl_cap_info_present = true;
  }
}

// OruUserPlaneConfiguration ::= SEQUENCE
SRSASN_CODE oru_user_plane_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_array_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(rx_array_list.size() > 0, 1));

  if (tx_array_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tx_array_list, 1, 64, true));
  }
  if (rx_array_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rx_array_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE oru_user_plane_cfg_s::unpack(cbit_ref& bref)
{
  bool tx_array_list_present;
  HANDLE_CODE(bref.unpack(tx_array_list_present, 1));
  bool rx_array_list_present;
  HANDLE_CODE(bref.unpack(rx_array_list_present, 1));

  if (tx_array_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tx_array_list, bref, 1, 64, true));
  }
  if (rx_array_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rx_array_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void oru_user_plane_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_array_list.size() > 0) {
    j.start_array("txArrayList");
    for (const auto& e1 : tx_array_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rx_array_list.size() > 0) {
    j.start_array("rxArrayList");
    for (const auto& e1 : rx_array_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const oru_user_plane_cfg_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.tx_array_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.tx_array_list) {
        j_array.push_back(e1);
      }
      j["txArrayList"] = j_array;
    }
  }
  if (obj.rx_array_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.rx_array_list) {
        j_array.push_back(e1);
      }
      j["rxArrayList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, oru_user_plane_cfg_s& obj)
{
  if (in_j.contains("txArrayList")) {
    {
      const auto& in_arr  = in_j.at("txArrayList");
      auto&       out_arr = obj.tx_array_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<tx_array_item_s>();
      }
    }
  }
  if (in_j.contains("rxArrayList")) {
    {
      const auto& in_arr  = in_j.at("rxArrayList");
      auto&       out_arr = obj.rx_array_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<rx_array_item_s>();
      }
    }
  }
}

// QueryStyle ::= SEQUENCE
SRSASN_CODE query_style_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, query_service_style_type, false, true));
  HANDLE_CODE(query_service_style_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, query_service_hdr_format_type, false, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, query_service_definition_format_type, false, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, query_service_outcome_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE query_style_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(query_service_style_type, bref, false, true));
  HANDLE_CODE(query_service_style_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(query_service_hdr_format_type, bref, false, true));
  HANDLE_CODE(unpack_unconstrained_integer(query_service_definition_format_type, bref, false, true));
  HANDLE_CODE(unpack_unconstrained_integer(query_service_outcome_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void query_style_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("queryServiceStyleType", query_service_style_type);
  j.write_str("queryServiceStyleName", query_service_style_name.to_string());
  j.write_int("queryServiceHeaderFormatType", query_service_hdr_format_type);
  j.write_int("queryServiceDefinitionFormatType", query_service_definition_format_type);
  j.write_int("queryServiceOutcomeFormatType", query_service_outcome_format_type);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const query_style_s& obj)
{
  nlohmann::ordered_json& j             = out_j;
  j["queryServiceStyleType"]            = obj.query_service_style_type;
  j["queryServiceStyleName"]            = obj.query_service_style_name.to_string();
  j["queryServiceHeaderFormatType"]     = obj.query_service_hdr_format_type;
  j["queryServiceDefinitionFormatType"] = obj.query_service_definition_format_type;
  j["queryServiceOutcomeFormatType"]    = obj.query_service_outcome_format_type;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, query_style_s& obj)
{
  in_j["queryServiceStyleType"].get_to(obj.query_service_style_type);
  obj.query_service_style_name.from_string(in_j["queryServiceStyleName"].get<std::string>());
  in_j["queryServiceHeaderFormatType"].get_to(obj.query_service_hdr_format_type);
  in_j["queryServiceDefinitionFormatType"].get_to(obj.query_service_definition_format_type);
  in_j["queryServiceOutcomeFormatType"].get_to(obj.query_service_outcome_format_type);
}

// ReportStyle ::= SEQUENCE
SRSASN_CODE report_style_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, report_service_style_type, false, true));
  HANDLE_CODE(report_service_style_name.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_event_trigger_styles_for_report_style, 1, 64, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, report_service_action_definition_format_type, false, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, report_service_ind_hdr_format_type, false, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, report_service_ind_msg_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_style_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(report_service_style_type, bref, false, true));
  HANDLE_CODE(report_service_style_name.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_event_trigger_styles_for_report_style, bref, 1, 64, true));
  HANDLE_CODE(unpack_unconstrained_integer(report_service_action_definition_format_type, bref, false, true));
  HANDLE_CODE(unpack_unconstrained_integer(report_service_ind_hdr_format_type, bref, false, true));
  HANDLE_CODE(unpack_unconstrained_integer(report_service_ind_msg_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void report_style_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reportServiceStyleType", report_service_style_type);
  j.write_str("reportServiceStyleName", report_service_style_name.to_string());
  j.start_array("listOfSupportedEventTriggerStylesForReportStyle");
  for (const auto& e1 : list_of_supported_event_trigger_styles_for_report_style) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("reportServiceActionDefinitionFormatType", report_service_action_definition_format_type);
  j.write_int("reportServiceIndicationHeaderFormatType", report_service_ind_hdr_format_type);
  j.write_int("reportServiceIndicationMessageFormatType", report_service_ind_msg_format_type);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const report_style_s& obj)
{
  nlohmann::ordered_json& j   = out_j;
  j["reportServiceStyleType"] = obj.report_service_style_type;
  j["reportServiceStyleName"] = obj.report_service_style_name.to_string();
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_supported_event_trigger_styles_for_report_style) {
      j_array.push_back(e1);
    }
    j["listOfSupportedEventTriggerStylesForReportStyle"] = j_array;
  }
  j["reportServiceActionDefinitionFormatType"]  = obj.report_service_action_definition_format_type;
  j["reportServiceIndicationHeaderFormatType"]  = obj.report_service_ind_hdr_format_type;
  j["reportServiceIndicationMessageFormatType"] = obj.report_service_ind_msg_format_type;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, report_style_s& obj)
{
  in_j["reportServiceStyleType"].get_to(obj.report_service_style_type);
  obj.report_service_style_name.from_string(in_j["reportServiceStyleName"].get<std::string>());
  {
    const auto& in_arr  = in_j.at("listOfSupportedEventTriggerStylesForReportStyle");
    auto&       out_arr = obj.list_of_supported_event_trigger_styles_for_report_style;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<event_trigger_style_type_s>();
    }
  }
  in_j["reportServiceActionDefinitionFormatType"].get_to(obj.report_service_action_definition_format_type);
  in_j["reportServiceIndicationHeaderFormatType"].get_to(obj.report_service_ind_hdr_format_type);
  in_j["reportServiceIndicationMessageFormatType"].get_to(obj.report_service_ind_msg_format_type);
}

// ResourceType ::= ENUMERATED
const char* res_type_opts::to_string() const
{
  static const char* names[] = {"PRB_DL", "PRB_UL", "DRB", "RRC"};
  return convert_enum_idx(names, 4, value, "res_type_e");
}
bool res_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"PRB_DL", "PRB_UL", "DRB", "RRC"};
  uint32_t           idx;
  if (convert_enum_str(names, 4, str.c_str(), idx, "res_type_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// O-CellDTXDRXConfig ::= SEQUENCE
SRSASN_CODE o_cell_dtxdrx_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(on_dur_timer_present, 1));
  HANDLE_CODE(bref.pack(cycle_start_offset_present, 1));
  HANDLE_CODE(bref.pack(slot_offset_present, 1));
  HANDLE_CODE(bref.pack(cfg_type_present, 1));
  HANDLE_CODE(bref.pack(activation_status_present, 1));
  HANDLE_CODE(bref.pack(l1_activation_present, 1));

  if (on_dur_timer_present) {
    HANDLE_CODE(on_dur_timer.pack(bref));
  }
  if (cycle_start_offset_present) {
    HANDLE_CODE(cycle_start_offset.pack(bref));
  }
  if (slot_offset_present) {
    HANDLE_CODE(pack_integer(bref, slot_offset, (uint8_t)0u, (uint8_t)31u, false, true));
  }
  if (cfg_type_present) {
    HANDLE_CODE(cfg_type.pack(bref));
  }
  if (activation_status_present) {
    HANDLE_CODE(activation_status.pack(bref));
  }
  if (l1_activation_present) {
    HANDLE_CODE(l1_activation.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_cell_dtxdrx_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(on_dur_timer_present, 1));
  HANDLE_CODE(bref.unpack(cycle_start_offset_present, 1));
  HANDLE_CODE(bref.unpack(slot_offset_present, 1));
  HANDLE_CODE(bref.unpack(cfg_type_present, 1));
  HANDLE_CODE(bref.unpack(activation_status_present, 1));
  HANDLE_CODE(bref.unpack(l1_activation_present, 1));

  if (on_dur_timer_present) {
    HANDLE_CODE(on_dur_timer.unpack(bref));
  }
  if (cycle_start_offset_present) {
    HANDLE_CODE(cycle_start_offset.unpack(bref));
  }
  if (slot_offset_present) {
    HANDLE_CODE(unpack_integer(slot_offset, bref, (uint8_t)0u, (uint8_t)31u, false, true));
  }
  if (cfg_type_present) {
    HANDLE_CODE(cfg_type.unpack(bref));
  }
  if (activation_status_present) {
    HANDLE_CODE(activation_status.unpack(bref));
  }
  if (l1_activation_present) {
    HANDLE_CODE(l1_activation.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void o_cell_dtxdrx_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (on_dur_timer_present) {
    j.write_fieldname("onDurationTimer");
    on_dur_timer.to_json(j);
  }
  if (cycle_start_offset_present) {
    j.write_fieldname("cycleStartOffset");
    cycle_start_offset.to_json(j);
  }
  if (slot_offset_present) {
    j.write_int("slotOffset", slot_offset);
  }
  if (cfg_type_present) {
    j.write_str("configType", cfg_type.to_string());
  }
  if (activation_status_present) {
    j.write_str("activationStatus", activation_status.to_string());
  }
  if (l1_activation_present) {
    j.write_str("l1Activation", l1_activation.to_string());
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_cell_dtxdrx_cfg_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.on_dur_timer_present) {
    j["onDurationTimer"] = obj.on_dur_timer;
  }
  if (obj.cycle_start_offset_present) {
    j["cycleStartOffset"] = obj.cycle_start_offset;
  }
  if (obj.slot_offset_present) {
    j["slotOffset"] = obj.slot_offset;
  }
  if (obj.cfg_type_present) {
    j["configType"] = obj.cfg_type.to_string();
  }
  if (obj.activation_status_present) {
    j["activationStatus"] = obj.activation_status.to_string();
  }
  if (obj.l1_activation_present) {
    j["l1Activation"] = obj.l1_activation.to_string();
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_cell_dtxdrx_cfg_s& obj)
{
  if (in_j.contains("onDurationTimer")) {
    in_j["onDurationTimer"].get_to(obj.on_dur_timer);
    obj.on_dur_timer_present = true;
  }
  if (in_j.contains("cycleStartOffset")) {
    in_j["cycleStartOffset"].get_to(obj.cycle_start_offset);
    obj.cycle_start_offset_present = true;
  }
  if (in_j.contains("slotOffset")) {
    in_j["slotOffset"].get_to(obj.slot_offset);
    obj.slot_offset_present = true;
  }
  if (in_j.contains("configType")) {
    obj.cfg_type.from_string(in_j["configType"].get<std::string>());
    obj.cfg_type_present = true;
  }
  if (in_j.contains("activationStatus")) {
    obj.activation_status.from_string(in_j["activationStatus"].get<std::string>());
    obj.activation_status_present = true;
  }
  if (in_j.contains("l1Activation")) {
    obj.l1_activation.from_string(in_j["l1Activation"].get<std::string>());
    obj.l1_activation_present = true;
  }
}

const char* o_cell_dtxdrx_cfg_s::cfg_type_opts::to_string() const
{
  static const char* names[] = {"DTX", "DRX", "DTXDRX"};
  return convert_enum_idx(names, 3, value, "o_cell_dtxdrx_cfg_s::cfg_type_e_");
}
bool o_cell_dtxdrx_cfg_s::cfg_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"DTX", "DRX", "DTXDRX"};
  uint32_t           idx;
  if (convert_enum_str(names, 3, str.c_str(), idx, "o_cell_dtxdrx_cfg_s::cfg_type_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

const char* o_cell_dtxdrx_cfg_s::activation_status_opts::to_string() const
{
  static const char* names[] = {"ACTIVATED", "DEACTIVATED"};
  return convert_enum_idx(names, 2, value, "o_cell_dtxdrx_cfg_s::activation_status_e_");
}
bool o_cell_dtxdrx_cfg_s::activation_status_opts::from_string(const std::string& str)
{
  static const char* names[] = {"ACTIVATED", "DEACTIVATED"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_cell_dtxdrx_cfg_s::activation_status_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

const char* o_cell_dtxdrx_cfg_s::l1_activation_opts::to_string() const
{
  static const char* names[] = {"ENABLED", "DISABLED"};
  return convert_enum_idx(names, 2, value, "o_cell_dtxdrx_cfg_s::l1_activation_e_");
}
bool o_cell_dtxdrx_cfg_s::l1_activation_opts::from_string(const std::string& str)
{
  static const char* names[] = {"ENABLED", "DISABLED"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_cell_dtxdrx_cfg_s::l1_activation_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// O-CESManagementFunction ::= SEQUENCE
SRSASN_CODE o_ces_management_function_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ces_switch_present, 1));
  HANDLE_CODE(bref.pack(energy_saving_state_present, 1));
  HANDLE_CODE(bref.pack(energy_saving_ctrl_present, 1));

  if (ces_switch_present) {
    HANDLE_CODE(bref.pack(ces_switch, 1));
  }
  if (energy_saving_state_present) {
    HANDLE_CODE(energy_saving_state.pack(bref));
  }
  if (energy_saving_ctrl_present) {
    HANDLE_CODE(energy_saving_ctrl.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_ces_management_function_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ces_switch_present, 1));
  HANDLE_CODE(bref.unpack(energy_saving_state_present, 1));
  HANDLE_CODE(bref.unpack(energy_saving_ctrl_present, 1));

  if (ces_switch_present) {
    HANDLE_CODE(bref.unpack(ces_switch, 1));
  }
  if (energy_saving_state_present) {
    HANDLE_CODE(energy_saving_state.unpack(bref));
  }
  if (energy_saving_ctrl_present) {
    HANDLE_CODE(energy_saving_ctrl.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void o_ces_management_function_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ces_switch_present) {
    j.write_bool("cesSwitch", ces_switch);
  }
  if (energy_saving_state_present) {
    j.write_str("energySavingState", energy_saving_state.to_string());
  }
  if (energy_saving_ctrl_present) {
    j.write_str("energySavingControl", energy_saving_ctrl.to_string());
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_ces_management_function_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.ces_switch_present) {
    j["cesSwitch"] = obj.ces_switch;
  }
  if (obj.energy_saving_state_present) {
    j["energySavingState"] = obj.energy_saving_state.to_string();
  }
  if (obj.energy_saving_ctrl_present) {
    j["energySavingControl"] = obj.energy_saving_ctrl.to_string();
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_ces_management_function_s& obj)
{
  if (in_j.contains("cesSwitch")) {
    in_j["cesSwitch"].get_to(obj.ces_switch);
    obj.ces_switch_present = true;
  }
  if (in_j.contains("energySavingState")) {
    obj.energy_saving_state.from_string(in_j["energySavingState"].get<std::string>());
    obj.energy_saving_state_present = true;
  }
  if (in_j.contains("energySavingControl")) {
    obj.energy_saving_ctrl.from_string(in_j["energySavingControl"].get<std::string>());
    obj.energy_saving_ctrl_present = true;
  }
}

const char* o_ces_management_function_s::energy_saving_state_opts::to_string() const
{
  static const char* names[] = {"isNotEnergySaving", "isEnergySaving"};
  return convert_enum_idx(names, 2, value, "o_ces_management_function_s::energy_saving_state_e_");
}
bool o_ces_management_function_s::energy_saving_state_opts::from_string(const std::string& str)
{
  static const char* names[] = {"isNotEnergySaving", "isEnergySaving"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_ces_management_function_s::energy_saving_state_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

const char* o_ces_management_function_s::energy_saving_ctrl_opts::to_string() const
{
  static const char* names[] = {"toBeEnergySaving", "toBeNotEnergySaving"};
  return convert_enum_idx(names, 2, value, "o_ces_management_function_s::energy_saving_ctrl_e_");
}
bool o_ces_management_function_s::energy_saving_ctrl_opts::from_string(const std::string& str)
{
  static const char* names[] = {"toBeEnergySaving", "toBeNotEnergySaving"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_ces_management_function_s::energy_saving_ctrl_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// O-GnbCuCpFunction ::= SEQUENCE
SRSASN_CODE o_gnb_cu_cp_function_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gnb_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_id_len_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_name_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_present, 1));
  HANDLE_CODE(bref.pack(x2_block_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(xn_block_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(x2_allow_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(xn_allow_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(x2_ho_block_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(xn_ho_block_list.size() > 0, 1));

  if (gnb_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_id_len_present) {
    HANDLE_CODE(pack_integer(bref, gnb_id_len, (uint8_t)22u, (uint8_t)32u, false, true));
  }
  if (gnb_cu_name_present) {
    HANDLE_CODE(gnb_cu_name.pack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  if (x2_block_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, x2_block_list, 1, 64, true));
  }
  if (xn_block_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, xn_block_list, 1, 64, true));
  }
  if (x2_allow_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, x2_allow_list, 1, 64, true));
  }
  if (xn_allow_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, xn_allow_list, 1, 64, true));
  }
  if (x2_ho_block_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, x2_ho_block_list, 1, 64, true));
  }
  if (xn_ho_block_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, xn_ho_block_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_gnb_cu_cp_function_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gnb_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_id_len_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_name_present, 1));
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));
  bool x2_block_list_present;
  HANDLE_CODE(bref.unpack(x2_block_list_present, 1));
  bool xn_block_list_present;
  HANDLE_CODE(bref.unpack(xn_block_list_present, 1));
  bool x2_allow_list_present;
  HANDLE_CODE(bref.unpack(x2_allow_list_present, 1));
  bool xn_allow_list_present;
  HANDLE_CODE(bref.unpack(xn_allow_list_present, 1));
  bool x2_ho_block_list_present;
  HANDLE_CODE(bref.unpack(x2_ho_block_list_present, 1));
  bool xn_ho_block_list_present;
  HANDLE_CODE(bref.unpack(xn_ho_block_list_present, 1));

  if (gnb_id_present) {
    HANDLE_CODE(unpack_integer(gnb_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_id_len_present) {
    HANDLE_CODE(unpack_integer(gnb_id_len, bref, (uint8_t)22u, (uint8_t)32u, false, true));
  }
  if (gnb_cu_name_present) {
    HANDLE_CODE(gnb_cu_name.unpack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  if (x2_block_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(x2_block_list, bref, 1, 64, true));
  }
  if (xn_block_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(xn_block_list, bref, 1, 64, true));
  }
  if (x2_allow_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(x2_allow_list, bref, 1, 64, true));
  }
  if (xn_allow_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(xn_allow_list, bref, 1, 64, true));
  }
  if (x2_ho_block_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(x2_ho_block_list, bref, 1, 64, true));
  }
  if (xn_ho_block_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(xn_ho_block_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void o_gnb_cu_cp_function_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnb_id_present) {
    j.write_int("gnbId", gnb_id);
  }
  if (gnb_id_len_present) {
    j.write_int("gnbIdLength", gnb_id_len);
  }
  if (gnb_cu_name_present) {
    j.write_str("gnbCuName", gnb_cu_name.to_string());
  }
  if (plmn_id_present) {
    j.write_fieldname("plmnId");
    plmn_id.to_json(j);
  }
  if (x2_block_list.size() > 0) {
    j.start_array("x2BlockList");
    for (const auto& e1 : x2_block_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (xn_block_list.size() > 0) {
    j.start_array("xnBlockList");
    for (const auto& e1 : xn_block_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (x2_allow_list.size() > 0) {
    j.start_array("x2AllowList");
    for (const auto& e1 : x2_allow_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (xn_allow_list.size() > 0) {
    j.start_array("xnAllowList");
    for (const auto& e1 : xn_allow_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (x2_ho_block_list.size() > 0) {
    j.start_array("x2HOBlockList");
    for (const auto& e1 : x2_ho_block_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (xn_ho_block_list.size() > 0) {
    j.start_array("xnHOBlockList");
    for (const auto& e1 : xn_ho_block_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_gnb_cu_cp_function_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.gnb_id_present) {
    j["gnbId"] = obj.gnb_id;
  }
  if (obj.gnb_id_len_present) {
    j["gnbIdLength"] = obj.gnb_id_len;
  }
  if (obj.gnb_cu_name_present) {
    j["gnbCuName"] = obj.gnb_cu_name.to_string();
  }
  if (obj.plmn_id_present) {
    j["plmnId"] = obj.plmn_id;
  }
  if (obj.x2_block_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.x2_block_list) {
        j_array.push_back(e1.to_string());
      }
      j["x2BlockList"] = j_array;
    }
  }
  if (obj.xn_block_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.xn_block_list) {
        j_array.push_back(e1.to_string());
      }
      j["xnBlockList"] = j_array;
    }
  }
  if (obj.x2_allow_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.x2_allow_list) {
        j_array.push_back(e1.to_string());
      }
      j["x2AllowList"] = j_array;
    }
  }
  if (obj.xn_allow_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.xn_allow_list) {
        j_array.push_back(e1.to_string());
      }
      j["xnAllowList"] = j_array;
    }
  }
  if (obj.x2_ho_block_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.x2_ho_block_list) {
        j_array.push_back(e1.to_string());
      }
      j["x2HOBlockList"] = j_array;
    }
  }
  if (obj.xn_ho_block_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.xn_ho_block_list) {
        j_array.push_back(e1.to_string());
      }
      j["xnHOBlockList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_gnb_cu_cp_function_s& obj)
{
  if (in_j.contains("gnbId")) {
    in_j["gnbId"].get_to(obj.gnb_id);
    obj.gnb_id_present = true;
  }
  if (in_j.contains("gnbIdLength")) {
    in_j["gnbIdLength"].get_to(obj.gnb_id_len);
    obj.gnb_id_len_present = true;
  }
  if (in_j.contains("gnbCuName")) {
    obj.gnb_cu_name.from_string(in_j["gnbCuName"].get<std::string>());
    obj.gnb_cu_name_present = true;
  }
  if (in_j.contains("plmnId")) {
    in_j["plmnId"].get_to(obj.plmn_id);
    obj.plmn_id_present = true;
  }
  if (in_j.contains("x2BlockList")) {
    {
      const auto& in_arr  = in_j.at("x2BlockList");
      auto&       out_arr = obj.x2_block_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i].from_string(in_arr[i].get<std::string>());
      }
    }
  }
  if (in_j.contains("xnBlockList")) {
    {
      const auto& in_arr  = in_j.at("xnBlockList");
      auto&       out_arr = obj.xn_block_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i].from_string(in_arr[i].get<std::string>());
      }
    }
  }
  if (in_j.contains("x2AllowList")) {
    {
      const auto& in_arr  = in_j.at("x2AllowList");
      auto&       out_arr = obj.x2_allow_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i].from_string(in_arr[i].get<std::string>());
      }
    }
  }
  if (in_j.contains("xnAllowList")) {
    {
      const auto& in_arr  = in_j.at("xnAllowList");
      auto&       out_arr = obj.xn_allow_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i].from_string(in_arr[i].get<std::string>());
      }
    }
  }
  if (in_j.contains("x2HOBlockList")) {
    {
      const auto& in_arr  = in_j.at("x2HOBlockList");
      auto&       out_arr = obj.x2_ho_block_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i].from_string(in_arr[i].get<std::string>());
      }
    }
  }
  if (in_j.contains("xnHOBlockList")) {
    {
      const auto& in_arr  = in_j.at("xnHOBlockList");
      auto&       out_arr = obj.xn_ho_block_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i].from_string(in_arr[i].get<std::string>());
      }
    }
  }
}

// O-GnbCuUpFunction ::= SEQUENCE
SRSASN_CODE o_gnb_cu_up_function_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gnb_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_id_len_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_up_id_present, 1));
  HANDLE_CODE(bref.pack(plmn_info_list.size() > 0, 1));

  if (gnb_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_id_len_present) {
    HANDLE_CODE(pack_integer(bref, gnb_id_len, (uint8_t)22u, (uint8_t)32u, false, true));
  }
  if (gnb_cu_up_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (plmn_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_info_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_gnb_cu_up_function_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gnb_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_id_len_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_up_id_present, 1));
  bool plmn_info_list_present;
  HANDLE_CODE(bref.unpack(plmn_info_list_present, 1));

  if (gnb_id_present) {
    HANDLE_CODE(unpack_integer(gnb_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_id_len_present) {
    HANDLE_CODE(unpack_integer(gnb_id_len, bref, (uint8_t)22u, (uint8_t)32u, false, true));
  }
  if (gnb_cu_up_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (plmn_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_info_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void o_gnb_cu_up_function_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnb_id_present) {
    j.write_int("gnbId", gnb_id);
  }
  if (gnb_id_len_present) {
    j.write_int("gnbIdLength", gnb_id_len);
  }
  if (gnb_cu_up_id_present) {
    j.write_int("gnbCuUpId", gnb_cu_up_id);
  }
  if (plmn_info_list.size() > 0) {
    j.start_array("plmnInfoList");
    for (const auto& e1 : plmn_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_gnb_cu_up_function_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.gnb_id_present) {
    j["gnbId"] = obj.gnb_id;
  }
  if (obj.gnb_id_len_present) {
    j["gnbIdLength"] = obj.gnb_id_len;
  }
  if (obj.gnb_cu_up_id_present) {
    j["gnbCuUpId"] = obj.gnb_cu_up_id;
  }
  if (obj.plmn_info_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.plmn_info_list) {
        j_array.push_back(e1);
      }
      j["plmnInfoList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_gnb_cu_up_function_s& obj)
{
  if (in_j.contains("gnbId")) {
    in_j["gnbId"].get_to(obj.gnb_id);
    obj.gnb_id_present = true;
  }
  if (in_j.contains("gnbIdLength")) {
    in_j["gnbIdLength"].get_to(obj.gnb_id_len);
    obj.gnb_id_len_present = true;
  }
  if (in_j.contains("gnbCuUpId")) {
    in_j["gnbCuUpId"].get_to(obj.gnb_cu_up_id);
    obj.gnb_cu_up_id_present = true;
  }
  if (in_j.contains("plmnInfoList")) {
    {
      const auto& in_arr  = in_j.at("plmnInfoList");
      auto&       out_arr = obj.plmn_info_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<plmn_info_s>();
      }
    }
  }
}

// O-GnbDuFunction ::= SEQUENCE
SRSASN_CODE o_gnb_du_function_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gnb_du_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_du_name_present, 1));
  HANDLE_CODE(bref.pack(gnb_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_id_len_present, 1));

  if (gnb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_du_name_present) {
    HANDLE_CODE(gnb_du_name.pack(bref));
  }
  if (gnb_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_id_len_present) {
    HANDLE_CODE(pack_integer(bref, gnb_id_len, (uint8_t)22u, (uint8_t)32u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_gnb_du_function_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gnb_du_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_du_name_present, 1));
  HANDLE_CODE(bref.unpack(gnb_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_id_len_present, 1));

  if (gnb_du_id_present) {
    HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_du_name_present) {
    HANDLE_CODE(gnb_du_name.unpack(bref));
  }
  if (gnb_id_present) {
    HANDLE_CODE(unpack_integer(gnb_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_id_len_present) {
    HANDLE_CODE(unpack_integer(gnb_id_len, bref, (uint8_t)22u, (uint8_t)32u, false, true));
  }

  return SRSASN_SUCCESS;
}
void o_gnb_du_function_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnb_du_id_present) {
    j.write_int("gnbDuId", gnb_du_id);
  }
  if (gnb_du_name_present) {
    j.write_str("gnbDuName", gnb_du_name.to_string());
  }
  if (gnb_id_present) {
    j.write_int("gnbId", gnb_id);
  }
  if (gnb_id_len_present) {
    j.write_int("gnbIdLength", gnb_id_len);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_gnb_du_function_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.gnb_du_id_present) {
    j["gnbDuId"] = obj.gnb_du_id;
  }
  if (obj.gnb_du_name_present) {
    j["gnbDuName"] = obj.gnb_du_name.to_string();
  }
  if (obj.gnb_id_present) {
    j["gnbId"] = obj.gnb_id;
  }
  if (obj.gnb_id_len_present) {
    j["gnbIdLength"] = obj.gnb_id_len;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_gnb_du_function_s& obj)
{
  if (in_j.contains("gnbDuId")) {
    in_j["gnbDuId"].get_to(obj.gnb_du_id);
    obj.gnb_du_id_present = true;
  }
  if (in_j.contains("gnbDuName")) {
    obj.gnb_du_name.from_string(in_j["gnbDuName"].get<std::string>());
    obj.gnb_du_name_present = true;
  }
  if (in_j.contains("gnbId")) {
    in_j["gnbId"].get_to(obj.gnb_id);
    obj.gnb_id_present = true;
  }
  if (in_j.contains("gnbIdLength")) {
    in_j["gnbIdLength"].get_to(obj.gnb_id_len);
    obj.gnb_id_len_present = true;
  }
}

// O-NESPolicy ::= SEQUENCE
SRSASN_CODE o_nes_policy_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(policy_type_present, 1));
  HANDLE_CODE(bref.pack(ant_mask_name_present, 1));
  HANDLE_CODE(bref.pack(ant_mask_present, 1));
  HANDLE_CODE(bref.pack(sleep_mode_present, 1));
  HANDLE_CODE(bref.pack(data_dir_present, 1));
  HANDLE_CODE(bref.pack(symbol_mask_present, 1));
  HANDLE_CODE(bref.pack(slot_mask_present, 1));
  HANDLE_CODE(bref.pack(valid_dur_present, 1));
  HANDLE_CODE(bref.pack(es_objive_present, 1));
  HANDLE_CODE(bref.pack(perf_objive_list.size() > 0, 1));

  if (policy_type_present) {
    HANDLE_CODE(policy_type.pack(bref));
  }
  if (ant_mask_name_present) {
    HANDLE_CODE(ant_mask_name.pack(bref));
  }
  if (ant_mask_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, ant_mask, false, true));
  }
  if (sleep_mode_present) {
    HANDLE_CODE(sleep_mode.pack(bref));
  }
  if (data_dir_present) {
    HANDLE_CODE(data_dir.pack(bref));
  }
  if (symbol_mask_present) {
    HANDLE_CODE(pack_integer(bref, symbol_mask, (uint16_t)0u, (uint16_t)16383u, false, true));
  }
  if (slot_mask_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, slot_mask, false, true));
  }
  if (valid_dur_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, valid_dur, false, true));
  }
  if (es_objive_present) {
    HANDLE_CODE(es_objive.pack(bref));
  }
  if (perf_objive_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, perf_objive_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_nes_policy_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(policy_type_present, 1));
  HANDLE_CODE(bref.unpack(ant_mask_name_present, 1));
  HANDLE_CODE(bref.unpack(ant_mask_present, 1));
  HANDLE_CODE(bref.unpack(sleep_mode_present, 1));
  HANDLE_CODE(bref.unpack(data_dir_present, 1));
  HANDLE_CODE(bref.unpack(symbol_mask_present, 1));
  HANDLE_CODE(bref.unpack(slot_mask_present, 1));
  HANDLE_CODE(bref.unpack(valid_dur_present, 1));
  HANDLE_CODE(bref.unpack(es_objive_present, 1));
  bool perf_objive_list_present;
  HANDLE_CODE(bref.unpack(perf_objive_list_present, 1));

  if (policy_type_present) {
    HANDLE_CODE(policy_type.unpack(bref));
  }
  if (ant_mask_name_present) {
    HANDLE_CODE(ant_mask_name.unpack(bref));
  }
  if (ant_mask_present) {
    HANDLE_CODE(unpack_unconstrained_integer(ant_mask, bref, false, true));
  }
  if (sleep_mode_present) {
    HANDLE_CODE(sleep_mode.unpack(bref));
  }
  if (data_dir_present) {
    HANDLE_CODE(data_dir.unpack(bref));
  }
  if (symbol_mask_present) {
    HANDLE_CODE(unpack_integer(symbol_mask, bref, (uint16_t)0u, (uint16_t)16383u, false, true));
  }
  if (slot_mask_present) {
    HANDLE_CODE(unpack_unconstrained_integer(slot_mask, bref, false, true));
  }
  if (valid_dur_present) {
    HANDLE_CODE(unpack_unconstrained_integer(valid_dur, bref, false, true));
  }
  if (es_objive_present) {
    HANDLE_CODE(es_objive.unpack(bref));
  }
  if (perf_objive_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(perf_objive_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void o_nes_policy_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (policy_type_present) {
    j.write_str("policyType", policy_type.to_string());
  }
  if (ant_mask_name_present) {
    j.write_str("antennaMaskName", ant_mask_name.to_string());
  }
  if (ant_mask_present) {
    j.write_int("antennaMask", ant_mask);
  }
  if (sleep_mode_present) {
    j.write_str("sleepMode", sleep_mode.to_string());
  }
  if (data_dir_present) {
    j.write_str("dataDir", data_dir.to_string());
  }
  if (symbol_mask_present) {
    j.write_int("symbolMask", symbol_mask);
  }
  if (slot_mask_present) {
    j.write_int("slotMask", slot_mask);
  }
  if (valid_dur_present) {
    j.write_int("validDuration", valid_dur);
  }
  if (es_objive_present) {
    j.write_fieldname("esObjective");
    es_objive.to_json(j);
  }
  if (perf_objive_list.size() > 0) {
    j.start_array("perfObjectiveList");
    for (const auto& e1 : perf_objive_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_nes_policy_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.policy_type_present) {
    j["policyType"] = obj.policy_type.to_string();
  }
  if (obj.ant_mask_name_present) {
    j["antennaMaskName"] = obj.ant_mask_name.to_string();
  }
  if (obj.ant_mask_present) {
    j["antennaMask"] = obj.ant_mask;
  }
  if (obj.sleep_mode_present) {
    j["sleepMode"] = obj.sleep_mode.to_string();
  }
  if (obj.data_dir_present) {
    j["dataDir"] = obj.data_dir.to_string();
  }
  if (obj.symbol_mask_present) {
    j["symbolMask"] = obj.symbol_mask;
  }
  if (obj.slot_mask_present) {
    j["slotMask"] = obj.slot_mask;
  }
  if (obj.valid_dur_present) {
    j["validDuration"] = obj.valid_dur;
  }
  if (obj.es_objive_present) {
    j["esObjective"] = obj.es_objive;
  }
  if (obj.perf_objive_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.perf_objive_list) {
        j_array.push_back(e1);
      }
      j["perfObjectiveList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_nes_policy_s& obj)
{
  if (in_j.contains("policyType")) {
    obj.policy_type.from_string(in_j["policyType"].get<std::string>());
    obj.policy_type_present = true;
  }
  if (in_j.contains("antennaMaskName")) {
    obj.ant_mask_name.from_string(in_j["antennaMaskName"].get<std::string>());
    obj.ant_mask_name_present = true;
  }
  if (in_j.contains("antennaMask")) {
    in_j["antennaMask"].get_to(obj.ant_mask);
    obj.ant_mask_present = true;
  }
  if (in_j.contains("sleepMode")) {
    obj.sleep_mode.from_string(in_j["sleepMode"].get<std::string>());
    obj.sleep_mode_present = true;
  }
  if (in_j.contains("dataDir")) {
    obj.data_dir.from_string(in_j["dataDir"].get<std::string>());
    obj.data_dir_present = true;
  }
  if (in_j.contains("symbolMask")) {
    in_j["symbolMask"].get_to(obj.symbol_mask);
    obj.symbol_mask_present = true;
  }
  if (in_j.contains("slotMask")) {
    in_j["slotMask"].get_to(obj.slot_mask);
    obj.slot_mask_present = true;
  }
  if (in_j.contains("validDuration")) {
    in_j["validDuration"].get_to(obj.valid_dur);
    obj.valid_dur_present = true;
  }
  if (in_j.contains("esObjective")) {
    in_j["esObjective"].get_to(obj.es_objive);
    obj.es_objive_present = true;
  }
  if (in_j.contains("perfObjectiveList")) {
    {
      const auto& in_arr  = in_j.at("perfObjectiveList");
      auto&       out_arr = obj.perf_objive_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<perf_objive_s>();
      }
    }
  }
}

const char* o_nes_policy_s::policy_type_opts::to_string() const
{
  static const char* names[] = {"ASM", "TRX_CONTROL"};
  return convert_enum_idx(names, 2, value, "o_nes_policy_s::policy_type_e_");
}
bool o_nes_policy_s::policy_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"ASM", "TRX_CONTROL"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_nes_policy_s::policy_type_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

const char* o_nes_policy_s::sleep_mode_opts::to_string() const
{
  static const char* names[] = {"SLEEP_MODE0", "SLEEP_MODE1", "SLEEP_MODE2", "SLEEP_MODE3"};
  return convert_enum_idx(names, 4, value, "o_nes_policy_s::sleep_mode_e_");
}
bool o_nes_policy_s::sleep_mode_opts::from_string(const std::string& str)
{
  static const char* names[] = {"SLEEP_MODE0", "SLEEP_MODE1", "SLEEP_MODE2", "SLEEP_MODE3"};
  uint32_t           idx;
  if (convert_enum_str(names, 4, str.c_str(), idx, "o_nes_policy_s::sleep_mode_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t o_nes_policy_s::sleep_mode_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "o_nes_policy_s::sleep_mode_e_");
}

const char* o_nes_policy_s::data_dir_opts::to_string() const
{
  static const char* names[] = {"DL", "UL"};
  return convert_enum_idx(names, 2, value, "o_nes_policy_s::data_dir_e_");
}
bool o_nes_policy_s::data_dir_opts::from_string(const std::string& str)
{
  static const char* names[] = {"DL", "UL"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_nes_policy_s::data_dir_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// O-NrCellCu ::= SEQUENCE
SRSASN_CODE o_nr_cell_cu_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_local_id_present, 1));
  HANDLE_CODE(bref.pack(plmn_info_list.size() > 0, 1));

  if (cell_local_id_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, cell_local_id, false, true));
  }
  if (plmn_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_info_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_nr_cell_cu_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_local_id_present, 1));
  bool plmn_info_list_present;
  HANDLE_CODE(bref.unpack(plmn_info_list_present, 1));

  if (cell_local_id_present) {
    HANDLE_CODE(unpack_unconstrained_integer(cell_local_id, bref, false, true));
  }
  if (plmn_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_info_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void o_nr_cell_cu_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_local_id_present) {
    j.write_int("cellLocalId", cell_local_id);
  }
  if (plmn_info_list.size() > 0) {
    j.start_array("plmnInfoList");
    for (const auto& e1 : plmn_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_nr_cell_cu_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.cell_local_id_present) {
    j["cellLocalId"] = obj.cell_local_id;
  }
  if (obj.plmn_info_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.plmn_info_list) {
        j_array.push_back(e1);
      }
      j["plmnInfoList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_nr_cell_cu_s& obj)
{
  if (in_j.contains("cellLocalId")) {
    in_j["cellLocalId"].get_to(obj.cell_local_id);
    obj.cell_local_id_present = true;
  }
  if (in_j.contains("plmnInfoList")) {
    {
      const auto& in_arr  = in_j.at("plmnInfoList");
      auto&       out_arr = obj.plmn_info_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<plmn_info_s>();
      }
    }
  }
}

// O-NrCellDu ::= SEQUENCE
SRSASN_CODE o_nr_cell_du_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_local_id_present, 1));
  HANDLE_CODE(bref.pack(operational_state_present, 1));
  HANDLE_CODE(bref.pack(administrative_state_present, 1));
  HANDLE_CODE(bref.pack(cell_state_present, 1));
  HANDLE_CODE(bref.pack(plmn_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(nr_pci_present, 1));
  HANDLE_CODE(bref.pack(nr_tac_present, 1));
  HANDLE_CODE(bref.pack(arfcn_dl_present, 1));
  HANDLE_CODE(bref.pack(arfcn_ul_present, 1));
  HANDLE_CODE(bref.pack(arfcn_sul_present, 1));
  HANDLE_CODE(bref.pack(bs_ch_bw_dl_present, 1));
  HANDLE_CODE(bref.pack(ssb_freq_present, 1));
  HANDLE_CODE(bref.pack(ssb_periodicity_present, 1));
  HANDLE_CODE(bref.pack(ssb_sub_carrier_spacing_present, 1));
  HANDLE_CODE(bref.pack(ssb_offset_present, 1));
  HANDLE_CODE(bref.pack(ssb_dur_present, 1));
  HANDLE_CODE(bref.pack(bs_ch_bw_ul_present, 1));
  HANDLE_CODE(bref.pack(bs_ch_bw_sul_present, 1));
  HANDLE_CODE(bref.pack(bwp_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(partition_list.size() > 0, 1));

  if (cell_local_id_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, cell_local_id, false, true));
  }
  if (operational_state_present) {
    HANDLE_CODE(operational_state.pack(bref));
  }
  if (administrative_state_present) {
    HANDLE_CODE(administrative_state.pack(bref));
  }
  if (cell_state_present) {
    HANDLE_CODE(cell_state.pack(bref));
  }
  if (plmn_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_info_list, 1, 64, true));
  }
  if (nr_pci_present) {
    HANDLE_CODE(pack_integer(bref, nr_pci, (uint16_t)0u, (uint16_t)503u, false, true));
  }
  if (nr_tac_present) {
    HANDLE_CODE(pack_integer(bref, nr_tac, (uint32_t)0u, (uint32_t)16777215u, false, true));
  }
  if (arfcn_dl_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, arfcn_dl, false, true));
  }
  if (arfcn_ul_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, arfcn_ul, false, true));
  }
  if (arfcn_sul_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, arfcn_sul, false, true));
  }
  if (bs_ch_bw_dl_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, bs_ch_bw_dl, false, true));
  }
  if (ssb_freq_present) {
    HANDLE_CODE(pack_integer(bref, ssb_freq, (uint32_t)0u, (uint32_t)3279165u, false, true));
  }
  if (ssb_periodicity_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, ssb_periodicity, false, true));
  }
  if (ssb_sub_carrier_spacing_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, ssb_sub_carrier_spacing, false, true));
  }
  if (ssb_offset_present) {
    HANDLE_CODE(pack_integer(bref, ssb_offset, (uint8_t)0u, (uint8_t)159u, false, true));
  }
  if (ssb_dur_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, ssb_dur, false, true));
  }
  if (bs_ch_bw_ul_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, bs_ch_bw_ul, false, true));
  }
  if (bs_ch_bw_sul_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, bs_ch_bw_sul, false, true));
  }
  if (bwp_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bwp_list, 1, 64, true));
  }
  if (partition_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, partition_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_nr_cell_du_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_local_id_present, 1));
  HANDLE_CODE(bref.unpack(operational_state_present, 1));
  HANDLE_CODE(bref.unpack(administrative_state_present, 1));
  HANDLE_CODE(bref.unpack(cell_state_present, 1));
  bool plmn_info_list_present;
  HANDLE_CODE(bref.unpack(plmn_info_list_present, 1));
  HANDLE_CODE(bref.unpack(nr_pci_present, 1));
  HANDLE_CODE(bref.unpack(nr_tac_present, 1));
  HANDLE_CODE(bref.unpack(arfcn_dl_present, 1));
  HANDLE_CODE(bref.unpack(arfcn_ul_present, 1));
  HANDLE_CODE(bref.unpack(arfcn_sul_present, 1));
  HANDLE_CODE(bref.unpack(bs_ch_bw_dl_present, 1));
  HANDLE_CODE(bref.unpack(ssb_freq_present, 1));
  HANDLE_CODE(bref.unpack(ssb_periodicity_present, 1));
  HANDLE_CODE(bref.unpack(ssb_sub_carrier_spacing_present, 1));
  HANDLE_CODE(bref.unpack(ssb_offset_present, 1));
  HANDLE_CODE(bref.unpack(ssb_dur_present, 1));
  HANDLE_CODE(bref.unpack(bs_ch_bw_ul_present, 1));
  HANDLE_CODE(bref.unpack(bs_ch_bw_sul_present, 1));
  bool bwp_list_present;
  HANDLE_CODE(bref.unpack(bwp_list_present, 1));
  bool partition_list_present;
  HANDLE_CODE(bref.unpack(partition_list_present, 1));

  if (cell_local_id_present) {
    HANDLE_CODE(unpack_unconstrained_integer(cell_local_id, bref, false, true));
  }
  if (operational_state_present) {
    HANDLE_CODE(operational_state.unpack(bref));
  }
  if (administrative_state_present) {
    HANDLE_CODE(administrative_state.unpack(bref));
  }
  if (cell_state_present) {
    HANDLE_CODE(cell_state.unpack(bref));
  }
  if (plmn_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_info_list, bref, 1, 64, true));
  }
  if (nr_pci_present) {
    HANDLE_CODE(unpack_integer(nr_pci, bref, (uint16_t)0u, (uint16_t)503u, false, true));
  }
  if (nr_tac_present) {
    HANDLE_CODE(unpack_integer(nr_tac, bref, (uint32_t)0u, (uint32_t)16777215u, false, true));
  }
  if (arfcn_dl_present) {
    HANDLE_CODE(unpack_unconstrained_integer(arfcn_dl, bref, false, true));
  }
  if (arfcn_ul_present) {
    HANDLE_CODE(unpack_unconstrained_integer(arfcn_ul, bref, false, true));
  }
  if (arfcn_sul_present) {
    HANDLE_CODE(unpack_unconstrained_integer(arfcn_sul, bref, false, true));
  }
  if (bs_ch_bw_dl_present) {
    HANDLE_CODE(unpack_unconstrained_integer(bs_ch_bw_dl, bref, false, true));
  }
  if (ssb_freq_present) {
    HANDLE_CODE(unpack_integer(ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u, false, true));
  }
  if (ssb_periodicity_present) {
    HANDLE_CODE(unpack_unconstrained_integer(ssb_periodicity, bref, false, true));
  }
  if (ssb_sub_carrier_spacing_present) {
    HANDLE_CODE(unpack_unconstrained_integer(ssb_sub_carrier_spacing, bref, false, true));
  }
  if (ssb_offset_present) {
    HANDLE_CODE(unpack_integer(ssb_offset, bref, (uint8_t)0u, (uint8_t)159u, false, true));
  }
  if (ssb_dur_present) {
    HANDLE_CODE(unpack_unconstrained_integer(ssb_dur, bref, false, true));
  }
  if (bs_ch_bw_ul_present) {
    HANDLE_CODE(unpack_unconstrained_integer(bs_ch_bw_ul, bref, false, true));
  }
  if (bs_ch_bw_sul_present) {
    HANDLE_CODE(unpack_unconstrained_integer(bs_ch_bw_sul, bref, false, true));
  }
  if (bwp_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bwp_list, bref, 1, 64, true));
  }
  if (partition_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(partition_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void o_nr_cell_du_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_local_id_present) {
    j.write_int("cellLocalId", cell_local_id);
  }
  if (operational_state_present) {
    j.write_str("operationalState", operational_state.to_string());
  }
  if (administrative_state_present) {
    j.write_str("administrativeState", administrative_state.to_string());
  }
  if (cell_state_present) {
    j.write_str("cellState", cell_state.to_string());
  }
  if (plmn_info_list.size() > 0) {
    j.start_array("plmnInfoList");
    for (const auto& e1 : plmn_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nr_pci_present) {
    j.write_int("nrPci", nr_pci);
  }
  if (nr_tac_present) {
    j.write_int("nrTac", nr_tac);
  }
  if (arfcn_dl_present) {
    j.write_int("arfcnDL", arfcn_dl);
  }
  if (arfcn_ul_present) {
    j.write_int("arfcnUL", arfcn_ul);
  }
  if (arfcn_sul_present) {
    j.write_int("arfcnSUL", arfcn_sul);
  }
  if (bs_ch_bw_dl_present) {
    j.write_int("bSChannelBwDL", bs_ch_bw_dl);
  }
  if (ssb_freq_present) {
    j.write_int("ssbFrequency", ssb_freq);
  }
  if (ssb_periodicity_present) {
    j.write_int("ssbPeriodicity", ssb_periodicity);
  }
  if (ssb_sub_carrier_spacing_present) {
    j.write_int("ssbSubCarrierSpacing", ssb_sub_carrier_spacing);
  }
  if (ssb_offset_present) {
    j.write_int("ssbOffset", ssb_offset);
  }
  if (ssb_dur_present) {
    j.write_int("ssbDuration", ssb_dur);
  }
  if (bs_ch_bw_ul_present) {
    j.write_int("bSChannelBwUL", bs_ch_bw_ul);
  }
  if (bs_ch_bw_sul_present) {
    j.write_int("bSChannelBwSUL", bs_ch_bw_sul);
  }
  if (bwp_list.size() > 0) {
    j.start_array("bwpList");
    for (const auto& e1 : bwp_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (partition_list.size() > 0) {
    j.start_array("partitionList");
    for (const auto& e1 : partition_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_nr_cell_du_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.cell_local_id_present) {
    j["cellLocalId"] = obj.cell_local_id;
  }
  if (obj.operational_state_present) {
    j["operationalState"] = obj.operational_state.to_string();
  }
  if (obj.administrative_state_present) {
    j["administrativeState"] = obj.administrative_state.to_string();
  }
  if (obj.cell_state_present) {
    j["cellState"] = obj.cell_state.to_string();
  }
  if (obj.plmn_info_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.plmn_info_list) {
        j_array.push_back(e1);
      }
      j["plmnInfoList"] = j_array;
    }
  }
  if (obj.nr_pci_present) {
    j["nrPci"] = obj.nr_pci;
  }
  if (obj.nr_tac_present) {
    j["nrTac"] = obj.nr_tac;
  }
  if (obj.arfcn_dl_present) {
    j["arfcnDL"] = obj.arfcn_dl;
  }
  if (obj.arfcn_ul_present) {
    j["arfcnUL"] = obj.arfcn_ul;
  }
  if (obj.arfcn_sul_present) {
    j["arfcnSUL"] = obj.arfcn_sul;
  }
  if (obj.bs_ch_bw_dl_present) {
    j["bSChannelBwDL"] = obj.bs_ch_bw_dl;
  }
  if (obj.ssb_freq_present) {
    j["ssbFrequency"] = obj.ssb_freq;
  }
  if (obj.ssb_periodicity_present) {
    j["ssbPeriodicity"] = obj.ssb_periodicity;
  }
  if (obj.ssb_sub_carrier_spacing_present) {
    j["ssbSubCarrierSpacing"] = obj.ssb_sub_carrier_spacing;
  }
  if (obj.ssb_offset_present) {
    j["ssbOffset"] = obj.ssb_offset;
  }
  if (obj.ssb_dur_present) {
    j["ssbDuration"] = obj.ssb_dur;
  }
  if (obj.bs_ch_bw_ul_present) {
    j["bSChannelBwUL"] = obj.bs_ch_bw_ul;
  }
  if (obj.bs_ch_bw_sul_present) {
    j["bSChannelBwSUL"] = obj.bs_ch_bw_sul;
  }
  if (obj.bwp_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.bwp_list) {
        j_array.push_back(e1);
      }
      j["bwpList"] = j_array;
    }
  }
  if (obj.partition_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.partition_list) {
        j_array.push_back(e1);
      }
      j["partitionList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_nr_cell_du_s& obj)
{
  if (in_j.contains("cellLocalId")) {
    in_j["cellLocalId"].get_to(obj.cell_local_id);
    obj.cell_local_id_present = true;
  }
  if (in_j.contains("operationalState")) {
    obj.operational_state.from_string(in_j["operationalState"].get<std::string>());
    obj.operational_state_present = true;
  }
  if (in_j.contains("administrativeState")) {
    obj.administrative_state.from_string(in_j["administrativeState"].get<std::string>());
    obj.administrative_state_present = true;
  }
  if (in_j.contains("cellState")) {
    obj.cell_state.from_string(in_j["cellState"].get<std::string>());
    obj.cell_state_present = true;
  }
  if (in_j.contains("plmnInfoList")) {
    {
      const auto& in_arr  = in_j.at("plmnInfoList");
      auto&       out_arr = obj.plmn_info_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<plmn_info_s>();
      }
    }
  }
  if (in_j.contains("nrPci")) {
    in_j["nrPci"].get_to(obj.nr_pci);
    obj.nr_pci_present = true;
  }
  if (in_j.contains("nrTac")) {
    in_j["nrTac"].get_to(obj.nr_tac);
    obj.nr_tac_present = true;
  }
  if (in_j.contains("arfcnDL")) {
    in_j["arfcnDL"].get_to(obj.arfcn_dl);
    obj.arfcn_dl_present = true;
  }
  if (in_j.contains("arfcnUL")) {
    in_j["arfcnUL"].get_to(obj.arfcn_ul);
    obj.arfcn_ul_present = true;
  }
  if (in_j.contains("arfcnSUL")) {
    in_j["arfcnSUL"].get_to(obj.arfcn_sul);
    obj.arfcn_sul_present = true;
  }
  if (in_j.contains("bSChannelBwDL")) {
    in_j["bSChannelBwDL"].get_to(obj.bs_ch_bw_dl);
    obj.bs_ch_bw_dl_present = true;
  }
  if (in_j.contains("ssbFrequency")) {
    in_j["ssbFrequency"].get_to(obj.ssb_freq);
    obj.ssb_freq_present = true;
  }
  if (in_j.contains("ssbPeriodicity")) {
    in_j["ssbPeriodicity"].get_to(obj.ssb_periodicity);
    obj.ssb_periodicity_present = true;
  }
  if (in_j.contains("ssbSubCarrierSpacing")) {
    in_j["ssbSubCarrierSpacing"].get_to(obj.ssb_sub_carrier_spacing);
    obj.ssb_sub_carrier_spacing_present = true;
  }
  if (in_j.contains("ssbOffset")) {
    in_j["ssbOffset"].get_to(obj.ssb_offset);
    obj.ssb_offset_present = true;
  }
  if (in_j.contains("ssbDuration")) {
    in_j["ssbDuration"].get_to(obj.ssb_dur);
    obj.ssb_dur_present = true;
  }
  if (in_j.contains("bSChannelBwUL")) {
    in_j["bSChannelBwUL"].get_to(obj.bs_ch_bw_ul);
    obj.bs_ch_bw_ul_present = true;
  }
  if (in_j.contains("bSChannelBwSUL")) {
    in_j["bSChannelBwSUL"].get_to(obj.bs_ch_bw_sul);
    obj.bs_ch_bw_sul_present = true;
  }
  if (in_j.contains("bwpList")) {
    {
      const auto& in_arr  = in_j.at("bwpList");
      auto&       out_arr = obj.bwp_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<o_bwp_s>();
      }
    }
  }
  if (in_j.contains("partitionList")) {
    {
      const auto& in_arr  = in_j.at("partitionList");
      auto&       out_arr = obj.partition_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<partition_list_item_s_>();
      }
    }
  }
}

// O-PRBBlankingPolicy ::= SEQUENCE
SRSASN_CODE o_prb_blanking_policy_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(policy_category_present, 1));
  HANDLE_CODE(bref.pack(link_dir_present, 1));
  HANDLE_CODE(bref.pack(start_time_present, 1));
  HANDLE_CODE(bref.pack(end_time_present, 1));
  HANDLE_CODE(bref.pack(prb_blanking_cfg_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(prb_blanking_obj_present, 1));

  if (policy_category_present) {
    HANDLE_CODE(policy_category.pack(bref));
  }
  if (link_dir_present) {
    HANDLE_CODE(link_dir.pack(bref));
  }
  if (start_time_present) {
    HANDLE_CODE(start_time.pack(bref));
  }
  if (end_time_present) {
    HANDLE_CODE(end_time.pack(bref));
  }
  if (prb_blanking_cfg_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, prb_blanking_cfg_list, 1, 64, true));
  }
  if (prb_blanking_obj_present) {
    HANDLE_CODE(prb_blanking_obj.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_prb_blanking_policy_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(policy_category_present, 1));
  HANDLE_CODE(bref.unpack(link_dir_present, 1));
  HANDLE_CODE(bref.unpack(start_time_present, 1));
  HANDLE_CODE(bref.unpack(end_time_present, 1));
  bool prb_blanking_cfg_list_present;
  HANDLE_CODE(bref.unpack(prb_blanking_cfg_list_present, 1));
  HANDLE_CODE(bref.unpack(prb_blanking_obj_present, 1));

  if (policy_category_present) {
    HANDLE_CODE(policy_category.unpack(bref));
  }
  if (link_dir_present) {
    HANDLE_CODE(link_dir.unpack(bref));
  }
  if (start_time_present) {
    HANDLE_CODE(start_time.unpack(bref));
  }
  if (end_time_present) {
    HANDLE_CODE(end_time.unpack(bref));
  }
  if (prb_blanking_cfg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(prb_blanking_cfg_list, bref, 1, 64, true));
  }
  if (prb_blanking_obj_present) {
    HANDLE_CODE(prb_blanking_obj.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void o_prb_blanking_policy_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (policy_category_present) {
    j.write_str("policyCategory", policy_category.to_string());
  }
  if (link_dir_present) {
    j.write_str("linkDir", link_dir.to_string());
  }
  if (start_time_present) {
    j.write_str("startTime", start_time.to_string());
  }
  if (end_time_present) {
    j.write_str("endTime", end_time.to_string());
  }
  if (prb_blanking_cfg_list.size() > 0) {
    j.start_array("prbBlankingConfigurationList");
    for (const auto& e1 : prb_blanking_cfg_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (prb_blanking_obj_present) {
    j.write_fieldname("prbBlankingObj");
    prb_blanking_obj.to_json(j);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_prb_blanking_policy_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.policy_category_present) {
    j["policyCategory"] = obj.policy_category.to_string();
  }
  if (obj.link_dir_present) {
    j["linkDir"] = obj.link_dir.to_string();
  }
  if (obj.start_time_present) {
    j["startTime"] = obj.start_time.to_string();
  }
  if (obj.end_time_present) {
    j["endTime"] = obj.end_time.to_string();
  }
  if (obj.prb_blanking_cfg_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.prb_blanking_cfg_list) {
        j_array.push_back(e1);
      }
      j["prbBlankingConfigurationList"] = j_array;
    }
  }
  if (obj.prb_blanking_obj_present) {
    j["prbBlankingObj"] = obj.prb_blanking_obj;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_prb_blanking_policy_s& obj)
{
  if (in_j.contains("policyCategory")) {
    obj.policy_category.from_string(in_j["policyCategory"].get<std::string>());
    obj.policy_category_present = true;
  }
  if (in_j.contains("linkDir")) {
    obj.link_dir.from_string(in_j["linkDir"].get<std::string>());
    obj.link_dir_present = true;
  }
  if (in_j.contains("startTime")) {
    obj.start_time.from_string(in_j["startTime"].get<std::string>());
    obj.start_time_present = true;
  }
  if (in_j.contains("endTime")) {
    obj.end_time.from_string(in_j["endTime"].get<std::string>());
    obj.end_time_present = true;
  }
  if (in_j.contains("prbBlankingConfigurationList")) {
    {
      const auto& in_arr  = in_j.at("prbBlankingConfigurationList");
      auto&       out_arr = obj.prb_blanking_cfg_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<prb_blanking_cfg_item_s>();
      }
    }
  }
  if (in_j.contains("prbBlankingObj")) {
    in_j["prbBlankingObj"].get_to(obj.prb_blanking_obj);
    obj.prb_blanking_obj_present = true;
  }
}

const char* o_prb_blanking_policy_s::policy_category_opts::to_string() const
{
  static const char* names[] = {"prbBlankingConfiguration", "prbBlankingObjective"};
  return convert_enum_idx(names, 2, value, "o_prb_blanking_policy_s::policy_category_e_");
}
bool o_prb_blanking_policy_s::policy_category_opts::from_string(const std::string& str)
{
  static const char* names[] = {"prbBlankingConfiguration", "prbBlankingObjective"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_prb_blanking_policy_s::policy_category_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

const char* o_prb_blanking_policy_s::link_dir_opts::to_string() const
{
  static const char* names[] = {"DL", "UL"};
  return convert_enum_idx(names, 2, value, "o_prb_blanking_policy_s::link_dir_e_");
}
bool o_prb_blanking_policy_s::link_dir_opts::from_string(const std::string& str)
{
  static const char* names[] = {"DL", "UL"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "o_prb_blanking_policy_s::link_dir_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// O-RRMPolicyRatio ::= SEQUENCE
SRSASN_CODE o_rrm_policy_ratio_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(res_type_present, 1));
  HANDLE_CODE(bref.pack(rrm_policy_member_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(rrm_policy_max_ratio_present, 1));
  HANDLE_CODE(bref.pack(rrm_policy_min_ratio_present, 1));
  HANDLE_CODE(bref.pack(rrm_policy_ded_ratio_present, 1));

  if (res_type_present) {
    HANDLE_CODE(res_type.pack(bref));
  }
  if (rrm_policy_member_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rrm_policy_member_list, 1, 64, true));
  }
  if (rrm_policy_max_ratio_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, rrm_policy_max_ratio, false, true));
  }
  if (rrm_policy_min_ratio_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, rrm_policy_min_ratio, false, true));
  }
  if (rrm_policy_ded_ratio_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, rrm_policy_ded_ratio, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_rrm_policy_ratio_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(res_type_present, 1));
  bool rrm_policy_member_list_present;
  HANDLE_CODE(bref.unpack(rrm_policy_member_list_present, 1));
  HANDLE_CODE(bref.unpack(rrm_policy_max_ratio_present, 1));
  HANDLE_CODE(bref.unpack(rrm_policy_min_ratio_present, 1));
  HANDLE_CODE(bref.unpack(rrm_policy_ded_ratio_present, 1));

  if (res_type_present) {
    HANDLE_CODE(res_type.unpack(bref));
  }
  if (rrm_policy_member_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rrm_policy_member_list, bref, 1, 64, true));
  }
  if (rrm_policy_max_ratio_present) {
    HANDLE_CODE(unpack_unconstrained_integer(rrm_policy_max_ratio, bref, false, true));
  }
  if (rrm_policy_min_ratio_present) {
    HANDLE_CODE(unpack_unconstrained_integer(rrm_policy_min_ratio, bref, false, true));
  }
  if (rrm_policy_ded_ratio_present) {
    HANDLE_CODE(unpack_unconstrained_integer(rrm_policy_ded_ratio, bref, false, true));
  }

  return SRSASN_SUCCESS;
}
void o_rrm_policy_ratio_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (res_type_present) {
    j.write_str("resourceType", res_type.to_string());
  }
  if (rrm_policy_member_list.size() > 0) {
    j.start_array("rRMPolicyMemberList");
    for (const auto& e1 : rrm_policy_member_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rrm_policy_max_ratio_present) {
    j.write_int("rRMPolicyMaxRatio", rrm_policy_max_ratio);
  }
  if (rrm_policy_min_ratio_present) {
    j.write_int("rRMPolicyMinRatio", rrm_policy_min_ratio);
  }
  if (rrm_policy_ded_ratio_present) {
    j.write_int("rRMPolicyDedicatedRatio", rrm_policy_ded_ratio);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_rrm_policy_ratio_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.res_type_present) {
    j["resourceType"] = obj.res_type.to_string();
  }
  if (obj.rrm_policy_member_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.rrm_policy_member_list) {
        j_array.push_back(e1);
      }
      j["rRMPolicyMemberList"] = j_array;
    }
  }
  if (obj.rrm_policy_max_ratio_present) {
    j["rRMPolicyMaxRatio"] = obj.rrm_policy_max_ratio;
  }
  if (obj.rrm_policy_min_ratio_present) {
    j["rRMPolicyMinRatio"] = obj.rrm_policy_min_ratio;
  }
  if (obj.rrm_policy_ded_ratio_present) {
    j["rRMPolicyDedicatedRatio"] = obj.rrm_policy_ded_ratio;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_rrm_policy_ratio_s& obj)
{
  if (in_j.contains("resourceType")) {
    obj.res_type.from_string(in_j["resourceType"].get<std::string>());
    obj.res_type_present = true;
  }
  if (in_j.contains("rRMPolicyMemberList")) {
    {
      const auto& in_arr  = in_j.at("rRMPolicyMemberList");
      auto&       out_arr = obj.rrm_policy_member_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<rrm_policy_member_s>();
      }
    }
  }
  if (in_j.contains("rRMPolicyMaxRatio")) {
    in_j["rRMPolicyMaxRatio"].get_to(obj.rrm_policy_max_ratio);
    obj.rrm_policy_max_ratio_present = true;
  }
  if (in_j.contains("rRMPolicyMinRatio")) {
    in_j["rRMPolicyMinRatio"].get_to(obj.rrm_policy_min_ratio);
    obj.rrm_policy_min_ratio_present = true;
  }
  if (in_j.contains("rRMPolicyDedicatedRatio")) {
    in_j["rRMPolicyDedicatedRatio"].get_to(obj.rrm_policy_ded_ratio);
    obj.rrm_policy_ded_ratio_present = true;
  }
}

// O-RUInfo ::= SEQUENCE
SRSASN_CODE o_ru_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(oru_user_plane_cfg_present, 1));
  HANDLE_CODE(bref.pack(oru_cap_present, 1));

  if (oru_user_plane_cfg_present) {
    HANDLE_CODE(oru_user_plane_cfg.pack(bref));
  }
  if (oru_cap_present) {
    HANDLE_CODE(oru_cap.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE o_ru_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(oru_user_plane_cfg_present, 1));
  HANDLE_CODE(bref.unpack(oru_cap_present, 1));

  if (oru_user_plane_cfg_present) {
    HANDLE_CODE(oru_user_plane_cfg.unpack(bref));
  }
  if (oru_cap_present) {
    HANDLE_CODE(oru_cap.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void o_ru_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (oru_user_plane_cfg_present) {
    j.write_fieldname("oruUserPlaneConfiguration");
    oru_user_plane_cfg.to_json(j);
  }
  if (oru_cap_present) {
    j.write_fieldname("oruCapabilities");
    oru_cap.to_json(j);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const o_ru_info_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.oru_user_plane_cfg_present) {
    j["oruUserPlaneConfiguration"] = obj.oru_user_plane_cfg;
  }
  if (obj.oru_cap_present) {
    j["oruCapabilities"] = obj.oru_cap;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, o_ru_info_s& obj)
{
  if (in_j.contains("oruUserPlaneConfiguration")) {
    in_j["oruUserPlaneConfiguration"].get_to(obj.oru_user_plane_cfg);
    obj.oru_user_plane_cfg_present = true;
  }
  if (in_j.contains("oruCapabilities")) {
    in_j["oruCapabilities"].get_to(obj.oru_cap);
    obj.oru_cap_present = true;
  }
}

// ControlService ::= SEQUENCE
SRSASN_CODE ctrl_service_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_ctrl_styles, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ctrl_service_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_ctrl_styles, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void ctrl_service_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfSupportedControlStyles");
  for (const auto& e1 : list_of_supported_ctrl_styles) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ctrl_service_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_supported_ctrl_styles) {
      j_array.push_back(e1);
    }
    j["listOfSupportedControlStyles"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ctrl_service_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfSupportedControlStyles");
    auto&       out_arr = obj.list_of_supported_ctrl_styles;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<ctrl_style_s>();
    }
  }
}

// E2SM-CCC-RAN-Configuration-Structure ::= CHOICE
void e2_sm_ccc_ran_cfg_structure_c::destroy_()
{
  switch (type_) {
    case types::o_gnb_cu_cp_function:
      c.destroy<o_gnb_cu_cp_function_s>();
      break;
    case types::o_gnb_cu_up_function:
      c.destroy<o_gnb_cu_up_function_s>();
      break;
    case types::o_gnb_du_function:
      c.destroy<o_gnb_du_function_s>();
      break;
    case types::o_nr_cell_cu:
      c.destroy<o_nr_cell_cu_s>();
      break;
    case types::o_nr_cell_du:
      c.destroy<o_nr_cell_du_s>();
      break;
    case types::o_rrm_policy_ratio:
      c.destroy<o_rrm_policy_ratio_s>();
      break;
    case types::o_bwp:
      c.destroy<o_bwp_s>();
      break;
    case types::o_ces_management_function:
      c.destroy<o_ces_management_function_s>();
      break;
    case types::o_nes_policy:
      c.destroy<o_nes_policy_s>();
      break;
    case types::o_cell_dtxdrx_cfg:
      c.destroy<o_cell_dtxdrx_cfg_s>();
      break;
    case types::o_ru_info:
      c.destroy<o_ru_info_s>();
      break;
    case types::o_prb_blanking_policy:
      c.destroy<o_prb_blanking_policy_s>();
      break;
    default:
      break;
  }
}
void e2_sm_ccc_ran_cfg_structure_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::o_gnb_cu_cp_function:
      c.init<o_gnb_cu_cp_function_s>();
      break;
    case types::o_gnb_cu_up_function:
      c.init<o_gnb_cu_up_function_s>();
      break;
    case types::o_gnb_du_function:
      c.init<o_gnb_du_function_s>();
      break;
    case types::o_nr_cell_cu:
      c.init<o_nr_cell_cu_s>();
      break;
    case types::o_nr_cell_du:
      c.init<o_nr_cell_du_s>();
      break;
    case types::o_rrm_policy_ratio:
      c.init<o_rrm_policy_ratio_s>();
      break;
    case types::o_bwp:
      c.init<o_bwp_s>();
      break;
    case types::o_ces_management_function:
      c.init<o_ces_management_function_s>();
      break;
    case types::o_nes_policy:
      c.init<o_nes_policy_s>();
      break;
    case types::o_cell_dtxdrx_cfg:
      c.init<o_cell_dtxdrx_cfg_s>();
      break;
    case types::o_ru_info:
      c.init<o_ru_info_s>();
      break;
    case types::o_prb_blanking_policy:
      c.init<o_prb_blanking_policy_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_ccc_ran_cfg_structure_c");
  }
}
e2_sm_ccc_ran_cfg_structure_c::e2_sm_ccc_ran_cfg_structure_c(const e2_sm_ccc_ran_cfg_structure_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::o_gnb_cu_cp_function:
      c.init(other.c.get<o_gnb_cu_cp_function_s>());
      break;
    case types::o_gnb_cu_up_function:
      c.init(other.c.get<o_gnb_cu_up_function_s>());
      break;
    case types::o_gnb_du_function:
      c.init(other.c.get<o_gnb_du_function_s>());
      break;
    case types::o_nr_cell_cu:
      c.init(other.c.get<o_nr_cell_cu_s>());
      break;
    case types::o_nr_cell_du:
      c.init(other.c.get<o_nr_cell_du_s>());
      break;
    case types::o_rrm_policy_ratio:
      c.init(other.c.get<o_rrm_policy_ratio_s>());
      break;
    case types::o_bwp:
      c.init(other.c.get<o_bwp_s>());
      break;
    case types::o_ces_management_function:
      c.init(other.c.get<o_ces_management_function_s>());
      break;
    case types::o_nes_policy:
      c.init(other.c.get<o_nes_policy_s>());
      break;
    case types::o_cell_dtxdrx_cfg:
      c.init(other.c.get<o_cell_dtxdrx_cfg_s>());
      break;
    case types::o_ru_info:
      c.init(other.c.get<o_ru_info_s>());
      break;
    case types::o_prb_blanking_policy:
      c.init(other.c.get<o_prb_blanking_policy_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_ccc_ran_cfg_structure_c");
  }
}
e2_sm_ccc_ran_cfg_structure_c& e2_sm_ccc_ran_cfg_structure_c::operator=(const e2_sm_ccc_ran_cfg_structure_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::o_gnb_cu_cp_function:
      c.set(other.c.get<o_gnb_cu_cp_function_s>());
      break;
    case types::o_gnb_cu_up_function:
      c.set(other.c.get<o_gnb_cu_up_function_s>());
      break;
    case types::o_gnb_du_function:
      c.set(other.c.get<o_gnb_du_function_s>());
      break;
    case types::o_nr_cell_cu:
      c.set(other.c.get<o_nr_cell_cu_s>());
      break;
    case types::o_nr_cell_du:
      c.set(other.c.get<o_nr_cell_du_s>());
      break;
    case types::o_rrm_policy_ratio:
      c.set(other.c.get<o_rrm_policy_ratio_s>());
      break;
    case types::o_bwp:
      c.set(other.c.get<o_bwp_s>());
      break;
    case types::o_ces_management_function:
      c.set(other.c.get<o_ces_management_function_s>());
      break;
    case types::o_nes_policy:
      c.set(other.c.get<o_nes_policy_s>());
      break;
    case types::o_cell_dtxdrx_cfg:
      c.set(other.c.get<o_cell_dtxdrx_cfg_s>());
      break;
    case types::o_ru_info:
      c.set(other.c.get<o_ru_info_s>());
      break;
    case types::o_prb_blanking_policy:
      c.set(other.c.get<o_prb_blanking_policy_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_ccc_ran_cfg_structure_c");
  }

  return *this;
}
o_gnb_cu_cp_function_s& e2_sm_ccc_ran_cfg_structure_c::set_o_gnb_cu_cp_function()
{
  set(types::o_gnb_cu_cp_function);
  return c.get<o_gnb_cu_cp_function_s>();
}
o_gnb_cu_up_function_s& e2_sm_ccc_ran_cfg_structure_c::set_o_gnb_cu_up_function()
{
  set(types::o_gnb_cu_up_function);
  return c.get<o_gnb_cu_up_function_s>();
}
o_gnb_du_function_s& e2_sm_ccc_ran_cfg_structure_c::set_o_gnb_du_function()
{
  set(types::o_gnb_du_function);
  return c.get<o_gnb_du_function_s>();
}
o_nr_cell_cu_s& e2_sm_ccc_ran_cfg_structure_c::set_o_nr_cell_cu()
{
  set(types::o_nr_cell_cu);
  return c.get<o_nr_cell_cu_s>();
}
o_nr_cell_du_s& e2_sm_ccc_ran_cfg_structure_c::set_o_nr_cell_du()
{
  set(types::o_nr_cell_du);
  return c.get<o_nr_cell_du_s>();
}
o_rrm_policy_ratio_s& e2_sm_ccc_ran_cfg_structure_c::set_o_rrm_policy_ratio()
{
  set(types::o_rrm_policy_ratio);
  return c.get<o_rrm_policy_ratio_s>();
}
o_bwp_s& e2_sm_ccc_ran_cfg_structure_c::set_o_bwp()
{
  set(types::o_bwp);
  return c.get<o_bwp_s>();
}
o_ces_management_function_s& e2_sm_ccc_ran_cfg_structure_c::set_o_ces_management_function()
{
  set(types::o_ces_management_function);
  return c.get<o_ces_management_function_s>();
}
o_nes_policy_s& e2_sm_ccc_ran_cfg_structure_c::set_o_nes_policy()
{
  set(types::o_nes_policy);
  return c.get<o_nes_policy_s>();
}
o_cell_dtxdrx_cfg_s& e2_sm_ccc_ran_cfg_structure_c::set_o_cell_dtxdrx_cfg()
{
  set(types::o_cell_dtxdrx_cfg);
  return c.get<o_cell_dtxdrx_cfg_s>();
}
o_ru_info_s& e2_sm_ccc_ran_cfg_structure_c::set_o_ru_info()
{
  set(types::o_ru_info);
  return c.get<o_ru_info_s>();
}
o_prb_blanking_policy_s& e2_sm_ccc_ran_cfg_structure_c::set_o_prb_blanking_policy()
{
  set(types::o_prb_blanking_policy);
  return c.get<o_prb_blanking_policy_s>();
}
void e2_sm_ccc_ran_cfg_structure_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::o_gnb_cu_cp_function:
      j.write_fieldname("o-GnbCuCpFunction");
      c.get<o_gnb_cu_cp_function_s>().to_json(j);
      break;
    case types::o_gnb_cu_up_function:
      j.write_fieldname("o-GnbCuUpFunction");
      c.get<o_gnb_cu_up_function_s>().to_json(j);
      break;
    case types::o_gnb_du_function:
      j.write_fieldname("o-GnbDuFunction");
      c.get<o_gnb_du_function_s>().to_json(j);
      break;
    case types::o_nr_cell_cu:
      j.write_fieldname("o-NrCellCu");
      c.get<o_nr_cell_cu_s>().to_json(j);
      break;
    case types::o_nr_cell_du:
      j.write_fieldname("o-NrCellDu");
      c.get<o_nr_cell_du_s>().to_json(j);
      break;
    case types::o_rrm_policy_ratio:
      j.write_fieldname("o-RRMPolicyRatio");
      c.get<o_rrm_policy_ratio_s>().to_json(j);
      break;
    case types::o_bwp:
      j.write_fieldname("o-Bwp");
      c.get<o_bwp_s>().to_json(j);
      break;
    case types::o_ces_management_function:
      j.write_fieldname("o-CESManagementFunction");
      c.get<o_ces_management_function_s>().to_json(j);
      break;
    case types::o_nes_policy:
      j.write_fieldname("o-NESPolicy");
      c.get<o_nes_policy_s>().to_json(j);
      break;
    case types::o_cell_dtxdrx_cfg:
      j.write_fieldname("o-CellDTXDRXConfig");
      c.get<o_cell_dtxdrx_cfg_s>().to_json(j);
      break;
    case types::o_ru_info:
      j.write_fieldname("o-RUInfo");
      c.get<o_ru_info_s>().to_json(j);
      break;
    case types::o_prb_blanking_policy:
      j.write_fieldname("o-PRBBlankingPolicy");
      c.get<o_prb_blanking_policy_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_ccc_ran_cfg_structure_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_sm_ccc_ran_cfg_structure_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::o_gnb_cu_cp_function:
      HANDLE_CODE(c.get<o_gnb_cu_cp_function_s>().pack(bref));
      break;
    case types::o_gnb_cu_up_function:
      HANDLE_CODE(c.get<o_gnb_cu_up_function_s>().pack(bref));
      break;
    case types::o_gnb_du_function:
      HANDLE_CODE(c.get<o_gnb_du_function_s>().pack(bref));
      break;
    case types::o_nr_cell_cu:
      HANDLE_CODE(c.get<o_nr_cell_cu_s>().pack(bref));
      break;
    case types::o_nr_cell_du:
      HANDLE_CODE(c.get<o_nr_cell_du_s>().pack(bref));
      break;
    case types::o_rrm_policy_ratio:
      HANDLE_CODE(c.get<o_rrm_policy_ratio_s>().pack(bref));
      break;
    case types::o_bwp:
      HANDLE_CODE(c.get<o_bwp_s>().pack(bref));
      break;
    case types::o_ces_management_function:
      HANDLE_CODE(c.get<o_ces_management_function_s>().pack(bref));
      break;
    case types::o_nes_policy:
      HANDLE_CODE(c.get<o_nes_policy_s>().pack(bref));
      break;
    case types::o_cell_dtxdrx_cfg:
      HANDLE_CODE(c.get<o_cell_dtxdrx_cfg_s>().pack(bref));
      break;
    case types::o_ru_info:
      HANDLE_CODE(c.get<o_ru_info_s>().pack(bref));
      break;
    case types::o_prb_blanking_policy:
      HANDLE_CODE(c.get<o_prb_blanking_policy_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_ccc_ran_cfg_structure_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ran_cfg_structure_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::o_gnb_cu_cp_function:
      HANDLE_CODE(c.get<o_gnb_cu_cp_function_s>().unpack(bref));
      break;
    case types::o_gnb_cu_up_function:
      HANDLE_CODE(c.get<o_gnb_cu_up_function_s>().unpack(bref));
      break;
    case types::o_gnb_du_function:
      HANDLE_CODE(c.get<o_gnb_du_function_s>().unpack(bref));
      break;
    case types::o_nr_cell_cu:
      HANDLE_CODE(c.get<o_nr_cell_cu_s>().unpack(bref));
      break;
    case types::o_nr_cell_du:
      HANDLE_CODE(c.get<o_nr_cell_du_s>().unpack(bref));
      break;
    case types::o_rrm_policy_ratio:
      HANDLE_CODE(c.get<o_rrm_policy_ratio_s>().unpack(bref));
      break;
    case types::o_bwp:
      HANDLE_CODE(c.get<o_bwp_s>().unpack(bref));
      break;
    case types::o_ces_management_function:
      HANDLE_CODE(c.get<o_ces_management_function_s>().unpack(bref));
      break;
    case types::o_nes_policy:
      HANDLE_CODE(c.get<o_nes_policy_s>().unpack(bref));
      break;
    case types::o_cell_dtxdrx_cfg:
      HANDLE_CODE(c.get<o_cell_dtxdrx_cfg_s>().unpack(bref));
      break;
    case types::o_ru_info:
      HANDLE_CODE(c.get<o_ru_info_s>().unpack(bref));
      break;
    case types::o_prb_blanking_policy:
      HANDLE_CODE(c.get<o_prb_blanking_policy_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_ccc_ran_cfg_structure_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ran_cfg_structure_c& obj)
{
  switch (obj.type()) {
    case e2_sm_ccc_ran_cfg_structure_c::types::o_gnb_cu_cp_function:
      out_j = obj.o_gnb_cu_cp_function();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_gnb_cu_up_function:
      out_j = obj.o_gnb_cu_up_function();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_gnb_du_function:
      out_j = obj.o_gnb_du_function();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_nr_cell_cu:
      out_j = obj.o_nr_cell_cu();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_nr_cell_du:
      out_j = obj.o_nr_cell_du();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_rrm_policy_ratio:
      out_j = obj.o_rrm_policy_ratio();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_bwp:
      out_j = obj.o_bwp();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_ces_management_function:
      out_j = obj.o_ces_management_function();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_nes_policy:
      out_j = obj.o_nes_policy();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_cell_dtxdrx_cfg:
      out_j = obj.o_cell_dtxdrx_cfg();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_ru_info:
      out_j = obj.o_ru_info();
      break;
    case e2_sm_ccc_ran_cfg_structure_c::types::o_prb_blanking_policy:
      out_j = obj.o_prb_blanking_policy();
      break;
    default:
      log_invalid_choice_id(obj.type(), "e2_sm_ccc_ran_cfg_structure_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ran_cfg_structure_c& obj)
{
  // TODO: make sure the types are recognized correctly.
  if (in_j.find("x2BlockList") != in_j.end()) {
    auto& format = obj.set_o_gnb_cu_cp_function();
    in_j.get_to(format);
  } else if (in_j.find("plmnInfoList") != in_j.end()) {
    auto& format = obj.set_o_gnb_cu_up_function();
    in_j.get_to(format);
  } else if (in_j.find("o_gnb_du_function") != in_j.end()) {
    auto& format = obj.set_o_gnb_du_function();
    in_j.get_to(format);
  } else if (in_j.find("cellLocalId") != in_j.end()) {
    auto& format = obj.set_o_nr_cell_cu();
    in_j.get_to(format);
  } else if (in_j.find("bwpList") != in_j.end()) {
    auto& format = obj.set_o_nr_cell_du();
    in_j.get_to(format);
  } else if (in_j.find("rRMPolicyMemberList") != in_j.end()) {
    auto& format = obj.set_o_rrm_policy_ratio();
    in_j.get_to(format);
  } else if (in_j.find("bwpContext") != in_j.end()) {
    auto& format = obj.set_o_bwp();
    in_j.get_to(format);
  } else if (in_j.find("energySavingState") != in_j.end()) {
    auto& format = obj.set_o_ces_management_function();
    in_j.get_to(format);
  } else if (in_j.find("policyType") != in_j.end()) {
    auto& format = obj.set_o_nes_policy();
    in_j.get_to(format);
  } else if (in_j.find("cycleStartOffset") != in_j.end()) {
    auto& format = obj.set_o_cell_dtxdrx_cfg();
    in_j.get_to(format);
  } else if (in_j.find("oruUserPlaneConfiguration") != in_j.end()) {
    auto& format = obj.set_o_ru_info();
    in_j.get_to(format);
  } else if (in_j.find("prbBlankingConfigurationList") != in_j.end()) {
    auto& format = obj.set_o_prb_blanking_policy();
    in_j.get_to(format);
  }
}

const char* e2_sm_ccc_ran_cfg_structure_c::types_opts::to_string() const
{
  static const char* names[] = {"o-GnbCuCpFunction",
                                "o-GnbCuUpFunction",
                                "o-GnbDuFunction",
                                "o-NrCellCu",
                                "o-NrCellDu",
                                "o-RRMPolicyRatio",
                                "o-Bwp",
                                "o-CESManagementFunction",
                                "o-NESPolicy",
                                "o-CellDTXDRXConfig",
                                "o-RUInfo",
                                "o-PRBBlankingPolicy"};
  return convert_enum_idx(names, 12, value, "e2_sm_ccc_ran_cfg_structure_c::types");
}
bool e2_sm_ccc_ran_cfg_structure_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"o-GnbCuCpFunction",
                                "o-GnbCuUpFunction",
                                "o-GnbDuFunction",
                                "o-NrCellCu",
                                "o-NrCellDu",
                                "o-RRMPolicyRatio",
                                "o-Bwp",
                                "o-CESManagementFunction",
                                "o-NESPolicy",
                                "o-CellDTXDRXConfig",
                                "o-RUInfo",
                                "o-PRBBlankingPolicy"};
  uint32_t           idx;
  if (convert_enum_str(names, 12, str.c_str(), idx, "e2_sm_ccc_ran_cfg_structure_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// EventTrigger ::= SEQUENCE
SRSASN_CODE event_trigger_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_event_trigger_styles, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_event_trigger_styles, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void event_trigger_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfSupportedEventTriggerStyles");
  for (const auto& e1 : list_of_supported_event_trigger_styles) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const event_trigger_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_supported_event_trigger_styles) {
      j_array.push_back(e1);
    }
    j["listOfSupportedEventTriggerStyles"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, event_trigger_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfSupportedEventTriggerStyles");
    auto&       out_arr = obj.list_of_supported_event_trigger_styles;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<event_trigger_style_s>();
    }
  }
}

// QueryService ::= SEQUENCE
SRSASN_CODE query_service_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_query_styles, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE query_service_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_query_styles, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void query_service_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfSupportedQueryStyles");
  for (const auto& e1 : list_of_supported_query_styles) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const query_service_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_supported_query_styles) {
      j_array.push_back(e1);
    }
    j["listOfSupportedQueryStyles"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, query_service_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfSupportedQueryStyles");
    auto&       out_arr = obj.list_of_supported_query_styles;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<query_style_s>();
    }
  }
}

// ReportService ::= SEQUENCE
SRSASN_CODE report_service_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_report_styles, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_service_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_report_styles, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void report_service_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfSupportedReportStyles");
  for (const auto& e1 : list_of_supported_report_styles) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const report_service_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_supported_report_styles) {
      j_array.push_back(e1);
    }
    j["listOfSupportedReportStyles"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, report_service_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfSupportedReportStyles");
    auto&       out_arr = obj.list_of_supported_report_styles;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<report_style_s>();
    }
  }
}

// AttributeName ::= SEQUENCE
SRSASN_CODE attribute_name_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(attribute_name.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE attribute_name_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(attribute_name.unpack(bref));

  return SRSASN_SUCCESS;
}
void attribute_name_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("attributeName", attribute_name.to_string());
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const attribute_name_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["attributeName"]        = obj.attribute_name.to_string();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, attribute_name_s& obj)
{
  obj.attribute_name.from_string(in_j["attributeName"].get<std::string>());
}

// Cause ::= ENUMERATED
const char* cause_opts::to_string() const
{
  static const char* names[] = {
      "NotSupported", "NotAvailable", "IncompatibleState", "JsonError", "SemanticError", "Unspecified"};
  return convert_enum_idx(names, 6, value, "cause_e");
}
bool cause_opts::from_string(const std::string& str)
{
  static const char* names[] = {
      "NotSupported", "NotAvailable", "IncompatibleState", "JsonError", "SemanticError", "Unspecified"};
  uint32_t idx;
  if (convert_enum_str(names, 6, str.c_str(), idx, "cause_e")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// RICServices ::= SEQUENCE
SRSASN_CODE ric_services_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(event_trigger_present, 1));
  HANDLE_CODE(bref.pack(report_service_present, 1));
  HANDLE_CODE(bref.pack(insert_service_present, 1));
  HANDLE_CODE(bref.pack(ctrl_service_present, 1));
  HANDLE_CODE(bref.pack(policy_service_present, 1));
  HANDLE_CODE(bref.pack(query_service_present, 1));

  if (event_trigger_present) {
    HANDLE_CODE(event_trigger.pack(bref));
  }
  if (report_service_present) {
    HANDLE_CODE(report_service.pack(bref));
  }
  if (insert_service_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, insert_service, false, true));
  }
  if (ctrl_service_present) {
    HANDLE_CODE(ctrl_service.pack(bref));
  }
  if (policy_service_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, policy_service, false, true));
  }
  if (query_service_present) {
    HANDLE_CODE(query_service.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_services_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(event_trigger_present, 1));
  HANDLE_CODE(bref.unpack(report_service_present, 1));
  HANDLE_CODE(bref.unpack(insert_service_present, 1));
  HANDLE_CODE(bref.unpack(ctrl_service_present, 1));
  HANDLE_CODE(bref.unpack(policy_service_present, 1));
  HANDLE_CODE(bref.unpack(query_service_present, 1));

  if (event_trigger_present) {
    HANDLE_CODE(event_trigger.unpack(bref));
  }
  if (report_service_present) {
    HANDLE_CODE(report_service.unpack(bref));
  }
  if (insert_service_present) {
    HANDLE_CODE(unpack_unconstrained_integer(insert_service, bref, false, true));
  }
  if (ctrl_service_present) {
    HANDLE_CODE(ctrl_service.unpack(bref));
  }
  if (policy_service_present) {
    HANDLE_CODE(unpack_unconstrained_integer(policy_service, bref, false, true));
  }
  if (query_service_present) {
    HANDLE_CODE(query_service.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ric_services_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (event_trigger_present) {
    j.write_fieldname("eventTrigger");
    event_trigger.to_json(j);
  }
  if (report_service_present) {
    j.write_fieldname("reportService");
    report_service.to_json(j);
  }
  if (insert_service_present) {
    j.write_int("insertService", insert_service);
  }
  if (ctrl_service_present) {
    j.write_fieldname("controlService");
    ctrl_service.to_json(j);
  }
  if (policy_service_present) {
    j.write_int("policyService", policy_service);
  }
  if (query_service_present) {
    j.write_fieldname("queryService");
    query_service.to_json(j);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_services_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.event_trigger_present) {
    j["eventTrigger"] = obj.event_trigger;
  }
  if (obj.report_service_present) {
    j["reportService"] = obj.report_service;
  }
  if (obj.insert_service_present) {
    j["insertService"] = obj.insert_service;
  }
  if (obj.ctrl_service_present) {
    j["controlService"] = obj.ctrl_service;
  }
  if (obj.policy_service_present) {
    j["policyService"] = obj.policy_service;
  }
  if (obj.query_service_present) {
    j["queryService"] = obj.query_service;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_services_s& obj)
{
  if (in_j.contains("eventTrigger")) {
    in_j["eventTrigger"].get_to(obj.event_trigger);
    obj.event_trigger_present = true;
  }
  if (in_j.contains("reportService")) {
    in_j["reportService"].get_to(obj.report_service);
    obj.report_service_present = true;
  }
  if (in_j.contains("insertService")) {
    in_j["insertService"].get_to(obj.insert_service);
    obj.insert_service_present = true;
  }
  if (in_j.contains("controlService")) {
    in_j["controlService"].get_to(obj.ctrl_service);
    obj.ctrl_service_present = true;
  }
  if (in_j.contains("policyService")) {
    in_j["policyService"].get_to(obj.policy_service);
    obj.policy_service_present = true;
  }
  if (in_j.contains("queryService")) {
    in_j["queryService"].get_to(obj.query_service);
    obj.query_service_present = true;
  }
}

// ValuesOfAttributes ::= SEQUENCE
SRSASN_CODE values_of_attributes_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ran_cfg_structure.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE values_of_attributes_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ran_cfg_structure.unpack(bref));

  return SRSASN_SUCCESS;
}
void values_of_attributes_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ranConfigurationStructure");
  ran_cfg_structure.to_json(j);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const values_of_attributes_s& obj)
{
  nlohmann::ordered_json& j      = out_j;
  j["ranConfigurationStructure"] = obj.ran_cfg_structure;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, values_of_attributes_s& obj)
{
  in_j["ranConfigurationStructure"].get_to(obj.ran_cfg_structure);
}

// Attribute ::= SEQUENCE
SRSASN_CODE attribute_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(attribute_name.pack(bref));
  HANDLE_CODE(supported_services.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE attribute_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(attribute_name.unpack(bref));
  HANDLE_CODE(supported_services.unpack(bref));

  return SRSASN_SUCCESS;
}
void attribute_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("attributeName", attribute_name.to_string());
  j.write_fieldname("supportedServices");
  supported_services.to_json(j);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const attribute_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["attributeName"]        = obj.attribute_name.to_string();
  j["supportedServices"]    = obj.supported_services;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, attribute_s& obj)
{
  obj.attribute_name.from_string(in_j["attributeName"].get<std::string>());
  in_j["supportedServices"].get_to(obj.supported_services);
}

// CelllevelConfigurationStructuresForQuery ::= SEQUENCE
SRSASN_CODE celllevel_cfg_structures_for_query_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_attributes.size() > 0, 1));

  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  if (list_of_attributes.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_attributes, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE celllevel_cfg_structures_for_query_s::unpack(cbit_ref& bref)
{
  bool list_of_attributes_present;
  HANDLE_CODE(bref.unpack(list_of_attributes_present, 1));

  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  if (list_of_attributes_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_attributes, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void celllevel_cfg_structures_for_query_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  if (list_of_attributes.size() > 0) {
    j.start_array("listOfAttributes");
    for (const auto& e1 : list_of_attributes) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const celllevel_cfg_structures_for_query_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  if (obj.list_of_attributes.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_attributes) {
        j_array.push_back(e1);
      }
      j["listOfAttributes"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, celllevel_cfg_structures_for_query_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  if (in_j.contains("listOfAttributes")) {
    {
      const auto& in_arr  = in_j.at("listOfAttributes");
      auto&       out_arr = obj.list_of_attributes;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<attribute_name_s>();
      }
    }
  }
}

// ConfigurationStructureAccepted ::= SEQUENCE
SRSASN_CODE cfg_structure_accepted_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(applied_timestamp_present, 1));

  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  HANDLE_CODE(old_values_of_attributes.pack(bref));
  HANDLE_CODE(current_values_of_attributes.pack(bref));
  if (applied_timestamp_present) {
    HANDLE_CODE(applied_timestamp.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_structure_accepted_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(applied_timestamp_present, 1));

  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  HANDLE_CODE(old_values_of_attributes.unpack(bref));
  HANDLE_CODE(current_values_of_attributes.unpack(bref));
  if (applied_timestamp_present) {
    HANDLE_CODE(applied_timestamp.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cfg_structure_accepted_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  j.write_fieldname("oldValuesOfAttributes");
  old_values_of_attributes.to_json(j);
  j.write_fieldname("currentValuesOfAttributes");
  current_values_of_attributes.to_json(j);
  if (applied_timestamp_present) {
    j.write_str("appliedTimestamp", applied_timestamp.to_string());
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cfg_structure_accepted_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  j["oldValuesOfAttributes"]         = obj.old_values_of_attributes;
  j["currentValuesOfAttributes"]     = obj.current_values_of_attributes;
  if (obj.applied_timestamp_present) {
    j["appliedTimestamp"] = obj.applied_timestamp.to_string();
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cfg_structure_accepted_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  in_j["oldValuesOfAttributes"].get_to(obj.old_values_of_attributes);
  in_j["currentValuesOfAttributes"].get_to(obj.current_values_of_attributes);
  if (in_j.contains("appliedTimestamp")) {
    obj.applied_timestamp.from_string(in_j["appliedTimestamp"].get<std::string>());
    obj.applied_timestamp_present = true;
  }
}

// ConfigurationStructureFailed ::= SEQUENCE
SRSASN_CODE cfg_structure_failed_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  HANDLE_CODE(old_values_of_attributes.pack(bref));
  HANDLE_CODE(requested_values_of_attributes.pack(bref));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_structure_failed_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  HANDLE_CODE(old_values_of_attributes.unpack(bref));
  HANDLE_CODE(requested_values_of_attributes.unpack(bref));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void cfg_structure_failed_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  j.write_fieldname("oldValuesOfAttributes");
  old_values_of_attributes.to_json(j);
  j.write_fieldname("requestedValuesOfAttributes");
  requested_values_of_attributes.to_json(j);
  j.write_str("cause", cause.to_string());
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cfg_structure_failed_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  j["oldValuesOfAttributes"]         = obj.old_values_of_attributes;
  j["requestedValuesOfAttributes"]   = obj.requested_values_of_attributes;
  j["cause"]                         = obj.cause.to_string();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cfg_structure_failed_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  in_j["oldValuesOfAttributes"].get_to(obj.old_values_of_attributes);
  in_j["requestedValuesOfAttributes"].get_to(obj.requested_values_of_attributes);
  obj.cause.from_string(in_j["cause"].get<std::string>());
}

// ConfigurationStructure ::= SEQUENCE
SRSASN_CODE cfg_structure_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(old_values_of_attributes_present, 1));

  HANDLE_CODE(change_type.pack(bref));
  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  HANDLE_CODE(values_of_attributes.pack(bref));
  if (old_values_of_attributes_present) {
    HANDLE_CODE(old_values_of_attributes.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_structure_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(old_values_of_attributes_present, 1));

  HANDLE_CODE(change_type.unpack(bref));
  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  HANDLE_CODE(values_of_attributes.unpack(bref));
  if (old_values_of_attributes_present) {
    HANDLE_CODE(old_values_of_attributes.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cfg_structure_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("changeType", change_type.to_string());
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  j.write_fieldname("valuesOfAttributes");
  values_of_attributes.to_json(j);
  if (old_values_of_attributes_present) {
    j.write_fieldname("oldValuesOfAttributes");
    old_values_of_attributes.to_json(j);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cfg_structure_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["changeType"]                    = obj.change_type.to_string();
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  j["valuesOfAttributes"]            = obj.values_of_attributes;
  if (obj.old_values_of_attributes_present) {
    j["oldValuesOfAttributes"] = obj.old_values_of_attributes;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cfg_structure_s& obj)
{
  obj.change_type.from_string(in_j["changeType"].get<std::string>());
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  in_j["valuesOfAttributes"].get_to(obj.values_of_attributes);
  if (in_j.contains("oldValuesOfAttributes")) {
    in_j["oldValuesOfAttributes"].get_to(obj.old_values_of_attributes);
    obj.old_values_of_attributes_present = true;
  }
}

const char* cfg_structure_s::change_type_opts::to_string() const
{
  static const char* names[] = {"none", "modification", "addition", "deletion"};
  return convert_enum_idx(names, 4, value, "cfg_structure_s::change_type_e_");
}
bool cfg_structure_s::change_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"none", "modification", "addition", "deletion"};
  uint32_t           idx;
  if (convert_enum_str(names, 4, str.c_str(), idx, "cfg_structure_s::change_type_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t cfg_structure_s::change_type_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cfg_structure_s::change_type_e_");
}

// ConfigurationStructureWrite ::= SEQUENCE
SRSASN_CODE cfg_structure_write_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  HANDLE_CODE(old_values_of_attributes.pack(bref));
  HANDLE_CODE(new_values_of_attributes.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_structure_write_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  HANDLE_CODE(old_values_of_attributes.unpack(bref));
  HANDLE_CODE(new_values_of_attributes.unpack(bref));

  return SRSASN_SUCCESS;
}
void cfg_structure_write_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  j.write_fieldname("oldValuesOfAttributes");
  old_values_of_attributes.to_json(j);
  j.write_fieldname("newValuesOfAttributes");
  new_values_of_attributes.to_json(j);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cfg_structure_write_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  j["oldValuesOfAttributes"]         = obj.old_values_of_attributes;
  j["newValuesOfAttributes"]         = obj.new_values_of_attributes;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cfg_structure_write_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  in_j["oldValuesOfAttributes"].get_to(obj.old_values_of_attributes);
  in_j["newValuesOfAttributes"].get_to(obj.new_values_of_attributes);
}

// ConfigurationStructuresToBeReportedForQuery ::= SEQUENCE
SRSASN_CODE cfg_structures_to_be_reported_for_query_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  HANDLE_CODE(values_of_attributes.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_structures_to_be_reported_for_query_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  HANDLE_CODE(values_of_attributes.unpack(bref));

  return SRSASN_SUCCESS;
}
void cfg_structures_to_be_reported_for_query_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  j.write_fieldname("valuesOfAttributes");
  values_of_attributes.to_json(j);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cfg_structures_to_be_reported_for_query_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  j["valuesOfAttributes"]            = obj.values_of_attributes;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cfg_structures_to_be_reported_for_query_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  in_j["valuesOfAttributes"].get_to(obj.values_of_attributes);
}

// EUTRA-CGI ::= SEQUENCE
SRSASN_CODE eutra_cgi_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(eutra_cell_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_cgi_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(eutra_cell_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void eutra_cgi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmnIdentity");
  plmn_id.to_json(j);
  j.write_str("eUTRACellIdentity", eutra_cell_id.to_string());
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const eutra_cgi_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["plmnIdentity"]         = obj.plmn_id;
  j["eUTRACellIdentity"]    = obj.eutra_cell_id.to_string();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, eutra_cgi_s& obj)
{
  in_j["plmnIdentity"].get_to(obj.plmn_id);
  obj.eutra_cell_id.from_string(in_j["eUTRACellIdentity"].get<std::string>());
}

// NR-CGI ::= SEQUENCE
SRSASN_CODE nr_cgi_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(nr_cell_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_cgi_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(nr_cell_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void nr_cgi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmnIdentity");
  plmn_id.to_json(j);
  j.write_str("nRCellIdentity", nr_cell_id.to_string());
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const nr_cgi_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["plmnIdentity"]         = obj.plmn_id;

  std::stringstream ss;
  ss << std::uppercase << std::hex << std::setw(9) << std::setfill('0') << obj.nr_cell_id.to_number();
  j["nRCellIdentity"] = ss.str();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, nr_cgi_s& obj)
{
  in_j["plmnIdentity"].get_to(obj.plmn_id);
  std::string       hex_str = in_j["nRCellIdentity"].get<std::string>();
  uint64_t          value   = 0;
  std::stringstream ss(hex_str);
  ss >> std::hex >> value;
  if (ss.fail()) {
    fmt::println("Failed to parse nRCellIdentity hex string.");
  } else {
    obj.nr_cell_id.from_number(value);
  }
}

// RANConfigurationStructureForADF ::= SEQUENCE
SRSASN_CODE ran_cfg_structure_for_adf_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_attributes.size() > 0, 1));

  HANDLE_CODE(report_type.pack(bref));
  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  if (list_of_attributes.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_attributes, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_cfg_structure_for_adf_s::unpack(cbit_ref& bref)
{
  bool list_of_attributes_present;
  HANDLE_CODE(bref.unpack(list_of_attributes_present, 1));

  HANDLE_CODE(report_type.unpack(bref));
  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  if (list_of_attributes_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_attributes, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void ran_cfg_structure_for_adf_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reportType", report_type.to_string());
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  if (list_of_attributes.size() > 0) {
    j.start_array("listOfAttributes");
    for (const auto& e1 : list_of_attributes) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ran_cfg_structure_for_adf_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["reportType"]                    = obj.report_type.to_string();
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  if (obj.list_of_attributes.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_attributes) {
        j_array.push_back(e1);
      }
      j["listOfAttributes"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ran_cfg_structure_for_adf_s& obj)
{
  obj.report_type.from_string(in_j["reportType"].get<std::string>());
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  if (in_j.contains("listOfAttributes")) {
    {
      const auto& in_arr  = in_j.at("listOfAttributes");
      auto&       out_arr = obj.list_of_attributes;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<attribute_name_s>();
      }
    }
  }
}

const char* ran_cfg_structure_for_adf_s::report_type_opts::to_string() const
{
  static const char* names[] = {"all", "change"};
  return convert_enum_idx(names, 2, value, "ran_cfg_structure_for_adf_s::report_type_e_");
}
bool ran_cfg_structure_for_adf_s::report_type_opts::from_string(const std::string& str)
{
  static const char* names[] = {"all", "change"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "ran_cfg_structure_for_adf_s::report_type_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// RANConfigurationStructureForEventTrigger ::= SEQUENCE
SRSASN_CODE ran_cfg_structure_for_event_trigger_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_attributes.size() > 0, 1));

  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  if (list_of_attributes.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_attributes, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_cfg_structure_for_event_trigger_s::unpack(cbit_ref& bref)
{
  bool list_of_attributes_present;
  HANDLE_CODE(bref.unpack(list_of_attributes_present, 1));

  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  if (list_of_attributes_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_attributes, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void ran_cfg_structure_for_event_trigger_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  if (list_of_attributes.size() > 0) {
    j.start_array("listOfAttributes");
    for (const auto& e1 : list_of_attributes) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ran_cfg_structure_for_event_trigger_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  if (obj.list_of_attributes.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_attributes) {
        j_array.push_back(e1.to_string());
      }
      j["listOfAttributes"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ran_cfg_structure_for_event_trigger_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  if (in_j.contains("listOfAttributes")) {
    {
      const auto& in_arr  = in_j.at("listOfAttributes");
      auto&       out_arr = obj.list_of_attributes;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i].from_string(in_arr[i].get<std::string>());
      }
    }
  }
}

// CellGlobalId ::= CHOICE
void cell_global_id_c::destroy_()
{
  switch (type_) {
    case types::nr_cgi:
      c.destroy<nr_cgi_s>();
      break;
    case types::eutra_cgi:
      c.destroy<eutra_cgi_s>();
      break;
    default:
      break;
  }
}
void cell_global_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_cgi:
      c.init<nr_cgi_s>();
      break;
    case types::eutra_cgi:
      c.init<eutra_cgi_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_c");
  }
}
cell_global_id_c::cell_global_id_c(const cell_global_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_cgi:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::eutra_cgi:
      c.init(other.c.get<eutra_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_c");
  }
}
cell_global_id_c& cell_global_id_c::operator=(const cell_global_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_cgi:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::eutra_cgi:
      c.set(other.c.get<eutra_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_c");
  }

  return *this;
}
nr_cgi_s& cell_global_id_c::set_nr_cgi()
{
  set(types::nr_cgi);
  return c.get<nr_cgi_s>();
}
eutra_cgi_s& cell_global_id_c::set_eutra_cgi()
{
  set(types::eutra_cgi);
  return c.get<eutra_cgi_s>();
}
void cell_global_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_cgi:
      j.write_fieldname("nR-CGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    case types::eutra_cgi:
      j.write_fieldname("eUTRA-CGI");
      c.get<eutra_cgi_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_c");
  }
  j.end_obj();
}
SRSASN_CODE cell_global_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_cgi:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    case types::eutra_cgi:
      HANDLE_CODE(c.get<eutra_cgi_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_global_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_cgi:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    case types::eutra_cgi:
      HANDLE_CODE(c.get<eutra_cgi_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cell_global_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cell_global_id_c& obj)
{
  nlohmann::ordered_json& j = out_j;
  switch (obj.type()) {
    case cell_global_id_c::types::nr_cgi:
      j = obj.nr_cgi();
      break;
    case cell_global_id_c::types::eutra_cgi:
      j = obj.eutra_cgi();
      break;
    default:
      log_invalid_choice_id(obj.type(), "cell_global_id_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cell_global_id_c& obj)
{
  if (in_j.find("nRCellIdentity") != in_j.end()) {
    auto& format = obj.set_nr_cgi();
    in_j.get_to(format);
  } else {
    auto& format = obj.set_eutra_cgi();
    in_j.get_to(format);
  }
}

const char* cell_global_id_c::types_opts::to_string() const
{
  static const char* names[] = {"nR-CGI", "eUTRA-CGI"};
  return convert_enum_idx(names, 2, value, "cell_global_id_c::types");
}
bool cell_global_id_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"nR-CGI", "eUTRA-CGI"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "cell_global_id_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// CellConfigurationToBeReportedForADF ::= SEQUENCE
SRSASN_CODE cell_cfg_to_be_reported_for_adf_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_global_id_present, 1));
  HANDLE_CODE(bref.pack(list_of_cell_level_ran_cfg_structures_for_adf.size() > 0, 1));

  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.pack(bref));
  }
  if (list_of_cell_level_ran_cfg_structures_for_adf.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cell_level_ran_cfg_structures_for_adf, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_cfg_to_be_reported_for_adf_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_global_id_present, 1));
  bool list_of_cell_level_ran_cfg_structures_for_adf_present;
  HANDLE_CODE(bref.unpack(list_of_cell_level_ran_cfg_structures_for_adf_present, 1));

  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.unpack(bref));
  }
  if (list_of_cell_level_ran_cfg_structures_for_adf_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_cell_level_ran_cfg_structures_for_adf, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void cell_cfg_to_be_reported_for_adf_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_global_id_present) {
    j.write_fieldname("cellGlobalId");
    cell_global_id.to_json(j);
  }
  if (list_of_cell_level_ran_cfg_structures_for_adf.size() > 0) {
    j.start_array("listOfCellLevelRANConfigurationStructuresForADF");
    for (const auto& e1 : list_of_cell_level_ran_cfg_structures_for_adf) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cell_cfg_to_be_reported_for_adf_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.cell_global_id_present) {
    j["cellGlobalId"] = obj.cell_global_id;
  }
  if (obj.list_of_cell_level_ran_cfg_structures_for_adf.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_cell_level_ran_cfg_structures_for_adf) {
        j_array.push_back(e1);
      }
      j["listOfCellLevelRANConfigurationStructuresForADF"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cell_cfg_to_be_reported_for_adf_s& obj)
{
  if (in_j.contains("cellGlobalId")) {
    in_j["cellGlobalId"].get_to(obj.cell_global_id);
    obj.cell_global_id_present = true;
  }
  if (in_j.contains("listOfCellLevelRANConfigurationStructuresForADF")) {
    {
      const auto& in_arr  = in_j.at("listOfCellLevelRANConfigurationStructuresForADF");
      auto&       out_arr = obj.list_of_cell_level_ran_cfg_structures_for_adf;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<ran_cfg_structure_for_adf_s>();
      }
    }
  }
}

// CellControlOutcome ::= SEQUENCE
SRSASN_CODE cell_ctrl_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ran_cfg_structures_accepted_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ran_cfg_structures_failed_list.size() > 0, 1));

  HANDLE_CODE(cell_global_id.pack(bref));
  if (ran_cfg_structures_accepted_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_cfg_structures_accepted_list, 1, 64, true));
  }
  if (ran_cfg_structures_failed_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_cfg_structures_failed_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_ctrl_outcome_s::unpack(cbit_ref& bref)
{
  bool ran_cfg_structures_accepted_list_present;
  HANDLE_CODE(bref.unpack(ran_cfg_structures_accepted_list_present, 1));
  bool ran_cfg_structures_failed_list_present;
  HANDLE_CODE(bref.unpack(ran_cfg_structures_failed_list_present, 1));

  HANDLE_CODE(cell_global_id.unpack(bref));
  if (ran_cfg_structures_accepted_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ran_cfg_structures_accepted_list, bref, 1, 64, true));
  }
  if (ran_cfg_structures_failed_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ran_cfg_structures_failed_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void cell_ctrl_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellGlobalId");
  cell_global_id.to_json(j);
  if (ran_cfg_structures_accepted_list.size() > 0) {
    j.start_array("ranConfigurationStructuresAcceptedList");
    for (const auto& e1 : ran_cfg_structures_accepted_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_cfg_structures_failed_list.size() > 0) {
    j.start_array("ranConfigurationStructuresFailedList");
    for (const auto& e1 : ran_cfg_structures_failed_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cell_ctrl_outcome_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["cellGlobalId"]         = obj.cell_global_id;
  if (obj.ran_cfg_structures_accepted_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.ran_cfg_structures_accepted_list) {
        j_array.push_back(e1);
      }
      j["ranConfigurationStructuresAcceptedList"] = j_array;
    }
  }
  if (obj.ran_cfg_structures_failed_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.ran_cfg_structures_failed_list) {
        j_array.push_back(e1);
      }
      j["ranConfigurationStructuresFailedList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cell_ctrl_outcome_s& obj)
{
  in_j["cellGlobalId"].get_to(obj.cell_global_id);
  if (in_j.contains("ranConfigurationStructuresAcceptedList")) {
    {
      const auto& in_arr  = in_j.at("ranConfigurationStructuresAcceptedList");
      auto&       out_arr = obj.ran_cfg_structures_accepted_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<cfg_structure_accepted_s>();
      }
    }
  }
  if (in_j.contains("ranConfigurationStructuresFailedList")) {
    {
      const auto& in_arr  = in_j.at("ranConfigurationStructuresFailedList");
      auto&       out_arr = obj.ran_cfg_structures_failed_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<cfg_structure_failed_s>();
      }
    }
  }
}

// CellControlled ::= SEQUENCE
SRSASN_CODE cell_ctrl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_global_id.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cfg_structures, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_ctrl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_global_id.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cfg_structures, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void cell_ctrl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellGlobalId");
  cell_global_id.to_json(j);
  j.start_array("listOfConfigurationStructures");
  for (const auto& e1 : list_of_cfg_structures) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cell_ctrl_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["cellGlobalId"]         = obj.cell_global_id;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cfg_structures) {
      j_array.push_back(e1);
    }
    j["listOfConfigurationStructures"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cell_ctrl_s& obj)
{
  in_j["cellGlobalId"].get_to(obj.cell_global_id);
  const auto& in_arr  = in_j.at("listOfConfigurationStructures");
  auto&       out_arr = obj.list_of_cfg_structures;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<cfg_structure_write_s>();
  }
}

// CellLevelConfigurationStructureForEventTrigger ::= SEQUENCE
SRSASN_CODE cell_level_cfg_structure_for_event_trigger_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_global_id_present, 1));

  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_ran_cfg_structures_for_event_trigger, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_level_cfg_structure_for_event_trigger_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_global_id_present, 1));

  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(list_of_ran_cfg_structures_for_event_trigger, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void cell_level_cfg_structure_for_event_trigger_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_global_id_present) {
    j.write_fieldname("cellGlobalId");
    cell_global_id.to_json(j);
  }
  j.start_array("listOfRANConfigurationStructuresForEventTrigger");
  for (const auto& e1 : list_of_ran_cfg_structures_for_event_trigger) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cell_level_cfg_structure_for_event_trigger_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.cell_global_id_present) {
    j["cellGlobalId"] = obj.cell_global_id;
  }
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_ran_cfg_structures_for_event_trigger) {
      j_array.push_back(e1);
    }
    j["listOfRANConfigurationStructuresForEventTrigger"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cell_level_cfg_structure_for_event_trigger_s& obj)
{
  if (in_j.contains("cellGlobalId")) {
    in_j["cellGlobalId"].get_to(obj.cell_global_id);
    obj.cell_global_id_present = true;
  }
  {
    const auto& in_arr  = in_j.at("listOfRANConfigurationStructuresForEventTrigger");
    auto&       out_arr = obj.list_of_ran_cfg_structures_for_event_trigger;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<ran_cfg_structure_for_event_trigger_s>();
    }
  }
}

// CellReported ::= SEQUENCE
SRSASN_CODE cell_reported_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_global_id.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cfg_structures_reported, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_reported_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_global_id.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cfg_structures_reported, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void cell_reported_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellGlobalId");
  cell_global_id.to_json(j);
  j.start_array("listOfConfigurationStructuresReported");
  for (const auto& e1 : list_of_cfg_structures_reported) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cell_reported_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["cellGlobalId"]         = obj.cell_global_id;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cfg_structures_reported) {
      j_array.push_back(e1);
    }
    j["listOfConfigurationStructuresReported"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cell_reported_s& obj)
{
  in_j["cellGlobalId"].get_to(obj.cell_global_id);
  {
    const auto& in_arr  = in_j.at("listOfConfigurationStructuresReported");
    auto&       out_arr = obj.list_of_cfg_structures_reported;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<cfg_structure_s>();
    }
  }
}

// CellsToBeQueried ::= SEQUENCE
SRSASN_CODE cells_to_be_queried_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_global_id_present, 1));

  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_celllevel_cfg_structures_for_query, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_be_queried_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_global_id_present, 1));

  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(list_of_celllevel_cfg_structures_for_query, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void cells_to_be_queried_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_global_id_present) {
    j.write_fieldname("cellGlobalId");
    cell_global_id.to_json(j);
  }
  j.start_array("listOfCelllevelConfigurationStructuresForQuery");
  for (const auto& e1 : list_of_celllevel_cfg_structures_for_query) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cells_to_be_queried_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.cell_global_id_present) {
    j["cellGlobalId"] = obj.cell_global_id;
  }
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_celllevel_cfg_structures_for_query) {
      j_array.push_back(e1);
    }
    j["listOfCelllevelConfigurationStructuresForQuery"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cells_to_be_queried_s& obj)
{
  if (in_j.contains("cellGlobalId")) {
    in_j["cellGlobalId"].get_to(obj.cell_global_id);
    obj.cell_global_id_present = true;
  }
  {
    const auto& in_arr  = in_j.at("listOfCelllevelConfigurationStructuresForQuery");
    auto&       out_arr = obj.list_of_celllevel_cfg_structures_for_query;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<celllevel_cfg_structures_for_query_s>();
    }
  }
}

// CellsToBeReportedForQuery ::= SEQUENCE
SRSASN_CODE cells_to_be_reported_for_query_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_global_id.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cfg_structures_to_be_reported_for_query, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_be_reported_for_query_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_global_id.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cfg_structures_to_be_reported_for_query, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void cells_to_be_reported_for_query_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellGlobalId");
  cell_global_id.to_json(j);
  j.start_array("listOfConfigurationStructuresToBeReportedForQuery");
  for (const auto& e1 : list_of_cfg_structures_to_be_reported_for_query) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cells_to_be_reported_for_query_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["cellGlobalId"]         = obj.cell_global_id;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cfg_structures_to_be_reported_for_query) {
      j_array.push_back(e1);
    }
    j["listOfConfigurationStructuresToBeReportedForQuery"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cells_to_be_reported_for_query_s& obj)
{
  in_j["cellGlobalId"].get_to(obj.cell_global_id);
  {
    const auto& in_arr  = in_j.at("listOfConfigurationStructuresToBeReportedForQuery");
    auto&       out_arr = obj.list_of_cfg_structures_to_be_reported_for_query;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<cfg_structures_to_be_reported_for_query_s>();
    }
  }
}

// NodelevelRanConfigurationStructuresForQuery ::= SEQUENCE
SRSASN_CODE nodelevel_ran_cfg_structures_for_query_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_attributes.size() > 0, 1));

  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  if (list_of_attributes.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_attributes, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nodelevel_ran_cfg_structures_for_query_s::unpack(cbit_ref& bref)
{
  bool list_of_attributes_present;
  HANDLE_CODE(bref.unpack(list_of_attributes_present, 1));

  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  if (list_of_attributes_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_attributes, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void nodelevel_ran_cfg_structures_for_query_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  if (list_of_attributes.size() > 0) {
    j.start_array("listOfAttributes");
    for (const auto& e1 : list_of_attributes) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const nodelevel_ran_cfg_structures_for_query_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  if (obj.list_of_attributes.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_attributes) {
        j_array.push_back(e1);
      }
      j["listOfAttributes"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, nodelevel_ran_cfg_structures_for_query_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  if (in_j.contains("listOfAttributes")) {
    {
      const auto& in_arr  = in_j.at("listOfAttributes");
      auto&       out_arr = obj.list_of_attributes;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<attribute_name_s>();
      }
    }
  }
}

// RANConfigurationStructure ::= SEQUENCE
SRSASN_CODE ran_cfg_structure_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_supported_attributes.size() > 0, 1));

  HANDLE_CODE(ran_cfg_structure_name.pack(bref));
  if (list_of_supported_attributes.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_attributes, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_cfg_structure_s::unpack(cbit_ref& bref)
{
  bool list_of_supported_attributes_present;
  HANDLE_CODE(bref.unpack(list_of_supported_attributes_present, 1));

  HANDLE_CODE(ran_cfg_structure_name.unpack(bref));
  if (list_of_supported_attributes_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_attributes, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void ran_cfg_structure_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranConfigurationStructureName", ran_cfg_structure_name.to_string());
  if (list_of_supported_attributes.size() > 0) {
    j.start_array("listOfSupportedAttributes");
    for (const auto& e1 : list_of_supported_attributes) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ran_cfg_structure_s& obj)
{
  nlohmann::ordered_json& j          = out_j;
  j["ranConfigurationStructureName"] = obj.ran_cfg_structure_name.to_string();
  if (obj.list_of_supported_attributes.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_supported_attributes) {
        j_array.push_back(e1);
      }
      j["listOfSupportedAttributes"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ran_cfg_structure_s& obj)
{
  obj.ran_cfg_structure_name.from_string(in_j["ranConfigurationStructureName"].get<std::string>());
  if (in_j.contains("listOfSupportedAttributes")) {
    {
      const auto& in_arr  = in_j.at("listOfSupportedAttributes");
      auto&       out_arr = obj.list_of_supported_attributes;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<attribute_s>();
      }
    }
  }
}

// CellForRANFunctionDefinition ::= SEQUENCE
SRSASN_CODE cell_for_ran_function_definition_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_supported_cell_level_ran_cfg_structures.size() > 0, 1));

  HANDLE_CODE(cell_global_id.pack(bref));
  if (list_of_supported_cell_level_ran_cfg_structures.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_cell_level_ran_cfg_structures, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_for_ran_function_definition_s::unpack(cbit_ref& bref)
{
  bool list_of_supported_cell_level_ran_cfg_structures_present;
  HANDLE_CODE(bref.unpack(list_of_supported_cell_level_ran_cfg_structures_present, 1));

  HANDLE_CODE(cell_global_id.unpack(bref));
  if (list_of_supported_cell_level_ran_cfg_structures_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_cell_level_ran_cfg_structures, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void cell_for_ran_function_definition_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellGlobalID");
  cell_global_id.to_json(j);
  if (list_of_supported_cell_level_ran_cfg_structures.size() > 0) {
    j.start_array("listOfSupportedCellLevelRANConfigurationStructures");
    for (const auto& e1 : list_of_supported_cell_level_ran_cfg_structures) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const cell_for_ran_function_definition_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["cellGlobalID"]         = obj.cell_global_id;
  if (obj.list_of_supported_cell_level_ran_cfg_structures.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_supported_cell_level_ran_cfg_structures) {
        j_array.push_back(e1);
      }
      j["listOfSupportedCellLevelRANConfigurationStructures"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, cell_for_ran_function_definition_s& obj)
{
  in_j["cellGlobalID"].get_to(obj.cell_global_id);
  if (in_j.contains("listOfSupportedCellLevelRANConfigurationStructures")) {
    {
      const auto& in_arr  = in_j.at("listOfSupportedCellLevelRANConfigurationStructures");
      auto&       out_arr = obj.list_of_supported_cell_level_ran_cfg_structures;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<ran_cfg_structure_s>();
      }
    }
  }
}

// E2SM-CCC-ActionDefinitionFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_action_definition_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_node_level_ran_cfg_structures_for_adf, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_action_definition_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_node_level_ran_cfg_structures_for_adf, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_action_definition_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfNodeLevelRANConfigurationStructuresForADF");
  for (const auto& e1 : list_of_node_level_ran_cfg_structures_for_adf) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_action_definition_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_node_level_ran_cfg_structures_for_adf) {
      j_array.push_back(e1);
    }
    j["listOfNodeLevelRANConfigurationStructuresForADF"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_action_definition_format1_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfNodeLevelRANConfigurationStructuresForADF");
    auto&       out_arr = obj.list_of_node_level_ran_cfg_structures_for_adf;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<ran_cfg_structure_for_adf_s>();
    }
  }
}

// E2SM-CCC-ActionDefinitionFormat2 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_action_definition_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_cell_cfgs_to_be_reported_for_adf.size() > 0, 1));

  if (list_of_cell_cfgs_to_be_reported_for_adf.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cell_cfgs_to_be_reported_for_adf, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_action_definition_format2_s::unpack(cbit_ref& bref)
{
  bool list_of_cell_cfgs_to_be_reported_for_adf_present;
  HANDLE_CODE(bref.unpack(list_of_cell_cfgs_to_be_reported_for_adf_present, 1));

  if (list_of_cell_cfgs_to_be_reported_for_adf_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_cell_cfgs_to_be_reported_for_adf, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_action_definition_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (list_of_cell_cfgs_to_be_reported_for_adf.size() > 0) {
    j.start_array("listOfCellConfigurationsToBeReportedForADF");
    for (const auto& e1 : list_of_cell_cfgs_to_be_reported_for_adf) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_action_definition_format2_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.list_of_cell_cfgs_to_be_reported_for_adf.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_cell_cfgs_to_be_reported_for_adf) {
        j_array.push_back(e1);
      }
      j["listOfCellConfigurationsToBeReportedForADF"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_action_definition_format2_s& obj)
{
  if (in_j.contains("listOfCellConfigurationsToBeReportedForADF")) {
    {
      const auto& in_arr  = in_j.at("listOfCellConfigurationsToBeReportedForADF");
      auto&       out_arr = obj.list_of_cell_cfgs_to_be_reported_for_adf;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<cell_cfg_to_be_reported_for_adf_s>();
      }
    }
  }
}

// E2SM-CCC-ControlHeaderFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ctrl_hdr_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_style_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ctrl_hdr_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(ric_style_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ctrl_hdr_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricStyleType", ric_style_type);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_hdr_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["ricStyleType"]         = obj.ric_style_type;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_hdr_format1_s& obj)
{
  in_j["ricStyleType"].get_to(obj.ric_style_type);
}

// E2SM-CCC-ControlMessageFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ctrl_msg_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cfg_structures, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ctrl_msg_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cfg_structures, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ctrl_msg_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfConfigurationStructures");
  for (const auto& e1 : list_of_cfg_structures) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_msg_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cfg_structures) {
      j_array.push_back(e1);
    }
    j["listOfConfigurationStructures"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_msg_format1_s& obj)
{
  const auto& in_arr  = in_j.at("listOfConfigurationStructures");
  auto&       out_arr = obj.list_of_cfg_structures;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<cfg_structure_write_s>();
  }
}

// E2SM-CCC-ControlMessageFormat2 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ctrl_msg_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cells_ctrl, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ctrl_msg_format2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cells_ctrl, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ctrl_msg_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfCellsControlled");
  for (const auto& e1 : list_of_cells_ctrl) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_msg_format2_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cells_ctrl) {
      j_array.push_back(e1);
    }
    j["listOfCellsControlled"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_msg_format2_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfCellsControlled");
    auto&       out_arr = obj.list_of_cells_ctrl;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<cell_ctrl_s>();
    }
  }
}

// E2SM-CCC-ControlOutcomeFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ctrl_outcome_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rx_timestamp_present, 1));
  HANDLE_CODE(bref.pack(ran_cfg_structures_accepted_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ran_cfg_structures_failed_list.size() > 0, 1));

  if (rx_timestamp_present) {
    HANDLE_CODE(rx_timestamp.pack(bref));
  }
  if (ran_cfg_structures_accepted_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_cfg_structures_accepted_list, 1, 64, true));
  }
  if (ran_cfg_structures_failed_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_cfg_structures_failed_list, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ctrl_outcome_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rx_timestamp_present, 1));
  bool ran_cfg_structures_accepted_list_present;
  HANDLE_CODE(bref.unpack(ran_cfg_structures_accepted_list_present, 1));
  bool ran_cfg_structures_failed_list_present;
  HANDLE_CODE(bref.unpack(ran_cfg_structures_failed_list_present, 1));

  if (rx_timestamp_present) {
    HANDLE_CODE(rx_timestamp.unpack(bref));
  }
  if (ran_cfg_structures_accepted_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ran_cfg_structures_accepted_list, bref, 1, 64, true));
  }
  if (ran_cfg_structures_failed_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ran_cfg_structures_failed_list, bref, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ctrl_outcome_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rx_timestamp_present) {
    j.write_str("receivedTimestamp", rx_timestamp.to_string());
  }
  if (ran_cfg_structures_accepted_list.size() > 0) {
    j.start_array("ranConfigurationStructuresAcceptedList");
    for (const auto& e1 : ran_cfg_structures_accepted_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_cfg_structures_failed_list.size() > 0) {
    j.start_array("ranConfigurationStructuresFailedList");
    for (const auto& e1 : ran_cfg_structures_failed_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_outcome_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.rx_timestamp_present) {
    j["receivedTimestamp"] = obj.rx_timestamp.to_string();
  }
  if (obj.ran_cfg_structures_accepted_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.ran_cfg_structures_accepted_list) {
        j_array.push_back(e1);
      }
      j["ranConfigurationStructuresAcceptedList"] = j_array;
    }
  }
  if (obj.ran_cfg_structures_failed_list.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.ran_cfg_structures_failed_list) {
        j_array.push_back(e1);
      }
      j["ranConfigurationStructuresFailedList"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_outcome_format1_s& obj)
{
  if (in_j.contains("receivedTimestamp")) {
    obj.rx_timestamp.from_string(in_j["receivedTimestamp"].get<std::string>());
    obj.rx_timestamp_present = true;
  }
  if (in_j.contains("ranConfigurationStructuresAcceptedList")) {
    {
      const auto& in_arr  = in_j.at("ranConfigurationStructuresAcceptedList");
      auto&       out_arr = obj.ran_cfg_structures_accepted_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<cfg_structure_accepted_s>();
      }
    }
  }
  if (in_j.contains("ranConfigurationStructuresFailedList")) {
    {
      const auto& in_arr  = in_j.at("ranConfigurationStructuresFailedList");
      auto&       out_arr = obj.ran_cfg_structures_failed_list;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<cfg_structure_failed_s>();
      }
    }
  }
}

// E2SM-CCC-ControlOutcomeFormat2 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ctrl_outcome_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rx_timestamp_present, 1));

  if (rx_timestamp_present) {
    HANDLE_CODE(rx_timestamp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cells_for_ctrl_outcome, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ctrl_outcome_format2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rx_timestamp_present, 1));

  if (rx_timestamp_present) {
    HANDLE_CODE(rx_timestamp.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cells_for_ctrl_outcome, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ctrl_outcome_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rx_timestamp_present) {
    j.write_str("receivedTimestamp", rx_timestamp.to_string());
  }
  j.start_array("listOfCellsForControlOutcome");
  for (const auto& e1 : list_of_cells_for_ctrl_outcome) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_outcome_format2_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  if (obj.rx_timestamp_present) {
    j["receivedTimestamp"] = obj.rx_timestamp.to_string();
  }
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cells_for_ctrl_outcome) {
      j_array.push_back(e1);
    }
    j["listOfCellsForControlOutcome"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_outcome_format2_s& obj)
{
  if (in_j.contains("receivedTimestamp")) {
    obj.rx_timestamp.from_string(in_j["receivedTimestamp"].get<std::string>());
    obj.rx_timestamp_present = true;
  }
  {
    const auto& in_arr  = in_j.at("listOfCellsForControlOutcome");
    auto&       out_arr = obj.list_of_cells_for_ctrl_outcome;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<cell_ctrl_outcome_s>();
    }
  }
}

// E2SM-CCC-EventTriggerDefinition-Format1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_event_trigger_definition_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_node_level_cfg_structures_for_event_trigger, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_event_trigger_definition_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_node_level_cfg_structures_for_event_trigger, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_event_trigger_definition_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfNodeLevelConfigurationStructuresForEventTrigger");
  for (const auto& e1 : list_of_node_level_cfg_structures_for_event_trigger) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_event_trigger_definition_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_node_level_cfg_structures_for_event_trigger) {
      j_array.push_back(e1);
    }
    j["listOfNodeLevelConfigurationStructuresForEventTrigger"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_event_trigger_definition_format1_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfNodeLevelConfigurationStructuresForEventTrigger");
    auto&       out_arr = obj.list_of_node_level_cfg_structures_for_event_trigger;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<ran_cfg_structure_for_event_trigger_s>();
    }
  }
}

// E2SM-CCC-EventTriggerDefinition-Format2 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_event_trigger_definition_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cell_level_cfg_structures_for_event_trigger, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_event_trigger_definition_format2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cell_level_cfg_structures_for_event_trigger, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_event_trigger_definition_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfCellLevelConfigurationStructuresForEventTrigger");
  for (const auto& e1 : list_of_cell_level_cfg_structures_for_event_trigger) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_event_trigger_definition_format2_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cell_level_cfg_structures_for_event_trigger) {
      j_array.push_back(e1);
    }
    j["listOfCellLevelConfigurationStructuresForEventTrigger"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_event_trigger_definition_format2_s& obj)
{
  {
    const auto& in_arr  = in_j.at("listOfCellLevelConfigurationStructuresForEventTrigger");
    auto&       out_arr = obj.list_of_cell_level_cfg_structures_for_event_trigger;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<cell_level_cfg_structure_for_event_trigger_s>();
    }
  }
}

// E2SM-CCC-EventTriggerDefinition-Format3 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_event_trigger_definition_format3_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, period, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_event_trigger_definition_format3_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(period, bref, false, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_event_trigger_definition_format3_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("period", period);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_event_trigger_definition_format3_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["period"]               = obj.period;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_event_trigger_definition_format3_s& obj)
{
  in_j["period"].get_to(obj.period);
}

// E2SM-CCC-IndicationHeaderFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ind_hdr_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ind_reason.pack(bref));
  HANDLE_CODE(event_time.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ind_hdr_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ind_reason.unpack(bref));
  HANDLE_CODE(event_time.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ind_hdr_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("indicationReason", ind_reason.to_string());
  j.write_str("eventTime", event_time.to_string());
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ind_hdr_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["indicationReason"]     = obj.ind_reason.to_string();
  j["eventTime"]            = obj.event_time.to_string();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ind_hdr_format1_s& obj)
{
  obj.ind_reason.from_string(in_j["indicationReason"].get<std::string>());
  obj.event_time.from_string(in_j["eventTime"].get<std::string>());
}

const char* e2_sm_ccc_ind_hdr_format1_s::ind_reason_opts::to_string() const
{
  static const char* names[] = {"uponSubscription", "uponChange", "periodic"};
  return convert_enum_idx(names, 3, value, "e2_sm_ccc_ind_hdr_format1_s::ind_reason_e_");
}
bool e2_sm_ccc_ind_hdr_format1_s::ind_reason_opts::from_string(const std::string& str)
{
  static const char* names[] = {"uponSubscription", "uponChange", "periodic"};
  uint32_t           idx;
  if (convert_enum_str(names, 3, str.c_str(), idx, "e2_sm_ccc_ind_hdr_format1_s::ind_reason_e_")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}

// E2SM-CCC-IndicationMessageFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ind_msg_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cfg_structures_reported, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ind_msg_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cfg_structures_reported, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ind_msg_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfConfigurationStructuresReported");
  for (const auto& e1 : list_of_cfg_structures_reported) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ind_msg_format1_s& obj)
{
  nlohmann::ordered_json& j       = out_j;
  nlohmann::ordered_json  j_array = nlohmann::ordered_json::array();
  for (const auto& e1 : obj.list_of_cfg_structures_reported) {
    j_array.push_back(e1);
  }
  j["listOfConfigurationStructuresReported"] = j_array;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ind_msg_format1_s& obj)
{
  const auto& in_arr  = in_j.at("listOfConfigurationStructuresReported");
  auto&       out_arr = obj.list_of_cfg_structures_reported;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<cfg_structure_s>();
  }
}

// E2SM-CCC-IndicationMessageFormat2 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_ind_msg_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cells_reported, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_ind_msg_format2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cells_reported, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_ind_msg_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfCellsReported");
  for (const auto& e1 : list_of_cells_reported) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ind_msg_format2_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cells_reported) {
      j_array.push_back(e1);
    }
    j["listOfCellsReported"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ind_msg_format2_s& obj)
{
  const auto& in_arr  = in_j.at("listOfCellsReported");
  auto&       out_arr = obj.list_of_cells_reported;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<cell_reported_s>();
  }
}

// E2SM-CCC-QueryDefinitionFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_query_definition_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_nodelevel_ran_cfg_structures_for_query, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_query_definition_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_nodelevel_ran_cfg_structures_for_query, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_query_definition_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfNodelevelRanConfigurationStructuresForQuery");
  for (const auto& e1 : list_of_nodelevel_ran_cfg_structures_for_query) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_definition_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_nodelevel_ran_cfg_structures_for_query) {
      j_array.push_back(e1);
    }
    j["listOfNodelevelRanConfigurationStructuresForQuery"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_definition_format1_s& obj)
{
  const auto& in_arr  = in_j.at("listOfNodelevelRanConfigurationStructuresForQuery");
  auto&       out_arr = obj.list_of_nodelevel_ran_cfg_structures_for_query;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<nodelevel_ran_cfg_structures_for_query_s>();
  }
}

// E2SM-CCC-QueryDefinitionFormat2 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_query_definition_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cells_to_be_queried_for_query, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_query_definition_format2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cells_to_be_queried_for_query, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_query_definition_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfCellsToBeQueriedForQuery");
  for (const auto& e1 : list_of_cells_to_be_queried_for_query) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_definition_format2_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cells_to_be_queried_for_query) {
      j_array.push_back(e1);
    }
    j["listOfCellsToBeQueriedForQuery"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_definition_format2_s& obj)
{
  const auto& in_arr  = in_j.at("listOfCellsToBeQueriedForQuery");
  auto&       out_arr = obj.list_of_cells_to_be_queried_for_query;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<cells_to_be_queried_s>();
  }
}

// E2SM-CCC-QueryHeaderFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_query_hdr_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_style_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_query_hdr_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(ric_style_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_query_hdr_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricStyleType", ric_style_type);
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_hdr_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  j["ricStyleType"]         = obj.ric_style_type;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_hdr_format1_s& obj)
{
  in_j["ricStyleType"].get_to(obj.ric_style_type);
}

// E2SM-CCC-QueryOutcomeFormat1 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_query_outcome_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cfg_structures_to_be_reported_for_query, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_query_outcome_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cfg_structures_to_be_reported_for_query, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_query_outcome_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfConfigurationStructuresToBeReportedForQuery");
  for (const auto& e1 : list_of_cfg_structures_to_be_reported_for_query) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_outcome_format1_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cfg_structures_to_be_reported_for_query) {
      j_array.push_back(e1);
    }
    j["listOfConfigurationStructuresToBeReportedForQuery"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_outcome_format1_s& obj)
{
  const auto& in_arr  = in_j.at("listOfConfigurationStructuresToBeReportedForQuery");
  auto&       out_arr = obj.list_of_cfg_structures_to_be_reported_for_query;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<cfg_structures_to_be_reported_for_query_s>();
  }
}

// E2SM-CCC-QueryOutcomeFormat2 ::= SEQUENCE
SRSASN_CODE e2_sm_ccc_query_outcome_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cells_to_be_reported_for_query, 1, 64, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_ccc_query_outcome_format2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(list_of_cells_to_be_reported_for_query, bref, 1, 64, true));

  return SRSASN_SUCCESS;
}
void e2_sm_ccc_query_outcome_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("listOfCellsToBeReportedForQuery");
  for (const auto& e1 : list_of_cells_to_be_reported_for_query) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_outcome_format2_s& obj)
{
  nlohmann::ordered_json& j = out_j;
  {
    nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
    for (const auto& e1 : obj.list_of_cells_to_be_reported_for_query) {
      j_array.push_back(e1);
    }
    j["listOfCellsToBeReportedForQuery"] = j_array;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_outcome_format2_s& obj)
{
  const auto& in_arr  = in_j.at("listOfCellsToBeReportedForQuery");
  auto&       out_arr = obj.list_of_cells_to_be_reported_for_query;
  out_arr.resize(in_arr.size());
  for (size_t i = 0; i < in_arr.size(); ++i) {
    out_arr[i] = in_arr[i].get<cells_to_be_reported_for_query_s>();
  }
}

// ActionDefinitionFormat ::= CHOICE
void action_definition_format_c::destroy_()
{
  switch (type_) {
    case types::action_definition_format1:
      c.destroy<e2_sm_ccc_action_definition_format1_s>();
      break;
    case types::action_definition_format2:
      c.destroy<e2_sm_ccc_action_definition_format2_s>();
      break;
    default:
      break;
  }
}
void action_definition_format_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::action_definition_format1:
      c.init<e2_sm_ccc_action_definition_format1_s>();
      break;
    case types::action_definition_format2:
      c.init<e2_sm_ccc_action_definition_format2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "action_definition_format_c");
  }
}
action_definition_format_c::action_definition_format_c(const action_definition_format_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::action_definition_format1:
      c.init(other.c.get<e2_sm_ccc_action_definition_format1_s>());
      break;
    case types::action_definition_format2:
      c.init(other.c.get<e2_sm_ccc_action_definition_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "action_definition_format_c");
  }
}
action_definition_format_c& action_definition_format_c::operator=(const action_definition_format_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::action_definition_format1:
      c.set(other.c.get<e2_sm_ccc_action_definition_format1_s>());
      break;
    case types::action_definition_format2:
      c.set(other.c.get<e2_sm_ccc_action_definition_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "action_definition_format_c");
  }

  return *this;
}
e2_sm_ccc_action_definition_format1_s& action_definition_format_c::set_action_definition_format1()
{
  set(types::action_definition_format1);
  return c.get<e2_sm_ccc_action_definition_format1_s>();
}
e2_sm_ccc_action_definition_format2_s& action_definition_format_c::set_action_definition_format2()
{
  set(types::action_definition_format2);
  return c.get<e2_sm_ccc_action_definition_format2_s>();
}
void action_definition_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::action_definition_format1:
      j.write_fieldname("actionDefinition-Format1");
      c.get<e2_sm_ccc_action_definition_format1_s>().to_json(j);
      break;
    case types::action_definition_format2:
      j.write_fieldname("actionDefinition-Format2");
      c.get<e2_sm_ccc_action_definition_format2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "action_definition_format_c");
  }
  j.end_obj();
}
SRSASN_CODE action_definition_format_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::action_definition_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_action_definition_format1_s>().pack(bref));
      break;
    case types::action_definition_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_action_definition_format2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "action_definition_format_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE action_definition_format_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::action_definition_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_action_definition_format1_s>().unpack(bref));
      break;
    case types::action_definition_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_action_definition_format2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "action_definition_format_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const action_definition_format_c& obj)
{
  switch (obj.type()) {
    case action_definition_format_c::types::action_definition_format1:
      out_j = obj.action_definition_format1();
      break;
    case action_definition_format_c::types::action_definition_format2:
      out_j = obj.action_definition_format2();
      break;
    default:
      log_invalid_choice_id(obj.type(), "action_definition_format_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, action_definition_format_c& obj)
{
  if (in_j.find("listOfNodeLevelRANConfigurationStructuresForADF") != in_j.end()) {
    auto& format = obj.set_action_definition_format1();
    in_j.get_to(format);
  } else if (in_j.find("listOfCellConfigurationsToBeReportedForADF") != in_j.end()) {
    auto& format = obj.set_action_definition_format2();
    in_j.get_to(format);
  }
}

const char* action_definition_format_c::types_opts::to_string() const
{
  static const char* names[] = {"actionDefinition-Format1", "actionDefinition-Format2"};
  return convert_enum_idx(names, 2, value, "action_definition_format_c::types");
}
bool action_definition_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"actionDefinition-Format1", "actionDefinition-Format2"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "action_definition_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t action_definition_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "action_definition_format_c::types");
}

// ControlHeaderFormat ::= CHOICE
void ctrl_hdr_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("controlHeader-Format1");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ctrl_hdr_format_c::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ctrl_hdr_format_c::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ctrl_hdr_format_c& obj)
{
  out_j = obj.ctrl_hdr_format1();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ctrl_hdr_format_c& obj)
{
  auto& format = obj.ctrl_hdr_format1();
  in_j.get_to(format);
}

const char* ctrl_hdr_format_c::types_opts::to_string() const
{
  static const char* names[] = {"controlHeader-Format1"};
  return convert_enum_idx(names, 1, value, "ctrl_hdr_format_c::types");
}
bool ctrl_hdr_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"controlHeader-Format1"};
  uint32_t           idx;
  if (convert_enum_str(names, 1, str.c_str(), idx, "ctrl_hdr_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t ctrl_hdr_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ctrl_hdr_format_c::types");
}

// ControlMessageFormat ::= CHOICE
void ctrl_msg_format_c::destroy_()
{
  switch (type_) {
    case types::ctrl_msg_format1:
      c.destroy<e2_sm_ccc_ctrl_msg_format1_s>();
      break;
    case types::ctrl_msg_format2:
      c.destroy<e2_sm_ccc_ctrl_msg_format2_s>();
      break;
    default:
      break;
  }
}
void ctrl_msg_format_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ctrl_msg_format1:
      c.init<e2_sm_ccc_ctrl_msg_format1_s>();
      break;
    case types::ctrl_msg_format2:
      c.init<e2_sm_ccc_ctrl_msg_format2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_msg_format_c");
  }
}
ctrl_msg_format_c::ctrl_msg_format_c(const ctrl_msg_format_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ctrl_msg_format1:
      c.init(other.c.get<e2_sm_ccc_ctrl_msg_format1_s>());
      break;
    case types::ctrl_msg_format2:
      c.init(other.c.get<e2_sm_ccc_ctrl_msg_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_msg_format_c");
  }
}
ctrl_msg_format_c& ctrl_msg_format_c::operator=(const ctrl_msg_format_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ctrl_msg_format1:
      c.set(other.c.get<e2_sm_ccc_ctrl_msg_format1_s>());
      break;
    case types::ctrl_msg_format2:
      c.set(other.c.get<e2_sm_ccc_ctrl_msg_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_msg_format_c");
  }

  return *this;
}
e2_sm_ccc_ctrl_msg_format1_s& ctrl_msg_format_c::set_ctrl_msg_format1()
{
  set(types::ctrl_msg_format1);
  return c.get<e2_sm_ccc_ctrl_msg_format1_s>();
}
e2_sm_ccc_ctrl_msg_format2_s& ctrl_msg_format_c::set_ctrl_msg_format2()
{
  set(types::ctrl_msg_format2);
  return c.get<e2_sm_ccc_ctrl_msg_format2_s>();
}
void ctrl_msg_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ctrl_msg_format1:
      j.write_fieldname("controlMessage-Format1");
      c.get<e2_sm_ccc_ctrl_msg_format1_s>().to_json(j);
      break;
    case types::ctrl_msg_format2:
      j.write_fieldname("controlMessage-Format2");
      c.get<e2_sm_ccc_ctrl_msg_format2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_msg_format_c");
  }
  j.end_obj();
}
SRSASN_CODE ctrl_msg_format_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ctrl_msg_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_msg_format1_s>().pack(bref));
      break;
    case types::ctrl_msg_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_msg_format2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_msg_format_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ctrl_msg_format_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ctrl_msg_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_msg_format1_s>().unpack(bref));
      break;
    case types::ctrl_msg_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_msg_format2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_msg_format_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ctrl_msg_format_c& obj)
{
  switch (obj.type()) {
    case ctrl_msg_format_c::types::ctrl_msg_format1:
      out_j = obj.ctrl_msg_format1();
      break;
    case ctrl_msg_format_c::types::ctrl_msg_format2:
      out_j = obj.ctrl_msg_format2();
      break;
    default:
      log_invalid_choice_id(obj.type(), "ctrl_msg_format_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ctrl_msg_format_c& obj)
{
  if (in_j.contains("listOfConfigurationStructures")) {
    obj.set_ctrl_msg_format1();
    const auto& in_arr  = in_j.at("listOfConfigurationStructures");
    auto&       out_arr = obj.ctrl_msg_format1().list_of_cfg_structures;
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<cfg_structure_write_s>();
    }
  }
  if (in_j.contains("listOfCellsControlled")) {
    obj.set_ctrl_msg_format2();
    const auto& in_arr  = in_j.at("listOfCellsControlled");
    auto&       out_arr = obj.ctrl_msg_format2().list_of_cells_ctrl;
    // std::vector<cell_ctrl_s> out_arr;
    // for (const auto& item : in_arr) {
    //   out_arr.push_back(item.get<cell_ctrl_s>());
    // }
    out_arr.resize(in_arr.size());
    for (size_t i = 0; i < in_arr.size(); ++i) {
      out_arr[i] = in_arr[i].get<cell_ctrl_s>();
    }
  }
}

const char* ctrl_msg_format_c::types_opts::to_string() const
{
  static const char* names[] = {"controlMessage-Format1", "controlMessage-Format2"};
  return convert_enum_idx(names, 2, value, "ctrl_msg_format_c::types");
}
bool ctrl_msg_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"controlMessage-Format1", "controlMessage-Format2"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "ctrl_msg_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t ctrl_msg_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ctrl_msg_format_c::types");
}

// ControlOutcomeFormat ::= CHOICE
void ctrl_outcome_format_c::destroy_()
{
  switch (type_) {
    case types::ctrl_outcome_format1:
      c.destroy<e2_sm_ccc_ctrl_outcome_format1_s>();
      break;
    case types::ctrl_outcome_format2:
      c.destroy<e2_sm_ccc_ctrl_outcome_format2_s>();
      break;
    default:
      break;
  }
}
void ctrl_outcome_format_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ctrl_outcome_format1:
      c.init<e2_sm_ccc_ctrl_outcome_format1_s>();
      break;
    case types::ctrl_outcome_format2:
      c.init<e2_sm_ccc_ctrl_outcome_format2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_outcome_format_c");
  }
}
ctrl_outcome_format_c::ctrl_outcome_format_c(const ctrl_outcome_format_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ctrl_outcome_format1:
      c.init(other.c.get<e2_sm_ccc_ctrl_outcome_format1_s>());
      break;
    case types::ctrl_outcome_format2:
      c.init(other.c.get<e2_sm_ccc_ctrl_outcome_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_outcome_format_c");
  }
}
ctrl_outcome_format_c& ctrl_outcome_format_c::operator=(const ctrl_outcome_format_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ctrl_outcome_format1:
      c.set(other.c.get<e2_sm_ccc_ctrl_outcome_format1_s>());
      break;
    case types::ctrl_outcome_format2:
      c.set(other.c.get<e2_sm_ccc_ctrl_outcome_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_outcome_format_c");
  }

  return *this;
}
e2_sm_ccc_ctrl_outcome_format1_s& ctrl_outcome_format_c::set_ctrl_outcome_format1()
{
  set(types::ctrl_outcome_format1);
  return c.get<e2_sm_ccc_ctrl_outcome_format1_s>();
}
e2_sm_ccc_ctrl_outcome_format2_s& ctrl_outcome_format_c::set_ctrl_outcome_format2()
{
  set(types::ctrl_outcome_format2);
  return c.get<e2_sm_ccc_ctrl_outcome_format2_s>();
}
void ctrl_outcome_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ctrl_outcome_format1:
      j.write_fieldname("controlOutcome-Format1");
      c.get<e2_sm_ccc_ctrl_outcome_format1_s>().to_json(j);
      break;
    case types::ctrl_outcome_format2:
      j.write_fieldname("controlOutcome-Format2");
      c.get<e2_sm_ccc_ctrl_outcome_format2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_outcome_format_c");
  }
  j.end_obj();
}
SRSASN_CODE ctrl_outcome_format_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ctrl_outcome_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_outcome_format1_s>().pack(bref));
      break;
    case types::ctrl_outcome_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_outcome_format2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_outcome_format_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ctrl_outcome_format_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ctrl_outcome_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_outcome_format1_s>().unpack(bref));
      break;
    case types::ctrl_outcome_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_ctrl_outcome_format2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ctrl_outcome_format_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ctrl_outcome_format_c& obj)
{
  switch (obj.type()) {
    case ctrl_outcome_format_c::types::ctrl_outcome_format1:
      out_j = obj.ctrl_outcome_format1();
      break;
    case ctrl_outcome_format_c::types::ctrl_outcome_format2:
      out_j = obj.ctrl_outcome_format2();
      break;
    default:
      log_invalid_choice_id(obj.type(), "ctrl_outcome_format_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ctrl_outcome_format_c& obj)
{
  if (in_j.find("ranConfigurationStructuresAcceptedList") != in_j.end()) {
    auto& format = obj.set_ctrl_outcome_format1();
    in_j.get_to(format);
  } else if (in_j.find("listOfCellsForControlOutcome") != in_j.end()) {
    auto& format = obj.set_ctrl_outcome_format2();
    in_j.get_to(format);
  }
}

const char* ctrl_outcome_format_c::types_opts::to_string() const
{
  static const char* names[] = {"controlOutcome-Format1", "controlOutcome-Format2"};
  return convert_enum_idx(names, 2, value, "ctrl_outcome_format_c::types");
}
bool ctrl_outcome_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"controlOutcome-Format1", "controlOutcome-Format2"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "ctrl_outcome_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t ctrl_outcome_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ctrl_outcome_format_c::types");
}

// EventTriggerDefinitionFormat ::= CHOICE
void event_trigger_definition_format_c::destroy_()
{
  switch (type_) {
    case types::event_trigger_definition_format1:
      c.destroy<e2_sm_ccc_event_trigger_definition_format1_s>();
      break;
    case types::event_trigger_definition_format2:
      c.destroy<e2_sm_ccc_event_trigger_definition_format2_s>();
      break;
    case types::event_trigger_definition_format3:
      c.destroy<e2_sm_ccc_event_trigger_definition_format3_s>();
      break;
    default:
      break;
  }
}
void event_trigger_definition_format_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event_trigger_definition_format1:
      c.init<e2_sm_ccc_event_trigger_definition_format1_s>();
      break;
    case types::event_trigger_definition_format2:
      c.init<e2_sm_ccc_event_trigger_definition_format2_s>();
      break;
    case types::event_trigger_definition_format3:
      c.init<e2_sm_ccc_event_trigger_definition_format3_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_definition_format_c");
  }
}
event_trigger_definition_format_c::event_trigger_definition_format_c(const event_trigger_definition_format_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event_trigger_definition_format1:
      c.init(other.c.get<e2_sm_ccc_event_trigger_definition_format1_s>());
      break;
    case types::event_trigger_definition_format2:
      c.init(other.c.get<e2_sm_ccc_event_trigger_definition_format2_s>());
      break;
    case types::event_trigger_definition_format3:
      c.init(other.c.get<e2_sm_ccc_event_trigger_definition_format3_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_definition_format_c");
  }
}
event_trigger_definition_format_c&
event_trigger_definition_format_c::operator=(const event_trigger_definition_format_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event_trigger_definition_format1:
      c.set(other.c.get<e2_sm_ccc_event_trigger_definition_format1_s>());
      break;
    case types::event_trigger_definition_format2:
      c.set(other.c.get<e2_sm_ccc_event_trigger_definition_format2_s>());
      break;
    case types::event_trigger_definition_format3:
      c.set(other.c.get<e2_sm_ccc_event_trigger_definition_format3_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_definition_format_c");
  }

  return *this;
}
e2_sm_ccc_event_trigger_definition_format1_s& event_trigger_definition_format_c::set_event_trigger_definition_format1()
{
  set(types::event_trigger_definition_format1);
  return c.get<e2_sm_ccc_event_trigger_definition_format1_s>();
}
e2_sm_ccc_event_trigger_definition_format2_s& event_trigger_definition_format_c::set_event_trigger_definition_format2()
{
  set(types::event_trigger_definition_format2);
  return c.get<e2_sm_ccc_event_trigger_definition_format2_s>();
}
e2_sm_ccc_event_trigger_definition_format3_s& event_trigger_definition_format_c::set_event_trigger_definition_format3()
{
  set(types::event_trigger_definition_format3);
  return c.get<e2_sm_ccc_event_trigger_definition_format3_s>();
}
void event_trigger_definition_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event_trigger_definition_format1:
      j.write_fieldname("eventTriggerDefinition-Format1");
      c.get<e2_sm_ccc_event_trigger_definition_format1_s>().to_json(j);
      break;
    case types::event_trigger_definition_format2:
      j.write_fieldname("eventTriggerDefinition-Format2");
      c.get<e2_sm_ccc_event_trigger_definition_format2_s>().to_json(j);
      break;
    case types::event_trigger_definition_format3:
      j.write_fieldname("eventTriggerDefinition-Format3");
      c.get<e2_sm_ccc_event_trigger_definition_format3_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_definition_format_c");
  }
  j.end_obj();
}
SRSASN_CODE event_trigger_definition_format_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event_trigger_definition_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_event_trigger_definition_format1_s>().pack(bref));
      break;
    case types::event_trigger_definition_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_event_trigger_definition_format2_s>().pack(bref));
      break;
    case types::event_trigger_definition_format3:
      HANDLE_CODE(c.get<e2_sm_ccc_event_trigger_definition_format3_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_definition_format_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_definition_format_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event_trigger_definition_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_event_trigger_definition_format1_s>().unpack(bref));
      break;
    case types::event_trigger_definition_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_event_trigger_definition_format2_s>().unpack(bref));
      break;
    case types::event_trigger_definition_format3:
      HANDLE_CODE(c.get<e2_sm_ccc_event_trigger_definition_format3_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_definition_format_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const event_trigger_definition_format_c& obj)
{
  switch (obj.type()) {
    case event_trigger_definition_format_c::types::event_trigger_definition_format1:
      out_j = obj.event_trigger_definition_format1();
      break;
    case event_trigger_definition_format_c::types::event_trigger_definition_format2:
      out_j = obj.event_trigger_definition_format2();
      break;
    case event_trigger_definition_format_c::types::event_trigger_definition_format3:
      out_j = obj.event_trigger_definition_format3();
      break;
    default:
      log_invalid_choice_id(obj.type(), "event_trigger_definition_format_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, event_trigger_definition_format_c& obj)
{
  if (in_j.find("listOfNodeLevelConfigurationStructuresForEventTrigger") != in_j.end()) {
    auto& format = obj.set_event_trigger_definition_format1();
    in_j.get_to(format);
  } else if (in_j.find("listOfCellLevelConfigurationStructuresForEventTrigger") != in_j.end()) {
    auto& format = obj.set_event_trigger_definition_format2();
    in_j.get_to(format);
  } else if (in_j.find("period") != in_j.end()) {
    auto& format = obj.set_event_trigger_definition_format3();
    in_j.get_to(format);
  }
}

const char* event_trigger_definition_format_c::types_opts::to_string() const
{
  static const char* names[] = {
      "eventTriggerDefinition-Format1", "eventTriggerDefinition-Format2", "eventTriggerDefinition-Format3"};
  return convert_enum_idx(names, 3, value, "event_trigger_definition_format_c::types");
}
bool event_trigger_definition_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {
      "eventTriggerDefinition-Format1", "eventTriggerDefinition-Format2", "eventTriggerDefinition-Format3"};
  uint32_t idx;
  if (convert_enum_str(names, 3, str.c_str(), idx, "event_trigger_definition_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t event_trigger_definition_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "event_trigger_definition_format_c::types");
}

// IndicationHeaderFormat ::= CHOICE
void ind_hdr_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("indicationHeader-Format1");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ind_hdr_format_c::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ind_hdr_format_c::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ind_hdr_format_c& obj)
{
  out_j = obj.ind_hdr_format1();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ind_hdr_format_c& obj)
{
  auto& format = obj.ind_hdr_format1();
  in_j.get_to(format);
}

const char* ind_hdr_format_c::types_opts::to_string() const
{
  static const char* names[] = {"indicationHeader-Format1"};
  return convert_enum_idx(names, 1, value, "ind_hdr_format_c::types");
}
bool ind_hdr_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"indicationHeader-Format1"};
  uint32_t           idx;
  if (convert_enum_str(names, 1, str.c_str(), idx, "ind_hdr_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t ind_hdr_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ind_hdr_format_c::types");
}

// IndicationMessageFormat ::= CHOICE
void ind_msg_format_c::destroy_()
{
  switch (type_) {
    case types::ind_msg_format1:
      c.destroy<e2_sm_ccc_ind_msg_format1_s>();
      break;
    case types::ind_msg_format2:
      c.destroy<e2_sm_ccc_ind_msg_format2_s>();
      break;
    default:
      break;
  }
}
void ind_msg_format_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ind_msg_format1:
      c.init<e2_sm_ccc_ind_msg_format1_s>();
      break;
    case types::ind_msg_format2:
      c.init<e2_sm_ccc_ind_msg_format2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ind_msg_format_c");
  }
}
ind_msg_format_c::ind_msg_format_c(const ind_msg_format_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ind_msg_format1:
      c.init(other.c.get<e2_sm_ccc_ind_msg_format1_s>());
      break;
    case types::ind_msg_format2:
      c.init(other.c.get<e2_sm_ccc_ind_msg_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ind_msg_format_c");
  }
}
ind_msg_format_c& ind_msg_format_c::operator=(const ind_msg_format_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ind_msg_format1:
      c.set(other.c.get<e2_sm_ccc_ind_msg_format1_s>());
      break;
    case types::ind_msg_format2:
      c.set(other.c.get<e2_sm_ccc_ind_msg_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ind_msg_format_c");
  }

  return *this;
}
e2_sm_ccc_ind_msg_format1_s& ind_msg_format_c::set_ind_msg_format1()
{
  set(types::ind_msg_format1);
  return c.get<e2_sm_ccc_ind_msg_format1_s>();
}
e2_sm_ccc_ind_msg_format2_s& ind_msg_format_c::set_ind_msg_format2()
{
  set(types::ind_msg_format2);
  return c.get<e2_sm_ccc_ind_msg_format2_s>();
}
void ind_msg_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ind_msg_format1:
      j.write_fieldname("indicationMessage-Format1");
      c.get<e2_sm_ccc_ind_msg_format1_s>().to_json(j);
      break;
    case types::ind_msg_format2:
      j.write_fieldname("indicationMessage-Format2");
      c.get<e2_sm_ccc_ind_msg_format2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ind_msg_format_c");
  }
  j.end_obj();
}
SRSASN_CODE ind_msg_format_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ind_msg_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_ind_msg_format1_s>().pack(bref));
      break;
    case types::ind_msg_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_ind_msg_format2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ind_msg_format_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ind_msg_format_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ind_msg_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_ind_msg_format1_s>().unpack(bref));
      break;
    case types::ind_msg_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_ind_msg_format2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ind_msg_format_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ind_msg_format_c& obj)
{
  switch (obj.type()) {
    case ind_msg_format_c::types::ind_msg_format1:
      out_j = obj.ind_msg_format1();
      break;
    case ind_msg_format_c::types::ind_msg_format2:
      out_j = obj.ind_msg_format2();
      break;
    default:
      log_invalid_choice_id(obj.type(), "ind_msg_format_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ind_msg_format_c& obj)
{
  if (in_j.find("listOfConfigurationStructuresReported") != in_j.end()) {
    auto& format = obj.set_ind_msg_format1();
    in_j.get_to(format);
  } else if (in_j.find("listOfCellsReported") != in_j.end()) {
    auto& format = obj.set_ind_msg_format2();
    in_j.get_to(format);
  }
}

const char* ind_msg_format_c::types_opts::to_string() const
{
  static const char* names[] = {"indicationMessage-Format1", "indicationMessage-Format2"};
  return convert_enum_idx(names, 2, value, "ind_msg_format_c::types");
}
bool ind_msg_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"indicationMessage-Format1", "indicationMessage-Format2"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "ind_msg_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t ind_msg_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ind_msg_format_c::types");
}

// QueryDefinitionFormat ::= CHOICE
void query_definition_format_c::destroy_()
{
  switch (type_) {
    case types::query_definition_format1:
      c.destroy<e2_sm_ccc_query_definition_format1_s>();
      break;
    case types::query_definition_format2:
      c.destroy<e2_sm_ccc_query_definition_format2_s>();
      break;
    default:
      break;
  }
}
void query_definition_format_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::query_definition_format1:
      c.init<e2_sm_ccc_query_definition_format1_s>();
      break;
    case types::query_definition_format2:
      c.init<e2_sm_ccc_query_definition_format2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "query_definition_format_c");
  }
}
query_definition_format_c::query_definition_format_c(const query_definition_format_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::query_definition_format1:
      c.init(other.c.get<e2_sm_ccc_query_definition_format1_s>());
      break;
    case types::query_definition_format2:
      c.init(other.c.get<e2_sm_ccc_query_definition_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "query_definition_format_c");
  }
}
query_definition_format_c& query_definition_format_c::operator=(const query_definition_format_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::query_definition_format1:
      c.set(other.c.get<e2_sm_ccc_query_definition_format1_s>());
      break;
    case types::query_definition_format2:
      c.set(other.c.get<e2_sm_ccc_query_definition_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "query_definition_format_c");
  }

  return *this;
}
e2_sm_ccc_query_definition_format1_s& query_definition_format_c::set_query_definition_format1()
{
  set(types::query_definition_format1);
  return c.get<e2_sm_ccc_query_definition_format1_s>();
}
e2_sm_ccc_query_definition_format2_s& query_definition_format_c::set_query_definition_format2()
{
  set(types::query_definition_format2);
  return c.get<e2_sm_ccc_query_definition_format2_s>();
}
void query_definition_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::query_definition_format1:
      j.write_fieldname("queryDefinition-Format1");
      c.get<e2_sm_ccc_query_definition_format1_s>().to_json(j);
      break;
    case types::query_definition_format2:
      j.write_fieldname("queryDefinition-Format2");
      c.get<e2_sm_ccc_query_definition_format2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "query_definition_format_c");
  }
  j.end_obj();
}
SRSASN_CODE query_definition_format_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::query_definition_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_query_definition_format1_s>().pack(bref));
      break;
    case types::query_definition_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_query_definition_format2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "query_definition_format_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE query_definition_format_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::query_definition_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_query_definition_format1_s>().unpack(bref));
      break;
    case types::query_definition_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_query_definition_format2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "query_definition_format_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const query_definition_format_c& obj)
{
  switch (obj.type()) {
    case query_definition_format_c::types::query_definition_format1:
      out_j = obj.query_definition_format1();
      break;
    case query_definition_format_c::types::query_definition_format2:
      out_j = obj.query_definition_format2();
      break;
    default:
      log_invalid_choice_id(obj.type(), "query_definition_format_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, query_definition_format_c& obj)
{
  if (in_j.find("listOfNodelevelRanConfigurationStructuresForQuery") != in_j.end()) {
    auto& format = obj.set_query_definition_format1();
    in_j.get_to(format);
  } else if (in_j.find("listOfCellsToBeQueriedForQuery") != in_j.end()) {
    auto& format = obj.set_query_definition_format1();
    in_j.get_to(format);
  }
}

const char* query_definition_format_c::types_opts::to_string() const
{
  static const char* names[] = {"queryDefinition-Format1", "queryDefinition-Format2"};
  return convert_enum_idx(names, 2, value, "query_definition_format_c::types");
}
bool query_definition_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"queryDefinition-Format1", "queryDefinition-Format2"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "query_definition_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t query_definition_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "query_definition_format_c::types");
}

// QueryHeaderFormat ::= CHOICE
void query_hdr_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("queryHeader-Format1");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE query_hdr_format_c::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE query_hdr_format_c::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const query_hdr_format_c& obj)
{
  out_j = obj.query_hdr_format1();
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, query_hdr_format_c& obj)
{
  auto& format = obj.query_hdr_format1();
  in_j.get_to(format);
}

const char* query_hdr_format_c::types_opts::to_string() const
{
  static const char* names[] = {"queryHeader-Format1"};
  return convert_enum_idx(names, 1, value, "query_hdr_format_c::types");
}
bool query_hdr_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"queryHeader-Format1"};
  uint32_t           idx;
  if (convert_enum_str(names, 1, str.c_str(), idx, "query_hdr_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t query_hdr_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "query_hdr_format_c::types");
}

// QueryOutcomeFormat ::= CHOICE
void query_outcome_format_c::destroy_()
{
  switch (type_) {
    case types::query_outcome_format1:
      c.destroy<e2_sm_ccc_query_outcome_format1_s>();
      break;
    case types::query_outcome_format2:
      c.destroy<e2_sm_ccc_query_outcome_format2_s>();
      break;
    default:
      break;
  }
}
void query_outcome_format_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::query_outcome_format1:
      c.init<e2_sm_ccc_query_outcome_format1_s>();
      break;
    case types::query_outcome_format2:
      c.init<e2_sm_ccc_query_outcome_format2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "query_outcome_format_c");
  }
}
query_outcome_format_c::query_outcome_format_c(const query_outcome_format_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::query_outcome_format1:
      c.init(other.c.get<e2_sm_ccc_query_outcome_format1_s>());
      break;
    case types::query_outcome_format2:
      c.init(other.c.get<e2_sm_ccc_query_outcome_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "query_outcome_format_c");
  }
}
query_outcome_format_c& query_outcome_format_c::operator=(const query_outcome_format_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::query_outcome_format1:
      c.set(other.c.get<e2_sm_ccc_query_outcome_format1_s>());
      break;
    case types::query_outcome_format2:
      c.set(other.c.get<e2_sm_ccc_query_outcome_format2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "query_outcome_format_c");
  }

  return *this;
}
e2_sm_ccc_query_outcome_format1_s& query_outcome_format_c::set_query_outcome_format1()
{
  set(types::query_outcome_format1);
  return c.get<e2_sm_ccc_query_outcome_format1_s>();
}
e2_sm_ccc_query_outcome_format2_s& query_outcome_format_c::set_query_outcome_format2()
{
  set(types::query_outcome_format2);
  return c.get<e2_sm_ccc_query_outcome_format2_s>();
}
void query_outcome_format_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::query_outcome_format1:
      j.write_fieldname("queryOutcome-Format1");
      c.get<e2_sm_ccc_query_outcome_format1_s>().to_json(j);
      break;
    case types::query_outcome_format2:
      j.write_fieldname("queryOutcome-Format2");
      c.get<e2_sm_ccc_query_outcome_format2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "query_outcome_format_c");
  }
  j.end_obj();
}
SRSASN_CODE query_outcome_format_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::query_outcome_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_query_outcome_format1_s>().pack(bref));
      break;
    case types::query_outcome_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_query_outcome_format2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "query_outcome_format_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE query_outcome_format_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::query_outcome_format1:
      HANDLE_CODE(c.get<e2_sm_ccc_query_outcome_format1_s>().unpack(bref));
      break;
    case types::query_outcome_format2:
      HANDLE_CODE(c.get<e2_sm_ccc_query_outcome_format2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "query_outcome_format_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const query_outcome_format_c& obj)
{
  switch (obj.type()) {
    case query_outcome_format_c::types::query_outcome_format1:
      out_j = obj.query_outcome_format1();
      break;
    case query_outcome_format_c::types::query_outcome_format2:
      out_j = obj.query_outcome_format2();
      break;
    default:
      log_invalid_choice_id(obj.type(), "query_outcome_format_c");
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, query_outcome_format_c& obj)
{
  if (in_j.find("listOfConfigurationStructuresToBeReportedForQuery") != in_j.end()) {
    auto& format = obj.set_query_outcome_format1();
    in_j.get_to(format);
  } else if (in_j.find("listOfCellsToBeReportedForQuery") != in_j.end()) {
    auto& format = obj.set_query_outcome_format2();
    in_j.get_to(format);
  }
}

const char* query_outcome_format_c::types_opts::to_string() const
{
  static const char* names[] = {"queryOutcome-Format1", "queryOutcome-Format2"};
  return convert_enum_idx(names, 2, value, "query_outcome_format_c::types");
}
bool query_outcome_format_c::types_opts::from_string(const std::string& str)
{
  static const char* names[] = {"queryOutcome-Format1", "queryOutcome-Format2"};
  uint32_t           idx;
  if (convert_enum_str(names, 2, str.c_str(), idx, "query_outcome_format_c::types")) {
    value = static_cast<options>(idx);
    return true;
  }
  value = nulltype;
  return false;
}
uint8_t query_outcome_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "query_outcome_format_c::types");
}

// RANFunctionName ::= SEQUENCE
SRSASN_CODE ran_function_name_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ran_function_instance_present, 1));

  HANDLE_CODE(ran_function_short_name.pack(bref));
  HANDLE_CODE(ran_function_service_model_o_id.pack(bref));
  HANDLE_CODE(ran_function_description.pack(bref));
  if (ran_function_instance_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, ran_function_instance, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_name_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ran_function_instance_present, 1));

  HANDLE_CODE(ran_function_short_name.unpack(bref));
  HANDLE_CODE(ran_function_service_model_o_id.unpack(bref));
  HANDLE_CODE(ran_function_description.unpack(bref));
  if (ran_function_instance_present) {
    HANDLE_CODE(unpack_unconstrained_integer(ran_function_instance, bref, false, true));
  }

  return SRSASN_SUCCESS;
}
void ran_function_name_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranFunctionShortName", ran_function_short_name.to_string());
  j.write_str("ranFunctionServiceModelOID", ran_function_service_model_o_id.to_string());
  j.write_str("ranFunctionDescription", ran_function_description.to_string());
  if (ran_function_instance_present) {
    j.write_int("ranFunctionInstance", ran_function_instance);
  }
  j.end_obj();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ran_function_name_s& obj)
{
  nlohmann::ordered_json& j       = out_j;
  j["ranFunctionShortName"]       = obj.ran_function_short_name.to_string();
  j["ranFunctionServiceModelOID"] = obj.ran_function_service_model_o_id.to_string();
  j["ranFunctionDescription"]     = obj.ran_function_description.to_string();
  if (obj.ran_function_instance_present) {
    j["ranFunctionInstance"] = obj.ran_function_instance;
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ran_function_name_s& obj)
{
  obj.ran_function_short_name.from_string(in_j["ranFunctionShortName"].get<std::string>());
  obj.ran_function_service_model_o_id.from_string(in_j["ranFunctionServiceModelOID"].get<std::string>());
  obj.ran_function_description.from_string(in_j["ranFunctionDescription"].get<std::string>());
  if (in_j.contains("ranFunctionInstance")) {
    in_j["ranFunctionInstance"].get_to(obj.ran_function_instance);
    obj.ran_function_instance_present = true;
  }
}

// RANFunctionDefinition ::= SEQUENCE
SRSASN_CODE ran_function_definition_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(list_of_supported_node_level_cfg_structures.size() > 0, 1));
  HANDLE_CODE(bref.pack(list_of_cells_for_ran_function_definition.size() > 0, 1));

  HANDLE_CODE(ran_function_name.pack(bref));
  if (list_of_supported_node_level_cfg_structures.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_supported_node_level_cfg_structures, 1, 64, true));
  }
  if (list_of_cells_for_ran_function_definition.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, list_of_cells_for_ran_function_definition, 1, 64, true));
  }

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_definition_s::unpack(cbit_ref& bref)
{
  bool list_of_supported_node_level_cfg_structures_present;
  HANDLE_CODE(bref.unpack(list_of_supported_node_level_cfg_structures_present, 1));
  bool list_of_cells_for_ran_function_definition_present;
  HANDLE_CODE(bref.unpack(list_of_cells_for_ran_function_definition_present, 1));

  HANDLE_CODE(ran_function_name.unpack(bref));
  if (list_of_supported_node_level_cfg_structures_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_supported_node_level_cfg_structures, bref, 1, 64, true));
  }
  if (list_of_cells_for_ran_function_definition_present) {
    HANDLE_CODE(unpack_dyn_seq_of(list_of_cells_for_ran_function_definition, bref, 1, 64, true));
  }

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ran_function_definition_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RANFunctionDefinition");
  j.write_fieldname("ranFunctionName");
  ran_function_name.to_json(j);
  if (list_of_supported_node_level_cfg_structures.size() > 0) {
    j.start_array("listOfSupportedNodeLevelConfigurationStructures");
    for (const auto& e1 : list_of_supported_node_level_cfg_structures) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (list_of_cells_for_ran_function_definition.size() > 0) {
    j.start_array("listOfCellsForRANFunctionDefinition");
    for (const auto& e1 : list_of_cells_for_ran_function_definition) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ran_function_definition_s& obj)
{
  out_j["ranFunctionName"] = obj.ran_function_name;
  if (obj.list_of_supported_node_level_cfg_structures.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_supported_node_level_cfg_structures) {
        j_array.push_back(e1);
      }
      out_j["listOfSupportedNodeLevelConfigurationStructures"] = j_array;
    }
  }
  if (obj.list_of_cells_for_ran_function_definition.size() > 0) {
    {
      nlohmann::ordered_json j_array = nlohmann::ordered_json::array();
      for (const auto& e1 : obj.list_of_cells_for_ran_function_definition) {
        j_array.push_back(e1);
      }
      out_j["listOfCellsForRANFunctionDefinition"] = j_array;
    }
  }
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ran_function_definition_s& obj)
{
  in_j["ranFunctionName"].get_to(obj.ran_function_name);
  if (in_j.contains("listOfSupportedNodeLevelConfigurationStructures")) {
    {
      const auto& in_arr  = in_j.at("listOfSupportedNodeLevelConfigurationStructures");
      auto&       out_arr = obj.list_of_supported_node_level_cfg_structures;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<ran_cfg_structure_s>();
      }
    }
  }
  if (in_j.contains("listOfCellsForRANFunctionDefinition")) {
    {
      const auto& in_arr  = in_j.at("listOfCellsForRANFunctionDefinition");
      auto&       out_arr = obj.list_of_cells_for_ran_function_definition;
      out_arr.resize(in_arr.size());
      for (size_t i = 0; i < in_arr.size(); ++i) {
        out_arr[i] = in_arr[i].get<cell_for_ran_function_definition_s>();
      }
    }
  }
}

// RICActionDefinition ::= SEQUENCE
SRSASN_CODE ric_action_definition_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_style_type, false, true));
  HANDLE_CODE(action_definition_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_definition_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_unconstrained_integer(ric_style_type, bref, false, true));
  HANDLE_CODE(action_definition_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_action_definition_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RICActionDefinition");
  j.write_int("ricStyleType", ric_style_type);
  j.write_fieldname("actionDefinitionFormat");
  action_definition_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_action_definition_s& obj)
{
  out_j["ricStyleType"]           = obj.ric_style_type;
  out_j["actionDefinitionFormat"] = obj.action_definition_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_action_definition_s& obj)
{
  in_j["ricStyleType"].get_to(obj.ric_style_type);
  in_j["actionDefinitionFormat"].get_to(obj.action_definition_format);
}

// RIC-Control-Header ::= SEQUENCE
SRSASN_CODE ric_ctrl_hdr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ctrl_hdr_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_hdr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ctrl_hdr_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_ctrl_hdr_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Control-Header");
  j.write_fieldname("controlHeaderFormat");
  ctrl_hdr_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_ctrl_hdr_s& obj)
{
  out_j["controlHeaderFormat"] = obj.ctrl_hdr_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_ctrl_hdr_s& obj)
{
  obj.ctrl_hdr_format = in_j["controlHeaderFormat"].get<ctrl_hdr_format_c>();
}

// RIC-Control-Message ::= SEQUENCE
SRSASN_CODE ric_ctrl_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ctrl_msg_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ctrl_msg_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_ctrl_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Control-Message");
  j.write_fieldname("controlMessageFormat");
  ctrl_msg_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_ctrl_msg_s& obj)
{
  out_j["controlMessageFormat"] = obj.ctrl_msg_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_ctrl_msg_s& obj)
{
  in_j["controlMessageFormat"].get_to(obj.ctrl_msg_format);
}

// RIC-Control-Outcome ::= SEQUENCE
SRSASN_CODE ric_ctrl_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ctrl_outcome_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ctrl_outcome_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_ctrl_outcome_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Control-Outcome");
  j.write_fieldname("controlOutcomeFormat");
  ctrl_outcome_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_ctrl_outcome_s& obj)
{
  out_j["controlOutcomeFormat"] = obj.ctrl_outcome_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_ctrl_outcome_s& obj)
{
  in_j["controlOutcomeFormat"].get_to(obj.ctrl_outcome_format);
}

// RICEventTriggerDefinition ::= SEQUENCE
SRSASN_CODE ric_event_trigger_definition_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(event_trigger_definition_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_event_trigger_definition_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(event_trigger_definition_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_event_trigger_definition_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RICEventTriggerDefinition");
  j.write_fieldname("eventTriggerDefinitionFormat");
  event_trigger_definition_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_event_trigger_definition_s& obj)
{
  out_j["eventTriggerDefinitionFormat"] = obj.event_trigger_definition_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_event_trigger_definition_s& obj)
{
  in_j["eventTriggerDefinitionFormat"].get_to(obj.event_trigger_definition_format);
}

// RIC-Indication-Header ::= SEQUENCE
SRSASN_CODE ric_ind_hdr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ind_hdr_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ind_hdr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ind_hdr_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_ind_hdr_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Indication-Header");
  j.write_fieldname("indicationHeaderFormat");
  ind_hdr_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_ind_hdr_s& obj)
{
  out_j["indicationHeaderFormat"] = obj.ind_hdr_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_ind_hdr_s& obj)
{
  in_j["indicationHeaderFormat"].get_to(obj.ind_hdr_format);
}

// RIC-Indication-Message ::= SEQUENCE
SRSASN_CODE ric_ind_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ind_msg_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ind_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ind_msg_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_ind_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Indication-Message");
  j.write_fieldname("indicationMessageFormat");
  ind_msg_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_ind_msg_s& obj)
{
  out_j["indicationMessageFormat"] = obj.ind_msg_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_ind_msg_s& obj)
{
  in_j["indicationMessageFormat"].get_to(obj.ind_msg_format);
}

// RIC-Query-Definition ::= SEQUENCE
SRSASN_CODE ric_query_definition_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(query_definition_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_definition_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(query_definition_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_query_definition_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Query-Definition");
  j.write_fieldname("queryDefinitionFormat");
  query_definition_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_query_definition_s& obj)
{
  out_j["queryDefinitionFormat"] = obj.query_definition_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_query_definition_s& obj)
{
  in_j["queryDefinitionFormat"].get_to(obj.query_definition_format);
}

// RIC-Query-Header ::= SEQUENCE
SRSASN_CODE ric_query_hdr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(query_hdr_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_hdr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(query_hdr_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_query_hdr_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Query-Header");
  j.write_fieldname("queryHeaderFormat");
  query_hdr_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_query_hdr_s& obj)
{
  out_j["queryHeaderFormat"] = obj.query_hdr_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_query_hdr_s& obj)
{
  in_j["queryHeaderFormat"].get_to(obj.query_hdr_format);
}

// RIC-Query-Outcome ::= SEQUENCE
SRSASN_CODE ric_query_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(query_outcome_format.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(query_outcome_format.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ric_query_outcome_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("RIC-Query-Outcome");
  j.write_fieldname("queryOutcomeFormat");
  query_outcome_format.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
void asn1::e2sm_ccc::to_json(nlohmann::ordered_json& out_j, const ric_query_outcome_s& obj)
{
  out_j["queryOutcomeFormat"] = obj.query_outcome_format;
}
void asn1::e2sm_ccc::from_json(const nlohmann::ordered_json& in_j, ric_query_outcome_s& obj)
{
  in_j["queryOutcomeFormat"].get_to(obj.query_outcome_format);
}

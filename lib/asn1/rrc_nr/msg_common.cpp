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

#include "srsran/asn1/rrc_nr/msg_common.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SL-MinMaxMCS-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_min_max_mcs_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_mcs_table_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, sl_min_mcs_pssch_r16, (uint8_t)0u, (uint8_t)27u));
  HANDLE_CODE(pack_integer(bref, sl_max_mcs_pssch_r16, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_min_max_mcs_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_mcs_table_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(sl_min_mcs_pssch_r16, bref, (uint8_t)0u, (uint8_t)27u));
  HANDLE_CODE(unpack_integer(sl_max_mcs_pssch_r16, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void sl_min_max_mcs_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-MCS-Table-r16", sl_mcs_table_r16.to_string());
  j.write_int("sl-MinMCS-PSSCH-r16", sl_min_mcs_pssch_r16);
  j.write_int("sl-MaxMCS-PSSCH-r16", sl_max_mcs_pssch_r16);
  j.end_obj();
}

const char* sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16_opts::to_string() const
{
  static const char* names[] = {"qam64", "qam256", "qam64LowSE"};
  return convert_enum_idx(names, 3, value, "sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16_e_");
}

// SL-PriorityTxConfigIndex-r16 ::= SEQUENCE
SRSASN_CODE sl_prio_tx_cfg_idx_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_prio_thres_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_default_tx_cfg_idx_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_cbr_cfg_idx_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_tx_cfg_idx_list_r16.size() > 0, 1));

  if (sl_prio_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_prio_thres_r16, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_default_tx_cfg_idx_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_default_tx_cfg_idx_r16, (uint8_t)0u, (uint8_t)15u));
  }
  if (sl_cbr_cfg_idx_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_cbr_cfg_idx_r16, (uint8_t)0u, (uint8_t)7u));
  }
  if (sl_tx_cfg_idx_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_cfg_idx_list_r16, 1, 16, integer_packer<uint8_t>(0, 63)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_prio_tx_cfg_idx_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_prio_thres_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_default_tx_cfg_idx_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_cbr_cfg_idx_r16_present, 1));
  bool sl_tx_cfg_idx_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_tx_cfg_idx_list_r16_present, 1));

  if (sl_prio_thres_r16_present) {
    HANDLE_CODE(unpack_integer(sl_prio_thres_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_default_tx_cfg_idx_r16_present) {
    HANDLE_CODE(unpack_integer(sl_default_tx_cfg_idx_r16, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (sl_cbr_cfg_idx_r16_present) {
    HANDLE_CODE(unpack_integer(sl_cbr_cfg_idx_r16, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (sl_tx_cfg_idx_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_tx_cfg_idx_list_r16, bref, 1, 16, integer_packer<uint8_t>(0, 63)));
  }

  return SRSASN_SUCCESS;
}
void sl_prio_tx_cfg_idx_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_prio_thres_r16_present) {
    j.write_int("sl-PriorityThreshold-r16", sl_prio_thres_r16);
  }
  if (sl_default_tx_cfg_idx_r16_present) {
    j.write_int("sl-DefaultTxConfigIndex-r16", sl_default_tx_cfg_idx_r16);
  }
  if (sl_cbr_cfg_idx_r16_present) {
    j.write_int("sl-CBR-ConfigIndex-r16", sl_cbr_cfg_idx_r16);
  }
  if (sl_tx_cfg_idx_list_r16.size() > 0) {
    j.start_array("sl-Tx-ConfigIndexList-r16");
    for (const auto& e1 : sl_tx_cfg_idx_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-PriorityTxConfigIndex-v1650 ::= SEQUENCE
SRSASN_CODE sl_prio_tx_cfg_idx_v1650_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_mcs_range_list_r16.size() > 0, 1));

  if (sl_mcs_range_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_mcs_range_list_r16, 1, 16, SeqOfPacker<Packer>(1, 3, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_prio_tx_cfg_idx_v1650_s::unpack(cbit_ref& bref)
{
  bool sl_mcs_range_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_mcs_range_list_r16_present, 1));

  if (sl_mcs_range_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_mcs_range_list_r16, bref, 1, 16, SeqOfPacker<Packer>(1, 3, Packer())));
  }

  return SRSASN_SUCCESS;
}
void sl_prio_tx_cfg_idx_v1650_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_mcs_range_list_r16.size() > 0) {
    j.start_array("sl-MCS-RangeList-r16");
    for (const auto& e1 : sl_mcs_range_list_r16) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-SelectionWindowConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_sel_win_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sl_prio_r16, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(sl_sel_win_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sel_win_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sl_prio_r16, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(sl_sel_win_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_sel_win_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-Priority-r16", sl_prio_r16);
  j.write_str("sl-SelectionWindow-r16", sl_sel_win_r16.to_string());
  j.end_obj();
}

const char* sl_sel_win_cfg_r16_s::sl_sel_win_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n5", "n10", "n20"};
  return convert_enum_idx(names, 4, value, "sl_sel_win_cfg_r16_s::sl_sel_win_r16_e_");
}
uint8_t sl_sel_win_cfg_r16_s::sl_sel_win_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 5, 10, 20};
  return map_enum_number(numbers, 4, value, "sl_sel_win_cfg_r16_s::sl_sel_win_r16_e_");
}

// SL-ThresholdRSRP-Condition1-B-1-r17 ::= SEQUENCE
SRSASN_CODE sl_thres_rsrp_condition1_b_1_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sl_prio_r17, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, sl_thres_rsrp_condition1_b_1_r17, (uint8_t)0u, (uint8_t)66u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_thres_rsrp_condition1_b_1_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sl_prio_r17, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(sl_thres_rsrp_condition1_b_1_r17, bref, (uint8_t)0u, (uint8_t)66u));

  return SRSASN_SUCCESS;
}
void sl_thres_rsrp_condition1_b_1_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-Priority-r17", sl_prio_r17);
  j.write_int("sl-ThresholdRSRP-Condition1-B-1-r17", sl_thres_rsrp_condition1_b_1_r17);
  j.end_obj();
}

// SL-InterUE-CoordinationScheme1-r17 ::= SEQUENCE
SRSASN_CODE sl_inter_ue_coordination_scheme1_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_iuc_explicit_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_iuc_condition_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_condition1_a_2_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_thres_rsrp_condition1_b_1_option1_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_thres_rsrp_condition1_b_1_option2_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_container_coord_info_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_container_request_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_trigger_condition_coord_info_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_trigger_condition_request_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_prio_coord_info_explicit_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_prio_coord_info_condition_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_prio_request_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_prio_preferred_res_set_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_max_slot_offset_triv_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_num_sub_ch_preferred_res_set_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_reserved_period_preferred_res_set_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_determine_res_type_r17_present, 1));

  if (sl_iuc_explicit_r17_present) {
    HANDLE_CODE(sl_iuc_explicit_r17.pack(bref));
  }
  if (sl_iuc_condition_r17_present) {
    HANDLE_CODE(sl_iuc_condition_r17.pack(bref));
  }
  if (sl_thres_rsrp_condition1_b_1_option1_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_thres_rsrp_condition1_b_1_option1_list_r17, 1, 8));
  }
  if (sl_thres_rsrp_condition1_b_1_option2_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_thres_rsrp_condition1_b_1_option2_list_r17, 1, 8));
  }
  if (sl_container_coord_info_r17_present) {
    HANDLE_CODE(sl_container_coord_info_r17.pack(bref));
  }
  if (sl_container_request_r17_present) {
    HANDLE_CODE(sl_container_request_r17.pack(bref));
  }
  if (sl_trigger_condition_coord_info_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_trigger_condition_coord_info_r17, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_trigger_condition_request_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_trigger_condition_request_r17, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_prio_coord_info_explicit_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_prio_coord_info_explicit_r17, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_prio_coord_info_condition_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_prio_coord_info_condition_r17, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_prio_request_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_prio_request_r17, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_prio_preferred_res_set_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_prio_preferred_res_set_r17, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_max_slot_offset_triv_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_max_slot_offset_triv_r17, (uint16_t)1u, (uint16_t)8000u));
  }
  if (sl_num_sub_ch_preferred_res_set_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_num_sub_ch_preferred_res_set_r17, (uint8_t)1u, (uint8_t)27u));
  }
  if (sl_reserved_period_preferred_res_set_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_reserved_period_preferred_res_set_r17, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_determine_res_type_r17_present) {
    HANDLE_CODE(sl_determine_res_type_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_inter_ue_coordination_scheme1_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_iuc_explicit_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_iuc_condition_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_condition1_a_2_r17_present, 1));
  bool sl_thres_rsrp_condition1_b_1_option1_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_thres_rsrp_condition1_b_1_option1_list_r17_present, 1));
  bool sl_thres_rsrp_condition1_b_1_option2_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_thres_rsrp_condition1_b_1_option2_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_container_coord_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_container_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_trigger_condition_coord_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_trigger_condition_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_prio_coord_info_explicit_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_prio_coord_info_condition_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_prio_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_prio_preferred_res_set_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_slot_offset_triv_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_num_sub_ch_preferred_res_set_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_reserved_period_preferred_res_set_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_determine_res_type_r17_present, 1));

  if (sl_iuc_explicit_r17_present) {
    HANDLE_CODE(sl_iuc_explicit_r17.unpack(bref));
  }
  if (sl_iuc_condition_r17_present) {
    HANDLE_CODE(sl_iuc_condition_r17.unpack(bref));
  }
  if (sl_thres_rsrp_condition1_b_1_option1_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_thres_rsrp_condition1_b_1_option1_list_r17, bref, 1, 8));
  }
  if (sl_thres_rsrp_condition1_b_1_option2_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_thres_rsrp_condition1_b_1_option2_list_r17, bref, 1, 8));
  }
  if (sl_container_coord_info_r17_present) {
    HANDLE_CODE(sl_container_coord_info_r17.unpack(bref));
  }
  if (sl_container_request_r17_present) {
    HANDLE_CODE(sl_container_request_r17.unpack(bref));
  }
  if (sl_trigger_condition_coord_info_r17_present) {
    HANDLE_CODE(unpack_integer(sl_trigger_condition_coord_info_r17, bref, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_trigger_condition_request_r17_present) {
    HANDLE_CODE(unpack_integer(sl_trigger_condition_request_r17, bref, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_prio_coord_info_explicit_r17_present) {
    HANDLE_CODE(unpack_integer(sl_prio_coord_info_explicit_r17, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_prio_coord_info_condition_r17_present) {
    HANDLE_CODE(unpack_integer(sl_prio_coord_info_condition_r17, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_prio_request_r17_present) {
    HANDLE_CODE(unpack_integer(sl_prio_request_r17, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_prio_preferred_res_set_r17_present) {
    HANDLE_CODE(unpack_integer(sl_prio_preferred_res_set_r17, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_max_slot_offset_triv_r17_present) {
    HANDLE_CODE(unpack_integer(sl_max_slot_offset_triv_r17, bref, (uint16_t)1u, (uint16_t)8000u));
  }
  if (sl_num_sub_ch_preferred_res_set_r17_present) {
    HANDLE_CODE(unpack_integer(sl_num_sub_ch_preferred_res_set_r17, bref, (uint8_t)1u, (uint8_t)27u));
  }
  if (sl_reserved_period_preferred_res_set_r17_present) {
    HANDLE_CODE(unpack_integer(sl_reserved_period_preferred_res_set_r17, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_determine_res_type_r17_present) {
    HANDLE_CODE(sl_determine_res_type_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_inter_ue_coordination_scheme1_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_iuc_explicit_r17_present) {
    j.write_str("sl-IUC-Explicit-r17", sl_iuc_explicit_r17.to_string());
  }
  if (sl_iuc_condition_r17_present) {
    j.write_str("sl-IUC-Condition-r17", sl_iuc_condition_r17.to_string());
  }
  if (sl_condition1_a_2_r17_present) {
    j.write_str("sl-Condition1-A-2-r17", "disabled");
  }
  if (sl_thres_rsrp_condition1_b_1_option1_list_r17.size() > 0) {
    j.start_array("sl-ThresholdRSRP-Condition1-B-1-Option1List-r17");
    for (const auto& e1 : sl_thres_rsrp_condition1_b_1_option1_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_thres_rsrp_condition1_b_1_option2_list_r17.size() > 0) {
    j.start_array("sl-ThresholdRSRP-Condition1-B-1-Option2List-r17");
    for (const auto& e1 : sl_thres_rsrp_condition1_b_1_option2_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_container_coord_info_r17_present) {
    j.write_str("sl-ContainerCoordInfo-r17", sl_container_coord_info_r17.to_string());
  }
  if (sl_container_request_r17_present) {
    j.write_str("sl-ContainerRequest-r17", sl_container_request_r17.to_string());
  }
  if (sl_trigger_condition_coord_info_r17_present) {
    j.write_int("sl-TriggerConditionCoordInfo-r17", sl_trigger_condition_coord_info_r17);
  }
  if (sl_trigger_condition_request_r17_present) {
    j.write_int("sl-TriggerConditionRequest-r17", sl_trigger_condition_request_r17);
  }
  if (sl_prio_coord_info_explicit_r17_present) {
    j.write_int("sl-PriorityCoordInfoExplicit-r17", sl_prio_coord_info_explicit_r17);
  }
  if (sl_prio_coord_info_condition_r17_present) {
    j.write_int("sl-PriorityCoordInfoCondition-r17", sl_prio_coord_info_condition_r17);
  }
  if (sl_prio_request_r17_present) {
    j.write_int("sl-PriorityRequest-r17", sl_prio_request_r17);
  }
  if (sl_prio_preferred_res_set_r17_present) {
    j.write_int("sl-PriorityPreferredResourceSet-r17", sl_prio_preferred_res_set_r17);
  }
  if (sl_max_slot_offset_triv_r17_present) {
    j.write_int("sl-MaxSlotOffsetTRIV-r17", sl_max_slot_offset_triv_r17);
  }
  if (sl_num_sub_ch_preferred_res_set_r17_present) {
    j.write_int("sl-NumSubCH-PreferredResourceSet-r17", sl_num_sub_ch_preferred_res_set_r17);
  }
  if (sl_reserved_period_preferred_res_set_r17_present) {
    j.write_int("sl-ReservedPeriodPreferredResourceSet-r17", sl_reserved_period_preferred_res_set_r17);
  }
  if (sl_determine_res_type_r17_present) {
    j.write_str("sl-DetermineResourceType-r17", sl_determine_res_type_r17.to_string());
  }
  j.end_obj();
}

const char* sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17_e_");
}

const char* sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17_e_");
}

const char* sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17_e_");
}

const char* sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17_e_");
}

const char* sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17_opts::to_string() const
{
  static const char* names[] = {"uea", "ueb"};
  return convert_enum_idx(names, 2, value, "sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17_e_");
}

// SL-InterUE-CoordinationScheme2-r17 ::= SEQUENCE
SRSASN_CODE sl_inter_ue_coordination_scheme2_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_iuc_scheme2_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_rb_set_psfch_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_type_ue_a_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_psfch_occasion_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_slot_level_res_exclusion_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_option_for_condition2_a_1_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_ind_ue_b_r17_present, 1));

  if (sl_rb_set_psfch_r17_present) {
    HANDLE_CODE(sl_rb_set_psfch_r17.pack(bref));
  }
  if (sl_psfch_occasion_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_psfch_occasion_r17, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_option_for_condition2_a_1_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_option_for_condition2_a_1_r17, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_ind_ue_b_r17_present) {
    HANDLE_CODE(sl_ind_ue_b_r17.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_delta_rsrp_thresh_v1720_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_delta_rsrp_thresh_v1720_present, 1));
      if (sl_delta_rsrp_thresh_v1720_present) {
        HANDLE_CODE(pack_integer(bref, sl_delta_rsrp_thresh_v1720, (int8_t)-30, (int8_t)30));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_inter_ue_coordination_scheme2_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_iuc_scheme2_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_rb_set_psfch_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_type_ue_a_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_psfch_occasion_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_slot_level_res_exclusion_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_option_for_condition2_a_1_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_ind_ue_b_r17_present, 1));

  if (sl_rb_set_psfch_r17_present) {
    HANDLE_CODE(sl_rb_set_psfch_r17.unpack(bref));
  }
  if (sl_psfch_occasion_r17_present) {
    HANDLE_CODE(unpack_integer(sl_psfch_occasion_r17, bref, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_option_for_condition2_a_1_r17_present) {
    HANDLE_CODE(unpack_integer(sl_option_for_condition2_a_1_r17, bref, (uint8_t)0u, (uint8_t)1u));
  }
  if (sl_ind_ue_b_r17_present) {
    HANDLE_CODE(sl_ind_ue_b_r17.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sl_delta_rsrp_thresh_v1720_present, 1));
      if (sl_delta_rsrp_thresh_v1720_present) {
        HANDLE_CODE(unpack_integer(sl_delta_rsrp_thresh_v1720, bref, (int8_t)-30, (int8_t)30));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_inter_ue_coordination_scheme2_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_iuc_scheme2_r17_present) {
    j.write_str("sl-IUC-Scheme2-r17", "enabled");
  }
  if (sl_rb_set_psfch_r17_present) {
    j.write_str("sl-RB-SetPSFCH-r17", sl_rb_set_psfch_r17.to_string());
  }
  if (sl_type_ue_a_r17_present) {
    j.write_str("sl-TypeUE-A-r17", "enabled");
  }
  if (sl_psfch_occasion_r17_present) {
    j.write_int("sl-PSFCH-Occasion-r17", sl_psfch_occasion_r17);
  }
  if (sl_slot_level_res_exclusion_r17_present) {
    j.write_str("sl-SlotLevelResourceExclusion-r17", "enabled");
  }
  if (sl_option_for_condition2_a_1_r17_present) {
    j.write_int("sl-OptionForCondition2-A-1-r17", sl_option_for_condition2_a_1_r17);
  }
  if (sl_ind_ue_b_r17_present) {
    j.write_str("sl-IndicationUE-B-r17", sl_ind_ue_b_r17.to_string());
  }
  if (ext) {
    if (sl_delta_rsrp_thresh_v1720_present) {
      j.write_int("sl-DeltaRSRP-Thresh-v1720", sl_delta_rsrp_thresh_v1720);
    }
  }
  j.end_obj();
}

const char* sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17_e_");
}

// SL-ResourceReservePeriod-r16 ::= CHOICE
void sl_res_reserve_period_r16_c::destroy_() {}
void sl_res_reserve_period_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_res_reserve_period_r16_c::sl_res_reserve_period_r16_c(const sl_res_reserve_period_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl_res_reserve_period1_r16:
      c.init(other.c.get<sl_res_reserve_period1_r16_e_>());
      break;
    case types::sl_res_reserve_period2_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_res_reserve_period_r16_c");
  }
}
sl_res_reserve_period_r16_c& sl_res_reserve_period_r16_c::operator=(const sl_res_reserve_period_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl_res_reserve_period1_r16:
      c.set(other.c.get<sl_res_reserve_period1_r16_e_>());
      break;
    case types::sl_res_reserve_period2_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_res_reserve_period_r16_c");
  }

  return *this;
}
sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_&
sl_res_reserve_period_r16_c::set_sl_res_reserve_period1_r16()
{
  set(types::sl_res_reserve_period1_r16);
  return c.get<sl_res_reserve_period1_r16_e_>();
}
uint8_t& sl_res_reserve_period_r16_c::set_sl_res_reserve_period2_r16()
{
  set(types::sl_res_reserve_period2_r16);
  return c.get<uint8_t>();
}
void sl_res_reserve_period_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl_res_reserve_period1_r16:
      j.write_str("sl-ResourceReservePeriod1-r16", c.get<sl_res_reserve_period1_r16_e_>().to_string());
      break;
    case types::sl_res_reserve_period2_r16:
      j.write_int("sl-ResourceReservePeriod2-r16", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sl_res_reserve_period_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_res_reserve_period_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl_res_reserve_period1_r16:
      HANDLE_CODE(c.get<sl_res_reserve_period1_r16_e_>().pack(bref));
      break;
    case types::sl_res_reserve_period2_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)99u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_res_reserve_period_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_res_reserve_period_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl_res_reserve_period1_r16:
      HANDLE_CODE(c.get<sl_res_reserve_period1_r16_e_>().unpack(bref));
      break;
    case types::sl_res_reserve_period2_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)99u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_res_reserve_period_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_opts::to_string() const
{
  static const char* names[] = {
      "ms0", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms700", "ms800", "ms900", "ms1000"};
  return convert_enum_idx(names, 11, value, "sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_");
}
uint16_t sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(numbers, 11, value, "sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_");
}

// SL-TxPercentageConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_tx_percentage_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sl_prio_r16, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(sl_tx_percentage_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_percentage_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sl_prio_r16, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(sl_tx_percentage_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_tx_percentage_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-Priority-r16", sl_prio_r16);
  j.write_str("sl-TxPercentage-r16", sl_tx_percentage_r16.to_string());
  j.end_obj();
}

const char* sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_opts::to_string() const
{
  static const char* names[] = {"p20", "p35", "p50"};
  return convert_enum_idx(names, 3, value, "sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_e_");
}
uint8_t sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 35, 50};
  return map_enum_number(numbers, 3, value, "sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_e_");
}

// SL-ZoneConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_zone_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sl_zone_len_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_zone_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sl_zone_len_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_zone_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-ZoneLength-r16", sl_zone_len_r16.to_string());
  j.end_obj();
}

const char* sl_zone_cfg_r16_s::sl_zone_len_r16_opts::to_string() const
{
  static const char* names[] = {"m5", "m10", "m20", "m30", "m40", "m50", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_zone_cfg_r16_s::sl_zone_len_r16_e_");
}
uint8_t sl_zone_cfg_r16_s::sl_zone_len_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 30, 40, 50};
  return map_enum_number(numbers, 6, value, "sl_zone_cfg_r16_s::sl_zone_len_r16_e_");
}

// FilterCoefficient ::= ENUMERATED
const char* filt_coef_opts::to_string() const
{
  static const char* names[] = {"fc0",
                                "fc1",
                                "fc2",
                                "fc3",
                                "fc4",
                                "fc5",
                                "fc6",
                                "fc7",
                                "fc8",
                                "fc9",
                                "fc11",
                                "fc13",
                                "fc15",
                                "fc17",
                                "fc19",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "filt_coef_e");
}
uint8_t filt_coef_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19};
  return map_enum_number(numbers, 15, value, "filt_coef_e");
}

// SL-InterUE-CoordinationConfig-r17 ::= SEQUENCE
SRSASN_CODE sl_inter_ue_coordination_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_inter_ue_coordination_scheme1_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_inter_ue_coordination_scheme2_r17_present, 1));

  if (sl_inter_ue_coordination_scheme1_r17_present) {
    HANDLE_CODE(sl_inter_ue_coordination_scheme1_r17.pack(bref));
  }
  if (sl_inter_ue_coordination_scheme2_r17_present) {
    HANDLE_CODE(sl_inter_ue_coordination_scheme2_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_inter_ue_coordination_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_inter_ue_coordination_scheme1_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_inter_ue_coordination_scheme2_r17_present, 1));

  if (sl_inter_ue_coordination_scheme1_r17_present) {
    HANDLE_CODE(sl_inter_ue_coordination_scheme1_r17.unpack(bref));
  }
  if (sl_inter_ue_coordination_scheme2_r17_present) {
    HANDLE_CODE(sl_inter_ue_coordination_scheme2_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_inter_ue_coordination_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_inter_ue_coordination_scheme1_r17_present) {
    j.write_fieldname("sl-InterUE-CoordinationScheme1-r17");
    sl_inter_ue_coordination_scheme1_r17.to_json(j);
  }
  if (sl_inter_ue_coordination_scheme2_r17_present) {
    j.write_fieldname("sl-InterUE-CoordinationScheme2-r17");
    sl_inter_ue_coordination_scheme2_r17.to_json(j);
  }
  j.end_obj();
}

// SL-PBPS-CPS-Config-r17 ::= SEQUENCE
SRSASN_CODE sl_pbps_cp_s_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_allowed_res_sel_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_min_num_candidate_slots_periodic_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_pbps_occasion_reserve_period_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_add_pbps_occasion_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_cp_s_win_periodic_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_min_num_candidate_slots_aperiodic_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_min_num_rssi_meas_slots_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_default_cbr_random_sel_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_default_cbr_partial_sensing_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_cp_s_win_aperiodic_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_partial_sensing_inactive_time_r17_present, 1));

  if (sl_allowed_res_sel_cfg_r17_present) {
    HANDLE_CODE(sl_allowed_res_sel_cfg_r17.pack(bref));
  }
  if (sl_min_num_candidate_slots_periodic_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_min_num_candidate_slots_periodic_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (sl_pbps_occasion_reserve_period_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_pbps_occasion_reserve_period_list_r17, 1, 16, integer_packer<uint8_t>(1, 16)));
  }
  if (sl_cp_s_win_periodic_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_cp_s_win_periodic_r17, (uint8_t)5u, (uint8_t)30u));
  }
  if (sl_min_num_candidate_slots_aperiodic_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_min_num_candidate_slots_aperiodic_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (sl_min_num_rssi_meas_slots_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_min_num_rssi_meas_slots_r17, (uint16_t)1u, (uint16_t)800u));
  }
  if (sl_default_cbr_random_sel_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_default_cbr_random_sel_r17, (uint8_t)0u, (uint8_t)100u));
  }
  if (sl_default_cbr_partial_sensing_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_default_cbr_partial_sensing_r17, (uint8_t)0u, (uint8_t)100u));
  }
  if (sl_cp_s_win_aperiodic_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_cp_s_win_aperiodic_r17, (uint8_t)0u, (uint8_t)30u));
  }
  if (sl_partial_sensing_inactive_time_r17_present) {
    HANDLE_CODE(sl_partial_sensing_inactive_time_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pbps_cp_s_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_allowed_res_sel_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_min_num_candidate_slots_periodic_r17_present, 1));
  bool sl_pbps_occasion_reserve_period_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_pbps_occasion_reserve_period_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_add_pbps_occasion_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_cp_s_win_periodic_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_min_num_candidate_slots_aperiodic_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_min_num_rssi_meas_slots_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_default_cbr_random_sel_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_default_cbr_partial_sensing_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_cp_s_win_aperiodic_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_partial_sensing_inactive_time_r17_present, 1));

  if (sl_allowed_res_sel_cfg_r17_present) {
    HANDLE_CODE(sl_allowed_res_sel_cfg_r17.unpack(bref));
  }
  if (sl_min_num_candidate_slots_periodic_r17_present) {
    HANDLE_CODE(unpack_integer(sl_min_num_candidate_slots_periodic_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (sl_pbps_occasion_reserve_period_list_r17_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_pbps_occasion_reserve_period_list_r17, bref, 1, 16, integer_packer<uint8_t>(1, 16)));
  }
  if (sl_cp_s_win_periodic_r17_present) {
    HANDLE_CODE(unpack_integer(sl_cp_s_win_periodic_r17, bref, (uint8_t)5u, (uint8_t)30u));
  }
  if (sl_min_num_candidate_slots_aperiodic_r17_present) {
    HANDLE_CODE(unpack_integer(sl_min_num_candidate_slots_aperiodic_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (sl_min_num_rssi_meas_slots_r17_present) {
    HANDLE_CODE(unpack_integer(sl_min_num_rssi_meas_slots_r17, bref, (uint16_t)1u, (uint16_t)800u));
  }
  if (sl_default_cbr_random_sel_r17_present) {
    HANDLE_CODE(unpack_integer(sl_default_cbr_random_sel_r17, bref, (uint8_t)0u, (uint8_t)100u));
  }
  if (sl_default_cbr_partial_sensing_r17_present) {
    HANDLE_CODE(unpack_integer(sl_default_cbr_partial_sensing_r17, bref, (uint8_t)0u, (uint8_t)100u));
  }
  if (sl_cp_s_win_aperiodic_r17_present) {
    HANDLE_CODE(unpack_integer(sl_cp_s_win_aperiodic_r17, bref, (uint8_t)0u, (uint8_t)30u));
  }
  if (sl_partial_sensing_inactive_time_r17_present) {
    HANDLE_CODE(sl_partial_sensing_inactive_time_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_pbps_cp_s_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_allowed_res_sel_cfg_r17_present) {
    j.write_str("sl-AllowedResourceSelectionConfig-r17", sl_allowed_res_sel_cfg_r17.to_string());
  }
  if (sl_min_num_candidate_slots_periodic_r17_present) {
    j.write_int("sl-MinNumCandidateSlotsPeriodic-r17", sl_min_num_candidate_slots_periodic_r17);
  }
  if (sl_pbps_occasion_reserve_period_list_r17.size() > 0) {
    j.start_array("sl-PBPS-OccasionReservePeriodList-r17");
    for (const auto& e1 : sl_pbps_occasion_reserve_period_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_add_pbps_occasion_r17_present) {
    j.write_str("sl-Additional-PBPS-Occasion-r17", "monitored");
  }
  if (sl_cp_s_win_periodic_r17_present) {
    j.write_int("sl-CPS-WindowPeriodic-r17", sl_cp_s_win_periodic_r17);
  }
  if (sl_min_num_candidate_slots_aperiodic_r17_present) {
    j.write_int("sl-MinNumCandidateSlotsAperiodic-r17", sl_min_num_candidate_slots_aperiodic_r17);
  }
  if (sl_min_num_rssi_meas_slots_r17_present) {
    j.write_int("sl-MinNumRssiMeasurementSlots-r17", sl_min_num_rssi_meas_slots_r17);
  }
  if (sl_default_cbr_random_sel_r17_present) {
    j.write_int("sl-DefaultCBR-RandomSelection-r17", sl_default_cbr_random_sel_r17);
  }
  if (sl_default_cbr_partial_sensing_r17_present) {
    j.write_int("sl-DefaultCBR-PartialSensing-r17", sl_default_cbr_partial_sensing_r17);
  }
  if (sl_cp_s_win_aperiodic_r17_present) {
    j.write_int("sl-CPS-WindowAperiodic-r17", sl_cp_s_win_aperiodic_r17);
  }
  if (sl_partial_sensing_inactive_time_r17_present) {
    j.write_str("sl-PartialSensingInactiveTime-r17", sl_partial_sensing_inactive_time_r17.to_string());
  }
  j.end_obj();
}

const char* sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_opts::to_string() const
{
  static const char* names[] = {"c1", "c2", "c3", "c4", "c5", "c6", "c7"};
  return convert_enum_idx(names, 7, value, "sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_e_");
}
uint8_t sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(numbers, 7, value, "sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_e_");
}

const char* sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17_e_");
}

// SL-PSCCH-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_pscch_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_time_res_pscch_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_freq_res_pscch_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_dmrs_scramble_id_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_num_reserved_bits_r16_present, 1));

  if (sl_time_res_pscch_r16_present) {
    HANDLE_CODE(sl_time_res_pscch_r16.pack(bref));
  }
  if (sl_freq_res_pscch_r16_present) {
    HANDLE_CODE(sl_freq_res_pscch_r16.pack(bref));
  }
  if (sl_dmrs_scramble_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_dmrs_scramble_id_r16, (uint32_t)0u, (uint32_t)65535u));
  }
  if (sl_num_reserved_bits_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_num_reserved_bits_r16, (uint8_t)2u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pscch_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_time_res_pscch_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_freq_res_pscch_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_dmrs_scramble_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_num_reserved_bits_r16_present, 1));

  if (sl_time_res_pscch_r16_present) {
    HANDLE_CODE(sl_time_res_pscch_r16.unpack(bref));
  }
  if (sl_freq_res_pscch_r16_present) {
    HANDLE_CODE(sl_freq_res_pscch_r16.unpack(bref));
  }
  if (sl_dmrs_scramble_id_r16_present) {
    HANDLE_CODE(unpack_integer(sl_dmrs_scramble_id_r16, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (sl_num_reserved_bits_r16_present) {
    HANDLE_CODE(unpack_integer(sl_num_reserved_bits_r16, bref, (uint8_t)2u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
void sl_pscch_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_time_res_pscch_r16_present) {
    j.write_str("sl-TimeResourcePSCCH-r16", sl_time_res_pscch_r16.to_string());
  }
  if (sl_freq_res_pscch_r16_present) {
    j.write_str("sl-FreqResourcePSCCH-r16", sl_freq_res_pscch_r16.to_string());
  }
  if (sl_dmrs_scramble_id_r16_present) {
    j.write_int("sl-DMRS-ScrambleID-r16", sl_dmrs_scramble_id_r16);
  }
  if (sl_num_reserved_bits_r16_present) {
    j.write_int("sl-NumReservedBits-r16", sl_num_reserved_bits_r16);
  }
  j.end_obj();
}

const char* sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_opts::to_string() const
{
  static const char* names[] = {"n2", "n3"};
  return convert_enum_idx(names, 2, value, "sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_e_");
}
uint8_t sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_e_");
}

const char* sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_opts::to_string() const
{
  static const char* names[] = {"n10", "n12", "n15", "n20", "n25"};
  return convert_enum_idx(names, 5, value, "sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_e_");
}
uint8_t sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 12, 15, 20, 25};
  return map_enum_number(numbers, 5, value, "sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_e_");
}

// SL-PSFCH-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_psfch_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_psfch_period_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_psfch_rb_set_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_num_mux_cs_pair_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_min_time_gap_psfch_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_psfch_hop_id_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_psfch_candidate_res_type_r16_present, 1));

  if (sl_psfch_period_r16_present) {
    HANDLE_CODE(sl_psfch_period_r16.pack(bref));
  }
  if (sl_psfch_rb_set_r16_present) {
    HANDLE_CODE(sl_psfch_rb_set_r16.pack(bref));
  }
  if (sl_num_mux_cs_pair_r16_present) {
    HANDLE_CODE(sl_num_mux_cs_pair_r16.pack(bref));
  }
  if (sl_min_time_gap_psfch_r16_present) {
    HANDLE_CODE(sl_min_time_gap_psfch_r16.pack(bref));
  }
  if (sl_psfch_hop_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_psfch_hop_id_r16, (uint16_t)0u, (uint16_t)1023u));
  }
  if (sl_psfch_candidate_res_type_r16_present) {
    HANDLE_CODE(sl_psfch_candidate_res_type_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_psfch_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_psfch_period_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_psfch_rb_set_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_num_mux_cs_pair_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_min_time_gap_psfch_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_psfch_hop_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_psfch_candidate_res_type_r16_present, 1));

  if (sl_psfch_period_r16_present) {
    HANDLE_CODE(sl_psfch_period_r16.unpack(bref));
  }
  if (sl_psfch_rb_set_r16_present) {
    HANDLE_CODE(sl_psfch_rb_set_r16.unpack(bref));
  }
  if (sl_num_mux_cs_pair_r16_present) {
    HANDLE_CODE(sl_num_mux_cs_pair_r16.unpack(bref));
  }
  if (sl_min_time_gap_psfch_r16_present) {
    HANDLE_CODE(sl_min_time_gap_psfch_r16.unpack(bref));
  }
  if (sl_psfch_hop_id_r16_present) {
    HANDLE_CODE(unpack_integer(sl_psfch_hop_id_r16, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (sl_psfch_candidate_res_type_r16_present) {
    HANDLE_CODE(sl_psfch_candidate_res_type_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_psfch_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_psfch_period_r16_present) {
    j.write_str("sl-PSFCH-Period-r16", sl_psfch_period_r16.to_string());
  }
  if (sl_psfch_rb_set_r16_present) {
    j.write_str("sl-PSFCH-RB-Set-r16", sl_psfch_rb_set_r16.to_string());
  }
  if (sl_num_mux_cs_pair_r16_present) {
    j.write_str("sl-NumMuxCS-Pair-r16", sl_num_mux_cs_pair_r16.to_string());
  }
  if (sl_min_time_gap_psfch_r16_present) {
    j.write_str("sl-MinTimeGapPSFCH-r16", sl_min_time_gap_psfch_r16.to_string());
  }
  if (sl_psfch_hop_id_r16_present) {
    j.write_int("sl-PSFCH-HopID-r16", sl_psfch_hop_id_r16);
  }
  if (sl_psfch_candidate_res_type_r16_present) {
    j.write_str("sl-PSFCH-CandidateResourceType-r16", sl_psfch_candidate_res_type_r16.to_string());
  }
  j.end_obj();
}

const char* sl_psfch_cfg_r16_s::sl_psfch_period_r16_opts::to_string() const
{
  static const char* names[] = {"sl0", "sl1", "sl2", "sl4"};
  return convert_enum_idx(names, 4, value, "sl_psfch_cfg_r16_s::sl_psfch_period_r16_e_");
}
uint8_t sl_psfch_cfg_r16_s::sl_psfch_period_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_psfch_cfg_r16_s::sl_psfch_period_r16_e_");
}

const char* sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n6"};
  return convert_enum_idx(names, 4, value, "sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_e_");
}
uint8_t sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 6};
  return map_enum_number(numbers, 4, value, "sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_e_");
}

const char* sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_opts::to_string() const
{
  static const char* names[] = {"sl2", "sl3"};
  return convert_enum_idx(names, 2, value, "sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_e_");
}
uint8_t sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_e_");
}

const char* sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16_opts::to_string() const
{
  static const char* names[] = {"startSubCH", "allocSubCH"};
  return convert_enum_idx(names, 2, value, "sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16_e_");
}

// SL-PSSCH-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_pssch_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_pssch_dmrs_time_pattern_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_beta_offsets2nd_sci_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_scaling_r16_present, 1));

  if (sl_pssch_dmrs_time_pattern_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_pssch_dmrs_time_pattern_list_r16, 1, 3, integer_packer<uint8_t>(2, 4)));
  }
  if (sl_beta_offsets2nd_sci_r16_present) {
    HANDLE_CODE(pack_fixed_seq_of(
        bref, sl_beta_offsets2nd_sci_r16, sl_beta_offsets2nd_sci_r16.size(), integer_packer<uint8_t>(0, 31)));
  }
  if (sl_scaling_r16_present) {
    HANDLE_CODE(sl_scaling_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pssch_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_pssch_dmrs_time_pattern_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_pssch_dmrs_time_pattern_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_beta_offsets2nd_sci_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_scaling_r16_present, 1));

  if (sl_pssch_dmrs_time_pattern_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_pssch_dmrs_time_pattern_list_r16, bref, 1, 3, integer_packer<uint8_t>(2, 4)));
  }
  if (sl_beta_offsets2nd_sci_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(
        sl_beta_offsets2nd_sci_r16, bref, sl_beta_offsets2nd_sci_r16.size(), integer_packer<uint8_t>(0, 31)));
  }
  if (sl_scaling_r16_present) {
    HANDLE_CODE(sl_scaling_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_pssch_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_pssch_dmrs_time_pattern_list_r16.size() > 0) {
    j.start_array("sl-PSSCH-DMRS-TimePatternList-r16");
    for (const auto& e1 : sl_pssch_dmrs_time_pattern_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_beta_offsets2nd_sci_r16_present) {
    j.start_array("sl-BetaOffsets2ndSCI-r16");
    for (const auto& e1 : sl_beta_offsets2nd_sci_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_scaling_r16_present) {
    j.write_str("sl-Scaling-r16", sl_scaling_r16.to_string());
  }
  j.end_obj();
}

const char* sl_pssch_cfg_r16_s::sl_scaling_r16_opts::to_string() const
{
  static const char* names[] = {"f0p5", "f0p65", "f0p8", "f1"};
  return convert_enum_idx(names, 4, value, "sl_pssch_cfg_r16_s::sl_scaling_r16_e_");
}

// SL-PTRS-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_ptrs_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_ptrs_freq_density_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ptrs_time_density_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ptrs_re_offset_r16_present, 1));

  if (sl_ptrs_freq_density_r16_present) {
    HANDLE_CODE(pack_fixed_seq_of(
        bref, sl_ptrs_freq_density_r16, sl_ptrs_freq_density_r16.size(), integer_packer<uint16_t>(1, 276)));
  }
  if (sl_ptrs_time_density_r16_present) {
    HANDLE_CODE(pack_fixed_seq_of(
        bref, sl_ptrs_time_density_r16, sl_ptrs_time_density_r16.size(), integer_packer<uint8_t>(0, 29)));
  }
  if (sl_ptrs_re_offset_r16_present) {
    HANDLE_CODE(sl_ptrs_re_offset_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_ptrs_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_ptrs_freq_density_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ptrs_time_density_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ptrs_re_offset_r16_present, 1));

  if (sl_ptrs_freq_density_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(
        sl_ptrs_freq_density_r16, bref, sl_ptrs_freq_density_r16.size(), integer_packer<uint16_t>(1, 276)));
  }
  if (sl_ptrs_time_density_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(
        sl_ptrs_time_density_r16, bref, sl_ptrs_time_density_r16.size(), integer_packer<uint8_t>(0, 29)));
  }
  if (sl_ptrs_re_offset_r16_present) {
    HANDLE_CODE(sl_ptrs_re_offset_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_ptrs_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_ptrs_freq_density_r16_present) {
    j.start_array("sl-PTRS-FreqDensity-r16");
    for (const auto& e1 : sl_ptrs_freq_density_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_ptrs_time_density_r16_present) {
    j.start_array("sl-PTRS-TimeDensity-r16");
    for (const auto& e1 : sl_ptrs_time_density_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_ptrs_re_offset_r16_present) {
    j.write_str("sl-PTRS-RE-Offset-r16", sl_ptrs_re_offset_r16.to_string());
  }
  j.end_obj();
}

const char* sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_opts::to_string() const
{
  static const char* names[] = {"offset01", "offset10", "offset11"};
  return convert_enum_idx(names, 3, value, "sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_e_");
}
float sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_opts::to_number() const
{
  static const float numbers[] = {0.1, 1.0, 1.1};
  return map_enum_number(numbers, 3, value, "sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_e_");
}
const char* sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.1", "1.0", "1.1"};
  return convert_enum_idx(number_strs, 3, value, "sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_e_");
}

// SL-PowerControl-r16 ::= SEQUENCE
SRSASN_CODE sl_pwr_ctrl_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_alpha_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.pack(dl_alpha_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_p0_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.pack(dl_p0_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.pack(dl_alpha_psfch_r16_present, 1));
  HANDLE_CODE(bref.pack(dl_p0_psfch_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_max_trans_pwr_r16, (int8_t)-30, (int8_t)33));
  if (sl_alpha_pssch_pscch_r16_present) {
    HANDLE_CODE(sl_alpha_pssch_pscch_r16.pack(bref));
  }
  if (dl_alpha_pssch_pscch_r16_present) {
    HANDLE_CODE(dl_alpha_pssch_pscch_r16.pack(bref));
  }
  if (sl_p0_pssch_pscch_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_p0_pssch_pscch_r16, (int8_t)-16, (int8_t)15));
  }
  if (dl_p0_pssch_pscch_r16_present) {
    HANDLE_CODE(pack_integer(bref, dl_p0_pssch_pscch_r16, (int8_t)-16, (int8_t)15));
  }
  if (dl_alpha_psfch_r16_present) {
    HANDLE_CODE(dl_alpha_psfch_r16.pack(bref));
  }
  if (dl_p0_psfch_r16_present) {
    HANDLE_CODE(pack_integer(bref, dl_p0_psfch_r16, (int8_t)-16, (int8_t)15));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dl_p0_pssch_pscch_r17_present;
    group_flags[0] |= sl_p0_pssch_pscch_r17_present;
    group_flags[0] |= dl_p0_psfch_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_p0_pssch_pscch_r17_present, 1));
      HANDLE_CODE(bref.pack(sl_p0_pssch_pscch_r17_present, 1));
      HANDLE_CODE(bref.pack(dl_p0_psfch_r17_present, 1));
      if (dl_p0_pssch_pscch_r17_present) {
        HANDLE_CODE(pack_integer(bref, dl_p0_pssch_pscch_r17, (int16_t)-202, (int16_t)24));
      }
      if (sl_p0_pssch_pscch_r17_present) {
        HANDLE_CODE(pack_integer(bref, sl_p0_pssch_pscch_r17, (int16_t)-202, (int16_t)24));
      }
      if (dl_p0_psfch_r17_present) {
        HANDLE_CODE(pack_integer(bref, dl_p0_psfch_r17, (int16_t)-202, (int16_t)24));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pwr_ctrl_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_alpha_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.unpack(dl_alpha_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_p0_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.unpack(dl_p0_pssch_pscch_r16_present, 1));
  HANDLE_CODE(bref.unpack(dl_alpha_psfch_r16_present, 1));
  HANDLE_CODE(bref.unpack(dl_p0_psfch_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_max_trans_pwr_r16, bref, (int8_t)-30, (int8_t)33));
  if (sl_alpha_pssch_pscch_r16_present) {
    HANDLE_CODE(sl_alpha_pssch_pscch_r16.unpack(bref));
  }
  if (dl_alpha_pssch_pscch_r16_present) {
    HANDLE_CODE(dl_alpha_pssch_pscch_r16.unpack(bref));
  }
  if (sl_p0_pssch_pscch_r16_present) {
    HANDLE_CODE(unpack_integer(sl_p0_pssch_pscch_r16, bref, (int8_t)-16, (int8_t)15));
  }
  if (dl_p0_pssch_pscch_r16_present) {
    HANDLE_CODE(unpack_integer(dl_p0_pssch_pscch_r16, bref, (int8_t)-16, (int8_t)15));
  }
  if (dl_alpha_psfch_r16_present) {
    HANDLE_CODE(dl_alpha_psfch_r16.unpack(bref));
  }
  if (dl_p0_psfch_r16_present) {
    HANDLE_CODE(unpack_integer(dl_p0_psfch_r16, bref, (int8_t)-16, (int8_t)15));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dl_p0_pssch_pscch_r17_present, 1));
      HANDLE_CODE(bref.unpack(sl_p0_pssch_pscch_r17_present, 1));
      HANDLE_CODE(bref.unpack(dl_p0_psfch_r17_present, 1));
      if (dl_p0_pssch_pscch_r17_present) {
        HANDLE_CODE(unpack_integer(dl_p0_pssch_pscch_r17, bref, (int16_t)-202, (int16_t)24));
      }
      if (sl_p0_pssch_pscch_r17_present) {
        HANDLE_CODE(unpack_integer(sl_p0_pssch_pscch_r17, bref, (int16_t)-202, (int16_t)24));
      }
      if (dl_p0_psfch_r17_present) {
        HANDLE_CODE(unpack_integer(dl_p0_psfch_r17, bref, (int16_t)-202, (int16_t)24));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_pwr_ctrl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-MaxTransPower-r16", sl_max_trans_pwr_r16);
  if (sl_alpha_pssch_pscch_r16_present) {
    j.write_str("sl-Alpha-PSSCH-PSCCH-r16", sl_alpha_pssch_pscch_r16.to_string());
  }
  if (dl_alpha_pssch_pscch_r16_present) {
    j.write_str("dl-Alpha-PSSCH-PSCCH-r16", dl_alpha_pssch_pscch_r16.to_string());
  }
  if (sl_p0_pssch_pscch_r16_present) {
    j.write_int("sl-P0-PSSCH-PSCCH-r16", sl_p0_pssch_pscch_r16);
  }
  if (dl_p0_pssch_pscch_r16_present) {
    j.write_int("dl-P0-PSSCH-PSCCH-r16", dl_p0_pssch_pscch_r16);
  }
  if (dl_alpha_psfch_r16_present) {
    j.write_str("dl-Alpha-PSFCH-r16", dl_alpha_psfch_r16.to_string());
  }
  if (dl_p0_psfch_r16_present) {
    j.write_int("dl-P0-PSFCH-r16", dl_p0_psfch_r16);
  }
  if (ext) {
    if (dl_p0_pssch_pscch_r17_present) {
      j.write_int("dl-P0-PSSCH-PSCCH-r17", dl_p0_pssch_pscch_r17);
    }
    if (sl_p0_pssch_pscch_r17_present) {
      j.write_int("sl-P0-PSSCH-PSCCH-r17", sl_p0_pssch_pscch_r17);
    }
    if (dl_p0_psfch_r17_present) {
      j.write_int("dl-P0-PSFCH-r17", dl_p0_psfch_r17);
    }
  }
  j.end_obj();
}

const char* sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_opts::to_string() const
{
  static const char* names[] = {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
  return convert_enum_idx(names, 8, value, "sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_e_");
}
float sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_e_");
}
const char* sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_e_");
}

const char* sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_opts::to_string() const
{
  static const char* names[] = {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
  return convert_enum_idx(names, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_e_");
}
float sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_e_");
}
const char* sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_e_");
}

const char* sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_opts::to_string() const
{
  static const char* names[] = {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
  return convert_enum_idx(names, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_e_");
}
float sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_e_");
}
const char* sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_e_");
}

// SL-SyncAllowed-r16 ::= SEQUENCE
SRSASN_CODE sl_sync_allowed_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gnss_sync_r16_present, 1));
  HANDLE_CODE(bref.pack(gnb_enb_sync_r16_present, 1));
  HANDLE_CODE(bref.pack(ue_sync_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sync_allowed_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gnss_sync_r16_present, 1));
  HANDLE_CODE(bref.unpack(gnb_enb_sync_r16_present, 1));
  HANDLE_CODE(bref.unpack(ue_sync_r16_present, 1));

  return SRSASN_SUCCESS;
}
void sl_sync_allowed_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnss_sync_r16_present) {
    j.write_str("gnss-Sync-r16", "true");
  }
  if (gnb_enb_sync_r16_present) {
    j.write_str("gnbEnb-Sync-r16", "true");
  }
  if (ue_sync_r16_present) {
    j.write_str("ue-Sync-r16", "true");
  }
  j.end_obj();
}

// SL-UE-SelectedConfigRP-r16 ::= SEQUENCE
SRSASN_CODE sl_ue_sel_cfg_rp_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_cbr_prio_tx_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_thres_rsrp_list_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_multi_reserve_res_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_max_num_per_reserve_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_sensing_win_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_sel_win_list_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_res_reserve_period_list_r16.size() > 0, 1));

  if (sl_cbr_prio_tx_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_cbr_prio_tx_cfg_list_r16, 1, 8));
  }
  if (sl_thres_rsrp_list_r16_present) {
    HANDLE_CODE(
        pack_fixed_seq_of(bref, sl_thres_rsrp_list_r16, sl_thres_rsrp_list_r16.size(), integer_packer<uint8_t>(0, 66)));
  }
  if (sl_max_num_per_reserve_r16_present) {
    HANDLE_CODE(sl_max_num_per_reserve_r16.pack(bref));
  }
  if (sl_sensing_win_r16_present) {
    HANDLE_CODE(sl_sensing_win_r16.pack(bref));
  }
  if (sl_sel_win_list_r16_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, sl_sel_win_list_r16, sl_sel_win_list_r16.size()));
  }
  if (sl_res_reserve_period_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_res_reserve_period_list_r16, 1, 16));
  }
  HANDLE_CODE(sl_rs_for_sensing_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_cbr_prio_tx_cfg_list_v1650.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_cbr_prio_tx_cfg_list_v1650.is_present(), 1));
      if (sl_cbr_prio_tx_cfg_list_v1650.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sl_cbr_prio_tx_cfg_list_v1650, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_ue_sel_cfg_rp_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_cbr_prio_tx_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_cbr_prio_tx_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_thres_rsrp_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_multi_reserve_res_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_num_per_reserve_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_sensing_win_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_sel_win_list_r16_present, 1));
  bool sl_res_reserve_period_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_res_reserve_period_list_r16_present, 1));

  if (sl_cbr_prio_tx_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_cbr_prio_tx_cfg_list_r16, bref, 1, 8));
  }
  if (sl_thres_rsrp_list_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(
        sl_thres_rsrp_list_r16, bref, sl_thres_rsrp_list_r16.size(), integer_packer<uint8_t>(0, 66)));
  }
  if (sl_max_num_per_reserve_r16_present) {
    HANDLE_CODE(sl_max_num_per_reserve_r16.unpack(bref));
  }
  if (sl_sensing_win_r16_present) {
    HANDLE_CODE(sl_sensing_win_r16.unpack(bref));
  }
  if (sl_sel_win_list_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(sl_sel_win_list_r16, bref, sl_sel_win_list_r16.size()));
  }
  if (sl_res_reserve_period_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_res_reserve_period_list_r16, bref, 1, 16));
  }
  HANDLE_CODE(sl_rs_for_sensing_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_cbr_prio_tx_cfg_list_v1650, bref);
      if (sl_cbr_prio_tx_cfg_list_v1650.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sl_cbr_prio_tx_cfg_list_v1650, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_ue_sel_cfg_rp_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_cbr_prio_tx_cfg_list_r16.size() > 0) {
    j.start_array("sl-CBR-PriorityTxConfigList-r16");
    for (const auto& e1 : sl_cbr_prio_tx_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_thres_rsrp_list_r16_present) {
    j.start_array("sl-Thres-RSRP-List-r16");
    for (const auto& e1 : sl_thres_rsrp_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_multi_reserve_res_r16_present) {
    j.write_str("sl-MultiReserveResource-r16", "enabled");
  }
  if (sl_max_num_per_reserve_r16_present) {
    j.write_str("sl-MaxNumPerReserve-r16", sl_max_num_per_reserve_r16.to_string());
  }
  if (sl_sensing_win_r16_present) {
    j.write_str("sl-SensingWindow-r16", sl_sensing_win_r16.to_string());
  }
  if (sl_sel_win_list_r16_present) {
    j.start_array("sl-SelectionWindowList-r16");
    for (const auto& e1 : sl_sel_win_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_res_reserve_period_list_r16.size() > 0) {
    j.start_array("sl-ResourceReservePeriodList-r16");
    for (const auto& e1 : sl_res_reserve_period_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("sl-RS-ForSensing-r16", sl_rs_for_sensing_r16.to_string());
  if (ext) {
    if (sl_cbr_prio_tx_cfg_list_v1650.is_present()) {
      j.start_array("sl-CBR-PriorityTxConfigList-v1650");
      for (const auto& e1 : *sl_cbr_prio_tx_cfg_list_v1650) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_opts::to_string() const
{
  static const char* names[] = {"n2", "n3"};
  return convert_enum_idx(names, 2, value, "sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_e_");
}
uint8_t sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_e_");
}

const char* sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms1100"};
  return convert_enum_idx(names, 2, value, "sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_e_");
}
uint16_t sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 1100};
  return map_enum_number(numbers, 2, value, "sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_e_");
}

const char* sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16_opts::to_string() const
{
  static const char* names[] = {"pscch", "pssch"};
  return convert_enum_idx(names, 2, value, "sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16_e_");
}

// SL-ZoneConfigMCR-r16 ::= SEQUENCE
SRSASN_CODE sl_zone_cfg_mcr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_trans_range_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_zone_cfg_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_zone_cfg_mcr_idx_r16, (uint8_t)0u, (uint8_t)15u));
  if (sl_trans_range_r16_present) {
    HANDLE_CODE(sl_trans_range_r16.pack(bref));
  }
  if (sl_zone_cfg_r16_present) {
    HANDLE_CODE(sl_zone_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_zone_cfg_mcr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_trans_range_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_zone_cfg_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_zone_cfg_mcr_idx_r16, bref, (uint8_t)0u, (uint8_t)15u));
  if (sl_trans_range_r16_present) {
    HANDLE_CODE(sl_trans_range_r16.unpack(bref));
  }
  if (sl_zone_cfg_r16_present) {
    HANDLE_CODE(sl_zone_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_zone_cfg_mcr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-ZoneConfigMCR-Index-r16", sl_zone_cfg_mcr_idx_r16);
  if (sl_trans_range_r16_present) {
    j.write_str("sl-TransRange-r16", sl_trans_range_r16.to_string());
  }
  if (sl_zone_cfg_r16_present) {
    j.write_fieldname("sl-ZoneConfig-r16");
    sl_zone_cfg_r16.to_json(j);
  }
  j.end_obj();
}

const char* sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_opts::to_string() const
{
  static const char* names[] = {"m20",    "m50",    "m80",    "m100",   "m120",   "m150",   "m180",   "m200",
                                "m220",   "m250",   "m270",   "m300",   "m350",   "m370",   "m400",   "m420",
                                "m450",   "m480",   "m500",   "m550",   "m600",   "m700",   "m1000",  "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_e_");
}
uint16_t sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {20,  50,  80,  100, 120, 150, 180, 200, 220, 250, 270, 300,
                                     350, 370, 400, 420, 450, 480, 500, 550, 600, 700, 1000};
  return map_enum_number(numbers, 23, value, "sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_e_");
}

// ReportInterval ::= ENUMERATED
const char* report_interv_opts::to_string() const
{
  static const char* names[] = {"ms120",
                                "ms240",
                                "ms480",
                                "ms640",
                                "ms1024",
                                "ms2048",
                                "ms5120",
                                "ms10240",
                                "ms20480",
                                "ms40960",
                                "min1",
                                "min6",
                                "min12",
                                "min30"};
  return convert_enum_idx(names, 14, value, "report_interv_e");
}
uint16_t report_interv_opts::to_number() const
{
  static const uint16_t numbers[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 1, 6, 12, 30};
  return map_enum_number(numbers, 14, value, "report_interv_e");
}

// SL-MeasReportQuantity-r16 ::= CHOICE
void sl_meas_report_quant_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("sl-RSRP-r16", c);
  j.end_obj();
}
SRSASN_CODE sl_meas_report_quant_r16_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(bref.pack(c, 1));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_report_quant_r16_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "sl_meas_report_quant_r16_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(bref.unpack(c, 1));
  return SRSASN_SUCCESS;
}

// SL-MeasTriggerQuantity-r16 ::= CHOICE
void sl_meas_trigger_quant_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-RSRP-r16", c);
  j.end_obj();
}
SRSASN_CODE sl_meas_trigger_quant_r16_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(pack_integer(bref, c, (uint8_t)0u, (uint8_t)127u));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_trigger_quant_r16_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "sl_meas_trigger_quant_r16_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(unpack_integer(c, bref, (uint8_t)0u, (uint8_t)127u));
  return SRSASN_SUCCESS;
}

// SL-RS-Type-r16 ::= ENUMERATED
const char* sl_rs_type_r16_opts::to_string() const
{
  static const char* names[] = {"dmrs", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "sl_rs_type_r16_e");
}

// SL-ResourcePool-r16 ::= SEQUENCE
SRSASN_CODE sl_res_pool_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_pscch_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_pssch_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_psfch_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_sync_allowed_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_subch_size_r16_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(sl_start_rb_subch_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_num_subch_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_add_mcs_table_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_thresh_s_rssi_cbr_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_time_win_size_cbr_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_time_win_size_cr_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ptrs_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ue_sel_cfg_rp_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_rx_params_ncell_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_zone_cfg_mcr_list_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_filt_coef_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_rb_num_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_preemption_enable_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_prio_thres_ul_urllc_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_prio_thres_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_x_overhead_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_pwr_ctrl_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_tx_percentage_list_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_min_max_mcs_list_r16.size() > 0, 1));

  if (sl_pscch_cfg_r16_present) {
    HANDLE_CODE(sl_pscch_cfg_r16.pack(bref));
  }
  if (sl_pssch_cfg_r16_present) {
    HANDLE_CODE(sl_pssch_cfg_r16.pack(bref));
  }
  if (sl_psfch_cfg_r16_present) {
    HANDLE_CODE(sl_psfch_cfg_r16.pack(bref));
  }
  if (sl_sync_allowed_r16_present) {
    HANDLE_CODE(sl_sync_allowed_r16.pack(bref));
  }
  if (sl_subch_size_r16_present) {
    HANDLE_CODE(sl_subch_size_r16.pack(bref));
  }
  if (dummy_present) {
    HANDLE_CODE(pack_integer(bref, dummy, (uint8_t)10u, (uint8_t)160u));
  }
  if (sl_start_rb_subch_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_start_rb_subch_r16, (uint16_t)0u, (uint16_t)265u));
  }
  if (sl_num_subch_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_num_subch_r16, (uint8_t)1u, (uint8_t)27u));
  }
  if (sl_add_mcs_table_r16_present) {
    HANDLE_CODE(sl_add_mcs_table_r16.pack(bref));
  }
  if (sl_thresh_s_rssi_cbr_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_thresh_s_rssi_cbr_r16, (uint8_t)0u, (uint8_t)45u));
  }
  if (sl_time_win_size_cbr_r16_present) {
    HANDLE_CODE(sl_time_win_size_cbr_r16.pack(bref));
  }
  if (sl_time_win_size_cr_r16_present) {
    HANDLE_CODE(sl_time_win_size_cr_r16.pack(bref));
  }
  if (sl_ptrs_cfg_r16_present) {
    HANDLE_CODE(sl_ptrs_cfg_r16.pack(bref));
  }
  if (sl_ue_sel_cfg_rp_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_rp_r16.pack(bref));
  }
  if (sl_rx_params_ncell_r16_present) {
    HANDLE_CODE(bref.pack(sl_rx_params_ncell_r16.sl_tdd_cfg_r16_present, 1));
    if (sl_rx_params_ncell_r16.sl_tdd_cfg_r16_present) {
      HANDLE_CODE(sl_rx_params_ncell_r16.sl_tdd_cfg_r16.pack(bref));
    }
    HANDLE_CODE(pack_integer(bref, sl_rx_params_ncell_r16.sl_sync_cfg_idx_r16, (uint8_t)0u, (uint8_t)15u));
  }
  if (sl_zone_cfg_mcr_list_r16_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, sl_zone_cfg_mcr_list_r16, sl_zone_cfg_mcr_list_r16.size()));
  }
  if (sl_filt_coef_r16_present) {
    HANDLE_CODE(sl_filt_coef_r16.pack(bref));
  }
  if (sl_rb_num_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_rb_num_r16, (uint16_t)10u, (uint16_t)275u));
  }
  if (sl_preemption_enable_r16_present) {
    HANDLE_CODE(sl_preemption_enable_r16.pack(bref));
  }
  if (sl_prio_thres_ul_urllc_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_prio_thres_ul_urllc_r16, (uint8_t)1u, (uint8_t)9u));
  }
  if (sl_prio_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_prio_thres_r16, (uint8_t)1u, (uint8_t)9u));
  }
  if (sl_x_overhead_r16_present) {
    HANDLE_CODE(sl_x_overhead_r16.pack(bref));
  }
  if (sl_pwr_ctrl_r16_present) {
    HANDLE_CODE(sl_pwr_ctrl_r16.pack(bref));
  }
  if (sl_tx_percentage_list_r16_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, sl_tx_percentage_list_r16, sl_tx_percentage_list_r16.size()));
  }
  if (sl_min_max_mcs_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_min_max_mcs_list_r16, 1, 3));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_time_res_r16_present;
    group_flags[1] |= sl_pbps_cp_s_cfg_r17.is_present();
    group_flags[1] |= sl_inter_ue_coordination_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_time_res_r16_present, 1));
      if (sl_time_res_r16_present) {
        HANDLE_CODE(sl_time_res_r16.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_pbps_cp_s_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_inter_ue_coordination_cfg_r17.is_present(), 1));
      if (sl_pbps_cp_s_cfg_r17.is_present()) {
        HANDLE_CODE(sl_pbps_cp_s_cfg_r17->pack(bref));
      }
      if (sl_inter_ue_coordination_cfg_r17.is_present()) {
        HANDLE_CODE(sl_inter_ue_coordination_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_res_pool_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_pscch_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_pssch_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_psfch_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_sync_allowed_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_subch_size_r16_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(sl_start_rb_subch_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_num_subch_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_add_mcs_table_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_thresh_s_rssi_cbr_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_time_win_size_cbr_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_time_win_size_cr_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ptrs_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ue_sel_cfg_rp_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_rx_params_ncell_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_zone_cfg_mcr_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_filt_coef_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_rb_num_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_preemption_enable_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_prio_thres_ul_urllc_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_prio_thres_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_x_overhead_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_pwr_ctrl_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_percentage_list_r16_present, 1));
  bool sl_min_max_mcs_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_min_max_mcs_list_r16_present, 1));

  if (sl_pscch_cfg_r16_present) {
    HANDLE_CODE(sl_pscch_cfg_r16.unpack(bref));
  }
  if (sl_pssch_cfg_r16_present) {
    HANDLE_CODE(sl_pssch_cfg_r16.unpack(bref));
  }
  if (sl_psfch_cfg_r16_present) {
    HANDLE_CODE(sl_psfch_cfg_r16.unpack(bref));
  }
  if (sl_sync_allowed_r16_present) {
    HANDLE_CODE(sl_sync_allowed_r16.unpack(bref));
  }
  if (sl_subch_size_r16_present) {
    HANDLE_CODE(sl_subch_size_r16.unpack(bref));
  }
  if (dummy_present) {
    HANDLE_CODE(unpack_integer(dummy, bref, (uint8_t)10u, (uint8_t)160u));
  }
  if (sl_start_rb_subch_r16_present) {
    HANDLE_CODE(unpack_integer(sl_start_rb_subch_r16, bref, (uint16_t)0u, (uint16_t)265u));
  }
  if (sl_num_subch_r16_present) {
    HANDLE_CODE(unpack_integer(sl_num_subch_r16, bref, (uint8_t)1u, (uint8_t)27u));
  }
  if (sl_add_mcs_table_r16_present) {
    HANDLE_CODE(sl_add_mcs_table_r16.unpack(bref));
  }
  if (sl_thresh_s_rssi_cbr_r16_present) {
    HANDLE_CODE(unpack_integer(sl_thresh_s_rssi_cbr_r16, bref, (uint8_t)0u, (uint8_t)45u));
  }
  if (sl_time_win_size_cbr_r16_present) {
    HANDLE_CODE(sl_time_win_size_cbr_r16.unpack(bref));
  }
  if (sl_time_win_size_cr_r16_present) {
    HANDLE_CODE(sl_time_win_size_cr_r16.unpack(bref));
  }
  if (sl_ptrs_cfg_r16_present) {
    HANDLE_CODE(sl_ptrs_cfg_r16.unpack(bref));
  }
  if (sl_ue_sel_cfg_rp_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_rp_r16.unpack(bref));
  }
  if (sl_rx_params_ncell_r16_present) {
    HANDLE_CODE(bref.unpack(sl_rx_params_ncell_r16.sl_tdd_cfg_r16_present, 1));
    if (sl_rx_params_ncell_r16.sl_tdd_cfg_r16_present) {
      HANDLE_CODE(sl_rx_params_ncell_r16.sl_tdd_cfg_r16.unpack(bref));
    }
    HANDLE_CODE(unpack_integer(sl_rx_params_ncell_r16.sl_sync_cfg_idx_r16, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (sl_zone_cfg_mcr_list_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(sl_zone_cfg_mcr_list_r16, bref, sl_zone_cfg_mcr_list_r16.size()));
  }
  if (sl_filt_coef_r16_present) {
    HANDLE_CODE(sl_filt_coef_r16.unpack(bref));
  }
  if (sl_rb_num_r16_present) {
    HANDLE_CODE(unpack_integer(sl_rb_num_r16, bref, (uint16_t)10u, (uint16_t)275u));
  }
  if (sl_preemption_enable_r16_present) {
    HANDLE_CODE(sl_preemption_enable_r16.unpack(bref));
  }
  if (sl_prio_thres_ul_urllc_r16_present) {
    HANDLE_CODE(unpack_integer(sl_prio_thres_ul_urllc_r16, bref, (uint8_t)1u, (uint8_t)9u));
  }
  if (sl_prio_thres_r16_present) {
    HANDLE_CODE(unpack_integer(sl_prio_thres_r16, bref, (uint8_t)1u, (uint8_t)9u));
  }
  if (sl_x_overhead_r16_present) {
    HANDLE_CODE(sl_x_overhead_r16.unpack(bref));
  }
  if (sl_pwr_ctrl_r16_present) {
    HANDLE_CODE(sl_pwr_ctrl_r16.unpack(bref));
  }
  if (sl_tx_percentage_list_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(sl_tx_percentage_list_r16, bref, sl_tx_percentage_list_r16.size()));
  }
  if (sl_min_max_mcs_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_min_max_mcs_list_r16, bref, 1, 3));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sl_time_res_r16_present, 1));
      if (sl_time_res_r16_present) {
        HANDLE_CODE(sl_time_res_r16.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_pbps_cp_s_cfg_r17, bref);
      unpack_presence_flag(sl_inter_ue_coordination_cfg_r17, bref);
      if (sl_pbps_cp_s_cfg_r17.is_present()) {
        HANDLE_CODE(sl_pbps_cp_s_cfg_r17->unpack(bref));
      }
      if (sl_inter_ue_coordination_cfg_r17.is_present()) {
        HANDLE_CODE(sl_inter_ue_coordination_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_res_pool_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_pscch_cfg_r16_present) {
    j.write_fieldname("sl-PSCCH-Config-r16");
    sl_pscch_cfg_r16.to_json(j);
  }
  if (sl_pssch_cfg_r16_present) {
    j.write_fieldname("sl-PSSCH-Config-r16");
    sl_pssch_cfg_r16.to_json(j);
  }
  if (sl_psfch_cfg_r16_present) {
    j.write_fieldname("sl-PSFCH-Config-r16");
    sl_psfch_cfg_r16.to_json(j);
  }
  if (sl_sync_allowed_r16_present) {
    j.write_fieldname("sl-SyncAllowed-r16");
    sl_sync_allowed_r16.to_json(j);
  }
  if (sl_subch_size_r16_present) {
    j.write_str("sl-SubchannelSize-r16", sl_subch_size_r16.to_string());
  }
  if (dummy_present) {
    j.write_int("dummy", dummy);
  }
  if (sl_start_rb_subch_r16_present) {
    j.write_int("sl-StartRB-Subchannel-r16", sl_start_rb_subch_r16);
  }
  if (sl_num_subch_r16_present) {
    j.write_int("sl-NumSubchannel-r16", sl_num_subch_r16);
  }
  if (sl_add_mcs_table_r16_present) {
    j.write_str("sl-Additional-MCS-Table-r16", sl_add_mcs_table_r16.to_string());
  }
  if (sl_thresh_s_rssi_cbr_r16_present) {
    j.write_int("sl-ThreshS-RSSI-CBR-r16", sl_thresh_s_rssi_cbr_r16);
  }
  if (sl_time_win_size_cbr_r16_present) {
    j.write_str("sl-TimeWindowSizeCBR-r16", sl_time_win_size_cbr_r16.to_string());
  }
  if (sl_time_win_size_cr_r16_present) {
    j.write_str("sl-TimeWindowSizeCR-r16", sl_time_win_size_cr_r16.to_string());
  }
  if (sl_ptrs_cfg_r16_present) {
    j.write_fieldname("sl-PTRS-Config-r16");
    sl_ptrs_cfg_r16.to_json(j);
  }
  if (sl_ue_sel_cfg_rp_r16_present) {
    j.write_fieldname("sl-UE-SelectedConfigRP-r16");
    sl_ue_sel_cfg_rp_r16.to_json(j);
  }
  if (sl_rx_params_ncell_r16_present) {
    j.write_fieldname("sl-RxParametersNcell-r16");
    j.start_obj();
    if (sl_rx_params_ncell_r16.sl_tdd_cfg_r16_present) {
      j.write_fieldname("sl-TDD-Configuration-r16");
      sl_rx_params_ncell_r16.sl_tdd_cfg_r16.to_json(j);
    }
    j.write_int("sl-SyncConfigIndex-r16", sl_rx_params_ncell_r16.sl_sync_cfg_idx_r16);
    j.end_obj();
  }
  if (sl_zone_cfg_mcr_list_r16_present) {
    j.start_array("sl-ZoneConfigMCR-List-r16");
    for (const auto& e1 : sl_zone_cfg_mcr_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_filt_coef_r16_present) {
    j.write_str("sl-FilterCoefficient-r16", sl_filt_coef_r16.to_string());
  }
  if (sl_rb_num_r16_present) {
    j.write_int("sl-RB-Number-r16", sl_rb_num_r16);
  }
  if (sl_preemption_enable_r16_present) {
    j.write_str("sl-PreemptionEnable-r16", sl_preemption_enable_r16.to_string());
  }
  if (sl_prio_thres_ul_urllc_r16_present) {
    j.write_int("sl-PriorityThreshold-UL-URLLC-r16", sl_prio_thres_ul_urllc_r16);
  }
  if (sl_prio_thres_r16_present) {
    j.write_int("sl-PriorityThreshold-r16", sl_prio_thres_r16);
  }
  if (sl_x_overhead_r16_present) {
    j.write_str("sl-X-Overhead-r16", sl_x_overhead_r16.to_string());
  }
  if (sl_pwr_ctrl_r16_present) {
    j.write_fieldname("sl-PowerControl-r16");
    sl_pwr_ctrl_r16.to_json(j);
  }
  if (sl_tx_percentage_list_r16_present) {
    j.start_array("sl-TxPercentageList-r16");
    for (const auto& e1 : sl_tx_percentage_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_min_max_mcs_list_r16.size() > 0) {
    j.start_array("sl-MinMaxMCS-List-r16");
    for (const auto& e1 : sl_min_max_mcs_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (sl_time_res_r16_present) {
      j.write_str("sl-TimeResource-r16", sl_time_res_r16.to_string());
    }
    if (sl_pbps_cp_s_cfg_r17.is_present()) {
      j.write_fieldname("sl-PBPS-CPS-Config-r17");
      sl_pbps_cp_s_cfg_r17->to_json(j);
    }
    if (sl_inter_ue_coordination_cfg_r17.is_present()) {
      j.write_fieldname("sl-InterUE-CoordinationConfig-r17");
      sl_inter_ue_coordination_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* sl_res_pool_r16_s::sl_subch_size_r16_opts::to_string() const
{
  static const char* names[] = {"n10", "n12", "n15", "n20", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(names, 8, value, "sl_res_pool_r16_s::sl_subch_size_r16_e_");
}
uint8_t sl_res_pool_r16_s::sl_subch_size_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 12, 15, 20, 25, 50, 75, 100};
  return map_enum_number(numbers, 8, value, "sl_res_pool_r16_s::sl_subch_size_r16_e_");
}

const char* sl_res_pool_r16_s::sl_add_mcs_table_r16_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE", "qam256-qam64LowSE"};
  return convert_enum_idx(names, 3, value, "sl_res_pool_r16_s::sl_add_mcs_table_r16_e_");
}

const char* sl_res_pool_r16_s::sl_time_win_size_cbr_r16_opts::to_string() const
{
  static const char* names[] = {"ms100", "slot100"};
  return convert_enum_idx(names, 2, value, "sl_res_pool_r16_s::sl_time_win_size_cbr_r16_e_");
}

const char* sl_res_pool_r16_s::sl_time_win_size_cr_r16_opts::to_string() const
{
  static const char* names[] = {"ms1000", "slot1000"};
  return convert_enum_idx(names, 2, value, "sl_res_pool_r16_s::sl_time_win_size_cr_r16_e_");
}

const char* sl_res_pool_r16_s::sl_preemption_enable_r16_opts::to_string() const
{
  static const char* names[] = {"enabled", "pl1", "pl2", "pl3", "pl4", "pl5", "pl6", "pl7", "pl8"};
  return convert_enum_idx(names, 9, value, "sl_res_pool_r16_s::sl_preemption_enable_r16_e_");
}
uint8_t sl_res_pool_r16_s::sl_preemption_enable_r16_opts::to_number() const
{
  switch (value) {
    case pl1:
      return 1;
    case pl2:
      return 2;
    case pl3:
      return 3;
    case pl4:
      return 4;
    case pl5:
      return 5;
    case pl6:
      return 6;
    case pl7:
      return 7;
    case pl8:
      return 8;
    default:
      invalid_enum_number(value, "sl_res_pool_r16_s::sl_preemption_enable_r16_e_");
  }
  return 0;
}

const char* sl_res_pool_r16_s::sl_x_overhead_r16_opts::to_string() const
{
  static const char* names[] = {"n0", "n3", "n6", "n9"};
  return convert_enum_idx(names, 4, value, "sl_res_pool_r16_s::sl_x_overhead_r16_e_");
}
uint8_t sl_res_pool_r16_s::sl_x_overhead_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 3, 6, 9};
  return map_enum_number(numbers, 4, value, "sl_res_pool_r16_s::sl_x_overhead_r16_e_");
}

// TimeToTrigger ::= ENUMERATED
const char* time_to_trigger_opts::to_string() const
{
  static const char* names[] = {"ms0",
                                "ms40",
                                "ms64",
                                "ms80",
                                "ms100",
                                "ms128",
                                "ms160",
                                "ms256",
                                "ms320",
                                "ms480",
                                "ms512",
                                "ms640",
                                "ms1024",
                                "ms1280",
                                "ms2560",
                                "ms5120"};
  return convert_enum_idx(names, 16, value, "time_to_trigger_e");
}
uint16_t time_to_trigger_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};
  return map_enum_number(numbers, 16, value, "time_to_trigger_e");
}

// SL-CG-MaxTransNum-r16 ::= SEQUENCE
SRSASN_CODE sl_cg_max_trans_num_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sl_prio_r16, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, sl_max_trans_num_r16, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cg_max_trans_num_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sl_prio_r16, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(sl_max_trans_num_r16, bref, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
void sl_cg_max_trans_num_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-Priority-r16", sl_prio_r16);
  j.write_int("sl-MaxTransNum-r16", sl_max_trans_num_r16);
  j.end_obj();
}

// SL-EventTriggerConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_event_trigger_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sl_event_id_r16.pack(bref));
  HANDLE_CODE(sl_report_interv_r16.pack(bref));
  HANDLE_CODE(sl_report_amount_r16.pack(bref));
  HANDLE_CODE(sl_report_quant_r16.pack(bref));
  HANDLE_CODE(sl_rs_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_event_trigger_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sl_event_id_r16.unpack(bref));
  HANDLE_CODE(sl_report_interv_r16.unpack(bref));
  HANDLE_CODE(sl_report_amount_r16.unpack(bref));
  HANDLE_CODE(sl_report_quant_r16.unpack(bref));
  HANDLE_CODE(sl_rs_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_event_trigger_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-EventId-r16");
  sl_event_id_r16.to_json(j);
  j.write_str("sl-ReportInterval-r16", sl_report_interv_r16.to_string());
  j.write_str("sl-ReportAmount-r16", sl_report_amount_r16.to_string());
  j.write_fieldname("sl-ReportQuantity-r16");
  sl_report_quant_r16.to_json(j);
  j.write_str("sl-RS-Type-r16", sl_rs_type_r16.to_string());
  j.end_obj();
}

void sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::event_s1_r16:
      c.destroy<event_s1_r16_s_>();
      break;
    case types::event_s2_r16:
      c.destroy<event_s2_r16_s_>();
      break;
    default:
      break;
  }
}
void sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event_s1_r16:
      c.init<event_s1_r16_s_>();
      break;
    case types::event_s2_r16:
      c.init<event_s2_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_");
  }
}
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::sl_event_id_r16_c_(
    const sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event_s1_r16:
      c.init(other.c.get<event_s1_r16_s_>());
      break;
    case types::event_s2_r16:
      c.init(other.c.get<event_s2_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_");
  }
}
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_&
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::operator=(const sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event_s1_r16:
      c.set(other.c.get<event_s1_r16_s_>());
      break;
    case types::event_s2_r16:
      c.set(other.c.get<event_s2_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_");
  }

  return *this;
}
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_&
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::set_event_s1_r16()
{
  set(types::event_s1_r16);
  return c.get<event_s1_r16_s_>();
}
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_&
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::set_event_s2_r16()
{
  set(types::event_s2_r16);
  return c.get<event_s2_r16_s_>();
}
void sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event_s1_r16:
      j.write_fieldname("eventS1-r16");
      j.start_obj();
      j.write_fieldname("s1-Threshold-r16");
      c.get<event_s1_r16_s_>().s1_thres_r16.to_json(j);
      j.write_bool("sl-ReportOnLeave-r16", c.get<event_s1_r16_s_>().sl_report_on_leave_r16);
      j.write_int("sl-Hysteresis-r16", c.get<event_s1_r16_s_>().sl_hysteresis_r16);
      j.write_str("sl-TimeToTrigger-r16", c.get<event_s1_r16_s_>().sl_time_to_trigger_r16.to_string());
      j.end_obj();
      break;
    case types::event_s2_r16:
      j.write_fieldname("eventS2-r16");
      j.start_obj();
      j.write_fieldname("s2-Threshold-r16");
      c.get<event_s2_r16_s_>().s2_thres_r16.to_json(j);
      j.write_bool("sl-ReportOnLeave-r16", c.get<event_s2_r16_s_>().sl_report_on_leave_r16);
      j.write_int("sl-Hysteresis-r16", c.get<event_s2_r16_s_>().sl_hysteresis_r16);
      j.write_str("sl-TimeToTrigger-r16", c.get<event_s2_r16_s_>().sl_time_to_trigger_r16.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event_s1_r16:
      bref.pack(c.get<event_s1_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_s1_r16_s_>().s1_thres_r16.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_s1_r16_s_>().sl_report_on_leave_r16, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_s1_r16_s_>().sl_hysteresis_r16, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_s1_r16_s_>().sl_time_to_trigger_r16.pack(bref));
      break;
    case types::event_s2_r16:
      bref.pack(c.get<event_s2_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_s2_r16_s_>().s2_thres_r16.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_s2_r16_s_>().sl_report_on_leave_r16, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_s2_r16_s_>().sl_hysteresis_r16, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_s2_r16_s_>().sl_time_to_trigger_r16.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event_s1_r16:
      bref.unpack(c.get<event_s1_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_s1_r16_s_>().s1_thres_r16.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_s1_r16_s_>().sl_report_on_leave_r16, 1));
      HANDLE_CODE(unpack_integer(c.get<event_s1_r16_s_>().sl_hysteresis_r16, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_s1_r16_s_>().sl_time_to_trigger_r16.unpack(bref));
      break;
    case types::event_s2_r16:
      bref.unpack(c.get<event_s2_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_s2_r16_s_>().s2_thres_r16.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_s2_r16_s_>().sl_report_on_leave_r16, 1));
      HANDLE_CODE(unpack_integer(c.get<event_s2_r16_s_>().sl_hysteresis_r16, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_s2_r16_s_>().sl_time_to_trigger_r16.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_event_trigger_cfg_r16_s::sl_report_amount_r16_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "sl_event_trigger_cfg_r16_s::sl_report_amount_r16_e_");
}
int8_t sl_event_trigger_cfg_r16_s::sl_report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "sl_event_trigger_cfg_r16_s::sl_report_amount_r16_e_");
}

// SL-PQI-r16 ::= CHOICE
void sl_pqi_r16_c::destroy_()
{
  switch (type_) {
    case types::sl_non_standardized_pqi_r16:
      c.destroy<sl_non_standardized_pqi_r16_s_>();
      break;
    default:
      break;
  }
}
void sl_pqi_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sl_standardized_pqi_r16:
      break;
    case types::sl_non_standardized_pqi_r16:
      c.init<sl_non_standardized_pqi_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_pqi_r16_c");
  }
}
sl_pqi_r16_c::sl_pqi_r16_c(const sl_pqi_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl_standardized_pqi_r16:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl_non_standardized_pqi_r16:
      c.init(other.c.get<sl_non_standardized_pqi_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_pqi_r16_c");
  }
}
sl_pqi_r16_c& sl_pqi_r16_c::operator=(const sl_pqi_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl_standardized_pqi_r16:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl_non_standardized_pqi_r16:
      c.set(other.c.get<sl_non_standardized_pqi_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_pqi_r16_c");
  }

  return *this;
}
uint16_t& sl_pqi_r16_c::set_sl_standardized_pqi_r16()
{
  set(types::sl_standardized_pqi_r16);
  return c.get<uint16_t>();
}
sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_& sl_pqi_r16_c::set_sl_non_standardized_pqi_r16()
{
  set(types::sl_non_standardized_pqi_r16);
  return c.get<sl_non_standardized_pqi_r16_s_>();
}
void sl_pqi_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl_standardized_pqi_r16:
      j.write_int("sl-StandardizedPQI-r16", c.get<uint16_t>());
      break;
    case types::sl_non_standardized_pqi_r16:
      j.write_fieldname("sl-Non-StandardizedPQI-r16");
      j.start_obj();
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16_present) {
        j.write_str("sl-ResourceType-r16", c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16.to_string());
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16_present) {
        j.write_int("sl-PriorityLevel-r16", c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16);
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16_present) {
        j.write_int("sl-PacketDelayBudget-r16", c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16);
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16_present) {
        j.write_int("sl-PacketErrorRate-r16", c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16);
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16_present) {
        j.write_int("sl-AveragingWindow-r16", c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16);
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16_present) {
        j.write_int("sl-MaxDataBurstVolume-r16", c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_pqi_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_pqi_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl_standardized_pqi_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sl_non_standardized_pqi_r16:
      bref.pack(c.get<sl_non_standardized_pqi_r16_s_>().ext, 1);
      HANDLE_CODE(bref.pack(c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16_present, 1));
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16_present) {
        HANDLE_CODE(c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16.pack(bref));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16_present) {
        HANDLE_CODE(
            pack_integer(bref, c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16, (uint8_t)1u, (uint8_t)8u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16_present) {
        HANDLE_CODE(pack_integer(
            bref, c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16_present) {
        HANDLE_CODE(pack_integer(
            bref, c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16, (uint8_t)0u, (uint8_t)9u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16_present) {
        HANDLE_CODE(pack_integer(
            bref, c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16, (uint16_t)0u, (uint16_t)4095u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16_present) {
        HANDLE_CODE(pack_integer(
            bref, c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16, (uint16_t)0u, (uint16_t)4095u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_pqi_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pqi_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl_standardized_pqi_r16:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sl_non_standardized_pqi_r16:
      bref.unpack(c.get<sl_non_standardized_pqi_r16_s_>().ext, 1);
      HANDLE_CODE(bref.unpack(c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16_present, 1));
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16_present) {
        HANDLE_CODE(c.get<sl_non_standardized_pqi_r16_s_>().sl_res_type_r16.unpack(bref));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16_present) {
        HANDLE_CODE(
            unpack_integer(c.get<sl_non_standardized_pqi_r16_s_>().sl_prio_level_r16, bref, (uint8_t)1u, (uint8_t)8u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16_present) {
        HANDLE_CODE(unpack_integer(
            c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_delay_budget_r16, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16_present) {
        HANDLE_CODE(unpack_integer(
            c.get<sl_non_standardized_pqi_r16_s_>().sl_packet_error_rate_r16, bref, (uint8_t)0u, (uint8_t)9u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16_present) {
        HANDLE_CODE(unpack_integer(
            c.get<sl_non_standardized_pqi_r16_s_>().sl_averaging_win_r16, bref, (uint16_t)0u, (uint16_t)4095u));
      }
      if (c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16_present) {
        HANDLE_CODE(unpack_integer(
            c.get<sl_non_standardized_pqi_r16_s_>().sl_max_data_burst_volume_r16, bref, (uint16_t)0u, (uint16_t)4095u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_pqi_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16_opts::to_string() const
{
  static const char* names[] = {"gbr", "non-GBR", "delayCriticalGBR", "spare1"};
  return convert_enum_idx(names, 4, value, "sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16_e_");
}

// SL-PeriodicalReportConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_periodical_report_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sl_report_interv_r16.pack(bref));
  HANDLE_CODE(sl_report_amount_r16.pack(bref));
  HANDLE_CODE(sl_report_quant_r16.pack(bref));
  HANDLE_CODE(sl_rs_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_periodical_report_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sl_report_interv_r16.unpack(bref));
  HANDLE_CODE(sl_report_amount_r16.unpack(bref));
  HANDLE_CODE(sl_report_quant_r16.unpack(bref));
  HANDLE_CODE(sl_rs_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_periodical_report_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-ReportInterval-r16", sl_report_interv_r16.to_string());
  j.write_str("sl-ReportAmount-r16", sl_report_amount_r16.to_string());
  j.write_fieldname("sl-ReportQuantity-r16");
  sl_report_quant_r16.to_json(j);
  j.write_str("sl-RS-Type-r16", sl_rs_type_r16.to_string());
  j.end_obj();
}

const char* sl_periodical_report_cfg_r16_s::sl_report_amount_r16_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "sl_periodical_report_cfg_r16_s::sl_report_amount_r16_e_");
}
int8_t sl_periodical_report_cfg_r16_s::sl_report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "sl_periodical_report_cfg_r16_s::sl_report_amount_r16_e_");
}

// SL-ResourcePoolConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_res_pool_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_res_pool_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_res_pool_id_r16, (uint8_t)1u, (uint8_t)16u));
  if (sl_res_pool_r16_present) {
    HANDLE_CODE(sl_res_pool_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_res_pool_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_res_pool_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_res_pool_id_r16, bref, (uint8_t)1u, (uint8_t)16u));
  if (sl_res_pool_r16_present) {
    HANDLE_CODE(sl_res_pool_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_res_pool_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-ResourcePoolID-r16", sl_res_pool_id_r16);
  if (sl_res_pool_r16_present) {
    j.write_fieldname("sl-ResourcePool-r16");
    sl_res_pool_r16.to_json(j);
  }
  j.end_obj();
}

// SL-TxPower-r16 ::= CHOICE
void sl_tx_pwr_r16_c::set(types::options e)
{
  type_ = e;
}
void sl_tx_pwr_r16_c::set_minusinfinity_r16()
{
  set(types::minusinfinity_r16);
}
int8_t& sl_tx_pwr_r16_c::set_tx_pwr_r16()
{
  set(types::tx_pwr_r16);
  return c;
}
void sl_tx_pwr_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::minusinfinity_r16:
      break;
    case types::tx_pwr_r16:
      j.write_int("txPower-r16", c);
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_pwr_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_tx_pwr_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::minusinfinity_r16:
      break;
    case types::tx_pwr_r16:
      HANDLE_CODE(pack_integer(bref, c, (int8_t)-30, (int8_t)33));
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_pwr_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_pwr_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::minusinfinity_r16:
      break;
    case types::tx_pwr_r16:
      HANDLE_CODE(unpack_integer(c, bref, (int8_t)-30, (int8_t)33));
      break;
    default:
      log_invalid_choice_id(type_, "sl_tx_pwr_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-MeasObject-r16 ::= SEQUENCE
SRSASN_CODE sl_meas_obj_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, freq_info_sl_r16, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_obj_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(freq_info_sl_r16, bref, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
void sl_meas_obj_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("frequencyInfoSL-r16", freq_info_sl_r16);
  j.end_obj();
}

// SL-PSBCH-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_ps_bch_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_p0_ps_bch_r16_present, 1));
  HANDLE_CODE(bref.pack(dl_alpha_ps_bch_r16_present, 1));

  if (dl_p0_ps_bch_r16_present) {
    HANDLE_CODE(pack_integer(bref, dl_p0_ps_bch_r16, (int8_t)-16, (int8_t)15));
  }
  if (dl_alpha_ps_bch_r16_present) {
    HANDLE_CODE(dl_alpha_ps_bch_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dl_p0_ps_bch_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_p0_ps_bch_r17_present, 1));
      if (dl_p0_ps_bch_r17_present) {
        HANDLE_CODE(pack_integer(bref, dl_p0_ps_bch_r17, (int16_t)-202, (int16_t)24));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_ps_bch_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_p0_ps_bch_r16_present, 1));
  HANDLE_CODE(bref.unpack(dl_alpha_ps_bch_r16_present, 1));

  if (dl_p0_ps_bch_r16_present) {
    HANDLE_CODE(unpack_integer(dl_p0_ps_bch_r16, bref, (int8_t)-16, (int8_t)15));
  }
  if (dl_alpha_ps_bch_r16_present) {
    HANDLE_CODE(dl_alpha_ps_bch_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dl_p0_ps_bch_r17_present, 1));
      if (dl_p0_ps_bch_r17_present) {
        HANDLE_CODE(unpack_integer(dl_p0_ps_bch_r17, bref, (int16_t)-202, (int16_t)24));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_ps_bch_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_p0_ps_bch_r16_present) {
    j.write_int("dl-P0-PSBCH-r16", dl_p0_ps_bch_r16);
  }
  if (dl_alpha_ps_bch_r16_present) {
    j.write_str("dl-Alpha-PSBCH-r16", dl_alpha_ps_bch_r16.to_string());
  }
  if (ext) {
    if (dl_p0_ps_bch_r17_present) {
      j.write_int("dl-P0-PSBCH-r17", dl_p0_ps_bch_r17);
    }
  }
  j.end_obj();
}

const char* sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_opts::to_string() const
{
  static const char* names[] = {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
  return convert_enum_idx(names, 8, value, "sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_e_");
}
float sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_e_");
}
const char* sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_e_");
}

// SL-PSSCH-TxParameters-r16 ::= SEQUENCE
SRSASN_CODE sl_pssch_tx_params_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_max_tx_pwr_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_min_mcs_pssch_r16, (uint8_t)0u, (uint8_t)27u));
  HANDLE_CODE(pack_integer(bref, sl_max_mcs_pssch_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, sl_min_sub_ch_num_pssch_r16, (uint8_t)1u, (uint8_t)27u));
  HANDLE_CODE(pack_integer(bref, sl_max_subch_num_pssch_r16, (uint8_t)1u, (uint8_t)27u));
  HANDLE_CODE(pack_integer(bref, sl_max_tx_trans_num_pssch_r16, (uint8_t)1u, (uint8_t)32u));
  if (sl_max_tx_pwr_r16_present) {
    HANDLE_CODE(sl_max_tx_pwr_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pssch_tx_params_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_max_tx_pwr_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_min_mcs_pssch_r16, bref, (uint8_t)0u, (uint8_t)27u));
  HANDLE_CODE(unpack_integer(sl_max_mcs_pssch_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(sl_min_sub_ch_num_pssch_r16, bref, (uint8_t)1u, (uint8_t)27u));
  HANDLE_CODE(unpack_integer(sl_max_subch_num_pssch_r16, bref, (uint8_t)1u, (uint8_t)27u));
  HANDLE_CODE(unpack_integer(sl_max_tx_trans_num_pssch_r16, bref, (uint8_t)1u, (uint8_t)32u));
  if (sl_max_tx_pwr_r16_present) {
    HANDLE_CODE(sl_max_tx_pwr_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_pssch_tx_params_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-MinMCS-PSSCH-r16", sl_min_mcs_pssch_r16);
  j.write_int("sl-MaxMCS-PSSCH-r16", sl_max_mcs_pssch_r16);
  j.write_int("sl-MinSubChannelNumPSSCH-r16", sl_min_sub_ch_num_pssch_r16);
  j.write_int("sl-MaxSubchannelNumPSSCH-r16", sl_max_subch_num_pssch_r16);
  j.write_int("sl-MaxTxTransNumPSSCH-r16", sl_max_tx_trans_num_pssch_r16);
  if (sl_max_tx_pwr_r16_present) {
    j.write_fieldname("sl-MaxTxPower-r16");
    sl_max_tx_pwr_r16.to_json(j);
  }
  j.end_obj();
}

// SL-PeriodCG-r16 ::= CHOICE
void sl_period_cg_r16_c::destroy_() {}
void sl_period_cg_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_period_cg_r16_c::sl_period_cg_r16_c(const sl_period_cg_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl_period_cg1_r16:
      c.init(other.c.get<sl_period_cg1_r16_e_>());
      break;
    case types::sl_period_cg2_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_period_cg_r16_c");
  }
}
sl_period_cg_r16_c& sl_period_cg_r16_c::operator=(const sl_period_cg_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl_period_cg1_r16:
      c.set(other.c.get<sl_period_cg1_r16_e_>());
      break;
    case types::sl_period_cg2_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_period_cg_r16_c");
  }

  return *this;
}
sl_period_cg_r16_c::sl_period_cg1_r16_e_& sl_period_cg_r16_c::set_sl_period_cg1_r16()
{
  set(types::sl_period_cg1_r16);
  return c.get<sl_period_cg1_r16_e_>();
}
uint8_t& sl_period_cg_r16_c::set_sl_period_cg2_r16()
{
  set(types::sl_period_cg2_r16);
  return c.get<uint8_t>();
}
void sl_period_cg_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl_period_cg1_r16:
      j.write_str("sl-PeriodCG1-r16", c.get<sl_period_cg1_r16_e_>().to_string());
      break;
    case types::sl_period_cg2_r16:
      j.write_int("sl-PeriodCG2-r16", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sl_period_cg_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_period_cg_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl_period_cg1_r16:
      HANDLE_CODE(c.get<sl_period_cg1_r16_e_>().pack(bref));
      break;
    case types::sl_period_cg2_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)99u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_period_cg_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_period_cg_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl_period_cg1_r16:
      HANDLE_CODE(c.get<sl_period_cg1_r16_e_>().unpack(bref));
      break;
    case types::sl_period_cg2_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)99u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_period_cg_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_period_cg_r16_c::sl_period_cg1_r16_opts::to_string() const
{
  static const char* names[] = {"ms100",
                                "ms200",
                                "ms300",
                                "ms400",
                                "ms500",
                                "ms600",
                                "ms700",
                                "ms800",
                                "ms900",
                                "ms1000",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sl_period_cg_r16_c::sl_period_cg1_r16_e_");
}
uint16_t sl_period_cg_r16_c::sl_period_cg1_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(numbers, 10, value, "sl_period_cg_r16_c::sl_period_cg1_r16_e_");
}

// SL-QoS-Profile-r16 ::= SEQUENCE
SRSASN_CODE sl_qos_profile_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_pqi_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_gfbr_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_mfbr_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_range_r16_present, 1));

  if (sl_pqi_r16_present) {
    HANDLE_CODE(sl_pqi_r16.pack(bref));
  }
  if (sl_gfbr_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_gfbr_r16, (uint32_t)0u, (uint32_t)4000000000u));
  }
  if (sl_mfbr_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_mfbr_r16, (uint32_t)0u, (uint32_t)4000000000u));
  }
  if (sl_range_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_range_r16, (uint16_t)1u, (uint16_t)1000u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_qos_profile_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_pqi_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_gfbr_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_mfbr_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_range_r16_present, 1));

  if (sl_pqi_r16_present) {
    HANDLE_CODE(sl_pqi_r16.unpack(bref));
  }
  if (sl_gfbr_r16_present) {
    HANDLE_CODE(unpack_integer(sl_gfbr_r16, bref, (uint32_t)0u, (uint32_t)4000000000u));
  }
  if (sl_mfbr_r16_present) {
    HANDLE_CODE(unpack_integer(sl_mfbr_r16, bref, (uint32_t)0u, (uint32_t)4000000000u));
  }
  if (sl_range_r16_present) {
    HANDLE_CODE(unpack_integer(sl_range_r16, bref, (uint16_t)1u, (uint16_t)1000u));
  }

  return SRSASN_SUCCESS;
}
void sl_qos_profile_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_pqi_r16_present) {
    j.write_fieldname("sl-PQI-r16");
    sl_pqi_r16.to_json(j);
  }
  if (sl_gfbr_r16_present) {
    j.write_int("sl-GFBR-r16", sl_gfbr_r16);
  }
  if (sl_mfbr_r16_present) {
    j.write_int("sl-MFBR-r16", sl_mfbr_r16);
  }
  if (sl_range_r16_present) {
    j.write_int("sl-Range-r16", sl_range_r16);
  }
  j.end_obj();
}

// SL-ReportConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_report_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sl_report_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_report_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sl_report_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_report_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-ReportType-r16");
  sl_report_type_r16.to_json(j);
  j.end_obj();
}

void sl_report_cfg_r16_s::sl_report_type_r16_c_::destroy_()
{
  switch (type_) {
    case types::sl_periodical_r16:
      c.destroy<sl_periodical_report_cfg_r16_s>();
      break;
    case types::sl_event_triggered_r16:
      c.destroy<sl_event_trigger_cfg_r16_s>();
      break;
    default:
      break;
  }
}
void sl_report_cfg_r16_s::sl_report_type_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sl_periodical_r16:
      c.init<sl_periodical_report_cfg_r16_s>();
      break;
    case types::sl_event_triggered_r16:
      c.init<sl_event_trigger_cfg_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_report_cfg_r16_s::sl_report_type_r16_c_");
  }
}
sl_report_cfg_r16_s::sl_report_type_r16_c_::sl_report_type_r16_c_(
    const sl_report_cfg_r16_s::sl_report_type_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl_periodical_r16:
      c.init(other.c.get<sl_periodical_report_cfg_r16_s>());
      break;
    case types::sl_event_triggered_r16:
      c.init(other.c.get<sl_event_trigger_cfg_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_report_cfg_r16_s::sl_report_type_r16_c_");
  }
}
sl_report_cfg_r16_s::sl_report_type_r16_c_&
sl_report_cfg_r16_s::sl_report_type_r16_c_::operator=(const sl_report_cfg_r16_s::sl_report_type_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl_periodical_r16:
      c.set(other.c.get<sl_periodical_report_cfg_r16_s>());
      break;
    case types::sl_event_triggered_r16:
      c.set(other.c.get<sl_event_trigger_cfg_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_report_cfg_r16_s::sl_report_type_r16_c_");
  }

  return *this;
}
sl_periodical_report_cfg_r16_s& sl_report_cfg_r16_s::sl_report_type_r16_c_::set_sl_periodical_r16()
{
  set(types::sl_periodical_r16);
  return c.get<sl_periodical_report_cfg_r16_s>();
}
sl_event_trigger_cfg_r16_s& sl_report_cfg_r16_s::sl_report_type_r16_c_::set_sl_event_triggered_r16()
{
  set(types::sl_event_triggered_r16);
  return c.get<sl_event_trigger_cfg_r16_s>();
}
void sl_report_cfg_r16_s::sl_report_type_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl_periodical_r16:
      j.write_fieldname("sl-Periodical-r16");
      c.get<sl_periodical_report_cfg_r16_s>().to_json(j);
      break;
    case types::sl_event_triggered_r16:
      j.write_fieldname("sl-EventTriggered-r16");
      c.get<sl_event_trigger_cfg_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sl_report_cfg_r16_s::sl_report_type_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_report_cfg_r16_s::sl_report_type_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl_periodical_r16:
      HANDLE_CODE(c.get<sl_periodical_report_cfg_r16_s>().pack(bref));
      break;
    case types::sl_event_triggered_r16:
      HANDLE_CODE(c.get<sl_event_trigger_cfg_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_report_cfg_r16_s::sl_report_type_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_report_cfg_r16_s::sl_report_type_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl_periodical_r16:
      HANDLE_CODE(c.get<sl_periodical_report_cfg_r16_s>().unpack(bref));
      break;
    case types::sl_event_triggered_r16:
      HANDLE_CODE(c.get<sl_event_trigger_cfg_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_report_cfg_r16_s::sl_report_type_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-SSB-TimeAllocation-r16 ::= SEQUENCE
SRSASN_CODE sl_ssb_time_alloc_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_num_ssb_within_period_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_time_offset_ssb_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_time_interv_r16_present, 1));

  if (sl_num_ssb_within_period_r16_present) {
    HANDLE_CODE(sl_num_ssb_within_period_r16.pack(bref));
  }
  if (sl_time_offset_ssb_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_time_offset_ssb_r16, (uint16_t)0u, (uint16_t)1279u));
  }
  if (sl_time_interv_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_time_interv_r16, (uint16_t)0u, (uint16_t)639u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_ssb_time_alloc_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_num_ssb_within_period_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_time_offset_ssb_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_time_interv_r16_present, 1));

  if (sl_num_ssb_within_period_r16_present) {
    HANDLE_CODE(sl_num_ssb_within_period_r16.unpack(bref));
  }
  if (sl_time_offset_ssb_r16_present) {
    HANDLE_CODE(unpack_integer(sl_time_offset_ssb_r16, bref, (uint16_t)0u, (uint16_t)1279u));
  }
  if (sl_time_interv_r16_present) {
    HANDLE_CODE(unpack_integer(sl_time_interv_r16, bref, (uint16_t)0u, (uint16_t)639u));
  }

  return SRSASN_SUCCESS;
}
void sl_ssb_time_alloc_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_num_ssb_within_period_r16_present) {
    j.write_str("sl-NumSSB-WithinPeriod-r16", sl_num_ssb_within_period_r16.to_string());
  }
  if (sl_time_offset_ssb_r16_present) {
    j.write_int("sl-TimeOffsetSSB-r16", sl_time_offset_ssb_r16);
  }
  if (sl_time_interv_r16_present) {
    j.write_int("sl-TimeInterval-r16", sl_time_interv_r16);
  }
  j.end_obj();
}

const char* sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 7, value, "sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_e_");
}
uint8_t sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_e_");
}

// SL-TxPoolDedicated-r16 ::= SEQUENCE
SRSASN_CODE sl_tx_pool_ded_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_pool_to_release_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_pool_to_add_mod_list_r16.size() > 0, 1));

  if (sl_pool_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_pool_to_release_list_r16, 1, 8, integer_packer<uint8_t>(1, 16)));
  }
  if (sl_pool_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_pool_to_add_mod_list_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_tx_pool_ded_r16_s::unpack(cbit_ref& bref)
{
  bool sl_pool_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_pool_to_release_list_r16_present, 1));
  bool sl_pool_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_pool_to_add_mod_list_r16_present, 1));

  if (sl_pool_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_pool_to_release_list_r16, bref, 1, 8, integer_packer<uint8_t>(1, 16)));
  }
  if (sl_pool_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_pool_to_add_mod_list_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void sl_tx_pool_ded_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_pool_to_release_list_r16.size() > 0) {
    j.start_array("sl-PoolToReleaseList-r16");
    for (const auto& e1 : sl_pool_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_pool_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-PoolToAddModList-r16");
    for (const auto& e1 : sl_pool_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-TypeTxSync-r16 ::= ENUMERATED
const char* sl_type_tx_sync_r16_opts::to_string() const
{
  static const char* names[] = {"gnss", "gnbEnb", "ue"};
  return convert_enum_idx(names, 3, value, "sl_type_tx_sync_r16_e");
}

// BSR-Config ::= SEQUENCE
SRSASN_CODE bsr_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lc_ch_sr_delay_timer_present, 1));

  HANDLE_CODE(periodic_bsr_timer.pack(bref));
  HANDLE_CODE(retx_bsr_timer.pack(bref));
  if (lc_ch_sr_delay_timer_present) {
    HANDLE_CODE(lc_ch_sr_delay_timer.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bsr_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lc_ch_sr_delay_timer_present, 1));

  HANDLE_CODE(periodic_bsr_timer.unpack(bref));
  HANDLE_CODE(retx_bsr_timer.unpack(bref));
  if (lc_ch_sr_delay_timer_present) {
    HANDLE_CODE(lc_ch_sr_delay_timer.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bsr_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("periodicBSR-Timer", periodic_bsr_timer.to_string());
  j.write_str("retxBSR-Timer", retx_bsr_timer.to_string());
  if (lc_ch_sr_delay_timer_present) {
    j.write_str("logicalChannelSR-DelayTimer", lc_ch_sr_delay_timer.to_string());
  }
  j.end_obj();
}

const char* bsr_cfg_s::periodic_bsr_timer_opts::to_string() const
{
  static const char* names[] = {"sf1",
                                "sf5",
                                "sf10",
                                "sf16",
                                "sf20",
                                "sf32",
                                "sf40",
                                "sf64",
                                "sf80",
                                "sf128",
                                "sf160",
                                "sf320",
                                "sf640",
                                "sf1280",
                                "sf2560",
                                "infinity"};
  return convert_enum_idx(names, 16, value, "bsr_cfg_s::periodic_bsr_timer_e_");
}
int16_t bsr_cfg_s::periodic_bsr_timer_opts::to_number() const
{
  static const int16_t numbers[] = {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return map_enum_number(numbers, 16, value, "bsr_cfg_s::periodic_bsr_timer_e_");
}

const char* bsr_cfg_s::retx_bsr_timer_opts::to_string() const
{
  static const char* names[] = {"sf10",
                                "sf20",
                                "sf40",
                                "sf80",
                                "sf160",
                                "sf320",
                                "sf640",
                                "sf1280",
                                "sf2560",
                                "sf5120",
                                "sf10240",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "bsr_cfg_s::retx_bsr_timer_e_");
}
uint16_t bsr_cfg_s::retx_bsr_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(numbers, 11, value, "bsr_cfg_s::retx_bsr_timer_e_");
}

const char* bsr_cfg_s::lc_ch_sr_delay_timer_opts::to_string() const
{
  static const char* names[] = {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
  return convert_enum_idx(names, 8, value, "bsr_cfg_s::lc_ch_sr_delay_timer_e_");
}
uint16_t bsr_cfg_s::lc_ch_sr_delay_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(numbers, 7, value, "bsr_cfg_s::lc_ch_sr_delay_timer_e_");
}

// PollByte ::= ENUMERATED
const char* poll_byte_opts::to_string() const
{
  static const char* names[] = {"kB1",     "kB2",     "kB5",     "kB8",      "kB10",    "kB15",    "kB25",    "kB50",
                                "kB75",    "kB100",   "kB125",   "kB250",    "kB375",   "kB500",   "kB750",   "kB1000",
                                "kB1250",  "kB1500",  "kB2000",  "kB3000",   "kB4000",  "kB4500",  "kB5000",  "kB5500",
                                "kB6000",  "kB6500",  "kB7000",  "kB7500",   "mB8",     "mB9",     "mB10",    "mB11",
                                "mB12",    "mB13",    "mB14",    "mB15",     "mB16",    "mB17",    "mB18",    "mB20",
                                "mB25",    "mB30",    "mB40",    "infinity", "spare20", "spare19", "spare18", "spare17",
                                "spare16", "spare15", "spare14", "spare13",  "spare12", "spare11", "spare10", "spare9",
                                "spare8",  "spare7",  "spare6",  "spare5",   "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 64, value, "poll_byte_e");
}
int32_t poll_byte_opts::to_number() const
{
  static const int32_t numbers[] = {1,     2,     5,     8,     10,    15,    25,    50,    75,    100,   125,
                                    250,   375,   500,   750,   1000,  1250,  1500,  2000,  3000,  4000,  4500,
                                    5000,  5500,  6000,  6500,  7000,  7500,  8000,  9000,  10000, 11000, 12000,
                                    13000, 14000, 15000, 16000, 17000, 18000, 20000, 25000, 30000, 40000, -1};
  return map_enum_number(numbers, 44, value, "poll_byte_e");
}

// PollPDU ::= ENUMERATED
const char* poll_pdu_opts::to_string() const
{
  static const char* names[] = {"p4",     "p8",     "p16",    "p32",    "p64",    "p128",   "p256",   "p512",
                                "p1024",  "p2048",  "p4096",  "p6144",  "p8192",  "p12288", "p16384", "p20480",
                                "p24576", "p28672", "p32768", "p40960", "p49152", "p57344", "p65536", "infinity",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "poll_pdu_e");
}
int32_t poll_pdu_opts::to_number() const
{
  static const int32_t numbers[] = {4,    8,     16,    32,    64,    128,   256,   512,   1024,  2048,  4096,  6144,
                                    8192, 12288, 16384, 20480, 24576, 28672, 32768, 40960, 49152, 57344, 65536, -1};
  return map_enum_number(numbers, 24, value, "poll_pdu_e");
}

// SL-BWP-DiscPoolConfig-r17 ::= SEQUENCE
SRSASN_CODE sl_bwp_disc_pool_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_disc_rx_pool_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_disc_tx_pool_sel_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_disc_tx_pool_sched_r17_present, 1));

  if (sl_disc_rx_pool_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_disc_rx_pool_r17, 1, 16));
  }
  if (sl_disc_tx_pool_sel_r17_present) {
    HANDLE_CODE(sl_disc_tx_pool_sel_r17.pack(bref));
  }
  if (sl_disc_tx_pool_sched_r17_present) {
    HANDLE_CODE(sl_disc_tx_pool_sched_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_bwp_disc_pool_cfg_r17_s::unpack(cbit_ref& bref)
{
  bool sl_disc_rx_pool_r17_present;
  HANDLE_CODE(bref.unpack(sl_disc_rx_pool_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_disc_tx_pool_sel_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_disc_tx_pool_sched_r17_present, 1));

  if (sl_disc_rx_pool_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_disc_rx_pool_r17, bref, 1, 16));
  }
  if (sl_disc_tx_pool_sel_r17_present) {
    HANDLE_CODE(sl_disc_tx_pool_sel_r17.unpack(bref));
  }
  if (sl_disc_tx_pool_sched_r17_present) {
    HANDLE_CODE(sl_disc_tx_pool_sched_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_bwp_disc_pool_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_disc_rx_pool_r17.size() > 0) {
    j.start_array("sl-DiscRxPool-r17");
    for (const auto& e1 : sl_disc_rx_pool_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_disc_tx_pool_sel_r17_present) {
    j.write_fieldname("sl-DiscTxPoolSelected-r17");
    sl_disc_tx_pool_sel_r17.to_json(j);
  }
  if (sl_disc_tx_pool_sched_r17_present) {
    j.write_fieldname("sl-DiscTxPoolScheduling-r17");
    sl_disc_tx_pool_sched_r17.to_json(j);
  }
  j.end_obj();
}

// SL-BWP-Generic-r16 ::= SEQUENCE
SRSASN_CODE sl_bwp_generic_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_bwp_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_len_symbols_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_start_symbol_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ps_bch_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_tx_direct_current_location_r16_present, 1));

  if (sl_bwp_r16_present) {
    HANDLE_CODE(sl_bwp_r16.pack(bref));
  }
  if (sl_len_symbols_r16_present) {
    HANDLE_CODE(sl_len_symbols_r16.pack(bref));
  }
  if (sl_start_symbol_r16_present) {
    HANDLE_CODE(sl_start_symbol_r16.pack(bref));
  }
  if (sl_ps_bch_cfg_r16_present) {
    HANDLE_CODE(sl_ps_bch_cfg_r16.pack(bref));
  }
  if (sl_tx_direct_current_location_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_tx_direct_current_location_r16, (uint16_t)0u, (uint16_t)3301u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_bwp_generic_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_bwp_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_len_symbols_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_start_symbol_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ps_bch_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_direct_current_location_r16_present, 1));

  if (sl_bwp_r16_present) {
    HANDLE_CODE(sl_bwp_r16.unpack(bref));
  }
  if (sl_len_symbols_r16_present) {
    HANDLE_CODE(sl_len_symbols_r16.unpack(bref));
  }
  if (sl_start_symbol_r16_present) {
    HANDLE_CODE(sl_start_symbol_r16.unpack(bref));
  }
  if (sl_ps_bch_cfg_r16_present) {
    HANDLE_CODE(sl_ps_bch_cfg_r16.unpack(bref));
  }
  if (sl_tx_direct_current_location_r16_present) {
    HANDLE_CODE(unpack_integer(sl_tx_direct_current_location_r16, bref, (uint16_t)0u, (uint16_t)3301u));
  }

  return SRSASN_SUCCESS;
}
void sl_bwp_generic_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_bwp_r16_present) {
    j.write_fieldname("sl-BWP-r16");
    sl_bwp_r16.to_json(j);
  }
  if (sl_len_symbols_r16_present) {
    j.write_str("sl-LengthSymbols-r16", sl_len_symbols_r16.to_string());
  }
  if (sl_start_symbol_r16_present) {
    j.write_str("sl-StartSymbol-r16", sl_start_symbol_r16.to_string());
  }
  if (sl_ps_bch_cfg_r16_present) {
    j.write_fieldname("sl-PSBCH-Config-r16");
    sl_ps_bch_cfg_r16.to_json(j);
  }
  if (sl_tx_direct_current_location_r16_present) {
    j.write_int("sl-TxDirectCurrentLocation-r16", sl_tx_direct_current_location_r16);
  }
  j.end_obj();
}

const char* sl_bwp_generic_r16_s::sl_len_symbols_r16_opts::to_string() const
{
  static const char* names[] = {"sym7", "sym8", "sym9", "sym10", "sym11", "sym12", "sym13", "sym14"};
  return convert_enum_idx(names, 8, value, "sl_bwp_generic_r16_s::sl_len_symbols_r16_e_");
}
uint8_t sl_bwp_generic_r16_s::sl_len_symbols_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 8, 9, 10, 11, 12, 13, 14};
  return map_enum_number(numbers, 8, value, "sl_bwp_generic_r16_s::sl_len_symbols_r16_e_");
}

const char* sl_bwp_generic_r16_s::sl_start_symbol_r16_opts::to_string() const
{
  static const char* names[] = {"sym0", "sym1", "sym2", "sym3", "sym4", "sym5", "sym6", "sym7"};
  return convert_enum_idx(names, 8, value, "sl_bwp_generic_r16_s::sl_start_symbol_r16_e_");
}
uint8_t sl_bwp_generic_r16_s::sl_start_symbol_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(numbers, 8, value, "sl_bwp_generic_r16_s::sl_start_symbol_r16_e_");
}

// SL-BWP-PoolConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_bwp_pool_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_rx_pool_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_tx_pool_sel_normal_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_tx_pool_sched_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_tx_pool_exceptional_r16_present, 1));

  if (sl_rx_pool_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rx_pool_r16, 1, 16));
  }
  if (sl_tx_pool_sel_normal_r16_present) {
    HANDLE_CODE(sl_tx_pool_sel_normal_r16.pack(bref));
  }
  if (sl_tx_pool_sched_r16_present) {
    HANDLE_CODE(sl_tx_pool_sched_r16.pack(bref));
  }
  if (sl_tx_pool_exceptional_r16_present) {
    HANDLE_CODE(sl_tx_pool_exceptional_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_bwp_pool_cfg_r16_s::unpack(cbit_ref& bref)
{
  bool sl_rx_pool_r16_present;
  HANDLE_CODE(bref.unpack(sl_rx_pool_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_pool_sel_normal_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_pool_sched_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_pool_exceptional_r16_present, 1));

  if (sl_rx_pool_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rx_pool_r16, bref, 1, 16));
  }
  if (sl_tx_pool_sel_normal_r16_present) {
    HANDLE_CODE(sl_tx_pool_sel_normal_r16.unpack(bref));
  }
  if (sl_tx_pool_sched_r16_present) {
    HANDLE_CODE(sl_tx_pool_sched_r16.unpack(bref));
  }
  if (sl_tx_pool_exceptional_r16_present) {
    HANDLE_CODE(sl_tx_pool_exceptional_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_bwp_pool_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_rx_pool_r16.size() > 0) {
    j.start_array("sl-RxPool-r16");
    for (const auto& e1 : sl_rx_pool_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_tx_pool_sel_normal_r16_present) {
    j.write_fieldname("sl-TxPoolSelectedNormal-r16");
    sl_tx_pool_sel_normal_r16.to_json(j);
  }
  if (sl_tx_pool_sched_r16_present) {
    j.write_fieldname("sl-TxPoolScheduling-r16");
    sl_tx_pool_sched_r16.to_json(j);
  }
  if (sl_tx_pool_exceptional_r16_present) {
    j.write_fieldname("sl-TxPoolExceptional-r16");
    sl_tx_pool_exceptional_r16.to_json(j);
  }
  j.end_obj();
}

// SL-CBR-PSSCH-TxConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_cbr_pssch_tx_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_cr_limit_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_tx_params_r16_present, 1));

  if (sl_cr_limit_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_cr_limit_r16, (uint16_t)0u, (uint16_t)10000u));
  }
  if (sl_tx_params_r16_present) {
    HANDLE_CODE(sl_tx_params_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cbr_pssch_tx_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_cr_limit_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_params_r16_present, 1));

  if (sl_cr_limit_r16_present) {
    HANDLE_CODE(unpack_integer(sl_cr_limit_r16, bref, (uint16_t)0u, (uint16_t)10000u));
  }
  if (sl_tx_params_r16_present) {
    HANDLE_CODE(sl_tx_params_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_cbr_pssch_tx_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_cr_limit_r16_present) {
    j.write_int("sl-CR-Limit-r16", sl_cr_limit_r16);
  }
  if (sl_tx_params_r16_present) {
    j.write_fieldname("sl-TxParameters-r16");
    sl_tx_params_r16.to_json(j);
  }
  j.end_obj();
}

// SL-ConfiguredGrantConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_cfg_grant_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_period_cg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_nr_of_harq_processes_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_harq_proc_id_offset_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_cg_max_trans_num_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_cfg_idx_cg_r16, (uint8_t)0u, (uint8_t)7u));
  if (sl_period_cg_r16_present) {
    HANDLE_CODE(sl_period_cg_r16.pack(bref));
  }
  if (sl_nr_of_harq_processes_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_nr_of_harq_processes_r16, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_harq_proc_id_offset_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_harq_proc_id_offset_r16, (uint8_t)0u, (uint8_t)15u));
  }
  if (sl_cg_max_trans_num_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_cg_max_trans_num_list_r16, 1, 8));
  }
  if (rrc_cfg_sidelink_grant_r16_present) {
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_sidelink_grant_r16.sl_time_ref_sfn_type1_r16_present, 1));
    if (rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16_present) {
      HANDLE_CODE(
          pack_integer(bref, rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16, (uint16_t)0u, (uint16_t)496u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16_present) {
      HANDLE_CODE(
          pack_integer(bref, rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16, (uint8_t)0u, (uint8_t)26u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16_present) {
      HANDLE_CODE(
          pack_integer(bref, rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16, (uint16_t)0u, (uint16_t)6929u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16_present) {
      HANDLE_CODE(
          pack_integer(bref, rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16, (uint16_t)0u, (uint16_t)7999u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16_present) {
      HANDLE_CODE(pack_integer(bref, rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16, (uint8_t)0u, (uint8_t)127u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16_present) {
      HANDLE_CODE(
          pack_integer(bref, rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16, (uint8_t)0u, (uint8_t)15u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16_present) {
      HANDLE_CODE(pack_integer(bref, rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16, (uint8_t)1u, (uint8_t)16u));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_n1_pucch_an_type2_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_n1_pucch_an_type2_r16_present, 1));
      if (sl_n1_pucch_an_type2_r16_present) {
        HANDLE_CODE(pack_integer(bref, sl_n1_pucch_an_type2_r16, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cfg_grant_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_period_cg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_nr_of_harq_processes_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_harq_proc_id_offset_r16_present, 1));
  bool sl_cg_max_trans_num_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_cg_max_trans_num_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_cfg_idx_cg_r16, bref, (uint8_t)0u, (uint8_t)7u));
  if (sl_period_cg_r16_present) {
    HANDLE_CODE(sl_period_cg_r16.unpack(bref));
  }
  if (sl_nr_of_harq_processes_r16_present) {
    HANDLE_CODE(unpack_integer(sl_nr_of_harq_processes_r16, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_harq_proc_id_offset_r16_present) {
    HANDLE_CODE(unpack_integer(sl_harq_proc_id_offset_r16, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (sl_cg_max_trans_num_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_cg_max_trans_num_list_r16, bref, 1, 8));
  }
  if (rrc_cfg_sidelink_grant_r16_present) {
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_sidelink_grant_r16.sl_time_ref_sfn_type1_r16_present, 1));
    if (rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16_present) {
      HANDLE_CODE(
          unpack_integer(rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16, bref, (uint16_t)0u, (uint16_t)496u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16_present) {
      HANDLE_CODE(
          unpack_integer(rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16, bref, (uint8_t)0u, (uint8_t)26u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16_present) {
      HANDLE_CODE(
          unpack_integer(rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16, bref, (uint16_t)0u, (uint16_t)6929u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16_present) {
      HANDLE_CODE(
          unpack_integer(rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16, bref, (uint16_t)0u, (uint16_t)7999u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16_present) {
      HANDLE_CODE(unpack_integer(rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16, bref, (uint8_t)0u, (uint8_t)127u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16_present) {
      HANDLE_CODE(
          unpack_integer(rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16, bref, (uint8_t)0u, (uint8_t)15u));
    }
    if (rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16_present) {
      HANDLE_CODE(unpack_integer(rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16, bref, (uint8_t)1u, (uint8_t)16u));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sl_n1_pucch_an_type2_r16_present, 1));
      if (sl_n1_pucch_an_type2_r16_present) {
        HANDLE_CODE(unpack_integer(sl_n1_pucch_an_type2_r16, bref, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_cfg_grant_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-ConfigIndexCG-r16", sl_cfg_idx_cg_r16);
  if (sl_period_cg_r16_present) {
    j.write_fieldname("sl-PeriodCG-r16");
    sl_period_cg_r16.to_json(j);
  }
  if (sl_nr_of_harq_processes_r16_present) {
    j.write_int("sl-NrOfHARQ-Processes-r16", sl_nr_of_harq_processes_r16);
  }
  if (sl_harq_proc_id_offset_r16_present) {
    j.write_int("sl-HARQ-ProcID-offset-r16", sl_harq_proc_id_offset_r16);
  }
  if (sl_cg_max_trans_num_list_r16.size() > 0) {
    j.start_array("sl-CG-MaxTransNumList-r16");
    for (const auto& e1 : sl_cg_max_trans_num_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rrc_cfg_sidelink_grant_r16_present) {
    j.write_fieldname("rrc-ConfiguredSidelinkGrant-r16");
    j.start_obj();
    if (rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16_present) {
      j.write_int("sl-TimeResourceCG-Type1-r16", rrc_cfg_sidelink_grant_r16.sl_time_res_cg_type1_r16);
    }
    if (rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16_present) {
      j.write_int("sl-StartSubchannelCG-Type1-r16", rrc_cfg_sidelink_grant_r16.sl_start_subch_cg_type1_r16);
    }
    if (rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16_present) {
      j.write_int("sl-FreqResourceCG-Type1-r16", rrc_cfg_sidelink_grant_r16.sl_freq_res_cg_type1_r16);
    }
    if (rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16_present) {
      j.write_int("sl-TimeOffsetCG-Type1-r16", rrc_cfg_sidelink_grant_r16.sl_time_offset_cg_type1_r16);
    }
    if (rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16_present) {
      j.write_int("sl-N1PUCCH-AN-r16", rrc_cfg_sidelink_grant_r16.sl_n1_pucch_an_r16);
    }
    if (rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16_present) {
      j.write_int("sl-PSFCH-ToPUCCH-CG-Type1-r16", rrc_cfg_sidelink_grant_r16.sl_psfch_to_pucch_cg_type1_r16);
    }
    if (rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16_present) {
      j.write_int("sl-ResourcePoolID-r16", rrc_cfg_sidelink_grant_r16.sl_res_pool_id_r16);
    }
    if (rrc_cfg_sidelink_grant_r16.sl_time_ref_sfn_type1_r16_present) {
      j.write_str("sl-TimeReferenceSFN-Type1-r16", "sfn512");
    }
    j.end_obj();
  }
  if (ext) {
    if (sl_n1_pucch_an_type2_r16_present) {
      j.write_int("sl-N1PUCCH-AN-Type2-r16", sl_n1_pucch_an_type2_r16);
    }
  }
  j.end_obj();
}

// SL-DRX-ConfigUC-r17 ::= SEQUENCE
SRSASN_CODE sl_drx_cfg_uc_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_drx_harq_rtt_timer1_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_drx_harq_rtt_timer2_r17_present, 1));

  HANDLE_CODE(sl_drx_on_dur_timer_r17.pack(bref));
  HANDLE_CODE(sl_drx_inactivity_timer_r17.pack(bref));
  if (sl_drx_harq_rtt_timer1_r17_present) {
    HANDLE_CODE(sl_drx_harq_rtt_timer1_r17.pack(bref));
  }
  if (sl_drx_harq_rtt_timer2_r17_present) {
    HANDLE_CODE(sl_drx_harq_rtt_timer2_r17.pack(bref));
  }
  HANDLE_CODE(sl_drx_retx_timer_r17.pack(bref));
  HANDLE_CODE(sl_drx_cycle_start_offset_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, sl_drx_slot_offset, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_uc_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_drx_harq_rtt_timer1_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_drx_harq_rtt_timer2_r17_present, 1));

  HANDLE_CODE(sl_drx_on_dur_timer_r17.unpack(bref));
  HANDLE_CODE(sl_drx_inactivity_timer_r17.unpack(bref));
  if (sl_drx_harq_rtt_timer1_r17_present) {
    HANDLE_CODE(sl_drx_harq_rtt_timer1_r17.unpack(bref));
  }
  if (sl_drx_harq_rtt_timer2_r17_present) {
    HANDLE_CODE(sl_drx_harq_rtt_timer2_r17.unpack(bref));
  }
  HANDLE_CODE(sl_drx_retx_timer_r17.unpack(bref));
  HANDLE_CODE(sl_drx_cycle_start_offset_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(sl_drx_slot_offset, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void sl_drx_cfg_uc_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-drx-onDurationTimer-r17");
  sl_drx_on_dur_timer_r17.to_json(j);
  j.write_str("sl-drx-InactivityTimer-r17", sl_drx_inactivity_timer_r17.to_string());
  if (sl_drx_harq_rtt_timer1_r17_present) {
    j.write_str("sl-drx-HARQ-RTT-Timer1-r17", sl_drx_harq_rtt_timer1_r17.to_string());
  }
  if (sl_drx_harq_rtt_timer2_r17_present) {
    j.write_str("sl-drx-HARQ-RTT-Timer2-r17", sl_drx_harq_rtt_timer2_r17.to_string());
  }
  j.write_str("sl-drx-RetransmissionTimer-r17", sl_drx_retx_timer_r17.to_string());
  j.write_fieldname("sl-drx-CycleStartOffset-r17");
  sl_drx_cycle_start_offset_r17.to_json(j);
  j.write_int("sl-drx-SlotOffset", sl_drx_slot_offset);
  j.end_obj();
}

void sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::destroy_() {}
void sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::sl_drx_on_dur_timer_r17_c_(
    const sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_& other)
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_");
  }
}
sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_&
sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::operator=(const sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_& other)
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_");
  }

  return *this;
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_&
sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_");
}
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 24, value, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_");
}

const char* sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_opts::to_string() const
{
  static const char* names[] = {"ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",
                                "ms10",   "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",
                                "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_e_");
}
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_e_");
}

const char* sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_opts::to_string() const
{
  static const char* names[] = {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_e_");
}
uint8_t sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_e_");
}

const char* sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_opts::to_string() const
{
  static const char* names[] = {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_e_");
}
uint8_t sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_e_");
}

const char* sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_opts::to_string() const
{
  static const char* names[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_e_");
}
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_e_");
}

void sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::destroy_() {}
void sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::sl_drx_cycle_start_offset_r17_c_(
    const sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_& other)
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_");
  }
}
sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::operator=(
    const sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_& other)
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_");
  }

  return *this;
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
void sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-DRX-GC-BC-QoS-r17 ::= SEQUENCE
SRSASN_CODE sl_drx_gc_bc_qos_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_drx_gc_bc_mapped_qos_flow_list_r17.size() > 0, 1));

  if (sl_drx_gc_bc_mapped_qos_flow_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drx_gc_bc_mapped_qos_flow_list_r17, 1, 2048));
  }
  HANDLE_CODE(sl_drx_gc_bc_on_dur_timer_r17.pack(bref));
  HANDLE_CODE(sl_drx_gc_inactivity_timer_r17.pack(bref));
  HANDLE_CODE(sl_drx_gc_bc_cycle_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_gc_bc_qos_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_drx_gc_bc_mapped_qos_flow_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_drx_gc_bc_mapped_qos_flow_list_r17_present, 1));

  if (sl_drx_gc_bc_mapped_qos_flow_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_drx_gc_bc_mapped_qos_flow_list_r17, bref, 1, 2048));
  }
  HANDLE_CODE(sl_drx_gc_bc_on_dur_timer_r17.unpack(bref));
  HANDLE_CODE(sl_drx_gc_inactivity_timer_r17.unpack(bref));
  HANDLE_CODE(sl_drx_gc_bc_cycle_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_drx_gc_bc_qos_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_drx_gc_bc_mapped_qos_flow_list_r17.size() > 0) {
    j.start_array("sl-DRX-GC-BC-MappedQoS-FlowList-r17");
    for (const auto& e1 : sl_drx_gc_bc_mapped_qos_flow_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_fieldname("sl-DRX-GC-BC-OnDurationTimer-r17");
  sl_drx_gc_bc_on_dur_timer_r17.to_json(j);
  j.write_str("sl-DRX-GC-InactivityTimer-r17", sl_drx_gc_inactivity_timer_r17.to_string());
  j.write_str("sl-DRX-GC-BC-Cycle-r17", sl_drx_gc_bc_cycle_r17.to_string());
  j.end_obj();
}

void sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::destroy_() {}
void sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::sl_drx_gc_bc_on_dur_timer_r17_c_(
    const sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_& other)
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
      log_invalid_choice_id(type_, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_");
  }
}
sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_&
sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::operator=(
    const sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_& other)
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
      log_invalid_choice_id(type_, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_");
  }

  return *this;
}
uint8_t& sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_&
sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      names, 32, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_");
}
uint16_t sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(
      numbers, 24, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_");
}

const char* sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_opts::to_string() const
{
  static const char* names[] = {"ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",
                                "ms10",   "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",
                                "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_e_");
}
uint16_t sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_e_");
}

const char* sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",    "ms60",    "ms64",    "ms70",    "ms80",
                                "ms128",  "ms160",  "ms256",  "ms320",   "ms512",   "ms640",   "ms1024",  "ms1280",
                                "ms2048", "ms2560", "ms5120", "ms10240", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7", "spare6", "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_e_");
}
uint16_t sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_e_");
}

// SL-DRX-GC-Generic-r17 ::= SEQUENCE
SRSASN_CODE sl_drx_gc_generic_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_drx_gc_harq_rtt_timer1_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_drx_gc_harq_rtt_timer2_r17_present, 1));

  if (sl_drx_gc_harq_rtt_timer1_r17_present) {
    HANDLE_CODE(sl_drx_gc_harq_rtt_timer1_r17.pack(bref));
  }
  if (sl_drx_gc_harq_rtt_timer2_r17_present) {
    HANDLE_CODE(sl_drx_gc_harq_rtt_timer2_r17.pack(bref));
  }
  HANDLE_CODE(sl_drx_gc_retx_timer_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_gc_generic_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_drx_gc_harq_rtt_timer1_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_drx_gc_harq_rtt_timer2_r17_present, 1));

  if (sl_drx_gc_harq_rtt_timer1_r17_present) {
    HANDLE_CODE(sl_drx_gc_harq_rtt_timer1_r17.unpack(bref));
  }
  if (sl_drx_gc_harq_rtt_timer2_r17_present) {
    HANDLE_CODE(sl_drx_gc_harq_rtt_timer2_r17.unpack(bref));
  }
  HANDLE_CODE(sl_drx_gc_retx_timer_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_drx_gc_generic_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_drx_gc_harq_rtt_timer1_r17_present) {
    j.write_str("sl-DRX-GC-HARQ-RTT-Timer1-r17", sl_drx_gc_harq_rtt_timer1_r17.to_string());
  }
  if (sl_drx_gc_harq_rtt_timer2_r17_present) {
    j.write_str("sl-DRX-GC-HARQ-RTT-Timer2-r17", sl_drx_gc_harq_rtt_timer2_r17.to_string());
  }
  j.write_str("sl-DRX-GC-RetransmissionTimer-r17", sl_drx_gc_retx_timer_r17.to_string());
  j.end_obj();
}

const char* sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_opts::to_string() const
{
  static const char* names[] = {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_e_");
}
uint8_t sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_e_");
}

const char* sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_opts::to_string() const
{
  static const char* names[] = {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_e_");
}
uint8_t sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_e_");
}

const char* sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_opts::to_string() const
{
  static const char* names[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_e_");
}
uint16_t sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_e_");
}

// SL-MeasIdInfo-r16 ::= SEQUENCE
SRSASN_CODE sl_meas_id_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, sl_meas_id_r16, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, sl_meas_obj_id_r16, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, sl_report_cfg_id_r16, (uint8_t)1u, (uint8_t)64u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_id_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(sl_meas_id_r16, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(sl_meas_obj_id_r16, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(sl_report_cfg_id_r16, bref, (uint8_t)1u, (uint8_t)64u));

  return SRSASN_SUCCESS;
}
void sl_meas_id_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-MeasId-r16", sl_meas_id_r16);
  j.write_int("sl-MeasObjectId-r16", sl_meas_obj_id_r16);
  j.write_int("sl-ReportConfigId-r16", sl_report_cfg_id_r16);
  j.end_obj();
}

// SL-MeasObjectInfo-r16 ::= SEQUENCE
SRSASN_CODE sl_meas_obj_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, sl_meas_obj_id_r16, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(sl_meas_obj_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_obj_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(sl_meas_obj_id_r16, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(sl_meas_obj_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_meas_obj_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-MeasObjectId-r16", sl_meas_obj_id_r16);
  j.write_fieldname("sl-MeasObject-r16");
  sl_meas_obj_r16.to_json(j);
  j.end_obj();
}

// SL-PSSCH-TxConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_pssch_tx_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_type_tx_sync_r16_present, 1));

  if (sl_type_tx_sync_r16_present) {
    HANDLE_CODE(sl_type_tx_sync_r16.pack(bref));
  }
  HANDLE_CODE(sl_thres_ue_speed_r16.pack(bref));
  HANDLE_CODE(sl_params_above_thres_r16.pack(bref));
  HANDLE_CODE(sl_params_below_thres_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_params_above_thres_v1650.is_present();
    group_flags[0] |= sl_params_below_thres_v1650.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_params_above_thres_v1650.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_params_below_thres_v1650.is_present(), 1));
      if (sl_params_above_thres_v1650.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sl_params_above_thres_v1650, 1, 3));
      }
      if (sl_params_below_thres_v1650.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sl_params_below_thres_v1650, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pssch_tx_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_type_tx_sync_r16_present, 1));

  if (sl_type_tx_sync_r16_present) {
    HANDLE_CODE(sl_type_tx_sync_r16.unpack(bref));
  }
  HANDLE_CODE(sl_thres_ue_speed_r16.unpack(bref));
  HANDLE_CODE(sl_params_above_thres_r16.unpack(bref));
  HANDLE_CODE(sl_params_below_thres_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_params_above_thres_v1650, bref);
      unpack_presence_flag(sl_params_below_thres_v1650, bref);
      if (sl_params_above_thres_v1650.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sl_params_above_thres_v1650, bref, 1, 3));
      }
      if (sl_params_below_thres_v1650.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sl_params_below_thres_v1650, bref, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_pssch_tx_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_type_tx_sync_r16_present) {
    j.write_str("sl-TypeTxSync-r16", sl_type_tx_sync_r16.to_string());
  }
  j.write_str("sl-ThresUE-Speed-r16", sl_thres_ue_speed_r16.to_string());
  j.write_fieldname("sl-ParametersAboveThres-r16");
  sl_params_above_thres_r16.to_json(j);
  j.write_fieldname("sl-ParametersBelowThres-r16");
  sl_params_below_thres_r16.to_json(j);
  if (ext) {
    if (sl_params_above_thres_v1650.is_present()) {
      j.start_array("sl-ParametersAboveThres-v1650");
      for (const auto& e1 : *sl_params_above_thres_v1650) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (sl_params_below_thres_v1650.is_present()) {
      j.start_array("sl-ParametersBelowThres-v1650");
      for (const auto& e1 : *sl_params_below_thres_v1650) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_opts::to_string() const
{
  static const char* names[] = {"kmph60", "kmph80", "kmph100", "kmph120", "kmph140", "kmph160", "kmph180", "kmph200"};
  return convert_enum_idx(names, 8, value, "sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_e_");
}
uint8_t sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {60, 80, 100, 120, 140, 160, 180, 200};
  return map_enum_number(numbers, 8, value, "sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_e_");
}

// SL-ReportConfigInfo-r16 ::= SEQUENCE
SRSASN_CODE sl_report_cfg_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, sl_report_cfg_id_r16, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(sl_report_cfg_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_report_cfg_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(sl_report_cfg_id_r16, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(sl_report_cfg_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_report_cfg_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-ReportConfigId-r16", sl_report_cfg_id_r16);
  j.write_fieldname("sl-ReportConfig-r16");
  sl_report_cfg_r16.to_json(j);
  j.end_obj();
}

// SL-SyncConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_sync_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_sync_ref_min_hyst_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_sync_ref_diff_hyst_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_filt_coef_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ssb_time_alloc1_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ssb_time_alloc2_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ssb_time_alloc3_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_s_si_d_r16_present, 1));
  HANDLE_CODE(bref.pack(gnss_sync_r16_present, 1));

  if (sl_sync_ref_min_hyst_r16_present) {
    HANDLE_CODE(sl_sync_ref_min_hyst_r16.pack(bref));
  }
  if (sl_sync_ref_diff_hyst_r16_present) {
    HANDLE_CODE(sl_sync_ref_diff_hyst_r16.pack(bref));
  }
  if (sl_filt_coef_r16_present) {
    HANDLE_CODE(sl_filt_coef_r16.pack(bref));
  }
  if (sl_ssb_time_alloc1_r16_present) {
    HANDLE_CODE(sl_ssb_time_alloc1_r16.pack(bref));
  }
  if (sl_ssb_time_alloc2_r16_present) {
    HANDLE_CODE(sl_ssb_time_alloc2_r16.pack(bref));
  }
  if (sl_ssb_time_alloc3_r16_present) {
    HANDLE_CODE(sl_ssb_time_alloc3_r16.pack(bref));
  }
  if (sl_s_si_d_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_s_si_d_r16, (uint16_t)0u, (uint16_t)671u));
  }
  HANDLE_CODE(bref.pack(tx_params_r16.sync_tx_thresh_ic_r16_present, 1));
  HANDLE_CODE(bref.pack(tx_params_r16.sync_tx_thresh_oo_c_r16_present, 1));
  HANDLE_CODE(bref.pack(tx_params_r16.sync_info_reserved_r16_present, 1));
  if (tx_params_r16.sync_tx_thresh_ic_r16_present) {
    HANDLE_CODE(pack_integer(bref, tx_params_r16.sync_tx_thresh_ic_r16, (uint8_t)0u, (uint8_t)13u));
  }
  if (tx_params_r16.sync_tx_thresh_oo_c_r16_present) {
    HANDLE_CODE(pack_integer(bref, tx_params_r16.sync_tx_thresh_oo_c_r16, (uint8_t)0u, (uint8_t)13u));
  }
  if (tx_params_r16.sync_info_reserved_r16_present) {
    HANDLE_CODE(tx_params_r16.sync_info_reserved_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sync_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_sync_ref_min_hyst_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_sync_ref_diff_hyst_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_filt_coef_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ssb_time_alloc1_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ssb_time_alloc2_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ssb_time_alloc3_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_s_si_d_r16_present, 1));
  HANDLE_CODE(bref.unpack(gnss_sync_r16_present, 1));

  if (sl_sync_ref_min_hyst_r16_present) {
    HANDLE_CODE(sl_sync_ref_min_hyst_r16.unpack(bref));
  }
  if (sl_sync_ref_diff_hyst_r16_present) {
    HANDLE_CODE(sl_sync_ref_diff_hyst_r16.unpack(bref));
  }
  if (sl_filt_coef_r16_present) {
    HANDLE_CODE(sl_filt_coef_r16.unpack(bref));
  }
  if (sl_ssb_time_alloc1_r16_present) {
    HANDLE_CODE(sl_ssb_time_alloc1_r16.unpack(bref));
  }
  if (sl_ssb_time_alloc2_r16_present) {
    HANDLE_CODE(sl_ssb_time_alloc2_r16.unpack(bref));
  }
  if (sl_ssb_time_alloc3_r16_present) {
    HANDLE_CODE(sl_ssb_time_alloc3_r16.unpack(bref));
  }
  if (sl_s_si_d_r16_present) {
    HANDLE_CODE(unpack_integer(sl_s_si_d_r16, bref, (uint16_t)0u, (uint16_t)671u));
  }
  HANDLE_CODE(bref.unpack(tx_params_r16.sync_tx_thresh_ic_r16_present, 1));
  HANDLE_CODE(bref.unpack(tx_params_r16.sync_tx_thresh_oo_c_r16_present, 1));
  HANDLE_CODE(bref.unpack(tx_params_r16.sync_info_reserved_r16_present, 1));
  if (tx_params_r16.sync_tx_thresh_ic_r16_present) {
    HANDLE_CODE(unpack_integer(tx_params_r16.sync_tx_thresh_ic_r16, bref, (uint8_t)0u, (uint8_t)13u));
  }
  if (tx_params_r16.sync_tx_thresh_oo_c_r16_present) {
    HANDLE_CODE(unpack_integer(tx_params_r16.sync_tx_thresh_oo_c_r16, bref, (uint8_t)0u, (uint8_t)13u));
  }
  if (tx_params_r16.sync_info_reserved_r16_present) {
    HANDLE_CODE(tx_params_r16.sync_info_reserved_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_sync_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_sync_ref_min_hyst_r16_present) {
    j.write_str("sl-SyncRefMinHyst-r16", sl_sync_ref_min_hyst_r16.to_string());
  }
  if (sl_sync_ref_diff_hyst_r16_present) {
    j.write_str("sl-SyncRefDiffHyst-r16", sl_sync_ref_diff_hyst_r16.to_string());
  }
  if (sl_filt_coef_r16_present) {
    j.write_str("sl-filterCoefficient-r16", sl_filt_coef_r16.to_string());
  }
  if (sl_ssb_time_alloc1_r16_present) {
    j.write_fieldname("sl-SSB-TimeAllocation1-r16");
    sl_ssb_time_alloc1_r16.to_json(j);
  }
  if (sl_ssb_time_alloc2_r16_present) {
    j.write_fieldname("sl-SSB-TimeAllocation2-r16");
    sl_ssb_time_alloc2_r16.to_json(j);
  }
  if (sl_ssb_time_alloc3_r16_present) {
    j.write_fieldname("sl-SSB-TimeAllocation3-r16");
    sl_ssb_time_alloc3_r16.to_json(j);
  }
  if (sl_s_si_d_r16_present) {
    j.write_int("sl-SSID-r16", sl_s_si_d_r16);
  }
  j.write_fieldname("txParameters-r16");
  j.start_obj();
  if (tx_params_r16.sync_tx_thresh_ic_r16_present) {
    j.write_int("syncTxThreshIC-r16", tx_params_r16.sync_tx_thresh_ic_r16);
  }
  if (tx_params_r16.sync_tx_thresh_oo_c_r16_present) {
    j.write_int("syncTxThreshOoC-r16", tx_params_r16.sync_tx_thresh_oo_c_r16);
  }
  if (tx_params_r16.sync_info_reserved_r16_present) {
    j.write_str("syncInfoReserved-r16", tx_params_r16.sync_info_reserved_r16.to_string());
  }
  j.end_obj();
  if (gnss_sync_r16_present) {
    j.write_str("gnss-Sync-r16", "true");
  }
  j.end_obj();
}

const char* sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_opts::to_string() const
{
  static const char* names[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(names, 5, value, "sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_e_");
}
uint8_t sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 3, 6, 9, 12};
  return map_enum_number(numbers, 5, value, "sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_e_");
}

const char* sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_opts::to_string() const
{
  static const char* names[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(names, 6, value, "sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_e_");
}
uint8_t sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 3, 6, 9, 12};
  return map_enum_number(numbers, 5, value, "sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_e_");
}

// SN-FieldLengthAM ::= ENUMERATED
const char* sn_field_len_am_opts::to_string() const
{
  static const char* names[] = {"size12", "size18"};
  return convert_enum_idx(names, 2, value, "sn_field_len_am_e");
}
uint8_t sn_field_len_am_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "sn_field_len_am_e");
}

// SN-FieldLengthUM ::= ENUMERATED
const char* sn_field_len_um_opts::to_string() const
{
  static const char* names[] = {"size6", "size12"};
  return convert_enum_idx(names, 2, value, "sn_field_len_um_e");
}
uint8_t sn_field_len_um_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12};
  return map_enum_number(numbers, 2, value, "sn_field_len_um_e");
}

// T-PollRetransmit ::= ENUMERATED
const char* t_poll_retx_opts::to_string() const
{
  static const char* names[] = {
      "ms5",       "ms10",      "ms15",      "ms20",  "ms25",  "ms30",  "ms35",   "ms40",   "ms45",   "ms50",
      "ms55",      "ms60",      "ms65",      "ms70",  "ms75",  "ms80",  "ms85",   "ms90",   "ms95",   "ms100",
      "ms105",     "ms110",     "ms115",     "ms120", "ms125", "ms130", "ms135",  "ms140",  "ms145",  "ms150",
      "ms155",     "ms160",     "ms165",     "ms170", "ms175", "ms180", "ms185",  "ms190",  "ms195",  "ms200",
      "ms205",     "ms210",     "ms215",     "ms220", "ms225", "ms230", "ms235",  "ms240",  "ms245",  "ms250",
      "ms300",     "ms350",     "ms400",     "ms450", "ms500", "ms800", "ms1000", "ms2000", "ms4000", "ms1-v1610",
      "ms2-v1610", "ms3-v1610", "ms4-v1610", "spare1"};
  return convert_enum_idx(names, 64, value, "t_poll_retx_e");
}
uint16_t t_poll_retx_opts::to_number() const
{
  static const uint16_t numbers[] = {5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,  65,  70,  75,  80,
                                     85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140, 145, 150, 155, 160,
                                     165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220, 225, 230, 235, 240,
                                     245, 250, 300, 350, 400, 450, 500, 800, 1000, 2000, 4000, 1,   2,   3,   4};
  return map_enum_number(numbers, 63, value, "t_poll_retx_e");
}

// DiscardTimerExt-r16 ::= ENUMERATED
const char* discard_timer_ext_r16_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms1", "ms2", "ms4", "ms6", "ms8", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "discard_timer_ext_r16_e");
}
float discard_timer_ext_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 4.0, 6.0, 8.0};
  return map_enum_number(numbers, 6, value, "discard_timer_ext_r16_e");
}
const char* discard_timer_ext_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "4", "6", "8"};
  return convert_enum_idx(number_strs, 8, value, "discard_timer_ext_r16_e");
}

// DiscardTimerExt2-r17 ::= ENUMERATED
const char* discard_timer_ext2_r17_opts::to_string() const
{
  static const char* names[] = {"ms2000", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "discard_timer_ext2_r17_e");
}
uint16_t discard_timer_ext2_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {2000};
  return map_enum_number(numbers, 1, value, "discard_timer_ext2_r17_e");
}

// EthernetHeaderCompression-r16 ::= SEQUENCE
SRSASN_CODE ethernet_hdr_compress_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ehc_dl_r16_present, 1));
  HANDLE_CODE(bref.pack(ehc_ul_r16_present, 1));

  bref.pack(ehc_common_r16.ext, 1);
  HANDLE_CODE(ehc_common_r16.ehc_c_id_len_r16.pack(bref));
  if (ehc_dl_r16_present) {
    bref.pack(ehc_dl_r16.ext, 1);
    HANDLE_CODE(bref.pack(ehc_dl_r16.drb_continue_ehc_dl_r16_present, 1));
  }
  if (ehc_ul_r16_present) {
    bref.pack(ehc_ul_r16.ext, 1);
    HANDLE_CODE(bref.pack(ehc_ul_r16.drb_continue_ehc_ul_r16_present, 1));
    HANDLE_CODE(pack_integer(bref, ehc_ul_r16.max_c_id_ehc_ul_r16, (uint16_t)1u, (uint16_t)32767u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ethernet_hdr_compress_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ehc_dl_r16_present, 1));
  HANDLE_CODE(bref.unpack(ehc_ul_r16_present, 1));

  bref.unpack(ehc_common_r16.ext, 1);
  HANDLE_CODE(ehc_common_r16.ehc_c_id_len_r16.unpack(bref));
  if (ehc_dl_r16_present) {
    bref.unpack(ehc_dl_r16.ext, 1);
    HANDLE_CODE(bref.unpack(ehc_dl_r16.drb_continue_ehc_dl_r16_present, 1));
  }
  if (ehc_ul_r16_present) {
    bref.unpack(ehc_ul_r16.ext, 1);
    HANDLE_CODE(bref.unpack(ehc_ul_r16.drb_continue_ehc_ul_r16_present, 1));
    HANDLE_CODE(unpack_integer(ehc_ul_r16.max_c_id_ehc_ul_r16, bref, (uint16_t)1u, (uint16_t)32767u));
  }

  return SRSASN_SUCCESS;
}
void ethernet_hdr_compress_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ehc-Common-r16");
  j.start_obj();
  j.write_str("ehc-CID-Length-r16", ehc_common_r16.ehc_c_id_len_r16.to_string());
  j.end_obj();
  if (ehc_dl_r16_present) {
    j.write_fieldname("ehc-Downlink-r16");
    j.start_obj();
    if (ehc_dl_r16.drb_continue_ehc_dl_r16_present) {
      j.write_str("drb-ContinueEHC-DL-r16", "true");
    }
    j.end_obj();
  }
  if (ehc_ul_r16_present) {
    j.write_fieldname("ehc-Uplink-r16");
    j.start_obj();
    j.write_int("maxCID-EHC-UL-r16", ehc_ul_r16.max_c_id_ehc_ul_r16);
    if (ehc_ul_r16.drb_continue_ehc_ul_r16_present) {
      j.write_str("drb-ContinueEHC-UL-r16", "true");
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_opts::to_string() const
{
  static const char* names[] = {"bits7", "bits15"};
  return convert_enum_idx(names, 2, value, "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_e_");
}
uint8_t ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 15};
  return map_enum_number(numbers, 2, value, "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_e_");
}

// MAC-MainConfigSL-r16 ::= SEQUENCE
SRSASN_CODE mac_main_cfg_sl_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_bsr_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(ul_prioritization_thres_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_prioritization_thres_r16_present, 1));

  if (sl_bsr_cfg_r16_present) {
    HANDLE_CODE(sl_bsr_cfg_r16.pack(bref));
  }
  if (ul_prioritization_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, ul_prioritization_thres_r16, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_prioritization_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_prioritization_thres_r16, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_sl_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_bsr_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(ul_prioritization_thres_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_prioritization_thres_r16_present, 1));

  if (sl_bsr_cfg_r16_present) {
    HANDLE_CODE(sl_bsr_cfg_r16.unpack(bref));
  }
  if (ul_prioritization_thres_r16_present) {
    HANDLE_CODE(unpack_integer(ul_prioritization_thres_r16, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_prioritization_thres_r16_present) {
    HANDLE_CODE(unpack_integer(sl_prioritization_thres_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void mac_main_cfg_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_bsr_cfg_r16_present) {
    j.write_fieldname("sl-BSR-Config-r16");
    sl_bsr_cfg_r16.to_json(j);
  }
  if (ul_prioritization_thres_r16_present) {
    j.write_int("ul-PrioritizationThres-r16", ul_prioritization_thres_r16);
  }
  if (sl_prioritization_thres_r16_present) {
    j.write_int("sl-PrioritizationThres-r16", sl_prioritization_thres_r16);
  }
  j.end_obj();
}

// SL-BWP-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_bwp_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_bwp_generic_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_bwp_pool_cfg_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_bwp_id, (uint8_t)0u, (uint8_t)4u));
  if (sl_bwp_generic_r16_present) {
    HANDLE_CODE(sl_bwp_generic_r16.pack(bref));
  }
  if (sl_bwp_pool_cfg_r16_present) {
    HANDLE_CODE(sl_bwp_pool_cfg_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_bwp_pool_cfg_ps_r17.is_present();
    group_flags[0] |= sl_bwp_disc_pool_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_bwp_pool_cfg_ps_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_bwp_disc_pool_cfg_r17.is_present(), 1));
      if (sl_bwp_pool_cfg_ps_r17.is_present()) {
        HANDLE_CODE(sl_bwp_pool_cfg_ps_r17->pack(bref));
      }
      if (sl_bwp_disc_pool_cfg_r17.is_present()) {
        HANDLE_CODE(sl_bwp_disc_pool_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_bwp_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_bwp_generic_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_bwp_pool_cfg_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  if (sl_bwp_generic_r16_present) {
    HANDLE_CODE(sl_bwp_generic_r16.unpack(bref));
  }
  if (sl_bwp_pool_cfg_r16_present) {
    HANDLE_CODE(sl_bwp_pool_cfg_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_bwp_pool_cfg_ps_r17, bref);
      unpack_presence_flag(sl_bwp_disc_pool_cfg_r17, bref);
      if (sl_bwp_pool_cfg_ps_r17.is_present()) {
        HANDLE_CODE(sl_bwp_pool_cfg_ps_r17->unpack(bref));
      }
      if (sl_bwp_disc_pool_cfg_r17.is_present()) {
        HANDLE_CODE(sl_bwp_disc_pool_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_bwp_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-BWP-Id", sl_bwp_id);
  if (sl_bwp_generic_r16_present) {
    j.write_fieldname("sl-BWP-Generic-r16");
    sl_bwp_generic_r16.to_json(j);
  }
  if (sl_bwp_pool_cfg_r16_present) {
    j.write_fieldname("sl-BWP-PoolConfig-r16");
    sl_bwp_pool_cfg_r16.to_json(j);
  }
  if (ext) {
    if (sl_bwp_pool_cfg_ps_r17.is_present()) {
      j.write_fieldname("sl-BWP-PoolConfigPS-r17");
      sl_bwp_pool_cfg_ps_r17->to_json(j);
    }
    if (sl_bwp_disc_pool_cfg_r17.is_present()) {
      j.write_fieldname("sl-BWP-DiscPoolConfig-r17");
      sl_bwp_disc_pool_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

// SL-CBR-CommonTxConfigList-r16 ::= SEQUENCE
SRSASN_CODE sl_cbr_common_tx_cfg_list_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_cbr_range_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_cbr_pssch_tx_cfg_list_r16.size() > 0, 1));

  if (sl_cbr_range_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                sl_cbr_range_cfg_list_r16,
                                1,
                                8,
                                SeqOfPacker<integer_packer<uint8_t>>(1, 16, integer_packer<uint8_t>(0, 100))));
  }
  if (sl_cbr_pssch_tx_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_cbr_pssch_tx_cfg_list_r16, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cbr_common_tx_cfg_list_r16_s::unpack(cbit_ref& bref)
{
  bool sl_cbr_range_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_cbr_range_cfg_list_r16_present, 1));
  bool sl_cbr_pssch_tx_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_cbr_pssch_tx_cfg_list_r16_present, 1));

  if (sl_cbr_range_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_cbr_range_cfg_list_r16,
                                  bref,
                                  1,
                                  8,
                                  SeqOfPacker<integer_packer<uint8_t>>(1, 16, integer_packer<uint8_t>(0, 100))));
  }
  if (sl_cbr_pssch_tx_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_cbr_pssch_tx_cfg_list_r16, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void sl_cbr_common_tx_cfg_list_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_cbr_range_cfg_list_r16.size() > 0) {
    j.start_array("sl-CBR-RangeConfigList-r16");
    for (const auto& e1 : sl_cbr_range_cfg_list_r16) {
      j.start_array();
      for (const auto& e2 : e1) {
        j.write_int(e2);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (sl_cbr_pssch_tx_cfg_list_r16.size() > 0) {
    j.start_array("sl-CBR-PSSCH-TxConfigList-r16");
    for (const auto& e1 : sl_cbr_pssch_tx_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-ConfiguredGrantConfigList-r16 ::= SEQUENCE
SRSASN_CODE sl_cfg_grant_cfg_list_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_cfg_grant_cfg_to_release_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_cfg_grant_cfg_to_add_mod_list_r16.size() > 0, 1));

  if (sl_cfg_grant_cfg_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_cfg_grant_cfg_to_release_list_r16, 1, 8, integer_packer<uint8_t>(0, 7)));
  }
  if (sl_cfg_grant_cfg_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_cfg_grant_cfg_to_add_mod_list_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cfg_grant_cfg_list_r16_s::unpack(cbit_ref& bref)
{
  bool sl_cfg_grant_cfg_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_cfg_grant_cfg_to_release_list_r16_present, 1));
  bool sl_cfg_grant_cfg_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_cfg_grant_cfg_to_add_mod_list_r16_present, 1));

  if (sl_cfg_grant_cfg_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_cfg_grant_cfg_to_release_list_r16, bref, 1, 8, integer_packer<uint8_t>(0, 7)));
  }
  if (sl_cfg_grant_cfg_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_cfg_grant_cfg_to_add_mod_list_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void sl_cfg_grant_cfg_list_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_cfg_grant_cfg_to_release_list_r16.size() > 0) {
    j.start_array("sl-ConfiguredGrantConfigToReleaseList-r16");
    for (const auto& e1 : sl_cfg_grant_cfg_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_cfg_grant_cfg_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-ConfiguredGrantConfigToAddModList-r16");
    for (const auto& e1 : sl_cfg_grant_cfg_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-DRX-ConfigGC-BC-r17 ::= SEQUENCE
SRSASN_CODE sl_drx_cfg_gc_bc_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_drx_gc_bc_per_qos_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_drx_gc_generic_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_default_drx_gc_bc_r17_present, 1));

  if (sl_drx_gc_bc_per_qos_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drx_gc_bc_per_qos_list_r17, 1, 16));
  }
  if (sl_drx_gc_generic_r17_present) {
    HANDLE_CODE(sl_drx_gc_generic_r17.pack(bref));
  }
  if (sl_default_drx_gc_bc_r17_present) {
    HANDLE_CODE(sl_default_drx_gc_bc_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_gc_bc_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_drx_gc_bc_per_qos_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_drx_gc_bc_per_qos_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_drx_gc_generic_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_default_drx_gc_bc_r17_present, 1));

  if (sl_drx_gc_bc_per_qos_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_drx_gc_bc_per_qos_list_r17, bref, 1, 16));
  }
  if (sl_drx_gc_generic_r17_present) {
    HANDLE_CODE(sl_drx_gc_generic_r17.unpack(bref));
  }
  if (sl_default_drx_gc_bc_r17_present) {
    HANDLE_CODE(sl_default_drx_gc_bc_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_drx_cfg_gc_bc_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_drx_gc_bc_per_qos_list_r17.size() > 0) {
    j.start_array("sl-DRX-GC-BC-PerQoS-List-r17");
    for (const auto& e1 : sl_drx_gc_bc_per_qos_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drx_gc_generic_r17_present) {
    j.write_fieldname("sl-DRX-GC-generic-r17");
    sl_drx_gc_generic_r17.to_json(j);
  }
  if (sl_default_drx_gc_bc_r17_present) {
    j.write_fieldname("sl-DefaultDRX-GC-BC-r17");
    sl_default_drx_gc_bc_r17.to_json(j);
  }
  j.end_obj();
}

// SL-DRX-ConfigUC-Info-r17 ::= SEQUENCE
SRSASN_CODE sl_drx_cfg_uc_info_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_dest_idx_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_drx_cfg_uc_r17_present, 1));

  if (sl_dest_idx_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_dest_idx_r17, (uint8_t)0u, (uint8_t)31u));
  }
  if (sl_drx_cfg_uc_r17_present) {
    HANDLE_CODE(sl_drx_cfg_uc_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_uc_info_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_dest_idx_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_drx_cfg_uc_r17_present, 1));

  if (sl_dest_idx_r17_present) {
    HANDLE_CODE(unpack_integer(sl_dest_idx_r17, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (sl_drx_cfg_uc_r17_present) {
    HANDLE_CODE(sl_drx_cfg_uc_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_drx_cfg_uc_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_dest_idx_r17_present) {
    j.write_int("sl-DestinationIndex-r17", sl_dest_idx_r17);
  }
  if (sl_drx_cfg_uc_r17_present) {
    j.write_fieldname("sl-DRX-ConfigUC-r17");
    sl_drx_cfg_uc_r17.to_json(j);
  }
  j.end_obj();
}

// SL-LogicalChannelConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_lc_ch_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_cfg_grant_type1_allowed_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_harq_feedback_enabled_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_allowed_cg_list_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_allowed_scs_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_max_pusch_dur_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_lc_ch_group_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_sched_request_id_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_lc_ch_sr_delay_timer_applied_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_prio_r16, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(sl_prioritised_bit_rate_r16.pack(bref));
  HANDLE_CODE(sl_bucket_size_dur_r16.pack(bref));
  if (sl_harq_feedback_enabled_r16_present) {
    HANDLE_CODE(sl_harq_feedback_enabled_r16.pack(bref));
  }
  if (sl_allowed_cg_list_r16_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_allowed_cg_list_r16, 0, 7, integer_packer<uint8_t>(0, 7)));
  }
  if (sl_allowed_scs_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_allowed_scs_list_r16, 1, 5));
  }
  if (sl_max_pusch_dur_r16_present) {
    HANDLE_CODE(sl_max_pusch_dur_r16.pack(bref));
  }
  if (sl_lc_ch_group_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_lc_ch_group_r16, (uint8_t)0u, (uint8_t)7u));
  }
  if (sl_sched_request_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_sched_request_id_r16, (uint8_t)0u, (uint8_t)7u));
  }
  if (sl_lc_ch_sr_delay_timer_applied_r16_present) {
    HANDLE_CODE(bref.pack(sl_lc_ch_sr_delay_timer_applied_r16, 1));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_lc_ch_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_cfg_grant_type1_allowed_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_harq_feedback_enabled_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_allowed_cg_list_r16_present, 1));
  bool sl_allowed_scs_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_allowed_scs_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_pusch_dur_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_lc_ch_group_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_sched_request_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_lc_ch_sr_delay_timer_applied_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_prio_r16, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(sl_prioritised_bit_rate_r16.unpack(bref));
  HANDLE_CODE(sl_bucket_size_dur_r16.unpack(bref));
  if (sl_harq_feedback_enabled_r16_present) {
    HANDLE_CODE(sl_harq_feedback_enabled_r16.unpack(bref));
  }
  if (sl_allowed_cg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_allowed_cg_list_r16, bref, 0, 7, integer_packer<uint8_t>(0, 7)));
  }
  if (sl_allowed_scs_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_allowed_scs_list_r16, bref, 1, 5));
  }
  if (sl_max_pusch_dur_r16_present) {
    HANDLE_CODE(sl_max_pusch_dur_r16.unpack(bref));
  }
  if (sl_lc_ch_group_r16_present) {
    HANDLE_CODE(unpack_integer(sl_lc_ch_group_r16, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (sl_sched_request_id_r16_present) {
    HANDLE_CODE(unpack_integer(sl_sched_request_id_r16, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (sl_lc_ch_sr_delay_timer_applied_r16_present) {
    HANDLE_CODE(bref.unpack(sl_lc_ch_sr_delay_timer_applied_r16, 1));
  }

  return SRSASN_SUCCESS;
}
void sl_lc_ch_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-Priority-r16", sl_prio_r16);
  j.write_str("sl-PrioritisedBitRate-r16", sl_prioritised_bit_rate_r16.to_string());
  j.write_str("sl-BucketSizeDuration-r16", sl_bucket_size_dur_r16.to_string());
  if (sl_cfg_grant_type1_allowed_r16_present) {
    j.write_str("sl-ConfiguredGrantType1Allowed-r16", "true");
  }
  if (sl_harq_feedback_enabled_r16_present) {
    j.write_str("sl-HARQ-FeedbackEnabled-r16", sl_harq_feedback_enabled_r16.to_string());
  }
  if (sl_allowed_cg_list_r16_present) {
    j.start_array("sl-AllowedCG-List-r16");
    for (const auto& e1 : sl_allowed_cg_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_allowed_scs_list_r16.size() > 0) {
    j.start_array("sl-AllowedSCS-List-r16");
    for (const auto& e1 : sl_allowed_scs_list_r16) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (sl_max_pusch_dur_r16_present) {
    j.write_str("sl-MaxPUSCH-Duration-r16", sl_max_pusch_dur_r16.to_string());
  }
  if (sl_lc_ch_group_r16_present) {
    j.write_int("sl-LogicalChannelGroup-r16", sl_lc_ch_group_r16);
  }
  if (sl_sched_request_id_r16_present) {
    j.write_int("sl-SchedulingRequestId-r16", sl_sched_request_id_r16);
  }
  if (sl_lc_ch_sr_delay_timer_applied_r16_present) {
    j.write_bool("sl-LogicalChannelSR-DelayTimerApplied-r16", sl_lc_ch_sr_delay_timer_applied_r16);
  }
  j.end_obj();
}

const char* sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_opts::to_string() const
{
  static const char* names[] = {"kBps0",
                                "kBps8",
                                "kBps16",
                                "kBps32",
                                "kBps64",
                                "kBps128",
                                "kBps256",
                                "kBps512",
                                "kBps1024",
                                "kBps2048",
                                "kBps4096",
                                "kBps8192",
                                "kBps16384",
                                "kBps32768",
                                "kBps65536",
                                "infinity"};
  return convert_enum_idx(names, 16, value, "sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_e_");
}
int32_t sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_opts::to_number() const
{
  static const int32_t numbers[] = {0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, -1};
  return map_enum_number(numbers, 16, value, "sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_e_");
}

const char* sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_opts::to_string() const
{
  static const char* names[] = {"ms5",
                                "ms10",
                                "ms20",
                                "ms50",
                                "ms100",
                                "ms150",
                                "ms300",
                                "ms500",
                                "ms1000",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_e_");
}
uint16_t sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 50, 100, 150, 300, 500, 1000};
  return map_enum_number(numbers, 9, value, "sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_e_");
}

const char* sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16_e_");
}

const char* sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16_opts::to_string() const
{
  static const char* names[] = {"ms0p02", "ms0p04", "ms0p0625", "ms0p125", "ms0p25", "ms0p5", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16_e_");
}

// SL-MappedQoS-FlowsListDedicated-r16 ::= SEQUENCE
SRSASN_CODE sl_mapped_qos_flows_list_ded_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_mapped_qos_flows_to_add_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_mapped_qos_flows_to_release_list_r16.size() > 0, 1));

  if (sl_mapped_qos_flows_to_add_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_mapped_qos_flows_to_add_list_r16, 1, 2048, integer_packer<uint16_t>(1, 2048)));
  }
  if (sl_mapped_qos_flows_to_release_list_r16.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, sl_mapped_qos_flows_to_release_list_r16, 1, 2048, integer_packer<uint16_t>(1, 2048)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_mapped_qos_flows_list_ded_r16_s::unpack(cbit_ref& bref)
{
  bool sl_mapped_qos_flows_to_add_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_mapped_qos_flows_to_add_list_r16_present, 1));
  bool sl_mapped_qos_flows_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_mapped_qos_flows_to_release_list_r16_present, 1));

  if (sl_mapped_qos_flows_to_add_list_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_mapped_qos_flows_to_add_list_r16, bref, 1, 2048, integer_packer<uint16_t>(1, 2048)));
  }
  if (sl_mapped_qos_flows_to_release_list_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_mapped_qos_flows_to_release_list_r16, bref, 1, 2048, integer_packer<uint16_t>(1, 2048)));
  }

  return SRSASN_SUCCESS;
}
void sl_mapped_qos_flows_list_ded_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_mapped_qos_flows_to_add_list_r16.size() > 0) {
    j.start_array("sl-MappedQoS-FlowsToAddList-r16");
    for (const auto& e1 : sl_mapped_qos_flows_to_add_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_mapped_qos_flows_to_release_list_r16.size() > 0) {
    j.start_array("sl-MappedQoS-FlowsToReleaseList-r16");
    for (const auto& e1 : sl_mapped_qos_flows_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-QuantityConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_quant_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_filt_coef_dmrs_r16_present, 1));

  if (sl_filt_coef_dmrs_r16_present) {
    HANDLE_CODE(sl_filt_coef_dmrs_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_quant_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_filt_coef_dmrs_r16_present, 1));

  if (sl_filt_coef_dmrs_r16_present) {
    HANDLE_CODE(sl_filt_coef_dmrs_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_quant_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_filt_coef_dmrs_r16_present) {
    j.write_str("sl-FilterCoefficientDMRS-r16", sl_filt_coef_dmrs_r16.to_string());
  }
  j.end_obj();
}

// SL-RLC-Config-r16 ::= CHOICE
void sl_rlc_cfg_r16_c::destroy_()
{
  switch (type_) {
    case types::sl_am_rlc_r16:
      c.destroy<sl_am_rlc_r16_s_>();
      break;
    case types::sl_um_rlc_r16:
      c.destroy<sl_um_rlc_r16_s_>();
      break;
    default:
      break;
  }
}
void sl_rlc_cfg_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sl_am_rlc_r16:
      c.init<sl_am_rlc_r16_s_>();
      break;
    case types::sl_um_rlc_r16:
      c.init<sl_um_rlc_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_rlc_cfg_r16_c");
  }
}
sl_rlc_cfg_r16_c::sl_rlc_cfg_r16_c(const sl_rlc_cfg_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl_am_rlc_r16:
      c.init(other.c.get<sl_am_rlc_r16_s_>());
      break;
    case types::sl_um_rlc_r16:
      c.init(other.c.get<sl_um_rlc_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_rlc_cfg_r16_c");
  }
}
sl_rlc_cfg_r16_c& sl_rlc_cfg_r16_c::operator=(const sl_rlc_cfg_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl_am_rlc_r16:
      c.set(other.c.get<sl_am_rlc_r16_s_>());
      break;
    case types::sl_um_rlc_r16:
      c.set(other.c.get<sl_um_rlc_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_rlc_cfg_r16_c");
  }

  return *this;
}
sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_& sl_rlc_cfg_r16_c::set_sl_am_rlc_r16()
{
  set(types::sl_am_rlc_r16);
  return c.get<sl_am_rlc_r16_s_>();
}
sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_& sl_rlc_cfg_r16_c::set_sl_um_rlc_r16()
{
  set(types::sl_um_rlc_r16);
  return c.get<sl_um_rlc_r16_s_>();
}
void sl_rlc_cfg_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl_am_rlc_r16:
      j.write_fieldname("sl-AM-RLC-r16");
      j.start_obj();
      if (c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16_present) {
        j.write_str("sl-SN-FieldLengthAM-r16", c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16.to_string());
      }
      j.write_str("sl-T-PollRetransmit-r16", c.get<sl_am_rlc_r16_s_>().sl_t_poll_retx_r16.to_string());
      j.write_str("sl-PollPDU-r16", c.get<sl_am_rlc_r16_s_>().sl_poll_pdu_r16.to_string());
      j.write_str("sl-PollByte-r16", c.get<sl_am_rlc_r16_s_>().sl_poll_byte_r16.to_string());
      j.write_str("sl-MaxRetxThreshold-r16", c.get<sl_am_rlc_r16_s_>().sl_max_retx_thres_r16.to_string());
      j.end_obj();
      break;
    case types::sl_um_rlc_r16:
      j.write_fieldname("sl-UM-RLC-r16");
      j.start_obj();
      if (c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16_present) {
        j.write_str("sl-SN-FieldLengthUM-r16", c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sl_rlc_cfg_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_rlc_cfg_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl_am_rlc_r16:
      bref.pack(c.get<sl_am_rlc_r16_s_>().ext, 1);
      HANDLE_CODE(bref.pack(c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16_present, 1));
      if (c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16_present) {
        HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16.pack(bref));
      }
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_t_poll_retx_r16.pack(bref));
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_poll_pdu_r16.pack(bref));
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_poll_byte_r16.pack(bref));
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_max_retx_thres_r16.pack(bref));
      break;
    case types::sl_um_rlc_r16:
      bref.pack(c.get<sl_um_rlc_r16_s_>().ext, 1);
      HANDLE_CODE(bref.pack(c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16_present, 1));
      if (c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16_present) {
        HANDLE_CODE(c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_rlc_cfg_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_rlc_cfg_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl_am_rlc_r16:
      bref.unpack(c.get<sl_am_rlc_r16_s_>().ext, 1);
      HANDLE_CODE(bref.unpack(c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16_present, 1));
      if (c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16_present) {
        HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_sn_field_len_am_r16.unpack(bref));
      }
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_t_poll_retx_r16.unpack(bref));
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_poll_pdu_r16.unpack(bref));
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_poll_byte_r16.unpack(bref));
      HANDLE_CODE(c.get<sl_am_rlc_r16_s_>().sl_max_retx_thres_r16.unpack(bref));
      break;
    case types::sl_um_rlc_r16:
      bref.unpack(c.get<sl_um_rlc_r16_s_>().ext, 1);
      HANDLE_CODE(bref.unpack(c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16_present, 1));
      if (c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16_present) {
        HANDLE_CODE(c.get<sl_um_rlc_r16_s_>().sl_sn_field_len_um_r16.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sl_rlc_cfg_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_opts::to_string() const
{
  static const char* names[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(names, 8, value, "sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_e_");
}
uint8_t sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_e_");
}

// SL-RemoteUE-RB-Identity-r17 ::= CHOICE
void sl_remote_ue_rb_id_r17_c::destroy_() {}
void sl_remote_ue_rb_id_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
sl_remote_ue_rb_id_r17_c::sl_remote_ue_rb_id_r17_c(const sl_remote_ue_rb_id_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::srb_id_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::drb_id_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_remote_ue_rb_id_r17_c");
  }
}
sl_remote_ue_rb_id_r17_c& sl_remote_ue_rb_id_r17_c::operator=(const sl_remote_ue_rb_id_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::srb_id_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::drb_id_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_remote_ue_rb_id_r17_c");
  }

  return *this;
}
uint8_t& sl_remote_ue_rb_id_r17_c::set_srb_id_r17()
{
  set(types::srb_id_r17);
  return c.get<uint8_t>();
}
uint8_t& sl_remote_ue_rb_id_r17_c::set_drb_id_r17()
{
  set(types::drb_id_r17);
  return c.get<uint8_t>();
}
void sl_remote_ue_rb_id_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::srb_id_r17:
      j.write_int("srb-Identity-r17", c.get<uint8_t>());
      break;
    case types::drb_id_r17:
      j.write_int("drb-Identity-r17", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sl_remote_ue_rb_id_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE sl_remote_ue_rb_id_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::srb_id_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::drb_id_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)32u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_remote_ue_rb_id_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_remote_ue_rb_id_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::srb_id_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::drb_id_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)32u));
      break;
    default:
      log_invalid_choice_id(type_, "sl_remote_ue_rb_id_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-ReselectionConfig-r17 ::= SEQUENCE
SRSASN_CODE sl_resel_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_rsrp_thresh_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_filt_coef_rsrp_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_hyst_min_r17_present, 1));

  if (sl_rsrp_thresh_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_rsrp_thresh_r17, (uint8_t)0u, (uint8_t)13u));
  }
  if (sl_filt_coef_rsrp_r17_present) {
    HANDLE_CODE(sl_filt_coef_rsrp_r17.pack(bref));
  }
  if (sl_hyst_min_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_hyst_min_r17, (uint8_t)0u, (uint8_t)30u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_resel_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_rsrp_thresh_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_filt_coef_rsrp_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_hyst_min_r17_present, 1));

  if (sl_rsrp_thresh_r17_present) {
    HANDLE_CODE(unpack_integer(sl_rsrp_thresh_r17, bref, (uint8_t)0u, (uint8_t)13u));
  }
  if (sl_filt_coef_rsrp_r17_present) {
    HANDLE_CODE(sl_filt_coef_rsrp_r17.unpack(bref));
  }
  if (sl_hyst_min_r17_present) {
    HANDLE_CODE(unpack_integer(sl_hyst_min_r17, bref, (uint8_t)0u, (uint8_t)30u));
  }

  return SRSASN_SUCCESS;
}
void sl_resel_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_rsrp_thresh_r17_present) {
    j.write_int("sl-RSRP-Thresh-r17", sl_rsrp_thresh_r17);
  }
  if (sl_filt_coef_rsrp_r17_present) {
    j.write_str("sl-FilterCoefficientRSRP-r17", sl_filt_coef_rsrp_r17.to_string());
  }
  if (sl_hyst_min_r17_present) {
    j.write_int("sl-HystMin-r17", sl_hyst_min_r17);
  }
  j.end_obj();
}

// UL-DataSplitThreshold ::= ENUMERATED
const char* ul_data_split_thres_opts::to_string() const
{
  static const char* names[] = {"b0",       "b100",     "b200",     "b400",     "b800",     "b1600",    "b3200",
                                "b6400",    "b12800",   "b25600",   "b51200",   "b102400",  "b204800",  "b409600",
                                "b819200",  "b1228800", "b1638400", "b2457600", "b3276800", "b4096000", "b4915200",
                                "b5734400", "b6553600", "infinity", "spare8",   "spare7",   "spare6",   "spare5",
                                "spare4",   "spare3",   "spare2",   "spare1"};
  return convert_enum_idx(names, 32, value, "ul_data_split_thres_e");
}
int32_t ul_data_split_thres_opts::to_number() const
{
  static const int32_t numbers[] = {0,       100,     200,     400,     800,     1600,    3200,    6400,
                                    12800,   25600,   51200,   102400,  204800,  409600,  819200,  1228800,
                                    1638400, 2457600, 3276800, 4096000, 4915200, 5734400, 6553600, -1};
  return map_enum_number(numbers, 24, value, "ul_data_split_thres_e");
}

// UplinkDataCompression-r17 ::= CHOICE
void ul_data_compress_r17_c::set(types::options e)
{
  type_ = e;
}
ul_data_compress_r17_c::new_setup_s_& ul_data_compress_r17_c::set_new_setup()
{
  set(types::new_setup);
  return c;
}
void ul_data_compress_r17_c::set_drb_continue_udc()
{
  set(types::drb_continue_udc);
}
void ul_data_compress_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::new_setup:
      j.write_fieldname("newSetup");
      j.start_obj();
      j.write_str("bufferSize-r17", c.buffer_size_r17.to_string());
      if (c.dictionary_r17_present) {
        j.write_str("dictionary-r17", c.dictionary_r17.to_string());
      }
      j.end_obj();
      break;
    case types::drb_continue_udc:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_compress_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_data_compress_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::new_setup:
      HANDLE_CODE(bref.pack(c.dictionary_r17_present, 1));
      HANDLE_CODE(c.buffer_size_r17.pack(bref));
      if (c.dictionary_r17_present) {
        HANDLE_CODE(c.dictionary_r17.pack(bref));
      }
      break;
    case types::drb_continue_udc:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_compress_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_data_compress_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::new_setup:
      HANDLE_CODE(bref.unpack(c.dictionary_r17_present, 1));
      HANDLE_CODE(c.buffer_size_r17.unpack(bref));
      if (c.dictionary_r17_present) {
        HANDLE_CODE(c.dictionary_r17.unpack(bref));
      }
      break;
    case types::drb_continue_udc:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_compress_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_opts::to_string() const
{
  static const char* names[] = {"kbyte2", "kbyte4", "kbyte8", "spare1"};
  return convert_enum_idx(names, 4, value, "ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_e_");
}
uint8_t ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_e_");
}

const char* ul_data_compress_r17_c::new_setup_s_::dictionary_r17_opts::to_string() const
{
  static const char* names[] = {"sip-SDP", "operator"};
  return convert_enum_idx(names, 2, value, "ul_data_compress_r17_c::new_setup_s_::dictionary_r17_e_");
}

// CipheringAlgorithm ::= ENUMERATED
const char* ciphering_algorithm_opts::to_string() const
{
  static const char* names[] = {"nea0", "nea1", "nea2", "nea3", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ciphering_algorithm_e");
}
uint8_t ciphering_algorithm_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "ciphering_algorithm_e");
}

// IntegrityProtAlgorithm ::= ENUMERATED
const char* integrity_prot_algorithm_opts::to_string() const
{
  static const char* names[] = {"nia0", "nia1", "nia2", "nia3", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "integrity_prot_algorithm_e");
}
uint8_t integrity_prot_algorithm_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "integrity_prot_algorithm_e");
}

// PDCP-Config ::= SEQUENCE
SRSASN_CODE pdcp_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_present, 1));
  HANDLE_CODE(bref.pack(more_than_one_rlc_present, 1));
  HANDLE_CODE(bref.pack(t_reordering_present, 1));

  if (drb_present) {
    HANDLE_CODE(bref.pack(drb.discard_timer_present, 1));
    HANDLE_CODE(bref.pack(drb.pdcp_sn_size_ul_present, 1));
    HANDLE_CODE(bref.pack(drb.pdcp_sn_size_dl_present, 1));
    HANDLE_CODE(bref.pack(drb.integrity_protection_present, 1));
    HANDLE_CODE(bref.pack(drb.status_report_required_present, 1));
    HANDLE_CODE(bref.pack(drb.out_of_order_delivery_present, 1));
    if (drb.discard_timer_present) {
      HANDLE_CODE(drb.discard_timer.pack(bref));
    }
    if (drb.pdcp_sn_size_ul_present) {
      HANDLE_CODE(drb.pdcp_sn_size_ul.pack(bref));
    }
    if (drb.pdcp_sn_size_dl_present) {
      HANDLE_CODE(drb.pdcp_sn_size_dl.pack(bref));
    }
    HANDLE_CODE(drb.hdr_compress.pack(bref));
  }
  if (more_than_one_rlc_present) {
    HANDLE_CODE(bref.pack(more_than_one_rlc.ul_data_split_thres_present, 1));
    HANDLE_CODE(bref.pack(more_than_one_rlc.pdcp_dupl_present, 1));
    HANDLE_CODE(bref.pack(more_than_one_rlc.primary_path.cell_group_present, 1));
    HANDLE_CODE(bref.pack(more_than_one_rlc.primary_path.lc_ch_present, 1));
    if (more_than_one_rlc.primary_path.cell_group_present) {
      HANDLE_CODE(pack_integer(bref, more_than_one_rlc.primary_path.cell_group, (uint8_t)0u, (uint8_t)3u));
    }
    if (more_than_one_rlc.primary_path.lc_ch_present) {
      HANDLE_CODE(pack_integer(bref, more_than_one_rlc.primary_path.lc_ch, (uint8_t)1u, (uint8_t)32u));
    }
    if (more_than_one_rlc.ul_data_split_thres_present) {
      HANDLE_CODE(more_than_one_rlc.ul_data_split_thres.pack(bref));
    }
    if (more_than_one_rlc.pdcp_dupl_present) {
      HANDLE_CODE(bref.pack(more_than_one_rlc.pdcp_dupl, 1));
    }
  }
  if (t_reordering_present) {
    HANDLE_CODE(t_reordering.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ciphering_disabled_present;
    group_flags[1] |= discard_timer_ext_r16.is_present();
    group_flags[1] |= more_than_two_rlc_drb_r16.is_present();
    group_flags[1] |= ethernet_hdr_compress_r16.is_present();
    group_flags[2] |= survival_time_state_support_r17_present;
    group_flags[2] |= ul_data_compress_r17.is_present();
    group_flags[2] |= discard_timer_ext2_r17.is_present();
    group_flags[2] |= init_rx_deliv_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ciphering_disabled_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(discard_timer_ext_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(more_than_two_rlc_drb_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ethernet_hdr_compress_r16.is_present(), 1));
      if (discard_timer_ext_r16.is_present()) {
        HANDLE_CODE(discard_timer_ext_r16->pack(bref));
      }
      if (more_than_two_rlc_drb_r16.is_present()) {
        HANDLE_CODE(bref.pack(more_than_two_rlc_drb_r16->split_secondary_path_r16_present, 1));
        HANDLE_CODE(bref.pack(more_than_two_rlc_drb_r16->dupl_state_r16_present, 1));
        if (more_than_two_rlc_drb_r16->split_secondary_path_r16_present) {
          HANDLE_CODE(
              pack_integer(bref, more_than_two_rlc_drb_r16->split_secondary_path_r16, (uint8_t)1u, (uint8_t)32u));
        }
        if (more_than_two_rlc_drb_r16->dupl_state_r16_present) {
          HANDLE_CODE(pack_fixed_seq_of(bref,
                                        more_than_two_rlc_drb_r16->dupl_state_r16,
                                        more_than_two_rlc_drb_r16->dupl_state_r16.size(),
                                        BitPacker(1)));
        }
      }
      if (ethernet_hdr_compress_r16.is_present()) {
        HANDLE_CODE(ethernet_hdr_compress_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(survival_time_state_support_r17_present, 1));
      HANDLE_CODE(bref.pack(ul_data_compress_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(discard_timer_ext2_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(init_rx_deliv_r17_present, 1));
      if (ul_data_compress_r17.is_present()) {
        HANDLE_CODE(ul_data_compress_r17->pack(bref));
      }
      if (discard_timer_ext2_r17.is_present()) {
        HANDLE_CODE(discard_timer_ext2_r17->pack(bref));
      }
      if (init_rx_deliv_r17_present) {
        HANDLE_CODE(init_rx_deliv_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_present, 1));
  HANDLE_CODE(bref.unpack(more_than_one_rlc_present, 1));
  HANDLE_CODE(bref.unpack(t_reordering_present, 1));

  if (drb_present) {
    HANDLE_CODE(bref.unpack(drb.discard_timer_present, 1));
    HANDLE_CODE(bref.unpack(drb.pdcp_sn_size_ul_present, 1));
    HANDLE_CODE(bref.unpack(drb.pdcp_sn_size_dl_present, 1));
    HANDLE_CODE(bref.unpack(drb.integrity_protection_present, 1));
    HANDLE_CODE(bref.unpack(drb.status_report_required_present, 1));
    HANDLE_CODE(bref.unpack(drb.out_of_order_delivery_present, 1));
    if (drb.discard_timer_present) {
      HANDLE_CODE(drb.discard_timer.unpack(bref));
    }
    if (drb.pdcp_sn_size_ul_present) {
      HANDLE_CODE(drb.pdcp_sn_size_ul.unpack(bref));
    }
    if (drb.pdcp_sn_size_dl_present) {
      HANDLE_CODE(drb.pdcp_sn_size_dl.unpack(bref));
    }
    HANDLE_CODE(drb.hdr_compress.unpack(bref));
  }
  if (more_than_one_rlc_present) {
    HANDLE_CODE(bref.unpack(more_than_one_rlc.ul_data_split_thres_present, 1));
    HANDLE_CODE(bref.unpack(more_than_one_rlc.pdcp_dupl_present, 1));
    HANDLE_CODE(bref.unpack(more_than_one_rlc.primary_path.cell_group_present, 1));
    HANDLE_CODE(bref.unpack(more_than_one_rlc.primary_path.lc_ch_present, 1));
    if (more_than_one_rlc.primary_path.cell_group_present) {
      HANDLE_CODE(unpack_integer(more_than_one_rlc.primary_path.cell_group, bref, (uint8_t)0u, (uint8_t)3u));
    }
    if (more_than_one_rlc.primary_path.lc_ch_present) {
      HANDLE_CODE(unpack_integer(more_than_one_rlc.primary_path.lc_ch, bref, (uint8_t)1u, (uint8_t)32u));
    }
    if (more_than_one_rlc.ul_data_split_thres_present) {
      HANDLE_CODE(more_than_one_rlc.ul_data_split_thres.unpack(bref));
    }
    if (more_than_one_rlc.pdcp_dupl_present) {
      HANDLE_CODE(bref.unpack(more_than_one_rlc.pdcp_dupl, 1));
    }
  }
  if (t_reordering_present) {
    HANDLE_CODE(t_reordering.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ciphering_disabled_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(discard_timer_ext_r16, bref);
      unpack_presence_flag(more_than_two_rlc_drb_r16, bref);
      unpack_presence_flag(ethernet_hdr_compress_r16, bref);
      if (discard_timer_ext_r16.is_present()) {
        HANDLE_CODE(discard_timer_ext_r16->unpack(bref));
      }
      if (more_than_two_rlc_drb_r16.is_present()) {
        HANDLE_CODE(bref.unpack(more_than_two_rlc_drb_r16->split_secondary_path_r16_present, 1));
        HANDLE_CODE(bref.unpack(more_than_two_rlc_drb_r16->dupl_state_r16_present, 1));
        if (more_than_two_rlc_drb_r16->split_secondary_path_r16_present) {
          HANDLE_CODE(
              unpack_integer(more_than_two_rlc_drb_r16->split_secondary_path_r16, bref, (uint8_t)1u, (uint8_t)32u));
        }
        if (more_than_two_rlc_drb_r16->dupl_state_r16_present) {
          HANDLE_CODE(unpack_fixed_seq_of(more_than_two_rlc_drb_r16->dupl_state_r16,
                                          bref,
                                          more_than_two_rlc_drb_r16->dupl_state_r16.size(),
                                          BitPacker(1)));
        }
      }
      if (ethernet_hdr_compress_r16.is_present()) {
        HANDLE_CODE(ethernet_hdr_compress_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(survival_time_state_support_r17_present, 1));
      unpack_presence_flag(ul_data_compress_r17, bref);
      unpack_presence_flag(discard_timer_ext2_r17, bref);
      HANDLE_CODE(bref.unpack(init_rx_deliv_r17_present, 1));
      if (ul_data_compress_r17.is_present()) {
        HANDLE_CODE(ul_data_compress_r17->unpack(bref));
      }
      if (discard_timer_ext2_r17.is_present()) {
        HANDLE_CODE(discard_timer_ext2_r17->unpack(bref));
      }
      if (init_rx_deliv_r17_present) {
        HANDLE_CODE(init_rx_deliv_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pdcp_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_present) {
    j.write_fieldname("drb");
    j.start_obj();
    if (drb.discard_timer_present) {
      j.write_str("discardTimer", drb.discard_timer.to_string());
    }
    if (drb.pdcp_sn_size_ul_present) {
      j.write_str("pdcp-SN-SizeUL", drb.pdcp_sn_size_ul.to_string());
    }
    if (drb.pdcp_sn_size_dl_present) {
      j.write_str("pdcp-SN-SizeDL", drb.pdcp_sn_size_dl.to_string());
    }
    j.write_fieldname("headerCompression");
    drb.hdr_compress.to_json(j);
    if (drb.integrity_protection_present) {
      j.write_str("integrityProtection", "enabled");
    }
    if (drb.status_report_required_present) {
      j.write_str("statusReportRequired", "true");
    }
    if (drb.out_of_order_delivery_present) {
      j.write_str("outOfOrderDelivery", "true");
    }
    j.end_obj();
  }
  if (more_than_one_rlc_present) {
    j.write_fieldname("moreThanOneRLC");
    j.start_obj();
    j.write_fieldname("primaryPath");
    j.start_obj();
    if (more_than_one_rlc.primary_path.cell_group_present) {
      j.write_int("cellGroup", more_than_one_rlc.primary_path.cell_group);
    }
    if (more_than_one_rlc.primary_path.lc_ch_present) {
      j.write_int("logicalChannel", more_than_one_rlc.primary_path.lc_ch);
    }
    j.end_obj();
    if (more_than_one_rlc.ul_data_split_thres_present) {
      j.write_str("ul-DataSplitThreshold", more_than_one_rlc.ul_data_split_thres.to_string());
    }
    if (more_than_one_rlc.pdcp_dupl_present) {
      j.write_bool("pdcp-Duplication", more_than_one_rlc.pdcp_dupl);
    }
    j.end_obj();
  }
  if (t_reordering_present) {
    j.write_str("t-Reordering", t_reordering.to_string());
  }
  if (ext) {
    if (ciphering_disabled_present) {
      j.write_str("cipheringDisabled", "true");
    }
    if (discard_timer_ext_r16.is_present()) {
      j.write_fieldname("discardTimerExt-r16");
      discard_timer_ext_r16->to_json(j);
    }
    if (more_than_two_rlc_drb_r16.is_present()) {
      j.write_fieldname("moreThanTwoRLC-DRB-r16");
      j.start_obj();
      if (more_than_two_rlc_drb_r16->split_secondary_path_r16_present) {
        j.write_int("splitSecondaryPath-r16", more_than_two_rlc_drb_r16->split_secondary_path_r16);
      }
      if (more_than_two_rlc_drb_r16->dupl_state_r16_present) {
        j.start_array("duplicationState-r16");
        for (const auto& e1 : more_than_two_rlc_drb_r16->dupl_state_r16) {
          j.write_bool(e1);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (ethernet_hdr_compress_r16.is_present()) {
      j.write_fieldname("ethernetHeaderCompression-r16");
      ethernet_hdr_compress_r16->to_json(j);
    }
    if (survival_time_state_support_r17_present) {
      j.write_str("survivalTimeStateSupport-r17", "true");
    }
    if (ul_data_compress_r17.is_present()) {
      j.write_fieldname("uplinkDataCompression-r17");
      ul_data_compress_r17->to_json(j);
    }
    if (discard_timer_ext2_r17.is_present()) {
      j.write_fieldname("discardTimerExt2-r17");
      discard_timer_ext2_r17->to_json(j);
    }
    if (init_rx_deliv_r17_present) {
      j.write_str("initialRX-DELIV-r17", init_rx_deliv_r17.to_string());
    }
  }
  j.end_obj();
}

const char* pdcp_cfg_s::drb_s_::discard_timer_opts::to_string() const
{
  static const char* names[] = {"ms10",
                                "ms20",
                                "ms30",
                                "ms40",
                                "ms50",
                                "ms60",
                                "ms75",
                                "ms100",
                                "ms150",
                                "ms200",
                                "ms250",
                                "ms300",
                                "ms500",
                                "ms750",
                                "ms1500",
                                "infinity"};
  return convert_enum_idx(names, 16, value, "pdcp_cfg_s::drb_s_::discard_timer_e_");
}
int16_t pdcp_cfg_s::drb_s_::discard_timer_opts::to_number() const
{
  static const int16_t numbers[] = {10, 20, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(numbers, 16, value, "pdcp_cfg_s::drb_s_::discard_timer_e_");
}

const char* pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::to_string() const
{
  static const char* names[] = {"len12bits", "len18bits"};
  return convert_enum_idx(names, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_");
}
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_");
}

const char* pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::to_string() const
{
  static const char* names[] = {"len12bits", "len18bits"};
  return convert_enum_idx(names, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_");
}
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_");
}

void pdcp_cfg_s::drb_s_::hdr_compress_c_::destroy_()
{
  switch (type_) {
    case types::rohc:
      c.destroy<rohc_s_>();
      break;
    case types::ul_only_rohc:
      c.destroy<ul_only_rohc_s_>();
      break;
    default:
      break;
  }
}
void pdcp_cfg_s::drb_s_::hdr_compress_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      c.init<rohc_s_>();
      break;
    case types::ul_only_rohc:
      c.init<ul_only_rohc_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::hdr_compress_c_(const pdcp_cfg_s::drb_s_::hdr_compress_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      c.init(other.c.get<rohc_s_>());
      break;
    case types::ul_only_rohc:
      c.init(other.c.get<ul_only_rohc_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }
}
pdcp_cfg_s::drb_s_::hdr_compress_c_&
pdcp_cfg_s::drb_s_::hdr_compress_c_::operator=(const pdcp_cfg_s::drb_s_::hdr_compress_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      c.set(other.c.get<rohc_s_>());
      break;
    case types::ul_only_rohc:
      c.set(other.c.get<ul_only_rohc_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }

  return *this;
}
void pdcp_cfg_s::drb_s_::hdr_compress_c_::set_not_used()
{
  set(types::not_used);
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_& pdcp_cfg_s::drb_s_::hdr_compress_c_::set_rohc()
{
  set(types::rohc);
  return c.get<rohc_s_>();
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_& pdcp_cfg_s::drb_s_::hdr_compress_c_::set_ul_only_rohc()
{
  set(types::ul_only_rohc);
  return c.get<ul_only_rohc_s_>();
}
void pdcp_cfg_s::drb_s_::hdr_compress_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      j.write_fieldname("rohc");
      j.start_obj();
      if (c.get<rohc_s_>().max_c_id_present) {
        j.write_int("maxCID", c.get<rohc_s_>().max_c_id);
      }
      j.write_fieldname("profiles");
      j.start_obj();
      j.write_bool("profile0x0001", c.get<rohc_s_>().profiles.profile0x0001);
      j.write_bool("profile0x0002", c.get<rohc_s_>().profiles.profile0x0002);
      j.write_bool("profile0x0003", c.get<rohc_s_>().profiles.profile0x0003);
      j.write_bool("profile0x0004", c.get<rohc_s_>().profiles.profile0x0004);
      j.write_bool("profile0x0006", c.get<rohc_s_>().profiles.profile0x0006);
      j.write_bool("profile0x0101", c.get<rohc_s_>().profiles.profile0x0101);
      j.write_bool("profile0x0102", c.get<rohc_s_>().profiles.profile0x0102);
      j.write_bool("profile0x0103", c.get<rohc_s_>().profiles.profile0x0103);
      j.write_bool("profile0x0104", c.get<rohc_s_>().profiles.profile0x0104);
      j.end_obj();
      if (c.get<rohc_s_>().drb_continue_rohc_present) {
        j.write_str("drb-ContinueROHC", "true");
      }
      j.end_obj();
      break;
    case types::ul_only_rohc:
      j.write_fieldname("uplinkOnlyROHC");
      j.start_obj();
      if (c.get<ul_only_rohc_s_>().max_c_id_present) {
        j.write_int("maxCID", c.get<ul_only_rohc_s_>().max_c_id);
      }
      j.write_fieldname("profiles");
      j.start_obj();
      j.write_bool("profile0x0006", c.get<ul_only_rohc_s_>().profiles.profile0x0006);
      j.end_obj();
      if (c.get<ul_only_rohc_s_>().drb_continue_rohc_present) {
        j.write_str("drb-ContinueROHC", "true");
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::drb_s_::hdr_compress_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<rohc_s_>().max_c_id_present) {
        HANDLE_CODE(pack_integer(bref, c.get<rohc_s_>().max_c_id, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0001, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0002, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0003, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0004, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0006, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0101, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0102, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0103, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0104, 1));
      break;
    case types::ul_only_rohc:
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<ul_only_rohc_s_>().max_c_id_present) {
        HANDLE_CODE(pack_integer(bref, c.get<ul_only_rohc_s_>().max_c_id, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().profiles.profile0x0006, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::drb_s_::hdr_compress_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<rohc_s_>().max_c_id_present) {
        HANDLE_CODE(unpack_integer(c.get<rohc_s_>().max_c_id, bref, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0001, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0002, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0003, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0004, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0006, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0101, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0102, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0103, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0104, 1));
      break;
    case types::ul_only_rohc:
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<ul_only_rohc_s_>().max_c_id_present) {
        HANDLE_CODE(unpack_integer(c.get<ul_only_rohc_s_>().max_c_id, bref, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().profiles.profile0x0006, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdcp_cfg_s::t_reordering_opts::to_string() const
{
  static const char* names[] = {"ms0",     "ms1",     "ms2",     "ms4",     "ms5",     "ms8",     "ms10",    "ms15",
                                "ms20",    "ms30",    "ms40",    "ms50",    "ms60",    "ms80",    "ms100",   "ms120",
                                "ms140",   "ms160",   "ms180",   "ms200",   "ms220",   "ms240",   "ms260",   "ms280",
                                "ms300",   "ms500",   "ms750",   "ms1000",  "ms1250",  "ms1500",  "ms1750",  "ms2000",
                                "ms2250",  "ms2500",  "ms2750",  "ms3000",  "spare28", "spare27", "spare26", "spare25",
                                "spare24", "spare23", "spare22", "spare21", "spare20", "spare19", "spare18", "spare17",
                                "spare16", "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare09",
                                "spare08", "spare07", "spare06", "spare05", "spare04", "spare03", "spare02", "spare01"};
  return convert_enum_idx(names, 64, value, "pdcp_cfg_s::t_reordering_e_");
}
uint16_t pdcp_cfg_s::t_reordering_opts::to_number() const
{
  static const uint16_t numbers[] = {0,   1,   2,   4,    5,    8,    10,   15,   20,   30,   40,   50,
                                     60,  80,  100, 120,  140,  160,  180,  200,  220,  240,  260,  280,
                                     300, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000};
  return map_enum_number(numbers, 36, value, "pdcp_cfg_s::t_reordering_e_");
}

// SDAP-Config ::= SEQUENCE
SRSASN_CODE sdap_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mapped_qos_flows_to_add.size() > 0, 1));
  HANDLE_CODE(bref.pack(mapped_qos_flows_to_release.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(sdap_hdr_dl.pack(bref));
  HANDLE_CODE(sdap_hdr_ul.pack(bref));
  HANDLE_CODE(bref.pack(default_drb, 1));
  if (mapped_qos_flows_to_add.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mapped_qos_flows_to_add, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (mapped_qos_flows_to_release.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mapped_qos_flows_to_release, 1, 64, integer_packer<uint8_t>(0, 63)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdap_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool mapped_qos_flows_to_add_present;
  HANDLE_CODE(bref.unpack(mapped_qos_flows_to_add_present, 1));
  bool mapped_qos_flows_to_release_present;
  HANDLE_CODE(bref.unpack(mapped_qos_flows_to_release_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(sdap_hdr_dl.unpack(bref));
  HANDLE_CODE(sdap_hdr_ul.unpack(bref));
  HANDLE_CODE(bref.unpack(default_drb, 1));
  if (mapped_qos_flows_to_add_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mapped_qos_flows_to_add, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (mapped_qos_flows_to_release_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mapped_qos_flows_to_release, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }

  return SRSASN_SUCCESS;
}
void sdap_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdu-Session", pdu_session);
  j.write_str("sdap-HeaderDL", sdap_hdr_dl.to_string());
  j.write_str("sdap-HeaderUL", sdap_hdr_ul.to_string());
  j.write_bool("defaultDRB", default_drb);
  if (mapped_qos_flows_to_add.size() > 0) {
    j.start_array("mappedQoS-FlowsToAdd");
    for (const auto& e1 : mapped_qos_flows_to_add) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mapped_qos_flows_to_release.size() > 0) {
    j.start_array("mappedQoS-FlowsToRelease");
    for (const auto& e1 : mapped_qos_flows_to_release) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* sdap_cfg_s::sdap_hdr_dl_opts::to_string() const
{
  static const char* names[] = {"present", "absent"};
  return convert_enum_idx(names, 2, value, "sdap_cfg_s::sdap_hdr_dl_e_");
}

const char* sdap_cfg_s::sdap_hdr_ul_opts::to_string() const
{
  static const char* names[] = {"present", "absent"};
  return convert_enum_idx(names, 2, value, "sdap_cfg_s::sdap_hdr_ul_e_");
}

// SL-DRX-Config-r17 ::= SEQUENCE
SRSASN_CODE sl_drx_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_drx_cfg_gc_bc_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_drx_cfg_uc_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_drx_cfg_uc_to_add_mod_list_r17.size() > 0, 1));

  if (sl_drx_cfg_gc_bc_r17_present) {
    HANDLE_CODE(sl_drx_cfg_gc_bc_r17.pack(bref));
  }
  if (sl_drx_cfg_uc_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drx_cfg_uc_to_release_list_r17, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (sl_drx_cfg_uc_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drx_cfg_uc_to_add_mod_list_r17, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drx_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_drx_cfg_gc_bc_r17_present, 1));
  bool sl_drx_cfg_uc_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_drx_cfg_uc_to_release_list_r17_present, 1));
  bool sl_drx_cfg_uc_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_drx_cfg_uc_to_add_mod_list_r17_present, 1));

  if (sl_drx_cfg_gc_bc_r17_present) {
    HANDLE_CODE(sl_drx_cfg_gc_bc_r17.unpack(bref));
  }
  if (sl_drx_cfg_uc_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_drx_cfg_uc_to_release_list_r17, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (sl_drx_cfg_uc_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_drx_cfg_uc_to_add_mod_list_r17, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void sl_drx_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_drx_cfg_gc_bc_r17_present) {
    j.write_fieldname("sl-DRX-ConfigGC-BC-r17");
    sl_drx_cfg_gc_bc_r17.to_json(j);
  }
  if (sl_drx_cfg_uc_to_release_list_r17.size() > 0) {
    j.start_array("sl-DRX-ConfigUC-ToReleaseList-r17");
    for (const auto& e1 : sl_drx_cfg_uc_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_drx_cfg_uc_to_add_mod_list_r17.size() > 0) {
    j.start_array("sl-DRX-ConfigUC-ToAddModList-r17");
    for (const auto& e1 : sl_drx_cfg_uc_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-FreqConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_freq_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_absolute_freq_point_a_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_absolute_freq_ssb_r16_present, 1));
  HANDLE_CODE(bref.pack(freq_shift7p5khz_sl_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_bwp_to_release_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_bwp_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_sync_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_sync_prio_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_freq_id_r16, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_scs_specific_carrier_list_r16, 1, 5));
  if (sl_absolute_freq_point_a_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_absolute_freq_point_a_r16, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (sl_absolute_freq_ssb_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_absolute_freq_ssb_r16, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(pack_integer(bref, value_n_r16, (int8_t)-1, (int8_t)1));
  if (sl_bwp_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_bwp_to_release_list_r16, 1, 4, integer_packer<uint8_t>(0, 4)));
  }
  if (sl_bwp_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_bwp_to_add_mod_list_r16, 1, 4));
  }
  if (sl_sync_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_sync_cfg_list_r16, 1, 16));
  }
  if (sl_sync_prio_r16_present) {
    HANDLE_CODE(sl_sync_prio_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_freq_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_absolute_freq_point_a_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_absolute_freq_ssb_r16_present, 1));
  HANDLE_CODE(bref.unpack(freq_shift7p5khz_sl_r16_present, 1));
  bool sl_bwp_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_bwp_to_release_list_r16_present, 1));
  bool sl_bwp_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_bwp_to_add_mod_list_r16_present, 1));
  bool sl_sync_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_sync_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_sync_prio_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_freq_id_r16, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_dyn_seq_of(sl_scs_specific_carrier_list_r16, bref, 1, 5));
  if (sl_absolute_freq_point_a_r16_present) {
    HANDLE_CODE(unpack_integer(sl_absolute_freq_point_a_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (sl_absolute_freq_ssb_r16_present) {
    HANDLE_CODE(unpack_integer(sl_absolute_freq_ssb_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(unpack_integer(value_n_r16, bref, (int8_t)-1, (int8_t)1));
  if (sl_bwp_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_bwp_to_release_list_r16, bref, 1, 4, integer_packer<uint8_t>(0, 4)));
  }
  if (sl_bwp_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_bwp_to_add_mod_list_r16, bref, 1, 4));
  }
  if (sl_sync_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_sync_cfg_list_r16, bref, 1, 16));
  }
  if (sl_sync_prio_r16_present) {
    HANDLE_CODE(sl_sync_prio_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_freq_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-Freq-Id-r16", sl_freq_id_r16);
  j.start_array("sl-SCS-SpecificCarrierList-r16");
  for (const auto& e1 : sl_scs_specific_carrier_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (sl_absolute_freq_point_a_r16_present) {
    j.write_int("sl-AbsoluteFrequencyPointA-r16", sl_absolute_freq_point_a_r16);
  }
  if (sl_absolute_freq_ssb_r16_present) {
    j.write_int("sl-AbsoluteFrequencySSB-r16", sl_absolute_freq_ssb_r16);
  }
  if (freq_shift7p5khz_sl_r16_present) {
    j.write_str("frequencyShift7p5khzSL-r16", "true");
  }
  j.write_int("valueN-r16", value_n_r16);
  if (sl_bwp_to_release_list_r16.size() > 0) {
    j.start_array("sl-BWP-ToReleaseList-r16");
    for (const auto& e1 : sl_bwp_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_bwp_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-BWP-ToAddModList-r16");
    for (const auto& e1 : sl_bwp_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_sync_cfg_list_r16.size() > 0) {
    j.start_array("sl-SyncConfigList-r16");
    for (const auto& e1 : sl_sync_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_sync_prio_r16_present) {
    j.write_str("sl-SyncPriority-r16", sl_sync_prio_r16.to_string());
  }
  j.end_obj();
}

const char* sl_freq_cfg_r16_s::sl_sync_prio_r16_opts::to_string() const
{
  static const char* names[] = {"gnss", "gnbEnb"};
  return convert_enum_idx(names, 2, value, "sl_freq_cfg_r16_s::sl_sync_prio_r16_e_");
}

// SL-MappingToAddMod-r17 ::= SEQUENCE
SRSASN_CODE sl_map_to_add_mod_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_egress_rlc_ch_uu_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_egress_rlc_ch_pc5_r17_present, 1));

  HANDLE_CODE(sl_remote_ue_rb_id_r17.pack(bref));
  if (sl_egress_rlc_ch_uu_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_egress_rlc_ch_uu_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (sl_egress_rlc_ch_pc5_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_egress_rlc_ch_pc5_r17, (uint16_t)1u, (uint16_t)512u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_map_to_add_mod_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_egress_rlc_ch_uu_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_egress_rlc_ch_pc5_r17_present, 1));

  HANDLE_CODE(sl_remote_ue_rb_id_r17.unpack(bref));
  if (sl_egress_rlc_ch_uu_r17_present) {
    HANDLE_CODE(unpack_integer(sl_egress_rlc_ch_uu_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (sl_egress_rlc_ch_pc5_r17_present) {
    HANDLE_CODE(unpack_integer(sl_egress_rlc_ch_pc5_r17, bref, (uint16_t)1u, (uint16_t)512u));
  }

  return SRSASN_SUCCESS;
}
void sl_map_to_add_mod_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-RemoteUE-RB-Identity-r17");
  sl_remote_ue_rb_id_r17.to_json(j);
  if (sl_egress_rlc_ch_uu_r17_present) {
    j.write_int("sl-EgressRLC-ChannelUu-r17", sl_egress_rlc_ch_uu_r17);
  }
  if (sl_egress_rlc_ch_pc5_r17_present) {
    j.write_int("sl-EgressRLC-ChannelPC5-r17", sl_egress_rlc_ch_pc5_r17);
  }
  j.end_obj();
}

// SL-MeasConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_meas_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_meas_obj_to_rem_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_obj_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_report_cfg_to_rem_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_report_cfg_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_id_to_rem_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_id_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_quant_cfg_r16_present, 1));

  if (sl_meas_obj_to_rem_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_obj_to_rem_list_r16, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sl_meas_obj_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_obj_to_add_mod_list_r16, 1, 64));
  }
  if (sl_report_cfg_to_rem_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_report_cfg_to_rem_list_r16, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sl_report_cfg_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_report_cfg_to_add_mod_list_r16, 1, 64));
  }
  if (sl_meas_id_to_rem_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_id_to_rem_list_r16, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sl_meas_id_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_id_to_add_mod_list_r16, 1, 64));
  }
  if (sl_quant_cfg_r16_present) {
    HANDLE_CODE(sl_quant_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_meas_obj_to_rem_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_obj_to_rem_list_r16_present, 1));
  bool sl_meas_obj_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_obj_to_add_mod_list_r16_present, 1));
  bool sl_report_cfg_to_rem_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_report_cfg_to_rem_list_r16_present, 1));
  bool sl_report_cfg_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_report_cfg_to_add_mod_list_r16_present, 1));
  bool sl_meas_id_to_rem_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_id_to_rem_list_r16_present, 1));
  bool sl_meas_id_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_id_to_add_mod_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_quant_cfg_r16_present, 1));

  if (sl_meas_obj_to_rem_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_obj_to_rem_list_r16, bref, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sl_meas_obj_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_obj_to_add_mod_list_r16, bref, 1, 64));
  }
  if (sl_report_cfg_to_rem_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_report_cfg_to_rem_list_r16, bref, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sl_report_cfg_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_report_cfg_to_add_mod_list_r16, bref, 1, 64));
  }
  if (sl_meas_id_to_rem_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_id_to_rem_list_r16, bref, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sl_meas_id_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_id_to_add_mod_list_r16, bref, 1, 64));
  }
  if (sl_quant_cfg_r16_present) {
    HANDLE_CODE(sl_quant_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_meas_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_meas_obj_to_rem_list_r16.size() > 0) {
    j.start_array("sl-MeasObjectToRemoveList-r16");
    for (const auto& e1 : sl_meas_obj_to_rem_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_meas_obj_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-MeasObjectToAddModList-r16");
    for (const auto& e1 : sl_meas_obj_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_report_cfg_to_rem_list_r16.size() > 0) {
    j.start_array("sl-ReportConfigToRemoveList-r16");
    for (const auto& e1 : sl_report_cfg_to_rem_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_report_cfg_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-ReportConfigToAddModList-r16");
    for (const auto& e1 : sl_report_cfg_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_meas_id_to_rem_list_r16.size() > 0) {
    j.start_array("sl-MeasIdToRemoveList-r16");
    for (const auto& e1 : sl_meas_id_to_rem_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_meas_id_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-MeasIdToAddModList-r16");
    for (const auto& e1 : sl_meas_id_to_add_mod_list_r16) {
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

// SL-PDCP-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_pdcp_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_discard_timer_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_pdcp_sn_size_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_out_of_order_delivery_present, 1));

  if (sl_discard_timer_r16_present) {
    HANDLE_CODE(sl_discard_timer_r16.pack(bref));
  }
  if (sl_pdcp_sn_size_r16_present) {
    HANDLE_CODE(sl_pdcp_sn_size_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pdcp_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_discard_timer_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_pdcp_sn_size_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_out_of_order_delivery_present, 1));

  if (sl_discard_timer_r16_present) {
    HANDLE_CODE(sl_discard_timer_r16.unpack(bref));
  }
  if (sl_pdcp_sn_size_r16_present) {
    HANDLE_CODE(sl_pdcp_sn_size_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_pdcp_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_discard_timer_r16_present) {
    j.write_str("sl-DiscardTimer-r16", sl_discard_timer_r16.to_string());
  }
  if (sl_pdcp_sn_size_r16_present) {
    j.write_str("sl-PDCP-SN-Size-r16", sl_pdcp_sn_size_r16.to_string());
  }
  if (sl_out_of_order_delivery_present) {
    j.write_str("sl-OutOfOrderDelivery", "true");
  }
  j.end_obj();
}

const char* sl_pdcp_cfg_r16_s::sl_discard_timer_r16_opts::to_string() const
{
  static const char* names[] = {"ms3",
                                "ms10",
                                "ms20",
                                "ms25",
                                "ms30",
                                "ms40",
                                "ms50",
                                "ms60",
                                "ms75",
                                "ms100",
                                "ms150",
                                "ms200",
                                "ms250",
                                "ms300",
                                "ms500",
                                "ms750",
                                "ms1500",
                                "infinity"};
  return convert_enum_idx(names, 18, value, "sl_pdcp_cfg_r16_s::sl_discard_timer_r16_e_");
}
int16_t sl_pdcp_cfg_r16_s::sl_discard_timer_r16_opts::to_number() const
{
  static const int16_t numbers[] = {3, 10, 20, 25, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(numbers, 18, value, "sl_pdcp_cfg_r16_s::sl_discard_timer_r16_e_");
}

const char* sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_opts::to_string() const
{
  static const char* names[] = {"len12bits", "len18bits"};
  return convert_enum_idx(names, 2, value, "sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_e_");
}
uint8_t sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_e_");
}

// SL-RLC-BearerConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_rlc_bearer_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_served_radio_bearer_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_rlc_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_mac_lc_ch_cfg_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_rlc_bearer_cfg_idx_r16, (uint16_t)1u, (uint16_t)512u));
  if (sl_served_radio_bearer_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_served_radio_bearer_r16, (uint16_t)1u, (uint16_t)512u));
  }
  if (sl_rlc_cfg_r16_present) {
    HANDLE_CODE(sl_rlc_cfg_r16.pack(bref));
  }
  if (sl_mac_lc_ch_cfg_r16_present) {
    HANDLE_CODE(sl_mac_lc_ch_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_rlc_bearer_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_served_radio_bearer_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_rlc_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_mac_lc_ch_cfg_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_rlc_bearer_cfg_idx_r16, bref, (uint16_t)1u, (uint16_t)512u));
  if (sl_served_radio_bearer_r16_present) {
    HANDLE_CODE(unpack_integer(sl_served_radio_bearer_r16, bref, (uint16_t)1u, (uint16_t)512u));
  }
  if (sl_rlc_cfg_r16_present) {
    HANDLE_CODE(sl_rlc_cfg_r16.unpack(bref));
  }
  if (sl_mac_lc_ch_cfg_r16_present) {
    HANDLE_CODE(sl_mac_lc_ch_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_rlc_bearer_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-RLC-BearerConfigIndex-r16", sl_rlc_bearer_cfg_idx_r16);
  if (sl_served_radio_bearer_r16_present) {
    j.write_int("sl-ServedRadioBearer-r16", sl_served_radio_bearer_r16);
  }
  if (sl_rlc_cfg_r16_present) {
    j.write_fieldname("sl-RLC-Config-r16");
    sl_rlc_cfg_r16.to_json(j);
  }
  if (sl_mac_lc_ch_cfg_r16_present) {
    j.write_fieldname("sl-MAC-LogicalChannelConfig-r16");
    sl_mac_lc_ch_cfg_r16.to_json(j);
  }
  j.end_obj();
}

// SL-RLC-ChannelConfig-r17 ::= SEQUENCE
SRSASN_CODE sl_rlc_ch_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_rlc_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_mac_lc_ch_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_packet_delay_budget_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_rlc_ch_id_r17, (uint16_t)1u, (uint16_t)512u));
  if (sl_rlc_cfg_r17_present) {
    HANDLE_CODE(sl_rlc_cfg_r17.pack(bref));
  }
  if (sl_mac_lc_ch_cfg_r17_present) {
    HANDLE_CODE(sl_mac_lc_ch_cfg_r17.pack(bref));
  }
  if (sl_packet_delay_budget_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_packet_delay_budget_r17, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_rlc_ch_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_rlc_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_mac_lc_ch_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_packet_delay_budget_r17_present, 1));

  HANDLE_CODE(unpack_integer(sl_rlc_ch_id_r17, bref, (uint16_t)1u, (uint16_t)512u));
  if (sl_rlc_cfg_r17_present) {
    HANDLE_CODE(sl_rlc_cfg_r17.unpack(bref));
  }
  if (sl_mac_lc_ch_cfg_r17_present) {
    HANDLE_CODE(sl_mac_lc_ch_cfg_r17.unpack(bref));
  }
  if (sl_packet_delay_budget_r17_present) {
    HANDLE_CODE(unpack_integer(sl_packet_delay_budget_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
void sl_rlc_ch_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-RLC-ChannelID-r17", sl_rlc_ch_id_r17);
  if (sl_rlc_cfg_r17_present) {
    j.write_fieldname("sl-RLC-Config-r17");
    sl_rlc_cfg_r17.to_json(j);
  }
  if (sl_mac_lc_ch_cfg_r17_present) {
    j.write_fieldname("sl-MAC-LogicalChannelConfig-r17");
    sl_mac_lc_ch_cfg_r17.to_json(j);
  }
  if (sl_packet_delay_budget_r17_present) {
    j.write_int("sl-PacketDelayBudget-r17", sl_packet_delay_budget_r17);
  }
  j.end_obj();
}

// SL-RelayUE-Config-r17 ::= SEQUENCE
SRSASN_CODE sl_relay_ue_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thresh_high_relay_r17_present, 1));
  HANDLE_CODE(bref.pack(thresh_low_relay_r17_present, 1));
  HANDLE_CODE(bref.pack(hyst_max_relay_r17_present, 1));
  HANDLE_CODE(bref.pack(hyst_min_relay_r17_present, 1));

  if (thresh_high_relay_r17_present) {
    HANDLE_CODE(pack_integer(bref, thresh_high_relay_r17, (uint8_t)0u, (uint8_t)127u));
  }
  if (thresh_low_relay_r17_present) {
    HANDLE_CODE(pack_integer(bref, thresh_low_relay_r17, (uint8_t)0u, (uint8_t)127u));
  }
  if (hyst_max_relay_r17_present) {
    HANDLE_CODE(pack_integer(bref, hyst_max_relay_r17, (uint8_t)0u, (uint8_t)30u));
  }
  if (hyst_min_relay_r17_present) {
    HANDLE_CODE(pack_integer(bref, hyst_min_relay_r17, (uint8_t)0u, (uint8_t)30u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_relay_ue_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thresh_high_relay_r17_present, 1));
  HANDLE_CODE(bref.unpack(thresh_low_relay_r17_present, 1));
  HANDLE_CODE(bref.unpack(hyst_max_relay_r17_present, 1));
  HANDLE_CODE(bref.unpack(hyst_min_relay_r17_present, 1));

  if (thresh_high_relay_r17_present) {
    HANDLE_CODE(unpack_integer(thresh_high_relay_r17, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (thresh_low_relay_r17_present) {
    HANDLE_CODE(unpack_integer(thresh_low_relay_r17, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (hyst_max_relay_r17_present) {
    HANDLE_CODE(unpack_integer(hyst_max_relay_r17, bref, (uint8_t)0u, (uint8_t)30u));
  }
  if (hyst_min_relay_r17_present) {
    HANDLE_CODE(unpack_integer(hyst_min_relay_r17, bref, (uint8_t)0u, (uint8_t)30u));
  }

  return SRSASN_SUCCESS;
}
void sl_relay_ue_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thresh_high_relay_r17_present) {
    j.write_int("threshHighRelay-r17", thresh_high_relay_r17);
  }
  if (thresh_low_relay_r17_present) {
    j.write_int("threshLowRelay-r17", thresh_low_relay_r17);
  }
  if (hyst_max_relay_r17_present) {
    j.write_int("hystMaxRelay-r17", hyst_max_relay_r17);
  }
  if (hyst_min_relay_r17_present) {
    j.write_int("hystMinRelay-r17", hyst_min_relay_r17);
  }
  j.end_obj();
}

// SL-RemoteUE-Config-r17 ::= SEQUENCE
SRSASN_CODE sl_remote_ue_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thresh_high_remote_r17_present, 1));
  HANDLE_CODE(bref.pack(hyst_max_remote_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_resel_cfg_r17_present, 1));

  if (thresh_high_remote_r17_present) {
    HANDLE_CODE(pack_integer(bref, thresh_high_remote_r17, (uint8_t)0u, (uint8_t)127u));
  }
  if (hyst_max_remote_r17_present) {
    HANDLE_CODE(pack_integer(bref, hyst_max_remote_r17, (uint8_t)0u, (uint8_t)30u));
  }
  if (sl_resel_cfg_r17_present) {
    HANDLE_CODE(sl_resel_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_remote_ue_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thresh_high_remote_r17_present, 1));
  HANDLE_CODE(bref.unpack(hyst_max_remote_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_resel_cfg_r17_present, 1));

  if (thresh_high_remote_r17_present) {
    HANDLE_CODE(unpack_integer(thresh_high_remote_r17, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (hyst_max_remote_r17_present) {
    HANDLE_CODE(unpack_integer(hyst_max_remote_r17, bref, (uint8_t)0u, (uint8_t)30u));
  }
  if (sl_resel_cfg_r17_present) {
    HANDLE_CODE(sl_resel_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_remote_ue_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thresh_high_remote_r17_present) {
    j.write_int("threshHighRemote-r17", thresh_high_remote_r17);
  }
  if (hyst_max_remote_r17_present) {
    j.write_int("hystMaxRemote-r17", hyst_max_remote_r17);
  }
  if (sl_resel_cfg_r17_present) {
    j.write_fieldname("sl-ReselectionConfig-r17");
    sl_resel_cfg_r17.to_json(j);
  }
  j.end_obj();
}

// SL-SDAP-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_sdap_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_mapped_qos_flows_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_cast_type_r16_present, 1));

  HANDLE_CODE(sl_sdap_hdr_r16.pack(bref));
  HANDLE_CODE(bref.pack(sl_default_rb_r16, 1));
  if (sl_mapped_qos_flows_r16_present) {
    HANDLE_CODE(sl_mapped_qos_flows_r16.pack(bref));
  }
  if (sl_cast_type_r16_present) {
    HANDLE_CODE(sl_cast_type_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sdap_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_mapped_qos_flows_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_cast_type_r16_present, 1));

  HANDLE_CODE(sl_sdap_hdr_r16.unpack(bref));
  HANDLE_CODE(bref.unpack(sl_default_rb_r16, 1));
  if (sl_mapped_qos_flows_r16_present) {
    HANDLE_CODE(sl_mapped_qos_flows_r16.unpack(bref));
  }
  if (sl_cast_type_r16_present) {
    HANDLE_CODE(sl_cast_type_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_sdap_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-SDAP-Header-r16", sl_sdap_hdr_r16.to_string());
  j.write_bool("sl-DefaultRB-r16", sl_default_rb_r16);
  if (sl_mapped_qos_flows_r16_present) {
    j.write_fieldname("sl-MappedQoS-Flows-r16");
    sl_mapped_qos_flows_r16.to_json(j);
  }
  if (sl_cast_type_r16_present) {
    j.write_str("sl-CastType-r16", sl_cast_type_r16.to_string());
  }
  j.end_obj();
}

const char* sl_sdap_cfg_r16_s::sl_sdap_hdr_r16_opts::to_string() const
{
  static const char* names[] = {"present", "absent"};
  return convert_enum_idx(names, 2, value, "sl_sdap_cfg_r16_s::sl_sdap_hdr_r16_e_");
}

void sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::destroy_()
{
  switch (type_) {
    case types::sl_mapped_qos_flows_list_r16:
      c.destroy<sl_mapped_qos_flows_list_r16_l_>();
      break;
    case types::sl_mapped_qos_flows_list_ded_r16:
      c.destroy<sl_mapped_qos_flows_list_ded_r16_s>();
      break;
    default:
      break;
  }
}
void sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sl_mapped_qos_flows_list_r16:
      c.init<sl_mapped_qos_flows_list_r16_l_>();
      break;
    case types::sl_mapped_qos_flows_list_ded_r16:
      c.init<sl_mapped_qos_flows_list_ded_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_");
  }
}
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::sl_mapped_qos_flows_r16_c_(
    const sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl_mapped_qos_flows_list_r16:
      c.init(other.c.get<sl_mapped_qos_flows_list_r16_l_>());
      break;
    case types::sl_mapped_qos_flows_list_ded_r16:
      c.init(other.c.get<sl_mapped_qos_flows_list_ded_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_");
  }
}
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_&
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::operator=(const sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl_mapped_qos_flows_list_r16:
      c.set(other.c.get<sl_mapped_qos_flows_list_r16_l_>());
      break;
    case types::sl_mapped_qos_flows_list_ded_r16:
      c.set(other.c.get<sl_mapped_qos_flows_list_ded_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_");
  }

  return *this;
}
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::sl_mapped_qos_flows_list_r16_l_&
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::set_sl_mapped_qos_flows_list_r16()
{
  set(types::sl_mapped_qos_flows_list_r16);
  return c.get<sl_mapped_qos_flows_list_r16_l_>();
}
sl_mapped_qos_flows_list_ded_r16_s&
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::set_sl_mapped_qos_flows_list_ded_r16()
{
  set(types::sl_mapped_qos_flows_list_ded_r16);
  return c.get<sl_mapped_qos_flows_list_ded_r16_s>();
}
void sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl_mapped_qos_flows_list_r16:
      j.start_array("sl-MappedQoS-FlowsList-r16");
      for (const auto& e1 : c.get<sl_mapped_qos_flows_list_r16_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_mapped_qos_flows_list_ded_r16:
      j.write_fieldname("sl-MappedQoS-FlowsListDedicated-r16");
      c.get<sl_mapped_qos_flows_list_ded_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl_mapped_qos_flows_list_r16:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_mapped_qos_flows_list_r16_l_>(), 1, 2048));
      break;
    case types::sl_mapped_qos_flows_list_ded_r16:
      HANDLE_CODE(c.get<sl_mapped_qos_flows_list_ded_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl_mapped_qos_flows_list_r16:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_mapped_qos_flows_list_r16_l_>(), bref, 1, 2048));
      break;
    case types::sl_mapped_qos_flows_list_ded_r16:
      HANDLE_CODE(c.get<sl_mapped_qos_flows_list_ded_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sl_sdap_cfg_r16_s::sl_cast_type_r16_opts::to_string() const
{
  static const char* names[] = {"broadcast", "groupcast", "unicast", "spare1"};
  return convert_enum_idx(names, 4, value, "sl_sdap_cfg_r16_s::sl_cast_type_r16_e_");
}

// SL-ScheduledConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_sched_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mac_main_cfg_sl_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_cs_rnti_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_psfch_to_pucch_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_cfg_grant_cfg_list_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, sl_rnti_r16, (uint32_t)0u, (uint32_t)65535u));
  if (mac_main_cfg_sl_r16_present) {
    HANDLE_CODE(mac_main_cfg_sl_r16.pack(bref));
  }
  if (sl_cs_rnti_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_cs_rnti_r16, (uint32_t)0u, (uint32_t)65535u));
  }
  if (sl_psfch_to_pucch_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_psfch_to_pucch_r16, 1, 8, integer_packer<uint8_t>(0, 15)));
  }
  if (sl_cfg_grant_cfg_list_r16_present) {
    HANDLE_CODE(sl_cfg_grant_cfg_list_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_dci_to_sl_trans_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_dci_to_sl_trans_r16.is_present(), 1));
      if (sl_dci_to_sl_trans_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sl_dci_to_sl_trans_r16, 1, 8, integer_packer<uint8_t>(1, 32)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_sched_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mac_main_cfg_sl_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_cs_rnti_r16_present, 1));
  bool sl_psfch_to_pucch_r16_present;
  HANDLE_CODE(bref.unpack(sl_psfch_to_pucch_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_cfg_grant_cfg_list_r16_present, 1));

  HANDLE_CODE(unpack_integer(sl_rnti_r16, bref, (uint32_t)0u, (uint32_t)65535u));
  if (mac_main_cfg_sl_r16_present) {
    HANDLE_CODE(mac_main_cfg_sl_r16.unpack(bref));
  }
  if (sl_cs_rnti_r16_present) {
    HANDLE_CODE(unpack_integer(sl_cs_rnti_r16, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (sl_psfch_to_pucch_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_psfch_to_pucch_r16, bref, 1, 8, integer_packer<uint8_t>(0, 15)));
  }
  if (sl_cfg_grant_cfg_list_r16_present) {
    HANDLE_CODE(sl_cfg_grant_cfg_list_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_dci_to_sl_trans_r16, bref);
      if (sl_dci_to_sl_trans_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sl_dci_to_sl_trans_r16, bref, 1, 8, integer_packer<uint8_t>(1, 32)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_sched_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-RNTI-r16", sl_rnti_r16);
  if (mac_main_cfg_sl_r16_present) {
    j.write_fieldname("mac-MainConfigSL-r16");
    mac_main_cfg_sl_r16.to_json(j);
  }
  if (sl_cs_rnti_r16_present) {
    j.write_int("sl-CS-RNTI-r16", sl_cs_rnti_r16);
  }
  if (sl_psfch_to_pucch_r16.size() > 0) {
    j.start_array("sl-PSFCH-ToPUCCH-r16");
    for (const auto& e1 : sl_psfch_to_pucch_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_cfg_grant_cfg_list_r16_present) {
    j.write_fieldname("sl-ConfiguredGrantConfigList-r16");
    sl_cfg_grant_cfg_list_r16.to_json(j);
  }
  if (ext) {
    if (sl_dci_to_sl_trans_r16.is_present()) {
      j.start_array("sl-DCI-ToSL-Trans-r16");
      for (const auto& e1 : *sl_dci_to_sl_trans_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SL-UE-SelectedConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_ue_sel_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_pssch_tx_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_prob_res_keep_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_reselect_after_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_cbr_common_tx_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.pack(ul_prioritization_thres_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_prioritization_thres_r16_present, 1));

  if (sl_pssch_tx_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_pssch_tx_cfg_list_r16, 1, 16));
  }
  if (sl_prob_res_keep_r16_present) {
    HANDLE_CODE(sl_prob_res_keep_r16.pack(bref));
  }
  if (sl_reselect_after_r16_present) {
    HANDLE_CODE(sl_reselect_after_r16.pack(bref));
  }
  if (sl_cbr_common_tx_cfg_list_r16_present) {
    HANDLE_CODE(sl_cbr_common_tx_cfg_list_r16.pack(bref));
  }
  if (ul_prioritization_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, ul_prioritization_thres_r16, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_prioritization_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_prioritization_thres_r16, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_ue_sel_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_pssch_tx_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_pssch_tx_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_prob_res_keep_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_reselect_after_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_cbr_common_tx_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(ul_prioritization_thres_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_prioritization_thres_r16_present, 1));

  if (sl_pssch_tx_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_pssch_tx_cfg_list_r16, bref, 1, 16));
  }
  if (sl_prob_res_keep_r16_present) {
    HANDLE_CODE(sl_prob_res_keep_r16.unpack(bref));
  }
  if (sl_reselect_after_r16_present) {
    HANDLE_CODE(sl_reselect_after_r16.unpack(bref));
  }
  if (sl_cbr_common_tx_cfg_list_r16_present) {
    HANDLE_CODE(sl_cbr_common_tx_cfg_list_r16.unpack(bref));
  }
  if (ul_prioritization_thres_r16_present) {
    HANDLE_CODE(unpack_integer(ul_prioritization_thres_r16, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (sl_prioritization_thres_r16_present) {
    HANDLE_CODE(unpack_integer(sl_prioritization_thres_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void sl_ue_sel_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_pssch_tx_cfg_list_r16.size() > 0) {
    j.start_array("sl-PSSCH-TxConfigList-r16");
    for (const auto& e1 : sl_pssch_tx_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_prob_res_keep_r16_present) {
    j.write_str("sl-ProbResourceKeep-r16", sl_prob_res_keep_r16.to_string());
  }
  if (sl_reselect_after_r16_present) {
    j.write_str("sl-ReselectAfter-r16", sl_reselect_after_r16.to_string());
  }
  if (sl_cbr_common_tx_cfg_list_r16_present) {
    j.write_fieldname("sl-CBR-CommonTxConfigList-r16");
    sl_cbr_common_tx_cfg_list_r16.to_json(j);
  }
  if (ul_prioritization_thres_r16_present) {
    j.write_int("ul-PrioritizationThres-r16", ul_prioritization_thres_r16);
  }
  if (sl_prioritization_thres_r16_present) {
    j.write_int("sl-PrioritizationThres-r16", sl_prioritization_thres_r16);
  }
  j.end_obj();
}

const char* sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_opts::to_string() const
{
  static const char* names[] = {"v0", "v0dot2", "v0dot4", "v0dot6", "v0dot8"};
  return convert_enum_idx(names, 5, value, "sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_e_");
}
float sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.2, 0.4, 0.6, 0.8};
  return map_enum_number(numbers, 5, value, "sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_e_");
}
const char* sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(number_strs, 5, value, "sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_e_");
}

const char* sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n7", "n8", "n9"};
  return convert_enum_idx(names, 9, value, "sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_e_");
}
uint8_t sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  return map_enum_number(numbers, 9, value, "sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_e_");
}

// TMGI-r17 ::= SEQUENCE
SRSASN_CODE tmgi_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r17.pack(bref));
  HANDLE_CODE(service_id_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tmgi_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r17.unpack(bref));
  HANDLE_CODE(service_id_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void tmgi_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Id-r17");
  plmn_id_r17.to_json(j);
  j.write_str("serviceId-r17", service_id_r17.to_string());
  j.end_obj();
}

void tmgi_r17_s::plmn_id_r17_c_::destroy_()
{
  switch (type_) {
    case types::explicit_value:
      c.destroy<plmn_id_s>();
      break;
    default:
      break;
  }
}
void tmgi_r17_s::plmn_id_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_idx:
      break;
    case types::explicit_value:
      c.init<plmn_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }
}
tmgi_r17_s::plmn_id_r17_c_::plmn_id_r17_c_(const tmgi_r17_s::plmn_id_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::explicit_value:
      c.init(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }
}
tmgi_r17_s::plmn_id_r17_c_& tmgi_r17_s::plmn_id_r17_c_::operator=(const tmgi_r17_s::plmn_id_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::explicit_value:
      c.set(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }

  return *this;
}
uint8_t& tmgi_r17_s::plmn_id_r17_c_::set_plmn_idx()
{
  set(types::plmn_idx);
  return c.get<uint8_t>();
}
plmn_id_s& tmgi_r17_s::plmn_id_r17_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c.get<plmn_id_s>();
}
void tmgi_r17_s::plmn_id_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_idx:
      j.write_int("plmn-Index", c.get<uint8_t>());
      break;
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.get<plmn_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE tmgi_r17_s::plmn_id_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)12u));
      break;
    case types::explicit_value:
      HANDLE_CODE(c.get<plmn_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tmgi_r17_s::plmn_id_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)12u));
      break;
    case types::explicit_value:
      HANDLE_CODE(c.get<plmn_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DRB-ToAddMod ::= SEQUENCE
SRSASN_CODE drb_to_add_mod_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cn_assoc_present, 1));
  HANDLE_CODE(bref.pack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.pack(recover_pdcp_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_present, 1));

  if (cn_assoc_present) {
    HANDLE_CODE(cn_assoc.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= daps_cfg_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(daps_cfg_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cn_assoc_present, 1));
  HANDLE_CODE(bref.unpack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(recover_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_present, 1));

  if (cn_assoc_present) {
    HANDLE_CODE(cn_assoc.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(daps_cfg_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void drb_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cn_assoc_present) {
    j.write_fieldname("cnAssociation");
    cn_assoc.to_json(j);
  }
  j.write_int("drb-Identity", drb_id);
  if (reestablish_pdcp_present) {
    j.write_str("reestablishPDCP", "true");
  }
  if (recover_pdcp_present) {
    j.write_str("recoverPDCP", "true");
  }
  if (pdcp_cfg_present) {
    j.write_fieldname("pdcp-Config");
    pdcp_cfg.to_json(j);
  }
  if (ext) {
    if (daps_cfg_r16_present) {
      j.write_str("daps-Config-r16", "true");
    }
  }
  j.end_obj();
}

void drb_to_add_mod_s::cn_assoc_c_::destroy_()
{
  switch (type_) {
    case types::sdap_cfg:
      c.destroy<sdap_cfg_s>();
      break;
    default:
      break;
  }
}
void drb_to_add_mod_s::cn_assoc_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eps_bearer_id:
      break;
    case types::sdap_cfg:
      c.init<sdap_cfg_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }
}
drb_to_add_mod_s::cn_assoc_c_::cn_assoc_c_(const drb_to_add_mod_s::cn_assoc_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eps_bearer_id:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sdap_cfg:
      c.init(other.c.get<sdap_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }
}
drb_to_add_mod_s::cn_assoc_c_& drb_to_add_mod_s::cn_assoc_c_::operator=(const drb_to_add_mod_s::cn_assoc_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eps_bearer_id:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sdap_cfg:
      c.set(other.c.get<sdap_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }

  return *this;
}
uint8_t& drb_to_add_mod_s::cn_assoc_c_::set_eps_bearer_id()
{
  set(types::eps_bearer_id);
  return c.get<uint8_t>();
}
sdap_cfg_s& drb_to_add_mod_s::cn_assoc_c_::set_sdap_cfg()
{
  set(types::sdap_cfg);
  return c.get<sdap_cfg_s>();
}
void drb_to_add_mod_s::cn_assoc_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eps_bearer_id:
      j.write_int("eps-BearerIdentity", c.get<uint8_t>());
      break;
    case types::sdap_cfg:
      j.write_fieldname("sdap-Config");
      c.get<sdap_cfg_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }
  j.end_obj();
}
SRSASN_CODE drb_to_add_mod_s::cn_assoc_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eps_bearer_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sdap_cfg:
      HANDLE_CODE(c.get<sdap_cfg_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_s::cn_assoc_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eps_bearer_id:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sdap_cfg:
      HANDLE_CODE(c.get<sdap_cfg_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MRB-ToAddMod-r17 ::= SEQUENCE
SRSASN_CODE mrb_to_add_mod_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbs_session_id_r17_present, 1));
  HANDLE_CODE(bref.pack(mrb_id_new_r17_present, 1));
  HANDLE_CODE(bref.pack(reestablish_pdcp_r17_present, 1));
  HANDLE_CODE(bref.pack(recover_pdcp_r17_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_r17_present, 1));

  if (mbs_session_id_r17_present) {
    HANDLE_CODE(mbs_session_id_r17.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, mrb_id_r17, (uint16_t)1u, (uint16_t)512u));
  if (mrb_id_new_r17_present) {
    HANDLE_CODE(pack_integer(bref, mrb_id_new_r17, (uint16_t)1u, (uint16_t)512u));
  }
  if (pdcp_cfg_r17_present) {
    HANDLE_CODE(pdcp_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_to_add_mod_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mbs_session_id_r17_present, 1));
  HANDLE_CODE(bref.unpack(mrb_id_new_r17_present, 1));
  HANDLE_CODE(bref.unpack(reestablish_pdcp_r17_present, 1));
  HANDLE_CODE(bref.unpack(recover_pdcp_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_r17_present, 1));

  if (mbs_session_id_r17_present) {
    HANDLE_CODE(mbs_session_id_r17.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(mrb_id_r17, bref, (uint16_t)1u, (uint16_t)512u));
  if (mrb_id_new_r17_present) {
    HANDLE_CODE(unpack_integer(mrb_id_new_r17, bref, (uint16_t)1u, (uint16_t)512u));
  }
  if (pdcp_cfg_r17_present) {
    HANDLE_CODE(pdcp_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrb_to_add_mod_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbs_session_id_r17_present) {
    j.write_fieldname("mbs-SessionId-r17");
    mbs_session_id_r17.to_json(j);
  }
  j.write_int("mrb-Identity-r17", mrb_id_r17);
  if (mrb_id_new_r17_present) {
    j.write_int("mrb-IdentityNew-r17", mrb_id_new_r17);
  }
  if (reestablish_pdcp_r17_present) {
    j.write_str("reestablishPDCP-r17", "true");
  }
  if (recover_pdcp_r17_present) {
    j.write_str("recoverPDCP-r17", "true");
  }
  if (pdcp_cfg_r17_present) {
    j.write_fieldname("pdcp-Config-r17");
    pdcp_cfg_r17.to_json(j);
  }
  j.end_obj();
}

// SL-DiscConfig-r17 ::= SEQUENCE
SRSASN_CODE sl_disc_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_relay_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_remote_ue_cfg_r17_present, 1));

  if (sl_relay_ue_cfg_r17_present) {
    HANDLE_CODE(sl_relay_ue_cfg_r17.pack(bref));
  }
  if (sl_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_remote_ue_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_relay_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_remote_ue_cfg_r17_present, 1));

  if (sl_relay_ue_cfg_r17_present) {
    HANDLE_CODE(sl_relay_ue_cfg_r17.unpack(bref));
  }
  if (sl_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_remote_ue_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_disc_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_relay_ue_cfg_r17_present) {
    j.write_fieldname("sl-RelayUE-Config-r17");
    sl_relay_ue_cfg_r17.to_json(j);
  }
  if (sl_remote_ue_cfg_r17_present) {
    j.write_fieldname("sl-RemoteUE-Config-r17");
    sl_remote_ue_cfg_r17.to_json(j);
  }
  j.end_obj();
}

// SL-MeasConfigInfo-r16 ::= SEQUENCE
SRSASN_CODE sl_meas_cfg_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, sl_dest_idx_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(sl_meas_cfg_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_cfg_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(sl_dest_idx_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(sl_meas_cfg_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_meas_cfg_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-DestinationIndex-r16", sl_dest_idx_r16);
  j.write_fieldname("sl-MeasConfig-r16");
  sl_meas_cfg_r16.to_json(j);
  j.end_obj();
}

// SL-PHY-MAC-RLC-Config-r16 ::= SEQUENCE
SRSASN_CODE sl_phy_mac_rlc_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_sched_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ue_sel_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_freq_info_to_release_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_freq_info_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rlc_bearer_to_release_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rlc_bearer_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_csi_sched_request_id_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ssb_prio_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(network_ctrl_sync_tx_r16_present, 1));

  if (sl_sched_cfg_r16_present) {
    HANDLE_CODE(sl_sched_cfg_r16.pack(bref));
  }
  if (sl_ue_sel_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_r16.pack(bref));
  }
  if (sl_freq_info_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_freq_info_to_release_list_r16, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_freq_info_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_freq_info_to_add_mod_list_r16, 1, 8));
  }
  if (sl_rlc_bearer_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_bearer_to_release_list_r16, 1, 512, integer_packer<uint16_t>(1, 512)));
  }
  if (sl_rlc_bearer_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_bearer_to_add_mod_list_r16, 1, 512));
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    HANDLE_CODE(sl_max_num_consecutive_dtx_r16.pack(bref));
  }
  if (sl_csi_sched_request_id_r16_present) {
    HANDLE_CODE(sl_csi_sched_request_id_r16.pack(bref));
  }
  if (sl_ssb_prio_nr_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_ssb_prio_nr_r16, (uint8_t)1u, (uint8_t)8u));
  }
  if (network_ctrl_sync_tx_r16_present) {
    HANDLE_CODE(network_ctrl_sync_tx_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_phy_mac_rlc_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_sched_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ue_sel_cfg_r16_present, 1));
  bool sl_freq_info_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_freq_info_to_release_list_r16_present, 1));
  bool sl_freq_info_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_freq_info_to_add_mod_list_r16_present, 1));
  bool sl_rlc_bearer_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_rlc_bearer_to_release_list_r16_present, 1));
  bool sl_rlc_bearer_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_rlc_bearer_to_add_mod_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_csi_sched_request_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ssb_prio_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(network_ctrl_sync_tx_r16_present, 1));

  if (sl_sched_cfg_r16_present) {
    HANDLE_CODE(sl_sched_cfg_r16.unpack(bref));
  }
  if (sl_ue_sel_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_r16.unpack(bref));
  }
  if (sl_freq_info_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_freq_info_to_release_list_r16, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sl_freq_info_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_freq_info_to_add_mod_list_r16, bref, 1, 8));
  }
  if (sl_rlc_bearer_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_bearer_to_release_list_r16, bref, 1, 512, integer_packer<uint16_t>(1, 512)));
  }
  if (sl_rlc_bearer_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_bearer_to_add_mod_list_r16, bref, 1, 512));
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    HANDLE_CODE(sl_max_num_consecutive_dtx_r16.unpack(bref));
  }
  if (sl_csi_sched_request_id_r16_present) {
    HANDLE_CODE(sl_csi_sched_request_id_r16.unpack(bref));
  }
  if (sl_ssb_prio_nr_r16_present) {
    HANDLE_CODE(unpack_integer(sl_ssb_prio_nr_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (network_ctrl_sync_tx_r16_present) {
    HANDLE_CODE(network_ctrl_sync_tx_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_phy_mac_rlc_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_sched_cfg_r16_present) {
    j.write_fieldname("sl-ScheduledConfig-r16");
    sl_sched_cfg_r16.to_json(j);
  }
  if (sl_ue_sel_cfg_r16_present) {
    j.write_fieldname("sl-UE-SelectedConfig-r16");
    sl_ue_sel_cfg_r16.to_json(j);
  }
  if (sl_freq_info_to_release_list_r16.size() > 0) {
    j.start_array("sl-FreqInfoToReleaseList-r16");
    for (const auto& e1 : sl_freq_info_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_freq_info_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-FreqInfoToAddModList-r16");
    for (const auto& e1 : sl_freq_info_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_rlc_bearer_to_release_list_r16.size() > 0) {
    j.start_array("sl-RLC-BearerToReleaseList-r16");
    for (const auto& e1 : sl_rlc_bearer_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_rlc_bearer_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-RLC-BearerToAddModList-r16");
    for (const auto& e1 : sl_rlc_bearer_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    j.write_str("sl-MaxNumConsecutiveDTX-r16", sl_max_num_consecutive_dtx_r16.to_string());
  }
  if (sl_csi_acquisition_r16_present) {
    j.write_str("sl-CSI-Acquisition-r16", "enabled");
  }
  if (sl_csi_sched_request_id_r16_present) {
    j.write_fieldname("sl-CSI-SchedulingRequestId-r16");
    sl_csi_sched_request_id_r16.to_json(j);
  }
  if (sl_ssb_prio_nr_r16_present) {
    j.write_int("sl-SSB-PriorityNR-r16", sl_ssb_prio_nr_r16);
  }
  if (network_ctrl_sync_tx_r16_present) {
    j.write_str("networkControlledSyncTx-r16", network_ctrl_sync_tx_r16.to_string());
  }
  j.end_obj();
}

const char* sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n16", "n32"};
  return convert_enum_idx(names, 8, value, "sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}
uint8_t sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}

const char* sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16_opts::to_string() const
{
  static const char* names[] = {"on", "off"};
  return convert_enum_idx(names, 2, value, "sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16_e_");
}

// SL-PHY-MAC-RLC-Config-v1700 ::= SEQUENCE
SRSASN_CODE sl_phy_mac_rlc_cfg_v1700_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_drx_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_rlc_ch_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rlc_ch_to_add_mod_list_r17.size() > 0, 1));

  if (sl_drx_cfg_r17_present) {
    HANDLE_CODE(sl_drx_cfg_r17.pack(bref));
  }
  if (sl_rlc_ch_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_ch_to_release_list_r17, 1, 512, integer_packer<uint16_t>(1, 512)));
  }
  if (sl_rlc_ch_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_ch_to_add_mod_list_r17, 1, 512));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_phy_mac_rlc_cfg_v1700_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_drx_cfg_r17_present, 1));
  bool sl_rlc_ch_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_rlc_ch_to_release_list_r17_present, 1));
  bool sl_rlc_ch_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_rlc_ch_to_add_mod_list_r17_present, 1));

  if (sl_drx_cfg_r17_present) {
    HANDLE_CODE(sl_drx_cfg_r17.unpack(bref));
  }
  if (sl_rlc_ch_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_ch_to_release_list_r17, bref, 1, 512, integer_packer<uint16_t>(1, 512)));
  }
  if (sl_rlc_ch_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_ch_to_add_mod_list_r17, bref, 1, 512));
  }

  return SRSASN_SUCCESS;
}
void sl_phy_mac_rlc_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_drx_cfg_r17_present) {
    j.write_fieldname("sl-DRX-Config-r17");
    sl_drx_cfg_r17.to_json(j);
  }
  if (sl_rlc_ch_to_release_list_r17.size() > 0) {
    j.start_array("sl-RLC-ChannelToReleaseList-r17");
    for (const auto& e1 : sl_rlc_ch_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_rlc_ch_to_add_mod_list_r17.size() > 0) {
    j.start_array("sl-RLC-ChannelToAddModList-r17");
    for (const auto& e1 : sl_rlc_ch_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-RadioBearerConfig-r16 ::= SEQUENCE
SRSASN_CODE sl_radio_bearer_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_sdap_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_pdcp_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_trans_range_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, slrb_uu_cfg_idx_r16, (uint16_t)1u, (uint16_t)512u));
  if (sl_sdap_cfg_r16_present) {
    HANDLE_CODE(sl_sdap_cfg_r16.pack(bref));
  }
  if (sl_pdcp_cfg_r16_present) {
    HANDLE_CODE(sl_pdcp_cfg_r16.pack(bref));
  }
  if (sl_trans_range_r16_present) {
    HANDLE_CODE(sl_trans_range_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_radio_bearer_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_sdap_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_pdcp_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_trans_range_r16_present, 1));

  HANDLE_CODE(unpack_integer(slrb_uu_cfg_idx_r16, bref, (uint16_t)1u, (uint16_t)512u));
  if (sl_sdap_cfg_r16_present) {
    HANDLE_CODE(sl_sdap_cfg_r16.unpack(bref));
  }
  if (sl_pdcp_cfg_r16_present) {
    HANDLE_CODE(sl_pdcp_cfg_r16.unpack(bref));
  }
  if (sl_trans_range_r16_present) {
    HANDLE_CODE(sl_trans_range_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_radio_bearer_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("slrb-Uu-ConfigIndex-r16", slrb_uu_cfg_idx_r16);
  if (sl_sdap_cfg_r16_present) {
    j.write_fieldname("sl-SDAP-Config-r16");
    sl_sdap_cfg_r16.to_json(j);
  }
  if (sl_pdcp_cfg_r16_present) {
    j.write_fieldname("sl-PDCP-Config-r16");
    sl_pdcp_cfg_r16.to_json(j);
  }
  if (sl_trans_range_r16_present) {
    j.write_str("sl-TransRange-r16", sl_trans_range_r16.to_string());
  }
  j.end_obj();
}

const char* sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_opts::to_string() const
{
  static const char* names[] = {"m20",    "m50",    "m80",    "m100",   "m120",   "m150",   "m180",   "m200",
                                "m220",   "m250",   "m270",   "m300",   "m350",   "m370",   "m400",   "m420",
                                "m450",   "m480",   "m500",   "m550",   "m600",   "m700",   "m1000",  "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_e_");
}
uint16_t sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {20,  50,  80,  100, 120, 150, 180, 200, 220, 250, 270, 300,
                                     350, 370, 400, 420, 450, 480, 500, 550, 600, 700, 1000};
  return map_enum_number(numbers, 23, value, "sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_e_");
}

// SL-SRAP-Config-r17 ::= SEQUENCE
SRSASN_CODE sl_srap_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_local_id_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_map_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_map_to_release_list_r17.size() > 0, 1));

  if (sl_local_id_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_local_id_r17, (uint16_t)0u, (uint16_t)255u));
  }
  if (sl_map_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_map_to_add_mod_list_r17, 1, 32));
  }
  if (sl_map_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_map_to_release_list_r17, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_srap_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_local_id_r17_present, 1));
  bool sl_map_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_map_to_add_mod_list_r17_present, 1));
  bool sl_map_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_map_to_release_list_r17_present, 1));

  if (sl_local_id_r17_present) {
    HANDLE_CODE(unpack_integer(sl_local_id_r17, bref, (uint16_t)0u, (uint16_t)255u));
  }
  if (sl_map_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_map_to_add_mod_list_r17, bref, 1, 32));
  }
  if (sl_map_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_map_to_release_list_r17, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void sl_srap_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_local_id_r17_present) {
    j.write_int("sl-LocalIdentity-r17", sl_local_id_r17);
  }
  if (sl_map_to_add_mod_list_r17.size() > 0) {
    j.start_array("sl-MappingToAddModList-r17");
    for (const auto& e1 : sl_map_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_map_to_release_list_r17.size() > 0) {
    j.start_array("sl-MappingToReleaseList-r17");
    for (const auto& e1 : sl_map_to_release_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SRB-ToAddMod ::= SEQUENCE
SRSASN_CODE srb_to_add_mod_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.pack(discard_on_pdcp_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_present, 1));

  HANDLE_CODE(pack_integer(bref, srb_id, (uint8_t)1u, (uint8_t)3u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= srb_id_v1700_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srb_id_v1700_present, 1));
      if (srb_id_v1700_present) {
        HANDLE_CODE(pack_integer(bref, srb_id_v1700, (uint8_t)4u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(discard_on_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_present, 1));

  HANDLE_CODE(unpack_integer(srb_id, bref, (uint8_t)1u, (uint8_t)3u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(srb_id_v1700_present, 1));
      if (srb_id_v1700_present) {
        HANDLE_CODE(unpack_integer(srb_id_v1700, bref, (uint8_t)4u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srb_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srb-Identity", srb_id);
  if (reestablish_pdcp_present) {
    j.write_str("reestablishPDCP", "true");
  }
  if (discard_on_pdcp_present) {
    j.write_str("discardOnPDCP", "true");
  }
  if (pdcp_cfg_present) {
    j.write_fieldname("pdcp-Config");
    pdcp_cfg.to_json(j);
  }
  if (ext) {
    if (srb_id_v1700_present) {
      j.write_int("srb-Identity-v1700", srb_id_v1700);
    }
  }
  j.end_obj();
}

// SecurityAlgorithmConfig ::= SEQUENCE
SRSASN_CODE security_algorithm_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(integrity_prot_algorithm_present, 1));

  HANDLE_CODE(ciphering_algorithm.pack(bref));
  if (integrity_prot_algorithm_present) {
    HANDLE_CODE(integrity_prot_algorithm.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_algorithm_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(integrity_prot_algorithm_present, 1));

  HANDLE_CODE(ciphering_algorithm.unpack(bref));
  if (integrity_prot_algorithm_present) {
    HANDLE_CODE(integrity_prot_algorithm.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_algorithm_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cipheringAlgorithm", ciphering_algorithm.to_string());
  if (integrity_prot_algorithm_present) {
    j.write_str("integrityProtAlgorithm", integrity_prot_algorithm.to_string());
  }
  j.end_obj();
}

// SL-ConfigDedicatedNR-r16 ::= SEQUENCE
SRSASN_CODE sl_cfg_ded_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_phy_mac_rlc_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_radio_bearer_to_release_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_radio_bearer_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_cfg_info_to_release_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_cfg_info_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(t400_r16_present, 1));

  if (sl_phy_mac_rlc_cfg_r16_present) {
    HANDLE_CODE(sl_phy_mac_rlc_cfg_r16.pack(bref));
  }
  if (sl_radio_bearer_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_radio_bearer_to_release_list_r16, 1, 512, integer_packer<uint16_t>(1, 512)));
  }
  if (sl_radio_bearer_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_radio_bearer_to_add_mod_list_r16, 1, 512));
  }
  if (sl_meas_cfg_info_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_cfg_info_to_release_list_r16, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (sl_meas_cfg_info_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_cfg_info_to_add_mod_list_r16, 1, 32));
  }
  if (t400_r16_present) {
    HANDLE_CODE(t400_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_phy_mac_rlc_cfg_v1700.is_present();
    group_flags[0] |= sl_disc_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_phy_mac_rlc_cfg_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_disc_cfg_r17.is_present(), 1));
      if (sl_phy_mac_rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(sl_phy_mac_rlc_cfg_v1700->pack(bref));
      }
      if (sl_disc_cfg_r17.is_present()) {
        HANDLE_CODE(sl_disc_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cfg_ded_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_phy_mac_rlc_cfg_r16_present, 1));
  bool sl_radio_bearer_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_radio_bearer_to_release_list_r16_present, 1));
  bool sl_radio_bearer_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_radio_bearer_to_add_mod_list_r16_present, 1));
  bool sl_meas_cfg_info_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_cfg_info_to_release_list_r16_present, 1));
  bool sl_meas_cfg_info_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_cfg_info_to_add_mod_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(t400_r16_present, 1));

  if (sl_phy_mac_rlc_cfg_r16_present) {
    HANDLE_CODE(sl_phy_mac_rlc_cfg_r16.unpack(bref));
  }
  if (sl_radio_bearer_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_radio_bearer_to_release_list_r16, bref, 1, 512, integer_packer<uint16_t>(1, 512)));
  }
  if (sl_radio_bearer_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_radio_bearer_to_add_mod_list_r16, bref, 1, 512));
  }
  if (sl_meas_cfg_info_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_cfg_info_to_release_list_r16, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (sl_meas_cfg_info_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_cfg_info_to_add_mod_list_r16, bref, 1, 32));
  }
  if (t400_r16_present) {
    HANDLE_CODE(t400_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_phy_mac_rlc_cfg_v1700, bref);
      unpack_presence_flag(sl_disc_cfg_r17, bref);
      if (sl_phy_mac_rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(sl_phy_mac_rlc_cfg_v1700->unpack(bref));
      }
      if (sl_disc_cfg_r17.is_present()) {
        HANDLE_CODE(sl_disc_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_cfg_ded_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_phy_mac_rlc_cfg_r16_present) {
    j.write_fieldname("sl-PHY-MAC-RLC-Config-r16");
    sl_phy_mac_rlc_cfg_r16.to_json(j);
  }
  if (sl_radio_bearer_to_release_list_r16.size() > 0) {
    j.start_array("sl-RadioBearerToReleaseList-r16");
    for (const auto& e1 : sl_radio_bearer_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_radio_bearer_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-RadioBearerToAddModList-r16");
    for (const auto& e1 : sl_radio_bearer_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_meas_cfg_info_to_release_list_r16.size() > 0) {
    j.start_array("sl-MeasConfigInfoToReleaseList-r16");
    for (const auto& e1 : sl_meas_cfg_info_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_meas_cfg_info_to_add_mod_list_r16.size() > 0) {
    j.start_array("sl-MeasConfigInfoToAddModList-r16");
    for (const auto& e1 : sl_meas_cfg_info_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (t400_r16_present) {
    j.write_str("t400-r16", t400_r16.to_string());
  }
  if (ext) {
    if (sl_phy_mac_rlc_cfg_v1700.is_present()) {
      j.write_fieldname("sl-PHY-MAC-RLC-Config-v1700");
      sl_phy_mac_rlc_cfg_v1700->to_json(j);
    }
    if (sl_disc_cfg_r17.is_present()) {
      j.write_fieldname("sl-DiscConfig-r17");
      sl_disc_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* sl_cfg_ded_nr_r16_s::t400_r16_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "sl_cfg_ded_nr_r16_s::t400_r16_e_");
}
uint16_t sl_cfg_ded_nr_r16_s::t400_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "sl_cfg_ded_nr_r16_s::t400_r16_e_");
}

// SL-L2RemoteUE-Config-r17 ::= SEQUENCE
SRSASN_CODE sl_l2_remote_ue_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_srap_cfg_remote_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_ue_id_remote_r17_present, 1));

  if (sl_srap_cfg_remote_r17_present) {
    HANDLE_CODE(sl_srap_cfg_remote_r17.pack(bref));
  }
  if (sl_ue_id_remote_r17_present) {
    HANDLE_CODE(pack_integer(bref, sl_ue_id_remote_r17, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_l2_remote_ue_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_srap_cfg_remote_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_ue_id_remote_r17_present, 1));

  if (sl_srap_cfg_remote_r17_present) {
    HANDLE_CODE(sl_srap_cfg_remote_r17.unpack(bref));
  }
  if (sl_ue_id_remote_r17_present) {
    HANDLE_CODE(unpack_integer(sl_ue_id_remote_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
void sl_l2_remote_ue_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_srap_cfg_remote_r17_present) {
    j.write_fieldname("sl-SRAP-ConfigRemote-r17");
    sl_srap_cfg_remote_r17.to_json(j);
  }
  if (sl_ue_id_remote_r17_present) {
    j.write_int("sl-UEIdentityRemote-r17", sl_ue_id_remote_r17);
  }
  j.end_obj();
}

// SecurityConfig ::= SEQUENCE
SRSASN_CODE security_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(security_algorithm_cfg_present, 1));
  HANDLE_CODE(bref.pack(key_to_use_present, 1));

  if (security_algorithm_cfg_present) {
    HANDLE_CODE(security_algorithm_cfg.pack(bref));
  }
  if (key_to_use_present) {
    HANDLE_CODE(key_to_use.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(security_algorithm_cfg_present, 1));
  HANDLE_CODE(bref.unpack(key_to_use_present, 1));

  if (security_algorithm_cfg_present) {
    HANDLE_CODE(security_algorithm_cfg.unpack(bref));
  }
  if (key_to_use_present) {
    HANDLE_CODE(key_to_use.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (security_algorithm_cfg_present) {
    j.write_fieldname("securityAlgorithmConfig");
    security_algorithm_cfg.to_json(j);
  }
  if (key_to_use_present) {
    j.write_str("keyToUse", key_to_use.to_string());
  }
  j.end_obj();
}

const char* security_cfg_s::key_to_use_opts::to_string() const
{
  static const char* names[] = {"master", "secondary"};
  return convert_enum_idx(names, 2, value, "security_cfg_s::key_to_use_e_");
}

// RRCSetup-v1700-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_cfg_ded_nr_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_l2_remote_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sl_cfg_ded_nr_r17_present) {
    HANDLE_CODE(sl_cfg_ded_nr_r17.pack(bref));
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_cfg_ded_nr_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_l2_remote_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sl_cfg_ded_nr_r17_present) {
    HANDLE_CODE(sl_cfg_ded_nr_r17.unpack(bref));
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_setup_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_cfg_ded_nr_r17_present) {
    j.write_fieldname("sl-ConfigDedicatedNR-r17");
    sl_cfg_ded_nr_r17.to_json(j);
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    j.write_fieldname("sl-L2RemoteUE-Config-r17");
    sl_l2_remote_ue_cfg_r17.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RadioBearerConfig ::= SEQUENCE
SRSASN_CODE radio_bearer_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srb_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(srb3_to_release_present, 1));
  HANDLE_CODE(bref.pack(drb_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(security_cfg_present, 1));

  if (srb_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srb_to_add_mod_list, 1, 2));
  }
  if (drb_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_add_mod_list, 1, 29));
  }
  if (drb_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_release_list, 1, 29, integer_packer<uint8_t>(1, 32)));
  }
  if (security_cfg_present) {
    HANDLE_CODE(security_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mrb_to_add_mod_list_r17.is_present();
    group_flags[0] |= mrb_to_release_list_r17.is_present();
    group_flags[0] |= srb4_to_add_mod_r17.is_present();
    group_flags[0] |= srb4_to_release_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mrb_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(mrb_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(srb4_to_add_mod_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(srb4_to_release_r17_present, 1));
      if (mrb_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mrb_to_add_mod_list_r17, 1, 32));
      }
      if (mrb_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mrb_to_release_list_r17, 1, 32, integer_packer<uint16_t>(1, 512)));
      }
      if (srb4_to_add_mod_r17.is_present()) {
        HANDLE_CODE(srb4_to_add_mod_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE radio_bearer_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool srb_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(srb_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(srb3_to_release_present, 1));
  bool drb_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(drb_to_add_mod_list_present, 1));
  bool drb_to_release_list_present;
  HANDLE_CODE(bref.unpack(drb_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(security_cfg_present, 1));

  if (srb_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srb_to_add_mod_list, bref, 1, 2));
  }
  if (drb_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_add_mod_list, bref, 1, 29));
  }
  if (drb_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_release_list, bref, 1, 29, integer_packer<uint8_t>(1, 32)));
  }
  if (security_cfg_present) {
    HANDLE_CODE(security_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(mrb_to_add_mod_list_r17, bref);
      unpack_presence_flag(mrb_to_release_list_r17, bref);
      unpack_presence_flag(srb4_to_add_mod_r17, bref);
      HANDLE_CODE(bref.unpack(srb4_to_release_r17_present, 1));
      if (mrb_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mrb_to_add_mod_list_r17, bref, 1, 32));
      }
      if (mrb_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mrb_to_release_list_r17, bref, 1, 32, integer_packer<uint16_t>(1, 512)));
      }
      if (srb4_to_add_mod_r17.is_present()) {
        HANDLE_CODE(srb4_to_add_mod_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void radio_bearer_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srb_to_add_mod_list.size() > 0) {
    j.start_array("srb-ToAddModList");
    for (const auto& e1 : srb_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srb3_to_release_present) {
    j.write_str("srb3-ToRelease", "true");
  }
  if (drb_to_add_mod_list.size() > 0) {
    j.start_array("drb-ToAddModList");
    for (const auto& e1 : drb_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_release_list.size() > 0) {
    j.start_array("drb-ToReleaseList");
    for (const auto& e1 : drb_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (security_cfg_present) {
    j.write_fieldname("securityConfig");
    security_cfg.to_json(j);
  }
  if (ext) {
    if (mrb_to_add_mod_list_r17.is_present()) {
      j.start_array("mrb-ToAddModList-r17");
      for (const auto& e1 : *mrb_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (mrb_to_release_list_r17.is_present()) {
      j.start_array("mrb-ToReleaseList-r17");
      for (const auto& e1 : *mrb_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (srb4_to_add_mod_r17.is_present()) {
      j.write_fieldname("srb4-ToAddMod-r17");
      srb4_to_add_mod_r17->to_json(j);
    }
    if (srb4_to_release_r17_present) {
      j.write_str("srb4-ToRelease-r17", "true");
    }
  }
  j.end_obj();
}

// RRCReject-IEs ::= SEQUENCE
SRSASN_CODE rrc_reject_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wait_time_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(pack_integer(bref, wait_time, (uint8_t)1u, (uint8_t)16u));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reject_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wait_time_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(unpack_integer(wait_time, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_reject_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wait_time_present) {
    j.write_int("waitTime", wait_time);
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

// RRCSetup-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(radio_bearer_cfg.pack(bref));
  HANDLE_CODE(master_cell_group.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(radio_bearer_cfg.unpack(bref));
  HANDLE_CODE(master_cell_group.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_setup_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("radioBearerConfig");
  radio_bearer_cfg.to_json(j);
  j.write_str("masterCellGroup", master_cell_group.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCReject ::= SEQUENCE
SRSASN_CODE rrc_reject_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reject_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reject_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_reject_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_reject_ies_s& rrc_reject_s::crit_exts_c_::set_rrc_reject()
{
  set(types::rrc_reject);
  return c;
}
void rrc_reject_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_reject_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_reject:
      j.write_fieldname("rrcReject");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reject_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_reject_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reject_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reject_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reject_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCSetup ::= SEQUENCE
SRSASN_CODE rrc_setup_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_setup_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_setup_ies_s& rrc_setup_s::crit_exts_c_::set_rrc_setup()
{
  set(types::rrc_setup);
  return c;
}
void rrc_setup_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_setup_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_setup:
      j.write_fieldname("rrcSetup");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_setup_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_setup:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DL-CCCH-MessageType ::= CHOICE
void dl_ccch_msg_type_c::set(types::options e)
{
  type_ = e;
}
dl_ccch_msg_type_c::c1_c_& dl_ccch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void dl_ccch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void dl_ccch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_ccch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_reject:
      c.destroy<rrc_reject_s>();
      break;
    case types::rrc_setup:
      c.destroy<rrc_setup_s>();
      break;
    default:
      break;
  }
}
void dl_ccch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_reject:
      c.init<rrc_reject_s>();
      break;
    case types::rrc_setup:
      c.init<rrc_setup_s>();
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
}
dl_ccch_msg_type_c::c1_c_::c1_c_(const dl_ccch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_reject:
      c.init(other.c.get<rrc_reject_s>());
      break;
    case types::rrc_setup:
      c.init(other.c.get<rrc_setup_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
}
dl_ccch_msg_type_c::c1_c_& dl_ccch_msg_type_c::c1_c_::operator=(const dl_ccch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_reject:
      c.set(other.c.get<rrc_reject_s>());
      break;
    case types::rrc_setup:
      c.set(other.c.get<rrc_setup_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }

  return *this;
}
rrc_reject_s& dl_ccch_msg_type_c::c1_c_::set_rrc_reject()
{
  set(types::rrc_reject);
  return c.get<rrc_reject_s>();
}
rrc_setup_s& dl_ccch_msg_type_c::c1_c_::set_rrc_setup()
{
  set(types::rrc_setup);
  return c.get<rrc_setup_s>();
}
void dl_ccch_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_ccch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void dl_ccch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_reject:
      j.write_fieldname("rrcReject");
      c.get<rrc_reject_s>().to_json(j);
      break;
    case types::rrc_setup:
      j.write_fieldname("rrcSetup");
      c.get<rrc_setup_s>().to_json(j);
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.get<rrc_reject_s>().pack(bref));
      break;
    case types::rrc_setup:
      HANDLE_CODE(c.get<rrc_setup_s>().pack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.get<rrc_reject_s>().unpack(bref));
      break;
    case types::rrc_setup:
      HANDLE_CODE(c.get<rrc_setup_s>().unpack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DL-CCCH-Message ::= SEQUENCE
SRSASN_CODE dl_ccch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void dl_ccch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("DL-CCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// MUSIM-Starting-SFN-AndSubframe-r17 ::= SEQUENCE
SRSASN_CODE mu_si_m_start_sfn_and_sf_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, start_sfn_r17, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, start_sf_r17, (uint8_t)0u, (uint8_t)9u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_start_sfn_and_sf_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(start_sfn_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(start_sf_r17, bref, (uint8_t)0u, (uint8_t)9u));

  return SRSASN_SUCCESS;
}
void mu_si_m_start_sfn_and_sf_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("starting-SFN-r17", start_sfn_r17);
  j.write_int("startingSubframe-r17", start_sf_r17);
  j.end_obj();
}

// MUSIM-GapInfo-r17 ::= SEQUENCE
SRSASN_CODE mu_si_m_gap_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(musim_start_sfn_and_sf_r17_present, 1));
  HANDLE_CODE(bref.pack(musim_gap_len_r17_present, 1));
  HANDLE_CODE(bref.pack(musim_gap_repeat_and_offset_r17_present, 1));

  if (musim_start_sfn_and_sf_r17_present) {
    HANDLE_CODE(musim_start_sfn_and_sf_r17.pack(bref));
  }
  if (musim_gap_len_r17_present) {
    HANDLE_CODE(musim_gap_len_r17.pack(bref));
  }
  if (musim_gap_repeat_and_offset_r17_present) {
    HANDLE_CODE(musim_gap_repeat_and_offset_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_gap_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(musim_start_sfn_and_sf_r17_present, 1));
  HANDLE_CODE(bref.unpack(musim_gap_len_r17_present, 1));
  HANDLE_CODE(bref.unpack(musim_gap_repeat_and_offset_r17_present, 1));

  if (musim_start_sfn_and_sf_r17_present) {
    HANDLE_CODE(musim_start_sfn_and_sf_r17.unpack(bref));
  }
  if (musim_gap_len_r17_present) {
    HANDLE_CODE(musim_gap_len_r17.unpack(bref));
  }
  if (musim_gap_repeat_and_offset_r17_present) {
    HANDLE_CODE(musim_gap_repeat_and_offset_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mu_si_m_gap_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (musim_start_sfn_and_sf_r17_present) {
    j.write_fieldname("musim-Starting-SFN-AndSubframe-r17");
    musim_start_sfn_and_sf_r17.to_json(j);
  }
  if (musim_gap_len_r17_present) {
    j.write_str("musim-GapLength-r17", musim_gap_len_r17.to_string());
  }
  if (musim_gap_repeat_and_offset_r17_present) {
    j.write_fieldname("musim-GapRepetitionAndOffset-r17");
    musim_gap_repeat_and_offset_r17.to_json(j);
  }
  j.end_obj();
}

const char* mu_si_m_gap_info_r17_s::musim_gap_len_r17_opts::to_string() const
{
  static const char* names[] = {"ms3", "ms4", "ms6", "ms10", "ms20"};
  return convert_enum_idx(names, 5, value, "mu_si_m_gap_info_r17_s::musim_gap_len_r17_e_");
}
uint8_t mu_si_m_gap_info_r17_s::musim_gap_len_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4, 6, 10, 20};
  return map_enum_number(numbers, 5, value, "mu_si_m_gap_info_r17_s::musim_gap_len_r17_e_");
}

void mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::destroy_() {}
void mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::musim_gap_repeat_and_offset_r17_c_(
    const mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms20_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms40_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms80_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms160_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms320_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms640_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1280_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2560_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms5120_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_");
  }
}
mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_&
mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::operator=(
    const mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms20_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms40_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms80_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms160_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms320_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms640_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1280_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2560_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms5120_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_");
  }

  return *this;
}
uint8_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms20_r17()
{
  set(types::ms20_r17);
  return c.get<uint8_t>();
}
uint8_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms40_r17()
{
  set(types::ms40_r17);
  return c.get<uint8_t>();
}
uint8_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms80_r17()
{
  set(types::ms80_r17);
  return c.get<uint8_t>();
}
uint8_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms160_r17()
{
  set(types::ms160_r17);
  return c.get<uint8_t>();
}
uint16_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms320_r17()
{
  set(types::ms320_r17);
  return c.get<uint16_t>();
}
uint16_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms640_r17()
{
  set(types::ms640_r17);
  return c.get<uint16_t>();
}
uint16_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms1280_r17()
{
  set(types::ms1280_r17);
  return c.get<uint16_t>();
}
uint16_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms2560_r17()
{
  set(types::ms2560_r17);
  return c.get<uint16_t>();
}
uint16_t& mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::set_ms5120_r17()
{
  set(types::ms5120_r17);
  return c.get<uint16_t>();
}
void mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms20_r17:
      j.write_int("ms20-r17", c.get<uint8_t>());
      break;
    case types::ms40_r17:
      j.write_int("ms40-r17", c.get<uint8_t>());
      break;
    case types::ms80_r17:
      j.write_int("ms80-r17", c.get<uint8_t>());
      break;
    case types::ms160_r17:
      j.write_int("ms160-r17", c.get<uint8_t>());
      break;
    case types::ms320_r17:
      j.write_int("ms320-r17", c.get<uint16_t>());
      break;
    case types::ms640_r17:
      j.write_int("ms640-r17", c.get<uint16_t>());
      break;
    case types::ms1280_r17:
      j.write_int("ms1280-r17", c.get<uint16_t>());
      break;
    case types::ms2560_r17:
      j.write_int("ms2560-r17", c.get<uint16_t>());
      break;
    case types::ms5120_r17:
      j.write_int("ms5120-r17", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms20_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms40_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms80_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms160_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms320_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms640_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1280_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2560_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    default:
      log_invalid_choice_id(type_, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms20_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms40_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms80_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms160_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms320_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms640_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1280_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2560_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    default:
      log_invalid_choice_id(type_, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// IAB-IP-Address-r16 ::= CHOICE
void iab_ip_address_r16_c::destroy_()
{
  switch (type_) {
    case types::ip_v4_address_r16:
      c.destroy<fixed_bitstring<32>>();
      break;
    case types::ip_v6_address_r16:
      c.destroy<fixed_bitstring<128>>();
      break;
    case types::ip_v6_prefix_r16:
      c.destroy<fixed_bitstring<64>>();
      break;
    default:
      break;
  }
}
void iab_ip_address_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ip_v4_address_r16:
      c.init<fixed_bitstring<32>>();
      break;
    case types::ip_v6_address_r16:
      c.init<fixed_bitstring<128>>();
      break;
    case types::ip_v6_prefix_r16:
      c.init<fixed_bitstring<64>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_ip_address_r16_c");
  }
}
iab_ip_address_r16_c::iab_ip_address_r16_c(const iab_ip_address_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ip_v4_address_r16:
      c.init(other.c.get<fixed_bitstring<32>>());
      break;
    case types::ip_v6_address_r16:
      c.init(other.c.get<fixed_bitstring<128>>());
      break;
    case types::ip_v6_prefix_r16:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_ip_address_r16_c");
  }
}
iab_ip_address_r16_c& iab_ip_address_r16_c::operator=(const iab_ip_address_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ip_v4_address_r16:
      c.set(other.c.get<fixed_bitstring<32>>());
      break;
    case types::ip_v6_address_r16:
      c.set(other.c.get<fixed_bitstring<128>>());
      break;
    case types::ip_v6_prefix_r16:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_ip_address_r16_c");
  }

  return *this;
}
fixed_bitstring<32>& iab_ip_address_r16_c::set_ip_v4_address_r16()
{
  set(types::ip_v4_address_r16);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<128>& iab_ip_address_r16_c::set_ip_v6_address_r16()
{
  set(types::ip_v6_address_r16);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<64>& iab_ip_address_r16_c::set_ip_v6_prefix_r16()
{
  set(types::ip_v6_prefix_r16);
  return c.get<fixed_bitstring<64>>();
}
void iab_ip_address_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ip_v4_address_r16:
      j.write_str("iPv4-Address-r16", c.get<fixed_bitstring<32>>().to_string());
      break;
    case types::ip_v6_address_r16:
      j.write_str("iPv6-Address-r16", c.get<fixed_bitstring<128>>().to_string());
      break;
    case types::ip_v6_prefix_r16:
      j.write_str("iPv6-Prefix-r16", c.get<fixed_bitstring<64>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "iab_ip_address_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE iab_ip_address_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ip_v4_address_r16:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().pack(bref));
      break;
    case types::ip_v6_address_r16:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().pack(bref));
      break;
    case types::ip_v6_prefix_r16:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "iab_ip_address_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_ip_address_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ip_v4_address_r16:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().unpack(bref));
      break;
    case types::ip_v6_address_r16:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().unpack(bref));
      break;
    case types::ip_v6_prefix_r16:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "iab_ip_address_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasTriggerQuantity ::= CHOICE
void meas_trigger_quant_c::destroy_() {}
void meas_trigger_quant_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_trigger_quant_c::meas_trigger_quant_c(const meas_trigger_quant_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rsrp:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rsrq:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sinr:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_c");
  }
}
meas_trigger_quant_c& meas_trigger_quant_c::operator=(const meas_trigger_quant_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rsrp:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rsrq:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sinr:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_c");
  }

  return *this;
}
uint8_t& meas_trigger_quant_c::set_rsrp()
{
  set(types::rsrp);
  return c.get<uint8_t>();
}
uint8_t& meas_trigger_quant_c::set_rsrq()
{
  set(types::rsrq);
  return c.get<uint8_t>();
}
uint8_t& meas_trigger_quant_c::set_sinr()
{
  set(types::sinr);
  return c.get<uint8_t>();
}
void meas_trigger_quant_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rsrp:
      j.write_int("rsrp", c.get<uint8_t>());
      break;
    case types::rsrq:
      j.write_int("rsrq", c.get<uint8_t>());
      break;
    case types::sinr:
      j.write_int("sinr", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_trigger_quant_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rsrp:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::rsrq:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sinr:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_trigger_quant_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rsrp:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::rsrq:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sinr:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasTriggerQuantityOffset ::= CHOICE
void meas_trigger_quant_offset_c::destroy_() {}
void meas_trigger_quant_offset_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_trigger_quant_offset_c::meas_trigger_quant_offset_c(const meas_trigger_quant_offset_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rsrp:
      c.init(other.c.get<int8_t>());
      break;
    case types::rsrq:
      c.init(other.c.get<int8_t>());
      break;
    case types::sinr:
      c.init(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_offset_c");
  }
}
meas_trigger_quant_offset_c& meas_trigger_quant_offset_c::operator=(const meas_trigger_quant_offset_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rsrp:
      c.set(other.c.get<int8_t>());
      break;
    case types::rsrq:
      c.set(other.c.get<int8_t>());
      break;
    case types::sinr:
      c.set(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_offset_c");
  }

  return *this;
}
int8_t& meas_trigger_quant_offset_c::set_rsrp()
{
  set(types::rsrp);
  return c.get<int8_t>();
}
int8_t& meas_trigger_quant_offset_c::set_rsrq()
{
  set(types::rsrq);
  return c.get<int8_t>();
}
int8_t& meas_trigger_quant_offset_c::set_sinr()
{
  set(types::sinr);
  return c.get<int8_t>();
}
void meas_trigger_quant_offset_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rsrp:
      j.write_int("rsrp", c.get<int8_t>());
      break;
    case types::rsrq:
      j.write_int("rsrq", c.get<int8_t>());
      break;
    case types::sinr:
      j.write_int("sinr", c.get<int8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_offset_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_trigger_quant_offset_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rsrp:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-30, (int8_t)30));
      break;
    case types::rsrq:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-30, (int8_t)30));
      break;
    case types::sinr:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-30, (int8_t)30));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_offset_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_trigger_quant_offset_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rsrp:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-30, (int8_t)30));
      break;
    case types::rsrq:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-30, (int8_t)30));
      break;
    case types::sinr:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-30, (int8_t)30));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_offset_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// NR-RS-Type ::= ENUMERATED
const char* nr_rs_type_opts::to_string() const
{
  static const char* names[] = {"ssb", "csi-rs"};
  return convert_enum_idx(names, 2, value, "nr_rs_type_e");
}

// CGI-Info-Logging-r16 ::= SEQUENCE
SRSASN_CODE cgi_info_logging_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tac_r16_present, 1));

  HANDLE_CODE(plmn_id_r16.pack(bref));
  HANDLE_CODE(cell_id_r16.pack(bref));
  if (tac_r16_present) {
    HANDLE_CODE(tac_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_info_logging_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tac_r16_present, 1));

  HANDLE_CODE(plmn_id_r16.unpack(bref));
  HANDLE_CODE(cell_id_r16.unpack(bref));
  if (tac_r16_present) {
    HANDLE_CODE(tac_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cgi_info_logging_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-r16");
  plmn_id_r16.to_json(j);
  j.write_str("cellIdentity-r16", cell_id_r16.to_string());
  if (tac_r16_present) {
    j.write_str("trackingAreaCode-r16", tac_r16.to_string());
  }
  j.end_obj();
}

// CondTriggerConfig-r16 ::= SEQUENCE
SRSASN_CODE cond_trigger_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(cond_event_id.pack(bref));
  HANDLE_CODE(rs_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cond_trigger_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(cond_event_id.unpack(bref));
  HANDLE_CODE(rs_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void cond_trigger_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("condEventId");
  cond_event_id.to_json(j);
  j.write_str("rsType-r16", rs_type_r16.to_string());
  j.end_obj();
}

void cond_trigger_cfg_r16_s::cond_event_id_c_::destroy_()
{
  switch (type_) {
    case types::cond_event_a3:
      c.destroy<cond_event_a3_s_>();
      break;
    case types::cond_event_a5:
      c.destroy<cond_event_a5_s_>();
      break;
    case types::cond_event_a4_r17:
      c.destroy<cond_event_a4_r17_s_>();
      break;
    case types::cond_event_d1_r17:
      c.destroy<cond_event_d1_r17_s_>();
      break;
    case types::cond_event_t1_r17:
      c.destroy<cond_event_t1_r17_s_>();
      break;
    default:
      break;
  }
}
void cond_trigger_cfg_r16_s::cond_event_id_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cond_event_a3:
      c.init<cond_event_a3_s_>();
      break;
    case types::cond_event_a5:
      c.init<cond_event_a5_s_>();
      break;
    case types::cond_event_a4_r17:
      c.init<cond_event_a4_r17_s_>();
      break;
    case types::cond_event_d1_r17:
      c.init<cond_event_d1_r17_s_>();
      break;
    case types::cond_event_t1_r17:
      c.init<cond_event_t1_r17_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cond_trigger_cfg_r16_s::cond_event_id_c_");
  }
}
cond_trigger_cfg_r16_s::cond_event_id_c_::cond_event_id_c_(const cond_trigger_cfg_r16_s::cond_event_id_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cond_event_a3:
      c.init(other.c.get<cond_event_a3_s_>());
      break;
    case types::cond_event_a5:
      c.init(other.c.get<cond_event_a5_s_>());
      break;
    case types::cond_event_a4_r17:
      c.init(other.c.get<cond_event_a4_r17_s_>());
      break;
    case types::cond_event_d1_r17:
      c.init(other.c.get<cond_event_d1_r17_s_>());
      break;
    case types::cond_event_t1_r17:
      c.init(other.c.get<cond_event_t1_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cond_trigger_cfg_r16_s::cond_event_id_c_");
  }
}
cond_trigger_cfg_r16_s::cond_event_id_c_&
cond_trigger_cfg_r16_s::cond_event_id_c_::operator=(const cond_trigger_cfg_r16_s::cond_event_id_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cond_event_a3:
      c.set(other.c.get<cond_event_a3_s_>());
      break;
    case types::cond_event_a5:
      c.set(other.c.get<cond_event_a5_s_>());
      break;
    case types::cond_event_a4_r17:
      c.set(other.c.get<cond_event_a4_r17_s_>());
      break;
    case types::cond_event_d1_r17:
      c.set(other.c.get<cond_event_d1_r17_s_>());
      break;
    case types::cond_event_t1_r17:
      c.set(other.c.get<cond_event_t1_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cond_trigger_cfg_r16_s::cond_event_id_c_");
  }

  return *this;
}
cond_trigger_cfg_r16_s::cond_event_id_c_::cond_event_a3_s_&
cond_trigger_cfg_r16_s::cond_event_id_c_::set_cond_event_a3()
{
  set(types::cond_event_a3);
  return c.get<cond_event_a3_s_>();
}
cond_trigger_cfg_r16_s::cond_event_id_c_::cond_event_a5_s_&
cond_trigger_cfg_r16_s::cond_event_id_c_::set_cond_event_a5()
{
  set(types::cond_event_a5);
  return c.get<cond_event_a5_s_>();
}
cond_trigger_cfg_r16_s::cond_event_id_c_::cond_event_a4_r17_s_&
cond_trigger_cfg_r16_s::cond_event_id_c_::set_cond_event_a4_r17()
{
  set(types::cond_event_a4_r17);
  return c.get<cond_event_a4_r17_s_>();
}
cond_trigger_cfg_r16_s::cond_event_id_c_::cond_event_d1_r17_s_&
cond_trigger_cfg_r16_s::cond_event_id_c_::set_cond_event_d1_r17()
{
  set(types::cond_event_d1_r17);
  return c.get<cond_event_d1_r17_s_>();
}
cond_trigger_cfg_r16_s::cond_event_id_c_::cond_event_t1_r17_s_&
cond_trigger_cfg_r16_s::cond_event_id_c_::set_cond_event_t1_r17()
{
  set(types::cond_event_t1_r17);
  return c.get<cond_event_t1_r17_s_>();
}
void cond_trigger_cfg_r16_s::cond_event_id_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cond_event_a3:
      j.write_fieldname("condEventA3");
      j.start_obj();
      j.write_fieldname("a3-Offset");
      c.get<cond_event_a3_s_>().a3_offset.to_json(j);
      j.write_int("hysteresis", c.get<cond_event_a3_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<cond_event_a3_s_>().time_to_trigger.to_string());
      j.end_obj();
      break;
    case types::cond_event_a5:
      j.write_fieldname("condEventA5");
      j.start_obj();
      j.write_fieldname("a5-Threshold1");
      c.get<cond_event_a5_s_>().a5_thres1.to_json(j);
      j.write_fieldname("a5-Threshold2");
      c.get<cond_event_a5_s_>().a5_thres2.to_json(j);
      j.write_int("hysteresis", c.get<cond_event_a5_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<cond_event_a5_s_>().time_to_trigger.to_string());
      j.end_obj();
      break;
    case types::cond_event_a4_r17:
      j.write_fieldname("condEventA4-r17");
      j.start_obj();
      j.write_fieldname("a4-Threshold-r17");
      c.get<cond_event_a4_r17_s_>().a4_thres_r17.to_json(j);
      j.write_int("hysteresis-r17", c.get<cond_event_a4_r17_s_>().hysteresis_r17);
      j.write_str("timeToTrigger-r17", c.get<cond_event_a4_r17_s_>().time_to_trigger_r17.to_string());
      j.end_obj();
      break;
    case types::cond_event_d1_r17:
      j.write_fieldname("condEventD1-r17");
      j.start_obj();
      j.write_int("distanceThreshFromReference1-r17", c.get<cond_event_d1_r17_s_>().distance_thresh_from_ref1_r17);
      j.write_int("distanceThreshFromReference2-r17", c.get<cond_event_d1_r17_s_>().distance_thresh_from_ref2_r17);
      j.write_str("referenceLocation1-r17", c.get<cond_event_d1_r17_s_>().ref_location1_r17.to_string());
      j.write_str("referenceLocation2-r17", c.get<cond_event_d1_r17_s_>().ref_location2_r17.to_string());
      j.write_int("hysteresisLocation-r17", c.get<cond_event_d1_r17_s_>().hysteresis_location_r17);
      j.write_str("timeToTrigger-r17", c.get<cond_event_d1_r17_s_>().time_to_trigger_r17.to_string());
      j.end_obj();
      break;
    case types::cond_event_t1_r17:
      j.write_fieldname("condEventT1-r17");
      j.start_obj();
      j.write_int("t1-Threshold-r17", c.get<cond_event_t1_r17_s_>().t1_thres_r17);
      j.write_int("duration-r17", c.get<cond_event_t1_r17_s_>().dur_r17);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cond_trigger_cfg_r16_s::cond_event_id_c_");
  }
  j.end_obj();
}
SRSASN_CODE cond_trigger_cfg_r16_s::cond_event_id_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cond_event_a3:
      HANDLE_CODE(c.get<cond_event_a3_s_>().a3_offset.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<cond_event_a3_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<cond_event_a3_s_>().time_to_trigger.pack(bref));
      break;
    case types::cond_event_a5:
      HANDLE_CODE(c.get<cond_event_a5_s_>().a5_thres1.pack(bref));
      HANDLE_CODE(c.get<cond_event_a5_s_>().a5_thres2.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<cond_event_a5_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<cond_event_a5_s_>().time_to_trigger.pack(bref));
      break;
    case types::cond_event_a4_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cond_event_a4_r17_s_>().a4_thres_r17.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<cond_event_a4_r17_s_>().hysteresis_r17, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<cond_event_a4_r17_s_>().time_to_trigger_r17.pack(bref));
    } break;
    case types::cond_event_d1_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_integer(
          bref, c.get<cond_event_d1_r17_s_>().distance_thresh_from_ref1_r17, (uint16_t)0u, (uint16_t)65525u));
      HANDLE_CODE(pack_integer(
          bref, c.get<cond_event_d1_r17_s_>().distance_thresh_from_ref2_r17, (uint16_t)0u, (uint16_t)65525u));
      HANDLE_CODE(c.get<cond_event_d1_r17_s_>().ref_location1_r17.pack(bref));
      HANDLE_CODE(c.get<cond_event_d1_r17_s_>().ref_location2_r17.pack(bref));
      HANDLE_CODE(
          pack_integer(bref, c.get<cond_event_d1_r17_s_>().hysteresis_location_r17, (uint16_t)0u, (uint16_t)32768u));
      HANDLE_CODE(c.get<cond_event_d1_r17_s_>().time_to_trigger_r17.pack(bref));
    } break;
    case types::cond_event_t1_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(
          pack_integer(bref, c.get<cond_event_t1_r17_s_>().t1_thres_r17, (uint64_t)0u, (uint64_t)549755813887u));
      HANDLE_CODE(pack_integer(bref, c.get<cond_event_t1_r17_s_>().dur_r17, (uint16_t)1u, (uint16_t)6000u));
    } break;
    default:
      log_invalid_choice_id(type_, "cond_trigger_cfg_r16_s::cond_event_id_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cond_trigger_cfg_r16_s::cond_event_id_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cond_event_a3:
      HANDLE_CODE(c.get<cond_event_a3_s_>().a3_offset.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<cond_event_a3_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<cond_event_a3_s_>().time_to_trigger.unpack(bref));
      break;
    case types::cond_event_a5:
      HANDLE_CODE(c.get<cond_event_a5_s_>().a5_thres1.unpack(bref));
      HANDLE_CODE(c.get<cond_event_a5_s_>().a5_thres2.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<cond_event_a5_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<cond_event_a5_s_>().time_to_trigger.unpack(bref));
      break;
    case types::cond_event_a4_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cond_event_a4_r17_s_>().a4_thres_r17.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<cond_event_a4_r17_s_>().hysteresis_r17, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<cond_event_a4_r17_s_>().time_to_trigger_r17.unpack(bref));
    } break;
    case types::cond_event_d1_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_integer(
          c.get<cond_event_d1_r17_s_>().distance_thresh_from_ref1_r17, bref, (uint16_t)0u, (uint16_t)65525u));
      HANDLE_CODE(unpack_integer(
          c.get<cond_event_d1_r17_s_>().distance_thresh_from_ref2_r17, bref, (uint16_t)0u, (uint16_t)65525u));
      HANDLE_CODE(c.get<cond_event_d1_r17_s_>().ref_location1_r17.unpack(bref));
      HANDLE_CODE(c.get<cond_event_d1_r17_s_>().ref_location2_r17.unpack(bref));
      HANDLE_CODE(
          unpack_integer(c.get<cond_event_d1_r17_s_>().hysteresis_location_r17, bref, (uint16_t)0u, (uint16_t)32768u));
      HANDLE_CODE(c.get<cond_event_d1_r17_s_>().time_to_trigger_r17.unpack(bref));
    } break;
    case types::cond_event_t1_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(
          unpack_integer(c.get<cond_event_t1_r17_s_>().t1_thres_r17, bref, (uint64_t)0u, (uint64_t)549755813887u));
      HANDLE_CODE(unpack_integer(c.get<cond_event_t1_r17_s_>().dur_r17, bref, (uint16_t)1u, (uint16_t)6000u));
    } break;
    default:
      log_invalid_choice_id(type_, "cond_trigger_cfg_r16_s::cond_event_id_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RAT-Type ::= ENUMERATED
const char* rat_type_opts::to_string() const
{
  static const char* names[] = {"nr", "eutra-nr", "eutra", "utra-fdd-v1610"};
  return convert_enum_idx(names, 4, value, "rat_type_e");
}

// RxTxTimeDiff-r17 ::= SEQUENCE
SRSASN_CODE rx_tx_time_diff_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(result_k5_r17_present, 1));

  if (result_k5_r17_present) {
    HANDLE_CODE(pack_integer(bref, result_k5_r17, (uint16_t)0u, (uint16_t)61565u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rx_tx_time_diff_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(result_k5_r17_present, 1));

  if (result_k5_r17_present) {
    HANDLE_CODE(unpack_integer(result_k5_r17, bref, (uint16_t)0u, (uint16_t)61565u));
  }

  return SRSASN_SUCCESS;
}
void rx_tx_time_diff_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (result_k5_r17_present) {
    j.write_int("result-k5-r17", result_k5_r17);
  }
  j.end_obj();
}

// TraceReference-r16 ::= SEQUENCE
SRSASN_CODE trace_ref_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r16.pack(bref));
  HANDLE_CODE(trace_id_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE trace_ref_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r16.unpack(bref));
  HANDLE_CODE(trace_id_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void trace_ref_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-r16");
  plmn_id_r16.to_json(j);
  j.write_str("traceId-r16", trace_id_r16.to_string());
  j.end_obj();
}

// EstablishmentCause ::= ENUMERATED
const char* establishment_cause_opts::to_string() const
{
  static const char* names[] = {"emergency",
                                "highPriorityAccess",
                                "mt-Access",
                                "mo-Signalling",
                                "mo-Data",
                                "mo-VoiceCall",
                                "mo-VideoCall",
                                "mo-SMS",
                                "mps-PriorityAccess",
                                "mcs-PriorityAccess",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "establishment_cause_e");
}

// InitialUE-Identity ::= CHOICE
void init_ue_id_c::destroy_()
{
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.destroy<fixed_bitstring<39>>();
      break;
    case types::random_value:
      c.destroy<fixed_bitstring<39>>();
      break;
    default:
      break;
  }
}
void init_ue_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.init<fixed_bitstring<39>>();
      break;
    case types::random_value:
      c.init<fixed_bitstring<39>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c::init_ue_id_c(const init_ue_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.init(other.c.get<fixed_bitstring<39>>());
      break;
    case types::random_value:
      c.init(other.c.get<fixed_bitstring<39>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c& init_ue_id_c::operator=(const init_ue_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.set(other.c.get<fixed_bitstring<39>>());
      break;
    case types::random_value:
      c.set(other.c.get<fixed_bitstring<39>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }

  return *this;
}
fixed_bitstring<39>& init_ue_id_c::set_ng_5_g_s_tmsi_part1()
{
  set(types::ng_5_g_s_tmsi_part1);
  return c.get<fixed_bitstring<39>>();
}
fixed_bitstring<39>& init_ue_id_c::set_random_value()
{
  set(types::random_value);
  return c.get<fixed_bitstring<39>>();
}
void init_ue_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      j.write_str("ng-5G-S-TMSI-Part1", c.get<fixed_bitstring<39>>().to_string());
      break;
    case types::random_value:
      j.write_str("randomValue", c.get<fixed_bitstring<39>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
  j.end_obj();
}
SRSASN_CODE init_ue_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().pack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE init_ue_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().unpack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ReestabUE-Identity ::= SEQUENCE
SRSASN_CODE reestab_ue_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, c_rnti, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(short_mac_i.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reestab_ue_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(short_mac_i.unpack(bref));

  return SRSASN_SUCCESS;
}
void reestab_ue_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("c-RNTI", c_rnti);
  j.write_int("physCellId", pci);
  j.write_str("shortMAC-I", short_mac_i.to_string());
  j.end_obj();
}

// ReestablishmentCause ::= ENUMERATED
const char* reest_cause_opts::to_string() const
{
  static const char* names[] = {"reconfigurationFailure", "handoverFailure", "otherFailure", "spare1"};
  return convert_enum_idx(names, 4, value, "reest_cause_e");
}

// ResumeCause ::= ENUMERATED
const char* resume_cause_opts::to_string() const
{
  static const char* names[] = {"emergency",
                                "highPriorityAccess",
                                "mt-Access",
                                "mo-Signalling",
                                "mo-Data",
                                "mo-VoiceCall",
                                "mo-VideoCall",
                                "mo-SMS",
                                "rna-Update",
                                "mps-PriorityAccess",
                                "mcs-PriorityAccess",
                                "spare1",
                                "spare2",
                                "spare3",
                                "spare4",
                                "spare5"};
  return convert_enum_idx(names, 16, value, "resume_cause_e");
}

// RRC-PosSystemInfoRequest-r16-IEs ::= SEQUENCE
SRSASN_CODE rrc_pos_sys_info_request_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(requested_pos_si_list.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_pos_sys_info_request_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(requested_pos_si_list.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_pos_sys_info_request_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("requestedPosSI-List", requested_pos_si_list.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCReestablishmentRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(reest_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(reest_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("reestablishmentCause", reest_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCResumeRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id.pack(bref));
  HANDLE_CODE(resume_mac_i.pack(bref));
  HANDLE_CODE(resume_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id.unpack(bref));
  HANDLE_CODE(resume_mac_i.unpack(bref));
  HANDLE_CODE(resume_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("resumeIdentity", resume_id.to_string());
  j.write_str("resumeMAC-I", resume_mac_i.to_string());
  j.write_str("resumeCause", resume_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCSetupRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(establishment_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(establishment_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("establishmentCause", establishment_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCSystemInfoRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_sys_info_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(requested_si_list.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(requested_si_list.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_sys_info_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("requested-SI-List", requested_si_list.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCReestablishmentRequest ::= SEQUENCE
SRSASN_CODE rrc_reest_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_reest_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_reest_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcReestablishmentRequest");
  rrc_reest_request.to_json(j);
  j.end_obj();
}

// RRCResumeRequest ::= SEQUENCE
SRSASN_CODE rrc_resume_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_resume_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_resume_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcResumeRequest");
  rrc_resume_request.to_json(j);
  j.end_obj();
}

// RRCSetupRequest ::= SEQUENCE
SRSASN_CODE rrc_setup_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_setup_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_setup_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcSetupRequest");
  rrc_setup_request.to_json(j);
  j.end_obj();
}

// RRCSystemInfoRequest ::= SEQUENCE
SRSASN_CODE rrc_sys_info_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_sys_info_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_sys_info_request_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_sys_info_request:
      c.destroy<rrc_sys_info_request_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.destroy<crit_exts_future_r16_c_>();
      break;
    default:
      break;
  }
}
void rrc_sys_info_request_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_sys_info_request:
      c.init<rrc_sys_info_request_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.init<crit_exts_future_r16_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }
}
rrc_sys_info_request_s::crit_exts_c_::crit_exts_c_(const rrc_sys_info_request_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_sys_info_request:
      c.init(other.c.get<rrc_sys_info_request_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.init(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }
}
rrc_sys_info_request_s::crit_exts_c_&
rrc_sys_info_request_s::crit_exts_c_::operator=(const rrc_sys_info_request_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_sys_info_request:
      c.set(other.c.get<rrc_sys_info_request_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.set(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }

  return *this;
}
rrc_sys_info_request_ies_s& rrc_sys_info_request_s::crit_exts_c_::set_rrc_sys_info_request()
{
  set(types::rrc_sys_info_request);
  return c.get<rrc_sys_info_request_ies_s>();
}
rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_&
rrc_sys_info_request_s::crit_exts_c_::set_crit_exts_future_r16()
{
  set(types::crit_exts_future_r16);
  return c.get<crit_exts_future_r16_c_>();
}
void rrc_sys_info_request_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_sys_info_request:
      j.write_fieldname("rrcSystemInfoRequest");
      c.get<rrc_sys_info_request_ies_s>().to_json(j);
      break;
    case types::crit_exts_future_r16:
      j.write_fieldname("criticalExtensionsFuture-r16");
      c.get<crit_exts_future_r16_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_ies_s>().pack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set(types::options e)
{
  type_ = e;
}
rrc_pos_sys_info_request_r16_ies_s&
rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set_rrc_pos_sys_info_request_r16()
{
  set(types::rrc_pos_sys_info_request_r16);
  return c;
}
void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_pos_sys_info_request_r16:
      j.write_fieldname("rrcPosSystemInfoRequest-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_pos_sys_info_request_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_pos_sys_info_request_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UL-CCCH-MessageType ::= CHOICE
void ul_ccch_msg_type_c::set(types::options e)
{
  type_ = e;
}
ul_ccch_msg_type_c::c1_c_& ul_ccch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void ul_ccch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void ul_ccch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_setup_request:
      c.destroy<rrc_setup_request_s>();
      break;
    case types::rrc_resume_request:
      c.destroy<rrc_resume_request_s>();
      break;
    case types::rrc_reest_request:
      c.destroy<rrc_reest_request_s>();
      break;
    case types::rrc_sys_info_request:
      c.destroy<rrc_sys_info_request_s>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_setup_request:
      c.init<rrc_setup_request_s>();
      break;
    case types::rrc_resume_request:
      c.init<rrc_resume_request_s>();
      break;
    case types::rrc_reest_request:
      c.init<rrc_reest_request_s>();
      break;
    case types::rrc_sys_info_request:
      c.init<rrc_sys_info_request_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
}
ul_ccch_msg_type_c::c1_c_::c1_c_(const ul_ccch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_setup_request:
      c.init(other.c.get<rrc_setup_request_s>());
      break;
    case types::rrc_resume_request:
      c.init(other.c.get<rrc_resume_request_s>());
      break;
    case types::rrc_reest_request:
      c.init(other.c.get<rrc_reest_request_s>());
      break;
    case types::rrc_sys_info_request:
      c.init(other.c.get<rrc_sys_info_request_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
}
ul_ccch_msg_type_c::c1_c_& ul_ccch_msg_type_c::c1_c_::operator=(const ul_ccch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_setup_request:
      c.set(other.c.get<rrc_setup_request_s>());
      break;
    case types::rrc_resume_request:
      c.set(other.c.get<rrc_resume_request_s>());
      break;
    case types::rrc_reest_request:
      c.set(other.c.get<rrc_reest_request_s>());
      break;
    case types::rrc_sys_info_request:
      c.set(other.c.get<rrc_sys_info_request_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }

  return *this;
}
rrc_setup_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_setup_request()
{
  set(types::rrc_setup_request);
  return c.get<rrc_setup_request_s>();
}
rrc_resume_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_resume_request()
{
  set(types::rrc_resume_request);
  return c.get<rrc_resume_request_s>();
}
rrc_reest_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_reest_request()
{
  set(types::rrc_reest_request);
  return c.get<rrc_reest_request_s>();
}
rrc_sys_info_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_sys_info_request()
{
  set(types::rrc_sys_info_request);
  return c.get<rrc_sys_info_request_s>();
}
void ul_ccch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_setup_request:
      j.write_fieldname("rrcSetupRequest");
      c.get<rrc_setup_request_s>().to_json(j);
      break;
    case types::rrc_resume_request:
      j.write_fieldname("rrcResumeRequest");
      c.get<rrc_resume_request_s>().to_json(j);
      break;
    case types::rrc_reest_request:
      j.write_fieldname("rrcReestablishmentRequest");
      c.get<rrc_reest_request_s>().to_json(j);
      break;
    case types::rrc_sys_info_request:
      j.write_fieldname("rrcSystemInfoRequest");
      c.get<rrc_sys_info_request_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_setup_request:
      HANDLE_CODE(c.get<rrc_setup_request_s>().pack(bref));
      break;
    case types::rrc_resume_request:
      HANDLE_CODE(c.get<rrc_resume_request_s>().pack(bref));
      break;
    case types::rrc_reest_request:
      HANDLE_CODE(c.get<rrc_reest_request_s>().pack(bref));
      break;
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_setup_request:
      HANDLE_CODE(c.get<rrc_setup_request_s>().unpack(bref));
      break;
    case types::rrc_resume_request:
      HANDLE_CODE(c.get<rrc_resume_request_s>().unpack(bref));
      break;
    case types::rrc_reest_request:
      HANDLE_CODE(c.get<rrc_reest_request_s>().unpack(bref));
      break;
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UL-CCCH-Message ::= SEQUENCE
SRSASN_CODE ul_ccch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_ccch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-CCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

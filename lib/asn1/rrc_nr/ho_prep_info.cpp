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

#include "srsran/asn1/rrc_nr/ho_prep_info.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// ReestabNCellInfo ::= SEQUENCE
SRSASN_CODE reestab_ncell_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_id.pack(bref));
  HANDLE_CODE(key_g_node_b_star.pack(bref));
  HANDLE_CODE(short_mac_i.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reestab_ncell_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_id.unpack(bref));
  HANDLE_CODE(key_g_node_b_star.unpack(bref));
  HANDLE_CODE(short_mac_i.unpack(bref));

  return SRSASN_SUCCESS;
}
void reestab_ncell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellIdentity", cell_id.to_string());
  j.write_str("key-gNodeB-Star", key_g_node_b_star.to_string());
  j.write_str("shortMAC-I", short_mac_i.to_string());
  j.end_obj();
}

// BandCombinationInfoSN ::= SEQUENCE
SRSASN_CODE band_combination_info_sn_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_combination_idx, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(pack_integer(bref, requested_feature_sets, (uint8_t)1u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_info_sn_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_combination_idx, bref, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(unpack_integer(requested_feature_sets, bref, (uint8_t)1u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
void band_combination_info_sn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandCombinationIndex", band_combination_idx);
  j.write_int("requestedFeatureSets", requested_feature_sets);
  j.end_obj();
}

// ConfigRestrictInfoDAPS-r16 ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_daps_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_coordination_r16_present, 1));

  if (pwr_coordination_r16_present) {
    HANDLE_CODE(pack_integer(bref, pwr_coordination_r16.p_daps_source_r16, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(pack_integer(bref, pwr_coordination_r16.p_daps_target_r16, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(pwr_coordination_r16.ul_pwr_sharing_daps_mode_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_daps_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_coordination_r16_present, 1));

  if (pwr_coordination_r16_present) {
    HANDLE_CODE(unpack_integer(pwr_coordination_r16.p_daps_source_r16, bref, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(unpack_integer(pwr_coordination_r16.p_daps_target_r16, bref, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(pwr_coordination_r16.ul_pwr_sharing_daps_mode_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cfg_restrict_info_daps_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pwr_coordination_r16_present) {
    j.write_fieldname("powerCoordination-r16");
    j.start_obj();
    j.write_int("p-DAPS-Source-r16", pwr_coordination_r16.p_daps_source_r16);
    j.write_int("p-DAPS-Target-r16", pwr_coordination_r16.p_daps_target_r16);
    j.write_str("uplinkPowerSharingDAPS-Mode-r16", pwr_coordination_r16.ul_pwr_sharing_daps_mode_r16.to_string());
    j.end_obj();
  }
  j.end_obj();
}

const char* cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(
      names, 3, value, "cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_e_");
}
uint8_t cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_e_");
}

// ConfigRestrictInfoDAPS-v1640 ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_daps_v1640_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, source_feature_set_per_dl_cc_r16, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(pack_integer(bref, source_feature_set_per_ul_cc_r16, (uint16_t)1u, (uint16_t)1024u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_daps_v1640_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(source_feature_set_per_dl_cc_r16, bref, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(unpack_integer(source_feature_set_per_ul_cc_r16, bref, (uint16_t)1u, (uint16_t)1024u));

  return SRSASN_SUCCESS;
}
void cfg_restrict_info_daps_v1640_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourceFeatureSetPerDownlinkCC-r16", source_feature_set_per_dl_cc_r16);
  j.write_int("sourceFeatureSetPerUplinkCC-r16", source_feature_set_per_ul_cc_r16);
  j.end_obj();
}

// ReestablishmentInfo ::= SEQUENCE
SRSASN_CODE reest_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_reestab_info_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, source_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_short_mac_i.pack(bref));
  if (add_reestab_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, add_reestab_info_list, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reest_info_s::unpack(cbit_ref& bref)
{
  bool add_reestab_info_list_present;
  HANDLE_CODE(bref.unpack(add_reestab_info_list_present, 1));

  HANDLE_CODE(unpack_integer(source_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_short_mac_i.unpack(bref));
  if (add_reestab_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(add_reestab_info_list, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void reest_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId", source_pci);
  j.write_str("targetCellShortMAC-I", target_cell_short_mac_i.to_string());
  if (add_reestab_info_list.size() > 0) {
    j.start_array("additionalReestabInfoList");
    for (const auto& e1 : add_reestab_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// AS-Config ::= SEQUENCE
SRSASN_CODE as_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(rrc_recfg.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= source_rb_sn_cfg.size() > 0;
    group_flags[0] |= source_scg_nr_cfg.size() > 0;
    group_flags[0] |= source_scg_eutra_cfg.size() > 0;
    group_flags[1] |= source_scg_cfg_present;
    group_flags[2] |= sdt_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_rb_sn_cfg.size() > 0, 1));
      HANDLE_CODE(bref.pack(source_scg_nr_cfg.size() > 0, 1));
      HANDLE_CODE(bref.pack(source_scg_eutra_cfg.size() > 0, 1));
      if (source_rb_sn_cfg.size() > 0) {
        HANDLE_CODE(source_rb_sn_cfg.pack(bref));
      }
      if (source_scg_nr_cfg.size() > 0) {
        HANDLE_CODE(source_scg_nr_cfg.pack(bref));
      }
      if (source_scg_eutra_cfg.size() > 0) {
        HANDLE_CODE(source_scg_eutra_cfg.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_scg_cfg_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sdt_cfg_r17.is_present(), 1));
      if (sdt_cfg_r17.is_present()) {
        HANDLE_CODE(sdt_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(rrc_recfg.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool source_rb_sn_cfg_present;
      HANDLE_CODE(bref.unpack(source_rb_sn_cfg_present, 1));
      bool source_scg_nr_cfg_present;
      HANDLE_CODE(bref.unpack(source_scg_nr_cfg_present, 1));
      bool source_scg_eutra_cfg_present;
      HANDLE_CODE(bref.unpack(source_scg_eutra_cfg_present, 1));
      if (source_rb_sn_cfg_present) {
        HANDLE_CODE(source_rb_sn_cfg.unpack(bref));
      }
      if (source_scg_nr_cfg_present) {
        HANDLE_CODE(source_scg_nr_cfg.unpack(bref));
      }
      if (source_scg_eutra_cfg_present) {
        HANDLE_CODE(source_scg_eutra_cfg.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(source_scg_cfg_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sdt_cfg_r17, bref);
      if (sdt_cfg_r17.is_present()) {
        HANDLE_CODE(sdt_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void as_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rrcReconfiguration", rrc_recfg.to_string());
  if (ext) {
    if (source_rb_sn_cfg.size() > 0) {
      j.write_str("sourceRB-SN-Config", source_rb_sn_cfg.to_string());
    }
    if (source_scg_nr_cfg.size() > 0) {
      j.write_str("sourceSCG-NR-Config", source_scg_nr_cfg.to_string());
    }
    if (source_scg_eutra_cfg.size() > 0) {
      j.write_str("sourceSCG-EUTRA-Config", source_scg_eutra_cfg.to_string());
    }
    if (source_scg_cfg_present) {
      j.write_str("sourceSCG-Configured", "true");
    }
    if (sdt_cfg_r17.is_present()) {
      j.write_fieldname("sdt-Config-r17");
      sdt_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

// AS-Context ::= SEQUENCE
SRSASN_CODE as_context_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(reest_info_present, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_info_present, 1));

  if (reest_info_present) {
    HANDLE_CODE(reest_info.pack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ran_notif_area_info.is_present();
    group_flags[1] |= ue_assist_info.size() > 0;
    group_flags[2] |= sel_band_combination_sn.is_present();
    group_flags[3] |= cfg_restrict_info_daps_r16.is_present();
    group_flags[3] |= sidelink_ue_info_nr_r16.size() > 0;
    group_flags[3] |= sidelink_ue_info_eutra_r16.size() > 0;
    group_flags[3] |= ue_assist_info_eutra_r16.size() > 0;
    group_flags[3] |= ue_assist_info_scg_r16.size() > 0;
    group_flags[3] |= need_for_gaps_info_nr_r16.is_present();
    group_flags[4] |= cfg_restrict_info_daps_v1640.is_present();
    group_flags[5] |= need_for_gap_ncsg_info_nr_r17.is_present();
    group_flags[5] |= need_for_gap_ncsg_info_eutra_r17.is_present();
    group_flags[5] |= mbs_interest_ind_r17.size() > 0;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ran_notif_area_info.is_present(), 1));
      if (ran_notif_area_info.is_present()) {
        HANDLE_CODE(ran_notif_area_info->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ue_assist_info.size() > 0, 1));
      if (ue_assist_info.size() > 0) {
        HANDLE_CODE(ue_assist_info.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sel_band_combination_sn.is_present(), 1));
      if (sel_band_combination_sn.is_present()) {
        HANDLE_CODE(sel_band_combination_sn->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cfg_restrict_info_daps_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(sidelink_ue_info_nr_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(sidelink_ue_info_eutra_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(ue_assist_info_eutra_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(ue_assist_info_scg_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(need_for_gaps_info_nr_r16.is_present(), 1));
      if (cfg_restrict_info_daps_r16.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_r16->pack(bref));
      }
      if (sidelink_ue_info_nr_r16.size() > 0) {
        HANDLE_CODE(sidelink_ue_info_nr_r16.pack(bref));
      }
      if (sidelink_ue_info_eutra_r16.size() > 0) {
        HANDLE_CODE(sidelink_ue_info_eutra_r16.pack(bref));
      }
      if (ue_assist_info_eutra_r16.size() > 0) {
        HANDLE_CODE(ue_assist_info_eutra_r16.pack(bref));
      }
      if (ue_assist_info_scg_r16.size() > 0) {
        HANDLE_CODE(ue_assist_info_scg_r16.pack(bref));
      }
      if (need_for_gaps_info_nr_r16.is_present()) {
        HANDLE_CODE(need_for_gaps_info_nr_r16->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cfg_restrict_info_daps_v1640.is_present(), 1));
      if (cfg_restrict_info_daps_v1640.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_v1640->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_nr_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_eutra_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(mbs_interest_ind_r17.size() > 0, 1));
      if (need_for_gap_ncsg_info_nr_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_nr_r17->pack(bref));
      }
      if (need_for_gap_ncsg_info_eutra_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17->pack(bref));
      }
      if (mbs_interest_ind_r17.size() > 0) {
        HANDLE_CODE(mbs_interest_ind_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(reest_info_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_info_present, 1));

  if (reest_info_present) {
    HANDLE_CODE(reest_info.unpack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(6);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ran_notif_area_info, bref);
      if (ran_notif_area_info.is_present()) {
        HANDLE_CODE(ran_notif_area_info->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ue_assist_info_present;
      HANDLE_CODE(bref.unpack(ue_assist_info_present, 1));
      if (ue_assist_info_present) {
        HANDLE_CODE(ue_assist_info.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sel_band_combination_sn, bref);
      if (sel_band_combination_sn.is_present()) {
        HANDLE_CODE(sel_band_combination_sn->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(cfg_restrict_info_daps_r16, bref);
      bool sidelink_ue_info_nr_r16_present;
      HANDLE_CODE(bref.unpack(sidelink_ue_info_nr_r16_present, 1));
      bool sidelink_ue_info_eutra_r16_present;
      HANDLE_CODE(bref.unpack(sidelink_ue_info_eutra_r16_present, 1));
      bool ue_assist_info_eutra_r16_present;
      HANDLE_CODE(bref.unpack(ue_assist_info_eutra_r16_present, 1));
      bool ue_assist_info_scg_r16_present;
      HANDLE_CODE(bref.unpack(ue_assist_info_scg_r16_present, 1));
      unpack_presence_flag(need_for_gaps_info_nr_r16, bref);
      if (cfg_restrict_info_daps_r16.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_r16->unpack(bref));
      }
      if (sidelink_ue_info_nr_r16_present) {
        HANDLE_CODE(sidelink_ue_info_nr_r16.unpack(bref));
      }
      if (sidelink_ue_info_eutra_r16_present) {
        HANDLE_CODE(sidelink_ue_info_eutra_r16.unpack(bref));
      }
      if (ue_assist_info_eutra_r16_present) {
        HANDLE_CODE(ue_assist_info_eutra_r16.unpack(bref));
      }
      if (ue_assist_info_scg_r16_present) {
        HANDLE_CODE(ue_assist_info_scg_r16.unpack(bref));
      }
      if (need_for_gaps_info_nr_r16.is_present()) {
        HANDLE_CODE(need_for_gaps_info_nr_r16->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(cfg_restrict_info_daps_v1640, bref);
      if (cfg_restrict_info_daps_v1640.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_v1640->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(need_for_gap_ncsg_info_nr_r17, bref);
      unpack_presence_flag(need_for_gap_ncsg_info_eutra_r17, bref);
      bool mbs_interest_ind_r17_present;
      HANDLE_CODE(bref.unpack(mbs_interest_ind_r17_present, 1));
      if (need_for_gap_ncsg_info_nr_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_nr_r17->unpack(bref));
      }
      if (need_for_gap_ncsg_info_eutra_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17->unpack(bref));
      }
      if (mbs_interest_ind_r17_present) {
        HANDLE_CODE(mbs_interest_ind_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void as_context_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reest_info_present) {
    j.write_fieldname("reestablishmentInfo");
    reest_info.to_json(j);
  }
  if (cfg_restrict_info_present) {
    j.write_fieldname("configRestrictInfo");
    cfg_restrict_info.to_json(j);
  }
  if (ext) {
    if (ran_notif_area_info.is_present()) {
      j.write_fieldname("ran-NotificationAreaInfo");
      ran_notif_area_info->to_json(j);
    }
    if (ue_assist_info.size() > 0) {
      j.write_str("ueAssistanceInformation", ue_assist_info.to_string());
    }
    if (sel_band_combination_sn.is_present()) {
      j.write_fieldname("selectedBandCombinationSN");
      sel_band_combination_sn->to_json(j);
    }
    if (cfg_restrict_info_daps_r16.is_present()) {
      j.write_fieldname("configRestrictInfoDAPS-r16");
      cfg_restrict_info_daps_r16->to_json(j);
    }
    if (sidelink_ue_info_nr_r16.size() > 0) {
      j.write_str("sidelinkUEInformationNR-r16", sidelink_ue_info_nr_r16.to_string());
    }
    if (sidelink_ue_info_eutra_r16.size() > 0) {
      j.write_str("sidelinkUEInformationEUTRA-r16", sidelink_ue_info_eutra_r16.to_string());
    }
    if (ue_assist_info_eutra_r16.size() > 0) {
      j.write_str("ueAssistanceInformationEUTRA-r16", ue_assist_info_eutra_r16.to_string());
    }
    if (ue_assist_info_scg_r16.size() > 0) {
      j.write_str("ueAssistanceInformationSCG-r16", ue_assist_info_scg_r16.to_string());
    }
    if (need_for_gaps_info_nr_r16.is_present()) {
      j.write_fieldname("needForGapsInfoNR-r16");
      need_for_gaps_info_nr_r16->to_json(j);
    }
    if (cfg_restrict_info_daps_v1640.is_present()) {
      j.write_fieldname("configRestrictInfoDAPS-v1640");
      cfg_restrict_info_daps_v1640->to_json(j);
    }
    if (need_for_gap_ncsg_info_nr_r17.is_present()) {
      j.write_fieldname("needForGapNCSG-InfoNR-r17");
      need_for_gap_ncsg_info_nr_r17->to_json(j);
    }
    if (need_for_gap_ncsg_info_eutra_r17.is_present()) {
      j.write_fieldname("needForGapNCSG-InfoEUTRA-r17");
      need_for_gap_ncsg_info_eutra_r17->to_json(j);
    }
    if (mbs_interest_ind_r17.size() > 0) {
      j.write_str("mbsInterestIndication-r17", mbs_interest_ind_r17.to_string());
    }
  }
  j.end_obj();
}

// RRM-Config ::= SEQUENCE
SRSASN_CODE rrm_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_inactive_time_present, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list.size() > 0, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.pack(bref));
  }
  if (candidate_cell_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_info_list, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= candidate_cell_info_list_sn_eutra.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(candidate_cell_info_list_sn_eutra.is_present(), 1));
      if (candidate_cell_info_list_sn_eutra.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *candidate_cell_info_list_sn_eutra, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrm_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_inactive_time_present, 1));
  bool candidate_cell_info_list_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_present, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.unpack(bref));
  }
  if (candidate_cell_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_info_list, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(candidate_cell_info_list_sn_eutra, bref);
      if (candidate_cell_info_list_sn_eutra.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*candidate_cell_info_list_sn_eutra, bref, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rrm_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_inactive_time_present) {
    j.write_str("ue-InactiveTime", ue_inactive_time.to_string());
  }
  if (candidate_cell_info_list.size() > 0) {
    j.start_array("candidateCellInfoList");
    for (const auto& e1 : candidate_cell_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (candidate_cell_info_list_sn_eutra.is_present()) {
      j.start_array("candidateCellInfoListSN-EUTRA");
      for (const auto& e1 : *candidate_cell_info_list_sn_eutra) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* rrm_cfg_s::ue_inactive_time_opts::to_string() const
{
  static const char* names[] = {
      "s1",    "s2",       "s3",    "s5",           "s7",      "s10",   "s15",     "s20",      "s25",     "s30",
      "s40",   "s50",      "min1",  "min1s20",      "min1s40", "min2",  "min2s30", "min3",     "min3s30", "min4",
      "min5",  "min6",     "min7",  "min8",         "min9",    "min10", "min12",   "min14",    "min17",   "min20",
      "min24", "min28",    "min33", "min38",        "min44",   "min50", "hr1",     "hr1min30", "hr2",     "hr2min30",
      "hr3",   "hr3min30", "hr4",   "hr5",          "hr6",     "hr8",   "hr10",    "hr13",     "hr16",    "hr20",
      "day1",  "day1hr12", "day2",  "day2hr12",     "day3",    "day4",  "day5",    "day7",     "day10",   "day14",
      "day19", "day24",    "day30", "dayMoreThan30"};
  return convert_enum_idx(names, 64, value, "rrm_cfg_s::ue_inactive_time_e_");
}

// HandoverPreparationInformation-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(source_cfg_present, 1));
  HANDLE_CODE(bref.pack(rrm_cfg_present, 1));
  HANDLE_CODE(bref.pack(as_context_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, ue_cap_rat_list, 0, 8));
  if (source_cfg_present) {
    HANDLE_CODE(source_cfg.pack(bref));
  }
  if (rrm_cfg_present) {
    HANDLE_CODE(rrm_cfg.pack(bref));
  }
  if (as_context_present) {
    HANDLE_CODE(as_context.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(source_cfg_present, 1));
  HANDLE_CODE(bref.unpack(rrm_cfg_present, 1));
  HANDLE_CODE(bref.unpack(as_context_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(ue_cap_rat_list, bref, 0, 8));
  if (source_cfg_present) {
    HANDLE_CODE(source_cfg.unpack(bref));
  }
  if (rrm_cfg_present) {
    HANDLE_CODE(rrm_cfg.unpack(bref));
  }
  if (as_context_present) {
    HANDLE_CODE(as_context.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ue-CapabilityRAT-List");
  for (const auto& e1 : ue_cap_rat_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (source_cfg_present) {
    j.write_fieldname("sourceConfig");
    source_cfg.to_json(j);
  }
  if (rrm_cfg_present) {
    j.write_fieldname("rrm-Config");
    rrm_cfg.to_json(j);
  }
  if (as_context_present) {
    j.write_fieldname("as-Context");
    as_context.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// HandoverPreparationInformation ::= SEQUENCE
SRSASN_CODE ho_prep_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_prep_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_prep_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_s::crit_exts_c_::c1_c_& ho_prep_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_prep_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ho_prep_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_prep_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_ies_s& ho_prep_info_s::crit_exts_c_::c1_c_::set_ho_prep_info()
{
  set(types::ho_prep_info);
  return c;
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_prep_info:
      j.write_fieldname("handoverPreparationInformation");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_prep_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_prep_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"handoverPreparationInformation", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ho_prep_info_s::crit_exts_c_::c1_c_::types");
}

const char* ho_prep_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ho_prep_info_s::crit_exts_c_::types");
}
uint8_t ho_prep_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ho_prep_info_s::crit_exts_c_::types");
}

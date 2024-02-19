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

#include "srsran/asn1/rrc_nr/nr_sidelink_preconf.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SL-BWP-DiscPoolConfigCommon-r17 ::= SEQUENCE
SRSASN_CODE sl_bwp_disc_pool_cfg_common_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_disc_rx_pool_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_disc_tx_pool_sel_r17.size() > 0, 1));

  if (sl_disc_rx_pool_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_disc_rx_pool_r17, 1, 16));
  }
  if (sl_disc_tx_pool_sel_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_disc_tx_pool_sel_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_bwp_disc_pool_cfg_common_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_disc_rx_pool_r17_present;
  HANDLE_CODE(bref.unpack(sl_disc_rx_pool_r17_present, 1));
  bool sl_disc_tx_pool_sel_r17_present;
  HANDLE_CODE(bref.unpack(sl_disc_tx_pool_sel_r17_present, 1));

  if (sl_disc_rx_pool_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_disc_rx_pool_r17, bref, 1, 16));
  }
  if (sl_disc_tx_pool_sel_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_disc_tx_pool_sel_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void sl_bwp_disc_pool_cfg_common_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_disc_rx_pool_r17.size() > 0) {
    j.start_array("sl-DiscRxPool-r17");
    for (const auto& e1 : sl_disc_rx_pool_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_disc_tx_pool_sel_r17.size() > 0) {
    j.start_array("sl-DiscTxPoolSelected-r17");
    for (const auto& e1 : sl_disc_tx_pool_sel_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-BWP-PoolConfigCommon-r16 ::= SEQUENCE
SRSASN_CODE sl_bwp_pool_cfg_common_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_rx_pool_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_tx_pool_sel_normal_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_tx_pool_exceptional_r16_present, 1));

  if (sl_rx_pool_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rx_pool_r16, 1, 16));
  }
  if (sl_tx_pool_sel_normal_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_tx_pool_sel_normal_r16, 1, 8));
  }
  if (sl_tx_pool_exceptional_r16_present) {
    HANDLE_CODE(sl_tx_pool_exceptional_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_bwp_pool_cfg_common_r16_s::unpack(cbit_ref& bref)
{
  bool sl_rx_pool_r16_present;
  HANDLE_CODE(bref.unpack(sl_rx_pool_r16_present, 1));
  bool sl_tx_pool_sel_normal_r16_present;
  HANDLE_CODE(bref.unpack(sl_tx_pool_sel_normal_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_tx_pool_exceptional_r16_present, 1));

  if (sl_rx_pool_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rx_pool_r16, bref, 1, 16));
  }
  if (sl_tx_pool_sel_normal_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_tx_pool_sel_normal_r16, bref, 1, 8));
  }
  if (sl_tx_pool_exceptional_r16_present) {
    HANDLE_CODE(sl_tx_pool_exceptional_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_bwp_pool_cfg_common_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_rx_pool_r16.size() > 0) {
    j.start_array("sl-RxPool-r16");
    for (const auto& e1 : sl_rx_pool_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_tx_pool_sel_normal_r16.size() > 0) {
    j.start_array("sl-TxPoolSelectedNormal-r16");
    for (const auto& e1 : sl_tx_pool_sel_normal_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_tx_pool_exceptional_r16_present) {
    j.write_fieldname("sl-TxPoolExceptional-r16");
    sl_tx_pool_exceptional_r16.to_json(j);
  }
  j.end_obj();
}

// SL-BWP-ConfigCommon-r16 ::= SEQUENCE
SRSASN_CODE sl_bwp_cfg_common_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_bwp_generic_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_bwp_pool_cfg_common_r16_present, 1));

  if (sl_bwp_generic_r16_present) {
    HANDLE_CODE(sl_bwp_generic_r16.pack(bref));
  }
  if (sl_bwp_pool_cfg_common_r16_present) {
    HANDLE_CODE(sl_bwp_pool_cfg_common_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_bwp_pool_cfg_common_ps_r17.is_present();
    group_flags[0] |= sl_bwp_disc_pool_cfg_common_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_bwp_pool_cfg_common_ps_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_bwp_disc_pool_cfg_common_r17.is_present(), 1));
      if (sl_bwp_pool_cfg_common_ps_r17.is_present()) {
        HANDLE_CODE(sl_bwp_pool_cfg_common_ps_r17->pack(bref));
      }
      if (sl_bwp_disc_pool_cfg_common_r17.is_present()) {
        HANDLE_CODE(sl_bwp_disc_pool_cfg_common_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_bwp_cfg_common_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_bwp_generic_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_bwp_pool_cfg_common_r16_present, 1));

  if (sl_bwp_generic_r16_present) {
    HANDLE_CODE(sl_bwp_generic_r16.unpack(bref));
  }
  if (sl_bwp_pool_cfg_common_r16_present) {
    HANDLE_CODE(sl_bwp_pool_cfg_common_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_bwp_pool_cfg_common_ps_r17, bref);
      unpack_presence_flag(sl_bwp_disc_pool_cfg_common_r17, bref);
      if (sl_bwp_pool_cfg_common_ps_r17.is_present()) {
        HANDLE_CODE(sl_bwp_pool_cfg_common_ps_r17->unpack(bref));
      }
      if (sl_bwp_disc_pool_cfg_common_r17.is_present()) {
        HANDLE_CODE(sl_bwp_disc_pool_cfg_common_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_bwp_cfg_common_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_bwp_generic_r16_present) {
    j.write_fieldname("sl-BWP-Generic-r16");
    sl_bwp_generic_r16.to_json(j);
  }
  if (sl_bwp_pool_cfg_common_r16_present) {
    j.write_fieldname("sl-BWP-PoolConfigCommon-r16");
    sl_bwp_pool_cfg_common_r16.to_json(j);
  }
  if (ext) {
    if (sl_bwp_pool_cfg_common_ps_r17.is_present()) {
      j.write_fieldname("sl-BWP-PoolConfigCommonPS-r17");
      sl_bwp_pool_cfg_common_ps_r17->to_json(j);
    }
    if (sl_bwp_disc_pool_cfg_common_r17.is_present()) {
      j.write_fieldname("sl-BWP-DiscPoolConfigCommon-r17");
      sl_bwp_disc_pool_cfg_common_r17->to_json(j);
    }
  }
  j.end_obj();
}

// SL-FreqConfigCommon-r16 ::= SEQUENCE
SRSASN_CODE sl_freq_cfg_common_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_absolute_freq_ssb_r16_present, 1));
  HANDLE_CODE(bref.pack(freq_shift7p5khz_sl_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_bwp_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_sync_prio_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_nb_as_sync_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_sync_cfg_list_r16.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sl_scs_specific_carrier_list_r16, 1, 5));
  HANDLE_CODE(pack_integer(bref, sl_absolute_freq_point_a_r16, (uint32_t)0u, (uint32_t)3279165u));
  if (sl_absolute_freq_ssb_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_absolute_freq_ssb_r16, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(pack_integer(bref, value_n_r16, (int8_t)-1, (int8_t)1));
  if (sl_bwp_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_bwp_list_r16, 1, 4));
  }
  if (sl_sync_prio_r16_present) {
    HANDLE_CODE(sl_sync_prio_r16.pack(bref));
  }
  if (sl_nb_as_sync_r16_present) {
    HANDLE_CODE(bref.pack(sl_nb_as_sync_r16, 1));
  }
  if (sl_sync_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_sync_cfg_list_r16, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_freq_cfg_common_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_absolute_freq_ssb_r16_present, 1));
  HANDLE_CODE(bref.unpack(freq_shift7p5khz_sl_r16_present, 1));
  bool sl_bwp_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_bwp_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_sync_prio_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_nb_as_sync_r16_present, 1));
  bool sl_sync_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_sync_cfg_list_r16_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sl_scs_specific_carrier_list_r16, bref, 1, 5));
  HANDLE_CODE(unpack_integer(sl_absolute_freq_point_a_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (sl_absolute_freq_ssb_r16_present) {
    HANDLE_CODE(unpack_integer(sl_absolute_freq_ssb_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(unpack_integer(value_n_r16, bref, (int8_t)-1, (int8_t)1));
  if (sl_bwp_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_bwp_list_r16, bref, 1, 4));
  }
  if (sl_sync_prio_r16_present) {
    HANDLE_CODE(sl_sync_prio_r16.unpack(bref));
  }
  if (sl_nb_as_sync_r16_present) {
    HANDLE_CODE(bref.unpack(sl_nb_as_sync_r16, 1));
  }
  if (sl_sync_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_sync_cfg_list_r16, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void sl_freq_cfg_common_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sl-SCS-SpecificCarrierList-r16");
  for (const auto& e1 : sl_scs_specific_carrier_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("sl-AbsoluteFrequencyPointA-r16", sl_absolute_freq_point_a_r16);
  if (sl_absolute_freq_ssb_r16_present) {
    j.write_int("sl-AbsoluteFrequencySSB-r16", sl_absolute_freq_ssb_r16);
  }
  if (freq_shift7p5khz_sl_r16_present) {
    j.write_str("frequencyShift7p5khzSL-r16", "true");
  }
  j.write_int("valueN-r16", value_n_r16);
  if (sl_bwp_list_r16.size() > 0) {
    j.start_array("sl-BWP-List-r16");
    for (const auto& e1 : sl_bwp_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_sync_prio_r16_present) {
    j.write_str("sl-SyncPriority-r16", sl_sync_prio_r16.to_string());
  }
  if (sl_nb_as_sync_r16_present) {
    j.write_bool("sl-NbAsSync-r16", sl_nb_as_sync_r16);
  }
  if (sl_sync_cfg_list_r16.size() > 0) {
    j.start_array("sl-SyncConfigList-r16");
    for (const auto& e1 : sl_sync_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* sl_freq_cfg_common_r16_s::sl_sync_prio_r16_opts::to_string() const
{
  static const char* names[] = {"gnss", "gnbEnb"};
  return convert_enum_idx(names, 2, value, "sl_freq_cfg_common_r16_s::sl_sync_prio_r16_e_");
}

// SL-MeasConfigCommon-r16 ::= SEQUENCE
SRSASN_CODE sl_meas_cfg_common_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_meas_obj_list_common_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_report_cfg_list_common_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_id_list_common_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_quant_cfg_common_r16_present, 1));

  if (sl_meas_obj_list_common_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_obj_list_common_r16, 1, 64));
  }
  if (sl_report_cfg_list_common_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_report_cfg_list_common_r16, 1, 64));
  }
  if (sl_meas_id_list_common_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_meas_id_list_common_r16, 1, 64));
  }
  if (sl_quant_cfg_common_r16_present) {
    HANDLE_CODE(sl_quant_cfg_common_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_meas_cfg_common_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_meas_obj_list_common_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_obj_list_common_r16_present, 1));
  bool sl_report_cfg_list_common_r16_present;
  HANDLE_CODE(bref.unpack(sl_report_cfg_list_common_r16_present, 1));
  bool sl_meas_id_list_common_r16_present;
  HANDLE_CODE(bref.unpack(sl_meas_id_list_common_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_quant_cfg_common_r16_present, 1));

  if (sl_meas_obj_list_common_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_obj_list_common_r16, bref, 1, 64));
  }
  if (sl_report_cfg_list_common_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_report_cfg_list_common_r16, bref, 1, 64));
  }
  if (sl_meas_id_list_common_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_meas_id_list_common_r16, bref, 1, 64));
  }
  if (sl_quant_cfg_common_r16_present) {
    HANDLE_CODE(sl_quant_cfg_common_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_meas_cfg_common_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_meas_obj_list_common_r16.size() > 0) {
    j.start_array("sl-MeasObjectListCommon-r16");
    for (const auto& e1 : sl_meas_obj_list_common_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_report_cfg_list_common_r16.size() > 0) {
    j.start_array("sl-ReportConfigListCommon-r16");
    for (const auto& e1 : sl_report_cfg_list_common_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_meas_id_list_common_r16.size() > 0) {
    j.start_array("sl-MeasIdListCommon-r16");
    for (const auto& e1 : sl_meas_id_list_common_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_quant_cfg_common_r16_present) {
    j.write_fieldname("sl-QuantityConfigCommon-r16");
    sl_quant_cfg_common_r16.to_json(j);
  }
  j.end_obj();
}

// SL-PreconfigGeneral-r16 ::= SEQUENCE
SRSASN_CODE sl_precfg_general_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_tdd_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(reserved_bits_r16_present, 1));

  if (sl_tdd_cfg_r16_present) {
    HANDLE_CODE(sl_tdd_cfg_r16.pack(bref));
  }
  if (reserved_bits_r16_present) {
    HANDLE_CODE(reserved_bits_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_general_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_tdd_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(reserved_bits_r16_present, 1));

  if (sl_tdd_cfg_r16_present) {
    HANDLE_CODE(sl_tdd_cfg_r16.unpack(bref));
  }
  if (reserved_bits_r16_present) {
    HANDLE_CODE(reserved_bits_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_precfg_general_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_tdd_cfg_r16_present) {
    j.write_fieldname("sl-TDD-Configuration-r16");
    sl_tdd_cfg_r16.to_json(j);
  }
  if (reserved_bits_r16_present) {
    j.write_str("reservedBits-r16", reserved_bits_r16.to_string());
  }
  j.end_obj();
}

// SL-TxProfile-r17 ::= ENUMERATED
const char* sl_tx_profile_r17_opts::to_string() const
{
  static const char* names[] = {
      "drx-Compatible", "drx-Incompatible", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sl_tx_profile_r17_e");
}

// SL-RoHC-Profiles-r16 ::= SEQUENCE
SRSASN_CODE sl_ro_hc_profiles_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(profile0x0001_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0002_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0003_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0004_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0006_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0101_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0102_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0103_r16, 1));
  HANDLE_CODE(bref.pack(profile0x0104_r16, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_ro_hc_profiles_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(profile0x0001_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0002_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0003_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0004_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0006_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0101_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0102_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0103_r16, 1));
  HANDLE_CODE(bref.unpack(profile0x0104_r16, 1));

  return SRSASN_SUCCESS;
}
void sl_ro_hc_profiles_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("profile0x0001-r16", profile0x0001_r16);
  j.write_bool("profile0x0002-r16", profile0x0002_r16);
  j.write_bool("profile0x0003-r16", profile0x0003_r16);
  j.write_bool("profile0x0004-r16", profile0x0004_r16);
  j.write_bool("profile0x0006-r16", profile0x0006_r16);
  j.write_bool("profile0x0101-r16", profile0x0101_r16);
  j.write_bool("profile0x0102-r16", profile0x0102_r16);
  j.write_bool("profile0x0103-r16", profile0x0103_r16);
  j.write_bool("profile0x0104-r16", profile0x0104_r16);
  j.end_obj();
}

// SidelinkPreconfigNR-r16 ::= SEQUENCE
SRSASN_CODE sidelink_precfg_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_precfg_freq_info_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_precfg_nr_anchor_carrier_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_precfg_eutra_anchor_carrier_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_radio_bearer_pre_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rlc_bearer_pre_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_pre_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_offset_dfn_r16_present, 1));
  HANDLE_CODE(bref.pack(t400_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ssb_prio_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_precfg_general_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ue_sel_pre_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ro_hc_profiles_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_max_c_id_r16_present, 1));

  if (sl_precfg_freq_info_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_precfg_freq_info_list_r16, 1, 8));
  }
  if (sl_precfg_nr_anchor_carrier_freq_list_r16.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, sl_precfg_nr_anchor_carrier_freq_list_r16, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sl_precfg_eutra_anchor_carrier_freq_list_r16.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, sl_precfg_eutra_anchor_carrier_freq_list_r16, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (sl_radio_bearer_pre_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_radio_bearer_pre_cfg_list_r16, 1, 512));
  }
  if (sl_rlc_bearer_pre_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_bearer_pre_cfg_list_r16, 1, 512));
  }
  if (sl_meas_pre_cfg_r16_present) {
    HANDLE_CODE(sl_meas_pre_cfg_r16.pack(bref));
  }
  if (sl_offset_dfn_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_offset_dfn_r16, (uint16_t)1u, (uint16_t)1000u));
  }
  if (t400_r16_present) {
    HANDLE_CODE(t400_r16.pack(bref));
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    HANDLE_CODE(sl_max_num_consecutive_dtx_r16.pack(bref));
  }
  if (sl_ssb_prio_nr_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_ssb_prio_nr_r16, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_precfg_general_r16_present) {
    HANDLE_CODE(sl_precfg_general_r16.pack(bref));
  }
  if (sl_ue_sel_pre_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_pre_cfg_r16.pack(bref));
  }
  if (sl_ro_hc_profiles_r16_present) {
    HANDLE_CODE(sl_ro_hc_profiles_r16.pack(bref));
  }
  if (sl_max_c_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_max_c_id_r16, (uint16_t)1u, (uint16_t)16383u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_drx_pre_cfg_gc_bc_r17.is_present();
    group_flags[0] |= sl_tx_profile_list_r17.is_present();
    group_flags[0] |= sl_precfg_disc_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_drx_pre_cfg_gc_bc_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_tx_profile_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_precfg_disc_cfg_r17.is_present(), 1));
      if (sl_drx_pre_cfg_gc_bc_r17.is_present()) {
        HANDLE_CODE(sl_drx_pre_cfg_gc_bc_r17->pack(bref));
      }
      if (sl_tx_profile_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sl_tx_profile_list_r17, 1, 256));
      }
      if (sl_precfg_disc_cfg_r17.is_present()) {
        HANDLE_CODE(sl_precfg_disc_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sidelink_precfg_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_precfg_freq_info_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_precfg_freq_info_list_r16_present, 1));
  bool sl_precfg_nr_anchor_carrier_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_precfg_nr_anchor_carrier_freq_list_r16_present, 1));
  bool sl_precfg_eutra_anchor_carrier_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_precfg_eutra_anchor_carrier_freq_list_r16_present, 1));
  bool sl_radio_bearer_pre_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_radio_bearer_pre_cfg_list_r16_present, 1));
  bool sl_rlc_bearer_pre_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_rlc_bearer_pre_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_meas_pre_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_offset_dfn_r16_present, 1));
  HANDLE_CODE(bref.unpack(t400_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ssb_prio_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_precfg_general_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ue_sel_pre_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ro_hc_profiles_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_c_id_r16_present, 1));

  if (sl_precfg_freq_info_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_precfg_freq_info_list_r16, bref, 1, 8));
  }
  if (sl_precfg_nr_anchor_carrier_freq_list_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_precfg_nr_anchor_carrier_freq_list_r16, bref, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sl_precfg_eutra_anchor_carrier_freq_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(
        sl_precfg_eutra_anchor_carrier_freq_list_r16, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (sl_radio_bearer_pre_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_radio_bearer_pre_cfg_list_r16, bref, 1, 512));
  }
  if (sl_rlc_bearer_pre_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_bearer_pre_cfg_list_r16, bref, 1, 512));
  }
  if (sl_meas_pre_cfg_r16_present) {
    HANDLE_CODE(sl_meas_pre_cfg_r16.unpack(bref));
  }
  if (sl_offset_dfn_r16_present) {
    HANDLE_CODE(unpack_integer(sl_offset_dfn_r16, bref, (uint16_t)1u, (uint16_t)1000u));
  }
  if (t400_r16_present) {
    HANDLE_CODE(t400_r16.unpack(bref));
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    HANDLE_CODE(sl_max_num_consecutive_dtx_r16.unpack(bref));
  }
  if (sl_ssb_prio_nr_r16_present) {
    HANDLE_CODE(unpack_integer(sl_ssb_prio_nr_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (sl_precfg_general_r16_present) {
    HANDLE_CODE(sl_precfg_general_r16.unpack(bref));
  }
  if (sl_ue_sel_pre_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_pre_cfg_r16.unpack(bref));
  }
  if (sl_ro_hc_profiles_r16_present) {
    HANDLE_CODE(sl_ro_hc_profiles_r16.unpack(bref));
  }
  if (sl_max_c_id_r16_present) {
    HANDLE_CODE(unpack_integer(sl_max_c_id_r16, bref, (uint16_t)1u, (uint16_t)16383u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_drx_pre_cfg_gc_bc_r17, bref);
      unpack_presence_flag(sl_tx_profile_list_r17, bref);
      unpack_presence_flag(sl_precfg_disc_cfg_r17, bref);
      if (sl_drx_pre_cfg_gc_bc_r17.is_present()) {
        HANDLE_CODE(sl_drx_pre_cfg_gc_bc_r17->unpack(bref));
      }
      if (sl_tx_profile_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sl_tx_profile_list_r17, bref, 1, 256));
      }
      if (sl_precfg_disc_cfg_r17.is_present()) {
        HANDLE_CODE(sl_precfg_disc_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sidelink_precfg_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_precfg_freq_info_list_r16.size() > 0) {
    j.start_array("sl-PreconfigFreqInfoList-r16");
    for (const auto& e1 : sl_precfg_freq_info_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_precfg_nr_anchor_carrier_freq_list_r16.size() > 0) {
    j.start_array("sl-PreconfigNR-AnchorCarrierFreqList-r16");
    for (const auto& e1 : sl_precfg_nr_anchor_carrier_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_precfg_eutra_anchor_carrier_freq_list_r16.size() > 0) {
    j.start_array("sl-PreconfigEUTRA-AnchorCarrierFreqList-r16");
    for (const auto& e1 : sl_precfg_eutra_anchor_carrier_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_radio_bearer_pre_cfg_list_r16.size() > 0) {
    j.start_array("sl-RadioBearerPreConfigList-r16");
    for (const auto& e1 : sl_radio_bearer_pre_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_rlc_bearer_pre_cfg_list_r16.size() > 0) {
    j.start_array("sl-RLC-BearerPreConfigList-r16");
    for (const auto& e1 : sl_rlc_bearer_pre_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_meas_pre_cfg_r16_present) {
    j.write_fieldname("sl-MeasPreConfig-r16");
    sl_meas_pre_cfg_r16.to_json(j);
  }
  if (sl_offset_dfn_r16_present) {
    j.write_int("sl-OffsetDFN-r16", sl_offset_dfn_r16);
  }
  if (t400_r16_present) {
    j.write_str("t400-r16", t400_r16.to_string());
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    j.write_str("sl-MaxNumConsecutiveDTX-r16", sl_max_num_consecutive_dtx_r16.to_string());
  }
  if (sl_ssb_prio_nr_r16_present) {
    j.write_int("sl-SSB-PriorityNR-r16", sl_ssb_prio_nr_r16);
  }
  if (sl_precfg_general_r16_present) {
    j.write_fieldname("sl-PreconfigGeneral-r16");
    sl_precfg_general_r16.to_json(j);
  }
  if (sl_ue_sel_pre_cfg_r16_present) {
    j.write_fieldname("sl-UE-SelectedPreConfig-r16");
    sl_ue_sel_pre_cfg_r16.to_json(j);
  }
  if (sl_csi_acquisition_r16_present) {
    j.write_str("sl-CSI-Acquisition-r16", "enabled");
  }
  if (sl_ro_hc_profiles_r16_present) {
    j.write_fieldname("sl-RoHC-Profiles-r16");
    sl_ro_hc_profiles_r16.to_json(j);
  }
  if (sl_max_c_id_r16_present) {
    j.write_int("sl-MaxCID-r16", sl_max_c_id_r16);
  }
  if (ext) {
    if (sl_drx_pre_cfg_gc_bc_r17.is_present()) {
      j.write_fieldname("sl-DRX-PreConfigGC-BC-r17");
      sl_drx_pre_cfg_gc_bc_r17->to_json(j);
    }
    if (sl_tx_profile_list_r17.is_present()) {
      j.start_array("sl-TxProfileList-r17");
      for (const auto& e1 : *sl_tx_profile_list_r17) {
        j.write_str(e1.to_string());
      }
      j.end_array();
    }
    if (sl_precfg_disc_cfg_r17.is_present()) {
      j.write_fieldname("sl-PreconfigDiscConfig-r17");
      sl_precfg_disc_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* sidelink_precfg_nr_r16_s::t400_r16_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "sidelink_precfg_nr_r16_s::t400_r16_e_");
}
uint16_t sidelink_precfg_nr_r16_s::t400_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "sidelink_precfg_nr_r16_s::t400_r16_e_");
}

const char* sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n16", "n32"};
  return convert_enum_idx(names, 8, value, "sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}
uint8_t sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sidelink_precfg_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}

// SL-PreconfigurationNR-r16 ::= SEQUENCE
SRSASN_CODE sl_precfg_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sidelink_precfg_nr_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sidelink_precfg_nr_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_precfg_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sidelinkPreconfigNR-r16");
  sidelink_precfg_nr_r16.to_json(j);
  j.end_obj();
}

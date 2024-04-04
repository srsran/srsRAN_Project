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

#include "srsran/asn1/rrc_nr/meas_timing_cfg.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MeasurementTimingConfiguration-v1610-IEs ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_subcarrier_spacing_r16.pack(bref));
  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_cell_mob_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, csi_rs_cfg_r16.ref_ssb_freq_r16, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_subcarrier_spacing_r16.unpack(bref));
  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_cell_mob_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(csi_rs_cfg_r16.ref_ssb_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("csi-RS-Config-r16");
  j.start_obj();
  j.write_str("csi-RS-SubcarrierSpacing-r16", csi_rs_cfg_r16.csi_rs_subcarrier_spacing_r16.to_string());
  j.write_fieldname("csi-RS-CellMobility-r16");
  csi_rs_cfg_r16.csi_rs_cell_mob_r16.to_json(j);
  j.write_int("refSSBFreq-r16", csi_rs_cfg_r16.ref_ssb_freq_r16);
  j.end_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MeasTiming ::= SEQUENCE
SRSASN_CODE meas_timing_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_and_timing_present, 1));

  if (freq_and_timing_present) {
    HANDLE_CODE(bref.pack(freq_and_timing.ss_rssi_meas_present, 1));
    HANDLE_CODE(pack_integer(bref, freq_and_timing.carrier_freq, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(freq_and_timing.ssb_subcarrier_spacing.pack(bref));
    HANDLE_CODE(freq_and_timing.ssb_meas_timing_cfg.pack(bref));
    if (freq_and_timing.ss_rssi_meas_present) {
      HANDLE_CODE(freq_and_timing.ss_rssi_meas.pack(bref));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ssb_to_measure.is_present();
    group_flags[0] |= pci_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ssb_to_measure.is_present(), 1));
      HANDLE_CODE(bref.pack(pci_present, 1));
      if (ssb_to_measure.is_present()) {
        HANDLE_CODE(ssb_to_measure->pack(bref));
      }
      if (pci_present) {
        HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_and_timing_present, 1));

  if (freq_and_timing_present) {
    HANDLE_CODE(bref.unpack(freq_and_timing.ss_rssi_meas_present, 1));
    HANDLE_CODE(unpack_integer(freq_and_timing.carrier_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(freq_and_timing.ssb_subcarrier_spacing.unpack(bref));
    HANDLE_CODE(freq_and_timing.ssb_meas_timing_cfg.unpack(bref));
    if (freq_and_timing.ss_rssi_meas_present) {
      HANDLE_CODE(freq_and_timing.ss_rssi_meas.unpack(bref));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ssb_to_measure, bref);
      HANDLE_CODE(bref.unpack(pci_present, 1));
      if (ssb_to_measure.is_present()) {
        HANDLE_CODE(ssb_to_measure->unpack(bref));
      }
      if (pci_present) {
        HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_timing_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_and_timing_present) {
    j.write_fieldname("frequencyAndTiming");
    j.start_obj();
    j.write_int("carrierFreq", freq_and_timing.carrier_freq);
    j.write_str("ssbSubcarrierSpacing", freq_and_timing.ssb_subcarrier_spacing.to_string());
    j.write_fieldname("ssb-MeasurementTimingConfiguration");
    freq_and_timing.ssb_meas_timing_cfg.to_json(j);
    if (freq_and_timing.ss_rssi_meas_present) {
      j.write_fieldname("ss-RSSI-Measurement");
      freq_and_timing.ss_rssi_meas.to_json(j);
    }
    j.end_obj();
  }
  if (ext) {
    if (ssb_to_measure.is_present()) {
      j.write_fieldname("ssb-ToMeasure");
      ssb_to_measure->to_json(j);
    }
    if (pci_present) {
      j.write_int("physCellId", pci);
    }
  }
  j.end_obj();
}

// MeasurementTimingConfiguration-v1550-IEs ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_v1550_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.pack(camp_on_first_ssb, 1));
  HANDLE_CODE(bref.pack(ps_cell_only_on_first_ssb, 1));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_v1550_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(camp_on_first_ssb, 1));
  HANDLE_CODE(bref.unpack(ps_cell_only_on_first_ssb, 1));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_v1550_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("campOnFirstSSB", camp_on_first_ssb);
  j.write_bool("psCellOnlyOnFirstSSB", ps_cell_only_on_first_ssb);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MeasurementTimingConfiguration-IEs ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_timing.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_timing.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_timing, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_ies_s::unpack(cbit_ref& bref)
{
  bool meas_timing_present;
  HANDLE_CODE(bref.unpack(meas_timing_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_timing_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_timing, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_timing.size() > 0) {
    j.start_array("measTiming");
    for (const auto& e1 : meas_timing) {
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

// MeasurementTimingConfiguration ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void meas_timing_cfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_timing_cfg_s::crit_exts_c_::c1_c_& meas_timing_cfg_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void meas_timing_cfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void meas_timing_cfg_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void meas_timing_cfg_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
meas_timing_cfg_ies_s& meas_timing_cfg_s::crit_exts_c_::c1_c_::set_meas_timing_conf()
{
  set(types::meas_timing_conf);
  return c;
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_timing_conf:
      j.write_fieldname("measTimingConf");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_timing_conf:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_timing_conf:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_timing_cfg_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"measTimingConf", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "meas_timing_cfg_s::crit_exts_c_::c1_c_::types");
}

const char* meas_timing_cfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "meas_timing_cfg_s::crit_exts_c_::types");
}
uint8_t meas_timing_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "meas_timing_cfg_s::crit_exts_c_::types");
}

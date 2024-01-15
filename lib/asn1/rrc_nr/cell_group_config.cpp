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

#include "srsran/asn1/rrc_nr/cell_group_config.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// TA-Info-r17 ::= SEQUENCE
SRSASN_CODE ta_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ta_common_drift_r17_present, 1));
  HANDLE_CODE(bref.pack(ta_common_drift_variant_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, ta_common_r17, (uint32_t)0u, (uint32_t)66485757u));
  if (ta_common_drift_r17_present) {
    HANDLE_CODE(pack_integer(bref, ta_common_drift_r17, (int32_t)-257303, (int32_t)257303));
  }
  if (ta_common_drift_variant_r17_present) {
    HANDLE_CODE(pack_integer(bref, ta_common_drift_variant_r17, (uint16_t)0u, (uint16_t)28949u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ta_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ta_common_drift_r17_present, 1));
  HANDLE_CODE(bref.unpack(ta_common_drift_variant_r17_present, 1));

  HANDLE_CODE(unpack_integer(ta_common_r17, bref, (uint32_t)0u, (uint32_t)66485757u));
  if (ta_common_drift_r17_present) {
    HANDLE_CODE(unpack_integer(ta_common_drift_r17, bref, (int32_t)-257303, (int32_t)257303));
  }
  if (ta_common_drift_variant_r17_present) {
    HANDLE_CODE(unpack_integer(ta_common_drift_variant_r17, bref, (uint16_t)0u, (uint16_t)28949u));
  }

  return SRSASN_SUCCESS;
}
void ta_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ta-Common-r17", ta_common_r17);
  if (ta_common_drift_r17_present) {
    j.write_int("ta-CommonDrift-r17", ta_common_drift_r17);
  }
  if (ta_common_drift_variant_r17_present) {
    j.write_int("ta-CommonDriftVariant-r17", ta_common_drift_variant_r17);
  }
  j.end_obj();
}

// NTN-Config-r17 ::= SEQUENCE
SRSASN_CODE ntn_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(epoch_time_r17_present, 1));
  HANDLE_CODE(bref.pack(ntn_ul_sync_validity_dur_r17_present, 1));
  HANDLE_CODE(bref.pack(cell_specific_koffset_r17_present, 1));
  HANDLE_CODE(bref.pack(kmac_r17_present, 1));
  HANDLE_CODE(bref.pack(ta_info_r17_present, 1));
  HANDLE_CODE(bref.pack(ntn_polarization_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(ntn_polarization_ul_r17_present, 1));
  HANDLE_CODE(bref.pack(ephemeris_info_r17_present, 1));
  HANDLE_CODE(bref.pack(ta_report_r17_present, 1));

  if (epoch_time_r17_present) {
    HANDLE_CODE(epoch_time_r17.pack(bref));
  }
  if (ntn_ul_sync_validity_dur_r17_present) {
    HANDLE_CODE(ntn_ul_sync_validity_dur_r17.pack(bref));
  }
  if (cell_specific_koffset_r17_present) {
    HANDLE_CODE(pack_integer(bref, cell_specific_koffset_r17, (uint16_t)1u, (uint16_t)1023u));
  }
  if (kmac_r17_present) {
    HANDLE_CODE(pack_integer(bref, kmac_r17, (uint16_t)1u, (uint16_t)512u));
  }
  if (ta_info_r17_present) {
    HANDLE_CODE(ta_info_r17.pack(bref));
  }
  if (ntn_polarization_dl_r17_present) {
    HANDLE_CODE(ntn_polarization_dl_r17.pack(bref));
  }
  if (ntn_polarization_ul_r17_present) {
    HANDLE_CODE(ntn_polarization_ul_r17.pack(bref));
  }
  if (ephemeris_info_r17_present) {
    HANDLE_CODE(ephemeris_info_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ntn_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(epoch_time_r17_present, 1));
  HANDLE_CODE(bref.unpack(ntn_ul_sync_validity_dur_r17_present, 1));
  HANDLE_CODE(bref.unpack(cell_specific_koffset_r17_present, 1));
  HANDLE_CODE(bref.unpack(kmac_r17_present, 1));
  HANDLE_CODE(bref.unpack(ta_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(ntn_polarization_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(ntn_polarization_ul_r17_present, 1));
  HANDLE_CODE(bref.unpack(ephemeris_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(ta_report_r17_present, 1));

  if (epoch_time_r17_present) {
    HANDLE_CODE(epoch_time_r17.unpack(bref));
  }
  if (ntn_ul_sync_validity_dur_r17_present) {
    HANDLE_CODE(ntn_ul_sync_validity_dur_r17.unpack(bref));
  }
  if (cell_specific_koffset_r17_present) {
    HANDLE_CODE(unpack_integer(cell_specific_koffset_r17, bref, (uint16_t)1u, (uint16_t)1023u));
  }
  if (kmac_r17_present) {
    HANDLE_CODE(unpack_integer(kmac_r17, bref, (uint16_t)1u, (uint16_t)512u));
  }
  if (ta_info_r17_present) {
    HANDLE_CODE(ta_info_r17.unpack(bref));
  }
  if (ntn_polarization_dl_r17_present) {
    HANDLE_CODE(ntn_polarization_dl_r17.unpack(bref));
  }
  if (ntn_polarization_ul_r17_present) {
    HANDLE_CODE(ntn_polarization_ul_r17.unpack(bref));
  }
  if (ephemeris_info_r17_present) {
    HANDLE_CODE(ephemeris_info_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ntn_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (epoch_time_r17_present) {
    j.write_fieldname("epochTime-r17");
    epoch_time_r17.to_json(j);
  }
  if (ntn_ul_sync_validity_dur_r17_present) {
    j.write_str("ntn-UlSyncValidityDuration-r17", ntn_ul_sync_validity_dur_r17.to_string());
  }
  if (cell_specific_koffset_r17_present) {
    j.write_int("cellSpecificKoffset-r17", cell_specific_koffset_r17);
  }
  if (kmac_r17_present) {
    j.write_int("kmac-r17", kmac_r17);
  }
  if (ta_info_r17_present) {
    j.write_fieldname("ta-Info-r17");
    ta_info_r17.to_json(j);
  }
  if (ntn_polarization_dl_r17_present) {
    j.write_str("ntn-PolarizationDL-r17", ntn_polarization_dl_r17.to_string());
  }
  if (ntn_polarization_ul_r17_present) {
    j.write_str("ntn-PolarizationUL-r17", ntn_polarization_ul_r17.to_string());
  }
  if (ephemeris_info_r17_present) {
    j.write_fieldname("ephemerisInfo-r17");
    ephemeris_info_r17.to_json(j);
  }
  if (ta_report_r17_present) {
    j.write_str("ta-Report-r17", "enabled");
  }
  j.end_obj();
}

const char* ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::to_string() const
{
  static const char* names[] = {"s5",
                                "s10",
                                "s15",
                                "s20",
                                "s25",
                                "s30",
                                "s35",
                                "s40",
                                "s45",
                                "s50",
                                "s55",
                                "s60",
                                "s120",
                                "s180",
                                "s240",
                                "s900"};
  return convert_enum_idx(names, 16, value, "ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_e_");
}
uint16_t ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900};
  return map_enum_number(numbers, 16, value, "ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_e_");
}

const char* ntn_cfg_r17_s::ntn_polarization_dl_r17_opts::to_string() const
{
  static const char* names[] = {"rhcp", "lhcp", "linear"};
  return convert_enum_idx(names, 3, value, "ntn_cfg_r17_s::ntn_polarization_dl_r17_e_");
}

const char* ntn_cfg_r17_s::ntn_polarization_ul_r17_opts::to_string() const
{
  static const char* names[] = {"rhcp", "lhcp", "linear"};
  return convert_enum_idx(names, 3, value, "ntn_cfg_r17_s::ntn_polarization_ul_r17_e_");
}

// T-Reassembly ::= ENUMERATED
const char* t_reassembly_opts::to_string() const
{
  static const char* names[] = {"ms0",   "ms5",   "ms10",  "ms15",  "ms20",  "ms25",  "ms30",  "ms35",
                                "ms40",  "ms45",  "ms50",  "ms55",  "ms60",  "ms65",  "ms70",  "ms75",
                                "ms80",  "ms85",  "ms90",  "ms95",  "ms100", "ms110", "ms120", "ms130",
                                "ms140", "ms150", "ms160", "ms170", "ms180", "ms190", "ms200", "spare1"};
  return convert_enum_idx(names, 32, value, "t_reassembly_e");
}
uint8_t t_reassembly_opts::to_number() const
{
  static const uint8_t numbers[] = {0,  5,  10, 15, 20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70, 75,
                                    80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
  return map_enum_number(numbers, 31, value, "t_reassembly_e");
}

// DRX-ConfigPTM-r17 ::= SEQUENCE
SRSASN_CODE drx_cfg_ptm_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_harq_rtt_timer_dl_ptm_r17_present, 1));
  HANDLE_CODE(bref.pack(drx_retx_timer_dl_ptm_r17_present, 1));

  HANDLE_CODE(drx_on_dur_timer_ptm_r17.pack(bref));
  HANDLE_CODE(drx_inactivity_timer_ptm_r17.pack(bref));
  if (drx_harq_rtt_timer_dl_ptm_r17_present) {
    HANDLE_CODE(pack_integer(bref, drx_harq_rtt_timer_dl_ptm_r17, (uint8_t)0u, (uint8_t)56u));
  }
  if (drx_retx_timer_dl_ptm_r17_present) {
    HANDLE_CODE(drx_retx_timer_dl_ptm_r17.pack(bref));
  }
  HANDLE_CODE(drx_long_cycle_start_offset_ptm_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, drx_slot_offset_ptm_r17, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_ptm_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_harq_rtt_timer_dl_ptm_r17_present, 1));
  HANDLE_CODE(bref.unpack(drx_retx_timer_dl_ptm_r17_present, 1));

  HANDLE_CODE(drx_on_dur_timer_ptm_r17.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer_ptm_r17.unpack(bref));
  if (drx_harq_rtt_timer_dl_ptm_r17_present) {
    HANDLE_CODE(unpack_integer(drx_harq_rtt_timer_dl_ptm_r17, bref, (uint8_t)0u, (uint8_t)56u));
  }
  if (drx_retx_timer_dl_ptm_r17_present) {
    HANDLE_CODE(drx_retx_timer_dl_ptm_r17.unpack(bref));
  }
  HANDLE_CODE(drx_long_cycle_start_offset_ptm_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(drx_slot_offset_ptm_r17, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void drx_cfg_ptm_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-onDurationTimerPTM-r17");
  drx_on_dur_timer_ptm_r17.to_json(j);
  j.write_str("drx-InactivityTimerPTM-r17", drx_inactivity_timer_ptm_r17.to_string());
  if (drx_harq_rtt_timer_dl_ptm_r17_present) {
    j.write_int("drx-HARQ-RTT-TimerDL-PTM-r17", drx_harq_rtt_timer_dl_ptm_r17);
  }
  if (drx_retx_timer_dl_ptm_r17_present) {
    j.write_str("drx-RetransmissionTimerDL-PTM-r17", drx_retx_timer_dl_ptm_r17.to_string());
  }
  j.write_fieldname("drx-LongCycleStartOffsetPTM-r17");
  drx_long_cycle_start_offset_ptm_r17.to_json(j);
  j.write_int("drx-SlotOffsetPTM-r17", drx_slot_offset_ptm_r17);
  j.end_obj();
}

void drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::destroy_() {}
void drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::drx_on_dur_timer_ptm_r17_c_(
    const drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_& other)
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_");
  }
}
drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_&
drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::operator=(const drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_& other)
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_");
  }

  return *this;
}
uint8_t& drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_&
drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_");
}
uint16_t drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 24, value, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_");
}

const char* drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_opts::to_string() const
{
  static const char* names[] = {"ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",
                                "ms10",   "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",
                                "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_e_");
}
uint16_t drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_e_");
}

const char* drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_opts::to_string() const
{
  static const char* names[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_e_");
}
uint16_t drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_e_");
}

void drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::destroy_() {}
void drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::drx_long_cycle_start_offset_ptm_r17_c_(
    const drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_& other)
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_");
  }
}
drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_&
drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::operator=(
    const drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_& other)
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_");
  }

  return *this;
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
void drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// BH-LogicalChannelIdentity-r16 ::= CHOICE
void bh_lc_ch_id_r16_c::destroy_() {}
void bh_lc_ch_id_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
bh_lc_ch_id_r16_c::bh_lc_ch_id_r16_c(const bh_lc_ch_id_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::bh_lc_ch_id_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::bh_lc_ch_id_ext_r16:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bh_lc_ch_id_r16_c");
  }
}
bh_lc_ch_id_r16_c& bh_lc_ch_id_r16_c::operator=(const bh_lc_ch_id_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::bh_lc_ch_id_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::bh_lc_ch_id_ext_r16:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bh_lc_ch_id_r16_c");
  }

  return *this;
}
uint8_t& bh_lc_ch_id_r16_c::set_bh_lc_ch_id_r16()
{
  set(types::bh_lc_ch_id_r16);
  return c.get<uint8_t>();
}
uint32_t& bh_lc_ch_id_r16_c::set_bh_lc_ch_id_ext_r16()
{
  set(types::bh_lc_ch_id_ext_r16);
  return c.get<uint32_t>();
}
void bh_lc_ch_id_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::bh_lc_ch_id_r16:
      j.write_int("bh-LogicalChannelIdentity-r16", c.get<uint8_t>());
      break;
    case types::bh_lc_ch_id_ext_r16:
      j.write_int("bh-LogicalChannelIdentityExt-r16", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "bh_lc_ch_id_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE bh_lc_ch_id_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::bh_lc_ch_id_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)32u));
      break;
    case types::bh_lc_ch_id_ext_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)320u, (uint32_t)65855u));
      break;
    default:
      log_invalid_choice_id(type_, "bh_lc_ch_id_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_lc_ch_id_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::bh_lc_ch_id_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)32u));
      break;
    case types::bh_lc_ch_id_ext_r16:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)320u, (uint32_t)65855u));
      break;
    default:
      log_invalid_choice_id(type_, "bh_lc_ch_id_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// T-StatusProhibit ::= ENUMERATED
const char* t_status_prohibit_opts::to_string() const
{
  static const char* names[] = {
      "ms0",   "ms5",   "ms10",   "ms15",   "ms20",   "ms25",   "ms30",   "ms35",   "ms40",  "ms45",  "ms50",
      "ms55",  "ms60",  "ms65",   "ms70",   "ms75",   "ms80",   "ms85",   "ms90",   "ms95",  "ms100", "ms105",
      "ms110", "ms115", "ms120",  "ms125",  "ms130",  "ms135",  "ms140",  "ms145",  "ms150", "ms155", "ms160",
      "ms165", "ms170", "ms175",  "ms180",  "ms185",  "ms190",  "ms195",  "ms200",  "ms205", "ms210", "ms215",
      "ms220", "ms225", "ms230",  "ms235",  "ms240",  "ms245",  "ms250",  "ms300",  "ms350", "ms400", "ms450",
      "ms500", "ms800", "ms1000", "ms1200", "ms1600", "ms2000", "ms2400", "spare2", "spare1"};
  return convert_enum_idx(names, 64, value, "t_status_prohibit_e");
}
uint16_t t_status_prohibit_opts::to_number() const
{
  static const uint16_t numbers[] = {0,   5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,   65,  70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140,  145, 150, 155,
                                     160, 165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220,  225, 230, 235,
                                     240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 1200, 1600, 2000, 2400};
  return map_enum_number(numbers, 62, value, "t_status_prohibit_e");
}

// DL-AM-RLC ::= SEQUENCE
SRSASN_CODE dl_am_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.pack(bref));
  }
  HANDLE_CODE(t_reassembly.pack(bref));
  HANDLE_CODE(t_status_prohibit.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_am_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.unpack(bref));
  }
  HANDLE_CODE(t_reassembly.unpack(bref));
  HANDLE_CODE(t_status_prohibit.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_am_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sn_field_len_present) {
    j.write_str("sn-FieldLength", sn_field_len.to_string());
  }
  j.write_str("t-Reassembly", t_reassembly.to_string());
  j.write_str("t-StatusProhibit", t_status_prohibit.to_string());
  j.end_obj();
}

// DL-UM-RLC ::= SEQUENCE
SRSASN_CODE dl_um_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.pack(bref));
  }
  HANDLE_CODE(t_reassembly.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_um_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.unpack(bref));
  }
  HANDLE_CODE(t_reassembly.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_um_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sn_field_len_present) {
    j.write_str("sn-FieldLength", sn_field_len.to_string());
  }
  j.write_str("t-Reassembly", t_reassembly.to_string());
  j.end_obj();
}

// UL-AM-RLC ::= SEQUENCE
SRSASN_CODE ul_am_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.pack(bref));
  }
  HANDLE_CODE(t_poll_retx.pack(bref));
  HANDLE_CODE(poll_pdu.pack(bref));
  HANDLE_CODE(poll_byte.pack(bref));
  HANDLE_CODE(max_retx_thres.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_am_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.unpack(bref));
  }
  HANDLE_CODE(t_poll_retx.unpack(bref));
  HANDLE_CODE(poll_pdu.unpack(bref));
  HANDLE_CODE(poll_byte.unpack(bref));
  HANDLE_CODE(max_retx_thres.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_am_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sn_field_len_present) {
    j.write_str("sn-FieldLength", sn_field_len.to_string());
  }
  j.write_str("t-PollRetransmit", t_poll_retx.to_string());
  j.write_str("pollPDU", poll_pdu.to_string());
  j.write_str("pollByte", poll_byte.to_string());
  j.write_str("maxRetxThreshold", max_retx_thres.to_string());
  j.end_obj();
}

const char* ul_am_rlc_s::max_retx_thres_opts::to_string() const
{
  static const char* names[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(names, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}
uint8_t ul_am_rlc_s::max_retx_thres_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}

// UL-UM-RLC ::= SEQUENCE
SRSASN_CODE ul_um_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_um_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sn_field_len_present, 1));

  if (sn_field_len_present) {
    HANDLE_CODE(sn_field_len.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_um_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sn_field_len_present) {
    j.write_str("sn-FieldLength", sn_field_len.to_string());
  }
  j.end_obj();
}

// LogicalChannelConfig ::= SEQUENCE
SRSASN_CODE lc_ch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_specific_params_present, 1));

  if (ul_specific_params_present) {
    HANDLE_CODE(ul_specific_params.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ch_access_prio_r16_present;
    group_flags[0] |= bit_rate_multiplier_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ch_access_prio_r16_present, 1));
      HANDLE_CODE(bref.pack(bit_rate_multiplier_r16_present, 1));
      if (ch_access_prio_r16_present) {
        HANDLE_CODE(pack_integer(bref, ch_access_prio_r16, (uint8_t)1u, (uint8_t)4u));
      }
      if (bit_rate_multiplier_r16_present) {
        HANDLE_CODE(bit_rate_multiplier_r16.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lc_ch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_specific_params_present, 1));

  if (ul_specific_params_present) {
    HANDLE_CODE(ul_specific_params.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ch_access_prio_r16_present, 1));
      HANDLE_CODE(bref.unpack(bit_rate_multiplier_r16_present, 1));
      if (ch_access_prio_r16_present) {
        HANDLE_CODE(unpack_integer(ch_access_prio_r16, bref, (uint8_t)1u, (uint8_t)4u));
      }
      if (bit_rate_multiplier_r16_present) {
        HANDLE_CODE(bit_rate_multiplier_r16.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void lc_ch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_specific_params_present) {
    j.write_fieldname("ul-SpecificParameters");
    ul_specific_params.to_json(j);
  }
  if (ext) {
    if (ch_access_prio_r16_present) {
      j.write_int("channelAccessPriority-r16", ch_access_prio_r16);
    }
    if (bit_rate_multiplier_r16_present) {
      j.write_str("bitRateMultiplier-r16", bit_rate_multiplier_r16.to_string());
    }
  }
  j.end_obj();
}

SRSASN_CODE lc_ch_cfg_s::ul_specific_params_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(allowed_serving_cells.size() > 0, 1));
  HANDLE_CODE(bref.pack(allowed_scs_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(max_pusch_dur_present, 1));
  HANDLE_CODE(bref.pack(cfg_grant_type1_allowed_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_group_present, 1));
  HANDLE_CODE(bref.pack(sched_request_id_present, 1));

  HANDLE_CODE(pack_integer(bref, prio, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(prioritised_bit_rate.pack(bref));
  HANDLE_CODE(bucket_size_dur.pack(bref));
  if (allowed_serving_cells.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_serving_cells, 1, 31, integer_packer<uint8_t>(0, 31)));
  }
  if (allowed_scs_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_scs_list, 1, 5));
  }
  if (max_pusch_dur_present) {
    HANDLE_CODE(max_pusch_dur.pack(bref));
  }
  if (lc_ch_group_present) {
    HANDLE_CODE(pack_integer(bref, lc_ch_group, (uint8_t)0u, (uint8_t)7u));
  }
  if (sched_request_id_present) {
    HANDLE_CODE(pack_integer(bref, sched_request_id, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(bref.pack(lc_ch_sr_mask, 1));
  HANDLE_CODE(bref.pack(lc_ch_sr_delay_timer_applied, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= bit_rate_query_prohibit_timer_present;
    group_flags[1] |= allowed_cg_list_r16.is_present();
    group_flags[1] |= allowed_phy_prio_idx_r16_present;
    group_flags[2] |= lc_ch_group_iab_ext_r17_present;
    group_flags[2] |= allowed_harq_mode_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(bit_rate_query_prohibit_timer_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(allowed_cg_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(allowed_phy_prio_idx_r16_present, 1));
      if (allowed_cg_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *allowed_cg_list_r16, 0, 31, integer_packer<uint8_t>(0, 31)));
      }
      if (allowed_phy_prio_idx_r16_present) {
        HANDLE_CODE(allowed_phy_prio_idx_r16.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(lc_ch_group_iab_ext_r17_present, 1));
      HANDLE_CODE(bref.pack(allowed_harq_mode_r17_present, 1));
      if (lc_ch_group_iab_ext_r17_present) {
        HANDLE_CODE(pack_integer(bref, lc_ch_group_iab_ext_r17, (uint16_t)0u, (uint16_t)255u));
      }
      if (allowed_harq_mode_r17_present) {
        HANDLE_CODE(allowed_harq_mode_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lc_ch_cfg_s::ul_specific_params_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool allowed_serving_cells_present;
  HANDLE_CODE(bref.unpack(allowed_serving_cells_present, 1));
  bool allowed_scs_list_present;
  HANDLE_CODE(bref.unpack(allowed_scs_list_present, 1));
  HANDLE_CODE(bref.unpack(max_pusch_dur_present, 1));
  HANDLE_CODE(bref.unpack(cfg_grant_type1_allowed_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_group_present, 1));
  HANDLE_CODE(bref.unpack(sched_request_id_present, 1));

  HANDLE_CODE(unpack_integer(prio, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(prioritised_bit_rate.unpack(bref));
  HANDLE_CODE(bucket_size_dur.unpack(bref));
  if (allowed_serving_cells_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_serving_cells, bref, 1, 31, integer_packer<uint8_t>(0, 31)));
  }
  if (allowed_scs_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_scs_list, bref, 1, 5));
  }
  if (max_pusch_dur_present) {
    HANDLE_CODE(max_pusch_dur.unpack(bref));
  }
  if (lc_ch_group_present) {
    HANDLE_CODE(unpack_integer(lc_ch_group, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (sched_request_id_present) {
    HANDLE_CODE(unpack_integer(sched_request_id, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(bref.unpack(lc_ch_sr_mask, 1));
  HANDLE_CODE(bref.unpack(lc_ch_sr_delay_timer_applied, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(bit_rate_query_prohibit_timer_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(allowed_cg_list_r16, bref);
      HANDLE_CODE(bref.unpack(allowed_phy_prio_idx_r16_present, 1));
      if (allowed_cg_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*allowed_cg_list_r16, bref, 0, 31, integer_packer<uint8_t>(0, 31)));
      }
      if (allowed_phy_prio_idx_r16_present) {
        HANDLE_CODE(allowed_phy_prio_idx_r16.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(lc_ch_group_iab_ext_r17_present, 1));
      HANDLE_CODE(bref.unpack(allowed_harq_mode_r17_present, 1));
      if (lc_ch_group_iab_ext_r17_present) {
        HANDLE_CODE(unpack_integer(lc_ch_group_iab_ext_r17, bref, (uint16_t)0u, (uint16_t)255u));
      }
      if (allowed_harq_mode_r17_present) {
        HANDLE_CODE(allowed_harq_mode_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void lc_ch_cfg_s::ul_specific_params_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("priority", prio);
  j.write_str("prioritisedBitRate", prioritised_bit_rate.to_string());
  j.write_str("bucketSizeDuration", bucket_size_dur.to_string());
  if (allowed_serving_cells.size() > 0) {
    j.start_array("allowedServingCells");
    for (const auto& e1 : allowed_serving_cells) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (allowed_scs_list.size() > 0) {
    j.start_array("allowedSCS-List");
    for (const auto& e1 : allowed_scs_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (max_pusch_dur_present) {
    j.write_str("maxPUSCH-Duration", max_pusch_dur.to_string());
  }
  if (cfg_grant_type1_allowed_present) {
    j.write_str("configuredGrantType1Allowed", "true");
  }
  if (lc_ch_group_present) {
    j.write_int("logicalChannelGroup", lc_ch_group);
  }
  if (sched_request_id_present) {
    j.write_int("schedulingRequestID", sched_request_id);
  }
  j.write_bool("logicalChannelSR-Mask", lc_ch_sr_mask);
  j.write_bool("logicalChannelSR-DelayTimerApplied", lc_ch_sr_delay_timer_applied);
  if (ext) {
    if (bit_rate_query_prohibit_timer_present) {
      j.write_str("bitRateQueryProhibitTimer", bit_rate_query_prohibit_timer.to_string());
    }
    if (allowed_cg_list_r16.is_present()) {
      j.start_array("allowedCG-List-r16");
      for (const auto& e1 : *allowed_cg_list_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (allowed_phy_prio_idx_r16_present) {
      j.write_str("allowedPHY-PriorityIndex-r16", allowed_phy_prio_idx_r16.to_string());
    }
    if (lc_ch_group_iab_ext_r17_present) {
      j.write_int("logicalChannelGroupIAB-Ext-r17", lc_ch_group_iab_ext_r17);
    }
    if (allowed_harq_mode_r17_present) {
      j.write_str("allowedHARQ-mode-r17", allowed_harq_mode_r17.to_string());
    }
  }
  j.end_obj();
}

const char* lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_string() const
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
  return convert_enum_idx(names, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}
int32_t lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_number() const
{
  static const int32_t numbers[] = {0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, -1};
  return map_enum_number(numbers, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_string() const
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
  return convert_enum_idx(names, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}
uint16_t lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 50, 100, 150, 300, 500, 1000};
  return map_enum_number(numbers, 9, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur_opts::to_string() const
{
  static const char* names[] = {"ms0p02", "ms0p04", "ms0p0625", "ms0p125", "ms0p25", "ms0p5", "ms0p01-v1700", "spare1"};
  return convert_enum_idx(names, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_string() const
{
  static const char* names[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(names, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
}
float lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(numbers, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
}
const char* lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(
      number_strs, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_opts::to_string() const
{
  static const char* names[] = {"p0", "p1"};
  return convert_enum_idx(names, 2, value, "lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_e_");
}
uint8_t lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17_opts::to_string() const
{
  static const char* names[] = {"harqModeA", "harqModeB"};
  return convert_enum_idx(names, 2, value, "lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17_e_");
}

const char* lc_ch_cfg_s::bit_rate_multiplier_r16_opts::to_string() const
{
  static const char* names[] = {"x40", "x70", "x100", "x200"};
  return convert_enum_idx(names, 4, value, "lc_ch_cfg_s::bit_rate_multiplier_r16_e_");
}
uint8_t lc_ch_cfg_s::bit_rate_multiplier_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {40, 70, 100, 200};
  return map_enum_number(numbers, 4, value, "lc_ch_cfg_s::bit_rate_multiplier_r16_e_");
}

// RLC-Config ::= CHOICE
void rlc_cfg_c::destroy_()
{
  switch (type_) {
    case types::am:
      c.destroy<am_s_>();
      break;
    case types::um_bi_dir:
      c.destroy<um_bi_dir_s_>();
      break;
    case types::um_uni_dir_ul:
      c.destroy<um_uni_dir_ul_s_>();
      break;
    case types::um_uni_dir_dl:
      c.destroy<um_uni_dir_dl_s_>();
      break;
    default:
      break;
  }
}
void rlc_cfg_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::am:
      c.init<am_s_>();
      break;
    case types::um_bi_dir:
      c.init<um_bi_dir_s_>();
      break;
    case types::um_uni_dir_ul:
      c.init<um_uni_dir_ul_s_>();
      break;
    case types::um_uni_dir_dl:
      c.init<um_uni_dir_dl_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }
}
rlc_cfg_c::rlc_cfg_c(const rlc_cfg_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::am:
      c.init(other.c.get<am_s_>());
      break;
    case types::um_bi_dir:
      c.init(other.c.get<um_bi_dir_s_>());
      break;
    case types::um_uni_dir_ul:
      c.init(other.c.get<um_uni_dir_ul_s_>());
      break;
    case types::um_uni_dir_dl:
      c.init(other.c.get<um_uni_dir_dl_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }
}
rlc_cfg_c& rlc_cfg_c::operator=(const rlc_cfg_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::am:
      c.set(other.c.get<am_s_>());
      break;
    case types::um_bi_dir:
      c.set(other.c.get<um_bi_dir_s_>());
      break;
    case types::um_uni_dir_ul:
      c.set(other.c.get<um_uni_dir_ul_s_>());
      break;
    case types::um_uni_dir_dl:
      c.set(other.c.get<um_uni_dir_dl_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }

  return *this;
}
rlc_cfg_c::am_s_& rlc_cfg_c::set_am()
{
  set(types::am);
  return c.get<am_s_>();
}
rlc_cfg_c::um_bi_dir_s_& rlc_cfg_c::set_um_bi_dir()
{
  set(types::um_bi_dir);
  return c.get<um_bi_dir_s_>();
}
rlc_cfg_c::um_uni_dir_ul_s_& rlc_cfg_c::set_um_uni_dir_ul()
{
  set(types::um_uni_dir_ul);
  return c.get<um_uni_dir_ul_s_>();
}
rlc_cfg_c::um_uni_dir_dl_s_& rlc_cfg_c::set_um_uni_dir_dl()
{
  set(types::um_uni_dir_dl);
  return c.get<um_uni_dir_dl_s_>();
}
void rlc_cfg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::am:
      j.write_fieldname("am");
      j.start_obj();
      j.write_fieldname("ul-AM-RLC");
      c.get<am_s_>().ul_am_rlc.to_json(j);
      j.write_fieldname("dl-AM-RLC");
      c.get<am_s_>().dl_am_rlc.to_json(j);
      j.end_obj();
      break;
    case types::um_bi_dir:
      j.write_fieldname("um-Bi-Directional");
      j.start_obj();
      j.write_fieldname("ul-UM-RLC");
      c.get<um_bi_dir_s_>().ul_um_rlc.to_json(j);
      j.write_fieldname("dl-UM-RLC");
      c.get<um_bi_dir_s_>().dl_um_rlc.to_json(j);
      j.end_obj();
      break;
    case types::um_uni_dir_ul:
      j.write_fieldname("um-Uni-Directional-UL");
      j.start_obj();
      j.write_fieldname("ul-UM-RLC");
      c.get<um_uni_dir_ul_s_>().ul_um_rlc.to_json(j);
      j.end_obj();
      break;
    case types::um_uni_dir_dl:
      j.write_fieldname("um-Uni-Directional-DL");
      j.start_obj();
      j.write_fieldname("dl-UM-RLC");
      c.get<um_uni_dir_dl_s_>().dl_um_rlc.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }
  j.end_obj();
}
SRSASN_CODE rlc_cfg_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::am:
      HANDLE_CODE(c.get<am_s_>().ul_am_rlc.pack(bref));
      HANDLE_CODE(c.get<am_s_>().dl_am_rlc.pack(bref));
      break;
    case types::um_bi_dir:
      HANDLE_CODE(c.get<um_bi_dir_s_>().ul_um_rlc.pack(bref));
      HANDLE_CODE(c.get<um_bi_dir_s_>().dl_um_rlc.pack(bref));
      break;
    case types::um_uni_dir_ul:
      HANDLE_CODE(c.get<um_uni_dir_ul_s_>().ul_um_rlc.pack(bref));
      break;
    case types::um_uni_dir_dl:
      HANDLE_CODE(c.get<um_uni_dir_dl_s_>().dl_um_rlc.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::am:
      HANDLE_CODE(c.get<am_s_>().ul_am_rlc.unpack(bref));
      HANDLE_CODE(c.get<am_s_>().dl_am_rlc.unpack(bref));
      break;
    case types::um_bi_dir:
      HANDLE_CODE(c.get<um_bi_dir_s_>().ul_um_rlc.unpack(bref));
      HANDLE_CODE(c.get<um_bi_dir_s_>().dl_um_rlc.unpack(bref));
      break;
    case types::um_uni_dir_ul:
      HANDLE_CODE(c.get<um_uni_dir_ul_s_>().ul_um_rlc.unpack(bref));
      break;
    case types::um_uni_dir_dl:
      HANDLE_CODE(c.get<um_uni_dir_dl_s_>().dl_um_rlc.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// BH-RLC-ChannelConfig-r16 ::= SEQUENCE
SRSASN_CODE bh_rlc_ch_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(bh_lc_ch_id_r16_present, 1));
  HANDLE_CODE(bref.pack(reestablish_rlc_r16_present, 1));
  HANDLE_CODE(bref.pack(rlc_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(mac_lc_ch_cfg_r16_present, 1));

  if (bh_lc_ch_id_r16_present) {
    HANDLE_CODE(bh_lc_ch_id_r16.pack(bref));
  }
  HANDLE_CODE(bh_rlc_ch_id_r16.pack(bref));
  if (rlc_cfg_r16_present) {
    HANDLE_CODE(rlc_cfg_r16.pack(bref));
  }
  if (mac_lc_ch_cfg_r16_present) {
    HANDLE_CODE(mac_lc_ch_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_rlc_ch_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(bh_lc_ch_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(reestablish_rlc_r16_present, 1));
  HANDLE_CODE(bref.unpack(rlc_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(mac_lc_ch_cfg_r16_present, 1));

  if (bh_lc_ch_id_r16_present) {
    HANDLE_CODE(bh_lc_ch_id_r16.unpack(bref));
  }
  HANDLE_CODE(bh_rlc_ch_id_r16.unpack(bref));
  if (rlc_cfg_r16_present) {
    HANDLE_CODE(rlc_cfg_r16.unpack(bref));
  }
  if (mac_lc_ch_cfg_r16_present) {
    HANDLE_CODE(mac_lc_ch_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bh_rlc_ch_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bh_lc_ch_id_r16_present) {
    j.write_fieldname("bh-LogicalChannelIdentity-r16");
    bh_lc_ch_id_r16.to_json(j);
  }
  j.write_str("bh-RLC-ChannelID-r16", bh_rlc_ch_id_r16.to_string());
  if (reestablish_rlc_r16_present) {
    j.write_str("reestablishRLC-r16", "true");
  }
  if (rlc_cfg_r16_present) {
    j.write_fieldname("rlc-Config-r16");
    rlc_cfg_r16.to_json(j);
  }
  if (mac_lc_ch_cfg_r16_present) {
    j.write_fieldname("mac-LogicalChannelConfig-r16");
    mac_lc_ch_cfg_r16.to_json(j);
  }
  j.end_obj();
}

// CarrierState-r17 ::= CHOICE
void carrier_state_r17_c::set(types::options e)
{
  type_ = e;
}
void carrier_state_r17_c::set_de_activ_r17()
{
  set(types::de_activ_r17);
}
uint8_t& carrier_state_r17_c::set_active_bwp_r17()
{
  set(types::active_bwp_r17);
  return c;
}
void carrier_state_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::de_activ_r17:
      break;
    case types::active_bwp_r17:
      j.write_int("activeBWP-r17", c);
      break;
    default:
      log_invalid_choice_id(type_, "carrier_state_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE carrier_state_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::de_activ_r17:
      break;
    case types::active_bwp_r17:
      HANDLE_CODE(pack_integer(bref, c, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "carrier_state_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_state_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::de_activ_r17:
      break;
    case types::active_bwp_r17:
      HANDLE_CODE(unpack_integer(c, bref, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "carrier_state_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CC-State-r17 ::= SEQUENCE
SRSASN_CODE cc_state_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_carrier_r17_present, 1));
  HANDLE_CODE(bref.pack(ul_carrier_r17_present, 1));

  if (dl_carrier_r17_present) {
    HANDLE_CODE(dl_carrier_r17.pack(bref));
  }
  if (ul_carrier_r17_present) {
    HANDLE_CODE(ul_carrier_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cc_state_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_carrier_r17_present, 1));
  HANDLE_CODE(bref.unpack(ul_carrier_r17_present, 1));

  if (dl_carrier_r17_present) {
    HANDLE_CODE(dl_carrier_r17.unpack(bref));
  }
  if (ul_carrier_r17_present) {
    HANDLE_CODE(ul_carrier_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cc_state_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_carrier_r17_present) {
    j.write_fieldname("dlCarrier-r17");
    dl_carrier_r17.to_json(j);
  }
  if (ul_carrier_r17_present) {
    j.write_fieldname("ulCarrier-r17");
    ul_carrier_r17.to_json(j);
  }
  j.end_obj();
}

// CFRA-CSIRS-Resource ::= SEQUENCE
SRSASN_CODE cfra_csi_rs_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_rs, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(pack_dyn_seq_of(bref, ra_occasion_list, 1, 64, integer_packer<uint16_t>(0, 511)));
  HANDLE_CODE(pack_integer(bref, ra_preamb_idx, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfra_csi_rs_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_rs, bref, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(unpack_dyn_seq_of(ra_occasion_list, bref, 1, 64, integer_packer<uint16_t>(0, 511)));
  HANDLE_CODE(unpack_integer(ra_preamb_idx, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void cfra_csi_rs_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS", csi_rs);
  j.start_array("ra-OccasionList");
  for (const auto& e1 : ra_occasion_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_int("ra-PreambleIndex", ra_preamb_idx);
  j.end_obj();
}

// CFRA-SSB-Resource ::= SEQUENCE
SRSASN_CODE cfra_ssb_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ssb, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, ra_preamb_idx, (uint8_t)0u, (uint8_t)63u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= msg_a_pusch_res_idx_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(msg_a_pusch_res_idx_r16_present, 1));
      if (msg_a_pusch_res_idx_r16_present) {
        HANDLE_CODE(pack_integer(bref, msg_a_pusch_res_idx_r16, (uint16_t)0u, (uint16_t)3071u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfra_ssb_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ssb, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(ra_preamb_idx, bref, (uint8_t)0u, (uint8_t)63u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(msg_a_pusch_res_idx_r16_present, 1));
      if (msg_a_pusch_res_idx_r16_present) {
        HANDLE_CODE(unpack_integer(msg_a_pusch_res_idx_r16, bref, (uint16_t)0u, (uint16_t)3071u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cfra_ssb_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb", ssb);
  j.write_int("ra-PreambleIndex", ra_preamb_idx);
  if (ext) {
    if (msg_a_pusch_res_idx_r16_present) {
      j.write_int("msgA-PUSCH-Resource-Index-r16", msg_a_pusch_res_idx_r16);
    }
  }
  j.end_obj();
}

// CFRA ::= SEQUENCE
SRSASN_CODE cfra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(occasions_present, 1));

  if (occasions_present) {
    HANDLE_CODE(bref.pack(occasions.ssb_per_rach_occasion_present, 1));
    HANDLE_CODE(occasions.rach_cfg_generic.pack(bref));
    if (occasions.ssb_per_rach_occasion_present) {
      HANDLE_CODE(occasions.ssb_per_rach_occasion.pack(bref));
    }
  }
  HANDLE_CODE(res.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= total_nof_ra_preambs_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(total_nof_ra_preambs_present, 1));
      if (total_nof_ra_preambs_present) {
        HANDLE_CODE(pack_integer(bref, total_nof_ra_preambs, (uint8_t)1u, (uint8_t)63u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(occasions_present, 1));

  if (occasions_present) {
    HANDLE_CODE(bref.unpack(occasions.ssb_per_rach_occasion_present, 1));
    HANDLE_CODE(occasions.rach_cfg_generic.unpack(bref));
    if (occasions.ssb_per_rach_occasion_present) {
      HANDLE_CODE(occasions.ssb_per_rach_occasion.unpack(bref));
    }
  }
  HANDLE_CODE(res.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(total_nof_ra_preambs_present, 1));
      if (total_nof_ra_preambs_present) {
        HANDLE_CODE(unpack_integer(total_nof_ra_preambs, bref, (uint8_t)1u, (uint8_t)63u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cfra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (occasions_present) {
    j.write_fieldname("occasions");
    j.start_obj();
    j.write_fieldname("rach-ConfigGeneric");
    occasions.rach_cfg_generic.to_json(j);
    if (occasions.ssb_per_rach_occasion_present) {
      j.write_str("ssb-perRACH-Occasion", occasions.ssb_per_rach_occasion.to_string());
    }
    j.end_obj();
  }
  j.write_fieldname("resources");
  res.to_json(j);
  if (ext) {
    if (total_nof_ra_preambs_present) {
      j.write_int("totalNumberOfRA-Preambles", total_nof_ra_preambs);
    }
  }
  j.end_obj();
}

const char* cfra_s::occasions_s_::ssb_per_rach_occasion_opts::to_string() const
{
  static const char* names[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(names, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
}
float cfra_s::occasions_s_::ssb_per_rach_occasion_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(numbers, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
}
const char* cfra_s::occasions_s_::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(number_strs, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
}

void cfra_s::res_c_::destroy_()
{
  switch (type_) {
    case types::ssb:
      c.destroy<ssb_s_>();
      break;
    case types::csirs:
      c.destroy<csirs_s_>();
      break;
    default:
      break;
  }
}
void cfra_s::res_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ssb:
      c.init<ssb_s_>();
      break;
    case types::csirs:
      c.init<csirs_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cfra_s::res_c_");
  }
}
cfra_s::res_c_::res_c_(const cfra_s::res_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb:
      c.init(other.c.get<ssb_s_>());
      break;
    case types::csirs:
      c.init(other.c.get<csirs_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cfra_s::res_c_");
  }
}
cfra_s::res_c_& cfra_s::res_c_::operator=(const cfra_s::res_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb:
      c.set(other.c.get<ssb_s_>());
      break;
    case types::csirs:
      c.set(other.c.get<csirs_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cfra_s::res_c_");
  }

  return *this;
}
cfra_s::res_c_::ssb_s_& cfra_s::res_c_::set_ssb()
{
  set(types::ssb);
  return c.get<ssb_s_>();
}
cfra_s::res_c_::csirs_s_& cfra_s::res_c_::set_csirs()
{
  set(types::csirs);
  return c.get<csirs_s_>();
}
void cfra_s::res_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb:
      j.write_fieldname("ssb");
      j.start_obj();
      j.start_array("ssb-ResourceList");
      for (const auto& e1 : c.get<ssb_s_>().ssb_res_list) {
        e1.to_json(j);
      }
      j.end_array();
      j.write_int("ra-ssb-OccasionMaskIndex", c.get<ssb_s_>().ra_ssb_occasion_mask_idx);
      j.end_obj();
      break;
    case types::csirs:
      j.write_fieldname("csirs");
      j.start_obj();
      j.start_array("csirs-ResourceList");
      for (const auto& e1 : c.get<csirs_s_>().csirs_res_list) {
        e1.to_json(j);
      }
      j.end_array();
      j.write_int("rsrp-ThresholdCSI-RS", c.get<csirs_s_>().rsrp_thres_csi_rs);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cfra_s::res_c_");
  }
  j.end_obj();
}
SRSASN_CODE cfra_s::res_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ssb_s_>().ssb_res_list, 1, 64));
      HANDLE_CODE(pack_integer(bref, c.get<ssb_s_>().ra_ssb_occasion_mask_idx, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::csirs:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<csirs_s_>().csirs_res_list, 1, 96));
      HANDLE_CODE(pack_integer(bref, c.get<csirs_s_>().rsrp_thres_csi_rs, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "cfra_s::res_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfra_s::res_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ssb_s_>().ssb_res_list, bref, 1, 64));
      HANDLE_CODE(unpack_integer(c.get<ssb_s_>().ra_ssb_occasion_mask_idx, bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::csirs:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<csirs_s_>().csirs_res_list, bref, 1, 96));
      HANDLE_CODE(unpack_integer(c.get<csirs_s_>().rsrp_thres_csi_rs, bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "cfra_s::res_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CFRA-TwoStep-r16 ::= SEQUENCE
SRSASN_CODE cfra_two_step_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(occasions_two_step_ra_r16_present, 1));
  HANDLE_CODE(bref.pack(msg_a_trans_max_r16_present, 1));

  if (occasions_two_step_ra_r16_present) {
    HANDLE_CODE(occasions_two_step_ra_r16.rach_cfg_generic_two_step_ra_r16.pack(bref));
    HANDLE_CODE(occasions_two_step_ra_r16.ssb_per_rach_occasion_two_step_ra_r16.pack(bref));
  }
  HANDLE_CODE(msg_a_cfra_pusch_r16.pack(bref));
  if (msg_a_trans_max_r16_present) {
    HANDLE_CODE(msg_a_trans_max_r16.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, res_two_step_r16.ssb_res_list, 1, 64));
  HANDLE_CODE(pack_integer(bref, res_two_step_r16.ra_ssb_occasion_mask_idx, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfra_two_step_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(occasions_two_step_ra_r16_present, 1));
  HANDLE_CODE(bref.unpack(msg_a_trans_max_r16_present, 1));

  if (occasions_two_step_ra_r16_present) {
    HANDLE_CODE(occasions_two_step_ra_r16.rach_cfg_generic_two_step_ra_r16.unpack(bref));
    HANDLE_CODE(occasions_two_step_ra_r16.ssb_per_rach_occasion_two_step_ra_r16.unpack(bref));
  }
  HANDLE_CODE(msg_a_cfra_pusch_r16.unpack(bref));
  if (msg_a_trans_max_r16_present) {
    HANDLE_CODE(msg_a_trans_max_r16.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(res_two_step_r16.ssb_res_list, bref, 1, 64));
  HANDLE_CODE(unpack_integer(res_two_step_r16.ra_ssb_occasion_mask_idx, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void cfra_two_step_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (occasions_two_step_ra_r16_present) {
    j.write_fieldname("occasionsTwoStepRA-r16");
    j.start_obj();
    j.write_fieldname("rach-ConfigGenericTwoStepRA-r16");
    occasions_two_step_ra_r16.rach_cfg_generic_two_step_ra_r16.to_json(j);
    j.write_str("ssb-PerRACH-OccasionTwoStepRA-r16",
                occasions_two_step_ra_r16.ssb_per_rach_occasion_two_step_ra_r16.to_string());
    j.end_obj();
  }
  j.write_fieldname("msgA-CFRA-PUSCH-r16");
  msg_a_cfra_pusch_r16.to_json(j);
  if (msg_a_trans_max_r16_present) {
    j.write_str("msgA-TransMax-r16", msg_a_trans_max_r16.to_string());
  }
  j.write_fieldname("resourcesTwoStep-r16");
  j.start_obj();
  j.start_array("ssb-ResourceList");
  for (const auto& e1 : res_two_step_r16.ssb_res_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("ra-ssb-OccasionMaskIndex", res_two_step_r16.ra_ssb_occasion_mask_idx);
  j.end_obj();
  j.end_obj();
}

const char*
cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_opts::to_string() const
{
  static const char* names[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(
      names, 8, value, "cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_e_");
}
float cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(
      numbers, 8, value, "cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_e_");
}
const char*
cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(
      number_strs,
      8,
      value,
      "cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_e_");
}

const char* cfra_two_step_r16_s::msg_a_trans_max_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n6", "n8", "n10", "n20", "n50", "n100", "n200"};
  return convert_enum_idx(names, 10, value, "cfra_two_step_r16_s::msg_a_trans_max_r16_e_");
}
uint8_t cfra_two_step_r16_s::msg_a_trans_max_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6, 8, 10, 20, 50, 100, 200};
  return map_enum_number(numbers, 10, value, "cfra_two_step_r16_s::msg_a_trans_max_r16_e_");
}

// FrequencyInfoDL ::= SEQUENCE
SRSASN_CODE freq_info_dl_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(absolute_freq_ssb_present, 1));

  if (absolute_freq_ssb_present) {
    HANDLE_CODE(pack_integer(bref, absolute_freq_ssb, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(pack_integer(bref, absolute_freq_point_a, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, scs_specific_carrier_list, 1, 5));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_info_dl_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(absolute_freq_ssb_present, 1));

  if (absolute_freq_ssb_present) {
    HANDLE_CODE(unpack_integer(absolute_freq_ssb, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(unpack_integer(absolute_freq_point_a, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(scs_specific_carrier_list, bref, 1, 5));

  return SRSASN_SUCCESS;
}
void freq_info_dl_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (absolute_freq_ssb_present) {
    j.write_int("absoluteFrequencySSB", absolute_freq_ssb);
  }
  j.start_array("frequencyBandList");
  for (const auto& e1 : freq_band_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_int("absoluteFrequencyPointA", absolute_freq_point_a);
  j.start_array("scs-SpecificCarrierList");
  for (const auto& e1 : scs_specific_carrier_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// FrequencyInfoUL ::= SEQUENCE
SRSASN_CODE freq_info_ul_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(absolute_freq_point_a_present, 1));
  HANDLE_CODE(bref.pack(add_spec_emission_present, 1));
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(freq_shift7p5khz_present, 1));

  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (absolute_freq_point_a_present) {
    HANDLE_CODE(pack_integer(bref, absolute_freq_point_a, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, scs_specific_carrier_list, 1, 5));
  if (add_spec_emission_present) {
    HANDLE_CODE(pack_integer(bref, add_spec_emission, (uint8_t)0u, (uint8_t)7u));
  }
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_info_ul_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  HANDLE_CODE(bref.unpack(absolute_freq_point_a_present, 1));
  HANDLE_CODE(bref.unpack(add_spec_emission_present, 1));
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(freq_shift7p5khz_present, 1));

  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (absolute_freq_point_a_present) {
    HANDLE_CODE(unpack_integer(absolute_freq_point_a, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(scs_specific_carrier_list, bref, 1, 5));
  if (add_spec_emission_present) {
    HANDLE_CODE(unpack_integer(add_spec_emission, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }

  return SRSASN_SUCCESS;
}
void freq_info_ul_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (absolute_freq_point_a_present) {
    j.write_int("absoluteFrequencyPointA", absolute_freq_point_a);
  }
  j.start_array("scs-SpecificCarrierList");
  for (const auto& e1 : scs_specific_carrier_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (add_spec_emission_present) {
    j.write_int("additionalSpectrumEmission", add_spec_emission);
  }
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  if (freq_shift7p5khz_present) {
    j.write_str("frequencyShift7p5khz", "true");
  }
  j.end_obj();
}

// DownlinkConfigCommon ::= SEQUENCE
SRSASN_CODE dl_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_info_dl_present, 1));
  HANDLE_CODE(bref.pack(init_dl_bwp_present, 1));

  if (freq_info_dl_present) {
    HANDLE_CODE(freq_info_dl.pack(bref));
  }
  if (init_dl_bwp_present) {
    HANDLE_CODE(init_dl_bwp.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= init_dl_bwp_red_cap_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(init_dl_bwp_red_cap_r17.is_present(), 1));
      if (init_dl_bwp_red_cap_r17.is_present()) {
        HANDLE_CODE(init_dl_bwp_red_cap_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_info_dl_present, 1));
  HANDLE_CODE(bref.unpack(init_dl_bwp_present, 1));

  if (freq_info_dl_present) {
    HANDLE_CODE(freq_info_dl.unpack(bref));
  }
  if (init_dl_bwp_present) {
    HANDLE_CODE(init_dl_bwp.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(init_dl_bwp_red_cap_r17, bref);
      if (init_dl_bwp_red_cap_r17.is_present()) {
        HANDLE_CODE(init_dl_bwp_red_cap_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void dl_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_info_dl_present) {
    j.write_fieldname("frequencyInfoDL");
    freq_info_dl.to_json(j);
  }
  if (init_dl_bwp_present) {
    j.write_fieldname("initialDownlinkBWP");
    init_dl_bwp.to_json(j);
  }
  if (ext) {
    if (init_dl_bwp_red_cap_r17.is_present()) {
      j.write_fieldname("initialDownlinkBWP-RedCap-r17");
      init_dl_bwp_red_cap_r17->to_json(j);
    }
  }
  j.end_obj();
}

// HighSpeedConfig-v1700 ::= SEQUENCE
SRSASN_CODE high_speed_cfg_v1700_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(high_speed_meas_ca_scell_r17_present, 1));
  HANDLE_CODE(bref.pack(high_speed_meas_inter_freq_r17_present, 1));
  HANDLE_CODE(bref.pack(high_speed_demod_ca_scell_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE high_speed_cfg_v1700_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(high_speed_meas_ca_scell_r17_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_meas_inter_freq_r17_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_demod_ca_scell_r17_present, 1));

  return SRSASN_SUCCESS;
}
void high_speed_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (high_speed_meas_ca_scell_r17_present) {
    j.write_str("highSpeedMeasCA-Scell-r17", "true");
  }
  if (high_speed_meas_inter_freq_r17_present) {
    j.write_str("highSpeedMeasInterFreq-r17", "true");
  }
  if (high_speed_demod_ca_scell_r17_present) {
    j.write_str("highSpeedDemodCA-Scell-r17", "true");
  }
  j.end_obj();
}

// T-ReassemblyExt-r17 ::= ENUMERATED
const char* t_reassembly_ext_r17_opts::to_string() const
{
  static const char* names[] = {"ms210", "ms220", "ms340", "ms350", "ms550", "ms1100", "ms1650", "ms2200"};
  return convert_enum_idx(names, 8, value, "t_reassembly_ext_r17_e");
}
uint16_t t_reassembly_ext_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {210, 220, 340, 350, 550, 1100, 1650, 2200};
  return map_enum_number(numbers, 8, value, "t_reassembly_ext_r17_e");
}

// T-StatusProhibit-v1610 ::= ENUMERATED
const char* t_status_prohibit_v1610_opts::to_string() const
{
  static const char* names[] = {"ms1", "ms2", "ms3", "ms4", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "t_status_prohibit_v1610_e");
}
uint8_t t_status_prohibit_v1610_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "t_status_prohibit_v1610_e");
}

// UplinkConfigCommon ::= SEQUENCE
SRSASN_CODE ul_cfg_common_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_info_ul_present, 1));
  HANDLE_CODE(bref.pack(init_ul_bwp_present, 1));

  if (freq_info_ul_present) {
    HANDLE_CODE(freq_info_ul.pack(bref));
  }
  if (init_ul_bwp_present) {
    HANDLE_CODE(init_ul_bwp.pack(bref));
  }
  HANDLE_CODE(dummy.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_common_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_info_ul_present, 1));
  HANDLE_CODE(bref.unpack(init_ul_bwp_present, 1));

  if (freq_info_ul_present) {
    HANDLE_CODE(freq_info_ul.unpack(bref));
  }
  if (init_ul_bwp_present) {
    HANDLE_CODE(init_ul_bwp.unpack(bref));
  }
  HANDLE_CODE(dummy.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_info_ul_present) {
    j.write_fieldname("frequencyInfoUL");
    freq_info_ul.to_json(j);
  }
  if (init_ul_bwp_present) {
    j.write_fieldname("initialUplinkBWP");
    init_ul_bwp.to_json(j);
  }
  j.write_str("dummy", dummy.to_string());
  j.end_obj();
}

// UplinkConfigCommon-v1700 ::= SEQUENCE
SRSASN_CODE ul_cfg_common_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(init_ul_bwp_red_cap_r17_present, 1));

  if (init_ul_bwp_red_cap_r17_present) {
    HANDLE_CODE(init_ul_bwp_red_cap_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_common_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(init_ul_bwp_red_cap_r17_present, 1));

  if (init_ul_bwp_red_cap_r17_present) {
    HANDLE_CODE(init_ul_bwp_red_cap_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_cfg_common_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (init_ul_bwp_red_cap_r17_present) {
    j.write_fieldname("initialUplinkBWP-RedCap-r17");
    init_ul_bwp_red_cap_r17.to_json(j);
  }
  j.end_obj();
}

// DAPS-UplinkPowerConfig-r16 ::= SEQUENCE
SRSASN_CODE daps_ul_pwr_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p_daps_source_r16, (int8_t)-30, (int8_t)33));
  HANDLE_CODE(pack_integer(bref, p_daps_target_r16, (int8_t)-30, (int8_t)33));
  HANDLE_CODE(ul_pwr_sharing_daps_mode_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE daps_ul_pwr_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p_daps_source_r16, bref, (int8_t)-30, (int8_t)33));
  HANDLE_CODE(unpack_integer(p_daps_target_r16, bref, (int8_t)-30, (int8_t)33));
  HANDLE_CODE(ul_pwr_sharing_daps_mode_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void daps_ul_pwr_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p-DAPS-Source-r16", p_daps_source_r16);
  j.write_int("p-DAPS-Target-r16", p_daps_target_r16);
  j.write_str("uplinkPowerSharingDAPS-Mode-r16", ul_pwr_sharing_daps_mode_r16.to_string());
  j.end_obj();
}

const char* daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(names, 3, value, "daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_e_");
}
uint8_t daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_e_");
}

// DL-AM-RLC-v1610 ::= SEQUENCE
SRSASN_CODE dl_am_rlc_v1610_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(t_status_prohibit_v1610_present, 1));

  if (t_status_prohibit_v1610_present) {
    HANDLE_CODE(t_status_prohibit_v1610.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_am_rlc_v1610_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(t_status_prohibit_v1610_present, 1));

  if (t_status_prohibit_v1610_present) {
    HANDLE_CODE(t_status_prohibit_v1610.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_am_rlc_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (t_status_prohibit_v1610_present) {
    j.write_str("t-StatusProhibit-v1610", t_status_prohibit_v1610.to_string());
  }
  j.end_obj();
}

// DL-AM-RLC-v1700 ::= SEQUENCE
SRSASN_CODE dl_am_rlc_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(t_reassembly_ext_r17_present, 1));

  if (t_reassembly_ext_r17_present) {
    HANDLE_CODE(t_reassembly_ext_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_am_rlc_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(t_reassembly_ext_r17_present, 1));

  if (t_reassembly_ext_r17_present) {
    HANDLE_CODE(t_reassembly_ext_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_am_rlc_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (t_reassembly_ext_r17_present) {
    j.write_str("t-ReassemblyExt-r17", t_reassembly_ext_r17.to_string());
  }
  j.end_obj();
}

// DL-UM-RLC-v1700 ::= SEQUENCE
SRSASN_CODE dl_um_rlc_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(t_reassembly_ext_r17_present, 1));

  if (t_reassembly_ext_r17_present) {
    HANDLE_CODE(t_reassembly_ext_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_um_rlc_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(t_reassembly_ext_r17_present, 1));

  if (t_reassembly_ext_r17_present) {
    HANDLE_CODE(t_reassembly_ext_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_um_rlc_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (t_reassembly_ext_r17_present) {
    j.write_str("t-ReassemblyExt-r17", t_reassembly_ext_r17.to_string());
  }
  j.end_obj();
}

// MPE-Config-FR2-r16 ::= SEQUENCE
SRSASN_CODE mpe_cfg_fr2_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mpe_prohibit_timer_r16.pack(bref));
  HANDLE_CODE(mpe_thres_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mpe_cfg_fr2_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mpe_prohibit_timer_r16.unpack(bref));
  HANDLE_CODE(mpe_thres_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void mpe_cfg_fr2_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mpe-ProhibitTimer-r16", mpe_prohibit_timer_r16.to_string());
  j.write_str("mpe-Threshold-r16", mpe_thres_r16.to_string());
  j.end_obj();
}

const char* mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
  return convert_enum_idx(names, 8, value, "mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_e_");
}
uint16_t mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(numbers, 8, value, "mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_e_");
}

const char* mpe_cfg_fr2_r16_s::mpe_thres_r16_opts::to_string() const
{
  static const char* names[] = {"dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(names, 4, value, "mpe_cfg_fr2_r16_s::mpe_thres_r16_e_");
}
uint8_t mpe_cfg_fr2_r16_s::mpe_thres_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12};
  return map_enum_number(numbers, 4, value, "mpe_cfg_fr2_r16_s::mpe_thres_r16_e_");
}

// MPE-Config-FR2-r17 ::= SEQUENCE
SRSASN_CODE mpe_cfg_fr2_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(mpe_prohibit_timer_r17.pack(bref));
  HANDLE_CODE(mpe_thres_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, nof_n_r17, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mpe_cfg_fr2_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(mpe_prohibit_timer_r17.unpack(bref));
  HANDLE_CODE(mpe_thres_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(nof_n_r17, bref, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
void mpe_cfg_fr2_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mpe-ProhibitTimer-r17", mpe_prohibit_timer_r17.to_string());
  j.write_str("mpe-Threshold-r17", mpe_thres_r17.to_string());
  j.write_int("numberOfN-r17", nof_n_r17);
  j.end_obj();
}

const char* mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_opts::to_string() const
{
  static const char* names[] = {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
  return convert_enum_idx(names, 8, value, "mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_e_");
}
uint16_t mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(numbers, 8, value, "mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_e_");
}

const char* mpe_cfg_fr2_r17_s::mpe_thres_r17_opts::to_string() const
{
  static const char* names[] = {"dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(names, 4, value, "mpe_cfg_fr2_r17_s::mpe_thres_r17_e_");
}
uint8_t mpe_cfg_fr2_r17_s::mpe_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12};
  return map_enum_number(numbers, 4, value, "mpe_cfg_fr2_r17_s::mpe_thres_r17_e_");
}

const char* pdsch_harq_ack_codebook_list_r16_item_opts::to_string() const
{
  static const char* names[] = {"semiStatic", "dynamic"};
  return convert_enum_idx(names, 2, value, "pdsch_harq_ack_codebook_list_r16_item_e_");
}

// RACH-ConfigDedicated ::= SEQUENCE
SRSASN_CODE rach_cfg_ded_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cfra_present, 1));
  HANDLE_CODE(bref.pack(ra_prioritization_present, 1));

  if (cfra_present) {
    HANDLE_CODE(cfra.pack(bref));
  }
  if (ra_prioritization_present) {
    HANDLE_CODE(ra_prioritization.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ra_prioritization_two_step_r16.is_present();
    group_flags[0] |= cfra_two_step_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ra_prioritization_two_step_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(cfra_two_step_r16.is_present(), 1));
      if (ra_prioritization_two_step_r16.is_present()) {
        HANDLE_CODE(ra_prioritization_two_step_r16->pack(bref));
      }
      if (cfra_two_step_r16.is_present()) {
        HANDLE_CODE(cfra_two_step_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_ded_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cfra_present, 1));
  HANDLE_CODE(bref.unpack(ra_prioritization_present, 1));

  if (cfra_present) {
    HANDLE_CODE(cfra.unpack(bref));
  }
  if (ra_prioritization_present) {
    HANDLE_CODE(ra_prioritization.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ra_prioritization_two_step_r16, bref);
      unpack_presence_flag(cfra_two_step_r16, bref);
      if (ra_prioritization_two_step_r16.is_present()) {
        HANDLE_CODE(ra_prioritization_two_step_r16->unpack(bref));
      }
      if (cfra_two_step_r16.is_present()) {
        HANDLE_CODE(cfra_two_step_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rach_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cfra_present) {
    j.write_fieldname("cfra");
    cfra.to_json(j);
  }
  if (ra_prioritization_present) {
    j.write_fieldname("ra-Prioritization");
    ra_prioritization.to_json(j);
  }
  if (ext) {
    if (ra_prioritization_two_step_r16.is_present()) {
      j.write_fieldname("ra-PrioritizationTwoStep-r16");
      ra_prioritization_two_step_r16->to_json(j);
    }
    if (cfra_two_step_r16.is_present()) {
      j.write_fieldname("cfra-TwoStep-r16");
      cfra_two_step_r16->to_json(j);
    }
  }
  j.end_obj();
}

// SL-PathSwitchConfig-r17 ::= SEQUENCE
SRSASN_CODE sl_path_switch_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(target_relay_ue_id_r17.pack(bref));
  HANDLE_CODE(t420_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_path_switch_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(target_relay_ue_id_r17.unpack(bref));
  HANDLE_CODE(t420_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_path_switch_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("targetRelayUE-Identity-r17", target_relay_ue_id_r17.to_string());
  j.write_str("t420-r17", t420_r17.to_string());
  j.end_obj();
}

const char* sl_path_switch_cfg_r17_s::t420_r17_opts::to_string() const
{
  static const char* names[] = {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "ms10000"};
  return convert_enum_idx(names, 8, value, "sl_path_switch_cfg_r17_s::t420_r17_e_");
}
uint16_t sl_path_switch_cfg_r17_s::t420_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return map_enum_number(numbers, 8, value, "sl_path_switch_cfg_r17_s::t420_r17_e_");
}

// SchedulingRequestToAddMod ::= SEQUENCE
SRSASN_CODE sched_request_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sr_prohibit_timer_present, 1));

  HANDLE_CODE(pack_integer(bref, sched_request_id, (uint8_t)0u, (uint8_t)7u));
  if (sr_prohibit_timer_present) {
    HANDLE_CODE(sr_prohibit_timer.pack(bref));
  }
  HANDLE_CODE(sr_trans_max.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sr_prohibit_timer_present, 1));

  HANDLE_CODE(unpack_integer(sched_request_id, bref, (uint8_t)0u, (uint8_t)7u));
  if (sr_prohibit_timer_present) {
    HANDLE_CODE(sr_prohibit_timer.unpack(bref));
  }
  HANDLE_CODE(sr_trans_max.unpack(bref));

  return SRSASN_SUCCESS;
}
void sched_request_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("schedulingRequestId", sched_request_id);
  if (sr_prohibit_timer_present) {
    j.write_str("sr-ProhibitTimer", sr_prohibit_timer.to_string());
  }
  j.write_str("sr-TransMax", sr_trans_max.to_string());
  j.end_obj();
}

const char* sched_request_to_add_mod_s::sr_prohibit_timer_opts::to_string() const
{
  static const char* names[] = {"ms1", "ms2", "ms4", "ms8", "ms16", "ms32", "ms64", "ms128"};
  return convert_enum_idx(names, 8, value, "sched_request_to_add_mod_s::sr_prohibit_timer_e_");
}
uint8_t sched_request_to_add_mod_s::sr_prohibit_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(numbers, 8, value, "sched_request_to_add_mod_s::sr_prohibit_timer_e_");
}

const char* sched_request_to_add_mod_s::sr_trans_max_opts::to_string() const
{
  static const char* names[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sched_request_to_add_mod_s::sr_trans_max_e_");
}
uint8_t sched_request_to_add_mod_s::sr_trans_max_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16, 32, 64};
  return map_enum_number(numbers, 5, value, "sched_request_to_add_mod_s::sr_trans_max_e_");
}

// SchedulingRequestToAddModExt-v1700 ::= SEQUENCE
SRSASN_CODE sched_request_to_add_mod_ext_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sr_prohibit_timer_v1700_present, 1));

  if (sr_prohibit_timer_v1700_present) {
    HANDLE_CODE(sr_prohibit_timer_v1700.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_to_add_mod_ext_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sr_prohibit_timer_v1700_present, 1));

  if (sr_prohibit_timer_v1700_present) {
    HANDLE_CODE(sr_prohibit_timer_v1700.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sched_request_to_add_mod_ext_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sr_prohibit_timer_v1700_present) {
    j.write_str("sr-ProhibitTimer-v1700", sr_prohibit_timer_v1700.to_string());
  }
  j.end_obj();
}

const char* sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_opts::to_string() const
{
  static const char* names[] = {"ms192",
                                "ms256",
                                "ms320",
                                "ms384",
                                "ms448",
                                "ms512",
                                "ms576",
                                "ms640",
                                "ms1082",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_e_");
}
uint16_t sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_opts::to_number() const
{
  static const uint16_t numbers[] = {192, 256, 320, 384, 448, 512, 576, 640, 1082};
  return map_enum_number(numbers, 9, value, "sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_e_");
}

// ServingCellConfigCommon ::= SEQUENCE
SRSASN_CODE serving_cell_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pci_present, 1));
  HANDLE_CODE(bref.pack(dl_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(ul_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(supplementary_ul_cfg_present, 1));
  HANDLE_CODE(bref.pack(n_timing_advance_offset_present, 1));
  HANDLE_CODE(bref.pack(ssb_positions_in_burst_present, 1));
  HANDLE_CODE(bref.pack(ssb_periodicity_serving_cell_present, 1));
  HANDLE_CODE(bref.pack(lte_crs_to_match_around_present, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ssb_subcarrier_spacing_present, 1));
  HANDLE_CODE(bref.pack(tdd_ul_dl_cfg_common_present, 1));

  if (pci_present) {
    HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  }
  if (dl_cfg_common_present) {
    HANDLE_CODE(dl_cfg_common.pack(bref));
  }
  if (ul_cfg_common_present) {
    HANDLE_CODE(ul_cfg_common.pack(bref));
  }
  if (supplementary_ul_cfg_present) {
    HANDLE_CODE(supplementary_ul_cfg.pack(bref));
  }
  if (n_timing_advance_offset_present) {
    HANDLE_CODE(n_timing_advance_offset.pack(bref));
  }
  if (ssb_positions_in_burst_present) {
    HANDLE_CODE(ssb_positions_in_burst.pack(bref));
  }
  if (ssb_periodicity_serving_cell_present) {
    HANDLE_CODE(ssb_periodicity_serving_cell.pack(bref));
  }
  HANDLE_CODE(dmrs_type_a_position.pack(bref));
  if (lte_crs_to_match_around_present) {
    HANDLE_CODE(lte_crs_to_match_around.pack(bref));
  }
  if (rate_match_pattern_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_to_add_mod_list, 1, 4));
  }
  if (rate_match_pattern_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_to_release_list, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (ssb_subcarrier_spacing_present) {
    HANDLE_CODE(ssb_subcarrier_spacing.pack(bref));
  }
  if (tdd_ul_dl_cfg_common_present) {
    HANDLE_CODE(tdd_ul_dl_cfg_common.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, ss_pbch_block_pwr, (int8_t)-60, (int8_t)50));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ch_access_mode_r16.is_present();
    group_flags[0] |= discovery_burst_win_len_r16_present;
    group_flags[0] |= ssb_position_qcl_r16_present;
    group_flags[0] |= high_speed_cfg_r16.is_present();
    group_flags[1] |= high_speed_cfg_v1700.is_present();
    group_flags[1] |= ch_access_mode2_r17_present;
    group_flags[1] |= discovery_burst_win_len_r17_present;
    group_flags[1] |= ssb_position_qcl_r17_present;
    group_flags[1] |= high_speed_cfg_fr2_r17.is_present();
    group_flags[1] |= ul_cfg_common_v1700.is_present();
    group_flags[1] |= ntn_cfg_r17.is_present();
    group_flags[2] |= feature_priorities_r17.is_present();
    group_flags[3] |= ra_ch_access_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ch_access_mode_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(discovery_burst_win_len_r16_present, 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_r16_present, 1));
      HANDLE_CODE(bref.pack(high_speed_cfg_r16.is_present(), 1));
      if (ch_access_mode_r16.is_present()) {
        HANDLE_CODE(ch_access_mode_r16->pack(bref));
      }
      if (discovery_burst_win_len_r16_present) {
        HANDLE_CODE(discovery_burst_win_len_r16.pack(bref));
      }
      if (ssb_position_qcl_r16_present) {
        HANDLE_CODE(ssb_position_qcl_r16.pack(bref));
      }
      if (high_speed_cfg_r16.is_present()) {
        HANDLE_CODE(high_speed_cfg_r16->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(high_speed_cfg_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(ch_access_mode2_r17_present, 1));
      HANDLE_CODE(bref.pack(discovery_burst_win_len_r17_present, 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_r17_present, 1));
      HANDLE_CODE(bref.pack(high_speed_cfg_fr2_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_cfg_common_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(ntn_cfg_r17.is_present(), 1));
      if (high_speed_cfg_v1700.is_present()) {
        HANDLE_CODE(high_speed_cfg_v1700->pack(bref));
      }
      if (discovery_burst_win_len_r17_present) {
        HANDLE_CODE(discovery_burst_win_len_r17.pack(bref));
      }
      if (ssb_position_qcl_r17_present) {
        HANDLE_CODE(ssb_position_qcl_r17.pack(bref));
      }
      if (high_speed_cfg_fr2_r17.is_present()) {
        HANDLE_CODE(high_speed_cfg_fr2_r17->pack(bref));
      }
      if (ul_cfg_common_v1700.is_present()) {
        HANDLE_CODE(ul_cfg_common_v1700->pack(bref));
      }
      if (ntn_cfg_r17.is_present()) {
        HANDLE_CODE(ntn_cfg_r17->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(feature_priorities_r17.is_present(), 1));
      if (feature_priorities_r17.is_present()) {
        HANDLE_CODE(bref.pack(feature_priorities_r17->red_cap_prio_r17_present, 1));
        HANDLE_CODE(bref.pack(feature_priorities_r17->slicing_prio_r17_present, 1));
        HANDLE_CODE(bref.pack(feature_priorities_r17->msg3_repeats_prio_r17_present, 1));
        HANDLE_CODE(bref.pack(feature_priorities_r17->sdt_prio_r17_present, 1));
        if (feature_priorities_r17->red_cap_prio_r17_present) {
          HANDLE_CODE(pack_integer(bref, feature_priorities_r17->red_cap_prio_r17, (uint8_t)0u, (uint8_t)7u));
        }
        if (feature_priorities_r17->slicing_prio_r17_present) {
          HANDLE_CODE(pack_integer(bref, feature_priorities_r17->slicing_prio_r17, (uint8_t)0u, (uint8_t)7u));
        }
        if (feature_priorities_r17->msg3_repeats_prio_r17_present) {
          HANDLE_CODE(pack_integer(bref, feature_priorities_r17->msg3_repeats_prio_r17, (uint8_t)0u, (uint8_t)7u));
        }
        if (feature_priorities_r17->sdt_prio_r17_present) {
          HANDLE_CODE(pack_integer(bref, feature_priorities_r17->sdt_prio_r17, (uint8_t)0u, (uint8_t)7u));
        }
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ra_ch_access_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pci_present, 1));
  HANDLE_CODE(bref.unpack(dl_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(ul_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(supplementary_ul_cfg_present, 1));
  HANDLE_CODE(bref.unpack(n_timing_advance_offset_present, 1));
  HANDLE_CODE(bref.unpack(ssb_positions_in_burst_present, 1));
  HANDLE_CODE(bref.unpack(ssb_periodicity_serving_cell_present, 1));
  HANDLE_CODE(bref.unpack(lte_crs_to_match_around_present, 1));
  bool rate_match_pattern_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_to_add_mod_list_present, 1));
  bool rate_match_pattern_to_release_list_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(ssb_subcarrier_spacing_present, 1));
  HANDLE_CODE(bref.unpack(tdd_ul_dl_cfg_common_present, 1));

  if (pci_present) {
    HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  }
  if (dl_cfg_common_present) {
    HANDLE_CODE(dl_cfg_common.unpack(bref));
  }
  if (ul_cfg_common_present) {
    HANDLE_CODE(ul_cfg_common.unpack(bref));
  }
  if (supplementary_ul_cfg_present) {
    HANDLE_CODE(supplementary_ul_cfg.unpack(bref));
  }
  if (n_timing_advance_offset_present) {
    HANDLE_CODE(n_timing_advance_offset.unpack(bref));
  }
  if (ssb_positions_in_burst_present) {
    HANDLE_CODE(ssb_positions_in_burst.unpack(bref));
  }
  if (ssb_periodicity_serving_cell_present) {
    HANDLE_CODE(ssb_periodicity_serving_cell.unpack(bref));
  }
  HANDLE_CODE(dmrs_type_a_position.unpack(bref));
  if (lte_crs_to_match_around_present) {
    HANDLE_CODE(lte_crs_to_match_around.unpack(bref));
  }
  if (rate_match_pattern_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_to_add_mod_list, bref, 1, 4));
  }
  if (rate_match_pattern_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (ssb_subcarrier_spacing_present) {
    HANDLE_CODE(ssb_subcarrier_spacing.unpack(bref));
  }
  if (tdd_ul_dl_cfg_common_present) {
    HANDLE_CODE(tdd_ul_dl_cfg_common.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(ss_pbch_block_pwr, bref, (int8_t)-60, (int8_t)50));

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ch_access_mode_r16, bref);
      HANDLE_CODE(bref.unpack(discovery_burst_win_len_r16_present, 1));
      HANDLE_CODE(bref.unpack(ssb_position_qcl_r16_present, 1));
      unpack_presence_flag(high_speed_cfg_r16, bref);
      if (ch_access_mode_r16.is_present()) {
        HANDLE_CODE(ch_access_mode_r16->unpack(bref));
      }
      if (discovery_burst_win_len_r16_present) {
        HANDLE_CODE(discovery_burst_win_len_r16.unpack(bref));
      }
      if (ssb_position_qcl_r16_present) {
        HANDLE_CODE(ssb_position_qcl_r16.unpack(bref));
      }
      if (high_speed_cfg_r16.is_present()) {
        HANDLE_CODE(high_speed_cfg_r16->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(high_speed_cfg_v1700, bref);
      HANDLE_CODE(bref.unpack(ch_access_mode2_r17_present, 1));
      HANDLE_CODE(bref.unpack(discovery_burst_win_len_r17_present, 1));
      HANDLE_CODE(bref.unpack(ssb_position_qcl_r17_present, 1));
      unpack_presence_flag(high_speed_cfg_fr2_r17, bref);
      unpack_presence_flag(ul_cfg_common_v1700, bref);
      unpack_presence_flag(ntn_cfg_r17, bref);
      if (high_speed_cfg_v1700.is_present()) {
        HANDLE_CODE(high_speed_cfg_v1700->unpack(bref));
      }
      if (discovery_burst_win_len_r17_present) {
        HANDLE_CODE(discovery_burst_win_len_r17.unpack(bref));
      }
      if (ssb_position_qcl_r17_present) {
        HANDLE_CODE(ssb_position_qcl_r17.unpack(bref));
      }
      if (high_speed_cfg_fr2_r17.is_present()) {
        HANDLE_CODE(high_speed_cfg_fr2_r17->unpack(bref));
      }
      if (ul_cfg_common_v1700.is_present()) {
        HANDLE_CODE(ul_cfg_common_v1700->unpack(bref));
      }
      if (ntn_cfg_r17.is_present()) {
        HANDLE_CODE(ntn_cfg_r17->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(feature_priorities_r17, bref);
      if (feature_priorities_r17.is_present()) {
        HANDLE_CODE(bref.unpack(feature_priorities_r17->red_cap_prio_r17_present, 1));
        HANDLE_CODE(bref.unpack(feature_priorities_r17->slicing_prio_r17_present, 1));
        HANDLE_CODE(bref.unpack(feature_priorities_r17->msg3_repeats_prio_r17_present, 1));
        HANDLE_CODE(bref.unpack(feature_priorities_r17->sdt_prio_r17_present, 1));
        if (feature_priorities_r17->red_cap_prio_r17_present) {
          HANDLE_CODE(unpack_integer(feature_priorities_r17->red_cap_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
        }
        if (feature_priorities_r17->slicing_prio_r17_present) {
          HANDLE_CODE(unpack_integer(feature_priorities_r17->slicing_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
        }
        if (feature_priorities_r17->msg3_repeats_prio_r17_present) {
          HANDLE_CODE(unpack_integer(feature_priorities_r17->msg3_repeats_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
        }
        if (feature_priorities_r17->sdt_prio_r17_present) {
          HANDLE_CODE(unpack_integer(feature_priorities_r17->sdt_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
        }
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ra_ch_access_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void serving_cell_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_present) {
    j.write_int("physCellId", pci);
  }
  if (dl_cfg_common_present) {
    j.write_fieldname("downlinkConfigCommon");
    dl_cfg_common.to_json(j);
  }
  if (ul_cfg_common_present) {
    j.write_fieldname("uplinkConfigCommon");
    ul_cfg_common.to_json(j);
  }
  if (supplementary_ul_cfg_present) {
    j.write_fieldname("supplementaryUplinkConfig");
    supplementary_ul_cfg.to_json(j);
  }
  if (n_timing_advance_offset_present) {
    j.write_str("n-TimingAdvanceOffset", n_timing_advance_offset.to_string());
  }
  if (ssb_positions_in_burst_present) {
    j.write_fieldname("ssb-PositionsInBurst");
    ssb_positions_in_burst.to_json(j);
  }
  if (ssb_periodicity_serving_cell_present) {
    j.write_str("ssb-periodicityServingCell", ssb_periodicity_serving_cell.to_string());
  }
  j.write_str("dmrs-TypeA-Position", dmrs_type_a_position.to_string());
  if (lte_crs_to_match_around_present) {
    j.write_fieldname("lte-CRS-ToMatchAround");
    lte_crs_to_match_around.to_json(j);
  }
  if (rate_match_pattern_to_add_mod_list.size() > 0) {
    j.start_array("rateMatchPatternToAddModList");
    for (const auto& e1 : rate_match_pattern_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rate_match_pattern_to_release_list.size() > 0) {
    j.start_array("rateMatchPatternToReleaseList");
    for (const auto& e1 : rate_match_pattern_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (ssb_subcarrier_spacing_present) {
    j.write_str("ssbSubcarrierSpacing", ssb_subcarrier_spacing.to_string());
  }
  if (tdd_ul_dl_cfg_common_present) {
    j.write_fieldname("tdd-UL-DL-ConfigurationCommon");
    tdd_ul_dl_cfg_common.to_json(j);
  }
  j.write_int("ss-PBCH-BlockPower", ss_pbch_block_pwr);
  if (ext) {
    if (ch_access_mode_r16.is_present()) {
      j.write_fieldname("channelAccessMode-r16");
      ch_access_mode_r16->to_json(j);
    }
    if (discovery_burst_win_len_r16_present) {
      j.write_str("discoveryBurstWindowLength-r16", discovery_burst_win_len_r16.to_string());
    }
    if (ssb_position_qcl_r16_present) {
      j.write_str("ssb-PositionQCL-r16", ssb_position_qcl_r16.to_string());
    }
    if (high_speed_cfg_r16.is_present()) {
      j.write_fieldname("highSpeedConfig-r16");
      high_speed_cfg_r16->to_json(j);
    }
    if (high_speed_cfg_v1700.is_present()) {
      j.write_fieldname("highSpeedConfig-v1700");
      high_speed_cfg_v1700->to_json(j);
    }
    if (ch_access_mode2_r17_present) {
      j.write_str("channelAccessMode2-r17", "enabled");
    }
    if (discovery_burst_win_len_r17_present) {
      j.write_str("discoveryBurstWindowLength-r17", discovery_burst_win_len_r17.to_string());
    }
    if (ssb_position_qcl_r17_present) {
      j.write_str("ssb-PositionQCL-r17", ssb_position_qcl_r17.to_string());
    }
    if (high_speed_cfg_fr2_r17.is_present()) {
      j.write_fieldname("highSpeedConfigFR2-r17");
      high_speed_cfg_fr2_r17->to_json(j);
    }
    if (ul_cfg_common_v1700.is_present()) {
      j.write_fieldname("uplinkConfigCommon-v1700");
      ul_cfg_common_v1700->to_json(j);
    }
    if (ntn_cfg_r17.is_present()) {
      j.write_fieldname("ntn-Config-r17");
      ntn_cfg_r17->to_json(j);
    }
    if (feature_priorities_r17.is_present()) {
      j.write_fieldname("featurePriorities-r17");
      j.start_obj();
      if (feature_priorities_r17->red_cap_prio_r17_present) {
        j.write_int("redCapPriority-r17", feature_priorities_r17->red_cap_prio_r17);
      }
      if (feature_priorities_r17->slicing_prio_r17_present) {
        j.write_int("slicingPriority-r17", feature_priorities_r17->slicing_prio_r17);
      }
      if (feature_priorities_r17->msg3_repeats_prio_r17_present) {
        j.write_int("msg3-Repetitions-Priority-r17", feature_priorities_r17->msg3_repeats_prio_r17);
      }
      if (feature_priorities_r17->sdt_prio_r17_present) {
        j.write_int("sdt-Priority-r17", feature_priorities_r17->sdt_prio_r17);
      }
      j.end_obj();
    }
    if (ra_ch_access_r17_present) {
      j.write_str("ra-ChannelAccess-r17", "enabled");
    }
  }
  j.end_obj();
}

const char* serving_cell_cfg_common_s::n_timing_advance_offset_opts::to_string() const
{
  static const char* names[] = {"n0", "n25600", "n39936"};
  return convert_enum_idx(names, 3, value, "serving_cell_cfg_common_s::n_timing_advance_offset_e_");
}
uint16_t serving_cell_cfg_common_s::n_timing_advance_offset_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 25600, 39936};
  return map_enum_number(numbers, 3, value, "serving_cell_cfg_common_s::n_timing_advance_offset_e_");
}

void serving_cell_cfg_common_s::ssb_positions_in_burst_c_::destroy_()
{
  switch (type_) {
    case types::short_bitmap:
      c.destroy<fixed_bitstring<4>>();
      break;
    case types::medium_bitmap:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::long_bitmap:
      c.destroy<fixed_bitstring<64>>();
      break;
    default:
      break;
  }
}
void serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::short_bitmap:
      c.init<fixed_bitstring<4>>();
      break;
    case types::medium_bitmap:
      c.init<fixed_bitstring<8>>();
      break;
    case types::long_bitmap:
      c.init<fixed_bitstring<64>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_");
  }
}
serving_cell_cfg_common_s::ssb_positions_in_burst_c_::ssb_positions_in_burst_c_(
    const serving_cell_cfg_common_s::ssb_positions_in_burst_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::short_bitmap:
      c.init(other.c.get<fixed_bitstring<4>>());
      break;
    case types::medium_bitmap:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::long_bitmap:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_");
  }
}
serving_cell_cfg_common_s::ssb_positions_in_burst_c_& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::operator=(
    const serving_cell_cfg_common_s::ssb_positions_in_burst_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::short_bitmap:
      c.set(other.c.get<fixed_bitstring<4>>());
      break;
    case types::medium_bitmap:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::long_bitmap:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_");
  }

  return *this;
}
fixed_bitstring<4>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_short_bitmap()
{
  set(types::short_bitmap);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64>>();
}
void serving_cell_cfg_common_s::ssb_positions_in_burst_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::short_bitmap:
      j.write_str("shortBitmap", c.get<fixed_bitstring<4>>().to_string());
      break;
    case types::medium_bitmap:
      j.write_str("mediumBitmap", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::long_bitmap:
      j.write_str("longBitmap", c.get<fixed_bitstring<64>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_");
  }
  j.end_obj();
}
SRSASN_CODE serving_cell_cfg_common_s::ssb_positions_in_burst_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::short_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().pack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_cfg_common_s::ssb_positions_in_burst_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::short_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().unpack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::to_string() const
{
  static const char* names[] = {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "serving_cell_cfg_common_s::ssb_periodicity_serving_cell_e_");
}
uint8_t serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_s::ssb_periodicity_serving_cell_e_");
}

const char* serving_cell_cfg_common_s::dmrs_type_a_position_opts::to_string() const
{
  static const char* names[] = {"pos2", "pos3"};
  return convert_enum_idx(names, 2, value, "serving_cell_cfg_common_s::dmrs_type_a_position_e_");
}
uint8_t serving_cell_cfg_common_s::dmrs_type_a_position_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "serving_cell_cfg_common_s::dmrs_type_a_position_e_");
}

void serving_cell_cfg_common_s::ch_access_mode_r16_c_::set(types::options e)
{
  type_ = e;
}
void serving_cell_cfg_common_s::ch_access_mode_r16_c_::set_dyn()
{
  set(types::dyn);
}
semi_static_ch_access_cfg_r16_s& serving_cell_cfg_common_s::ch_access_mode_r16_c_::set_semi_static()
{
  set(types::semi_static);
  return c;
}
void serving_cell_cfg_common_s::ch_access_mode_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dyn:
      break;
    case types::semi_static:
      j.write_fieldname("semiStatic");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ch_access_mode_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE serving_cell_cfg_common_s::ch_access_mode_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dyn:
      break;
    case types::semi_static:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ch_access_mode_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_cfg_common_s::ch_access_mode_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dyn:
      break;
    case types::semi_static:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ch_access_mode_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_cell_cfg_common_s::discovery_burst_win_len_r16_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms1", "ms2", "ms3", "ms4", "ms5"};
  return convert_enum_idx(names, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r16_e_");
}
float serving_cell_cfg_common_s::discovery_burst_win_len_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r16_e_");
}
const char* serving_cell_cfg_common_s::discovery_burst_win_len_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "3", "4", "5"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r16_e_");
}

const char* serving_cell_cfg_common_s::discovery_burst_win_len_r17_opts::to_string() const
{
  static const char* names[] = {"ms0dot125", "ms0dot25", "ms0dot5", "ms0dot75", "ms1", "ms1dot25"};
  return convert_enum_idx(names, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r17_e_");
}
float serving_cell_cfg_common_s::discovery_burst_win_len_r17_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 0.75, 1.0, 1.25};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r17_e_");
}
const char* serving_cell_cfg_common_s::discovery_burst_win_len_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.125", "0.25", "0.5", "0.75", "1", "1.25"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r17_e_");
}

// TAG ::= SEQUENCE
SRSASN_CODE tag_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, tag_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(time_align_timer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tag_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(tag_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(time_align_timer.unpack(bref));

  return SRSASN_SUCCESS;
}
void tag_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("tag-Id", tag_id);
  j.write_str("timeAlignmentTimer", time_align_timer.to_string());
  j.end_obj();
}

// DCP-Config-r16 ::= SEQUENCE
SRSASN_CODE d_cp_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ps_wake_up_r16_present, 1));
  HANDLE_CODE(bref.pack(ps_tx_periodic_l1_rsrp_r16_present, 1));
  HANDLE_CODE(bref.pack(ps_tx_other_periodic_csi_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, ps_rnti_r16, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, ps_offset_r16, (uint8_t)1u, (uint8_t)120u));
  HANDLE_CODE(pack_integer(bref, size_dci_2_6_r16, (uint8_t)1u, (uint8_t)140u));
  HANDLE_CODE(pack_integer(bref, ps_position_dci_2_6_r16, (uint8_t)0u, (uint8_t)139u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE d_cp_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ps_wake_up_r16_present, 1));
  HANDLE_CODE(bref.unpack(ps_tx_periodic_l1_rsrp_r16_present, 1));
  HANDLE_CODE(bref.unpack(ps_tx_other_periodic_csi_r16_present, 1));

  HANDLE_CODE(unpack_integer(ps_rnti_r16, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(ps_offset_r16, bref, (uint8_t)1u, (uint8_t)120u));
  HANDLE_CODE(unpack_integer(size_dci_2_6_r16, bref, (uint8_t)1u, (uint8_t)140u));
  HANDLE_CODE(unpack_integer(ps_position_dci_2_6_r16, bref, (uint8_t)0u, (uint8_t)139u));

  return SRSASN_SUCCESS;
}
void d_cp_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ps-RNTI-r16", ps_rnti_r16);
  j.write_int("ps-Offset-r16", ps_offset_r16);
  j.write_int("sizeDCI-2-6-r16", size_dci_2_6_r16);
  j.write_int("ps-PositionDCI-2-6-r16", ps_position_dci_2_6_r16);
  if (ps_wake_up_r16_present) {
    j.write_str("ps-WakeUp-r16", "true");
  }
  if (ps_tx_periodic_l1_rsrp_r16_present) {
    j.write_str("ps-TransmitPeriodicL1-RSRP-r16", "true");
  }
  if (ps_tx_other_periodic_csi_r16_present) {
    j.write_str("ps-TransmitOtherPeriodicCSI-r16", "true");
  }
  j.end_obj();
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

// DRX-ConfigExt-v1700 ::= SEQUENCE
SRSASN_CODE drx_cfg_ext_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, drx_harq_rtt_timer_dl_r17, (uint16_t)0u, (uint16_t)448u));
  HANDLE_CODE(pack_integer(bref, drx_harq_rtt_timer_ul_r17, (uint16_t)0u, (uint16_t)448u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_ext_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(drx_harq_rtt_timer_dl_r17, bref, (uint16_t)0u, (uint16_t)448u));
  HANDLE_CODE(unpack_integer(drx_harq_rtt_timer_ul_r17, bref, (uint16_t)0u, (uint16_t)448u));

  return SRSASN_SUCCESS;
}
void drx_cfg_ext_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drx-HARQ-RTT-TimerDL-r17", drx_harq_rtt_timer_dl_r17);
  j.write_int("drx-HARQ-RTT-TimerUL-r17", drx_harq_rtt_timer_ul_r17);
  j.end_obj();
}

// DRX-ConfigSL-r17 ::= SEQUENCE
SRSASN_CODE drx_cfg_sl_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, drx_harq_rtt_timer_sl_r17, (uint8_t)0u, (uint8_t)56u));
  HANDLE_CODE(drx_retx_timer_sl_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_sl_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(drx_harq_rtt_timer_sl_r17, bref, (uint8_t)0u, (uint8_t)56u));
  HANDLE_CODE(drx_retx_timer_sl_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void drx_cfg_sl_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drx-HARQ-RTT-TimerSL-r17", drx_harq_rtt_timer_sl_r17);
  j.write_str("drx-RetransmissionTimerSL-r17", drx_retx_timer_sl_r17.to_string());
  j.end_obj();
}

const char* drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_opts::to_string() const
{
  static const char* names[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_e_");
}
uint16_t drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_e_");
}

// DRX-ConfigSecondaryGroup-r16 ::= SEQUENCE
SRSASN_CODE drx_cfg_secondary_group_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(drx_on_dur_timer_r16.pack(bref));
  HANDLE_CODE(drx_inactivity_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_secondary_group_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(drx_on_dur_timer_r16.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void drx_cfg_secondary_group_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-onDurationTimer-r16");
  drx_on_dur_timer_r16.to_json(j);
  j.write_str("drx-InactivityTimer-r16", drx_inactivity_timer_r16.to_string());
  j.end_obj();
}

void drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::destroy_() {}
void drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::drx_on_dur_timer_r16_c_(
    const drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_& other)
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
      log_invalid_choice_id(type_, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_");
  }
}
drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_&
drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::operator=(
    const drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_& other)
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
      log_invalid_choice_id(type_, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_");
  }

  return *this;
}
uint8_t& drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_&
drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_");
}
uint16_t drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(
      numbers, 24, value, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_");
}

const char* drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_opts::to_string() const
{
  static const char* names[] = {"ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",
                                "ms10",   "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",
                                "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_e_");
}
uint16_t drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_e_");
}

// DataInactivityTimer ::= ENUMERATED
const char* data_inactivity_timer_opts::to_string() const
{
  static const char* names[] = {
      "s1", "s2", "s3", "s5", "s7", "s10", "s15", "s20", "s40", "s50", "s60", "s80", "s100", "s120", "s150", "s180"};
  return convert_enum_idx(names, 16, value, "data_inactivity_timer_e");
}
uint8_t data_inactivity_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 5, 7, 10, 15, 20, 40, 50, 60, 80, 100, 120, 150, 180};
  return map_enum_number(numbers, 16, value, "data_inactivity_timer_e");
}

// DeactivatedSCG-Config-r17 ::= SEQUENCE
SRSASN_CODE deactiv_scg_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(bfd_and_rlm_r17, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE deactiv_scg_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(bfd_and_rlm_r17, 1));

  return SRSASN_SUCCESS;
}
void deactiv_scg_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("bfd-and-RLM-r17", bfd_and_rlm_r17);
  j.end_obj();
}

// GoodServingCellEvaluation-r17 ::= SEQUENCE
SRSASN_CODE good_serving_cell_eval_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(offset_r17_present, 1));

  if (offset_r17_present) {
    HANDLE_CODE(offset_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE good_serving_cell_eval_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(offset_r17_present, 1));

  if (offset_r17_present) {
    HANDLE_CODE(offset_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void good_serving_cell_eval_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (offset_r17_present) {
    j.write_str("offset-r17", offset_r17.to_string());
  }
  j.end_obj();
}

const char* good_serving_cell_eval_r17_s::offset_r17_opts::to_string() const
{
  static const char* names[] = {"db2", "db4", "db6", "db8"};
  return convert_enum_idx(names, 4, value, "good_serving_cell_eval_r17_s::offset_r17_e_");
}
uint8_t good_serving_cell_eval_r17_s::offset_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8};
  return map_enum_number(numbers, 4, value, "good_serving_cell_eval_r17_s::offset_r17_e_");
}

// IntraBandCC-CombinationReqList-r17 ::= SEQUENCE
SRSASN_CODE intra_band_cc_combination_req_list_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_idx_list_r17, 1, 32, integer_packer<uint8_t>(0, 31)));
  HANDLE_CODE(pack_dyn_seq_of(bref, cc_combination_list_r17, 1, 128, SeqOfPacker<Packer>(1, 32, Packer())));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_band_cc_combination_req_list_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(serv_cell_idx_list_r17, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
  HANDLE_CODE(unpack_dyn_seq_of(cc_combination_list_r17, bref, 1, 128, SeqOfPacker<Packer>(1, 32, Packer())));

  return SRSASN_SUCCESS;
}
void intra_band_cc_combination_req_list_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("servCellIndexList-r17");
  for (const auto& e1 : serv_cell_idx_list_r17) {
    j.write_int(e1);
  }
  j.end_array();
  j.start_array("cc-CombinationList-r17");
  for (const auto& e1 : cc_combination_list_r17) {
    j.start_array();
    for (const auto& e2 : e1) {
      e2.to_json(j);
    }
    j.end_array();
  }
  j.end_array();
  j.end_obj();
}

// MBS-RNTI-SpecificConfig-r17 ::= SEQUENCE
SRSASN_CODE mbs_rnti_specific_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_cfg_ptm_r17_present, 1));
  HANDLE_CODE(bref.pack(harq_feedback_enabler_multicast_r17_present, 1));
  HANDLE_CODE(bref.pack(harq_feedback_option_multicast_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_aggregation_factor_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, mbs_rnti_specific_cfg_id_r17, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(group_common_rnti_r17.pack(bref));
  if (drx_cfg_ptm_r17_present) {
    HANDLE_CODE(drx_cfg_ptm_r17.pack(bref));
  }
  if (harq_feedback_enabler_multicast_r17_present) {
    HANDLE_CODE(harq_feedback_enabler_multicast_r17.pack(bref));
  }
  if (harq_feedback_option_multicast_r17_present) {
    HANDLE_CODE(harq_feedback_option_multicast_r17.pack(bref));
  }
  if (pdsch_aggregation_factor_r17_present) {
    HANDLE_CODE(pdsch_aggregation_factor_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_rnti_specific_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_cfg_ptm_r17_present, 1));
  HANDLE_CODE(bref.unpack(harq_feedback_enabler_multicast_r17_present, 1));
  HANDLE_CODE(bref.unpack(harq_feedback_option_multicast_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_aggregation_factor_r17_present, 1));

  HANDLE_CODE(unpack_integer(mbs_rnti_specific_cfg_id_r17, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(group_common_rnti_r17.unpack(bref));
  if (drx_cfg_ptm_r17_present) {
    HANDLE_CODE(drx_cfg_ptm_r17.unpack(bref));
  }
  if (harq_feedback_enabler_multicast_r17_present) {
    HANDLE_CODE(harq_feedback_enabler_multicast_r17.unpack(bref));
  }
  if (harq_feedback_option_multicast_r17_present) {
    HANDLE_CODE(harq_feedback_option_multicast_r17.unpack(bref));
  }
  if (pdsch_aggregation_factor_r17_present) {
    HANDLE_CODE(pdsch_aggregation_factor_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbs_rnti_specific_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("mbs-RNTI-SpecificConfigId-r17", mbs_rnti_specific_cfg_id_r17);
  j.write_fieldname("groupCommon-RNTI-r17");
  group_common_rnti_r17.to_json(j);
  if (drx_cfg_ptm_r17_present) {
    j.write_fieldname("drx-ConfigPTM-r17");
    drx_cfg_ptm_r17.to_json(j);
  }
  if (harq_feedback_enabler_multicast_r17_present) {
    j.write_str("harq-FeedbackEnablerMulticast-r17", harq_feedback_enabler_multicast_r17.to_string());
  }
  if (harq_feedback_option_multicast_r17_present) {
    j.write_str("harq-FeedbackOptionMulticast-r17", harq_feedback_option_multicast_r17.to_string());
  }
  if (pdsch_aggregation_factor_r17_present) {
    j.write_str("pdsch-AggregationFactor-r17", pdsch_aggregation_factor_r17.to_string());
  }
  j.end_obj();
}

void mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::destroy_() {}
void mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::group_common_rnti_r17_c_(
    const mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::g_rnti:
      c.init(other.c.get<uint32_t>());
      break;
    case types::g_cs_rnti:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_");
  }
}
mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_& mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::operator=(
    const mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::g_rnti:
      c.set(other.c.get<uint32_t>());
      break;
    case types::g_cs_rnti:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_");
  }

  return *this;
}
uint32_t& mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::set_g_rnti()
{
  set(types::g_rnti);
  return c.get<uint32_t>();
}
uint32_t& mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::set_g_cs_rnti()
{
  set(types::g_cs_rnti);
  return c.get<uint32_t>();
}
void mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::g_rnti:
      j.write_int("g-RNTI", c.get<uint32_t>());
      break;
    case types::g_cs_rnti:
      j.write_int("g-CS-RNTI", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::g_rnti:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u));
      break;
    case types::g_cs_rnti:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u));
      break;
    default:
      log_invalid_choice_id(type_, "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::g_rnti:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u));
      break;
    case types::g_cs_rnti:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u));
      break;
    default:
      log_invalid_choice_id(type_, "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17_opts::to_string() const
{
  static const char* names[] = {"dci-enabler", "enabled"};
  return convert_enum_idx(names, 2, value, "mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17_e_");
}

const char* mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17_opts::to_string() const
{
  static const char* names[] = {"ack-nack", "nack-only"};
  return convert_enum_idx(names, 2, value, "mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17_e_");
}

const char* mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8"};
  return convert_enum_idx(names, 3, value, "mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_e_");
}
uint8_t mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_e_");
}

// MulticastConfig-r17 ::= SEQUENCE
SRSASN_CODE multicast_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdsch_harq_ack_codebook_list_multicast_r17_present, 1));
  HANDLE_CODE(bref.pack(type1_codebook_generation_mode_r17_present, 1));

  if (pdsch_harq_ack_codebook_list_multicast_r17_present) {
    HANDLE_CODE(pdsch_harq_ack_codebook_list_multicast_r17.pack(bref));
  }
  if (type1_codebook_generation_mode_r17_present) {
    HANDLE_CODE(type1_codebook_generation_mode_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdsch_harq_ack_codebook_list_multicast_r17_present, 1));
  HANDLE_CODE(bref.unpack(type1_codebook_generation_mode_r17_present, 1));

  if (pdsch_harq_ack_codebook_list_multicast_r17_present) {
    HANDLE_CODE(pdsch_harq_ack_codebook_list_multicast_r17.unpack(bref));
  }
  if (type1_codebook_generation_mode_r17_present) {
    HANDLE_CODE(type1_codebook_generation_mode_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void multicast_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdsch_harq_ack_codebook_list_multicast_r17_present) {
    j.write_fieldname("pdsch-HARQ-ACK-CodebookListMulticast-r17");
    pdsch_harq_ack_codebook_list_multicast_r17.to_json(j);
  }
  if (type1_codebook_generation_mode_r17_present) {
    j.write_str("type1CodebookGenerationMode-r17", type1_codebook_generation_mode_r17.to_string());
  }
  j.end_obj();
}

const char* multicast_cfg_r17_s::type1_codebook_generation_mode_r17_opts::to_string() const
{
  static const char* names[] = {"mode1", "mode2"};
  return convert_enum_idx(names, 2, value, "multicast_cfg_r17_s::type1_codebook_generation_mode_r17_e_");
}
uint8_t multicast_cfg_r17_s::type1_codebook_generation_mode_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "multicast_cfg_r17_s::type1_codebook_generation_mode_r17_e_");
}

// MulticastRLC-BearerConfig-r17 ::= SEQUENCE
SRSASN_CODE multicast_rlc_bearer_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(is_ptm_entity_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, served_mbs_radio_bearer_r17, (uint16_t)1u, (uint16_t)512u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_rlc_bearer_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(is_ptm_entity_r17_present, 1));

  HANDLE_CODE(unpack_integer(served_mbs_radio_bearer_r17, bref, (uint16_t)1u, (uint16_t)512u));

  return SRSASN_SUCCESS;
}
void multicast_rlc_bearer_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servedMBS-RadioBearer-r17", served_mbs_radio_bearer_r17);
  if (is_ptm_entity_r17_present) {
    j.write_str("isPTM-Entity-r17", "true");
  }
  j.end_obj();
}

// PDCCH-BlindDetectionCA-CombIndicator-r16 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_ca_comb_ind_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca1_r16, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca2_r16, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_ca_comb_ind_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca1_r16, bref, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca2_r16, bref, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_ca_comb_ind_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdcch-BlindDetectionCA1-r16", pdcch_blind_detection_ca1_r16);
  j.write_int("pdcch-BlindDetectionCA2-r16", pdcch_blind_detection_ca2_r16);
  j.end_obj();
}

// PDCCH-BlindDetectionCA-CombIndicator-r17 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_ca_comb_ind_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcch_blind_detection_ca1_r17_present, 1));
  HANDLE_CODE(bref.pack(pdcch_blind_detection_ca2_r17_present, 1));

  if (pdcch_blind_detection_ca1_r17_present) {
    HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca1_r17, (uint8_t)1u, (uint8_t)15u));
  }
  if (pdcch_blind_detection_ca2_r17_present) {
    HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca2_r17, (uint8_t)1u, (uint8_t)15u));
  }
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca3_r17, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_ca_comb_ind_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcch_blind_detection_ca1_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_blind_detection_ca2_r17_present, 1));

  if (pdcch_blind_detection_ca1_r17_present) {
    HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca1_r17, bref, (uint8_t)1u, (uint8_t)15u));
  }
  if (pdcch_blind_detection_ca2_r17_present) {
    HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca2_r17, bref, (uint8_t)1u, (uint8_t)15u));
  }
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca3_r17, bref, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_ca_comb_ind_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcch_blind_detection_ca1_r17_present) {
    j.write_int("pdcch-BlindDetectionCA1-r17", pdcch_blind_detection_ca1_r17);
  }
  if (pdcch_blind_detection_ca2_r17_present) {
    j.write_int("pdcch-BlindDetectionCA2-r17", pdcch_blind_detection_ca2_r17);
  }
  j.write_int("pdcch-BlindDetectionCA3-r17", pdcch_blind_detection_ca3_r17);
  j.end_obj();
}

// PDSCH-HARQ-ACK-EnhType3-r17 ::= SEQUENCE
SRSASN_CODE pdsch_harq_ack_enh_type3_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_ndi_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_cbg_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, pdsch_harq_ack_enh_type3_idx_r17, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(applicable_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_harq_ack_enh_type3_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdsch_harq_ack_enh_type3_ndi_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_harq_ack_enh_type3_cbg_r17_present, 1));

  HANDLE_CODE(unpack_integer(pdsch_harq_ack_enh_type3_idx_r17, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(applicable_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void pdsch_harq_ack_enh_type3_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdsch-HARQ-ACK-EnhType3Index-r17", pdsch_harq_ack_enh_type3_idx_r17);
  j.write_fieldname("applicable-r17");
  applicable_r17.to_json(j);
  if (pdsch_harq_ack_enh_type3_ndi_r17_present) {
    j.write_str("pdsch-HARQ-ACK-EnhType3NDI-r17", "true");
  }
  if (pdsch_harq_ack_enh_type3_cbg_r17_present) {
    j.write_str("pdsch-HARQ-ACK-EnhType3CBG-r17", "true");
  }
  j.end_obj();
}

void pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::destroy_()
{
  switch (type_) {
    case types::per_cc:
      c.destroy<per_cc_l_>();
      break;
    case types::per_harq:
      c.destroy<per_harq_l_>();
      break;
    default:
      break;
  }
}
void pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::per_cc:
      c.init<per_cc_l_>();
      break;
    case types::per_harq:
      c.init<per_harq_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_");
  }
}
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::applicable_r17_c_(
    const pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::per_cc:
      c.init(other.c.get<per_cc_l_>());
      break;
    case types::per_harq:
      c.init(other.c.get<per_harq_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_");
  }
}
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_& pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::operator=(
    const pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::per_cc:
      c.set(other.c.get<per_cc_l_>());
      break;
    case types::per_harq:
      c.set(other.c.get<per_harq_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_");
  }

  return *this;
}
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_cc_l_&
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::set_per_cc()
{
  set(types::per_cc);
  return c.get<per_cc_l_>();
}
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_harq_l_&
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::set_per_harq()
{
  set(types::per_harq);
  return c.get<per_harq_l_>();
}
void pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::per_cc:
      j.start_array("perCC");
      for (const auto& e1 : c.get<per_cc_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::per_harq:
      j.start_array("perHARQ");
      for (const auto& e1 : c.get<per_harq_l_>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::per_cc:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<per_cc_l_>(), 1, 32, integer_packer<uint8_t>(0, 1)));
      break;
    case types::per_harq:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<per_harq_l_>(), 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::per_cc:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<per_cc_l_>(), bref, 1, 32, integer_packer<uint8_t>(0, 1)));
      break;
    case types::per_harq:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<per_harq_l_>(), bref, 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PHR-Config ::= SEQUENCE
SRSASN_CODE phr_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(phr_periodic_timer.pack(bref));
  HANDLE_CODE(phr_prohibit_timer.pack(bref));
  HANDLE_CODE(phr_tx_pwr_factor_change.pack(bref));
  HANDLE_CODE(bref.pack(multiple_phr, 1));
  HANDLE_CODE(bref.pack(dummy, 1));
  HANDLE_CODE(bref.pack(phr_type2_other_cell, 1));
  HANDLE_CODE(phr_mode_other_cg.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mpe_report_fr2_r16.is_present();
    group_flags[1] |= mpe_report_fr2_r17.is_present();
    group_flags[1] |= two_phr_mode_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mpe_report_fr2_r16.is_present(), 1));
      if (mpe_report_fr2_r16.is_present()) {
        HANDLE_CODE(mpe_report_fr2_r16->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mpe_report_fr2_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(two_phr_mode_r17_present, 1));
      if (mpe_report_fr2_r17.is_present()) {
        HANDLE_CODE(mpe_report_fr2_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phr_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(phr_periodic_timer.unpack(bref));
  HANDLE_CODE(phr_prohibit_timer.unpack(bref));
  HANDLE_CODE(phr_tx_pwr_factor_change.unpack(bref));
  HANDLE_CODE(bref.unpack(multiple_phr, 1));
  HANDLE_CODE(bref.unpack(dummy, 1));
  HANDLE_CODE(bref.unpack(phr_type2_other_cell, 1));
  HANDLE_CODE(phr_mode_other_cg.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(mpe_report_fr2_r16, bref);
      if (mpe_report_fr2_r16.is_present()) {
        HANDLE_CODE(mpe_report_fr2_r16->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(mpe_report_fr2_r17, bref);
      HANDLE_CODE(bref.unpack(two_phr_mode_r17_present, 1));
      if (mpe_report_fr2_r17.is_present()) {
        HANDLE_CODE(mpe_report_fr2_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phr_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("phr-PeriodicTimer", phr_periodic_timer.to_string());
  j.write_str("phr-ProhibitTimer", phr_prohibit_timer.to_string());
  j.write_str("phr-Tx-PowerFactorChange", phr_tx_pwr_factor_change.to_string());
  j.write_bool("multiplePHR", multiple_phr);
  j.write_bool("dummy", dummy);
  j.write_bool("phr-Type2OtherCell", phr_type2_other_cell);
  j.write_str("phr-ModeOtherCG", phr_mode_other_cg.to_string());
  if (ext) {
    if (mpe_report_fr2_r16.is_present()) {
      j.write_fieldname("mpe-Reporting-FR2-r16");
      mpe_report_fr2_r16->to_json(j);
    }
    if (mpe_report_fr2_r17.is_present()) {
      j.write_fieldname("mpe-Reporting-FR2-r17");
      mpe_report_fr2_r17->to_json(j);
    }
    if (two_phr_mode_r17_present) {
      j.write_str("twoPHRMode-r17", "enabled");
    }
  }
  j.end_obj();
}

const char* phr_cfg_s::phr_periodic_timer_opts::to_string() const
{
  static const char* names[] = {"sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000", "infinity"};
  return convert_enum_idx(names, 8, value, "phr_cfg_s::phr_periodic_timer_e_");
}
int16_t phr_cfg_s::phr_periodic_timer_opts::to_number() const
{
  static const int16_t numbers[] = {10, 20, 50, 100, 200, 500, 1000, -1};
  return map_enum_number(numbers, 8, value, "phr_cfg_s::phr_periodic_timer_e_");
}

const char* phr_cfg_s::phr_prohibit_timer_opts::to_string() const
{
  static const char* names[] = {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
  return convert_enum_idx(names, 8, value, "phr_cfg_s::phr_prohibit_timer_e_");
}
uint16_t phr_cfg_s::phr_prohibit_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(numbers, 8, value, "phr_cfg_s::phr_prohibit_timer_e_");
}

const char* phr_cfg_s::phr_tx_pwr_factor_change_opts::to_string() const
{
  static const char* names[] = {"dB1", "dB3", "dB6", "infinity"};
  return convert_enum_idx(names, 4, value, "phr_cfg_s::phr_tx_pwr_factor_change_e_");
}
int8_t phr_cfg_s::phr_tx_pwr_factor_change_opts::to_number() const
{
  static const int8_t numbers[] = {1, 3, 6, -1};
  return map_enum_number(numbers, 4, value, "phr_cfg_s::phr_tx_pwr_factor_change_e_");
}

const char* phr_cfg_s::phr_mode_other_cg_opts::to_string() const
{
  static const char* names[] = {"real", "virtual"};
  return convert_enum_idx(names, 2, value, "phr_cfg_s::phr_mode_other_cg_e_");
}

// RLC-Config-v1610 ::= SEQUENCE
SRSASN_CODE rlc_cfg_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dl_am_rlc_v1610.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dl_am_rlc_v1610.unpack(bref));

  return SRSASN_SUCCESS;
}
void rlc_cfg_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-AM-RLC-v1610");
  dl_am_rlc_v1610.to_json(j);
  j.end_obj();
}

// RLC-Config-v1700 ::= SEQUENCE
SRSASN_CODE rlc_cfg_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dl_am_rlc_v1700.pack(bref));
  HANDLE_CODE(dl_um_rlc_v1700.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dl_am_rlc_v1700.unpack(bref));
  HANDLE_CODE(dl_um_rlc_v1700.unpack(bref));

  return SRSASN_SUCCESS;
}
void rlc_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dl-AM-RLC-v1700");
  dl_am_rlc_v1700.to_json(j);
  j.write_fieldname("dl-UM-RLC-v1700");
  dl_um_rlc_v1700.to_json(j);
  j.end_obj();
}

// RLF-TimersAndConstants ::= SEQUENCE
SRSASN_CODE rlf_timers_and_consts_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(t310.pack(bref));
  HANDLE_CODE(n310.pack(bref));
  HANDLE_CODE(n311.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= t311;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(t311.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(t310.unpack(bref));
  HANDLE_CODE(n310.unpack(bref));
  HANDLE_CODE(n311.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(t311.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void rlf_timers_and_consts_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t310", t310.to_string());
  j.write_str("n310", n310.to_string());
  j.write_str("n311", n311.to_string());
  if (ext) {
    j.write_str("t311", t311.to_string());
  }
  j.end_obj();
}

const char* rlf_timers_and_consts_s::t310_opts::to_string() const
{
  static const char* names[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000", "ms4000", "ms6000"};
  return convert_enum_idx(names, 9, value, "rlf_timers_and_consts_s::t310_e_");
}
uint16_t rlf_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 50, 100, 200, 500, 1000, 2000, 4000, 6000};
  return map_enum_number(numbers, 9, value, "rlf_timers_and_consts_s::t310_e_");
}

const char* rlf_timers_and_consts_s::n310_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(names, 8, value, "rlf_timers_and_consts_s::n310_e_");
}
uint8_t rlf_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(numbers, 8, value, "rlf_timers_and_consts_s::n310_e_");
}

const char* rlf_timers_and_consts_s::n311_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(names, 8, value, "rlf_timers_and_consts_s::n311_e_");
}
uint8_t rlf_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(numbers, 8, value, "rlf_timers_and_consts_s::n311_e_");
}

const char* rlf_timers_and_consts_s::t311_opts::to_string() const
{
  static const char* names[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(names, 7, value, "rlf_timers_and_consts_s::t311_e_");
}
uint16_t rlf_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t numbers[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(numbers, 7, value, "rlf_timers_and_consts_s::t311_e_");
}

// ReconfigurationWithSync ::= SEQUENCE
SRSASN_CODE recfg_with_sync_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sp_cell_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(rach_cfg_ded_present, 1));

  if (sp_cell_cfg_common_present) {
    HANDLE_CODE(sp_cell_cfg_common.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, new_ue_id, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(t304.pack(bref));
  if (rach_cfg_ded_present) {
    HANDLE_CODE(rach_cfg_ded.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= smtc.is_present();
    group_flags[1] |= daps_ul_pwr_cfg_r16.is_present();
    group_flags[2] |= sl_path_switch_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc.is_present(), 1));
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(daps_ul_pwr_cfg_r16.is_present(), 1));
      if (daps_ul_pwr_cfg_r16.is_present()) {
        HANDLE_CODE(daps_ul_pwr_cfg_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_path_switch_cfg_r17.is_present(), 1));
      if (sl_path_switch_cfg_r17.is_present()) {
        HANDLE_CODE(sl_path_switch_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE recfg_with_sync_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sp_cell_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(rach_cfg_ded_present, 1));

  if (sp_cell_cfg_common_present) {
    HANDLE_CODE(sp_cell_cfg_common.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(new_ue_id, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(t304.unpack(bref));
  if (rach_cfg_ded_present) {
    HANDLE_CODE(rach_cfg_ded.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(smtc, bref);
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(daps_ul_pwr_cfg_r16, bref);
      if (daps_ul_pwr_cfg_r16.is_present()) {
        HANDLE_CODE(daps_ul_pwr_cfg_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_path_switch_cfg_r17, bref);
      if (sl_path_switch_cfg_r17.is_present()) {
        HANDLE_CODE(sl_path_switch_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void recfg_with_sync_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sp_cell_cfg_common_present) {
    j.write_fieldname("spCellConfigCommon");
    sp_cell_cfg_common.to_json(j);
  }
  j.write_int("newUE-Identity", new_ue_id);
  j.write_str("t304", t304.to_string());
  if (rach_cfg_ded_present) {
    j.write_fieldname("rach-ConfigDedicated");
    rach_cfg_ded.to_json(j);
  }
  if (ext) {
    if (smtc.is_present()) {
      j.write_fieldname("smtc");
      smtc->to_json(j);
    }
    if (daps_ul_pwr_cfg_r16.is_present()) {
      j.write_fieldname("daps-UplinkPowerConfig-r16");
      daps_ul_pwr_cfg_r16->to_json(j);
    }
    if (sl_path_switch_cfg_r17.is_present()) {
      j.write_fieldname("sl-PathSwitchConfig-r17");
      sl_path_switch_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* recfg_with_sync_s::t304_opts::to_string() const
{
  static const char* names[] = {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "ms10000"};
  return convert_enum_idx(names, 8, value, "recfg_with_sync_s::t304_e_");
}
uint16_t recfg_with_sync_s::t304_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return map_enum_number(numbers, 8, value, "recfg_with_sync_s::t304_e_");
}

void recfg_with_sync_s::rach_cfg_ded_c_::destroy_()
{
  switch (type_) {
    case types::ul:
      c.destroy<rach_cfg_ded_s>();
      break;
    case types::supplementary_ul:
      c.destroy<rach_cfg_ded_s>();
      break;
    default:
      break;
  }
}
void recfg_with_sync_s::rach_cfg_ded_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ul:
      c.init<rach_cfg_ded_s>();
      break;
    case types::supplementary_ul:
      c.init<rach_cfg_ded_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "recfg_with_sync_s::rach_cfg_ded_c_");
  }
}
recfg_with_sync_s::rach_cfg_ded_c_::rach_cfg_ded_c_(const recfg_with_sync_s::rach_cfg_ded_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ul:
      c.init(other.c.get<rach_cfg_ded_s>());
      break;
    case types::supplementary_ul:
      c.init(other.c.get<rach_cfg_ded_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "recfg_with_sync_s::rach_cfg_ded_c_");
  }
}
recfg_with_sync_s::rach_cfg_ded_c_&
recfg_with_sync_s::rach_cfg_ded_c_::operator=(const recfg_with_sync_s::rach_cfg_ded_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ul:
      c.set(other.c.get<rach_cfg_ded_s>());
      break;
    case types::supplementary_ul:
      c.set(other.c.get<rach_cfg_ded_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "recfg_with_sync_s::rach_cfg_ded_c_");
  }

  return *this;
}
rach_cfg_ded_s& recfg_with_sync_s::rach_cfg_ded_c_::set_ul()
{
  set(types::ul);
  return c.get<rach_cfg_ded_s>();
}
rach_cfg_ded_s& recfg_with_sync_s::rach_cfg_ded_c_::set_supplementary_ul()
{
  set(types::supplementary_ul);
  return c.get<rach_cfg_ded_s>();
}
void recfg_with_sync_s::rach_cfg_ded_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul:
      j.write_fieldname("uplink");
      c.get<rach_cfg_ded_s>().to_json(j);
      break;
    case types::supplementary_ul:
      j.write_fieldname("supplementaryUplink");
      c.get<rach_cfg_ded_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "recfg_with_sync_s::rach_cfg_ded_c_");
  }
  j.end_obj();
}
SRSASN_CODE recfg_with_sync_s::rach_cfg_ded_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul:
      HANDLE_CODE(c.get<rach_cfg_ded_s>().pack(bref));
      break;
    case types::supplementary_ul:
      HANDLE_CODE(c.get<rach_cfg_ded_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "recfg_with_sync_s::rach_cfg_ded_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE recfg_with_sync_s::rach_cfg_ded_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul:
      HANDLE_CODE(c.get<rach_cfg_ded_s>().unpack(bref));
      break;
    case types::supplementary_ul:
      HANDLE_CODE(c.get<rach_cfg_ded_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "recfg_with_sync_s::rach_cfg_ded_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SchedulingRequestConfig ::= SEQUENCE
SRSASN_CODE sched_request_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sched_request_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(sched_request_to_release_list.size() > 0, 1));

  if (sched_request_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_request_to_add_mod_list, 1, 8));
  }
  if (sched_request_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_request_to_release_list, 1, 8, integer_packer<uint8_t>(0, 7)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_cfg_s::unpack(cbit_ref& bref)
{
  bool sched_request_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(sched_request_to_add_mod_list_present, 1));
  bool sched_request_to_release_list_present;
  HANDLE_CODE(bref.unpack(sched_request_to_release_list_present, 1));

  if (sched_request_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_request_to_add_mod_list, bref, 1, 8));
  }
  if (sched_request_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_request_to_release_list, bref, 1, 8, integer_packer<uint8_t>(0, 7)));
  }

  return SRSASN_SUCCESS;
}
void sched_request_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sched_request_to_add_mod_list.size() > 0) {
    j.start_array("schedulingRequestToAddModList");
    for (const auto& e1 : sched_request_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sched_request_to_release_list.size() > 0) {
    j.start_array("schedulingRequestToReleaseList");
    for (const auto& e1 : sched_request_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// SchedulingRequestConfig-v1700 ::= SEQUENCE
SRSASN_CODE sched_request_cfg_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sched_request_to_add_mod_list_ext_v1700.size() > 0, 1));

  if (sched_request_to_add_mod_list_ext_v1700.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_request_to_add_mod_list_ext_v1700, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_cfg_v1700_s::unpack(cbit_ref& bref)
{
  bool sched_request_to_add_mod_list_ext_v1700_present;
  HANDLE_CODE(bref.unpack(sched_request_to_add_mod_list_ext_v1700_present, 1));

  if (sched_request_to_add_mod_list_ext_v1700_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_request_to_add_mod_list_ext_v1700, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void sched_request_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sched_request_to_add_mod_list_ext_v1700.size() > 0) {
    j.start_array("schedulingRequestToAddModListExt-v1700");
    for (const auto& e1 : sched_request_to_add_mod_list_ext_v1700) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// TAG-Config ::= SEQUENCE
SRSASN_CODE tag_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tag_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(tag_to_add_mod_list.size() > 0, 1));

  if (tag_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tag_to_release_list, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (tag_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tag_to_add_mod_list, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tag_cfg_s::unpack(cbit_ref& bref)
{
  bool tag_to_release_list_present;
  HANDLE_CODE(bref.unpack(tag_to_release_list_present, 1));
  bool tag_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(tag_to_add_mod_list_present, 1));

  if (tag_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tag_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (tag_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tag_to_add_mod_list, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void tag_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tag_to_release_list.size() > 0) {
    j.start_array("tag-ToReleaseList");
    for (const auto& e1 : tag_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (tag_to_add_mod_list.size() > 0) {
    j.start_array("tag-ToAddModList");
    for (const auto& e1 : tag_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// TAR-Config-r17 ::= SEQUENCE
SRSASN_CODE tar_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(offset_thres_ta_r17_present, 1));
  HANDLE_CODE(bref.pack(timing_advance_sr_r17_present, 1));

  if (offset_thres_ta_r17_present) {
    HANDLE_CODE(offset_thres_ta_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tar_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(offset_thres_ta_r17_present, 1));
  HANDLE_CODE(bref.unpack(timing_advance_sr_r17_present, 1));

  if (offset_thres_ta_r17_present) {
    HANDLE_CODE(offset_thres_ta_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tar_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (offset_thres_ta_r17_present) {
    j.write_str("offsetThresholdTA-r17", offset_thres_ta_r17.to_string());
  }
  if (timing_advance_sr_r17_present) {
    j.write_str("timingAdvanceSR-r17", "enabled");
  }
  j.end_obj();
}

const char* tar_cfg_r17_s::offset_thres_ta_r17_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms1",     "ms2",     "ms3",     "ms4",    "ms5",    "ms6",    "ms7",
                                "ms8",     "ms9",     "ms10",    "ms11",    "ms12",   "ms13",   "ms14",   "ms15",
                                "spare13", "spare12", "spare11", "spare10", "spare9", "spare8", "spare7", "spare6",
                                "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(names, 29, value, "tar_cfg_r17_s::offset_thres_ta_r17_e_");
}
float tar_cfg_r17_s::offset_thres_ta_r17_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
  return map_enum_number(numbers, 16, value, "tar_cfg_r17_s::offset_thres_ta_r17_e_");
}
const char* tar_cfg_r17_s::offset_thres_ta_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
  return convert_enum_idx(number_strs, 29, value, "tar_cfg_r17_s::offset_thres_ta_r17_e_");
}

// IAB-ResourceConfig-r17 ::= SEQUENCE
SRSASN_CODE iab_res_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slot_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(periodicity_slot_list_r17_present, 1));
  HANDLE_CODE(bref.pack(slot_list_subcarrier_spacing_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, iab_res_cfg_id_r17, (uint32_t)0u, (uint32_t)65535u));
  if (slot_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slot_list_r17, 1, 5120, integer_packer<uint16_t>(0, 5119)));
  }
  if (periodicity_slot_list_r17_present) {
    HANDLE_CODE(periodicity_slot_list_r17.pack(bref));
  }
  if (slot_list_subcarrier_spacing_r17_present) {
    HANDLE_CODE(slot_list_subcarrier_spacing_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_res_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool slot_list_r17_present;
  HANDLE_CODE(bref.unpack(slot_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(periodicity_slot_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(slot_list_subcarrier_spacing_r17_present, 1));

  HANDLE_CODE(unpack_integer(iab_res_cfg_id_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  if (slot_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slot_list_r17, bref, 1, 5120, integer_packer<uint16_t>(0, 5119)));
  }
  if (periodicity_slot_list_r17_present) {
    HANDLE_CODE(periodicity_slot_list_r17.unpack(bref));
  }
  if (slot_list_subcarrier_spacing_r17_present) {
    HANDLE_CODE(slot_list_subcarrier_spacing_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void iab_res_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("iab-ResourceConfigID-r17", iab_res_cfg_id_r17);
  if (slot_list_r17.size() > 0) {
    j.start_array("slotList-r17");
    for (const auto& e1 : slot_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (periodicity_slot_list_r17_present) {
    j.write_str("periodicitySlotList-r17", periodicity_slot_list_r17.to_string());
  }
  if (slot_list_subcarrier_spacing_r17_present) {
    j.write_str("slotListSubcarrierSpacing-r17", slot_list_subcarrier_spacing_r17.to_string());
  }
  j.end_obj();
}

const char* iab_res_cfg_r17_s::periodicity_slot_list_r17_opts::to_string() const
{
  static const char* names[] = {
      "ms0p5", "ms0p625", "ms1", "ms1p25", "ms2", "ms2p5", "ms5", "ms10", "ms20", "ms40", "ms80", "ms160"};
  return convert_enum_idx(names, 12, value, "iab_res_cfg_r17_s::periodicity_slot_list_r17_e_");
}

// MAC-CellGroupConfig ::= SEQUENCE
SRSASN_CODE mac_cell_group_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drx_cfg_present, 1));
  HANDLE_CODE(bref.pack(sched_request_cfg_present, 1));
  HANDLE_CODE(bref.pack(bsr_cfg_present, 1));
  HANDLE_CODE(bref.pack(tag_cfg_present, 1));
  HANDLE_CODE(bref.pack(phr_cfg_present, 1));

  if (drx_cfg_present) {
    HANDLE_CODE(drx_cfg.pack(bref));
  }
  if (sched_request_cfg_present) {
    HANDLE_CODE(sched_request_cfg.pack(bref));
  }
  if (bsr_cfg_present) {
    HANDLE_CODE(bsr_cfg.pack(bref));
  }
  if (tag_cfg_present) {
    HANDLE_CODE(tag_cfg.pack(bref));
  }
  if (phr_cfg_present) {
    HANDLE_CODE(phr_cfg.pack(bref));
  }
  HANDLE_CODE(bref.pack(skip_ul_tx_dyn, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_mask_present;
    group_flags[0] |= data_inactivity_timer.is_present();
    group_flags[1] |= use_pre_bsr_r16_present;
    group_flags[1] |= sched_request_id_lbt_scell_r16_present;
    group_flags[1] |= lch_based_prioritization_r16_present;
    group_flags[1] |= sched_request_id_bfr_scell_r16_present;
    group_flags[1] |= drx_cfg_secondary_group_r16.is_present();
    group_flags[2] |= enhanced_skip_ul_tx_dyn_r16_present;
    group_flags[2] |= enhanced_skip_ul_tx_cfg_r16_present;
    group_flags[3] |= intra_cg_prioritization_r17_present;
    group_flags[3] |= drx_cfg_sl_r17.is_present();
    group_flags[3] |= drx_cfg_ext_v1700.is_present();
    group_flags[3] |= sched_request_id_bfr_r17_present;
    group_flags[3] |= sched_request_id_bfr2_r17_present;
    group_flags[3] |= sched_request_cfg_v1700.is_present();
    group_flags[3] |= tar_cfg_r17.is_present();
    group_flags[3] |= g_rnti_cfg_to_add_mod_list_r17.is_present();
    group_flags[3] |= g_rnti_cfg_to_release_list_r17.is_present();
    group_flags[3] |= g_cs_rnti_cfg_to_add_mod_list_r17.is_present();
    group_flags[3] |= g_cs_rnti_cfg_to_release_list_r17.is_present();
    group_flags[3] |= allow_csi_srs_tx_multicast_drx_active_r17_present;
    group_flags[4] |= sched_request_id_pos_mg_request_r17_present;
    group_flags[4] |= drx_last_tx_ul_r17_present;
    group_flags[5] |= pos_mg_request_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_mask_present, 1));
      HANDLE_CODE(bref.pack(data_inactivity_timer.is_present(), 1));
      if (csi_mask_present) {
        HANDLE_CODE(bref.pack(csi_mask, 1));
      }
      if (data_inactivity_timer.is_present()) {
        HANDLE_CODE(data_inactivity_timer->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(use_pre_bsr_r16_present, 1));
      HANDLE_CODE(bref.pack(sched_request_id_lbt_scell_r16_present, 1));
      HANDLE_CODE(bref.pack(lch_based_prioritization_r16_present, 1));
      HANDLE_CODE(bref.pack(sched_request_id_bfr_scell_r16_present, 1));
      HANDLE_CODE(bref.pack(drx_cfg_secondary_group_r16.is_present(), 1));
      if (sched_request_id_lbt_scell_r16_present) {
        HANDLE_CODE(pack_integer(bref, sched_request_id_lbt_scell_r16, (uint8_t)0u, (uint8_t)7u));
      }
      if (sched_request_id_bfr_scell_r16_present) {
        HANDLE_CODE(pack_integer(bref, sched_request_id_bfr_scell_r16, (uint8_t)0u, (uint8_t)7u));
      }
      if (drx_cfg_secondary_group_r16.is_present()) {
        HANDLE_CODE(drx_cfg_secondary_group_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(enhanced_skip_ul_tx_dyn_r16_present, 1));
      HANDLE_CODE(bref.pack(enhanced_skip_ul_tx_cfg_r16_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_cg_prioritization_r17_present, 1));
      HANDLE_CODE(bref.pack(drx_cfg_sl_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(drx_cfg_ext_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(sched_request_id_bfr_r17_present, 1));
      HANDLE_CODE(bref.pack(sched_request_id_bfr2_r17_present, 1));
      HANDLE_CODE(bref.pack(sched_request_cfg_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(tar_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(g_rnti_cfg_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(g_rnti_cfg_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(g_cs_rnti_cfg_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(g_cs_rnti_cfg_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(allow_csi_srs_tx_multicast_drx_active_r17_present, 1));
      if (drx_cfg_sl_r17.is_present()) {
        HANDLE_CODE(drx_cfg_sl_r17->pack(bref));
      }
      if (drx_cfg_ext_v1700.is_present()) {
        HANDLE_CODE(drx_cfg_ext_v1700->pack(bref));
      }
      if (sched_request_id_bfr_r17_present) {
        HANDLE_CODE(pack_integer(bref, sched_request_id_bfr_r17, (uint8_t)0u, (uint8_t)7u));
      }
      if (sched_request_id_bfr2_r17_present) {
        HANDLE_CODE(pack_integer(bref, sched_request_id_bfr2_r17, (uint8_t)0u, (uint8_t)7u));
      }
      if (sched_request_cfg_v1700.is_present()) {
        HANDLE_CODE(sched_request_cfg_v1700->pack(bref));
      }
      if (tar_cfg_r17.is_present()) {
        HANDLE_CODE(tar_cfg_r17->pack(bref));
      }
      if (g_rnti_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *g_rnti_cfg_to_add_mod_list_r17, 1, 16));
      }
      if (g_rnti_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *g_rnti_cfg_to_release_list_r17, 1, 16, integer_packer<uint8_t>(0, 15)));
      }
      if (g_cs_rnti_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *g_cs_rnti_cfg_to_add_mod_list_r17, 1, 8));
      }
      if (g_cs_rnti_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *g_cs_rnti_cfg_to_release_list_r17, 1, 8, integer_packer<uint8_t>(0, 15)));
      }
      if (allow_csi_srs_tx_multicast_drx_active_r17_present) {
        HANDLE_CODE(bref.pack(allow_csi_srs_tx_multicast_drx_active_r17, 1));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sched_request_id_pos_mg_request_r17_present, 1));
      HANDLE_CODE(bref.pack(drx_last_tx_ul_r17_present, 1));
      if (sched_request_id_pos_mg_request_r17_present) {
        HANDLE_CODE(pack_integer(bref, sched_request_id_pos_mg_request_r17, (uint8_t)0u, (uint8_t)7u));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pos_mg_request_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_cell_group_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drx_cfg_present, 1));
  HANDLE_CODE(bref.unpack(sched_request_cfg_present, 1));
  HANDLE_CODE(bref.unpack(bsr_cfg_present, 1));
  HANDLE_CODE(bref.unpack(tag_cfg_present, 1));
  HANDLE_CODE(bref.unpack(phr_cfg_present, 1));

  if (drx_cfg_present) {
    HANDLE_CODE(drx_cfg.unpack(bref));
  }
  if (sched_request_cfg_present) {
    HANDLE_CODE(sched_request_cfg.unpack(bref));
  }
  if (bsr_cfg_present) {
    HANDLE_CODE(bsr_cfg.unpack(bref));
  }
  if (tag_cfg_present) {
    HANDLE_CODE(tag_cfg.unpack(bref));
  }
  if (phr_cfg_present) {
    HANDLE_CODE(phr_cfg.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(skip_ul_tx_dyn, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(6);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(csi_mask_present, 1));
      unpack_presence_flag(data_inactivity_timer, bref);
      if (csi_mask_present) {
        HANDLE_CODE(bref.unpack(csi_mask, 1));
      }
      if (data_inactivity_timer.is_present()) {
        HANDLE_CODE(data_inactivity_timer->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(use_pre_bsr_r16_present, 1));
      HANDLE_CODE(bref.unpack(sched_request_id_lbt_scell_r16_present, 1));
      HANDLE_CODE(bref.unpack(lch_based_prioritization_r16_present, 1));
      HANDLE_CODE(bref.unpack(sched_request_id_bfr_scell_r16_present, 1));
      unpack_presence_flag(drx_cfg_secondary_group_r16, bref);
      if (sched_request_id_lbt_scell_r16_present) {
        HANDLE_CODE(unpack_integer(sched_request_id_lbt_scell_r16, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (sched_request_id_bfr_scell_r16_present) {
        HANDLE_CODE(unpack_integer(sched_request_id_bfr_scell_r16, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (drx_cfg_secondary_group_r16.is_present()) {
        HANDLE_CODE(drx_cfg_secondary_group_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(enhanced_skip_ul_tx_dyn_r16_present, 1));
      HANDLE_CODE(bref.unpack(enhanced_skip_ul_tx_cfg_r16_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(intra_cg_prioritization_r17_present, 1));
      unpack_presence_flag(drx_cfg_sl_r17, bref);
      unpack_presence_flag(drx_cfg_ext_v1700, bref);
      HANDLE_CODE(bref.unpack(sched_request_id_bfr_r17_present, 1));
      HANDLE_CODE(bref.unpack(sched_request_id_bfr2_r17_present, 1));
      unpack_presence_flag(sched_request_cfg_v1700, bref);
      unpack_presence_flag(tar_cfg_r17, bref);
      unpack_presence_flag(g_rnti_cfg_to_add_mod_list_r17, bref);
      unpack_presence_flag(g_rnti_cfg_to_release_list_r17, bref);
      unpack_presence_flag(g_cs_rnti_cfg_to_add_mod_list_r17, bref);
      unpack_presence_flag(g_cs_rnti_cfg_to_release_list_r17, bref);
      HANDLE_CODE(bref.unpack(allow_csi_srs_tx_multicast_drx_active_r17_present, 1));
      if (drx_cfg_sl_r17.is_present()) {
        HANDLE_CODE(drx_cfg_sl_r17->unpack(bref));
      }
      if (drx_cfg_ext_v1700.is_present()) {
        HANDLE_CODE(drx_cfg_ext_v1700->unpack(bref));
      }
      if (sched_request_id_bfr_r17_present) {
        HANDLE_CODE(unpack_integer(sched_request_id_bfr_r17, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (sched_request_id_bfr2_r17_present) {
        HANDLE_CODE(unpack_integer(sched_request_id_bfr2_r17, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (sched_request_cfg_v1700.is_present()) {
        HANDLE_CODE(sched_request_cfg_v1700->unpack(bref));
      }
      if (tar_cfg_r17.is_present()) {
        HANDLE_CODE(tar_cfg_r17->unpack(bref));
      }
      if (g_rnti_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*g_rnti_cfg_to_add_mod_list_r17, bref, 1, 16));
      }
      if (g_rnti_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*g_rnti_cfg_to_release_list_r17, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
      }
      if (g_cs_rnti_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*g_cs_rnti_cfg_to_add_mod_list_r17, bref, 1, 8));
      }
      if (g_cs_rnti_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*g_cs_rnti_cfg_to_release_list_r17, bref, 1, 8, integer_packer<uint8_t>(0, 15)));
      }
      if (allow_csi_srs_tx_multicast_drx_active_r17_present) {
        HANDLE_CODE(bref.unpack(allow_csi_srs_tx_multicast_drx_active_r17, 1));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sched_request_id_pos_mg_request_r17_present, 1));
      HANDLE_CODE(bref.unpack(drx_last_tx_ul_r17_present, 1));
      if (sched_request_id_pos_mg_request_r17_present) {
        HANDLE_CODE(unpack_integer(sched_request_id_pos_mg_request_r17, bref, (uint8_t)0u, (uint8_t)7u));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pos_mg_request_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void mac_cell_group_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drx_cfg_present) {
    j.write_fieldname("drx-Config");
    drx_cfg.to_json(j);
  }
  if (sched_request_cfg_present) {
    j.write_fieldname("schedulingRequestConfig");
    sched_request_cfg.to_json(j);
  }
  if (bsr_cfg_present) {
    j.write_fieldname("bsr-Config");
    bsr_cfg.to_json(j);
  }
  if (tag_cfg_present) {
    j.write_fieldname("tag-Config");
    tag_cfg.to_json(j);
  }
  if (phr_cfg_present) {
    j.write_fieldname("phr-Config");
    phr_cfg.to_json(j);
  }
  j.write_bool("skipUplinkTxDynamic", skip_ul_tx_dyn);
  if (ext) {
    if (csi_mask_present) {
      j.write_bool("csi-Mask", csi_mask);
    }
    if (data_inactivity_timer.is_present()) {
      j.write_fieldname("dataInactivityTimer");
      data_inactivity_timer->to_json(j);
    }
    if (use_pre_bsr_r16_present) {
      j.write_str("usePreBSR-r16", "true");
    }
    if (sched_request_id_lbt_scell_r16_present) {
      j.write_int("schedulingRequestID-LBT-SCell-r16", sched_request_id_lbt_scell_r16);
    }
    if (lch_based_prioritization_r16_present) {
      j.write_str("lch-BasedPrioritization-r16", "enabled");
    }
    if (sched_request_id_bfr_scell_r16_present) {
      j.write_int("schedulingRequestID-BFR-SCell-r16", sched_request_id_bfr_scell_r16);
    }
    if (drx_cfg_secondary_group_r16.is_present()) {
      j.write_fieldname("drx-ConfigSecondaryGroup-r16");
      drx_cfg_secondary_group_r16->to_json(j);
    }
    if (enhanced_skip_ul_tx_dyn_r16_present) {
      j.write_str("enhancedSkipUplinkTxDynamic-r16", "true");
    }
    if (enhanced_skip_ul_tx_cfg_r16_present) {
      j.write_str("enhancedSkipUplinkTxConfigured-r16", "true");
    }
    if (intra_cg_prioritization_r17_present) {
      j.write_str("intraCG-Prioritization-r17", "enabled");
    }
    if (drx_cfg_sl_r17.is_present()) {
      j.write_fieldname("drx-ConfigSL-r17");
      drx_cfg_sl_r17->to_json(j);
    }
    if (drx_cfg_ext_v1700.is_present()) {
      j.write_fieldname("drx-ConfigExt-v1700");
      drx_cfg_ext_v1700->to_json(j);
    }
    if (sched_request_id_bfr_r17_present) {
      j.write_int("schedulingRequestID-BFR-r17", sched_request_id_bfr_r17);
    }
    if (sched_request_id_bfr2_r17_present) {
      j.write_int("schedulingRequestID-BFR2-r17", sched_request_id_bfr2_r17);
    }
    if (sched_request_cfg_v1700.is_present()) {
      j.write_fieldname("schedulingRequestConfig-v1700");
      sched_request_cfg_v1700->to_json(j);
    }
    if (tar_cfg_r17.is_present()) {
      j.write_fieldname("tar-Config-r17");
      tar_cfg_r17->to_json(j);
    }
    if (g_rnti_cfg_to_add_mod_list_r17.is_present()) {
      j.start_array("g-RNTI-ConfigToAddModList-r17");
      for (const auto& e1 : *g_rnti_cfg_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (g_rnti_cfg_to_release_list_r17.is_present()) {
      j.start_array("g-RNTI-ConfigToReleaseList-r17");
      for (const auto& e1 : *g_rnti_cfg_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (g_cs_rnti_cfg_to_add_mod_list_r17.is_present()) {
      j.start_array("g-CS-RNTI-ConfigToAddModList-r17");
      for (const auto& e1 : *g_cs_rnti_cfg_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (g_cs_rnti_cfg_to_release_list_r17.is_present()) {
      j.start_array("g-CS-RNTI-ConfigToReleaseList-r17");
      for (const auto& e1 : *g_cs_rnti_cfg_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (allow_csi_srs_tx_multicast_drx_active_r17_present) {
      j.write_bool("allowCSI-SRS-Tx-MulticastDRX-Active-r17", allow_csi_srs_tx_multicast_drx_active_r17);
    }
    if (sched_request_id_pos_mg_request_r17_present) {
      j.write_int("schedulingRequestID-PosMG-Request-r17", sched_request_id_pos_mg_request_r17);
    }
    if (drx_last_tx_ul_r17_present) {
      j.write_str("drx-LastTransmissionUL-r17", "enabled");
    }
    if (pos_mg_request_r17_present) {
      j.write_str("posMG-Request-r17", "enabled");
    }
  }
  j.end_obj();
}

// PhysicalCellGroupConfig ::= SEQUENCE
SRSASN_CODE phys_cell_group_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(harq_ack_spatial_bundling_pucch_present, 1));
  HANDLE_CODE(bref.pack(harq_ack_spatial_bundling_pusch_present, 1));
  HANDLE_CODE(bref.pack(p_nr_fr1_present, 1));
  HANDLE_CODE(bref.pack(tpc_srs_rnti_present, 1));
  HANDLE_CODE(bref.pack(tpc_pucch_rnti_present, 1));
  HANDLE_CODE(bref.pack(tpc_pusch_rnti_present, 1));
  HANDLE_CODE(bref.pack(sp_csi_rnti_present, 1));
  HANDLE_CODE(bref.pack(cs_rnti_present, 1));

  if (p_nr_fr1_present) {
    HANDLE_CODE(pack_integer(bref, p_nr_fr1, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pdsch_harq_ack_codebook.pack(bref));
  if (tpc_srs_rnti_present) {
    HANDLE_CODE(pack_integer(bref, tpc_srs_rnti, (uint32_t)0u, (uint32_t)65535u));
  }
  if (tpc_pucch_rnti_present) {
    HANDLE_CODE(pack_integer(bref, tpc_pucch_rnti, (uint32_t)0u, (uint32_t)65535u));
  }
  if (tpc_pusch_rnti_present) {
    HANDLE_CODE(pack_integer(bref, tpc_pusch_rnti, (uint32_t)0u, (uint32_t)65535u));
  }
  if (sp_csi_rnti_present) {
    HANDLE_CODE(pack_integer(bref, sp_csi_rnti, (uint32_t)0u, (uint32_t)65535u));
  }
  if (cs_rnti_present) {
    HANDLE_CODE(cs_rnti.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mcs_c_rnti_present;
    group_flags[0] |= p_ue_fr1_present;
    group_flags[1] |= xscale_present;
    group_flags[2] |= pdcch_blind_detection.is_present();
    group_flags[3] |= dcp_cfg_r16.is_present();
    group_flags[3] |= harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present;
    group_flags[3] |= harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present;
    group_flags[3] |= pdsch_harq_ack_codebook_secondary_pucch_group_r16_present;
    group_flags[3] |= p_nr_fr2_r16_present;
    group_flags[3] |= p_ue_fr2_r16_present;
    group_flags[3] |= nrdc_p_cmode_fr1_r16_present;
    group_flags[3] |= nrdc_p_cmode_fr2_r16_present;
    group_flags[3] |= pdsch_harq_ack_codebook_r16_present;
    group_flags[3] |= nfi_total_dai_included_r16_present;
    group_flags[3] |= ul_total_dai_included_r16_present;
    group_flags[3] |= pdsch_harq_ack_one_shot_feedback_r16_present;
    group_flags[3] |= pdsch_harq_ack_one_shot_feedback_ndi_r16_present;
    group_flags[3] |= pdsch_harq_ack_one_shot_feedback_cbg_r16_present;
    group_flags[3] |= dl_assign_idx_dci_0_2_r16_present;
    group_flags[3] |= dl_assign_idx_dci_1_2_r16_present;
    group_flags[3] |= pdsch_harq_ack_codebook_list_r16.is_present();
    group_flags[3] |= ack_nack_feedback_mode_r16_present;
    group_flags[3] |= pdcch_blind_detection_ca_comb_ind_r16.is_present();
    group_flags[3] |= pdcch_blind_detection2_r16.is_present();
    group_flags[3] |= pdcch_blind_detection3_r16.is_present();
    group_flags[3] |= bd_factor_r_r16_present;
    group_flags[4] |= pdsch_harq_ack_enh_type3_to_add_mod_list_r17.is_present();
    group_flags[4] |= pdsch_harq_ack_enh_type3_to_release_list_r17.is_present();
    group_flags[4] |= pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17.is_present();
    group_flags[4] |= pdsch_harq_ack_enh_type3_secondary_to_release_list_r17.is_present();
    group_flags[4] |= pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present;
    group_flags[4] |= pdsch_harq_ack_enh_type3_dci_field_r17_present;
    group_flags[4] |= pdsch_harq_ack_retx_r17_present;
    group_flags[4] |= pdsch_harq_ack_retx_secondary_pucch_group_r17_present;
    group_flags[4] |= pucch_s_scell_r17_present;
    group_flags[4] |= pucch_s_scell_secondary_pucch_group_r17_present;
    group_flags[4] |= pucch_s_scell_dyn_r17_present;
    group_flags[4] |= pucch_s_scell_dyn_secondary_pucch_group_r17_present;
    group_flags[4] |= pucch_s_scell_pattern_r17.is_present();
    group_flags[4] |= pucch_s_scell_pattern_secondary_pucch_group_r17.is_present();
    group_flags[4] |= uci_mux_with_diff_prio_r17_present;
    group_flags[4] |= uci_mux_with_diff_prio_secondary_pucch_group_r17_present;
    group_flags[4] |= simul_pucch_pusch_r17_present;
    group_flags[4] |= simul_pucch_pusch_secondary_pucch_group_r17_present;
    group_flags[4] |= prio_low_dg_high_cg_r17_present;
    group_flags[4] |= prio_high_dg_low_cg_r17_present;
    group_flags[4] |= two_qcl_type_d_for_pdcch_repeat_r17_present;
    group_flags[4] |= multicast_cfg_r17.is_present();
    group_flags[4] |= pdcch_blind_detection_ca_comb_ind_r17.is_present();
    group_flags[5] |= simul_sr_pusch_diff_pucch_groups_r17_present;
    group_flags[6] |= intra_band_nc_prach_simul_tx_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mcs_c_rnti_present, 1));
      HANDLE_CODE(bref.pack(p_ue_fr1_present, 1));
      if (mcs_c_rnti_present) {
        HANDLE_CODE(pack_integer(bref, mcs_c_rnti, (uint32_t)0u, (uint32_t)65535u));
      }
      if (p_ue_fr1_present) {
        HANDLE_CODE(pack_integer(bref, p_ue_fr1, (int8_t)-30, (int8_t)33));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(xscale_present, 1));
      if (xscale_present) {
        HANDLE_CODE(xscale.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdcch_blind_detection.is_present(), 1));
      if (pdcch_blind_detection.is_present()) {
        HANDLE_CODE(pdcch_blind_detection->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dcp_cfg_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present, 1));
      HANDLE_CODE(bref.pack(harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_codebook_secondary_pucch_group_r16_present, 1));
      HANDLE_CODE(bref.pack(p_nr_fr2_r16_present, 1));
      HANDLE_CODE(bref.pack(p_ue_fr2_r16_present, 1));
      HANDLE_CODE(bref.pack(nrdc_p_cmode_fr1_r16_present, 1));
      HANDLE_CODE(bref.pack(nrdc_p_cmode_fr2_r16_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_codebook_r16_present, 1));
      HANDLE_CODE(bref.pack(nfi_total_dai_included_r16_present, 1));
      HANDLE_CODE(bref.pack(ul_total_dai_included_r16_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_one_shot_feedback_r16_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_one_shot_feedback_ndi_r16_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_one_shot_feedback_cbg_r16_present, 1));
      HANDLE_CODE(bref.pack(dl_assign_idx_dci_0_2_r16_present, 1));
      HANDLE_CODE(bref.pack(dl_assign_idx_dci_1_2_r16_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_codebook_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ack_nack_feedback_mode_r16_present, 1));
      HANDLE_CODE(bref.pack(pdcch_blind_detection_ca_comb_ind_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_blind_detection2_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_blind_detection3_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(bd_factor_r_r16_present, 1));
      if (dcp_cfg_r16.is_present()) {
        HANDLE_CODE(dcp_cfg_r16->pack(bref));
      }
      if (harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present) {
        HANDLE_CODE(harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16.pack(bref));
      }
      if (harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present) {
        HANDLE_CODE(harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16.pack(bref));
      }
      if (pdsch_harq_ack_codebook_secondary_pucch_group_r16_present) {
        HANDLE_CODE(pdsch_harq_ack_codebook_secondary_pucch_group_r16.pack(bref));
      }
      if (p_nr_fr2_r16_present) {
        HANDLE_CODE(pack_integer(bref, p_nr_fr2_r16, (int8_t)-30, (int8_t)33));
      }
      if (p_ue_fr2_r16_present) {
        HANDLE_CODE(pack_integer(bref, p_ue_fr2_r16, (int8_t)-30, (int8_t)33));
      }
      if (nrdc_p_cmode_fr1_r16_present) {
        HANDLE_CODE(nrdc_p_cmode_fr1_r16.pack(bref));
      }
      if (nrdc_p_cmode_fr2_r16_present) {
        HANDLE_CODE(nrdc_p_cmode_fr2_r16.pack(bref));
      }
      if (dl_assign_idx_dci_1_2_r16_present) {
        HANDLE_CODE(dl_assign_idx_dci_1_2_r16.pack(bref));
      }
      if (pdsch_harq_ack_codebook_list_r16.is_present()) {
        HANDLE_CODE(pdsch_harq_ack_codebook_list_r16->pack(bref));
      }
      if (ack_nack_feedback_mode_r16_present) {
        HANDLE_CODE(ack_nack_feedback_mode_r16.pack(bref));
      }
      if (pdcch_blind_detection_ca_comb_ind_r16.is_present()) {
        HANDLE_CODE(pdcch_blind_detection_ca_comb_ind_r16->pack(bref));
      }
      if (pdcch_blind_detection2_r16.is_present()) {
        HANDLE_CODE(pdcch_blind_detection2_r16->pack(bref));
      }
      if (pdcch_blind_detection3_r16.is_present()) {
        HANDLE_CODE(pdcch_blind_detection3_r16->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_secondary_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_enh_type3_dci_field_r17_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_retx_r17_present, 1));
      HANDLE_CODE(bref.pack(pdsch_harq_ack_retx_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.pack(pucch_s_scell_r17_present, 1));
      HANDLE_CODE(bref.pack(pucch_s_scell_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.pack(pucch_s_scell_dyn_r17_present, 1));
      HANDLE_CODE(bref.pack(pucch_s_scell_dyn_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.pack(pucch_s_scell_pattern_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_s_scell_pattern_secondary_pucch_group_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(uci_mux_with_diff_prio_r17_present, 1));
      HANDLE_CODE(bref.pack(uci_mux_with_diff_prio_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.pack(simul_pucch_pusch_r17_present, 1));
      HANDLE_CODE(bref.pack(simul_pucch_pusch_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.pack(prio_low_dg_high_cg_r17_present, 1));
      HANDLE_CODE(bref.pack(prio_high_dg_low_cg_r17_present, 1));
      HANDLE_CODE(bref.pack(two_qcl_type_d_for_pdcch_repeat_r17_present, 1));
      HANDLE_CODE(bref.pack(multicast_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_blind_detection_ca_comb_ind_r17.is_present(), 1));
      if (pdsch_harq_ack_enh_type3_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *pdsch_harq_ack_enh_type3_to_add_mod_list_r17, 1, 8));
      }
      if (pdsch_harq_ack_enh_type3_to_release_list_r17.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *pdsch_harq_ack_enh_type3_to_release_list_r17, 1, 8, integer_packer<uint8_t>(0, 7)));
      }
      if (pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17, 1, 8));
      }
      if (pdsch_harq_ack_enh_type3_secondary_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(
            bref, *pdsch_harq_ack_enh_type3_secondary_to_release_list_r17, 1, 8, integer_packer<uint8_t>(0, 7)));
      }
      if (pucch_s_scell_r17_present) {
        HANDLE_CODE(pack_integer(bref, pucch_s_scell_r17, (uint8_t)1u, (uint8_t)31u));
      }
      if (pucch_s_scell_secondary_pucch_group_r17_present) {
        HANDLE_CODE(pack_integer(bref, pucch_s_scell_secondary_pucch_group_r17, (uint8_t)1u, (uint8_t)31u));
      }
      if (pucch_s_scell_pattern_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *pucch_s_scell_pattern_r17, 1, 320, integer_packer<uint8_t>(0, 1)));
      }
      if (pucch_s_scell_pattern_secondary_pucch_group_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(
            bref, *pucch_s_scell_pattern_secondary_pucch_group_r17, 1, 320, integer_packer<uint8_t>(0, 1)));
      }
      if (multicast_cfg_r17.is_present()) {
        HANDLE_CODE(multicast_cfg_r17->pack(bref));
      }
      if (pdcch_blind_detection_ca_comb_ind_r17.is_present()) {
        HANDLE_CODE(pdcch_blind_detection_ca_comb_ind_r17->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(simul_sr_pusch_diff_pucch_groups_r17_present, 1));
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_band_nc_prach_simul_tx_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cell_group_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(harq_ack_spatial_bundling_pucch_present, 1));
  HANDLE_CODE(bref.unpack(harq_ack_spatial_bundling_pusch_present, 1));
  HANDLE_CODE(bref.unpack(p_nr_fr1_present, 1));
  HANDLE_CODE(bref.unpack(tpc_srs_rnti_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pucch_rnti_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pusch_rnti_present, 1));
  HANDLE_CODE(bref.unpack(sp_csi_rnti_present, 1));
  HANDLE_CODE(bref.unpack(cs_rnti_present, 1));

  if (p_nr_fr1_present) {
    HANDLE_CODE(unpack_integer(p_nr_fr1, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pdsch_harq_ack_codebook.unpack(bref));
  if (tpc_srs_rnti_present) {
    HANDLE_CODE(unpack_integer(tpc_srs_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (tpc_pucch_rnti_present) {
    HANDLE_CODE(unpack_integer(tpc_pucch_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (tpc_pusch_rnti_present) {
    HANDLE_CODE(unpack_integer(tpc_pusch_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (sp_csi_rnti_present) {
    HANDLE_CODE(unpack_integer(sp_csi_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (cs_rnti_present) {
    HANDLE_CODE(cs_rnti.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(mcs_c_rnti_present, 1));
      HANDLE_CODE(bref.unpack(p_ue_fr1_present, 1));
      if (mcs_c_rnti_present) {
        HANDLE_CODE(unpack_integer(mcs_c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
      }
      if (p_ue_fr1_present) {
        HANDLE_CODE(unpack_integer(p_ue_fr1, bref, (int8_t)-30, (int8_t)33));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(xscale_present, 1));
      if (xscale_present) {
        HANDLE_CODE(xscale.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(pdcch_blind_detection, bref);
      if (pdcch_blind_detection.is_present()) {
        HANDLE_CODE(pdcch_blind_detection->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(dcp_cfg_r16, bref);
      HANDLE_CODE(bref.unpack(harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present, 1));
      HANDLE_CODE(bref.unpack(harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_codebook_secondary_pucch_group_r16_present, 1));
      HANDLE_CODE(bref.unpack(p_nr_fr2_r16_present, 1));
      HANDLE_CODE(bref.unpack(p_ue_fr2_r16_present, 1));
      HANDLE_CODE(bref.unpack(nrdc_p_cmode_fr1_r16_present, 1));
      HANDLE_CODE(bref.unpack(nrdc_p_cmode_fr2_r16_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_codebook_r16_present, 1));
      HANDLE_CODE(bref.unpack(nfi_total_dai_included_r16_present, 1));
      HANDLE_CODE(bref.unpack(ul_total_dai_included_r16_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_one_shot_feedback_r16_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_one_shot_feedback_ndi_r16_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_one_shot_feedback_cbg_r16_present, 1));
      HANDLE_CODE(bref.unpack(dl_assign_idx_dci_0_2_r16_present, 1));
      HANDLE_CODE(bref.unpack(dl_assign_idx_dci_1_2_r16_present, 1));
      unpack_presence_flag(pdsch_harq_ack_codebook_list_r16, bref);
      HANDLE_CODE(bref.unpack(ack_nack_feedback_mode_r16_present, 1));
      unpack_presence_flag(pdcch_blind_detection_ca_comb_ind_r16, bref);
      unpack_presence_flag(pdcch_blind_detection2_r16, bref);
      unpack_presence_flag(pdcch_blind_detection3_r16, bref);
      HANDLE_CODE(bref.unpack(bd_factor_r_r16_present, 1));
      if (dcp_cfg_r16.is_present()) {
        HANDLE_CODE(dcp_cfg_r16->unpack(bref));
      }
      if (harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present) {
        HANDLE_CODE(harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16.unpack(bref));
      }
      if (harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present) {
        HANDLE_CODE(harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16.unpack(bref));
      }
      if (pdsch_harq_ack_codebook_secondary_pucch_group_r16_present) {
        HANDLE_CODE(pdsch_harq_ack_codebook_secondary_pucch_group_r16.unpack(bref));
      }
      if (p_nr_fr2_r16_present) {
        HANDLE_CODE(unpack_integer(p_nr_fr2_r16, bref, (int8_t)-30, (int8_t)33));
      }
      if (p_ue_fr2_r16_present) {
        HANDLE_CODE(unpack_integer(p_ue_fr2_r16, bref, (int8_t)-30, (int8_t)33));
      }
      if (nrdc_p_cmode_fr1_r16_present) {
        HANDLE_CODE(nrdc_p_cmode_fr1_r16.unpack(bref));
      }
      if (nrdc_p_cmode_fr2_r16_present) {
        HANDLE_CODE(nrdc_p_cmode_fr2_r16.unpack(bref));
      }
      if (dl_assign_idx_dci_1_2_r16_present) {
        HANDLE_CODE(dl_assign_idx_dci_1_2_r16.unpack(bref));
      }
      if (pdsch_harq_ack_codebook_list_r16.is_present()) {
        HANDLE_CODE(pdsch_harq_ack_codebook_list_r16->unpack(bref));
      }
      if (ack_nack_feedback_mode_r16_present) {
        HANDLE_CODE(ack_nack_feedback_mode_r16.unpack(bref));
      }
      if (pdcch_blind_detection_ca_comb_ind_r16.is_present()) {
        HANDLE_CODE(pdcch_blind_detection_ca_comb_ind_r16->unpack(bref));
      }
      if (pdcch_blind_detection2_r16.is_present()) {
        HANDLE_CODE(pdcch_blind_detection2_r16->unpack(bref));
      }
      if (pdcch_blind_detection3_r16.is_present()) {
        HANDLE_CODE(pdcch_blind_detection3_r16->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(pdsch_harq_ack_enh_type3_to_add_mod_list_r17, bref);
      unpack_presence_flag(pdsch_harq_ack_enh_type3_to_release_list_r17, bref);
      unpack_presence_flag(pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17, bref);
      unpack_presence_flag(pdsch_harq_ack_enh_type3_secondary_to_release_list_r17, bref);
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_enh_type3_dci_field_r17_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_retx_r17_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_harq_ack_retx_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.unpack(pucch_s_scell_r17_present, 1));
      HANDLE_CODE(bref.unpack(pucch_s_scell_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.unpack(pucch_s_scell_dyn_r17_present, 1));
      HANDLE_CODE(bref.unpack(pucch_s_scell_dyn_secondary_pucch_group_r17_present, 1));
      unpack_presence_flag(pucch_s_scell_pattern_r17, bref);
      unpack_presence_flag(pucch_s_scell_pattern_secondary_pucch_group_r17, bref);
      HANDLE_CODE(bref.unpack(uci_mux_with_diff_prio_r17_present, 1));
      HANDLE_CODE(bref.unpack(uci_mux_with_diff_prio_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.unpack(simul_pucch_pusch_r17_present, 1));
      HANDLE_CODE(bref.unpack(simul_pucch_pusch_secondary_pucch_group_r17_present, 1));
      HANDLE_CODE(bref.unpack(prio_low_dg_high_cg_r17_present, 1));
      HANDLE_CODE(bref.unpack(prio_high_dg_low_cg_r17_present, 1));
      HANDLE_CODE(bref.unpack(two_qcl_type_d_for_pdcch_repeat_r17_present, 1));
      unpack_presence_flag(multicast_cfg_r17, bref);
      unpack_presence_flag(pdcch_blind_detection_ca_comb_ind_r17, bref);
      if (pdsch_harq_ack_enh_type3_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*pdsch_harq_ack_enh_type3_to_add_mod_list_r17, bref, 1, 8));
      }
      if (pdsch_harq_ack_enh_type3_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(
            *pdsch_harq_ack_enh_type3_to_release_list_r17, bref, 1, 8, integer_packer<uint8_t>(0, 7)));
      }
      if (pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17, bref, 1, 8));
      }
      if (pdsch_harq_ack_enh_type3_secondary_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(
            *pdsch_harq_ack_enh_type3_secondary_to_release_list_r17, bref, 1, 8, integer_packer<uint8_t>(0, 7)));
      }
      if (pucch_s_scell_r17_present) {
        HANDLE_CODE(unpack_integer(pucch_s_scell_r17, bref, (uint8_t)1u, (uint8_t)31u));
      }
      if (pucch_s_scell_secondary_pucch_group_r17_present) {
        HANDLE_CODE(unpack_integer(pucch_s_scell_secondary_pucch_group_r17, bref, (uint8_t)1u, (uint8_t)31u));
      }
      if (pucch_s_scell_pattern_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*pucch_s_scell_pattern_r17, bref, 1, 320, integer_packer<uint8_t>(0, 1)));
      }
      if (pucch_s_scell_pattern_secondary_pucch_group_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(
            *pucch_s_scell_pattern_secondary_pucch_group_r17, bref, 1, 320, integer_packer<uint8_t>(0, 1)));
      }
      if (multicast_cfg_r17.is_present()) {
        HANDLE_CODE(multicast_cfg_r17->unpack(bref));
      }
      if (pdcch_blind_detection_ca_comb_ind_r17.is_present()) {
        HANDLE_CODE(pdcch_blind_detection_ca_comb_ind_r17->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(simul_sr_pusch_diff_pucch_groups_r17_present, 1));
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(intra_band_nc_prach_simul_tx_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void phys_cell_group_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (harq_ack_spatial_bundling_pucch_present) {
    j.write_str("harq-ACK-SpatialBundlingPUCCH", "true");
  }
  if (harq_ack_spatial_bundling_pusch_present) {
    j.write_str("harq-ACK-SpatialBundlingPUSCH", "true");
  }
  if (p_nr_fr1_present) {
    j.write_int("p-NR-FR1", p_nr_fr1);
  }
  j.write_str("pdsch-HARQ-ACK-Codebook", pdsch_harq_ack_codebook.to_string());
  if (tpc_srs_rnti_present) {
    j.write_int("tpc-SRS-RNTI", tpc_srs_rnti);
  }
  if (tpc_pucch_rnti_present) {
    j.write_int("tpc-PUCCH-RNTI", tpc_pucch_rnti);
  }
  if (tpc_pusch_rnti_present) {
    j.write_int("tpc-PUSCH-RNTI", tpc_pusch_rnti);
  }
  if (sp_csi_rnti_present) {
    j.write_int("sp-CSI-RNTI", sp_csi_rnti);
  }
  if (cs_rnti_present) {
    j.write_fieldname("cs-RNTI");
    cs_rnti.to_json(j);
  }
  if (ext) {
    if (mcs_c_rnti_present) {
      j.write_int("mcs-C-RNTI", mcs_c_rnti);
    }
    if (p_ue_fr1_present) {
      j.write_int("p-UE-FR1", p_ue_fr1);
    }
    if (xscale_present) {
      j.write_str("xScale", xscale.to_string());
    }
    if (pdcch_blind_detection.is_present()) {
      j.write_fieldname("pdcch-BlindDetection");
      pdcch_blind_detection->to_json(j);
    }
    if (dcp_cfg_r16.is_present()) {
      j.write_fieldname("dcp-Config-r16");
      dcp_cfg_r16->to_json(j);
    }
    if (harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present) {
      j.write_str("harq-ACK-SpatialBundlingPUCCH-secondaryPUCCHgroup-r16",
                  harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16.to_string());
    }
    if (harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present) {
      j.write_str("harq-ACK-SpatialBundlingPUSCH-secondaryPUCCHgroup-r16",
                  harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16.to_string());
    }
    if (pdsch_harq_ack_codebook_secondary_pucch_group_r16_present) {
      j.write_str("pdsch-HARQ-ACK-Codebook-secondaryPUCCHgroup-r16",
                  pdsch_harq_ack_codebook_secondary_pucch_group_r16.to_string());
    }
    if (p_nr_fr2_r16_present) {
      j.write_int("p-NR-FR2-r16", p_nr_fr2_r16);
    }
    if (p_ue_fr2_r16_present) {
      j.write_int("p-UE-FR2-r16", p_ue_fr2_r16);
    }
    if (nrdc_p_cmode_fr1_r16_present) {
      j.write_str("nrdc-PCmode-FR1-r16", nrdc_p_cmode_fr1_r16.to_string());
    }
    if (nrdc_p_cmode_fr2_r16_present) {
      j.write_str("nrdc-PCmode-FR2-r16", nrdc_p_cmode_fr2_r16.to_string());
    }
    if (pdsch_harq_ack_codebook_r16_present) {
      j.write_str("pdsch-HARQ-ACK-Codebook-r16", "enhancedDynamic");
    }
    if (nfi_total_dai_included_r16_present) {
      j.write_str("nfi-TotalDAI-Included-r16", "true");
    }
    if (ul_total_dai_included_r16_present) {
      j.write_str("ul-TotalDAI-Included-r16", "true");
    }
    if (pdsch_harq_ack_one_shot_feedback_r16_present) {
      j.write_str("pdsch-HARQ-ACK-OneShotFeedback-r16", "true");
    }
    if (pdsch_harq_ack_one_shot_feedback_ndi_r16_present) {
      j.write_str("pdsch-HARQ-ACK-OneShotFeedbackNDI-r16", "true");
    }
    if (pdsch_harq_ack_one_shot_feedback_cbg_r16_present) {
      j.write_str("pdsch-HARQ-ACK-OneShotFeedbackCBG-r16", "true");
    }
    if (dl_assign_idx_dci_0_2_r16_present) {
      j.write_str("downlinkAssignmentIndexDCI-0-2-r16", "enabled");
    }
    if (dl_assign_idx_dci_1_2_r16_present) {
      j.write_str("downlinkAssignmentIndexDCI-1-2-r16", dl_assign_idx_dci_1_2_r16.to_string());
    }
    if (pdsch_harq_ack_codebook_list_r16.is_present()) {
      j.write_fieldname("pdsch-HARQ-ACK-CodebookList-r16");
      pdsch_harq_ack_codebook_list_r16->to_json(j);
    }
    if (ack_nack_feedback_mode_r16_present) {
      j.write_str("ackNackFeedbackMode-r16", ack_nack_feedback_mode_r16.to_string());
    }
    if (pdcch_blind_detection_ca_comb_ind_r16.is_present()) {
      j.write_fieldname("pdcch-BlindDetectionCA-CombIndicator-r16");
      pdcch_blind_detection_ca_comb_ind_r16->to_json(j);
    }
    if (pdcch_blind_detection2_r16.is_present()) {
      j.write_fieldname("pdcch-BlindDetection2-r16");
      pdcch_blind_detection2_r16->to_json(j);
    }
    if (pdcch_blind_detection3_r16.is_present()) {
      j.write_fieldname("pdcch-BlindDetection3-r16");
      pdcch_blind_detection3_r16->to_json(j);
    }
    if (bd_factor_r_r16_present) {
      j.write_str("bdFactorR-r16", "n1");
    }
    if (pdsch_harq_ack_enh_type3_to_add_mod_list_r17.is_present()) {
      j.start_array("pdsch-HARQ-ACK-EnhType3ToAddModList-r17");
      for (const auto& e1 : *pdsch_harq_ack_enh_type3_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (pdsch_harq_ack_enh_type3_to_release_list_r17.is_present()) {
      j.start_array("pdsch-HARQ-ACK-EnhType3ToReleaseList-r17");
      for (const auto& e1 : *pdsch_harq_ack_enh_type3_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17.is_present()) {
      j.start_array("pdsch-HARQ-ACK-EnhType3SecondaryToAddModList-r17");
      for (const auto& e1 : *pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (pdsch_harq_ack_enh_type3_secondary_to_release_list_r17.is_present()) {
      j.start_array("pdsch-HARQ-ACK-EnhType3SecondaryToReleaseList-r17");
      for (const auto& e1 : *pdsch_harq_ack_enh_type3_secondary_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present) {
      j.write_str("pdsch-HARQ-ACK-EnhType3DCI-FieldSecondaryPUCCHgroup-r17", "enabled");
    }
    if (pdsch_harq_ack_enh_type3_dci_field_r17_present) {
      j.write_str("pdsch-HARQ-ACK-EnhType3DCI-Field-r17", "enabled");
    }
    if (pdsch_harq_ack_retx_r17_present) {
      j.write_str("pdsch-HARQ-ACK-Retx-r17", "enabled");
    }
    if (pdsch_harq_ack_retx_secondary_pucch_group_r17_present) {
      j.write_str("pdsch-HARQ-ACK-RetxSecondaryPUCCHgroup-r17", "enabled");
    }
    if (pucch_s_scell_r17_present) {
      j.write_int("pucch-sSCell-r17", pucch_s_scell_r17);
    }
    if (pucch_s_scell_secondary_pucch_group_r17_present) {
      j.write_int("pucch-sSCellSecondaryPUCCHgroup-r17", pucch_s_scell_secondary_pucch_group_r17);
    }
    if (pucch_s_scell_dyn_r17_present) {
      j.write_str("pucch-sSCellDyn-r17", "enabled");
    }
    if (pucch_s_scell_dyn_secondary_pucch_group_r17_present) {
      j.write_str("pucch-sSCellDynSecondaryPUCCHgroup-r17", "enabled");
    }
    if (pucch_s_scell_pattern_r17.is_present()) {
      j.start_array("pucch-sSCellPattern-r17");
      for (const auto& e1 : *pucch_s_scell_pattern_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (pucch_s_scell_pattern_secondary_pucch_group_r17.is_present()) {
      j.start_array("pucch-sSCellPatternSecondaryPUCCHgroup-r17");
      for (const auto& e1 : *pucch_s_scell_pattern_secondary_pucch_group_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (uci_mux_with_diff_prio_r17_present) {
      j.write_str("uci-MuxWithDiffPrio-r17", "enabled");
    }
    if (uci_mux_with_diff_prio_secondary_pucch_group_r17_present) {
      j.write_str("uci-MuxWithDiffPrioSecondaryPUCCHgroup-r17", "enabled");
    }
    if (simul_pucch_pusch_r17_present) {
      j.write_str("simultaneousPUCCH-PUSCH-r17", "enabled");
    }
    if (simul_pucch_pusch_secondary_pucch_group_r17_present) {
      j.write_str("simultaneousPUCCH-PUSCH-SecondaryPUCCHgroup-r17", "enabled");
    }
    if (prio_low_dg_high_cg_r17_present) {
      j.write_str("prioLowDG-HighCG-r17", "enabled");
    }
    if (prio_high_dg_low_cg_r17_present) {
      j.write_str("prioHighDG-LowCG-r17", "enabled");
    }
    if (two_qcl_type_d_for_pdcch_repeat_r17_present) {
      j.write_str("twoQCLTypeDforPDCCHRepetition-r17", "enabled");
    }
    if (multicast_cfg_r17.is_present()) {
      j.write_fieldname("multicastConfig-r17");
      multicast_cfg_r17->to_json(j);
    }
    if (pdcch_blind_detection_ca_comb_ind_r17.is_present()) {
      j.write_fieldname("pdcch-BlindDetectionCA-CombIndicator-r17");
      pdcch_blind_detection_ca_comb_ind_r17->to_json(j);
    }
    if (simul_sr_pusch_diff_pucch_groups_r17_present) {
      j.write_str("simultaneousSR-PUSCH-diffPUCCH-Groups-r17", "enabled");
    }
    if (intra_band_nc_prach_simul_tx_r17_present) {
      j.write_str("intraBandNC-PRACH-simulTx-r17", "enabled");
    }
  }
  j.end_obj();
}

const char* phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::to_string() const
{
  static const char* names[] = {"semiStatic", "dynamic"};
  return convert_enum_idx(names, 2, value, "phys_cell_group_cfg_s::pdsch_harq_ack_codebook_e_");
}

const char* phys_cell_group_cfg_s::xscale_opts::to_string() const
{
  static const char* names[] = {"dB0", "dB6", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "phys_cell_group_cfg_s::xscale_e_");
}
uint8_t phys_cell_group_cfg_s::xscale_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 6};
  return map_enum_number(numbers, 2, value, "phys_cell_group_cfg_s::xscale_e_");
}

const char* phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(
      names, 2, value, "phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e_");
}

const char* phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(
      names, 2, value, "phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e_");
}

const char* phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16_opts::to_string() const
{
  static const char* names[] = {"semiStatic", "dynamic"};
  return convert_enum_idx(
      names, 2, value, "phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16_e_");
}

const char* phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(names, 3, value, "phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_e_");
}
uint8_t phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_e_");
}

const char* phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(names, 3, value, "phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_e_");
}
uint8_t phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_e_");
}

const char* phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_e_");
}
uint8_t phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_e_");
}

const char* phys_cell_group_cfg_s::ack_nack_feedback_mode_r16_opts::to_string() const
{
  static const char* names[] = {"joint", "separate"};
  return convert_enum_idx(names, 2, value, "phys_cell_group_cfg_s::ack_nack_feedback_mode_r16_e_");
}

// RLC-BearerConfig ::= SEQUENCE
SRSASN_CODE rlc_bearer_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(served_radio_bearer_present, 1));
  HANDLE_CODE(bref.pack(reestablish_rlc_present, 1));
  HANDLE_CODE(bref.pack(rlc_cfg_present, 1));
  HANDLE_CODE(bref.pack(mac_lc_ch_cfg_present, 1));

  HANDLE_CODE(pack_integer(bref, lc_ch_id, (uint8_t)1u, (uint8_t)32u));
  if (served_radio_bearer_present) {
    HANDLE_CODE(served_radio_bearer.pack(bref));
  }
  if (rlc_cfg_present) {
    HANDLE_CODE(rlc_cfg.pack(bref));
  }
  if (mac_lc_ch_cfg_present) {
    HANDLE_CODE(mac_lc_ch_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rlc_cfg_v1610.is_present();
    group_flags[1] |= rlc_cfg_v1700.is_present();
    group_flags[1] |= lc_ch_id_ext_r17_present;
    group_flags[1] |= multicast_rlc_bearer_cfg_r17.is_present();
    group_flags[1] |= served_radio_bearer_srb4_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1610.is_present(), 1));
      if (rlc_cfg_v1610.is_present()) {
        HANDLE_CODE(rlc_cfg_v1610->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(lc_ch_id_ext_r17_present, 1));
      HANDLE_CODE(bref.pack(multicast_rlc_bearer_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(served_radio_bearer_srb4_r17_present, 1));
      if (rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(rlc_cfg_v1700->pack(bref));
      }
      if (lc_ch_id_ext_r17_present) {
        HANDLE_CODE(pack_integer(bref, lc_ch_id_ext_r17, (uint32_t)320u, (uint32_t)65855u));
      }
      if (multicast_rlc_bearer_cfg_r17.is_present()) {
        HANDLE_CODE(multicast_rlc_bearer_cfg_r17->pack(bref));
      }
      if (served_radio_bearer_srb4_r17_present) {
        HANDLE_CODE(pack_integer(bref, served_radio_bearer_srb4_r17, (uint8_t)4u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_bearer_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(served_radio_bearer_present, 1));
  HANDLE_CODE(bref.unpack(reestablish_rlc_present, 1));
  HANDLE_CODE(bref.unpack(rlc_cfg_present, 1));
  HANDLE_CODE(bref.unpack(mac_lc_ch_cfg_present, 1));

  HANDLE_CODE(unpack_integer(lc_ch_id, bref, (uint8_t)1u, (uint8_t)32u));
  if (served_radio_bearer_present) {
    HANDLE_CODE(served_radio_bearer.unpack(bref));
  }
  if (rlc_cfg_present) {
    HANDLE_CODE(rlc_cfg.unpack(bref));
  }
  if (mac_lc_ch_cfg_present) {
    HANDLE_CODE(mac_lc_ch_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(rlc_cfg_v1610, bref);
      if (rlc_cfg_v1610.is_present()) {
        HANDLE_CODE(rlc_cfg_v1610->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(rlc_cfg_v1700, bref);
      HANDLE_CODE(bref.unpack(lc_ch_id_ext_r17_present, 1));
      unpack_presence_flag(multicast_rlc_bearer_cfg_r17, bref);
      HANDLE_CODE(bref.unpack(served_radio_bearer_srb4_r17_present, 1));
      if (rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(rlc_cfg_v1700->unpack(bref));
      }
      if (lc_ch_id_ext_r17_present) {
        HANDLE_CODE(unpack_integer(lc_ch_id_ext_r17, bref, (uint32_t)320u, (uint32_t)65855u));
      }
      if (multicast_rlc_bearer_cfg_r17.is_present()) {
        HANDLE_CODE(multicast_rlc_bearer_cfg_r17->unpack(bref));
      }
      if (served_radio_bearer_srb4_r17_present) {
        HANDLE_CODE(unpack_integer(served_radio_bearer_srb4_r17, bref, (uint8_t)4u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rlc_bearer_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("logicalChannelIdentity", lc_ch_id);
  if (served_radio_bearer_present) {
    j.write_fieldname("servedRadioBearer");
    served_radio_bearer.to_json(j);
  }
  if (reestablish_rlc_present) {
    j.write_str("reestablishRLC", "true");
  }
  if (rlc_cfg_present) {
    j.write_fieldname("rlc-Config");
    rlc_cfg.to_json(j);
  }
  if (mac_lc_ch_cfg_present) {
    j.write_fieldname("mac-LogicalChannelConfig");
    mac_lc_ch_cfg.to_json(j);
  }
  if (ext) {
    if (rlc_cfg_v1610.is_present()) {
      j.write_fieldname("rlc-Config-v1610");
      rlc_cfg_v1610->to_json(j);
    }
    if (rlc_cfg_v1700.is_present()) {
      j.write_fieldname("rlc-Config-v1700");
      rlc_cfg_v1700->to_json(j);
    }
    if (lc_ch_id_ext_r17_present) {
      j.write_int("logicalChannelIdentityExt-r17", lc_ch_id_ext_r17);
    }
    if (multicast_rlc_bearer_cfg_r17.is_present()) {
      j.write_fieldname("multicastRLC-BearerConfig-r17");
      multicast_rlc_bearer_cfg_r17->to_json(j);
    }
    if (served_radio_bearer_srb4_r17_present) {
      j.write_int("servedRadioBearerSRB4-r17", served_radio_bearer_srb4_r17);
    }
  }
  j.end_obj();
}

void rlc_bearer_cfg_s::served_radio_bearer_c_::destroy_() {}
void rlc_bearer_cfg_s::served_radio_bearer_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rlc_bearer_cfg_s::served_radio_bearer_c_::served_radio_bearer_c_(const rlc_bearer_cfg_s::served_radio_bearer_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::srb_id:
      c.init(other.c.get<uint8_t>());
      break;
    case types::drb_id:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_s::served_radio_bearer_c_");
  }
}
rlc_bearer_cfg_s::served_radio_bearer_c_&
rlc_bearer_cfg_s::served_radio_bearer_c_::operator=(const rlc_bearer_cfg_s::served_radio_bearer_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::srb_id:
      c.set(other.c.get<uint8_t>());
      break;
    case types::drb_id:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_s::served_radio_bearer_c_");
  }

  return *this;
}
uint8_t& rlc_bearer_cfg_s::served_radio_bearer_c_::set_srb_id()
{
  set(types::srb_id);
  return c.get<uint8_t>();
}
uint8_t& rlc_bearer_cfg_s::served_radio_bearer_c_::set_drb_id()
{
  set(types::drb_id);
  return c.get<uint8_t>();
}
void rlc_bearer_cfg_s::served_radio_bearer_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::srb_id:
      j.write_int("srb-Identity", c.get<uint8_t>());
      break;
    case types::drb_id:
      j.write_int("drb-Identity", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_s::served_radio_bearer_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlc_bearer_cfg_s::served_radio_bearer_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::srb_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)3u));
      break;
    case types::drb_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)32u));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_s::served_radio_bearer_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_bearer_cfg_s::served_radio_bearer_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::srb_id:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)3u));
      break;
    case types::drb_id:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)32u));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_s::served_radio_bearer_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SCellConfig ::= SEQUENCE
SRSASN_CODE scell_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(scell_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(scell_cfg_ded_present, 1));

  HANDLE_CODE(pack_integer(bref, scell_idx, (uint8_t)1u, (uint8_t)31u));
  if (scell_cfg_common_present) {
    HANDLE_CODE(scell_cfg_common.pack(bref));
  }
  if (scell_cfg_ded_present) {
    HANDLE_CODE(scell_cfg_ded.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= smtc.is_present();
    group_flags[1] |= scell_state_r16_present;
    group_flags[1] |= secondary_drx_group_cfg_r16_present;
    group_flags[2] |= pre_conf_gap_status_r17_present;
    group_flags[2] |= good_serving_cell_eval_bfd_r17.is_present();
    group_flags[2] |= scell_sib20_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc.is_present(), 1));
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_state_r16_present, 1));
      HANDLE_CODE(bref.pack(secondary_drx_group_cfg_r16_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pre_conf_gap_status_r17_present, 1));
      HANDLE_CODE(bref.pack(good_serving_cell_eval_bfd_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(scell_sib20_r17.is_present(), 1));
      if (pre_conf_gap_status_r17_present) {
        HANDLE_CODE(pre_conf_gap_status_r17.pack(bref));
      }
      if (good_serving_cell_eval_bfd_r17.is_present()) {
        HANDLE_CODE(good_serving_cell_eval_bfd_r17->pack(bref));
      }
      if (scell_sib20_r17.is_present()) {
        HANDLE_CODE(scell_sib20_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(scell_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(scell_cfg_ded_present, 1));

  HANDLE_CODE(unpack_integer(scell_idx, bref, (uint8_t)1u, (uint8_t)31u));
  if (scell_cfg_common_present) {
    HANDLE_CODE(scell_cfg_common.unpack(bref));
  }
  if (scell_cfg_ded_present) {
    HANDLE_CODE(scell_cfg_ded.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(smtc, bref);
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(scell_state_r16_present, 1));
      HANDLE_CODE(bref.unpack(secondary_drx_group_cfg_r16_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pre_conf_gap_status_r17_present, 1));
      unpack_presence_flag(good_serving_cell_eval_bfd_r17, bref);
      unpack_presence_flag(scell_sib20_r17, bref);
      if (pre_conf_gap_status_r17_present) {
        HANDLE_CODE(pre_conf_gap_status_r17.unpack(bref));
      }
      if (good_serving_cell_eval_bfd_r17.is_present()) {
        HANDLE_CODE(good_serving_cell_eval_bfd_r17->unpack(bref));
      }
      if (scell_sib20_r17.is_present()) {
        HANDLE_CODE(scell_sib20_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void scell_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sCellIndex", scell_idx);
  if (scell_cfg_common_present) {
    j.write_fieldname("sCellConfigCommon");
    scell_cfg_common.to_json(j);
  }
  if (scell_cfg_ded_present) {
    j.write_fieldname("sCellConfigDedicated");
    scell_cfg_ded.to_json(j);
  }
  if (ext) {
    if (smtc.is_present()) {
      j.write_fieldname("smtc");
      smtc->to_json(j);
    }
    if (scell_state_r16_present) {
      j.write_str("sCellState-r16", "activated");
    }
    if (secondary_drx_group_cfg_r16_present) {
      j.write_str("secondaryDRX-GroupConfig-r16", "true");
    }
    if (pre_conf_gap_status_r17_present) {
      j.write_str("preConfGapStatus-r17", pre_conf_gap_status_r17.to_string());
    }
    if (good_serving_cell_eval_bfd_r17.is_present()) {
      j.write_fieldname("goodServingCellEvaluationBFD-r17");
      good_serving_cell_eval_bfd_r17->to_json(j);
    }
    if (scell_sib20_r17.is_present()) {
      j.write_fieldname("sCellSIB20-r17");
      scell_sib20_r17->to_json(j);
    }
  }
  j.end_obj();
}

// SpCellConfig ::= SEQUENCE
SRSASN_CODE sp_cell_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(serv_cell_idx_present, 1));
  HANDLE_CODE(bref.pack(recfg_with_sync_present, 1));
  HANDLE_CODE(bref.pack(rlf_timers_and_consts_present, 1));
  HANDLE_CODE(bref.pack(rlm_in_sync_out_of_sync_thres_present, 1));
  HANDLE_CODE(bref.pack(sp_cell_cfg_ded_present, 1));

  if (serv_cell_idx_present) {
    HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  }
  if (recfg_with_sync_present) {
    HANDLE_CODE(recfg_with_sync.pack(bref));
  }
  if (rlf_timers_and_consts_present) {
    HANDLE_CODE(rlf_timers_and_consts.pack(bref));
  }
  if (sp_cell_cfg_ded_present) {
    HANDLE_CODE(sp_cell_cfg_ded.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= low_mob_eval_connected_r17.is_present();
    group_flags[0] |= good_serving_cell_eval_rlm_r17.is_present();
    group_flags[0] |= good_serving_cell_eval_bfd_r17.is_present();
    group_flags[0] |= deactiv_scg_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(low_mob_eval_connected_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(good_serving_cell_eval_rlm_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(good_serving_cell_eval_bfd_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(deactiv_scg_cfg_r17.is_present(), 1));
      if (low_mob_eval_connected_r17.is_present()) {
        HANDLE_CODE(low_mob_eval_connected_r17->s_search_delta_p_connected_r17.pack(bref));
        HANDLE_CODE(low_mob_eval_connected_r17->t_search_delta_p_connected_r17.pack(bref));
      }
      if (good_serving_cell_eval_rlm_r17.is_present()) {
        HANDLE_CODE(good_serving_cell_eval_rlm_r17->pack(bref));
      }
      if (good_serving_cell_eval_bfd_r17.is_present()) {
        HANDLE_CODE(good_serving_cell_eval_bfd_r17->pack(bref));
      }
      if (deactiv_scg_cfg_r17.is_present()) {
        HANDLE_CODE(deactiv_scg_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sp_cell_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(serv_cell_idx_present, 1));
  HANDLE_CODE(bref.unpack(recfg_with_sync_present, 1));
  HANDLE_CODE(bref.unpack(rlf_timers_and_consts_present, 1));
  HANDLE_CODE(bref.unpack(rlm_in_sync_out_of_sync_thres_present, 1));
  HANDLE_CODE(bref.unpack(sp_cell_cfg_ded_present, 1));

  if (serv_cell_idx_present) {
    HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (recfg_with_sync_present) {
    HANDLE_CODE(recfg_with_sync.unpack(bref));
  }
  if (rlf_timers_and_consts_present) {
    HANDLE_CODE(rlf_timers_and_consts.unpack(bref));
  }
  if (sp_cell_cfg_ded_present) {
    HANDLE_CODE(sp_cell_cfg_ded.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(low_mob_eval_connected_r17, bref);
      unpack_presence_flag(good_serving_cell_eval_rlm_r17, bref);
      unpack_presence_flag(good_serving_cell_eval_bfd_r17, bref);
      unpack_presence_flag(deactiv_scg_cfg_r17, bref);
      if (low_mob_eval_connected_r17.is_present()) {
        HANDLE_CODE(low_mob_eval_connected_r17->s_search_delta_p_connected_r17.unpack(bref));
        HANDLE_CODE(low_mob_eval_connected_r17->t_search_delta_p_connected_r17.unpack(bref));
      }
      if (good_serving_cell_eval_rlm_r17.is_present()) {
        HANDLE_CODE(good_serving_cell_eval_rlm_r17->unpack(bref));
      }
      if (good_serving_cell_eval_bfd_r17.is_present()) {
        HANDLE_CODE(good_serving_cell_eval_bfd_r17->unpack(bref));
      }
      if (deactiv_scg_cfg_r17.is_present()) {
        HANDLE_CODE(deactiv_scg_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sp_cell_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_cell_idx_present) {
    j.write_int("servCellIndex", serv_cell_idx);
  }
  if (recfg_with_sync_present) {
    j.write_fieldname("reconfigurationWithSync");
    recfg_with_sync.to_json(j);
  }
  if (rlf_timers_and_consts_present) {
    j.write_fieldname("rlf-TimersAndConstants");
    rlf_timers_and_consts.to_json(j);
  }
  if (rlm_in_sync_out_of_sync_thres_present) {
    j.write_str("rlmInSyncOutOfSyncThreshold", "n1");
  }
  if (sp_cell_cfg_ded_present) {
    j.write_fieldname("spCellConfigDedicated");
    sp_cell_cfg_ded.to_json(j);
  }
  if (ext) {
    if (low_mob_eval_connected_r17.is_present()) {
      j.write_fieldname("lowMobilityEvaluationConnected-r17");
      j.start_obj();
      j.write_str("s-SearchDeltaP-Connected-r17",
                  low_mob_eval_connected_r17->s_search_delta_p_connected_r17.to_string());
      j.write_str("t-SearchDeltaP-Connected-r17",
                  low_mob_eval_connected_r17->t_search_delta_p_connected_r17.to_string());
      j.end_obj();
    }
    if (good_serving_cell_eval_rlm_r17.is_present()) {
      j.write_fieldname("goodServingCellEvaluationRLM-r17");
      good_serving_cell_eval_rlm_r17->to_json(j);
    }
    if (good_serving_cell_eval_bfd_r17.is_present()) {
      j.write_fieldname("goodServingCellEvaluationBFD-r17");
      good_serving_cell_eval_bfd_r17->to_json(j);
    }
    if (deactiv_scg_cfg_r17.is_present()) {
      j.write_fieldname("deactivatedSCG-Config-r17");
      deactiv_scg_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_opts::to_string() const
{
  static const char* names[] = {"dB3", "dB6", "dB9", "dB12", "dB15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      names, 8, value, "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_e_");
}
uint8_t sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 5, value, "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_e_");
}

const char* sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_opts::to_string() const
{
  static const char* names[] = {"s5",
                                "s10",
                                "s20",
                                "s30",
                                "s60",
                                "s120",
                                "s180",
                                "s240",
                                "s300",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(
      names, 16, value, "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_e_");
}
uint16_t sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_e_");
}

// Uu-RelayRLC-ChannelConfig-r17 ::= SEQUENCE
SRSASN_CODE uu_relay_rlc_ch_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(uu_lc_ch_id_r17_present, 1));
  HANDLE_CODE(bref.pack(reestablish_rlc_r17_present, 1));
  HANDLE_CODE(bref.pack(rlc_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(mac_lc_ch_cfg_r17_present, 1));

  if (uu_lc_ch_id_r17_present) {
    HANDLE_CODE(pack_integer(bref, uu_lc_ch_id_r17, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(pack_integer(bref, uu_relay_rlc_ch_id_r17, (uint8_t)1u, (uint8_t)32u));
  if (rlc_cfg_r17_present) {
    HANDLE_CODE(rlc_cfg_r17.pack(bref));
  }
  if (mac_lc_ch_cfg_r17_present) {
    HANDLE_CODE(mac_lc_ch_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE uu_relay_rlc_ch_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(uu_lc_ch_id_r17_present, 1));
  HANDLE_CODE(bref.unpack(reestablish_rlc_r17_present, 1));
  HANDLE_CODE(bref.unpack(rlc_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(mac_lc_ch_cfg_r17_present, 1));

  if (uu_lc_ch_id_r17_present) {
    HANDLE_CODE(unpack_integer(uu_lc_ch_id_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(unpack_integer(uu_relay_rlc_ch_id_r17, bref, (uint8_t)1u, (uint8_t)32u));
  if (rlc_cfg_r17_present) {
    HANDLE_CODE(rlc_cfg_r17.unpack(bref));
  }
  if (mac_lc_ch_cfg_r17_present) {
    HANDLE_CODE(mac_lc_ch_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void uu_relay_rlc_ch_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (uu_lc_ch_id_r17_present) {
    j.write_int("uu-LogicalChannelIdentity-r17", uu_lc_ch_id_r17);
  }
  j.write_int("uu-RelayRLC-ChannelID-r17", uu_relay_rlc_ch_id_r17);
  if (reestablish_rlc_r17_present) {
    j.write_str("reestablishRLC-r17", "true");
  }
  if (rlc_cfg_r17_present) {
    j.write_fieldname("rlc-Config-r17");
    rlc_cfg_r17.to_json(j);
  }
  if (mac_lc_ch_cfg_r17_present) {
    j.write_fieldname("mac-LogicalChannelConfig-r17");
    mac_lc_ch_cfg_r17.to_json(j);
  }
  j.end_obj();
}

// CellGroupConfig ::= SEQUENCE
SRSASN_CODE cell_group_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rlc_bearer_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(rlc_bearer_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(mac_cell_group_cfg_present, 1));
  HANDLE_CODE(bref.pack(phys_cell_group_cfg_present, 1));
  HANDLE_CODE(bref.pack(sp_cell_cfg_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(scell_to_release_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, cell_group_id, (uint8_t)0u, (uint8_t)3u));
  if (rlc_bearer_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rlc_bearer_to_add_mod_list, 1, 32));
  }
  if (rlc_bearer_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rlc_bearer_to_release_list, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (mac_cell_group_cfg_present) {
    HANDLE_CODE(mac_cell_group_cfg.pack(bref));
  }
  if (phys_cell_group_cfg_present) {
    HANDLE_CODE(phys_cell_group_cfg.pack(bref));
  }
  if (sp_cell_cfg_present) {
    HANDLE_CODE(sp_cell_cfg.pack(bref));
  }
  if (scell_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list, 1, 31));
  }
  if (scell_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_release_list, 1, 31, integer_packer<uint8_t>(1, 31)));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= report_ul_tx_direct_current_present;
    group_flags[1] |= bap_address_r16_present;
    group_flags[1] |= bh_rlc_ch_to_add_mod_list_r16.is_present();
    group_flags[1] |= bh_rlc_ch_to_release_list_r16.is_present();
    group_flags[1] |= f1c_transfer_path_r16_present;
    group_flags[1] |= simul_tci_upd_list1_r16.is_present();
    group_flags[1] |= simul_tci_upd_list2_r16.is_present();
    group_flags[1] |= simul_spatial_upd_list1_r16.is_present();
    group_flags[1] |= simul_spatial_upd_list2_r16.is_present();
    group_flags[1] |= ul_tx_switching_option_r16_present;
    group_flags[1] |= ul_tx_switching_pwr_boosting_r16_present;
    group_flags[2] |= report_ul_tx_direct_current_two_carrier_r16_present;
    group_flags[3] |= f1c_transfer_path_nrdc_r17_present;
    group_flags[3] |= ul_tx_switching_2_t_mode_r17_present;
    group_flags[3] |= ul_tx_switching_dual_ul_tx_state_r17_present;
    group_flags[3] |= uu_relay_rlc_ch_to_add_mod_list_r17.is_present();
    group_flags[3] |= uu_relay_rlc_ch_to_release_list_r17.is_present();
    group_flags[3] |= simul_u_tci_upd_list1_r17.is_present();
    group_flags[3] |= simul_u_tci_upd_list2_r17.is_present();
    group_flags[3] |= simul_u_tci_upd_list3_r17.is_present();
    group_flags[3] |= simul_u_tci_upd_list4_r17.is_present();
    group_flags[3] |= rlc_bearer_to_release_list_ext_r17.is_present();
    group_flags[3] |= iab_res_cfg_to_add_mod_list_r17.is_present();
    group_flags[3] |= iab_res_cfg_to_release_list_r17.is_present();
    group_flags[4] |= report_ul_tx_direct_current_more_carrier_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_ul_tx_direct_current_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(bap_address_r16_present, 1));
      HANDLE_CODE(bref.pack(bh_rlc_ch_to_add_mod_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(bh_rlc_ch_to_release_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(f1c_transfer_path_r16_present, 1));
      HANDLE_CODE(bref.pack(simul_tci_upd_list1_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(simul_tci_upd_list2_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(simul_spatial_upd_list1_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(simul_spatial_upd_list2_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_tx_switching_option_r16_present, 1));
      HANDLE_CODE(bref.pack(ul_tx_switching_pwr_boosting_r16_present, 1));
      if (bap_address_r16_present) {
        HANDLE_CODE(bap_address_r16.pack(bref));
      }
      if (bh_rlc_ch_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *bh_rlc_ch_to_add_mod_list_r16, 1, 65536));
      }
      if (bh_rlc_ch_to_release_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *bh_rlc_ch_to_release_list_r16, 1, 65536));
      }
      if (f1c_transfer_path_r16_present) {
        HANDLE_CODE(f1c_transfer_path_r16.pack(bref));
      }
      if (simul_tci_upd_list1_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_tci_upd_list1_r16, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_tci_upd_list2_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_tci_upd_list2_r16, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_spatial_upd_list1_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_spatial_upd_list1_r16, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_spatial_upd_list2_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_spatial_upd_list2_r16, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (ul_tx_switching_option_r16_present) {
        HANDLE_CODE(ul_tx_switching_option_r16.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_ul_tx_direct_current_two_carrier_r16_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(f1c_transfer_path_nrdc_r17_present, 1));
      HANDLE_CODE(bref.pack(ul_tx_switching_2_t_mode_r17_present, 1));
      HANDLE_CODE(bref.pack(ul_tx_switching_dual_ul_tx_state_r17_present, 1));
      HANDLE_CODE(bref.pack(uu_relay_rlc_ch_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(uu_relay_rlc_ch_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(simul_u_tci_upd_list1_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(simul_u_tci_upd_list2_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(simul_u_tci_upd_list3_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(simul_u_tci_upd_list4_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(rlc_bearer_to_release_list_ext_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(iab_res_cfg_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(iab_res_cfg_to_release_list_r17.is_present(), 1));
      if (f1c_transfer_path_nrdc_r17_present) {
        HANDLE_CODE(f1c_transfer_path_nrdc_r17.pack(bref));
      }
      if (ul_tx_switching_dual_ul_tx_state_r17_present) {
        HANDLE_CODE(ul_tx_switching_dual_ul_tx_state_r17.pack(bref));
      }
      if (uu_relay_rlc_ch_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *uu_relay_rlc_ch_to_add_mod_list_r17, 1, 32));
      }
      if (uu_relay_rlc_ch_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *uu_relay_rlc_ch_to_release_list_r17, 1, 32, integer_packer<uint8_t>(1, 32)));
      }
      if (simul_u_tci_upd_list1_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_u_tci_upd_list1_r17, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_u_tci_upd_list2_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_u_tci_upd_list2_r17, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_u_tci_upd_list3_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_u_tci_upd_list3_r17, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_u_tci_upd_list4_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *simul_u_tci_upd_list4_r17, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (rlc_bearer_to_release_list_ext_r17.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *rlc_bearer_to_release_list_ext_r17, 1, 32, integer_packer<uint32_t>(320, 65855)));
      }
      if (iab_res_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *iab_res_cfg_to_add_mod_list_r17, 1, 65536));
      }
      if (iab_res_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *iab_res_cfg_to_release_list_r17, 1, 65536, integer_packer<uint32_t>(0, 65535)));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_ul_tx_direct_current_more_carrier_r17.is_present(), 1));
      if (report_ul_tx_direct_current_more_carrier_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *report_ul_tx_direct_current_more_carrier_r17, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_group_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool rlc_bearer_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(rlc_bearer_to_add_mod_list_present, 1));
  bool rlc_bearer_to_release_list_present;
  HANDLE_CODE(bref.unpack(rlc_bearer_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(mac_cell_group_cfg_present, 1));
  HANDLE_CODE(bref.unpack(phys_cell_group_cfg_present, 1));
  HANDLE_CODE(bref.unpack(sp_cell_cfg_present, 1));
  bool scell_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_present, 1));
  bool scell_to_release_list_present;
  HANDLE_CODE(bref.unpack(scell_to_release_list_present, 1));

  HANDLE_CODE(unpack_integer(cell_group_id, bref, (uint8_t)0u, (uint8_t)3u));
  if (rlc_bearer_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rlc_bearer_to_add_mod_list, bref, 1, 32));
  }
  if (rlc_bearer_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rlc_bearer_to_release_list, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (mac_cell_group_cfg_present) {
    HANDLE_CODE(mac_cell_group_cfg.unpack(bref));
  }
  if (phys_cell_group_cfg_present) {
    HANDLE_CODE(phys_cell_group_cfg.unpack(bref));
  }
  if (sp_cell_cfg_present) {
    HANDLE_CODE(sp_cell_cfg.unpack(bref));
  }
  if (scell_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list, bref, 1, 31));
  }
  if (scell_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_release_list, bref, 1, 31, integer_packer<uint8_t>(1, 31)));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_ul_tx_direct_current_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(bap_address_r16_present, 1));
      unpack_presence_flag(bh_rlc_ch_to_add_mod_list_r16, bref);
      unpack_presence_flag(bh_rlc_ch_to_release_list_r16, bref);
      HANDLE_CODE(bref.unpack(f1c_transfer_path_r16_present, 1));
      unpack_presence_flag(simul_tci_upd_list1_r16, bref);
      unpack_presence_flag(simul_tci_upd_list2_r16, bref);
      unpack_presence_flag(simul_spatial_upd_list1_r16, bref);
      unpack_presence_flag(simul_spatial_upd_list2_r16, bref);
      HANDLE_CODE(bref.unpack(ul_tx_switching_option_r16_present, 1));
      HANDLE_CODE(bref.unpack(ul_tx_switching_pwr_boosting_r16_present, 1));
      if (bap_address_r16_present) {
        HANDLE_CODE(bap_address_r16.unpack(bref));
      }
      if (bh_rlc_ch_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*bh_rlc_ch_to_add_mod_list_r16, bref, 1, 65536));
      }
      if (bh_rlc_ch_to_release_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*bh_rlc_ch_to_release_list_r16, bref, 1, 65536));
      }
      if (f1c_transfer_path_r16_present) {
        HANDLE_CODE(f1c_transfer_path_r16.unpack(bref));
      }
      if (simul_tci_upd_list1_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_tci_upd_list1_r16, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_tci_upd_list2_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_tci_upd_list2_r16, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_spatial_upd_list1_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_spatial_upd_list1_r16, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_spatial_upd_list2_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_spatial_upd_list2_r16, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (ul_tx_switching_option_r16_present) {
        HANDLE_CODE(ul_tx_switching_option_r16.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_ul_tx_direct_current_two_carrier_r16_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(f1c_transfer_path_nrdc_r17_present, 1));
      HANDLE_CODE(bref.unpack(ul_tx_switching_2_t_mode_r17_present, 1));
      HANDLE_CODE(bref.unpack(ul_tx_switching_dual_ul_tx_state_r17_present, 1));
      unpack_presence_flag(uu_relay_rlc_ch_to_add_mod_list_r17, bref);
      unpack_presence_flag(uu_relay_rlc_ch_to_release_list_r17, bref);
      unpack_presence_flag(simul_u_tci_upd_list1_r17, bref);
      unpack_presence_flag(simul_u_tci_upd_list2_r17, bref);
      unpack_presence_flag(simul_u_tci_upd_list3_r17, bref);
      unpack_presence_flag(simul_u_tci_upd_list4_r17, bref);
      unpack_presence_flag(rlc_bearer_to_release_list_ext_r17, bref);
      unpack_presence_flag(iab_res_cfg_to_add_mod_list_r17, bref);
      unpack_presence_flag(iab_res_cfg_to_release_list_r17, bref);
      if (f1c_transfer_path_nrdc_r17_present) {
        HANDLE_CODE(f1c_transfer_path_nrdc_r17.unpack(bref));
      }
      if (ul_tx_switching_dual_ul_tx_state_r17_present) {
        HANDLE_CODE(ul_tx_switching_dual_ul_tx_state_r17.unpack(bref));
      }
      if (uu_relay_rlc_ch_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*uu_relay_rlc_ch_to_add_mod_list_r17, bref, 1, 32));
      }
      if (uu_relay_rlc_ch_to_release_list_r17.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*uu_relay_rlc_ch_to_release_list_r17, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
      }
      if (simul_u_tci_upd_list1_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_u_tci_upd_list1_r17, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_u_tci_upd_list2_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_u_tci_upd_list2_r17, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_u_tci_upd_list3_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_u_tci_upd_list3_r17, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (simul_u_tci_upd_list4_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*simul_u_tci_upd_list4_r17, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
      }
      if (rlc_bearer_to_release_list_ext_r17.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*rlc_bearer_to_release_list_ext_r17, bref, 1, 32, integer_packer<uint32_t>(320, 65855)));
      }
      if (iab_res_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*iab_res_cfg_to_add_mod_list_r17, bref, 1, 65536));
      }
      if (iab_res_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*iab_res_cfg_to_release_list_r17, bref, 1, 65536, integer_packer<uint32_t>(0, 65535)));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(report_ul_tx_direct_current_more_carrier_r17, bref);
      if (report_ul_tx_direct_current_more_carrier_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*report_ul_tx_direct_current_more_carrier_r17, bref, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cell_group_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellGroupId", cell_group_id);
  if (rlc_bearer_to_add_mod_list.size() > 0) {
    j.start_array("rlc-BearerToAddModList");
    for (const auto& e1 : rlc_bearer_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rlc_bearer_to_release_list.size() > 0) {
    j.start_array("rlc-BearerToReleaseList");
    for (const auto& e1 : rlc_bearer_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mac_cell_group_cfg_present) {
    j.write_fieldname("mac-CellGroupConfig");
    mac_cell_group_cfg.to_json(j);
  }
  if (phys_cell_group_cfg_present) {
    j.write_fieldname("physicalCellGroupConfig");
    phys_cell_group_cfg.to_json(j);
  }
  if (sp_cell_cfg_present) {
    j.write_fieldname("spCellConfig");
    sp_cell_cfg.to_json(j);
  }
  if (scell_to_add_mod_list.size() > 0) {
    j.start_array("sCellToAddModList");
    for (const auto& e1 : scell_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_to_release_list.size() > 0) {
    j.start_array("sCellToReleaseList");
    for (const auto& e1 : scell_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (ext) {
    if (report_ul_tx_direct_current_present) {
      j.write_str("reportUplinkTxDirectCurrent", "true");
    }
    if (bap_address_r16_present) {
      j.write_str("bap-Address-r16", bap_address_r16.to_string());
    }
    if (bh_rlc_ch_to_add_mod_list_r16.is_present()) {
      j.start_array("bh-RLC-ChannelToAddModList-r16");
      for (const auto& e1 : *bh_rlc_ch_to_add_mod_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (bh_rlc_ch_to_release_list_r16.is_present()) {
      j.start_array("bh-RLC-ChannelToReleaseList-r16");
      for (const auto& e1 : *bh_rlc_ch_to_release_list_r16) {
        j.write_str(e1.to_string());
      }
      j.end_array();
    }
    if (f1c_transfer_path_r16_present) {
      j.write_str("f1c-TransferPath-r16", f1c_transfer_path_r16.to_string());
    }
    if (simul_tci_upd_list1_r16.is_present()) {
      j.start_array("simultaneousTCI-UpdateList1-r16");
      for (const auto& e1 : *simul_tci_upd_list1_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (simul_tci_upd_list2_r16.is_present()) {
      j.start_array("simultaneousTCI-UpdateList2-r16");
      for (const auto& e1 : *simul_tci_upd_list2_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (simul_spatial_upd_list1_r16.is_present()) {
      j.start_array("simultaneousSpatial-UpdatedList1-r16");
      for (const auto& e1 : *simul_spatial_upd_list1_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (simul_spatial_upd_list2_r16.is_present()) {
      j.start_array("simultaneousSpatial-UpdatedList2-r16");
      for (const auto& e1 : *simul_spatial_upd_list2_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (ul_tx_switching_option_r16_present) {
      j.write_str("uplinkTxSwitchingOption-r16", ul_tx_switching_option_r16.to_string());
    }
    if (ul_tx_switching_pwr_boosting_r16_present) {
      j.write_str("uplinkTxSwitchingPowerBoosting-r16", "enabled");
    }
    if (report_ul_tx_direct_current_two_carrier_r16_present) {
      j.write_str("reportUplinkTxDirectCurrentTwoCarrier-r16", "true");
    }
    if (f1c_transfer_path_nrdc_r17_present) {
      j.write_str("f1c-TransferPathNRDC-r17", f1c_transfer_path_nrdc_r17.to_string());
    }
    if (ul_tx_switching_2_t_mode_r17_present) {
      j.write_str("uplinkTxSwitching-2T-Mode-r17", "enabled");
    }
    if (ul_tx_switching_dual_ul_tx_state_r17_present) {
      j.write_str("uplinkTxSwitching-DualUL-TxState-r17", ul_tx_switching_dual_ul_tx_state_r17.to_string());
    }
    if (uu_relay_rlc_ch_to_add_mod_list_r17.is_present()) {
      j.start_array("uu-RelayRLC-ChannelToAddModList-r17");
      for (const auto& e1 : *uu_relay_rlc_ch_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (uu_relay_rlc_ch_to_release_list_r17.is_present()) {
      j.start_array("uu-RelayRLC-ChannelToReleaseList-r17");
      for (const auto& e1 : *uu_relay_rlc_ch_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (simul_u_tci_upd_list1_r17.is_present()) {
      j.start_array("simultaneousU-TCI-UpdateList1-r17");
      for (const auto& e1 : *simul_u_tci_upd_list1_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (simul_u_tci_upd_list2_r17.is_present()) {
      j.start_array("simultaneousU-TCI-UpdateList2-r17");
      for (const auto& e1 : *simul_u_tci_upd_list2_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (simul_u_tci_upd_list3_r17.is_present()) {
      j.start_array("simultaneousU-TCI-UpdateList3-r17");
      for (const auto& e1 : *simul_u_tci_upd_list3_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (simul_u_tci_upd_list4_r17.is_present()) {
      j.start_array("simultaneousU-TCI-UpdateList4-r17");
      for (const auto& e1 : *simul_u_tci_upd_list4_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (rlc_bearer_to_release_list_ext_r17.is_present()) {
      j.start_array("rlc-BearerToReleaseListExt-r17");
      for (const auto& e1 : *rlc_bearer_to_release_list_ext_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (iab_res_cfg_to_add_mod_list_r17.is_present()) {
      j.start_array("iab-ResourceConfigToAddModList-r17");
      for (const auto& e1 : *iab_res_cfg_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (iab_res_cfg_to_release_list_r17.is_present()) {
      j.start_array("iab-ResourceConfigToReleaseList-r17");
      for (const auto& e1 : *iab_res_cfg_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (report_ul_tx_direct_current_more_carrier_r17.is_present()) {
      j.start_array("reportUplinkTxDirectCurrentMoreCarrier-r17");
      for (const auto& e1 : *report_ul_tx_direct_current_more_carrier_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* cell_group_cfg_s::f1c_transfer_path_r16_opts::to_string() const
{
  static const char* names[] = {"lte", "nr", "both"};
  return convert_enum_idx(names, 3, value, "cell_group_cfg_s::f1c_transfer_path_r16_e_");
}

const char* cell_group_cfg_s::ul_tx_switching_option_r16_opts::to_string() const
{
  static const char* names[] = {"switchedUL", "dualUL"};
  return convert_enum_idx(names, 2, value, "cell_group_cfg_s::ul_tx_switching_option_r16_e_");
}

const char* cell_group_cfg_s::f1c_transfer_path_nrdc_r17_opts::to_string() const
{
  static const char* names[] = {"mcg", "scg", "both"};
  return convert_enum_idx(names, 3, value, "cell_group_cfg_s::f1c_transfer_path_nrdc_r17_e_");
}

const char* cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_opts::to_string() const
{
  static const char* names[] = {"oneT", "twoT"};
  return convert_enum_idx(names, 2, value, "cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_e_");
}
uint8_t cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_e_");
}

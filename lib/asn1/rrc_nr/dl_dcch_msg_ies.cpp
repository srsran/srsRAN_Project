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

#include "srsran/asn1/rrc_nr/dl_dcch_msg_ies.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

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

const char* mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms20-r17",
                                "ms40-r17",
                                "ms80-r17",
                                "ms160-r17",
                                "ms320-r17",
                                "ms640-r17",
                                "ms1280-r17",
                                "ms2560-r17",
                                "ms5120-r17"};
  return convert_enum_idx(names, 9, value, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types");
}
uint16_t mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(numbers, 9, value, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types");
}

// NeighbourCellInfo-r17 ::= SEQUENCE
SRSASN_CODE neighbour_cell_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(epoch_time_r17.pack(bref));
  HANDLE_CODE(ephemeris_info_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neighbour_cell_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(epoch_time_r17.unpack(bref));
  HANDLE_CODE(ephemeris_info_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void neighbour_cell_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("epochTime-r17");
  epoch_time_r17.to_json(j);
  j.write_fieldname("ephemerisInfo-r17");
  ephemeris_info_r17.to_json(j);
  j.end_obj();
}

// RAN-VisibleParameters-r17 ::= SEQUENCE
SRSASN_CODE ran_visible_params_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ran_visible_periodicity_r17_present, 1));
  HANDLE_CODE(bref.pack(nof_buffer_level_entries_r17_present, 1));
  HANDLE_CODE(bref.pack(report_playout_delay_for_media_startup_r17_present, 1));

  if (ran_visible_periodicity_r17_present) {
    HANDLE_CODE(ran_visible_periodicity_r17.pack(bref));
  }
  if (nof_buffer_level_entries_r17_present) {
    HANDLE_CODE(pack_integer(bref, nof_buffer_level_entries_r17, (uint8_t)1u, (uint8_t)8u));
  }
  if (report_playout_delay_for_media_startup_r17_present) {
    HANDLE_CODE(bref.pack(report_playout_delay_for_media_startup_r17, 1));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_visible_params_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ran_visible_periodicity_r17_present, 1));
  HANDLE_CODE(bref.unpack(nof_buffer_level_entries_r17_present, 1));
  HANDLE_CODE(bref.unpack(report_playout_delay_for_media_startup_r17_present, 1));

  if (ran_visible_periodicity_r17_present) {
    HANDLE_CODE(ran_visible_periodicity_r17.unpack(bref));
  }
  if (nof_buffer_level_entries_r17_present) {
    HANDLE_CODE(unpack_integer(nof_buffer_level_entries_r17, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (report_playout_delay_for_media_startup_r17_present) {
    HANDLE_CODE(bref.unpack(report_playout_delay_for_media_startup_r17, 1));
  }

  return SRSASN_SUCCESS;
}
void ran_visible_params_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ran_visible_periodicity_r17_present) {
    j.write_str("ran-VisiblePeriodicity-r17", ran_visible_periodicity_r17.to_string());
  }
  if (nof_buffer_level_entries_r17_present) {
    j.write_int("numberOfBufferLevelEntries-r17", nof_buffer_level_entries_r17);
  }
  if (report_playout_delay_for_media_startup_r17_present) {
    j.write_bool("reportPlayoutDelayForMediaStartup-r17", report_playout_delay_for_media_startup_r17);
  }
  j.end_obj();
}

const char* ran_visible_params_r17_s::ran_visible_periodicity_r17_opts::to_string() const
{
  static const char* names[] = {"ms120", "ms240", "ms480", "ms640", "ms1024"};
  return convert_enum_idx(names, 5, value, "ran_visible_params_r17_s::ran_visible_periodicity_r17_e_");
}
uint16_t ran_visible_params_r17_s::ran_visible_periodicity_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {120, 240, 480, 640, 1024};
  return map_enum_number(numbers, 5, value, "ran_visible_params_r17_s::ran_visible_periodicity_r17_e_");
}

// BFD-RelaxationReportingConfig-r17 ::= SEQUENCE
SRSASN_CODE bfd_relaxation_report_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bfd_relaxtion_report_prohibit_timer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bfd_relaxation_report_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bfd_relaxtion_report_prohibit_timer.unpack(bref));

  return SRSASN_SUCCESS;
}
void bfd_relaxation_report_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bfd-RelaxtionReportingProhibitTimer", bfd_relaxtion_report_prohibit_timer.to_string());
  j.end_obj();
}

const char* bfd_relaxation_report_cfg_r17_s::bfd_relaxtion_report_prohibit_timer_opts::to_string() const
{
  static const char* names[] = {"s0",
                                "s0dot5",
                                "s1",
                                "s2",
                                "s5",
                                "s10",
                                "s20",
                                "s30",
                                "s60",
                                "s90",
                                "s120",
                                "s300",
                                "s600",
                                "infinity",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "bfd_relaxation_report_cfg_r17_s::bfd_relaxtion_report_prohibit_timer_e_");
}
float bfd_relaxation_report_cfg_r17_s::bfd_relaxtion_report_prohibit_timer_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0, -1.0};
  return map_enum_number(numbers, 14, value, "bfd_relaxation_report_cfg_r17_s::bfd_relaxtion_report_prohibit_timer_e_");
}
const char* bfd_relaxation_report_cfg_r17_s::bfd_relaxtion_report_prohibit_timer_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600", "inf"};
  return convert_enum_idx(
      number_strs, 16, value, "bfd_relaxation_report_cfg_r17_s::bfd_relaxtion_report_prohibit_timer_e_");
}

// MUSIM-GapAssistanceConfig-r17 ::= SEQUENCE
SRSASN_CODE mu_si_m_gap_assist_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(musim_gap_prohibit_timer_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_gap_assist_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(musim_gap_prohibit_timer_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mu_si_m_gap_assist_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("musim-GapProhibitTimer-r17", musim_gap_prohibit_timer_r17.to_string());
  j.end_obj();
}

const char* mu_si_m_gap_assist_cfg_r17_s::musim_gap_prohibit_timer_r17_opts::to_string() const
{
  static const char* names[] = {"s0",
                                "s0dot1",
                                "s0dot2",
                                "s0dot3",
                                "s0dot4",
                                "s0dot5",
                                "s1",
                                "s2",
                                "s3",
                                "s4",
                                "s5",
                                "s6",
                                "s7",
                                "s8",
                                "s9",
                                "s10"};
  return convert_enum_idx(names, 16, value, "mu_si_m_gap_assist_cfg_r17_s::musim_gap_prohibit_timer_r17_e_");
}
float mu_si_m_gap_assist_cfg_r17_s::musim_gap_prohibit_timer_r17_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  return map_enum_number(numbers, 16, value, "mu_si_m_gap_assist_cfg_r17_s::musim_gap_prohibit_timer_r17_e_");
}
const char* mu_si_m_gap_assist_cfg_r17_s::musim_gap_prohibit_timer_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0", "0.1", "0.2", "0.3", "0.4", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
  return convert_enum_idx(number_strs, 16, value, "mu_si_m_gap_assist_cfg_r17_s::musim_gap_prohibit_timer_r17_e_");
}

// MUSIM-Gap-r17 ::= SEQUENCE
SRSASN_CODE mu_si_m_gap_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, musim_gap_id_r17, (uint8_t)0u, (uint8_t)2u));
  HANDLE_CODE(musim_gap_info_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_gap_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(musim_gap_id_r17, bref, (uint8_t)0u, (uint8_t)2u));
  HANDLE_CODE(musim_gap_info_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mu_si_m_gap_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("musim-GapId-r17", musim_gap_id_r17);
  j.write_fieldname("musim-GapInfo-r17");
  musim_gap_info_r17.to_json(j);
  j.end_obj();
}

// MUSIM-LeaveAssistanceConfig-r17 ::= SEQUENCE
SRSASN_CODE mu_si_m_leave_assist_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(musim_leave_without_resp_timer_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_leave_assist_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(musim_leave_without_resp_timer_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mu_si_m_leave_assist_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("musim-LeaveWithoutResponseTimer-r17", musim_leave_without_resp_timer_r17.to_string());
  j.end_obj();
}

const char* mu_si_m_leave_assist_cfg_r17_s::musim_leave_without_resp_timer_r17_opts::to_string() const
{
  static const char* names[] = {"ms10", "ms20", "ms40", "ms60", "ms80", "ms100", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "mu_si_m_leave_assist_cfg_r17_s::musim_leave_without_resp_timer_r17_e_");
}
uint8_t mu_si_m_leave_assist_cfg_r17_s::musim_leave_without_resp_timer_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 60, 80, 100};
  return map_enum_number(numbers, 6, value, "mu_si_m_leave_assist_cfg_r17_s::musim_leave_without_resp_timer_r17_e_");
}

// PropDelayDiffReportConfig-r17 ::= SEQUENCE
SRSASN_CODE prop_delay_diff_report_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thresh_prop_delay_diff_r17_present, 1));
  HANDLE_CODE(bref.pack(neigh_cell_info_list_r17.size() > 0, 1));

  if (thresh_prop_delay_diff_r17_present) {
    HANDLE_CODE(thresh_prop_delay_diff_r17.pack(bref));
  }
  if (neigh_cell_info_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, neigh_cell_info_list_r17, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE prop_delay_diff_report_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thresh_prop_delay_diff_r17_present, 1));
  bool neigh_cell_info_list_r17_present;
  HANDLE_CODE(bref.unpack(neigh_cell_info_list_r17_present, 1));

  if (thresh_prop_delay_diff_r17_present) {
    HANDLE_CODE(thresh_prop_delay_diff_r17.unpack(bref));
  }
  if (neigh_cell_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(neigh_cell_info_list_r17, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void prop_delay_diff_report_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thresh_prop_delay_diff_r17_present) {
    j.write_str("threshPropDelayDiff-r17", thresh_prop_delay_diff_r17.to_string());
  }
  if (neigh_cell_info_list_r17.size() > 0) {
    j.start_array("neighCellInfoList-r17");
    for (const auto& e1 : neigh_cell_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* prop_delay_diff_report_cfg_r17_s::thresh_prop_delay_diff_r17_opts::to_string() const
{
  static const char* names[] = {"ms0dot5",
                                "ms1",
                                "ms2",
                                "ms3",
                                "ms4",
                                "ms5",
                                "ms6",
                                "ms7",
                                "ms8",
                                "ms9",
                                "ms10",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "prop_delay_diff_report_cfg_r17_s::thresh_prop_delay_diff_r17_e_");
}
float prop_delay_diff_report_cfg_r17_s::thresh_prop_delay_diff_r17_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  return map_enum_number(numbers, 11, value, "prop_delay_diff_report_cfg_r17_s::thresh_prop_delay_diff_r17_e_");
}
const char* prop_delay_diff_report_cfg_r17_s::thresh_prop_delay_diff_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
  return convert_enum_idx(number_strs, 16, value, "prop_delay_diff_report_cfg_r17_s::thresh_prop_delay_diff_r17_e_");
}

// RLM-RelaxationReportingConfig-r17 ::= SEQUENCE
SRSASN_CODE rlm_relaxation_report_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rlm_relaxtion_report_prohibit_timer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlm_relaxation_report_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rlm_relaxtion_report_prohibit_timer.unpack(bref));

  return SRSASN_SUCCESS;
}
void rlm_relaxation_report_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rlm-RelaxtionReportingProhibitTimer", rlm_relaxtion_report_prohibit_timer.to_string());
  j.end_obj();
}

const char* rlm_relaxation_report_cfg_r17_s::rlm_relaxtion_report_prohibit_timer_opts::to_string() const
{
  static const char* names[] = {"s0",
                                "s0dot5",
                                "s1",
                                "s2",
                                "s5",
                                "s10",
                                "s20",
                                "s30",
                                "s60",
                                "s90",
                                "s120",
                                "s300",
                                "s600",
                                "infinity",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "rlm_relaxation_report_cfg_r17_s::rlm_relaxtion_report_prohibit_timer_e_");
}
float rlm_relaxation_report_cfg_r17_s::rlm_relaxtion_report_prohibit_timer_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0, -1.0};
  return map_enum_number(numbers, 14, value, "rlm_relaxation_report_cfg_r17_s::rlm_relaxtion_report_prohibit_timer_e_");
}
const char* rlm_relaxation_report_cfg_r17_s::rlm_relaxtion_report_prohibit_timer_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600", "inf"};
  return convert_enum_idx(
      number_strs, 16, value, "rlm_relaxation_report_cfg_r17_s::rlm_relaxtion_report_prohibit_timer_e_");
}

// RRM-MeasRelaxationReportingConfig-r17 ::= SEQUENCE
SRSASN_CODE rrm_meas_relaxation_report_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(s_search_delta_p_stationary_r17.pack(bref));
  HANDLE_CODE(t_search_delta_p_stationary_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrm_meas_relaxation_report_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(s_search_delta_p_stationary_r17.unpack(bref));
  HANDLE_CODE(t_search_delta_p_stationary_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrm_meas_relaxation_report_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("s-SearchDeltaP-Stationary-r17", s_search_delta_p_stationary_r17.to_string());
  j.write_str("t-SearchDeltaP-Stationary-r17", t_search_delta_p_stationary_r17.to_string());
  j.end_obj();
}

const char* rrm_meas_relaxation_report_cfg_r17_s::s_search_delta_p_stationary_r17_opts::to_string() const
{
  static const char* names[] = {"dB2", "dB3", "dB6", "dB9", "dB12", "dB15", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "rrm_meas_relaxation_report_cfg_r17_s::s_search_delta_p_stationary_r17_e_");
}
uint8_t rrm_meas_relaxation_report_cfg_r17_s::s_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 6, 9, 12, 15};
  return map_enum_number(numbers, 6, value, "rrm_meas_relaxation_report_cfg_r17_s::s_search_delta_p_stationary_r17_e_");
}

const char* rrm_meas_relaxation_report_cfg_r17_s::t_search_delta_p_stationary_r17_opts::to_string() const
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
  return convert_enum_idx(names, 16, value, "rrm_meas_relaxation_report_cfg_r17_s::t_search_delta_p_stationary_r17_e_");
}
uint16_t rrm_meas_relaxation_report_cfg_r17_s::t_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(numbers, 9, value, "rrm_meas_relaxation_report_cfg_r17_s::t_search_delta_p_stationary_r17_e_");
}

// SCG-DeactivationPreferenceConfig-r17 ::= SEQUENCE
SRSASN_CODE scg_deactivation_pref_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(scg_deactivation_pref_prohibit_timer_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_deactivation_pref_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(scg_deactivation_pref_prohibit_timer_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_deactivation_pref_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("scg-DeactivationPreferenceProhibitTimer-r17", scg_deactivation_pref_prohibit_timer_r17.to_string());
  j.end_obj();
}

const char* scg_deactivation_pref_cfg_r17_s::scg_deactivation_pref_prohibit_timer_r17_opts::to_string() const
{
  static const char* names[] = {"s0",
                                "s1",
                                "s2",
                                "s4",
                                "s8",
                                "s10",
                                "s15",
                                "s30",
                                "s60",
                                "s120",
                                "s180",
                                "s240",
                                "s300",
                                "s600",
                                "s900",
                                "s1800"};
  return convert_enum_idx(
      names, 16, value, "scg_deactivation_pref_cfg_r17_s::scg_deactivation_pref_prohibit_timer_r17_e_");
}
uint16_t scg_deactivation_pref_cfg_r17_s::scg_deactivation_pref_prohibit_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 8, 10, 15, 30, 60, 120, 180, 240, 300, 600, 900, 1800};
  return map_enum_number(
      numbers, 16, value, "scg_deactivation_pref_cfg_r17_s::scg_deactivation_pref_prohibit_timer_r17_e_");
}

// SL-RemoteUE-ToAddMod-r17 ::= SEQUENCE
SRSASN_CODE sl_remote_ue_to_add_mod_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_srap_cfg_relay_r17_present, 1));

  HANDLE_CODE(sl_l2_id_remote_r17.pack(bref));
  if (sl_srap_cfg_relay_r17_present) {
    HANDLE_CODE(sl_srap_cfg_relay_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_remote_ue_to_add_mod_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_srap_cfg_relay_r17_present, 1));

  HANDLE_CODE(sl_l2_id_remote_r17.unpack(bref));
  if (sl_srap_cfg_relay_r17_present) {
    HANDLE_CODE(sl_srap_cfg_relay_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_remote_ue_to_add_mod_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-L2IdentityRemote-r17", sl_l2_id_remote_r17.to_string());
  if (sl_srap_cfg_relay_r17_present) {
    j.write_fieldname("sl-SRAP-ConfigRelay-r17");
    sl_srap_cfg_relay_r17.to_json(j);
  }
  j.end_obj();
}

// SuccessHO-Config-r17 ::= SEQUENCE
SRSASN_CODE success_ho_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(thres_percentage_t304_r17_present, 1));
  HANDLE_CODE(bref.pack(thres_percentage_t310_r17_present, 1));
  HANDLE_CODE(bref.pack(thres_percentage_t312_r17_present, 1));
  HANDLE_CODE(bref.pack(source_daps_fail_report_r17_present, 1));

  if (thres_percentage_t304_r17_present) {
    HANDLE_CODE(thres_percentage_t304_r17.pack(bref));
  }
  if (thres_percentage_t310_r17_present) {
    HANDLE_CODE(thres_percentage_t310_r17.pack(bref));
  }
  if (thres_percentage_t312_r17_present) {
    HANDLE_CODE(thres_percentage_t312_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE success_ho_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(thres_percentage_t304_r17_present, 1));
  HANDLE_CODE(bref.unpack(thres_percentage_t310_r17_present, 1));
  HANDLE_CODE(bref.unpack(thres_percentage_t312_r17_present, 1));
  HANDLE_CODE(bref.unpack(source_daps_fail_report_r17_present, 1));

  if (thres_percentage_t304_r17_present) {
    HANDLE_CODE(thres_percentage_t304_r17.unpack(bref));
  }
  if (thres_percentage_t310_r17_present) {
    HANDLE_CODE(thres_percentage_t310_r17.unpack(bref));
  }
  if (thres_percentage_t312_r17_present) {
    HANDLE_CODE(thres_percentage_t312_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void success_ho_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thres_percentage_t304_r17_present) {
    j.write_str("thresholdPercentageT304-r17", thres_percentage_t304_r17.to_string());
  }
  if (thres_percentage_t310_r17_present) {
    j.write_str("thresholdPercentageT310-r17", thres_percentage_t310_r17.to_string());
  }
  if (thres_percentage_t312_r17_present) {
    j.write_str("thresholdPercentageT312-r17", thres_percentage_t312_r17.to_string());
  }
  if (source_daps_fail_report_r17_present) {
    j.write_str("sourceDAPS-FailureReporting-r17", "true");
  }
  j.end_obj();
}

const char* success_ho_cfg_r17_s::thres_percentage_t304_r17_opts::to_string() const
{
  static const char* names[] = {"p40", "p60", "p80", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "success_ho_cfg_r17_s::thres_percentage_t304_r17_e_");
}
uint8_t success_ho_cfg_r17_s::thres_percentage_t304_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {40, 60, 80};
  return map_enum_number(numbers, 3, value, "success_ho_cfg_r17_s::thres_percentage_t304_r17_e_");
}

const char* success_ho_cfg_r17_s::thres_percentage_t310_r17_opts::to_string() const
{
  static const char* names[] = {"p40", "p60", "p80", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "success_ho_cfg_r17_s::thres_percentage_t310_r17_e_");
}
uint8_t success_ho_cfg_r17_s::thres_percentage_t310_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {40, 60, 80};
  return map_enum_number(numbers, 3, value, "success_ho_cfg_r17_s::thres_percentage_t310_r17_e_");
}

const char* success_ho_cfg_r17_s::thres_percentage_t312_r17_opts::to_string() const
{
  static const char* names[] = {"p20", "p40", "p60", "p80", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "success_ho_cfg_r17_s::thres_percentage_t312_r17_e_");
}
uint8_t success_ho_cfg_r17_s::thres_percentage_t312_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 40, 60, 80};
  return map_enum_number(numbers, 4, value, "success_ho_cfg_r17_s::thres_percentage_t312_r17_e_");
}

// BAP-RoutingID-r16 ::= SEQUENCE
SRSASN_CODE bap_routing_id_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bap_address_r16.pack(bref));
  HANDLE_CODE(bap_path_id_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bap_routing_id_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bap_address_r16.unpack(bref));
  HANDLE_CODE(bap_path_id_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void bap_routing_id_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bap-Address-r16", bap_address_r16.to_string());
  j.write_str("bap-PathId-r16", bap_path_id_r16.to_string());
  j.end_obj();
}

// CondReconfigToAddMod-r16 ::= SEQUENCE
SRSASN_CODE cond_recfg_to_add_mod_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cond_execution_cond_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(cond_rrc_recfg_r16.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, cond_recfg_id_r16, (uint8_t)1u, (uint8_t)8u));
  if (cond_execution_cond_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cond_execution_cond_r16, 1, 2, integer_packer<uint8_t>(1, 64)));
  }
  if (cond_rrc_recfg_r16.size() > 0) {
    HANDLE_CODE(cond_rrc_recfg_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cond_execution_cond_scg_r17.size() > 0;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cond_execution_cond_scg_r17.size() > 0, 1));
      if (cond_execution_cond_scg_r17.size() > 0) {
        HANDLE_CODE(cond_execution_cond_scg_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cond_recfg_to_add_mod_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cond_execution_cond_r16_present;
  HANDLE_CODE(bref.unpack(cond_execution_cond_r16_present, 1));
  bool cond_rrc_recfg_r16_present;
  HANDLE_CODE(bref.unpack(cond_rrc_recfg_r16_present, 1));

  HANDLE_CODE(unpack_integer(cond_recfg_id_r16, bref, (uint8_t)1u, (uint8_t)8u));
  if (cond_execution_cond_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cond_execution_cond_r16, bref, 1, 2, integer_packer<uint8_t>(1, 64)));
  }
  if (cond_rrc_recfg_r16_present) {
    HANDLE_CODE(cond_rrc_recfg_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cond_execution_cond_scg_r17_present;
      HANDLE_CODE(bref.unpack(cond_execution_cond_scg_r17_present, 1));
      if (cond_execution_cond_scg_r17_present) {
        HANDLE_CODE(cond_execution_cond_scg_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cond_recfg_to_add_mod_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("condReconfigId-r16", cond_recfg_id_r16);
  if (cond_execution_cond_r16.size() > 0) {
    j.start_array("condExecutionCond-r16");
    for (const auto& e1 : cond_execution_cond_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cond_rrc_recfg_r16.size() > 0) {
    j.write_str("condRRCReconfig-r16", cond_rrc_recfg_r16.to_string());
  }
  if (ext) {
    if (cond_execution_cond_scg_r17.size() > 0) {
      j.write_str("condExecutionCondSCG-r17", cond_execution_cond_scg_r17.to_string());
    }
  }
  j.end_obj();
}

// DRX-PreferenceConfig-r16 ::= SEQUENCE
SRSASN_CODE drx_pref_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(drx_pref_prohibit_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_pref_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(drx_pref_prohibit_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void drx_pref_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("drx-PreferenceProhibitTimer-r16", drx_pref_prohibit_timer_r16.to_string());
  j.end_obj();
}

const char* drx_pref_cfg_r16_s::drx_pref_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {
      "s0", "s0dot5", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s20", "s30", "spare2", "spare1"};
  return convert_enum_idx(names, 16, value, "drx_pref_cfg_r16_s::drx_pref_prohibit_timer_r16_e_");
}
float drx_pref_cfg_r16_s::drx_pref_prohibit_timer_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 20.0, 30.0};
  return map_enum_number(numbers, 14, value, "drx_pref_cfg_r16_s::drx_pref_prohibit_timer_r16_e_");
}
const char* drx_pref_cfg_r16_s::drx_pref_prohibit_timer_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30"};
  return convert_enum_idx(number_strs, 16, value, "drx_pref_cfg_r16_s::drx_pref_prohibit_timer_r16_e_");
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

const char* iab_ip_address_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"iPv4-Address-r16", "iPv6-Address-r16", "iPv6-Prefix-r16"};
  return convert_enum_idx(names, 3, value, "iab_ip_address_r16_c::types");
}

// IAB-IP-Usage-r16 ::= ENUMERATED
const char* iab_ip_usage_r16_opts::to_string() const
{
  static const char* names[] = {"f1-C", "f1-U", "non-F1", "spare"};
  return convert_enum_idx(names, 4, value, "iab_ip_usage_r16_e");
}

// IDC-AssistanceConfig-r16 ::= SEQUENCE
SRSASN_CODE idc_assist_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(candidate_serving_freq_list_nr_r16.size() > 0, 1));

  if (candidate_serving_freq_list_nr_r16.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, candidate_serving_freq_list_nr_r16, 1, 128, integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE idc_assist_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool candidate_serving_freq_list_nr_r16_present;
  HANDLE_CODE(bref.unpack(candidate_serving_freq_list_nr_r16_present, 1));

  if (candidate_serving_freq_list_nr_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(candidate_serving_freq_list_nr_r16, bref, 1, 128, integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
void idc_assist_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_serving_freq_list_nr_r16.size() > 0) {
    j.start_array("candidateServingFreqListNR-r16");
    for (const auto& e1 : candidate_serving_freq_list_nr_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// MaxBW-PreferenceConfig-r16 ::= SEQUENCE
SRSASN_CODE max_bw_pref_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_bw_pref_prohibit_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_bw_pref_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_bw_pref_prohibit_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void max_bw_pref_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxBW-PreferenceProhibitTimer-r16", max_bw_pref_prohibit_timer_r16.to_string());
  j.end_obj();
}

const char* max_bw_pref_cfg_r16_s::max_bw_pref_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {
      "s0", "s0dot5", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s20", "s30", "spare2", "spare1"};
  return convert_enum_idx(names, 16, value, "max_bw_pref_cfg_r16_s::max_bw_pref_prohibit_timer_r16_e_");
}
float max_bw_pref_cfg_r16_s::max_bw_pref_prohibit_timer_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 20.0, 30.0};
  return map_enum_number(numbers, 14, value, "max_bw_pref_cfg_r16_s::max_bw_pref_prohibit_timer_r16_e_");
}
const char* max_bw_pref_cfg_r16_s::max_bw_pref_prohibit_timer_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30"};
  return convert_enum_idx(number_strs, 16, value, "max_bw_pref_cfg_r16_s::max_bw_pref_prohibit_timer_r16_e_");
}

// MaxCC-PreferenceConfig-r16 ::= SEQUENCE
SRSASN_CODE max_cc_pref_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_cc_pref_prohibit_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_cc_pref_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_cc_pref_prohibit_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void max_cc_pref_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxCC-PreferenceProhibitTimer-r16", max_cc_pref_prohibit_timer_r16.to_string());
  j.end_obj();
}

const char* max_cc_pref_cfg_r16_s::max_cc_pref_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {
      "s0", "s0dot5", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s20", "s30", "spare2", "spare1"};
  return convert_enum_idx(names, 16, value, "max_cc_pref_cfg_r16_s::max_cc_pref_prohibit_timer_r16_e_");
}
float max_cc_pref_cfg_r16_s::max_cc_pref_prohibit_timer_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 20.0, 30.0};
  return map_enum_number(numbers, 14, value, "max_cc_pref_cfg_r16_s::max_cc_pref_prohibit_timer_r16_e_");
}
const char* max_cc_pref_cfg_r16_s::max_cc_pref_prohibit_timer_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30"};
  return convert_enum_idx(number_strs, 16, value, "max_cc_pref_cfg_r16_s::max_cc_pref_prohibit_timer_r16_e_");
}

// MaxMIMO-LayerPreferenceConfig-r16 ::= SEQUENCE
SRSASN_CODE max_mimo_layer_pref_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_mimo_layer_pref_prohibit_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_mimo_layer_pref_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_mimo_layer_pref_prohibit_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void max_mimo_layer_pref_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxMIMO-LayerPreferenceProhibitTimer-r16", max_mimo_layer_pref_prohibit_timer_r16.to_string());
  j.end_obj();
}

const char* max_mimo_layer_pref_cfg_r16_s::max_mimo_layer_pref_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {
      "s0", "s0dot5", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s20", "s30", "spare2", "spare1"};
  return convert_enum_idx(names, 16, value, "max_mimo_layer_pref_cfg_r16_s::max_mimo_layer_pref_prohibit_timer_r16_e_");
}
float max_mimo_layer_pref_cfg_r16_s::max_mimo_layer_pref_prohibit_timer_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 20.0, 30.0};
  return map_enum_number(
      numbers, 14, value, "max_mimo_layer_pref_cfg_r16_s::max_mimo_layer_pref_prohibit_timer_r16_e_");
}
const char* max_mimo_layer_pref_cfg_r16_s::max_mimo_layer_pref_prohibit_timer_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30"};
  return convert_enum_idx(
      number_strs, 16, value, "max_mimo_layer_pref_cfg_r16_s::max_mimo_layer_pref_prohibit_timer_r16_e_");
}

// MeasConfigAppLayer-r17 ::= SEQUENCE
SRSASN_CODE meas_cfg_app_layer_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_cfg_app_layer_container_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(service_type_r17_present, 1));
  HANDLE_CODE(bref.pack(pause_report_r17_present, 1));
  HANDLE_CODE(bref.pack(tx_of_session_start_stop_r17_present, 1));
  HANDLE_CODE(bref.pack(ran_visible_params_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_cfg_app_layer_id_r17, (uint8_t)0u, (uint8_t)15u));
  if (meas_cfg_app_layer_container_r17.size() > 0) {
    HANDLE_CODE(meas_cfg_app_layer_container_r17.pack(bref));
  }
  if (service_type_r17_present) {
    HANDLE_CODE(service_type_r17.pack(bref));
  }
  if (pause_report_r17_present) {
    HANDLE_CODE(bref.pack(pause_report_r17, 1));
  }
  if (tx_of_session_start_stop_r17_present) {
    HANDLE_CODE(bref.pack(tx_of_session_start_stop_r17, 1));
  }
  if (ran_visible_params_r17_present) {
    HANDLE_CODE(ran_visible_params_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_app_layer_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_cfg_app_layer_container_r17_present;
  HANDLE_CODE(bref.unpack(meas_cfg_app_layer_container_r17_present, 1));
  HANDLE_CODE(bref.unpack(service_type_r17_present, 1));
  HANDLE_CODE(bref.unpack(pause_report_r17_present, 1));
  HANDLE_CODE(bref.unpack(tx_of_session_start_stop_r17_present, 1));
  HANDLE_CODE(bref.unpack(ran_visible_params_r17_present, 1));

  HANDLE_CODE(unpack_integer(meas_cfg_app_layer_id_r17, bref, (uint8_t)0u, (uint8_t)15u));
  if (meas_cfg_app_layer_container_r17_present) {
    HANDLE_CODE(meas_cfg_app_layer_container_r17.unpack(bref));
  }
  if (service_type_r17_present) {
    HANDLE_CODE(service_type_r17.unpack(bref));
  }
  if (pause_report_r17_present) {
    HANDLE_CODE(bref.unpack(pause_report_r17, 1));
  }
  if (tx_of_session_start_stop_r17_present) {
    HANDLE_CODE(bref.unpack(tx_of_session_start_stop_r17, 1));
  }
  if (ran_visible_params_r17_present) {
    HANDLE_CODE(ran_visible_params_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_cfg_app_layer_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measConfigAppLayerId-r17", meas_cfg_app_layer_id_r17);
  if (meas_cfg_app_layer_container_r17.size() > 0) {
    j.write_str("measConfigAppLayerContainer-r17", meas_cfg_app_layer_container_r17.to_string());
  }
  if (service_type_r17_present) {
    j.write_str("serviceType-r17", service_type_r17.to_string());
  }
  if (pause_report_r17_present) {
    j.write_bool("pauseReporting-r17", pause_report_r17);
  }
  if (tx_of_session_start_stop_r17_present) {
    j.write_bool("transmissionOfSessionStartStop-r17", tx_of_session_start_stop_r17);
  }
  if (ran_visible_params_r17_present) {
    j.write_fieldname("ran-VisibleParameters-r17");
    ran_visible_params_r17.to_json(j);
  }
  j.end_obj();
}

const char* meas_cfg_app_layer_r17_s::service_type_r17_opts::to_string() const
{
  static const char* names[] = {"streaming", "mtsi", "vr", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "meas_cfg_app_layer_r17_s::service_type_r17_e_");
}

// MinSchedulingOffsetPreferenceConfig-r16 ::= SEQUENCE
SRSASN_CODE min_sched_offset_pref_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(min_sched_offset_pref_prohibit_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE min_sched_offset_pref_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(min_sched_offset_pref_prohibit_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void min_sched_offset_pref_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("minSchedulingOffsetPreferenceProhibitTimer-r16", min_sched_offset_pref_prohibit_timer_r16.to_string());
  j.end_obj();
}

const char* min_sched_offset_pref_cfg_r16_s::min_sched_offset_pref_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {
      "s0", "s0dot5", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s20", "s30", "spare2", "spare1"};
  return convert_enum_idx(
      names, 16, value, "min_sched_offset_pref_cfg_r16_s::min_sched_offset_pref_prohibit_timer_r16_e_");
}
float min_sched_offset_pref_cfg_r16_s::min_sched_offset_pref_prohibit_timer_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 20.0, 30.0};
  return map_enum_number(
      numbers, 14, value, "min_sched_offset_pref_cfg_r16_s::min_sched_offset_pref_prohibit_timer_r16_e_");
}
const char* min_sched_offset_pref_cfg_r16_s::min_sched_offset_pref_prohibit_timer_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30"};
  return convert_enum_idx(
      number_strs, 16, value, "min_sched_offset_pref_cfg_r16_s::min_sched_offset_pref_prohibit_timer_r16_e_");
}

// MUSIM-GapConfig-r17 ::= SEQUENCE
SRSASN_CODE mu_si_m_gap_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(musim_gap_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(musim_gap_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(musim_aperiodic_gap_r17_present, 1));

  if (musim_gap_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, musim_gap_to_release_list_r17, 1, 3, integer_packer<uint8_t>(0, 2)));
  }
  if (musim_gap_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, musim_gap_to_add_mod_list_r17, 1, 3));
  }
  if (musim_aperiodic_gap_r17_present) {
    HANDLE_CODE(musim_aperiodic_gap_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mu_si_m_gap_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool musim_gap_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(musim_gap_to_release_list_r17_present, 1));
  bool musim_gap_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(musim_gap_to_add_mod_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(musim_aperiodic_gap_r17_present, 1));

  if (musim_gap_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(musim_gap_to_release_list_r17, bref, 1, 3, integer_packer<uint8_t>(0, 2)));
  }
  if (musim_gap_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(musim_gap_to_add_mod_list_r17, bref, 1, 3));
  }
  if (musim_aperiodic_gap_r17_present) {
    HANDLE_CODE(musim_aperiodic_gap_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mu_si_m_gap_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (musim_gap_to_release_list_r17.size() > 0) {
    j.start_array("musim-GapToReleaseList-r17");
    for (const auto& e1 : musim_gap_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (musim_gap_to_add_mod_list_r17.size() > 0) {
    j.start_array("musim-GapToAddModList-r17");
    for (const auto& e1 : musim_gap_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (musim_aperiodic_gap_r17_present) {
    j.write_fieldname("musim-AperiodicGap-r17");
    musim_aperiodic_gap_r17.to_json(j);
  }
  j.end_obj();
}

// NeedForGapNCSG-ConfigEUTRA-r17 ::= SEQUENCE
SRSASN_CODE need_for_gap_ncsg_cfg_eutra_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(requested_target_band_filt_ncsg_eutra_r17.size() > 0, 1));

  if (requested_target_band_filt_ncsg_eutra_r17.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, requested_target_band_filt_ncsg_eutra_r17, 1, 256, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gap_ncsg_cfg_eutra_r17_s::unpack(cbit_ref& bref)
{
  bool requested_target_band_filt_ncsg_eutra_r17_present;
  HANDLE_CODE(bref.unpack(requested_target_band_filt_ncsg_eutra_r17_present, 1));

  if (requested_target_band_filt_ncsg_eutra_r17_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(requested_target_band_filt_ncsg_eutra_r17, bref, 1, 256, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
void need_for_gap_ncsg_cfg_eutra_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_target_band_filt_ncsg_eutra_r17.size() > 0) {
    j.start_array("requestedTargetBandFilterNCSG-EUTRA-r17");
    for (const auto& e1 : requested_target_band_filt_ncsg_eutra_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// NeedForGapNCSG-ConfigNR-r17 ::= SEQUENCE
SRSASN_CODE need_for_gap_ncsg_cfg_nr_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(requested_target_band_filt_ncsg_nr_r17.size() > 0, 1));

  if (requested_target_band_filt_ncsg_nr_r17.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, requested_target_band_filt_ncsg_nr_r17, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gap_ncsg_cfg_nr_r17_s::unpack(cbit_ref& bref)
{
  bool requested_target_band_filt_ncsg_nr_r17_present;
  HANDLE_CODE(bref.unpack(requested_target_band_filt_ncsg_nr_r17_present, 1));

  if (requested_target_band_filt_ncsg_nr_r17_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(requested_target_band_filt_ncsg_nr_r17, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
void need_for_gap_ncsg_cfg_nr_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_target_band_filt_ncsg_nr_r17.size() > 0) {
    j.start_array("requestedTargetBandFilterNCSG-NR-r17");
    for (const auto& e1 : requested_target_band_filt_ncsg_nr_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// ReleasePreferenceConfig-r16 ::= SEQUENCE
SRSASN_CODE release_pref_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(connected_report_present, 1));

  HANDLE_CODE(release_pref_prohibit_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE release_pref_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(connected_report_present, 1));

  HANDLE_CODE(release_pref_prohibit_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void release_pref_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("releasePreferenceProhibitTimer-r16", release_pref_prohibit_timer_r16.to_string());
  if (connected_report_present) {
    j.write_str("connectedReporting", "true");
  }
  j.end_obj();
}

const char* release_pref_cfg_r16_s::release_pref_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {
      "s0", "s0dot5", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s20", "s30", "infinity", "spare1"};
  return convert_enum_idx(names, 16, value, "release_pref_cfg_r16_s::release_pref_prohibit_timer_r16_e_");
}
float release_pref_cfg_r16_s::release_pref_prohibit_timer_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 20.0, 30.0, -1.0};
  return map_enum_number(numbers, 15, value, "release_pref_cfg_r16_s::release_pref_prohibit_timer_r16_e_");
}
const char* release_pref_cfg_r16_s::release_pref_prohibit_timer_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30", "inf"};
  return convert_enum_idx(number_strs, 16, value, "release_pref_cfg_r16_s::release_pref_prohibit_timer_r16_e_");
}

// SL-L2RelayUE-Config-r17 ::= SEQUENCE
SRSASN_CODE sl_l2_relay_ue_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_remote_ue_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_remote_ue_to_release_list_r17.size() > 0, 1));

  if (sl_remote_ue_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_remote_ue_to_add_mod_list_r17, 1, 32));
  }
  if (sl_remote_ue_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_remote_ue_to_release_list_r17, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_l2_relay_ue_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool sl_remote_ue_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_remote_ue_to_add_mod_list_r17_present, 1));
  bool sl_remote_ue_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(sl_remote_ue_to_release_list_r17_present, 1));

  if (sl_remote_ue_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_remote_ue_to_add_mod_list_r17, bref, 1, 32));
  }
  if (sl_remote_ue_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_remote_ue_to_release_list_r17, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void sl_l2_relay_ue_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_remote_ue_to_add_mod_list_r17.size() > 0) {
    j.start_array("sl-RemoteUE-ToAddModList-r17");
    for (const auto& e1 : sl_remote_ue_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_remote_ue_to_release_list_r17.size() > 0) {
    j.start_array("sl-RemoteUE-ToReleaseList-r17");
    for (const auto& e1 : sl_remote_ue_to_release_list_r17) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-TimeOffsetEUTRA-r16 ::= ENUMERATED
const char* sl_time_offset_eutra_r16_opts::to_string() const
{
  static const char* names[] = {"ms0",
                                "ms0dot25",
                                "ms0dot5",
                                "ms0dot625",
                                "ms0dot75",
                                "ms1",
                                "ms1dot25",
                                "ms1dot5",
                                "ms1dot75",
                                "ms2",
                                "ms2dot5",
                                "ms3",
                                "ms4",
                                "ms5",
                                "ms6",
                                "ms8",
                                "ms10",
                                "ms20"};
  return convert_enum_idx(names, 18, value, "sl_time_offset_eutra_r16_e");
}
float sl_time_offset_eutra_r16_opts::to_number() const
{
  static const float numbers[] = {
      0.0, 0.25, 0.5, 0.625, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0, 20.0};
  return map_enum_number(numbers, 18, value, "sl_time_offset_eutra_r16_e");
}
const char* sl_time_offset_eutra_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0", "0.25", "0.5", "0.625", "0.75", "1", "1.25", "1.5", "1.75", "2", "2.5", "3", "4", "5", "6", "8", "10", "20"};
  return convert_enum_idx(number_strs, 18, value, "sl_time_offset_eutra_r16_e");
}

// UE-TxTEG-RequestUL-TDOA-Config-r17 ::= CHOICE
void ue_tx_teg_request_ul_tdoa_cfg_r17_c::set(types::options e)
{
  type_ = e;
}
void ue_tx_teg_request_ul_tdoa_cfg_r17_c::set_one_shot_r17()
{
  set(types::one_shot_r17);
}
ue_tx_teg_request_ul_tdoa_cfg_r17_c::periodic_report_r17_e_&
ue_tx_teg_request_ul_tdoa_cfg_r17_c::set_periodic_report_r17()
{
  set(types::periodic_report_r17);
  return c;
}
void ue_tx_teg_request_ul_tdoa_cfg_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_shot_r17:
      break;
    case types::periodic_report_r17:
      j.write_str("periodicReporting-r17", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ue_tx_teg_request_ul_tdoa_cfg_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_tx_teg_request_ul_tdoa_cfg_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_shot_r17:
      break;
    case types::periodic_report_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_tx_teg_request_ul_tdoa_cfg_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_tx_teg_request_ul_tdoa_cfg_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_shot_r17:
      break;
    case types::periodic_report_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_tx_teg_request_ul_tdoa_cfg_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_tx_teg_request_ul_tdoa_cfg_r17_c::periodic_report_r17_opts::to_string() const
{
  static const char* names[] = {"ms160", "ms320", "ms1280", "ms2560", "ms61440", "ms81920", "ms368640", "ms737280"};
  return convert_enum_idx(names, 8, value, "ue_tx_teg_request_ul_tdoa_cfg_r17_c::periodic_report_r17_e_");
}
uint32_t ue_tx_teg_request_ul_tdoa_cfg_r17_c::periodic_report_r17_opts::to_number() const
{
  static const uint32_t numbers[] = {160, 320, 1280, 2560, 61440, 81920, 368640, 737280};
  return map_enum_number(numbers, 8, value, "ue_tx_teg_request_ul_tdoa_cfg_r17_c::periodic_report_r17_e_");
}

const char* ue_tx_teg_request_ul_tdoa_cfg_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"oneShot-r17", "periodicReporting-r17"};
  return convert_enum_idx(names, 2, value, "ue_tx_teg_request_ul_tdoa_cfg_r17_c::types");
}
uint8_t ue_tx_teg_request_ul_tdoa_cfg_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_tx_teg_request_ul_tdoa_cfg_r17_c::types");
}

// UL-GapFR2-Config-r17 ::= SEQUENCE
SRSASN_CODE ul_gap_fr2_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ref_fr2_serv_cell_async_ca_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, gap_offset_r17, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(ugl_r17.pack(bref));
  HANDLE_CODE(ugrp_r17.pack(bref));
  if (ref_fr2_serv_cell_async_ca_r17_present) {
    HANDLE_CODE(pack_integer(bref, ref_fr2_serv_cell_async_ca_r17, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_gap_fr2_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ref_fr2_serv_cell_async_ca_r17_present, 1));

  HANDLE_CODE(unpack_integer(gap_offset_r17, bref, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(ugl_r17.unpack(bref));
  HANDLE_CODE(ugrp_r17.unpack(bref));
  if (ref_fr2_serv_cell_async_ca_r17_present) {
    HANDLE_CODE(unpack_integer(ref_fr2_serv_cell_async_ca_r17, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void ul_gap_fr2_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gapOffset-r17", gap_offset_r17);
  j.write_str("ugl-r17", ugl_r17.to_string());
  j.write_str("ugrp-r17", ugrp_r17.to_string());
  if (ref_fr2_serv_cell_async_ca_r17_present) {
    j.write_int("refFR2-ServCellAsyncCA-r17", ref_fr2_serv_cell_async_ca_r17);
  }
  j.end_obj();
}

const char* ul_gap_fr2_cfg_r17_s::ugl_r17_opts::to_string() const
{
  static const char* names[] = {"ms0dot125", "ms0dot25", "ms0dot5", "ms1"};
  return convert_enum_idx(names, 4, value, "ul_gap_fr2_cfg_r17_s::ugl_r17_e_");
}
float ul_gap_fr2_cfg_r17_s::ugl_r17_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0};
  return map_enum_number(numbers, 4, value, "ul_gap_fr2_cfg_r17_s::ugl_r17_e_");
}
const char* ul_gap_fr2_cfg_r17_s::ugl_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.125", "0.25", "0.5", "1"};
  return convert_enum_idx(number_strs, 4, value, "ul_gap_fr2_cfg_r17_s::ugl_r17_e_");
}

const char* ul_gap_fr2_cfg_r17_s::ugrp_r17_opts::to_string() const
{
  static const char* names[] = {"ms5", "ms20", "ms40", "ms160"};
  return convert_enum_idx(names, 4, value, "ul_gap_fr2_cfg_r17_s::ugrp_r17_e_");
}
uint8_t ul_gap_fr2_cfg_r17_s::ugrp_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 20, 40, 160};
  return map_enum_number(numbers, 4, value, "ul_gap_fr2_cfg_r17_s::ugrp_r17_e_");
}

// AppLayerMeasConfig-r17 ::= SEQUENCE
SRSASN_CODE app_layer_meas_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_cfg_app_layer_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_cfg_app_layer_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(rrc_seg_allowed_r17_present, 1));

  if (meas_cfg_app_layer_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_cfg_app_layer_to_add_mod_list_r17, 1, 16));
  }
  if (meas_cfg_app_layer_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_cfg_app_layer_to_release_list_r17, 1, 16, integer_packer<uint8_t>(0, 15)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE app_layer_meas_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_cfg_app_layer_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(meas_cfg_app_layer_to_add_mod_list_r17_present, 1));
  bool meas_cfg_app_layer_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(meas_cfg_app_layer_to_release_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(rrc_seg_allowed_r17_present, 1));

  if (meas_cfg_app_layer_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_cfg_app_layer_to_add_mod_list_r17, bref, 1, 16));
  }
  if (meas_cfg_app_layer_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_cfg_app_layer_to_release_list_r17, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
  }

  return SRSASN_SUCCESS;
}
void app_layer_meas_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_cfg_app_layer_to_add_mod_list_r17.size() > 0) {
    j.start_array("measConfigAppLayerToAddModList-r17");
    for (const auto& e1 : meas_cfg_app_layer_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_cfg_app_layer_to_release_list_r17.size() > 0) {
    j.start_array("measConfigAppLayerToReleaseList-r17");
    for (const auto& e1 : meas_cfg_app_layer_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (rrc_seg_allowed_r17_present) {
    j.write_str("rrc-SegAllowed-r17", "enabled");
  }
  j.end_obj();
}

// BAP-Config-r16 ::= SEQUENCE
SRSASN_CODE bap_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(bap_address_r16_present, 1));
  HANDLE_CODE(bref.pack(default_ul_bap_routing_id_r16_present, 1));
  HANDLE_CODE(bref.pack(default_ul_bh_rlc_ch_r16_present, 1));
  HANDLE_CODE(bref.pack(flow_ctrl_feedback_type_r16_present, 1));

  if (bap_address_r16_present) {
    HANDLE_CODE(bap_address_r16.pack(bref));
  }
  if (default_ul_bap_routing_id_r16_present) {
    HANDLE_CODE(default_ul_bap_routing_id_r16.pack(bref));
  }
  if (default_ul_bh_rlc_ch_r16_present) {
    HANDLE_CODE(default_ul_bh_rlc_ch_r16.pack(bref));
  }
  if (flow_ctrl_feedback_type_r16_present) {
    HANDLE_CODE(flow_ctrl_feedback_type_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bap_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(bap_address_r16_present, 1));
  HANDLE_CODE(bref.unpack(default_ul_bap_routing_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(default_ul_bh_rlc_ch_r16_present, 1));
  HANDLE_CODE(bref.unpack(flow_ctrl_feedback_type_r16_present, 1));

  if (bap_address_r16_present) {
    HANDLE_CODE(bap_address_r16.unpack(bref));
  }
  if (default_ul_bap_routing_id_r16_present) {
    HANDLE_CODE(default_ul_bap_routing_id_r16.unpack(bref));
  }
  if (default_ul_bh_rlc_ch_r16_present) {
    HANDLE_CODE(default_ul_bh_rlc_ch_r16.unpack(bref));
  }
  if (flow_ctrl_feedback_type_r16_present) {
    HANDLE_CODE(flow_ctrl_feedback_type_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bap_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bap_address_r16_present) {
    j.write_str("bap-Address-r16", bap_address_r16.to_string());
  }
  if (default_ul_bap_routing_id_r16_present) {
    j.write_fieldname("defaultUL-BAP-RoutingID-r16");
    default_ul_bap_routing_id_r16.to_json(j);
  }
  if (default_ul_bh_rlc_ch_r16_present) {
    j.write_str("defaultUL-BH-RLC-Channel-r16", default_ul_bh_rlc_ch_r16.to_string());
  }
  if (flow_ctrl_feedback_type_r16_present) {
    j.write_str("flowControlFeedbackType-r16", flow_ctrl_feedback_type_r16.to_string());
  }
  j.end_obj();
}

const char* bap_cfg_r16_s::flow_ctrl_feedback_type_r16_opts::to_string() const
{
  static const char* names[] = {"perBH-RLC-Channel", "perRoutingID", "both"};
  return convert_enum_idx(names, 3, value, "bap_cfg_r16_s::flow_ctrl_feedback_type_r16_e_");
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

// IAB-IP-AddressConfiguration-r16 ::= SEQUENCE
SRSASN_CODE iab_ip_address_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(iab_ip_address_r16_present, 1));
  HANDLE_CODE(bref.pack(iab_ip_usage_r16_present, 1));
  HANDLE_CODE(bref.pack(iab_donor_du_bap_address_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, iab_ip_address_idx_r16, (uint8_t)1u, (uint8_t)32u));
  if (iab_ip_address_r16_present) {
    HANDLE_CODE(iab_ip_address_r16.pack(bref));
  }
  if (iab_ip_usage_r16_present) {
    HANDLE_CODE(iab_ip_usage_r16.pack(bref));
  }
  if (iab_donor_du_bap_address_r16_present) {
    HANDLE_CODE(iab_donor_du_bap_address_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_ip_address_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(iab_ip_address_r16_present, 1));
  HANDLE_CODE(bref.unpack(iab_ip_usage_r16_present, 1));
  HANDLE_CODE(bref.unpack(iab_donor_du_bap_address_r16_present, 1));

  HANDLE_CODE(unpack_integer(iab_ip_address_idx_r16, bref, (uint8_t)1u, (uint8_t)32u));
  if (iab_ip_address_r16_present) {
    HANDLE_CODE(iab_ip_address_r16.unpack(bref));
  }
  if (iab_ip_usage_r16_present) {
    HANDLE_CODE(iab_ip_usage_r16.unpack(bref));
  }
  if (iab_donor_du_bap_address_r16_present) {
    HANDLE_CODE(iab_donor_du_bap_address_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void iab_ip_address_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("iab-IP-AddressIndex-r16", iab_ip_address_idx_r16);
  if (iab_ip_address_r16_present) {
    j.write_fieldname("iab-IP-Address-r16");
    iab_ip_address_r16.to_json(j);
  }
  if (iab_ip_usage_r16_present) {
    j.write_str("iab-IP-Usage-r16", iab_ip_usage_r16.to_string());
  }
  if (iab_donor_du_bap_address_r16_present) {
    j.write_str("iab-donor-DU-BAP-Address-r16", iab_donor_du_bap_address_r16.to_string());
  }
  j.end_obj();
}

// NeedForGapsConfigNR-r16 ::= SEQUENCE
SRSASN_CODE need_for_gaps_cfg_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(requested_target_band_filt_nr_r16.size() > 0, 1));

  if (requested_target_band_filt_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, requested_target_band_filt_nr_r16, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE need_for_gaps_cfg_nr_r16_s::unpack(cbit_ref& bref)
{
  bool requested_target_band_filt_nr_r16_present;
  HANDLE_CODE(bref.unpack(requested_target_band_filt_nr_r16_present, 1));

  if (requested_target_band_filt_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(requested_target_band_filt_nr_r16, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
void need_for_gaps_cfg_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_target_band_filt_nr_r16.size() > 0) {
    j.start_array("requestedTargetBandFilterNR-r16");
    for (const auto& e1 : requested_target_band_filt_nr_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// OnDemandSIB-Request-r16 ::= SEQUENCE
SRSASN_CODE on_demand_sib_request_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(on_demand_sib_request_prohibit_timer_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE on_demand_sib_request_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(on_demand_sib_request_prohibit_timer_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void on_demand_sib_request_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("onDemandSIB-RequestProhibitTimer-r16", on_demand_sib_request_prohibit_timer_r16.to_string());
  j.end_obj();
}

const char* on_demand_sib_request_r16_s::on_demand_sib_request_prohibit_timer_r16_opts::to_string() const
{
  static const char* names[] = {"s0", "s0dot5", "s1", "s2", "s5", "s10", "s20", "s30"};
  return convert_enum_idx(names, 8, value, "on_demand_sib_request_r16_s::on_demand_sib_request_prohibit_timer_r16_e_");
}
float on_demand_sib_request_r16_s::on_demand_sib_request_prohibit_timer_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0};
  return map_enum_number(numbers, 8, value, "on_demand_sib_request_r16_s::on_demand_sib_request_prohibit_timer_r16_e_");
}
const char* on_demand_sib_request_r16_s::on_demand_sib_request_prohibit_timer_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "5", "10", "20", "30"};
  return convert_enum_idx(
      number_strs, 8, value, "on_demand_sib_request_r16_s::on_demand_sib_request_prohibit_timer_r16_e_");
}

// OtherConfig-v1700 ::= SEQUENCE
SRSASN_CODE other_cfg_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_gap_fr2_pref_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(musim_gap_assist_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(musim_leave_assist_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(success_ho_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(max_bw_pref_cfg_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.pack(max_mimo_layer_pref_cfg_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.pack(min_sched_offset_pref_cfg_ext_r17_present, 1));
  HANDLE_CODE(bref.pack(rlm_relaxation_report_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(bfd_relaxation_report_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(scg_deactivation_pref_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(rrm_meas_relaxation_report_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(prop_delay_diff_report_cfg_r17_present, 1));

  if (musim_gap_assist_cfg_r17_present) {
    HANDLE_CODE(musim_gap_assist_cfg_r17.pack(bref));
  }
  if (musim_leave_assist_cfg_r17_present) {
    HANDLE_CODE(musim_leave_assist_cfg_r17.pack(bref));
  }
  if (success_ho_cfg_r17_present) {
    HANDLE_CODE(success_ho_cfg_r17.pack(bref));
  }
  if (rlm_relaxation_report_cfg_r17_present) {
    HANDLE_CODE(rlm_relaxation_report_cfg_r17.pack(bref));
  }
  if (bfd_relaxation_report_cfg_r17_present) {
    HANDLE_CODE(bfd_relaxation_report_cfg_r17.pack(bref));
  }
  if (scg_deactivation_pref_cfg_r17_present) {
    HANDLE_CODE(scg_deactivation_pref_cfg_r17.pack(bref));
  }
  if (rrm_meas_relaxation_report_cfg_r17_present) {
    HANDLE_CODE(rrm_meas_relaxation_report_cfg_r17.pack(bref));
  }
  if (prop_delay_diff_report_cfg_r17_present) {
    HANDLE_CODE(prop_delay_diff_report_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_gap_fr2_pref_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(musim_gap_assist_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(musim_leave_assist_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(success_ho_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(max_bw_pref_cfg_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.unpack(max_mimo_layer_pref_cfg_fr2_2_r17_present, 1));
  HANDLE_CODE(bref.unpack(min_sched_offset_pref_cfg_ext_r17_present, 1));
  HANDLE_CODE(bref.unpack(rlm_relaxation_report_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(bfd_relaxation_report_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(scg_deactivation_pref_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(rrm_meas_relaxation_report_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(prop_delay_diff_report_cfg_r17_present, 1));

  if (musim_gap_assist_cfg_r17_present) {
    HANDLE_CODE(musim_gap_assist_cfg_r17.unpack(bref));
  }
  if (musim_leave_assist_cfg_r17_present) {
    HANDLE_CODE(musim_leave_assist_cfg_r17.unpack(bref));
  }
  if (success_ho_cfg_r17_present) {
    HANDLE_CODE(success_ho_cfg_r17.unpack(bref));
  }
  if (rlm_relaxation_report_cfg_r17_present) {
    HANDLE_CODE(rlm_relaxation_report_cfg_r17.unpack(bref));
  }
  if (bfd_relaxation_report_cfg_r17_present) {
    HANDLE_CODE(bfd_relaxation_report_cfg_r17.unpack(bref));
  }
  if (scg_deactivation_pref_cfg_r17_present) {
    HANDLE_CODE(scg_deactivation_pref_cfg_r17.unpack(bref));
  }
  if (rrm_meas_relaxation_report_cfg_r17_present) {
    HANDLE_CODE(rrm_meas_relaxation_report_cfg_r17.unpack(bref));
  }
  if (prop_delay_diff_report_cfg_r17_present) {
    HANDLE_CODE(prop_delay_diff_report_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void other_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_gap_fr2_pref_cfg_r17_present) {
    j.write_str("ul-GapFR2-PreferenceConfig-r17", "true");
  }
  if (musim_gap_assist_cfg_r17_present) {
    j.write_fieldname("musim-GapAssistanceConfig-r17");
    musim_gap_assist_cfg_r17.to_json(j);
  }
  if (musim_leave_assist_cfg_r17_present) {
    j.write_fieldname("musim-LeaveAssistanceConfig-r17");
    musim_leave_assist_cfg_r17.to_json(j);
  }
  if (success_ho_cfg_r17_present) {
    j.write_fieldname("successHO-Config-r17");
    success_ho_cfg_r17.to_json(j);
  }
  if (max_bw_pref_cfg_fr2_2_r17_present) {
    j.write_str("maxBW-PreferenceConfigFR2-2-r17", "true");
  }
  if (max_mimo_layer_pref_cfg_fr2_2_r17_present) {
    j.write_str("maxMIMO-LayerPreferenceConfigFR2-2-r17", "true");
  }
  if (min_sched_offset_pref_cfg_ext_r17_present) {
    j.write_str("minSchedulingOffsetPreferenceConfigExt-r17", "true");
  }
  if (rlm_relaxation_report_cfg_r17_present) {
    j.write_fieldname("rlm-RelaxationReportingConfig-r17");
    rlm_relaxation_report_cfg_r17.to_json(j);
  }
  if (bfd_relaxation_report_cfg_r17_present) {
    j.write_fieldname("bfd-RelaxationReportingConfig-r17");
    bfd_relaxation_report_cfg_r17.to_json(j);
  }
  if (scg_deactivation_pref_cfg_r17_present) {
    j.write_fieldname("scg-DeactivationPreferenceConfig-r17");
    scg_deactivation_pref_cfg_r17.to_json(j);
  }
  if (rrm_meas_relaxation_report_cfg_r17_present) {
    j.write_fieldname("rrm-MeasRelaxationReportingConfig-r17");
    rrm_meas_relaxation_report_cfg_r17.to_json(j);
  }
  if (prop_delay_diff_report_cfg_r17_present) {
    j.write_fieldname("propDelayDiffReportConfig-r17");
    prop_delay_diff_report_cfg_r17.to_json(j);
  }
  j.end_obj();
}

// SL-ConfigDedicatedEUTRA-Info-r16 ::= SEQUENCE
SRSASN_CODE sl_cfg_ded_eutra_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_cfg_ded_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_time_offset_eutra_list_r16_present, 1));

  if (sl_cfg_ded_eutra_r16.size() > 0) {
    HANDLE_CODE(sl_cfg_ded_eutra_r16.pack(bref));
  }
  if (sl_time_offset_eutra_list_r16_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, sl_time_offset_eutra_list_r16, sl_time_offset_eutra_list_r16.size()));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cfg_ded_eutra_info_r16_s::unpack(cbit_ref& bref)
{
  bool sl_cfg_ded_eutra_r16_present;
  HANDLE_CODE(bref.unpack(sl_cfg_ded_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_time_offset_eutra_list_r16_present, 1));

  if (sl_cfg_ded_eutra_r16_present) {
    HANDLE_CODE(sl_cfg_ded_eutra_r16.unpack(bref));
  }
  if (sl_time_offset_eutra_list_r16_present) {
    HANDLE_CODE(unpack_fixed_seq_of(sl_time_offset_eutra_list_r16, bref, sl_time_offset_eutra_list_r16.size()));
  }

  return SRSASN_SUCCESS;
}
void sl_cfg_ded_eutra_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_cfg_ded_eutra_r16.size() > 0) {
    j.write_str("sl-ConfigDedicatedEUTRA-r16", sl_cfg_ded_eutra_r16.to_string());
  }
  if (sl_time_offset_eutra_list_r16_present) {
    j.start_array("sl-TimeOffsetEUTRA-List-r16");
    for (const auto& e1 : sl_time_offset_eutra_list_r16) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// T316-r16 ::= ENUMERATED
const char* t316_r16_opts::to_string() const
{
  static const char* names[] = {
      "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 10, value, "t316_r16_e");
}
uint16_t t316_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 200, 300, 400, 500, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 10, value, "t316_r16_e");
}

// ValidityArea-r16 ::= SEQUENCE
SRSASN_CODE validity_area_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(validity_cell_list_r16.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  if (validity_cell_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, validity_cell_list_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE validity_area_r16_s::unpack(cbit_ref& bref)
{
  bool validity_cell_list_r16_present;
  HANDLE_CODE(bref.unpack(validity_cell_list_r16_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (validity_cell_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(validity_cell_list_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void validity_area_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  if (validity_cell_list_r16.size() > 0) {
    j.start_array("validityCellList-r16");
    for (const auto& e1 : validity_cell_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// ConditionalReconfiguration-r16 ::= SEQUENCE
SRSASN_CODE conditional_recfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(attempt_cond_recfg_r16_present, 1));
  HANDLE_CODE(bref.pack(cond_recfg_to_rem_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(cond_recfg_to_add_mod_list_r16.size() > 0, 1));

  if (cond_recfg_to_rem_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cond_recfg_to_rem_list_r16, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (cond_recfg_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cond_recfg_to_add_mod_list_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE conditional_recfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(attempt_cond_recfg_r16_present, 1));
  bool cond_recfg_to_rem_list_r16_present;
  HANDLE_CODE(bref.unpack(cond_recfg_to_rem_list_r16_present, 1));
  bool cond_recfg_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(cond_recfg_to_add_mod_list_r16_present, 1));

  if (cond_recfg_to_rem_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cond_recfg_to_rem_list_r16, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (cond_recfg_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cond_recfg_to_add_mod_list_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void conditional_recfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (attempt_cond_recfg_r16_present) {
    j.write_str("attemptCondReconfig-r16", "true");
  }
  if (cond_recfg_to_rem_list_r16.size() > 0) {
    j.start_array("condReconfigToRemoveList-r16");
    for (const auto& e1 : cond_recfg_to_rem_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cond_recfg_to_add_mod_list_r16.size() > 0) {
    j.start_array("condReconfigToAddModList-r16");
    for (const auto& e1 : cond_recfg_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// IAB-IP-AddressConfigurationList-r16 ::= SEQUENCE
SRSASN_CODE iab_ip_address_cfg_list_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(iab_ip_address_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(iab_ip_address_to_release_list_r16.size() > 0, 1));

  if (iab_ip_address_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, iab_ip_address_to_add_mod_list_r16, 1, 32));
  }
  if (iab_ip_address_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, iab_ip_address_to_release_list_r16, 1, 32, integer_packer<uint8_t>(1, 32)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_ip_address_cfg_list_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool iab_ip_address_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(iab_ip_address_to_add_mod_list_r16_present, 1));
  bool iab_ip_address_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(iab_ip_address_to_release_list_r16_present, 1));

  if (iab_ip_address_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(iab_ip_address_to_add_mod_list_r16, bref, 1, 32));
  }
  if (iab_ip_address_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(iab_ip_address_to_release_list_r16, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }

  return SRSASN_SUCCESS;
}
void iab_ip_address_cfg_list_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (iab_ip_address_to_add_mod_list_r16.size() > 0) {
    j.start_array("iab-IP-AddressToAddModList-r16");
    for (const auto& e1 : iab_ip_address_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (iab_ip_address_to_release_list_r16.size() > 0) {
    j.start_array("iab-IP-AddressToReleaseList-r16");
    for (const auto& e1 : iab_ip_address_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// MRDC-SecondaryCellGroupConfig ::= SEQUENCE
SRSASN_CODE mrdc_secondary_cell_group_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mrdc_release_and_add_present, 1));

  HANDLE_CODE(mrdc_secondary_cell_group.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_secondary_cell_group_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mrdc_release_and_add_present, 1));

  HANDLE_CODE(mrdc_secondary_cell_group.unpack(bref));

  return SRSASN_SUCCESS;
}
void mrdc_secondary_cell_group_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mrdc_release_and_add_present) {
    j.write_str("mrdc-ReleaseAndAdd", "true");
  }
  j.write_fieldname("mrdc-SecondaryCellGroup");
  mrdc_secondary_cell_group.to_json(j);
  j.end_obj();
}

void mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::destroy_()
{
  switch (type_) {
    case types::nr_scg:
      c.destroy<dyn_octstring>();
      break;
    case types::eutra_scg:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_scg:
      c.init<dyn_octstring>();
      break;
    case types::eutra_scg:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_");
  }
}
mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::mrdc_secondary_cell_group_c_(
    const mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_scg:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_");
  }
}
mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_&
mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::operator=(
    const mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_scg:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_");
  }

  return *this;
}
dyn_octstring& mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::set_nr_scg()
{
  set(types::nr_scg);
  return c.get<dyn_octstring>();
}
dyn_octstring& mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::set_eutra_scg()
{
  set(types::eutra_scg);
  return c.get<dyn_octstring>();
}
void mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_scg:
      j.write_str("nr-SCG", c.get<dyn_octstring>().to_string());
      break;
    case types::eutra_scg:
      j.write_str("eutra-SCG", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_");
  }
  j.end_obj();
}
SRSASN_CODE mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_scg:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::eutra_scg:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_scg:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::eutra_scg:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG", "eutra-SCG"};
  return convert_enum_idx(names, 2, value, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::types");
}

// OtherConfig-v1610 ::= SEQUENCE
SRSASN_CODE other_cfg_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idc_assist_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(drx_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(max_bw_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(max_cc_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(max_mimo_layer_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(min_sched_offset_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(release_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(ref_time_pref_report_r16_present, 1));
  HANDLE_CODE(bref.pack(bt_name_list_r16_present, 1));
  HANDLE_CODE(bref.pack(wlan_name_list_r16_present, 1));
  HANDLE_CODE(bref.pack(sensor_name_list_r16_present, 1));
  HANDLE_CODE(bref.pack(obtain_common_location_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_assist_cfg_nr_r16_present, 1));

  if (idc_assist_cfg_r16_present) {
    HANDLE_CODE(idc_assist_cfg_r16.pack(bref));
  }
  if (drx_pref_cfg_r16_present) {
    HANDLE_CODE(drx_pref_cfg_r16.pack(bref));
  }
  if (max_bw_pref_cfg_r16_present) {
    HANDLE_CODE(max_bw_pref_cfg_r16.pack(bref));
  }
  if (max_cc_pref_cfg_r16_present) {
    HANDLE_CODE(max_cc_pref_cfg_r16.pack(bref));
  }
  if (max_mimo_layer_pref_cfg_r16_present) {
    HANDLE_CODE(max_mimo_layer_pref_cfg_r16.pack(bref));
  }
  if (min_sched_offset_pref_cfg_r16_present) {
    HANDLE_CODE(min_sched_offset_pref_cfg_r16.pack(bref));
  }
  if (release_pref_cfg_r16_present) {
    HANDLE_CODE(release_pref_cfg_r16.pack(bref));
  }
  if (bt_name_list_r16_present) {
    HANDLE_CODE(bt_name_list_r16.pack(bref));
  }
  if (wlan_name_list_r16_present) {
    HANDLE_CODE(wlan_name_list_r16.pack(bref));
  }
  if (sensor_name_list_r16_present) {
    HANDLE_CODE(sensor_name_list_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idc_assist_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(drx_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(max_bw_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(max_cc_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(max_mimo_layer_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(min_sched_offset_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(release_pref_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(ref_time_pref_report_r16_present, 1));
  HANDLE_CODE(bref.unpack(bt_name_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(wlan_name_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sensor_name_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(obtain_common_location_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_assist_cfg_nr_r16_present, 1));

  if (idc_assist_cfg_r16_present) {
    HANDLE_CODE(idc_assist_cfg_r16.unpack(bref));
  }
  if (drx_pref_cfg_r16_present) {
    HANDLE_CODE(drx_pref_cfg_r16.unpack(bref));
  }
  if (max_bw_pref_cfg_r16_present) {
    HANDLE_CODE(max_bw_pref_cfg_r16.unpack(bref));
  }
  if (max_cc_pref_cfg_r16_present) {
    HANDLE_CODE(max_cc_pref_cfg_r16.unpack(bref));
  }
  if (max_mimo_layer_pref_cfg_r16_present) {
    HANDLE_CODE(max_mimo_layer_pref_cfg_r16.unpack(bref));
  }
  if (min_sched_offset_pref_cfg_r16_present) {
    HANDLE_CODE(min_sched_offset_pref_cfg_r16.unpack(bref));
  }
  if (release_pref_cfg_r16_present) {
    HANDLE_CODE(release_pref_cfg_r16.unpack(bref));
  }
  if (bt_name_list_r16_present) {
    HANDLE_CODE(bt_name_list_r16.unpack(bref));
  }
  if (wlan_name_list_r16_present) {
    HANDLE_CODE(wlan_name_list_r16.unpack(bref));
  }
  if (sensor_name_list_r16_present) {
    HANDLE_CODE(sensor_name_list_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void other_cfg_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idc_assist_cfg_r16_present) {
    j.write_fieldname("idc-AssistanceConfig-r16");
    idc_assist_cfg_r16.to_json(j);
  }
  if (drx_pref_cfg_r16_present) {
    j.write_fieldname("drx-PreferenceConfig-r16");
    drx_pref_cfg_r16.to_json(j);
  }
  if (max_bw_pref_cfg_r16_present) {
    j.write_fieldname("maxBW-PreferenceConfig-r16");
    max_bw_pref_cfg_r16.to_json(j);
  }
  if (max_cc_pref_cfg_r16_present) {
    j.write_fieldname("maxCC-PreferenceConfig-r16");
    max_cc_pref_cfg_r16.to_json(j);
  }
  if (max_mimo_layer_pref_cfg_r16_present) {
    j.write_fieldname("maxMIMO-LayerPreferenceConfig-r16");
    max_mimo_layer_pref_cfg_r16.to_json(j);
  }
  if (min_sched_offset_pref_cfg_r16_present) {
    j.write_fieldname("minSchedulingOffsetPreferenceConfig-r16");
    min_sched_offset_pref_cfg_r16.to_json(j);
  }
  if (release_pref_cfg_r16_present) {
    j.write_fieldname("releasePreferenceConfig-r16");
    release_pref_cfg_r16.to_json(j);
  }
  if (ref_time_pref_report_r16_present) {
    j.write_str("referenceTimePreferenceReporting-r16", "true");
  }
  if (bt_name_list_r16_present) {
    j.write_fieldname("btNameList-r16");
    bt_name_list_r16.to_json(j);
  }
  if (wlan_name_list_r16_present) {
    j.write_fieldname("wlanNameList-r16");
    wlan_name_list_r16.to_json(j);
  }
  if (sensor_name_list_r16_present) {
    j.write_fieldname("sensorNameList-r16");
    sensor_name_list_r16.to_json(j);
  }
  if (obtain_common_location_r16_present) {
    j.write_str("obtainCommonLocation-r16", "true");
  }
  if (sl_assist_cfg_nr_r16_present) {
    j.write_str("sl-AssistanceConfigNR-r16", "true");
  }
  j.end_obj();
}

// OverheatingAssistanceConfig ::= SEQUENCE
SRSASN_CODE overheat_assist_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(overheat_ind_prohibit_timer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE overheat_assist_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(overheat_ind_prohibit_timer.unpack(bref));

  return SRSASN_SUCCESS;
}
void overheat_assist_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("overheatingIndicationProhibitTimer", overheat_ind_prohibit_timer.to_string());
  j.end_obj();
}

const char* overheat_assist_cfg_s::overheat_ind_prohibit_timer_opts::to_string() const
{
  static const char* names[] = {"s0",
                                "s0dot5",
                                "s1",
                                "s2",
                                "s5",
                                "s10",
                                "s20",
                                "s30",
                                "s60",
                                "s90",
                                "s120",
                                "s300",
                                "s600",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "overheat_assist_cfg_s::overheat_ind_prohibit_timer_e_");
}
float overheat_assist_cfg_s::overheat_ind_prohibit_timer_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(numbers, 13, value, "overheat_assist_cfg_s::overheat_ind_prohibit_timer_e_");
}
const char* overheat_assist_cfg_s::overheat_ind_prohibit_timer_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(number_strs, 16, value, "overheat_assist_cfg_s::overheat_ind_prohibit_timer_e_");
}

// RAN-AreaConfig ::= SEQUENCE
SRSASN_CODE ran_area_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ran_area_code_list.size() > 0, 1));

  HANDLE_CODE(tac.pack(bref));
  if (ran_area_code_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_area_code_list, 1, 32, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_area_cfg_s::unpack(cbit_ref& bref)
{
  bool ran_area_code_list_present;
  HANDLE_CODE(bref.unpack(ran_area_code_list_present, 1));

  HANDLE_CODE(tac.unpack(bref));
  if (ran_area_code_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ran_area_code_list, bref, 1, 32, integer_packer<uint16_t>(0, 255)));
  }

  return SRSASN_SUCCESS;
}
void ran_area_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("trackingAreaCode", tac.to_string());
  if (ran_area_code_list.size() > 0) {
    j.start_array("ran-AreaCodeList");
    for (const auto& e1 : ran_area_code_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// RRCReconfiguration-v1700-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(other_cfg_v1700_present, 1));
  HANDLE_CODE(bref.pack(sl_l2_relay_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_l2_remote_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(ded_paging_delivery_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_cfg_nr_r17_present, 1));
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_cfg_eutra_r17_present, 1));
  HANDLE_CODE(bref.pack(musim_gap_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(ul_gap_fr2_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(scg_state_r17_present, 1));
  HANDLE_CODE(bref.pack(app_layer_meas_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(ue_tx_teg_request_ul_tdoa_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (other_cfg_v1700_present) {
    HANDLE_CODE(other_cfg_v1700.pack(bref));
  }
  if (sl_l2_relay_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_relay_ue_cfg_r17.pack(bref));
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.pack(bref));
  }
  if (ded_paging_delivery_r17.size() > 0) {
    HANDLE_CODE(ded_paging_delivery_r17.pack(bref));
  }
  if (need_for_gap_ncsg_cfg_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_nr_r17.pack(bref));
  }
  if (need_for_gap_ncsg_cfg_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_eutra_r17.pack(bref));
  }
  if (musim_gap_cfg_r17_present) {
    HANDLE_CODE(musim_gap_cfg_r17.pack(bref));
  }
  if (ul_gap_fr2_cfg_r17_present) {
    HANDLE_CODE(ul_gap_fr2_cfg_r17.pack(bref));
  }
  if (app_layer_meas_cfg_r17_present) {
    HANDLE_CODE(app_layer_meas_cfg_r17.pack(bref));
  }
  if (ue_tx_teg_request_ul_tdoa_cfg_r17_present) {
    HANDLE_CODE(ue_tx_teg_request_ul_tdoa_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(other_cfg_v1700_present, 1));
  HANDLE_CODE(bref.unpack(sl_l2_relay_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_l2_remote_ue_cfg_r17_present, 1));
  bool ded_paging_delivery_r17_present;
  HANDLE_CODE(bref.unpack(ded_paging_delivery_r17_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_cfg_nr_r17_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_cfg_eutra_r17_present, 1));
  HANDLE_CODE(bref.unpack(musim_gap_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(ul_gap_fr2_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(scg_state_r17_present, 1));
  HANDLE_CODE(bref.unpack(app_layer_meas_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(ue_tx_teg_request_ul_tdoa_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (other_cfg_v1700_present) {
    HANDLE_CODE(other_cfg_v1700.unpack(bref));
  }
  if (sl_l2_relay_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_relay_ue_cfg_r17.unpack(bref));
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.unpack(bref));
  }
  if (ded_paging_delivery_r17_present) {
    HANDLE_CODE(ded_paging_delivery_r17.unpack(bref));
  }
  if (need_for_gap_ncsg_cfg_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_nr_r17.unpack(bref));
  }
  if (need_for_gap_ncsg_cfg_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_eutra_r17.unpack(bref));
  }
  if (musim_gap_cfg_r17_present) {
    HANDLE_CODE(musim_gap_cfg_r17.unpack(bref));
  }
  if (ul_gap_fr2_cfg_r17_present) {
    HANDLE_CODE(ul_gap_fr2_cfg_r17.unpack(bref));
  }
  if (app_layer_meas_cfg_r17_present) {
    HANDLE_CODE(app_layer_meas_cfg_r17.unpack(bref));
  }
  if (ue_tx_teg_request_ul_tdoa_cfg_r17_present) {
    HANDLE_CODE(ue_tx_teg_request_ul_tdoa_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (other_cfg_v1700_present) {
    j.write_fieldname("otherConfig-v1700");
    other_cfg_v1700.to_json(j);
  }
  if (sl_l2_relay_ue_cfg_r17_present) {
    j.write_fieldname("sl-L2RelayUE-Config-r17");
    sl_l2_relay_ue_cfg_r17.to_json(j);
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    j.write_fieldname("sl-L2RemoteUE-Config-r17");
    sl_l2_remote_ue_cfg_r17.to_json(j);
  }
  if (ded_paging_delivery_r17.size() > 0) {
    j.write_str("dedicatedPagingDelivery-r17", ded_paging_delivery_r17.to_string());
  }
  if (need_for_gap_ncsg_cfg_nr_r17_present) {
    j.write_fieldname("needForGapNCSG-ConfigNR-r17");
    need_for_gap_ncsg_cfg_nr_r17.to_json(j);
  }
  if (need_for_gap_ncsg_cfg_eutra_r17_present) {
    j.write_fieldname("needForGapNCSG-ConfigEUTRA-r17");
    need_for_gap_ncsg_cfg_eutra_r17.to_json(j);
  }
  if (musim_gap_cfg_r17_present) {
    j.write_fieldname("musim-GapConfig-r17");
    musim_gap_cfg_r17.to_json(j);
  }
  if (ul_gap_fr2_cfg_r17_present) {
    j.write_fieldname("ul-GapFR2-Config-r17");
    ul_gap_fr2_cfg_r17.to_json(j);
  }
  if (scg_state_r17_present) {
    j.write_str("scg-State-r17", "deactivated");
  }
  if (app_layer_meas_cfg_r17_present) {
    j.write_fieldname("appLayerMeasConfig-r17");
    app_layer_meas_cfg_r17.to_json(j);
  }
  if (ue_tx_teg_request_ul_tdoa_cfg_r17_present) {
    j.write_fieldname("ue-TxTEG-RequestUL-TDOA-Config-r17");
    ue_tx_teg_request_ul_tdoa_cfg_r17.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SliceInfoDedicated-r17 ::= SEQUENCE
SRSASN_CODE slice_info_ded_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nsag_cell_resel_prio_r17_present, 1));
  HANDLE_CODE(bref.pack(nsag_cell_resel_sub_prio_r17_present, 1));

  HANDLE_CODE(nsag_id_info_r17.pack(bref));
  if (nsag_cell_resel_prio_r17_present) {
    HANDLE_CODE(pack_integer(bref, nsag_cell_resel_prio_r17, (uint8_t)0u, (uint8_t)7u));
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    HANDLE_CODE(nsag_cell_resel_sub_prio_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE slice_info_ded_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nsag_cell_resel_prio_r17_present, 1));
  HANDLE_CODE(bref.unpack(nsag_cell_resel_sub_prio_r17_present, 1));

  HANDLE_CODE(nsag_id_info_r17.unpack(bref));
  if (nsag_cell_resel_prio_r17_present) {
    HANDLE_CODE(unpack_integer(nsag_cell_resel_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    HANDLE_CODE(nsag_cell_resel_sub_prio_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void slice_info_ded_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nsag-IdentityInfo-r17");
  nsag_id_info_r17.to_json(j);
  if (nsag_cell_resel_prio_r17_present) {
    j.write_int("nsag-CellReselectionPriority-r17", nsag_cell_resel_prio_r17);
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    j.write_str("nsag-CellReselectionSubPriority-r17", nsag_cell_resel_sub_prio_r17.to_string());
  }
  j.end_obj();
}

// TrackingAreaIdentity-r16 ::= SEQUENCE
SRSASN_CODE tracking_area_id_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r16.pack(bref));
  HANDLE_CODE(tac_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tracking_area_id_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r16.unpack(bref));
  HANDLE_CODE(tac_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void tracking_area_id_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-r16");
  plmn_id_r16.to_json(j);
  j.write_str("trackingAreaCode-r16", tac_r16.to_string());
  j.end_obj();
}

// MeasIdleConfigDedicated-r16 ::= SEQUENCE
SRSASN_CODE meas_idle_cfg_ded_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
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
SRSASN_CODE meas_idle_cfg_ded_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
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
void meas_idle_cfg_ded_r16_s::to_json(json_writer& j) const
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

const char* meas_idle_cfg_ded_r16_s::meas_idle_dur_r16_opts::to_string() const
{
  static const char* names[] = {"sec10", "sec30", "sec60", "sec120", "sec180", "sec240", "sec300", "spare"};
  return convert_enum_idx(names, 8, value, "meas_idle_cfg_ded_r16_s::meas_idle_dur_r16_e_");
}
uint16_t meas_idle_cfg_ded_r16_s::meas_idle_dur_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 30, 60, 120, 180, 240, 300};
  return map_enum_number(numbers, 7, value, "meas_idle_cfg_ded_r16_s::meas_idle_dur_r16_e_");
}

// PLMN-RAN-AreaCell ::= SEQUENCE
SRSASN_CODE plmn_ran_area_cell_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ran_area_cells, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_ran_area_cell_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ran_area_cells, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void plmn_ran_area_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_present) {
    j.write_fieldname("plmn-Identity");
    plmn_id.to_json(j);
  }
  j.start_array("ran-AreaCells");
  for (const auto& e1 : ran_area_cells) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// PLMN-RAN-AreaConfig ::= SEQUENCE
SRSASN_CODE plmn_ran_area_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ran_area, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_ran_area_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ran_area, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void plmn_ran_area_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_present) {
    j.write_fieldname("plmn-Identity");
    plmn_id.to_json(j);
  }
  j.start_array("ran-Area");
  for (const auto& e1 : ran_area) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RRCReconfiguration-v1610-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(other_cfg_v1610_present, 1));
  HANDLE_CODE(bref.pack(bap_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(iab_ip_address_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.pack(conditional_recfg_r16_present, 1));
  HANDLE_CODE(bref.pack(daps_source_release_r16_present, 1));
  HANDLE_CODE(bref.pack(t316_r16_present, 1));
  HANDLE_CODE(bref.pack(need_for_gaps_cfg_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(on_demand_sib_request_r16_present, 1));
  HANDLE_CODE(bref.pack(ded_pos_sys_info_delivery_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_cfg_ded_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_cfg_ded_eutra_info_r16_present, 1));
  HANDLE_CODE(bref.pack(target_cell_smtc_scg_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (other_cfg_v1610_present) {
    HANDLE_CODE(other_cfg_v1610.pack(bref));
  }
  if (bap_cfg_r16_present) {
    HANDLE_CODE(bap_cfg_r16.pack(bref));
  }
  if (iab_ip_address_cfg_list_r16_present) {
    HANDLE_CODE(iab_ip_address_cfg_list_r16.pack(bref));
  }
  if (conditional_recfg_r16_present) {
    HANDLE_CODE(conditional_recfg_r16.pack(bref));
  }
  if (t316_r16_present) {
    HANDLE_CODE(t316_r16.pack(bref));
  }
  if (need_for_gaps_cfg_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_cfg_nr_r16.pack(bref));
  }
  if (on_demand_sib_request_r16_present) {
    HANDLE_CODE(on_demand_sib_request_r16.pack(bref));
  }
  if (ded_pos_sys_info_delivery_r16.size() > 0) {
    HANDLE_CODE(ded_pos_sys_info_delivery_r16.pack(bref));
  }
  if (sl_cfg_ded_nr_r16_present) {
    HANDLE_CODE(sl_cfg_ded_nr_r16.pack(bref));
  }
  if (sl_cfg_ded_eutra_info_r16_present) {
    HANDLE_CODE(sl_cfg_ded_eutra_info_r16.pack(bref));
  }
  if (target_cell_smtc_scg_r16_present) {
    HANDLE_CODE(target_cell_smtc_scg_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(other_cfg_v1610_present, 1));
  HANDLE_CODE(bref.unpack(bap_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(iab_ip_address_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(conditional_recfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(daps_source_release_r16_present, 1));
  HANDLE_CODE(bref.unpack(t316_r16_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gaps_cfg_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(on_demand_sib_request_r16_present, 1));
  bool ded_pos_sys_info_delivery_r16_present;
  HANDLE_CODE(bref.unpack(ded_pos_sys_info_delivery_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_cfg_ded_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_cfg_ded_eutra_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(target_cell_smtc_scg_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (other_cfg_v1610_present) {
    HANDLE_CODE(other_cfg_v1610.unpack(bref));
  }
  if (bap_cfg_r16_present) {
    HANDLE_CODE(bap_cfg_r16.unpack(bref));
  }
  if (iab_ip_address_cfg_list_r16_present) {
    HANDLE_CODE(iab_ip_address_cfg_list_r16.unpack(bref));
  }
  if (conditional_recfg_r16_present) {
    HANDLE_CODE(conditional_recfg_r16.unpack(bref));
  }
  if (t316_r16_present) {
    HANDLE_CODE(t316_r16.unpack(bref));
  }
  if (need_for_gaps_cfg_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_cfg_nr_r16.unpack(bref));
  }
  if (on_demand_sib_request_r16_present) {
    HANDLE_CODE(on_demand_sib_request_r16.unpack(bref));
  }
  if (ded_pos_sys_info_delivery_r16_present) {
    HANDLE_CODE(ded_pos_sys_info_delivery_r16.unpack(bref));
  }
  if (sl_cfg_ded_nr_r16_present) {
    HANDLE_CODE(sl_cfg_ded_nr_r16.unpack(bref));
  }
  if (sl_cfg_ded_eutra_info_r16_present) {
    HANDLE_CODE(sl_cfg_ded_eutra_info_r16.unpack(bref));
  }
  if (target_cell_smtc_scg_r16_present) {
    HANDLE_CODE(target_cell_smtc_scg_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (other_cfg_v1610_present) {
    j.write_fieldname("otherConfig-v1610");
    other_cfg_v1610.to_json(j);
  }
  if (bap_cfg_r16_present) {
    j.write_fieldname("bap-Config-r16");
    bap_cfg_r16.to_json(j);
  }
  if (iab_ip_address_cfg_list_r16_present) {
    j.write_fieldname("iab-IP-AddressConfigurationList-r16");
    iab_ip_address_cfg_list_r16.to_json(j);
  }
  if (conditional_recfg_r16_present) {
    j.write_fieldname("conditionalReconfiguration-r16");
    conditional_recfg_r16.to_json(j);
  }
  if (daps_source_release_r16_present) {
    j.write_str("daps-SourceRelease-r16", "true");
  }
  if (t316_r16_present) {
    j.write_fieldname("t316-r16");
    t316_r16.to_json(j);
  }
  if (need_for_gaps_cfg_nr_r16_present) {
    j.write_fieldname("needForGapsConfigNR-r16");
    need_for_gaps_cfg_nr_r16.to_json(j);
  }
  if (on_demand_sib_request_r16_present) {
    j.write_fieldname("onDemandSIB-Request-r16");
    on_demand_sib_request_r16.to_json(j);
  }
  if (ded_pos_sys_info_delivery_r16.size() > 0) {
    j.write_str("dedicatedPosSysInfoDelivery-r16", ded_pos_sys_info_delivery_r16.to_string());
  }
  if (sl_cfg_ded_nr_r16_present) {
    j.write_fieldname("sl-ConfigDedicatedNR-r16");
    sl_cfg_ded_nr_r16.to_json(j);
  }
  if (sl_cfg_ded_eutra_info_r16_present) {
    j.write_fieldname("sl-ConfigDedicatedEUTRA-Info-r16");
    sl_cfg_ded_eutra_info_r16.to_json(j);
  }
  if (target_cell_smtc_scg_r16_present) {
    j.write_fieldname("targetCellSMTC-SCG-r16");
    target_cell_smtc_scg_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCRelease-v1710-IEs ::= SEQUENCE
SRSASN_CODE rrc_release_v1710_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(no_last_cell_upd_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_release_v1710_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(no_last_cell_upd_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_release_v1710_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (no_last_cell_upd_r17_present) {
    j.write_str("noLastCellUpdate-r17", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// AreaConfig-r16 ::= CHOICE
void area_cfg_r16_c::destroy_()
{
  switch (type_) {
    case types::cell_global_id_list_r16:
      c.destroy<cell_global_id_list_r16_l>();
      break;
    case types::tac_list_r16:
      c.destroy<tac_list_r16_l>();
      break;
    case types::tracking_area_id_list_r16:
      c.destroy<tracking_area_id_list_r16_l>();
      break;
    default:
      break;
  }
}
void area_cfg_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_global_id_list_r16:
      c.init<cell_global_id_list_r16_l>();
      break;
    case types::tac_list_r16:
      c.init<tac_list_r16_l>();
      break;
    case types::tracking_area_id_list_r16:
      c.init<tracking_area_id_list_r16_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r16_c");
  }
}
area_cfg_r16_c::area_cfg_r16_c(const area_cfg_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_global_id_list_r16:
      c.init(other.c.get<cell_global_id_list_r16_l>());
      break;
    case types::tac_list_r16:
      c.init(other.c.get<tac_list_r16_l>());
      break;
    case types::tracking_area_id_list_r16:
      c.init(other.c.get<tracking_area_id_list_r16_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r16_c");
  }
}
area_cfg_r16_c& area_cfg_r16_c::operator=(const area_cfg_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_global_id_list_r16:
      c.set(other.c.get<cell_global_id_list_r16_l>());
      break;
    case types::tac_list_r16:
      c.set(other.c.get<tac_list_r16_l>());
      break;
    case types::tracking_area_id_list_r16:
      c.set(other.c.get<tracking_area_id_list_r16_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r16_c");
  }

  return *this;
}
cell_global_id_list_r16_l& area_cfg_r16_c::set_cell_global_id_list_r16()
{
  set(types::cell_global_id_list_r16);
  return c.get<cell_global_id_list_r16_l>();
}
tac_list_r16_l& area_cfg_r16_c::set_tac_list_r16()
{
  set(types::tac_list_r16);
  return c.get<tac_list_r16_l>();
}
tracking_area_id_list_r16_l& area_cfg_r16_c::set_tracking_area_id_list_r16()
{
  set(types::tracking_area_id_list_r16);
  return c.get<tracking_area_id_list_r16_l>();
}
void area_cfg_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_global_id_list_r16:
      j.start_array("cellGlobalIdList-r16");
      for (const auto& e1 : c.get<cell_global_id_list_r16_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::tac_list_r16:
      j.start_array("trackingAreaCodeList-r16");
      for (const auto& e1 : c.get<tac_list_r16_l>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    case types::tracking_area_id_list_r16:
      j.start_array("trackingAreaIdentityList-r16");
      for (const auto& e1 : c.get<tracking_area_id_list_r16_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE area_cfg_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_global_id_list_r16:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<cell_global_id_list_r16_l>(), 1, 32));
      break;
    case types::tac_list_r16:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<tac_list_r16_l>(), 1, 8));
      break;
    case types::tracking_area_id_list_r16:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<tracking_area_id_list_r16_l>(), 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE area_cfg_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_global_id_list_r16:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<cell_global_id_list_r16_l>(), bref, 1, 32));
      break;
    case types::tac_list_r16:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<tac_list_r16_l>(), bref, 1, 8));
      break;
    case types::tracking_area_id_list_r16:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<tracking_area_id_list_r16_l>(), bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "area_cfg_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* area_cfg_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalIdList-r16", "trackingAreaCodeList-r16", "trackingAreaIdentityList-r16"};
  return convert_enum_idx(names, 3, value, "area_cfg_r16_c::types");
}

// FreqPriorityDedicatedSlicing-r17 ::= SEQUENCE
SRSASN_CODE freq_prio_ded_slicing_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(slice_info_list_ded_r17.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, dl_explicit_carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  if (slice_info_list_ded_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slice_info_list_ded_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_ded_slicing_r17_s::unpack(cbit_ref& bref)
{
  bool slice_info_list_ded_r17_present;
  HANDLE_CODE(bref.unpack(slice_info_list_ded_r17_present, 1));

  HANDLE_CODE(unpack_integer(dl_explicit_carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (slice_info_list_ded_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slice_info_list_ded_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_ded_slicing_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-ExplicitCarrierFreq-r17", dl_explicit_carrier_freq_r17);
  if (slice_info_list_ded_r17.size() > 0) {
    j.start_array("sliceInfoListDedicated-r17");
    for (const auto& e1 : slice_info_list_ded_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// FreqPriorityEUTRA ::= SEQUENCE
SRSASN_CODE freq_prio_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_int("cellReselectionPriority", cell_resel_prio);
  if (cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_sub_prio.to_string());
  }
  j.end_obj();
}

// FreqPriorityNR ::= SEQUENCE
SRSASN_CODE freq_prio_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_int("cellReselectionPriority", cell_resel_prio);
  if (cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_sub_prio.to_string());
  }
  j.end_obj();
}

// InterFreqTargetInfo-r16 ::= SEQUENCE
SRSASN_CODE inter_freq_target_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_list_r16.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, dl_carrier_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  if (cell_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_list_r16, 1, 32, integer_packer<uint16_t>(0, 1007)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_target_info_r16_s::unpack(cbit_ref& bref)
{
  bool cell_list_r16_present;
  HANDLE_CODE(bref.unpack(cell_list_r16_present, 1));

  HANDLE_CODE(unpack_integer(dl_carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (cell_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_list_r16, bref, 1, 32, integer_packer<uint16_t>(0, 1007)));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_target_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq-r16", dl_carrier_freq_r16);
  if (cell_list_r16.size() > 0) {
    j.start_array("cellList-r16");
    for (const auto& e1 : cell_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// OtherConfig-v1540 ::= SEQUENCE
SRSASN_CODE other_cfg_v1540_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(overheat_assist_cfg_present, 1));

  if (overheat_assist_cfg_present) {
    HANDLE_CODE(overheat_assist_cfg.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_v1540_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(overheat_assist_cfg_present, 1));

  if (overheat_assist_cfg_present) {
    HANDLE_CODE(overheat_assist_cfg.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void other_cfg_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (overheat_assist_cfg_present) {
    j.write_fieldname("overheatingAssistanceConfig");
    overheat_assist_cfg.to_json(j);
  }
  j.end_obj();
}

// RAT-Type ::= ENUMERATED
const char* rat_type_opts::to_string() const
{
  static const char* names[] = {"nr", "eutra-nr", "eutra", "utra-fdd-v1610"};
  return convert_enum_idx(names, 4, value, "rat_type_e");
}

// RRCReconfiguration-v1560-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mrdc_secondary_cell_group_cfg_present, 1));
  HANDLE_CODE(bref.pack(radio_bearer_cfg2.size() > 0, 1));
  HANDLE_CODE(bref.pack(sk_counter_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mrdc_secondary_cell_group_cfg_present) {
    HANDLE_CODE(mrdc_secondary_cell_group_cfg.pack(bref));
  }
  if (radio_bearer_cfg2.size() > 0) {
    HANDLE_CODE(radio_bearer_cfg2.pack(bref));
  }
  if (sk_counter_present) {
    HANDLE_CODE(pack_integer(bref, sk_counter, (uint32_t)0u, (uint32_t)65535u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mrdc_secondary_cell_group_cfg_present, 1));
  bool radio_bearer_cfg2_present;
  HANDLE_CODE(bref.unpack(radio_bearer_cfg2_present, 1));
  HANDLE_CODE(bref.unpack(sk_counter_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mrdc_secondary_cell_group_cfg_present) {
    HANDLE_CODE(mrdc_secondary_cell_group_cfg.unpack(bref));
  }
  if (radio_bearer_cfg2_present) {
    HANDLE_CODE(radio_bearer_cfg2.unpack(bref));
  }
  if (sk_counter_present) {
    HANDLE_CODE(unpack_integer(sk_counter, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mrdc_secondary_cell_group_cfg_present) {
    j.write_fieldname("mrdc-SecondaryCellGroupConfig");
    mrdc_secondary_cell_group_cfg.to_json(j);
  }
  if (radio_bearer_cfg2.size() > 0) {
    j.write_str("radioBearerConfig2", radio_bearer_cfg2.to_string());
  }
  if (sk_counter_present) {
    j.write_int("sk-Counter", sk_counter);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCRelease-v1650-IEs ::= SEQUENCE
SRSASN_CODE rrc_release_v1650_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mps_prio_ind_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_release_v1650_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mps_prio_ind_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_release_v1650_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mps_prio_ind_r16_present) {
    j.write_str("mpsPriorityIndication-r16", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCResume-v1700-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_cfg_ded_nr_r17_present, 1));
  HANDLE_CODE(bref.pack(sl_l2_remote_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_cfg_nr_r17_present, 1));
  HANDLE_CODE(bref.pack(need_for_gap_ncsg_cfg_eutra_r17_present, 1));
  HANDLE_CODE(bref.pack(scg_state_r17_present, 1));
  HANDLE_CODE(bref.pack(app_layer_meas_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sl_cfg_ded_nr_r17_present) {
    HANDLE_CODE(sl_cfg_ded_nr_r17.pack(bref));
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.pack(bref));
  }
  if (need_for_gap_ncsg_cfg_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_nr_r17.pack(bref));
  }
  if (need_for_gap_ncsg_cfg_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_eutra_r17.pack(bref));
  }
  if (app_layer_meas_cfg_r17_present) {
    HANDLE_CODE(app_layer_meas_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_cfg_ded_nr_r17_present, 1));
  HANDLE_CODE(bref.unpack(sl_l2_remote_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_cfg_nr_r17_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gap_ncsg_cfg_eutra_r17_present, 1));
  HANDLE_CODE(bref.unpack(scg_state_r17_present, 1));
  HANDLE_CODE(bref.unpack(app_layer_meas_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sl_cfg_ded_nr_r17_present) {
    HANDLE_CODE(sl_cfg_ded_nr_r17.unpack(bref));
  }
  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.unpack(bref));
  }
  if (need_for_gap_ncsg_cfg_nr_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_nr_r17.unpack(bref));
  }
  if (need_for_gap_ncsg_cfg_eutra_r17_present) {
    HANDLE_CODE(need_for_gap_ncsg_cfg_eutra_r17.unpack(bref));
  }
  if (app_layer_meas_cfg_r17_present) {
    HANDLE_CODE(app_layer_meas_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_v1700_ies_s::to_json(json_writer& j) const
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
  if (need_for_gap_ncsg_cfg_nr_r17_present) {
    j.write_fieldname("needForGapNCSG-ConfigNR-r17");
    need_for_gap_ncsg_cfg_nr_r17.to_json(j);
  }
  if (need_for_gap_ncsg_cfg_eutra_r17_present) {
    j.write_fieldname("needForGapNCSG-ConfigEUTRA-r17");
    need_for_gap_ncsg_cfg_eutra_r17.to_json(j);
  }
  if (scg_state_r17_present) {
    j.write_str("scg-State-r17", "deactivated");
  }
  if (app_layer_meas_cfg_r17_present) {
    j.write_fieldname("appLayerMeasConfig-r17");
    app_layer_meas_cfg_r17.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
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

// SDT-Config-r17 ::= SEQUENCE
SRSASN_CODE sdt_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sdt_drb_list_r17_present, 1));
  HANDLE_CODE(bref.pack(sdt_srb2_ind_r17_present, 1));
  HANDLE_CODE(bref.pack(sdt_mac_phy_cg_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(sdt_drb_continue_rohc_r17_present, 1));

  if (sdt_drb_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sdt_drb_list_r17, 0, 29, integer_packer<uint8_t>(1, 32)));
  }
  if (sdt_mac_phy_cg_cfg_r17_present) {
    HANDLE_CODE(sdt_mac_phy_cg_cfg_r17.pack(bref));
  }
  if (sdt_drb_continue_rohc_r17_present) {
    HANDLE_CODE(sdt_drb_continue_rohc_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdt_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sdt_drb_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(sdt_srb2_ind_r17_present, 1));
  HANDLE_CODE(bref.unpack(sdt_mac_phy_cg_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(sdt_drb_continue_rohc_r17_present, 1));

  if (sdt_drb_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sdt_drb_list_r17, bref, 0, 29, integer_packer<uint8_t>(1, 32)));
  }
  if (sdt_mac_phy_cg_cfg_r17_present) {
    HANDLE_CODE(sdt_mac_phy_cg_cfg_r17.unpack(bref));
  }
  if (sdt_drb_continue_rohc_r17_present) {
    HANDLE_CODE(sdt_drb_continue_rohc_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sdt_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sdt_drb_list_r17_present) {
    j.start_array("sdt-DRB-List-r17");
    for (const auto& e1 : sdt_drb_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sdt_srb2_ind_r17_present) {
    j.write_str("sdt-SRB2-Indication-r17", "allowed");
  }
  if (sdt_mac_phy_cg_cfg_r17_present) {
    j.write_fieldname("sdt-MAC-PHY-CG-Config-r17");
    sdt_mac_phy_cg_cfg_r17.to_json(j);
  }
  if (sdt_drb_continue_rohc_r17_present) {
    j.write_str("sdt-DRB-ContinueROHC-r17", sdt_drb_continue_rohc_r17.to_string());
  }
  j.end_obj();
}

const char* sdt_cfg_r17_s::sdt_drb_continue_rohc_r17_opts::to_string() const
{
  static const char* names[] = {"cell", "rna"};
  return convert_enum_idx(names, 2, value, "sdt_cfg_r17_s::sdt_drb_continue_rohc_r17_e_");
}

// AreaConfiguration-v1700 ::= SEQUENCE
SRSASN_CODE area_cfg_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_target_list_r17.size() > 0, 1));

  if (area_cfg_r17_present) {
    HANDLE_CODE(area_cfg_r17.pack(bref));
  }
  if (inter_freq_target_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_target_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE area_cfg_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r17_present, 1));
  bool inter_freq_target_list_r17_present;
  HANDLE_CODE(bref.unpack(inter_freq_target_list_r17_present, 1));

  if (area_cfg_r17_present) {
    HANDLE_CODE(area_cfg_r17.unpack(bref));
  }
  if (inter_freq_target_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_target_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void area_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (area_cfg_r17_present) {
    j.write_fieldname("areaConfig-r17");
    area_cfg_r17.to_json(j);
  }
  if (inter_freq_target_list_r17.size() > 0) {
    j.start_array("interFreqTargetList-r17");
    for (const auto& e1 : inter_freq_target_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// CarrierInfoNR ::= SEQUENCE
SRSASN_CODE carrier_info_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(smtc_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(ssb_subcarrier_spacing.pack(bref));
  if (smtc_present) {
    HANDLE_CODE(smtc.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_info_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(smtc_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(ssb_subcarrier_spacing.unpack(bref));
  if (smtc_present) {
    HANDLE_CODE(smtc.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void carrier_info_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_str("ssbSubcarrierSpacing", ssb_subcarrier_spacing.to_string());
  if (smtc_present) {
    j.write_fieldname("smtc");
    smtc.to_json(j);
  }
  j.end_obj();
}

// DLInformationTransfer-v1700-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_info_f1c_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(rx_tx_time_diff_gnb_r17_present, 1));
  HANDLE_CODE(bref.pack(ta_pdc_r17_present, 1));
  HANDLE_CODE(bref.pack(sib9_fallback_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_info_f1c_r17.size() > 0) {
    HANDLE_CODE(ded_info_f1c_r17.pack(bref));
  }
  if (rx_tx_time_diff_gnb_r17_present) {
    HANDLE_CODE(rx_tx_time_diff_gnb_r17.pack(bref));
  }
  if (ta_pdc_r17_present) {
    HANDLE_CODE(ta_pdc_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool ded_info_f1c_r17_present;
  HANDLE_CODE(bref.unpack(ded_info_f1c_r17_present, 1));
  HANDLE_CODE(bref.unpack(rx_tx_time_diff_gnb_r17_present, 1));
  HANDLE_CODE(bref.unpack(ta_pdc_r17_present, 1));
  HANDLE_CODE(bref.unpack(sib9_fallback_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_info_f1c_r17_present) {
    HANDLE_CODE(ded_info_f1c_r17.unpack(bref));
  }
  if (rx_tx_time_diff_gnb_r17_present) {
    HANDLE_CODE(rx_tx_time_diff_gnb_r17.unpack(bref));
  }
  if (ta_pdc_r17_present) {
    HANDLE_CODE(ta_pdc_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_info_transfer_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_info_f1c_r17.size() > 0) {
    j.write_str("dedicatedInfoF1c-r17", ded_info_f1c_r17.to_string());
  }
  if (rx_tx_time_diff_gnb_r17_present) {
    j.write_fieldname("rxTxTimeDiff-gNB-r17");
    rx_tx_time_diff_gnb_r17.to_json(j);
  }
  if (ta_pdc_r17_present) {
    j.write_str("ta-PDC-r17", ta_pdc_r17.to_string());
  }
  if (sib9_fallback_r17_present) {
    j.write_str("sib9Fallback-r17", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* dl_info_transfer_v1700_ies_s::ta_pdc_r17_opts::to_string() const
{
  static const char* names[] = {"activate", "deactivate"};
  return convert_enum_idx(names, 2, value, "dl_info_transfer_v1700_ies_s::ta_pdc_r17_e_");
}

// DRB-CountMSB-Info ::= SEQUENCE
SRSASN_CODE drb_count_msb_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, count_msb_ul, (uint32_t)0u, (uint32_t)33554431u));
  HANDLE_CODE(pack_integer(bref, count_msb_dl, (uint32_t)0u, (uint32_t)33554431u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_count_msb_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(count_msb_ul, bref, (uint32_t)0u, (uint32_t)33554431u));
  HANDLE_CODE(unpack_integer(count_msb_dl, bref, (uint32_t)0u, (uint32_t)33554431u));

  return SRSASN_SUCCESS;
}
void drb_count_msb_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Identity", drb_id);
  j.write_int("countMSB-Uplink", count_msb_ul);
  j.write_int("countMSB-Downlink", count_msb_dl);
  j.end_obj();
}

// EventType-r16 ::= CHOICE
void event_type_r16_c::set(types::options e)
{
  type_ = e;
}
void event_type_r16_c::set_out_of_coverage()
{
  set(types::out_of_coverage);
}
event_type_r16_c::event_l1_s_& event_type_r16_c::set_event_l1()
{
  set(types::event_l1);
  return c;
}
void event_type_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::out_of_coverage:
      break;
    case types::event_l1:
      j.write_fieldname("eventL1");
      j.start_obj();
      j.write_fieldname("l1-Threshold");
      c.l1_thres.to_json(j);
      j.write_int("hysteresis", c.hysteresis);
      j.write_str("timeToTrigger", c.time_to_trigger.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "event_type_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE event_type_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::out_of_coverage:
      break;
    case types::event_l1:
      HANDLE_CODE(c.l1_thres.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.time_to_trigger.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "event_type_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE event_type_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::out_of_coverage:
      break;
    case types::event_l1:
      HANDLE_CODE(c.l1_thres.unpack(bref));
      HANDLE_CODE(unpack_integer(c.hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.time_to_trigger.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "event_type_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* event_type_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"outOfCoverage", "eventL1"};
  return convert_enum_idx(names, 2, value, "event_type_r16_c::types");
}
uint8_t event_type_r16_c::types_opts::to_number() const
{
  if (value == event_l1) {
    return 1;
  }
  invalid_enum_number(value, "event_type_r16_c::types");
  return 0;
}

// ExtendedPagingCycle-r17 ::= ENUMERATED
const char* extended_paging_cycle_r17_opts::to_string() const
{
  static const char* names[] = {"rf256", "rf512", "rf1024", "spare1"};
  return convert_enum_idx(names, 4, value, "extended_paging_cycle_r17_e");
}
uint16_t extended_paging_cycle_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 512, 1024};
  return map_enum_number(numbers, 3, value, "extended_paging_cycle_r17_e");
}

// LoggingInterval-r16 ::= ENUMERATED
const char* logging_interv_r16_opts::to_string() const
{
  static const char* names[] = {"ms320",
                                "ms640",
                                "ms1280",
                                "ms2560",
                                "ms5120",
                                "ms10240",
                                "ms20480",
                                "ms30720",
                                "ms40960",
                                "ms61440",
                                "infinity"};
  return convert_enum_idx(names, 11, value, "logging_interv_r16_e");
}
int32_t logging_interv_r16_opts::to_number() const
{
  static const int32_t numbers[] = {320, 640, 1280, 2560, 5120, 10240, 20480, 30720, 40960, 61440, -1};
  return map_enum_number(numbers, 11, value, "logging_interv_r16_e");
}

// MasterKeyUpdate ::= SEQUENCE
SRSASN_CODE master_key_upd_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(nas_container.size() > 0, 1));

  HANDLE_CODE(bref.pack(key_set_change_ind, 1));
  HANDLE_CODE(pack_integer(bref, next_hop_chaining_count, (uint8_t)0u, (uint8_t)7u));
  if (nas_container.size() > 0) {
    HANDLE_CODE(nas_container.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE master_key_upd_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool nas_container_present;
  HANDLE_CODE(bref.unpack(nas_container_present, 1));

  HANDLE_CODE(bref.unpack(key_set_change_ind, 1));
  HANDLE_CODE(unpack_integer(next_hop_chaining_count, bref, (uint8_t)0u, (uint8_t)7u));
  if (nas_container_present) {
    HANDLE_CODE(nas_container.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void master_key_upd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("keySetChangeIndicator", key_set_change_ind);
  j.write_int("nextHopChainingCount", next_hop_chaining_count);
  if (nas_container.size() > 0) {
    j.write_str("nas-Container", nas_container.to_string());
  }
  j.end_obj();
}

// OtherConfig ::= SEQUENCE
SRSASN_CODE other_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delay_budget_report_cfg_present, 1));

  if (delay_budget_report_cfg_present) {
    HANDLE_CODE(delay_budget_report_cfg.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delay_budget_report_cfg_present, 1));

  if (delay_budget_report_cfg_present) {
    HANDLE_CODE(delay_budget_report_cfg.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void other_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (delay_budget_report_cfg_present) {
    j.write_fieldname("delayBudgetReportingConfig");
    delay_budget_report_cfg.to_json(j);
  }
  j.end_obj();
}

void other_cfg_s::delay_budget_report_cfg_c_::set(types::options e)
{
  type_ = e;
}
void other_cfg_s::delay_budget_report_cfg_c_::set_release()
{
  set(types::release);
}
other_cfg_s::delay_budget_report_cfg_c_::setup_s_& other_cfg_s::delay_budget_report_cfg_c_::set_setup()
{
  set(types::setup);
  return c;
}
void other_cfg_s::delay_budget_report_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("delayBudgetReportingProhibitTimer", c.delay_budget_report_prohibit_timer.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_s::delay_budget_report_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE other_cfg_s::delay_budget_report_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.delay_budget_report_prohibit_timer.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_s::delay_budget_report_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE other_cfg_s::delay_budget_report_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.delay_budget_report_prohibit_timer.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "other_cfg_s::delay_budget_report_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_opts::to_string() const
{
  static const char* names[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(
      names, 8, value, "other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_e_");
}
float other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(
      numbers, 8, value, "other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_e_");
}
const char*
other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(number_strs,
                          8,
                          value,
                          "other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_e_");
}

const char* other_cfg_s::delay_budget_report_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"release", "setup"};
  return convert_enum_idx(names, 2, value, "other_cfg_s::delay_budget_report_cfg_c_::types");
}

// PeriodicRNAU-TimerValue ::= ENUMERATED
const char* periodic_rnau_timer_value_opts::to_string() const
{
  static const char* names[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min360", "min720"};
  return convert_enum_idx(names, 8, value, "periodic_rnau_timer_value_e");
}
uint16_t periodic_rnau_timer_value_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 360, 720};
  return map_enum_number(numbers, 8, value, "periodic_rnau_timer_value_e");
}

// RAN-NotificationAreaInfo ::= CHOICE
void ran_notif_area_info_c::destroy_()
{
  switch (type_) {
    case types::cell_list:
      c.destroy<plmn_ran_area_cell_list_l>();
      break;
    case types::ran_area_cfg_list:
      c.destroy<plmn_ran_area_cfg_list_l>();
      break;
    default:
      break;
  }
}
void ran_notif_area_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_list:
      c.init<plmn_ran_area_cell_list_l>();
      break;
    case types::ran_area_cfg_list:
      c.init<plmn_ran_area_cfg_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_c");
  }
}
ran_notif_area_info_c::ran_notif_area_info_c(const ran_notif_area_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_list:
      c.init(other.c.get<plmn_ran_area_cell_list_l>());
      break;
    case types::ran_area_cfg_list:
      c.init(other.c.get<plmn_ran_area_cfg_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_c");
  }
}
ran_notif_area_info_c& ran_notif_area_info_c::operator=(const ran_notif_area_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_list:
      c.set(other.c.get<plmn_ran_area_cell_list_l>());
      break;
    case types::ran_area_cfg_list:
      c.set(other.c.get<plmn_ran_area_cfg_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_c");
  }

  return *this;
}
plmn_ran_area_cell_list_l& ran_notif_area_info_c::set_cell_list()
{
  set(types::cell_list);
  return c.get<plmn_ran_area_cell_list_l>();
}
plmn_ran_area_cfg_list_l& ran_notif_area_info_c::set_ran_area_cfg_list()
{
  set(types::ran_area_cfg_list);
  return c.get<plmn_ran_area_cfg_list_l>();
}
void ran_notif_area_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_list:
      j.start_array("cellList");
      for (const auto& e1 : c.get<plmn_ran_area_cell_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ran_area_cfg_list:
      j.start_array("ran-AreaConfigList");
      for (const auto& e1 : c.get<plmn_ran_area_cfg_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_c");
  }
  j.end_obj();
}
SRSASN_CODE ran_notif_area_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<plmn_ran_area_cell_list_l>(), 1, 8));
      break;
    case types::ran_area_cfg_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<plmn_ran_area_cfg_list_l>(), 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_notif_area_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<plmn_ran_area_cell_list_l>(), bref, 1, 8));
      break;
    case types::ran_area_cfg_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<plmn_ran_area_cfg_list_l>(), bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "ran_notif_area_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ran_notif_area_info_c::types_opts::to_string() const
{
  static const char* names[] = {"cellList", "ran-AreaConfigList"};
  return convert_enum_idx(names, 2, value, "ran_notif_area_info_c::types");
}

// RedirectedCarrierInfo-EUTRA ::= SEQUENCE
SRSASN_CODE redirected_carrier_info_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cn_type_present, 1));

  HANDLE_CODE(pack_integer(bref, eutra_freq, (uint32_t)0u, (uint32_t)262143u));
  if (cn_type_present) {
    HANDLE_CODE(cn_type.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE redirected_carrier_info_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cn_type_present, 1));

  HANDLE_CODE(unpack_integer(eutra_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  if (cn_type_present) {
    HANDLE_CODE(cn_type.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void redirected_carrier_info_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutraFrequency", eutra_freq);
  if (cn_type_present) {
    j.write_str("cnType", cn_type.to_string());
  }
  j.end_obj();
}

const char* redirected_carrier_info_eutra_s::cn_type_opts::to_string() const
{
  static const char* names[] = {"epc", "fiveGC"};
  return convert_enum_idx(names, 2, value, "redirected_carrier_info_eutra_s::cn_type_e_");
}
uint8_t redirected_carrier_info_eutra_s::cn_type_opts::to_number() const
{
  if (value == five_gc) {
    return 5;
  }
  invalid_enum_number(value, "redirected_carrier_info_eutra_s::cn_type_e_");
  return 0;
}

// RRCReconfiguration-v1540-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(other_cfg_v1540_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (other_cfg_v1540_present) {
    HANDLE_CODE(other_cfg_v1540.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(other_cfg_v1540_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (other_cfg_v1540_present) {
    HANDLE_CODE(other_cfg_v1540.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (other_cfg_v1540_present) {
    j.write_fieldname("otherConfig-v1540");
    other_cfg_v1540.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCRelease-v1610-IEs ::= SEQUENCE
SRSASN_CODE rrc_release_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(voice_fallback_ind_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_idle_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_idle_cfg_r16_present) {
    HANDLE_CODE(meas_idle_cfg_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_release_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(voice_fallback_ind_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_idle_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_idle_cfg_r16_present) {
    HANDLE_CODE(meas_idle_cfg_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_release_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (voice_fallback_ind_r16_present) {
    j.write_str("voiceFallbackIndication-r16", "true");
  }
  if (meas_idle_cfg_r16_present) {
    j.write_fieldname("measIdleConfig-r16");
    meas_idle_cfg_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCResume-v1610-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idle_mode_meas_req_r16_present, 1));
  HANDLE_CODE(bref.pack(restore_mcg_scells_r16_present, 1));
  HANDLE_CODE(bref.pack(restore_scg_r16_present, 1));
  HANDLE_CODE(bref.pack(mrdc_secondary_cell_group_r16_present, 1));
  HANDLE_CODE(bref.pack(need_for_gaps_cfg_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mrdc_secondary_cell_group_r16_present) {
    HANDLE_CODE(mrdc_secondary_cell_group_r16.pack(bref));
  }
  if (need_for_gaps_cfg_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_cfg_nr_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idle_mode_meas_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(restore_mcg_scells_r16_present, 1));
  HANDLE_CODE(bref.unpack(restore_scg_r16_present, 1));
  HANDLE_CODE(bref.unpack(mrdc_secondary_cell_group_r16_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gaps_cfg_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mrdc_secondary_cell_group_r16_present) {
    HANDLE_CODE(mrdc_secondary_cell_group_r16.unpack(bref));
  }
  if (need_for_gaps_cfg_nr_r16_present) {
    HANDLE_CODE(need_for_gaps_cfg_nr_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idle_mode_meas_req_r16_present) {
    j.write_str("idleModeMeasurementReq-r16", "true");
  }
  if (restore_mcg_scells_r16_present) {
    j.write_str("restoreMCG-SCells-r16", "true");
  }
  if (restore_scg_r16_present) {
    j.write_str("restoreSCG-r16", "true");
  }
  if (mrdc_secondary_cell_group_r16_present) {
    j.write_fieldname("mrdc-SecondaryCellGroup-r16");
    mrdc_secondary_cell_group_r16.to_json(j);
  }
  if (need_for_gaps_cfg_nr_r16_present) {
    j.write_fieldname("needForGapsConfigNR-r16");
    need_for_gaps_cfg_nr_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::destroy_()
{
  switch (type_) {
    case types::nr_scg_r16:
      c.destroy<dyn_octstring>();
      break;
    case types::eutra_scg_r16:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_scg_r16:
      c.init<dyn_octstring>();
      break;
    case types::eutra_scg_r16:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_");
  }
}
rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::mrdc_secondary_cell_group_r16_c_(
    const rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_scg_r16:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_r16:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_");
  }
}
rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_&
rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::operator=(
    const rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_scg_r16:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_r16:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_");
  }

  return *this;
}
dyn_octstring& rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::set_nr_scg_r16()
{
  set(types::nr_scg_r16);
  return c.get<dyn_octstring>();
}
dyn_octstring& rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::set_eutra_scg_r16()
{
  set(types::eutra_scg_r16);
  return c.get<dyn_octstring>();
}
void rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_scg_r16:
      j.write_str("nr-SCG-r16", c.get<dyn_octstring>().to_string());
      break;
    case types::eutra_scg_r16:
      j.write_str("eutra-SCG-r16", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_scg_r16:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::eutra_scg_r16:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_scg_r16:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::eutra_scg_r16:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG-r16", "eutra-SCG-r16"};
  return convert_enum_idx(names, 2, value, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::types");
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

// UE-CapabilityRAT-Request ::= SEQUENCE
SRSASN_CODE ue_cap_rat_request_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cap_request_filt.size() > 0, 1));

  HANDLE_CODE(rat_type.pack(bref));
  if (cap_request_filt.size() > 0) {
    HANDLE_CODE(cap_request_filt.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_rat_request_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cap_request_filt_present;
  HANDLE_CODE(bref.unpack(cap_request_filt_present, 1));

  HANDLE_CODE(rat_type.unpack(bref));
  if (cap_request_filt_present) {
    HANDLE_CODE(cap_request_filt.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_rat_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rat-Type", rat_type.to_string());
  if (cap_request_filt.size() > 0) {
    j.write_str("capabilityRequestFilter", cap_request_filt.to_string());
  }
  j.end_obj();
}

// AreaConfiguration-r16 ::= SEQUENCE
SRSASN_CODE area_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_target_list_r16.size() > 0, 1));

  HANDLE_CODE(area_cfg_r16.pack(bref));
  if (inter_freq_target_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_target_list_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE area_cfg_r16_s::unpack(cbit_ref& bref)
{
  bool inter_freq_target_list_r16_present;
  HANDLE_CODE(bref.unpack(inter_freq_target_list_r16_present, 1));

  HANDLE_CODE(area_cfg_r16.unpack(bref));
  if (inter_freq_target_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_target_list_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void area_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("areaConfig-r16");
  area_cfg_r16.to_json(j);
  if (inter_freq_target_list_r16.size() > 0) {
    j.start_array("interFreqTargetList-r16");
    for (const auto& e1 : inter_freq_target_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// CellReselectionPriorities ::= SEQUENCE
SRSASN_CODE cell_resel_priorities_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_prio_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_prio_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(t320_present, 1));

  if (freq_prio_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_eutra, 1, 8));
  }
  if (freq_prio_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_nr, 1, 8));
  }
  if (t320_present) {
    HANDLE_CODE(t320.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= freq_prio_list_ded_slicing_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(freq_prio_list_ded_slicing_r17.is_present(), 1));
      if (freq_prio_list_ded_slicing_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *freq_prio_list_ded_slicing_r17, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_priorities_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_prio_list_eutra_present;
  HANDLE_CODE(bref.unpack(freq_prio_list_eutra_present, 1));
  bool freq_prio_list_nr_present;
  HANDLE_CODE(bref.unpack(freq_prio_list_nr_present, 1));
  HANDLE_CODE(bref.unpack(t320_present, 1));

  if (freq_prio_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_eutra, bref, 1, 8));
  }
  if (freq_prio_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_nr, bref, 1, 8));
  }
  if (t320_present) {
    HANDLE_CODE(t320.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(freq_prio_list_ded_slicing_r17, bref);
      if (freq_prio_list_ded_slicing_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*freq_prio_list_ded_slicing_r17, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cell_resel_priorities_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_prio_list_eutra.size() > 0) {
    j.start_array("freqPriorityListEUTRA");
    for (const auto& e1 : freq_prio_list_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_prio_list_nr.size() > 0) {
    j.start_array("freqPriorityListNR");
    for (const auto& e1 : freq_prio_list_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (t320_present) {
    j.write_str("t320", t320.to_string());
  }
  if (ext) {
    if (freq_prio_list_ded_slicing_r17.is_present()) {
      j.start_array("freqPriorityListDedicatedSlicing-r17");
      for (const auto& e1 : *freq_prio_list_ded_slicing_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* cell_resel_priorities_s::t320_opts::to_string() const
{
  static const char* names[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min180", "spare1"};
  return convert_enum_idx(names, 8, value, "cell_resel_priorities_s::t320_e_");
}
uint8_t cell_resel_priorities_s::t320_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 30, 60, 120, 180};
  return map_enum_number(numbers, 7, value, "cell_resel_priorities_s::t320_e_");
}

// DLInformationTransfer-v1610-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ref_time_info_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ref_time_info_r16_present) {
    HANDLE_CODE(ref_time_info_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ref_time_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ref_time_info_r16_present) {
    HANDLE_CODE(ref_time_info_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_info_transfer_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ref_time_info_r16_present) {
    j.write_fieldname("referenceTimeInfo-r16");
    ref_time_info_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// LoggedEventTriggerConfig-r16 ::= SEQUENCE
SRSASN_CODE logged_event_trigger_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(event_type_r16.pack(bref));
  HANDLE_CODE(logging_interv_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_event_trigger_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(event_type_r16.unpack(bref));
  HANDLE_CODE(logging_interv_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void logged_event_trigger_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventType-r16");
  event_type_r16.to_json(j);
  j.write_str("loggingInterval-r16", logging_interv_r16.to_string());
  j.end_obj();
}

// LoggedMeasurementConfiguration-v1700-IEs ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sig_logged_meas_type_r17_present, 1));
  HANDLE_CODE(bref.pack(early_meas_ind_r17_present, 1));
  HANDLE_CODE(bref.pack(area_cfg_v1700_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (area_cfg_v1700_present) {
    HANDLE_CODE(area_cfg_v1700.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sig_logged_meas_type_r17_present, 1));
  HANDLE_CODE(bref.unpack(early_meas_ind_r17_present, 1));
  HANDLE_CODE(bref.unpack(area_cfg_v1700_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (area_cfg_v1700_present) {
    HANDLE_CODE(area_cfg_v1700.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sig_logged_meas_type_r17_present) {
    j.write_str("sigLoggedMeasType-r17", "true");
  }
  if (early_meas_ind_r17_present) {
    j.write_str("earlyMeasIndication-r17", "true");
  }
  if (area_cfg_v1700_present) {
    j.write_fieldname("areaConfiguration-v1700");
    area_cfg_v1700.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// LoggedPeriodicalReportConfig-r16 ::= SEQUENCE
SRSASN_CODE logged_periodical_report_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(logging_interv_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_periodical_report_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(logging_interv_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void logged_periodical_report_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("loggingInterval-r16", logging_interv_r16.to_string());
  j.end_obj();
}

// LoggingDuration-r16 ::= ENUMERATED
const char* logging_dur_r16_opts::to_string() const
{
  static const char* names[] = {"min10", "min20", "min40", "min60", "min90", "min120", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "logging_dur_r16_e");
}
uint8_t logging_dur_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 60, 90, 120};
  return map_enum_number(numbers, 6, value, "logging_dur_r16_e");
}

// MobilityFromNRCommand-v1610-IEs ::= SEQUENCE
SRSASN_CODE mob_from_nr_cmd_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(voice_fallback_ind_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_nr_cmd_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(voice_fallback_ind_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void mob_from_nr_cmd_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (voice_fallback_ind_r16_present) {
    j.write_str("voiceFallbackIndication-r16", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// RedirectedCarrierInfo ::= CHOICE
void redirected_carrier_info_c::destroy_()
{
  switch (type_) {
    case types::nr:
      c.destroy<carrier_info_nr_s>();
      break;
    case types::eutra:
      c.destroy<redirected_carrier_info_eutra_s>();
      break;
    default:
      break;
  }
}
void redirected_carrier_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr:
      c.init<carrier_info_nr_s>();
      break;
    case types::eutra:
      c.init<redirected_carrier_info_eutra_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }
}
redirected_carrier_info_c::redirected_carrier_info_c(const redirected_carrier_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr:
      c.init(other.c.get<carrier_info_nr_s>());
      break;
    case types::eutra:
      c.init(other.c.get<redirected_carrier_info_eutra_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }
}
redirected_carrier_info_c& redirected_carrier_info_c::operator=(const redirected_carrier_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr:
      c.set(other.c.get<carrier_info_nr_s>());
      break;
    case types::eutra:
      c.set(other.c.get<redirected_carrier_info_eutra_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }

  return *this;
}
carrier_info_nr_s& redirected_carrier_info_c::set_nr()
{
  set(types::nr);
  return c.get<carrier_info_nr_s>();
}
redirected_carrier_info_eutra_s& redirected_carrier_info_c::set_eutra()
{
  set(types::eutra);
  return c.get<redirected_carrier_info_eutra_s>();
}
void redirected_carrier_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr:
      j.write_fieldname("nr");
      c.get<carrier_info_nr_s>().to_json(j);
      break;
    case types::eutra:
      j.write_fieldname("eutra");
      c.get<redirected_carrier_info_eutra_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
  }
  j.end_obj();
}
SRSASN_CODE redirected_carrier_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(c.get<carrier_info_nr_s>().pack(bref));
      break;
    case types::eutra:
      HANDLE_CODE(c.get<redirected_carrier_info_eutra_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE redirected_carrier_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(c.get<carrier_info_nr_s>().unpack(bref));
      break;
    case types::eutra:
      HANDLE_CODE(c.get<redirected_carrier_info_eutra_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* redirected_carrier_info_c::types_opts::to_string() const
{
  static const char* names[] = {"nr", "eutra"};
  return convert_enum_idx(names, 2, value, "redirected_carrier_info_c::types");
}

// RRCReconfiguration-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(master_cell_group.size() > 0, 1));
  HANDLE_CODE(bref.pack(full_cfg_present, 1));
  HANDLE_CODE(bref.pack(ded_nas_msg_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(master_key_upd_present, 1));
  HANDLE_CODE(bref.pack(ded_sib1_delivery.size() > 0, 1));
  HANDLE_CODE(bref.pack(ded_sys_info_delivery.size() > 0, 1));
  HANDLE_CODE(bref.pack(other_cfg_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (master_cell_group.size() > 0) {
    HANDLE_CODE(master_cell_group.pack(bref));
  }
  if (ded_nas_msg_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ded_nas_msg_list, 1, 29));
  }
  if (master_key_upd_present) {
    HANDLE_CODE(master_key_upd.pack(bref));
  }
  if (ded_sib1_delivery.size() > 0) {
    HANDLE_CODE(ded_sib1_delivery.pack(bref));
  }
  if (ded_sys_info_delivery.size() > 0) {
    HANDLE_CODE(ded_sys_info_delivery.pack(bref));
  }
  if (other_cfg_present) {
    HANDLE_CODE(other_cfg.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_v1530_ies_s::unpack(cbit_ref& bref)
{
  bool master_cell_group_present;
  HANDLE_CODE(bref.unpack(master_cell_group_present, 1));
  HANDLE_CODE(bref.unpack(full_cfg_present, 1));
  bool ded_nas_msg_list_present;
  HANDLE_CODE(bref.unpack(ded_nas_msg_list_present, 1));
  HANDLE_CODE(bref.unpack(master_key_upd_present, 1));
  bool ded_sib1_delivery_present;
  HANDLE_CODE(bref.unpack(ded_sib1_delivery_present, 1));
  bool ded_sys_info_delivery_present;
  HANDLE_CODE(bref.unpack(ded_sys_info_delivery_present, 1));
  HANDLE_CODE(bref.unpack(other_cfg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (master_cell_group_present) {
    HANDLE_CODE(master_cell_group.unpack(bref));
  }
  if (ded_nas_msg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ded_nas_msg_list, bref, 1, 29));
  }
  if (master_key_upd_present) {
    HANDLE_CODE(master_key_upd.unpack(bref));
  }
  if (ded_sib1_delivery_present) {
    HANDLE_CODE(ded_sib1_delivery.unpack(bref));
  }
  if (ded_sys_info_delivery_present) {
    HANDLE_CODE(ded_sys_info_delivery.unpack(bref));
  }
  if (other_cfg_present) {
    HANDLE_CODE(other_cfg.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (master_cell_group.size() > 0) {
    j.write_str("masterCellGroup", master_cell_group.to_string());
  }
  if (full_cfg_present) {
    j.write_str("fullConfig", "true");
  }
  if (ded_nas_msg_list.size() > 0) {
    j.start_array("dedicatedNAS-MessageList");
    for (const auto& e1 : ded_nas_msg_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (master_key_upd_present) {
    j.write_fieldname("masterKeyUpdate");
    master_key_upd.to_json(j);
  }
  if (ded_sib1_delivery.size() > 0) {
    j.write_str("dedicatedSIB1-Delivery", ded_sib1_delivery.to_string());
  }
  if (ded_sys_info_delivery.size() > 0) {
    j.write_str("dedicatedSystemInformationDelivery", ded_sys_info_delivery.to_string());
  }
  if (other_cfg_present) {
    j.write_fieldname("otherConfig");
    other_cfg.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCReestablishment-v1700-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_l2_remote_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sl_l2_remote_ue_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sl_l2_remote_ue_cfg_r17_present) {
    HANDLE_CODE(sl_l2_remote_ue_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_reest_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
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

// RRCRelease-v1540-IEs ::= SEQUENCE
SRSASN_CODE rrc_release_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wait_time_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(pack_integer(bref, wait_time, (uint8_t)1u, (uint8_t)16u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_release_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wait_time_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(unpack_integer(wait_time, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_release_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wait_time_present) {
    j.write_int("waitTime", wait_time);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCResume-v1560-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(radio_bearer_cfg2.size() > 0, 1));
  HANDLE_CODE(bref.pack(sk_counter_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (radio_bearer_cfg2.size() > 0) {
    HANDLE_CODE(radio_bearer_cfg2.pack(bref));
  }
  if (sk_counter_present) {
    HANDLE_CODE(pack_integer(bref, sk_counter, (uint32_t)0u, (uint32_t)65535u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_v1560_ies_s::unpack(cbit_ref& bref)
{
  bool radio_bearer_cfg2_present;
  HANDLE_CODE(bref.unpack(radio_bearer_cfg2_present, 1));
  HANDLE_CODE(bref.unpack(sk_counter_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (radio_bearer_cfg2_present) {
    HANDLE_CODE(radio_bearer_cfg2.unpack(bref));
  }
  if (sk_counter_present) {
    HANDLE_CODE(unpack_integer(sk_counter, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (radio_bearer_cfg2.size() > 0) {
    j.write_str("radioBearerConfig2", radio_bearer_cfg2.to_string());
  }
  if (sk_counter_present) {
    j.write_int("sk-Counter", sk_counter);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SecurityConfigSMC ::= SEQUENCE
SRSASN_CODE security_cfg_smc_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(security_algorithm_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_smc_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(security_algorithm_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_cfg_smc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("securityAlgorithmConfig");
  security_algorithm_cfg.to_json(j);
  j.end_obj();
}

// SuspendConfig ::= SEQUENCE
SRSASN_CODE suspend_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ran_notif_area_info_present, 1));
  HANDLE_CODE(bref.pack(t380_present, 1));

  HANDLE_CODE(full_i_rnti.pack(bref));
  HANDLE_CODE(short_i_rnti.pack(bref));
  HANDLE_CODE(ran_paging_cycle.pack(bref));
  if (ran_notif_area_info_present) {
    HANDLE_CODE(ran_notif_area_info.pack(bref));
  }
  if (t380_present) {
    HANDLE_CODE(t380.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, next_hop_chaining_count, (uint8_t)0u, (uint8_t)7u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_ue_id_remote_r17_present;
    group_flags[0] |= sdt_cfg_r17.is_present();
    group_flags[0] |= srs_pos_rrc_inactive_r17.is_present();
    group_flags[0] |= ran_extended_paging_cycle_r17_present;
    group_flags[1] |= ncd_ssb_red_cap_init_bwp_sdt_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_ue_id_remote_r17_present, 1));
      HANDLE_CODE(bref.pack(sdt_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_pos_rrc_inactive_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(ran_extended_paging_cycle_r17_present, 1));
      if (sl_ue_id_remote_r17_present) {
        HANDLE_CODE(pack_integer(bref, sl_ue_id_remote_r17, (uint32_t)0u, (uint32_t)65535u));
      }
      if (sdt_cfg_r17.is_present()) {
        HANDLE_CODE(sdt_cfg_r17->pack(bref));
      }
      if (srs_pos_rrc_inactive_r17.is_present()) {
        HANDLE_CODE(srs_pos_rrc_inactive_r17->pack(bref));
      }
      if (ran_extended_paging_cycle_r17_present) {
        HANDLE_CODE(ran_extended_paging_cycle_r17.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ncd_ssb_red_cap_init_bwp_sdt_r17.is_present(), 1));
      if (ncd_ssb_red_cap_init_bwp_sdt_r17.is_present()) {
        HANDLE_CODE(ncd_ssb_red_cap_init_bwp_sdt_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE suspend_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ran_notif_area_info_present, 1));
  HANDLE_CODE(bref.unpack(t380_present, 1));

  HANDLE_CODE(full_i_rnti.unpack(bref));
  HANDLE_CODE(short_i_rnti.unpack(bref));
  HANDLE_CODE(ran_paging_cycle.unpack(bref));
  if (ran_notif_area_info_present) {
    HANDLE_CODE(ran_notif_area_info.unpack(bref));
  }
  if (t380_present) {
    HANDLE_CODE(t380.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(next_hop_chaining_count, bref, (uint8_t)0u, (uint8_t)7u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sl_ue_id_remote_r17_present, 1));
      unpack_presence_flag(sdt_cfg_r17, bref);
      unpack_presence_flag(srs_pos_rrc_inactive_r17, bref);
      HANDLE_CODE(bref.unpack(ran_extended_paging_cycle_r17_present, 1));
      if (sl_ue_id_remote_r17_present) {
        HANDLE_CODE(unpack_integer(sl_ue_id_remote_r17, bref, (uint32_t)0u, (uint32_t)65535u));
      }
      if (sdt_cfg_r17.is_present()) {
        HANDLE_CODE(sdt_cfg_r17->unpack(bref));
      }
      if (srs_pos_rrc_inactive_r17.is_present()) {
        HANDLE_CODE(srs_pos_rrc_inactive_r17->unpack(bref));
      }
      if (ran_extended_paging_cycle_r17_present) {
        HANDLE_CODE(ran_extended_paging_cycle_r17.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ncd_ssb_red_cap_init_bwp_sdt_r17, bref);
      if (ncd_ssb_red_cap_init_bwp_sdt_r17.is_present()) {
        HANDLE_CODE(ncd_ssb_red_cap_init_bwp_sdt_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void suspend_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("fullI-RNTI", full_i_rnti.to_string());
  j.write_str("shortI-RNTI", short_i_rnti.to_string());
  j.write_str("ran-PagingCycle", ran_paging_cycle.to_string());
  if (ran_notif_area_info_present) {
    j.write_fieldname("ran-NotificationAreaInfo");
    ran_notif_area_info.to_json(j);
  }
  if (t380_present) {
    j.write_str("t380", t380.to_string());
  }
  j.write_int("nextHopChainingCount", next_hop_chaining_count);
  if (ext) {
    if (sl_ue_id_remote_r17_present) {
      j.write_int("sl-UEIdentityRemote-r17", sl_ue_id_remote_r17);
    }
    if (sdt_cfg_r17.is_present()) {
      j.write_fieldname("sdt-Config-r17");
      sdt_cfg_r17->to_json(j);
    }
    if (srs_pos_rrc_inactive_r17.is_present()) {
      j.write_fieldname("srs-PosRRC-Inactive-r17");
      srs_pos_rrc_inactive_r17->to_json(j);
    }
    if (ran_extended_paging_cycle_r17_present) {
      j.write_str("ran-ExtendedPagingCycle-r17", ran_extended_paging_cycle_r17.to_string());
    }
    if (ncd_ssb_red_cap_init_bwp_sdt_r17.is_present()) {
      j.write_fieldname("ncd-SSB-RedCapInitialBWP-SDT-r17");
      ncd_ssb_red_cap_init_bwp_sdt_r17->to_json(j);
    }
  }
  j.end_obj();
}

// UEInformationRequest-v1700-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(success_ho_report_req_r17_present, 1));
  HANDLE_CODE(bref.pack(coarse_location_request_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(success_ho_report_req_r17_present, 1));
  HANDLE_CODE(bref.unpack(coarse_location_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void ue_info_request_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (success_ho_report_req_r17_present) {
    j.write_str("successHO-ReportReq-r17", "true");
  }
  if (coarse_location_request_r17_present) {
    j.write_str("coarseLocationRequest-r17", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CounterCheck-IEs ::= SEQUENCE
SRSASN_CODE counter_check_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, drb_count_msb_info_list, 1, 29));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(drb_count_msb_info_list, bref, 1, 29));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void counter_check_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("drb-CountMSB-InfoList");
  for (const auto& e1 : drb_count_msb_info_list) {
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

// DLDedicatedMessageSegment-r16-IEs ::= SEQUENCE
SRSASN_CODE dl_ded_msg_segment_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, segment_num_r16, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(rrc_msg_segment_container_r16.pack(bref));
  HANDLE_CODE(rrc_msg_segment_type_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ded_msg_segment_r16_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(segment_num_r16, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(rrc_msg_segment_container_r16.unpack(bref));
  HANDLE_CODE(rrc_msg_segment_type_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_ded_msg_segment_r16_ies_s::to_json(json_writer& j) const
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

const char* dl_ded_msg_segment_r16_ies_s::rrc_msg_segment_type_r16_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "dl_ded_msg_segment_r16_ies_s::rrc_msg_segment_type_r16_e_");
}

// DLInformationTransfer-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE dl_info_transfer_ies_s::unpack(cbit_ref& bref)
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
void dl_info_transfer_ies_s::to_json(json_writer& j) const
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

// DLInformationTransferMRDC-r16-IEs ::= SEQUENCE
SRSASN_CODE dl_info_transfer_mrdc_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_dcch_msg_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(dl_dcch_msg_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (dl_dcch_msg_nr_r16.size() > 0) {
    HANDLE_CODE(dl_dcch_msg_nr_r16.pack(bref));
  }
  if (dl_dcch_msg_eutra_r16.size() > 0) {
    HANDLE_CODE(dl_dcch_msg_eutra_r16.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_mrdc_r16_ies_s::unpack(cbit_ref& bref)
{
  bool dl_dcch_msg_nr_r16_present;
  HANDLE_CODE(bref.unpack(dl_dcch_msg_nr_r16_present, 1));
  bool dl_dcch_msg_eutra_r16_present;
  HANDLE_CODE(bref.unpack(dl_dcch_msg_eutra_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (dl_dcch_msg_nr_r16_present) {
    HANDLE_CODE(dl_dcch_msg_nr_r16.unpack(bref));
  }
  if (dl_dcch_msg_eutra_r16_present) {
    HANDLE_CODE(dl_dcch_msg_eutra_r16.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_info_transfer_mrdc_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_dcch_msg_nr_r16.size() > 0) {
    j.write_str("dl-DCCH-MessageNR-r16", dl_dcch_msg_nr_r16.to_string());
  }
  if (dl_dcch_msg_eutra_r16.size() > 0) {
    j.write_str("dl-DCCH-MessageEUTRA-r16", dl_dcch_msg_eutra_r16.to_string());
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

// LoggedMeasurementConfiguration-r16-IEs ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(bt_name_list_r16_present, 1));
  HANDLE_CODE(bref.pack(wlan_name_list_r16_present, 1));
  HANDLE_CODE(bref.pack(sensor_name_list_r16_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(trace_ref_r16.pack(bref));
  HANDLE_CODE(trace_recording_session_ref_r16.pack(bref));
  HANDLE_CODE(tce_id_r16.pack(bref));
  HANDLE_CODE(absolute_time_info_r16.pack(bref));
  if (area_cfg_r16_present) {
    HANDLE_CODE(area_cfg_r16.pack(bref));
  }
  if (plmn_id_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r16, 1, 16));
  }
  if (bt_name_list_r16_present) {
    HANDLE_CODE(bt_name_list_r16.pack(bref));
  }
  if (wlan_name_list_r16_present) {
    HANDLE_CODE(wlan_name_list_r16.pack(bref));
  }
  if (sensor_name_list_r16_present) {
    HANDLE_CODE(sensor_name_list_r16.pack(bref));
  }
  HANDLE_CODE(logging_dur_r16.pack(bref));
  HANDLE_CODE(report_type.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r16_present, 1));
  bool plmn_id_list_r16_present;
  HANDLE_CODE(bref.unpack(plmn_id_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(bt_name_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(wlan_name_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sensor_name_list_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(trace_ref_r16.unpack(bref));
  HANDLE_CODE(trace_recording_session_ref_r16.unpack(bref));
  HANDLE_CODE(tce_id_r16.unpack(bref));
  HANDLE_CODE(absolute_time_info_r16.unpack(bref));
  if (area_cfg_r16_present) {
    HANDLE_CODE(area_cfg_r16.unpack(bref));
  }
  if (plmn_id_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r16, bref, 1, 16));
  }
  if (bt_name_list_r16_present) {
    HANDLE_CODE(bt_name_list_r16.unpack(bref));
  }
  if (wlan_name_list_r16_present) {
    HANDLE_CODE(wlan_name_list_r16.unpack(bref));
  }
  if (sensor_name_list_r16_present) {
    HANDLE_CODE(sensor_name_list_r16.unpack(bref));
  }
  HANDLE_CODE(logging_dur_r16.unpack(bref));
  HANDLE_CODE(report_type.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("traceReference-r16");
  trace_ref_r16.to_json(j);
  j.write_str("traceRecordingSessionRef-r16", trace_recording_session_ref_r16.to_string());
  j.write_str("tce-Id-r16", tce_id_r16.to_string());
  j.write_str("absoluteTimeInfo-r16", absolute_time_info_r16.to_string());
  if (area_cfg_r16_present) {
    j.write_fieldname("areaConfiguration-r16");
    area_cfg_r16.to_json(j);
  }
  if (plmn_id_list_r16.size() > 0) {
    j.start_array("plmn-IdentityList-r16");
    for (const auto& e1 : plmn_id_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bt_name_list_r16_present) {
    j.write_fieldname("bt-NameList-r16");
    bt_name_list_r16.to_json(j);
  }
  if (wlan_name_list_r16_present) {
    j.write_fieldname("wlan-NameList-r16");
    wlan_name_list_r16.to_json(j);
  }
  if (sensor_name_list_r16_present) {
    j.write_fieldname("sensor-NameList-r16");
    sensor_name_list_r16.to_json(j);
  }
  j.write_str("loggingDuration-r16", logging_dur_r16.to_string());
  j.write_fieldname("reportType");
  report_type.to_json(j);
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void logged_meas_cfg_r16_ies_s::report_type_c_::destroy_()
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
void logged_meas_cfg_r16_ies_s::report_type_c_::set(types::options e)
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_ies_s::report_type_c_");
  }
}
logged_meas_cfg_r16_ies_s::report_type_c_::report_type_c_(const logged_meas_cfg_r16_ies_s::report_type_c_& other)
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_ies_s::report_type_c_");
  }
}
logged_meas_cfg_r16_ies_s::report_type_c_&
logged_meas_cfg_r16_ies_s::report_type_c_::operator=(const logged_meas_cfg_r16_ies_s::report_type_c_& other)
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_ies_s::report_type_c_");
  }

  return *this;
}
logged_periodical_report_cfg_r16_s& logged_meas_cfg_r16_ies_s::report_type_c_::set_periodical()
{
  set(types::periodical);
  return c.get<logged_periodical_report_cfg_r16_s>();
}
logged_event_trigger_cfg_r16_s& logged_meas_cfg_r16_ies_s::report_type_c_::set_event_triggered()
{
  set(types::event_triggered);
  return c.get<logged_event_trigger_cfg_r16_s>();
}
void logged_meas_cfg_r16_ies_s::report_type_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_ies_s::report_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE logged_meas_cfg_r16_ies_s::report_type_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_ies_s::report_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r16_ies_s::report_type_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_ies_s::report_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* logged_meas_cfg_r16_ies_s::report_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical", "eventTriggered"};
  return convert_enum_idx(names, 2, value, "logged_meas_cfg_r16_ies_s::report_type_c_::types");
}

// MobilityFromNRCommand-IEs ::= SEQUENCE
SRSASN_CODE mob_from_nr_cmd_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nas_security_param_from_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(target_rat_type.pack(bref));
  HANDLE_CODE(target_rat_msg_container.pack(bref));
  if (nas_security_param_from_nr.size() > 0) {
    HANDLE_CODE(nas_security_param_from_nr.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_nr_cmd_ies_s::unpack(cbit_ref& bref)
{
  bool nas_security_param_from_nr_present;
  HANDLE_CODE(bref.unpack(nas_security_param_from_nr_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(target_rat_type.unpack(bref));
  HANDLE_CODE(target_rat_msg_container.unpack(bref));
  if (nas_security_param_from_nr_present) {
    HANDLE_CODE(nas_security_param_from_nr.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mob_from_nr_cmd_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("targetRAT-Type", target_rat_type.to_string());
  j.write_str("targetRAT-MessageContainer", target_rat_msg_container.to_string());
  if (nas_security_param_from_nr.size() > 0) {
    j.write_str("nas-SecurityParamFromNR", nas_security_param_from_nr.to_string());
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

const char* mob_from_nr_cmd_ies_s::target_rat_type_opts::to_string() const
{
  static const char* names[] = {"eutra", "utra-fdd-v1610", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "mob_from_nr_cmd_ies_s::target_rat_type_e_");
}

// RRCReconfiguration-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(radio_bearer_cfg_present, 1));
  HANDLE_CODE(bref.pack(secondary_cell_group.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_cfg_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (radio_bearer_cfg_present) {
    HANDLE_CODE(radio_bearer_cfg.pack(bref));
  }
  if (secondary_cell_group.size() > 0) {
    HANDLE_CODE(secondary_cell_group.pack(bref));
  }
  if (meas_cfg_present) {
    HANDLE_CODE(meas_cfg.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(radio_bearer_cfg_present, 1));
  bool secondary_cell_group_present;
  HANDLE_CODE(bref.unpack(secondary_cell_group_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (radio_bearer_cfg_present) {
    HANDLE_CODE(radio_bearer_cfg.unpack(bref));
  }
  if (secondary_cell_group_present) {
    HANDLE_CODE(secondary_cell_group.unpack(bref));
  }
  if (meas_cfg_present) {
    HANDLE_CODE(meas_cfg.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (radio_bearer_cfg_present) {
    j.write_fieldname("radioBearerConfig");
    radio_bearer_cfg.to_json(j);
  }
  if (secondary_cell_group.size() > 0) {
    j.write_str("secondaryCellGroup", secondary_cell_group.to_string());
  }
  if (meas_cfg_present) {
    j.write_fieldname("measConfig");
    meas_cfg.to_json(j);
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

// RRCReestablishment-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, next_hop_chaining_count, (uint8_t)0u, (uint8_t)7u));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(next_hop_chaining_count, bref, (uint8_t)0u, (uint8_t)7u));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_reest_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nextHopChainingCount", next_hop_chaining_count);
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCRelease-IEs ::= SEQUENCE
SRSASN_CODE rrc_release_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(redirected_carrier_info_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_priorities_present, 1));
  HANDLE_CODE(bref.pack(suspend_cfg_present, 1));
  HANDLE_CODE(bref.pack(depriorit_req_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (redirected_carrier_info_present) {
    HANDLE_CODE(redirected_carrier_info.pack(bref));
  }
  if (cell_resel_priorities_present) {
    HANDLE_CODE(cell_resel_priorities.pack(bref));
  }
  if (suspend_cfg_present) {
    HANDLE_CODE(suspend_cfg.pack(bref));
  }
  if (depriorit_req_present) {
    HANDLE_CODE(depriorit_req.depriorit_type.pack(bref));
    HANDLE_CODE(depriorit_req.depriorit_timer.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_release_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(redirected_carrier_info_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_priorities_present, 1));
  HANDLE_CODE(bref.unpack(suspend_cfg_present, 1));
  HANDLE_CODE(bref.unpack(depriorit_req_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (redirected_carrier_info_present) {
    HANDLE_CODE(redirected_carrier_info.unpack(bref));
  }
  if (cell_resel_priorities_present) {
    HANDLE_CODE(cell_resel_priorities.unpack(bref));
  }
  if (suspend_cfg_present) {
    HANDLE_CODE(suspend_cfg.unpack(bref));
  }
  if (depriorit_req_present) {
    HANDLE_CODE(depriorit_req.depriorit_type.unpack(bref));
    HANDLE_CODE(depriorit_req.depriorit_timer.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_release_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (redirected_carrier_info_present) {
    j.write_fieldname("redirectedCarrierInfo");
    redirected_carrier_info.to_json(j);
  }
  if (cell_resel_priorities_present) {
    j.write_fieldname("cellReselectionPriorities");
    cell_resel_priorities.to_json(j);
  }
  if (suspend_cfg_present) {
    j.write_fieldname("suspendConfig");
    suspend_cfg.to_json(j);
  }
  if (depriorit_req_present) {
    j.write_fieldname("deprioritisationReq");
    j.start_obj();
    j.write_str("deprioritisationType", depriorit_req.depriorit_type.to_string());
    j.write_str("deprioritisationTimer", depriorit_req.depriorit_timer.to_string());
    j.end_obj();
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

const char* rrc_release_ies_s::depriorit_req_s_::depriorit_type_opts::to_string() const
{
  static const char* names[] = {"frequency", "nr"};
  return convert_enum_idx(names, 2, value, "rrc_release_ies_s::depriorit_req_s_::depriorit_type_e_");
}

const char* rrc_release_ies_s::depriorit_req_s_::depriorit_timer_opts::to_string() const
{
  static const char* names[] = {"min5", "min10", "min15", "min30"};
  return convert_enum_idx(names, 4, value, "rrc_release_ies_s::depriorit_req_s_::depriorit_timer_e_");
}
uint8_t rrc_release_ies_s::depriorit_req_s_::depriorit_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 15, 30};
  return map_enum_number(numbers, 4, value, "rrc_release_ies_s::depriorit_req_s_::depriorit_timer_e_");
}

// RRCResume-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(radio_bearer_cfg_present, 1));
  HANDLE_CODE(bref.pack(master_cell_group.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_cfg_present, 1));
  HANDLE_CODE(bref.pack(full_cfg_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (radio_bearer_cfg_present) {
    HANDLE_CODE(radio_bearer_cfg.pack(bref));
  }
  if (master_cell_group.size() > 0) {
    HANDLE_CODE(master_cell_group.pack(bref));
  }
  if (meas_cfg_present) {
    HANDLE_CODE(meas_cfg.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(radio_bearer_cfg_present, 1));
  bool master_cell_group_present;
  HANDLE_CODE(bref.unpack(master_cell_group_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_present, 1));
  HANDLE_CODE(bref.unpack(full_cfg_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (radio_bearer_cfg_present) {
    HANDLE_CODE(radio_bearer_cfg.unpack(bref));
  }
  if (master_cell_group_present) {
    HANDLE_CODE(master_cell_group.unpack(bref));
  }
  if (meas_cfg_present) {
    HANDLE_CODE(meas_cfg.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (radio_bearer_cfg_present) {
    j.write_fieldname("radioBearerConfig");
    radio_bearer_cfg.to_json(j);
  }
  if (master_cell_group.size() > 0) {
    j.write_str("masterCellGroup", master_cell_group.to_string());
  }
  if (meas_cfg_present) {
    j.write_fieldname("measConfig");
    meas_cfg.to_json(j);
  }
  if (full_cfg_present) {
    j.write_str("fullConfig", "true");
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

// SecurityModeCommand-IEs ::= SEQUENCE
SRSASN_CODE security_mode_cmd_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(security_cfg_smc.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(security_cfg_smc.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_cmd_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("securityConfigSMC");
  security_cfg_smc.to_json(j);
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

// UECapabilityEnquiry-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(ue_cap_enquiry_ext.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, ue_cap_rat_request_list, 1, 8));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (ue_cap_enquiry_ext.size() > 0) {
    HANDLE_CODE(ue_cap_enquiry_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  bool ue_cap_enquiry_ext_present;
  HANDLE_CODE(bref.unpack(ue_cap_enquiry_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(ue_cap_rat_request_list, bref, 1, 8));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (ue_cap_enquiry_ext_present) {
    HANDLE_CODE(ue_cap_enquiry_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ue-CapabilityRAT-RequestList");
  for (const auto& e1 : ue_cap_rat_request_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ue_cap_enquiry_ext.size() > 0) {
    j.write_str("ue-CapabilityEnquiryExt", ue_cap_enquiry_ext.to_string());
  }
  j.end_obj();
}

// UEInformationRequest-r16-IEs ::= SEQUENCE
SRSASN_CODE ue_info_request_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idle_mode_meas_req_r16_present, 1));
  HANDLE_CODE(bref.pack(log_meas_report_req_r16_present, 1));
  HANDLE_CODE(bref.pack(conn_est_fail_report_req_r16_present, 1));
  HANDLE_CODE(bref.pack(ra_report_req_r16_present, 1));
  HANDLE_CODE(bref.pack(rlf_report_req_r16_present, 1));
  HANDLE_CODE(bref.pack(mob_history_report_req_r16_present, 1));
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
SRSASN_CODE ue_info_request_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idle_mode_meas_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(log_meas_report_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(conn_est_fail_report_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(ra_report_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(rlf_report_req_r16_present, 1));
  HANDLE_CODE(bref.unpack(mob_history_report_req_r16_present, 1));
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
void ue_info_request_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idle_mode_meas_req_r16_present) {
    j.write_str("idleModeMeasurementReq-r16", "true");
  }
  if (log_meas_report_req_r16_present) {
    j.write_str("logMeasReportReq-r16", "true");
  }
  if (conn_est_fail_report_req_r16_present) {
    j.write_str("connEstFailReportReq-r16", "true");
  }
  if (ra_report_req_r16_present) {
    j.write_str("ra-ReportReq-r16", "true");
  }
  if (rlf_report_req_r16_present) {
    j.write_str("rlf-ReportReq-r16", "true");
  }
  if (mob_history_report_req_r16_present) {
    j.write_str("mobilityHistoryReportReq-r16", "true");
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

// CounterCheck ::= SEQUENCE
SRSASN_CODE counter_check_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void counter_check_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void counter_check_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
counter_check_ies_s& counter_check_s::crit_exts_c_::set_counter_check()
{
  set(types::counter_check);
  return c;
}
void counter_check_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void counter_check_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::counter_check:
      j.write_fieldname("counterCheck");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE counter_check_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::counter_check:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::counter_check:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* counter_check_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"counterCheck", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "counter_check_s::crit_exts_c_::types");
}

// DLDedicatedMessageSegment-r16 ::= SEQUENCE
SRSASN_CODE dl_ded_msg_segment_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ded_msg_segment_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_ded_msg_segment_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void dl_ded_msg_segment_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
dl_ded_msg_segment_r16_ies_s& dl_ded_msg_segment_r16_s::crit_exts_c_::set_dl_ded_msg_segment_r16()
{
  set(types::dl_ded_msg_segment_r16);
  return c;
}
void dl_ded_msg_segment_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void dl_ded_msg_segment_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_ded_msg_segment_r16:
      j.write_fieldname("dlDedicatedMessageSegment-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ded_msg_segment_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_ded_msg_segment_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_ded_msg_segment_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ded_msg_segment_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ded_msg_segment_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_ded_msg_segment_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ded_msg_segment_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_ded_msg_segment_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"dlDedicatedMessageSegment-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "dl_ded_msg_segment_r16_s::crit_exts_c_::types");
}

// DLInformationTransferMRDC-r16 ::= SEQUENCE
SRSASN_CODE dl_info_transfer_mrdc_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_mrdc_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_info_transfer_mrdc_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void dl_info_transfer_mrdc_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_& dl_info_transfer_mrdc_r16_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void dl_info_transfer_mrdc_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void dl_info_transfer_mrdc_r16_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "dl_info_transfer_mrdc_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_mrdc_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_mrdc_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_mrdc_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "dl_info_transfer_mrdc_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
dl_info_transfer_mrdc_r16_ies_s& dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::set_dl_info_transfer_mrdc_r16()
{
  set(types::dl_info_transfer_mrdc_r16);
  return c;
}
void dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_info_transfer_mrdc_r16:
      j.write_fieldname("dlInformationTransferMRDC-r16");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_info_transfer_mrdc_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_info_transfer_mrdc_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"dlInformationTransferMRDC-r16", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::types");
}

const char* dl_info_transfer_mrdc_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::types");
}
uint8_t dl_info_transfer_mrdc_r16_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::types");
}

// DLInformationTransfer ::= SEQUENCE
SRSASN_CODE dl_info_transfer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_info_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void dl_info_transfer_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
dl_info_transfer_ies_s& dl_info_transfer_s::crit_exts_c_::set_dl_info_transfer()
{
  set(types::dl_info_transfer);
  return c;
}
void dl_info_transfer_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void dl_info_transfer_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dl_info_transfer:
      j.write_fieldname("dlInformationTransfer");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_info_transfer_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dl_info_transfer:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_info_transfer_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dl_info_transfer:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "dl_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_info_transfer_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"dlInformationTransfer", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "dl_info_transfer_s::crit_exts_c_::types");
}

// LoggedMeasurementConfiguration-r16 ::= SEQUENCE
SRSASN_CODE logged_meas_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void logged_meas_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void logged_meas_cfg_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
logged_meas_cfg_r16_ies_s& logged_meas_cfg_r16_s::crit_exts_c_::set_logged_meas_cfg_r16()
{
  set(types::logged_meas_cfg_r16);
  return c;
}
void logged_meas_cfg_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void logged_meas_cfg_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::logged_meas_cfg_r16:
      j.write_fieldname("loggedMeasurementConfiguration-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE logged_meas_cfg_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::logged_meas_cfg_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE logged_meas_cfg_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::logged_meas_cfg_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "logged_meas_cfg_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* logged_meas_cfg_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"loggedMeasurementConfiguration-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "logged_meas_cfg_r16_s::crit_exts_c_::types");
}

// MobilityFromNRCommand ::= SEQUENCE
SRSASN_CODE mob_from_nr_cmd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_nr_cmd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mob_from_nr_cmd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mob_from_nr_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mob_from_nr_cmd_ies_s& mob_from_nr_cmd_s::crit_exts_c_::set_mob_from_nr_cmd()
{
  set(types::mob_from_nr_cmd);
  return c;
}
void mob_from_nr_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mob_from_nr_cmd_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mob_from_nr_cmd:
      j.write_fieldname("mobilityFromNRCommand");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_nr_cmd_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mob_from_nr_cmd_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mob_from_nr_cmd:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_nr_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_from_nr_cmd_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mob_from_nr_cmd:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mob_from_nr_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mob_from_nr_cmd_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mobilityFromNRCommand", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mob_from_nr_cmd_s::crit_exts_c_::types");
}

// RRCReconfiguration ::= SEQUENCE
SRSASN_CODE rrc_recfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_recfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_recfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_recfg_ies_s& rrc_recfg_s::crit_exts_c_::set_rrc_recfg()
{
  set(types::rrc_recfg);
  return c;
}
void rrc_recfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_recfg_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_recfg:
      j.write_fieldname("rrcReconfiguration");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_recfg_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_recfg:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_recfg:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_recfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReconfiguration", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_recfg_s::crit_exts_c_::types");
}

// RRCReestablishment ::= SEQUENCE
SRSASN_CODE rrc_reest_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_reest_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_reest_ies_s& rrc_reest_s::crit_exts_c_::set_rrc_reest()
{
  set(types::rrc_reest);
  return c;
}
void rrc_reest_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_reest_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_reest:
      j.write_fieldname("rrcReestablishment");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_reest_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_reest:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_reest:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_reest_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReestablishment", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_reest_s::crit_exts_c_::types");
}

// RRCRelease ::= SEQUENCE
SRSASN_CODE rrc_release_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_release_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_release_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_release_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_release_ies_s& rrc_release_s::crit_exts_c_::set_rrc_release()
{
  set(types::rrc_release);
  return c;
}
void rrc_release_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_release_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_release:
      j.write_fieldname("rrcRelease");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_release_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_release_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_release:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_release_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_release_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_release:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_release_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_release_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcRelease", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_release_s::crit_exts_c_::types");
}

// RRCResume ::= SEQUENCE
SRSASN_CODE rrc_resume_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_resume_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_resume_ies_s& rrc_resume_s::crit_exts_c_::set_rrc_resume()
{
  set(types::rrc_resume);
  return c;
}
void rrc_resume_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_resume_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_resume:
      j.write_fieldname("rrcResume");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_resume_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_resume:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_resume:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_resume_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcResume", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_resume_s::crit_exts_c_::types");
}

// SecurityModeCommand ::= SEQUENCE
SRSASN_CODE security_mode_cmd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_cmd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_cmd_ies_s& security_mode_cmd_s::crit_exts_c_::set_security_mode_cmd()
{
  set(types::security_mode_cmd);
  return c;
}
void security_mode_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_cmd_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_cmd:
      j.write_fieldname("securityModeCommand");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_cmd_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_cmd:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_cmd_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_cmd:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* security_mode_cmd_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"securityModeCommand", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "security_mode_cmd_s::crit_exts_c_::types");
}

// UECapabilityEnquiry ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_cap_enquiry_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_enquiry_ies_s& ue_cap_enquiry_s::crit_exts_c_::set_ue_cap_enquiry()
{
  set(types::ue_cap_enquiry);
  return c;
}
void ue_cap_enquiry_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_cap_enquiry_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_cap_enquiry:
      j.write_fieldname("ueCapabilityEnquiry");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_enquiry_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_cap_enquiry:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_cap_enquiry:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_enquiry_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_cap_enquiry_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueCapabilityEnquiry", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_cap_enquiry_s::crit_exts_c_::types");
}

// UEInformationRequest-r16 ::= SEQUENCE
SRSASN_CODE ue_info_request_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_info_request_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_info_request_r16_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_info_request_r16_ies_s& ue_info_request_r16_s::crit_exts_c_::set_ue_info_request_r16()
{
  set(types::ue_info_request_r16);
  return c;
}
void ue_info_request_r16_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_info_request_r16_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_info_request_r16:
      j.write_fieldname("ueInformationRequest-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_request_r16_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_info_request_r16_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_info_request_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_request_r16_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_info_request_r16_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_info_request_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_info_request_r16_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_info_request_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueInformationRequest-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_info_request_r16_s::crit_exts_c_::types");
}

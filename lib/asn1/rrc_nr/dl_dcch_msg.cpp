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
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PCI-Range ::= SEQUENCE
SRSASN_CODE pci_range_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(range_present, 1));

  HANDLE_CODE(pack_integer(bref, start, (uint16_t)0u, (uint16_t)1007u));
  if (range_present) {
    HANDLE_CODE(range.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_range_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(range_present, 1));

  HANDLE_CODE(unpack_integer(start, bref, (uint16_t)0u, (uint16_t)1007u));
  if (range_present) {
    HANDLE_CODE(range.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pci_range_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("start", start);
  if (range_present) {
    j.write_str("range", range.to_string());
  }
  j.end_obj();
}

const char* pci_range_s::range_opts::to_string() const
{
  static const char* names[] = {"n4",
                                "n8",
                                "n12",
                                "n16",
                                "n24",
                                "n32",
                                "n48",
                                "n64",
                                "n84",
                                "n96",
                                "n128",
                                "n168",
                                "n252",
                                "n504",
                                "n1008",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "pci_range_s::range_e_");
}
uint16_t pci_range_s::range_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504, 1008};
  return map_enum_number(numbers, 15, value, "pci_range_s::range_e_");
}

// CellReselectionSubPriority ::= ENUMERATED
const char* cell_resel_sub_prio_opts::to_string() const
{
  static const char* names[] = {"oDot2", "oDot4", "oDot6", "oDot8"};
  return convert_enum_idx(names, 4, value, "cell_resel_sub_prio_e");
}
float cell_resel_sub_prio_opts::to_number() const
{
  static const float numbers[] = {0.2, 0.4, 0.6, 0.8};
  return map_enum_number(numbers, 4, value, "cell_resel_sub_prio_e");
}
const char* cell_resel_sub_prio_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(number_strs, 4, value, "cell_resel_sub_prio_e");
}

// EUTRA-Q-OffsetRange ::= ENUMERATED
const char* eutra_q_offset_range_opts::to_string() const
{
  static const char* names[] = {"dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10",
                                "dB-8",  "dB-6",  "dB-5",  "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",
                                "dB1",   "dB2",   "dB3",   "dB4",   "dB5",   "dB6",   "dB8",   "dB10",
                                "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
  return convert_enum_idx(names, 31, value, "eutra_q_offset_range_e");
}
int8_t eutra_q_offset_range_opts::to_number() const
{
  static const int8_t numbers[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 31, value, "eutra_q_offset_range_e");
}

// NSAG-IdentityInfo-r17 ::= SEQUENCE
SRSASN_CODE nsag_id_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tac_r17_present, 1));

  HANDLE_CODE(nsag_id_r17.pack(bref));
  if (tac_r17_present) {
    HANDLE_CODE(tac_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nsag_id_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tac_r17_present, 1));

  HANDLE_CODE(nsag_id_r17.unpack(bref));
  if (tac_r17_present) {
    HANDLE_CODE(tac_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nsag_id_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("nsag-ID-r17", nsag_id_r17.to_string());
  if (tac_r17_present) {
    j.write_str("trackingAreaCode-r17", tac_r17.to_string());
  }
  j.end_obj();
}

// Orbital-r17 ::= SEQUENCE
SRSASN_CODE orbital_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, semi_major_axis_r17, (uint64_t)0u, (uint64_t)8589934591u));
  HANDLE_CODE(pack_integer(bref, eccentricity_r17, (uint32_t)0u, (uint32_t)1048575u));
  HANDLE_CODE(pack_integer(bref, periapsis_r17, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(pack_integer(bref, longitude_r17, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(pack_integer(bref, inclination_r17, (int32_t)-67108864, (int32_t)67108863));
  HANDLE_CODE(pack_integer(bref, mean_anomaly_r17, (uint32_t)0u, (uint32_t)268435455u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE orbital_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(semi_major_axis_r17, bref, (uint64_t)0u, (uint64_t)8589934591u));
  HANDLE_CODE(unpack_integer(eccentricity_r17, bref, (uint32_t)0u, (uint32_t)1048575u));
  HANDLE_CODE(unpack_integer(periapsis_r17, bref, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(unpack_integer(longitude_r17, bref, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(unpack_integer(inclination_r17, bref, (int32_t)-67108864, (int32_t)67108863));
  HANDLE_CODE(unpack_integer(mean_anomaly_r17, bref, (uint32_t)0u, (uint32_t)268435455u));

  return SRSASN_SUCCESS;
}
void orbital_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("semiMajorAxis-r17", semi_major_axis_r17);
  j.write_int("eccentricity-r17", eccentricity_r17);
  j.write_int("periapsis-r17", periapsis_r17);
  j.write_int("longitude-r17", longitude_r17);
  j.write_int("inclination-r17", inclination_r17);
  j.write_int("meanAnomaly-r17", mean_anomaly_r17);
  j.end_obj();
}

// PositionVelocity-r17 ::= SEQUENCE
SRSASN_CODE position_velocity_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, position_x_r17, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(pack_integer(bref, position_y_r17, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(pack_integer(bref, position_z_r17, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(pack_integer(bref, velocity_vx_r17, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(pack_integer(bref, velocity_vy_r17, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(pack_integer(bref, velocity_vz_r17, (int32_t)-131072, (int32_t)131071));

  return SRSASN_SUCCESS;
}
SRSASN_CODE position_velocity_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(position_x_r17, bref, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(unpack_integer(position_y_r17, bref, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(unpack_integer(position_z_r17, bref, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(unpack_integer(velocity_vx_r17, bref, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(unpack_integer(velocity_vy_r17, bref, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(unpack_integer(velocity_vz_r17, bref, (int32_t)-131072, (int32_t)131071));

  return SRSASN_SUCCESS;
}
void position_velocity_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("positionX-r17", position_x_r17);
  j.write_int("positionY-r17", position_y_r17);
  j.write_int("positionZ-r17", position_z_r17);
  j.write_int("velocityVX-r17", velocity_vx_r17);
  j.write_int("velocityVY-r17", velocity_vy_r17);
  j.write_int("velocityVZ-r17", velocity_vz_r17);
  j.end_obj();
}

// Q-OffsetRange ::= ENUMERATED
const char* q_offset_range_opts::to_string() const
{
  static const char* names[] = {"dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10",
                                "dB-8",  "dB-6",  "dB-5",  "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",
                                "dB1",   "dB2",   "dB3",   "dB4",   "dB5",   "dB6",   "dB8",   "dB10",
                                "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
  return convert_enum_idx(names, 31, value, "q_offset_range_e");
}
int8_t q_offset_range_opts::to_number() const
{
  static const int8_t numbers[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 31, value, "q_offset_range_e");
}

// SSB-PositionQCL-Relation-r16 ::= ENUMERATED
const char* ssb_position_qcl_relation_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(names, 4, value, "ssb_position_qcl_relation_r16_e");
}
uint8_t ssb_position_qcl_relation_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "ssb_position_qcl_relation_r16_e");
}

// SSB-PositionQCL-Relation-r17 ::= ENUMERATED
const char* ssb_position_qcl_relation_r17_opts::to_string() const
{
  static const char* names[] = {"n32", "n64"};
  return convert_enum_idx(names, 2, value, "ssb_position_qcl_relation_r17_e");
}
uint8_t ssb_position_qcl_relation_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {32, 64};
  return map_enum_number(numbers, 2, value, "ssb_position_qcl_relation_r17_e");
}

// EUTRA-PhysCellIdRange ::= SEQUENCE
SRSASN_CODE eutra_pci_range_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(range_present, 1));

  HANDLE_CODE(pack_integer(bref, start, (uint16_t)0u, (uint16_t)503u));
  if (range_present) {
    HANDLE_CODE(range.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_pci_range_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(range_present, 1));

  HANDLE_CODE(unpack_integer(start, bref, (uint16_t)0u, (uint16_t)503u));
  if (range_present) {
    HANDLE_CODE(range.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void eutra_pci_range_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("start", start);
  if (range_present) {
    j.write_str("range", range.to_string());
  }
  j.end_obj();
}

const char* eutra_pci_range_s::range_opts::to_string() const
{
  static const char* names[] = {"n4",
                                "n8",
                                "n12",
                                "n16",
                                "n24",
                                "n32",
                                "n48",
                                "n64",
                                "n84",
                                "n96",
                                "n128",
                                "n168",
                                "n252",
                                "n504",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "eutra_pci_range_s::range_e_");
}
uint16_t eutra_pci_range_s::range_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504};
  return map_enum_number(numbers, 14, value, "eutra_pci_range_s::range_e_");
}

// EphemerisInfo-r17 ::= CHOICE
void ephemeris_info_r17_c::destroy_()
{
  switch (type_) {
    case types::position_velocity_r17:
      c.destroy<position_velocity_r17_s>();
      break;
    case types::orbital_r17:
      c.destroy<orbital_r17_s>();
      break;
    default:
      break;
  }
}
void ephemeris_info_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::position_velocity_r17:
      c.init<position_velocity_r17_s>();
      break;
    case types::orbital_r17:
      c.init<orbital_r17_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ephemeris_info_r17_c");
  }
}
ephemeris_info_r17_c::ephemeris_info_r17_c(const ephemeris_info_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::position_velocity_r17:
      c.init(other.c.get<position_velocity_r17_s>());
      break;
    case types::orbital_r17:
      c.init(other.c.get<orbital_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ephemeris_info_r17_c");
  }
}
ephemeris_info_r17_c& ephemeris_info_r17_c::operator=(const ephemeris_info_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::position_velocity_r17:
      c.set(other.c.get<position_velocity_r17_s>());
      break;
    case types::orbital_r17:
      c.set(other.c.get<orbital_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ephemeris_info_r17_c");
  }

  return *this;
}
position_velocity_r17_s& ephemeris_info_r17_c::set_position_velocity_r17()
{
  set(types::position_velocity_r17);
  return c.get<position_velocity_r17_s>();
}
orbital_r17_s& ephemeris_info_r17_c::set_orbital_r17()
{
  set(types::orbital_r17);
  return c.get<orbital_r17_s>();
}
void ephemeris_info_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::position_velocity_r17:
      j.write_fieldname("positionVelocity-r17");
      c.get<position_velocity_r17_s>().to_json(j);
      break;
    case types::orbital_r17:
      j.write_fieldname("orbital-r17");
      c.get<orbital_r17_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ephemeris_info_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE ephemeris_info_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::position_velocity_r17:
      HANDLE_CODE(c.get<position_velocity_r17_s>().pack(bref));
      break;
    case types::orbital_r17:
      HANDLE_CODE(c.get<orbital_r17_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ephemeris_info_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ephemeris_info_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::position_velocity_r17:
      HANDLE_CODE(c.get<position_velocity_r17_s>().unpack(bref));
      break;
    case types::orbital_r17:
      HANDLE_CODE(c.get<orbital_r17_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ephemeris_info_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// EpochTime-r17 ::= SEQUENCE
SRSASN_CODE epoch_time_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sfn_r17, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, sub_frame_nr_r17, (uint8_t)0u, (uint8_t)9u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE epoch_time_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sfn_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(sub_frame_nr_r17, bref, (uint8_t)0u, (uint8_t)9u));

  return SRSASN_SUCCESS;
}
void epoch_time_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sfn-r17", sfn_r17);
  j.write_int("subFrameNR-r17", sub_frame_nr_r17);
  j.end_obj();
}

// SSB-MTC4-r17 ::= SEQUENCE
SRSASN_CODE ssb_mtc4_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pci_list_r17.size() > 0, 1));

  if (pci_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_list_r17, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(pack_integer(bref, offset_r17, (uint8_t)0u, (uint8_t)159u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc4_r17_s::unpack(cbit_ref& bref)
{
  bool pci_list_r17_present;
  HANDLE_CODE(bref.unpack(pci_list_r17_present, 1));

  if (pci_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_list_r17, bref, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(unpack_integer(offset_r17, bref, (uint8_t)0u, (uint8_t)159u));

  return SRSASN_SUCCESS;
}
void ssb_mtc4_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_list_r17.size() > 0) {
    j.start_array("pci-List-r17");
    for (const auto& e1 : pci_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_int("offset-r17", offset_r17);
  j.end_obj();
}

// BeamMeasConfigIdle-NR-r16 ::= SEQUENCE
SRSASN_CODE beam_meas_cfg_idle_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(report_quant_rs_idxes_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_nrof_rs_idxes_to_report_r16, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(bref.pack(include_beam_meass_r16, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE beam_meas_cfg_idle_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(report_quant_rs_idxes_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(max_nrof_rs_idxes_to_report_r16, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(bref.unpack(include_beam_meass_r16, 1));

  return SRSASN_SUCCESS;
}
void beam_meas_cfg_idle_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reportQuantityRS-Indexes-r16", report_quant_rs_idxes_r16.to_string());
  j.write_int("maxNrofRS-IndexesToReport-r16", max_nrof_rs_idxes_to_report_r16);
  j.write_bool("includeBeamMeasurements-r16", include_beam_meass_r16);
  j.end_obj();
}

const char* beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "both"};
  return convert_enum_idx(names, 3, value, "beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16_e_");
}

// EUTRA-AllowedMeasBandwidth ::= ENUMERATED
const char* eutra_allowed_meas_bw_opts::to_string() const
{
  static const char* names[] = {"mbw6", "mbw15", "mbw25", "mbw50", "mbw75", "mbw100"};
  return convert_enum_idx(names, 6, value, "eutra_allowed_meas_bw_e");
}
uint8_t eutra_allowed_meas_bw_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "eutra_allowed_meas_bw_e");
}

// SS-RSSI-Measurement ::= SEQUENCE
SRSASN_CODE ss_rssi_meas_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(meas_slots.pack(bref));
  HANDLE_CODE(pack_integer(bref, end_symbol, (uint8_t)0u, (uint8_t)3u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ss_rssi_meas_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(meas_slots.unpack(bref));
  HANDLE_CODE(unpack_integer(end_symbol, bref, (uint8_t)0u, (uint8_t)3u));

  return SRSASN_SUCCESS;
}
void ss_rssi_meas_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measurementSlots", meas_slots.to_string());
  j.write_int("endSymbol", end_symbol);
  j.end_obj();
}

// SSB-MTC ::= SEQUENCE
SRSASN_CODE ssb_mtc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(periodicity_and_offset.pack(bref));
  HANDLE_CODE(dur.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(periodicity_and_offset.unpack(bref));
  HANDLE_CODE(dur.unpack(bref));

  return SRSASN_SUCCESS;
}
void ssb_mtc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityAndOffset");
  periodicity_and_offset.to_json(j);
  j.write_str("duration", dur.to_string());
  j.end_obj();
}

void ssb_mtc_s::periodicity_and_offset_c_::destroy_() {}
void ssb_mtc_s::periodicity_and_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
ssb_mtc_s::periodicity_and_offset_c_::periodicity_and_offset_c_(const ssb_mtc_s::periodicity_and_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf5:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_s::periodicity_and_offset_c_");
  }
}
ssb_mtc_s::periodicity_and_offset_c_&
ssb_mtc_s::periodicity_and_offset_c_::operator=(const ssb_mtc_s::periodicity_and_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf5:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_s::periodicity_and_offset_c_");
  }

  return *this;
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf5()
{
  set(types::sf5);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf10()
{
  set(types::sf10);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf20()
{
  set(types::sf20);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf40()
{
  set(types::sf40);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf80()
{
  set(types::sf80);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf160()
{
  set(types::sf160);
  return c.get<uint8_t>();
}
void ssb_mtc_s::periodicity_and_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf5:
      j.write_int("sf5", c.get<uint8_t>());
      break;
    case types::sf10:
      j.write_int("sf10", c.get<uint8_t>());
      break;
    case types::sf20:
      j.write_int("sf20", c.get<uint8_t>());
      break;
    case types::sf40:
      j.write_int("sf40", c.get<uint8_t>());
      break;
    case types::sf80:
      j.write_int("sf80", c.get<uint8_t>());
      break;
    case types::sf160:
      j.write_int("sf160", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_s::periodicity_and_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE ssb_mtc_s::periodicity_and_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf5:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sf10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_s::periodicity_and_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc_s::periodicity_and_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf5:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sf10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_s::periodicity_and_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ssb_mtc_s::dur_opts::to_string() const
{
  static const char* names[] = {"sf1", "sf2", "sf3", "sf4", "sf5"};
  return convert_enum_idx(names, 5, value, "ssb_mtc_s::dur_e_");
}
uint8_t ssb_mtc_s::dur_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5};
  return map_enum_number(numbers, 5, value, "ssb_mtc_s::dur_e_");
}

// SSB-ToMeasure ::= CHOICE
void ssb_to_measure_c::destroy_()
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
void ssb_to_measure_c::set(types::options e)
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
      log_invalid_choice_id(type_, "ssb_to_measure_c");
  }
}
ssb_to_measure_c::ssb_to_measure_c(const ssb_to_measure_c& other)
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
      log_invalid_choice_id(type_, "ssb_to_measure_c");
  }
}
ssb_to_measure_c& ssb_to_measure_c::operator=(const ssb_to_measure_c& other)
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
      log_invalid_choice_id(type_, "ssb_to_measure_c");
  }

  return *this;
}
fixed_bitstring<4>& ssb_to_measure_c::set_short_bitmap()
{
  set(types::short_bitmap);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>& ssb_to_measure_c::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& ssb_to_measure_c::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64>>();
}
void ssb_to_measure_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ssb_to_measure_c");
  }
  j.end_obj();
}
SRSASN_CODE ssb_to_measure_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "ssb_to_measure_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_to_measure_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ssb_to_measure_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ThresholdNR ::= SEQUENCE
SRSASN_CODE thres_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thres_rsrp_present, 1));
  HANDLE_CODE(bref.pack(thres_rsrq_present, 1));
  HANDLE_CODE(bref.pack(thres_sinr_present, 1));

  if (thres_rsrp_present) {
    HANDLE_CODE(pack_integer(bref, thres_rsrp, (uint8_t)0u, (uint8_t)127u));
  }
  if (thres_rsrq_present) {
    HANDLE_CODE(pack_integer(bref, thres_rsrq, (uint8_t)0u, (uint8_t)127u));
  }
  if (thres_sinr_present) {
    HANDLE_CODE(pack_integer(bref, thres_sinr, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE thres_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thres_rsrp_present, 1));
  HANDLE_CODE(bref.unpack(thres_rsrq_present, 1));
  HANDLE_CODE(bref.unpack(thres_sinr_present, 1));

  if (thres_rsrp_present) {
    HANDLE_CODE(unpack_integer(thres_rsrp, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (thres_rsrq_present) {
    HANDLE_CODE(unpack_integer(thres_rsrq, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (thres_sinr_present) {
    HANDLE_CODE(unpack_integer(thres_sinr, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void thres_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thres_rsrp_present) {
    j.write_int("thresholdRSRP", thres_rsrp);
  }
  if (thres_rsrq_present) {
    j.write_int("thresholdRSRQ", thres_rsrq);
  }
  if (thres_sinr_present) {
    j.write_int("thresholdSINR", thres_sinr);
  }
  j.end_obj();
}

// MeasIdleCarrierEUTRA-r16 ::= SEQUENCE
SRSASN_CODE meas_idle_carrier_eutra_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_cell_list_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(quality_thres_eutra_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_eutra_r16, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw_r16.pack(bref));
  if (meas_cell_list_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_cell_list_eutra_r16, 1, 8));
  }
  HANDLE_CODE(report_quantities_eutra_r16.pack(bref));
  if (quality_thres_eutra_r16_present) {
    HANDLE_CODE(bref.pack(quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16_present, 1));
    HANDLE_CODE(bref.pack(quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16_present, 1));
    if (quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16, (uint8_t)0u, (uint8_t)97u));
    }
    if (quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16, (int8_t)-30, (int8_t)46));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_carrier_eutra_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_cell_list_eutra_r16_present;
  HANDLE_CODE(bref.unpack(meas_cell_list_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(quality_thres_eutra_r16_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_eutra_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw_r16.unpack(bref));
  if (meas_cell_list_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_cell_list_eutra_r16, bref, 1, 8));
  }
  HANDLE_CODE(report_quantities_eutra_r16.unpack(bref));
  if (quality_thres_eutra_r16_present) {
    HANDLE_CODE(bref.unpack(quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16_present, 1));
    HANDLE_CODE(bref.unpack(quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16_present, 1));
    if (quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16_present) {
      HANDLE_CODE(unpack_integer(quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16, bref, (uint8_t)0u, (uint8_t)97u));
    }
    if (quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16_present) {
      HANDLE_CODE(unpack_integer(quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16, bref, (int8_t)-30, (int8_t)46));
    }
  }

  return SRSASN_SUCCESS;
}
void meas_idle_carrier_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreqEUTRA-r16", carrier_freq_eutra_r16);
  j.write_str("allowedMeasBandwidth-r16", allowed_meas_bw_r16.to_string());
  if (meas_cell_list_eutra_r16.size() > 0) {
    j.start_array("measCellListEUTRA-r16");
    for (const auto& e1 : meas_cell_list_eutra_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("reportQuantitiesEUTRA-r16", report_quantities_eutra_r16.to_string());
  if (quality_thres_eutra_r16_present) {
    j.write_fieldname("qualityThresholdEUTRA-r16");
    j.start_obj();
    if (quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16_present) {
      j.write_int("idleRSRP-Threshold-EUTRA-r16", quality_thres_eutra_r16.idle_rsrp_thres_eutra_r16);
    }
    if (quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16_present) {
      j.write_int("idleRSRQ-Threshold-EUTRA-r16", quality_thres_eutra_r16.idle_rsrq_thres_eutra_r16);
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "both"};
  return convert_enum_idx(names, 3, value, "meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16_e_");
}

// MeasIdleCarrierNR-r16 ::= SEQUENCE
SRSASN_CODE meas_idle_carrier_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_cell_list_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(quality_thres_r16_present, 1));
  HANDLE_CODE(bref.pack(ssb_meas_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(beam_meas_cfg_idle_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(ssb_subcarrier_spacing_r16.pack(bref));
  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (meas_cell_list_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_cell_list_nr_r16, 1, 8));
  }
  HANDLE_CODE(report_quantities_r16.pack(bref));
  if (quality_thres_r16_present) {
    HANDLE_CODE(bref.pack(quality_thres_r16.idle_rsrp_thres_nr_r16_present, 1));
    HANDLE_CODE(bref.pack(quality_thres_r16.idle_rsrq_thres_nr_r16_present, 1));
    if (quality_thres_r16.idle_rsrp_thres_nr_r16_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_r16.idle_rsrp_thres_nr_r16, (uint8_t)0u, (uint8_t)127u));
    }
    if (quality_thres_r16.idle_rsrq_thres_nr_r16_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_r16.idle_rsrq_thres_nr_r16, (uint8_t)0u, (uint8_t)127u));
    }
  }
  if (ssb_meas_cfg_r16_present) {
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.smtc_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.ssb_to_measure_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.ss_rssi_meas_r16_present, 1));
    if (ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16_present) {
      HANDLE_CODE(pack_integer(bref, ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16, (uint8_t)2u, (uint8_t)16u));
    }
    if (ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16.pack(bref));
    }
    if (ssb_meas_cfg_r16.smtc_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.smtc_r16.pack(bref));
    }
    if (ssb_meas_cfg_r16.ssb_to_measure_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ssb_to_measure_r16.pack(bref));
    }
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.derive_ssb_idx_from_cell_r16, 1));
    if (ssb_meas_cfg_r16.ss_rssi_meas_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ss_rssi_meas_r16.pack(bref));
    }
  }
  if (beam_meas_cfg_idle_r16_present) {
    HANDLE_CODE(beam_meas_cfg_idle_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_carrier_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  bool meas_cell_list_nr_r16_present;
  HANDLE_CODE(bref.unpack(meas_cell_list_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(quality_thres_r16_present, 1));
  HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(beam_meas_cfg_idle_r16_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(ssb_subcarrier_spacing_r16.unpack(bref));
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (meas_cell_list_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_cell_list_nr_r16, bref, 1, 8));
  }
  HANDLE_CODE(report_quantities_r16.unpack(bref));
  if (quality_thres_r16_present) {
    HANDLE_CODE(bref.unpack(quality_thres_r16.idle_rsrp_thres_nr_r16_present, 1));
    HANDLE_CODE(bref.unpack(quality_thres_r16.idle_rsrq_thres_nr_r16_present, 1));
    if (quality_thres_r16.idle_rsrp_thres_nr_r16_present) {
      HANDLE_CODE(unpack_integer(quality_thres_r16.idle_rsrp_thres_nr_r16, bref, (uint8_t)0u, (uint8_t)127u));
    }
    if (quality_thres_r16.idle_rsrq_thres_nr_r16_present) {
      HANDLE_CODE(unpack_integer(quality_thres_r16.idle_rsrq_thres_nr_r16, bref, (uint8_t)0u, (uint8_t)127u));
    }
  }
  if (ssb_meas_cfg_r16_present) {
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.smtc_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.ssb_to_measure_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.ss_rssi_meas_r16_present, 1));
    if (ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16_present) {
      HANDLE_CODE(unpack_integer(ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16, bref, (uint8_t)2u, (uint8_t)16u));
    }
    if (ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16.unpack(bref));
    }
    if (ssb_meas_cfg_r16.smtc_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.smtc_r16.unpack(bref));
    }
    if (ssb_meas_cfg_r16.ssb_to_measure_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ssb_to_measure_r16.unpack(bref));
    }
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.derive_ssb_idx_from_cell_r16, 1));
    if (ssb_meas_cfg_r16.ss_rssi_meas_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ss_rssi_meas_r16.unpack(bref));
    }
  }
  if (beam_meas_cfg_idle_r16_present) {
    HANDLE_CODE(beam_meas_cfg_idle_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_idle_carrier_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.write_str("ssbSubcarrierSpacing-r16", ssb_subcarrier_spacing_r16.to_string());
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (meas_cell_list_nr_r16.size() > 0) {
    j.start_array("measCellListNR-r16");
    for (const auto& e1 : meas_cell_list_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("reportQuantities-r16", report_quantities_r16.to_string());
  if (quality_thres_r16_present) {
    j.write_fieldname("qualityThreshold-r16");
    j.start_obj();
    if (quality_thres_r16.idle_rsrp_thres_nr_r16_present) {
      j.write_int("idleRSRP-Threshold-NR-r16", quality_thres_r16.idle_rsrp_thres_nr_r16);
    }
    if (quality_thres_r16.idle_rsrq_thres_nr_r16_present) {
      j.write_int("idleRSRQ-Threshold-NR-r16", quality_thres_r16.idle_rsrq_thres_nr_r16);
    }
    j.end_obj();
  }
  if (ssb_meas_cfg_r16_present) {
    j.write_fieldname("ssb-MeasConfig-r16");
    j.start_obj();
    if (ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16_present) {
      j.write_int("nrofSS-BlocksToAverage-r16", ssb_meas_cfg_r16.nrof_ss_blocks_to_average_r16);
    }
    if (ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16_present) {
      j.write_fieldname("absThreshSS-BlocksConsolidation-r16");
      ssb_meas_cfg_r16.abs_thresh_ss_blocks_consolidation_r16.to_json(j);
    }
    if (ssb_meas_cfg_r16.smtc_r16_present) {
      j.write_fieldname("smtc-r16");
      ssb_meas_cfg_r16.smtc_r16.to_json(j);
    }
    if (ssb_meas_cfg_r16.ssb_to_measure_r16_present) {
      j.write_fieldname("ssb-ToMeasure-r16");
      ssb_meas_cfg_r16.ssb_to_measure_r16.to_json(j);
    }
    j.write_bool("deriveSSB-IndexFromCell-r16", ssb_meas_cfg_r16.derive_ssb_idx_from_cell_r16);
    if (ssb_meas_cfg_r16.ss_rssi_meas_r16_present) {
      j.write_fieldname("ss-RSSI-Measurement-r16");
      ssb_meas_cfg_r16.ss_rssi_meas_r16.to_json(j);
    }
    j.end_obj();
  }
  if (beam_meas_cfg_idle_r16_present) {
    j.write_fieldname("beamMeasConfigIdle-r16");
    beam_meas_cfg_idle_r16.to_json(j);
  }
  j.end_obj();
}

const char* meas_idle_carrier_nr_r16_s::report_quantities_r16_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "both"};
  return convert_enum_idx(names, 3, value, "meas_idle_carrier_nr_r16_s::report_quantities_r16_e_");
}

// ReferenceTime-r16 ::= SEQUENCE
SRSASN_CODE ref_time_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ref_days_r16, (uint32_t)0u, (uint32_t)72999u));
  HANDLE_CODE(pack_integer(bref, ref_seconds_r16, (uint32_t)0u, (uint32_t)86399u));
  HANDLE_CODE(pack_integer(bref, ref_milli_seconds_r16, (uint16_t)0u, (uint16_t)999u));
  HANDLE_CODE(pack_integer(bref, ref_ten_nano_seconds_r16, (uint32_t)0u, (uint32_t)99999u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ref_time_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ref_days_r16, bref, (uint32_t)0u, (uint32_t)72999u));
  HANDLE_CODE(unpack_integer(ref_seconds_r16, bref, (uint32_t)0u, (uint32_t)86399u));
  HANDLE_CODE(unpack_integer(ref_milli_seconds_r16, bref, (uint16_t)0u, (uint16_t)999u));
  HANDLE_CODE(unpack_integer(ref_ten_nano_seconds_r16, bref, (uint32_t)0u, (uint32_t)99999u));

  return SRSASN_SUCCESS;
}
void ref_time_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("refDays-r16", ref_days_r16);
  j.write_int("refSeconds-r16", ref_seconds_r16);
  j.write_int("refMilliSeconds-r16", ref_milli_seconds_r16);
  j.write_int("refTenNanoSeconds-r16", ref_ten_nano_seconds_r16);
  j.end_obj();
}

// ReferenceTimeInfo-r16 ::= SEQUENCE
SRSASN_CODE ref_time_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uncertainty_r16_present, 1));
  HANDLE_CODE(bref.pack(time_info_type_r16_present, 1));
  HANDLE_CODE(bref.pack(ref_sfn_r16_present, 1));

  HANDLE_CODE(time_r16.pack(bref));
  if (uncertainty_r16_present) {
    HANDLE_CODE(pack_integer(bref, uncertainty_r16, (uint16_t)0u, (uint16_t)32767u));
  }
  if (ref_sfn_r16_present) {
    HANDLE_CODE(pack_integer(bref, ref_sfn_r16, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ref_time_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uncertainty_r16_present, 1));
  HANDLE_CODE(bref.unpack(time_info_type_r16_present, 1));
  HANDLE_CODE(bref.unpack(ref_sfn_r16_present, 1));

  HANDLE_CODE(time_r16.unpack(bref));
  if (uncertainty_r16_present) {
    HANDLE_CODE(unpack_integer(uncertainty_r16, bref, (uint16_t)0u, (uint16_t)32767u));
  }
  if (ref_sfn_r16_present) {
    HANDLE_CODE(unpack_integer(ref_sfn_r16, bref, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
void ref_time_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("time-r16");
  time_r16.to_json(j);
  if (uncertainty_r16_present) {
    j.write_int("uncertainty-r16", uncertainty_r16);
  }
  if (time_info_type_r16_present) {
    j.write_str("timeInfoType-r16", "localClock");
  }
  if (ref_sfn_r16_present) {
    j.write_int("referenceSFN-r16", ref_sfn_r16);
  }
  j.end_obj();
}

// CSI-RS-Resource-Mobility ::= SEQUENCE
SRSASN_CODE csi_rs_res_mob_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(associated_ssb_present, 1));

  HANDLE_CODE(pack_integer(bref, csi_rs_idx, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(slot_cfg.pack(bref));
  if (associated_ssb_present) {
    HANDLE_CODE(pack_integer(bref, associated_ssb.ssb_idx, (uint8_t)0u, (uint8_t)63u));
    HANDLE_CODE(bref.pack(associated_ssb.is_quasi_colocated, 1));
  }
  HANDLE_CODE(freq_domain_alloc.pack(bref));
  HANDLE_CODE(pack_integer(bref, first_ofdm_symbol_in_time_domain, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(pack_integer(bref, seq_generation_cfg, (uint16_t)0u, (uint16_t)1023u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= slot_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(slot_cfg_r17.is_present(), 1));
      if (slot_cfg_r17.is_present()) {
        HANDLE_CODE(slot_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_mob_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(associated_ssb_present, 1));

  HANDLE_CODE(unpack_integer(csi_rs_idx, bref, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(slot_cfg.unpack(bref));
  if (associated_ssb_present) {
    HANDLE_CODE(unpack_integer(associated_ssb.ssb_idx, bref, (uint8_t)0u, (uint8_t)63u));
    HANDLE_CODE(bref.unpack(associated_ssb.is_quasi_colocated, 1));
  }
  HANDLE_CODE(freq_domain_alloc.unpack(bref));
  HANDLE_CODE(unpack_integer(first_ofdm_symbol_in_time_domain, bref, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(unpack_integer(seq_generation_cfg, bref, (uint16_t)0u, (uint16_t)1023u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(slot_cfg_r17, bref);
      if (slot_cfg_r17.is_present()) {
        HANDLE_CODE(slot_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void csi_rs_res_mob_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS-Index", csi_rs_idx);
  j.write_fieldname("slotConfig");
  slot_cfg.to_json(j);
  if (associated_ssb_present) {
    j.write_fieldname("associatedSSB");
    j.start_obj();
    j.write_int("ssb-Index", associated_ssb.ssb_idx);
    j.write_bool("isQuasiColocated", associated_ssb.is_quasi_colocated);
    j.end_obj();
  }
  j.write_fieldname("frequencyDomainAllocation");
  freq_domain_alloc.to_json(j);
  j.write_int("firstOFDMSymbolInTimeDomain", first_ofdm_symbol_in_time_domain);
  j.write_int("sequenceGenerationConfig", seq_generation_cfg);
  if (ext) {
    if (slot_cfg_r17.is_present()) {
      j.write_fieldname("slotConfig-r17");
      slot_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

void csi_rs_res_mob_s::slot_cfg_c_::destroy_() {}
void csi_rs_res_mob_s::slot_cfg_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
csi_rs_res_mob_s::slot_cfg_c_::slot_cfg_c_(const csi_rs_res_mob_s::slot_cfg_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms4:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms5:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_c_");
  }
}
csi_rs_res_mob_s::slot_cfg_c_& csi_rs_res_mob_s::slot_cfg_c_::operator=(const csi_rs_res_mob_s::slot_cfg_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms4:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms5:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_c_");
  }

  return *this;
}
uint8_t& csi_rs_res_mob_s::slot_cfg_c_::set_ms4()
{
  set(types::ms4);
  return c.get<uint8_t>();
}
uint8_t& csi_rs_res_mob_s::slot_cfg_c_::set_ms5()
{
  set(types::ms5);
  return c.get<uint8_t>();
}
uint8_t& csi_rs_res_mob_s::slot_cfg_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& csi_rs_res_mob_s::slot_cfg_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint16_t& csi_rs_res_mob_s::slot_cfg_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint16_t>();
}
void csi_rs_res_mob_s::slot_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms4:
      j.write_int("ms4", c.get<uint8_t>());
      break;
    case types::ms5:
      j.write_int("ms5", c.get<uint8_t>());
      break;
    case types::ms10:
      j.write_int("ms10", c.get<uint8_t>());
      break;
    case types::ms20:
      j.write_int("ms20", c.get<uint8_t>());
      break;
    case types::ms40:
      j.write_int("ms40", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_res_mob_s::slot_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms4:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms5:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms40:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_mob_s::slot_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms4:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms5:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms40:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void csi_rs_res_mob_s::freq_domain_alloc_c_::destroy_()
{
  switch (type_) {
    case types::row1:
      c.destroy<fixed_bitstring<4>>();
      break;
    case types::row2:
      c.destroy<fixed_bitstring<12>>();
      break;
    default:
      break;
  }
}
void csi_rs_res_mob_s::freq_domain_alloc_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::row1:
      c.init<fixed_bitstring<4>>();
      break;
    case types::row2:
      c.init<fixed_bitstring<12>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::freq_domain_alloc_c_");
  }
}
csi_rs_res_mob_s::freq_domain_alloc_c_::freq_domain_alloc_c_(const csi_rs_res_mob_s::freq_domain_alloc_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::row1:
      c.init(other.c.get<fixed_bitstring<4>>());
      break;
    case types::row2:
      c.init(other.c.get<fixed_bitstring<12>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::freq_domain_alloc_c_");
  }
}
csi_rs_res_mob_s::freq_domain_alloc_c_&
csi_rs_res_mob_s::freq_domain_alloc_c_::operator=(const csi_rs_res_mob_s::freq_domain_alloc_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::row1:
      c.set(other.c.get<fixed_bitstring<4>>());
      break;
    case types::row2:
      c.set(other.c.get<fixed_bitstring<12>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::freq_domain_alloc_c_");
  }

  return *this;
}
fixed_bitstring<4>& csi_rs_res_mob_s::freq_domain_alloc_c_::set_row1()
{
  set(types::row1);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<12>& csi_rs_res_mob_s::freq_domain_alloc_c_::set_row2()
{
  set(types::row2);
  return c.get<fixed_bitstring<12>>();
}
void csi_rs_res_mob_s::freq_domain_alloc_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::row1:
      j.write_str("row1", c.get<fixed_bitstring<4>>().to_string());
      break;
    case types::row2:
      j.write_str("row2", c.get<fixed_bitstring<12>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::freq_domain_alloc_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_res_mob_s::freq_domain_alloc_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::row1:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().pack(bref));
      break;
    case types::row2:
      HANDLE_CODE(c.get<fixed_bitstring<12>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::freq_domain_alloc_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_mob_s::freq_domain_alloc_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::row1:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().unpack(bref));
      break;
    case types::row2:
      HANDLE_CODE(c.get<fixed_bitstring<12>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::freq_domain_alloc_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void csi_rs_res_mob_s::slot_cfg_r17_c_::destroy_() {}
void csi_rs_res_mob_s::slot_cfg_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
csi_rs_res_mob_s::slot_cfg_r17_c_::slot_cfg_r17_c_(const csi_rs_res_mob_s::slot_cfg_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms4:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms5:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms10:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms20:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms40:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_r17_c_");
  }
}
csi_rs_res_mob_s::slot_cfg_r17_c_&
csi_rs_res_mob_s::slot_cfg_r17_c_::operator=(const csi_rs_res_mob_s::slot_cfg_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms4:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms5:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms10:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms20:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms40:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_r17_c_");
  }

  return *this;
}
uint16_t& csi_rs_res_mob_s::slot_cfg_r17_c_::set_ms4()
{
  set(types::ms4);
  return c.get<uint16_t>();
}
uint16_t& csi_rs_res_mob_s::slot_cfg_r17_c_::set_ms5()
{
  set(types::ms5);
  return c.get<uint16_t>();
}
uint16_t& csi_rs_res_mob_s::slot_cfg_r17_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint16_t>();
}
uint16_t& csi_rs_res_mob_s::slot_cfg_r17_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint16_t>();
}
uint16_t& csi_rs_res_mob_s::slot_cfg_r17_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint16_t>();
}
void csi_rs_res_mob_s::slot_cfg_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms4:
      j.write_int("ms4", c.get<uint16_t>());
      break;
    case types::ms5:
      j.write_int("ms5", c.get<uint16_t>());
      break;
    case types::ms10:
      j.write_int("ms10", c.get<uint16_t>());
      break;
    case types::ms20:
      j.write_int("ms20", c.get<uint16_t>());
      break;
    case types::ms40:
      j.write_int("ms40", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_res_mob_s::slot_cfg_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms4:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms5:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms10:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms20:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms40:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_mob_s::slot_cfg_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms4:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms5:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms10:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms20:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms40:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::slot_cfg_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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

// RSSI-PeriodicityAndOffset-r16 ::= CHOICE
void rssi_periodicity_and_offset_r16_c::destroy_() {}
void rssi_periodicity_and_offset_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
rssi_periodicity_and_offset_r16_c::rssi_periodicity_and_offset_r16_c(const rssi_periodicity_and_offset_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::s1640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rssi_periodicity_and_offset_r16_c");
  }
}
rssi_periodicity_and_offset_r16_c&
rssi_periodicity_and_offset_r16_c::operator=(const rssi_periodicity_and_offset_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::s1640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rssi_periodicity_and_offset_r16_c");
  }

  return *this;
}
uint8_t& rssi_periodicity_and_offset_r16_c::set_sl10()
{
  set(types::sl10);
  return c.get<uint8_t>();
}
uint8_t& rssi_periodicity_and_offset_r16_c::set_sl20()
{
  set(types::sl20);
  return c.get<uint8_t>();
}
uint8_t& rssi_periodicity_and_offset_r16_c::set_sl40()
{
  set(types::sl40);
  return c.get<uint8_t>();
}
uint8_t& rssi_periodicity_and_offset_r16_c::set_sl80()
{
  set(types::sl80);
  return c.get<uint8_t>();
}
uint8_t& rssi_periodicity_and_offset_r16_c::set_sl160()
{
  set(types::sl160);
  return c.get<uint8_t>();
}
uint16_t& rssi_periodicity_and_offset_r16_c::set_sl320()
{
  set(types::sl320);
  return c.get<uint16_t>();
}
uint16_t& rssi_periodicity_and_offset_r16_c::set_s1640()
{
  set(types::s1640);
  return c.get<uint16_t>();
}
void rssi_periodicity_and_offset_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl10:
      j.write_int("sl10", c.get<uint8_t>());
      break;
    case types::sl20:
      j.write_int("sl20", c.get<uint8_t>());
      break;
    case types::sl40:
      j.write_int("sl40", c.get<uint8_t>());
      break;
    case types::sl80:
      j.write_int("sl80", c.get<uint8_t>());
      break;
    case types::sl160:
      j.write_int("sl160", c.get<uint8_t>());
      break;
    case types::sl320:
      j.write_int("sl320", c.get<uint16_t>());
      break;
    case types::s1640:
      j.write_int("s1640", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rssi_periodicity_and_offset_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE rssi_periodicity_and_offset_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sl20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sl40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sl160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sl320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::s1640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    default:
      log_invalid_choice_id(type_, "rssi_periodicity_and_offset_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rssi_periodicity_and_offset_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sl20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sl40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sl160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sl320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::s1640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    default:
      log_invalid_choice_id(type_, "rssi_periodicity_and_offset_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SSB-PositionQCL-Cell-r17 ::= SEQUENCE
SRSASN_CODE ssb_position_qcl_cell_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(ssb_position_qcl_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_position_qcl_cell_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(ssb_position_qcl_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void ssb_position_qcl_cell_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r17", pci_r17);
  j.write_str("ssb-PositionQCL-r17", ssb_position_qcl_r17.to_string());
  j.end_obj();
}

// SSB-PositionQCL-CellsToAddMod-r16 ::= SEQUENCE
SRSASN_CODE ssb_position_qcl_cells_to_add_mod_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r16, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(ssb_position_qcl_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_position_qcl_cells_to_add_mod_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r16, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(ssb_position_qcl_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void ssb_position_qcl_cells_to_add_mod_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r16", pci_r16);
  j.write_str("ssb-PositionQCL-r16", ssb_position_qcl_r16.to_string());
  j.end_obj();
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

// CSI-RS-CellMobility ::= SEQUENCE
SRSASN_CODE csi_rs_cell_mob_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(density_present, 1));

  HANDLE_CODE(pack_integer(bref, cell_id, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(csi_rs_meas_bw.nrof_prbs.pack(bref));
  HANDLE_CODE(pack_integer(bref, csi_rs_meas_bw.start_prb, (uint16_t)0u, (uint16_t)2169u));
  if (density_present) {
    HANDLE_CODE(density.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, csi_rs_res_list_mob, 1, 96));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_cell_mob_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(density_present, 1));

  HANDLE_CODE(unpack_integer(cell_id, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(csi_rs_meas_bw.nrof_prbs.unpack(bref));
  HANDLE_CODE(unpack_integer(csi_rs_meas_bw.start_prb, bref, (uint16_t)0u, (uint16_t)2169u));
  if (density_present) {
    HANDLE_CODE(density.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(csi_rs_res_list_mob, bref, 1, 96));

  return SRSASN_SUCCESS;
}
void csi_rs_cell_mob_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellId", cell_id);
  j.write_fieldname("csi-rs-MeasurementBW");
  j.start_obj();
  j.write_str("nrofPRBs", csi_rs_meas_bw.nrof_prbs.to_string());
  j.write_int("startPRB", csi_rs_meas_bw.start_prb);
  j.end_obj();
  if (density_present) {
    j.write_str("density", density.to_string());
  }
  j.start_array("csi-rs-ResourceList-Mobility");
  for (const auto& e1 : csi_rs_res_list_mob) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* csi_rs_cell_mob_s::csi_rs_meas_bw_s_::nrof_prbs_opts::to_string() const
{
  static const char* names[] = {"size24", "size48", "size96", "size192", "size264"};
  return convert_enum_idx(names, 5, value, "csi_rs_cell_mob_s::csi_rs_meas_bw_s_::nrof_prbs_e_");
}
uint16_t csi_rs_cell_mob_s::csi_rs_meas_bw_s_::nrof_prbs_opts::to_number() const
{
  static const uint16_t numbers[] = {24, 48, 96, 192, 264};
  return map_enum_number(numbers, 5, value, "csi_rs_cell_mob_s::csi_rs_meas_bw_s_::nrof_prbs_e_");
}

const char* csi_rs_cell_mob_s::density_opts::to_string() const
{
  static const char* names[] = {"d1", "d3"};
  return convert_enum_idx(names, 2, value, "csi_rs_cell_mob_s::density_e_");
}
uint8_t csi_rs_cell_mob_s::density_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 3};
  return map_enum_number(numbers, 2, value, "csi_rs_cell_mob_s::density_e_");
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

// ExcessDelay-DRB-IdentityInfo-r17 ::= SEQUENCE
SRSASN_CODE excess_delay_drb_id_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, drb_id_list, 1, 29, integer_packer<uint8_t>(1, 32)));
  HANDLE_CODE(delay_thres.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE excess_delay_drb_id_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(drb_id_list, bref, 1, 29, integer_packer<uint8_t>(1, 32)));
  HANDLE_CODE(delay_thres.unpack(bref));

  return SRSASN_SUCCESS;
}
void excess_delay_drb_id_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("drb-IdentityList");
  for (const auto& e1 : drb_id_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_str("delayThreshold", delay_thres.to_string());
  j.end_obj();
}

const char* excess_delay_drb_id_info_r17_s::delay_thres_opts::to_string() const
{
  static const char* names[] = {"ms0dot25",
                                "ms0dot5",
                                "ms1",
                                "ms2",
                                "ms4",
                                "ms5",
                                "ms10",
                                "ms20",
                                "ms30",
                                "ms40",
                                "ms50",
                                "ms60",
                                "ms70",
                                "ms80",
                                "ms90",
                                "ms100",
                                "ms150",
                                "ms300",
                                "ms500"};
  return convert_enum_idx(names, 19, value, "excess_delay_drb_id_info_r17_s::delay_thres_e_");
}
float excess_delay_drb_id_info_r17_s::delay_thres_opts::to_number() const
{
  static const float numbers[] = {
      0.25, 0.5, 1.0, 2.0, 4.0, 5.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 150.0, 300.0, 500.0};
  return map_enum_number(numbers, 19, value, "excess_delay_drb_id_info_r17_s::delay_thres_e_");
}
const char* excess_delay_drb_id_info_r17_s::delay_thres_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.25",
                                      "0.5",
                                      "1",
                                      "2",
                                      "4",
                                      "5",
                                      "10",
                                      "20",
                                      "30",
                                      "40",
                                      "50",
                                      "60",
                                      "70",
                                      "80",
                                      "90",
                                      "100",
                                      "150",
                                      "300",
                                      "500"};
  return convert_enum_idx(number_strs, 19, value, "excess_delay_drb_id_info_r17_s::delay_thres_e_");
}

// IAB-IP-Usage-r16 ::= ENUMERATED
const char* iab_ip_usage_r16_opts::to_string() const
{
  static const char* names[] = {"f1-C", "f1-U", "non-F1", "spare"};
  return convert_enum_idx(names, 4, value, "iab_ip_usage_r16_e");
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

// Q-OffsetRangeList ::= SEQUENCE
SRSASN_CODE q_offset_range_list_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_offset_ssb_present, 1));
  HANDLE_CODE(bref.pack(rsrq_offset_ssb_present, 1));
  HANDLE_CODE(bref.pack(sinr_offset_ssb_present, 1));
  HANDLE_CODE(bref.pack(rsrp_offset_csi_rs_present, 1));
  HANDLE_CODE(bref.pack(rsrq_offset_csi_rs_present, 1));
  HANDLE_CODE(bref.pack(sinr_offset_csi_rs_present, 1));

  if (rsrp_offset_ssb_present) {
    HANDLE_CODE(rsrp_offset_ssb.pack(bref));
  }
  if (rsrq_offset_ssb_present) {
    HANDLE_CODE(rsrq_offset_ssb.pack(bref));
  }
  if (sinr_offset_ssb_present) {
    HANDLE_CODE(sinr_offset_ssb.pack(bref));
  }
  if (rsrp_offset_csi_rs_present) {
    HANDLE_CODE(rsrp_offset_csi_rs.pack(bref));
  }
  if (rsrq_offset_csi_rs_present) {
    HANDLE_CODE(rsrq_offset_csi_rs.pack(bref));
  }
  if (sinr_offset_csi_rs_present) {
    HANDLE_CODE(sinr_offset_csi_rs.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE q_offset_range_list_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_offset_ssb_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_offset_ssb_present, 1));
  HANDLE_CODE(bref.unpack(sinr_offset_ssb_present, 1));
  HANDLE_CODE(bref.unpack(rsrp_offset_csi_rs_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_offset_csi_rs_present, 1));
  HANDLE_CODE(bref.unpack(sinr_offset_csi_rs_present, 1));

  if (rsrp_offset_ssb_present) {
    HANDLE_CODE(rsrp_offset_ssb.unpack(bref));
  }
  if (rsrq_offset_ssb_present) {
    HANDLE_CODE(rsrq_offset_ssb.unpack(bref));
  }
  if (sinr_offset_ssb_present) {
    HANDLE_CODE(sinr_offset_ssb.unpack(bref));
  }
  if (rsrp_offset_csi_rs_present) {
    HANDLE_CODE(rsrp_offset_csi_rs.unpack(bref));
  }
  if (rsrq_offset_csi_rs_present) {
    HANDLE_CODE(rsrq_offset_csi_rs.unpack(bref));
  }
  if (sinr_offset_csi_rs_present) {
    HANDLE_CODE(sinr_offset_csi_rs.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void q_offset_range_list_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_offset_ssb_present) {
    j.write_str("rsrpOffsetSSB", rsrp_offset_ssb.to_string());
  }
  if (rsrq_offset_ssb_present) {
    j.write_str("rsrqOffsetSSB", rsrq_offset_ssb.to_string());
  }
  if (sinr_offset_ssb_present) {
    j.write_str("sinrOffsetSSB", sinr_offset_ssb.to_string());
  }
  if (rsrp_offset_csi_rs_present) {
    j.write_str("rsrpOffsetCSI-RS", rsrp_offset_csi_rs.to_string());
  }
  if (rsrq_offset_csi_rs_present) {
    j.write_str("rsrqOffsetCSI-RS", rsrq_offset_csi_rs.to_string());
  }
  if (sinr_offset_csi_rs_present) {
    j.write_str("sinrOffsetCSI-RS", sinr_offset_csi_rs.to_string());
  }
  j.end_obj();
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

// RSSI-ResourceConfigCLI-r16 ::= SEQUENCE
SRSASN_CODE rssi_res_cfg_cli_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ref_serv_cell_idx_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, rssi_res_id_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(rssi_scs_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, start_prb_r16, (uint16_t)0u, (uint16_t)2169u));
  HANDLE_CODE(pack_integer(bref, nrof_prbs_r16, (uint16_t)4u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, start_position_r16, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(pack_integer(bref, nrof_symbols_r16, (uint8_t)1u, (uint8_t)14u));
  HANDLE_CODE(rssi_periodicity_and_offset_r16.pack(bref));
  if (ref_serv_cell_idx_r16_present) {
    HANDLE_CODE(pack_integer(bref, ref_serv_cell_idx_r16, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rssi_res_cfg_cli_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ref_serv_cell_idx_r16_present, 1));

  HANDLE_CODE(unpack_integer(rssi_res_id_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(rssi_scs_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(start_prb_r16, bref, (uint16_t)0u, (uint16_t)2169u));
  HANDLE_CODE(unpack_integer(nrof_prbs_r16, bref, (uint16_t)4u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(start_position_r16, bref, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(unpack_integer(nrof_symbols_r16, bref, (uint8_t)1u, (uint8_t)14u));
  HANDLE_CODE(rssi_periodicity_and_offset_r16.unpack(bref));
  if (ref_serv_cell_idx_r16_present) {
    HANDLE_CODE(unpack_integer(ref_serv_cell_idx_r16, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void rssi_res_cfg_cli_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rssi-ResourceId-r16", rssi_res_id_r16);
  j.write_str("rssi-SCS-r16", rssi_scs_r16.to_string());
  j.write_int("startPRB-r16", start_prb_r16);
  j.write_int("nrofPRBs-r16", nrof_prbs_r16);
  j.write_int("startPosition-r16", start_position_r16);
  j.write_int("nrofSymbols-r16", nrof_symbols_r16);
  j.write_fieldname("rssi-PeriodicityAndOffset-r16");
  rssi_periodicity_and_offset_r16.to_json(j);
  if (ref_serv_cell_idx_r16_present) {
    j.write_int("refServCellIndex-r16", ref_serv_cell_idx_r16);
  }
  j.end_obj();
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

// SRS-ResourceConfigCLI-r16 ::= SEQUENCE
SRSASN_CODE srs_res_cfg_cli_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ref_serv_cell_idx_r16_present, 1));

  HANDLE_CODE(srs_res_r16.pack(bref));
  HANDLE_CODE(srs_scs_r16.pack(bref));
  if (ref_serv_cell_idx_r16_present) {
    HANDLE_CODE(pack_integer(bref, ref_serv_cell_idx_r16, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, ref_bwp_r16, (uint8_t)0u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_cfg_cli_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ref_serv_cell_idx_r16_present, 1));

  HANDLE_CODE(srs_res_r16.unpack(bref));
  HANDLE_CODE(srs_scs_r16.unpack(bref));
  if (ref_serv_cell_idx_r16_present) {
    HANDLE_CODE(unpack_integer(ref_serv_cell_idx_r16, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(ref_bwp_r16, bref, (uint8_t)0u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
void srs_res_cfg_cli_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("srs-Resource-r16");
  srs_res_r16.to_json(j);
  j.write_str("srs-SCS-r16", srs_scs_r16.to_string());
  if (ref_serv_cell_idx_r16_present) {
    j.write_int("refServCellIndex-r16", ref_serv_cell_idx_r16);
  }
  j.write_int("refBWP-r16", ref_bwp_r16);
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

// CSI-RS-ResourceConfigMobility ::= SEQUENCE
SRSASN_CODE csi_rs_res_cfg_mob_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(subcarrier_spacing.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, csi_rs_cell_list_mob, 1, 96));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ref_serv_cell_idx_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ref_serv_cell_idx_present, 1));
      if (ref_serv_cell_idx_present) {
        HANDLE_CODE(pack_integer(bref, ref_serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_cfg_mob_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(subcarrier_spacing.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(csi_rs_cell_list_mob, bref, 1, 96));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ref_serv_cell_idx_present, 1));
      if (ref_serv_cell_idx_present) {
        HANDLE_CODE(unpack_integer(ref_serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void csi_rs_res_cfg_mob_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("subcarrierSpacing", subcarrier_spacing.to_string());
  j.start_array("csi-RS-CellList-Mobility");
  for (const auto& e1 : csi_rs_cell_list_mob) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (ref_serv_cell_idx_present) {
      j.write_int("refServCellIndex", ref_serv_cell_idx);
    }
  }
  j.end_obj();
}

// CellsToAddMod ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(cell_individual_offset.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(cell_individual_offset.unpack(bref));

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_fieldname("cellIndividualOffset");
  cell_individual_offset.to_json(j);
  j.end_obj();
}

// CellsToAddModExt-v1710 ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_ext_v1710_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ntn_polarization_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(ntn_polarization_ul_r17_present, 1));

  if (ntn_polarization_dl_r17_present) {
    HANDLE_CODE(ntn_polarization_dl_r17.pack(bref));
  }
  if (ntn_polarization_ul_r17_present) {
    HANDLE_CODE(ntn_polarization_ul_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_ext_v1710_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ntn_polarization_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(ntn_polarization_ul_r17_present, 1));

  if (ntn_polarization_dl_r17_present) {
    HANDLE_CODE(ntn_polarization_dl_r17.unpack(bref));
  }
  if (ntn_polarization_ul_r17_present) {
    HANDLE_CODE(ntn_polarization_ul_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_ext_v1710_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ntn_polarization_dl_r17_present) {
    j.write_str("ntn-PolarizationDL-r17", ntn_polarization_dl_r17.to_string());
  }
  if (ntn_polarization_ul_r17_present) {
    j.write_str("ntn-PolarizationUL-r17", ntn_polarization_ul_r17.to_string());
  }
  j.end_obj();
}

const char* cells_to_add_mod_ext_v1710_s::ntn_polarization_dl_r17_opts::to_string() const
{
  static const char* names[] = {"rhcp", "lhcp", "linear"};
  return convert_enum_idx(names, 3, value, "cells_to_add_mod_ext_v1710_s::ntn_polarization_dl_r17_e_");
}

const char* cells_to_add_mod_ext_v1710_s::ntn_polarization_ul_r17_opts::to_string() const
{
  static const char* names[] = {"rhcp", "lhcp", "linear"};
  return convert_enum_idx(names, 3, value, "cells_to_add_mod_ext_v1710_s::ntn_polarization_ul_r17_e_");
}

// CellsToAddModUTRA-FDD-r16 ::= SEQUENCE
SRSASN_CODE cells_to_add_mod_utra_fdd_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx_utra_fdd_r16, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, pci_r16, (uint16_t)0u, (uint16_t)511u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_add_mod_utra_fdd_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx_utra_fdd_r16, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(pci_r16, bref, (uint16_t)0u, (uint16_t)511u));

  return SRSASN_SUCCESS;
}
void cells_to_add_mod_utra_fdd_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndexUTRA-FDD-r16", cell_idx_utra_fdd_r16);
  j.write_int("physCellId-r16", pci_r16);
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

// MeasRSSI-ReportConfig-r16 ::= SEQUENCE
SRSASN_CODE meas_rssi_report_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ch_occupancy_thres_r16_present, 1));

  if (ch_occupancy_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, ch_occupancy_thres_r16, (uint8_t)0u, (uint8_t)76u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_rssi_report_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ch_occupancy_thres_r16_present, 1));

  if (ch_occupancy_thres_r16_present) {
    HANDLE_CODE(unpack_integer(ch_occupancy_thres_r16, bref, (uint8_t)0u, (uint8_t)76u));
  }

  return SRSASN_SUCCESS;
}
void meas_rssi_report_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ch_occupancy_thres_r16_present) {
    j.write_int("channelOccupancyThreshold-r16", ch_occupancy_thres_r16);
  }
  j.end_obj();
}

// MeasReportQuantity ::= SEQUENCE
SRSASN_CODE meas_report_quant_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp, 1));
  HANDLE_CODE(bref.pack(rsrq, 1));
  HANDLE_CODE(bref.pack(sinr, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_quant_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp, 1));
  HANDLE_CODE(bref.unpack(rsrq, 1));
  HANDLE_CODE(bref.unpack(sinr, 1));

  return SRSASN_SUCCESS;
}
void meas_report_quant_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("rsrp", rsrp);
  j.write_bool("rsrq", rsrq);
  j.write_bool("sinr", sinr);
  j.end_obj();
}

// MeasReportQuantity-r16 ::= SEQUENCE
SRSASN_CODE meas_report_quant_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cbr_r16, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_quant_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cbr_r16, 1));

  return SRSASN_SUCCESS;
}
void meas_report_quant_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("cbr-r16", cbr_r16);
  j.end_obj();
}

// MeasReportQuantityCLI-r16 ::= ENUMERATED
const char* meas_report_quant_cli_r16_opts::to_string() const
{
  static const char* names[] = {"srs-rsrp", "cli-rssi"};
  return convert_enum_idx(names, 2, value, "meas_report_quant_cli_r16_e");
}

// MeasReportQuantityUTRA-FDD-r16 ::= SEQUENCE
SRSASN_CODE meas_report_quant_utra_fdd_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cpich_rs_cp, 1));
  HANDLE_CODE(bref.pack(cpich_ec_n0, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_quant_utra_fdd_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cpich_rs_cp, 1));
  HANDLE_CODE(bref.unpack(cpich_ec_n0, 1));

  return SRSASN_SUCCESS;
}
void meas_report_quant_utra_fdd_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("cpich-RSCP", cpich_rs_cp);
  j.write_bool("cpich-EcN0", cpich_ec_n0);
  j.end_obj();
}

// MeasTriggerQuantityCLI-r16 ::= CHOICE
void meas_trigger_quant_cli_r16_c::destroy_() {}
void meas_trigger_quant_cli_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_trigger_quant_cli_r16_c::meas_trigger_quant_cli_r16_c(const meas_trigger_quant_cli_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::srs_rsrp_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::cli_rssi_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_cli_r16_c");
  }
}
meas_trigger_quant_cli_r16_c& meas_trigger_quant_cli_r16_c::operator=(const meas_trigger_quant_cli_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::srs_rsrp_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::cli_rssi_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_cli_r16_c");
  }

  return *this;
}
uint8_t& meas_trigger_quant_cli_r16_c::set_srs_rsrp_r16()
{
  set(types::srs_rsrp_r16);
  return c.get<uint8_t>();
}
uint8_t& meas_trigger_quant_cli_r16_c::set_cli_rssi_r16()
{
  set(types::cli_rssi_r16);
  return c.get<uint8_t>();
}
void meas_trigger_quant_cli_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::srs_rsrp_r16:
      j.write_int("srs-RSRP-r16", c.get<uint8_t>());
      break;
    case types::cli_rssi_r16:
      j.write_int("cli-RSSI-r16", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_cli_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_trigger_quant_cli_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::srs_rsrp_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)98u));
      break;
    case types::cli_rssi_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)76u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_cli_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_trigger_quant_cli_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::srs_rsrp_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)98u));
      break;
    case types::cli_rssi_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)76u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_cli_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasTriggerQuantityEUTRA ::= CHOICE
void meas_trigger_quant_eutra_c::destroy_() {}
void meas_trigger_quant_eutra_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_trigger_quant_eutra_c::meas_trigger_quant_eutra_c(const meas_trigger_quant_eutra_c& other)
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
      log_invalid_choice_id(type_, "meas_trigger_quant_eutra_c");
  }
}
meas_trigger_quant_eutra_c& meas_trigger_quant_eutra_c::operator=(const meas_trigger_quant_eutra_c& other)
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
      log_invalid_choice_id(type_, "meas_trigger_quant_eutra_c");
  }

  return *this;
}
uint8_t& meas_trigger_quant_eutra_c::set_rsrp()
{
  set(types::rsrp);
  return c.get<uint8_t>();
}
uint8_t& meas_trigger_quant_eutra_c::set_rsrq()
{
  set(types::rsrq);
  return c.get<uint8_t>();
}
uint8_t& meas_trigger_quant_eutra_c::set_sinr()
{
  set(types::sinr);
  return c.get<uint8_t>();
}
void meas_trigger_quant_eutra_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "meas_trigger_quant_eutra_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_trigger_quant_eutra_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rsrp:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)97u));
      break;
    case types::rsrq:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)34u));
      break;
    case types::sinr:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_eutra_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_trigger_quant_eutra_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rsrp:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)97u));
      break;
    case types::rsrq:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)34u));
      break;
    case types::sinr:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_eutra_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// MeasTriggerQuantityUTRA-FDD-r16 ::= CHOICE
void meas_trigger_quant_utra_fdd_r16_c::destroy_() {}
void meas_trigger_quant_utra_fdd_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_trigger_quant_utra_fdd_r16_c::meas_trigger_quant_utra_fdd_r16_c(const meas_trigger_quant_utra_fdd_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::utra_fdd_rs_cp_r16:
      c.init(other.c.get<int8_t>());
      break;
    case types::utra_fdd_ec_n0_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_utra_fdd_r16_c");
  }
}
meas_trigger_quant_utra_fdd_r16_c&
meas_trigger_quant_utra_fdd_r16_c::operator=(const meas_trigger_quant_utra_fdd_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::utra_fdd_rs_cp_r16:
      c.set(other.c.get<int8_t>());
      break;
    case types::utra_fdd_ec_n0_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_utra_fdd_r16_c");
  }

  return *this;
}
int8_t& meas_trigger_quant_utra_fdd_r16_c::set_utra_fdd_rs_cp_r16()
{
  set(types::utra_fdd_rs_cp_r16);
  return c.get<int8_t>();
}
uint8_t& meas_trigger_quant_utra_fdd_r16_c::set_utra_fdd_ec_n0_r16()
{
  set(types::utra_fdd_ec_n0_r16);
  return c.get<uint8_t>();
}
void meas_trigger_quant_utra_fdd_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::utra_fdd_rs_cp_r16:
      j.write_int("utra-FDD-RSCP-r16", c.get<int8_t>());
      break;
    case types::utra_fdd_ec_n0_r16:
      j.write_int("utra-FDD-EcN0-r16", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_utra_fdd_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_trigger_quant_utra_fdd_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::utra_fdd_rs_cp_r16:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-5, (int8_t)91));
      break;
    case types::utra_fdd_ec_n0_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)49u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_utra_fdd_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_trigger_quant_utra_fdd_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::utra_fdd_rs_cp_r16:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-5, (int8_t)91));
      break;
    case types::utra_fdd_ec_n0_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)49u));
      break;
    default:
      log_invalid_choice_id(type_, "meas_trigger_quant_utra_fdd_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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

// RxTxReportInterval-r17 ::= ENUMERATED
const char* rx_tx_report_interv_r17_opts::to_string() const
{
  static const char* names[] = {"ms80",
                                "ms120",
                                "ms160",
                                "ms240",
                                "ms320",
                                "ms480",
                                "ms640",
                                "ms1024",
                                "ms1280",
                                "ms2048",
                                "ms2560",
                                "ms5120",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "rx_tx_report_interv_r17_e");
}
uint16_t rx_tx_report_interv_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {80, 120, 160, 240, 320, 480, 640, 1024, 1280, 2048, 2560, 5120};
  return map_enum_number(numbers, 12, value, "rx_tx_report_interv_r17_e");
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

// SSB-ConfigMobility ::= SEQUENCE
SRSASN_CODE ssb_cfg_mob_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ssb_to_measure_present, 1));
  HANDLE_CODE(bref.pack(ss_rssi_meas_present, 1));

  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.pack(bref));
  }
  HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell, 1));
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ssb_position_qcl_common_r16_present;
    group_flags[0] |= ssb_position_qcl_cells_to_add_mod_list_r16.is_present();
    group_flags[0] |= ssb_position_qcl_cells_to_rem_list_r16.is_present();
    group_flags[1] |= derive_ssb_idx_from_cell_inter_r17_present;
    group_flags[1] |= ssb_position_qcl_common_r17_present;
    group_flags[1] |= ssb_position_qcl_cells_r17.is_present();
    group_flags[2] |= cca_cells_to_add_mod_list_r17.is_present();
    group_flags[2] |= cca_cells_to_rem_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ssb_position_qcl_common_r16_present, 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_cells_to_add_mod_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_cells_to_rem_list_r16.is_present(), 1));
      if (ssb_position_qcl_common_r16_present) {
        HANDLE_CODE(ssb_position_qcl_common_r16.pack(bref));
      }
      if (ssb_position_qcl_cells_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ssb_position_qcl_cells_to_add_mod_list_r16, 1, 32));
      }
      if (ssb_position_qcl_cells_to_rem_list_r16.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *ssb_position_qcl_cells_to_rem_list_r16, 1, 32, integer_packer<uint16_t>(0, 1007)));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell_inter_r17_present, 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_common_r17_present, 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_cells_r17.is_present(), 1));
      if (derive_ssb_idx_from_cell_inter_r17_present) {
        HANDLE_CODE(pack_integer(bref, derive_ssb_idx_from_cell_inter_r17, (uint8_t)0u, (uint8_t)31u));
      }
      if (ssb_position_qcl_common_r17_present) {
        HANDLE_CODE(ssb_position_qcl_common_r17.pack(bref));
      }
      if (ssb_position_qcl_cells_r17.is_present()) {
        HANDLE_CODE(ssb_position_qcl_cells_r17->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cca_cells_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(cca_cells_to_rem_list_r17.is_present(), 1));
      if (cca_cells_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cca_cells_to_add_mod_list_r17, 1, 32, integer_packer<uint16_t>(0, 1007)));
      }
      if (cca_cells_to_rem_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cca_cells_to_rem_list_r17, 1, 32, integer_packer<uint16_t>(0, 1007)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_cfg_mob_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ssb_to_measure_present, 1));
  HANDLE_CODE(bref.unpack(ss_rssi_meas_present, 1));

  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell, 1));
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r16_present, 1));
      unpack_presence_flag(ssb_position_qcl_cells_to_add_mod_list_r16, bref);
      unpack_presence_flag(ssb_position_qcl_cells_to_rem_list_r16, bref);
      if (ssb_position_qcl_common_r16_present) {
        HANDLE_CODE(ssb_position_qcl_common_r16.unpack(bref));
      }
      if (ssb_position_qcl_cells_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ssb_position_qcl_cells_to_add_mod_list_r16, bref, 1, 32));
      }
      if (ssb_position_qcl_cells_to_rem_list_r16.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*ssb_position_qcl_cells_to_rem_list_r16, bref, 1, 32, integer_packer<uint16_t>(0, 1007)));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell_inter_r17_present, 1));
      HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r17_present, 1));
      unpack_presence_flag(ssb_position_qcl_cells_r17, bref);
      if (derive_ssb_idx_from_cell_inter_r17_present) {
        HANDLE_CODE(unpack_integer(derive_ssb_idx_from_cell_inter_r17, bref, (uint8_t)0u, (uint8_t)31u));
      }
      if (ssb_position_qcl_common_r17_present) {
        HANDLE_CODE(ssb_position_qcl_common_r17.unpack(bref));
      }
      if (ssb_position_qcl_cells_r17.is_present()) {
        HANDLE_CODE(ssb_position_qcl_cells_r17->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(cca_cells_to_add_mod_list_r17, bref);
      unpack_presence_flag(cca_cells_to_rem_list_r17, bref);
      if (cca_cells_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cca_cells_to_add_mod_list_r17, bref, 1, 32, integer_packer<uint16_t>(0, 1007)));
      }
      if (cca_cells_to_rem_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cca_cells_to_rem_list_r17, bref, 1, 32, integer_packer<uint16_t>(0, 1007)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ssb_cfg_mob_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_to_measure_present) {
    j.write_fieldname("ssb-ToMeasure");
    ssb_to_measure.to_json(j);
  }
  j.write_bool("deriveSSB-IndexFromCell", derive_ssb_idx_from_cell);
  if (ss_rssi_meas_present) {
    j.write_fieldname("ss-RSSI-Measurement");
    ss_rssi_meas.to_json(j);
  }
  if (ext) {
    if (ssb_position_qcl_common_r16_present) {
      j.write_str("ssb-PositionQCL-Common-r16", ssb_position_qcl_common_r16.to_string());
    }
    if (ssb_position_qcl_cells_to_add_mod_list_r16.is_present()) {
      j.start_array("ssb-PositionQCL-CellsToAddModList-r16");
      for (const auto& e1 : *ssb_position_qcl_cells_to_add_mod_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (ssb_position_qcl_cells_to_rem_list_r16.is_present()) {
      j.start_array("ssb-PositionQCL-CellsToRemoveList-r16");
      for (const auto& e1 : *ssb_position_qcl_cells_to_rem_list_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (derive_ssb_idx_from_cell_inter_r17_present) {
      j.write_int("deriveSSB-IndexFromCellInter-r17", derive_ssb_idx_from_cell_inter_r17);
    }
    if (ssb_position_qcl_common_r17_present) {
      j.write_str("ssb-PositionQCL-Common-r17", ssb_position_qcl_common_r17.to_string());
    }
    if (ssb_position_qcl_cells_r17.is_present()) {
      j.write_fieldname("ssb-PositionQCL-Cells-r17");
      ssb_position_qcl_cells_r17->to_json(j);
    }
    if (cca_cells_to_add_mod_list_r17.is_present()) {
      j.start_array("cca-CellsToAddModList-r17");
      for (const auto& e1 : *cca_cells_to_add_mod_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (cca_cells_to_rem_list_r17.is_present()) {
      j.start_array("cca-CellsToRemoveList-r17");
      for (const auto& e1 : *cca_cells_to_rem_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SSB-MTC3-r16 ::= SEQUENCE
SRSASN_CODE ssb_mtc3_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pci_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(ssb_to_measure_r16_present, 1));

  HANDLE_CODE(periodicity_and_offset_r16.pack(bref));
  HANDLE_CODE(dur_r16.pack(bref));
  if (pci_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_list_r16, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  if (ssb_to_measure_r16_present) {
    HANDLE_CODE(ssb_to_measure_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc3_r16_s::unpack(cbit_ref& bref)
{
  bool pci_list_r16_present;
  HANDLE_CODE(bref.unpack(pci_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(ssb_to_measure_r16_present, 1));

  HANDLE_CODE(periodicity_and_offset_r16.unpack(bref));
  HANDLE_CODE(dur_r16.unpack(bref));
  if (pci_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_list_r16, bref, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  if (ssb_to_measure_r16_present) {
    HANDLE_CODE(ssb_to_measure_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ssb_mtc3_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityAndOffset-r16");
  periodicity_and_offset_r16.to_json(j);
  j.write_str("duration-r16", dur_r16.to_string());
  if (pci_list_r16.size() > 0) {
    j.start_array("pci-List-r16");
    for (const auto& e1 : pci_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (ssb_to_measure_r16_present) {
    j.write_fieldname("ssb-ToMeasure-r16");
    ssb_to_measure_r16.to_json(j);
  }
  j.end_obj();
}

void ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::destroy_() {}
void ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::periodicity_and_offset_r16_c_(
    const ssb_mtc3_r16_s::periodicity_and_offset_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf5_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf10_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf320_r16:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf640_r16:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1280_r16:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_");
  }
}
ssb_mtc3_r16_s::periodicity_and_offset_r16_c_&
ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::operator=(const ssb_mtc3_r16_s::periodicity_and_offset_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf5_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf10_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf320_r16:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf640_r16:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1280_r16:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_");
  }

  return *this;
}
uint8_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf5_r16()
{
  set(types::sf5_r16);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf10_r16()
{
  set(types::sf10_r16);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf20_r16()
{
  set(types::sf20_r16);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf40_r16()
{
  set(types::sf40_r16);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf80_r16()
{
  set(types::sf80_r16);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf160_r16()
{
  set(types::sf160_r16);
  return c.get<uint8_t>();
}
uint16_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf320_r16()
{
  set(types::sf320_r16);
  return c.get<uint16_t>();
}
uint16_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf640_r16()
{
  set(types::sf640_r16);
  return c.get<uint16_t>();
}
uint16_t& ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::set_sf1280_r16()
{
  set(types::sf1280_r16);
  return c.get<uint16_t>();
}
void ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf5_r16:
      j.write_int("sf5-r16", c.get<uint8_t>());
      break;
    case types::sf10_r16:
      j.write_int("sf10-r16", c.get<uint8_t>());
      break;
    case types::sf20_r16:
      j.write_int("sf20-r16", c.get<uint8_t>());
      break;
    case types::sf40_r16:
      j.write_int("sf40-r16", c.get<uint8_t>());
      break;
    case types::sf80_r16:
      j.write_int("sf80-r16", c.get<uint8_t>());
      break;
    case types::sf160_r16:
      j.write_int("sf160-r16", c.get<uint8_t>());
      break;
    case types::sf320_r16:
      j.write_int("sf320-r16", c.get<uint16_t>());
      break;
    case types::sf640_r16:
      j.write_int("sf640-r16", c.get<uint16_t>());
      break;
    case types::sf1280_r16:
      j.write_int("sf1280-r16", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf5_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sf10_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf40_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf80_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf160_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf320_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf640_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1280_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf5_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sf10_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf40_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf80_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf160_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf320_r16:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf640_r16:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1280_r16:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ssb_mtc3_r16_s::dur_r16_opts::to_string() const
{
  static const char* names[] = {"sf1", "sf2", "sf3", "sf4", "sf5"};
  return convert_enum_idx(names, 5, value, "ssb_mtc3_r16_s::dur_r16_e_");
}
uint8_t ssb_mtc3_r16_s::dur_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5};
  return map_enum_number(numbers, 5, value, "ssb_mtc3_r16_s::dur_r16_e_");
}

// Sensor-NameList-r16 ::= SEQUENCE
SRSASN_CODE sensor_name_list_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_uncom_bar_pre_r16_present, 1));
  HANDLE_CODE(bref.pack(meas_ue_speed_present, 1));
  HANDLE_CODE(bref.pack(meas_ue_orientation_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sensor_name_list_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_uncom_bar_pre_r16_present, 1));
  HANDLE_CODE(bref.unpack(meas_ue_speed_present, 1));
  HANDLE_CODE(bref.unpack(meas_ue_orientation_present, 1));

  return SRSASN_SUCCESS;
}
void sensor_name_list_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_uncom_bar_pre_r16_present) {
    j.write_str("measUncomBarPre-r16", "true");
  }
  if (meas_ue_speed_present) {
    j.write_str("measUeSpeed", "true");
  }
  if (meas_ue_orientation_present) {
    j.write_str("measUeOrientation", "true");
  }
  j.end_obj();
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

// UL-DelayValueConfig-r16 ::= SEQUENCE
SRSASN_CODE ul_delay_value_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, delay_drb_list_r16, 1, 29, integer_packer<uint8_t>(1, 32)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_delay_value_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(delay_drb_list_r16, bref, 1, 29, integer_packer<uint8_t>(1, 32)));

  return SRSASN_SUCCESS;
}
void ul_delay_value_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("delay-DRBlist-r16");
  for (const auto& e1 : delay_drb_list_r16) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// UL-ExcessDelayConfig-r17 ::= SEQUENCE
SRSASN_CODE ul_excess_delay_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, excess_delay_drb_list_r17, 1, 29));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_excess_delay_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(excess_delay_drb_list_r17, bref, 1, 29));

  return SRSASN_SUCCESS;
}
void ul_excess_delay_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("excessDelay-DRBlist-r17");
  for (const auto& e1 : excess_delay_drb_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
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

// CLI-EventTriggerConfig-r16 ::= SEQUENCE
SRSASN_CODE cli_event_trigger_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(event_id_r16.pack(bref));
  HANDLE_CODE(report_interv_r16.pack(bref));
  HANDLE_CODE(report_amount_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cli_r16, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cli_event_trigger_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(event_id_r16.unpack(bref));
  HANDLE_CODE(report_interv_r16.unpack(bref));
  HANDLE_CODE(report_amount_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cli_r16, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void cli_event_trigger_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventId-r16");
  event_id_r16.to_json(j);
  j.write_str("reportInterval-r16", report_interv_r16.to_string());
  j.write_str("reportAmount-r16", report_amount_r16.to_string());
  j.write_int("maxReportCLI-r16", max_report_cli_r16);
  j.end_obj();
}

void cli_event_trigger_cfg_r16_s::event_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventI1-r16");
  j.start_obj();
  j.write_fieldname("i1-Threshold-r16");
  c.i1_thres_r16.to_json(j);
  j.write_bool("reportOnLeave-r16", c.report_on_leave_r16);
  j.write_int("hysteresis-r16", c.hysteresis_r16);
  j.write_str("timeToTrigger-r16", c.time_to_trigger_r16.to_string());
  j.end_obj();
  j.end_obj();
}
SRSASN_CODE cli_event_trigger_cfg_r16_s::event_id_r16_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.i1_thres_r16.pack(bref));
  HANDLE_CODE(bref.pack(c.report_on_leave_r16, 1));
  HANDLE_CODE(pack_integer(bref, c.hysteresis_r16, (uint8_t)0u, (uint8_t)30u));
  HANDLE_CODE(c.time_to_trigger_r16.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cli_event_trigger_cfg_r16_s::event_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "cli_event_trigger_cfg_r16_s::event_id_r16_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.i1_thres_r16.unpack(bref));
  HANDLE_CODE(bref.unpack(c.report_on_leave_r16, 1));
  HANDLE_CODE(unpack_integer(c.hysteresis_r16, bref, (uint8_t)0u, (uint8_t)30u));
  HANDLE_CODE(c.time_to_trigger_r16.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* cli_event_trigger_cfg_r16_s::report_amount_r16_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "cli_event_trigger_cfg_r16_s::report_amount_r16_e_");
}
int8_t cli_event_trigger_cfg_r16_s::report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "cli_event_trigger_cfg_r16_s::report_amount_r16_e_");
}

// CLI-PeriodicalReportConfig-r16 ::= SEQUENCE
SRSASN_CODE cli_periodical_report_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(report_interv_r16.pack(bref));
  HANDLE_CODE(report_amount_r16.pack(bref));
  HANDLE_CODE(report_quant_cli_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cli_r16, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cli_periodical_report_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(report_interv_r16.unpack(bref));
  HANDLE_CODE(report_amount_r16.unpack(bref));
  HANDLE_CODE(report_quant_cli_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cli_r16, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void cli_periodical_report_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reportInterval-r16", report_interv_r16.to_string());
  j.write_str("reportAmount-r16", report_amount_r16.to_string());
  j.write_str("reportQuantityCLI-r16", report_quant_cli_r16.to_string());
  j.write_int("maxReportCLI-r16", max_report_cli_r16);
  j.end_obj();
}

const char* cli_periodical_report_cfg_r16_s::report_amount_r16_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "cli_periodical_report_cfg_r16_s::report_amount_r16_e_");
}
int8_t cli_periodical_report_cfg_r16_s::report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "cli_periodical_report_cfg_r16_s::report_amount_r16_e_");
}

// CLI-ResourceConfig-r16 ::= SEQUENCE
SRSASN_CODE cli_res_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_res_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(rssi_res_cfg_r16_present, 1));

  if (srs_res_cfg_r16_present) {
    HANDLE_CODE(srs_res_cfg_r16.pack(bref));
  }
  if (rssi_res_cfg_r16_present) {
    HANDLE_CODE(rssi_res_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cli_res_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_res_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(rssi_res_cfg_r16_present, 1));

  if (srs_res_cfg_r16_present) {
    HANDLE_CODE(srs_res_cfg_r16.unpack(bref));
  }
  if (rssi_res_cfg_r16_present) {
    HANDLE_CODE(rssi_res_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cli_res_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_res_cfg_r16_present) {
    j.write_fieldname("srs-ResourceConfig-r16");
    srs_res_cfg_r16.to_json(j);
  }
  if (rssi_res_cfg_r16_present) {
    j.write_fieldname("rssi-ResourceConfig-r16");
    rssi_res_cfg_r16.to_json(j);
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

// EUTRA-Cell ::= SEQUENCE
SRSASN_CODE eutra_cell_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx_eutra, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(cell_individual_offset.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_cell_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx_eutra, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(cell_individual_offset.unpack(bref));

  return SRSASN_SUCCESS;
}
void eutra_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndexEUTRA", cell_idx_eutra);
  j.write_int("physCellId", pci);
  j.write_str("cellIndividualOffset", cell_individual_offset.to_string());
  j.end_obj();
}

// EUTRA-ExcludedCell ::= SEQUENCE
SRSASN_CODE eutra_excluded_cell_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx_eutra, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_excluded_cell_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx_eutra, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range.unpack(bref));

  return SRSASN_SUCCESS;
}
void eutra_excluded_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndexEUTRA", cell_idx_eutra);
  j.write_fieldname("physCellIdRange");
  pci_range.to_json(j);
  j.end_obj();
}

// EventTriggerConfig ::= SEQUENCE
SRSASN_CODE event_trigger_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(report_quant_rs_idxes_present, 1));
  HANDLE_CODE(bref.pack(max_nrof_rs_idxes_to_report_present, 1));
  HANDLE_CODE(bref.pack(report_add_neigh_meas_present, 1));

  HANDLE_CODE(event_id.pack(bref));
  HANDLE_CODE(rs_type.pack(bref));
  HANDLE_CODE(report_interv.pack(bref));
  HANDLE_CODE(report_amount.pack(bref));
  HANDLE_CODE(report_quant_cell.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cells, (uint8_t)1u, (uint8_t)8u));
  if (report_quant_rs_idxes_present) {
    HANDLE_CODE(report_quant_rs_idxes.pack(bref));
  }
  if (max_nrof_rs_idxes_to_report_present) {
    HANDLE_CODE(pack_integer(bref, max_nrof_rs_idxes_to_report, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(bref.pack(include_beam_meass, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_rssi_report_cfg_r16.is_present();
    group_flags[0] |= use_t312_r16_present;
    group_flags[0] |= include_common_location_info_r16_present;
    group_flags[0] |= include_bt_meas_r16.is_present();
    group_flags[0] |= include_wlan_meas_r16.is_present();
    group_flags[0] |= include_sensor_meas_r16.is_present();
    group_flags[1] |= coarse_location_request_r17_present;
    group_flags[1] |= report_quant_relay_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_rssi_report_cfg_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(use_t312_r16_present, 1));
      HANDLE_CODE(bref.pack(include_common_location_info_r16_present, 1));
      HANDLE_CODE(bref.pack(include_bt_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_wlan_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_sensor_meas_r16.is_present(), 1));
      if (meas_rssi_report_cfg_r16.is_present()) {
        HANDLE_CODE(meas_rssi_report_cfg_r16->pack(bref));
      }
      if (use_t312_r16_present) {
        HANDLE_CODE(bref.pack(use_t312_r16, 1));
      }
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->pack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->pack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(coarse_location_request_r17_present, 1));
      HANDLE_CODE(bref.pack(report_quant_relay_r17.is_present(), 1));
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(report_quant_rs_idxes_present, 1));
  HANDLE_CODE(bref.unpack(max_nrof_rs_idxes_to_report_present, 1));
  HANDLE_CODE(bref.unpack(report_add_neigh_meas_present, 1));

  HANDLE_CODE(event_id.unpack(bref));
  HANDLE_CODE(rs_type.unpack(bref));
  HANDLE_CODE(report_interv.unpack(bref));
  HANDLE_CODE(report_amount.unpack(bref));
  HANDLE_CODE(report_quant_cell.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cells, bref, (uint8_t)1u, (uint8_t)8u));
  if (report_quant_rs_idxes_present) {
    HANDLE_CODE(report_quant_rs_idxes.unpack(bref));
  }
  if (max_nrof_rs_idxes_to_report_present) {
    HANDLE_CODE(unpack_integer(max_nrof_rs_idxes_to_report, bref, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(bref.unpack(include_beam_meass, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_rssi_report_cfg_r16, bref);
      HANDLE_CODE(bref.unpack(use_t312_r16_present, 1));
      HANDLE_CODE(bref.unpack(include_common_location_info_r16_present, 1));
      unpack_presence_flag(include_bt_meas_r16, bref);
      unpack_presence_flag(include_wlan_meas_r16, bref);
      unpack_presence_flag(include_sensor_meas_r16, bref);
      if (meas_rssi_report_cfg_r16.is_present()) {
        HANDLE_CODE(meas_rssi_report_cfg_r16->unpack(bref));
      }
      if (use_t312_r16_present) {
        HANDLE_CODE(bref.unpack(use_t312_r16, 1));
      }
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->unpack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->unpack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(coarse_location_request_r17_present, 1));
      unpack_presence_flag(report_quant_relay_r17, bref);
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void event_trigger_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventId");
  event_id.to_json(j);
  j.write_str("rsType", rs_type.to_string());
  j.write_str("reportInterval", report_interv.to_string());
  j.write_str("reportAmount", report_amount.to_string());
  j.write_fieldname("reportQuantityCell");
  report_quant_cell.to_json(j);
  j.write_int("maxReportCells", max_report_cells);
  if (report_quant_rs_idxes_present) {
    j.write_fieldname("reportQuantityRS-Indexes");
    report_quant_rs_idxes.to_json(j);
  }
  if (max_nrof_rs_idxes_to_report_present) {
    j.write_int("maxNrofRS-IndexesToReport", max_nrof_rs_idxes_to_report);
  }
  j.write_bool("includeBeamMeasurements", include_beam_meass);
  if (report_add_neigh_meas_present) {
    j.write_str("reportAddNeighMeas", "setup");
  }
  if (ext) {
    if (meas_rssi_report_cfg_r16.is_present()) {
      j.write_fieldname("measRSSI-ReportConfig-r16");
      meas_rssi_report_cfg_r16->to_json(j);
    }
    if (use_t312_r16_present) {
      j.write_bool("useT312-r16", use_t312_r16);
    }
    if (include_common_location_info_r16_present) {
      j.write_str("includeCommonLocationInfo-r16", "true");
    }
    if (include_bt_meas_r16.is_present()) {
      j.write_fieldname("includeBT-Meas-r16");
      include_bt_meas_r16->to_json(j);
    }
    if (include_wlan_meas_r16.is_present()) {
      j.write_fieldname("includeWLAN-Meas-r16");
      include_wlan_meas_r16->to_json(j);
    }
    if (include_sensor_meas_r16.is_present()) {
      j.write_fieldname("includeSensor-Meas-r16");
      include_sensor_meas_r16->to_json(j);
    }
    if (coarse_location_request_r17_present) {
      j.write_str("coarseLocationRequest-r17", "true");
    }
    if (report_quant_relay_r17.is_present()) {
      j.write_fieldname("reportQuantityRelay-r17");
      report_quant_relay_r17->to_json(j);
    }
  }
  j.end_obj();
}

void event_trigger_cfg_s::event_id_c_::destroy_()
{
  switch (type_) {
    case types::event_a1:
      c.destroy<event_a1_s_>();
      break;
    case types::event_a2:
      c.destroy<event_a2_s_>();
      break;
    case types::event_a3:
      c.destroy<event_a3_s_>();
      break;
    case types::event_a4:
      c.destroy<event_a4_s_>();
      break;
    case types::event_a5:
      c.destroy<event_a5_s_>();
      break;
    case types::event_a6:
      c.destroy<event_a6_s_>();
      break;
    case types::event_x1_r17:
      c.destroy<event_x1_r17_s_>();
      break;
    case types::event_x2_r17:
      c.destroy<event_x2_r17_s_>();
      break;
    case types::event_d1_r17:
      c.destroy<event_d1_r17_s_>();
      break;
    default:
      break;
  }
}
void event_trigger_cfg_s::event_id_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event_a1:
      c.init<event_a1_s_>();
      break;
    case types::event_a2:
      c.init<event_a2_s_>();
      break;
    case types::event_a3:
      c.init<event_a3_s_>();
      break;
    case types::event_a4:
      c.init<event_a4_s_>();
      break;
    case types::event_a5:
      c.init<event_a5_s_>();
      break;
    case types::event_a6:
      c.init<event_a6_s_>();
      break;
    case types::event_x1_r17:
      c.init<event_x1_r17_s_>();
      break;
    case types::event_x2_r17:
      c.init<event_x2_r17_s_>();
      break;
    case types::event_d1_r17:
      c.init<event_d1_r17_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_s::event_id_c_");
  }
}
event_trigger_cfg_s::event_id_c_::event_id_c_(const event_trigger_cfg_s::event_id_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event_a1:
      c.init(other.c.get<event_a1_s_>());
      break;
    case types::event_a2:
      c.init(other.c.get<event_a2_s_>());
      break;
    case types::event_a3:
      c.init(other.c.get<event_a3_s_>());
      break;
    case types::event_a4:
      c.init(other.c.get<event_a4_s_>());
      break;
    case types::event_a5:
      c.init(other.c.get<event_a5_s_>());
      break;
    case types::event_a6:
      c.init(other.c.get<event_a6_s_>());
      break;
    case types::event_x1_r17:
      c.init(other.c.get<event_x1_r17_s_>());
      break;
    case types::event_x2_r17:
      c.init(other.c.get<event_x2_r17_s_>());
      break;
    case types::event_d1_r17:
      c.init(other.c.get<event_d1_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_s::event_id_c_");
  }
}
event_trigger_cfg_s::event_id_c_&
event_trigger_cfg_s::event_id_c_::operator=(const event_trigger_cfg_s::event_id_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event_a1:
      c.set(other.c.get<event_a1_s_>());
      break;
    case types::event_a2:
      c.set(other.c.get<event_a2_s_>());
      break;
    case types::event_a3:
      c.set(other.c.get<event_a3_s_>());
      break;
    case types::event_a4:
      c.set(other.c.get<event_a4_s_>());
      break;
    case types::event_a5:
      c.set(other.c.get<event_a5_s_>());
      break;
    case types::event_a6:
      c.set(other.c.get<event_a6_s_>());
      break;
    case types::event_x1_r17:
      c.set(other.c.get<event_x1_r17_s_>());
      break;
    case types::event_x2_r17:
      c.set(other.c.get<event_x2_r17_s_>());
      break;
    case types::event_d1_r17:
      c.set(other.c.get<event_d1_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_s::event_id_c_");
  }

  return *this;
}
event_trigger_cfg_s::event_id_c_::event_a1_s_& event_trigger_cfg_s::event_id_c_::set_event_a1()
{
  set(types::event_a1);
  return c.get<event_a1_s_>();
}
event_trigger_cfg_s::event_id_c_::event_a2_s_& event_trigger_cfg_s::event_id_c_::set_event_a2()
{
  set(types::event_a2);
  return c.get<event_a2_s_>();
}
event_trigger_cfg_s::event_id_c_::event_a3_s_& event_trigger_cfg_s::event_id_c_::set_event_a3()
{
  set(types::event_a3);
  return c.get<event_a3_s_>();
}
event_trigger_cfg_s::event_id_c_::event_a4_s_& event_trigger_cfg_s::event_id_c_::set_event_a4()
{
  set(types::event_a4);
  return c.get<event_a4_s_>();
}
event_trigger_cfg_s::event_id_c_::event_a5_s_& event_trigger_cfg_s::event_id_c_::set_event_a5()
{
  set(types::event_a5);
  return c.get<event_a5_s_>();
}
event_trigger_cfg_s::event_id_c_::event_a6_s_& event_trigger_cfg_s::event_id_c_::set_event_a6()
{
  set(types::event_a6);
  return c.get<event_a6_s_>();
}
event_trigger_cfg_s::event_id_c_::event_x1_r17_s_& event_trigger_cfg_s::event_id_c_::set_event_x1_r17()
{
  set(types::event_x1_r17);
  return c.get<event_x1_r17_s_>();
}
event_trigger_cfg_s::event_id_c_::event_x2_r17_s_& event_trigger_cfg_s::event_id_c_::set_event_x2_r17()
{
  set(types::event_x2_r17);
  return c.get<event_x2_r17_s_>();
}
event_trigger_cfg_s::event_id_c_::event_d1_r17_s_& event_trigger_cfg_s::event_id_c_::set_event_d1_r17()
{
  set(types::event_d1_r17);
  return c.get<event_d1_r17_s_>();
}
void event_trigger_cfg_s::event_id_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event_a1:
      j.write_fieldname("eventA1");
      j.start_obj();
      j.write_fieldname("a1-Threshold");
      c.get<event_a1_s_>().a1_thres.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_a1_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_a1_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_a1_s_>().time_to_trigger.to_string());
      j.end_obj();
      break;
    case types::event_a2:
      j.write_fieldname("eventA2");
      j.start_obj();
      j.write_fieldname("a2-Threshold");
      c.get<event_a2_s_>().a2_thres.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_a2_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_a2_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_a2_s_>().time_to_trigger.to_string());
      j.end_obj();
      break;
    case types::event_a3:
      j.write_fieldname("eventA3");
      j.start_obj();
      j.write_fieldname("a3-Offset");
      c.get<event_a3_s_>().a3_offset.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_a3_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_a3_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_a3_s_>().time_to_trigger.to_string());
      j.write_bool("useAllowedCellList", c.get<event_a3_s_>().use_allowed_cell_list);
      j.end_obj();
      break;
    case types::event_a4:
      j.write_fieldname("eventA4");
      j.start_obj();
      j.write_fieldname("a4-Threshold");
      c.get<event_a4_s_>().a4_thres.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_a4_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_a4_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_a4_s_>().time_to_trigger.to_string());
      j.write_bool("useAllowedCellList", c.get<event_a4_s_>().use_allowed_cell_list);
      j.end_obj();
      break;
    case types::event_a5:
      j.write_fieldname("eventA5");
      j.start_obj();
      j.write_fieldname("a5-Threshold1");
      c.get<event_a5_s_>().a5_thres1.to_json(j);
      j.write_fieldname("a5-Threshold2");
      c.get<event_a5_s_>().a5_thres2.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_a5_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_a5_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_a5_s_>().time_to_trigger.to_string());
      j.write_bool("useAllowedCellList", c.get<event_a5_s_>().use_allowed_cell_list);
      j.end_obj();
      break;
    case types::event_a6:
      j.write_fieldname("eventA6");
      j.start_obj();
      j.write_fieldname("a6-Offset");
      c.get<event_a6_s_>().a6_offset.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_a6_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_a6_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_a6_s_>().time_to_trigger.to_string());
      j.write_bool("useAllowedCellList", c.get<event_a6_s_>().use_allowed_cell_list);
      j.end_obj();
      break;
    case types::event_x1_r17:
      j.write_fieldname("eventX1-r17");
      j.start_obj();
      j.write_fieldname("x1-Threshold1-Relay-r17");
      c.get<event_x1_r17_s_>().x1_thres1_relay_r17.to_json(j);
      j.write_fieldname("x1-Threshold2-r17");
      c.get<event_x1_r17_s_>().x1_thres2_r17.to_json(j);
      j.write_bool("reportOnLeave-r17", c.get<event_x1_r17_s_>().report_on_leave_r17);
      j.write_int("hysteresis-r17", c.get<event_x1_r17_s_>().hysteresis_r17);
      j.write_str("timeToTrigger-r17", c.get<event_x1_r17_s_>().time_to_trigger_r17.to_string());
      j.write_bool("useAllowedCellList-r17", c.get<event_x1_r17_s_>().use_allowed_cell_list_r17);
      j.end_obj();
      break;
    case types::event_x2_r17:
      j.write_fieldname("eventX2-r17");
      j.start_obj();
      j.write_fieldname("x2-Threshold-Relay-r17");
      c.get<event_x2_r17_s_>().x2_thres_relay_r17.to_json(j);
      j.write_bool("reportOnLeave-r17", c.get<event_x2_r17_s_>().report_on_leave_r17);
      j.write_int("hysteresis-r17", c.get<event_x2_r17_s_>().hysteresis_r17);
      j.write_str("timeToTrigger-r17", c.get<event_x2_r17_s_>().time_to_trigger_r17.to_string());
      j.end_obj();
      break;
    case types::event_d1_r17:
      j.write_fieldname("eventD1-r17");
      j.start_obj();
      j.write_int("distanceThreshFromReference1-r17", c.get<event_d1_r17_s_>().distance_thresh_from_ref1_r17);
      j.write_int("distanceThreshFromReference2-r17", c.get<event_d1_r17_s_>().distance_thresh_from_ref2_r17);
      j.write_str("referenceLocation1-r17", c.get<event_d1_r17_s_>().ref_location1_r17.to_string());
      j.write_str("referenceLocation2-r17", c.get<event_d1_r17_s_>().ref_location2_r17.to_string());
      j.write_bool("reportOnLeave-r17", c.get<event_d1_r17_s_>().report_on_leave_r17);
      j.write_int("hysteresisLocation-r17", c.get<event_d1_r17_s_>().hysteresis_location_r17);
      j.write_str("timeToTrigger-r17", c.get<event_d1_r17_s_>().time_to_trigger_r17.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_s::event_id_c_");
  }
  j.end_obj();
}
SRSASN_CODE event_trigger_cfg_s::event_id_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event_a1:
      HANDLE_CODE(c.get<event_a1_s_>().a1_thres.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a1_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a1_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a1_s_>().time_to_trigger.pack(bref));
      break;
    case types::event_a2:
      HANDLE_CODE(c.get<event_a2_s_>().a2_thres.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a2_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a2_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a2_s_>().time_to_trigger.pack(bref));
      break;
    case types::event_a3:
      HANDLE_CODE(c.get<event_a3_s_>().a3_offset.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a3_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a3_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a3_s_>().time_to_trigger.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a3_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_a4:
      HANDLE_CODE(c.get<event_a4_s_>().a4_thres.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a4_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a4_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a4_s_>().time_to_trigger.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a4_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_a5:
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres1.pack(bref));
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres2.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a5_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a5_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a5_s_>().time_to_trigger.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a5_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_a6:
      HANDLE_CODE(c.get<event_a6_s_>().a6_offset.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a6_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a6_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a6_s_>().time_to_trigger.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a6_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_x1_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_x1_r17_s_>().x1_thres1_relay_r17.pack(bref));
      HANDLE_CODE(c.get<event_x1_r17_s_>().x1_thres2_r17.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_x1_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_x1_r17_s_>().hysteresis_r17, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_x1_r17_s_>().time_to_trigger_r17.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_x1_r17_s_>().use_allowed_cell_list_r17, 1));
    } break;
    case types::event_x2_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_x2_r17_s_>().x2_thres_relay_r17.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_x2_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_x2_r17_s_>().hysteresis_r17, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_x2_r17_s_>().time_to_trigger_r17.pack(bref));
    } break;
    case types::event_d1_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(
          pack_integer(bref, c.get<event_d1_r17_s_>().distance_thresh_from_ref1_r17, (uint16_t)1u, (uint16_t)65525u));
      HANDLE_CODE(
          pack_integer(bref, c.get<event_d1_r17_s_>().distance_thresh_from_ref2_r17, (uint16_t)1u, (uint16_t)65525u));
      HANDLE_CODE(c.get<event_d1_r17_s_>().ref_location1_r17.pack(bref));
      HANDLE_CODE(c.get<event_d1_r17_s_>().ref_location2_r17.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_d1_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_d1_r17_s_>().hysteresis_location_r17, (uint16_t)0u, (uint16_t)32768u));
      HANDLE_CODE(c.get<event_d1_r17_s_>().time_to_trigger_r17.pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_s::event_id_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_cfg_s::event_id_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event_a1:
      HANDLE_CODE(c.get<event_a1_s_>().a1_thres.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a1_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a1_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a1_s_>().time_to_trigger.unpack(bref));
      break;
    case types::event_a2:
      HANDLE_CODE(c.get<event_a2_s_>().a2_thres.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a2_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a2_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a2_s_>().time_to_trigger.unpack(bref));
      break;
    case types::event_a3:
      HANDLE_CODE(c.get<event_a3_s_>().a3_offset.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a3_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a3_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a3_s_>().time_to_trigger.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a3_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_a4:
      HANDLE_CODE(c.get<event_a4_s_>().a4_thres.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a4_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a4_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a4_s_>().time_to_trigger.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a4_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_a5:
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres1.unpack(bref));
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres2.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a5_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a5_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a5_s_>().time_to_trigger.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a5_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_a6:
      HANDLE_CODE(c.get<event_a6_s_>().a6_offset.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a6_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a6_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a6_s_>().time_to_trigger.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a6_s_>().use_allowed_cell_list, 1));
      break;
    case types::event_x1_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_x1_r17_s_>().x1_thres1_relay_r17.unpack(bref));
      HANDLE_CODE(c.get<event_x1_r17_s_>().x1_thres2_r17.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_x1_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(unpack_integer(c.get<event_x1_r17_s_>().hysteresis_r17, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_x1_r17_s_>().time_to_trigger_r17.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_x1_r17_s_>().use_allowed_cell_list_r17, 1));
    } break;
    case types::event_x2_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<event_x2_r17_s_>().x2_thres_relay_r17.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_x2_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(unpack_integer(c.get<event_x2_r17_s_>().hysteresis_r17, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_x2_r17_s_>().time_to_trigger_r17.unpack(bref));
    } break;
    case types::event_d1_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(
          unpack_integer(c.get<event_d1_r17_s_>().distance_thresh_from_ref1_r17, bref, (uint16_t)1u, (uint16_t)65525u));
      HANDLE_CODE(
          unpack_integer(c.get<event_d1_r17_s_>().distance_thresh_from_ref2_r17, bref, (uint16_t)1u, (uint16_t)65525u));
      HANDLE_CODE(c.get<event_d1_r17_s_>().ref_location1_r17.unpack(bref));
      HANDLE_CODE(c.get<event_d1_r17_s_>().ref_location2_r17.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_d1_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(
          unpack_integer(c.get<event_d1_r17_s_>().hysteresis_location_r17, bref, (uint16_t)0u, (uint16_t)32768u));
      HANDLE_CODE(c.get<event_d1_r17_s_>().time_to_trigger_r17.unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_s::event_id_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* event_trigger_cfg_s::report_amount_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "event_trigger_cfg_s::report_amount_e_");
}
int8_t event_trigger_cfg_s::report_amount_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "event_trigger_cfg_s::report_amount_e_");
}

// EventTriggerConfigInterRAT ::= SEQUENCE
SRSASN_CODE event_trigger_cfg_inter_rat_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(event_id.pack(bref));
  HANDLE_CODE(rs_type.pack(bref));
  HANDLE_CODE(report_interv.pack(bref));
  HANDLE_CODE(report_amount.pack(bref));
  HANDLE_CODE(report_quant.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cells, (uint8_t)1u, (uint8_t)8u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= report_quant_utra_fdd_r16.is_present();
    group_flags[1] |= include_common_location_info_r16_present;
    group_flags[1] |= include_bt_meas_r16.is_present();
    group_flags[1] |= include_wlan_meas_r16.is_present();
    group_flags[1] |= include_sensor_meas_r16.is_present();
    group_flags[2] |= report_quant_relay_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_quant_utra_fdd_r16.is_present(), 1));
      if (report_quant_utra_fdd_r16.is_present()) {
        HANDLE_CODE(report_quant_utra_fdd_r16->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(include_common_location_info_r16_present, 1));
      HANDLE_CODE(bref.pack(include_bt_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_wlan_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_sensor_meas_r16.is_present(), 1));
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->pack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->pack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_quant_relay_r17.is_present(), 1));
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_cfg_inter_rat_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(event_id.unpack(bref));
  HANDLE_CODE(rs_type.unpack(bref));
  HANDLE_CODE(report_interv.unpack(bref));
  HANDLE_CODE(report_amount.unpack(bref));
  HANDLE_CODE(report_quant.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cells, bref, (uint8_t)1u, (uint8_t)8u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(report_quant_utra_fdd_r16, bref);
      if (report_quant_utra_fdd_r16.is_present()) {
        HANDLE_CODE(report_quant_utra_fdd_r16->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(include_common_location_info_r16_present, 1));
      unpack_presence_flag(include_bt_meas_r16, bref);
      unpack_presence_flag(include_wlan_meas_r16, bref);
      unpack_presence_flag(include_sensor_meas_r16, bref);
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->unpack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->unpack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(report_quant_relay_r17, bref);
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void event_trigger_cfg_inter_rat_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventId");
  event_id.to_json(j);
  j.write_str("rsType", rs_type.to_string());
  j.write_str("reportInterval", report_interv.to_string());
  j.write_str("reportAmount", report_amount.to_string());
  j.write_fieldname("reportQuantity");
  report_quant.to_json(j);
  j.write_int("maxReportCells", max_report_cells);
  if (ext) {
    if (report_quant_utra_fdd_r16.is_present()) {
      j.write_fieldname("reportQuantityUTRA-FDD-r16");
      report_quant_utra_fdd_r16->to_json(j);
    }
    if (include_common_location_info_r16_present) {
      j.write_str("includeCommonLocationInfo-r16", "true");
    }
    if (include_bt_meas_r16.is_present()) {
      j.write_fieldname("includeBT-Meas-r16");
      include_bt_meas_r16->to_json(j);
    }
    if (include_wlan_meas_r16.is_present()) {
      j.write_fieldname("includeWLAN-Meas-r16");
      include_wlan_meas_r16->to_json(j);
    }
    if (include_sensor_meas_r16.is_present()) {
      j.write_fieldname("includeSensor-Meas-r16");
      include_sensor_meas_r16->to_json(j);
    }
    if (report_quant_relay_r17.is_present()) {
      j.write_fieldname("reportQuantityRelay-r17");
      report_quant_relay_r17->to_json(j);
    }
  }
  j.end_obj();
}

void event_trigger_cfg_inter_rat_s::event_id_c_::destroy_()
{
  switch (type_) {
    case types::event_b1:
      c.destroy<event_b1_s_>();
      break;
    case types::event_b2:
      c.destroy<event_b2_s_>();
      break;
    case types::event_b1_utra_fdd_r16:
      c.destroy<event_b1_utra_fdd_r16_s_>();
      break;
    case types::event_b2_utra_fdd_r16:
      c.destroy<event_b2_utra_fdd_r16_s_>();
      break;
    case types::event_y1_relay_r17:
      c.destroy<event_y1_relay_r17_s_>();
      break;
    case types::event_y2_relay_r17:
      c.destroy<event_y2_relay_r17_s_>();
      break;
    default:
      break;
  }
}
void event_trigger_cfg_inter_rat_s::event_id_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event_b1:
      c.init<event_b1_s_>();
      break;
    case types::event_b2:
      c.init<event_b2_s_>();
      break;
    case types::event_b1_utra_fdd_r16:
      c.init<event_b1_utra_fdd_r16_s_>();
      break;
    case types::event_b2_utra_fdd_r16:
      c.init<event_b2_utra_fdd_r16_s_>();
      break;
    case types::event_y1_relay_r17:
      c.init<event_y1_relay_r17_s_>();
      break;
    case types::event_y2_relay_r17:
      c.init<event_y2_relay_r17_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_inter_rat_s::event_id_c_");
  }
}
event_trigger_cfg_inter_rat_s::event_id_c_::event_id_c_(const event_trigger_cfg_inter_rat_s::event_id_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event_b1:
      c.init(other.c.get<event_b1_s_>());
      break;
    case types::event_b2:
      c.init(other.c.get<event_b2_s_>());
      break;
    case types::event_b1_utra_fdd_r16:
      c.init(other.c.get<event_b1_utra_fdd_r16_s_>());
      break;
    case types::event_b2_utra_fdd_r16:
      c.init(other.c.get<event_b2_utra_fdd_r16_s_>());
      break;
    case types::event_y1_relay_r17:
      c.init(other.c.get<event_y1_relay_r17_s_>());
      break;
    case types::event_y2_relay_r17:
      c.init(other.c.get<event_y2_relay_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_inter_rat_s::event_id_c_");
  }
}
event_trigger_cfg_inter_rat_s::event_id_c_&
event_trigger_cfg_inter_rat_s::event_id_c_::operator=(const event_trigger_cfg_inter_rat_s::event_id_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event_b1:
      c.set(other.c.get<event_b1_s_>());
      break;
    case types::event_b2:
      c.set(other.c.get<event_b2_s_>());
      break;
    case types::event_b1_utra_fdd_r16:
      c.set(other.c.get<event_b1_utra_fdd_r16_s_>());
      break;
    case types::event_b2_utra_fdd_r16:
      c.set(other.c.get<event_b2_utra_fdd_r16_s_>());
      break;
    case types::event_y1_relay_r17:
      c.set(other.c.get<event_y1_relay_r17_s_>());
      break;
    case types::event_y2_relay_r17:
      c.set(other.c.get<event_y2_relay_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_inter_rat_s::event_id_c_");
  }

  return *this;
}
event_trigger_cfg_inter_rat_s::event_id_c_::event_b1_s_& event_trigger_cfg_inter_rat_s::event_id_c_::set_event_b1()
{
  set(types::event_b1);
  return c.get<event_b1_s_>();
}
event_trigger_cfg_inter_rat_s::event_id_c_::event_b2_s_& event_trigger_cfg_inter_rat_s::event_id_c_::set_event_b2()
{
  set(types::event_b2);
  return c.get<event_b2_s_>();
}
event_trigger_cfg_inter_rat_s::event_id_c_::event_b1_utra_fdd_r16_s_&
event_trigger_cfg_inter_rat_s::event_id_c_::set_event_b1_utra_fdd_r16()
{
  set(types::event_b1_utra_fdd_r16);
  return c.get<event_b1_utra_fdd_r16_s_>();
}
event_trigger_cfg_inter_rat_s::event_id_c_::event_b2_utra_fdd_r16_s_&
event_trigger_cfg_inter_rat_s::event_id_c_::set_event_b2_utra_fdd_r16()
{
  set(types::event_b2_utra_fdd_r16);
  return c.get<event_b2_utra_fdd_r16_s_>();
}
event_trigger_cfg_inter_rat_s::event_id_c_::event_y1_relay_r17_s_&
event_trigger_cfg_inter_rat_s::event_id_c_::set_event_y1_relay_r17()
{
  set(types::event_y1_relay_r17);
  return c.get<event_y1_relay_r17_s_>();
}
event_trigger_cfg_inter_rat_s::event_id_c_::event_y2_relay_r17_s_&
event_trigger_cfg_inter_rat_s::event_id_c_::set_event_y2_relay_r17()
{
  set(types::event_y2_relay_r17);
  return c.get<event_y2_relay_r17_s_>();
}
void event_trigger_cfg_inter_rat_s::event_id_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event_b1:
      j.write_fieldname("eventB1");
      j.start_obj();
      j.write_fieldname("b1-ThresholdEUTRA");
      c.get<event_b1_s_>().b1_thres_eutra.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_b1_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_b1_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_b1_s_>().time_to_trigger.to_string());
      j.end_obj();
      break;
    case types::event_b2:
      j.write_fieldname("eventB2");
      j.start_obj();
      j.write_fieldname("b2-Threshold1");
      c.get<event_b2_s_>().b2_thres1.to_json(j);
      j.write_fieldname("b2-Threshold2EUTRA");
      c.get<event_b2_s_>().b2_thres2_eutra.to_json(j);
      j.write_bool("reportOnLeave", c.get<event_b2_s_>().report_on_leave);
      j.write_int("hysteresis", c.get<event_b2_s_>().hysteresis);
      j.write_str("timeToTrigger", c.get<event_b2_s_>().time_to_trigger.to_string());
      j.end_obj();
      break;
    case types::event_b1_utra_fdd_r16:
      j.write_fieldname("eventB1-UTRA-FDD-r16");
      j.start_obj();
      j.write_fieldname("b1-ThresholdUTRA-FDD-r16");
      c.get<event_b1_utra_fdd_r16_s_>().b1_thres_utra_fdd_r16.to_json(j);
      j.write_bool("reportOnLeave-r16", c.get<event_b1_utra_fdd_r16_s_>().report_on_leave_r16);
      j.write_int("hysteresis-r16", c.get<event_b1_utra_fdd_r16_s_>().hysteresis_r16);
      j.write_str("timeToTrigger-r16", c.get<event_b1_utra_fdd_r16_s_>().time_to_trigger_r16.to_string());
      j.end_obj();
      break;
    case types::event_b2_utra_fdd_r16:
      j.write_fieldname("eventB2-UTRA-FDD-r16");
      j.start_obj();
      j.write_fieldname("b2-Threshold1-r16");
      c.get<event_b2_utra_fdd_r16_s_>().b2_thres1_r16.to_json(j);
      j.write_fieldname("b2-Threshold2UTRA-FDD-r16");
      c.get<event_b2_utra_fdd_r16_s_>().b2_thres2_utra_fdd_r16.to_json(j);
      j.write_bool("reportOnLeave-r16", c.get<event_b2_utra_fdd_r16_s_>().report_on_leave_r16);
      j.write_int("hysteresis-r16", c.get<event_b2_utra_fdd_r16_s_>().hysteresis_r16);
      j.write_str("timeToTrigger-r16", c.get<event_b2_utra_fdd_r16_s_>().time_to_trigger_r16.to_string());
      j.end_obj();
      break;
    case types::event_y1_relay_r17:
      j.write_fieldname("eventY1-Relay-r17");
      j.start_obj();
      j.write_fieldname("y1-Threshold1-r17");
      c.get<event_y1_relay_r17_s_>().y1_thres1_r17.to_json(j);
      j.write_fieldname("y1-Threshold2-Relay-r17");
      c.get<event_y1_relay_r17_s_>().y1_thres2_relay_r17.to_json(j);
      j.write_bool("reportOnLeave-r17", c.get<event_y1_relay_r17_s_>().report_on_leave_r17);
      j.write_int("hysteresis-r17", c.get<event_y1_relay_r17_s_>().hysteresis_r17);
      j.write_str("timeToTrigger-r17", c.get<event_y1_relay_r17_s_>().time_to_trigger_r17.to_string());
      j.end_obj();
      break;
    case types::event_y2_relay_r17:
      j.write_fieldname("eventY2-Relay-r17");
      j.start_obj();
      j.write_fieldname("y2-Threshold-Relay-r17");
      c.get<event_y2_relay_r17_s_>().y2_thres_relay_r17.to_json(j);
      j.write_bool("reportOnLeave-r17", c.get<event_y2_relay_r17_s_>().report_on_leave_r17);
      j.write_int("hysteresis-r17", c.get<event_y2_relay_r17_s_>().hysteresis_r17);
      j.write_str("timeToTrigger-r17", c.get<event_y2_relay_r17_s_>().time_to_trigger_r17.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_inter_rat_s::event_id_c_");
  }
  j.end_obj();
}
SRSASN_CODE event_trigger_cfg_inter_rat_s::event_id_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event_b1:
      bref.pack(c.get<event_b1_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b1_s_>().b1_thres_eutra.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_b1_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_b1_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b1_s_>().time_to_trigger.pack(bref));
      break;
    case types::event_b2:
      bref.pack(c.get<event_b2_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres1.pack(bref));
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres2_eutra.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_b2_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_b2_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b2_s_>().time_to_trigger.pack(bref));
      break;
    case types::event_b1_utra_fdd_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      bref.pack(c.get<event_b1_utra_fdd_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b1_utra_fdd_r16_s_>().b1_thres_utra_fdd_r16.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_b1_utra_fdd_r16_s_>().report_on_leave_r16, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_b1_utra_fdd_r16_s_>().hysteresis_r16, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b1_utra_fdd_r16_s_>().time_to_trigger_r16.pack(bref));
    } break;
    case types::event_b2_utra_fdd_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      bref.pack(c.get<event_b2_utra_fdd_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b2_utra_fdd_r16_s_>().b2_thres1_r16.pack(bref));
      HANDLE_CODE(c.get<event_b2_utra_fdd_r16_s_>().b2_thres2_utra_fdd_r16.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_b2_utra_fdd_r16_s_>().report_on_leave_r16, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_b2_utra_fdd_r16_s_>().hysteresis_r16, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b2_utra_fdd_r16_s_>().time_to_trigger_r16.pack(bref));
    } break;
    case types::event_y1_relay_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      bref.pack(c.get<event_y1_relay_r17_s_>().ext, 1);
      HANDLE_CODE(c.get<event_y1_relay_r17_s_>().y1_thres1_r17.pack(bref));
      HANDLE_CODE(c.get<event_y1_relay_r17_s_>().y1_thres2_relay_r17.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_y1_relay_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_y1_relay_r17_s_>().hysteresis_r17, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_y1_relay_r17_s_>().time_to_trigger_r17.pack(bref));
    } break;
    case types::event_y2_relay_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      bref.pack(c.get<event_y2_relay_r17_s_>().ext, 1);
      HANDLE_CODE(c.get<event_y2_relay_r17_s_>().y2_thres_relay_r17.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_y2_relay_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_y2_relay_r17_s_>().hysteresis_r17, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_y2_relay_r17_s_>().time_to_trigger_r17.pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_inter_rat_s::event_id_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_cfg_inter_rat_s::event_id_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event_b1:
      bref.unpack(c.get<event_b1_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b1_s_>().b1_thres_eutra.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_b1_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_b1_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b1_s_>().time_to_trigger.unpack(bref));
      break;
    case types::event_b2:
      bref.unpack(c.get<event_b2_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres1.unpack(bref));
      HANDLE_CODE(c.get<event_b2_s_>().b2_thres2_eutra.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_b2_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_b2_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b2_s_>().time_to_trigger.unpack(bref));
      break;
    case types::event_b1_utra_fdd_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      bref.unpack(c.get<event_b1_utra_fdd_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b1_utra_fdd_r16_s_>().b1_thres_utra_fdd_r16.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_b1_utra_fdd_r16_s_>().report_on_leave_r16, 1));
      HANDLE_CODE(unpack_integer(c.get<event_b1_utra_fdd_r16_s_>().hysteresis_r16, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b1_utra_fdd_r16_s_>().time_to_trigger_r16.unpack(bref));
    } break;
    case types::event_b2_utra_fdd_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      bref.unpack(c.get<event_b2_utra_fdd_r16_s_>().ext, 1);
      HANDLE_CODE(c.get<event_b2_utra_fdd_r16_s_>().b2_thres1_r16.unpack(bref));
      HANDLE_CODE(c.get<event_b2_utra_fdd_r16_s_>().b2_thres2_utra_fdd_r16.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_b2_utra_fdd_r16_s_>().report_on_leave_r16, 1));
      HANDLE_CODE(unpack_integer(c.get<event_b2_utra_fdd_r16_s_>().hysteresis_r16, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_b2_utra_fdd_r16_s_>().time_to_trigger_r16.unpack(bref));
    } break;
    case types::event_y1_relay_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      bref.unpack(c.get<event_y1_relay_r17_s_>().ext, 1);
      HANDLE_CODE(c.get<event_y1_relay_r17_s_>().y1_thres1_r17.unpack(bref));
      HANDLE_CODE(c.get<event_y1_relay_r17_s_>().y1_thres2_relay_r17.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_y1_relay_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(unpack_integer(c.get<event_y1_relay_r17_s_>().hysteresis_r17, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_y1_relay_r17_s_>().time_to_trigger_r17.unpack(bref));
    } break;
    case types::event_y2_relay_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      bref.unpack(c.get<event_y2_relay_r17_s_>().ext, 1);
      HANDLE_CODE(c.get<event_y2_relay_r17_s_>().y2_thres_relay_r17.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_y2_relay_r17_s_>().report_on_leave_r17, 1));
      HANDLE_CODE(unpack_integer(c.get<event_y2_relay_r17_s_>().hysteresis_r17, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_y2_relay_r17_s_>().time_to_trigger_r17.unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_inter_rat_s::event_id_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* event_trigger_cfg_inter_rat_s::report_amount_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "event_trigger_cfg_inter_rat_s::report_amount_e_");
}
int8_t event_trigger_cfg_inter_rat_s::report_amount_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "event_trigger_cfg_inter_rat_s::report_amount_e_");
}

// EventTriggerConfigNR-SL-r16 ::= SEQUENCE
SRSASN_CODE event_trigger_cfg_nr_sl_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(event_id_r16.pack(bref));
  HANDLE_CODE(report_interv_r16.pack(bref));
  HANDLE_CODE(report_amount_r16.pack(bref));
  HANDLE_CODE(report_quant_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_cfg_nr_sl_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(event_id_r16.unpack(bref));
  HANDLE_CODE(report_interv_r16.unpack(bref));
  HANDLE_CODE(report_amount_r16.unpack(bref));
  HANDLE_CODE(report_quant_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void event_trigger_cfg_nr_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventId-r16");
  event_id_r16.to_json(j);
  j.write_str("reportInterval-r16", report_interv_r16.to_string());
  j.write_str("reportAmount-r16", report_amount_r16.to_string());
  j.write_fieldname("reportQuantity-r16");
  report_quant_r16.to_json(j);
  j.end_obj();
}

void event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::destroy_()
{
  switch (type_) {
    case types::event_c1:
      c.destroy<event_c1_s_>();
      break;
    case types::event_c2_r16:
      c.destroy<event_c2_r16_s_>();
      break;
    default:
      break;
  }
}
void event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::event_c1:
      c.init<event_c1_s_>();
      break;
    case types::event_c2_r16:
      c.init<event_c2_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_");
  }
}
event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::event_id_r16_c_(
    const event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::event_c1:
      c.init(other.c.get<event_c1_s_>());
      break;
    case types::event_c2_r16:
      c.init(other.c.get<event_c2_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_");
  }
}
event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_&
event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::operator=(const event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::event_c1:
      c.set(other.c.get<event_c1_s_>());
      break;
    case types::event_c2_r16:
      c.set(other.c.get<event_c2_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_");
  }

  return *this;
}
event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::event_c1_s_&
event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::set_event_c1()
{
  set(types::event_c1);
  return c.get<event_c1_s_>();
}
event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::event_c2_r16_s_&
event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::set_event_c2_r16()
{
  set(types::event_c2_r16);
  return c.get<event_c2_r16_s_>();
}
void event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::event_c1:
      j.write_fieldname("eventC1");
      j.start_obj();
      j.write_int("c1-Threshold-r16", c.get<event_c1_s_>().c1_thres_r16);
      j.write_int("hysteresis-r16", c.get<event_c1_s_>().hysteresis_r16);
      j.write_str("timeToTrigger-r16", c.get<event_c1_s_>().time_to_trigger_r16.to_string());
      j.end_obj();
      break;
    case types::event_c2_r16:
      j.write_fieldname("eventC2-r16");
      j.start_obj();
      j.write_int("c2-Threshold-r16", c.get<event_c2_r16_s_>().c2_thres_r16);
      j.write_int("hysteresis-r16", c.get<event_c2_r16_s_>().hysteresis_r16);
      j.write_str("timeToTrigger-r16", c.get<event_c2_r16_s_>().time_to_trigger_r16.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::event_c1:
      HANDLE_CODE(pack_integer(bref, c.get<event_c1_s_>().c1_thres_r16, (uint8_t)0u, (uint8_t)100u));
      HANDLE_CODE(pack_integer(bref, c.get<event_c1_s_>().hysteresis_r16, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_c1_s_>().time_to_trigger_r16.pack(bref));
      break;
    case types::event_c2_r16:
      HANDLE_CODE(pack_integer(bref, c.get<event_c2_r16_s_>().c2_thres_r16, (uint8_t)0u, (uint8_t)100u));
      HANDLE_CODE(pack_integer(bref, c.get<event_c2_r16_s_>().hysteresis_r16, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_c2_r16_s_>().time_to_trigger_r16.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::event_c1:
      HANDLE_CODE(unpack_integer(c.get<event_c1_s_>().c1_thres_r16, bref, (uint8_t)0u, (uint8_t)100u));
      HANDLE_CODE(unpack_integer(c.get<event_c1_s_>().hysteresis_r16, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_c1_s_>().time_to_trigger_r16.unpack(bref));
      break;
    case types::event_c2_r16:
      HANDLE_CODE(unpack_integer(c.get<event_c2_r16_s_>().c2_thres_r16, bref, (uint8_t)0u, (uint8_t)100u));
      HANDLE_CODE(unpack_integer(c.get<event_c2_r16_s_>().hysteresis_r16, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_c2_r16_s_>().time_to_trigger_r16.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* event_trigger_cfg_nr_sl_r16_s::report_amount_r16_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "event_trigger_cfg_nr_sl_r16_s::report_amount_r16_e_");
}
int8_t event_trigger_cfg_nr_sl_r16_s::report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "event_trigger_cfg_nr_sl_r16_s::report_amount_r16_e_");
}

// FilterConfig ::= SEQUENCE
SRSASN_CODE filt_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_rsrp_present, 1));
  HANDLE_CODE(bref.pack(filt_coef_rsrq_present, 1));
  HANDLE_CODE(bref.pack(filt_coef_rs_sinr_present, 1));

  if (filt_coef_rsrp_present) {
    HANDLE_CODE(filt_coef_rsrp.pack(bref));
  }
  if (filt_coef_rsrq_present) {
    HANDLE_CODE(filt_coef_rsrq.pack(bref));
  }
  if (filt_coef_rs_sinr_present) {
    HANDLE_CODE(filt_coef_rs_sinr.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE filt_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_rsrp_present, 1));
  HANDLE_CODE(bref.unpack(filt_coef_rsrq_present, 1));
  HANDLE_CODE(bref.unpack(filt_coef_rs_sinr_present, 1));

  if (filt_coef_rsrp_present) {
    HANDLE_CODE(filt_coef_rsrp.unpack(bref));
  }
  if (filt_coef_rsrq_present) {
    HANDLE_CODE(filt_coef_rsrq.unpack(bref));
  }
  if (filt_coef_rs_sinr_present) {
    HANDLE_CODE(filt_coef_rs_sinr.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void filt_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coef_rsrp_present) {
    j.write_str("filterCoefficientRSRP", filt_coef_rsrp.to_string());
  }
  if (filt_coef_rsrq_present) {
    j.write_str("filterCoefficientRSRQ", filt_coef_rsrq.to_string());
  }
  if (filt_coef_rs_sinr_present) {
    j.write_str("filterCoefficientRS-SINR", filt_coef_rs_sinr.to_string());
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

// PCI-RangeElement ::= SEQUENCE
SRSASN_CODE pci_range_elem_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_range_idx, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pci_range.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_range_elem_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_range_idx, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pci_range.unpack(bref));

  return SRSASN_SUCCESS;
}
void pci_range_elem_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pci-RangeIndex", pci_range_idx);
  j.write_fieldname("pci-Range");
  pci_range.to_json(j);
  j.end_obj();
}

// PeriodicalReportConfig ::= SEQUENCE
SRSASN_CODE periodical_report_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(report_quant_rs_idxes_present, 1));
  HANDLE_CODE(bref.pack(max_nrof_rs_idxes_to_report_present, 1));

  HANDLE_CODE(rs_type.pack(bref));
  HANDLE_CODE(report_interv.pack(bref));
  HANDLE_CODE(report_amount.pack(bref));
  HANDLE_CODE(report_quant_cell.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cells, (uint8_t)1u, (uint8_t)8u));
  if (report_quant_rs_idxes_present) {
    HANDLE_CODE(report_quant_rs_idxes.pack(bref));
  }
  if (max_nrof_rs_idxes_to_report_present) {
    HANDLE_CODE(pack_integer(bref, max_nrof_rs_idxes_to_report, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(bref.pack(include_beam_meass, 1));
  HANDLE_CODE(bref.pack(use_allowed_cell_list, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_rssi_report_cfg_r16.is_present();
    group_flags[0] |= include_common_location_info_r16_present;
    group_flags[0] |= include_bt_meas_r16.is_present();
    group_flags[0] |= include_wlan_meas_r16.is_present();
    group_flags[0] |= include_sensor_meas_r16.is_present();
    group_flags[0] |= ul_delay_value_cfg_r16.is_present();
    group_flags[0] |= report_add_neigh_meas_r16_present;
    group_flags[1] |= ul_excess_delay_cfg_r17.is_present();
    group_flags[1] |= coarse_location_request_r17_present;
    group_flags[1] |= report_quant_relay_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_rssi_report_cfg_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_common_location_info_r16_present, 1));
      HANDLE_CODE(bref.pack(include_bt_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_wlan_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_sensor_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_delay_value_cfg_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(report_add_neigh_meas_r16_present, 1));
      if (meas_rssi_report_cfg_r16.is_present()) {
        HANDLE_CODE(meas_rssi_report_cfg_r16->pack(bref));
      }
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->pack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->pack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->pack(bref));
      }
      if (ul_delay_value_cfg_r16.is_present()) {
        HANDLE_CODE(ul_delay_value_cfg_r16->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_excess_delay_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(coarse_location_request_r17_present, 1));
      HANDLE_CODE(bref.pack(report_quant_relay_r17.is_present(), 1));
      if (ul_excess_delay_cfg_r17.is_present()) {
        HANDLE_CODE(ul_excess_delay_cfg_r17->pack(bref));
      }
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE periodical_report_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(report_quant_rs_idxes_present, 1));
  HANDLE_CODE(bref.unpack(max_nrof_rs_idxes_to_report_present, 1));

  HANDLE_CODE(rs_type.unpack(bref));
  HANDLE_CODE(report_interv.unpack(bref));
  HANDLE_CODE(report_amount.unpack(bref));
  HANDLE_CODE(report_quant_cell.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cells, bref, (uint8_t)1u, (uint8_t)8u));
  if (report_quant_rs_idxes_present) {
    HANDLE_CODE(report_quant_rs_idxes.unpack(bref));
  }
  if (max_nrof_rs_idxes_to_report_present) {
    HANDLE_CODE(unpack_integer(max_nrof_rs_idxes_to_report, bref, (uint8_t)1u, (uint8_t)32u));
  }
  HANDLE_CODE(bref.unpack(include_beam_meass, 1));
  HANDLE_CODE(bref.unpack(use_allowed_cell_list, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_rssi_report_cfg_r16, bref);
      HANDLE_CODE(bref.unpack(include_common_location_info_r16_present, 1));
      unpack_presence_flag(include_bt_meas_r16, bref);
      unpack_presence_flag(include_wlan_meas_r16, bref);
      unpack_presence_flag(include_sensor_meas_r16, bref);
      unpack_presence_flag(ul_delay_value_cfg_r16, bref);
      HANDLE_CODE(bref.unpack(report_add_neigh_meas_r16_present, 1));
      if (meas_rssi_report_cfg_r16.is_present()) {
        HANDLE_CODE(meas_rssi_report_cfg_r16->unpack(bref));
      }
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->unpack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->unpack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->unpack(bref));
      }
      if (ul_delay_value_cfg_r16.is_present()) {
        HANDLE_CODE(ul_delay_value_cfg_r16->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ul_excess_delay_cfg_r17, bref);
      HANDLE_CODE(bref.unpack(coarse_location_request_r17_present, 1));
      unpack_presence_flag(report_quant_relay_r17, bref);
      if (ul_excess_delay_cfg_r17.is_present()) {
        HANDLE_CODE(ul_excess_delay_cfg_r17->unpack(bref));
      }
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void periodical_report_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rsType", rs_type.to_string());
  j.write_str("reportInterval", report_interv.to_string());
  j.write_str("reportAmount", report_amount.to_string());
  j.write_fieldname("reportQuantityCell");
  report_quant_cell.to_json(j);
  j.write_int("maxReportCells", max_report_cells);
  if (report_quant_rs_idxes_present) {
    j.write_fieldname("reportQuantityRS-Indexes");
    report_quant_rs_idxes.to_json(j);
  }
  if (max_nrof_rs_idxes_to_report_present) {
    j.write_int("maxNrofRS-IndexesToReport", max_nrof_rs_idxes_to_report);
  }
  j.write_bool("includeBeamMeasurements", include_beam_meass);
  j.write_bool("useAllowedCellList", use_allowed_cell_list);
  if (ext) {
    if (meas_rssi_report_cfg_r16.is_present()) {
      j.write_fieldname("measRSSI-ReportConfig-r16");
      meas_rssi_report_cfg_r16->to_json(j);
    }
    if (include_common_location_info_r16_present) {
      j.write_str("includeCommonLocationInfo-r16", "true");
    }
    if (include_bt_meas_r16.is_present()) {
      j.write_fieldname("includeBT-Meas-r16");
      include_bt_meas_r16->to_json(j);
    }
    if (include_wlan_meas_r16.is_present()) {
      j.write_fieldname("includeWLAN-Meas-r16");
      include_wlan_meas_r16->to_json(j);
    }
    if (include_sensor_meas_r16.is_present()) {
      j.write_fieldname("includeSensor-Meas-r16");
      include_sensor_meas_r16->to_json(j);
    }
    if (ul_delay_value_cfg_r16.is_present()) {
      j.write_fieldname("ul-DelayValueConfig-r16");
      ul_delay_value_cfg_r16->to_json(j);
    }
    if (report_add_neigh_meas_r16_present) {
      j.write_str("reportAddNeighMeas-r16", "setup");
    }
    if (ul_excess_delay_cfg_r17.is_present()) {
      j.write_fieldname("ul-ExcessDelayConfig-r17");
      ul_excess_delay_cfg_r17->to_json(j);
    }
    if (coarse_location_request_r17_present) {
      j.write_str("coarseLocationRequest-r17", "true");
    }
    if (report_quant_relay_r17.is_present()) {
      j.write_fieldname("reportQuantityRelay-r17");
      report_quant_relay_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* periodical_report_cfg_s::report_amount_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "periodical_report_cfg_s::report_amount_e_");
}
int8_t periodical_report_cfg_s::report_amount_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "periodical_report_cfg_s::report_amount_e_");
}

// PeriodicalReportConfigInterRAT ::= SEQUENCE
SRSASN_CODE periodical_report_cfg_inter_rat_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(report_interv.pack(bref));
  HANDLE_CODE(report_amount.pack(bref));
  HANDLE_CODE(report_quant.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cells, (uint8_t)1u, (uint8_t)8u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= report_quant_utra_fdd_r16.is_present();
    group_flags[1] |= include_common_location_info_r16_present;
    group_flags[1] |= include_bt_meas_r16.is_present();
    group_flags[1] |= include_wlan_meas_r16.is_present();
    group_flags[1] |= include_sensor_meas_r16.is_present();
    group_flags[2] |= report_quant_relay_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_quant_utra_fdd_r16.is_present(), 1));
      if (report_quant_utra_fdd_r16.is_present()) {
        HANDLE_CODE(report_quant_utra_fdd_r16->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(include_common_location_info_r16_present, 1));
      HANDLE_CODE(bref.pack(include_bt_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_wlan_meas_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(include_sensor_meas_r16.is_present(), 1));
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->pack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->pack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_quant_relay_r17.is_present(), 1));
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE periodical_report_cfg_inter_rat_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(report_interv.unpack(bref));
  HANDLE_CODE(report_amount.unpack(bref));
  HANDLE_CODE(report_quant.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cells, bref, (uint8_t)1u, (uint8_t)8u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(report_quant_utra_fdd_r16, bref);
      if (report_quant_utra_fdd_r16.is_present()) {
        HANDLE_CODE(report_quant_utra_fdd_r16->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(include_common_location_info_r16_present, 1));
      unpack_presence_flag(include_bt_meas_r16, bref);
      unpack_presence_flag(include_wlan_meas_r16, bref);
      unpack_presence_flag(include_sensor_meas_r16, bref);
      if (include_bt_meas_r16.is_present()) {
        HANDLE_CODE(include_bt_meas_r16->unpack(bref));
      }
      if (include_wlan_meas_r16.is_present()) {
        HANDLE_CODE(include_wlan_meas_r16->unpack(bref));
      }
      if (include_sensor_meas_r16.is_present()) {
        HANDLE_CODE(include_sensor_meas_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(report_quant_relay_r17, bref);
      if (report_quant_relay_r17.is_present()) {
        HANDLE_CODE(report_quant_relay_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void periodical_report_cfg_inter_rat_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reportInterval", report_interv.to_string());
  j.write_str("reportAmount", report_amount.to_string());
  j.write_fieldname("reportQuantity");
  report_quant.to_json(j);
  j.write_int("maxReportCells", max_report_cells);
  if (ext) {
    if (report_quant_utra_fdd_r16.is_present()) {
      j.write_fieldname("reportQuantityUTRA-FDD-r16");
      report_quant_utra_fdd_r16->to_json(j);
    }
    if (include_common_location_info_r16_present) {
      j.write_str("includeCommonLocationInfo-r16", "true");
    }
    if (include_bt_meas_r16.is_present()) {
      j.write_fieldname("includeBT-Meas-r16");
      include_bt_meas_r16->to_json(j);
    }
    if (include_wlan_meas_r16.is_present()) {
      j.write_fieldname("includeWLAN-Meas-r16");
      include_wlan_meas_r16->to_json(j);
    }
    if (include_sensor_meas_r16.is_present()) {
      j.write_fieldname("includeSensor-Meas-r16");
      include_sensor_meas_r16->to_json(j);
    }
    if (report_quant_relay_r17.is_present()) {
      j.write_fieldname("reportQuantityRelay-r17");
      report_quant_relay_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* periodical_report_cfg_inter_rat_s::report_amount_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "periodical_report_cfg_inter_rat_s::report_amount_e_");
}
int8_t periodical_report_cfg_inter_rat_s::report_amount_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "periodical_report_cfg_inter_rat_s::report_amount_e_");
}

// PeriodicalReportConfigNR-SL-r16 ::= SEQUENCE
SRSASN_CODE periodical_report_cfg_nr_sl_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(report_interv_r16.pack(bref));
  HANDLE_CODE(report_amount_r16.pack(bref));
  HANDLE_CODE(report_quant_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE periodical_report_cfg_nr_sl_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(report_interv_r16.unpack(bref));
  HANDLE_CODE(report_amount_r16.unpack(bref));
  HANDLE_CODE(report_quant_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void periodical_report_cfg_nr_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reportInterval-r16", report_interv_r16.to_string());
  j.write_str("reportAmount-r16", report_amount_r16.to_string());
  j.write_fieldname("reportQuantity-r16");
  report_quant_r16.to_json(j);
  j.end_obj();
}

const char* periodical_report_cfg_nr_sl_r16_s::report_amount_r16_opts::to_string() const
{
  static const char* names[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(names, 8, value, "periodical_report_cfg_nr_sl_r16_s::report_amount_r16_e_");
}
int8_t periodical_report_cfg_nr_sl_r16_s::report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "periodical_report_cfg_nr_sl_r16_s::report_amount_r16_e_");
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

// RMTC-Config-r16 ::= SEQUENCE
SRSASN_CODE rmtc_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rmtc_sf_offset_r16_present, 1));

  HANDLE_CODE(rmtc_periodicity_r16.pack(bref));
  if (rmtc_sf_offset_r16_present) {
    HANDLE_CODE(pack_integer(bref, rmtc_sf_offset_r16, (uint16_t)0u, (uint16_t)639u));
  }
  HANDLE_CODE(meas_dur_symbols_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, rmtc_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(ref_scs_cp_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rmtc_bw_r17_present;
    group_flags[0] |= meas_dur_symbols_v1700_present;
    group_flags[0] |= ref_scs_cp_v1700_present;
    group_flags[0] |= tci_state_info_r17.is_present();
    group_flags[1] |= ref_bwp_id_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rmtc_bw_r17_present, 1));
      HANDLE_CODE(bref.pack(meas_dur_symbols_v1700_present, 1));
      HANDLE_CODE(bref.pack(ref_scs_cp_v1700_present, 1));
      HANDLE_CODE(bref.pack(tci_state_info_r17.is_present(), 1));
      if (rmtc_bw_r17_present) {
        HANDLE_CODE(rmtc_bw_r17.pack(bref));
      }
      if (meas_dur_symbols_v1700_present) {
        HANDLE_CODE(meas_dur_symbols_v1700.pack(bref));
      }
      if (ref_scs_cp_v1700_present) {
        HANDLE_CODE(ref_scs_cp_v1700.pack(bref));
      }
      if (tci_state_info_r17.is_present()) {
        HANDLE_CODE(bref.pack(tci_state_info_r17->ref_serv_cell_id_r17_present, 1));
        HANDLE_CODE(pack_integer(bref, tci_state_info_r17->tci_state_id_r17, (uint8_t)0u, (uint8_t)127u));
        if (tci_state_info_r17->ref_serv_cell_id_r17_present) {
          HANDLE_CODE(pack_integer(bref, tci_state_info_r17->ref_serv_cell_id_r17, (uint8_t)0u, (uint8_t)31u));
        }
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ref_bwp_id_r17_present, 1));
      if (ref_bwp_id_r17_present) {
        HANDLE_CODE(pack_integer(bref, ref_bwp_id_r17, (uint8_t)0u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rmtc_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rmtc_sf_offset_r16_present, 1));

  HANDLE_CODE(rmtc_periodicity_r16.unpack(bref));
  if (rmtc_sf_offset_r16_present) {
    HANDLE_CODE(unpack_integer(rmtc_sf_offset_r16, bref, (uint16_t)0u, (uint16_t)639u));
  }
  HANDLE_CODE(meas_dur_symbols_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(rmtc_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(ref_scs_cp_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rmtc_bw_r17_present, 1));
      HANDLE_CODE(bref.unpack(meas_dur_symbols_v1700_present, 1));
      HANDLE_CODE(bref.unpack(ref_scs_cp_v1700_present, 1));
      unpack_presence_flag(tci_state_info_r17, bref);
      if (rmtc_bw_r17_present) {
        HANDLE_CODE(rmtc_bw_r17.unpack(bref));
      }
      if (meas_dur_symbols_v1700_present) {
        HANDLE_CODE(meas_dur_symbols_v1700.unpack(bref));
      }
      if (ref_scs_cp_v1700_present) {
        HANDLE_CODE(ref_scs_cp_v1700.unpack(bref));
      }
      if (tci_state_info_r17.is_present()) {
        HANDLE_CODE(bref.unpack(tci_state_info_r17->ref_serv_cell_id_r17_present, 1));
        HANDLE_CODE(unpack_integer(tci_state_info_r17->tci_state_id_r17, bref, (uint8_t)0u, (uint8_t)127u));
        if (tci_state_info_r17->ref_serv_cell_id_r17_present) {
          HANDLE_CODE(unpack_integer(tci_state_info_r17->ref_serv_cell_id_r17, bref, (uint8_t)0u, (uint8_t)31u));
        }
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ref_bwp_id_r17_present, 1));
      if (ref_bwp_id_r17_present) {
        HANDLE_CODE(unpack_integer(ref_bwp_id_r17, bref, (uint8_t)0u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rmtc_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rmtc-Periodicity-r16", rmtc_periodicity_r16.to_string());
  if (rmtc_sf_offset_r16_present) {
    j.write_int("rmtc-SubframeOffset-r16", rmtc_sf_offset_r16);
  }
  j.write_str("measDurationSymbols-r16", meas_dur_symbols_r16.to_string());
  j.write_int("rmtc-Frequency-r16", rmtc_freq_r16);
  j.write_str("ref-SCS-CP-r16", ref_scs_cp_r16.to_string());
  if (ext) {
    if (rmtc_bw_r17_present) {
      j.write_str("rmtc-Bandwidth-r17", rmtc_bw_r17.to_string());
    }
    if (meas_dur_symbols_v1700_present) {
      j.write_str("measDurationSymbols-v1700", meas_dur_symbols_v1700.to_string());
    }
    if (ref_scs_cp_v1700_present) {
      j.write_str("ref-SCS-CP-v1700", ref_scs_cp_v1700.to_string());
    }
    if (tci_state_info_r17.is_present()) {
      j.write_fieldname("tci-StateInfo-r17");
      j.start_obj();
      j.write_int("tci-StateId-r17", tci_state_info_r17->tci_state_id_r17);
      if (tci_state_info_r17->ref_serv_cell_id_r17_present) {
        j.write_int("ref-ServCellId-r17", tci_state_info_r17->ref_serv_cell_id_r17);
      }
      j.end_obj();
    }
    if (ref_bwp_id_r17_present) {
      j.write_int("ref-BWPId-r17", ref_bwp_id_r17);
    }
  }
  j.end_obj();
}

const char* rmtc_cfg_r16_s::rmtc_periodicity_r16_opts::to_string() const
{
  static const char* names[] = {"ms40", "ms80", "ms160", "ms320", "ms640"};
  return convert_enum_idx(names, 5, value, "rmtc_cfg_r16_s::rmtc_periodicity_r16_e_");
}
uint16_t rmtc_cfg_r16_s::rmtc_periodicity_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {40, 80, 160, 320, 640};
  return map_enum_number(numbers, 5, value, "rmtc_cfg_r16_s::rmtc_periodicity_r16_e_");
}

const char* rmtc_cfg_r16_s::meas_dur_symbols_r16_opts::to_string() const
{
  static const char* names[] = {"sym1", "sym14or12", "sym28or24", "sym42or36", "sym70or60"};
  return convert_enum_idx(names, 5, value, "rmtc_cfg_r16_s::meas_dur_symbols_r16_e_");
}
uint8_t rmtc_cfg_r16_s::meas_dur_symbols_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 14, 28, 42, 70};
  return map_enum_number(numbers, 5, value, "rmtc_cfg_r16_s::meas_dur_symbols_r16_e_");
}

const char* rmtc_cfg_r16_s::ref_scs_cp_r16_opts::to_string() const
{
  static const char* names[] = {"kHz15", "kHz30", "kHz60-NCP", "kHz60-ECP"};
  return convert_enum_idx(names, 4, value, "rmtc_cfg_r16_s::ref_scs_cp_r16_e_");
}

const char* rmtc_cfg_r16_s::rmtc_bw_r17_opts::to_string() const
{
  static const char* names[] = {"mhz100", "mhz400", "mhz800", "mhz1600", "mhz2000"};
  return convert_enum_idx(names, 5, value, "rmtc_cfg_r16_s::rmtc_bw_r17_e_");
}
uint16_t rmtc_cfg_r16_s::rmtc_bw_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 400, 800, 1600, 2000};
  return map_enum_number(numbers, 5, value, "rmtc_cfg_r16_s::rmtc_bw_r17_e_");
}

const char* rmtc_cfg_r16_s::meas_dur_symbols_v1700_opts::to_string() const
{
  static const char* names[] = {"sym140", "sym560", "sym1120"};
  return convert_enum_idx(names, 3, value, "rmtc_cfg_r16_s::meas_dur_symbols_v1700_e_");
}
uint16_t rmtc_cfg_r16_s::meas_dur_symbols_v1700_opts::to_number() const
{
  static const uint16_t numbers[] = {140, 560, 1120};
  return map_enum_number(numbers, 3, value, "rmtc_cfg_r16_s::meas_dur_symbols_v1700_e_");
}

const char* rmtc_cfg_r16_s::ref_scs_cp_v1700_opts::to_string() const
{
  static const char* names[] = {"kHz120", "kHz480", "kHz960"};
  return convert_enum_idx(names, 3, value, "rmtc_cfg_r16_s::ref_scs_cp_v1700_e_");
}
uint16_t rmtc_cfg_r16_s::ref_scs_cp_v1700_opts::to_number() const
{
  static const uint16_t numbers[] = {120, 480, 960};
  return map_enum_number(numbers, 3, value, "rmtc_cfg_r16_s::ref_scs_cp_v1700_e_");
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

// ReferenceSignalConfig ::= SEQUENCE
SRSASN_CODE ref_sig_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_cfg_mob_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_res_cfg_mob_present, 1));

  if (ssb_cfg_mob_present) {
    HANDLE_CODE(ssb_cfg_mob.pack(bref));
  }
  if (csi_rs_res_cfg_mob_present) {
    HANDLE_CODE(csi_rs_res_cfg_mob.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ref_sig_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_cfg_mob_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_res_cfg_mob_present, 1));

  if (ssb_cfg_mob_present) {
    HANDLE_CODE(ssb_cfg_mob.unpack(bref));
  }
  if (csi_rs_res_cfg_mob_present) {
    HANDLE_CODE(csi_rs_res_cfg_mob.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ref_sig_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_cfg_mob_present) {
    j.write_fieldname("ssb-ConfigMobility");
    ssb_cfg_mob.to_json(j);
  }
  if (csi_rs_res_cfg_mob_present) {
    j.write_fieldname("csi-rs-ResourceConfigMobility");
    csi_rs_res_cfg_mob.to_json(j);
  }
  j.end_obj();
}

// ReportCGI ::= SEQUENCE
SRSASN_CODE report_cgi_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)1007u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= use_autonomous_gaps_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(use_autonomous_gaps_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cgi_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)1007u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(use_autonomous_gaps_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void report_cgi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellForWhichToReportCGI", cell_for_which_to_report_cgi);
  if (ext) {
    if (use_autonomous_gaps_r16_present) {
      j.write_str("useAutonomousGaps-r16", "setup");
    }
  }
  j.end_obj();
}

// ReportCGI-EUTRA ::= SEQUENCE
SRSASN_CODE report_cgi_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)503u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= use_autonomous_gaps_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(use_autonomous_gaps_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cgi_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)503u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(use_autonomous_gaps_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void report_cgi_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellForWhichToReportCGI", cell_for_which_to_report_cgi);
  if (ext) {
    if (use_autonomous_gaps_r16_present) {
      j.write_str("useAutonomousGaps-r16", "setup");
    }
  }
  j.end_obj();
}

// ReportSFTD-EUTRA ::= SEQUENCE
SRSASN_CODE report_sftd_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(report_sftd_meas, 1));
  HANDLE_CODE(bref.pack(report_rsrp, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_sftd_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(report_sftd_meas, 1));
  HANDLE_CODE(bref.unpack(report_rsrp, 1));

  return SRSASN_SUCCESS;
}
void report_sftd_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("reportSFTD-Meas", report_sftd_meas);
  j.write_bool("reportRSRP", report_rsrp);
  j.end_obj();
}

// ReportSFTD-NR ::= SEQUENCE
SRSASN_CODE report_sftd_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(report_sftd_meas, 1));
  HANDLE_CODE(bref.pack(report_rsrp, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= report_sftd_neigh_meas_present;
    group_flags[0] |= drx_sftd_neigh_meas_present;
    group_flags[0] |= cells_for_which_to_report_sftd.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_sftd_neigh_meas_present, 1));
      HANDLE_CODE(bref.pack(drx_sftd_neigh_meas_present, 1));
      HANDLE_CODE(bref.pack(cells_for_which_to_report_sftd.is_present(), 1));
      if (cells_for_which_to_report_sftd.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cells_for_which_to_report_sftd, 1, 3, integer_packer<uint16_t>(0, 1007)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_sftd_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(report_sftd_meas, 1));
  HANDLE_CODE(bref.unpack(report_rsrp, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_sftd_neigh_meas_present, 1));
      HANDLE_CODE(bref.unpack(drx_sftd_neigh_meas_present, 1));
      unpack_presence_flag(cells_for_which_to_report_sftd, bref);
      if (cells_for_which_to_report_sftd.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cells_for_which_to_report_sftd, bref, 1, 3, integer_packer<uint16_t>(0, 1007)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void report_sftd_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("reportSFTD-Meas", report_sftd_meas);
  j.write_bool("reportRSRP", report_rsrp);
  if (ext) {
    if (report_sftd_neigh_meas_present) {
      j.write_str("reportSFTD-NeighMeas", "true");
    }
    if (drx_sftd_neigh_meas_present) {
      j.write_str("drx-SFTD-NeighMeas", "true");
    }
    if (cells_for_which_to_report_sftd.is_present()) {
      j.start_array("cellsForWhichToReportSFTD");
      for (const auto& e1 : *cells_for_which_to_report_sftd) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// RxTxPeriodical-r17 ::= SEQUENCE
SRSASN_CODE rx_tx_periodical_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rx_tx_report_interv_r17_present, 1));

  if (rx_tx_report_interv_r17_present) {
    HANDLE_CODE(rx_tx_report_interv_r17.pack(bref));
  }
  HANDLE_CODE(report_amount_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rx_tx_periodical_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rx_tx_report_interv_r17_present, 1));

  if (rx_tx_report_interv_r17_present) {
    HANDLE_CODE(rx_tx_report_interv_r17.unpack(bref));
  }
  HANDLE_CODE(report_amount_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void rx_tx_periodical_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rx_tx_report_interv_r17_present) {
    j.write_str("rxTxReportInterval-r17", rx_tx_report_interv_r17.to_string());
  }
  j.write_str("reportAmount-r17", report_amount_r17.to_string());
  j.end_obj();
}

const char* rx_tx_periodical_r17_s::report_amount_r17_opts::to_string() const
{
  static const char* names[] = {"r1", "infinity", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "rx_tx_periodical_r17_s::report_amount_r17_e_");
}
int8_t rx_tx_periodical_r17_s::report_amount_r17_opts::to_number() const
{
  static const int8_t numbers[] = {1, -1};
  return map_enum_number(numbers, 2, value, "rx_tx_periodical_r17_s::report_amount_r17_e_");
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

// SSB-MTC2 ::= SEQUENCE
SRSASN_CODE ssb_mtc2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pci_list.size() > 0, 1));

  if (pci_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_list, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc2_s::unpack(cbit_ref& bref)
{
  bool pci_list_present;
  HANDLE_CODE(bref.unpack(pci_list_present, 1));

  if (pci_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_list, bref, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity.unpack(bref));

  return SRSASN_SUCCESS;
}
void ssb_mtc2_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_list.size() > 0) {
    j.start_array("pci-List");
    for (const auto& e1 : pci_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_str("periodicity", periodicity.to_string());
  j.end_obj();
}

const char* ssb_mtc2_s::periodicity_opts::to_string() const
{
  static const char* names[] = {"sf5", "sf10", "sf20", "sf40", "sf80", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ssb_mtc2_s::periodicity_e_");
}
uint8_t ssb_mtc2_s::periodicity_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80};
  return map_enum_number(numbers, 5, value, "ssb_mtc2_s::periodicity_e_");
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

// T312-r16 ::= ENUMERATED
const char* t312_r16_opts::to_string() const
{
  static const char* names[] = {"ms0", "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms1000"};
  return convert_enum_idx(names, 8, value, "t312_r16_e");
}
uint16_t t312_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 50, 100, 200, 300, 400, 500, 1000};
  return map_enum_number(numbers, 8, value, "t312_r16_e");
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

// UTRA-FDD-Q-OffsetRange-r16 ::= ENUMERATED
const char* utra_fdd_q_offset_range_r16_opts::to_string() const
{
  static const char* names[] = {"dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10",
                                "dB-8",  "dB-6",  "dB-5",  "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",
                                "dB1",   "dB2",   "dB3",   "dB4",   "dB5",   "dB6",   "dB8",   "dB10",
                                "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
  return convert_enum_idx(names, 31, value, "utra_fdd_q_offset_range_r16_e");
}
int8_t utra_fdd_q_offset_range_r16_opts::to_number() const
{
  static const int8_t numbers[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 31, value, "utra_fdd_q_offset_range_r16_e");
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

// MeasGapSharingScheme ::= ENUMERATED
const char* meas_gap_sharing_scheme_opts::to_string() const
{
  static const char* names[] = {"scheme00", "scheme01", "scheme10", "scheme11"};
  return convert_enum_idx(names, 4, value, "meas_gap_sharing_scheme_e");
}
float meas_gap_sharing_scheme_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.1, 1.0, 1.1};
  return map_enum_number(numbers, 4, value, "meas_gap_sharing_scheme_e");
}
const char* meas_gap_sharing_scheme_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.0", "0.1", "1.0", "1.1"};
  return convert_enum_idx(number_strs, 4, value, "meas_gap_sharing_scheme_e");
}

// MeasObjectCLI-r16 ::= SEQUENCE
SRSASN_CODE meas_obj_cli_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(cli_res_cfg_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_cli_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(cli_res_cfg_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_obj_cli_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cli-ResourceConfig-r16");
  cli_res_cfg_r16.to_json(j);
  j.end_obj();
}

// MeasObjectEUTRA ::= SEQUENCE
SRSASN_CODE meas_obj_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cells_to_rem_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(cells_to_add_mod_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(excluded_cells_to_rem_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(excluded_cells_to_add_mod_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_q_offset_range_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw.pack(bref));
  if (cells_to_rem_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_rem_list_eutran, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_add_mod_list_eutran, 1, 32));
  }
  if (excluded_cells_to_rem_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, excluded_cells_to_rem_list_eutran, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (excluded_cells_to_add_mod_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, excluded_cells_to_add_mod_list_eutran, 1, 32));
  }
  HANDLE_CODE(bref.pack(eutra_presence_ant_port1, 1));
  if (eutra_q_offset_range_present) {
    HANDLE_CODE(eutra_q_offset_range.pack(bref));
  }
  HANDLE_CODE(bref.pack(wideband_rsrq_meas, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= associated_meas_gap_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(associated_meas_gap_r17_present, 1));
      if (associated_meas_gap_r17_present) {
        HANDLE_CODE(pack_integer(bref, associated_meas_gap_r17, (uint8_t)1u, (uint8_t)8u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cells_to_rem_list_eutran_present;
  HANDLE_CODE(bref.unpack(cells_to_rem_list_eutran_present, 1));
  bool cells_to_add_mod_list_eutran_present;
  HANDLE_CODE(bref.unpack(cells_to_add_mod_list_eutran_present, 1));
  bool excluded_cells_to_rem_list_eutran_present;
  HANDLE_CODE(bref.unpack(excluded_cells_to_rem_list_eutran_present, 1));
  bool excluded_cells_to_add_mod_list_eutran_present;
  HANDLE_CODE(bref.unpack(excluded_cells_to_add_mod_list_eutran_present, 1));
  HANDLE_CODE(bref.unpack(eutra_q_offset_range_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw.unpack(bref));
  if (cells_to_rem_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_rem_list_eutran, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_add_mod_list_eutran, bref, 1, 32));
  }
  if (excluded_cells_to_rem_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(excluded_cells_to_rem_list_eutran, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (excluded_cells_to_add_mod_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(excluded_cells_to_add_mod_list_eutran, bref, 1, 32));
  }
  HANDLE_CODE(bref.unpack(eutra_presence_ant_port1, 1));
  if (eutra_q_offset_range_present) {
    HANDLE_CODE(eutra_q_offset_range.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(wideband_rsrq_meas, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(associated_meas_gap_r17_present, 1));
      if (associated_meas_gap_r17_present) {
        HANDLE_CODE(unpack_integer(associated_meas_gap_r17, bref, (uint8_t)1u, (uint8_t)8u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_obj_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_str("allowedMeasBandwidth", allowed_meas_bw.to_string());
  if (cells_to_rem_list_eutran.size() > 0) {
    j.start_array("cellsToRemoveListEUTRAN");
    for (const auto& e1 : cells_to_rem_list_eutran) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cells_to_add_mod_list_eutran.size() > 0) {
    j.start_array("cellsToAddModListEUTRAN");
    for (const auto& e1 : cells_to_add_mod_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (excluded_cells_to_rem_list_eutran.size() > 0) {
    j.start_array("excludedCellsToRemoveListEUTRAN");
    for (const auto& e1 : excluded_cells_to_rem_list_eutran) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (excluded_cells_to_add_mod_list_eutran.size() > 0) {
    j.start_array("excludedCellsToAddModListEUTRAN");
    for (const auto& e1 : excluded_cells_to_add_mod_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_bool("eutra-PresenceAntennaPort1", eutra_presence_ant_port1);
  if (eutra_q_offset_range_present) {
    j.write_str("eutra-Q-OffsetRange", eutra_q_offset_range.to_string());
  }
  j.write_bool("widebandRSRQ-Meas", wideband_rsrq_meas);
  if (ext) {
    if (associated_meas_gap_r17_present) {
      j.write_int("associatedMeasGap-r17", associated_meas_gap_r17);
    }
  }
  j.end_obj();
}

// MeasObjectNR ::= SEQUENCE
SRSASN_CODE meas_obj_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ssb_freq_present, 1));
  HANDLE_CODE(bref.pack(ssb_subcarrier_spacing_present, 1));
  HANDLE_CODE(bref.pack(smtc1_present, 1));
  HANDLE_CODE(bref.pack(smtc2_present, 1));
  HANDLE_CODE(bref.pack(ref_freq_csi_rs_present, 1));
  HANDLE_CODE(bref.pack(abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.pack(abs_thresh_csi_rs_consolidation_present, 1));
  HANDLE_CODE(bref.pack(nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.pack(nrof_csi_rs_res_to_average_present, 1));
  HANDLE_CODE(bref.pack(cells_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(cells_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(excluded_cells_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(excluded_cells_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(allowed_cells_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(allowed_cells_to_add_mod_list.size() > 0, 1));

  if (ssb_freq_present) {
    HANDLE_CODE(pack_integer(bref, ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ssb_subcarrier_spacing_present) {
    HANDLE_CODE(ssb_subcarrier_spacing.pack(bref));
  }
  if (smtc1_present) {
    HANDLE_CODE(smtc1.pack(bref));
  }
  if (smtc2_present) {
    HANDLE_CODE(smtc2.pack(bref));
  }
  if (ref_freq_csi_rs_present) {
    HANDLE_CODE(pack_integer(bref, ref_freq_csi_rs, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(ref_sig_cfg.pack(bref));
  if (abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(abs_thresh_ss_blocks_consolidation.pack(bref));
  }
  if (abs_thresh_csi_rs_consolidation_present) {
    HANDLE_CODE(abs_thresh_csi_rs_consolidation.pack(bref));
  }
  if (nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(pack_integer(bref, nrof_ss_blocks_to_average, (uint8_t)2u, (uint8_t)16u));
  }
  if (nrof_csi_rs_res_to_average_present) {
    HANDLE_CODE(pack_integer(bref, nrof_csi_rs_res_to_average, (uint8_t)2u, (uint8_t)16u));
  }
  HANDLE_CODE(pack_integer(bref, quant_cfg_idx, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(offset_mo.pack(bref));
  if (cells_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_rem_list, 1, 32, integer_packer<uint16_t>(0, 1007)));
  }
  if (cells_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_add_mod_list, 1, 32));
  }
  if (excluded_cells_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, excluded_cells_to_rem_list, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (excluded_cells_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, excluded_cells_to_add_mod_list, 1, 8));
  }
  if (allowed_cells_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_cells_to_rem_list, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (allowed_cells_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_cells_to_add_mod_list, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= freq_band_ind_nr_present;
    group_flags[0] |= meas_cycle_scell_present;
    group_flags[1] |= smtc3list_r16.is_present();
    group_flags[1] |= rmtc_cfg_r16.is_present();
    group_flags[1] |= t312_r16.is_present();
    group_flags[2] |= associated_meas_gap_ssb_r17_present;
    group_flags[2] |= associated_meas_gap_csi_rs_r17_present;
    group_flags[2] |= smtc4list_r17.is_present();
    group_flags[2] |= meas_cycle_pscell_r17_present;
    group_flags[2] |= cells_to_add_mod_list_ext_v1710.is_present();
    group_flags[3] |= associated_meas_gap_ssb2_v1720_present;
    group_flags[3] |= associated_meas_gap_csi_rs2_v1720_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(freq_band_ind_nr_present, 1));
      HANDLE_CODE(bref.pack(meas_cycle_scell_present, 1));
      if (freq_band_ind_nr_present) {
        HANDLE_CODE(pack_integer(bref, freq_band_ind_nr, (uint16_t)1u, (uint16_t)1024u));
      }
      if (meas_cycle_scell_present) {
        HANDLE_CODE(meas_cycle_scell.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc3list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(rmtc_cfg_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(t312_r16.is_present(), 1));
      if (smtc3list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *smtc3list_r16, 1, 4));
      }
      if (rmtc_cfg_r16.is_present()) {
        HANDLE_CODE(rmtc_cfg_r16->pack(bref));
      }
      if (t312_r16.is_present()) {
        HANDLE_CODE(t312_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(associated_meas_gap_ssb_r17_present, 1));
      HANDLE_CODE(bref.pack(associated_meas_gap_csi_rs_r17_present, 1));
      HANDLE_CODE(bref.pack(smtc4list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_cycle_pscell_r17_present, 1));
      HANDLE_CODE(bref.pack(cells_to_add_mod_list_ext_v1710.is_present(), 1));
      if (associated_meas_gap_ssb_r17_present) {
        HANDLE_CODE(pack_integer(bref, associated_meas_gap_ssb_r17, (uint8_t)1u, (uint8_t)8u));
      }
      if (associated_meas_gap_csi_rs_r17_present) {
        HANDLE_CODE(pack_integer(bref, associated_meas_gap_csi_rs_r17, (uint8_t)1u, (uint8_t)8u));
      }
      if (smtc4list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *smtc4list_r17, 1, 3));
      }
      if (meas_cycle_pscell_r17_present) {
        HANDLE_CODE(meas_cycle_pscell_r17.pack(bref));
      }
      if (cells_to_add_mod_list_ext_v1710.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cells_to_add_mod_list_ext_v1710, 1, 32));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(associated_meas_gap_ssb2_v1720_present, 1));
      HANDLE_CODE(bref.pack(associated_meas_gap_csi_rs2_v1720_present, 1));
      if (associated_meas_gap_ssb2_v1720_present) {
        HANDLE_CODE(pack_integer(bref, associated_meas_gap_ssb2_v1720, (uint8_t)1u, (uint8_t)8u));
      }
      if (associated_meas_gap_csi_rs2_v1720_present) {
        HANDLE_CODE(pack_integer(bref, associated_meas_gap_csi_rs2_v1720, (uint8_t)1u, (uint8_t)8u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ssb_freq_present, 1));
  HANDLE_CODE(bref.unpack(ssb_subcarrier_spacing_present, 1));
  HANDLE_CODE(bref.unpack(smtc1_present, 1));
  HANDLE_CODE(bref.unpack(smtc2_present, 1));
  HANDLE_CODE(bref.unpack(ref_freq_csi_rs_present, 1));
  HANDLE_CODE(bref.unpack(abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.unpack(abs_thresh_csi_rs_consolidation_present, 1));
  HANDLE_CODE(bref.unpack(nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.unpack(nrof_csi_rs_res_to_average_present, 1));
  bool cells_to_rem_list_present;
  HANDLE_CODE(bref.unpack(cells_to_rem_list_present, 1));
  bool cells_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(cells_to_add_mod_list_present, 1));
  bool excluded_cells_to_rem_list_present;
  HANDLE_CODE(bref.unpack(excluded_cells_to_rem_list_present, 1));
  bool excluded_cells_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(excluded_cells_to_add_mod_list_present, 1));
  bool allowed_cells_to_rem_list_present;
  HANDLE_CODE(bref.unpack(allowed_cells_to_rem_list_present, 1));
  bool allowed_cells_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(allowed_cells_to_add_mod_list_present, 1));

  if (ssb_freq_present) {
    HANDLE_CODE(unpack_integer(ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ssb_subcarrier_spacing_present) {
    HANDLE_CODE(ssb_subcarrier_spacing.unpack(bref));
  }
  if (smtc1_present) {
    HANDLE_CODE(smtc1.unpack(bref));
  }
  if (smtc2_present) {
    HANDLE_CODE(smtc2.unpack(bref));
  }
  if (ref_freq_csi_rs_present) {
    HANDLE_CODE(unpack_integer(ref_freq_csi_rs, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(ref_sig_cfg.unpack(bref));
  if (abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(abs_thresh_ss_blocks_consolidation.unpack(bref));
  }
  if (abs_thresh_csi_rs_consolidation_present) {
    HANDLE_CODE(abs_thresh_csi_rs_consolidation.unpack(bref));
  }
  if (nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(unpack_integer(nrof_ss_blocks_to_average, bref, (uint8_t)2u, (uint8_t)16u));
  }
  if (nrof_csi_rs_res_to_average_present) {
    HANDLE_CODE(unpack_integer(nrof_csi_rs_res_to_average, bref, (uint8_t)2u, (uint8_t)16u));
  }
  HANDLE_CODE(unpack_integer(quant_cfg_idx, bref, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(offset_mo.unpack(bref));
  if (cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_rem_list, bref, 1, 32, integer_packer<uint16_t>(0, 1007)));
  }
  if (cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_add_mod_list, bref, 1, 32));
  }
  if (excluded_cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(excluded_cells_to_rem_list, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (excluded_cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(excluded_cells_to_add_mod_list, bref, 1, 8));
  }
  if (allowed_cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_cells_to_rem_list, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (allowed_cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_cells_to_add_mod_list, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(freq_band_ind_nr_present, 1));
      HANDLE_CODE(bref.unpack(meas_cycle_scell_present, 1));
      if (freq_band_ind_nr_present) {
        HANDLE_CODE(unpack_integer(freq_band_ind_nr, bref, (uint16_t)1u, (uint16_t)1024u));
      }
      if (meas_cycle_scell_present) {
        HANDLE_CODE(meas_cycle_scell.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(smtc3list_r16, bref);
      unpack_presence_flag(rmtc_cfg_r16, bref);
      unpack_presence_flag(t312_r16, bref);
      if (smtc3list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*smtc3list_r16, bref, 1, 4));
      }
      if (rmtc_cfg_r16.is_present()) {
        HANDLE_CODE(rmtc_cfg_r16->unpack(bref));
      }
      if (t312_r16.is_present()) {
        HANDLE_CODE(t312_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(associated_meas_gap_ssb_r17_present, 1));
      HANDLE_CODE(bref.unpack(associated_meas_gap_csi_rs_r17_present, 1));
      unpack_presence_flag(smtc4list_r17, bref);
      HANDLE_CODE(bref.unpack(meas_cycle_pscell_r17_present, 1));
      unpack_presence_flag(cells_to_add_mod_list_ext_v1710, bref);
      if (associated_meas_gap_ssb_r17_present) {
        HANDLE_CODE(unpack_integer(associated_meas_gap_ssb_r17, bref, (uint8_t)1u, (uint8_t)8u));
      }
      if (associated_meas_gap_csi_rs_r17_present) {
        HANDLE_CODE(unpack_integer(associated_meas_gap_csi_rs_r17, bref, (uint8_t)1u, (uint8_t)8u));
      }
      if (smtc4list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*smtc4list_r17, bref, 1, 3));
      }
      if (meas_cycle_pscell_r17_present) {
        HANDLE_CODE(meas_cycle_pscell_r17.unpack(bref));
      }
      if (cells_to_add_mod_list_ext_v1710.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cells_to_add_mod_list_ext_v1710, bref, 1, 32));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(associated_meas_gap_ssb2_v1720_present, 1));
      HANDLE_CODE(bref.unpack(associated_meas_gap_csi_rs2_v1720_present, 1));
      if (associated_meas_gap_ssb2_v1720_present) {
        HANDLE_CODE(unpack_integer(associated_meas_gap_ssb2_v1720, bref, (uint8_t)1u, (uint8_t)8u));
      }
      if (associated_meas_gap_csi_rs2_v1720_present) {
        HANDLE_CODE(unpack_integer(associated_meas_gap_csi_rs2_v1720, bref, (uint8_t)1u, (uint8_t)8u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_obj_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_freq_present) {
    j.write_int("ssbFrequency", ssb_freq);
  }
  if (ssb_subcarrier_spacing_present) {
    j.write_str("ssbSubcarrierSpacing", ssb_subcarrier_spacing.to_string());
  }
  if (smtc1_present) {
    j.write_fieldname("smtc1");
    smtc1.to_json(j);
  }
  if (smtc2_present) {
    j.write_fieldname("smtc2");
    smtc2.to_json(j);
  }
  if (ref_freq_csi_rs_present) {
    j.write_int("refFreqCSI-RS", ref_freq_csi_rs);
  }
  j.write_fieldname("referenceSignalConfig");
  ref_sig_cfg.to_json(j);
  if (abs_thresh_ss_blocks_consolidation_present) {
    j.write_fieldname("absThreshSS-BlocksConsolidation");
    abs_thresh_ss_blocks_consolidation.to_json(j);
  }
  if (abs_thresh_csi_rs_consolidation_present) {
    j.write_fieldname("absThreshCSI-RS-Consolidation");
    abs_thresh_csi_rs_consolidation.to_json(j);
  }
  if (nrof_ss_blocks_to_average_present) {
    j.write_int("nrofSS-BlocksToAverage", nrof_ss_blocks_to_average);
  }
  if (nrof_csi_rs_res_to_average_present) {
    j.write_int("nrofCSI-RS-ResourcesToAverage", nrof_csi_rs_res_to_average);
  }
  j.write_int("quantityConfigIndex", quant_cfg_idx);
  j.write_fieldname("offsetMO");
  offset_mo.to_json(j);
  if (cells_to_rem_list.size() > 0) {
    j.start_array("cellsToRemoveList");
    for (const auto& e1 : cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cells_to_add_mod_list.size() > 0) {
    j.start_array("cellsToAddModList");
    for (const auto& e1 : cells_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (excluded_cells_to_rem_list.size() > 0) {
    j.start_array("excludedCellsToRemoveList");
    for (const auto& e1 : excluded_cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (excluded_cells_to_add_mod_list.size() > 0) {
    j.start_array("excludedCellsToAddModList");
    for (const auto& e1 : excluded_cells_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (allowed_cells_to_rem_list.size() > 0) {
    j.start_array("allowedCellsToRemoveList");
    for (const auto& e1 : allowed_cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (allowed_cells_to_add_mod_list.size() > 0) {
    j.start_array("allowedCellsToAddModList");
    for (const auto& e1 : allowed_cells_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (freq_band_ind_nr_present) {
      j.write_int("freqBandIndicatorNR", freq_band_ind_nr);
    }
    if (meas_cycle_scell_present) {
      j.write_str("measCycleSCell", meas_cycle_scell.to_string());
    }
    if (smtc3list_r16.is_present()) {
      j.start_array("smtc3list-r16");
      for (const auto& e1 : *smtc3list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (rmtc_cfg_r16.is_present()) {
      j.write_fieldname("rmtc-Config-r16");
      rmtc_cfg_r16->to_json(j);
    }
    if (t312_r16.is_present()) {
      j.write_fieldname("t312-r16");
      t312_r16->to_json(j);
    }
    if (associated_meas_gap_ssb_r17_present) {
      j.write_int("associatedMeasGapSSB-r17", associated_meas_gap_ssb_r17);
    }
    if (associated_meas_gap_csi_rs_r17_present) {
      j.write_int("associatedMeasGapCSIRS-r17", associated_meas_gap_csi_rs_r17);
    }
    if (smtc4list_r17.is_present()) {
      j.start_array("smtc4list-r17");
      for (const auto& e1 : *smtc4list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_cycle_pscell_r17_present) {
      j.write_str("measCyclePSCell-r17", meas_cycle_pscell_r17.to_string());
    }
    if (cells_to_add_mod_list_ext_v1710.is_present()) {
      j.start_array("cellsToAddModListExt-v1710");
      for (const auto& e1 : *cells_to_add_mod_list_ext_v1710) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (associated_meas_gap_ssb2_v1720_present) {
      j.write_int("associatedMeasGapSSB2-v1720", associated_meas_gap_ssb2_v1720);
    }
    if (associated_meas_gap_csi_rs2_v1720_present) {
      j.write_int("associatedMeasGapCSIRS2-v1720", associated_meas_gap_csi_rs2_v1720);
    }
  }
  j.end_obj();
}

const char* meas_obj_nr_s::meas_cycle_scell_opts::to_string() const
{
  static const char* names[] = {"sf160", "sf256", "sf320", "sf512", "sf640", "sf1024", "sf1280"};
  return convert_enum_idx(names, 7, value, "meas_obj_nr_s::meas_cycle_scell_e_");
}
uint16_t meas_obj_nr_s::meas_cycle_scell_opts::to_number() const
{
  static const uint16_t numbers[] = {160, 256, 320, 512, 640, 1024, 1280};
  return map_enum_number(numbers, 7, value, "meas_obj_nr_s::meas_cycle_scell_e_");
}

const char* meas_obj_nr_s::meas_cycle_pscell_r17_opts::to_string() const
{
  static const char* names[] = {"ms160", "ms256", "ms320", "ms512", "ms640", "ms1024", "ms1280", "spare1"};
  return convert_enum_idx(names, 8, value, "meas_obj_nr_s::meas_cycle_pscell_r17_e_");
}
uint16_t meas_obj_nr_s::meas_cycle_pscell_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {160, 256, 320, 512, 640, 1024, 1280};
  return map_enum_number(numbers, 7, value, "meas_obj_nr_s::meas_cycle_pscell_r17_e_");
}

// MeasObjectNR-SL-r16 ::= SEQUENCE
SRSASN_CODE meas_obj_nr_sl_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_pool_meas_to_rem_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(tx_pool_meas_to_add_mod_list_r16.size() > 0, 1));

  if (tx_pool_meas_to_rem_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tx_pool_meas_to_rem_list_r16, 1, 8, integer_packer<uint8_t>(1, 16)));
  }
  if (tx_pool_meas_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tx_pool_meas_to_add_mod_list_r16, 1, 8, integer_packer<uint8_t>(1, 16)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_nr_sl_r16_s::unpack(cbit_ref& bref)
{
  bool tx_pool_meas_to_rem_list_r16_present;
  HANDLE_CODE(bref.unpack(tx_pool_meas_to_rem_list_r16_present, 1));
  bool tx_pool_meas_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(tx_pool_meas_to_add_mod_list_r16_present, 1));

  if (tx_pool_meas_to_rem_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tx_pool_meas_to_rem_list_r16, bref, 1, 8, integer_packer<uint8_t>(1, 16)));
  }
  if (tx_pool_meas_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tx_pool_meas_to_add_mod_list_r16, bref, 1, 8, integer_packer<uint8_t>(1, 16)));
  }

  return SRSASN_SUCCESS;
}
void meas_obj_nr_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_pool_meas_to_rem_list_r16.size() > 0) {
    j.start_array("tx-PoolMeasToRemoveList-r16");
    for (const auto& e1 : tx_pool_meas_to_rem_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (tx_pool_meas_to_add_mod_list_r16.size() > 0) {
    j.start_array("tx-PoolMeasToAddModList-r16");
    for (const auto& e1 : tx_pool_meas_to_add_mod_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// MeasObjectRxTxDiff-r17 ::= SEQUENCE
SRSASN_CODE meas_obj_rx_tx_diff_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_ref_r17_present, 1));

  if (dl_ref_r17_present) {
    HANDLE_CODE(dl_ref_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_rx_tx_diff_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_ref_r17_present, 1));

  if (dl_ref_r17_present) {
    HANDLE_CODE(dl_ref_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_obj_rx_tx_diff_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_ref_r17_present) {
    j.write_fieldname("dl-Ref-r17");
    dl_ref_r17.to_json(j);
  }
  j.end_obj();
}

void meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::set(types::options e)
{
  type_ = e;
}
void meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::set_prs_ref_r17()
{
  set(types::prs_ref_r17);
}
void meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::set_csi_rs_ref_r17()
{
  set(types::csi_rs_ref_r17);
}
void meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  return SRSASN_SUCCESS;
}

// MeasObjectUTRA-FDD-r16 ::= SEQUENCE
SRSASN_CODE meas_obj_utra_fdd_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(utra_fdd_q_offset_range_r16_present, 1));
  HANDLE_CODE(bref.pack(cells_to_rem_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(cells_to_add_mod_list_r16.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint16_t)0u, (uint16_t)16383u));
  if (utra_fdd_q_offset_range_r16_present) {
    HANDLE_CODE(utra_fdd_q_offset_range_r16.pack(bref));
  }
  if (cells_to_rem_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_rem_list_r16, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_add_mod_list_r16, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_utra_fdd_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(utra_fdd_q_offset_range_r16_present, 1));
  bool cells_to_rem_list_r16_present;
  HANDLE_CODE(bref.unpack(cells_to_rem_list_r16_present, 1));
  bool cells_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(cells_to_add_mod_list_r16_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint16_t)0u, (uint16_t)16383u));
  if (utra_fdd_q_offset_range_r16_present) {
    HANDLE_CODE(utra_fdd_q_offset_range_r16.unpack(bref));
  }
  if (cells_to_rem_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_rem_list_r16, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_add_mod_list_r16, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void meas_obj_utra_fdd_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  if (utra_fdd_q_offset_range_r16_present) {
    j.write_str("utra-FDD-Q-OffsetRange-r16", utra_fdd_q_offset_range_r16.to_string());
  }
  if (cells_to_rem_list_r16.size() > 0) {
    j.start_array("cellsToRemoveList-r16");
    for (const auto& e1 : cells_to_rem_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cells_to_add_mod_list_r16.size() > 0) {
    j.start_array("cellsToAddModList-r16");
    for (const auto& e1 : cells_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
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

// PosGapConfig-r17 ::= SEQUENCE
SRSASN_CODE pos_gap_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, meas_pos_pre_cfg_gap_id_r17, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, gap_offset_r17, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(mgl_r17.pack(bref));
  HANDLE_CODE(mgrp_r17.pack(bref));
  HANDLE_CODE(mgta_r17.pack(bref));
  HANDLE_CODE(gap_type_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_gap_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(meas_pos_pre_cfg_gap_id_r17, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(gap_offset_r17, bref, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(mgl_r17.unpack(bref));
  HANDLE_CODE(mgrp_r17.unpack(bref));
  HANDLE_CODE(mgta_r17.unpack(bref));
  HANDLE_CODE(gap_type_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void pos_gap_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measPosPreConfigGapId-r17", meas_pos_pre_cfg_gap_id_r17);
  j.write_int("gapOffset-r17", gap_offset_r17);
  j.write_str("mgl-r17", mgl_r17.to_string());
  j.write_str("mgrp-r17", mgrp_r17.to_string());
  j.write_str("mgta-r17", mgta_r17.to_string());
  j.write_str("gapType-r17", gap_type_r17.to_string());
  j.end_obj();
}

const char* pos_gap_cfg_r17_s::mgl_r17_opts::to_string() const
{
  static const char* names[] = {"ms1dot5", "ms3", "ms3dot5", "ms4", "ms5dot5", "ms6", "ms10", "ms20"};
  return convert_enum_idx(names, 8, value, "pos_gap_cfg_r17_s::mgl_r17_e_");
}
float pos_gap_cfg_r17_s::mgl_r17_opts::to_number() const
{
  static const float numbers[] = {1.5, 3.0, 3.5, 4.0, 5.5, 6.0, 10.0, 20.0};
  return map_enum_number(numbers, 8, value, "pos_gap_cfg_r17_s::mgl_r17_e_");
}
const char* pos_gap_cfg_r17_s::mgl_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"1.5", "3", "3.5", "4", "5.5", "6", "10", "20"};
  return convert_enum_idx(number_strs, 8, value, "pos_gap_cfg_r17_s::mgl_r17_e_");
}

const char* pos_gap_cfg_r17_s::mgrp_r17_opts::to_string() const
{
  static const char* names[] = {"ms20", "ms40", "ms80", "ms160"};
  return convert_enum_idx(names, 4, value, "pos_gap_cfg_r17_s::mgrp_r17_e_");
}
uint8_t pos_gap_cfg_r17_s::mgrp_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 40, 80, 160};
  return map_enum_number(numbers, 4, value, "pos_gap_cfg_r17_s::mgrp_r17_e_");
}

const char* pos_gap_cfg_r17_s::mgta_r17_opts::to_string() const
{
  static const char* names[] = {"ms0", "ms0dot25", "ms0dot5"};
  return convert_enum_idx(names, 3, value, "pos_gap_cfg_r17_s::mgta_r17_e_");
}
float pos_gap_cfg_r17_s::mgta_r17_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.25, 0.5};
  return map_enum_number(numbers, 3, value, "pos_gap_cfg_r17_s::mgta_r17_e_");
}
const char* pos_gap_cfg_r17_s::mgta_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.25", "0.5"};
  return convert_enum_idx(number_strs, 3, value, "pos_gap_cfg_r17_s::mgta_r17_e_");
}

const char* pos_gap_cfg_r17_s::gap_type_r17_opts::to_string() const
{
  static const char* names[] = {"perUE", "perFR1", "perFR2"};
  return convert_enum_idx(names, 3, value, "pos_gap_cfg_r17_s::gap_type_r17_e_");
}
uint8_t pos_gap_cfg_r17_s::gap_type_r17_opts::to_number() const
{
  switch (value) {
    case per_fr1:
      return 1;
    case per_fr2:
      return 2;
    default:
      invalid_enum_number(value, "pos_gap_cfg_r17_s::gap_type_r17_e_");
  }
  return 0;
}

// QuantityConfigRS ::= SEQUENCE
SRSASN_CODE quant_cfg_rs_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ssb_filt_cfg.pack(bref));
  HANDLE_CODE(csi_rs_filt_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_rs_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ssb_filt_cfg.unpack(bref));
  HANDLE_CODE(csi_rs_filt_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void quant_cfg_rs_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ssb-FilterConfig");
  ssb_filt_cfg.to_json(j);
  j.write_fieldname("csi-RS-FilterConfig");
  csi_rs_filt_cfg.to_json(j);
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

// ReportConfigInterRAT ::= SEQUENCE
SRSASN_CODE report_cfg_inter_rat_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(report_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_inter_rat_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(report_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void report_cfg_inter_rat_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("reportType");
  report_type.to_json(j);
  j.end_obj();
}

void report_cfg_inter_rat_s::report_type_c_::destroy_()
{
  switch (type_) {
    case types::periodical:
      c.destroy<periodical_report_cfg_inter_rat_s>();
      break;
    case types::event_triggered:
      c.destroy<event_trigger_cfg_inter_rat_s>();
      break;
    case types::report_cgi:
      c.destroy<report_cgi_eutra_s>();
      break;
    case types::report_sftd:
      c.destroy<report_sftd_eutra_s>();
      break;
    default:
      break;
  }
}
void report_cfg_inter_rat_s::report_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::periodical:
      c.init<periodical_report_cfg_inter_rat_s>();
      break;
    case types::event_triggered:
      c.init<event_trigger_cfg_inter_rat_s>();
      break;
    case types::report_cgi:
      c.init<report_cgi_eutra_s>();
      break;
    case types::report_sftd:
      c.init<report_sftd_eutra_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::report_type_c_");
  }
}
report_cfg_inter_rat_s::report_type_c_::report_type_c_(const report_cfg_inter_rat_s::report_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::periodical:
      c.init(other.c.get<periodical_report_cfg_inter_rat_s>());
      break;
    case types::event_triggered:
      c.init(other.c.get<event_trigger_cfg_inter_rat_s>());
      break;
    case types::report_cgi:
      c.init(other.c.get<report_cgi_eutra_s>());
      break;
    case types::report_sftd:
      c.init(other.c.get<report_sftd_eutra_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::report_type_c_");
  }
}
report_cfg_inter_rat_s::report_type_c_&
report_cfg_inter_rat_s::report_type_c_::operator=(const report_cfg_inter_rat_s::report_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::periodical:
      c.set(other.c.get<periodical_report_cfg_inter_rat_s>());
      break;
    case types::event_triggered:
      c.set(other.c.get<event_trigger_cfg_inter_rat_s>());
      break;
    case types::report_cgi:
      c.set(other.c.get<report_cgi_eutra_s>());
      break;
    case types::report_sftd:
      c.set(other.c.get<report_sftd_eutra_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::report_type_c_");
  }

  return *this;
}
periodical_report_cfg_inter_rat_s& report_cfg_inter_rat_s::report_type_c_::set_periodical()
{
  set(types::periodical);
  return c.get<periodical_report_cfg_inter_rat_s>();
}
event_trigger_cfg_inter_rat_s& report_cfg_inter_rat_s::report_type_c_::set_event_triggered()
{
  set(types::event_triggered);
  return c.get<event_trigger_cfg_inter_rat_s>();
}
report_cgi_eutra_s& report_cfg_inter_rat_s::report_type_c_::set_report_cgi()
{
  set(types::report_cgi);
  return c.get<report_cgi_eutra_s>();
}
report_sftd_eutra_s& report_cfg_inter_rat_s::report_type_c_::set_report_sftd()
{
  set(types::report_sftd);
  return c.get<report_sftd_eutra_s>();
}
void report_cfg_inter_rat_s::report_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::periodical:
      j.write_fieldname("periodical");
      c.get<periodical_report_cfg_inter_rat_s>().to_json(j);
      break;
    case types::event_triggered:
      j.write_fieldname("eventTriggered");
      c.get<event_trigger_cfg_inter_rat_s>().to_json(j);
      break;
    case types::report_cgi:
      j.write_fieldname("reportCGI");
      c.get<report_cgi_eutra_s>().to_json(j);
      break;
    case types::report_sftd:
      j.write_fieldname("reportSFTD");
      c.get<report_sftd_eutra_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::report_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_inter_rat_s::report_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::periodical:
      HANDLE_CODE(c.get<periodical_report_cfg_inter_rat_s>().pack(bref));
      break;
    case types::event_triggered:
      HANDLE_CODE(c.get<event_trigger_cfg_inter_rat_s>().pack(bref));
      break;
    case types::report_cgi:
      HANDLE_CODE(c.get<report_cgi_eutra_s>().pack(bref));
      break;
    case types::report_sftd: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_sftd_eutra_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::report_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_inter_rat_s::report_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::periodical:
      HANDLE_CODE(c.get<periodical_report_cfg_inter_rat_s>().unpack(bref));
      break;
    case types::event_triggered:
      HANDLE_CODE(c.get<event_trigger_cfg_inter_rat_s>().unpack(bref));
      break;
    case types::report_cgi:
      HANDLE_CODE(c.get<report_cgi_eutra_s>().unpack(bref));
      break;
    case types::report_sftd: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_sftd_eutra_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_inter_rat_s::report_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ReportConfigNR ::= SEQUENCE
SRSASN_CODE report_cfg_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(report_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(report_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void report_cfg_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("reportType");
  report_type.to_json(j);
  j.end_obj();
}

void report_cfg_nr_s::report_type_c_::destroy_()
{
  switch (type_) {
    case types::periodical:
      c.destroy<periodical_report_cfg_s>();
      break;
    case types::event_triggered:
      c.destroy<event_trigger_cfg_s>();
      break;
    case types::report_cgi:
      c.destroy<report_cgi_s>();
      break;
    case types::report_sftd:
      c.destroy<report_sftd_nr_s>();
      break;
    case types::cond_trigger_cfg_r16:
      c.destroy<cond_trigger_cfg_r16_s>();
      break;
    case types::cli_periodical_r16:
      c.destroy<cli_periodical_report_cfg_r16_s>();
      break;
    case types::cli_event_triggered_r16:
      c.destroy<cli_event_trigger_cfg_r16_s>();
      break;
    case types::rx_tx_periodical_r17:
      c.destroy<rx_tx_periodical_r17_s>();
      break;
    default:
      break;
  }
}
void report_cfg_nr_s::report_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::periodical:
      c.init<periodical_report_cfg_s>();
      break;
    case types::event_triggered:
      c.init<event_trigger_cfg_s>();
      break;
    case types::report_cgi:
      c.init<report_cgi_s>();
      break;
    case types::report_sftd:
      c.init<report_sftd_nr_s>();
      break;
    case types::cond_trigger_cfg_r16:
      c.init<cond_trigger_cfg_r16_s>();
      break;
    case types::cli_periodical_r16:
      c.init<cli_periodical_report_cfg_r16_s>();
      break;
    case types::cli_event_triggered_r16:
      c.init<cli_event_trigger_cfg_r16_s>();
      break;
    case types::rx_tx_periodical_r17:
      c.init<rx_tx_periodical_r17_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_s::report_type_c_");
  }
}
report_cfg_nr_s::report_type_c_::report_type_c_(const report_cfg_nr_s::report_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::periodical:
      c.init(other.c.get<periodical_report_cfg_s>());
      break;
    case types::event_triggered:
      c.init(other.c.get<event_trigger_cfg_s>());
      break;
    case types::report_cgi:
      c.init(other.c.get<report_cgi_s>());
      break;
    case types::report_sftd:
      c.init(other.c.get<report_sftd_nr_s>());
      break;
    case types::cond_trigger_cfg_r16:
      c.init(other.c.get<cond_trigger_cfg_r16_s>());
      break;
    case types::cli_periodical_r16:
      c.init(other.c.get<cli_periodical_report_cfg_r16_s>());
      break;
    case types::cli_event_triggered_r16:
      c.init(other.c.get<cli_event_trigger_cfg_r16_s>());
      break;
    case types::rx_tx_periodical_r17:
      c.init(other.c.get<rx_tx_periodical_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_s::report_type_c_");
  }
}
report_cfg_nr_s::report_type_c_&
report_cfg_nr_s::report_type_c_::operator=(const report_cfg_nr_s::report_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::periodical:
      c.set(other.c.get<periodical_report_cfg_s>());
      break;
    case types::event_triggered:
      c.set(other.c.get<event_trigger_cfg_s>());
      break;
    case types::report_cgi:
      c.set(other.c.get<report_cgi_s>());
      break;
    case types::report_sftd:
      c.set(other.c.get<report_sftd_nr_s>());
      break;
    case types::cond_trigger_cfg_r16:
      c.set(other.c.get<cond_trigger_cfg_r16_s>());
      break;
    case types::cli_periodical_r16:
      c.set(other.c.get<cli_periodical_report_cfg_r16_s>());
      break;
    case types::cli_event_triggered_r16:
      c.set(other.c.get<cli_event_trigger_cfg_r16_s>());
      break;
    case types::rx_tx_periodical_r17:
      c.set(other.c.get<rx_tx_periodical_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_s::report_type_c_");
  }

  return *this;
}
periodical_report_cfg_s& report_cfg_nr_s::report_type_c_::set_periodical()
{
  set(types::periodical);
  return c.get<periodical_report_cfg_s>();
}
event_trigger_cfg_s& report_cfg_nr_s::report_type_c_::set_event_triggered()
{
  set(types::event_triggered);
  return c.get<event_trigger_cfg_s>();
}
report_cgi_s& report_cfg_nr_s::report_type_c_::set_report_cgi()
{
  set(types::report_cgi);
  return c.get<report_cgi_s>();
}
report_sftd_nr_s& report_cfg_nr_s::report_type_c_::set_report_sftd()
{
  set(types::report_sftd);
  return c.get<report_sftd_nr_s>();
}
cond_trigger_cfg_r16_s& report_cfg_nr_s::report_type_c_::set_cond_trigger_cfg_r16()
{
  set(types::cond_trigger_cfg_r16);
  return c.get<cond_trigger_cfg_r16_s>();
}
cli_periodical_report_cfg_r16_s& report_cfg_nr_s::report_type_c_::set_cli_periodical_r16()
{
  set(types::cli_periodical_r16);
  return c.get<cli_periodical_report_cfg_r16_s>();
}
cli_event_trigger_cfg_r16_s& report_cfg_nr_s::report_type_c_::set_cli_event_triggered_r16()
{
  set(types::cli_event_triggered_r16);
  return c.get<cli_event_trigger_cfg_r16_s>();
}
rx_tx_periodical_r17_s& report_cfg_nr_s::report_type_c_::set_rx_tx_periodical_r17()
{
  set(types::rx_tx_periodical_r17);
  return c.get<rx_tx_periodical_r17_s>();
}
void report_cfg_nr_s::report_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::periodical:
      j.write_fieldname("periodical");
      c.get<periodical_report_cfg_s>().to_json(j);
      break;
    case types::event_triggered:
      j.write_fieldname("eventTriggered");
      c.get<event_trigger_cfg_s>().to_json(j);
      break;
    case types::report_cgi:
      j.write_fieldname("reportCGI");
      c.get<report_cgi_s>().to_json(j);
      break;
    case types::report_sftd:
      j.write_fieldname("reportSFTD");
      c.get<report_sftd_nr_s>().to_json(j);
      break;
    case types::cond_trigger_cfg_r16:
      j.write_fieldname("condTriggerConfig-r16");
      c.get<cond_trigger_cfg_r16_s>().to_json(j);
      break;
    case types::cli_periodical_r16:
      j.write_fieldname("cli-Periodical-r16");
      c.get<cli_periodical_report_cfg_r16_s>().to_json(j);
      break;
    case types::cli_event_triggered_r16:
      j.write_fieldname("cli-EventTriggered-r16");
      c.get<cli_event_trigger_cfg_r16_s>().to_json(j);
      break;
    case types::rx_tx_periodical_r17:
      j.write_fieldname("rxTxPeriodical-r17");
      c.get<rx_tx_periodical_r17_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_s::report_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_nr_s::report_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::periodical:
      HANDLE_CODE(c.get<periodical_report_cfg_s>().pack(bref));
      break;
    case types::event_triggered:
      HANDLE_CODE(c.get<event_trigger_cfg_s>().pack(bref));
      break;
    case types::report_cgi: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_cgi_s>().pack(bref));
    } break;
    case types::report_sftd: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_sftd_nr_s>().pack(bref));
    } break;
    case types::cond_trigger_cfg_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cond_trigger_cfg_r16_s>().pack(bref));
    } break;
    case types::cli_periodical_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cli_periodical_report_cfg_r16_s>().pack(bref));
    } break;
    case types::cli_event_triggered_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cli_event_trigger_cfg_r16_s>().pack(bref));
    } break;
    case types::rx_tx_periodical_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<rx_tx_periodical_r17_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_s::report_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_nr_s::report_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::periodical:
      HANDLE_CODE(c.get<periodical_report_cfg_s>().unpack(bref));
      break;
    case types::event_triggered:
      HANDLE_CODE(c.get<event_trigger_cfg_s>().unpack(bref));
      break;
    case types::report_cgi: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_cgi_s>().unpack(bref));
    } break;
    case types::report_sftd: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_sftd_nr_s>().unpack(bref));
    } break;
    case types::cond_trigger_cfg_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cond_trigger_cfg_r16_s>().unpack(bref));
    } break;
    case types::cli_periodical_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cli_periodical_report_cfg_r16_s>().unpack(bref));
    } break;
    case types::cli_event_triggered_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<cli_event_trigger_cfg_r16_s>().unpack(bref));
    } break;
    case types::rx_tx_periodical_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<rx_tx_periodical_r17_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_s::report_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ReportConfigNR-SL-r16 ::= SEQUENCE
SRSASN_CODE report_cfg_nr_sl_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(report_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_nr_sl_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(report_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void report_cfg_nr_sl_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("reportType-r16");
  report_type_r16.to_json(j);
  j.end_obj();
}

void report_cfg_nr_sl_r16_s::report_type_r16_c_::destroy_()
{
  switch (type_) {
    case types::periodical_r16:
      c.destroy<periodical_report_cfg_nr_sl_r16_s>();
      break;
    case types::event_triggered_r16:
      c.destroy<event_trigger_cfg_nr_sl_r16_s>();
      break;
    default:
      break;
  }
}
void report_cfg_nr_sl_r16_s::report_type_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::periodical_r16:
      c.init<periodical_report_cfg_nr_sl_r16_s>();
      break;
    case types::event_triggered_r16:
      c.init<event_trigger_cfg_nr_sl_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_sl_r16_s::report_type_r16_c_");
  }
}
report_cfg_nr_sl_r16_s::report_type_r16_c_::report_type_r16_c_(const report_cfg_nr_sl_r16_s::report_type_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::periodical_r16:
      c.init(other.c.get<periodical_report_cfg_nr_sl_r16_s>());
      break;
    case types::event_triggered_r16:
      c.init(other.c.get<event_trigger_cfg_nr_sl_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_sl_r16_s::report_type_r16_c_");
  }
}
report_cfg_nr_sl_r16_s::report_type_r16_c_&
report_cfg_nr_sl_r16_s::report_type_r16_c_::operator=(const report_cfg_nr_sl_r16_s::report_type_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::periodical_r16:
      c.set(other.c.get<periodical_report_cfg_nr_sl_r16_s>());
      break;
    case types::event_triggered_r16:
      c.set(other.c.get<event_trigger_cfg_nr_sl_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_sl_r16_s::report_type_r16_c_");
  }

  return *this;
}
periodical_report_cfg_nr_sl_r16_s& report_cfg_nr_sl_r16_s::report_type_r16_c_::set_periodical_r16()
{
  set(types::periodical_r16);
  return c.get<periodical_report_cfg_nr_sl_r16_s>();
}
event_trigger_cfg_nr_sl_r16_s& report_cfg_nr_sl_r16_s::report_type_r16_c_::set_event_triggered_r16()
{
  set(types::event_triggered_r16);
  return c.get<event_trigger_cfg_nr_sl_r16_s>();
}
void report_cfg_nr_sl_r16_s::report_type_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::periodical_r16:
      j.write_fieldname("periodical-r16");
      c.get<periodical_report_cfg_nr_sl_r16_s>().to_json(j);
      break;
    case types::event_triggered_r16:
      j.write_fieldname("eventTriggered-r16");
      c.get<event_trigger_cfg_nr_sl_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_sl_r16_s::report_type_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_nr_sl_r16_s::report_type_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::periodical_r16:
      HANDLE_CODE(c.get<periodical_report_cfg_nr_sl_r16_s>().pack(bref));
      break;
    case types::event_triggered_r16:
      HANDLE_CODE(c.get<event_trigger_cfg_nr_sl_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_sl_r16_s::report_type_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_nr_sl_r16_s::report_type_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::periodical_r16:
      HANDLE_CODE(c.get<periodical_report_cfg_nr_sl_r16_s>().unpack(bref));
      break;
    case types::event_triggered_r16:
      HANDLE_CODE(c.get<event_trigger_cfg_nr_sl_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_sl_r16_s::report_type_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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

// FilterConfigCLI-r16 ::= SEQUENCE
SRSASN_CODE filt_cfg_cli_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_srs_rsrp_r16_present, 1));
  HANDLE_CODE(bref.pack(filt_coef_cli_rssi_r16_present, 1));

  if (filt_coef_srs_rsrp_r16_present) {
    HANDLE_CODE(filt_coef_srs_rsrp_r16.pack(bref));
  }
  if (filt_coef_cli_rssi_r16_present) {
    HANDLE_CODE(filt_coef_cli_rssi_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE filt_cfg_cli_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_srs_rsrp_r16_present, 1));
  HANDLE_CODE(bref.unpack(filt_coef_cli_rssi_r16_present, 1));

  if (filt_coef_srs_rsrp_r16_present) {
    HANDLE_CODE(filt_coef_srs_rsrp_r16.unpack(bref));
  }
  if (filt_coef_cli_rssi_r16_present) {
    HANDLE_CODE(filt_coef_cli_rssi_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void filt_cfg_cli_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coef_srs_rsrp_r16_present) {
    j.write_str("filterCoefficientSRS-RSRP-r16", filt_coef_srs_rsrp_r16.to_string());
  }
  if (filt_coef_cli_rssi_r16_present) {
    j.write_str("filterCoefficientCLI-RSSI-r16", filt_coef_cli_rssi_r16.to_string());
  }
  j.end_obj();
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

// GapConfig ::= SEQUENCE
SRSASN_CODE gap_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, gap_offset, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(mgl.pack(bref));
  HANDLE_CODE(mgrp.pack(bref));
  HANDLE_CODE(mgta.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ref_serv_cell_ind_present;
    group_flags[1] |= ref_fr2_serv_cell_async_ca_r16_present;
    group_flags[1] |= mgl_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ref_serv_cell_ind_present, 1));
      if (ref_serv_cell_ind_present) {
        HANDLE_CODE(ref_serv_cell_ind.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ref_fr2_serv_cell_async_ca_r16_present, 1));
      HANDLE_CODE(bref.pack(mgl_r16_present, 1));
      if (ref_fr2_serv_cell_async_ca_r16_present) {
        HANDLE_CODE(pack_integer(bref, ref_fr2_serv_cell_async_ca_r16, (uint8_t)0u, (uint8_t)31u));
      }
      if (mgl_r16_present) {
        HANDLE_CODE(mgl_r16.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gap_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(gap_offset, bref, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(mgl.unpack(bref));
  HANDLE_CODE(mgrp.unpack(bref));
  HANDLE_CODE(mgta.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ref_serv_cell_ind_present, 1));
      if (ref_serv_cell_ind_present) {
        HANDLE_CODE(ref_serv_cell_ind.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ref_fr2_serv_cell_async_ca_r16_present, 1));
      HANDLE_CODE(bref.unpack(mgl_r16_present, 1));
      if (ref_fr2_serv_cell_async_ca_r16_present) {
        HANDLE_CODE(unpack_integer(ref_fr2_serv_cell_async_ca_r16, bref, (uint8_t)0u, (uint8_t)31u));
      }
      if (mgl_r16_present) {
        HANDLE_CODE(mgl_r16.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void gap_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gapOffset", gap_offset);
  j.write_str("mgl", mgl.to_string());
  j.write_str("mgrp", mgrp.to_string());
  j.write_str("mgta", mgta.to_string());
  if (ext) {
    if (ref_serv_cell_ind_present) {
      j.write_str("refServCellIndicator", ref_serv_cell_ind.to_string());
    }
    if (ref_fr2_serv_cell_async_ca_r16_present) {
      j.write_int("refFR2ServCellAsyncCA-r16", ref_fr2_serv_cell_async_ca_r16);
    }
    if (mgl_r16_present) {
      j.write_str("mgl-r16", mgl_r16.to_string());
    }
  }
  j.end_obj();
}

const char* gap_cfg_s::mgl_opts::to_string() const
{
  static const char* names[] = {"ms1dot5", "ms3", "ms3dot5", "ms4", "ms5dot5", "ms6"};
  return convert_enum_idx(names, 6, value, "gap_cfg_s::mgl_e_");
}
float gap_cfg_s::mgl_opts::to_number() const
{
  static const float numbers[] = {1.5, 3.0, 3.5, 4.0, 5.5, 6.0};
  return map_enum_number(numbers, 6, value, "gap_cfg_s::mgl_e_");
}
const char* gap_cfg_s::mgl_opts::to_number_string() const
{
  static const char* number_strs[] = {"1.5", "3", "3.5", "4", "5.5", "6"};
  return convert_enum_idx(number_strs, 6, value, "gap_cfg_s::mgl_e_");
}

const char* gap_cfg_s::mgrp_opts::to_string() const
{
  static const char* names[] = {"ms20", "ms40", "ms80", "ms160"};
  return convert_enum_idx(names, 4, value, "gap_cfg_s::mgrp_e_");
}
uint8_t gap_cfg_s::mgrp_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 40, 80, 160};
  return map_enum_number(numbers, 4, value, "gap_cfg_s::mgrp_e_");
}

const char* gap_cfg_s::mgta_opts::to_string() const
{
  static const char* names[] = {"ms0", "ms0dot25", "ms0dot5"};
  return convert_enum_idx(names, 3, value, "gap_cfg_s::mgta_e_");
}
float gap_cfg_s::mgta_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.25, 0.5};
  return map_enum_number(numbers, 3, value, "gap_cfg_s::mgta_e_");
}
const char* gap_cfg_s::mgta_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.25", "0.5"};
  return convert_enum_idx(number_strs, 3, value, "gap_cfg_s::mgta_e_");
}

const char* gap_cfg_s::ref_serv_cell_ind_opts::to_string() const
{
  static const char* names[] = {"pCell", "pSCell", "mcg-FR2"};
  return convert_enum_idx(names, 3, value, "gap_cfg_s::ref_serv_cell_ind_e_");
}
uint8_t gap_cfg_s::ref_serv_cell_ind_opts::to_number() const
{
  if (value == mcg_fr2) {
    return 2;
  }
  invalid_enum_number(value, "gap_cfg_s::ref_serv_cell_ind_e_");
  return 0;
}

const char* gap_cfg_s::mgl_r16_opts::to_string() const
{
  static const char* names[] = {"ms10", "ms20"};
  return convert_enum_idx(names, 2, value, "gap_cfg_s::mgl_r16_e_");
}
uint8_t gap_cfg_s::mgl_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20};
  return map_enum_number(numbers, 2, value, "gap_cfg_s::mgl_r16_e_");
}

// GapConfig-r17 ::= SEQUENCE
SRSASN_CODE gap_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ref_serv_cell_ind_r17_present, 1));
  HANDLE_CODE(bref.pack(ref_fr2_serv_cell_async_ca_r17_present, 1));
  HANDLE_CODE(bref.pack(pre_cfg_ind_r17_present, 1));
  HANDLE_CODE(bref.pack(ncsg_ind_r17_present, 1));
  HANDLE_CODE(bref.pack(gap_assoc_prs_r17_present, 1));
  HANDLE_CODE(bref.pack(gap_sharing_r17_present, 1));
  HANDLE_CODE(bref.pack(gap_prio_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_gap_id_r17, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(gap_type_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, gap_offset_r17, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(mgl_r17.pack(bref));
  HANDLE_CODE(mgrp_r17.pack(bref));
  HANDLE_CODE(mgta_r17.pack(bref));
  if (ref_serv_cell_ind_r17_present) {
    HANDLE_CODE(ref_serv_cell_ind_r17.pack(bref));
  }
  if (ref_fr2_serv_cell_async_ca_r17_present) {
    HANDLE_CODE(pack_integer(bref, ref_fr2_serv_cell_async_ca_r17, (uint8_t)0u, (uint8_t)31u));
  }
  if (gap_sharing_r17_present) {
    HANDLE_CODE(gap_sharing_r17.pack(bref));
  }
  if (gap_prio_r17_present) {
    HANDLE_CODE(pack_integer(bref, gap_prio_r17, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gap_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ref_serv_cell_ind_r17_present, 1));
  HANDLE_CODE(bref.unpack(ref_fr2_serv_cell_async_ca_r17_present, 1));
  HANDLE_CODE(bref.unpack(pre_cfg_ind_r17_present, 1));
  HANDLE_CODE(bref.unpack(ncsg_ind_r17_present, 1));
  HANDLE_CODE(bref.unpack(gap_assoc_prs_r17_present, 1));
  HANDLE_CODE(bref.unpack(gap_sharing_r17_present, 1));
  HANDLE_CODE(bref.unpack(gap_prio_r17_present, 1));

  HANDLE_CODE(unpack_integer(meas_gap_id_r17, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(gap_type_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(gap_offset_r17, bref, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(mgl_r17.unpack(bref));
  HANDLE_CODE(mgrp_r17.unpack(bref));
  HANDLE_CODE(mgta_r17.unpack(bref));
  if (ref_serv_cell_ind_r17_present) {
    HANDLE_CODE(ref_serv_cell_ind_r17.unpack(bref));
  }
  if (ref_fr2_serv_cell_async_ca_r17_present) {
    HANDLE_CODE(unpack_integer(ref_fr2_serv_cell_async_ca_r17, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (gap_sharing_r17_present) {
    HANDLE_CODE(gap_sharing_r17.unpack(bref));
  }
  if (gap_prio_r17_present) {
    HANDLE_CODE(unpack_integer(gap_prio_r17, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void gap_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measGapId-r17", meas_gap_id_r17);
  j.write_str("gapType-r17", gap_type_r17.to_string());
  j.write_int("gapOffset-r17", gap_offset_r17);
  j.write_str("mgl-r17", mgl_r17.to_string());
  j.write_str("mgrp-r17", mgrp_r17.to_string());
  j.write_str("mgta-r17", mgta_r17.to_string());
  if (ref_serv_cell_ind_r17_present) {
    j.write_str("refServCellIndicator-r17", ref_serv_cell_ind_r17.to_string());
  }
  if (ref_fr2_serv_cell_async_ca_r17_present) {
    j.write_int("refFR2-ServCellAsyncCA-r17", ref_fr2_serv_cell_async_ca_r17);
  }
  if (pre_cfg_ind_r17_present) {
    j.write_str("preConfigInd-r17", "true");
  }
  if (ncsg_ind_r17_present) {
    j.write_str("ncsgInd-r17", "true");
  }
  if (gap_assoc_prs_r17_present) {
    j.write_str("gapAssociationPRS-r17", "true");
  }
  if (gap_sharing_r17_present) {
    j.write_str("gapSharing-r17", gap_sharing_r17.to_string());
  }
  if (gap_prio_r17_present) {
    j.write_int("gapPriority-r17", gap_prio_r17);
  }
  j.end_obj();
}

const char* gap_cfg_r17_s::gap_type_r17_opts::to_string() const
{
  static const char* names[] = {"perUE", "perFR1", "perFR2"};
  return convert_enum_idx(names, 3, value, "gap_cfg_r17_s::gap_type_r17_e_");
}
uint8_t gap_cfg_r17_s::gap_type_r17_opts::to_number() const
{
  switch (value) {
    case per_fr1:
      return 1;
    case per_fr2:
      return 2;
    default:
      invalid_enum_number(value, "gap_cfg_r17_s::gap_type_r17_e_");
  }
  return 0;
}

const char* gap_cfg_r17_s::mgl_r17_opts::to_string() const
{
  static const char* names[] = {
      "ms1", "ms1dot5", "ms2", "ms3", "ms3dot5", "ms4", "ms5", "ms5dot5", "ms6", "ms10", "ms20"};
  return convert_enum_idx(names, 11, value, "gap_cfg_r17_s::mgl_r17_e_");
}
float gap_cfg_r17_s::mgl_r17_opts::to_number() const
{
  static const float numbers[] = {1.0, 1.5, 2.0, 3.0, 3.5, 4.0, 5.0, 5.5, 6.0, 10.0, 20.0};
  return map_enum_number(numbers, 11, value, "gap_cfg_r17_s::mgl_r17_e_");
}
const char* gap_cfg_r17_s::mgl_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"1", "1.5", "2", "3", "3.5", "4", "5", "5.5", "6", "10", "20"};
  return convert_enum_idx(number_strs, 11, value, "gap_cfg_r17_s::mgl_r17_e_");
}

const char* gap_cfg_r17_s::mgrp_r17_opts::to_string() const
{
  static const char* names[] = {"ms20", "ms40", "ms80", "ms160"};
  return convert_enum_idx(names, 4, value, "gap_cfg_r17_s::mgrp_r17_e_");
}
uint8_t gap_cfg_r17_s::mgrp_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 40, 80, 160};
  return map_enum_number(numbers, 4, value, "gap_cfg_r17_s::mgrp_r17_e_");
}

const char* gap_cfg_r17_s::mgta_r17_opts::to_string() const
{
  static const char* names[] = {"ms0", "ms0dot25", "ms0dot5", "ms0dot75"};
  return convert_enum_idx(names, 4, value, "gap_cfg_r17_s::mgta_r17_e_");
}
float gap_cfg_r17_s::mgta_r17_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.25, 0.5, 0.75};
  return map_enum_number(numbers, 4, value, "gap_cfg_r17_s::mgta_r17_e_");
}
const char* gap_cfg_r17_s::mgta_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.25", "0.5", "0.75"};
  return convert_enum_idx(number_strs, 4, value, "gap_cfg_r17_s::mgta_r17_e_");
}

const char* gap_cfg_r17_s::ref_serv_cell_ind_r17_opts::to_string() const
{
  static const char* names[] = {"pCell", "pSCell", "mcg-FR2"};
  return convert_enum_idx(names, 3, value, "gap_cfg_r17_s::ref_serv_cell_ind_r17_e_");
}
uint8_t gap_cfg_r17_s::ref_serv_cell_ind_r17_opts::to_number() const
{
  if (value == mcg_fr2) {
    return 2;
  }
  invalid_enum_number(value, "gap_cfg_r17_s::ref_serv_cell_ind_r17_e_");
  return 0;
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

// MeasIdToAddMod ::= SEQUENCE
SRSASN_CODE meas_id_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, meas_obj_id, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, report_cfg_id, (uint8_t)1u, (uint8_t)64u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_id_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(meas_obj_id, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(report_cfg_id, bref, (uint8_t)1u, (uint8_t)64u));

  return SRSASN_SUCCESS;
}
void meas_id_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  j.write_int("measObjectId", meas_obj_id);
  j.write_int("reportConfigId", report_cfg_id);
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

// MeasObjectToAddMod ::= SEQUENCE
SRSASN_CODE meas_obj_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, meas_obj_id, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(meas_obj.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(meas_obj_id, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(meas_obj.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_obj_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measObjectId", meas_obj_id);
  j.write_fieldname("measObject");
  meas_obj.to_json(j);
  j.end_obj();
}

void meas_obj_to_add_mod_s::meas_obj_c_::destroy_()
{
  switch (type_) {
    case types::meas_obj_nr:
      c.destroy<meas_obj_nr_s>();
      break;
    case types::meas_obj_eutra:
      c.destroy<meas_obj_eutra_s>();
      break;
    case types::meas_obj_utra_fdd_r16:
      c.destroy<meas_obj_utra_fdd_r16_s>();
      break;
    case types::meas_obj_nr_sl_r16:
      c.destroy<meas_obj_nr_sl_r16_s>();
      break;
    case types::meas_obj_cli_r16:
      c.destroy<meas_obj_cli_r16_s>();
      break;
    case types::meas_obj_rx_tx_diff_r17:
      c.destroy<meas_obj_rx_tx_diff_r17_s>();
      break;
    case types::meas_obj_relay_r17:
      c.destroy<sl_meas_obj_r16_s>();
      break;
    default:
      break;
  }
}
void meas_obj_to_add_mod_s::meas_obj_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_obj_nr:
      c.init<meas_obj_nr_s>();
      break;
    case types::meas_obj_eutra:
      c.init<meas_obj_eutra_s>();
      break;
    case types::meas_obj_utra_fdd_r16:
      c.init<meas_obj_utra_fdd_r16_s>();
      break;
    case types::meas_obj_nr_sl_r16:
      c.init<meas_obj_nr_sl_r16_s>();
      break;
    case types::meas_obj_cli_r16:
      c.init<meas_obj_cli_r16_s>();
      break;
    case types::meas_obj_rx_tx_diff_r17:
      c.init<meas_obj_rx_tx_diff_r17_s>();
      break;
    case types::meas_obj_relay_r17:
      c.init<sl_meas_obj_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }
}
meas_obj_to_add_mod_s::meas_obj_c_::meas_obj_c_(const meas_obj_to_add_mod_s::meas_obj_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_obj_nr:
      c.init(other.c.get<meas_obj_nr_s>());
      break;
    case types::meas_obj_eutra:
      c.init(other.c.get<meas_obj_eutra_s>());
      break;
    case types::meas_obj_utra_fdd_r16:
      c.init(other.c.get<meas_obj_utra_fdd_r16_s>());
      break;
    case types::meas_obj_nr_sl_r16:
      c.init(other.c.get<meas_obj_nr_sl_r16_s>());
      break;
    case types::meas_obj_cli_r16:
      c.init(other.c.get<meas_obj_cli_r16_s>());
      break;
    case types::meas_obj_rx_tx_diff_r17:
      c.init(other.c.get<meas_obj_rx_tx_diff_r17_s>());
      break;
    case types::meas_obj_relay_r17:
      c.init(other.c.get<sl_meas_obj_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }
}
meas_obj_to_add_mod_s::meas_obj_c_&
meas_obj_to_add_mod_s::meas_obj_c_::operator=(const meas_obj_to_add_mod_s::meas_obj_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_obj_nr:
      c.set(other.c.get<meas_obj_nr_s>());
      break;
    case types::meas_obj_eutra:
      c.set(other.c.get<meas_obj_eutra_s>());
      break;
    case types::meas_obj_utra_fdd_r16:
      c.set(other.c.get<meas_obj_utra_fdd_r16_s>());
      break;
    case types::meas_obj_nr_sl_r16:
      c.set(other.c.get<meas_obj_nr_sl_r16_s>());
      break;
    case types::meas_obj_cli_r16:
      c.set(other.c.get<meas_obj_cli_r16_s>());
      break;
    case types::meas_obj_rx_tx_diff_r17:
      c.set(other.c.get<meas_obj_rx_tx_diff_r17_s>());
      break;
    case types::meas_obj_relay_r17:
      c.set(other.c.get<sl_meas_obj_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }

  return *this;
}
meas_obj_nr_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_nr()
{
  set(types::meas_obj_nr);
  return c.get<meas_obj_nr_s>();
}
meas_obj_eutra_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_eutra()
{
  set(types::meas_obj_eutra);
  return c.get<meas_obj_eutra_s>();
}
meas_obj_utra_fdd_r16_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_utra_fdd_r16()
{
  set(types::meas_obj_utra_fdd_r16);
  return c.get<meas_obj_utra_fdd_r16_s>();
}
meas_obj_nr_sl_r16_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_nr_sl_r16()
{
  set(types::meas_obj_nr_sl_r16);
  return c.get<meas_obj_nr_sl_r16_s>();
}
meas_obj_cli_r16_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_cli_r16()
{
  set(types::meas_obj_cli_r16);
  return c.get<meas_obj_cli_r16_s>();
}
meas_obj_rx_tx_diff_r17_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_rx_tx_diff_r17()
{
  set(types::meas_obj_rx_tx_diff_r17);
  return c.get<meas_obj_rx_tx_diff_r17_s>();
}
sl_meas_obj_r16_s& meas_obj_to_add_mod_s::meas_obj_c_::set_meas_obj_relay_r17()
{
  set(types::meas_obj_relay_r17);
  return c.get<sl_meas_obj_r16_s>();
}
void meas_obj_to_add_mod_s::meas_obj_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_obj_nr:
      j.write_fieldname("measObjectNR");
      c.get<meas_obj_nr_s>().to_json(j);
      break;
    case types::meas_obj_eutra:
      j.write_fieldname("measObjectEUTRA");
      c.get<meas_obj_eutra_s>().to_json(j);
      break;
    case types::meas_obj_utra_fdd_r16:
      j.write_fieldname("measObjectUTRA-FDD-r16");
      c.get<meas_obj_utra_fdd_r16_s>().to_json(j);
      break;
    case types::meas_obj_nr_sl_r16:
      j.write_fieldname("measObjectNR-SL-r16");
      c.get<meas_obj_nr_sl_r16_s>().to_json(j);
      break;
    case types::meas_obj_cli_r16:
      j.write_fieldname("measObjectCLI-r16");
      c.get<meas_obj_cli_r16_s>().to_json(j);
      break;
    case types::meas_obj_rx_tx_diff_r17:
      j.write_fieldname("measObjectRxTxDiff-r17");
      c.get<meas_obj_rx_tx_diff_r17_s>().to_json(j);
      break;
    case types::meas_obj_relay_r17:
      j.write_fieldname("measObjectRelay-r17");
      c.get<sl_meas_obj_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_obj_to_add_mod_s::meas_obj_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_obj_nr:
      HANDLE_CODE(c.get<meas_obj_nr_s>().pack(bref));
      break;
    case types::meas_obj_eutra: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_eutra_s>().pack(bref));
    } break;
    case types::meas_obj_utra_fdd_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_utra_fdd_r16_s>().pack(bref));
    } break;
    case types::meas_obj_nr_sl_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_nr_sl_r16_s>().pack(bref));
    } break;
    case types::meas_obj_cli_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_cli_r16_s>().pack(bref));
    } break;
    case types::meas_obj_rx_tx_diff_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_rx_tx_diff_r17_s>().pack(bref));
    } break;
    case types::meas_obj_relay_r17: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sl_meas_obj_r16_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_to_add_mod_s::meas_obj_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_obj_nr:
      HANDLE_CODE(c.get<meas_obj_nr_s>().unpack(bref));
      break;
    case types::meas_obj_eutra: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_eutra_s>().unpack(bref));
    } break;
    case types::meas_obj_utra_fdd_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_utra_fdd_r16_s>().unpack(bref));
    } break;
    case types::meas_obj_nr_sl_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_nr_sl_r16_s>().unpack(bref));
    } break;
    case types::meas_obj_cli_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_cli_r16_s>().unpack(bref));
    } break;
    case types::meas_obj_rx_tx_diff_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<meas_obj_rx_tx_diff_r17_s>().unpack(bref));
    } break;
    case types::meas_obj_relay_r17: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sl_meas_obj_r16_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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

// QuantityConfigNR ::= SEQUENCE
SRSASN_CODE quant_cfg_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(quant_cfg_rs_idx_present, 1));

  HANDLE_CODE(quant_cfg_cell.pack(bref));
  if (quant_cfg_rs_idx_present) {
    HANDLE_CODE(quant_cfg_rs_idx.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(quant_cfg_rs_idx_present, 1));

  HANDLE_CODE(quant_cfg_cell.unpack(bref));
  if (quant_cfg_rs_idx_present) {
    HANDLE_CODE(quant_cfg_rs_idx.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("quantityConfigCell");
  quant_cfg_cell.to_json(j);
  if (quant_cfg_rs_idx_present) {
    j.write_fieldname("quantityConfigRS-Index");
    quant_cfg_rs_idx.to_json(j);
  }
  j.end_obj();
}

// QuantityConfigUTRA-FDD-r16 ::= SEQUENCE
SRSASN_CODE quant_cfg_utra_fdd_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(filt_coef_rs_cp_r16_present, 1));
  HANDLE_CODE(bref.pack(filt_coef_ec_no_r16_present, 1));

  if (filt_coef_rs_cp_r16_present) {
    HANDLE_CODE(filt_coef_rs_cp_r16.pack(bref));
  }
  if (filt_coef_ec_no_r16_present) {
    HANDLE_CODE(filt_coef_ec_no_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_utra_fdd_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(filt_coef_rs_cp_r16_present, 1));
  HANDLE_CODE(bref.unpack(filt_coef_ec_no_r16_present, 1));

  if (filt_coef_rs_cp_r16_present) {
    HANDLE_CODE(filt_coef_rs_cp_r16.unpack(bref));
  }
  if (filt_coef_ec_no_r16_present) {
    HANDLE_CODE(filt_coef_ec_no_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void quant_cfg_utra_fdd_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (filt_coef_rs_cp_r16_present) {
    j.write_str("filterCoefficientRSCP-r16", filt_coef_rs_cp_r16.to_string());
  }
  if (filt_coef_ec_no_r16_present) {
    j.write_str("filterCoefficientEcNO-r16", filt_coef_ec_no_r16.to_string());
  }
  j.end_obj();
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

// ReportConfigToAddMod ::= SEQUENCE
SRSASN_CODE report_cfg_to_add_mod_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, report_cfg_id, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(report_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_to_add_mod_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(report_cfg_id, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(report_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void report_cfg_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reportConfigId", report_cfg_id);
  j.write_fieldname("reportConfig");
  report_cfg.to_json(j);
  j.end_obj();
}

void report_cfg_to_add_mod_s::report_cfg_c_::destroy_()
{
  switch (type_) {
    case types::report_cfg_nr:
      c.destroy<report_cfg_nr_s>();
      break;
    case types::report_cfg_inter_rat:
      c.destroy<report_cfg_inter_rat_s>();
      break;
    case types::report_cfg_nr_sl_r16:
      c.destroy<report_cfg_nr_sl_r16_s>();
      break;
    default:
      break;
  }
}
void report_cfg_to_add_mod_s::report_cfg_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::report_cfg_nr:
      c.init<report_cfg_nr_s>();
      break;
    case types::report_cfg_inter_rat:
      c.init<report_cfg_inter_rat_s>();
      break;
    case types::report_cfg_nr_sl_r16:
      c.init<report_cfg_nr_sl_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }
}
report_cfg_to_add_mod_s::report_cfg_c_::report_cfg_c_(const report_cfg_to_add_mod_s::report_cfg_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::report_cfg_nr:
      c.init(other.c.get<report_cfg_nr_s>());
      break;
    case types::report_cfg_inter_rat:
      c.init(other.c.get<report_cfg_inter_rat_s>());
      break;
    case types::report_cfg_nr_sl_r16:
      c.init(other.c.get<report_cfg_nr_sl_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }
}
report_cfg_to_add_mod_s::report_cfg_c_&
report_cfg_to_add_mod_s::report_cfg_c_::operator=(const report_cfg_to_add_mod_s::report_cfg_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::report_cfg_nr:
      c.set(other.c.get<report_cfg_nr_s>());
      break;
    case types::report_cfg_inter_rat:
      c.set(other.c.get<report_cfg_inter_rat_s>());
      break;
    case types::report_cfg_nr_sl_r16:
      c.set(other.c.get<report_cfg_nr_sl_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }

  return *this;
}
report_cfg_nr_s& report_cfg_to_add_mod_s::report_cfg_c_::set_report_cfg_nr()
{
  set(types::report_cfg_nr);
  return c.get<report_cfg_nr_s>();
}
report_cfg_inter_rat_s& report_cfg_to_add_mod_s::report_cfg_c_::set_report_cfg_inter_rat()
{
  set(types::report_cfg_inter_rat);
  return c.get<report_cfg_inter_rat_s>();
}
report_cfg_nr_sl_r16_s& report_cfg_to_add_mod_s::report_cfg_c_::set_report_cfg_nr_sl_r16()
{
  set(types::report_cfg_nr_sl_r16);
  return c.get<report_cfg_nr_sl_r16_s>();
}
void report_cfg_to_add_mod_s::report_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::report_cfg_nr:
      j.write_fieldname("reportConfigNR");
      c.get<report_cfg_nr_s>().to_json(j);
      break;
    case types::report_cfg_inter_rat:
      j.write_fieldname("reportConfigInterRAT");
      c.get<report_cfg_inter_rat_s>().to_json(j);
      break;
    case types::report_cfg_nr_sl_r16:
      j.write_fieldname("reportConfigNR-SL-r16");
      c.get<report_cfg_nr_sl_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE report_cfg_to_add_mod_s::report_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::report_cfg_nr:
      HANDLE_CODE(c.get<report_cfg_nr_s>().pack(bref));
      break;
    case types::report_cfg_inter_rat: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_cfg_inter_rat_s>().pack(bref));
    } break;
    case types::report_cfg_nr_sl_r16: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_cfg_nr_sl_r16_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cfg_to_add_mod_s::report_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::report_cfg_nr:
      HANDLE_CODE(c.get<report_cfg_nr_s>().unpack(bref));
      break;
    case types::report_cfg_inter_rat: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_cfg_inter_rat_s>().unpack(bref));
    } break;
    case types::report_cfg_nr_sl_r16: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<report_cfg_nr_sl_r16_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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

// MeasGapConfig ::= SEQUENCE
SRSASN_CODE meas_gap_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gap_fr2_present, 1));

  if (gap_fr2_present) {
    HANDLE_CODE(gap_fr2.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= gap_fr1.is_present();
    group_flags[0] |= gap_ue.is_present();
    group_flags[1] |= gap_to_add_mod_list_r17.is_present();
    group_flags[1] |= gap_to_release_list_r17.is_present();
    group_flags[1] |= pos_meas_gap_pre_cfg_to_add_mod_list_r17.is_present();
    group_flags[1] |= pos_meas_gap_pre_cfg_to_release_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(gap_fr1.is_present(), 1));
      HANDLE_CODE(bref.pack(gap_ue.is_present(), 1));
      if (gap_fr1.is_present()) {
        HANDLE_CODE(gap_fr1->pack(bref));
      }
      if (gap_ue.is_present()) {
        HANDLE_CODE(gap_ue->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(gap_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(gap_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pos_meas_gap_pre_cfg_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(pos_meas_gap_pre_cfg_to_release_list_r17.is_present(), 1));
      if (gap_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *gap_to_add_mod_list_r17, 1, 8));
      }
      if (gap_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *gap_to_release_list_r17, 1, 8, integer_packer<uint8_t>(1, 8)));
      }
      if (pos_meas_gap_pre_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *pos_meas_gap_pre_cfg_to_add_mod_list_r17, 1, 16));
      }
      if (pos_meas_gap_pre_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *pos_meas_gap_pre_cfg_to_release_list_r17, 1, 16, integer_packer<uint8_t>(1, 16)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gap_fr2_present, 1));

  if (gap_fr2_present) {
    HANDLE_CODE(gap_fr2.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(gap_fr1, bref);
      unpack_presence_flag(gap_ue, bref);
      if (gap_fr1.is_present()) {
        HANDLE_CODE(gap_fr1->unpack(bref));
      }
      if (gap_ue.is_present()) {
        HANDLE_CODE(gap_ue->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(gap_to_add_mod_list_r17, bref);
      unpack_presence_flag(gap_to_release_list_r17, bref);
      unpack_presence_flag(pos_meas_gap_pre_cfg_to_add_mod_list_r17, bref);
      unpack_presence_flag(pos_meas_gap_pre_cfg_to_release_list_r17, bref);
      if (gap_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*gap_to_add_mod_list_r17, bref, 1, 8));
      }
      if (gap_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*gap_to_release_list_r17, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
      }
      if (pos_meas_gap_pre_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*pos_meas_gap_pre_cfg_to_add_mod_list_r17, bref, 1, 16));
      }
      if (pos_meas_gap_pre_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*pos_meas_gap_pre_cfg_to_release_list_r17, bref, 1, 16, integer_packer<uint8_t>(1, 16)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_gap_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gap_fr2_present) {
    j.write_fieldname("gapFR2");
    gap_fr2.to_json(j);
  }
  if (ext) {
    if (gap_fr1.is_present()) {
      j.write_fieldname("gapFR1");
      gap_fr1->to_json(j);
    }
    if (gap_ue.is_present()) {
      j.write_fieldname("gapUE");
      gap_ue->to_json(j);
    }
    if (gap_to_add_mod_list_r17.is_present()) {
      j.start_array("gapToAddModList-r17");
      for (const auto& e1 : *gap_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (gap_to_release_list_r17.is_present()) {
      j.start_array("gapToReleaseList-r17");
      for (const auto& e1 : *gap_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (pos_meas_gap_pre_cfg_to_add_mod_list_r17.is_present()) {
      j.start_array("posMeasGapPreConfigToAddModList-r17");
      for (const auto& e1 : *pos_meas_gap_pre_cfg_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (pos_meas_gap_pre_cfg_to_release_list_r17.is_present()) {
      j.start_array("posMeasGapPreConfigToReleaseList-r17");
      for (const auto& e1 : *pos_meas_gap_pre_cfg_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// MeasGapSharingConfig ::= SEQUENCE
SRSASN_CODE meas_gap_sharing_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gap_sharing_fr2_present, 1));

  if (gap_sharing_fr2_present) {
    HANDLE_CODE(gap_sharing_fr2.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= gap_sharing_fr1.is_present();
    group_flags[0] |= gap_sharing_ue.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(gap_sharing_fr1.is_present(), 1));
      HANDLE_CODE(bref.pack(gap_sharing_ue.is_present(), 1));
      if (gap_sharing_fr1.is_present()) {
        HANDLE_CODE(gap_sharing_fr1->pack(bref));
      }
      if (gap_sharing_ue.is_present()) {
        HANDLE_CODE(gap_sharing_ue->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_gap_sharing_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gap_sharing_fr2_present, 1));

  if (gap_sharing_fr2_present) {
    HANDLE_CODE(gap_sharing_fr2.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(gap_sharing_fr1, bref);
      unpack_presence_flag(gap_sharing_ue, bref);
      if (gap_sharing_fr1.is_present()) {
        HANDLE_CODE(gap_sharing_fr1->unpack(bref));
      }
      if (gap_sharing_ue.is_present()) {
        HANDLE_CODE(gap_sharing_ue->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_gap_sharing_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gap_sharing_fr2_present) {
    j.write_fieldname("gapSharingFR2");
    gap_sharing_fr2.to_json(j);
  }
  if (ext) {
    if (gap_sharing_fr1.is_present()) {
      j.write_fieldname("gapSharingFR1");
      gap_sharing_fr1->to_json(j);
    }
    if (gap_sharing_ue.is_present()) {
      j.write_fieldname("gapSharingUE");
      gap_sharing_ue->to_json(j);
    }
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

// QuantityConfig ::= SEQUENCE
SRSASN_CODE quant_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(quant_cfg_nr_list.size() > 0, 1));

  if (quant_cfg_nr_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, quant_cfg_nr_list, 1, 2));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= quant_cfg_eutra.is_present();
    group_flags[1] |= quant_cfg_utra_fdd_r16.is_present();
    group_flags[1] |= quant_cfg_cli_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(quant_cfg_eutra.is_present(), 1));
      if (quant_cfg_eutra.is_present()) {
        HANDLE_CODE(quant_cfg_eutra->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(quant_cfg_utra_fdd_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(quant_cfg_cli_r16.is_present(), 1));
      if (quant_cfg_utra_fdd_r16.is_present()) {
        HANDLE_CODE(quant_cfg_utra_fdd_r16->pack(bref));
      }
      if (quant_cfg_cli_r16.is_present()) {
        HANDLE_CODE(quant_cfg_cli_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE quant_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool quant_cfg_nr_list_present;
  HANDLE_CODE(bref.unpack(quant_cfg_nr_list_present, 1));

  if (quant_cfg_nr_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(quant_cfg_nr_list, bref, 1, 2));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(quant_cfg_eutra, bref);
      if (quant_cfg_eutra.is_present()) {
        HANDLE_CODE(quant_cfg_eutra->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(quant_cfg_utra_fdd_r16, bref);
      unpack_presence_flag(quant_cfg_cli_r16, bref);
      if (quant_cfg_utra_fdd_r16.is_present()) {
        HANDLE_CODE(quant_cfg_utra_fdd_r16->unpack(bref));
      }
      if (quant_cfg_cli_r16.is_present()) {
        HANDLE_CODE(quant_cfg_cli_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void quant_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (quant_cfg_nr_list.size() > 0) {
    j.start_array("quantityConfigNR-List");
    for (const auto& e1 : quant_cfg_nr_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (quant_cfg_eutra.is_present()) {
      j.write_fieldname("quantityConfigEUTRA");
      quant_cfg_eutra->to_json(j);
    }
    if (quant_cfg_utra_fdd_r16.is_present()) {
      j.write_fieldname("quantityConfigUTRA-FDD-r16");
      quant_cfg_utra_fdd_r16->to_json(j);
    }
    if (quant_cfg_cli_r16.is_present()) {
      j.write_fieldname("quantityConfigCLI-r16");
      quant_cfg_cli_r16->to_json(j);
    }
  }
  j.end_obj();
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

// MeasConfig ::= SEQUENCE
SRSASN_CODE meas_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_obj_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_obj_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(report_cfg_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(report_cfg_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_id_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_id_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(s_measure_cfg_present, 1));
  HANDLE_CODE(bref.pack(quant_cfg_present, 1));
  HANDLE_CODE(bref.pack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.pack(meas_gap_sharing_cfg_present, 1));

  if (meas_obj_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_to_rem_list, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (meas_obj_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_to_add_mod_list, 1, 64));
  }
  if (report_cfg_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, report_cfg_to_rem_list, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (report_cfg_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, report_cfg_to_add_mod_list, 1, 64));
  }
  if (meas_id_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_to_rem_list, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (meas_id_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_to_add_mod_list, 1, 64));
  }
  if (s_measure_cfg_present) {
    HANDLE_CODE(s_measure_cfg.pack(bref));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.pack(bref));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.pack(bref));
  }
  if (meas_gap_sharing_cfg_present) {
    HANDLE_CODE(meas_gap_sharing_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= interfreq_cfg_no_gap_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(interfreq_cfg_no_gap_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_obj_to_rem_list_present;
  HANDLE_CODE(bref.unpack(meas_obj_to_rem_list_present, 1));
  bool meas_obj_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(meas_obj_to_add_mod_list_present, 1));
  bool report_cfg_to_rem_list_present;
  HANDLE_CODE(bref.unpack(report_cfg_to_rem_list_present, 1));
  bool report_cfg_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(report_cfg_to_add_mod_list_present, 1));
  bool meas_id_to_rem_list_present;
  HANDLE_CODE(bref.unpack(meas_id_to_rem_list_present, 1));
  bool meas_id_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(meas_id_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(s_measure_cfg_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_sharing_cfg_present, 1));

  if (meas_obj_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_to_rem_list, bref, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (meas_obj_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_to_add_mod_list, bref, 1, 64));
  }
  if (report_cfg_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(report_cfg_to_rem_list, bref, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (report_cfg_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(report_cfg_to_add_mod_list, bref, 1, 64));
  }
  if (meas_id_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_to_rem_list, bref, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (meas_id_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_to_add_mod_list, bref, 1, 64));
  }
  if (s_measure_cfg_present) {
    HANDLE_CODE(s_measure_cfg.unpack(bref));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.unpack(bref));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.unpack(bref));
  }
  if (meas_gap_sharing_cfg_present) {
    HANDLE_CODE(meas_gap_sharing_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(interfreq_cfg_no_gap_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_obj_to_rem_list.size() > 0) {
    j.start_array("measObjectToRemoveList");
    for (const auto& e1 : meas_obj_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (meas_obj_to_add_mod_list.size() > 0) {
    j.start_array("measObjectToAddModList");
    for (const auto& e1 : meas_obj_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (report_cfg_to_rem_list.size() > 0) {
    j.start_array("reportConfigToRemoveList");
    for (const auto& e1 : report_cfg_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (report_cfg_to_add_mod_list.size() > 0) {
    j.start_array("reportConfigToAddModList");
    for (const auto& e1 : report_cfg_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_id_to_rem_list.size() > 0) {
    j.start_array("measIdToRemoveList");
    for (const auto& e1 : meas_id_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (meas_id_to_add_mod_list.size() > 0) {
    j.start_array("measIdToAddModList");
    for (const auto& e1 : meas_id_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (s_measure_cfg_present) {
    j.write_fieldname("s-MeasureConfig");
    s_measure_cfg.to_json(j);
  }
  if (quant_cfg_present) {
    j.write_fieldname("quantityConfig");
    quant_cfg.to_json(j);
  }
  if (meas_gap_cfg_present) {
    j.write_fieldname("measGapConfig");
    meas_gap_cfg.to_json(j);
  }
  if (meas_gap_sharing_cfg_present) {
    j.write_fieldname("measGapSharingConfig");
    meas_gap_sharing_cfg.to_json(j);
  }
  if (ext) {
    if (interfreq_cfg_no_gap_r16_present) {
      j.write_str("interFrequencyConfig-NoGap-r16", "true");
    }
  }
  j.end_obj();
}

void meas_cfg_s::s_measure_cfg_c_::destroy_() {}
void meas_cfg_s::s_measure_cfg_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
meas_cfg_s::s_measure_cfg_c_::s_measure_cfg_c_(const meas_cfg_s::s_measure_cfg_c_& other)
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
      log_invalid_choice_id(type_, "meas_cfg_s::s_measure_cfg_c_");
  }
}
meas_cfg_s::s_measure_cfg_c_& meas_cfg_s::s_measure_cfg_c_::operator=(const meas_cfg_s::s_measure_cfg_c_& other)
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
      log_invalid_choice_id(type_, "meas_cfg_s::s_measure_cfg_c_");
  }

  return *this;
}
uint8_t& meas_cfg_s::s_measure_cfg_c_::set_ssb_rsrp()
{
  set(types::ssb_rsrp);
  return c.get<uint8_t>();
}
uint8_t& meas_cfg_s::s_measure_cfg_c_::set_csi_rsrp()
{
  set(types::csi_rsrp);
  return c.get<uint8_t>();
}
void meas_cfg_s::s_measure_cfg_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "meas_cfg_s::s_measure_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_cfg_s::s_measure_cfg_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "meas_cfg_s::s_measure_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_s::s_measure_cfg_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "meas_cfg_s::s_measure_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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

void dl_dcch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_recfg:
      c.destroy<rrc_recfg_s>();
      break;
    case types::rrc_resume:
      c.destroy<rrc_resume_s>();
      break;
    case types::rrc_release:
      c.destroy<rrc_release_s>();
      break;
    case types::rrc_reest:
      c.destroy<rrc_reest_s>();
      break;
    case types::security_mode_cmd:
      c.destroy<security_mode_cmd_s>();
      break;
    case types::dl_info_transfer:
      c.destroy<dl_info_transfer_s>();
      break;
    case types::ue_cap_enquiry:
      c.destroy<ue_cap_enquiry_s>();
      break;
    case types::counter_check:
      c.destroy<counter_check_s>();
      break;
    case types::mob_from_nr_cmd:
      c.destroy<mob_from_nr_cmd_s>();
      break;
    case types::dl_ded_msg_segment_r16:
      c.destroy<dl_ded_msg_segment_r16_s>();
      break;
    case types::ue_info_request_r16:
      c.destroy<ue_info_request_r16_s>();
      break;
    case types::dl_info_transfer_mrdc_r16:
      c.destroy<dl_info_transfer_mrdc_r16_s>();
      break;
    case types::logged_meas_cfg_r16:
      c.destroy<logged_meas_cfg_r16_s>();
      break;
    default:
      break;
  }
}
void dl_dcch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_recfg:
      c.init<rrc_recfg_s>();
      break;
    case types::rrc_resume:
      c.init<rrc_resume_s>();
      break;
    case types::rrc_release:
      c.init<rrc_release_s>();
      break;
    case types::rrc_reest:
      c.init<rrc_reest_s>();
      break;
    case types::security_mode_cmd:
      c.init<security_mode_cmd_s>();
      break;
    case types::dl_info_transfer:
      c.init<dl_info_transfer_s>();
      break;
    case types::ue_cap_enquiry:
      c.init<ue_cap_enquiry_s>();
      break;
    case types::counter_check:
      c.init<counter_check_s>();
      break;
    case types::mob_from_nr_cmd:
      c.init<mob_from_nr_cmd_s>();
      break;
    case types::dl_ded_msg_segment_r16:
      c.init<dl_ded_msg_segment_r16_s>();
      break;
    case types::ue_info_request_r16:
      c.init<ue_info_request_r16_s>();
      break;
    case types::dl_info_transfer_mrdc_r16:
      c.init<dl_info_transfer_mrdc_r16_s>();
      break;
    case types::logged_meas_cfg_r16:
      c.init<logged_meas_cfg_r16_s>();
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
dl_dcch_msg_type_c::c1_c_::c1_c_(const dl_dcch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_recfg:
      c.init(other.c.get<rrc_recfg_s>());
      break;
    case types::rrc_resume:
      c.init(other.c.get<rrc_resume_s>());
      break;
    case types::rrc_release:
      c.init(other.c.get<rrc_release_s>());
      break;
    case types::rrc_reest:
      c.init(other.c.get<rrc_reest_s>());
      break;
    case types::security_mode_cmd:
      c.init(other.c.get<security_mode_cmd_s>());
      break;
    case types::dl_info_transfer:
      c.init(other.c.get<dl_info_transfer_s>());
      break;
    case types::ue_cap_enquiry:
      c.init(other.c.get<ue_cap_enquiry_s>());
      break;
    case types::counter_check:
      c.init(other.c.get<counter_check_s>());
      break;
    case types::mob_from_nr_cmd:
      c.init(other.c.get<mob_from_nr_cmd_s>());
      break;
    case types::dl_ded_msg_segment_r16:
      c.init(other.c.get<dl_ded_msg_segment_r16_s>());
      break;
    case types::ue_info_request_r16:
      c.init(other.c.get<ue_info_request_r16_s>());
      break;
    case types::dl_info_transfer_mrdc_r16:
      c.init(other.c.get<dl_info_transfer_mrdc_r16_s>());
      break;
    case types::logged_meas_cfg_r16:
      c.init(other.c.get<logged_meas_cfg_r16_s>());
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
dl_dcch_msg_type_c::c1_c_& dl_dcch_msg_type_c::c1_c_::operator=(const dl_dcch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_recfg:
      c.set(other.c.get<rrc_recfg_s>());
      break;
    case types::rrc_resume:
      c.set(other.c.get<rrc_resume_s>());
      break;
    case types::rrc_release:
      c.set(other.c.get<rrc_release_s>());
      break;
    case types::rrc_reest:
      c.set(other.c.get<rrc_reest_s>());
      break;
    case types::security_mode_cmd:
      c.set(other.c.get<security_mode_cmd_s>());
      break;
    case types::dl_info_transfer:
      c.set(other.c.get<dl_info_transfer_s>());
      break;
    case types::ue_cap_enquiry:
      c.set(other.c.get<ue_cap_enquiry_s>());
      break;
    case types::counter_check:
      c.set(other.c.get<counter_check_s>());
      break;
    case types::mob_from_nr_cmd:
      c.set(other.c.get<mob_from_nr_cmd_s>());
      break;
    case types::dl_ded_msg_segment_r16:
      c.set(other.c.get<dl_ded_msg_segment_r16_s>());
      break;
    case types::ue_info_request_r16:
      c.set(other.c.get<ue_info_request_r16_s>());
      break;
    case types::dl_info_transfer_mrdc_r16:
      c.set(other.c.get<dl_info_transfer_mrdc_r16_s>());
      break;
    case types::logged_meas_cfg_r16:
      c.set(other.c.get<logged_meas_cfg_r16_s>());
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

  return *this;
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

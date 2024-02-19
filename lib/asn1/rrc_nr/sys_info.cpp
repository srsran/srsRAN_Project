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

#include "srsran/asn1/rrc_nr/sys_info.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// EUTRA-NS-PmaxValue ::= SEQUENCE
SRSASN_CODE eutra_ns_pmax_value_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_pmax_present, 1));
  HANDLE_CODE(bref.pack(add_spec_emission_present, 1));

  if (add_pmax_present) {
    HANDLE_CODE(pack_integer(bref, add_pmax, (int8_t)-30, (int8_t)33));
  }
  if (add_spec_emission_present) {
    HANDLE_CODE(pack_integer(bref, add_spec_emission, (uint16_t)1u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_ns_pmax_value_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_pmax_present, 1));
  HANDLE_CODE(bref.unpack(add_spec_emission_present, 1));

  if (add_pmax_present) {
    HANDLE_CODE(unpack_integer(add_pmax, bref, (int8_t)-30, (int8_t)33));
  }
  if (add_spec_emission_present) {
    HANDLE_CODE(unpack_integer(add_spec_emission, bref, (uint16_t)1u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
void eutra_ns_pmax_value_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_pmax_present) {
    j.write_int("additionalPmax", add_pmax);
  }
  if (add_spec_emission_present) {
    j.write_int("additionalSpectrumEmission", add_spec_emission);
  }
  j.end_obj();
}

// GNSS-ID-r16 ::= SEQUENCE
SRSASN_CODE gnss_id_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(gnss_id_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnss_id_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(gnss_id_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void gnss_id_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gnss-id-r16", gnss_id_r16.to_string());
  j.end_obj();
}

const char* gnss_id_r16_s::gnss_id_r16_opts::to_string() const
{
  static const char* names[] = {"gps", "sbas", "qzss", "galileo", "glonass", "bds"};
  return convert_enum_idx(names, 6, value, "gnss_id_r16_s::gnss_id_r16_e_");
}

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

// SBAS-ID-r16 ::= SEQUENCE
SRSASN_CODE sbas_id_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sbas_id_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sbas_id_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sbas_id_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sbas_id_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sbas-id-r16", sbas_id_r16.to_string());
  j.end_obj();
}

const char* sbas_id_r16_s::sbas_id_r16_opts::to_string() const
{
  static const char* names[] = {"waas", "egnos", "msas", "gagan"};
  return convert_enum_idx(names, 4, value, "sbas_id_r16_s::sbas_id_r16_e_");
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

// SI-RequestResources ::= SEQUENCE
SRSASN_CODE si_request_res_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ra_assoc_period_idx_present, 1));
  HANDLE_CODE(bref.pack(ra_ssb_occasion_mask_idx_present, 1));

  HANDLE_CODE(pack_integer(bref, ra_preamb_start_idx, (uint8_t)0u, (uint8_t)63u));
  if (ra_assoc_period_idx_present) {
    HANDLE_CODE(pack_integer(bref, ra_assoc_period_idx, (uint8_t)0u, (uint8_t)15u));
  }
  if (ra_ssb_occasion_mask_idx_present) {
    HANDLE_CODE(pack_integer(bref, ra_ssb_occasion_mask_idx, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_request_res_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ra_assoc_period_idx_present, 1));
  HANDLE_CODE(bref.unpack(ra_ssb_occasion_mask_idx_present, 1));

  HANDLE_CODE(unpack_integer(ra_preamb_start_idx, bref, (uint8_t)0u, (uint8_t)63u));
  if (ra_assoc_period_idx_present) {
    HANDLE_CODE(unpack_integer(ra_assoc_period_idx, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (ra_ssb_occasion_mask_idx_present) {
    HANDLE_CODE(unpack_integer(ra_ssb_occasion_mask_idx, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void si_request_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ra-PreambleStartIndex", ra_preamb_start_idx);
  if (ra_assoc_period_idx_present) {
    j.write_int("ra-AssociationPeriodIndex", ra_assoc_period_idx);
  }
  if (ra_ssb_occasion_mask_idx_present) {
    j.write_int("ra-ssb-OccasionMaskIndex", ra_ssb_occasion_mask_idx);
  }
  j.end_obj();
}

// SIB-TypeInfo-v1700 ::= SEQUENCE
SRSASN_CODE sib_type_info_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(value_tag_r17_present, 1));
  HANDLE_CODE(bref.pack(area_scope_r17_present, 1));

  HANDLE_CODE(sib_type_r17.pack(bref));
  if (value_tag_r17_present) {
    HANDLE_CODE(pack_integer(bref, value_tag_r17, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type_info_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(value_tag_r17_present, 1));
  HANDLE_CODE(bref.unpack(area_scope_r17_present, 1));

  HANDLE_CODE(sib_type_r17.unpack(bref));
  if (value_tag_r17_present) {
    HANDLE_CODE(unpack_integer(value_tag_r17, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void sib_type_info_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sibType-r17");
  sib_type_r17.to_json(j);
  if (value_tag_r17_present) {
    j.write_int("valueTag-r17", value_tag_r17);
  }
  if (area_scope_r17_present) {
    j.write_str("areaScope-r17", "true");
  }
  j.end_obj();
}

void sib_type_info_v1700_s::sib_type_r17_c_::destroy_()
{
  switch (type_) {
    case types::type2_r17:
      c.destroy<type2_r17_s_>();
      break;
    default:
      break;
  }
}
void sib_type_info_v1700_s::sib_type_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type1_r17:
      break;
    case types::type2_r17:
      c.init<type2_r17_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }
}
sib_type_info_v1700_s::sib_type_r17_c_::sib_type_r17_c_(const sib_type_info_v1700_s::sib_type_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type1_r17:
      c.init(other.c.get<type1_r17_e_>());
      break;
    case types::type2_r17:
      c.init(other.c.get<type2_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }
}
sib_type_info_v1700_s::sib_type_r17_c_&
sib_type_info_v1700_s::sib_type_r17_c_::operator=(const sib_type_info_v1700_s::sib_type_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type1_r17:
      c.set(other.c.get<type1_r17_e_>());
      break;
    case types::type2_r17:
      c.set(other.c.get<type2_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }

  return *this;
}
sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_& sib_type_info_v1700_s::sib_type_r17_c_::set_type1_r17()
{
  set(types::type1_r17);
  return c.get<type1_r17_e_>();
}
sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_& sib_type_info_v1700_s::sib_type_r17_c_::set_type2_r17()
{
  set(types::type2_r17);
  return c.get<type2_r17_s_>();
}
void sib_type_info_v1700_s::sib_type_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type1_r17:
      j.write_str("type1-r17", c.get<type1_r17_e_>().to_string());
      break;
    case types::type2_r17:
      j.write_fieldname("type2-r17");
      j.start_obj();
      j.write_str("posSibType-r17", c.get<type2_r17_s_>().pos_sib_type_r17.to_string());
      if (c.get<type2_r17_s_>().encrypted_r17_present) {
        j.write_str("encrypted-r17", "true");
      }
      if (c.get<type2_r17_s_>().gnss_id_r17_present) {
        j.write_fieldname("gnss-id-r17");
        c.get<type2_r17_s_>().gnss_id_r17.to_json(j);
      }
      if (c.get<type2_r17_s_>().sbas_id_r17_present) {
        j.write_fieldname("sbas-id-r17");
        c.get<type2_r17_s_>().sbas_id_r17.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type_info_v1700_s::sib_type_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type1_r17:
      HANDLE_CODE(c.get<type1_r17_e_>().pack(bref));
      break;
    case types::type2_r17:
      HANDLE_CODE(bref.pack(c.get<type2_r17_s_>().encrypted_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type2_r17_s_>().gnss_id_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type2_r17_s_>().sbas_id_r17_present, 1));
      HANDLE_CODE(c.get<type2_r17_s_>().pos_sib_type_r17.pack(bref));
      if (c.get<type2_r17_s_>().gnss_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().gnss_id_r17.pack(bref));
      }
      if (c.get<type2_r17_s_>().sbas_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().sbas_id_r17.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type_info_v1700_s::sib_type_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type1_r17:
      HANDLE_CODE(c.get<type1_r17_e_>().unpack(bref));
      break;
    case types::type2_r17:
      HANDLE_CODE(bref.unpack(c.get<type2_r17_s_>().encrypted_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type2_r17_s_>().gnss_id_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type2_r17_s_>().sbas_id_r17_present, 1));
      HANDLE_CODE(c.get<type2_r17_s_>().pos_sib_type_r17.unpack(bref));
      if (c.get<type2_r17_s_>().gnss_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().gnss_id_r17.unpack(bref));
      }
      if (c.get<type2_r17_s_>().sbas_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().sbas_id_r17.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_opts::to_string() const
{
  static const char* names[] = {"sibType15",
                                "sibType16",
                                "sibType17",
                                "sibType18",
                                "sibType19",
                                "sibType20",
                                "sibType21",
                                "spare9",
                                "spare8",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_");
}
uint8_t sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 16, 17, 18, 19, 20, 21};
  return map_enum_number(numbers, 7, value, "sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_");
}

const char* sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17_opts::to_string() const
{
  static const char* names[] = {"posSibType1-9",
                                "posSibType1-10",
                                "posSibType2-24",
                                "posSibType2-25",
                                "posSibType6-4",
                                "posSibType6-5",
                                "posSibType6-6",
                                "spare9",
                                "spare8",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(
      names, 16, value, "sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17_e_");
}

const char* sib_type_info_v1700_s::sib_type_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"type1-r17", "type2-r17"};
  return convert_enum_idx(names, 2, value, "sib_type_info_v1700_s::sib_type_r17_c_::types");
}
uint8_t sib_type_info_v1700_s::sib_type_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sib_type_info_v1700_s::sib_type_r17_c_::types");
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

// EUTRA-FreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE eutra_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(dummy.pack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(dummy.unpack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void eutra_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("dummy", dummy.to_string());
  if (q_rx_lev_min_offset_cell_present) {
    j.write_int("q-RxLevMinOffsetCell", q_rx_lev_min_offset_cell);
  }
  if (q_qual_min_offset_cell_present) {
    j.write_int("q-QualMinOffsetCell", q_qual_min_offset_cell);
  }
  j.end_obj();
}

// EUTRA-MultiBandInfo ::= SEQUENCE
SRSASN_CODE eutra_multi_band_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_ns_pmax_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, eutra_freq_band_ind, (uint16_t)1u, (uint16_t)256u));
  if (eutra_ns_pmax_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_ns_pmax_list, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_multi_band_info_s::unpack(cbit_ref& bref)
{
  bool eutra_ns_pmax_list_present;
  HANDLE_CODE(bref.unpack(eutra_ns_pmax_list_present, 1));

  HANDLE_CODE(unpack_integer(eutra_freq_band_ind, bref, (uint16_t)1u, (uint16_t)256u));
  if (eutra_ns_pmax_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_ns_pmax_list, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void eutra_multi_band_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-FreqBandIndicator", eutra_freq_band_ind);
  if (eutra_ns_pmax_list.size() > 0) {
    j.start_array("eutra-NS-PmaxList");
    for (const auto& e1 : eutra_ns_pmax_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
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

const char* ephemeris_info_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"positionVelocity-r17", "orbital-r17"};
  return convert_enum_idx(names, 2, value, "ephemeris_info_r17_c::types");
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

// InterFreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.pack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell_sul, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.unpack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell_sul, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("q-OffsetCell", q_offset_cell.to_string());
  if (q_rx_lev_min_offset_cell_present) {
    j.write_int("q-RxLevMinOffsetCell", q_rx_lev_min_offset_cell);
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    j.write_int("q-RxLevMinOffsetCellSUL", q_rx_lev_min_offset_cell_sul);
  }
  if (q_qual_min_offset_cell_present) {
    j.write_int("q-QualMinOffsetCell", q_qual_min_offset_cell);
  }
  j.end_obj();
}

// InterFreqNeighCellInfo-v1610 ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r16_present) {
    j.write_str("ssb-PositionQCL-r16", ssb_position_qcl_r16.to_string());
  }
  j.end_obj();
}

// InterFreqNeighCellInfo-v1710 ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_v1710_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_v1710_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_v1710_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r17_present) {
    j.write_str("ssb-PositionQCL-r17", ssb_position_qcl_r17.to_string());
  }
  j.end_obj();
}

// SI-RequestConfig ::= SEQUENCE
SRSASN_CODE si_request_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rach_occasions_si_present, 1));
  HANDLE_CODE(bref.pack(si_request_period_present, 1));

  if (rach_occasions_si_present) {
    HANDLE_CODE(rach_occasions_si.rach_cfg_si.pack(bref));
    HANDLE_CODE(rach_occasions_si.ssb_per_rach_occasion.pack(bref));
  }
  if (si_request_period_present) {
    HANDLE_CODE(si_request_period.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, si_request_res, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_request_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rach_occasions_si_present, 1));
  HANDLE_CODE(bref.unpack(si_request_period_present, 1));

  if (rach_occasions_si_present) {
    HANDLE_CODE(rach_occasions_si.rach_cfg_si.unpack(bref));
    HANDLE_CODE(rach_occasions_si.ssb_per_rach_occasion.unpack(bref));
  }
  if (si_request_period_present) {
    HANDLE_CODE(si_request_period.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(si_request_res, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void si_request_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rach_occasions_si_present) {
    j.write_fieldname("rach-OccasionsSI");
    j.start_obj();
    j.write_fieldname("rach-ConfigSI");
    rach_occasions_si.rach_cfg_si.to_json(j);
    j.write_str("ssb-perRACH-Occasion", rach_occasions_si.ssb_per_rach_occasion.to_string());
    j.end_obj();
  }
  if (si_request_period_present) {
    j.write_str("si-RequestPeriod", si_request_period.to_string());
  }
  j.start_array("si-RequestResources");
  for (const auto& e1 : si_request_res) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_string() const
{
  static const char* names[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(names, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}
float si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(numbers, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}
const char* si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(number_strs, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}

const char* si_request_cfg_s::si_request_period_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "six", "eight", "ten", "twelve", "sixteen"};
  return convert_enum_idx(names, 8, value, "si_request_cfg_s::si_request_period_e_");
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

// SliceInfo-r17 ::= SEQUENCE
SRSASN_CODE slice_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nsag_cell_resel_prio_r17_present, 1));
  HANDLE_CODE(bref.pack(nsag_cell_resel_sub_prio_r17_present, 1));
  HANDLE_CODE(bref.pack(slice_cell_list_nr_r17_present, 1));

  HANDLE_CODE(nsag_id_info_r17.pack(bref));
  if (nsag_cell_resel_prio_r17_present) {
    HANDLE_CODE(pack_integer(bref, nsag_cell_resel_prio_r17, (uint8_t)0u, (uint8_t)7u));
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    HANDLE_CODE(nsag_cell_resel_sub_prio_r17.pack(bref));
  }
  if (slice_cell_list_nr_r17_present) {
    HANDLE_CODE(slice_cell_list_nr_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE slice_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nsag_cell_resel_prio_r17_present, 1));
  HANDLE_CODE(bref.unpack(nsag_cell_resel_sub_prio_r17_present, 1));
  HANDLE_CODE(bref.unpack(slice_cell_list_nr_r17_present, 1));

  HANDLE_CODE(nsag_id_info_r17.unpack(bref));
  if (nsag_cell_resel_prio_r17_present) {
    HANDLE_CODE(unpack_integer(nsag_cell_resel_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    HANDLE_CODE(nsag_cell_resel_sub_prio_r17.unpack(bref));
  }
  if (slice_cell_list_nr_r17_present) {
    HANDLE_CODE(slice_cell_list_nr_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void slice_info_r17_s::to_json(json_writer& j) const
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
  if (slice_cell_list_nr_r17_present) {
    j.write_fieldname("sliceCellListNR-r17");
    slice_cell_list_nr_r17.to_json(j);
  }
  j.end_obj();
}

void slice_info_r17_s::slice_cell_list_nr_r17_c_::destroy_()
{
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.destroy<slice_cell_list_nr_r17_l>();
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.destroy<slice_cell_list_nr_r17_l>();
      break;
    default:
      break;
  }
}
void slice_info_r17_s::slice_cell_list_nr_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.init<slice_cell_list_nr_r17_l>();
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.init<slice_cell_list_nr_r17_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }
}
slice_info_r17_s::slice_cell_list_nr_r17_c_::slice_cell_list_nr_r17_c_(
    const slice_info_r17_s::slice_cell_list_nr_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.init(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.init(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }
}
slice_info_r17_s::slice_cell_list_nr_r17_c_&
slice_info_r17_s::slice_cell_list_nr_r17_c_::operator=(const slice_info_r17_s::slice_cell_list_nr_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.set(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.set(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }

  return *this;
}
slice_cell_list_nr_r17_l& slice_info_r17_s::slice_cell_list_nr_r17_c_::set_slice_allowed_cell_list_nr_r17()
{
  set(types::slice_allowed_cell_list_nr_r17);
  return c.get<slice_cell_list_nr_r17_l>();
}
slice_cell_list_nr_r17_l& slice_info_r17_s::slice_cell_list_nr_r17_c_::set_slice_excluded_cell_list_nr_r17()
{
  set(types::slice_excluded_cell_list_nr_r17);
  return c.get<slice_cell_list_nr_r17_l>();
}
void slice_info_r17_s::slice_cell_list_nr_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      j.start_array("sliceAllowedCellListNR-r17");
      for (const auto& e1 : c.get<slice_cell_list_nr_r17_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::slice_excluded_cell_list_nr_r17:
      j.start_array("sliceExcludedCellListNR-r17");
      for (const auto& e1 : c.get<slice_cell_list_nr_r17_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE slice_info_r17_s::slice_cell_list_nr_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<slice_cell_list_nr_r17_l>(), 1, 16));
      break;
    case types::slice_excluded_cell_list_nr_r17:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<slice_cell_list_nr_r17_l>(), 1, 16));
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE slice_info_r17_s::slice_cell_list_nr_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<slice_cell_list_nr_r17_l>(), bref, 1, 16));
      break;
    case types::slice_excluded_cell_list_nr_r17:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<slice_cell_list_nr_r17_l>(), bref, 1, 16));
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* slice_info_r17_s::slice_cell_list_nr_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"sliceAllowedCellListNR-r17", "sliceExcludedCellListNR-r17"};
  return convert_enum_idx(names, 2, value, "slice_info_r17_s::slice_cell_list_nr_r17_c_::types");
}

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

// CAG-IdentityInfo-r16 ::= SEQUENCE
SRSASN_CODE cag_id_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(manual_ca_gsel_allowed_r16_present, 1));

  HANDLE_CODE(cag_id_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cag_id_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(manual_ca_gsel_allowed_r16_present, 1));

  HANDLE_CODE(cag_id_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void cag_id_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cag-Identity-r16", cag_id_r16.to_string());
  if (manual_ca_gsel_allowed_r16_present) {
    j.write_str("manualCAGselectionAllowed-r16", "true");
  }
  j.end_obj();
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

// InterFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
SRSASN_CODE inter_freq_cag_cell_list_per_plmn_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r16, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(pack_dyn_seq_of(bref, cag_cell_list_r16, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_cag_cell_list_per_plmn_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(plmn_id_idx_r16, bref, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(unpack_dyn_seq_of(cag_cell_list_r16, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void inter_freq_cag_cell_list_per_plmn_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r16", plmn_id_idx_r16);
  j.start_array("cag-CellList-r16");
  for (const auto& e1 : cag_cell_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
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

// PDSCH-ConfigPTM-r17 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ptm_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(data_scrambling_id_pdsch_r17_present, 1));
  HANDLE_CODE(bref.pack(dmrs_scrambling_id0_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_aggregation_factor_r17_present, 1));

  if (data_scrambling_id_pdsch_r17_present) {
    HANDLE_CODE(pack_integer(bref, data_scrambling_id_pdsch_r17, (uint16_t)0u, (uint16_t)1023u));
  }
  if (dmrs_scrambling_id0_r17_present) {
    HANDLE_CODE(pack_integer(bref, dmrs_scrambling_id0_r17, (uint32_t)0u, (uint32_t)65535u));
  }
  if (pdsch_aggregation_factor_r17_present) {
    HANDLE_CODE(pdsch_aggregation_factor_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ptm_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(data_scrambling_id_pdsch_r17_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_scrambling_id0_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_aggregation_factor_r17_present, 1));

  if (data_scrambling_id_pdsch_r17_present) {
    HANDLE_CODE(unpack_integer(data_scrambling_id_pdsch_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (dmrs_scrambling_id0_r17_present) {
    HANDLE_CODE(unpack_integer(dmrs_scrambling_id0_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (pdsch_aggregation_factor_r17_present) {
    HANDLE_CODE(pdsch_aggregation_factor_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ptm_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (data_scrambling_id_pdsch_r17_present) {
    j.write_int("dataScramblingIdentityPDSCH-r17", data_scrambling_id_pdsch_r17);
  }
  if (dmrs_scrambling_id0_r17_present) {
    j.write_int("dmrs-ScramblingID0-r17", dmrs_scrambling_id0_r17);
  }
  if (pdsch_aggregation_factor_r17_present) {
    j.write_str("pdsch-AggregationFactor-r17", pdsch_aggregation_factor_r17.to_string());
  }
  j.end_obj();
}

const char* pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8"};
  return convert_enum_idx(names, 3, value, "pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_e_");
}
uint8_t pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_e_");
}

// PosSIB-Type-r16 ::= SEQUENCE
SRSASN_CODE pos_sib_type_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(encrypted_r16_present, 1));
  HANDLE_CODE(bref.pack(gnss_id_r16_present, 1));
  HANDLE_CODE(bref.pack(sbas_id_r16_present, 1));
  HANDLE_CODE(bref.pack(area_scope_r16_present, 1));

  if (gnss_id_r16_present) {
    HANDLE_CODE(gnss_id_r16.pack(bref));
  }
  if (sbas_id_r16_present) {
    HANDLE_CODE(sbas_id_r16.pack(bref));
  }
  HANDLE_CODE(pos_sib_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sib_type_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(encrypted_r16_present, 1));
  HANDLE_CODE(bref.unpack(gnss_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(sbas_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(area_scope_r16_present, 1));

  if (gnss_id_r16_present) {
    HANDLE_CODE(gnss_id_r16.unpack(bref));
  }
  if (sbas_id_r16_present) {
    HANDLE_CODE(sbas_id_r16.unpack(bref));
  }
  HANDLE_CODE(pos_sib_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void pos_sib_type_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (encrypted_r16_present) {
    j.write_str("encrypted-r16", "true");
  }
  if (gnss_id_r16_present) {
    j.write_fieldname("gnss-id-r16");
    gnss_id_r16.to_json(j);
  }
  if (sbas_id_r16_present) {
    j.write_fieldname("sbas-id-r16");
    sbas_id_r16.to_json(j);
  }
  j.write_str("posSibType-r16", pos_sib_type_r16.to_string());
  if (area_scope_r16_present) {
    j.write_str("areaScope-r16", "true");
  }
  j.end_obj();
}

const char* pos_sib_type_r16_s::pos_sib_type_r16_opts::to_string() const
{
  static const char* names[] = {
      "posSibType1-1",  "posSibType1-2",  "posSibType1-3",  "posSibType1-4",  "posSibType1-5",  "posSibType1-6",
      "posSibType1-7",  "posSibType1-8",  "posSibType2-1",  "posSibType2-2",  "posSibType2-3",  "posSibType2-4",
      "posSibType2-5",  "posSibType2-6",  "posSibType2-7",  "posSibType2-8",  "posSibType2-9",  "posSibType2-10",
      "posSibType2-11", "posSibType2-12", "posSibType2-13", "posSibType2-14", "posSibType2-15", "posSibType2-16",
      "posSibType2-17", "posSibType2-18", "posSibType2-19", "posSibType2-20", "posSibType2-21", "posSibType2-22",
      "posSibType2-23", "posSibType3-1",  "posSibType4-1",  "posSibType5-1",  "posSibType6-1",  "posSibType6-2",
      "posSibType6-3"};
  return convert_enum_idx(names, 37, value, "pos_sib_type_r16_s::pos_sib_type_r16_e_");
}

// SI-SchedulingInfo-v1740 ::= SEQUENCE
SRSASN_CODE si_sched_info_v1740_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(si_request_cfg_red_cap_r17_present, 1));

  if (si_request_cfg_red_cap_r17_present) {
    HANDLE_CODE(si_request_cfg_red_cap_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_sched_info_v1740_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(si_request_cfg_red_cap_r17_present, 1));

  if (si_request_cfg_red_cap_r17_present) {
    HANDLE_CODE(si_request_cfg_red_cap_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void si_sched_info_v1740_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (si_request_cfg_red_cap_r17_present) {
    j.write_fieldname("si-RequestConfigRedCap-r17");
    si_request_cfg_red_cap_r17.to_json(j);
  }
  j.end_obj();
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

const char* ssb_mtc_s::periodicity_and_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"sf5", "sf10", "sf20", "sf40", "sf80", "sf160"};
  return convert_enum_idx(names, 6, value, "ssb_mtc_s::periodicity_and_offset_c_::types");
}
uint8_t ssb_mtc_s::periodicity_and_offset_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "ssb_mtc_s::periodicity_and_offset_c_::types");
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

// SSB-MTC2-LP-r16 ::= SEQUENCE
SRSASN_CODE ssb_mtc2_lp_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pci_list.size() > 0, 1));

  if (pci_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_list, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc2_lp_r16_s::unpack(cbit_ref& bref)
{
  bool pci_list_present;
  HANDLE_CODE(bref.unpack(pci_list_present, 1));

  if (pci_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_list, bref, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity.unpack(bref));

  return SRSASN_SUCCESS;
}
void ssb_mtc2_lp_r16_s::to_json(json_writer& j) const
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

const char* ssb_mtc2_lp_r16_s::periodicity_opts::to_string() const
{
  static const char* names[] = {"sf10", "sf20", "sf40", "sf80", "sf160", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ssb_mtc2_lp_r16_s::periodicity_e_");
}
uint8_t ssb_mtc2_lp_r16_s::periodicity_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 80, 160};
  return map_enum_number(numbers, 5, value, "ssb_mtc2_lp_r16_s::periodicity_e_");
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

const char* ssb_to_measure_c::types_opts::to_string() const
{
  static const char* names[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
  return convert_enum_idx(names, 3, value, "ssb_to_measure_c::types");
}

// SchedulingInfo2-r17 ::= SEQUENCE
SRSASN_CODE sched_info2_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_broadcast_status_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, si_win_position_r17, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(si_periodicity_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info_r17, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info2_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_broadcast_status_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(si_win_position_r17, bref, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(si_periodicity_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info_r17, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void sched_info2_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-BroadcastStatus-r17", si_broadcast_status_r17.to_string());
  j.write_int("si-WindowPosition-r17", si_win_position_r17);
  j.write_str("si-Periodicity-r17", si_periodicity_r17.to_string());
  j.start_array("sib-MappingInfo-r17");
  for (const auto& e1 : sib_map_info_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* sched_info2_r17_s::si_broadcast_status_r17_opts::to_string() const
{
  static const char* names[] = {"broadcasting", "notBroadcasting"};
  return convert_enum_idx(names, 2, value, "sched_info2_r17_s::si_broadcast_status_r17_e_");
}

const char* sched_info2_r17_s::si_periodicity_r17_opts::to_string() const
{
  static const char* names[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(names, 7, value, "sched_info2_r17_s::si_periodicity_r17_e_");
}
uint16_t sched_info2_r17_s::si_periodicity_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "sched_info2_r17_s::si_periodicity_r17_e_");
}

// SpeedStateScaleFactors ::= SEQUENCE
SRSASN_CODE speed_state_scale_factors_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_medium.pack(bref));
  HANDLE_CODE(sf_high.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE speed_state_scale_factors_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_medium.unpack(bref));
  HANDLE_CODE(sf_high.unpack(bref));

  return SRSASN_SUCCESS;
}
void speed_state_scale_factors_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sf-Medium", sf_medium.to_string());
  j.write_str("sf-High", sf_high.to_string());
  j.end_obj();
}

const char* speed_state_scale_factors_s::sf_medium_opts::to_string() const
{
  static const char* names[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(names, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
float speed_state_scale_factors_s::sf_medium_opts::to_number() const
{
  static const float numbers[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(numbers, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
const char* speed_state_scale_factors_s::sf_medium_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(number_strs, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}

const char* speed_state_scale_factors_s::sf_high_opts::to_string() const
{
  static const char* names[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(names, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
float speed_state_scale_factors_s::sf_high_opts::to_number() const
{
  static const float numbers[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(numbers, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
const char* speed_state_scale_factors_s::sf_high_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(number_strs, 4, value, "speed_state_scale_factors_s::sf_high_e_");
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

// UAC-BarringInfoSet-v1700 ::= SEQUENCE
SRSASN_CODE uac_barr_info_set_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_barr_factor_for_ai3_r17_present, 1));

  if (uac_barr_factor_for_ai3_r17_present) {
    HANDLE_CODE(uac_barr_factor_for_ai3_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_info_set_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_barr_factor_for_ai3_r17_present, 1));

  if (uac_barr_factor_for_ai3_r17_present) {
    HANDLE_CODE(uac_barr_factor_for_ai3_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void uac_barr_info_set_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (uac_barr_factor_for_ai3_r17_present) {
    j.write_str("uac-BarringFactorForAI3-r17", uac_barr_factor_for_ai3_r17.to_string());
  }
  j.end_obj();
}

const char* uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_string() const
{
  static const char* names[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(names, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}
float uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(numbers, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}
const char* uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(number_strs, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}

// CarrierFreqEUTRA ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_multi_band_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_excluded_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.pack(thresh_x_q_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  if (eutra_multi_band_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_multi_band_info_list, 1, 8));
  }
  if (eutra_freq_neigh_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_freq_neigh_cell_list, 1, 8));
  }
  if (eutra_excluded_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_excluded_cell_list, 1, 16));
  }
  HANDLE_CODE(allowed_meas_bw.pack(bref));
  HANDLE_CODE(bref.pack(presence_ant_port1, 1));
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(pack_integer(bref, q_qual_min, (int8_t)-34, (int8_t)-3));
  HANDLE_CODE(pack_integer(bref, p_max_eutra, (int8_t)-30, (int8_t)33));
  if (thresh_x_q_present) {
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_high_q, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_low_q, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_s::unpack(cbit_ref& bref)
{
  bool eutra_multi_band_info_list_present;
  HANDLE_CODE(bref.unpack(eutra_multi_band_info_list_present, 1));
  bool eutra_freq_neigh_cell_list_present;
  HANDLE_CODE(bref.unpack(eutra_freq_neigh_cell_list_present, 1));
  bool eutra_excluded_cell_list_present;
  HANDLE_CODE(bref.unpack(eutra_excluded_cell_list_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.unpack(thresh_x_q_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  if (eutra_multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_multi_band_info_list, bref, 1, 8));
  }
  if (eutra_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_freq_neigh_cell_list, bref, 1, 8));
  }
  if (eutra_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_excluded_cell_list, bref, 1, 16));
  }
  HANDLE_CODE(allowed_meas_bw.unpack(bref));
  HANDLE_CODE(bref.unpack(presence_ant_port1, 1));
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(unpack_integer(q_qual_min, bref, (int8_t)-34, (int8_t)-3));
  HANDLE_CODE(unpack_integer(p_max_eutra, bref, (int8_t)-30, (int8_t)33));
  if (thresh_x_q_present) {
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_high_q, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_low_q, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  if (eutra_multi_band_info_list.size() > 0) {
    j.start_array("eutra-multiBandInfoList");
    for (const auto& e1 : eutra_multi_band_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (eutra_freq_neigh_cell_list.size() > 0) {
    j.start_array("eutra-FreqNeighCellList");
    for (const auto& e1 : eutra_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (eutra_excluded_cell_list.size() > 0) {
    j.start_array("eutra-ExcludedCellList");
    for (const auto& e1 : eutra_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("allowedMeasBandwidth", allowed_meas_bw.to_string());
  j.write_bool("presenceAntennaPort1", presence_ant_port1);
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  if (cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_sub_prio.to_string());
  }
  j.write_int("threshX-High", thresh_x_high);
  j.write_int("threshX-Low", thresh_x_low);
  j.write_int("q-RxLevMin", q_rx_lev_min);
  j.write_int("q-QualMin", q_qual_min);
  j.write_int("p-MaxEUTRA", p_max_eutra);
  if (thresh_x_q_present) {
    j.write_fieldname("threshX-Q");
    j.start_obj();
    j.write_int("threshX-HighQ", thresh_x_q.thresh_x_high_q);
    j.write_int("threshX-LowQ", thresh_x_q.thresh_x_low_q);
    j.end_obj();
  }
  j.end_obj();
}

// CarrierFreqEUTRA-v1610 ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(high_speed_eutra_carrier_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(high_speed_eutra_carrier_r16_present, 1));

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (high_speed_eutra_carrier_r16_present) {
    j.write_str("highSpeedEUTRACarrier-r16", "true");
  }
  j.end_obj();
}

// CarrierFreqEUTRA-v1700 ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_freq_neigh_hsdn_cell_list_r17.size() > 0, 1));

  if (eutra_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_freq_neigh_hsdn_cell_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_v1700_s::unpack(cbit_ref& bref)
{
  bool eutra_freq_neigh_hsdn_cell_list_r17_present;
  HANDLE_CODE(bref.unpack(eutra_freq_neigh_hsdn_cell_list_r17_present, 1));

  if (eutra_freq_neigh_hsdn_cell_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_freq_neigh_hsdn_cell_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    j.start_array("eutra-FreqNeighHSDN-CellList-r17");
    for (const auto& e1 : eutra_freq_neigh_hsdn_cell_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// FreqPrioritySlicing-r17 ::= SEQUENCE
SRSASN_CODE freq_prio_slicing_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(slice_info_list_r17.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, dl_implicit_carrier_freq_r17, (uint8_t)0u, (uint8_t)8u));
  if (slice_info_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slice_info_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_slicing_r17_s::unpack(cbit_ref& bref)
{
  bool slice_info_list_r17_present;
  HANDLE_CODE(bref.unpack(slice_info_list_r17_present, 1));

  HANDLE_CODE(unpack_integer(dl_implicit_carrier_freq_r17, bref, (uint8_t)0u, (uint8_t)8u));
  if (slice_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slice_info_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_slicing_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-ImplicitCarrierFreq-r17", dl_implicit_carrier_freq_r17);
  if (slice_info_list_r17.size() > 0) {
    j.start_array("sliceInfoList-r17");
    for (const auto& e1 : slice_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// HRNN-r16 ::= SEQUENCE
SRSASN_CODE hrnn_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(hrnn_r16.size() > 0, 1));

  if (hrnn_r16.size() > 0) {
    HANDLE_CODE(hrnn_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE hrnn_r16_s::unpack(cbit_ref& bref)
{
  bool hrnn_r16_present;
  HANDLE_CODE(bref.unpack(hrnn_r16_present, 1));

  if (hrnn_r16_present) {
    HANDLE_CODE(hrnn_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void hrnn_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hrnn_r16.size() > 0) {
    j.write_str("hrnn-r16", hrnn_r16.to_string());
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_list_sul.size() > 0, 1));
  HANDLE_CODE(bref.pack(nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.pack(abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.pack(smtc_present, 1));
  HANDLE_CODE(bref.pack(ssb_to_measure_present, 1));
  HANDLE_CODE(bref.pack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.pack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_present, 1));
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(t_resel_nr_sf_present, 1));
  HANDLE_CODE(bref.pack(thresh_x_q_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.pack(q_offset_freq_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(inter_freq_excluded_cell_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, dl_carrier_freq, (uint32_t)0u, (uint32_t)3279165u));
  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8));
  }
  if (freq_band_list_sul.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_sul, 1, 8));
  }
  if (nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(pack_integer(bref, nrof_ss_blocks_to_average, (uint8_t)2u, (uint8_t)16u));
  }
  if (abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(abs_thresh_ss_blocks_consolidation.pack(bref));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.pack(bref));
  }
  HANDLE_CODE(ssb_subcarrier_spacing.pack(bref));
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.pack(bref));
  }
  HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell, 1));
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_sul, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min, (int8_t)-43, (int8_t)-12));
  }
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_nr, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_nr_sf_present) {
    HANDLE_CODE(t_resel_nr_sf.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high_p, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low_p, (uint8_t)0u, (uint8_t)31u));
  if (thresh_x_q_present) {
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_high_q, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_low_q, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.pack(bref));
  }
  if (q_offset_freq_present) {
    HANDLE_CODE(q_offset_freq.pack(bref));
  }
  if (inter_freq_neigh_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list, 1, 16));
  }
  if (inter_freq_excluded_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_excluded_cell_list, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  bool freq_band_list_sul_present;
  HANDLE_CODE(bref.unpack(freq_band_list_sul_present, 1));
  HANDLE_CODE(bref.unpack(nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.unpack(abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.unpack(smtc_present, 1));
  HANDLE_CODE(bref.unpack(ssb_to_measure_present, 1));
  HANDLE_CODE(bref.unpack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.unpack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_present, 1));
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_nr_sf_present, 1));
  HANDLE_CODE(bref.unpack(thresh_x_q_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.unpack(q_offset_freq_present, 1));
  bool inter_freq_neigh_cell_list_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_present, 1));
  bool inter_freq_excluded_cell_list_present;
  HANDLE_CODE(bref.unpack(inter_freq_excluded_cell_list_present, 1));

  HANDLE_CODE(unpack_integer(dl_carrier_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8));
  }
  if (freq_band_list_sul_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_sul, bref, 1, 8));
  }
  if (nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(unpack_integer(nrof_ss_blocks_to_average, bref, (uint8_t)2u, (uint8_t)16u));
  }
  if (abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(abs_thresh_ss_blocks_consolidation.unpack(bref));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.unpack(bref));
  }
  HANDLE_CODE(ssb_subcarrier_spacing.unpack(bref));
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell, 1));
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_sul, bref, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(unpack_integer(q_qual_min, bref, (int8_t)-43, (int8_t)-12));
  }
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(t_resel_nr, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_nr_sf_present) {
    HANDLE_CODE(t_resel_nr_sf.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high_p, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low_p, bref, (uint8_t)0u, (uint8_t)31u));
  if (thresh_x_q_present) {
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_high_q, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_low_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.unpack(bref));
  }
  if (q_offset_freq_present) {
    HANDLE_CODE(q_offset_freq.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list, bref, 1, 16));
  }
  if (inter_freq_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_excluded_cell_list, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq", dl_carrier_freq);
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_list_sul.size() > 0) {
    j.start_array("frequencyBandListSUL");
    for (const auto& e1 : freq_band_list_sul) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nrof_ss_blocks_to_average_present) {
    j.write_int("nrofSS-BlocksToAverage", nrof_ss_blocks_to_average);
  }
  if (abs_thresh_ss_blocks_consolidation_present) {
    j.write_fieldname("absThreshSS-BlocksConsolidation");
    abs_thresh_ss_blocks_consolidation.to_json(j);
  }
  if (smtc_present) {
    j.write_fieldname("smtc");
    smtc.to_json(j);
  }
  j.write_str("ssbSubcarrierSpacing", ssb_subcarrier_spacing.to_string());
  if (ssb_to_measure_present) {
    j.write_fieldname("ssb-ToMeasure");
    ssb_to_measure.to_json(j);
  }
  j.write_bool("deriveSSB-IndexFromCell", derive_ssb_idx_from_cell);
  if (ss_rssi_meas_present) {
    j.write_fieldname("ss-RSSI-Measurement");
    ss_rssi_meas.to_json(j);
  }
  j.write_int("q-RxLevMin", q_rx_lev_min);
  if (q_rx_lev_min_sul_present) {
    j.write_int("q-RxLevMinSUL", q_rx_lev_min_sul);
  }
  if (q_qual_min_present) {
    j.write_int("q-QualMin", q_qual_min);
  }
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  j.write_int("t-ReselectionNR", t_resel_nr);
  if (t_resel_nr_sf_present) {
    j.write_fieldname("t-ReselectionNR-SF");
    t_resel_nr_sf.to_json(j);
  }
  j.write_int("threshX-HighP", thresh_x_high_p);
  j.write_int("threshX-LowP", thresh_x_low_p);
  if (thresh_x_q_present) {
    j.write_fieldname("threshX-Q");
    j.start_obj();
    j.write_int("threshX-HighQ", thresh_x_q.thresh_x_high_q);
    j.write_int("threshX-LowQ", thresh_x_q.thresh_x_low_q);
    j.end_obj();
  }
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  if (cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_sub_prio.to_string());
  }
  if (q_offset_freq_present) {
    j.write_str("q-OffsetFreq", q_offset_freq.to_string());
  }
  if (inter_freq_neigh_cell_list.size() > 0) {
    j.start_array("interFreqNeighCellList");
    for (const auto& e1 : inter_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inter_freq_excluded_cell_list.size() > 0) {
    j.start_array("interFreqExcludedCellList");
    for (const auto& e1 : inter_freq_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1610 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_v1610.size() > 0, 1));
  HANDLE_CODE(bref.pack(smtc2_lp_r16_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_allowed_cell_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(ssb_position_qcl_common_r16_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_cag_cell_list_r16.size() > 0, 1));

  if (inter_freq_neigh_cell_list_v1610.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list_v1610, 1, 16));
  }
  if (smtc2_lp_r16_present) {
    HANDLE_CODE(smtc2_lp_r16.pack(bref));
  }
  if (inter_freq_allowed_cell_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_allowed_cell_list_r16, 1, 16));
  }
  if (ssb_position_qcl_common_r16_present) {
    HANDLE_CODE(ssb_position_qcl_common_r16.pack(bref));
  }
  if (inter_freq_cag_cell_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_cag_cell_list_r16, 1, 12));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1610_s::unpack(cbit_ref& bref)
{
  bool inter_freq_neigh_cell_list_v1610_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_v1610_present, 1));
  HANDLE_CODE(bref.unpack(smtc2_lp_r16_present, 1));
  bool inter_freq_allowed_cell_list_r16_present;
  HANDLE_CODE(bref.unpack(inter_freq_allowed_cell_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r16_present, 1));
  bool inter_freq_cag_cell_list_r16_present;
  HANDLE_CODE(bref.unpack(inter_freq_cag_cell_list_r16_present, 1));

  if (inter_freq_neigh_cell_list_v1610_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list_v1610, bref, 1, 16));
  }
  if (smtc2_lp_r16_present) {
    HANDLE_CODE(smtc2_lp_r16.unpack(bref));
  }
  if (inter_freq_allowed_cell_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_allowed_cell_list_r16, bref, 1, 16));
  }
  if (ssb_position_qcl_common_r16_present) {
    HANDLE_CODE(ssb_position_qcl_common_r16.unpack(bref));
  }
  if (inter_freq_cag_cell_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_cag_cell_list_r16, bref, 1, 12));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_neigh_cell_list_v1610.size() > 0) {
    j.start_array("interFreqNeighCellList-v1610");
    for (const auto& e1 : inter_freq_neigh_cell_list_v1610) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (smtc2_lp_r16_present) {
    j.write_fieldname("smtc2-LP-r16");
    smtc2_lp_r16.to_json(j);
  }
  if (inter_freq_allowed_cell_list_r16.size() > 0) {
    j.start_array("interFreqAllowedCellList-r16");
    for (const auto& e1 : inter_freq_allowed_cell_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ssb_position_qcl_common_r16_present) {
    j.write_str("ssb-PositionQCL-Common-r16", ssb_position_qcl_common_r16.to_string());
  }
  if (inter_freq_cag_cell_list_r16.size() > 0) {
    j.start_array("interFreqCAG-CellList-r16");
    for (const auto& e1 : inter_freq_cag_cell_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1700 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_neigh_hsdn_cell_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(high_speed_meas_inter_freq_r17_present, 1));
  HANDLE_CODE(bref.pack(red_cap_access_allowed_r17_present, 1));
  HANDLE_CODE(bref.pack(ssb_position_qcl_common_r17_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_v1710.size() > 0, 1));

  if (inter_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_hsdn_cell_list_r17, 1, 16));
  }
  if (ssb_position_qcl_common_r17_present) {
    HANDLE_CODE(ssb_position_qcl_common_r17.pack(bref));
  }
  if (inter_freq_neigh_cell_list_v1710.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list_v1710, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1700_s::unpack(cbit_ref& bref)
{
  bool inter_freq_neigh_hsdn_cell_list_r17_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_hsdn_cell_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_meas_inter_freq_r17_present, 1));
  HANDLE_CODE(bref.unpack(red_cap_access_allowed_r17_present, 1));
  HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r17_present, 1));
  bool inter_freq_neigh_cell_list_v1710_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_v1710_present, 1));

  if (inter_freq_neigh_hsdn_cell_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_hsdn_cell_list_r17, bref, 1, 16));
  }
  if (ssb_position_qcl_common_r17_present) {
    HANDLE_CODE(ssb_position_qcl_common_r17.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_v1710_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list_v1710, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    j.start_array("interFreqNeighHSDN-CellList-r17");
    for (const auto& e1 : inter_freq_neigh_hsdn_cell_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (high_speed_meas_inter_freq_r17_present) {
    j.write_str("highSpeedMeasInterFreq-r17", "true");
  }
  if (red_cap_access_allowed_r17_present) {
    j.write_str("redCapAccessAllowed-r17", "true");
  }
  if (ssb_position_qcl_common_r17_present) {
    j.write_str("ssb-PositionQCL-Common-r17", ssb_position_qcl_common_r17.to_string());
  }
  if (inter_freq_neigh_cell_list_v1710.size() > 0) {
    j.start_array("interFreqNeighCellList-v1710");
    for (const auto& e1 : inter_freq_neigh_cell_list_v1710) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1720 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1720_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(smtc4list_r17.size() > 0, 1));

  if (smtc4list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, smtc4list_r17, 1, 3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1720_s::unpack(cbit_ref& bref)
{
  bool smtc4list_r17_present;
  HANDLE_CODE(bref.unpack(smtc4list_r17_present, 1));

  if (smtc4list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(smtc4list_r17, bref, 1, 3));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1720_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (smtc4list_r17.size() > 0) {
    j.start_array("smtc4list-r17");
    for (const auto& e1 : smtc4list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1730 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ch_access_mode2_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ch_access_mode2_r17_present, 1));

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ch_access_mode2_r17_present) {
    j.write_str("channelAccessMode2-r17", "enabled");
  }
  j.end_obj();
}

// IntraFreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.pack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell_sul, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.unpack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell_sul, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("q-OffsetCell", q_offset_cell.to_string());
  if (q_rx_lev_min_offset_cell_present) {
    j.write_int("q-RxLevMinOffsetCell", q_rx_lev_min_offset_cell);
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    j.write_int("q-RxLevMinOffsetCellSUL", q_rx_lev_min_offset_cell_sul);
  }
  if (q_qual_min_offset_cell_present) {
    j.write_int("q-QualMinOffsetCell", q_qual_min_offset_cell);
  }
  j.end_obj();
}

// IntraFreqNeighCellInfo-v1610 ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r16_present) {
    j.write_str("ssb-PositionQCL-r16", ssb_position_qcl_r16.to_string());
  }
  j.end_obj();
}

// IntraFreqNeighCellInfo-v1710 ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_v1710_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_v1710_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_v1710_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r17_present) {
    j.write_str("ssb-PositionQCL-r17", ssb_position_qcl_r17.to_string());
  }
  j.end_obj();
}

// LocationAndBandwidthBroadcast-r17 ::= CHOICE
void location_and_bw_broadcast_r17_c::set(types::options e)
{
  type_ = e;
}
void location_and_bw_broadcast_r17_c::set_same_as_sib1_cfg_location_and_bw()
{
  set(types::same_as_sib1_cfg_location_and_bw);
}
uint16_t& location_and_bw_broadcast_r17_c::set_location_and_bw()
{
  set(types::location_and_bw);
  return c;
}
void location_and_bw_broadcast_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::same_as_sib1_cfg_location_and_bw:
      break;
    case types::location_and_bw:
      j.write_int("locationAndBandwidth", c);
      break;
    default:
      log_invalid_choice_id(type_, "location_and_bw_broadcast_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE location_and_bw_broadcast_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::same_as_sib1_cfg_location_and_bw:
      break;
    case types::location_and_bw:
      HANDLE_CODE(pack_integer(bref, c, (uint16_t)0u, (uint16_t)37949u));
      break;
    default:
      log_invalid_choice_id(type_, "location_and_bw_broadcast_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_and_bw_broadcast_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::same_as_sib1_cfg_location_and_bw:
      break;
    case types::location_and_bw:
      HANDLE_CODE(unpack_integer(c, bref, (uint16_t)0u, (uint16_t)37949u));
      break;
    default:
      log_invalid_choice_id(type_, "location_and_bw_broadcast_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* location_and_bw_broadcast_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"sameAsSib1ConfiguredLocationAndBW", "locationAndBandwidth"};
  return convert_enum_idx(names, 2, value, "location_and_bw_broadcast_r17_c::types");
}
uint8_t location_and_bw_broadcast_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "location_and_bw_broadcast_r17_c::types");
}

// MBS-FSAI-InterFreq-r17 ::= SEQUENCE
SRSASN_CODE mbs_fsai_inter_freq_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, dl_carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, mbs_fsai_list_r17, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_fsai_inter_freq_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(dl_carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(mbs_fsai_list_r17, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void mbs_fsai_inter_freq_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq-r17", dl_carrier_freq_r17);
  j.start_array("mbs-FSAI-List-r17");
  for (const auto& e1 : mbs_fsai_list_r17) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// MCCH-RepetitionPeriodAndOffset-r17 ::= CHOICE
void mcch_repeat_period_and_offset_r17_c::destroy_() {}
void mcch_repeat_period_and_offset_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
mcch_repeat_period_and_offset_r17_c::mcch_repeat_period_and_offset_r17_c(
    const mcch_repeat_period_and_offset_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rf1_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf2_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf4_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf8_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf16_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf32_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf64_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf128_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf256_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
  }
}
mcch_repeat_period_and_offset_r17_c&
mcch_repeat_period_and_offset_r17_c::operator=(const mcch_repeat_period_and_offset_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rf1_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf2_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf4_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf8_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf16_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf32_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf64_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf128_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf256_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
  }

  return *this;
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf1_r17()
{
  set(types::rf1_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf2_r17()
{
  set(types::rf2_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf4_r17()
{
  set(types::rf4_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf8_r17()
{
  set(types::rf8_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf16_r17()
{
  set(types::rf16_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf32_r17()
{
  set(types::rf32_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf64_r17()
{
  set(types::rf64_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf128_r17()
{
  set(types::rf128_r17);
  return c.get<uint8_t>();
}
uint16_t& mcch_repeat_period_and_offset_r17_c::set_rf256_r17()
{
  set(types::rf256_r17);
  return c.get<uint16_t>();
}
void mcch_repeat_period_and_offset_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rf1_r17:
      j.write_int("rf1-r17", c.get<uint8_t>());
      break;
    case types::rf2_r17:
      j.write_int("rf2-r17", c.get<uint8_t>());
      break;
    case types::rf4_r17:
      j.write_int("rf4-r17", c.get<uint8_t>());
      break;
    case types::rf8_r17:
      j.write_int("rf8-r17", c.get<uint8_t>());
      break;
    case types::rf16_r17:
      j.write_int("rf16-r17", c.get<uint8_t>());
      break;
    case types::rf32_r17:
      j.write_int("rf32-r17", c.get<uint8_t>());
      break;
    case types::rf64_r17:
      j.write_int("rf64-r17", c.get<uint8_t>());
      break;
    case types::rf128_r17:
      j.write_int("rf128-r17", c.get<uint8_t>());
      break;
    case types::rf256_r17:
      j.write_int("rf256-r17", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE mcch_repeat_period_and_offset_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rf1_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)0u));
      break;
    case types::rf2_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::rf4_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::rf8_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::rf16_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::rf32_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::rf64_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::rf128_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::rf256_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_repeat_period_and_offset_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rf1_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)0u));
      break;
    case types::rf2_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::rf4_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::rf8_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::rf16_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::rf32_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::rf64_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::rf128_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::rf256_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mcch_repeat_period_and_offset_r17_c::types_opts::to_string() const
{
  static const char* names[] = {
      "rf1-r17", "rf2-r17", "rf4-r17", "rf8-r17", "rf16-r17", "rf32-r17", "rf64-r17", "rf128-r17", "rf256-r17"};
  return convert_enum_idx(names, 9, value, "mcch_repeat_period_and_offset_r17_c::types");
}
uint16_t mcch_repeat_period_and_offset_r17_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(numbers, 9, value, "mcch_repeat_period_and_offset_r17_c::types");
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

// NPN-Identity-r16 ::= CHOICE
void npn_id_r16_c::destroy_()
{
  switch (type_) {
    case types::pni_npn_r16:
      c.destroy<pni_npn_r16_s_>();
      break;
    case types::snpn_r16:
      c.destroy<snpn_r16_s_>();
      break;
    default:
      break;
  }
}
void npn_id_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pni_npn_r16:
      c.init<pni_npn_r16_s_>();
      break;
    case types::snpn_r16:
      c.init<snpn_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_id_r16_c");
  }
}
npn_id_r16_c::npn_id_r16_c(const npn_id_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pni_npn_r16:
      c.init(other.c.get<pni_npn_r16_s_>());
      break;
    case types::snpn_r16:
      c.init(other.c.get<snpn_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_id_r16_c");
  }
}
npn_id_r16_c& npn_id_r16_c::operator=(const npn_id_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pni_npn_r16:
      c.set(other.c.get<pni_npn_r16_s_>());
      break;
    case types::snpn_r16:
      c.set(other.c.get<snpn_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_id_r16_c");
  }

  return *this;
}
npn_id_r16_c::pni_npn_r16_s_& npn_id_r16_c::set_pni_npn_r16()
{
  set(types::pni_npn_r16);
  return c.get<pni_npn_r16_s_>();
}
npn_id_r16_c::snpn_r16_s_& npn_id_r16_c::set_snpn_r16()
{
  set(types::snpn_r16);
  return c.get<snpn_r16_s_>();
}
void npn_id_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pni_npn_r16:
      j.write_fieldname("pni-npn-r16");
      j.start_obj();
      j.write_fieldname("plmn-Identity-r16");
      c.get<pni_npn_r16_s_>().plmn_id_r16.to_json(j);
      j.start_array("cag-IdentityList-r16");
      for (const auto& e1 : c.get<pni_npn_r16_s_>().cag_id_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    case types::snpn_r16:
      j.write_fieldname("snpn-r16");
      j.start_obj();
      j.write_fieldname("plmn-Identity-r16");
      c.get<snpn_r16_s_>().plmn_id_r16.to_json(j);
      j.start_array("nid-List-r16");
      for (const auto& e1 : c.get<snpn_r16_s_>().nid_list_r16) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "npn_id_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE npn_id_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pni_npn_r16:
      HANDLE_CODE(c.get<pni_npn_r16_s_>().plmn_id_r16.pack(bref));
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pni_npn_r16_s_>().cag_id_list_r16, 1, 12));
      break;
    case types::snpn_r16:
      HANDLE_CODE(c.get<snpn_r16_s_>().plmn_id_r16.pack(bref));
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<snpn_r16_s_>().nid_list_r16, 1, 12));
      break;
    default:
      log_invalid_choice_id(type_, "npn_id_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE npn_id_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pni_npn_r16:
      HANDLE_CODE(c.get<pni_npn_r16_s_>().plmn_id_r16.unpack(bref));
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pni_npn_r16_s_>().cag_id_list_r16, bref, 1, 12));
      break;
    case types::snpn_r16:
      HANDLE_CODE(c.get<snpn_r16_s_>().plmn_id_r16.unpack(bref));
      HANDLE_CODE(unpack_dyn_seq_of(c.get<snpn_r16_s_>().nid_list_r16, bref, 1, 12));
      break;
    default:
      log_invalid_choice_id(type_, "npn_id_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* npn_id_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"pni-npn-r16", "snpn-r16"};
  return convert_enum_idx(names, 2, value, "npn_id_r16_c::types");
}

// NTN-NeighCellConfig-r17 ::= SEQUENCE
SRSASN_CODE ntn_neigh_cell_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(carrier_freq_r17_present, 1));
  HANDLE_CODE(bref.pack(pci_r17_present, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.pack(bref));
  }
  if (carrier_freq_r17_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (pci_r17_present) {
    HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ntn_neigh_cell_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(carrier_freq_r17_present, 1));
  HANDLE_CODE(bref.unpack(pci_r17_present, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.unpack(bref));
  }
  if (carrier_freq_r17_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (pci_r17_present) {
    HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  }

  return SRSASN_SUCCESS;
}
void ntn_neigh_cell_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ntn_cfg_r17_present) {
    j.write_fieldname("ntn-Config-r17");
    ntn_cfg_r17.to_json(j);
  }
  if (carrier_freq_r17_present) {
    j.write_int("carrierFreq-r17", carrier_freq_r17);
  }
  if (pci_r17_present) {
    j.write_int("physCellId-r17", pci_r17);
  }
  j.end_obj();
}

// PDSCH-ConfigBroadcast-r17 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_broadcast_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdsch_time_domain_alloc_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(lte_crs_to_match_around_r17_present, 1));
  HANDLE_CODE(bref.pack(mcs_table_r17_present, 1));
  HANDLE_CODE(bref.pack(xoverhead_r17_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pdsch_cfg_list_r17, 1, 16));
  if (pdsch_time_domain_alloc_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pdsch_time_domain_alloc_list_r17, 1, 16));
  }
  if (rate_match_pattern_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_to_add_mod_list_r17, 1, 4));
  }
  if (lte_crs_to_match_around_r17_present) {
    HANDLE_CODE(lte_crs_to_match_around_r17.pack(bref));
  }
  if (mcs_table_r17_present) {
    HANDLE_CODE(mcs_table_r17.pack(bref));
  }
  if (xoverhead_r17_present) {
    HANDLE_CODE(xoverhead_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_broadcast_r17_s::unpack(cbit_ref& bref)
{
  bool pdsch_time_domain_alloc_list_r17_present;
  HANDLE_CODE(bref.unpack(pdsch_time_domain_alloc_list_r17_present, 1));
  bool rate_match_pattern_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_to_add_mod_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(lte_crs_to_match_around_r17_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_r17_present, 1));
  HANDLE_CODE(bref.unpack(xoverhead_r17_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pdsch_cfg_list_r17, bref, 1, 16));
  if (pdsch_time_domain_alloc_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pdsch_time_domain_alloc_list_r17, bref, 1, 16));
  }
  if (rate_match_pattern_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_to_add_mod_list_r17, bref, 1, 4));
  }
  if (lte_crs_to_match_around_r17_present) {
    HANDLE_CODE(lte_crs_to_match_around_r17.unpack(bref));
  }
  if (mcs_table_r17_present) {
    HANDLE_CODE(mcs_table_r17.unpack(bref));
  }
  if (xoverhead_r17_present) {
    HANDLE_CODE(xoverhead_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("pdschConfigList-r17");
  for (const auto& e1 : pdsch_cfg_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (pdsch_time_domain_alloc_list_r17.size() > 0) {
    j.start_array("pdsch-TimeDomainAllocationList-r17");
    for (const auto& e1 : pdsch_time_domain_alloc_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rate_match_pattern_to_add_mod_list_r17.size() > 0) {
    j.start_array("rateMatchPatternToAddModList-r17");
    for (const auto& e1 : rate_match_pattern_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (lte_crs_to_match_around_r17_present) {
    j.write_fieldname("lte-CRS-ToMatchAround-r17");
    lte_crs_to_match_around_r17.to_json(j);
  }
  if (mcs_table_r17_present) {
    j.write_str("mcs-Table-r17", mcs_table_r17.to_string());
  }
  if (xoverhead_r17_present) {
    j.write_str("xOverhead-r17", xoverhead_r17.to_string());
  }
  j.end_obj();
}

const char* pdsch_cfg_broadcast_r17_s::mcs_table_r17_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_broadcast_r17_s::mcs_table_r17_e_");
}
uint16_t pdsch_cfg_broadcast_r17_s::mcs_table_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_broadcast_r17_s::mcs_table_r17_e_");
}

const char* pdsch_cfg_broadcast_r17_s::xoverhead_r17_opts::to_string() const
{
  static const char* names[] = {"xOh6", "xOh12", "xOh18"};
  return convert_enum_idx(names, 3, value, "pdsch_cfg_broadcast_r17_s::xoverhead_r17_e_");
}
uint8_t pdsch_cfg_broadcast_r17_s::xoverhead_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12, 18};
  return map_enum_number(numbers, 3, value, "pdsch_cfg_broadcast_r17_s::xoverhead_r17_e_");
}

// RedCap-ConfigCommonSIB-r17 ::= SEQUENCE
SRSASN_CODE red_cap_cfg_common_sib_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(half_duplex_red_cap_allowed_r17_present, 1));
  HANDLE_CODE(bref.pack(cell_barred_red_cap_r17_present, 1));

  if (cell_barred_red_cap_r17_present) {
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap1_rx_r17.pack(bref));
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap2_rx_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE red_cap_cfg_common_sib_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(half_duplex_red_cap_allowed_r17_present, 1));
  HANDLE_CODE(bref.unpack(cell_barred_red_cap_r17_present, 1));

  if (cell_barred_red_cap_r17_present) {
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap1_rx_r17.unpack(bref));
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap2_rx_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void red_cap_cfg_common_sib_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (half_duplex_red_cap_allowed_r17_present) {
    j.write_str("halfDuplexRedCapAllowed-r17", "true");
  }
  if (cell_barred_red_cap_r17_present) {
    j.write_fieldname("cellBarredRedCap-r17");
    j.start_obj();
    j.write_str("cellBarredRedCap1Rx-r17", cell_barred_red_cap_r17.cell_barred_red_cap1_rx_r17.to_string());
    j.write_str("cellBarredRedCap2Rx-r17", cell_barred_red_cap_r17.cell_barred_red_cap2_rx_r17.to_string());
    j.end_obj();
  }
  j.end_obj();
}

const char*
red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17_opts::to_string() const
{
  static const char* names[] = {"barred", "notBarred"};
  return convert_enum_idx(
      names, 2, value, "red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17_e_");
}

const char*
red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17_opts::to_string() const
{
  static const char* names[] = {"barred", "notBarred"};
  return convert_enum_idx(
      names, 2, value, "red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17_e_");
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

// SDT-ConfigCommonSIB-r17 ::= SEQUENCE
SRSASN_CODE sdt_cfg_common_sib_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sdt_rsrp_thres_r17_present, 1));
  HANDLE_CODE(bref.pack(sdt_lc_ch_sr_delay_timer_r17_present, 1));

  if (sdt_rsrp_thres_r17_present) {
    HANDLE_CODE(pack_integer(bref, sdt_rsrp_thres_r17, (uint8_t)0u, (uint8_t)127u));
  }
  if (sdt_lc_ch_sr_delay_timer_r17_present) {
    HANDLE_CODE(sdt_lc_ch_sr_delay_timer_r17.pack(bref));
  }
  HANDLE_CODE(sdt_data_volume_thres_r17.pack(bref));
  HANDLE_CODE(t319a_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdt_cfg_common_sib_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sdt_rsrp_thres_r17_present, 1));
  HANDLE_CODE(bref.unpack(sdt_lc_ch_sr_delay_timer_r17_present, 1));

  if (sdt_rsrp_thres_r17_present) {
    HANDLE_CODE(unpack_integer(sdt_rsrp_thres_r17, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (sdt_lc_ch_sr_delay_timer_r17_present) {
    HANDLE_CODE(sdt_lc_ch_sr_delay_timer_r17.unpack(bref));
  }
  HANDLE_CODE(sdt_data_volume_thres_r17.unpack(bref));
  HANDLE_CODE(t319a_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sdt_cfg_common_sib_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sdt_rsrp_thres_r17_present) {
    j.write_int("sdt-RSRP-Threshold-r17", sdt_rsrp_thres_r17);
  }
  if (sdt_lc_ch_sr_delay_timer_r17_present) {
    j.write_str("sdt-LogicalChannelSR-DelayTimer-r17", sdt_lc_ch_sr_delay_timer_r17.to_string());
  }
  j.write_str("sdt-DataVolumeThreshold-r17", sdt_data_volume_thres_r17.to_string());
  j.write_str("t319a-r17", t319a_r17.to_string());
  j.end_obj();
}

const char* sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_opts::to_string() const
{
  static const char* names[] = {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
  return convert_enum_idx(names, 8, value, "sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_e_");
}
uint16_t sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(numbers, 7, value, "sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_e_");
}

const char* sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_opts::to_string() const
{
  static const char* names[] = {"byte32",
                                "byte100",
                                "byte200",
                                "byte400",
                                "byte600",
                                "byte800",
                                "byte1000",
                                "byte2000",
                                "byte4000",
                                "byte8000",
                                "byte9000",
                                "byte10000",
                                "byte12000",
                                "byte24000",
                                "byte48000",
                                "byte96000"};
  return convert_enum_idx(names, 16, value, "sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_e_");
}
uint32_t sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_opts::to_number() const
{
  static const uint32_t numbers[] = {
      32, 100, 200, 400, 600, 800, 1000, 2000, 4000, 8000, 9000, 10000, 12000, 24000, 48000, 96000};
  return map_enum_number(numbers, 16, value, "sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_e_");
}

const char* sdt_cfg_common_sib_r17_s::t319a_r17_opts::to_string() const
{
  static const char* names[] = {"ms100",
                                "ms200",
                                "ms300",
                                "ms400",
                                "ms600",
                                "ms1000",
                                "ms2000",
                                "ms3000",
                                "ms4000",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sdt_cfg_common_sib_r17_s::t319a_r17_e_");
}
uint16_t sdt_cfg_common_sib_r17_s::t319a_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 2000, 3000, 4000};
  return map_enum_number(numbers, 9, value, "sdt_cfg_common_sib_r17_s::t319a_r17_e_");
}

// SI-SchedulingInfo-v1700 ::= SEQUENCE
SRSASN_CODE si_sched_info_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dummy_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list2_r17, 1, 32));
  if (dummy_present) {
    HANDLE_CODE(dummy.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_sched_info_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sched_info_list2_r17, bref, 1, 32));
  if (dummy_present) {
    HANDLE_CODE(dummy.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void si_sched_info_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("schedulingInfoList2-r17");
  for (const auto& e1 : sched_info_list2_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (dummy_present) {
    j.write_fieldname("dummy");
    dummy.to_json(j);
  }
  j.end_obj();
}

// SIB-TypeInfo ::= SEQUENCE
SRSASN_CODE sib_type_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(value_tag_present, 1));
  HANDLE_CODE(bref.pack(area_scope_present, 1));

  HANDLE_CODE(type.pack(bref));
  if (value_tag_present) {
    HANDLE_CODE(pack_integer(bref, value_tag, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(value_tag_present, 1));
  HANDLE_CODE(bref.unpack(area_scope_present, 1));

  HANDLE_CODE(type.unpack(bref));
  if (value_tag_present) {
    HANDLE_CODE(unpack_integer(value_tag, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void sib_type_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("type", type.to_string());
  if (value_tag_present) {
    j.write_int("valueTag", value_tag);
  }
  if (area_scope_present) {
    j.write_str("areaScope", "true");
  }
  j.end_obj();
}

const char* sib_type_info_s::type_opts::to_string() const
{
  static const char* names[] = {"sibType2",
                                "sibType3",
                                "sibType4",
                                "sibType5",
                                "sibType6",
                                "sibType7",
                                "sibType8",
                                "sibType9",
                                "sibType10-v1610",
                                "sibType11-v1610",
                                "sibType12-v1610",
                                "sibType13-v1610",
                                "sibType14-v1610",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sib_type_info_s::type_e_");
}
uint8_t sib_type_info_s::type_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  return map_enum_number(numbers, 13, value, "sib_type_info_s::type_e_");
}

// SIB1-v1740-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1740_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(si_sched_info_v1740_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (si_sched_info_v1740_present) {
    HANDLE_CODE(si_sched_info_v1740.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1740_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(si_sched_info_v1740_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (si_sched_info_v1740_present) {
    HANDLE_CODE(si_sched_info_v1740.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1740_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (si_sched_info_v1740_present) {
    j.write_fieldname("si-SchedulingInfo-v1740");
    si_sched_info_v1740.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UAC-BarringPerCat ::= SEQUENCE
SRSASN_CODE uac_barr_per_cat_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, access_category, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, uac_barr_info_set_idx, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_cat_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(access_category, bref, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(uac_barr_info_set_idx, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void uac_barr_per_cat_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("accessCategory", access_category);
  j.write_int("uac-barringInfoSetIndex", uac_barr_info_set_idx);
  j.end_obj();
}

// ApplicableDisasterInfo-r17 ::= CHOICE
void applicable_disaster_info_r17_c::set(types::options e)
{
  type_ = e;
}
void applicable_disaster_info_r17_c::set_no_disaster_roaming_r17()
{
  set(types::no_disaster_roaming_r17);
}
void applicable_disaster_info_r17_c::set_disaster_related_ind_r17()
{
  set(types::disaster_related_ind_r17);
}
void applicable_disaster_info_r17_c::set_common_plmns_r17()
{
  set(types::common_plmns_r17);
}
applicable_disaster_info_r17_c::ded_plmns_r17_l_& applicable_disaster_info_r17_c::set_ded_plmns_r17()
{
  set(types::ded_plmns_r17);
  return c;
}
void applicable_disaster_info_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      j.start_array("dedicatedPLMNs-r17");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE applicable_disaster_info_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 12));
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE applicable_disaster_info_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 12));
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* applicable_disaster_info_r17_c::types_opts::to_string() const
{
  static const char* names[] = {
      "noDisasterRoaming-r17", "disasterRelatedIndication-r17", "commonPLMNs-r17", "dedicatedPLMNs-r17"};
  return convert_enum_idx(names, 4, value, "applicable_disaster_info_r17_c::types");
}

// CFR-ConfigMCCH-MTCH-r17 ::= SEQUENCE
SRSASN_CODE cfr_cfg_mcch_mtch_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(location_and_bw_broadcast_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_mcch_r17_present, 1));
  HANDLE_CODE(bref.pack(common_coreset_ext_r17_present, 1));

  if (location_and_bw_broadcast_r17_present) {
    HANDLE_CODE(location_and_bw_broadcast_r17.pack(bref));
  }
  if (pdsch_cfg_mcch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mcch_r17.pack(bref));
  }
  if (common_coreset_ext_r17_present) {
    HANDLE_CODE(common_coreset_ext_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfr_cfg_mcch_mtch_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(location_and_bw_broadcast_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_mcch_r17_present, 1));
  HANDLE_CODE(bref.unpack(common_coreset_ext_r17_present, 1));

  if (location_and_bw_broadcast_r17_present) {
    HANDLE_CODE(location_and_bw_broadcast_r17.unpack(bref));
  }
  if (pdsch_cfg_mcch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mcch_r17.unpack(bref));
  }
  if (common_coreset_ext_r17_present) {
    HANDLE_CODE(common_coreset_ext_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cfr_cfg_mcch_mtch_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (location_and_bw_broadcast_r17_present) {
    j.write_fieldname("locationAndBandwidthBroadcast-r17");
    location_and_bw_broadcast_r17.to_json(j);
  }
  if (pdsch_cfg_mcch_r17_present) {
    j.write_fieldname("pdsch-ConfigMCCH-r17");
    pdsch_cfg_mcch_r17.to_json(j);
  }
  if (common_coreset_ext_r17_present) {
    j.write_fieldname("commonControlResourceSetExt-r17");
    common_coreset_ext_r17.to_json(j);
  }
  j.end_obj();
}

// GIN-Element-r17 ::= SEQUENCE
SRSASN_CODE gin_elem_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, nid_list_r17, 1, 24));

  return SRSASN_SUCCESS;
}
SRSASN_CODE gin_elem_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(nid_list_r17, bref, 1, 24));

  return SRSASN_SUCCESS;
}
void gin_elem_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-r17");
  plmn_id_r17.to_json(j);
  j.start_array("nid-List-r17");
  for (const auto& e1 : nid_list_r17) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// GINs-PerSNPN-r17 ::= SEQUENCE
SRSASN_CODE gi_ns_per_sn_pn_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_gi_ns_r17_present, 1));

  if (supported_gi_ns_r17_present) {
    HANDLE_CODE(supported_gi_ns_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gi_ns_per_sn_pn_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_gi_ns_r17_present, 1));

  if (supported_gi_ns_r17_present) {
    HANDLE_CODE(supported_gi_ns_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gi_ns_per_sn_pn_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_gi_ns_r17_present) {
    j.write_str("supportedGINs-r17", supported_gi_ns_r17.to_string());
  }
  j.end_obj();
}

// IntraFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
SRSASN_CODE intra_freq_cag_cell_list_per_plmn_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r16, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(pack_dyn_seq_of(bref, cag_cell_list_r16, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_cag_cell_list_per_plmn_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(plmn_id_idx_r16, bref, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(unpack_dyn_seq_of(cag_cell_list_r16, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void intra_freq_cag_cell_list_per_plmn_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r16", plmn_id_idx_r16);
  j.start_array("cag-CellList-r16");
  for (const auto& e1 : cag_cell_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MCCH-Config-r17 ::= SEQUENCE
SRSASN_CODE mcch_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcch_win_dur_r17_present, 1));

  HANDLE_CODE(mcch_repeat_period_and_offset_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, mcch_win_start_slot_r17, (uint8_t)0u, (uint8_t)79u));
  if (mcch_win_dur_r17_present) {
    HANDLE_CODE(mcch_win_dur_r17.pack(bref));
  }
  HANDLE_CODE(mcch_mod_period_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcch_win_dur_r17_present, 1));

  HANDLE_CODE(mcch_repeat_period_and_offset_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(mcch_win_start_slot_r17, bref, (uint8_t)0u, (uint8_t)79u));
  if (mcch_win_dur_r17_present) {
    HANDLE_CODE(mcch_win_dur_r17.unpack(bref));
  }
  HANDLE_CODE(mcch_mod_period_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mcch_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mcch-RepetitionPeriodAndOffset-r17");
  mcch_repeat_period_and_offset_r17.to_json(j);
  j.write_int("mcch-WindowStartSlot-r17", mcch_win_start_slot_r17);
  if (mcch_win_dur_r17_present) {
    j.write_str("mcch-WindowDuration-r17", mcch_win_dur_r17.to_string());
  }
  j.write_str("mcch-ModificationPeriod-r17", mcch_mod_period_r17.to_string());
  j.end_obj();
}

const char* mcch_cfg_r17_s::mcch_win_dur_r17_opts::to_string() const
{
  static const char* names[] = {"sl2", "sl4", "sl8", "sl10", "sl20", "sl40", "sl80", "sl160"};
  return convert_enum_idx(names, 8, value, "mcch_cfg_r17_s::mcch_win_dur_r17_e_");
}
uint8_t mcch_cfg_r17_s::mcch_win_dur_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 8, value, "mcch_cfg_r17_s::mcch_win_dur_r17_e_");
}

const char* mcch_cfg_r17_s::mcch_mod_period_r17_opts::to_string() const
{
  static const char* names[] = {"rf2",
                                "rf4",
                                "rf8",
                                "rf16",
                                "rf32",
                                "rf64",
                                "rf128",
                                "rf256",
                                "rf512",
                                "rf1024",
                                "r2048",
                                "rf4096",
                                "rf8192",
                                "rf16384",
                                "rf32768",
                                "rf65536"};
  return convert_enum_idx(names, 16, value, "mcch_cfg_r17_s::mcch_mod_period_r17_e_");
}
uint32_t mcch_cfg_r17_s::mcch_mod_period_r17_opts::to_number() const
{
  static const uint32_t numbers[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
  return map_enum_number(numbers, 16, value, "mcch_cfg_r17_s::mcch_mod_period_r17_e_");
}

// MeasIdleConfigSIB-r16 ::= SEQUENCE
SRSASN_CODE meas_idle_cfg_sib_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_eutra_r16.size() > 0, 1));

  if (meas_idle_carrier_list_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_nr_r16, 1, 8));
  }
  if (meas_idle_carrier_list_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_eutra_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_cfg_sib_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_idle_carrier_list_nr_r16_present;
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_nr_r16_present, 1));
  bool meas_idle_carrier_list_eutra_r16_present;
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_eutra_r16_present, 1));

  if (meas_idle_carrier_list_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_nr_r16, bref, 1, 8));
  }
  if (meas_idle_carrier_list_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_eutra_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_idle_cfg_sib_r16_s::to_json(json_writer& j) const
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
  j.end_obj();
}

// MobilityStateParameters ::= SEQUENCE
SRSASN_CODE mob_state_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_eval.pack(bref));
  HANDLE_CODE(t_hyst_normal.pack(bref));
  HANDLE_CODE(pack_integer(bref, n_cell_change_medium, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, n_cell_change_high, (uint8_t)1u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_state_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_eval.unpack(bref));
  HANDLE_CODE(t_hyst_normal.unpack(bref));
  HANDLE_CODE(unpack_integer(n_cell_change_medium, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(n_cell_change_high, bref, (uint8_t)1u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
void mob_state_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-Evaluation", t_eval.to_string());
  j.write_str("t-HystNormal", t_hyst_normal.to_string());
  j.write_int("n-CellChangeMedium", n_cell_change_medium);
  j.write_int("n-CellChangeHigh", n_cell_change_high);
  j.end_obj();
}

const char* mob_state_params_s::t_eval_opts::to_string() const
{
  static const char* names[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "mob_state_params_s::t_eval_e_");
}
uint8_t mob_state_params_s::t_eval_opts::to_number() const
{
  static const uint8_t numbers[] = {30, 60, 120, 180, 240};
  return map_enum_number(numbers, 5, value, "mob_state_params_s::t_eval_e_");
}

const char* mob_state_params_s::t_hyst_normal_opts::to_string() const
{
  static const char* names[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "mob_state_params_s::t_hyst_normal_e_");
}
uint8_t mob_state_params_s::t_hyst_normal_opts::to_number() const
{
  static const uint8_t numbers[] = {30, 60, 120, 180, 240};
  return map_enum_number(numbers, 5, value, "mob_state_params_s::t_hyst_normal_e_");
}

// NPN-IdentityInfo-r16 ::= SEQUENCE
SRSASN_CODE npn_id_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ranac_r16_present, 1));
  HANDLE_CODE(bref.pack(iab_support_r16_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, npn_id_list_r16, 1, 12));
  HANDLE_CODE(tac_r16.pack(bref));
  if (ranac_r16_present) {
    HANDLE_CODE(pack_integer(bref, ranac_r16, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_r16.pack(bref));
  HANDLE_CODE(cell_reserved_for_oper_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= gnb_id_len_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(gnb_id_len_r17_present, 1));
      if (gnb_id_len_r17_present) {
        HANDLE_CODE(pack_integer(bref, gnb_id_len_r17, (uint8_t)22u, (uint8_t)32u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE npn_id_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ranac_r16_present, 1));
  HANDLE_CODE(bref.unpack(iab_support_r16_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(npn_id_list_r16, bref, 1, 12));
  HANDLE_CODE(tac_r16.unpack(bref));
  if (ranac_r16_present) {
    HANDLE_CODE(unpack_integer(ranac_r16, bref, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_r16.unpack(bref));
  HANDLE_CODE(cell_reserved_for_oper_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(gnb_id_len_r17_present, 1));
      if (gnb_id_len_r17_present) {
        HANDLE_CODE(unpack_integer(gnb_id_len_r17, bref, (uint8_t)22u, (uint8_t)32u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void npn_id_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("npn-IdentityList-r16");
  for (const auto& e1 : npn_id_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-r16", tac_r16.to_string());
  if (ranac_r16_present) {
    j.write_int("ranac-r16", ranac_r16);
  }
  j.write_str("cellIdentity-r16", cell_id_r16.to_string());
  j.write_str("cellReservedForOperatorUse-r16", cell_reserved_for_oper_r16.to_string());
  if (iab_support_r16_present) {
    j.write_str("iab-Support-r16", "true");
  }
  if (ext) {
    if (gnb_id_len_r17_present) {
      j.write_int("gNB-ID-Length-r17", gnb_id_len_r17);
    }
  }
  j.end_obj();
}

const char* npn_id_info_r16_s::cell_reserved_for_oper_r16_opts::to_string() const
{
  static const char* names[] = {"reserved", "notReserved"};
  return convert_enum_idx(names, 2, value, "npn_id_info_r16_s::cell_reserved_for_oper_r16_e_");
}

// PLMN-IdentityInfo ::= SEQUENCE
SRSASN_CODE plmn_id_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tac_present, 1));
  HANDLE_CODE(bref.pack(ranac_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list, 1, 12));
  if (tac_present) {
    HANDLE_CODE(tac.pack(bref));
  }
  if (ranac_present) {
    HANDLE_CODE(pack_integer(bref, ranac, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id.pack(bref));
  HANDLE_CODE(cell_reserved_for_oper.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= iab_support_r16_present;
    group_flags[1] |= tracking_area_list_r17.is_present();
    group_flags[1] |= gnb_id_len_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(iab_support_r16_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tracking_area_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(gnb_id_len_r17_present, 1));
      if (tracking_area_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *tracking_area_list_r17, 1, 12));
      }
      if (gnb_id_len_r17_present) {
        HANDLE_CODE(pack_integer(bref, gnb_id_len_r17, (uint8_t)22u, (uint8_t)32u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tac_present, 1));
  HANDLE_CODE(bref.unpack(ranac_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list, bref, 1, 12));
  if (tac_present) {
    HANDLE_CODE(tac.unpack(bref));
  }
  if (ranac_present) {
    HANDLE_CODE(unpack_integer(ranac, bref, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id.unpack(bref));
  HANDLE_CODE(cell_reserved_for_oper.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(iab_support_r16_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(tracking_area_list_r17, bref);
      HANDLE_CODE(bref.unpack(gnb_id_len_r17_present, 1));
      if (tracking_area_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*tracking_area_list_r17, bref, 1, 12));
      }
      if (gnb_id_len_r17_present) {
        HANDLE_CODE(unpack_integer(gnb_id_len_r17, bref, (uint8_t)22u, (uint8_t)32u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void plmn_id_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList");
  for (const auto& e1 : plmn_id_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (tac_present) {
    j.write_str("trackingAreaCode", tac.to_string());
  }
  if (ranac_present) {
    j.write_int("ranac", ranac);
  }
  j.write_str("cellIdentity", cell_id.to_string());
  j.write_str("cellReservedForOperatorUse", cell_reserved_for_oper.to_string());
  if (ext) {
    if (iab_support_r16_present) {
      j.write_str("iab-Support-r16", "true");
    }
    if (tracking_area_list_r17.is_present()) {
      j.start_array("trackingAreaList-r17");
      for (const auto& e1 : *tracking_area_list_r17) {
        j.write_str(e1.to_string());
      }
      j.end_array();
    }
    if (gnb_id_len_r17_present) {
      j.write_int("gNB-ID-Length-r17", gnb_id_len_r17);
    }
  }
  j.end_obj();
}

const char* plmn_id_info_s::cell_reserved_for_oper_opts::to_string() const
{
  static const char* names[] = {"reserved", "notReserved"};
  return convert_enum_idx(names, 2, value, "plmn_id_info_s::cell_reserved_for_oper_e_");
}

// PosSchedulingInfo-r16 ::= SEQUENCE
SRSASN_CODE pos_sched_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(offset_to_si_used_r16_present, 1));

  HANDLE_CODE(pos_si_periodicity_r16.pack(bref));
  HANDLE_CODE(pos_si_broadcast_status_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sib_map_info_r16, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sched_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(offset_to_si_used_r16_present, 1));

  HANDLE_CODE(pos_si_periodicity_r16.unpack(bref));
  HANDLE_CODE(pos_si_broadcast_status_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(pos_sib_map_info_r16, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void pos_sched_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (offset_to_si_used_r16_present) {
    j.write_str("offsetToSI-Used-r16", "true");
  }
  j.write_str("posSI-Periodicity-r16", pos_si_periodicity_r16.to_string());
  j.write_str("posSI-BroadcastStatus-r16", pos_si_broadcast_status_r16.to_string());
  j.start_array("posSIB-MappingInfo-r16");
  for (const auto& e1 : pos_sib_map_info_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* pos_sched_info_r16_s::pos_si_periodicity_r16_opts::to_string() const
{
  static const char* names[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(names, 7, value, "pos_sched_info_r16_s::pos_si_periodicity_r16_e_");
}
uint16_t pos_sched_info_r16_s::pos_si_periodicity_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "pos_sched_info_r16_s::pos_si_periodicity_r16_e_");
}

const char* pos_sched_info_r16_s::pos_si_broadcast_status_r16_opts::to_string() const
{
  static const char* names[] = {"broadcasting", "notBroadcasting"};
  return convert_enum_idx(names, 2, value, "pos_sched_info_r16_s::pos_si_broadcast_status_r16_e_");
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

// SIB1-v1700-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(hsdn_cell_r17_present, 1));
  HANDLE_CODE(bref.pack(uac_barr_info_v1700_present, 1));
  HANDLE_CODE(bref.pack(sdt_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.pack(red_cap_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.pack(feature_priorities_r17_present, 1));
  HANDLE_CODE(bref.pack(si_sched_info_v1700_present, 1));
  HANDLE_CODE(bref.pack(hyper_sfn_r17_present, 1));
  HANDLE_CODE(bref.pack(edrx_allowed_idle_r17_present, 1));
  HANDLE_CODE(bref.pack(edrx_allowed_inactive_r17_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_resel_red_cap_r17_present, 1));
  HANDLE_CODE(bref.pack(cell_barred_ntn_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (uac_barr_info_v1700_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info_v1700.uac_barr_info_set_list_v1700, 1, 8));
  }
  if (sdt_cfg_common_r17_present) {
    HANDLE_CODE(sdt_cfg_common_r17.pack(bref));
  }
  if (red_cap_cfg_common_r17_present) {
    HANDLE_CODE(red_cap_cfg_common_r17.pack(bref));
  }
  if (feature_priorities_r17_present) {
    HANDLE_CODE(bref.pack(feature_priorities_r17.red_cap_prio_r17_present, 1));
    HANDLE_CODE(bref.pack(feature_priorities_r17.slicing_prio_r17_present, 1));
    HANDLE_CODE(bref.pack(feature_priorities_r17.msg3_repeats_prio_r17_present, 1));
    HANDLE_CODE(bref.pack(feature_priorities_r17.sdt_prio_r17_present, 1));
    if (feature_priorities_r17.red_cap_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.red_cap_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.slicing_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.slicing_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.msg3_repeats_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.msg3_repeats_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.sdt_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.sdt_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
  }
  if (si_sched_info_v1700_present) {
    HANDLE_CODE(si_sched_info_v1700.pack(bref));
  }
  if (hyper_sfn_r17_present) {
    HANDLE_CODE(hyper_sfn_r17.pack(bref));
  }
  if (intra_freq_resel_red_cap_r17_present) {
    HANDLE_CODE(intra_freq_resel_red_cap_r17.pack(bref));
  }
  if (cell_barred_ntn_r17_present) {
    HANDLE_CODE(cell_barred_ntn_r17.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(hsdn_cell_r17_present, 1));
  HANDLE_CODE(bref.unpack(uac_barr_info_v1700_present, 1));
  HANDLE_CODE(bref.unpack(sdt_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.unpack(red_cap_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.unpack(feature_priorities_r17_present, 1));
  HANDLE_CODE(bref.unpack(si_sched_info_v1700_present, 1));
  HANDLE_CODE(bref.unpack(hyper_sfn_r17_present, 1));
  HANDLE_CODE(bref.unpack(edrx_allowed_idle_r17_present, 1));
  HANDLE_CODE(bref.unpack(edrx_allowed_inactive_r17_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_resel_red_cap_r17_present, 1));
  HANDLE_CODE(bref.unpack(cell_barred_ntn_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (uac_barr_info_v1700_present) {
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info_v1700.uac_barr_info_set_list_v1700, bref, 1, 8));
  }
  if (sdt_cfg_common_r17_present) {
    HANDLE_CODE(sdt_cfg_common_r17.unpack(bref));
  }
  if (red_cap_cfg_common_r17_present) {
    HANDLE_CODE(red_cap_cfg_common_r17.unpack(bref));
  }
  if (feature_priorities_r17_present) {
    HANDLE_CODE(bref.unpack(feature_priorities_r17.red_cap_prio_r17_present, 1));
    HANDLE_CODE(bref.unpack(feature_priorities_r17.slicing_prio_r17_present, 1));
    HANDLE_CODE(bref.unpack(feature_priorities_r17.msg3_repeats_prio_r17_present, 1));
    HANDLE_CODE(bref.unpack(feature_priorities_r17.sdt_prio_r17_present, 1));
    if (feature_priorities_r17.red_cap_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.red_cap_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.slicing_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.slicing_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.msg3_repeats_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.msg3_repeats_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.sdt_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.sdt_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
  }
  if (si_sched_info_v1700_present) {
    HANDLE_CODE(si_sched_info_v1700.unpack(bref));
  }
  if (hyper_sfn_r17_present) {
    HANDLE_CODE(hyper_sfn_r17.unpack(bref));
  }
  if (intra_freq_resel_red_cap_r17_present) {
    HANDLE_CODE(intra_freq_resel_red_cap_r17.unpack(bref));
  }
  if (cell_barred_ntn_r17_present) {
    HANDLE_CODE(cell_barred_ntn_r17.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hsdn_cell_r17_present) {
    j.write_str("hsdn-Cell-r17", "true");
  }
  if (uac_barr_info_v1700_present) {
    j.write_fieldname("uac-BarringInfo-v1700");
    j.start_obj();
    j.start_array("uac-BarringInfoSetList-v1700");
    for (const auto& e1 : uac_barr_info_v1700.uac_barr_info_set_list_v1700) {
      e1.to_json(j);
    }
    j.end_array();
    j.end_obj();
  }
  if (sdt_cfg_common_r17_present) {
    j.write_fieldname("sdt-ConfigCommon-r17");
    sdt_cfg_common_r17.to_json(j);
  }
  if (red_cap_cfg_common_r17_present) {
    j.write_fieldname("redCap-ConfigCommon-r17");
    red_cap_cfg_common_r17.to_json(j);
  }
  if (feature_priorities_r17_present) {
    j.write_fieldname("featurePriorities-r17");
    j.start_obj();
    if (feature_priorities_r17.red_cap_prio_r17_present) {
      j.write_int("redCapPriority-r17", feature_priorities_r17.red_cap_prio_r17);
    }
    if (feature_priorities_r17.slicing_prio_r17_present) {
      j.write_int("slicingPriority-r17", feature_priorities_r17.slicing_prio_r17);
    }
    if (feature_priorities_r17.msg3_repeats_prio_r17_present) {
      j.write_int("msg3-Repetitions-Priority-r17", feature_priorities_r17.msg3_repeats_prio_r17);
    }
    if (feature_priorities_r17.sdt_prio_r17_present) {
      j.write_int("sdt-Priority-r17", feature_priorities_r17.sdt_prio_r17);
    }
    j.end_obj();
  }
  if (si_sched_info_v1700_present) {
    j.write_fieldname("si-SchedulingInfo-v1700");
    si_sched_info_v1700.to_json(j);
  }
  if (hyper_sfn_r17_present) {
    j.write_str("hyperSFN-r17", hyper_sfn_r17.to_string());
  }
  if (edrx_allowed_idle_r17_present) {
    j.write_str("eDRX-AllowedIdle-r17", "true");
  }
  if (edrx_allowed_inactive_r17_present) {
    j.write_str("eDRX-AllowedInactive-r17", "true");
  }
  if (intra_freq_resel_red_cap_r17_present) {
    j.write_str("intraFreqReselectionRedCap-r17", intra_freq_resel_red_cap_r17.to_string());
  }
  if (cell_barred_ntn_r17_present) {
    j.write_str("cellBarredNTN-r17", cell_barred_ntn_r17.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* sib1_v1700_ies_s::intra_freq_resel_red_cap_r17_opts::to_string() const
{
  static const char* names[] = {"allowed", "notAllowed"};
  return convert_enum_idx(names, 2, value, "sib1_v1700_ies_s::intra_freq_resel_red_cap_r17_e_");
}

const char* sib1_v1700_ies_s::cell_barred_ntn_r17_opts::to_string() const
{
  static const char* names[] = {"barred", "notBarred"};
  return convert_enum_idx(names, 2, value, "sib1_v1700_ies_s::cell_barred_ntn_r17_e_");
}

// UAC-AC1-SelectAssistInfo-r16 ::= ENUMERATED
const char* uac_ac1_select_assist_info_r16_opts::to_string() const
{
  static const char* names[] = {"a", "b", "c", "notConfigured"};
  return convert_enum_idx(names, 4, value, "uac_ac1_select_assist_info_r16_e");
}

// PosSI-SchedulingInfo-r16 ::= SEQUENCE
SRSASN_CODE pos_si_sched_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pos_si_request_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(pos_si_request_cfg_sul_r16_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sched_info_list_r16, 1, 32));
  if (pos_si_request_cfg_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_r16.pack(bref));
  }
  if (pos_si_request_cfg_sul_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_sul_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pos_si_request_cfg_red_cap_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pos_si_request_cfg_red_cap_r17.is_present(), 1));
      if (pos_si_request_cfg_red_cap_r17.is_present()) {
        HANDLE_CODE(pos_si_request_cfg_red_cap_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_si_sched_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pos_si_request_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(pos_si_request_cfg_sul_r16_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pos_sched_info_list_r16, bref, 1, 32));
  if (pos_si_request_cfg_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_r16.unpack(bref));
  }
  if (pos_si_request_cfg_sul_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_sul_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(pos_si_request_cfg_red_cap_r17, bref);
      if (pos_si_request_cfg_red_cap_r17.is_present()) {
        HANDLE_CODE(pos_si_request_cfg_red_cap_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pos_si_sched_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("posSchedulingInfoList-r16");
  for (const auto& e1 : pos_sched_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (pos_si_request_cfg_r16_present) {
    j.write_fieldname("posSI-RequestConfig-r16");
    pos_si_request_cfg_r16.to_json(j);
  }
  if (pos_si_request_cfg_sul_r16_present) {
    j.write_fieldname("posSI-RequestConfigSUL-r16");
    pos_si_request_cfg_sul_r16.to_json(j);
  }
  if (ext) {
    if (pos_si_request_cfg_red_cap_r17.is_present()) {
      j.write_fieldname("posSI-RequestConfigRedCap-r17");
      pos_si_request_cfg_red_cap_r17->to_json(j);
    }
  }
  j.end_obj();
}

// SIB1-v1630-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1630_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_barr_info_v1630_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (uac_barr_info_v1630_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info_v1630.uac_ac1_select_assist_info_r16, 2, 12));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1630_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_barr_info_v1630_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (uac_barr_info_v1630_present) {
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info_v1630.uac_ac1_select_assist_info_r16, bref, 2, 12));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1630_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (uac_barr_info_v1630_present) {
    j.write_fieldname("uac-BarringInfo-v1630");
    j.start_obj();
    j.start_array("uac-AC1-SelectAssistInfo-r16");
    for (const auto& e1 : uac_barr_info_v1630.uac_ac1_select_assist_info_r16) {
      j.write_str(e1.to_string());
    }
    j.end_array();
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SIB10-r16 ::= SEQUENCE
SRSASN_CODE sib10_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(hrnn_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (hrnn_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, hrnn_list_r16, 1, 12));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib10_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool hrnn_list_r16_present;
  HANDLE_CODE(bref.unpack(hrnn_list_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (hrnn_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(hrnn_list_r16, bref, 1, 12));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib10_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hrnn_list_r16.size() > 0) {
    j.start_array("hrnn-List-r16");
    for (const auto& e1 : hrnn_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB11-r16 ::= SEQUENCE
SRSASN_CODE sib11_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_idle_cfg_sib_r16_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (meas_idle_cfg_sib_r16_present) {
    HANDLE_CODE(meas_idle_cfg_sib_r16.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib11_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_idle_cfg_sib_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (meas_idle_cfg_sib_r16_present) {
    HANDLE_CODE(meas_idle_cfg_sib_r16.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib11_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_idle_cfg_sib_r16_present) {
    j.write_fieldname("measIdleConfigSIB-r16");
    meas_idle_cfg_sib_r16.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB12-r16 ::= SEQUENCE
SRSASN_CODE sib12_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, segment_num_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r16.pack(bref));
  HANDLE_CODE(segment_container_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib12_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(segment_num_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r16.unpack(bref));
  HANDLE_CODE(segment_container_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib12_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("segmentNumber-r16", segment_num_r16);
  j.write_str("segmentType-r16", segment_type_r16.to_string());
  j.write_str("segmentContainer-r16", segment_container_r16.to_string());
  j.end_obj();
}

const char* sib12_r16_s::segment_type_r16_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib12_r16_s::segment_type_r16_e_");
}

// SIB13-r16 ::= SEQUENCE
SRSASN_CODE sib13_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(sl_v2x_cfg_common_r16.pack(bref));
  HANDLE_CODE(dummy.pack(bref));
  HANDLE_CODE(tdd_cfg_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib13_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sl_v2x_cfg_common_r16.unpack(bref));
  HANDLE_CODE(dummy.unpack(bref));
  HANDLE_CODE(tdd_cfg_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib13_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-V2X-ConfigCommon-r16", sl_v2x_cfg_common_r16.to_string());
  j.write_str("dummy", dummy.to_string());
  j.write_str("tdd-Config-r16", tdd_cfg_r16.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB14-r16 ::= SEQUENCE
SRSASN_CODE sib14_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(sl_v2x_cfg_common_ext_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib14_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sl_v2x_cfg_common_ext_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib14_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-V2X-ConfigCommonExt-r16", sl_v2x_cfg_common_ext_r16.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB15-r17 ::= SEQUENCE
SRSASN_CODE sib15_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(common_plmns_with_disaster_condition_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(applicable_disaster_info_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (common_plmns_with_disaster_condition_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, common_plmns_with_disaster_condition_r17, 1, 12));
  }
  if (applicable_disaster_info_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, applicable_disaster_info_list_r17, 1, 12));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib15_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool common_plmns_with_disaster_condition_r17_present;
  HANDLE_CODE(bref.unpack(common_plmns_with_disaster_condition_r17_present, 1));
  bool applicable_disaster_info_list_r17_present;
  HANDLE_CODE(bref.unpack(applicable_disaster_info_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (common_plmns_with_disaster_condition_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(common_plmns_with_disaster_condition_r17, bref, 1, 12));
  }
  if (applicable_disaster_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(applicable_disaster_info_list_r17, bref, 1, 12));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib15_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (common_plmns_with_disaster_condition_r17.size() > 0) {
    j.start_array("commonPLMNsWithDisasterCondition-r17");
    for (const auto& e1 : common_plmns_with_disaster_condition_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (applicable_disaster_info_list_r17.size() > 0) {
    j.start_array("applicableDisasterInfoList-r17");
    for (const auto& e1 : applicable_disaster_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB16-r17 ::= SEQUENCE
SRSASN_CODE sib16_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_prio_list_slicing_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (freq_prio_list_slicing_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_slicing_r17, 1, 9));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib16_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_prio_list_slicing_r17_present;
  HANDLE_CODE(bref.unpack(freq_prio_list_slicing_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (freq_prio_list_slicing_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_slicing_r17, bref, 1, 9));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib16_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_prio_list_slicing_r17.size() > 0) {
    j.start_array("freqPriorityListSlicing-r17");
    for (const auto& e1 : freq_prio_list_slicing_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB17-r17 ::= SEQUENCE
SRSASN_CODE sib17_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, segment_num_r17, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r17.pack(bref));
  HANDLE_CODE(segment_container_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib17_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(segment_num_r17, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r17.unpack(bref));
  HANDLE_CODE(segment_container_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib17_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("segmentNumber-r17", segment_num_r17);
  j.write_str("segmentType-r17", segment_type_r17.to_string());
  j.write_str("segmentContainer-r17", segment_container_r17.to_string());
  j.end_obj();
}

const char* sib17_r17_s::segment_type_r17_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib17_r17_s::segment_type_r17_e_");
}

// SIB18-r17 ::= SEQUENCE
SRSASN_CODE sib18_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gin_elem_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(gins_per_sn_pn_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (gin_elem_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gin_elem_list_r17, 1, 24));
  }
  if (gins_per_sn_pn_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gins_per_sn_pn_list_r17, 1, 12));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib18_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool gin_elem_list_r17_present;
  HANDLE_CODE(bref.unpack(gin_elem_list_r17_present, 1));
  bool gins_per_sn_pn_list_r17_present;
  HANDLE_CODE(bref.unpack(gins_per_sn_pn_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (gin_elem_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gin_elem_list_r17, bref, 1, 24));
  }
  if (gins_per_sn_pn_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gins_per_sn_pn_list_r17, bref, 1, 12));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib18_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gin_elem_list_r17.size() > 0) {
    j.start_array("gin-ElementList-r17");
    for (const auto& e1 : gin_elem_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (gins_per_sn_pn_list_r17.size() > 0) {
    j.start_array("gins-PerSNPN-List-r17");
    for (const auto& e1 : gins_per_sn_pn_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB19-r17 ::= SEQUENCE
SRSASN_CODE sib19_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(t_service_r17_present, 1));
  HANDLE_CODE(bref.pack(ref_location_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(distance_thresh_r17_present, 1));
  HANDLE_CODE(bref.pack(ntn_neigh_cell_cfg_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.pack(bref));
  }
  if (t_service_r17_present) {
    HANDLE_CODE(pack_integer(bref, t_service_r17, (uint64_t)0u, (uint64_t)549755813887u));
  }
  if (ref_location_r17.size() > 0) {
    HANDLE_CODE(ref_location_r17.pack(bref));
  }
  if (distance_thresh_r17_present) {
    HANDLE_CODE(pack_integer(bref, distance_thresh_r17, (uint16_t)0u, (uint16_t)65525u));
  }
  if (ntn_neigh_cell_cfg_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ntn_neigh_cell_cfg_list_r17, 1, 4));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ntn_neigh_cell_cfg_list_ext_v1720.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ntn_neigh_cell_cfg_list_ext_v1720.is_present(), 1));
      if (ntn_neigh_cell_cfg_list_ext_v1720.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ntn_neigh_cell_cfg_list_ext_v1720, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib19_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(t_service_r17_present, 1));
  bool ref_location_r17_present;
  HANDLE_CODE(bref.unpack(ref_location_r17_present, 1));
  HANDLE_CODE(bref.unpack(distance_thresh_r17_present, 1));
  bool ntn_neigh_cell_cfg_list_r17_present;
  HANDLE_CODE(bref.unpack(ntn_neigh_cell_cfg_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.unpack(bref));
  }
  if (t_service_r17_present) {
    HANDLE_CODE(unpack_integer(t_service_r17, bref, (uint64_t)0u, (uint64_t)549755813887u));
  }
  if (ref_location_r17_present) {
    HANDLE_CODE(ref_location_r17.unpack(bref));
  }
  if (distance_thresh_r17_present) {
    HANDLE_CODE(unpack_integer(distance_thresh_r17, bref, (uint16_t)0u, (uint16_t)65525u));
  }
  if (ntn_neigh_cell_cfg_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ntn_neigh_cell_cfg_list_r17, bref, 1, 4));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ntn_neigh_cell_cfg_list_ext_v1720, bref);
      if (ntn_neigh_cell_cfg_list_ext_v1720.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ntn_neigh_cell_cfg_list_ext_v1720, bref, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib19_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ntn_cfg_r17_present) {
    j.write_fieldname("ntn-Config-r17");
    ntn_cfg_r17.to_json(j);
  }
  if (t_service_r17_present) {
    j.write_int("t-Service-r17", t_service_r17);
  }
  if (ref_location_r17.size() > 0) {
    j.write_str("referenceLocation-r17", ref_location_r17.to_string());
  }
  if (distance_thresh_r17_present) {
    j.write_int("distanceThresh-r17", distance_thresh_r17);
  }
  if (ntn_neigh_cell_cfg_list_r17.size() > 0) {
    j.start_array("ntn-NeighCellConfigList-r17");
    for (const auto& e1 : ntn_neigh_cell_cfg_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (ntn_neigh_cell_cfg_list_ext_v1720.is_present()) {
      j.start_array("ntn-NeighCellConfigListExt-v1720");
      for (const auto& e1 : *ntn_neigh_cell_cfg_list_ext_v1720) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SIB2 ::= SEQUENCE
SRSASN_CODE sib2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  bref.pack(cell_resel_info_common.ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_info_common.nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_info_common.range_to_best_cell_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_info_common.speed_state_resel_pars_present, 1));
  if (cell_resel_info_common.nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_info_common.nrof_ss_blocks_to_average, (uint8_t)2u, (uint8_t)16u));
  }
  if (cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(cell_resel_info_common.abs_thresh_ss_blocks_consolidation.pack(bref));
  }
  if (cell_resel_info_common.range_to_best_cell_present) {
    HANDLE_CODE(cell_resel_info_common.range_to_best_cell.pack(bref));
  }
  HANDLE_CODE(cell_resel_info_common.q_hyst.pack(bref));
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.mob_state_params.pack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.pack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.pack(bref));
  }
  bref.pack(cell_resel_serving_freq_info.ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.s_non_intra_search_p_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.s_non_intra_search_q_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.thresh_serving_low_q_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.cell_resel_sub_prio_present, 1));
  if (cell_resel_serving_freq_info.s_non_intra_search_p_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.s_non_intra_search_p, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_serving_freq_info.s_non_intra_search_q_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.s_non_intra_search_q, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.thresh_serving_low_p, (uint8_t)0u, (uint8_t)31u));
  if (cell_resel_serving_freq_info.thresh_serving_low_q_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.thresh_serving_low_q, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_serving_freq_info.cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_serving_freq_info.cell_resel_sub_prio.pack(bref));
  }
  HANDLE_CODE(intra_freq_cell_resel_info.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= relaxed_meas_r16.is_present();
    group_flags[1] |= cell_equivalent_size_r17_present;
    group_flags[1] |= relaxed_meas_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(relaxed_meas_r16.is_present(), 1));
      if (relaxed_meas_r16.is_present()) {
        HANDLE_CODE(bref.pack(relaxed_meas_r16->low_mob_eval_r16_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r16->cell_edge_eval_r16_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r16->combine_relaxed_meas_condition_r16_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r16->high_prio_meas_relax_r16_present, 1));
        if (relaxed_meas_r16->low_mob_eval_r16_present) {
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.s_search_delta_p_r16.pack(bref));
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.t_search_delta_p_r16.pack(bref));
        }
        if (relaxed_meas_r16->cell_edge_eval_r16_present) {
          HANDLE_CODE(bref.pack(relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present, 1));
          HANDLE_CODE(
              pack_integer(bref, relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_p_r16, (uint8_t)0u, (uint8_t)31u));
          if (relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present) {
            HANDLE_CODE(pack_integer(
                bref, relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16, (uint8_t)0u, (uint8_t)31u));
          }
        }
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_equivalent_size_r17_present, 1));
      HANDLE_CODE(bref.pack(relaxed_meas_r17.is_present(), 1));
      if (cell_equivalent_size_r17_present) {
        HANDLE_CODE(pack_integer(bref, cell_equivalent_size_r17, (uint8_t)2u, (uint8_t)16u));
      }
      if (relaxed_meas_r17.is_present()) {
        HANDLE_CODE(bref.pack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r17->combine_relaxed_meas_condition2_r17_present, 1));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.s_search_delta_p_stationary_r17.pack(bref));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.t_search_delta_p_stationary_r17.pack(bref));
        if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present) {
          HANDLE_CODE(
              bref.pack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present, 1));
          HANDLE_CODE(pack_integer(bref,
                                   relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_p2_r17,
                                   (uint8_t)0u,
                                   (uint8_t)31u));
          if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present) {
            HANDLE_CODE(pack_integer(bref,
                                     relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17,
                                     (uint8_t)0u,
                                     (uint8_t)31u));
          }
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bref.unpack(cell_resel_info_common.ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_info_common.nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_info_common.range_to_best_cell_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_info_common.speed_state_resel_pars_present, 1));
  if (cell_resel_info_common.nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(unpack_integer(cell_resel_info_common.nrof_ss_blocks_to_average, bref, (uint8_t)2u, (uint8_t)16u));
  }
  if (cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(cell_resel_info_common.abs_thresh_ss_blocks_consolidation.unpack(bref));
  }
  if (cell_resel_info_common.range_to_best_cell_present) {
    HANDLE_CODE(cell_resel_info_common.range_to_best_cell.unpack(bref));
  }
  HANDLE_CODE(cell_resel_info_common.q_hyst.unpack(bref));
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.mob_state_params.unpack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.unpack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.unpack(bref));
  }
  bref.unpack(cell_resel_serving_freq_info.ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.s_non_intra_search_p_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.s_non_intra_search_q_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.thresh_serving_low_q_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.cell_resel_sub_prio_present, 1));
  if (cell_resel_serving_freq_info.s_non_intra_search_p_present) {
    HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.s_non_intra_search_p, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_serving_freq_info.s_non_intra_search_q_present) {
    HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.s_non_intra_search_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.thresh_serving_low_p, bref, (uint8_t)0u, (uint8_t)31u));
  if (cell_resel_serving_freq_info.thresh_serving_low_q_present) {
    HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.thresh_serving_low_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_serving_freq_info.cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_serving_freq_info.cell_resel_sub_prio.unpack(bref));
  }
  HANDLE_CODE(intra_freq_cell_resel_info.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(relaxed_meas_r16, bref);
      if (relaxed_meas_r16.is_present()) {
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->low_mob_eval_r16_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->cell_edge_eval_r16_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->combine_relaxed_meas_condition_r16_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->high_prio_meas_relax_r16_present, 1));
        if (relaxed_meas_r16->low_mob_eval_r16_present) {
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.s_search_delta_p_r16.unpack(bref));
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.t_search_delta_p_r16.unpack(bref));
        }
        if (relaxed_meas_r16->cell_edge_eval_r16_present) {
          HANDLE_CODE(bref.unpack(relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present, 1));
          HANDLE_CODE(unpack_integer(
              relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_p_r16, bref, (uint8_t)0u, (uint8_t)31u));
          if (relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present) {
            HANDLE_CODE(unpack_integer(
                relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16, bref, (uint8_t)0u, (uint8_t)31u));
          }
        }
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cell_equivalent_size_r17_present, 1));
      unpack_presence_flag(relaxed_meas_r17, bref);
      if (cell_equivalent_size_r17_present) {
        HANDLE_CODE(unpack_integer(cell_equivalent_size_r17, bref, (uint8_t)2u, (uint8_t)16u));
      }
      if (relaxed_meas_r17.is_present()) {
        HANDLE_CODE(bref.unpack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r17->combine_relaxed_meas_condition2_r17_present, 1));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.s_search_delta_p_stationary_r17.unpack(bref));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.t_search_delta_p_stationary_r17.unpack(bref));
        if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present) {
          HANDLE_CODE(
              bref.unpack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present, 1));
          HANDLE_CODE(unpack_integer(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_p2_r17,
                                     bref,
                                     (uint8_t)0u,
                                     (uint8_t)31u));
          if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present) {
            HANDLE_CODE(unpack_integer(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17,
                                       bref,
                                       (uint8_t)0u,
                                       (uint8_t)31u));
          }
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellReselectionInfoCommon");
  j.start_obj();
  if (cell_resel_info_common.nrof_ss_blocks_to_average_present) {
    j.write_int("nrofSS-BlocksToAverage", cell_resel_info_common.nrof_ss_blocks_to_average);
  }
  if (cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present) {
    j.write_fieldname("absThreshSS-BlocksConsolidation");
    cell_resel_info_common.abs_thresh_ss_blocks_consolidation.to_json(j);
  }
  if (cell_resel_info_common.range_to_best_cell_present) {
    j.write_str("rangeToBestCell", cell_resel_info_common.range_to_best_cell.to_string());
  }
  j.write_str("q-Hyst", cell_resel_info_common.q_hyst.to_string());
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    j.write_fieldname("speedStateReselectionPars");
    j.start_obj();
    j.write_fieldname("mobilityStateParameters");
    cell_resel_info_common.speed_state_resel_pars.mob_state_params.to_json(j);
    j.write_fieldname("q-HystSF");
    j.start_obj();
    j.write_str("sf-Medium", cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.to_string());
    j.write_str("sf-High", cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.to_string());
    j.end_obj();
    j.end_obj();
  }
  j.end_obj();
  j.write_fieldname("cellReselectionServingFreqInfo");
  j.start_obj();
  if (cell_resel_serving_freq_info.s_non_intra_search_p_present) {
    j.write_int("s-NonIntraSearchP", cell_resel_serving_freq_info.s_non_intra_search_p);
  }
  if (cell_resel_serving_freq_info.s_non_intra_search_q_present) {
    j.write_int("s-NonIntraSearchQ", cell_resel_serving_freq_info.s_non_intra_search_q);
  }
  j.write_int("threshServingLowP", cell_resel_serving_freq_info.thresh_serving_low_p);
  if (cell_resel_serving_freq_info.thresh_serving_low_q_present) {
    j.write_int("threshServingLowQ", cell_resel_serving_freq_info.thresh_serving_low_q);
  }
  j.write_int("cellReselectionPriority", cell_resel_serving_freq_info.cell_resel_prio);
  if (cell_resel_serving_freq_info.cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_serving_freq_info.cell_resel_sub_prio.to_string());
  }
  j.end_obj();
  j.write_fieldname("intraFreqCellReselectionInfo");
  intra_freq_cell_resel_info.to_json(j);
  if (ext) {
    if (relaxed_meas_r16.is_present()) {
      j.write_fieldname("relaxedMeasurement-r16");
      j.start_obj();
      if (relaxed_meas_r16->low_mob_eval_r16_present) {
        j.write_fieldname("lowMobilityEvaluation-r16");
        j.start_obj();
        j.write_str("s-SearchDeltaP-r16", relaxed_meas_r16->low_mob_eval_r16.s_search_delta_p_r16.to_string());
        j.write_str("t-SearchDeltaP-r16", relaxed_meas_r16->low_mob_eval_r16.t_search_delta_p_r16.to_string());
        j.end_obj();
      }
      if (relaxed_meas_r16->cell_edge_eval_r16_present) {
        j.write_fieldname("cellEdgeEvaluation-r16");
        j.start_obj();
        j.write_int("s-SearchThresholdP-r16", relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_p_r16);
        if (relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present) {
          j.write_int("s-SearchThresholdQ-r16", relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16);
        }
        j.end_obj();
      }
      if (relaxed_meas_r16->combine_relaxed_meas_condition_r16_present) {
        j.write_str("combineRelaxedMeasCondition-r16", "true");
      }
      if (relaxed_meas_r16->high_prio_meas_relax_r16_present) {
        j.write_str("highPriorityMeasRelax-r16", "true");
      }
      j.end_obj();
    }
    if (cell_equivalent_size_r17_present) {
      j.write_int("cellEquivalentSize-r17", cell_equivalent_size_r17);
    }
    if (relaxed_meas_r17.is_present()) {
      j.write_fieldname("relaxedMeasurement-r17");
      j.start_obj();
      j.write_fieldname("stationaryMobilityEvaluation-r17");
      j.start_obj();
      j.write_str("s-SearchDeltaP-Stationary-r17",
                  relaxed_meas_r17->stationary_mob_eval_r17.s_search_delta_p_stationary_r17.to_string());
      j.write_str("t-SearchDeltaP-Stationary-r17",
                  relaxed_meas_r17->stationary_mob_eval_r17.t_search_delta_p_stationary_r17.to_string());
      j.end_obj();
      if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present) {
        j.write_fieldname("cellEdgeEvaluationWhileStationary-r17");
        j.start_obj();
        j.write_int("s-SearchThresholdP2-r17",
                    relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_p2_r17);
        if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present) {
          j.write_int("s-SearchThresholdQ2-r17",
                      relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17);
        }
        j.end_obj();
      }
      if (relaxed_meas_r17->combine_relaxed_meas_condition2_r17_present) {
        j.write_str("combineRelaxedMeasCondition2-r17", "true");
      }
      j.end_obj();
    }
  }
  j.end_obj();
}

const char* sib2_s::cell_resel_info_common_s_::q_hyst_opts::to_string() const
{
  static const char* names[] = {"dB0",
                                "dB1",
                                "dB2",
                                "dB3",
                                "dB4",
                                "dB5",
                                "dB6",
                                "dB8",
                                "dB10",
                                "dB12",
                                "dB14",
                                "dB16",
                                "dB18",
                                "dB20",
                                "dB22",
                                "dB24"};
  return convert_enum_idx(names, 16, value, "sib2_s::cell_resel_info_common_s_::q_hyst_e_");
}
uint8_t sib2_s::cell_resel_info_common_s_::q_hyst_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 16, value, "sib2_s::cell_resel_info_common_s_::q_hyst_e_");
}

const char*
sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_string() const
{
  static const char* names[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      names, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_number() const
{
  static const int8_t numbers[] = {-6, -4, -2, 0};
  return map_enum_number(
      numbers, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}

const char* sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_string() const
{
  static const char* names[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      names, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_number() const
{
  static const int8_t numbers[] = {-6, -4, -2, 0};
  return map_enum_number(
      numbers, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}

SRSASN_CODE sib2_s::intra_freq_cell_resel_info_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_present, 1));
  HANDLE_CODE(bref.pack(s_intra_search_q_present, 1));
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_list_sul.size() > 0, 1));
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(smtc_present, 1));
  HANDLE_CODE(bref.pack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.pack(ssb_to_measure_present, 1));

  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_sul, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min, (int8_t)-43, (int8_t)-12));
  }
  HANDLE_CODE(pack_integer(bref, s_intra_search_p, (uint8_t)0u, (uint8_t)31u));
  if (s_intra_search_q_present) {
    HANDLE_CODE(pack_integer(bref, s_intra_search_q, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_nr, (uint8_t)0u, (uint8_t)7u));
  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8));
  }
  if (freq_band_list_sul.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_sul, 1, 8));
  }
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.pack(bref));
  }
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.pack(bref));
  }
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.pack(bref));
  }
  HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= t_resel_nr_sf.is_present();
    group_flags[1] |= smtc2_lp_r16.is_present();
    group_flags[1] |= ssb_position_qcl_common_r16_present;
    group_flags[2] |= ssb_position_qcl_common_r17_present;
    group_flags[3] |= smtc4list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t_resel_nr_sf.is_present(), 1));
      if (t_resel_nr_sf.is_present()) {
        HANDLE_CODE(t_resel_nr_sf->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc2_lp_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_common_r16_present, 1));
      if (smtc2_lp_r16.is_present()) {
        HANDLE_CODE(smtc2_lp_r16->pack(bref));
      }
      if (ssb_position_qcl_common_r16_present) {
        HANDLE_CODE(ssb_position_qcl_common_r16.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ssb_position_qcl_common_r17_present, 1));
      if (ssb_position_qcl_common_r17_present) {
        HANDLE_CODE(ssb_position_qcl_common_r17.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc4list_r17.is_present(), 1));
      if (smtc4list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *smtc4list_r17, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib2_s::intra_freq_cell_resel_info_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_present, 1));
  HANDLE_CODE(bref.unpack(s_intra_search_q_present, 1));
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  bool freq_band_list_sul_present;
  HANDLE_CODE(bref.unpack(freq_band_list_sul_present, 1));
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(smtc_present, 1));
  HANDLE_CODE(bref.unpack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.unpack(ssb_to_measure_present, 1));

  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_sul, bref, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(unpack_integer(q_qual_min, bref, (int8_t)-43, (int8_t)-12));
  }
  HANDLE_CODE(unpack_integer(s_intra_search_p, bref, (uint8_t)0u, (uint8_t)31u));
  if (s_intra_search_q_present) {
    HANDLE_CODE(unpack_integer(s_intra_search_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(t_resel_nr, bref, (uint8_t)0u, (uint8_t)7u));
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8));
  }
  if (freq_band_list_sul_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_sul, bref, 1, 8));
  }
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.unpack(bref));
  }
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.unpack(bref));
  }
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(t_resel_nr_sf, bref);
      if (t_resel_nr_sf.is_present()) {
        HANDLE_CODE(t_resel_nr_sf->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(smtc2_lp_r16, bref);
      HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r16_present, 1));
      if (smtc2_lp_r16.is_present()) {
        HANDLE_CODE(smtc2_lp_r16->unpack(bref));
      }
      if (ssb_position_qcl_common_r16_present) {
        HANDLE_CODE(ssb_position_qcl_common_r16.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r17_present, 1));
      if (ssb_position_qcl_common_r17_present) {
        HANDLE_CODE(ssb_position_qcl_common_r17.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(smtc4list_r17, bref);
      if (smtc4list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*smtc4list_r17, bref, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib2_s::intra_freq_cell_resel_info_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-RxLevMin", q_rx_lev_min);
  if (q_rx_lev_min_sul_present) {
    j.write_int("q-RxLevMinSUL", q_rx_lev_min_sul);
  }
  if (q_qual_min_present) {
    j.write_int("q-QualMin", q_qual_min);
  }
  j.write_int("s-IntraSearchP", s_intra_search_p);
  if (s_intra_search_q_present) {
    j.write_int("s-IntraSearchQ", s_intra_search_q);
  }
  j.write_int("t-ReselectionNR", t_resel_nr);
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_list_sul.size() > 0) {
    j.start_array("frequencyBandListSUL");
    for (const auto& e1 : freq_band_list_sul) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  if (smtc_present) {
    j.write_fieldname("smtc");
    smtc.to_json(j);
  }
  if (ss_rssi_meas_present) {
    j.write_fieldname("ss-RSSI-Measurement");
    ss_rssi_meas.to_json(j);
  }
  if (ssb_to_measure_present) {
    j.write_fieldname("ssb-ToMeasure");
    ssb_to_measure.to_json(j);
  }
  j.write_bool("deriveSSB-IndexFromCell", derive_ssb_idx_from_cell);
  if (ext) {
    if (t_resel_nr_sf.is_present()) {
      j.write_fieldname("t-ReselectionNR-SF");
      t_resel_nr_sf->to_json(j);
    }
    if (smtc2_lp_r16.is_present()) {
      j.write_fieldname("smtc2-LP-r16");
      smtc2_lp_r16->to_json(j);
    }
    if (ssb_position_qcl_common_r16_present) {
      j.write_str("ssb-PositionQCL-Common-r16", ssb_position_qcl_common_r16.to_string());
    }
    if (ssb_position_qcl_common_r17_present) {
      j.write_str("ssb-PositionQCL-Common-r17", ssb_position_qcl_common_r17.to_string());
    }
    if (smtc4list_r17.is_present()) {
      j.start_array("smtc4list-r17");
      for (const auto& e1 : *smtc4list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_opts::to_string() const
{
  static const char* names[] = {"dB3", "dB6", "dB9", "dB12", "dB15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_e_");
}
uint8_t sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 5, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_e_");
}

const char* sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_opts::to_string() const
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
      names, 16, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_e_");
}
uint16_t sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_e_");
}

const char*
sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_opts::to_string() const
{
  static const char* names[] = {"dB2", "dB3", "dB6", "dB9", "dB12", "dB15", "spare2", "spare1"};
  return convert_enum_idx(
      names, 8, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_e_");
}
uint8_t sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 6, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_e_");
}

const char*
sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_opts::to_string() const
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
      names, 16, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_e_");
}
uint16_t
sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_e_");
}

// SIB20-r17 ::= SEQUENCE
SRSASN_CODE sib20_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cfr_cfg_mcch_mtch_r17_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(mcch_cfg_r17.pack(bref));
  if (cfr_cfg_mcch_mtch_r17_present) {
    HANDLE_CODE(cfr_cfg_mcch_mtch_r17.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib20_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cfr_cfg_mcch_mtch_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(mcch_cfg_r17.unpack(bref));
  if (cfr_cfg_mcch_mtch_r17_present) {
    HANDLE_CODE(cfr_cfg_mcch_mtch_r17.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib20_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mcch-Config-r17");
  mcch_cfg_r17.to_json(j);
  if (cfr_cfg_mcch_mtch_r17_present) {
    j.write_fieldname("cfr-ConfigMCCH-MTCH-r17");
    cfr_cfg_mcch_mtch_r17.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB21-r17 ::= SEQUENCE
SRSASN_CODE sib21_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbs_fsai_intra_freq_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(mbs_fsai_inter_freq_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (mbs_fsai_intra_freq_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_fsai_intra_freq_r17, 1, 64));
  }
  if (mbs_fsai_inter_freq_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_fsai_inter_freq_list_r17, 1, 8));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib21_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool mbs_fsai_intra_freq_r17_present;
  HANDLE_CODE(bref.unpack(mbs_fsai_intra_freq_r17_present, 1));
  bool mbs_fsai_inter_freq_list_r17_present;
  HANDLE_CODE(bref.unpack(mbs_fsai_inter_freq_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (mbs_fsai_intra_freq_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_fsai_intra_freq_r17, bref, 1, 64));
  }
  if (mbs_fsai_inter_freq_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_fsai_inter_freq_list_r17, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib21_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbs_fsai_intra_freq_r17.size() > 0) {
    j.start_array("mbs-FSAI-IntraFreq-r17");
    for (const auto& e1 : mbs_fsai_intra_freq_r17) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (mbs_fsai_inter_freq_list_r17.size() > 0) {
    j.start_array("mbs-FSAI-InterFreqList-r17");
    for (const auto& e1 : mbs_fsai_inter_freq_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB3 ::= SEQUENCE
SRSASN_CODE sib3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(intra_freq_excluded_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (intra_freq_neigh_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_neigh_cell_list, 1, 16));
  }
  if (intra_freq_excluded_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_excluded_cell_list, 1, 16));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= intra_freq_neigh_cell_list_v1610.is_present();
    group_flags[0] |= intra_freq_allowed_cell_list_r16.is_present();
    group_flags[0] |= intra_freq_cag_cell_list_r16.is_present();
    group_flags[1] |= intra_freq_neigh_hsdn_cell_list_r17.is_present();
    group_flags[1] |= intra_freq_neigh_cell_list_v1710.is_present();
    group_flags[2] |= ch_access_mode2_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_allowed_cell_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_cag_cell_list_r16.is_present(), 1));
      if (intra_freq_neigh_cell_list_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_cell_list_v1610, 1, 16));
      }
      if (intra_freq_allowed_cell_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_allowed_cell_list_r16, 1, 16));
      }
      if (intra_freq_cag_cell_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_cag_cell_list_r16, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_neigh_hsdn_cell_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_v1710.is_present(), 1));
      if (intra_freq_neigh_hsdn_cell_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_hsdn_cell_list_r17, 1, 16));
      }
      if (intra_freq_neigh_cell_list_v1710.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_cell_list_v1710, 1, 16));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ch_access_mode2_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool intra_freq_neigh_cell_list_present;
  HANDLE_CODE(bref.unpack(intra_freq_neigh_cell_list_present, 1));
  bool intra_freq_excluded_cell_list_present;
  HANDLE_CODE(bref.unpack(intra_freq_excluded_cell_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (intra_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_neigh_cell_list, bref, 1, 16));
  }
  if (intra_freq_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_excluded_cell_list, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(intra_freq_neigh_cell_list_v1610, bref);
      unpack_presence_flag(intra_freq_allowed_cell_list_r16, bref);
      unpack_presence_flag(intra_freq_cag_cell_list_r16, bref);
      if (intra_freq_neigh_cell_list_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_cell_list_v1610, bref, 1, 16));
      }
      if (intra_freq_allowed_cell_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_allowed_cell_list_r16, bref, 1, 16));
      }
      if (intra_freq_cag_cell_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_cag_cell_list_r16, bref, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(intra_freq_neigh_hsdn_cell_list_r17, bref);
      unpack_presence_flag(intra_freq_neigh_cell_list_v1710, bref);
      if (intra_freq_neigh_hsdn_cell_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_hsdn_cell_list_r17, bref, 1, 16));
      }
      if (intra_freq_neigh_cell_list_v1710.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_cell_list_v1710, bref, 1, 16));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ch_access_mode2_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sib3_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_neigh_cell_list.size() > 0) {
    j.start_array("intraFreqNeighCellList");
    for (const auto& e1 : intra_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (intra_freq_excluded_cell_list.size() > 0) {
    j.start_array("intraFreqExcludedCellList");
    for (const auto& e1 : intra_freq_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (intra_freq_neigh_cell_list_v1610.is_present()) {
      j.start_array("intraFreqNeighCellList-v1610");
      for (const auto& e1 : *intra_freq_neigh_cell_list_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_allowed_cell_list_r16.is_present()) {
      j.start_array("intraFreqAllowedCellList-r16");
      for (const auto& e1 : *intra_freq_allowed_cell_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_cag_cell_list_r16.is_present()) {
      j.start_array("intraFreqCAG-CellList-r16");
      for (const auto& e1 : *intra_freq_cag_cell_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_neigh_hsdn_cell_list_r17.is_present()) {
      j.start_array("intraFreqNeighHSDN-CellList-r17");
      for (const auto& e1 : *intra_freq_neigh_hsdn_cell_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_neigh_cell_list_v1710.is_present()) {
      j.start_array("intraFreqNeighCellList-v1710");
      for (const auto& e1 : *intra_freq_neigh_cell_list_v1710) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (ch_access_mode2_r17_present) {
      j.write_str("channelAccessMode2-r17", "enabled");
    }
  }
  j.end_obj();
}

// SIB4 ::= SEQUENCE
SRSASN_CODE sib4_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list, 1, 8));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= inter_freq_carrier_freq_list_v1610.is_present();
    group_flags[1] |= inter_freq_carrier_freq_list_v1700.is_present();
    group_flags[2] |= inter_freq_carrier_freq_list_v1720.is_present();
    group_flags[3] |= inter_freq_carrier_freq_list_v1730.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1610.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1610, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1700.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1700, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1720.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1720.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1720, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1730.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1730.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1730, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib4_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list, bref, 1, 8));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1610, bref);
      if (inter_freq_carrier_freq_list_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1610, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1700, bref);
      if (inter_freq_carrier_freq_list_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1700, bref, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1720, bref);
      if (inter_freq_carrier_freq_list_v1720.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1720, bref, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1730, bref);
      if (inter_freq_carrier_freq_list_v1730.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1730, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib4_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("interFreqCarrierFreqList");
  for (const auto& e1 : inter_freq_carrier_freq_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (inter_freq_carrier_freq_list_v1610.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1610");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1700.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1700");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1720.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1720");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1720) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1730.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1730");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1730) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SIB5 ::= SEQUENCE
SRSASN_CODE sib5_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_freq_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(t_resel_eutra_sf_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (carrier_freq_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freq_list_eutra, 1, 8));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_eutra, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_present) {
    HANDLE_CODE(t_resel_eutra_sf.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= carrier_freq_list_eutra_v1610.is_present();
    group_flags[1] |= carrier_freq_list_eutra_v1700.is_present();
    group_flags[1] |= idle_mode_meas_voice_fallback_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_eutra_v1610.is_present(), 1));
      if (carrier_freq_list_eutra_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_eutra_v1610, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_eutra_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(idle_mode_meas_voice_fallback_r17_present, 1));
      if (carrier_freq_list_eutra_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_eutra_v1700, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib5_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool carrier_freq_list_eutra_present;
  HANDLE_CODE(bref.unpack(carrier_freq_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_eutra_sf_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (carrier_freq_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freq_list_eutra, bref, 1, 8));
  }
  HANDLE_CODE(unpack_integer(t_resel_eutra, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_present) {
    HANDLE_CODE(t_resel_eutra_sf.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(carrier_freq_list_eutra_v1610, bref);
      if (carrier_freq_list_eutra_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_eutra_v1610, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(carrier_freq_list_eutra_v1700, bref);
      HANDLE_CODE(bref.unpack(idle_mode_meas_voice_fallback_r17_present, 1));
      if (carrier_freq_list_eutra_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_eutra_v1700, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib5_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_list_eutra.size() > 0) {
    j.start_array("carrierFreqListEUTRA");
    for (const auto& e1 : carrier_freq_list_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("t-ReselectionEUTRA", t_resel_eutra);
  if (t_resel_eutra_sf_present) {
    j.write_fieldname("t-ReselectionEUTRA-SF");
    t_resel_eutra_sf.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (carrier_freq_list_eutra_v1610.is_present()) {
      j.start_array("carrierFreqListEUTRA-v1610");
      for (const auto& e1 : *carrier_freq_list_eutra_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (carrier_freq_list_eutra_v1700.is_present()) {
      j.start_array("carrierFreqListEUTRA-v1700");
      for (const auto& e1 : *carrier_freq_list_eutra_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (idle_mode_meas_voice_fallback_r17_present) {
      j.write_str("idleModeMeasVoiceFallback-r17", "true");
    }
  }
  j.end_obj();
}

// SIB6 ::= SEQUENCE
SRSASN_CODE sib6_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_type.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib6_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_type.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib6_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningType", warning_type.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB7 ::= SEQUENCE
SRSASN_CODE sib7_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_coding_scheme_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_msg_segment_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, warning_msg_segment_num, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.pack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib7_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_coding_scheme_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_msg_segment_type.unpack(bref));
  HANDLE_CODE(unpack_integer(warning_msg_segment_num, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.unpack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib7_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningMessageSegmentType", warning_msg_segment_type.to_string());
  j.write_int("warningMessageSegmentNumber", warning_msg_segment_num);
  j.write_str("warningMessageSegment", warning_msg_segment.to_string());
  if (data_coding_scheme_present) {
    j.write_str("dataCodingScheme", data_coding_scheme.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

const char* sib7_s::warning_msg_segment_type_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib7_s::warning_msg_segment_type_e_");
}

// SIB8 ::= SEQUENCE
SRSASN_CODE sib8_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_coding_scheme_present, 1));
  HANDLE_CODE(bref.pack(warning_area_coordinates_segment.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_msg_segment_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, warning_msg_segment_num, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.pack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.pack(bref));
  }
  if (warning_area_coordinates_segment.size() > 0) {
    HANDLE_CODE(warning_area_coordinates_segment.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib8_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_coding_scheme_present, 1));
  bool warning_area_coordinates_segment_present;
  HANDLE_CODE(bref.unpack(warning_area_coordinates_segment_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_msg_segment_type.unpack(bref));
  HANDLE_CODE(unpack_integer(warning_msg_segment_num, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.unpack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.unpack(bref));
  }
  if (warning_area_coordinates_segment_present) {
    HANDLE_CODE(warning_area_coordinates_segment.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib8_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningMessageSegmentType", warning_msg_segment_type.to_string());
  j.write_int("warningMessageSegmentNumber", warning_msg_segment_num);
  j.write_str("warningMessageSegment", warning_msg_segment.to_string());
  if (data_coding_scheme_present) {
    j.write_str("dataCodingScheme", data_coding_scheme.to_string());
  }
  if (warning_area_coordinates_segment.size() > 0) {
    j.write_str("warningAreaCoordinatesSegment", warning_area_coordinates_segment.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

const char* sib8_s::warning_msg_segment_type_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib8_s::warning_msg_segment_type_e_");
}

// SIB9 ::= SEQUENCE
SRSASN_CODE sib9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(time_info_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (time_info_present) {
    HANDLE_CODE(bref.pack(time_info.day_light_saving_time_present, 1));
    HANDLE_CODE(bref.pack(time_info.leap_seconds_present, 1));
    HANDLE_CODE(bref.pack(time_info.local_time_offset_present, 1));
    HANDLE_CODE(pack_integer(bref, time_info.time_info_utc, (uint64_t)0u, (uint64_t)549755813887u));
    if (time_info.day_light_saving_time_present) {
      HANDLE_CODE(time_info.day_light_saving_time.pack(bref));
    }
    if (time_info.leap_seconds_present) {
      HANDLE_CODE(pack_integer(bref, time_info.leap_seconds, (int16_t)-127, (int16_t)128));
    }
    if (time_info.local_time_offset_present) {
      HANDLE_CODE(pack_integer(bref, time_info.local_time_offset, (int8_t)-63, (int8_t)64));
    }
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ref_time_info_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ref_time_info_r16.is_present(), 1));
      if (ref_time_info_r16.is_present()) {
        HANDLE_CODE(ref_time_info_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(time_info_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (time_info_present) {
    HANDLE_CODE(bref.unpack(time_info.day_light_saving_time_present, 1));
    HANDLE_CODE(bref.unpack(time_info.leap_seconds_present, 1));
    HANDLE_CODE(bref.unpack(time_info.local_time_offset_present, 1));
    HANDLE_CODE(unpack_integer(time_info.time_info_utc, bref, (uint64_t)0u, (uint64_t)549755813887u));
    if (time_info.day_light_saving_time_present) {
      HANDLE_CODE(time_info.day_light_saving_time.unpack(bref));
    }
    if (time_info.leap_seconds_present) {
      HANDLE_CODE(unpack_integer(time_info.leap_seconds, bref, (int16_t)-127, (int16_t)128));
    }
    if (time_info.local_time_offset_present) {
      HANDLE_CODE(unpack_integer(time_info.local_time_offset, bref, (int8_t)-63, (int8_t)64));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ref_time_info_r16, bref);
      if (ref_time_info_r16.is_present()) {
        HANDLE_CODE(ref_time_info_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (time_info_present) {
    j.write_fieldname("timeInfo");
    j.start_obj();
    j.write_int("timeInfoUTC", time_info.time_info_utc);
    if (time_info.day_light_saving_time_present) {
      j.write_str("dayLightSavingTime", time_info.day_light_saving_time.to_string());
    }
    if (time_info.leap_seconds_present) {
      j.write_int("leapSeconds", time_info.leap_seconds);
    }
    if (time_info.local_time_offset_present) {
      j.write_int("localTimeOffset", time_info.local_time_offset);
    }
    j.end_obj();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (ref_time_info_r16.is_present()) {
      j.write_fieldname("referenceTimeInfo-r16");
      ref_time_info_r16->to_json(j);
    }
  }
  j.end_obj();
}

// SIBpos-r16 ::= SEQUENCE
SRSASN_CODE sib_pos_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(assist_data_sib_elem_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_pos_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(assist_data_sib_elem_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_pos_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("assistanceDataSIB-Element-r16", assist_data_sib_elem_r16.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SNPN-AccessInfo-r17 ::= SEQUENCE
SRSASN_CODE sn_pn_access_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ext_ch_supported_r17_present, 1));
  HANDLE_CODE(bref.pack(ext_ch_without_cfg_allowed_r17_present, 1));
  HANDLE_CODE(bref.pack(onboarding_enabled_r17_present, 1));
  HANDLE_CODE(bref.pack(ims_emergency_support_for_sn_pn_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sn_pn_access_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ext_ch_supported_r17_present, 1));
  HANDLE_CODE(bref.unpack(ext_ch_without_cfg_allowed_r17_present, 1));
  HANDLE_CODE(bref.unpack(onboarding_enabled_r17_present, 1));
  HANDLE_CODE(bref.unpack(ims_emergency_support_for_sn_pn_r17_present, 1));

  return SRSASN_SUCCESS;
}
void sn_pn_access_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ext_ch_supported_r17_present) {
    j.write_str("extCH-Supported-r17", "true");
  }
  if (ext_ch_without_cfg_allowed_r17_present) {
    j.write_str("extCH-WithoutConfigAllowed-r17", "true");
  }
  if (onboarding_enabled_r17_present) {
    j.write_str("onboardingEnabled-r17", "true");
  }
  if (ims_emergency_support_for_sn_pn_r17_present) {
    j.write_str("imsEmergencySupportForSNPN-r17", "true");
  }
  j.end_obj();
}

// SchedulingInfo ::= SEQUENCE
SRSASN_CODE sched_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_broadcast_status.pack(bref));
  HANDLE_CODE(si_periodicity.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_broadcast_status.unpack(bref));
  HANDLE_CODE(si_periodicity.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void sched_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-BroadcastStatus", si_broadcast_status.to_string());
  j.write_str("si-Periodicity", si_periodicity.to_string());
  j.start_array("sib-MappingInfo");
  for (const auto& e1 : sib_map_info) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* sched_info_s::si_broadcast_status_opts::to_string() const
{
  static const char* names[] = {"broadcasting", "notBroadcasting"};
  return convert_enum_idx(names, 2, value, "sched_info_s::si_broadcast_status_e_");
}

const char* sched_info_s::si_periodicity_opts::to_string() const
{
  static const char* names[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(names, 7, value, "sched_info_s::si_periodicity_e_");
}
uint16_t sched_info_s::si_periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "sched_info_s::si_periodicity_e_");
}

// UAC-BarringInfoSet ::= SEQUENCE
SRSASN_CODE uac_barr_info_set_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(uac_barr_factor.pack(bref));
  HANDLE_CODE(uac_barr_time.pack(bref));
  HANDLE_CODE(uac_barr_for_access_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_info_set_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(uac_barr_factor.unpack(bref));
  HANDLE_CODE(uac_barr_time.unpack(bref));
  HANDLE_CODE(uac_barr_for_access_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void uac_barr_info_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("uac-BarringFactor", uac_barr_factor.to_string());
  j.write_str("uac-BarringTime", uac_barr_time.to_string());
  j.write_str("uac-BarringForAccessIdentity", uac_barr_for_access_id.to_string());
  j.end_obj();
}

const char* uac_barr_info_set_s::uac_barr_factor_opts::to_string() const
{
  static const char* names[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(names, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}
float uac_barr_info_set_s::uac_barr_factor_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(numbers, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}
const char* uac_barr_info_set_s::uac_barr_factor_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(number_strs, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}

const char* uac_barr_info_set_s::uac_barr_time_opts::to_string() const
{
  static const char* names[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(names, 8, value, "uac_barr_info_set_s::uac_barr_time_e_");
}
uint16_t uac_barr_info_set_s::uac_barr_time_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 8, value, "uac_barr_info_set_s::uac_barr_time_e_");
}

// UAC-BarringPerPLMN ::= SEQUENCE
SRSASN_CODE uac_barr_per_plmn_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_ac_barr_list_type_present, 1));

  HANDLE_CODE(pack_integer(bref, plmn_id_idx, (uint8_t)1u, (uint8_t)12u));
  if (uac_ac_barr_list_type_present) {
    HANDLE_CODE(uac_ac_barr_list_type.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_plmn_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_ac_barr_list_type_present, 1));

  HANDLE_CODE(unpack_integer(plmn_id_idx, bref, (uint8_t)1u, (uint8_t)12u));
  if (uac_ac_barr_list_type_present) {
    HANDLE_CODE(uac_ac_barr_list_type.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void uac_barr_per_plmn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex", plmn_id_idx);
  if (uac_ac_barr_list_type_present) {
    j.write_fieldname("uac-ACBarringListType");
    uac_ac_barr_list_type.to_json(j);
  }
  j.end_obj();
}

void uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::destroy_()
{
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.destroy<uac_implicit_ac_barr_list_l_>();
      break;
    case types::uac_explicit_ac_barr_list:
      c.destroy<uac_barr_per_cat_list_l>();
      break;
    default:
      break;
  }
}
void uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.init<uac_implicit_ac_barr_list_l_>();
      break;
    case types::uac_explicit_ac_barr_list:
      c.init<uac_barr_per_cat_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }
}
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::uac_ac_barr_list_type_c_(
    const uac_barr_per_plmn_s::uac_ac_barr_list_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.init(other.c.get<uac_implicit_ac_barr_list_l_>());
      break;
    case types::uac_explicit_ac_barr_list:
      c.init(other.c.get<uac_barr_per_cat_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }
}
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_&
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::operator=(const uac_barr_per_plmn_s::uac_ac_barr_list_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.set(other.c.get<uac_implicit_ac_barr_list_l_>());
      break;
    case types::uac_explicit_ac_barr_list:
      c.set(other.c.get<uac_barr_per_cat_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }

  return *this;
}
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::uac_implicit_ac_barr_list_l_&
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set_uac_implicit_ac_barr_list()
{
  set(types::uac_implicit_ac_barr_list);
  return c.get<uac_implicit_ac_barr_list_l_>();
}
uac_barr_per_cat_list_l& uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set_uac_explicit_ac_barr_list()
{
  set(types::uac_explicit_ac_barr_list);
  return c.get<uac_barr_per_cat_list_l>();
}
void uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      j.start_array("uac-ImplicitACBarringList");
      for (const auto& e1 : c.get<uac_implicit_ac_barr_list_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::uac_explicit_ac_barr_list:
      j.start_array("uac-ExplicitACBarringList");
      for (const auto& e1 : c.get<uac_barr_per_cat_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    c.get<uac_implicit_ac_barr_list_l_>(),
                                    c.get<uac_implicit_ac_barr_list_l_>().size(),
                                    integer_packer<uint8_t>(1, 8)));
      break;
    case types::uac_explicit_ac_barr_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uac_barr_per_cat_list_l>(), 1, 63));
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      HANDLE_CODE(unpack_fixed_seq_of(c.get<uac_implicit_ac_barr_list_l_>(),
                                      bref,
                                      c.get<uac_implicit_ac_barr_list_l_>().size(),
                                      integer_packer<uint8_t>(1, 8)));
      break;
    case types::uac_explicit_ac_barr_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uac_barr_per_cat_list_l>(), bref, 1, 63));
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"uac-ImplicitACBarringList", "uac-ExplicitACBarringList"};
  return convert_enum_idx(names, 2, value, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types");
}

// CellAccessRelatedInfo ::= SEQUENCE
SRSASN_CODE cell_access_related_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_reserved_for_other_use_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_info_list, 1, 12));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cell_reserved_for_future_use_r16_present;
    group_flags[0] |= npn_id_info_list_r16.is_present();
    group_flags[1] |= snpn_access_info_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_reserved_for_future_use_r16_present, 1));
      HANDLE_CODE(bref.pack(npn_id_info_list_r16.is_present(), 1));
      if (npn_id_info_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *npn_id_info_list_r16, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(snpn_access_info_list_r17.is_present(), 1));
      if (snpn_access_info_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *snpn_access_info_list_r17, 1, 12));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_reserved_for_other_use_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_info_list, bref, 1, 12));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cell_reserved_for_future_use_r16_present, 1));
      unpack_presence_flag(npn_id_info_list_r16, bref);
      if (npn_id_info_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*npn_id_info_list_r16, bref, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(snpn_access_info_list_r17, bref);
      if (snpn_access_info_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*snpn_access_info_list_r17, bref, 1, 12));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cell_access_related_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityInfoList");
  for (const auto& e1 : plmn_id_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (cell_reserved_for_other_use_present) {
    j.write_str("cellReservedForOtherUse", "true");
  }
  if (ext) {
    if (cell_reserved_for_future_use_r16_present) {
      j.write_str("cellReservedForFutureUse-r16", "true");
    }
    if (npn_id_info_list_r16.is_present()) {
      j.start_array("npn-IdentityInfoList-r16");
      for (const auto& e1 : *npn_id_info_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (snpn_access_info_list_r17.is_present()) {
      j.start_array("snpn-AccessInfoList-r17");
      for (const auto& e1 : *snpn_access_info_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// ConnEstFailureControl ::= SEQUENCE
SRSASN_CODE conn_est_fail_ctrl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(conn_est_fail_offset_present, 1));

  HANDLE_CODE(conn_est_fail_count.pack(bref));
  HANDLE_CODE(conn_est_fail_offset_validity.pack(bref));
  if (conn_est_fail_offset_present) {
    HANDLE_CODE(pack_integer(bref, conn_est_fail_offset, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE conn_est_fail_ctrl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(conn_est_fail_offset_present, 1));

  HANDLE_CODE(conn_est_fail_count.unpack(bref));
  HANDLE_CODE(conn_est_fail_offset_validity.unpack(bref));
  if (conn_est_fail_offset_present) {
    HANDLE_CODE(unpack_integer(conn_est_fail_offset, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void conn_est_fail_ctrl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("connEstFailCount", conn_est_fail_count.to_string());
  j.write_str("connEstFailOffsetValidity", conn_est_fail_offset_validity.to_string());
  if (conn_est_fail_offset_present) {
    j.write_int("connEstFailOffset", conn_est_fail_offset);
  }
  j.end_obj();
}

const char* conn_est_fail_ctrl_s::conn_est_fail_count_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(names, 4, value, "conn_est_fail_ctrl_s::conn_est_fail_count_e_");
}
uint8_t conn_est_fail_ctrl_s::conn_est_fail_count_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "conn_est_fail_ctrl_s::conn_est_fail_count_e_");
}

const char* conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::to_string() const
{
  static const char* names[] = {"s30", "s60", "s120", "s240", "s300", "s420", "s600", "s900"};
  return convert_enum_idx(names, 8, value, "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_");
}
uint16_t conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::to_number() const
{
  static const uint16_t numbers[] = {30, 60, 120, 240, 300, 420, 600, 900};
  return map_enum_number(numbers, 8, value, "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_");
}

// PosSystemInformation-r16-IEs ::= SEQUENCE
SRSASN_CODE pos_sys_info_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sib_type_and_info_r16, 1, 32));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sys_info_r16_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pos_sib_type_and_info_r16, bref, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pos_sys_info_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("posSIB-TypeAndInfo-r16");
  for (const auto& e1 : pos_sib_type_and_info_r16) {
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

void pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::destroy_()
{
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_2_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_3_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_4_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_5_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_6_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_7_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_8_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_2_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_3_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_4_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_5_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_6_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_7_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_8_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_9_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_10_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_11_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_12_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_13_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_14_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_15_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_16_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_17_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_18_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_19_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_20_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_21_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_22_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_23_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib3_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib4_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib5_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_2_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_3_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_9_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_10_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_24_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_25_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_4_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_5_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_6_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    default:
      break;
  }
}
void pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_2_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_3_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_4_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_5_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_6_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_7_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_8_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_2_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_3_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_4_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_5_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_6_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_7_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_8_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_9_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_10_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_11_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_12_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_13_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_14_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_15_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_16_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_17_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_18_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_19_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_20_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_21_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_22_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_23_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib3_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib4_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib5_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_2_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_3_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_9_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_10_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_24_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_25_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_4_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_5_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_6_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }
}
pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::pos_sib_type_and_info_r16_item_c_(
    const pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_2_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_3_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_4_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_5_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_6_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_7_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_8_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_2_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_3_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_4_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_5_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_6_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_7_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_8_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_9_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_10_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_11_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_12_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_13_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_14_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_15_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_16_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_17_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_18_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_19_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_20_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_21_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_22_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_23_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib3_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib4_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib5_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_2_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_3_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_9_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_10_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_24_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_25_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_4_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_5_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_6_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }
}
pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_&
pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::operator=(
    const pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_2_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_3_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_4_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_5_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_6_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_7_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_8_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_2_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_3_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_4_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_5_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_6_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_7_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_8_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_9_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_10_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_11_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_12_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_13_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_14_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_15_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_16_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_17_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_18_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_19_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_20_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_21_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_22_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_23_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib3_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib4_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib5_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_2_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_3_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_9_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_10_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_24_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_25_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_4_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_5_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_6_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }

  return *this;
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_1_r16()
{
  set(types::pos_sib1_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_2_r16()
{
  set(types::pos_sib1_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_3_r16()
{
  set(types::pos_sib1_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_4_r16()
{
  set(types::pos_sib1_4_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_5_r16()
{
  set(types::pos_sib1_5_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_6_r16()
{
  set(types::pos_sib1_6_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_7_r16()
{
  set(types::pos_sib1_7_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_8_r16()
{
  set(types::pos_sib1_8_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_1_r16()
{
  set(types::pos_sib2_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_2_r16()
{
  set(types::pos_sib2_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_3_r16()
{
  set(types::pos_sib2_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_4_r16()
{
  set(types::pos_sib2_4_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_5_r16()
{
  set(types::pos_sib2_5_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_6_r16()
{
  set(types::pos_sib2_6_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_7_r16()
{
  set(types::pos_sib2_7_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_8_r16()
{
  set(types::pos_sib2_8_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_9_r16()
{
  set(types::pos_sib2_9_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_10_r16()
{
  set(types::pos_sib2_10_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_11_r16()
{
  set(types::pos_sib2_11_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_12_r16()
{
  set(types::pos_sib2_12_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_13_r16()
{
  set(types::pos_sib2_13_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_14_r16()
{
  set(types::pos_sib2_14_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_15_r16()
{
  set(types::pos_sib2_15_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_16_r16()
{
  set(types::pos_sib2_16_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_17_r16()
{
  set(types::pos_sib2_17_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_18_r16()
{
  set(types::pos_sib2_18_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_19_r16()
{
  set(types::pos_sib2_19_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_20_r16()
{
  set(types::pos_sib2_20_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_21_r16()
{
  set(types::pos_sib2_21_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_22_r16()
{
  set(types::pos_sib2_22_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_23_r16()
{
  set(types::pos_sib2_23_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib3_1_r16()
{
  set(types::pos_sib3_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib4_1_r16()
{
  set(types::pos_sib4_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib5_1_r16()
{
  set(types::pos_sib5_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_1_r16()
{
  set(types::pos_sib6_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_2_r16()
{
  set(types::pos_sib6_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_3_r16()
{
  set(types::pos_sib6_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_9_v1700()
{
  set(types::pos_sib1_9_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_10_v1700()
{
  set(types::pos_sib1_10_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_24_v1700()
{
  set(types::pos_sib2_24_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_25_v1700()
{
  set(types::pos_sib2_25_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_4_v1700()
{
  set(types::pos_sib6_4_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_5_v1700()
{
  set(types::pos_sib6_5_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_6_v1700()
{
  set(types::pos_sib6_6_v1700);
  return c.get<sib_pos_r16_s>();
}
void pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pos_sib1_1_r16:
      j.write_fieldname("posSib1-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_2_r16:
      j.write_fieldname("posSib1-2-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_3_r16:
      j.write_fieldname("posSib1-3-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_4_r16:
      j.write_fieldname("posSib1-4-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_5_r16:
      j.write_fieldname("posSib1-5-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_6_r16:
      j.write_fieldname("posSib1-6-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_7_r16:
      j.write_fieldname("posSib1-7-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_8_r16:
      j.write_fieldname("posSib1-8-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_1_r16:
      j.write_fieldname("posSib2-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_2_r16:
      j.write_fieldname("posSib2-2-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_3_r16:
      j.write_fieldname("posSib2-3-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_4_r16:
      j.write_fieldname("posSib2-4-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_5_r16:
      j.write_fieldname("posSib2-5-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_6_r16:
      j.write_fieldname("posSib2-6-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_7_r16:
      j.write_fieldname("posSib2-7-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_8_r16:
      j.write_fieldname("posSib2-8-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_9_r16:
      j.write_fieldname("posSib2-9-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_10_r16:
      j.write_fieldname("posSib2-10-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_11_r16:
      j.write_fieldname("posSib2-11-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_12_r16:
      j.write_fieldname("posSib2-12-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_13_r16:
      j.write_fieldname("posSib2-13-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_14_r16:
      j.write_fieldname("posSib2-14-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_15_r16:
      j.write_fieldname("posSib2-15-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_16_r16:
      j.write_fieldname("posSib2-16-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_17_r16:
      j.write_fieldname("posSib2-17-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_18_r16:
      j.write_fieldname("posSib2-18-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_19_r16:
      j.write_fieldname("posSib2-19-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_20_r16:
      j.write_fieldname("posSib2-20-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_21_r16:
      j.write_fieldname("posSib2-21-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_22_r16:
      j.write_fieldname("posSib2-22-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_23_r16:
      j.write_fieldname("posSib2-23-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib3_1_r16:
      j.write_fieldname("posSib3-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib4_1_r16:
      j.write_fieldname("posSib4-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib5_1_r16:
      j.write_fieldname("posSib5-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_1_r16:
      j.write_fieldname("posSib6-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_2_r16:
      j.write_fieldname("posSib6-2-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_3_r16:
      j.write_fieldname("posSib6-3-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_9_v1700:
      j.write_fieldname("posSib1-9-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_10_v1700:
      j.write_fieldname("posSib1-10-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_24_v1700:
      j.write_fieldname("posSib2-24-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_25_v1700:
      j.write_fieldname("posSib2-25-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_4_v1700:
      j.write_fieldname("posSib6-4-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_5_v1700:
      j.write_fieldname("posSib6-5-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_6_v1700:
      j.write_fieldname("posSib6-6-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pos_sib1_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_9_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_10_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_11_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_12_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_13_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_14_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_15_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_16_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_17_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_18_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_19_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_20_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_21_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_22_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_23_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib3_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib4_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib5_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib6_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib6_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib6_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_9_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib1_10_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib2_24_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib2_25_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib6_4_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib6_5_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib6_6_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pos_sib1_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_9_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_10_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_11_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_12_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_13_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_14_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_15_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_16_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_17_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_18_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_19_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_20_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_21_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_22_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_23_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib3_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib4_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib5_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib6_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib6_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib6_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_9_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib1_10_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib2_24_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib2_25_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib6_4_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib6_5_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib6_6_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "posSib1-1-r16",    "posSib1-2-r16",   "posSib1-3-r16",   "posSib1-4-r16",    "posSib1-5-r16",
      "posSib1-6-r16",    "posSib1-7-r16",   "posSib1-8-r16",   "posSib2-1-r16",    "posSib2-2-r16",
      "posSib2-3-r16",    "posSib2-4-r16",   "posSib2-5-r16",   "posSib2-6-r16",    "posSib2-7-r16",
      "posSib2-8-r16",    "posSib2-9-r16",   "posSib2-10-r16",  "posSib2-11-r16",   "posSib2-12-r16",
      "posSib2-13-r16",   "posSib2-14-r16",  "posSib2-15-r16",  "posSib2-16-r16",   "posSib2-17-r16",
      "posSib2-18-r16",   "posSib2-19-r16",  "posSib2-20-r16",  "posSib2-21-r16",   "posSib2-22-r16",
      "posSib2-23-r16",   "posSib3-1-r16",   "posSib4-1-r16",   "posSib5-1-r16",    "posSib6-1-r16",
      "posSib6-2-r16",    "posSib6-3-r16",   "posSib1-9-v1700", "posSib1-10-v1700", "posSib2-24-v1700",
      "posSib2-25-v1700", "posSib6-4-v1700", "posSib6-5-v1700", "posSib6-6-v1700"};
  return convert_enum_idx(names, 44, value, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::types");
}

// SI-SchedulingInfo ::= SEQUENCE
SRSASN_CODE si_sched_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(si_request_cfg_present, 1));
  HANDLE_CODE(bref.pack(si_request_cfg_sul_present, 1));
  HANDLE_CODE(bref.pack(sys_info_area_id_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list, 1, 32));
  HANDLE_CODE(si_win_len.pack(bref));
  if (si_request_cfg_present) {
    HANDLE_CODE(si_request_cfg.pack(bref));
  }
  if (si_request_cfg_sul_present) {
    HANDLE_CODE(si_request_cfg_sul.pack(bref));
  }
  if (sys_info_area_id_present) {
    HANDLE_CODE(sys_info_area_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_sched_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(si_request_cfg_present, 1));
  HANDLE_CODE(bref.unpack(si_request_cfg_sul_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_area_id_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sched_info_list, bref, 1, 32));
  HANDLE_CODE(si_win_len.unpack(bref));
  if (si_request_cfg_present) {
    HANDLE_CODE(si_request_cfg.unpack(bref));
  }
  if (si_request_cfg_sul_present) {
    HANDLE_CODE(si_request_cfg_sul.unpack(bref));
  }
  if (sys_info_area_id_present) {
    HANDLE_CODE(sys_info_area_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void si_sched_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("schedulingInfoList");
  for (const auto& e1 : sched_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("si-WindowLength", si_win_len.to_string());
  if (si_request_cfg_present) {
    j.write_fieldname("si-RequestConfig");
    si_request_cfg.to_json(j);
  }
  if (si_request_cfg_sul_present) {
    j.write_fieldname("si-RequestConfigSUL");
    si_request_cfg_sul.to_json(j);
  }
  if (sys_info_area_id_present) {
    j.write_str("systemInformationAreaID", sys_info_area_id.to_string());
  }
  j.end_obj();
}

const char* si_sched_info_s::si_win_len_opts::to_string() const
{
  static const char* names[] = {
      "s5", "s10", "s20", "s40", "s80", "s160", "s320", "s640", "s1280", "s2560-v1710", "s5120-v1710"};
  return convert_enum_idx(names, 11, value, "si_sched_info_s::si_win_len_e_");
}
uint16_t si_sched_info_s::si_win_len_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(numbers, 11, value, "si_sched_info_s::si_win_len_e_");
}

// SIB1-v1610-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idle_mode_meass_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(idle_mode_meass_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(pos_si_sched_info_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pos_si_sched_info_r16_present) {
    HANDLE_CODE(pos_si_sched_info_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idle_mode_meass_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(idle_mode_meass_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(pos_si_sched_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pos_si_sched_info_r16_present) {
    HANDLE_CODE(pos_si_sched_info_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idle_mode_meass_eutra_r16_present) {
    j.write_str("idleModeMeasurementsEUTRA-r16", "true");
  }
  if (idle_mode_meass_nr_r16_present) {
    j.write_str("idleModeMeasurementsNR-r16", "true");
  }
  if (pos_si_sched_info_r16_present) {
    j.write_fieldname("posSI-SchedulingInfo-r16");
    pos_si_sched_info_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformation-IEs ::= SEQUENCE
SRSASN_CODE sys_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sib_type_and_info, 1, 32));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sib_type_and_info, bref, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sys_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sib-TypeAndInfo");
  for (const auto& e1 : sib_type_and_info) {
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

void sys_info_ies_s::sib_type_and_info_item_c_::destroy_()
{
  switch (type_) {
    case types::sib2:
      c.destroy<sib2_s>();
      break;
    case types::sib3:
      c.destroy<sib3_s>();
      break;
    case types::sib4:
      c.destroy<sib4_s>();
      break;
    case types::sib5:
      c.destroy<sib5_s>();
      break;
    case types::sib6:
      c.destroy<sib6_s>();
      break;
    case types::sib7:
      c.destroy<sib7_s>();
      break;
    case types::sib8:
      c.destroy<sib8_s>();
      break;
    case types::sib9:
      c.destroy<sib9_s>();
      break;
    case types::sib10_v1610:
      c.destroy<sib10_r16_s>();
      break;
    case types::sib11_v1610:
      c.destroy<sib11_r16_s>();
      break;
    case types::sib12_v1610:
      c.destroy<sib12_r16_s>();
      break;
    case types::sib13_v1610:
      c.destroy<sib13_r16_s>();
      break;
    case types::sib14_v1610:
      c.destroy<sib14_r16_s>();
      break;
    case types::sib15_v1700:
      c.destroy<sib15_r17_s>();
      break;
    case types::sib16_v1700:
      c.destroy<sib16_r17_s>();
      break;
    case types::sib17_v1700:
      c.destroy<sib17_r17_s>();
      break;
    case types::sib18_v1700:
      c.destroy<sib18_r17_s>();
      break;
    case types::sib19_v1700:
      c.destroy<sib19_r17_s>();
      break;
    case types::sib20_v1700:
      c.destroy<sib20_r17_s>();
      break;
    case types::sib21_v1700:
      c.destroy<sib21_r17_s>();
      break;
    default:
      break;
  }
}
void sys_info_ies_s::sib_type_and_info_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sib2:
      c.init<sib2_s>();
      break;
    case types::sib3:
      c.init<sib3_s>();
      break;
    case types::sib4:
      c.init<sib4_s>();
      break;
    case types::sib5:
      c.init<sib5_s>();
      break;
    case types::sib6:
      c.init<sib6_s>();
      break;
    case types::sib7:
      c.init<sib7_s>();
      break;
    case types::sib8:
      c.init<sib8_s>();
      break;
    case types::sib9:
      c.init<sib9_s>();
      break;
    case types::sib10_v1610:
      c.init<sib10_r16_s>();
      break;
    case types::sib11_v1610:
      c.init<sib11_r16_s>();
      break;
    case types::sib12_v1610:
      c.init<sib12_r16_s>();
      break;
    case types::sib13_v1610:
      c.init<sib13_r16_s>();
      break;
    case types::sib14_v1610:
      c.init<sib14_r16_s>();
      break;
    case types::sib15_v1700:
      c.init<sib15_r17_s>();
      break;
    case types::sib16_v1700:
      c.init<sib16_r17_s>();
      break;
    case types::sib17_v1700:
      c.init<sib17_r17_s>();
      break;
    case types::sib18_v1700:
      c.init<sib18_r17_s>();
      break;
    case types::sib19_v1700:
      c.init<sib19_r17_s>();
      break;
    case types::sib20_v1700:
      c.init<sib20_r17_s>();
      break;
    case types::sib21_v1700:
      c.init<sib21_r17_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }
}
sys_info_ies_s::sib_type_and_info_item_c_::sib_type_and_info_item_c_(
    const sys_info_ies_s::sib_type_and_info_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sib2:
      c.init(other.c.get<sib2_s>());
      break;
    case types::sib3:
      c.init(other.c.get<sib3_s>());
      break;
    case types::sib4:
      c.init(other.c.get<sib4_s>());
      break;
    case types::sib5:
      c.init(other.c.get<sib5_s>());
      break;
    case types::sib6:
      c.init(other.c.get<sib6_s>());
      break;
    case types::sib7:
      c.init(other.c.get<sib7_s>());
      break;
    case types::sib8:
      c.init(other.c.get<sib8_s>());
      break;
    case types::sib9:
      c.init(other.c.get<sib9_s>());
      break;
    case types::sib10_v1610:
      c.init(other.c.get<sib10_r16_s>());
      break;
    case types::sib11_v1610:
      c.init(other.c.get<sib11_r16_s>());
      break;
    case types::sib12_v1610:
      c.init(other.c.get<sib12_r16_s>());
      break;
    case types::sib13_v1610:
      c.init(other.c.get<sib13_r16_s>());
      break;
    case types::sib14_v1610:
      c.init(other.c.get<sib14_r16_s>());
      break;
    case types::sib15_v1700:
      c.init(other.c.get<sib15_r17_s>());
      break;
    case types::sib16_v1700:
      c.init(other.c.get<sib16_r17_s>());
      break;
    case types::sib17_v1700:
      c.init(other.c.get<sib17_r17_s>());
      break;
    case types::sib18_v1700:
      c.init(other.c.get<sib18_r17_s>());
      break;
    case types::sib19_v1700:
      c.init(other.c.get<sib19_r17_s>());
      break;
    case types::sib20_v1700:
      c.init(other.c.get<sib20_r17_s>());
      break;
    case types::sib21_v1700:
      c.init(other.c.get<sib21_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }
}
sys_info_ies_s::sib_type_and_info_item_c_&
sys_info_ies_s::sib_type_and_info_item_c_::operator=(const sys_info_ies_s::sib_type_and_info_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sib2:
      c.set(other.c.get<sib2_s>());
      break;
    case types::sib3:
      c.set(other.c.get<sib3_s>());
      break;
    case types::sib4:
      c.set(other.c.get<sib4_s>());
      break;
    case types::sib5:
      c.set(other.c.get<sib5_s>());
      break;
    case types::sib6:
      c.set(other.c.get<sib6_s>());
      break;
    case types::sib7:
      c.set(other.c.get<sib7_s>());
      break;
    case types::sib8:
      c.set(other.c.get<sib8_s>());
      break;
    case types::sib9:
      c.set(other.c.get<sib9_s>());
      break;
    case types::sib10_v1610:
      c.set(other.c.get<sib10_r16_s>());
      break;
    case types::sib11_v1610:
      c.set(other.c.get<sib11_r16_s>());
      break;
    case types::sib12_v1610:
      c.set(other.c.get<sib12_r16_s>());
      break;
    case types::sib13_v1610:
      c.set(other.c.get<sib13_r16_s>());
      break;
    case types::sib14_v1610:
      c.set(other.c.get<sib14_r16_s>());
      break;
    case types::sib15_v1700:
      c.set(other.c.get<sib15_r17_s>());
      break;
    case types::sib16_v1700:
      c.set(other.c.get<sib16_r17_s>());
      break;
    case types::sib17_v1700:
      c.set(other.c.get<sib17_r17_s>());
      break;
    case types::sib18_v1700:
      c.set(other.c.get<sib18_r17_s>());
      break;
    case types::sib19_v1700:
      c.set(other.c.get<sib19_r17_s>());
      break;
    case types::sib20_v1700:
      c.set(other.c.get<sib20_r17_s>());
      break;
    case types::sib21_v1700:
      c.set(other.c.get<sib21_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }

  return *this;
}
sib2_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib2()
{
  set(types::sib2);
  return c.get<sib2_s>();
}
sib3_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib3()
{
  set(types::sib3);
  return c.get<sib3_s>();
}
sib4_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib4()
{
  set(types::sib4);
  return c.get<sib4_s>();
}
sib5_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib5()
{
  set(types::sib5);
  return c.get<sib5_s>();
}
sib6_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib6()
{
  set(types::sib6);
  return c.get<sib6_s>();
}
sib7_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib7()
{
  set(types::sib7);
  return c.get<sib7_s>();
}
sib8_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib8()
{
  set(types::sib8);
  return c.get<sib8_s>();
}
sib9_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib9()
{
  set(types::sib9);
  return c.get<sib9_s>();
}
sib10_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib10_v1610()
{
  set(types::sib10_v1610);
  return c.get<sib10_r16_s>();
}
sib11_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib11_v1610()
{
  set(types::sib11_v1610);
  return c.get<sib11_r16_s>();
}
sib12_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib12_v1610()
{
  set(types::sib12_v1610);
  return c.get<sib12_r16_s>();
}
sib13_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib13_v1610()
{
  set(types::sib13_v1610);
  return c.get<sib13_r16_s>();
}
sib14_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib14_v1610()
{
  set(types::sib14_v1610);
  return c.get<sib14_r16_s>();
}
sib15_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib15_v1700()
{
  set(types::sib15_v1700);
  return c.get<sib15_r17_s>();
}
sib16_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib16_v1700()
{
  set(types::sib16_v1700);
  return c.get<sib16_r17_s>();
}
sib17_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib17_v1700()
{
  set(types::sib17_v1700);
  return c.get<sib17_r17_s>();
}
sib18_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib18_v1700()
{
  set(types::sib18_v1700);
  return c.get<sib18_r17_s>();
}
sib19_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib19_v1700()
{
  set(types::sib19_v1700);
  return c.get<sib19_r17_s>();
}
sib20_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib20_v1700()
{
  set(types::sib20_v1700);
  return c.get<sib20_r17_s>();
}
sib21_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib21_v1700()
{
  set(types::sib21_v1700);
  return c.get<sib21_r17_s>();
}
void sys_info_ies_s::sib_type_and_info_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sib2:
      j.write_fieldname("sib2");
      c.get<sib2_s>().to_json(j);
      break;
    case types::sib3:
      j.write_fieldname("sib3");
      c.get<sib3_s>().to_json(j);
      break;
    case types::sib4:
      j.write_fieldname("sib4");
      c.get<sib4_s>().to_json(j);
      break;
    case types::sib5:
      j.write_fieldname("sib5");
      c.get<sib5_s>().to_json(j);
      break;
    case types::sib6:
      j.write_fieldname("sib6");
      c.get<sib6_s>().to_json(j);
      break;
    case types::sib7:
      j.write_fieldname("sib7");
      c.get<sib7_s>().to_json(j);
      break;
    case types::sib8:
      j.write_fieldname("sib8");
      c.get<sib8_s>().to_json(j);
      break;
    case types::sib9:
      j.write_fieldname("sib9");
      c.get<sib9_s>().to_json(j);
      break;
    case types::sib10_v1610:
      j.write_fieldname("sib10-v1610");
      c.get<sib10_r16_s>().to_json(j);
      break;
    case types::sib11_v1610:
      j.write_fieldname("sib11-v1610");
      c.get<sib11_r16_s>().to_json(j);
      break;
    case types::sib12_v1610:
      j.write_fieldname("sib12-v1610");
      c.get<sib12_r16_s>().to_json(j);
      break;
    case types::sib13_v1610:
      j.write_fieldname("sib13-v1610");
      c.get<sib13_r16_s>().to_json(j);
      break;
    case types::sib14_v1610:
      j.write_fieldname("sib14-v1610");
      c.get<sib14_r16_s>().to_json(j);
      break;
    case types::sib15_v1700:
      j.write_fieldname("sib15-v1700");
      c.get<sib15_r17_s>().to_json(j);
      break;
    case types::sib16_v1700:
      j.write_fieldname("sib16-v1700");
      c.get<sib16_r17_s>().to_json(j);
      break;
    case types::sib17_v1700:
      j.write_fieldname("sib17-v1700");
      c.get<sib17_r17_s>().to_json(j);
      break;
    case types::sib18_v1700:
      j.write_fieldname("sib18-v1700");
      c.get<sib18_r17_s>().to_json(j);
      break;
    case types::sib19_v1700:
      j.write_fieldname("sib19-v1700");
      c.get<sib19_r17_s>().to_json(j);
      break;
    case types::sib20_v1700:
      j.write_fieldname("sib20-v1700");
      c.get<sib20_r17_s>().to_json(j);
      break;
    case types::sib21_v1700:
      j.write_fieldname("sib21-v1700");
      c.get<sib21_r17_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_ies_s::sib_type_and_info_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sib2:
      HANDLE_CODE(c.get<sib2_s>().pack(bref));
      break;
    case types::sib3:
      HANDLE_CODE(c.get<sib3_s>().pack(bref));
      break;
    case types::sib4:
      HANDLE_CODE(c.get<sib4_s>().pack(bref));
      break;
    case types::sib5:
      HANDLE_CODE(c.get<sib5_s>().pack(bref));
      break;
    case types::sib6:
      HANDLE_CODE(c.get<sib6_s>().pack(bref));
      break;
    case types::sib7:
      HANDLE_CODE(c.get<sib7_s>().pack(bref));
      break;
    case types::sib8:
      HANDLE_CODE(c.get<sib8_s>().pack(bref));
      break;
    case types::sib9:
      HANDLE_CODE(c.get<sib9_s>().pack(bref));
      break;
    case types::sib10_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib10_r16_s>().pack(bref));
    } break;
    case types::sib11_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib11_r16_s>().pack(bref));
    } break;
    case types::sib12_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib12_r16_s>().pack(bref));
    } break;
    case types::sib13_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib13_r16_s>().pack(bref));
    } break;
    case types::sib14_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib14_r16_s>().pack(bref));
    } break;
    case types::sib15_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib15_r17_s>().pack(bref));
    } break;
    case types::sib16_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib16_r17_s>().pack(bref));
    } break;
    case types::sib17_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib17_r17_s>().pack(bref));
    } break;
    case types::sib18_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib18_r17_s>().pack(bref));
    } break;
    case types::sib19_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib19_r17_s>().pack(bref));
    } break;
    case types::sib20_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib20_r17_s>().pack(bref));
    } break;
    case types::sib21_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib21_r17_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_ies_s::sib_type_and_info_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sib2:
      HANDLE_CODE(c.get<sib2_s>().unpack(bref));
      break;
    case types::sib3:
      HANDLE_CODE(c.get<sib3_s>().unpack(bref));
      break;
    case types::sib4:
      HANDLE_CODE(c.get<sib4_s>().unpack(bref));
      break;
    case types::sib5:
      HANDLE_CODE(c.get<sib5_s>().unpack(bref));
      break;
    case types::sib6:
      HANDLE_CODE(c.get<sib6_s>().unpack(bref));
      break;
    case types::sib7:
      HANDLE_CODE(c.get<sib7_s>().unpack(bref));
      break;
    case types::sib8:
      HANDLE_CODE(c.get<sib8_s>().unpack(bref));
      break;
    case types::sib9:
      HANDLE_CODE(c.get<sib9_s>().unpack(bref));
      break;
    case types::sib10_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib10_r16_s>().unpack(bref));
    } break;
    case types::sib11_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib11_r16_s>().unpack(bref));
    } break;
    case types::sib12_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib12_r16_s>().unpack(bref));
    } break;
    case types::sib13_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib13_r16_s>().unpack(bref));
    } break;
    case types::sib14_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib14_r16_s>().unpack(bref));
    } break;
    case types::sib15_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib15_r17_s>().unpack(bref));
    } break;
    case types::sib16_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib16_r17_s>().unpack(bref));
    } break;
    case types::sib17_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib17_r17_s>().unpack(bref));
    } break;
    case types::sib18_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib18_r17_s>().unpack(bref));
    } break;
    case types::sib19_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib19_r17_s>().unpack(bref));
    } break;
    case types::sib20_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib20_r17_s>().unpack(bref));
    } break;
    case types::sib21_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib21_r17_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_info_ies_s::sib_type_and_info_item_c_::types_opts::to_string() const
{
  static const char* names[] = {"sib2",        "sib3",        "sib4",        "sib5",        "sib6",
                                "sib7",        "sib8",        "sib9",        "sib10-v1610", "sib11-v1610",
                                "sib12-v1610", "sib13-v1610", "sib14-v1610", "sib15-v1700", "sib16-v1700",
                                "sib17-v1700", "sib18-v1700", "sib19-v1700", "sib20-v1700", "sib21-v1700"};
  return convert_enum_idx(names, 20, value, "sys_info_ies_s::sib_type_and_info_item_c_::types");
}
uint8_t sys_info_ies_s::sib_type_and_info_item_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
  return map_enum_number(numbers, 20, value, "sys_info_ies_s::sib_type_and_info_item_c_::types");
}

// UAC-AccessCategory1-SelectionAssistanceInfo ::= ENUMERATED
const char* uac_access_category1_sel_assist_info_opts::to_string() const
{
  static const char* names[] = {"a", "b", "c"};
  return convert_enum_idx(names, 3, value, "uac_access_category1_sel_assist_info_e");
}

// UE-TimersAndConstants ::= SEQUENCE
SRSASN_CODE ue_timers_and_consts_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(t300.pack(bref));
  HANDLE_CODE(t301.pack(bref));
  HANDLE_CODE(t310.pack(bref));
  HANDLE_CODE(n310.pack(bref));
  HANDLE_CODE(t311.pack(bref));
  HANDLE_CODE(n311.pack(bref));
  HANDLE_CODE(t319.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_timers_and_consts_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(t300.unpack(bref));
  HANDLE_CODE(t301.unpack(bref));
  HANDLE_CODE(t310.unpack(bref));
  HANDLE_CODE(n310.unpack(bref));
  HANDLE_CODE(t311.unpack(bref));
  HANDLE_CODE(n311.unpack(bref));
  HANDLE_CODE(t319.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_timers_and_consts_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t300", t300.to_string());
  j.write_str("t301", t301.to_string());
  j.write_str("t310", t310.to_string());
  j.write_str("n310", n310.to_string());
  j.write_str("t311", t311.to_string());
  j.write_str("n311", n311.to_string());
  j.write_str("t319", t319.to_string());
  j.end_obj();
}

const char* ue_timers_and_consts_s::t300_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::t300_e_");
}
uint16_t ue_timers_and_consts_s::t300_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t300_e_");
}

const char* ue_timers_and_consts_s::t301_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::t301_e_");
}
uint16_t ue_timers_and_consts_s::t301_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t301_e_");
}

const char* ue_timers_and_consts_s::t310_opts::to_string() const
{
  static const char* names[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(names, 7, value, "ue_timers_and_consts_s::t310_e_");
}
uint16_t ue_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(numbers, 7, value, "ue_timers_and_consts_s::t310_e_");
}

const char* ue_timers_and_consts_s::n310_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::n310_e_");
}
uint8_t ue_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::n310_e_");
}

const char* ue_timers_and_consts_s::t311_opts::to_string() const
{
  static const char* names[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(names, 7, value, "ue_timers_and_consts_s::t311_e_");
}
uint16_t ue_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t numbers[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(numbers, 7, value, "ue_timers_and_consts_s::t311_e_");
}

const char* ue_timers_and_consts_s::n311_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::n311_e_");
}
uint8_t ue_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::n311_e_");
}

const char* ue_timers_and_consts_s::t319_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::t319_e_");
}
uint16_t ue_timers_and_consts_s::t319_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t319_e_");
}

// SIB1 ::= SEQUENCE
SRSASN_CODE sib1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_present, 1));
  HANDLE_CODE(bref.pack(conn_est_fail_ctrl_present, 1));
  HANDLE_CODE(bref.pack(si_sched_info_present, 1));
  HANDLE_CODE(bref.pack(serving_cell_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(ims_emergency_support_present, 1));
  HANDLE_CODE(bref.pack(ecall_over_ims_support_present, 1));
  HANDLE_CODE(bref.pack(ue_timers_and_consts_present, 1));
  HANDLE_CODE(bref.pack(uac_barr_info_present, 1));
  HANDLE_CODE(bref.pack(use_full_resume_id_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_present) {
    HANDLE_CODE(bref.pack(cell_sel_info.q_rx_lev_min_offset_present, 1));
    HANDLE_CODE(bref.pack(cell_sel_info.q_rx_lev_min_sul_present, 1));
    HANDLE_CODE(bref.pack(cell_sel_info.q_qual_min_present, 1));
    HANDLE_CODE(bref.pack(cell_sel_info.q_qual_min_offset_present, 1));
    HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min, (int8_t)-70, (int8_t)-22));
    if (cell_sel_info.q_rx_lev_min_offset_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min_offset, (uint8_t)1u, (uint8_t)8u));
    }
    if (cell_sel_info.q_rx_lev_min_sul_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min_sul, (int8_t)-70, (int8_t)-22));
    }
    if (cell_sel_info.q_qual_min_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_qual_min, (int8_t)-43, (int8_t)-12));
    }
    if (cell_sel_info.q_qual_min_offset_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_qual_min_offset, (uint8_t)1u, (uint8_t)8u));
    }
  }
  HANDLE_CODE(cell_access_related_info.pack(bref));
  if (conn_est_fail_ctrl_present) {
    HANDLE_CODE(conn_est_fail_ctrl.pack(bref));
  }
  if (si_sched_info_present) {
    HANDLE_CODE(si_sched_info.pack(bref));
  }
  if (serving_cell_cfg_common_present) {
    HANDLE_CODE(serving_cell_cfg_common.pack(bref));
  }
  if (ue_timers_and_consts_present) {
    HANDLE_CODE(ue_timers_and_consts.pack(bref));
  }
  if (uac_barr_info_present) {
    HANDLE_CODE(bref.pack(uac_barr_info.uac_barr_for_common.size() > 0, 1));
    HANDLE_CODE(bref.pack(uac_barr_info.uac_barr_per_plmn_list.size() > 0, 1));
    HANDLE_CODE(bref.pack(uac_barr_info.uac_access_category1_sel_assist_info_present, 1));
    if (uac_barr_info.uac_barr_for_common.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info.uac_barr_for_common, 1, 63));
    }
    if (uac_barr_info.uac_barr_per_plmn_list.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info.uac_barr_per_plmn_list, 1, 12));
    }
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info.uac_barr_info_set_list, 1, 8));
    if (uac_barr_info.uac_access_category1_sel_assist_info_present) {
      HANDLE_CODE(uac_barr_info.uac_access_category1_sel_assist_info.pack(bref));
    }
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_present, 1));
  HANDLE_CODE(bref.unpack(conn_est_fail_ctrl_present, 1));
  HANDLE_CODE(bref.unpack(si_sched_info_present, 1));
  HANDLE_CODE(bref.unpack(serving_cell_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(ims_emergency_support_present, 1));
  HANDLE_CODE(bref.unpack(ecall_over_ims_support_present, 1));
  HANDLE_CODE(bref.unpack(ue_timers_and_consts_present, 1));
  HANDLE_CODE(bref.unpack(uac_barr_info_present, 1));
  HANDLE_CODE(bref.unpack(use_full_resume_id_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_present) {
    HANDLE_CODE(bref.unpack(cell_sel_info.q_rx_lev_min_offset_present, 1));
    HANDLE_CODE(bref.unpack(cell_sel_info.q_rx_lev_min_sul_present, 1));
    HANDLE_CODE(bref.unpack(cell_sel_info.q_qual_min_present, 1));
    HANDLE_CODE(bref.unpack(cell_sel_info.q_qual_min_offset_present, 1));
    HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
    if (cell_sel_info.q_rx_lev_min_offset_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min_offset, bref, (uint8_t)1u, (uint8_t)8u));
    }
    if (cell_sel_info.q_rx_lev_min_sul_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min_sul, bref, (int8_t)-70, (int8_t)-22));
    }
    if (cell_sel_info.q_qual_min_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_qual_min, bref, (int8_t)-43, (int8_t)-12));
    }
    if (cell_sel_info.q_qual_min_offset_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_qual_min_offset, bref, (uint8_t)1u, (uint8_t)8u));
    }
  }
  HANDLE_CODE(cell_access_related_info.unpack(bref));
  if (conn_est_fail_ctrl_present) {
    HANDLE_CODE(conn_est_fail_ctrl.unpack(bref));
  }
  if (si_sched_info_present) {
    HANDLE_CODE(si_sched_info.unpack(bref));
  }
  if (serving_cell_cfg_common_present) {
    HANDLE_CODE(serving_cell_cfg_common.unpack(bref));
  }
  if (ue_timers_and_consts_present) {
    HANDLE_CODE(ue_timers_and_consts.unpack(bref));
  }
  if (uac_barr_info_present) {
    bool uac_barr_for_common_present;
    HANDLE_CODE(bref.unpack(uac_barr_for_common_present, 1));
    bool uac_barr_per_plmn_list_present;
    HANDLE_CODE(bref.unpack(uac_barr_per_plmn_list_present, 1));
    HANDLE_CODE(bref.unpack(uac_barr_info.uac_access_category1_sel_assist_info_present, 1));
    if (uac_barr_for_common_present) {
      HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info.uac_barr_for_common, bref, 1, 63));
    }
    if (uac_barr_per_plmn_list_present) {
      HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info.uac_barr_per_plmn_list, bref, 1, 12));
    }
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info.uac_barr_info_set_list, bref, 1, 8));
    if (uac_barr_info.uac_access_category1_sel_assist_info_present) {
      HANDLE_CODE(uac_barr_info.uac_access_category1_sel_assist_info.unpack(bref));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_present) {
    j.write_fieldname("cellSelectionInfo");
    j.start_obj();
    j.write_int("q-RxLevMin", cell_sel_info.q_rx_lev_min);
    if (cell_sel_info.q_rx_lev_min_offset_present) {
      j.write_int("q-RxLevMinOffset", cell_sel_info.q_rx_lev_min_offset);
    }
    if (cell_sel_info.q_rx_lev_min_sul_present) {
      j.write_int("q-RxLevMinSUL", cell_sel_info.q_rx_lev_min_sul);
    }
    if (cell_sel_info.q_qual_min_present) {
      j.write_int("q-QualMin", cell_sel_info.q_qual_min);
    }
    if (cell_sel_info.q_qual_min_offset_present) {
      j.write_int("q-QualMinOffset", cell_sel_info.q_qual_min_offset);
    }
    j.end_obj();
  }
  j.write_fieldname("cellAccessRelatedInfo");
  cell_access_related_info.to_json(j);
  if (conn_est_fail_ctrl_present) {
    j.write_fieldname("connEstFailureControl");
    conn_est_fail_ctrl.to_json(j);
  }
  if (si_sched_info_present) {
    j.write_fieldname("si-SchedulingInfo");
    si_sched_info.to_json(j);
  }
  if (serving_cell_cfg_common_present) {
    j.write_fieldname("servingCellConfigCommon");
    serving_cell_cfg_common.to_json(j);
  }
  if (ims_emergency_support_present) {
    j.write_str("ims-EmergencySupport", "true");
  }
  if (ecall_over_ims_support_present) {
    j.write_str("eCallOverIMS-Support", "true");
  }
  if (ue_timers_and_consts_present) {
    j.write_fieldname("ue-TimersAndConstants");
    ue_timers_and_consts.to_json(j);
  }
  if (uac_barr_info_present) {
    j.write_fieldname("uac-BarringInfo");
    j.start_obj();
    if (uac_barr_info.uac_barr_for_common.size() > 0) {
      j.start_array("uac-BarringForCommon");
      for (const auto& e1 : uac_barr_info.uac_barr_for_common) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (uac_barr_info.uac_barr_per_plmn_list.size() > 0) {
      j.start_array("uac-BarringPerPLMN-List");
      for (const auto& e1 : uac_barr_info.uac_barr_per_plmn_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.start_array("uac-BarringInfoSetList");
    for (const auto& e1 : uac_barr_info.uac_barr_info_set_list) {
      e1.to_json(j);
    }
    j.end_array();
    if (uac_barr_info.uac_access_category1_sel_assist_info_present) {
      j.write_fieldname("uac-AccessCategory1-SelectionAssistanceInfo");
      uac_barr_info.uac_access_category1_sel_assist_info.to_json(j);
    }
    j.end_obj();
  }
  if (use_full_resume_id_present) {
    j.write_str("useFullResumeID", "true");
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

void sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::destroy_()
{
  switch (type_) {
    case types::individual_plmn_list:
      c.destroy<individual_plmn_list_l_>();
      break;
    default:
      break;
  }
}
void sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_common:
      break;
    case types::individual_plmn_list:
      c.init<individual_plmn_list_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }
}
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::uac_access_category1_sel_assist_info_c_(
    const sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_common:
      c.init(other.c.get<uac_access_category1_sel_assist_info_e>());
      break;
    case types::individual_plmn_list:
      c.init(other.c.get<individual_plmn_list_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }
}
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::operator=(
    const sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_common:
      c.set(other.c.get<uac_access_category1_sel_assist_info_e>());
      break;
    case types::individual_plmn_list:
      c.set(other.c.get<individual_plmn_list_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }

  return *this;
}
uac_access_category1_sel_assist_info_e&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set_plmn_common()
{
  set(types::plmn_common);
  return c.get<uac_access_category1_sel_assist_info_e>();
}
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::individual_plmn_list_l_&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set_individual_plmn_list()
{
  set(types::individual_plmn_list);
  return c.get<individual_plmn_list_l_>();
}
void sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_common:
      j.write_str("plmnCommon", c.get<uac_access_category1_sel_assist_info_e>().to_string());
      break;
    case types::individual_plmn_list:
      j.start_array("individualPLMNList");
      for (const auto& e1 : c.get<individual_plmn_list_l_>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_common:
      HANDLE_CODE(c.get<uac_access_category1_sel_assist_info_e>().pack(bref));
      break;
    case types::individual_plmn_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<individual_plmn_list_l_>(), 2, 12));
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_common:
      HANDLE_CODE(c.get<uac_access_category1_sel_assist_info_e>().unpack(bref));
      break;
    case types::individual_plmn_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<individual_plmn_list_l_>(), bref, 2, 12));
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types_opts::to_string() const
{
  static const char* names[] = {"plmnCommon", "individualPLMNList"};
  return convert_enum_idx(names, 2, value, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types");
}

// SystemInformation ::= SEQUENCE
SRSASN_CODE sys_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void sys_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void sys_info_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::sys_info:
      c.destroy<sys_info_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.destroy<crit_exts_future_r16_c_>();
      break;
    default:
      break;
  }
}
void sys_info_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info:
      c.init<sys_info_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.init<crit_exts_future_r16_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
}
sys_info_s::crit_exts_c_::crit_exts_c_(const sys_info_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info:
      c.init(other.c.get<sys_info_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.init(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
}
sys_info_s::crit_exts_c_& sys_info_s::crit_exts_c_::operator=(const sys_info_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info:
      c.set(other.c.get<sys_info_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.set(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }

  return *this;
}
sys_info_ies_s& sys_info_s::crit_exts_c_::set_sys_info()
{
  set(types::sys_info);
  return c.get<sys_info_ies_s>();
}
sys_info_s::crit_exts_c_::crit_exts_future_r16_c_& sys_info_s::crit_exts_c_::set_crit_exts_future_r16()
{
  set(types::crit_exts_future_r16);
  return c.get<crit_exts_future_r16_c_>();
}
void sys_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info:
      j.write_fieldname("systemInformation");
      c.get<sys_info_ies_s>().to_json(j);
      break;
    case types::crit_exts_future_r16:
      j.write_fieldname("criticalExtensionsFuture-r16");
      c.get<crit_exts_future_r16_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_ies_s>().pack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set(types::options e)
{
  type_ = e;
}
pos_sys_info_r16_ies_s& sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set_pos_sys_info_r16()
{
  set(types::pos_sys_info_r16);
  return c;
}
void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pos_sys_info_r16:
      j.write_fieldname("posSystemInformation-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pos_sys_info_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pos_sys_info_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"posSystemInformation-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types");
}

const char* sys_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"systemInformation", "criticalExtensionsFuture-r16"};
  return convert_enum_idx(names, 2, value, "sys_info_s::crit_exts_c_::types");
}

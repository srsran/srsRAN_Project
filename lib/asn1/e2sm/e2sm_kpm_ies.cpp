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

#include "srsran/asn1/e2sm/e2sm_kpm_ies.h"
using namespace asn1;
using namespace asn1::e2sm;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MeasValueTestCond-Expression ::= ENUMERATED
const char* meas_value_test_cond_expression_opts::to_string() const
{
  static const char* names[] = {"equal", "greaterthan", "lessthan", "contains", "present"};
  return convert_enum_idx(names, 5, value, "meas_value_test_cond_expression_e");
}

// S-NSSAI ::= SEQUENCE
SRSASN_CODE s_nssai_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sd_present, 1));

  HANDLE_CODE(sst.pack(bref));
  if (sd_present) {
    HANDLE_CODE(sd.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE s_nssai_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sd_present, 1));

  HANDLE_CODE(sst.unpack(bref));
  if (sd_present) {
    HANDLE_CODE(sd.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void s_nssai_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sST", sst.to_string());
  if (sd_present) {
    j.write_str("sD", sd.to_string());
  }
  j.end_obj();
}

// TestCond-Value ::= CHOICE
void test_cond_value_c::destroy_()
{
  switch (type_) {
    case types::value_bool:
      c.destroy<bool>();
      break;
    case types::value_bit_s:
      c.destroy<dyn_bitstring>();
      break;
    case types::value_oct_s:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::value_prt_s:
      c.destroy<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
      break;
    case types::value_real:
      c.destroy<real_s>();
      break;
    default:
      break;
  }
}
void test_cond_value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::value_int:
      break;
    case types::value_enum:
      break;
    case types::value_bool:
      c.init<bool>();
      break;
    case types::value_bit_s:
      c.init<dyn_bitstring>();
      break;
    case types::value_oct_s:
      c.init<unbounded_octstring<true>>();
      break;
    case types::value_prt_s:
      c.init<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
      break;
    case types::value_real:
      c.init<real_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }
}
test_cond_value_c::test_cond_value_c(const test_cond_value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::value_int:
      c.init(other.c.get<int64_t>());
      break;
    case types::value_enum:
      c.init(other.c.get<int64_t>());
      break;
    case types::value_bool:
      c.init(other.c.get<bool>());
      break;
    case types::value_bit_s:
      c.init(other.c.get<dyn_bitstring>());
      break;
    case types::value_oct_s:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::value_prt_s:
      c.init(other.c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>());
      break;
    case types::value_real:
      c.init(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }
}
test_cond_value_c& test_cond_value_c::operator=(const test_cond_value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::value_int:
      c.set(other.c.get<int64_t>());
      break;
    case types::value_enum:
      c.set(other.c.get<int64_t>());
      break;
    case types::value_bool:
      c.set(other.c.get<bool>());
      break;
    case types::value_bit_s:
      c.set(other.c.get<dyn_bitstring>());
      break;
    case types::value_oct_s:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::value_prt_s:
      c.set(other.c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>());
      break;
    case types::value_real:
      c.set(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }

  return *this;
}
int64_t& test_cond_value_c::set_value_int()
{
  set(types::value_int);
  return c.get<int64_t>();
}
int64_t& test_cond_value_c::set_value_enum()
{
  set(types::value_enum);
  return c.get<int64_t>();
}
bool& test_cond_value_c::set_value_bool()
{
  set(types::value_bool);
  return c.get<bool>();
}
dyn_bitstring& test_cond_value_c::set_value_bit_s()
{
  set(types::value_bit_s);
  return c.get<dyn_bitstring>();
}
unbounded_octstring<true>& test_cond_value_c::set_value_oct_s()
{
  set(types::value_oct_s);
  return c.get<unbounded_octstring<true>>();
}
printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& test_cond_value_c::set_value_prt_s()
{
  set(types::value_prt_s);
  return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
}
real_s& test_cond_value_c::set_value_real()
{
  set(types::value_real);
  return c.get<real_s>();
}
void test_cond_value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::value_int:
      j.write_int("valueInt", c.get<int64_t>());
      break;
    case types::value_enum:
      j.write_int("valueEnum", c.get<int64_t>());
      break;
    case types::value_bool:
      j.write_bool("valueBool", c.get<bool>());
      break;
    case types::value_bit_s:
      j.write_str("valueBitS", c.get<dyn_bitstring>().to_string());
      break;
    case types::value_oct_s:
      j.write_str("valueOctS", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::value_prt_s:
      j.write_str("valuePrtS", c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>().to_string());
      break;
    case types::value_real:
      j.write_fieldname("valueReal");
      c.get<real_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }
  j.end_obj();
}
SRSASN_CODE test_cond_value_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(pack_unconstrained_integer(bref, c.get<int64_t>(), false, true));
      break;
    case types::value_enum:
      HANDLE_CODE(pack_unconstrained_integer(bref, c.get<int64_t>(), false, true));
      break;
    case types::value_bool:
      HANDLE_CODE(bref.pack(c.get<bool>(), 1));
      break;
    case types::value_bit_s:
      HANDLE_CODE(c.get<dyn_bitstring>().pack(bref));
      break;
    case types::value_oct_s:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::value_prt_s:
      HANDLE_CODE((c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>().pack(bref)));
      break;
    case types::value_real: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<real_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE test_cond_value_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(unpack_unconstrained_integer(c.get<int64_t>(), bref, false, true));
      break;
    case types::value_enum:
      HANDLE_CODE(unpack_unconstrained_integer(c.get<int64_t>(), bref, false, true));
      break;
    case types::value_bool:
      HANDLE_CODE(bref.unpack(c.get<bool>(), 1));
      break;
    case types::value_bit_s:
      HANDLE_CODE(c.get<dyn_bitstring>().unpack(bref));
      break;
    case types::value_oct_s:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::value_prt_s:
      HANDLE_CODE((c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>().unpack(bref)));
      break;
    case types::value_real: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<real_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* test_cond_value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "valueInt", "valueEnum", "valueBool", "valueBitS", "valueOctS", "valuePrtS", "valueReal"};
  return convert_enum_idx(names, 7, value, "test_cond_value_c::types");
}

// LogicalOR ::= ENUMERATED
const char* lc_or_opts::to_string() const
{
  static const char* names[] = {"true", "false"};
  return convert_enum_idx(names, 2, value, "lc_or_e");
}

// MeasurementLabel ::= SEQUENCE
SRSASN_CODE meas_label_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(no_label_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_present, 1));
  HANDLE_CODE(bref.pack(slice_id_present, 1));
  HANDLE_CODE(bref.pack(five_qi_present, 1));
  HANDLE_CODE(bref.pack(qfi_present, 1));
  HANDLE_CODE(bref.pack(qci_present, 1));
  HANDLE_CODE(bref.pack(qc_imax_present, 1));
  HANDLE_CODE(bref.pack(qc_imin_present, 1));
  HANDLE_CODE(bref.pack(ar_pmax_present, 1));
  HANDLE_CODE(bref.pack(ar_pmin_present, 1));
  HANDLE_CODE(bref.pack(bitrate_range_present, 1));
  HANDLE_CODE(bref.pack(layer_mu_mimo_present, 1));
  HANDLE_CODE(bref.pack(sum_present, 1));
  HANDLE_CODE(bref.pack(dist_bin_x_present, 1));
  HANDLE_CODE(bref.pack(dist_bin_y_present, 1));
  HANDLE_CODE(bref.pack(dist_bin_z_present, 1));
  HANDLE_CODE(bref.pack(pre_label_override_present, 1));
  HANDLE_CODE(bref.pack(start_end_ind_present, 1));
  HANDLE_CODE(bref.pack(min_present, 1));
  HANDLE_CODE(bref.pack(max_present, 1));
  HANDLE_CODE(bref.pack(avg_present, 1));

  if (no_label_present) {
    HANDLE_CODE(no_label.pack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  if (slice_id_present) {
    HANDLE_CODE(slice_id.pack(bref));
  }
  if (five_qi_present) {
    HANDLE_CODE(pack_integer(bref, five_qi, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (qfi_present) {
    HANDLE_CODE(pack_integer(bref, qfi, (uint8_t)0u, (uint8_t)63u, true, true));
  }
  if (qci_present) {
    HANDLE_CODE(pack_integer(bref, qci, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qc_imax_present) {
    HANDLE_CODE(pack_integer(bref, qc_imax, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qc_imin_present) {
    HANDLE_CODE(pack_integer(bref, qc_imin, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (ar_pmax_present) {
    HANDLE_CODE(pack_integer(bref, ar_pmax, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (ar_pmin_present) {
    HANDLE_CODE(pack_integer(bref, ar_pmin, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (bitrate_range_present) {
    HANDLE_CODE(pack_integer(bref, bitrate_range, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (layer_mu_mimo_present) {
    HANDLE_CODE(pack_integer(bref, layer_mu_mimo, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (sum_present) {
    HANDLE_CODE(sum.pack(bref));
  }
  if (dist_bin_x_present) {
    HANDLE_CODE(pack_integer(bref, dist_bin_x, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_y_present) {
    HANDLE_CODE(pack_integer(bref, dist_bin_y, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_z_present) {
    HANDLE_CODE(pack_integer(bref, dist_bin_z, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (pre_label_override_present) {
    HANDLE_CODE(pre_label_override.pack(bref));
  }
  if (start_end_ind_present) {
    HANDLE_CODE(start_end_ind.pack(bref));
  }
  if (min_present) {
    HANDLE_CODE(min.pack(bref));
  }
  if (max_present) {
    HANDLE_CODE(max.pack(bref));
  }
  if (avg_present) {
    HANDLE_CODE(avg.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(ssb_idx_present, 1));
    HANDLE_CODE(bref.pack(non_go_b_b_fmode_idx_present, 1));
    HANDLE_CODE(bref.pack(mimo_mode_idx_present, 1));
    HANDLE_CODE(bref.pack(cell_global_id.is_present(), 1));
    HANDLE_CODE(bref.pack(beam_id.is_present(), 1));

    if (ssb_idx_present) {
      HANDLE_CODE(pack_integer(bref, ssb_idx, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (non_go_b_b_fmode_idx_present) {
      HANDLE_CODE(pack_integer(bref, non_go_b_b_fmode_idx, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (mimo_mode_idx_present) {
      HANDLE_CODE(pack_integer(bref, mimo_mode_idx, (uint8_t)1u, (uint8_t)2u, true, true));
    }
    if (cell_global_id.is_present()) {
      HANDLE_CODE(cell_global_id->pack(bref));
    }
    if (beam_id.is_present()) {
      HANDLE_CODE(beam_id->pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_label_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(no_label_present, 1));
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));
  HANDLE_CODE(bref.unpack(slice_id_present, 1));
  HANDLE_CODE(bref.unpack(five_qi_present, 1));
  HANDLE_CODE(bref.unpack(qfi_present, 1));
  HANDLE_CODE(bref.unpack(qci_present, 1));
  HANDLE_CODE(bref.unpack(qc_imax_present, 1));
  HANDLE_CODE(bref.unpack(qc_imin_present, 1));
  HANDLE_CODE(bref.unpack(ar_pmax_present, 1));
  HANDLE_CODE(bref.unpack(ar_pmin_present, 1));
  HANDLE_CODE(bref.unpack(bitrate_range_present, 1));
  HANDLE_CODE(bref.unpack(layer_mu_mimo_present, 1));
  HANDLE_CODE(bref.unpack(sum_present, 1));
  HANDLE_CODE(bref.unpack(dist_bin_x_present, 1));
  HANDLE_CODE(bref.unpack(dist_bin_y_present, 1));
  HANDLE_CODE(bref.unpack(dist_bin_z_present, 1));
  HANDLE_CODE(bref.unpack(pre_label_override_present, 1));
  HANDLE_CODE(bref.unpack(start_end_ind_present, 1));
  HANDLE_CODE(bref.unpack(min_present, 1));
  HANDLE_CODE(bref.unpack(max_present, 1));
  HANDLE_CODE(bref.unpack(avg_present, 1));

  if (no_label_present) {
    HANDLE_CODE(no_label.unpack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  if (slice_id_present) {
    HANDLE_CODE(slice_id.unpack(bref));
  }
  if (five_qi_present) {
    HANDLE_CODE(unpack_integer(five_qi, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (qfi_present) {
    HANDLE_CODE(unpack_integer(qfi, bref, (uint8_t)0u, (uint8_t)63u, true, true));
  }
  if (qci_present) {
    HANDLE_CODE(unpack_integer(qci, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qc_imax_present) {
    HANDLE_CODE(unpack_integer(qc_imax, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qc_imin_present) {
    HANDLE_CODE(unpack_integer(qc_imin, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (ar_pmax_present) {
    HANDLE_CODE(unpack_integer(ar_pmax, bref, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (ar_pmin_present) {
    HANDLE_CODE(unpack_integer(ar_pmin, bref, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (bitrate_range_present) {
    HANDLE_CODE(unpack_integer(bitrate_range, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (layer_mu_mimo_present) {
    HANDLE_CODE(unpack_integer(layer_mu_mimo, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (sum_present) {
    HANDLE_CODE(sum.unpack(bref));
  }
  if (dist_bin_x_present) {
    HANDLE_CODE(unpack_integer(dist_bin_x, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_y_present) {
    HANDLE_CODE(unpack_integer(dist_bin_y, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_z_present) {
    HANDLE_CODE(unpack_integer(dist_bin_z, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (pre_label_override_present) {
    HANDLE_CODE(pre_label_override.unpack(bref));
  }
  if (start_end_ind_present) {
    HANDLE_CODE(start_end_ind.unpack(bref));
  }
  if (min_present) {
    HANDLE_CODE(min.unpack(bref));
  }
  if (max_present) {
    HANDLE_CODE(max.unpack(bref));
  }
  if (avg_present) {
    HANDLE_CODE(avg.unpack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(ssb_idx_present, 1));
    HANDLE_CODE(bref.unpack(non_go_b_b_fmode_idx_present, 1));
    HANDLE_CODE(bref.unpack(mimo_mode_idx_present, 1));
    unpack_presence_flag(cell_global_id, bref);
    unpack_presence_flag(beam_id, bref);

    if (ssb_idx_present) {
      HANDLE_CODE(unpack_integer(ssb_idx, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (non_go_b_b_fmode_idx_present) {
      HANDLE_CODE(unpack_integer(non_go_b_b_fmode_idx, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (mimo_mode_idx_present) {
      HANDLE_CODE(unpack_integer(mimo_mode_idx, bref, (uint8_t)1u, (uint8_t)2u, true, true));
    }
    if (cell_global_id.is_present()) {
      HANDLE_CODE(cell_global_id->unpack(bref));
    }
    if (beam_id.is_present()) {
      HANDLE_CODE(beam_id->unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_label_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (no_label_present) {
    j.write_str("noLabel", "true");
  }
  if (plmn_id_present) {
    j.write_str("plmnID", plmn_id.to_string());
  }
  if (slice_id_present) {
    j.write_fieldname("sliceID");
    slice_id.to_json(j);
  }
  if (five_qi_present) {
    j.write_int("fiveQI", five_qi);
  }
  if (qfi_present) {
    j.write_int("qFI", qfi);
  }
  if (qci_present) {
    j.write_int("qCI", qci);
  }
  if (qc_imax_present) {
    j.write_int("qCImax", qc_imax);
  }
  if (qc_imin_present) {
    j.write_int("qCImin", qc_imin);
  }
  if (ar_pmax_present) {
    j.write_int("aRPmax", ar_pmax);
  }
  if (ar_pmin_present) {
    j.write_int("aRPmin", ar_pmin);
  }
  if (bitrate_range_present) {
    j.write_int("bitrateRange", bitrate_range);
  }
  if (layer_mu_mimo_present) {
    j.write_int("layerMU-MIMO", layer_mu_mimo);
  }
  if (sum_present) {
    j.write_str("sUM", "true");
  }
  if (dist_bin_x_present) {
    j.write_int("distBinX", dist_bin_x);
  }
  if (dist_bin_y_present) {
    j.write_int("distBinY", dist_bin_y);
  }
  if (dist_bin_z_present) {
    j.write_int("distBinZ", dist_bin_z);
  }
  if (pre_label_override_present) {
    j.write_str("preLabelOverride", "true");
  }
  if (start_end_ind_present) {
    j.write_str("startEndInd", start_end_ind.to_string());
  }
  if (min_present) {
    j.write_str("min", "true");
  }
  if (max_present) {
    j.write_str("max", "true");
  }
  if (avg_present) {
    j.write_str("avg", "true");
  }
  if (ext) {
    if (ssb_idx_present) {
      j.write_int("ssbIndex", ssb_idx);
    }
    if (non_go_b_b_fmode_idx_present) {
      j.write_int("nonGoB-BFmode-Index", non_go_b_b_fmode_idx);
    }
    if (mimo_mode_idx_present) {
      j.write_int("mIMO-mode-Index", mimo_mode_idx);
    }
    if (cell_global_id.is_present()) {
      j.write_fieldname("cellGlobalID");
      cell_global_id->to_json(j);
    }
    if (beam_id.is_present()) {
      j.write_fieldname("beamID");
      beam_id->to_json(j);
    }
  }
  j.end_obj();
}

const char* meas_label_s::no_label_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "meas_label_s::no_label_e_");
}

const char* meas_label_s::sum_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "meas_label_s::sum_e_");
}

const char* meas_label_s::pre_label_override_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "meas_label_s::pre_label_override_e_");
}

const char* meas_label_s::start_end_ind_opts::to_string() const
{
  static const char* names[] = {"start", "end"};
  return convert_enum_idx(names, 2, value, "meas_label_s::start_end_ind_e_");
}

const char* meas_label_s::min_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "meas_label_s::min_e_");
}

const char* meas_label_s::max_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "meas_label_s::max_e_");
}

const char* meas_label_s::avg_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "meas_label_s::avg_e_");
}

// MeasValueReportCond ::= SEQUENCE
SRSASN_CODE meas_value_report_cond_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(test_expr.pack(bref));
  HANDLE_CODE(test_value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_value_report_cond_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(test_expr.unpack(bref));
  HANDLE_CODE(test_value.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_value_report_cond_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("testExpr", test_expr.to_string());
  j.write_fieldname("testValue");
  test_value.to_json(j);
  j.end_obj();
}

// BinRangeValue ::= CHOICE
void bin_range_value_c::destroy_()
{
  switch (type_) {
    case types::value_real:
      c.destroy<real_s>();
      break;
    default:
      break;
  }
}
void bin_range_value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::value_int:
      break;
    case types::value_real:
      c.init<real_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }
}
bin_range_value_c::bin_range_value_c(const bin_range_value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::value_int:
      c.init(other.c.get<int64_t>());
      break;
    case types::value_real:
      c.init(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }
}
bin_range_value_c& bin_range_value_c::operator=(const bin_range_value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::value_int:
      c.set(other.c.get<int64_t>());
      break;
    case types::value_real:
      c.set(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }

  return *this;
}
int64_t& bin_range_value_c::set_value_int()
{
  set(types::value_int);
  return c.get<int64_t>();
}
real_s& bin_range_value_c::set_value_real()
{
  set(types::value_real);
  return c.get<real_s>();
}
void bin_range_value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::value_int:
      j.write_int("valueInt", c.get<int64_t>());
      break;
    case types::value_real:
      j.write_fieldname("valueReal");
      c.get<real_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }
  j.end_obj();
}
SRSASN_CODE bin_range_value_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(pack_unconstrained_integer(bref, c.get<int64_t>(), false, true));
      break;
    case types::value_real:
      HANDLE_CODE(c.get<real_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bin_range_value_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(unpack_unconstrained_integer(c.get<int64_t>(), bref, false, true));
      break;
    case types::value_real:
      HANDLE_CODE(c.get<real_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bin_range_value_c::types_opts::to_string() const
{
  static const char* names[] = {"valueInt", "valueReal"};
  return convert_enum_idx(names, 2, value, "bin_range_value_c::types");
}

// LabelInfoItem ::= SEQUENCE
SRSASN_CODE label_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_label.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE label_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_label.unpack(bref));

  return SRSASN_SUCCESS;
}
void label_info_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measLabel");
  meas_label.to_json(j);
  j.end_obj();
}

// MatchCondReportItem ::= SEQUENCE
SRSASN_CODE match_cond_report_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lc_or_present, 1));

  HANDLE_CODE(meas_value_report_cond.pack(bref));
  if (lc_or_present) {
    HANDLE_CODE(lc_or.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE match_cond_report_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lc_or_present, 1));

  HANDLE_CODE(meas_value_report_cond.unpack(bref));
  if (lc_or_present) {
    HANDLE_CODE(lc_or.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void match_cond_report_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measValueReportCond");
  meas_value_report_cond.to_json(j);
  if (lc_or_present) {
    j.write_str("logicalOR", lc_or.to_string());
  }
  j.end_obj();
}

// MeasurementRecordItem ::= CHOICE
void meas_record_item_c::destroy_()
{
  switch (type_) {
    case types::real:
      c.destroy<real_s>();
      break;
    default:
      break;
  }
}
void meas_record_item_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::integer:
      break;
    case types::real:
      c.init<real_s>();
      break;
    case types::no_value:
      break;
    case types::not_satisfied:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }
}
meas_record_item_c::meas_record_item_c(const meas_record_item_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::integer:
      c.init(other.c.get<uint64_t>());
      break;
    case types::real:
      c.init(other.c.get<real_s>());
      break;
    case types::no_value:
      break;
    case types::not_satisfied:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }
}
meas_record_item_c& meas_record_item_c::operator=(const meas_record_item_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::integer:
      c.set(other.c.get<uint64_t>());
      break;
    case types::real:
      c.set(other.c.get<real_s>());
      break;
    case types::no_value:
      break;
    case types::not_satisfied:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }

  return *this;
}
uint64_t& meas_record_item_c::set_integer()
{
  set(types::integer);
  return c.get<uint64_t>();
}
real_s& meas_record_item_c::set_real()
{
  set(types::real);
  return c.get<real_s>();
}
void meas_record_item_c::set_no_value()
{
  set(types::no_value);
}
void meas_record_item_c::set_not_satisfied()
{
  set(types::not_satisfied);
}
void meas_record_item_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::integer:
      j.write_int("integer", c.get<uint64_t>());
      break;
    case types::real:
      j.write_fieldname("real");
      c.get<real_s>().to_json(j);
      break;
    case types::no_value:
      break;
    case types::not_satisfied:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_record_item_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::integer:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::real:
      HANDLE_CODE(c.get<real_s>().pack(bref));
      break;
    case types::no_value:
      break;
    case types::not_satisfied:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_record_item_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::integer:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::real:
      HANDLE_CODE(c.get<real_s>().unpack(bref));
      break;
    case types::no_value:
      break;
    case types::not_satisfied:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_record_item_c::types_opts::to_string() const
{
  static const char* names[] = {"integer", "real", "noValue", "notSatisfied"};
  return convert_enum_idx(names, 4, value, "meas_record_item_c::types");
}

// TestCond-Expression ::= ENUMERATED
const char* test_cond_expression_opts::to_string() const
{
  static const char* names[] = {"equal", "greaterthan", "lessthan", "contains", "present"};
  return convert_enum_idx(names, 5, value, "test_cond_expression_e");
}

// TestCond-Type ::= CHOICE
void test_cond_type_c::destroy_() {}
void test_cond_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
test_cond_type_c::test_cond_type_c(const test_cond_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gbr:
      c.init(other.c.get<gbr_e_>());
      break;
    case types::ambr:
      c.init(other.c.get<ambr_e_>());
      break;
    case types::is_stat:
      c.init(other.c.get<is_stat_e_>());
      break;
    case types::is_cat_m:
      c.init(other.c.get<is_cat_m_e_>());
      break;
    case types::rsrp:
      c.init(other.c.get<rsrp_e_>());
      break;
    case types::rsrq:
      c.init(other.c.get<rsrq_e_>());
      break;
    case types::ul_r_srp:
      c.init(other.c.get<ul_r_srp_e_>());
      break;
    case types::cqi:
      c.init(other.c.get<cqi_e_>());
      break;
    case types::five_qi:
      c.init(other.c.get<five_qi_e_>());
      break;
    case types::qci:
      c.init(other.c.get<qci_e_>());
      break;
    case types::snssai:
      c.init(other.c.get<snssai_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
  }
}
test_cond_type_c& test_cond_type_c::operator=(const test_cond_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gbr:
      c.set(other.c.get<gbr_e_>());
      break;
    case types::ambr:
      c.set(other.c.get<ambr_e_>());
      break;
    case types::is_stat:
      c.set(other.c.get<is_stat_e_>());
      break;
    case types::is_cat_m:
      c.set(other.c.get<is_cat_m_e_>());
      break;
    case types::rsrp:
      c.set(other.c.get<rsrp_e_>());
      break;
    case types::rsrq:
      c.set(other.c.get<rsrq_e_>());
      break;
    case types::ul_r_srp:
      c.set(other.c.get<ul_r_srp_e_>());
      break;
    case types::cqi:
      c.set(other.c.get<cqi_e_>());
      break;
    case types::five_qi:
      c.set(other.c.get<five_qi_e_>());
      break;
    case types::qci:
      c.set(other.c.get<qci_e_>());
      break;
    case types::snssai:
      c.set(other.c.get<snssai_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
  }

  return *this;
}
test_cond_type_c::gbr_e_& test_cond_type_c::set_gbr()
{
  set(types::gbr);
  return c.get<gbr_e_>();
}
test_cond_type_c::ambr_e_& test_cond_type_c::set_ambr()
{
  set(types::ambr);
  return c.get<ambr_e_>();
}
test_cond_type_c::is_stat_e_& test_cond_type_c::set_is_stat()
{
  set(types::is_stat);
  return c.get<is_stat_e_>();
}
test_cond_type_c::is_cat_m_e_& test_cond_type_c::set_is_cat_m()
{
  set(types::is_cat_m);
  return c.get<is_cat_m_e_>();
}
test_cond_type_c::rsrp_e_& test_cond_type_c::set_rsrp()
{
  set(types::rsrp);
  return c.get<rsrp_e_>();
}
test_cond_type_c::rsrq_e_& test_cond_type_c::set_rsrq()
{
  set(types::rsrq);
  return c.get<rsrq_e_>();
}
test_cond_type_c::ul_r_srp_e_& test_cond_type_c::set_ul_r_srp()
{
  set(types::ul_r_srp);
  return c.get<ul_r_srp_e_>();
}
test_cond_type_c::cqi_e_& test_cond_type_c::set_cqi()
{
  set(types::cqi);
  return c.get<cqi_e_>();
}
test_cond_type_c::five_qi_e_& test_cond_type_c::set_five_qi()
{
  set(types::five_qi);
  return c.get<five_qi_e_>();
}
test_cond_type_c::qci_e_& test_cond_type_c::set_qci()
{
  set(types::qci);
  return c.get<qci_e_>();
}
test_cond_type_c::snssai_e_& test_cond_type_c::set_snssai()
{
  set(types::snssai);
  return c.get<snssai_e_>();
}
void test_cond_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gbr:
      j.write_str("gBR", "true");
      break;
    case types::ambr:
      j.write_str("aMBR", "true");
      break;
    case types::is_stat:
      j.write_str("isStat", "true");
      break;
    case types::is_cat_m:
      j.write_str("isCatM", "true");
      break;
    case types::rsrp:
      j.write_str("rSRP", "true");
      break;
    case types::rsrq:
      j.write_str("rSRQ", "true");
      break;
    case types::ul_r_srp:
      j.write_str("ul-rSRP", "true");
      break;
    case types::cqi:
      j.write_str("cQI", "true");
      break;
    case types::five_qi:
      j.write_str("fiveQI", "true");
      break;
    case types::qci:
      j.write_str("qCI", "true");
      break;
    case types::snssai:
      j.write_str("sNSSAI", "true");
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
  }
  j.end_obj();
}
SRSASN_CODE test_cond_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gbr:
      HANDLE_CODE(c.get<gbr_e_>().pack(bref));
      break;
    case types::ambr:
      HANDLE_CODE(c.get<ambr_e_>().pack(bref));
      break;
    case types::is_stat:
      HANDLE_CODE(c.get<is_stat_e_>().pack(bref));
      break;
    case types::is_cat_m:
      HANDLE_CODE(c.get<is_cat_m_e_>().pack(bref));
      break;
    case types::rsrp:
      HANDLE_CODE(c.get<rsrp_e_>().pack(bref));
      break;
    case types::rsrq:
      HANDLE_CODE(c.get<rsrq_e_>().pack(bref));
      break;
    case types::ul_r_srp: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<ul_r_srp_e_>().pack(bref));
    } break;
    case types::cqi: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<cqi_e_>().pack(bref));
    } break;
    case types::five_qi: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<five_qi_e_>().pack(bref));
    } break;
    case types::qci: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<qci_e_>().pack(bref));
    } break;
    case types::snssai: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<snssai_e_>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE test_cond_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gbr:
      HANDLE_CODE(c.get<gbr_e_>().unpack(bref));
      break;
    case types::ambr:
      HANDLE_CODE(c.get<ambr_e_>().unpack(bref));
      break;
    case types::is_stat:
      HANDLE_CODE(c.get<is_stat_e_>().unpack(bref));
      break;
    case types::is_cat_m:
      HANDLE_CODE(c.get<is_cat_m_e_>().unpack(bref));
      break;
    case types::rsrp:
      HANDLE_CODE(c.get<rsrp_e_>().unpack(bref));
      break;
    case types::rsrq:
      HANDLE_CODE(c.get<rsrq_e_>().unpack(bref));
      break;
    case types::ul_r_srp: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<ul_r_srp_e_>().unpack(bref));
    } break;
    case types::cqi: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<cqi_e_>().unpack(bref));
    } break;
    case types::five_qi: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<five_qi_e_>().unpack(bref));
    } break;
    case types::qci: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<qci_e_>().unpack(bref));
    } break;
    case types::snssai: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<snssai_e_>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* test_cond_type_c::gbr_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::gbr_e_");
}

const char* test_cond_type_c::ambr_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::ambr_e_");
}

const char* test_cond_type_c::is_stat_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::is_stat_e_");
}

const char* test_cond_type_c::is_cat_m_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::is_cat_m_e_");
}

const char* test_cond_type_c::rsrp_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::rsrp_e_");
}

const char* test_cond_type_c::rsrq_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::rsrq_e_");
}

const char* test_cond_type_c::ul_r_srp_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::ul_r_srp_e_");
}

const char* test_cond_type_c::cqi_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::cqi_e_");
}

const char* test_cond_type_c::five_qi_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::five_qi_e_");
}

const char* test_cond_type_c::qci_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::qci_e_");
}

const char* test_cond_type_c::snssai_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "test_cond_type_c::snssai_e_");
}

const char* test_cond_type_c::types_opts::to_string() const
{
  static const char* names[] = {
      "gBR", "aMBR", "isStat", "isCatM", "rSRP", "rSRQ", "ul-rSRP", "cQI", "fiveQI", "qCI", "sNSSAI"};
  return convert_enum_idx(names, 11, value, "test_cond_type_c::types");
}
uint8_t test_cond_type_c::types_opts::to_number() const
{
  if (value == five_qi) {
    return 5;
  }
  invalid_enum_number(value, "test_cond_type_c::types");
  return 0;
}

// BinRangeItem ::= SEQUENCE
SRSASN_CODE bin_range_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, bin_idx, (uint32_t)1u, (uint32_t)65535u, true, true));
  HANDLE_CODE(start_value.pack(bref));
  HANDLE_CODE(end_value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bin_range_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(bin_idx, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  HANDLE_CODE(start_value.unpack(bref));
  HANDLE_CODE(end_value.unpack(bref));

  return SRSASN_SUCCESS;
}
void bin_range_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("binIndex", bin_idx);
  j.write_fieldname("startValue");
  start_value.to_json(j);
  j.write_fieldname("endValue");
  end_value.to_json(j);
  j.end_obj();
}

// MatchingUEidItem-PerGP ::= SEQUENCE
SRSASN_CODE matching_ue_id_item_per_gp_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ue_id_item_per_gp_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ue_id_item_per_gp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.end_obj();
}

// MeasurementType ::= CHOICE
void meas_type_c::destroy_()
{
  switch (type_) {
    case types::meas_name:
      c.destroy<printable_string<1, 150, true, true>>();
      break;
    default:
      break;
  }
}
void meas_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_name:
      c.init<printable_string<1, 150, true, true>>();
      break;
    case types::meas_id:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }
}
meas_type_c::meas_type_c(const meas_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_name:
      c.init(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::meas_id:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }
}
meas_type_c& meas_type_c::operator=(const meas_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_name:
      c.set(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::meas_id:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }

  return *this;
}
printable_string<1, 150, true, true>& meas_type_c::set_meas_name()
{
  set(types::meas_name);
  return c.get<printable_string<1, 150, true, true>>();
}
uint32_t& meas_type_c::set_meas_id()
{
  set(types::meas_id);
  return c.get<uint32_t>();
}
void meas_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_name:
      j.write_str("measName", c.get<printable_string<1, 150, true, true>>().to_string());
      break;
    case types::meas_id:
      j.write_int("measID", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().pack(bref)));
      break;
    case types::meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)1u, (uint32_t)65536u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().unpack(bref)));
      break;
    case types::meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)1u, (uint32_t)65536u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_type_c::types_opts::to_string() const
{
  static const char* names[] = {"measName", "measID"};
  return convert_enum_idx(names, 2, value, "meas_type_c::types");
}

// TestCondInfo ::= SEQUENCE
SRSASN_CODE test_cond_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(test_expr_present, 1));
  HANDLE_CODE(bref.pack(test_value_present, 1));

  HANDLE_CODE(test_type.pack(bref));
  if (test_expr_present) {
    HANDLE_CODE(test_expr.pack(bref));
  }
  if (test_value_present) {
    HANDLE_CODE(test_value.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE test_cond_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(test_expr_present, 1));
  HANDLE_CODE(bref.unpack(test_value_present, 1));

  HANDLE_CODE(test_type.unpack(bref));
  if (test_expr_present) {
    HANDLE_CODE(test_expr.unpack(bref));
  }
  if (test_value_present) {
    HANDLE_CODE(test_value.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void test_cond_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("testType");
  test_type.to_json(j);
  if (test_expr_present) {
    j.write_str("testExpr", test_expr.to_string());
  }
  if (test_value_present) {
    j.write_fieldname("testValue");
    test_value.to_json(j);
  }
  j.end_obj();
}

// MatchingCondItem-Choice ::= CHOICE
void matching_cond_item_choice_c::destroy_()
{
  switch (type_) {
    case types::meas_label:
      c.destroy<meas_label_s>();
      break;
    case types::test_cond_info:
      c.destroy<test_cond_info_s>();
      break;
    default:
      break;
  }
}
void matching_cond_item_choice_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_label:
      c.init<meas_label_s>();
      break;
    case types::test_cond_info:
      c.init<test_cond_info_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }
}
matching_cond_item_choice_c::matching_cond_item_choice_c(const matching_cond_item_choice_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_label:
      c.init(other.c.get<meas_label_s>());
      break;
    case types::test_cond_info:
      c.init(other.c.get<test_cond_info_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }
}
matching_cond_item_choice_c& matching_cond_item_choice_c::operator=(const matching_cond_item_choice_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_label:
      c.set(other.c.get<meas_label_s>());
      break;
    case types::test_cond_info:
      c.set(other.c.get<test_cond_info_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }

  return *this;
}
meas_label_s& matching_cond_item_choice_c::set_meas_label()
{
  set(types::meas_label);
  return c.get<meas_label_s>();
}
test_cond_info_s& matching_cond_item_choice_c::set_test_cond_info()
{
  set(types::test_cond_info);
  return c.get<test_cond_info_s>();
}
void matching_cond_item_choice_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_label:
      j.write_fieldname("measLabel");
      c.get<meas_label_s>().to_json(j);
      break;
    case types::test_cond_info:
      j.write_fieldname("testCondInfo");
      c.get<test_cond_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }
  j.end_obj();
}
SRSASN_CODE matching_cond_item_choice_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_label:
      HANDLE_CODE(c.get<meas_label_s>().pack(bref));
      break;
    case types::test_cond_info:
      HANDLE_CODE(c.get<test_cond_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_cond_item_choice_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_label:
      HANDLE_CODE(c.get<meas_label_s>().unpack(bref));
      break;
    case types::test_cond_info:
      HANDLE_CODE(c.get<test_cond_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* matching_cond_item_choice_c::types_opts::to_string() const
{
  static const char* names[] = {"measLabel", "testCondInfo"};
  return convert_enum_idx(names, 2, value, "matching_cond_item_choice_c::types");
}

// MeasurementDataItem ::= SEQUENCE
SRSASN_CODE meas_data_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(incomplete_flag_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_record, 1, 2147483647, true));
  if (incomplete_flag_present) {
    HANDLE_CODE(incomplete_flag.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_data_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(incomplete_flag_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_record, bref, 1, 2147483647, true));
  if (incomplete_flag_present) {
    HANDLE_CODE(incomplete_flag.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_data_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measRecord");
  for (const auto& e1 : meas_record) {
    e1.to_json(j);
  }
  j.end_array();
  if (incomplete_flag_present) {
    j.write_str("incompleteFlag", "true");
  }
  j.end_obj();
}

const char* meas_data_item_s::incomplete_flag_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "meas_data_item_s::incomplete_flag_e_");
}

// MeasurementInfoItem ::= SEQUENCE
SRSASN_CODE meas_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, label_info_list, 1, 2147483647, true));

  if (ext) {
    HANDLE_CODE(bref.pack(match_cond_report_list.is_present(), 1));

    if (match_cond_report_list.is_present()) {
      HANDLE_CODE(pack_dyn_seq_of(bref, *match_cond_report_list, 1, 32768, true));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(label_info_list, bref, 1, 2147483647, true));

  if (ext) {
    unpack_presence_flag(match_cond_report_list, bref);

    if (match_cond_report_list.is_present()) {
      HANDLE_CODE(unpack_dyn_seq_of(*match_cond_report_list, bref, 1, 32768, true));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_info_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.start_array("labelInfoList");
  for (const auto& e1 : label_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (match_cond_report_list.is_present()) {
      j.start_array("matchCondReportList");
      for (const auto& e1 : *match_cond_report_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// BinRangeDefinition ::= SEQUENCE
SRSASN_CODE bin_range_definition_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(bin_range_list_y.size() > 0, 1));
  HANDLE_CODE(bref.pack(bin_range_list_z.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, bin_range_list_x, 1, 65535, true));
  if (bin_range_list_y.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bin_range_list_y, 1, 65535, true));
  }
  if (bin_range_list_z.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bin_range_list_z, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bin_range_definition_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool bin_range_list_y_present;
  HANDLE_CODE(bref.unpack(bin_range_list_y_present, 1));
  bool bin_range_list_z_present;
  HANDLE_CODE(bref.unpack(bin_range_list_z_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(bin_range_list_x, bref, 1, 65535, true));
  if (bin_range_list_y_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bin_range_list_y, bref, 1, 65535, true));
  }
  if (bin_range_list_z_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bin_range_list_z, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void bin_range_definition_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("binRangeListX");
  for (const auto& e1 : bin_range_list_x) {
    e1.to_json(j);
  }
  j.end_array();
  if (bin_range_list_y.size() > 0) {
    j.start_array("binRangeListY");
    for (const auto& e1 : bin_range_list_y) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bin_range_list_z.size() > 0) {
    j.start_array("binRangeListZ");
    for (const auto& e1 : bin_range_list_z) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MatchingCondItem ::= SEQUENCE
SRSASN_CODE matching_cond_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lc_or_present, 1));

  HANDLE_CODE(matching_cond_choice.pack(bref));
  if (lc_or_present) {
    HANDLE_CODE(lc_or.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_cond_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lc_or_present, 1));

  HANDLE_CODE(matching_cond_choice.unpack(bref));
  if (lc_or_present) {
    HANDLE_CODE(lc_or.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void matching_cond_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("matchingCondChoice");
  matching_cond_choice.to_json(j);
  if (lc_or_present) {
    j.write_str("logicalOR", lc_or.to_string());
  }
  j.end_obj();
}

// MatchingUEidItem ::= SEQUENCE
SRSASN_CODE matching_ue_id_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ue_id_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ue_id_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.end_obj();
}

// MatchingUEidPerGP-Item ::= SEQUENCE
SRSASN_CODE matching_ue_id_per_gp_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(matched_per_gp.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ue_id_per_gp_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(matched_per_gp.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ue_id_per_gp_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("matchedPerGP");
  matched_per_gp.to_json(j);
  j.end_obj();
}

void matching_ue_id_per_gp_item_s::matched_per_gp_c_::destroy_()
{
  switch (type_) {
    case types::one_or_more_ue_matched:
      c.destroy<matching_ue_id_list_per_gp_l>();
      break;
    default:
      break;
  }
}
void matching_ue_id_per_gp_item_s::matched_per_gp_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::no_ue_matched:
      break;
    case types::one_or_more_ue_matched:
      c.init<matching_ue_id_list_per_gp_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_ue_id_per_gp_item_s::matched_per_gp_c_");
  }
}
matching_ue_id_per_gp_item_s::matched_per_gp_c_::matched_per_gp_c_(
    const matching_ue_id_per_gp_item_s::matched_per_gp_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::no_ue_matched:
      c.init(other.c.get<no_ue_matched_e_>());
      break;
    case types::one_or_more_ue_matched:
      c.init(other.c.get<matching_ue_id_list_per_gp_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_ue_id_per_gp_item_s::matched_per_gp_c_");
  }
}
matching_ue_id_per_gp_item_s::matched_per_gp_c_&
matching_ue_id_per_gp_item_s::matched_per_gp_c_::operator=(const matching_ue_id_per_gp_item_s::matched_per_gp_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::no_ue_matched:
      c.set(other.c.get<no_ue_matched_e_>());
      break;
    case types::one_or_more_ue_matched:
      c.set(other.c.get<matching_ue_id_list_per_gp_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_ue_id_per_gp_item_s::matched_per_gp_c_");
  }

  return *this;
}
matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_&
matching_ue_id_per_gp_item_s::matched_per_gp_c_::set_no_ue_matched()
{
  set(types::no_ue_matched);
  return c.get<no_ue_matched_e_>();
}
matching_ue_id_list_per_gp_l& matching_ue_id_per_gp_item_s::matched_per_gp_c_::set_one_or_more_ue_matched()
{
  set(types::one_or_more_ue_matched);
  return c.get<matching_ue_id_list_per_gp_l>();
}
void matching_ue_id_per_gp_item_s::matched_per_gp_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::no_ue_matched:
      j.write_str("noUEmatched", "true");
      break;
    case types::one_or_more_ue_matched:
      j.start_array("oneOrMoreUEmatched");
      for (const auto& e1 : c.get<matching_ue_id_list_per_gp_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "matching_ue_id_per_gp_item_s::matched_per_gp_c_");
  }
  j.end_obj();
}
SRSASN_CODE matching_ue_id_per_gp_item_s::matched_per_gp_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::no_ue_matched:
      HANDLE_CODE(c.get<no_ue_matched_e_>().pack(bref));
      break;
    case types::one_or_more_ue_matched:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<matching_ue_id_list_per_gp_l>(), 1, 65535, true));
      break;
    default:
      log_invalid_choice_id(type_, "matching_ue_id_per_gp_item_s::matched_per_gp_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ue_id_per_gp_item_s::matched_per_gp_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::no_ue_matched:
      HANDLE_CODE(c.get<no_ue_matched_e_>().unpack(bref));
      break;
    case types::one_or_more_ue_matched:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<matching_ue_id_list_per_gp_l>(), bref, 1, 65535, true));
      break;
    default:
      log_invalid_choice_id(type_, "matching_ue_id_per_gp_item_s::matched_per_gp_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_");
}

const char* matching_ue_id_per_gp_item_s::matched_per_gp_c_::types_opts::to_string() const
{
  static const char* names[] = {"noUEmatched", "oneOrMoreUEmatched"};
  return convert_enum_idx(names, 2, value, "matching_ue_id_per_gp_item_s::matched_per_gp_c_::types");
}
uint8_t matching_ue_id_per_gp_item_s::matched_per_gp_c_::types_opts::to_number() const
{
  if (value == one_or_more_ue_matched) {
    return 1;
  }
  invalid_enum_number(value, "matching_ue_id_per_gp_item_s::matched_per_gp_c_::types");
  return 0;
}

// DistMeasurementBinRangeItem ::= SEQUENCE
SRSASN_CODE dist_meas_bin_range_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(bin_range_def.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dist_meas_bin_range_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(bin_range_def.unpack(bref));

  return SRSASN_SUCCESS;
}
void dist_meas_bin_range_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.write_fieldname("binRangeDef");
  bin_range_def.to_json(j);
  j.end_obj();
}

// E2SM-KPM-IndicationMessage-Format1 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_ind_msg_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(granul_period_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_data, 1, 65535, true));
  if (meas_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_info_list, 1, 65535, true));
  }
  if (granul_period_present) {
    HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_msg_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_info_list_present;
  HANDLE_CODE(bref.unpack(meas_info_list_present, 1));
  HANDLE_CODE(bref.unpack(granul_period_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_data, bref, 1, 65535, true));
  if (meas_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_info_list, bref, 1, 65535, true));
  }
  if (granul_period_present) {
    HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2sm_kpm_ind_msg_format1_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage-Format1");
  j.start_array("measData");
  for (const auto& e1 : meas_data) {
    e1.to_json(j);
  }
  j.end_array();
  if (meas_info_list.size() > 0) {
    j.start_array("measInfoList");
    for (const auto& e1 : meas_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (granul_period_present) {
    j.write_int("granulPeriod", granul_period);
  }
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// MatchingUeCondPerSubItem ::= SEQUENCE
SRSASN_CODE matching_ue_cond_per_sub_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(test_cond_info.pack(bref));

  if (ext) {
    HANDLE_CODE(bref.pack(lc_or_present, 1));

    if (lc_or_present) {
      HANDLE_CODE(lc_or.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ue_cond_per_sub_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(test_cond_info.unpack(bref));

  if (ext) {
    HANDLE_CODE(bref.unpack(lc_or_present, 1));

    if (lc_or_present) {
      HANDLE_CODE(lc_or.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void matching_ue_cond_per_sub_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("testCondInfo");
  test_cond_info.to_json(j);
  if (ext) {
    if (lc_or_present) {
      j.write_str("logicalOR", lc_or.to_string());
    }
  }
  j.end_obj();
}

// MatchingUEidPerSubItem ::= SEQUENCE
SRSASN_CODE matching_ue_id_per_sub_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ue_id_per_sub_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ue_id_per_sub_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.end_obj();
}

// MeasurementCondItem ::= SEQUENCE
SRSASN_CODE meas_cond_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_cond, 1, 32768, true));

  if (ext) {
    HANDLE_CODE(bref.pack(bin_range_def.is_present(), 1));

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cond_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(matching_cond, bref, 1, 32768, true));

  if (ext) {
    unpack_presence_flag(bin_range_def, bref);

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cond_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.start_array("matchingCond");
  for (const auto& e1 : matching_cond) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (bin_range_def.is_present()) {
      j.write_fieldname("binRangeDef");
      bin_range_def->to_json(j);
    }
  }
  j.end_obj();
}

// MeasurementCondUEidItem ::= SEQUENCE
SRSASN_CODE meas_cond_ue_id_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(matching_ue_id_list.size() > 0, 1));

  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_cond, 1, 32768, true));
  if (matching_ue_id_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, matching_ue_id_list, 1, 65535, true));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(matching_ue_id_per_gp.is_present(), 1));

    if (matching_ue_id_per_gp.is_present()) {
      HANDLE_CODE(pack_dyn_seq_of(bref, *matching_ue_id_per_gp, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cond_ue_id_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool matching_ue_id_list_present;
  HANDLE_CODE(bref.unpack(matching_ue_id_list_present, 1));

  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(matching_cond, bref, 1, 32768, true));
  if (matching_ue_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(matching_ue_id_list, bref, 1, 65535, true));
  }

  if (ext) {
    unpack_presence_flag(matching_ue_id_per_gp, bref);

    if (matching_ue_id_per_gp.is_present()) {
      HANDLE_CODE(unpack_dyn_seq_of(*matching_ue_id_per_gp, bref, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cond_ue_id_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.start_array("matchingCond");
  for (const auto& e1 : matching_cond) {
    e1.to_json(j);
  }
  j.end_array();
  if (matching_ue_id_list.size() > 0) {
    j.start_array("matchingUEidList");
    for (const auto& e1 : matching_ue_id_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (matching_ue_id_per_gp.is_present()) {
      j.start_array("matchingUEidPerGP");
      for (const auto& e1 : *matching_ue_id_per_gp) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// MeasurementInfo-Action-Item ::= SEQUENCE
SRSASN_CODE meas_info_action_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_id_present, 1));

  HANDLE_CODE(meas_name.pack(bref));
  if (meas_id_present) {
    HANDLE_CODE(pack_integer(bref, meas_id, (uint32_t)1u, (uint32_t)65536u, true, true));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(bin_range_def.is_present(), 1));

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_info_action_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_id_present, 1));

  HANDLE_CODE(meas_name.unpack(bref));
  if (meas_id_present) {
    HANDLE_CODE(unpack_integer(meas_id, bref, (uint32_t)1u, (uint32_t)65536u, true, true));
  }

  if (ext) {
    unpack_presence_flag(bin_range_def, bref);

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_info_action_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measName", meas_name.to_string());
  if (meas_id_present) {
    j.write_int("measID", meas_id);
  }
  if (ext) {
    if (bin_range_def.is_present()) {
      j.write_fieldname("binRangeDef");
      bin_range_def->to_json(j);
    }
  }
  j.end_obj();
}

// UEMeasurementReportItem ::= SEQUENCE
SRSASN_CODE ue_meas_report_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(meas_report.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_meas_report_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(meas_report.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_meas_report_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.write_fieldname("measReport");
  meas_report.to_json(j);
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format1 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_action_definition_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_global_id_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_info_list, 1, 65535, true));
  HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(dist_meas_bin_range_info.is_present(), 1));

    if (dist_meas_bin_range_info.is_present()) {
      HANDLE_CODE(pack_dyn_seq_of(bref, *dist_meas_bin_range_info, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_action_definition_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_global_id_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_info_list, bref, 1, 65535, true));
  HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.unpack(bref));
  }

  if (ext) {
    unpack_presence_flag(dist_meas_bin_range_info, bref);

    if (dist_meas_bin_range_info.is_present()) {
      HANDLE_CODE(unpack_dyn_seq_of(*dist_meas_bin_range_info, bref, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
void e2sm_kpm_action_definition_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measInfoList");
  for (const auto& e1 : meas_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("granulPeriod", granul_period);
  if (cell_global_id_present) {
    j.write_fieldname("cellGlobalID");
    cell_global_id.to_json(j);
  }
  if (ext) {
    if (dist_meas_bin_range_info.is_present()) {
      j.start_array("distMeasBinRangeInfo");
      for (const auto& e1 : *dist_meas_bin_range_info) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format2 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_action_definition_format2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(subscript_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_action_definition_format2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(subscript_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2sm_kpm_action_definition_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.write_fieldname("subscriptInfo");
  subscript_info.to_json(j);
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format3 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_action_definition_format3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_global_id_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_cond_list, 1, 65535, true));
  HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_action_definition_format3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_global_id_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_cond_list, bref, 1, 65535, true));
  HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2sm_kpm_action_definition_format3_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measCondList");
  for (const auto& e1 : meas_cond_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("granulPeriod", granul_period);
  if (cell_global_id_present) {
    j.write_fieldname("cellGlobalID");
    cell_global_id.to_json(j);
  }
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format4 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_action_definition_format4_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_ue_cond_list, 1, 32768, true));
  HANDLE_CODE(sub_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_action_definition_format4_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(matching_ue_cond_list, bref, 1, 32768, true));
  HANDLE_CODE(sub_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2sm_kpm_action_definition_format4_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("matchingUeCondList");
  for (const auto& e1 : matching_ue_cond_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_fieldname("subscriptionInfo");
  sub_info.to_json(j);
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format5 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_action_definition_format5_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_ue_id_list, 2, 65535, true));
  HANDLE_CODE(sub_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_action_definition_format5_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(matching_ue_id_list, bref, 2, 65535, true));
  HANDLE_CODE(sub_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2sm_kpm_action_definition_format5_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("matchingUEidList");
  for (const auto& e1 : matching_ue_id_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_fieldname("subscriptionInfo");
  sub_info.to_json(j);
  j.end_obj();
}

// E2SM-KPM-EventTriggerDefinition-Format1 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_event_trigger_definition_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, report_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_event_trigger_definition_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(report_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
void e2sm_kpm_event_trigger_definition_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reportingPeriod", report_period);
  j.end_obj();
}

// E2SM-KPM-IndicationHeader-Format1 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_ind_hdr_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(file_formatversion_present, 1));
  HANDLE_CODE(bref.pack(sender_name_present, 1));
  HANDLE_CODE(bref.pack(sender_type_present, 1));
  HANDLE_CODE(bref.pack(vendor_name_present, 1));

  HANDLE_CODE(collet_start_time.pack(bref));
  if (file_formatversion_present) {
    HANDLE_CODE(file_formatversion.pack(bref));
  }
  if (sender_name_present) {
    HANDLE_CODE(sender_name.pack(bref));
  }
  if (sender_type_present) {
    HANDLE_CODE(sender_type.pack(bref));
  }
  if (vendor_name_present) {
    HANDLE_CODE(vendor_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_hdr_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(file_formatversion_present, 1));
  HANDLE_CODE(bref.unpack(sender_name_present, 1));
  HANDLE_CODE(bref.unpack(sender_type_present, 1));
  HANDLE_CODE(bref.unpack(vendor_name_present, 1));

  HANDLE_CODE(collet_start_time.unpack(bref));
  if (file_formatversion_present) {
    HANDLE_CODE(file_formatversion.unpack(bref));
  }
  if (sender_name_present) {
    HANDLE_CODE(sender_name.unpack(bref));
  }
  if (sender_type_present) {
    HANDLE_CODE(sender_type.unpack(bref));
  }
  if (vendor_name_present) {
    HANDLE_CODE(vendor_name.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2sm_kpm_ind_hdr_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("colletStartTime", collet_start_time.to_string());
  if (file_formatversion_present) {
    j.write_str("fileFormatversion", file_formatversion.to_string());
  }
  if (sender_name_present) {
    j.write_str("senderName", sender_name.to_string());
  }
  if (sender_type_present) {
    j.write_str("senderType", sender_type.to_string());
  }
  if (vendor_name_present) {
    j.write_str("vendorName", vendor_name.to_string());
  }
  j.end_obj();
}

// E2SM-KPM-IndicationMessage-Format2 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_ind_msg_format2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(granul_period_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_data, 1, 65535, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_cond_ue_id_list, 1, 65535, true));
  if (granul_period_present) {
    HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_msg_format2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(granul_period_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_data, bref, 1, 65535, true));
  HANDLE_CODE(unpack_dyn_seq_of(meas_cond_ue_id_list, bref, 1, 65535, true));
  if (granul_period_present) {
    HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2sm_kpm_ind_msg_format2_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage-Format2");
  j.start_array("measData");
  for (const auto& e1 : meas_data) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("measCondUEidList");
  for (const auto& e1 : meas_cond_ue_id_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (granul_period_present) {
    j.write_int("granulPeriod", granul_period);
  }
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// E2SM-KPM-IndicationMessage-Format3 ::= SEQUENCE
SRSASN_CODE e2sm_kpm_ind_msg_format3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, ue_meas_report_list, 1, 65535, true));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_msg_format3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(ue_meas_report_list, bref, 1, 65535, true));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2sm_kpm_ind_msg_format3_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage-Format3");
  j.start_array("ueMeasReportList");
  for (const auto& e1 : ue_meas_report_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// RIC-EventTriggerStyle-Item ::= SEQUENCE
SRSASN_CODE ric_event_trigger_style_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_event_trigger_style_type, false, true));
  HANDLE_CODE(ric_event_trigger_style_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_event_trigger_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_event_trigger_style_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_unconstrained_integer(ric_event_trigger_style_type, bref, false, true));
  HANDLE_CODE(ric_event_trigger_style_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(ric_event_trigger_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void ric_event_trigger_style_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ric-EventTriggerStyle-Type", ric_event_trigger_style_type);
  j.write_str("ric-EventTriggerStyle-Name", ric_event_trigger_style_name.to_string());
  j.write_int("ric-EventTriggerFormat-Type", ric_event_trigger_format_type);
  j.end_obj();
}

// RIC-ReportStyle-Item ::= SEQUENCE
SRSASN_CODE ric_report_style_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_report_style_type, false, true));
  HANDLE_CODE(ric_report_style_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_action_format_type, false, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_info_action_list, 1, 65535, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_ind_hdr_format_type, false, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_ind_msg_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_report_style_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_unconstrained_integer(ric_report_style_type, bref, false, true));
  HANDLE_CODE(ric_report_style_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(ric_action_format_type, bref, false, true));
  HANDLE_CODE(unpack_dyn_seq_of(meas_info_action_list, bref, 1, 65535, true));
  HANDLE_CODE(unpack_unconstrained_integer(ric_ind_hdr_format_type, bref, false, true));
  HANDLE_CODE(unpack_unconstrained_integer(ric_ind_msg_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void ric_report_style_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ric-ReportStyle-Type", ric_report_style_type);
  j.write_str("ric-ReportStyle-Name", ric_report_style_name.to_string());
  j.write_int("ric-ActionFormat-Type", ric_action_format_type);
  j.start_array("measInfo-Action-List");
  for (const auto& e1 : meas_info_action_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("ric-IndicationHeaderFormat-Type", ric_ind_hdr_format_type);
  j.write_int("ric-IndicationMessageFormat-Type", ric_ind_msg_format_type);
  j.end_obj();
}

// E2SM-KPM-ActionDefinition ::= SEQUENCE
SRSASN_CODE e2sm_kpm_action_definition_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_style_type, false, true));
  HANDLE_CODE(action_definition_formats.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_action_definition_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_unconstrained_integer(ric_style_type, bref, false, true));
  HANDLE_CODE(action_definition_formats.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2sm_kpm_action_definition_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ric-Style-Type", ric_style_type);
  j.write_fieldname("actionDefinition-formats");
  action_definition_formats.to_json(j);
  j.end_obj();
}

void e2sm_kpm_action_definition_s::action_definition_formats_c_::destroy_()
{
  switch (type_) {
    case types::action_definition_format1:
      c.destroy<e2sm_kpm_action_definition_format1_s>();
      break;
    case types::action_definition_format2:
      c.destroy<e2sm_kpm_action_definition_format2_s>();
      break;
    case types::action_definition_format3:
      c.destroy<e2sm_kpm_action_definition_format3_s>();
      break;
    case types::action_definition_format4:
      c.destroy<e2sm_kpm_action_definition_format4_s>();
      break;
    case types::action_definition_format5:
      c.destroy<e2sm_kpm_action_definition_format5_s>();
      break;
    default:
      break;
  }
}
void e2sm_kpm_action_definition_s::action_definition_formats_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::action_definition_format1:
      c.init<e2sm_kpm_action_definition_format1_s>();
      break;
    case types::action_definition_format2:
      c.init<e2sm_kpm_action_definition_format2_s>();
      break;
    case types::action_definition_format3:
      c.init<e2sm_kpm_action_definition_format3_s>();
      break;
    case types::action_definition_format4:
      c.init<e2sm_kpm_action_definition_format4_s>();
      break;
    case types::action_definition_format5:
      c.init<e2sm_kpm_action_definition_format5_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_action_definition_s::action_definition_formats_c_");
  }
}
e2sm_kpm_action_definition_s::action_definition_formats_c_::action_definition_formats_c_(
    const e2sm_kpm_action_definition_s::action_definition_formats_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::action_definition_format1:
      c.init(other.c.get<e2sm_kpm_action_definition_format1_s>());
      break;
    case types::action_definition_format2:
      c.init(other.c.get<e2sm_kpm_action_definition_format2_s>());
      break;
    case types::action_definition_format3:
      c.init(other.c.get<e2sm_kpm_action_definition_format3_s>());
      break;
    case types::action_definition_format4:
      c.init(other.c.get<e2sm_kpm_action_definition_format4_s>());
      break;
    case types::action_definition_format5:
      c.init(other.c.get<e2sm_kpm_action_definition_format5_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_action_definition_s::action_definition_formats_c_");
  }
}
e2sm_kpm_action_definition_s::action_definition_formats_c_&
e2sm_kpm_action_definition_s::action_definition_formats_c_::operator=(
    const e2sm_kpm_action_definition_s::action_definition_formats_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::action_definition_format1:
      c.set(other.c.get<e2sm_kpm_action_definition_format1_s>());
      break;
    case types::action_definition_format2:
      c.set(other.c.get<e2sm_kpm_action_definition_format2_s>());
      break;
    case types::action_definition_format3:
      c.set(other.c.get<e2sm_kpm_action_definition_format3_s>());
      break;
    case types::action_definition_format4:
      c.set(other.c.get<e2sm_kpm_action_definition_format4_s>());
      break;
    case types::action_definition_format5:
      c.set(other.c.get<e2sm_kpm_action_definition_format5_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_action_definition_s::action_definition_formats_c_");
  }

  return *this;
}
e2sm_kpm_action_definition_format1_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format1()
{
  set(types::action_definition_format1);
  return c.get<e2sm_kpm_action_definition_format1_s>();
}
e2sm_kpm_action_definition_format2_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format2()
{
  set(types::action_definition_format2);
  return c.get<e2sm_kpm_action_definition_format2_s>();
}
e2sm_kpm_action_definition_format3_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format3()
{
  set(types::action_definition_format3);
  return c.get<e2sm_kpm_action_definition_format3_s>();
}
e2sm_kpm_action_definition_format4_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format4()
{
  set(types::action_definition_format4);
  return c.get<e2sm_kpm_action_definition_format4_s>();
}
e2sm_kpm_action_definition_format5_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format5()
{
  set(types::action_definition_format5);
  return c.get<e2sm_kpm_action_definition_format5_s>();
}
void e2sm_kpm_action_definition_s::action_definition_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::action_definition_format1:
      j.write_fieldname("actionDefinition-Format1");
      c.get<e2sm_kpm_action_definition_format1_s>().to_json(j);
      break;
    case types::action_definition_format2:
      j.write_fieldname("actionDefinition-Format2");
      c.get<e2sm_kpm_action_definition_format2_s>().to_json(j);
      break;
    case types::action_definition_format3:
      j.write_fieldname("actionDefinition-Format3");
      c.get<e2sm_kpm_action_definition_format3_s>().to_json(j);
      break;
    case types::action_definition_format4:
      j.write_fieldname("actionDefinition-Format4");
      c.get<e2sm_kpm_action_definition_format4_s>().to_json(j);
      break;
    case types::action_definition_format5:
      j.write_fieldname("actionDefinition-Format5");
      c.get<e2sm_kpm_action_definition_format5_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_action_definition_s::action_definition_formats_c_");
  }
  j.end_obj();
}
SRSASN_CODE e2sm_kpm_action_definition_s::action_definition_formats_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::action_definition_format1:
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format1_s>().pack(bref));
      break;
    case types::action_definition_format2:
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format2_s>().pack(bref));
      break;
    case types::action_definition_format3:
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format3_s>().pack(bref));
      break;
    case types::action_definition_format4: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format4_s>().pack(bref));
    } break;
    case types::action_definition_format5: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format5_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_action_definition_s::action_definition_formats_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_action_definition_s::action_definition_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::action_definition_format1:
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format1_s>().unpack(bref));
      break;
    case types::action_definition_format2:
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format2_s>().unpack(bref));
      break;
    case types::action_definition_format3:
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format3_s>().unpack(bref));
      break;
    case types::action_definition_format4: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format4_s>().unpack(bref));
    } break;
    case types::action_definition_format5: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2sm_kpm_action_definition_format5_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_action_definition_s::action_definition_formats_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::to_string() const
{
  static const char* names[] = {"actionDefinition-Format1",
                                "actionDefinition-Format2",
                                "actionDefinition-Format3",
                                "actionDefinition-Format4",
                                "actionDefinition-Format5"};
  return convert_enum_idx(names, 5, value, "e2sm_kpm_action_definition_s::action_definition_formats_c_::types");
}
uint8_t e2sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5};
  return map_enum_number(numbers, 5, value, "e2sm_kpm_action_definition_s::action_definition_formats_c_::types");
}

// E2SM-KPM-EventTriggerDefinition ::= SEQUENCE
SRSASN_CODE e2sm_kpm_event_trigger_definition_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(event_definition_formats.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_event_trigger_definition_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(event_definition_formats.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2sm_kpm_event_trigger_definition_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventDefinition-formats");
  event_definition_formats.to_json(j);
  j.end_obj();
}

void e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventDefinition-Format1");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types_opts::to_string() const
{
  static const char* names[] = {"eventDefinition-Format1"};
  return convert_enum_idx(names, 1, value, "e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types");
}
uint8_t e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types");
}

// E2SM-KPM-IndicationHeader ::= SEQUENCE
SRSASN_CODE e2sm_kpm_ind_hdr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ind_hdr_formats.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_hdr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ind_hdr_formats.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2sm_kpm_ind_hdr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("indicationHeader-formats");
  ind_hdr_formats.to_json(j);
  j.end_obj();
}

void e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("indicationHeader-Format1");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types_opts::to_string() const
{
  static const char* names[] = {"indicationHeader-Format1"};
  return convert_enum_idx(names, 1, value, "e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types");
}
uint8_t e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types");
}

// E2SM-KPM-IndicationMessage ::= SEQUENCE
SRSASN_CODE e2sm_kpm_ind_msg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ind_msg_formats.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_msg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ind_msg_formats.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2sm_kpm_ind_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage");
  j.write_fieldname("indicationMessage-formats");
  ind_msg_formats.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

void e2sm_kpm_ind_msg_s::ind_msg_formats_c_::destroy_()
{
  switch (type_) {
    case types::ind_msg_format1:
      c.destroy<e2sm_kpm_ind_msg_format1_s>();
      break;
    case types::ind_msg_format2:
      c.destroy<e2sm_kpm_ind_msg_format2_s>();
      break;
    case types::ind_msg_format3:
      c.destroy<e2sm_kpm_ind_msg_format3_s>();
      break;
    default:
      break;
  }
}
void e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ind_msg_format1:
      c.init<e2sm_kpm_ind_msg_format1_s>();
      break;
    case types::ind_msg_format2:
      c.init<e2sm_kpm_ind_msg_format2_s>();
      break;
    case types::ind_msg_format3:
      c.init<e2sm_kpm_ind_msg_format3_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }
}
e2sm_kpm_ind_msg_s::ind_msg_formats_c_::ind_msg_formats_c_(const e2sm_kpm_ind_msg_s::ind_msg_formats_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ind_msg_format1:
      c.init(other.c.get<e2sm_kpm_ind_msg_format1_s>());
      break;
    case types::ind_msg_format2:
      c.init(other.c.get<e2sm_kpm_ind_msg_format2_s>());
      break;
    case types::ind_msg_format3:
      c.init(other.c.get<e2sm_kpm_ind_msg_format3_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }
}
e2sm_kpm_ind_msg_s::ind_msg_formats_c_&
e2sm_kpm_ind_msg_s::ind_msg_formats_c_::operator=(const e2sm_kpm_ind_msg_s::ind_msg_formats_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ind_msg_format1:
      c.set(other.c.get<e2sm_kpm_ind_msg_format1_s>());
      break;
    case types::ind_msg_format2:
      c.set(other.c.get<e2sm_kpm_ind_msg_format2_s>());
      break;
    case types::ind_msg_format3:
      c.set(other.c.get<e2sm_kpm_ind_msg_format3_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }

  return *this;
}
e2sm_kpm_ind_msg_format1_s& e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format1()
{
  set(types::ind_msg_format1);
  return c.get<e2sm_kpm_ind_msg_format1_s>();
}
e2sm_kpm_ind_msg_format2_s& e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format2()
{
  set(types::ind_msg_format2);
  return c.get<e2sm_kpm_ind_msg_format2_s>();
}
e2sm_kpm_ind_msg_format3_s& e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format3()
{
  set(types::ind_msg_format3);
  return c.get<e2sm_kpm_ind_msg_format3_s>();
}
void e2sm_kpm_ind_msg_s::ind_msg_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ind_msg_format1:
      j.write_fieldname("indicationMessage-Format1");
      c.get<e2sm_kpm_ind_msg_format1_s>().to_json(j);
      break;
    case types::ind_msg_format2:
      j.write_fieldname("indicationMessage-Format2");
      c.get<e2sm_kpm_ind_msg_format2_s>().to_json(j);
      break;
    case types::ind_msg_format3:
      j.write_fieldname("indicationMessage-Format3");
      c.get<e2sm_kpm_ind_msg_format3_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }
  j.end_obj();
}
SRSASN_CODE e2sm_kpm_ind_msg_s::ind_msg_formats_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ind_msg_format1:
      HANDLE_CODE(c.get<e2sm_kpm_ind_msg_format1_s>().pack(bref));
      break;
    case types::ind_msg_format2:
      HANDLE_CODE(c.get<e2sm_kpm_ind_msg_format2_s>().pack(bref));
      break;
    case types::ind_msg_format3: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2sm_kpm_ind_msg_format3_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ind_msg_s::ind_msg_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ind_msg_format1:
      HANDLE_CODE(c.get<e2sm_kpm_ind_msg_format1_s>().unpack(bref));
      break;
    case types::ind_msg_format2:
      HANDLE_CODE(c.get<e2sm_kpm_ind_msg_format2_s>().unpack(bref));
      break;
    case types::ind_msg_format3: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2sm_kpm_ind_msg_format3_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::to_string() const
{
  static const char* names[] = {"indicationMessage-Format1", "indicationMessage-Format2", "indicationMessage-Format3"};
  return convert_enum_idx(names, 3, value, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types");
}
uint8_t e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types");
}

// E2SM-KPM-RANfunction-Description ::= SEQUENCE
SRSASN_CODE e2sm_kpm_ran_function_description_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ric_event_trigger_style_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ric_report_style_list.size() > 0, 1));

  HANDLE_CODE(ran_function_name.pack(bref));
  if (ric_event_trigger_style_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_event_trigger_style_list, 1, 63, true));
  }
  if (ric_report_style_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_report_style_list, 1, 63, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2sm_kpm_ran_function_description_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ric_event_trigger_style_list_present;
  HANDLE_CODE(bref.unpack(ric_event_trigger_style_list_present, 1));
  bool ric_report_style_list_present;
  HANDLE_CODE(bref.unpack(ric_report_style_list_present, 1));

  HANDLE_CODE(ran_function_name.unpack(bref));
  if (ric_event_trigger_style_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ric_event_trigger_style_list, bref, 1, 63, true));
  }
  if (ric_report_style_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ric_report_style_list, bref, 1, 63, true));
  }

  return SRSASN_SUCCESS;
}
void e2sm_kpm_ran_function_description_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ranFunction-Name");
  ran_function_name.to_json(j);
  if (ric_event_trigger_style_list.size() > 0) {
    j.start_array("ric-EventTriggerStyle-List");
    for (const auto& e1 : ric_event_trigger_style_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_report_style_list.size() > 0) {
    j.start_array("ric-ReportStyle-List");
    for (const auto& e1 : ric_report_style_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

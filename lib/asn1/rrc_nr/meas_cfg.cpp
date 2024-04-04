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

#include "srsran/asn1/rrc_nr/meas_cfg.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

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

const char* csi_rs_res_mob_s::slot_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms4", "ms5", "ms10", "ms20", "ms40"};
  return convert_enum_idx(names, 5, value, "csi_rs_res_mob_s::slot_cfg_c_::types");
}
uint8_t csi_rs_res_mob_s::slot_cfg_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 5, 10, 20, 40};
  return map_enum_number(numbers, 5, value, "csi_rs_res_mob_s::slot_cfg_c_::types");
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

const char* csi_rs_res_mob_s::freq_domain_alloc_c_::types_opts::to_string() const
{
  static const char* names[] = {"row1", "row2"};
  return convert_enum_idx(names, 2, value, "csi_rs_res_mob_s::freq_domain_alloc_c_::types");
}
uint8_t csi_rs_res_mob_s::freq_domain_alloc_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_rs_res_mob_s::freq_domain_alloc_c_::types");
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

const char* csi_rs_res_mob_s::slot_cfg_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms4", "ms5", "ms10", "ms20", "ms40"};
  return convert_enum_idx(names, 5, value, "csi_rs_res_mob_s::slot_cfg_r17_c_::types");
}
uint8_t csi_rs_res_mob_s::slot_cfg_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 5, 10, 20, 40};
  return map_enum_number(numbers, 5, value, "csi_rs_res_mob_s::slot_cfg_r17_c_::types");
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

const char* meas_trigger_quant_c::types_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(names, 3, value, "meas_trigger_quant_c::types");
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

const char* meas_trigger_quant_offset_c::types_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(names, 3, value, "meas_trigger_quant_offset_c::types");
}

// NR-RS-Type ::= ENUMERATED
const char* nr_rs_type_opts::to_string() const
{
  static const char* names[] = {"ssb", "csi-rs"};
  return convert_enum_idx(names, 2, value, "nr_rs_type_e");
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

const char* rssi_periodicity_and_offset_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl10", "sl20", "sl40", "sl80", "sl160", "sl320", "s1640"};
  return convert_enum_idx(names, 7, value, "rssi_periodicity_and_offset_r16_c::types");
}
uint16_t rssi_periodicity_and_offset_r16_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 40, 80, 160, 320, 1640};
  return map_enum_number(numbers, 7, value, "rssi_periodicity_and_offset_r16_c::types");
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

const char* cond_trigger_cfg_r16_s::cond_event_id_c_::types_opts::to_string() const
{
  static const char* names[] = {"condEventA3", "condEventA5", "condEventA4-r17", "condEventD1-r17", "condEventT1-r17"};
  return convert_enum_idx(names, 5, value, "cond_trigger_cfg_r16_s::cond_event_id_c_::types");
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

// MeasReportQuantityCLI-r16 ::= ENUMERATED
const char* meas_report_quant_cli_r16_opts::to_string() const
{
  static const char* names[] = {"srs-rsrp", "cli-rssi"};
  return convert_enum_idx(names, 2, value, "meas_report_quant_cli_r16_e");
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

const char* meas_trigger_quant_cli_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"srs-RSRP-r16", "cli-RSSI-r16"};
  return convert_enum_idx(names, 2, value, "meas_trigger_quant_cli_r16_c::types");
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

const char* meas_trigger_quant_eutra_c::types_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(names, 3, value, "meas_trigger_quant_eutra_c::types");
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

const char* meas_trigger_quant_utra_fdd_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"utra-FDD-RSCP-r16", "utra-FDD-EcN0-r16"};
  return convert_enum_idx(names, 2, value, "meas_trigger_quant_utra_fdd_r16_c::types");
}
uint8_t meas_trigger_quant_utra_fdd_r16_c::types_opts::to_number() const
{
  if (value == utra_fdd_ec_n0_r16) {
    return 0;
  }
  invalid_enum_number(value, "meas_trigger_quant_utra_fdd_r16_c::types");
  return 0;
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

const char* ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "sf5-r16", "sf10-r16", "sf20-r16", "sf40-r16", "sf80-r16", "sf160-r16", "sf320-r16", "sf640-r16", "sf1280-r16"};
  return convert_enum_idx(names, 9, value, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types");
}
uint16_t ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 40, 80, 160, 320, 640, 1280};
  return map_enum_number(numbers, 9, value, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types");
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

const char* cli_event_trigger_cfg_r16_s::event_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"eventI1-r16"};
  return convert_enum_idx(names, 1, value, "cli_event_trigger_cfg_r16_s::event_id_r16_c_::types");
}
uint8_t cli_event_trigger_cfg_r16_s::event_id_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cli_event_trigger_cfg_r16_s::event_id_r16_c_::types");
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

const char* event_trigger_cfg_inter_rat_s::event_id_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "eventB1", "eventB2", "eventB1-UTRA-FDD-r16", "eventB2-UTRA-FDD-r16", "eventY1-Relay-r17", "eventY2-Relay-r17"};
  return convert_enum_idx(names, 6, value, "event_trigger_cfg_inter_rat_s::event_id_c_::types");
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

const char* event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"eventC1", "eventC2-r16"};
  return convert_enum_idx(names, 2, value, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types");
}
uint8_t event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types");
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

const char* event_trigger_cfg_s::event_id_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "eventA1", "eventA2", "eventA3", "eventA4", "eventA5", "eventA6", "eventX1-r17", "eventX2-r17", "eventD1-r17"};
  return convert_enum_idx(names, 9, value, "event_trigger_cfg_s::event_id_c_::types");
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

const char* meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"prs-Ref-r17", "csi-RS-Ref-r17"};
  return convert_enum_idx(names, 2, value, "meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::types");
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

const char* report_cfg_inter_rat_s::report_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical", "eventTriggered", "reportCGI", "reportSFTD"};
  return convert_enum_idx(names, 4, value, "report_cfg_inter_rat_s::report_type_c_::types");
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

const char* report_cfg_nr_s::report_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical",
                                "eventTriggered",
                                "reportCGI",
                                "reportSFTD",
                                "condTriggerConfig-r16",
                                "cli-Periodical-r16",
                                "cli-EventTriggered-r16",
                                "rxTxPeriodical-r17"};
  return convert_enum_idx(names, 8, value, "report_cfg_nr_s::report_type_c_::types");
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

const char* report_cfg_nr_sl_r16_s::report_type_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical-r16", "eventTriggered-r16"};
  return convert_enum_idx(names, 2, value, "report_cfg_nr_sl_r16_s::report_type_r16_c_::types");
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

const char* meas_obj_to_add_mod_s::meas_obj_c_::types_opts::to_string() const
{
  static const char* names[] = {"measObjectNR",
                                "measObjectEUTRA",
                                "measObjectUTRA-FDD-r16",
                                "measObjectNR-SL-r16",
                                "measObjectCLI-r16",
                                "measObjectRxTxDiff-r17",
                                "measObjectRelay-r17"};
  return convert_enum_idx(names, 7, value, "meas_obj_to_add_mod_s::meas_obj_c_::types");
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

const char* report_cfg_to_add_mod_s::report_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"reportConfigNR", "reportConfigInterRAT", "reportConfigNR-SL-r16"};
  return convert_enum_idx(names, 3, value, "report_cfg_to_add_mod_s::report_cfg_c_::types");
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

const char* meas_cfg_s::s_measure_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-RSRP", "csi-RSRP"};
  return convert_enum_idx(names, 2, value, "meas_cfg_s::s_measure_cfg_c_::types");
}

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

#include "srsran/asn1/rrc_nr/rrc_nr.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MRB-PDCP-ConfigBroadcast-r17 ::= SEQUENCE
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_sn_size_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(t_reordering_r17_present, 1));

  HANDLE_CODE(hdr_compress_r17.pack(bref));
  if (t_reordering_r17_present) {
    HANDLE_CODE(t_reordering_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_sn_size_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(t_reordering_r17_present, 1));

  HANDLE_CODE(hdr_compress_r17.unpack(bref));
  if (t_reordering_r17_present) {
    HANDLE_CODE(t_reordering_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrb_pdcp_cfg_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_sn_size_dl_r17_present) {
    j.write_str("pdcp-SN-SizeDL-r17", "len12bits");
  }
  j.write_fieldname("headerCompression-r17");
  hdr_compress_r17.to_json(j);
  if (t_reordering_r17_present) {
    j.write_str("t-Reordering-r17", t_reordering_r17.to_string());
  }
  j.end_obj();
}

void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set(types::options e)
{
  type_ = e;
}
void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set_not_used()
{
  set(types::not_used);
}
mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_&
mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set_rohc()
{
  set(types::rohc);
  return c;
}
void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      j.write_fieldname("rohc");
      j.start_obj();
      if (c.max_c_id_r17_present) {
        j.write_int("maxCID-r17", c.max_c_id_r17);
      }
      j.write_fieldname("profiles-r17");
      j.start_obj();
      j.write_bool("profile0x0000-r17", c.profiles_r17.profile0x0000_r17);
      j.write_bool("profile0x0001-r17", c.profiles_r17.profile0x0001_r17);
      j.write_bool("profile0x0002-r17", c.profiles_r17.profile0x0002_r17);
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.pack(c.max_c_id_r17_present, 1));
      if (c.max_c_id_r17_present) {
        HANDLE_CODE(pack_integer(bref, c.max_c_id_r17, (uint8_t)1u, (uint8_t)16u));
      }
      HANDLE_CODE(bref.pack(c.profiles_r17.profile0x0000_r17, 1));
      HANDLE_CODE(bref.pack(c.profiles_r17.profile0x0001_r17, 1));
      HANDLE_CODE(bref.pack(c.profiles_r17.profile0x0002_r17, 1));
      break;
    default:
      log_invalid_choice_id(type_, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.unpack(c.max_c_id_r17_present, 1));
      if (c.max_c_id_r17_present) {
        HANDLE_CODE(unpack_integer(c.max_c_id_r17, bref, (uint8_t)1u, (uint8_t)16u));
      }
      HANDLE_CODE(bref.unpack(c.profiles_r17.profile0x0000_r17, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r17.profile0x0001_r17, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r17.profile0x0002_r17, 1));
      break;
    default:
      log_invalid_choice_id(type_, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"notUsed", "rohc"};
  return convert_enum_idx(names, 2, value, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types");
}

const char* mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_opts::to_string() const
{
  static const char* names[] = {"ms1", "ms10", "ms40", "ms160", "ms500", "ms1000", "ms1250", "ms2750"};
  return convert_enum_idx(names, 8, value, "mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_e_");
}
uint16_t mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 10, 40, 160, 500, 1000, 1250, 2750};
  return map_enum_number(numbers, 8, value, "mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_e_");
}

// MRB-RLC-ConfigBroadcast-r17 ::= SEQUENCE
SRSASN_CODE mrb_rlc_cfg_broadcast_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sn_field_len_r17_present, 1));
  HANDLE_CODE(bref.pack(t_reassembly_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, lc_ch_id_r17, (uint8_t)1u, (uint8_t)32u));
  if (t_reassembly_r17_present) {
    HANDLE_CODE(t_reassembly_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_rlc_cfg_broadcast_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sn_field_len_r17_present, 1));
  HANDLE_CODE(bref.unpack(t_reassembly_r17_present, 1));

  HANDLE_CODE(unpack_integer(lc_ch_id_r17, bref, (uint8_t)1u, (uint8_t)32u));
  if (t_reassembly_r17_present) {
    HANDLE_CODE(t_reassembly_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrb_rlc_cfg_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("logicalChannelIdentity-r17", lc_ch_id_r17);
  if (sn_field_len_r17_present) {
    j.write_str("sn-FieldLength-r17", "size6");
  }
  if (t_reassembly_r17_present) {
    j.write_str("t-Reassembly-r17", t_reassembly_r17.to_string());
  }
  j.end_obj();
}

// MRB-InfoBroadcast-r17 ::= SEQUENCE
SRSASN_CODE mrb_info_broadcast_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pdcp_cfg_r17.pack(bref));
  HANDLE_CODE(rlc_cfg_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_info_broadcast_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(pdcp_cfg_r17.unpack(bref));
  HANDLE_CODE(rlc_cfg_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mrb_info_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pdcp-Config-r17");
  pdcp_cfg_r17.to_json(j);
  j.write_fieldname("rlc-Config-r17");
  rlc_cfg_r17.to_json(j);
  j.end_obj();
}

// MBS-NeighbourCell-r17 ::= SEQUENCE
SRSASN_CODE mbs_neighbour_cell_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));
  if (carrier_freq_r17_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_neighbour_cell_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_r17_present, 1));

  HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  if (carrier_freq_r17_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
void mbs_neighbour_cell_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r17", pci_r17);
  if (carrier_freq_r17_present) {
    j.write_int("carrierFreq-r17", carrier_freq_r17);
  }
  j.end_obj();
}

// MBS-SessionInfo-r17 ::= SEQUENCE
SRSASN_CODE mbs_session_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mtch_sched_info_r17_present, 1));
  HANDLE_CODE(bref.pack(mtch_neighbour_cell_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_idx_r17_present, 1));
  HANDLE_CODE(bref.pack(mtch_ssb_map_win_idx_r17_present, 1));

  HANDLE_CODE(mbs_session_id_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, g_rnti_r17, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_dyn_seq_of(bref, mrb_list_broadcast_r17, 1, 4));
  if (mtch_sched_info_r17_present) {
    HANDLE_CODE(pack_integer(bref, mtch_sched_info_r17, (uint8_t)0u, (uint8_t)63u));
  }
  if (mtch_neighbour_cell_r17_present) {
    HANDLE_CODE(mtch_neighbour_cell_r17.pack(bref));
  }
  if (pdsch_cfg_idx_r17_present) {
    HANDLE_CODE(pack_integer(bref, pdsch_cfg_idx_r17, (uint8_t)0u, (uint8_t)15u));
  }
  if (mtch_ssb_map_win_idx_r17_present) {
    HANDLE_CODE(pack_integer(bref, mtch_ssb_map_win_idx_r17, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_session_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mtch_sched_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(mtch_neighbour_cell_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_idx_r17_present, 1));
  HANDLE_CODE(bref.unpack(mtch_ssb_map_win_idx_r17_present, 1));

  HANDLE_CODE(mbs_session_id_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(g_rnti_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_dyn_seq_of(mrb_list_broadcast_r17, bref, 1, 4));
  if (mtch_sched_info_r17_present) {
    HANDLE_CODE(unpack_integer(mtch_sched_info_r17, bref, (uint8_t)0u, (uint8_t)63u));
  }
  if (mtch_neighbour_cell_r17_present) {
    HANDLE_CODE(mtch_neighbour_cell_r17.unpack(bref));
  }
  if (pdsch_cfg_idx_r17_present) {
    HANDLE_CODE(unpack_integer(pdsch_cfg_idx_r17, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (mtch_ssb_map_win_idx_r17_present) {
    HANDLE_CODE(unpack_integer(mtch_ssb_map_win_idx_r17, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void mbs_session_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbs-SessionId-r17");
  mbs_session_id_r17.to_json(j);
  j.write_int("g-RNTI-r17", g_rnti_r17);
  j.start_array("mrb-ListBroadcast-r17");
  for (const auto& e1 : mrb_list_broadcast_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (mtch_sched_info_r17_present) {
    j.write_int("mtch-SchedulingInfo-r17", mtch_sched_info_r17);
  }
  if (mtch_neighbour_cell_r17_present) {
    j.write_str("mtch-NeighbourCell-r17", mtch_neighbour_cell_r17.to_string());
  }
  if (pdsch_cfg_idx_r17_present) {
    j.write_int("pdsch-ConfigIndex-r17", pdsch_cfg_idx_r17);
  }
  if (mtch_ssb_map_win_idx_r17_present) {
    j.write_int("mtch-SSB-MappingWindowIndex-r17", mtch_ssb_map_win_idx_r17);
  }
  j.end_obj();
}

// MTCH-SSB-MappingWindowCycleOffset-r17 ::= CHOICE
void mtch_ssb_map_win_cycle_offset_r17_c::destroy_() {}
void mtch_ssb_map_win_cycle_offset_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
mtch_ssb_map_win_cycle_offset_r17_c::mtch_ssb_map_win_cycle_offset_r17_c(
    const mtch_ssb_map_win_cycle_offset_r17_c& other)
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
    case types::ms64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
  }
}
mtch_ssb_map_win_cycle_offset_r17_c&
mtch_ssb_map_win_cycle_offset_r17_c::operator=(const mtch_ssb_map_win_cycle_offset_r17_c& other)
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
    case types::ms64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
  }

  return *this;
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint16_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
void mtch_ssb_map_win_cycle_offset_r17_c::to_json(json_writer& j) const
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
    case types::ms64:
      j.write_int("ms64", c.get<uint8_t>());
      break;
    case types::ms128:
      j.write_int("ms128", c.get<uint8_t>());
      break;
    case types::ms256:
      j.write_int("ms256", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE mtch_ssb_map_win_cycle_offset_r17_c::pack(bit_ref& bref) const
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
    case types::ms64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mtch_ssb_map_win_cycle_offset_r17_c::unpack(cbit_ref& bref)
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
    case types::ms64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mtch_ssb_map_win_cycle_offset_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"ms10", "ms20", "ms32", "ms64", "ms128", "ms256"};
  return convert_enum_idx(names, 6, value, "mtch_ssb_map_win_cycle_offset_r17_c::types");
}
uint16_t mtch_ssb_map_win_cycle_offset_r17_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 32, 64, 128, 256};
  return map_enum_number(numbers, 6, value, "mtch_ssb_map_win_cycle_offset_r17_c::types");
}

// MBSBroadcastConfiguration-r17-IEs ::= SEQUENCE
SRSASN_CODE mbs_broadcast_cfg_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbs_session_info_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(mbs_neighbour_cell_list_r17_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_ptm_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_mtch_r17_present, 1));
  HANDLE_CODE(bref.pack(mtch_ssb_map_win_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbs_session_info_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_session_info_list_r17, 1, 1024));
  }
  if (mbs_neighbour_cell_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_neighbour_cell_list_r17, 0, 8));
  }
  if (drx_cfg_ptm_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drx_cfg_ptm_list_r17, 1, 64));
  }
  if (pdsch_cfg_mtch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mtch_r17.pack(bref));
  }
  if (mtch_ssb_map_win_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mtch_ssb_map_win_list_r17, 1, 16));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_broadcast_cfg_r17_ies_s::unpack(cbit_ref& bref)
{
  bool mbs_session_info_list_r17_present;
  HANDLE_CODE(bref.unpack(mbs_session_info_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(mbs_neighbour_cell_list_r17_present, 1));
  bool drx_cfg_ptm_list_r17_present;
  HANDLE_CODE(bref.unpack(drx_cfg_ptm_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_mtch_r17_present, 1));
  bool mtch_ssb_map_win_list_r17_present;
  HANDLE_CODE(bref.unpack(mtch_ssb_map_win_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbs_session_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_session_info_list_r17, bref, 1, 1024));
  }
  if (mbs_neighbour_cell_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_neighbour_cell_list_r17, bref, 0, 8));
  }
  if (drx_cfg_ptm_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drx_cfg_ptm_list_r17, bref, 1, 64));
  }
  if (pdsch_cfg_mtch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mtch_r17.unpack(bref));
  }
  if (mtch_ssb_map_win_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mtch_ssb_map_win_list_r17, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbs_broadcast_cfg_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbs_session_info_list_r17.size() > 0) {
    j.start_array("mbs-SessionInfoList-r17");
    for (const auto& e1 : mbs_session_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (mbs_neighbour_cell_list_r17_present) {
    j.start_array("mbs-NeighbourCellList-r17");
    for (const auto& e1 : mbs_neighbour_cell_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drx_cfg_ptm_list_r17.size() > 0) {
    j.start_array("drx-ConfigPTM-List-r17");
    for (const auto& e1 : drx_cfg_ptm_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdsch_cfg_mtch_r17_present) {
    j.write_fieldname("pdsch-ConfigMTCH-r17");
    pdsch_cfg_mtch_r17.to_json(j);
  }
  if (mtch_ssb_map_win_list_r17.size() > 0) {
    j.start_array("mtch-SSB-MappingWindowList-r17");
    for (const auto& e1 : mtch_ssb_map_win_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
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

// MBSBroadcastConfiguration-r17 ::= SEQUENCE
SRSASN_CODE mbs_broadcast_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_broadcast_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbs_broadcast_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mbs_broadcast_cfg_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mbs_broadcast_cfg_r17_ies_s& mbs_broadcast_cfg_r17_s::crit_exts_c_::set_mbs_broadcast_cfg_r17()
{
  set(types::mbs_broadcast_cfg_r17);
  return c;
}
void mbs_broadcast_cfg_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mbs_broadcast_cfg_r17_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      j.write_fieldname("mbsBroadcastConfiguration-r17");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_broadcast_cfg_r17_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbs_broadcast_cfg_r17_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_broadcast_cfg_r17_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_broadcast_cfg_r17_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_broadcast_cfg_r17_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mbs_broadcast_cfg_r17_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mbsBroadcastConfiguration-r17", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mbs_broadcast_cfg_r17_s::crit_exts_c_::types");
}

// MCCH-MessageType-r17 ::= CHOICE
void mcch_msg_type_r17_c::set(types::options e)
{
  type_ = e;
}
mcch_msg_type_r17_c::c1_c_& mcch_msg_type_r17_c::set_c1()
{
  set(types::c1);
  return c;
}
void mcch_msg_type_r17_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void mcch_msg_type_r17_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_r17_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mcch_msg_type_r17_c::c1_c_::set(types::options e)
{
  type_ = e;
}
mbs_broadcast_cfg_r17_s& mcch_msg_type_r17_c::c1_c_::set_mbs_broadcast_cfg_r17()
{
  set(types::mbs_broadcast_cfg_r17);
  return c;
}
void mcch_msg_type_r17_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void mcch_msg_type_r17_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      j.write_fieldname("mbsBroadcastConfiguration-r17");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_r17_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_r17_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mcch_msg_type_r17_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"mbsBroadcastConfiguration-r17", "spare1"};
  return convert_enum_idx(names, 2, value, "mcch_msg_type_r17_c::c1_c_::types");
}

const char* mcch_msg_type_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "mcch_msg_type_r17_c::types");
}
uint8_t mcch_msg_type_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "mcch_msg_type_r17_c::types");
}

// MCCH-Message-r17 ::= SEQUENCE
SRSASN_CODE mcch_msg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void mcch_msg_r17_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("MCCH-Message-r17");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// PDCCH-BlindDetectionCA-MixedExt-r16 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_ca_mixed_ext_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca1_r16, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca2_r16, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_ca_mixed_ext_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca1_r16, bref, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca2_r16, bref, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_ca_mixed_ext_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdcch-BlindDetectionCA1-r16", pdcch_blind_detection_ca1_r16);
  j.write_int("pdcch-BlindDetectionCA2-r16", pdcch_blind_detection_ca2_r16);
  j.end_obj();
}

// PDCCH-BlindDetectionCG-UE-MixedExt-r16 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_cg_ue_mixed_ext_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_cg_ue1_r16, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_cg_ue2_r16, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_cg_ue_mixed_ext_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_cg_ue1_r16, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_cg_ue2_r16, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_cg_ue_mixed_ext_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdcch-BlindDetectionCG-UE1-r16", pdcch_blind_detection_cg_ue1_r16);
  j.write_int("pdcch-BlindDetectionCG-UE2-r16", pdcch_blind_detection_cg_ue2_r16);
  j.end_obj();
}

// PDCCH-BlindDetectionMixedList-r16 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcch_blind_detection_ca_mixed_ext_r16_present, 1));
  HANDLE_CODE(bref.pack(pdcch_blind_detection_cg_ue_mixed_ext_r16_present, 1));

  if (pdcch_blind_detection_ca_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_ca_mixed_ext_r16.pack(bref));
  }
  if (pdcch_blind_detection_cg_ue_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_mcg_ue_mixed_v16a0.pack(bref));
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_scg_ue_mixed_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcch_blind_detection_ca_mixed_ext_r16_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_blind_detection_cg_ue_mixed_ext_r16_present, 1));

  if (pdcch_blind_detection_ca_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_ca_mixed_ext_r16.unpack(bref));
  }
  if (pdcch_blind_detection_cg_ue_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_mcg_ue_mixed_v16a0.unpack(bref));
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_scg_ue_mixed_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_mixed_list_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcch_blind_detection_ca_mixed_ext_r16_present) {
    j.write_fieldname("pdcch-BlindDetectionCA-MixedExt-r16");
    pdcch_blind_detection_ca_mixed_ext_r16.to_json(j);
  }
  if (pdcch_blind_detection_cg_ue_mixed_ext_r16_present) {
    j.write_fieldname("pdcch-BlindDetectionCG-UE-MixedExt-r16");
    j.start_obj();
    j.write_fieldname("pdcch-BlindDetectionMCG-UE-Mixed-v16a0");
    pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_mcg_ue_mixed_v16a0.to_json(j);
    j.write_fieldname("pdcch-BlindDetectionSCG-UE-Mixed-v16a0");
    pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_scg_ue_mixed_v16a0.to_json(j);
    j.end_obj();
  }
  j.end_obj();
}

void pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::destroy_()
{
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.destroy<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.destroy<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    default:
      break;
  }
}
void pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.init<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.init<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }
}
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::
    pdcch_blind_detection_ca_mixed_ext_r16_c_(
        const pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.init(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.init(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }
}
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_&
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::operator=(
    const pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.set(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.set(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }

  return *this;
}
pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_mixed_list_r16_s::
    pdcch_blind_detection_ca_mixed_ext_r16_c_::set_pdcch_blind_detection_ca_mixed_v16a0()
{
  set(types::pdcch_blind_detection_ca_mixed_v16a0);
  return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
}
pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_mixed_list_r16_s::
    pdcch_blind_detection_ca_mixed_ext_r16_c_::set_pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0()
{
  set(types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0);
  return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
}
void pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      j.write_fieldname("pdcch-BlindDetectionCA-Mixed-v16a0");
      c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().to_json(j);
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      j.write_fieldname("pdcch-BlindDetectionCA-Mixed-NonAlignedSpan-v16a0");
      c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().pack(bref));
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().unpack(bref));
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"pdcch-BlindDetectionCA-Mixed-v16a0",
                                "pdcch-BlindDetectionCA-Mixed-NonAlignedSpan-v16a0"};
  return convert_enum_idx(
      names, 2, value, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types");
}

// CA-ParametersNR-v16a0 ::= SEQUENCE
SRSASN_CODE ca_params_nr_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, pdcch_blind_detection_mixed_list_r16, 1, 7));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nr_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(pdcch_blind_detection_mixed_list_r16, bref, 1, 7));

  return SRSASN_SUCCESS;
}
void ca_params_nr_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("pdcch-BlindDetectionMixedList-r16");
  for (const auto& e1 : pdcch_blind_detection_mixed_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CA-ParametersNRDC-v16a0 ::= SEQUENCE
SRSASN_CODE ca_params_nrdc_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_params_nr_for_dc_v16a0_present, 1));

  if (ca_params_nr_for_dc_v16a0_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nrdc_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_params_nr_for_dc_v16a0_present, 1));

  if (ca_params_nr_for_dc_v16a0_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ca_params_nrdc_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ca_params_nr_for_dc_v16a0_present) {
    j.write_fieldname("ca-ParametersNR-ForDC-v16a0");
    ca_params_nr_for_dc_v16a0.to_json(j);
  }
  j.end_obj();
}

// BandCombination-v16a0 ::= SEQUENCE
SRSASN_CODE band_combination_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_params_nr_v16a0_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nrdc_v16a0_present, 1));

  if (ca_params_nr_v16a0_present) {
    HANDLE_CODE(ca_params_nr_v16a0.pack(bref));
  }
  if (ca_params_nrdc_v16a0_present) {
    HANDLE_CODE(ca_params_nrdc_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_params_nr_v16a0_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nrdc_v16a0_present, 1));

  if (ca_params_nr_v16a0_present) {
    HANDLE_CODE(ca_params_nr_v16a0.unpack(bref));
  }
  if (ca_params_nrdc_v16a0_present) {
    HANDLE_CODE(ca_params_nrdc_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ca_params_nr_v16a0_present) {
    j.write_fieldname("ca-ParametersNR-v16a0");
    ca_params_nr_v16a0.to_json(j);
  }
  if (ca_params_nrdc_v16a0_present) {
    j.write_fieldname("ca-ParametersNRDC-v16a0");
    ca_params_nrdc_v16a0.to_json(j);
  }
  j.end_obj();
}

// BandCombination-UplinkTxSwitch-v16a0 ::= SEQUENCE
SRSASN_CODE band_combination_ul_tx_switch_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_combination_v16a0_present, 1));

  if (band_combination_v16a0_present) {
    HANDLE_CODE(band_combination_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_ul_tx_switch_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_combination_v16a0_present, 1));

  if (band_combination_v16a0_present) {
    HANDLE_CODE(band_combination_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_ul_tx_switch_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_combination_v16a0_present) {
    j.write_fieldname("bandCombination-v16a0");
    band_combination_v16a0.to_json(j);
  }
  j.end_obj();
}

// CG-SDT-ConfigLCH-Restriction-r17 ::= SEQUENCE
SRSASN_CODE cg_sdt_cfg_lch_restrict_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cfg_grant_type1_allowed_r17_present, 1));
  HANDLE_CODE(bref.pack(allowed_cg_list_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, lc_ch_id_r17, (uint8_t)1u, (uint8_t)32u));
  if (allowed_cg_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_cg_list_r17, 0, 31, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_sdt_cfg_lch_restrict_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cfg_grant_type1_allowed_r17_present, 1));
  HANDLE_CODE(bref.unpack(allowed_cg_list_r17_present, 1));

  HANDLE_CODE(unpack_integer(lc_ch_id_r17, bref, (uint8_t)1u, (uint8_t)32u));
  if (allowed_cg_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_cg_list_r17, bref, 0, 31, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
void cg_sdt_cfg_lch_restrict_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("logicalChannelIdentity-r17", lc_ch_id_r17);
  if (cfg_grant_type1_allowed_r17_present) {
    j.write_str("configuredGrantType1Allowed-r17", "true");
  }
  if (allowed_cg_list_r17_present) {
    j.start_array("allowedCG-List-r17");
    for (const auto& e1 : allowed_cg_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// CellGrouping-r16 ::= SEQUENCE
SRSASN_CODE cell_grouping_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, mcg_r16, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(pack_dyn_seq_of(bref, scg_r16, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(mode_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_grouping_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(mcg_r16, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(unpack_dyn_seq_of(scg_r16, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(mode_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_grouping_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("mcg-r16");
  for (const auto& e1 : mcg_r16) {
    j.write_int(e1);
  }
  j.end_array();
  j.start_array("scg-r16");
  for (const auto& e1 : scg_r16) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_str("mode-r16", mode_r16.to_string());
  j.end_obj();
}

const char* cell_grouping_r16_s::mode_r16_opts::to_string() const
{
  static const char* names[] = {"sync", "async"};
  return convert_enum_idx(names, 2, value, "cell_grouping_r16_s::mode_r16_e_");
}

// MeasResultSCG-Failure ::= SEQUENCE
SRSASN_CODE meas_result_scg_fail_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_per_mo_list, 1, 8));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= location_info_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(location_info_r16.is_present(), 1));
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_scg_fail_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_per_mo_list, bref, 1, 8));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(location_info_r16, bref);
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_scg_fail_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measResultPerMOList");
  for (const auto& e1 : meas_result_per_mo_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (location_info_r16.is_present()) {
      j.write_fieldname("locationInfo-r16");
      location_info_r16->to_json(j);
    }
  }
  j.end_obj();
}

// Phy-ParametersCommon-v16a0 ::= SEQUENCE
SRSASN_CODE phy_params_common_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_periodicity_and_offset_ext_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_common_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_periodicity_and_offset_ext_r16_present, 1));

  return SRSASN_SUCCESS;
}
void phy_params_common_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_periodicity_and_offset_ext_r16_present) {
    j.write_str("srs-PeriodicityAndOffsetExt-r16", "supported");
  }
  j.end_obj();
}

// Phy-Parameters-v16a0 ::= SEQUENCE
SRSASN_CODE phy_params_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_params_common_v16a0_present, 1));

  if (phy_params_common_v16a0_present) {
    HANDLE_CODE(phy_params_common_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_params_common_v16a0_present, 1));

  if (phy_params_common_v16a0_present) {
    HANDLE_CODE(phy_params_common_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phy_params_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_params_common_v16a0_present) {
    j.write_fieldname("phy-ParametersCommon-v16a0");
    phy_params_common_v16a0.to_json(j);
  }
  j.end_obj();
}

// RF-Parameters-v15g0 ::= SEQUENCE
SRSASN_CODE rf_params_v15g0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_list_v15g0.size() > 0, 1));

  if (supported_band_combination_list_v15g0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_v15g0, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v15g0_s::unpack(cbit_ref& bref)
{
  bool supported_band_combination_list_v15g0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_v15g0_present, 1));

  if (supported_band_combination_list_v15g0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_v15g0, bref, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v15g0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_list_v15g0.size() > 0) {
    j.start_array("supportedBandCombinationList-v15g0");
    for (const auto& e1 : supported_band_combination_list_v15g0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v16a0 ::= SEQUENCE
SRSASN_CODE rf_params_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_list_v16a0.size() > 0, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v16a0.size() > 0, 1));

  if (supported_band_combination_list_v16a0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_v16a0, 1, 65536));
  }
  if (supported_band_combination_list_ul_tx_switch_v16a0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_ul_tx_switch_v16a0, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v16a0_s::unpack(cbit_ref& bref)
{
  bool supported_band_combination_list_v16a0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_v16a0_present, 1));
  bool supported_band_combination_list_ul_tx_switch_v16a0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_ul_tx_switch_v16a0_present, 1));

  if (supported_band_combination_list_v16a0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_v16a0, bref, 1, 65536));
  }
  if (supported_band_combination_list_ul_tx_switch_v16a0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_ul_tx_switch_v16a0, bref, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_list_v16a0.size() > 0) {
    j.start_array("supportedBandCombinationList-v16a0");
    for (const auto& e1 : supported_band_combination_list_v16a0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_list_ul_tx_switch_v16a0.size() > 0) {
    j.start_array("supportedBandCombinationList-UplinkTxSwitch-v16a0");
    for (const auto& e1 : supported_band_combination_list_ul_tx_switch_v16a0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandNR-v16c0 ::= SEQUENCE
SRSASN_CODE band_nr_v16c0_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pusch_repeat_type_a_v16c0_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_nr_v16c0_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pusch_repeat_type_a_v16c0_present, 1));

  return SRSASN_SUCCESS;
}
void band_nr_v16c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pusch_repeat_type_a_v16c0_present) {
    j.write_str("pusch-RepetitionTypeA-v16c0", "supported");
  }
  j.end_obj();
}

// RF-Parameters-v16c0 ::= SEQUENCE
SRSASN_CODE rf_params_v16c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_nr_v16c0, 1, 1024));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v16c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_nr_v16c0, bref, 1, 1024));

  return SRSASN_SUCCESS;
}
void rf_params_v16c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListNR-v16c0");
  for (const auto& e1 : supported_band_list_nr_v16c0) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CG-SDT-TA-ValidationConfig-r17 ::= SEQUENCE
SRSASN_CODE cg_sdt_ta_validation_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cg_sdt_rsrp_change_thres_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_sdt_ta_validation_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cg_sdt_rsrp_change_thres_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_sdt_ta_validation_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cg-SDT-RSRP-ChangeThreshold-r17", cg_sdt_rsrp_change_thres_r17.to_string());
  j.end_obj();
}

const char* cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_opts::to_string() const
{
  static const char* names[] = {"dB2",
                                "dB4",
                                "dB6",
                                "dB8",
                                "dB10",
                                "dB14",
                                "dB18",
                                "dB22",
                                "dB26",
                                "dB30",
                                "dB34",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_e_");
}
uint8_t cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8, 10, 14, 18, 22, 26, 30, 34};
  return map_enum_number(numbers, 11, value, "cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_e_");
}

// SDT-MAC-PHY-CG-Config-r17 ::= SEQUENCE
SRSASN_CODE sdt_mac_phy_cg_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cg_sdt_cfg_lch_restrict_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_lch_restrict_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_init_bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_init_bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_init_bwp_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_rsrp_thres_ssb_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_ta_validation_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cs_rnti_r17_present, 1));

  if (cg_sdt_cfg_lch_restrict_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cg_sdt_cfg_lch_restrict_to_add_mod_list_r17, 1, 32));
  }
  if (cg_sdt_cfg_lch_restrict_to_release_list_r17.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, cg_sdt_cfg_lch_restrict_to_release_list_r17, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cg_sdt_cfg_init_bwp_n_ul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_n_ul_r17.pack(bref));
  }
  if (cg_sdt_cfg_init_bwp_sul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_sul_r17.pack(bref));
  }
  if (cg_sdt_cfg_init_bwp_dl_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_dl_r17.pack(bref));
  }
  if (cg_sdt_time_align_timer_r17_present) {
    HANDLE_CODE(cg_sdt_time_align_timer_r17.pack(bref));
  }
  if (cg_sdt_rsrp_thres_ssb_r17_present) {
    HANDLE_CODE(pack_integer(bref, cg_sdt_rsrp_thres_ssb_r17, (uint8_t)0u, (uint8_t)127u));
  }
  if (cg_sdt_ta_validation_cfg_r17_present) {
    HANDLE_CODE(cg_sdt_ta_validation_cfg_r17.pack(bref));
  }
  if (cg_sdt_cs_rnti_r17_present) {
    HANDLE_CODE(pack_integer(bref, cg_sdt_cs_rnti_r17, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdt_mac_phy_cg_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_present, 1));
  bool cg_sdt_cfg_lch_restrict_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_lch_restrict_to_release_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_init_bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_init_bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_init_bwp_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_rsrp_thres_ssb_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_ta_validation_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cs_rnti_r17_present, 1));

  if (cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cg_sdt_cfg_lch_restrict_to_add_mod_list_r17, bref, 1, 32));
  }
  if (cg_sdt_cfg_lch_restrict_to_release_list_r17_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(cg_sdt_cfg_lch_restrict_to_release_list_r17, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cg_sdt_cfg_init_bwp_n_ul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_n_ul_r17.unpack(bref));
  }
  if (cg_sdt_cfg_init_bwp_sul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_sul_r17.unpack(bref));
  }
  if (cg_sdt_cfg_init_bwp_dl_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_dl_r17.unpack(bref));
  }
  if (cg_sdt_time_align_timer_r17_present) {
    HANDLE_CODE(cg_sdt_time_align_timer_r17.unpack(bref));
  }
  if (cg_sdt_rsrp_thres_ssb_r17_present) {
    HANDLE_CODE(unpack_integer(cg_sdt_rsrp_thres_ssb_r17, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (cg_sdt_ta_validation_cfg_r17_present) {
    HANDLE_CODE(cg_sdt_ta_validation_cfg_r17.unpack(bref));
  }
  if (cg_sdt_cs_rnti_r17_present) {
    HANDLE_CODE(unpack_integer(cg_sdt_cs_rnti_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
void sdt_mac_phy_cg_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cg_sdt_cfg_lch_restrict_to_add_mod_list_r17.size() > 0) {
    j.start_array("cg-SDT-ConfigLCH-RestrictionToAddModList-r17");
    for (const auto& e1 : cg_sdt_cfg_lch_restrict_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cg_sdt_cfg_lch_restrict_to_release_list_r17.size() > 0) {
    j.start_array("cg-SDT-ConfigLCH-RestrictionToReleaseList-r17");
    for (const auto& e1 : cg_sdt_cfg_lch_restrict_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cg_sdt_cfg_init_bwp_n_ul_r17_present) {
    j.write_fieldname("cg-SDT-ConfigInitialBWP-NUL-r17");
    cg_sdt_cfg_init_bwp_n_ul_r17.to_json(j);
  }
  if (cg_sdt_cfg_init_bwp_sul_r17_present) {
    j.write_fieldname("cg-SDT-ConfigInitialBWP-SUL-r17");
    cg_sdt_cfg_init_bwp_sul_r17.to_json(j);
  }
  if (cg_sdt_cfg_init_bwp_dl_r17_present) {
    j.write_fieldname("cg-SDT-ConfigInitialBWP-DL-r17");
    cg_sdt_cfg_init_bwp_dl_r17.to_json(j);
  }
  if (cg_sdt_time_align_timer_r17_present) {
    j.write_str("cg-SDT-TimeAlignmentTimer-r17", cg_sdt_time_align_timer_r17.to_string());
  }
  if (cg_sdt_rsrp_thres_ssb_r17_present) {
    j.write_int("cg-SDT-RSRP-ThresholdSSB-r17", cg_sdt_rsrp_thres_ssb_r17);
  }
  if (cg_sdt_ta_validation_cfg_r17_present) {
    j.write_fieldname("cg-SDT-TA-ValidationConfig-r17");
    cg_sdt_ta_validation_cfg_r17.to_json(j);
  }
  if (cg_sdt_cs_rnti_r17_present) {
    j.write_int("cg-SDT-CS-RNTI-r17", cg_sdt_cs_rnti_r17);
  }
  j.end_obj();
}

// SL-ConfigCommonNR-r16 ::= SEQUENCE
SRSASN_CODE sl_cfg_common_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_freq_info_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_ue_sel_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_nr_anchor_carrier_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_eutra_anchor_carrier_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_radio_bearer_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rlc_bearer_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_cfg_common_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_offset_dfn_r16_present, 1));
  HANDLE_CODE(bref.pack(t400_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ssb_prio_nr_r16_present, 1));

  if (sl_freq_info_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_freq_info_list_r16, 1, 8));
  }
  if (sl_ue_sel_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_r16.pack(bref));
  }
  if (sl_nr_anchor_carrier_freq_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_nr_anchor_carrier_freq_list_r16, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sl_eutra_anchor_carrier_freq_list_r16.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, sl_eutra_anchor_carrier_freq_list_r16, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (sl_radio_bearer_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_radio_bearer_cfg_list_r16, 1, 512));
  }
  if (sl_rlc_bearer_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_bearer_cfg_list_r16, 1, 512));
  }
  if (sl_meas_cfg_common_r16_present) {
    HANDLE_CODE(sl_meas_cfg_common_r16.pack(bref));
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

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cfg_common_nr_r16_s::unpack(cbit_ref& bref)
{
  bool sl_freq_info_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_freq_info_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ue_sel_cfg_r16_present, 1));
  bool sl_nr_anchor_carrier_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_nr_anchor_carrier_freq_list_r16_present, 1));
  bool sl_eutra_anchor_carrier_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_eutra_anchor_carrier_freq_list_r16_present, 1));
  bool sl_radio_bearer_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_radio_bearer_cfg_list_r16_present, 1));
  bool sl_rlc_bearer_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_rlc_bearer_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_meas_cfg_common_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_offset_dfn_r16_present, 1));
  HANDLE_CODE(bref.unpack(t400_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ssb_prio_nr_r16_present, 1));

  if (sl_freq_info_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_freq_info_list_r16, bref, 1, 8));
  }
  if (sl_ue_sel_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_r16.unpack(bref));
  }
  if (sl_nr_anchor_carrier_freq_list_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_nr_anchor_carrier_freq_list_r16, bref, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sl_eutra_anchor_carrier_freq_list_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_eutra_anchor_carrier_freq_list_r16, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (sl_radio_bearer_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_radio_bearer_cfg_list_r16, bref, 1, 512));
  }
  if (sl_rlc_bearer_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_bearer_cfg_list_r16, bref, 1, 512));
  }
  if (sl_meas_cfg_common_r16_present) {
    HANDLE_CODE(sl_meas_cfg_common_r16.unpack(bref));
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

  return SRSASN_SUCCESS;
}
void sl_cfg_common_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_freq_info_list_r16.size() > 0) {
    j.start_array("sl-FreqInfoList-r16");
    for (const auto& e1 : sl_freq_info_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_ue_sel_cfg_r16_present) {
    j.write_fieldname("sl-UE-SelectedConfig-r16");
    sl_ue_sel_cfg_r16.to_json(j);
  }
  if (sl_nr_anchor_carrier_freq_list_r16.size() > 0) {
    j.start_array("sl-NR-AnchorCarrierFreqList-r16");
    for (const auto& e1 : sl_nr_anchor_carrier_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_eutra_anchor_carrier_freq_list_r16.size() > 0) {
    j.start_array("sl-EUTRA-AnchorCarrierFreqList-r16");
    for (const auto& e1 : sl_eutra_anchor_carrier_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_radio_bearer_cfg_list_r16.size() > 0) {
    j.start_array("sl-RadioBearerConfigList-r16");
    for (const auto& e1 : sl_radio_bearer_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_rlc_bearer_cfg_list_r16.size() > 0) {
    j.start_array("sl-RLC-BearerConfigList-r16");
    for (const auto& e1 : sl_rlc_bearer_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_meas_cfg_common_r16_present) {
    j.write_fieldname("sl-MeasConfigCommon-r16");
    sl_meas_cfg_common_r16.to_json(j);
  }
  if (sl_csi_acquisition_r16_present) {
    j.write_str("sl-CSI-Acquisition-r16", "enabled");
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
  j.end_obj();
}

const char* sl_cfg_common_nr_r16_s::t400_r16_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "sl_cfg_common_nr_r16_s::t400_r16_e_");
}
uint16_t sl_cfg_common_nr_r16_s::t400_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "sl_cfg_common_nr_r16_s::t400_r16_e_");
}

const char* sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n16", "n32"};
  return convert_enum_idx(names, 8, value, "sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}
uint8_t sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}

// SL-DiscConfigCommon-r17 ::= SEQUENCE
SRSASN_CODE sl_disc_cfg_common_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_relay_ue_cfg_common_r17.pack(bref));
  HANDLE_CODE(sl_remote_ue_cfg_common_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_common_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_relay_ue_cfg_common_r17.unpack(bref));
  HANDLE_CODE(sl_remote_ue_cfg_common_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_disc_cfg_common_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-RelayUE-ConfigCommon-r17");
  sl_relay_ue_cfg_common_r17.to_json(j);
  j.write_fieldname("sl-RemoteUE-ConfigCommon-r17");
  sl_remote_ue_cfg_common_r17.to_json(j);
  j.end_obj();
}

// UE-TimersAndConstantsRemoteUE-r17 ::= SEQUENCE
SRSASN_CODE ue_timers_and_consts_remote_ue_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(t300_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.pack(t301_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.pack(t319_remote_ue_r17_present, 1));

  if (t300_remote_ue_r17_present) {
    HANDLE_CODE(t300_remote_ue_r17.pack(bref));
  }
  if (t301_remote_ue_r17_present) {
    HANDLE_CODE(t301_remote_ue_r17.pack(bref));
  }
  if (t319_remote_ue_r17_present) {
    HANDLE_CODE(t319_remote_ue_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_timers_and_consts_remote_ue_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(t300_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.unpack(t301_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.unpack(t319_remote_ue_r17_present, 1));

  if (t300_remote_ue_r17_present) {
    HANDLE_CODE(t300_remote_ue_r17.unpack(bref));
  }
  if (t301_remote_ue_r17_present) {
    HANDLE_CODE(t301_remote_ue_r17.unpack(bref));
  }
  if (t319_remote_ue_r17_present) {
    HANDLE_CODE(t319_remote_ue_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_timers_and_consts_remote_ue_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (t300_remote_ue_r17_present) {
    j.write_str("t300-RemoteUE-r17", t300_remote_ue_r17.to_string());
  }
  if (t301_remote_ue_r17_present) {
    j.write_str("t301-RemoteUE-r17", t301_remote_ue_r17.to_string());
  }
  if (t319_remote_ue_r17_present) {
    j.write_str("t319-RemoteUE-r17", t319_remote_ue_r17.to_string());
  }
  j.end_obj();
}

const char* ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_e_");
}
uint16_t ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_e_");
}

const char* ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_e_");
}
uint16_t ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_e_");
}

const char* ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_e_");
}
uint16_t ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_e_");
}

// SIB12-IEs-r16 ::= SEQUENCE
SRSASN_CODE sib12_ies_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(sl_cfg_common_nr_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_drx_cfg_common_gc_bc_r17.is_present();
    group_flags[0] |= sl_disc_cfg_common_r17.is_present();
    group_flags[0] |= sl_l2_u2_n_relay_r17_present;
    group_flags[0] |= sl_non_relay_discovery_r17_present;
    group_flags[0] |= sl_l3_u2_n_relay_discovery_r17_present;
    group_flags[0] |= sl_timers_and_consts_remote_ue_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_drx_cfg_common_gc_bc_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_disc_cfg_common_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_l2_u2_n_relay_r17_present, 1));
      HANDLE_CODE(bref.pack(sl_non_relay_discovery_r17_present, 1));
      HANDLE_CODE(bref.pack(sl_l3_u2_n_relay_discovery_r17_present, 1));
      HANDLE_CODE(bref.pack(sl_timers_and_consts_remote_ue_r17.is_present(), 1));
      if (sl_drx_cfg_common_gc_bc_r17.is_present()) {
        HANDLE_CODE(sl_drx_cfg_common_gc_bc_r17->pack(bref));
      }
      if (sl_disc_cfg_common_r17.is_present()) {
        HANDLE_CODE(sl_disc_cfg_common_r17->pack(bref));
      }
      if (sl_timers_and_consts_remote_ue_r17.is_present()) {
        HANDLE_CODE(sl_timers_and_consts_remote_ue_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib12_ies_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sl_cfg_common_nr_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_drx_cfg_common_gc_bc_r17, bref);
      unpack_presence_flag(sl_disc_cfg_common_r17, bref);
      HANDLE_CODE(bref.unpack(sl_l2_u2_n_relay_r17_present, 1));
      HANDLE_CODE(bref.unpack(sl_non_relay_discovery_r17_present, 1));
      HANDLE_CODE(bref.unpack(sl_l3_u2_n_relay_discovery_r17_present, 1));
      unpack_presence_flag(sl_timers_and_consts_remote_ue_r17, bref);
      if (sl_drx_cfg_common_gc_bc_r17.is_present()) {
        HANDLE_CODE(sl_drx_cfg_common_gc_bc_r17->unpack(bref));
      }
      if (sl_disc_cfg_common_r17.is_present()) {
        HANDLE_CODE(sl_disc_cfg_common_r17->unpack(bref));
      }
      if (sl_timers_and_consts_remote_ue_r17.is_present()) {
        HANDLE_CODE(sl_timers_and_consts_remote_ue_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib12_ies_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-ConfigCommonNR-r16");
  sl_cfg_common_nr_r16.to_json(j);
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (sl_drx_cfg_common_gc_bc_r17.is_present()) {
      j.write_fieldname("sl-DRX-ConfigCommonGC-BC-r17");
      sl_drx_cfg_common_gc_bc_r17->to_json(j);
    }
    if (sl_disc_cfg_common_r17.is_present()) {
      j.write_fieldname("sl-DiscConfigCommon-r17");
      sl_disc_cfg_common_r17->to_json(j);
    }
    if (sl_l2_u2_n_relay_r17_present) {
      j.write_str("sl-L2U2N-Relay-r17", "enabled");
    }
    if (sl_non_relay_discovery_r17_present) {
      j.write_str("sl-NonRelayDiscovery-r17", "enabled");
    }
    if (sl_l3_u2_n_relay_discovery_r17_present) {
      j.write_str("sl-L3U2N-RelayDiscovery-r17", "enabled");
    }
    if (sl_timers_and_consts_remote_ue_r17.is_present()) {
      j.write_fieldname("sl-TimersAndConstantsRemoteUE-r17");
      sl_timers_and_consts_remote_ue_r17->to_json(j);
    }
  }
  j.end_obj();
}

// TRS-ResourceSet-r17 ::= SEQUENCE
SRSASN_CODE trs_res_set_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pwr_ctrl_offset_ss_r17.pack(bref));
  HANDLE_CODE(scrambling_id_info_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, first_ofdm_symbol_in_time_domain_r17, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(pack_integer(bref, start_rb_r17, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(pack_integer(bref, nrof_rbs_r17, (uint16_t)24u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, ssb_idx_r17, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(periodicity_and_offset_r17.pack(bref));
  HANDLE_CODE(freq_domain_alloc_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, ind_bit_id_r17, (uint8_t)0u, (uint8_t)5u));
  HANDLE_CODE(nrof_res_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE trs_res_set_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pwr_ctrl_offset_ss_r17.unpack(bref));
  HANDLE_CODE(scrambling_id_info_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(first_ofdm_symbol_in_time_domain_r17, bref, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(unpack_integer(start_rb_r17, bref, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(unpack_integer(nrof_rbs_r17, bref, (uint16_t)24u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(ssb_idx_r17, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(periodicity_and_offset_r17.unpack(bref));
  HANDLE_CODE(freq_domain_alloc_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(ind_bit_id_r17, bref, (uint8_t)0u, (uint8_t)5u));
  HANDLE_CODE(nrof_res_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void trs_res_set_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("powerControlOffsetSS-r17", pwr_ctrl_offset_ss_r17.to_string());
  j.write_fieldname("scramblingID-Info-r17");
  scrambling_id_info_r17.to_json(j);
  j.write_int("firstOFDMSymbolInTimeDomain-r17", first_ofdm_symbol_in_time_domain_r17);
  j.write_int("startingRB-r17", start_rb_r17);
  j.write_int("nrofRBs-r17", nrof_rbs_r17);
  j.write_int("ssb-Index-r17", ssb_idx_r17);
  j.write_fieldname("periodicityAndOffset-r17");
  periodicity_and_offset_r17.to_json(j);
  j.write_str("frequencyDomainAllocation-r17", freq_domain_alloc_r17.to_string());
  j.write_int("indBitID-r17", ind_bit_id_r17);
  j.write_str("nrofResources-r17", nrof_res_r17.to_string());
  j.end_obj();
}

const char* trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_opts::to_string() const
{
  static const char* names[] = {"db-3", "db0", "db3", "db6"};
  return convert_enum_idx(names, 4, value, "trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_e_");
}
int8_t trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_opts::to_number() const
{
  static const int8_t numbers[] = {-3, 0, 3, 6};
  return map_enum_number(numbers, 4, value, "trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_e_");
}

void trs_res_set_r17_s::scrambling_id_info_r17_c_::destroy_()
{
  switch (type_) {
    case types::scrambling_id_per_res_list_with2_r17:
      c.destroy<scrambling_id_per_res_list_with2_r17_l_>();
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.destroy<scrambling_id_per_res_list_with4_r17_l_>();
      break;
    default:
      break;
  }
}
void trs_res_set_r17_s::scrambling_id_info_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      c.init<scrambling_id_per_res_list_with2_r17_l_>();
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.init<scrambling_id_per_res_list_with4_r17_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_info_r17_c_(
    const trs_res_set_r17_s::scrambling_id_info_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      c.init(other.c.get<scrambling_id_per_res_list_with2_r17_l_>());
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.init(other.c.get<scrambling_id_per_res_list_with4_r17_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }
}
trs_res_set_r17_s::scrambling_id_info_r17_c_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::operator=(const trs_res_set_r17_s::scrambling_id_info_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      c.set(other.c.get<scrambling_id_per_res_list_with2_r17_l_>());
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.set(other.c.get<scrambling_id_per_res_list_with4_r17_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }

  return *this;
}
uint16_t& trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_for_common_r17()
{
  set(types::scrambling_id_for_common_r17);
  return c.get<uint16_t>();
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with2_r17_l_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_per_res_list_with2_r17()
{
  set(types::scrambling_id_per_res_list_with2_r17);
  return c.get<scrambling_id_per_res_list_with2_r17_l_>();
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with4_r17_l_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_per_res_list_with4_r17()
{
  set(types::scrambling_id_per_res_list_with4_r17);
  return c.get<scrambling_id_per_res_list_with4_r17_l_>();
}
void trs_res_set_r17_s::scrambling_id_info_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      j.write_int("scramblingIDforCommon-r17", c.get<uint16_t>());
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      j.start_array("scramblingIDperResourceListWith2-r17");
      for (const auto& e1 : c.get<scrambling_id_per_res_list_with2_r17_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      j.start_array("scramblingIDperResourceListWith4-r17");
      for (const auto& e1 : c.get<scrambling_id_per_res_list_with4_r17_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE trs_res_set_r17_s::scrambling_id_info_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    c.get<scrambling_id_per_res_list_with2_r17_l_>(),
                                    c.get<scrambling_id_per_res_list_with2_r17_l_>().size(),
                                    integer_packer<uint16_t>(0, 1023)));
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    c.get<scrambling_id_per_res_list_with4_r17_l_>(),
                                    c.get<scrambling_id_per_res_list_with4_r17_l_>().size(),
                                    integer_packer<uint16_t>(0, 1023)));
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE trs_res_set_r17_s::scrambling_id_info_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      HANDLE_CODE(unpack_fixed_seq_of(c.get<scrambling_id_per_res_list_with2_r17_l_>(),
                                      bref,
                                      c.get<scrambling_id_per_res_list_with2_r17_l_>().size(),
                                      integer_packer<uint16_t>(0, 1023)));
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      HANDLE_CODE(unpack_fixed_seq_of(c.get<scrambling_id_per_res_list_with4_r17_l_>(),
                                      bref,
                                      c.get<scrambling_id_per_res_list_with4_r17_l_>().size(),
                                      integer_packer<uint16_t>(0, 1023)));
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* trs_res_set_r17_s::scrambling_id_info_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "scramblingIDforCommon-r17", "scramblingIDperResourceListWith2-r17", "scramblingIDperResourceListWith4-r17"};
  return convert_enum_idx(names, 3, value, "trs_res_set_r17_s::scrambling_id_info_r17_c_::types");
}
uint8_t trs_res_set_r17_s::scrambling_id_info_r17_c_::types_opts::to_number() const
{
  switch (value) {
    case scrambling_id_per_res_list_with2_r17:
      return 2;
    case scrambling_id_per_res_list_with4_r17:
      return 4;
    default:
      invalid_enum_number(value, "trs_res_set_r17_s::scrambling_id_info_r17_c_::types");
  }
  return 0;
}

void trs_res_set_r17_s::periodicity_and_offset_r17_c_::destroy_() {}
void trs_res_set_r17_s::periodicity_and_offset_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
trs_res_set_r17_s::periodicity_and_offset_r17_c_::periodicity_and_offset_r17_c_(
    const trs_res_set_r17_s::periodicity_and_offset_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slots10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
  }
}
trs_res_set_r17_s::periodicity_and_offset_r17_c_& trs_res_set_r17_s::periodicity_and_offset_r17_c_::operator=(
    const trs_res_set_r17_s::periodicity_and_offset_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slots10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
  }

  return *this;
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots10()
{
  set(types::slots10);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots20()
{
  set(types::slots20);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots40()
{
  set(types::slots40);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots80()
{
  set(types::slots80);
  return c.get<uint8_t>();
}
void trs_res_set_r17_s::periodicity_and_offset_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slots10:
      j.write_int("slots10", c.get<uint8_t>());
      break;
    case types::slots20:
      j.write_int("slots20", c.get<uint8_t>());
      break;
    case types::slots40:
      j.write_int("slots40", c.get<uint8_t>());
      break;
    case types::slots80:
      j.write_int("slots80", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE trs_res_set_r17_s::periodicity_and_offset_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slots10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::slots20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::slots40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::slots80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE trs_res_set_r17_s::periodicity_and_offset_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slots10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::slots20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::slots40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::slots80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* trs_res_set_r17_s::periodicity_and_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"slots10", "slots20", "slots40", "slots80"};
  return convert_enum_idx(names, 4, value, "trs_res_set_r17_s::periodicity_and_offset_r17_c_::types");
}
uint8_t trs_res_set_r17_s::periodicity_and_offset_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 80};
  return map_enum_number(numbers, 4, value, "trs_res_set_r17_s::periodicity_and_offset_r17_c_::types");
}

const char* trs_res_set_r17_s::nrof_res_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(names, 2, value, "trs_res_set_r17_s::nrof_res_r17_e_");
}
uint8_t trs_res_set_r17_s::nrof_res_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "trs_res_set_r17_s::nrof_res_r17_e_");
}

// SIB17-IEs-r17 ::= SEQUENCE
SRSASN_CODE sib17_ies_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(validity_dur_r17_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, trs_res_set_cfg_r17, 1, 64));
  if (validity_dur_r17_present) {
    HANDLE_CODE(validity_dur_r17.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib17_ies_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(validity_dur_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(trs_res_set_cfg_r17, bref, 1, 64));
  if (validity_dur_r17_present) {
    HANDLE_CODE(validity_dur_r17.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib17_ies_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("trs-ResourceSetConfig-r17");
  for (const auto& e1 : trs_res_set_cfg_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (validity_dur_r17_present) {
    j.write_str("validityDuration-r17", validity_dur_r17.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

const char* sib17_ies_r17_s::validity_dur_r17_opts::to_string() const
{
  static const char* names[] = {"t1",
                                "t2",
                                "t4",
                                "t8",
                                "t16",
                                "t32",
                                "t64",
                                "t128",
                                "t256",
                                "t512",
                                "infinity",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sib17_ies_r17_s::validity_dur_r17_e_");
}
int16_t sib17_ies_r17_s::validity_dur_r17_opts::to_number() const
{
  static const int16_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, -1};
  return map_enum_number(numbers, 11, value, "sib17_ies_r17_s::validity_dur_r17_e_");
}

// SL-ServingCellInfo-r17 ::= SEQUENCE
SRSASN_CODE sl_serving_cell_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sl_pci_r17, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(pack_integer(bref, sl_carrier_freq_nr_r17, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_serving_cell_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sl_pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(unpack_integer(sl_carrier_freq_nr_r17, bref, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
void sl_serving_cell_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-PhysCellId-r17", sl_pci_r17);
  j.write_int("sl-CarrierFreqNR-r17", sl_carrier_freq_nr_r17);
  j.end_obj();
}

// SRS-PosConfig-r17 ::= SEQUENCE
SRSASN_CODE srs_pos_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_pos_res_set_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_pos_res_set_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_pos_res_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_pos_res_to_add_mod_list_r17.size() > 0, 1));

  if (srs_pos_res_set_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_set_to_release_list_r17, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (srs_pos_res_set_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_set_to_add_mod_list_r17, 1, 16));
  }
  if (srs_pos_res_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_to_release_list_r17, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (srs_pos_res_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_to_add_mod_list_r17, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_cfg_r17_s::unpack(cbit_ref& bref)
{
  bool srs_pos_res_set_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_set_to_release_list_r17_present, 1));
  bool srs_pos_res_set_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_set_to_add_mod_list_r17_present, 1));
  bool srs_pos_res_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_to_release_list_r17_present, 1));
  bool srs_pos_res_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_to_add_mod_list_r17_present, 1));

  if (srs_pos_res_set_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_set_to_release_list_r17, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (srs_pos_res_set_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_set_to_add_mod_list_r17, bref, 1, 16));
  }
  if (srs_pos_res_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_to_release_list_r17, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (srs_pos_res_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_to_add_mod_list_r17, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void srs_pos_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_pos_res_set_to_release_list_r17.size() > 0) {
    j.start_array("srs-PosResourceSetToReleaseList-r17");
    for (const auto& e1 : srs_pos_res_set_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (srs_pos_res_set_to_add_mod_list_r17.size() > 0) {
    j.start_array("srs-PosResourceSetToAddModList-r17");
    for (const auto& e1 : srs_pos_res_set_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srs_pos_res_to_release_list_r17.size() > 0) {
    j.start_array("srs-PosResourceToReleaseList-r17");
    for (const auto& e1 : srs_pos_res_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (srs_pos_res_to_add_mod_list_r17.size() > 0) {
    j.start_array("srs-PosResourceToAddModList-r17");
    for (const auto& e1 : srs_pos_res_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RSRP-ChangeThreshold-r17 ::= ENUMERATED
const char* rsrp_change_thres_r17_opts::to_string() const
{
  static const char* names[] = {"dB4",
                                "dB6",
                                "dB8",
                                "dB10",
                                "dB14",
                                "dB18",
                                "dB22",
                                "dB26",
                                "dB30",
                                "dB34",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "rsrp_change_thres_r17_e");
}
uint8_t rsrp_change_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 6, 8, 10, 14, 18, 22, 26, 30, 34};
  return map_enum_number(numbers, 10, value, "rsrp_change_thres_r17_e");
}

// SRS-PosRRC-InactiveConfig-r17 ::= SEQUENCE
SRSASN_CODE srs_pos_rrc_inactive_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_pos_cfg_n_ul_r17_present, 1));
  HANDLE_CODE(bref.pack(srs_pos_cfg_sul_r17_present, 1));
  HANDLE_CODE(bref.pack(bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.pack(bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.pack(inactive_pos_srs_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.pack(inactive_pos_srs_rsrp_change_thres_r17_present, 1));

  if (srs_pos_cfg_n_ul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_n_ul_r17.pack(bref));
  }
  if (srs_pos_cfg_sul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_sul_r17.pack(bref));
  }
  if (bwp_n_ul_r17_present) {
    HANDLE_CODE(bwp_n_ul_r17.pack(bref));
  }
  if (bwp_sul_r17_present) {
    HANDLE_CODE(bwp_sul_r17.pack(bref));
  }
  if (inactive_pos_srs_time_align_timer_r17_present) {
    HANDLE_CODE(inactive_pos_srs_time_align_timer_r17.pack(bref));
  }
  if (inactive_pos_srs_rsrp_change_thres_r17_present) {
    HANDLE_CODE(inactive_pos_srs_rsrp_change_thres_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_rrc_inactive_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_pos_cfg_n_ul_r17_present, 1));
  HANDLE_CODE(bref.unpack(srs_pos_cfg_sul_r17_present, 1));
  HANDLE_CODE(bref.unpack(bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.unpack(bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.unpack(inactive_pos_srs_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.unpack(inactive_pos_srs_rsrp_change_thres_r17_present, 1));

  if (srs_pos_cfg_n_ul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_n_ul_r17.unpack(bref));
  }
  if (srs_pos_cfg_sul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_sul_r17.unpack(bref));
  }
  if (bwp_n_ul_r17_present) {
    HANDLE_CODE(bwp_n_ul_r17.unpack(bref));
  }
  if (bwp_sul_r17_present) {
    HANDLE_CODE(bwp_sul_r17.unpack(bref));
  }
  if (inactive_pos_srs_time_align_timer_r17_present) {
    HANDLE_CODE(inactive_pos_srs_time_align_timer_r17.unpack(bref));
  }
  if (inactive_pos_srs_rsrp_change_thres_r17_present) {
    HANDLE_CODE(inactive_pos_srs_rsrp_change_thres_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_pos_rrc_inactive_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_pos_cfg_n_ul_r17_present) {
    j.write_fieldname("srs-PosConfigNUL-r17");
    srs_pos_cfg_n_ul_r17.to_json(j);
  }
  if (srs_pos_cfg_sul_r17_present) {
    j.write_fieldname("srs-PosConfigSUL-r17");
    srs_pos_cfg_sul_r17.to_json(j);
  }
  if (bwp_n_ul_r17_present) {
    j.write_fieldname("bwp-NUL-r17");
    bwp_n_ul_r17.to_json(j);
  }
  if (bwp_sul_r17_present) {
    j.write_fieldname("bwp-SUL-r17");
    bwp_sul_r17.to_json(j);
  }
  if (inactive_pos_srs_time_align_timer_r17_present) {
    j.write_str("inactivePosSRS-TimeAlignmentTimer-r17", inactive_pos_srs_time_align_timer_r17.to_string());
  }
  if (inactive_pos_srs_rsrp_change_thres_r17_present) {
    j.write_str("inactivePosSRS-RSRP-ChangeThreshold-r17", inactive_pos_srs_rsrp_change_thres_r17.to_string());
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterCommon ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mrdc_request_present, 1));

  if (mrdc_request_present) {
    HANDLE_CODE(bref.pack(mrdc_request.omit_en_dc_present, 1));
    HANDLE_CODE(bref.pack(mrdc_request.include_nr_dc_present, 1));
    HANDLE_CODE(bref.pack(mrdc_request.include_ne_dc_present, 1));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= codebook_type_request_r16.is_present();
    group_flags[0] |= ul_tx_switch_request_r16_present;
    group_flags[1] |= requested_cell_grouping_r16.is_present();
    group_flags[2] |= fallback_group_five_request_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(codebook_type_request_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_tx_switch_request_r16_present, 1));
      if (codebook_type_request_r16.is_present()) {
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type1_single_panel_r16_present, 1));
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type1_multi_panel_r16_present, 1));
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type2_r16_present, 1));
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type2_port_sel_r16_present, 1));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(requested_cell_grouping_r16.is_present(), 1));
      if (requested_cell_grouping_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *requested_cell_grouping_r16, 1, 32));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(fallback_group_five_request_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mrdc_request_present, 1));

  if (mrdc_request_present) {
    HANDLE_CODE(bref.unpack(mrdc_request.omit_en_dc_present, 1));
    HANDLE_CODE(bref.unpack(mrdc_request.include_nr_dc_present, 1));
    HANDLE_CODE(bref.unpack(mrdc_request.include_ne_dc_present, 1));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(codebook_type_request_r16, bref);
      HANDLE_CODE(bref.unpack(ul_tx_switch_request_r16_present, 1));
      if (codebook_type_request_r16.is_present()) {
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type1_single_panel_r16_present, 1));
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type1_multi_panel_r16_present, 1));
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type2_r16_present, 1));
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type2_port_sel_r16_present, 1));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(requested_cell_grouping_r16, bref);
      if (requested_cell_grouping_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*requested_cell_grouping_r16, bref, 1, 32));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(fallback_group_five_request_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mrdc_request_present) {
    j.write_fieldname("mrdc-Request");
    j.start_obj();
    if (mrdc_request.omit_en_dc_present) {
      j.write_str("omitEN-DC", "true");
    }
    if (mrdc_request.include_nr_dc_present) {
      j.write_str("includeNR-DC", "true");
    }
    if (mrdc_request.include_ne_dc_present) {
      j.write_str("includeNE-DC", "true");
    }
    j.end_obj();
  }
  if (ext) {
    if (codebook_type_request_r16.is_present()) {
      j.write_fieldname("codebookTypeRequest-r16");
      j.start_obj();
      if (codebook_type_request_r16->type1_single_panel_r16_present) {
        j.write_str("type1-SinglePanel-r16", "true");
      }
      if (codebook_type_request_r16->type1_multi_panel_r16_present) {
        j.write_str("type1-MultiPanel-r16", "true");
      }
      if (codebook_type_request_r16->type2_r16_present) {
        j.write_str("type2-r16", "true");
      }
      if (codebook_type_request_r16->type2_port_sel_r16_present) {
        j.write_str("type2-PortSelection-r16", "true");
      }
      j.end_obj();
    }
    if (ul_tx_switch_request_r16_present) {
      j.write_str("uplinkTxSwitchRequest-r16", "true");
    }
    if (requested_cell_grouping_r16.is_present()) {
      j.start_array("requestedCellGrouping-r16");
      for (const auto& e1 : *requested_cell_grouping_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (fallback_group_five_request_r17_present) {
      j.write_str("fallbackGroupFiveRequest-r17", "true");
    }
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterNR-v1710 ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_nr_v1710_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sidelink_request_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_nr_v1710_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sidelink_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_nr_v1710_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sidelink_request_r17_present) {
    j.write_str("sidelinkRequest-r17", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterNR-v1540 ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_nr_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_switching_time_request_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_nr_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_switching_time_request_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_nr_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_switching_time_request_present) {
    j.write_str("srs-SwitchingTimeRequest", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterNR ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_list_filt.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_band_list_filt.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_filt, 1, 1280));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_nr_s::unpack(cbit_ref& bref)
{
  bool freq_band_list_filt_present;
  HANDLE_CODE(bref.unpack(freq_band_list_filt_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_band_list_filt_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_filt, bref, 1, 1280));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_list_filt.size() > 0) {
    j.start_array("frequencyBandListFilter");
    for (const auto& e1 : freq_band_list_filt) {
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

// UE-NR-Capability-v16c0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v16c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v16c0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v16c0_present) {
    HANDLE_CODE(rf_params_v16c0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v16c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v16c0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v16c0_present) {
    HANDLE_CODE(rf_params_v16c0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v16c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v16c0_present) {
    j.write_fieldname("rf-Parameters-v16c0");
    rf_params_v16c0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UE-NR-Capability-v16a0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_params_v16a0_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v16a0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_params_v16a0_present) {
    HANDLE_CODE(phy_params_v16a0.pack(bref));
  }
  if (rf_params_v16a0_present) {
    HANDLE_CODE(rf_params_v16a0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_params_v16a0_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v16a0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_params_v16a0_present) {
    HANDLE_CODE(phy_params_v16a0.unpack(bref));
  }
  if (rf_params_v16a0_present) {
    HANDLE_CODE(rf_params_v16a0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_params_v16a0_present) {
    j.write_fieldname("phy-Parameters-v16a0");
    phy_params_v16a0.to_json(j);
  }
  if (rf_params_v16a0_present) {
    j.write_fieldname("rf-Parameters-v16a0");
    rf_params_v16a0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-NR-Capability-v15j0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v15j0_s::pack(bit_ref& bref) const
{
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
SRSASN_CODE ue_nr_cap_v15j0_s::unpack(cbit_ref& bref)
{
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
void ue_nr_cap_v15j0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// NRDC-Parameters-v15c0 ::= SEQUENCE
SRSASN_CODE nrdc_params_v15c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nrdc_params_v15c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
void nrdc_params_v15c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_dupl_split_srb_present) {
    j.write_str("pdcp-DuplicationSplitSRB", "supported");
  }
  if (pdcp_dupl_split_drb_present) {
    j.write_str("pdcp-DuplicationSplitDRB", "supported");
  }
  j.end_obj();
}

// UE-NR-Capability-v15g0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v15g0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v15g0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v15g0_present) {
    HANDLE_CODE(rf_params_v15g0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v15g0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v15g0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v15g0_present) {
    HANDLE_CODE(rf_params_v15g0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v15g0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v15g0_present) {
    j.write_fieldname("rf-Parameters-v15g0");
    rf_params_v15g0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-NR-Capability-v15c0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v15c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nrdc_params_v15c0_present, 1));
  HANDLE_CODE(bref.pack(partial_fr2_fallback_rx_req_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (nrdc_params_v15c0_present) {
    HANDLE_CODE(nrdc_params_v15c0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v15c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nrdc_params_v15c0_present, 1));
  HANDLE_CODE(bref.unpack(partial_fr2_fallback_rx_req_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (nrdc_params_v15c0_present) {
    HANDLE_CODE(nrdc_params_v15c0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v15c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nrdc_params_v15c0_present) {
    j.write_fieldname("nrdc-Parameters-v15c0");
    nrdc_params_v15c0.to_json(j);
  }
  if (partial_fr2_fallback_rx_req_present) {
    j.write_str("partialFR2-FallbackRX-Req", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1610-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rrc_seg_allowed_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rrc_seg_allowed_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rrc_seg_allowed_r16_present) {
    j.write_str("rrc-SegAllowed-r16", "enabled");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1560-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cap_request_filt_common_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cap_request_filt_common_present) {
    HANDLE_CODE(cap_request_filt_common.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cap_request_filt_common_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cap_request_filt_common_present) {
    HANDLE_CODE(cap_request_filt_common.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cap_request_filt_common_present) {
    j.write_fieldname("capabilityRequestFilterCommon");
    cap_request_filt_common.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
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

// CG-CandidateInfoId-r17 ::= SEQUENCE
SRSASN_CODE cg_candidate_info_id_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ssb_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_info_id_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ssb_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));

  return SRSASN_SUCCESS;
}
void cg_candidate_info_id_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssbFrequency-r17", ssb_freq_r17);
  j.write_int("physCellId-r17", pci_r17);
  j.end_obj();
}

// CG-CandidateInfo-r17 ::= SEQUENCE
SRSASN_CODE cg_candidate_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cg_candidate_info_id_r17.pack(bref));
  HANDLE_CODE(candidate_cg_cfg_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cg_candidate_info_id_r17.unpack(bref));
  HANDLE_CODE(candidate_cg_cfg_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_candidate_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cg-CandidateInfoId-r17");
  cg_candidate_info_id_r17.to_json(j);
  j.write_str("candidateCG-Config-r17", candidate_cg_cfg_r17.to_string());
  j.end_obj();
}

// CG-CandidateList-r17-IEs ::= SEQUENCE
SRSASN_CODE cg_candidate_list_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cg_candidate_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(cg_candidate_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cg_candidate_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cg_candidate_to_add_mod_list_r17, 1, 8));
  }
  if (cg_candidate_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cg_candidate_to_release_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_r17_ies_s::unpack(cbit_ref& bref)
{
  bool cg_candidate_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_candidate_to_add_mod_list_r17_present, 1));
  bool cg_candidate_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_candidate_to_release_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cg_candidate_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cg_candidate_to_add_mod_list_r17, bref, 1, 8));
  }
  if (cg_candidate_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cg_candidate_to_release_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void cg_candidate_list_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cg_candidate_to_add_mod_list_r17.size() > 0) {
    j.start_array("cg-CandidateToAddModList-r17");
    for (const auto& e1 : cg_candidate_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cg_candidate_to_release_list_r17.size() > 0) {
    j.start_array("cg-CandidateToReleaseList-r17");
    for (const auto& e1 : cg_candidate_to_release_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CG-CandidateList ::= SEQUENCE
SRSASN_CODE cg_candidate_list_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_candidate_list_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void cg_candidate_list_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_candidate_list_s::crit_exts_c_::c1_c_& cg_candidate_list_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_candidate_list_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void cg_candidate_list_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cg_candidate_list_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_candidate_list_r17_ies_s& cg_candidate_list_s::crit_exts_c_::c1_c_::set_cg_candidate_list_r17()
{
  set(types::cg_candidate_list_r17);
  return c;
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cg_candidate_list_r17:
      j.write_fieldname("cg-CandidateList-r17");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cg_candidate_list_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cg_candidate_list_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cg_candidate_list_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"cg-CandidateList-r17", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "cg_candidate_list_s::crit_exts_c_::c1_c_::types");
}

const char* cg_candidate_list_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "cg_candidate_list_s::crit_exts_c_::types");
}
uint8_t cg_candidate_list_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_candidate_list_s::crit_exts_c_::types");
}

// CandidateCell-r17 ::= SEQUENCE
SRSASN_CODE candidate_cell_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cond_execution_cond_scg_r17.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));
  if (cond_execution_cond_scg_r17.size() > 0) {
    HANDLE_CODE(cond_execution_cond_scg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_cell_r17_s::unpack(cbit_ref& bref)
{
  bool cond_execution_cond_scg_r17_present;
  HANDLE_CODE(bref.unpack(cond_execution_cond_scg_r17_present, 1));

  HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  if (cond_execution_cond_scg_r17_present) {
    HANDLE_CODE(cond_execution_cond_scg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void candidate_cell_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r17", pci_r17);
  if (cond_execution_cond_scg_r17.size() > 0) {
    j.write_str("condExecutionCondSCG-r17", cond_execution_cond_scg_r17.to_string());
  }
  j.end_obj();
}

// CandidateCellInfo-r17 ::= SEQUENCE
SRSASN_CODE candidate_cell_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ssb_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, candidate_list_r17, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_cell_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ssb_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(candidate_list_r17, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void candidate_cell_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssbFrequency-r17", ssb_freq_r17);
  j.start_array("candidateList-r17");
  for (const auto& e1 : candidate_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CG-Config-v1730-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1730_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fr1_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.pack(fr2_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fr1_carriers_scg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr1_carriers_scg_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_scg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr2_carriers_scg_r17, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1730_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fr1_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.unpack(fr2_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fr1_carriers_scg_r17_present) {
    HANDLE_CODE(unpack_integer(fr1_carriers_scg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_scg_r17_present) {
    HANDLE_CODE(unpack_integer(fr2_carriers_scg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1730_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fr1_carriers_scg_r17_present) {
    j.write_int("fr1-Carriers-SCG-r17", fr1_carriers_scg_r17);
  }
  if (fr2_carriers_scg_r17_present) {
    j.write_int("fr2-Carriers-SCG-r17", fr2_carriers_scg_r17);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CG-Config-v1700-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(candidate_cell_info_list_cp_c_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(two_phr_mode_scg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_cp_c_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_info_list_cp_c_r17, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool candidate_cell_info_list_cp_c_r17_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_cp_c_r17_present, 1));
  HANDLE_CODE(bref.unpack(two_phr_mode_scg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_cp_c_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_info_list_cp_c_r17, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_cell_info_list_cp_c_r17.size() > 0) {
    j.start_array("candidateCellInfoListCPC-r17");
    for (const auto& e1 : candidate_cell_info_list_cp_c_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (two_phr_mode_scg_r17_present) {
    j.write_str("twoPHRModeSCG-r17", "enabled");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1640-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1640_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_cell_info_list_scg_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(serv_cell_info_list_scg_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (serv_cell_info_list_scg_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_scg_nr_r16, 1, 32));
  }
  if (serv_cell_info_list_scg_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_scg_eutra_r16, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1640_ies_s::unpack(cbit_ref& bref)
{
  bool serv_cell_info_list_scg_nr_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_scg_nr_r16_present, 1));
  bool serv_cell_info_list_scg_eutra_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_scg_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (serv_cell_info_list_scg_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_scg_nr_r16, bref, 1, 32));
  }
  if (serv_cell_info_list_scg_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_scg_eutra_r16, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1640_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_cell_info_list_scg_nr_r16.size() > 0) {
    j.start_array("servCellInfoListSCG-NR-r16");
    for (const auto& e1 : serv_cell_info_list_scg_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (serv_cell_info_list_scg_eutra_r16.size() > 0) {
    j.start_array("servCellInfoListSCG-EUTRA-r16");
    for (const auto& e1 : serv_cell_info_list_scg_eutra_r16) {
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

// CG-Config-v1630-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1630_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sel_toffset_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sel_toffset_r16_present) {
    HANDLE_CODE(sel_toffset_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1630_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sel_toffset_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sel_toffset_r16_present) {
    HANDLE_CODE(sel_toffset_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1630_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sel_toffset_r16_present) {
    j.write_str("selectedToffset-r16", sel_toffset_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1620-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1620_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_assist_info_scg_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_assist_info_scg_r16.size() > 0) {
    HANDLE_CODE(ue_assist_info_scg_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1620_ies_s::unpack(cbit_ref& bref)
{
  bool ue_assist_info_scg_r16_present;
  HANDLE_CODE(bref.unpack(ue_assist_info_scg_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_assist_info_scg_r16_present) {
    HANDLE_CODE(ue_assist_info_scg_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1620_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_assist_info_scg_r16.size() > 0) {
    j.write_str("ueAssistanceInformationSCG-r16", ue_assist_info_scg_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1610-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_info_scg2_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (drx_info_scg2_present) {
    HANDLE_CODE(drx_info_scg2.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_info_scg2_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (drx_info_scg2_present) {
    HANDLE_CODE(drx_info_scg2.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drx_info_scg2_present) {
    j.write_fieldname("drx-InfoSCG2");
    drx_info_scg2.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-UplinkCarrierSCG ::= SEQUENCE
SRSASN_CODE ph_ul_carrier_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ph_type1or3.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_ul_carrier_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ph_type1or3.unpack(bref));

  return SRSASN_SUCCESS;
}
void ph_ul_carrier_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ph-Type1or3", ph_type1or3.to_string());
  j.end_obj();
}

const char* ph_ul_carrier_scg_s::ph_type1or3_opts::to_string() const
{
  static const char* names[] = {"type1", "type3"};
  return convert_enum_idx(names, 2, value, "ph_ul_carrier_scg_s::ph_type1or3_e_");
}
uint8_t ph_ul_carrier_scg_s::ph_type1or3_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 3};
  return map_enum_number(numbers, 2, value, "ph_ul_carrier_scg_s::ph_type1or3_e_");
}

// CG-Config-v1590-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scell_frequencies_sn_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(scell_frequencies_sn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scell_frequencies_sn_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_frequencies_sn_nr, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (scell_frequencies_sn_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_frequencies_sn_eutra, 1, 31, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool scell_frequencies_sn_nr_present;
  HANDLE_CODE(bref.unpack(scell_frequencies_sn_nr_present, 1));
  bool scell_frequencies_sn_eutra_present;
  HANDLE_CODE(bref.unpack(scell_frequencies_sn_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scell_frequencies_sn_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_frequencies_sn_nr, bref, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (scell_frequencies_sn_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_frequencies_sn_eutra, bref, 1, 31, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scell_frequencies_sn_nr.size() > 0) {
    j.start_array("scellFrequenciesSN-NR");
    for (const auto& e1 : scell_frequencies_sn_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (scell_frequencies_sn_eutra.size() > 0) {
    j.start_array("scellFrequenciesSN-EUTRA");
    for (const auto& e1 : scell_frequencies_sn_eutra) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-InfoSCG ::= SEQUENCE
SRSASN_CODE ph_info_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.pack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= two_srs_pusch_repeat_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_info_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.unpack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void ph_info_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_fieldname("ph-Uplink");
  ph_ul.to_json(j);
  if (ph_supplementary_ul_present) {
    j.write_fieldname("ph-SupplementaryUplink");
    ph_supplementary_ul.to_json(j);
  }
  if (ext) {
    if (two_srs_pusch_repeat_r17_present) {
      j.write_str("twoSRS-PUSCH-Repetition-r17", "enabled");
    }
  }
  j.end_obj();
}

// CG-Config-v1560-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pscell_freq_eutra_present, 1));
  HANDLE_CODE(bref.pack(scg_cell_group_cfg_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_serving_freq_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(need_for_gaps_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_scg_present, 1));
  HANDLE_CODE(bref.pack(report_cgi_request_eutra_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pscell_freq_eutra_present) {
    HANDLE_CODE(pack_integer(bref, pscell_freq_eutra, (uint32_t)0u, (uint32_t)262143u));
  }
  if (scg_cell_group_cfg_eutra.size() > 0) {
    HANDLE_CODE(scg_cell_group_cfg_eutra.pack(bref));
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.pack(bref));
  }
  if (candidate_serving_freq_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_serving_freq_list_eutra, 1, 32, integer_packer<uint32_t>(0, 262143)));
  }
  if (drx_cfg_scg_present) {
    HANDLE_CODE(drx_cfg_scg.pack(bref));
  }
  if (report_cgi_request_eutra_present) {
    HANDLE_CODE(bref.pack(report_cgi_request_eutra.requested_cell_info_eutra_present, 1));
    if (report_cgi_request_eutra.requested_cell_info_eutra_present) {
      HANDLE_CODE(pack_integer(
          bref, report_cgi_request_eutra.requested_cell_info_eutra.eutra_freq, (uint32_t)0u, (uint32_t)262143u));
      HANDLE_CODE(pack_integer(bref,
                               report_cgi_request_eutra.requested_cell_info_eutra.cell_for_which_to_report_cgi_eutra,
                               (uint16_t)0u,
                               (uint16_t)503u));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pscell_freq_eutra_present, 1));
  bool scg_cell_group_cfg_eutra_present;
  HANDLE_CODE(bref.unpack(scg_cell_group_cfg_eutra_present, 1));
  bool candidate_cell_info_list_sn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_eutra_present, 1));
  bool candidate_serving_freq_list_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_serving_freq_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gaps_present, 1));
  HANDLE_CODE(bref.unpack(drx_cfg_scg_present, 1));
  HANDLE_CODE(bref.unpack(report_cgi_request_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pscell_freq_eutra_present) {
    HANDLE_CODE(unpack_integer(pscell_freq_eutra, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (scg_cell_group_cfg_eutra_present) {
    HANDLE_CODE(scg_cell_group_cfg_eutra.unpack(bref));
  }
  if (candidate_cell_info_list_sn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.unpack(bref));
  }
  if (candidate_serving_freq_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_serving_freq_list_eutra, bref, 1, 32, integer_packer<uint32_t>(0, 262143)));
  }
  if (drx_cfg_scg_present) {
    HANDLE_CODE(drx_cfg_scg.unpack(bref));
  }
  if (report_cgi_request_eutra_present) {
    HANDLE_CODE(bref.unpack(report_cgi_request_eutra.requested_cell_info_eutra_present, 1));
    if (report_cgi_request_eutra.requested_cell_info_eutra_present) {
      HANDLE_CODE(unpack_integer(
          report_cgi_request_eutra.requested_cell_info_eutra.eutra_freq, bref, (uint32_t)0u, (uint32_t)262143u));
      HANDLE_CODE(unpack_integer(report_cgi_request_eutra.requested_cell_info_eutra.cell_for_which_to_report_cgi_eutra,
                                 bref,
                                 (uint16_t)0u,
                                 (uint16_t)503u));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pscell_freq_eutra_present) {
    j.write_int("pSCellFrequencyEUTRA", pscell_freq_eutra);
  }
  if (scg_cell_group_cfg_eutra.size() > 0) {
    j.write_str("scg-CellGroupConfigEUTRA", scg_cell_group_cfg_eutra.to_string());
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListSN-EUTRA", candidate_cell_info_list_sn_eutra.to_string());
  }
  if (candidate_serving_freq_list_eutra.size() > 0) {
    j.start_array("candidateServingFreqListEUTRA");
    for (const auto& e1 : candidate_serving_freq_list_eutra) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (need_for_gaps_present) {
    j.write_str("needForGaps", "true");
  }
  if (drx_cfg_scg_present) {
    j.write_fieldname("drx-ConfigSCG");
    drx_cfg_scg.to_json(j);
  }
  if (report_cgi_request_eutra_present) {
    j.write_fieldname("reportCGI-RequestEUTRA");
    j.start_obj();
    if (report_cgi_request_eutra.requested_cell_info_eutra_present) {
      j.write_fieldname("requestedCellInfoEUTRA");
      j.start_obj();
      j.write_int("eutraFrequency", report_cgi_request_eutra.requested_cell_info_eutra.eutra_freq);
      j.write_int("cellForWhichToReportCGI-EUTRA",
                  report_cgi_request_eutra.requested_cell_info_eutra.cell_for_which_to_report_cgi_eutra);
      j.end_obj();
    }
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1540-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pscell_freq_present, 1));
  HANDLE_CODE(bref.pack(report_cgi_request_nr_present, 1));
  HANDLE_CODE(bref.pack(ph_info_scg.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pscell_freq_present) {
    HANDLE_CODE(pack_integer(bref, pscell_freq, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (report_cgi_request_nr_present) {
    HANDLE_CODE(bref.pack(report_cgi_request_nr.requested_cell_info_present, 1));
    if (report_cgi_request_nr.requested_cell_info_present) {
      HANDLE_CODE(
          pack_integer(bref, report_cgi_request_nr.requested_cell_info.ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
      HANDLE_CODE(pack_integer(
          bref, report_cgi_request_nr.requested_cell_info.cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)1007u));
    }
  }
  if (ph_info_scg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ph_info_scg, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pscell_freq_present, 1));
  HANDLE_CODE(bref.unpack(report_cgi_request_nr_present, 1));
  bool ph_info_scg_present;
  HANDLE_CODE(bref.unpack(ph_info_scg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pscell_freq_present) {
    HANDLE_CODE(unpack_integer(pscell_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (report_cgi_request_nr_present) {
    HANDLE_CODE(bref.unpack(report_cgi_request_nr.requested_cell_info_present, 1));
    if (report_cgi_request_nr.requested_cell_info_present) {
      HANDLE_CODE(
          unpack_integer(report_cgi_request_nr.requested_cell_info.ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
      HANDLE_CODE(unpack_integer(
          report_cgi_request_nr.requested_cell_info.cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)1007u));
    }
  }
  if (ph_info_scg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ph_info_scg, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pscell_freq_present) {
    j.write_int("pSCellFrequency", pscell_freq);
  }
  if (report_cgi_request_nr_present) {
    j.write_fieldname("reportCGI-RequestNR");
    j.start_obj();
    if (report_cgi_request_nr.requested_cell_info_present) {
      j.write_fieldname("requestedCellInfo");
      j.start_obj();
      j.write_int("ssbFrequency", report_cgi_request_nr.requested_cell_info.ssb_freq);
      j.write_int("cellForWhichToReportCGI", report_cgi_request_nr.requested_cell_info.cell_for_which_to_report_cgi);
      j.end_obj();
    }
    j.end_obj();
  }
  if (ph_info_scg.size() > 0) {
    j.start_array("ph-InfoSCG");
    for (const auto& e1 : ph_info_scg) {
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

// ConfigRestrictModReqSCG ::= SEQUENCE
SRSASN_CODE cfg_restrict_mod_req_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(requested_bc_mrdc_present, 1));
  HANDLE_CODE(bref.pack(requested_p_max_fr1_present, 1));

  if (requested_bc_mrdc_present) {
    HANDLE_CODE(requested_bc_mrdc.pack(bref));
  }
  if (requested_p_max_fr1_present) {
    HANDLE_CODE(pack_integer(bref, requested_p_max_fr1, (int8_t)-30, (int8_t)33));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= requested_pdcch_blind_detection_scg_present;
    group_flags[0] |= requested_p_max_eutra_present;
    group_flags[1] |= requested_p_max_fr2_r16_present;
    group_flags[1] |= requested_max_inter_freq_meas_id_scg_r16_present;
    group_flags[1] |= requested_max_intra_freq_meas_id_scg_r16_present;
    group_flags[1] |= requested_toffset_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(requested_pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.pack(requested_p_max_eutra_present, 1));
      if (requested_pdcch_blind_detection_scg_present) {
        HANDLE_CODE(pack_integer(bref, requested_pdcch_blind_detection_scg, (uint8_t)1u, (uint8_t)15u));
      }
      if (requested_p_max_eutra_present) {
        HANDLE_CODE(pack_integer(bref, requested_p_max_eutra, (int8_t)-30, (int8_t)33));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(requested_p_max_fr2_r16_present, 1));
      HANDLE_CODE(bref.pack(requested_max_inter_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(requested_max_intra_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(requested_toffset_r16_present, 1));
      if (requested_p_max_fr2_r16_present) {
        HANDLE_CODE(pack_integer(bref, requested_p_max_fr2_r16, (int8_t)-30, (int8_t)33));
      }
      if (requested_max_inter_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, requested_max_inter_freq_meas_id_scg_r16, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_max_intra_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, requested_max_intra_freq_meas_id_scg_r16, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_toffset_r16_present) {
        HANDLE_CODE(requested_toffset_r16.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_mod_req_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(requested_bc_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(requested_p_max_fr1_present, 1));

  if (requested_bc_mrdc_present) {
    HANDLE_CODE(requested_bc_mrdc.unpack(bref));
  }
  if (requested_p_max_fr1_present) {
    HANDLE_CODE(unpack_integer(requested_p_max_fr1, bref, (int8_t)-30, (int8_t)33));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(requested_pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.unpack(requested_p_max_eutra_present, 1));
      if (requested_pdcch_blind_detection_scg_present) {
        HANDLE_CODE(unpack_integer(requested_pdcch_blind_detection_scg, bref, (uint8_t)1u, (uint8_t)15u));
      }
      if (requested_p_max_eutra_present) {
        HANDLE_CODE(unpack_integer(requested_p_max_eutra, bref, (int8_t)-30, (int8_t)33));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(requested_p_max_fr2_r16_present, 1));
      HANDLE_CODE(bref.unpack(requested_max_inter_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(requested_max_intra_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(requested_toffset_r16_present, 1));
      if (requested_p_max_fr2_r16_present) {
        HANDLE_CODE(unpack_integer(requested_p_max_fr2_r16, bref, (int8_t)-30, (int8_t)33));
      }
      if (requested_max_inter_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(unpack_integer(requested_max_inter_freq_meas_id_scg_r16, bref, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_max_intra_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(unpack_integer(requested_max_intra_freq_meas_id_scg_r16, bref, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_toffset_r16_present) {
        HANDLE_CODE(requested_toffset_r16.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cfg_restrict_mod_req_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_bc_mrdc_present) {
    j.write_fieldname("requestedBC-MRDC");
    requested_bc_mrdc.to_json(j);
  }
  if (requested_p_max_fr1_present) {
    j.write_int("requestedP-MaxFR1", requested_p_max_fr1);
  }
  if (ext) {
    if (requested_pdcch_blind_detection_scg_present) {
      j.write_int("requestedPDCCH-BlindDetectionSCG", requested_pdcch_blind_detection_scg);
    }
    if (requested_p_max_eutra_present) {
      j.write_int("requestedP-MaxEUTRA", requested_p_max_eutra);
    }
    if (requested_p_max_fr2_r16_present) {
      j.write_int("requestedP-MaxFR2-r16", requested_p_max_fr2_r16);
    }
    if (requested_max_inter_freq_meas_id_scg_r16_present) {
      j.write_int("requestedMaxInterFreqMeasIdSCG-r16", requested_max_inter_freq_meas_id_scg_r16);
    }
    if (requested_max_intra_freq_meas_id_scg_r16_present) {
      j.write_int("requestedMaxIntraFreqMeasIdSCG-r16", requested_max_intra_freq_meas_id_scg_r16);
    }
    if (requested_toffset_r16_present) {
      j.write_str("requestedToffset-r16", requested_toffset_r16.to_string());
    }
  }
  j.end_obj();
}

// MeasConfigSN ::= SEQUENCE
SRSASN_CODE meas_cfg_sn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_frequencies_sn.size() > 0, 1));

  if (measured_frequencies_sn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, measured_frequencies_sn, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_sn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool measured_frequencies_sn_present;
  HANDLE_CODE(bref.unpack(measured_frequencies_sn_present, 1));

  if (measured_frequencies_sn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(measured_frequencies_sn, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void meas_cfg_sn_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_frequencies_sn.size() > 0) {
    j.start_array("measuredFrequenciesSN");
    for (const auto& e1 : measured_frequencies_sn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// CG-Config-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_cell_group_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_mod_req_present, 1));
  HANDLE_CODE(bref.pack(drx_info_scg_present, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_cfg_sn_present, 1));
  HANDLE_CODE(bref.pack(sel_band_combination_present, 1));
  HANDLE_CODE(bref.pack(fr_info_list_scg.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_serving_freq_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_cell_group_cfg.size() > 0) {
    HANDLE_CODE(scg_cell_group_cfg.pack(bref));
  }
  if (scg_rb_cfg.size() > 0) {
    HANDLE_CODE(scg_rb_cfg.pack(bref));
  }
  if (cfg_restrict_mod_req_present) {
    HANDLE_CODE(cfg_restrict_mod_req.pack(bref));
  }
  if (drx_info_scg_present) {
    HANDLE_CODE(drx_info_scg.pack(bref));
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn.pack(bref));
  }
  if (meas_cfg_sn_present) {
    HANDLE_CODE(meas_cfg_sn.pack(bref));
  }
  if (sel_band_combination_present) {
    HANDLE_CODE(sel_band_combination.pack(bref));
  }
  if (fr_info_list_scg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, fr_info_list_scg, 1, 31));
  }
  if (candidate_serving_freq_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_serving_freq_list_nr, 1, 32, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_ies_s::unpack(cbit_ref& bref)
{
  bool scg_cell_group_cfg_present;
  HANDLE_CODE(bref.unpack(scg_cell_group_cfg_present, 1));
  bool scg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(scg_rb_cfg_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_mod_req_present, 1));
  HANDLE_CODE(bref.unpack(drx_info_scg_present, 1));
  bool candidate_cell_info_list_sn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_sn_present, 1));
  HANDLE_CODE(bref.unpack(sel_band_combination_present, 1));
  bool fr_info_list_scg_present;
  HANDLE_CODE(bref.unpack(fr_info_list_scg_present, 1));
  bool candidate_serving_freq_list_nr_present;
  HANDLE_CODE(bref.unpack(candidate_serving_freq_list_nr_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_cell_group_cfg_present) {
    HANDLE_CODE(scg_cell_group_cfg.unpack(bref));
  }
  if (scg_rb_cfg_present) {
    HANDLE_CODE(scg_rb_cfg.unpack(bref));
  }
  if (cfg_restrict_mod_req_present) {
    HANDLE_CODE(cfg_restrict_mod_req.unpack(bref));
  }
  if (drx_info_scg_present) {
    HANDLE_CODE(drx_info_scg.unpack(bref));
  }
  if (candidate_cell_info_list_sn_present) {
    HANDLE_CODE(candidate_cell_info_list_sn.unpack(bref));
  }
  if (meas_cfg_sn_present) {
    HANDLE_CODE(meas_cfg_sn.unpack(bref));
  }
  if (sel_band_combination_present) {
    HANDLE_CODE(sel_band_combination.unpack(bref));
  }
  if (fr_info_list_scg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(fr_info_list_scg, bref, 1, 31));
  }
  if (candidate_serving_freq_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_serving_freq_list_nr, bref, 1, 32, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_cell_group_cfg.size() > 0) {
    j.write_str("scg-CellGroupConfig", scg_cell_group_cfg.to_string());
  }
  if (scg_rb_cfg.size() > 0) {
    j.write_str("scg-RB-Config", scg_rb_cfg.to_string());
  }
  if (cfg_restrict_mod_req_present) {
    j.write_fieldname("configRestrictModReq");
    cfg_restrict_mod_req.to_json(j);
  }
  if (drx_info_scg_present) {
    j.write_fieldname("drx-InfoSCG");
    drx_info_scg.to_json(j);
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    j.write_str("candidateCellInfoListSN", candidate_cell_info_list_sn.to_string());
  }
  if (meas_cfg_sn_present) {
    j.write_fieldname("measConfigSN");
    meas_cfg_sn.to_json(j);
  }
  if (sel_band_combination_present) {
    j.write_fieldname("selectedBandCombination");
    sel_band_combination.to_json(j);
  }
  if (fr_info_list_scg.size() > 0) {
    j.start_array("fr-InfoListSCG");
    for (const auto& e1 : fr_info_list_scg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (candidate_serving_freq_list_nr.size() > 0) {
    j.start_array("candidateServingFreqListNR");
    for (const auto& e1 : candidate_serving_freq_list_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config ::= SEQUENCE
SRSASN_CODE cg_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void cg_cfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_s::crit_exts_c_::c1_c_& cg_cfg_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_cfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void cg_cfg_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cg_cfg_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_ies_s& cg_cfg_s::crit_exts_c_::c1_c_::set_cg_cfg()
{
  set(types::cg_cfg);
  return c;
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void cg_cfg_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cg_cfg:
      j.write_fieldname("cg-Config");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cg_cfg:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cg_cfg:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cg_cfg_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"cg-Config", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "cg_cfg_s::crit_exts_c_::c1_c_::types");
}

const char* cg_cfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "cg_cfg_s::crit_exts_c_::types");
}
uint8_t cg_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_cfg_s::crit_exts_c_::types");
}

// HandoverCommand-IEs ::= SEQUENCE
SRSASN_CODE ho_cmd_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ho_cmd_msg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ho_cmd_msg.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_cmd_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("handoverCommandMessage", ho_cmd_msg.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// HandoverCommand ::= SEQUENCE
SRSASN_CODE ho_cmd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_cmd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_s::crit_exts_c_::c1_c_& ho_cmd_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ho_cmd_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_cmd_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_ies_s& ho_cmd_s::crit_exts_c_::c1_c_::set_ho_cmd()
{
  set(types::ho_cmd);
  return c;
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ho_cmd_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_cmd:
      j.write_fieldname("handoverCommand");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_cmd:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_cmd:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ho_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"handoverCommand", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ho_cmd_s::crit_exts_c_::c1_c_::types");
}

const char* ho_cmd_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ho_cmd_s::crit_exts_c_::types");
}
uint8_t ho_cmd_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ho_cmd_s::crit_exts_c_::types");
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

// SL-AccessInfo-L2U2N-r17 ::= SEQUENCE
SRSASN_CODE sl_access_info_l2_u2_n_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(cell_access_related_info_r17.pack(bref));
  HANDLE_CODE(sl_serving_cell_info_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_access_info_l2_u2_n_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(cell_access_related_info_r17.unpack(bref));
  HANDLE_CODE(sl_serving_cell_info_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_access_info_l2_u2_n_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellAccessRelatedInfo-r17");
  cell_access_related_info_r17.to_json(j);
  j.write_fieldname("sl-ServingCellInfo-r17");
  sl_serving_cell_info_r17.to_json(j);
  j.end_obj();
}

// UERadioAccessCapabilityInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ue-RadioAccessCapabilityInfo", ue_radio_access_cap_info.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UERadioAccessCapabilityInformation ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_access_cap_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_s::crit_exts_c_::c1_c_& ue_radio_access_cap_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_radio_access_cap_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_ies_s& ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_ue_radio_access_cap_info()
{
  set(types::ue_radio_access_cap_info);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_access_cap_info:
      j.write_fieldname("ueRadioAccessCapabilityInformation");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_access_cap_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_access_cap_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "ueRadioAccessCapabilityInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_access_cap_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_radio_access_cap_info_s::crit_exts_c_::types");
}
uint8_t ue_radio_access_cap_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_radio_access_cap_info_s::crit_exts_c_::types");
}

// UERadioPagingInformation-v1700-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_radio_paging_info_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(inactive_state_po_determination_r17_present, 1));
  HANDLE_CODE(bref.pack(nof_rx_red_cap_r17_present, 1));
  HANDLE_CODE(bref.pack(half_duplex_fdd_type_a_red_cap_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_radio_paging_info_r17.size() > 0) {
    HANDLE_CODE(ue_radio_paging_info_r17.pack(bref));
  }
  if (nof_rx_red_cap_r17_present) {
    HANDLE_CODE(nof_rx_red_cap_r17.pack(bref));
  }
  if (half_duplex_fdd_type_a_red_cap_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, half_duplex_fdd_type_a_red_cap_r17, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool ue_radio_paging_info_r17_present;
  HANDLE_CODE(bref.unpack(ue_radio_paging_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(inactive_state_po_determination_r17_present, 1));
  HANDLE_CODE(bref.unpack(nof_rx_red_cap_r17_present, 1));
  bool half_duplex_fdd_type_a_red_cap_r17_present;
  HANDLE_CODE(bref.unpack(half_duplex_fdd_type_a_red_cap_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_radio_paging_info_r17_present) {
    HANDLE_CODE(ue_radio_paging_info_r17.unpack(bref));
  }
  if (nof_rx_red_cap_r17_present) {
    HANDLE_CODE(nof_rx_red_cap_r17.unpack(bref));
  }
  if (half_duplex_fdd_type_a_red_cap_r17_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(half_duplex_fdd_type_a_red_cap_r17, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_radio_paging_info_r17.size() > 0) {
    j.write_str("ue-RadioPagingInfo-r17", ue_radio_paging_info_r17.to_string());
  }
  if (inactive_state_po_determination_r17_present) {
    j.write_str("inactiveStatePO-Determination-r17", "supported");
  }
  if (nof_rx_red_cap_r17_present) {
    j.write_str("numberOfRxRedCap-r17", nof_rx_red_cap_r17.to_string());
  }
  if (half_duplex_fdd_type_a_red_cap_r17.size() > 0) {
    j.start_array("halfDuplexFDD-TypeA-RedCap-r17");
    for (const auto& e1 : half_duplex_fdd_type_a_red_cap_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_opts::to_string() const
{
  static const char* names[] = {"one", "two"};
  return convert_enum_idx(names, 2, value, "ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_e_");
}
uint8_t ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_e_");
}

// UERadioPagingInformation-v15e0-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_v15e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_fdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_tdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_tdd_fr2_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_fdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_tdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_tdd_fr2_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_v15e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_fdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_tdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_tdd_fr2_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_fdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_tdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_tdd_fr2_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_v15e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_sched_offset_pdsch_type_a_fdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeA-FDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_a_tdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeA-TDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_a_tdd_fr2_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeA-TDD-FR2", "supported");
  }
  if (dl_sched_offset_pdsch_type_b_fdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeB-FDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_b_tdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeB-TDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_b_tdd_fr2_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeB-TDD-FR2", "supported");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioPagingInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_nr_for_paging.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (supported_band_list_nr_for_paging.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_nr_for_paging, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_ies_s::unpack(cbit_ref& bref)
{
  bool supported_band_list_nr_for_paging_present;
  HANDLE_CODE(bref.unpack(supported_band_list_nr_for_paging_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (supported_band_list_nr_for_paging_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_nr_for_paging, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_list_nr_for_paging.size() > 0) {
    j.start_array("supportedBandListNRForPaging");
    for (const auto& e1 : supported_band_list_nr_for_paging) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioPagingInformation ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_paging_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_s::crit_exts_c_::c1_c_& ue_radio_paging_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_radio_paging_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_ies_s& ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_ue_radio_paging_info()
{
  set(types::ue_radio_paging_info);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_paging_info:
      j.write_fieldname("ueRadioPagingInformation");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_paging_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_paging_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_radio_paging_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "ueRadioPagingInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ue_radio_paging_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_paging_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_radio_paging_info_s::crit_exts_c_::types");
}
uint8_t ue_radio_paging_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_radio_paging_info_s::crit_exts_c_::types");
}

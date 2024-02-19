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

#include "srsran/asn1/rrc_nr/radio_bearer_cfg.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// DiscardTimerExt-r16 ::= ENUMERATED
const char* discard_timer_ext_r16_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms1", "ms2", "ms4", "ms6", "ms8", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "discard_timer_ext_r16_e");
}
float discard_timer_ext_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 4.0, 6.0, 8.0};
  return map_enum_number(numbers, 6, value, "discard_timer_ext_r16_e");
}
const char* discard_timer_ext_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "4", "6", "8"};
  return convert_enum_idx(number_strs, 8, value, "discard_timer_ext_r16_e");
}

// DiscardTimerExt2-r17 ::= ENUMERATED
const char* discard_timer_ext2_r17_opts::to_string() const
{
  static const char* names[] = {"ms2000", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "discard_timer_ext2_r17_e");
}
uint16_t discard_timer_ext2_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {2000};
  return map_enum_number(numbers, 1, value, "discard_timer_ext2_r17_e");
}

// EthernetHeaderCompression-r16 ::= SEQUENCE
SRSASN_CODE ethernet_hdr_compress_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ehc_dl_r16_present, 1));
  HANDLE_CODE(bref.pack(ehc_ul_r16_present, 1));

  bref.pack(ehc_common_r16.ext, 1);
  HANDLE_CODE(ehc_common_r16.ehc_c_id_len_r16.pack(bref));
  if (ehc_dl_r16_present) {
    bref.pack(ehc_dl_r16.ext, 1);
    HANDLE_CODE(bref.pack(ehc_dl_r16.drb_continue_ehc_dl_r16_present, 1));
  }
  if (ehc_ul_r16_present) {
    bref.pack(ehc_ul_r16.ext, 1);
    HANDLE_CODE(bref.pack(ehc_ul_r16.drb_continue_ehc_ul_r16_present, 1));
    HANDLE_CODE(pack_integer(bref, ehc_ul_r16.max_c_id_ehc_ul_r16, (uint16_t)1u, (uint16_t)32767u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ethernet_hdr_compress_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ehc_dl_r16_present, 1));
  HANDLE_CODE(bref.unpack(ehc_ul_r16_present, 1));

  bref.unpack(ehc_common_r16.ext, 1);
  HANDLE_CODE(ehc_common_r16.ehc_c_id_len_r16.unpack(bref));
  if (ehc_dl_r16_present) {
    bref.unpack(ehc_dl_r16.ext, 1);
    HANDLE_CODE(bref.unpack(ehc_dl_r16.drb_continue_ehc_dl_r16_present, 1));
  }
  if (ehc_ul_r16_present) {
    bref.unpack(ehc_ul_r16.ext, 1);
    HANDLE_CODE(bref.unpack(ehc_ul_r16.drb_continue_ehc_ul_r16_present, 1));
    HANDLE_CODE(unpack_integer(ehc_ul_r16.max_c_id_ehc_ul_r16, bref, (uint16_t)1u, (uint16_t)32767u));
  }

  return SRSASN_SUCCESS;
}
void ethernet_hdr_compress_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ehc-Common-r16");
  j.start_obj();
  j.write_str("ehc-CID-Length-r16", ehc_common_r16.ehc_c_id_len_r16.to_string());
  j.end_obj();
  if (ehc_dl_r16_present) {
    j.write_fieldname("ehc-Downlink-r16");
    j.start_obj();
    if (ehc_dl_r16.drb_continue_ehc_dl_r16_present) {
      j.write_str("drb-ContinueEHC-DL-r16", "true");
    }
    j.end_obj();
  }
  if (ehc_ul_r16_present) {
    j.write_fieldname("ehc-Uplink-r16");
    j.start_obj();
    j.write_int("maxCID-EHC-UL-r16", ehc_ul_r16.max_c_id_ehc_ul_r16);
    if (ehc_ul_r16.drb_continue_ehc_ul_r16_present) {
      j.write_str("drb-ContinueEHC-UL-r16", "true");
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_opts::to_string() const
{
  static const char* names[] = {"bits7", "bits15"};
  return convert_enum_idx(names, 2, value, "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_e_");
}
uint8_t ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 15};
  return map_enum_number(numbers, 2, value, "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_e_");
}

// UL-DataSplitThreshold ::= ENUMERATED
const char* ul_data_split_thres_opts::to_string() const
{
  static const char* names[] = {"b0",       "b100",     "b200",     "b400",     "b800",     "b1600",    "b3200",
                                "b6400",    "b12800",   "b25600",   "b51200",   "b102400",  "b204800",  "b409600",
                                "b819200",  "b1228800", "b1638400", "b2457600", "b3276800", "b4096000", "b4915200",
                                "b5734400", "b6553600", "infinity", "spare8",   "spare7",   "spare6",   "spare5",
                                "spare4",   "spare3",   "spare2",   "spare1"};
  return convert_enum_idx(names, 32, value, "ul_data_split_thres_e");
}
int32_t ul_data_split_thres_opts::to_number() const
{
  static const int32_t numbers[] = {0,       100,     200,     400,     800,     1600,    3200,    6400,
                                    12800,   25600,   51200,   102400,  204800,  409600,  819200,  1228800,
                                    1638400, 2457600, 3276800, 4096000, 4915200, 5734400, 6553600, -1};
  return map_enum_number(numbers, 24, value, "ul_data_split_thres_e");
}

// UplinkDataCompression-r17 ::= CHOICE
void ul_data_compress_r17_c::set(types::options e)
{
  type_ = e;
}
ul_data_compress_r17_c::new_setup_s_& ul_data_compress_r17_c::set_new_setup()
{
  set(types::new_setup);
  return c;
}
void ul_data_compress_r17_c::set_drb_continue_udc()
{
  set(types::drb_continue_udc);
}
void ul_data_compress_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::new_setup:
      j.write_fieldname("newSetup");
      j.start_obj();
      j.write_str("bufferSize-r17", c.buffer_size_r17.to_string());
      if (c.dictionary_r17_present) {
        j.write_str("dictionary-r17", c.dictionary_r17.to_string());
      }
      j.end_obj();
      break;
    case types::drb_continue_udc:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_compress_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_data_compress_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::new_setup:
      HANDLE_CODE(bref.pack(c.dictionary_r17_present, 1));
      HANDLE_CODE(c.buffer_size_r17.pack(bref));
      if (c.dictionary_r17_present) {
        HANDLE_CODE(c.dictionary_r17.pack(bref));
      }
      break;
    case types::drb_continue_udc:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_compress_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_data_compress_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::new_setup:
      HANDLE_CODE(bref.unpack(c.dictionary_r17_present, 1));
      HANDLE_CODE(c.buffer_size_r17.unpack(bref));
      if (c.dictionary_r17_present) {
        HANDLE_CODE(c.dictionary_r17.unpack(bref));
      }
      break;
    case types::drb_continue_udc:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_compress_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_opts::to_string() const
{
  static const char* names[] = {"kbyte2", "kbyte4", "kbyte8", "spare1"};
  return convert_enum_idx(names, 4, value, "ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_e_");
}
uint8_t ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_e_");
}

const char* ul_data_compress_r17_c::new_setup_s_::dictionary_r17_opts::to_string() const
{
  static const char* names[] = {"sip-SDP", "operator"};
  return convert_enum_idx(names, 2, value, "ul_data_compress_r17_c::new_setup_s_::dictionary_r17_e_");
}

const char* ul_data_compress_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"newSetup", "drb-ContinueUDC"};
  return convert_enum_idx(names, 2, value, "ul_data_compress_r17_c::types");
}

// CipheringAlgorithm ::= ENUMERATED
const char* ciphering_algorithm_opts::to_string() const
{
  static const char* names[] = {"nea0", "nea1", "nea2", "nea3", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ciphering_algorithm_e");
}
uint8_t ciphering_algorithm_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "ciphering_algorithm_e");
}

// IntegrityProtAlgorithm ::= ENUMERATED
const char* integrity_prot_algorithm_opts::to_string() const
{
  static const char* names[] = {"nia0", "nia1", "nia2", "nia3", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "integrity_prot_algorithm_e");
}
uint8_t integrity_prot_algorithm_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "integrity_prot_algorithm_e");
}

// PDCP-Config ::= SEQUENCE
SRSASN_CODE pdcp_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_present, 1));
  HANDLE_CODE(bref.pack(more_than_one_rlc_present, 1));
  HANDLE_CODE(bref.pack(t_reordering_present, 1));

  if (drb_present) {
    HANDLE_CODE(bref.pack(drb.discard_timer_present, 1));
    HANDLE_CODE(bref.pack(drb.pdcp_sn_size_ul_present, 1));
    HANDLE_CODE(bref.pack(drb.pdcp_sn_size_dl_present, 1));
    HANDLE_CODE(bref.pack(drb.integrity_protection_present, 1));
    HANDLE_CODE(bref.pack(drb.status_report_required_present, 1));
    HANDLE_CODE(bref.pack(drb.out_of_order_delivery_present, 1));
    if (drb.discard_timer_present) {
      HANDLE_CODE(drb.discard_timer.pack(bref));
    }
    if (drb.pdcp_sn_size_ul_present) {
      HANDLE_CODE(drb.pdcp_sn_size_ul.pack(bref));
    }
    if (drb.pdcp_sn_size_dl_present) {
      HANDLE_CODE(drb.pdcp_sn_size_dl.pack(bref));
    }
    HANDLE_CODE(drb.hdr_compress.pack(bref));
  }
  if (more_than_one_rlc_present) {
    HANDLE_CODE(bref.pack(more_than_one_rlc.ul_data_split_thres_present, 1));
    HANDLE_CODE(bref.pack(more_than_one_rlc.pdcp_dupl_present, 1));
    HANDLE_CODE(bref.pack(more_than_one_rlc.primary_path.cell_group_present, 1));
    HANDLE_CODE(bref.pack(more_than_one_rlc.primary_path.lc_ch_present, 1));
    if (more_than_one_rlc.primary_path.cell_group_present) {
      HANDLE_CODE(pack_integer(bref, more_than_one_rlc.primary_path.cell_group, (uint8_t)0u, (uint8_t)3u));
    }
    if (more_than_one_rlc.primary_path.lc_ch_present) {
      HANDLE_CODE(pack_integer(bref, more_than_one_rlc.primary_path.lc_ch, (uint8_t)1u, (uint8_t)32u));
    }
    if (more_than_one_rlc.ul_data_split_thres_present) {
      HANDLE_CODE(more_than_one_rlc.ul_data_split_thres.pack(bref));
    }
    if (more_than_one_rlc.pdcp_dupl_present) {
      HANDLE_CODE(bref.pack(more_than_one_rlc.pdcp_dupl, 1));
    }
  }
  if (t_reordering_present) {
    HANDLE_CODE(t_reordering.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ciphering_disabled_present;
    group_flags[1] |= discard_timer_ext_r16.is_present();
    group_flags[1] |= more_than_two_rlc_drb_r16.is_present();
    group_flags[1] |= ethernet_hdr_compress_r16.is_present();
    group_flags[2] |= survival_time_state_support_r17_present;
    group_flags[2] |= ul_data_compress_r17.is_present();
    group_flags[2] |= discard_timer_ext2_r17.is_present();
    group_flags[2] |= init_rx_deliv_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ciphering_disabled_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(discard_timer_ext_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(more_than_two_rlc_drb_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ethernet_hdr_compress_r16.is_present(), 1));
      if (discard_timer_ext_r16.is_present()) {
        HANDLE_CODE(discard_timer_ext_r16->pack(bref));
      }
      if (more_than_two_rlc_drb_r16.is_present()) {
        HANDLE_CODE(bref.pack(more_than_two_rlc_drb_r16->split_secondary_path_r16_present, 1));
        HANDLE_CODE(bref.pack(more_than_two_rlc_drb_r16->dupl_state_r16_present, 1));
        if (more_than_two_rlc_drb_r16->split_secondary_path_r16_present) {
          HANDLE_CODE(
              pack_integer(bref, more_than_two_rlc_drb_r16->split_secondary_path_r16, (uint8_t)1u, (uint8_t)32u));
        }
        if (more_than_two_rlc_drb_r16->dupl_state_r16_present) {
          HANDLE_CODE(pack_fixed_seq_of(bref,
                                        more_than_two_rlc_drb_r16->dupl_state_r16,
                                        more_than_two_rlc_drb_r16->dupl_state_r16.size(),
                                        BitPacker(1)));
        }
      }
      if (ethernet_hdr_compress_r16.is_present()) {
        HANDLE_CODE(ethernet_hdr_compress_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(survival_time_state_support_r17_present, 1));
      HANDLE_CODE(bref.pack(ul_data_compress_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(discard_timer_ext2_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(init_rx_deliv_r17_present, 1));
      if (ul_data_compress_r17.is_present()) {
        HANDLE_CODE(ul_data_compress_r17->pack(bref));
      }
      if (discard_timer_ext2_r17.is_present()) {
        HANDLE_CODE(discard_timer_ext2_r17->pack(bref));
      }
      if (init_rx_deliv_r17_present) {
        HANDLE_CODE(init_rx_deliv_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_present, 1));
  HANDLE_CODE(bref.unpack(more_than_one_rlc_present, 1));
  HANDLE_CODE(bref.unpack(t_reordering_present, 1));

  if (drb_present) {
    HANDLE_CODE(bref.unpack(drb.discard_timer_present, 1));
    HANDLE_CODE(bref.unpack(drb.pdcp_sn_size_ul_present, 1));
    HANDLE_CODE(bref.unpack(drb.pdcp_sn_size_dl_present, 1));
    HANDLE_CODE(bref.unpack(drb.integrity_protection_present, 1));
    HANDLE_CODE(bref.unpack(drb.status_report_required_present, 1));
    HANDLE_CODE(bref.unpack(drb.out_of_order_delivery_present, 1));
    if (drb.discard_timer_present) {
      HANDLE_CODE(drb.discard_timer.unpack(bref));
    }
    if (drb.pdcp_sn_size_ul_present) {
      HANDLE_CODE(drb.pdcp_sn_size_ul.unpack(bref));
    }
    if (drb.pdcp_sn_size_dl_present) {
      HANDLE_CODE(drb.pdcp_sn_size_dl.unpack(bref));
    }
    HANDLE_CODE(drb.hdr_compress.unpack(bref));
  }
  if (more_than_one_rlc_present) {
    HANDLE_CODE(bref.unpack(more_than_one_rlc.ul_data_split_thres_present, 1));
    HANDLE_CODE(bref.unpack(more_than_one_rlc.pdcp_dupl_present, 1));
    HANDLE_CODE(bref.unpack(more_than_one_rlc.primary_path.cell_group_present, 1));
    HANDLE_CODE(bref.unpack(more_than_one_rlc.primary_path.lc_ch_present, 1));
    if (more_than_one_rlc.primary_path.cell_group_present) {
      HANDLE_CODE(unpack_integer(more_than_one_rlc.primary_path.cell_group, bref, (uint8_t)0u, (uint8_t)3u));
    }
    if (more_than_one_rlc.primary_path.lc_ch_present) {
      HANDLE_CODE(unpack_integer(more_than_one_rlc.primary_path.lc_ch, bref, (uint8_t)1u, (uint8_t)32u));
    }
    if (more_than_one_rlc.ul_data_split_thres_present) {
      HANDLE_CODE(more_than_one_rlc.ul_data_split_thres.unpack(bref));
    }
    if (more_than_one_rlc.pdcp_dupl_present) {
      HANDLE_CODE(bref.unpack(more_than_one_rlc.pdcp_dupl, 1));
    }
  }
  if (t_reordering_present) {
    HANDLE_CODE(t_reordering.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ciphering_disabled_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(discard_timer_ext_r16, bref);
      unpack_presence_flag(more_than_two_rlc_drb_r16, bref);
      unpack_presence_flag(ethernet_hdr_compress_r16, bref);
      if (discard_timer_ext_r16.is_present()) {
        HANDLE_CODE(discard_timer_ext_r16->unpack(bref));
      }
      if (more_than_two_rlc_drb_r16.is_present()) {
        HANDLE_CODE(bref.unpack(more_than_two_rlc_drb_r16->split_secondary_path_r16_present, 1));
        HANDLE_CODE(bref.unpack(more_than_two_rlc_drb_r16->dupl_state_r16_present, 1));
        if (more_than_two_rlc_drb_r16->split_secondary_path_r16_present) {
          HANDLE_CODE(
              unpack_integer(more_than_two_rlc_drb_r16->split_secondary_path_r16, bref, (uint8_t)1u, (uint8_t)32u));
        }
        if (more_than_two_rlc_drb_r16->dupl_state_r16_present) {
          HANDLE_CODE(unpack_fixed_seq_of(more_than_two_rlc_drb_r16->dupl_state_r16,
                                          bref,
                                          more_than_two_rlc_drb_r16->dupl_state_r16.size(),
                                          BitPacker(1)));
        }
      }
      if (ethernet_hdr_compress_r16.is_present()) {
        HANDLE_CODE(ethernet_hdr_compress_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(survival_time_state_support_r17_present, 1));
      unpack_presence_flag(ul_data_compress_r17, bref);
      unpack_presence_flag(discard_timer_ext2_r17, bref);
      HANDLE_CODE(bref.unpack(init_rx_deliv_r17_present, 1));
      if (ul_data_compress_r17.is_present()) {
        HANDLE_CODE(ul_data_compress_r17->unpack(bref));
      }
      if (discard_timer_ext2_r17.is_present()) {
        HANDLE_CODE(discard_timer_ext2_r17->unpack(bref));
      }
      if (init_rx_deliv_r17_present) {
        HANDLE_CODE(init_rx_deliv_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pdcp_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_present) {
    j.write_fieldname("drb");
    j.start_obj();
    if (drb.discard_timer_present) {
      j.write_str("discardTimer", drb.discard_timer.to_string());
    }
    if (drb.pdcp_sn_size_ul_present) {
      j.write_str("pdcp-SN-SizeUL", drb.pdcp_sn_size_ul.to_string());
    }
    if (drb.pdcp_sn_size_dl_present) {
      j.write_str("pdcp-SN-SizeDL", drb.pdcp_sn_size_dl.to_string());
    }
    j.write_fieldname("headerCompression");
    drb.hdr_compress.to_json(j);
    if (drb.integrity_protection_present) {
      j.write_str("integrityProtection", "enabled");
    }
    if (drb.status_report_required_present) {
      j.write_str("statusReportRequired", "true");
    }
    if (drb.out_of_order_delivery_present) {
      j.write_str("outOfOrderDelivery", "true");
    }
    j.end_obj();
  }
  if (more_than_one_rlc_present) {
    j.write_fieldname("moreThanOneRLC");
    j.start_obj();
    j.write_fieldname("primaryPath");
    j.start_obj();
    if (more_than_one_rlc.primary_path.cell_group_present) {
      j.write_int("cellGroup", more_than_one_rlc.primary_path.cell_group);
    }
    if (more_than_one_rlc.primary_path.lc_ch_present) {
      j.write_int("logicalChannel", more_than_one_rlc.primary_path.lc_ch);
    }
    j.end_obj();
    if (more_than_one_rlc.ul_data_split_thres_present) {
      j.write_str("ul-DataSplitThreshold", more_than_one_rlc.ul_data_split_thres.to_string());
    }
    if (more_than_one_rlc.pdcp_dupl_present) {
      j.write_bool("pdcp-Duplication", more_than_one_rlc.pdcp_dupl);
    }
    j.end_obj();
  }
  if (t_reordering_present) {
    j.write_str("t-Reordering", t_reordering.to_string());
  }
  if (ext) {
    if (ciphering_disabled_present) {
      j.write_str("cipheringDisabled", "true");
    }
    if (discard_timer_ext_r16.is_present()) {
      j.write_fieldname("discardTimerExt-r16");
      discard_timer_ext_r16->to_json(j);
    }
    if (more_than_two_rlc_drb_r16.is_present()) {
      j.write_fieldname("moreThanTwoRLC-DRB-r16");
      j.start_obj();
      if (more_than_two_rlc_drb_r16->split_secondary_path_r16_present) {
        j.write_int("splitSecondaryPath-r16", more_than_two_rlc_drb_r16->split_secondary_path_r16);
      }
      if (more_than_two_rlc_drb_r16->dupl_state_r16_present) {
        j.start_array("duplicationState-r16");
        for (const auto& e1 : more_than_two_rlc_drb_r16->dupl_state_r16) {
          j.write_bool(e1);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (ethernet_hdr_compress_r16.is_present()) {
      j.write_fieldname("ethernetHeaderCompression-r16");
      ethernet_hdr_compress_r16->to_json(j);
    }
    if (survival_time_state_support_r17_present) {
      j.write_str("survivalTimeStateSupport-r17", "true");
    }
    if (ul_data_compress_r17.is_present()) {
      j.write_fieldname("uplinkDataCompression-r17");
      ul_data_compress_r17->to_json(j);
    }
    if (discard_timer_ext2_r17.is_present()) {
      j.write_fieldname("discardTimerExt2-r17");
      discard_timer_ext2_r17->to_json(j);
    }
    if (init_rx_deliv_r17_present) {
      j.write_str("initialRX-DELIV-r17", init_rx_deliv_r17.to_string());
    }
  }
  j.end_obj();
}

const char* pdcp_cfg_s::drb_s_::discard_timer_opts::to_string() const
{
  static const char* names[] = {"ms10",
                                "ms20",
                                "ms30",
                                "ms40",
                                "ms50",
                                "ms60",
                                "ms75",
                                "ms100",
                                "ms150",
                                "ms200",
                                "ms250",
                                "ms300",
                                "ms500",
                                "ms750",
                                "ms1500",
                                "infinity"};
  return convert_enum_idx(names, 16, value, "pdcp_cfg_s::drb_s_::discard_timer_e_");
}
int16_t pdcp_cfg_s::drb_s_::discard_timer_opts::to_number() const
{
  static const int16_t numbers[] = {10, 20, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(numbers, 16, value, "pdcp_cfg_s::drb_s_::discard_timer_e_");
}

const char* pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::to_string() const
{
  static const char* names[] = {"len12bits", "len18bits"};
  return convert_enum_idx(names, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_");
}
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_");
}

const char* pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::to_string() const
{
  static const char* names[] = {"len12bits", "len18bits"};
  return convert_enum_idx(names, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_");
}
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_");
}

void pdcp_cfg_s::drb_s_::hdr_compress_c_::destroy_()
{
  switch (type_) {
    case types::rohc:
      c.destroy<rohc_s_>();
      break;
    case types::ul_only_rohc:
      c.destroy<ul_only_rohc_s_>();
      break;
    default:
      break;
  }
}
void pdcp_cfg_s::drb_s_::hdr_compress_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      c.init<rohc_s_>();
      break;
    case types::ul_only_rohc:
      c.init<ul_only_rohc_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::hdr_compress_c_(const pdcp_cfg_s::drb_s_::hdr_compress_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      c.init(other.c.get<rohc_s_>());
      break;
    case types::ul_only_rohc:
      c.init(other.c.get<ul_only_rohc_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }
}
pdcp_cfg_s::drb_s_::hdr_compress_c_&
pdcp_cfg_s::drb_s_::hdr_compress_c_::operator=(const pdcp_cfg_s::drb_s_::hdr_compress_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      c.set(other.c.get<rohc_s_>());
      break;
    case types::ul_only_rohc:
      c.set(other.c.get<ul_only_rohc_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }

  return *this;
}
void pdcp_cfg_s::drb_s_::hdr_compress_c_::set_not_used()
{
  set(types::not_used);
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_& pdcp_cfg_s::drb_s_::hdr_compress_c_::set_rohc()
{
  set(types::rohc);
  return c.get<rohc_s_>();
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_& pdcp_cfg_s::drb_s_::hdr_compress_c_::set_ul_only_rohc()
{
  set(types::ul_only_rohc);
  return c.get<ul_only_rohc_s_>();
}
void pdcp_cfg_s::drb_s_::hdr_compress_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      j.write_fieldname("rohc");
      j.start_obj();
      if (c.get<rohc_s_>().max_c_id_present) {
        j.write_int("maxCID", c.get<rohc_s_>().max_c_id);
      }
      j.write_fieldname("profiles");
      j.start_obj();
      j.write_bool("profile0x0001", c.get<rohc_s_>().profiles.profile0x0001);
      j.write_bool("profile0x0002", c.get<rohc_s_>().profiles.profile0x0002);
      j.write_bool("profile0x0003", c.get<rohc_s_>().profiles.profile0x0003);
      j.write_bool("profile0x0004", c.get<rohc_s_>().profiles.profile0x0004);
      j.write_bool("profile0x0006", c.get<rohc_s_>().profiles.profile0x0006);
      j.write_bool("profile0x0101", c.get<rohc_s_>().profiles.profile0x0101);
      j.write_bool("profile0x0102", c.get<rohc_s_>().profiles.profile0x0102);
      j.write_bool("profile0x0103", c.get<rohc_s_>().profiles.profile0x0103);
      j.write_bool("profile0x0104", c.get<rohc_s_>().profiles.profile0x0104);
      j.end_obj();
      if (c.get<rohc_s_>().drb_continue_rohc_present) {
        j.write_str("drb-ContinueROHC", "true");
      }
      j.end_obj();
      break;
    case types::ul_only_rohc:
      j.write_fieldname("uplinkOnlyROHC");
      j.start_obj();
      if (c.get<ul_only_rohc_s_>().max_c_id_present) {
        j.write_int("maxCID", c.get<ul_only_rohc_s_>().max_c_id);
      }
      j.write_fieldname("profiles");
      j.start_obj();
      j.write_bool("profile0x0006", c.get<ul_only_rohc_s_>().profiles.profile0x0006);
      j.end_obj();
      if (c.get<ul_only_rohc_s_>().drb_continue_rohc_present) {
        j.write_str("drb-ContinueROHC", "true");
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::drb_s_::hdr_compress_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<rohc_s_>().max_c_id_present) {
        HANDLE_CODE(pack_integer(bref, c.get<rohc_s_>().max_c_id, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0001, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0002, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0003, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0004, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0006, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0101, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0102, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0103, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().profiles.profile0x0104, 1));
      break;
    case types::ul_only_rohc:
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<ul_only_rohc_s_>().max_c_id_present) {
        HANDLE_CODE(pack_integer(bref, c.get<ul_only_rohc_s_>().max_c_id, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().profiles.profile0x0006, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::drb_s_::hdr_compress_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<rohc_s_>().max_c_id_present) {
        HANDLE_CODE(unpack_integer(c.get<rohc_s_>().max_c_id, bref, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0001, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0002, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0003, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0004, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0006, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0101, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0102, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0103, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().profiles.profile0x0104, 1));
      break;
    case types::ul_only_rohc:
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().max_c_id_present, 1));
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<ul_only_rohc_s_>().max_c_id_present) {
        HANDLE_CODE(unpack_integer(c.get<ul_only_rohc_s_>().max_c_id, bref, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().profiles.profile0x0006, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::drb_s_::hdr_compress_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdcp_cfg_s::drb_s_::hdr_compress_c_::types_opts::to_string() const
{
  static const char* names[] = {"notUsed", "rohc", "uplinkOnlyROHC"};
  return convert_enum_idx(names, 3, value, "pdcp_cfg_s::drb_s_::hdr_compress_c_::types");
}

const char* pdcp_cfg_s::t_reordering_opts::to_string() const
{
  static const char* names[] = {"ms0",     "ms1",     "ms2",     "ms4",     "ms5",     "ms8",     "ms10",    "ms15",
                                "ms20",    "ms30",    "ms40",    "ms50",    "ms60",    "ms80",    "ms100",   "ms120",
                                "ms140",   "ms160",   "ms180",   "ms200",   "ms220",   "ms240",   "ms260",   "ms280",
                                "ms300",   "ms500",   "ms750",   "ms1000",  "ms1250",  "ms1500",  "ms1750",  "ms2000",
                                "ms2250",  "ms2500",  "ms2750",  "ms3000",  "spare28", "spare27", "spare26", "spare25",
                                "spare24", "spare23", "spare22", "spare21", "spare20", "spare19", "spare18", "spare17",
                                "spare16", "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare09",
                                "spare08", "spare07", "spare06", "spare05", "spare04", "spare03", "spare02", "spare01"};
  return convert_enum_idx(names, 64, value, "pdcp_cfg_s::t_reordering_e_");
}
uint16_t pdcp_cfg_s::t_reordering_opts::to_number() const
{
  static const uint16_t numbers[] = {0,   1,   2,   4,    5,    8,    10,   15,   20,   30,   40,   50,
                                     60,  80,  100, 120,  140,  160,  180,  200,  220,  240,  260,  280,
                                     300, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000};
  return map_enum_number(numbers, 36, value, "pdcp_cfg_s::t_reordering_e_");
}

// SDAP-Config ::= SEQUENCE
SRSASN_CODE sdap_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mapped_qos_flows_to_add.size() > 0, 1));
  HANDLE_CODE(bref.pack(mapped_qos_flows_to_release.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(sdap_hdr_dl.pack(bref));
  HANDLE_CODE(sdap_hdr_ul.pack(bref));
  HANDLE_CODE(bref.pack(default_drb, 1));
  if (mapped_qos_flows_to_add.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mapped_qos_flows_to_add, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (mapped_qos_flows_to_release.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mapped_qos_flows_to_release, 1, 64, integer_packer<uint8_t>(0, 63)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdap_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool mapped_qos_flows_to_add_present;
  HANDLE_CODE(bref.unpack(mapped_qos_flows_to_add_present, 1));
  bool mapped_qos_flows_to_release_present;
  HANDLE_CODE(bref.unpack(mapped_qos_flows_to_release_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(sdap_hdr_dl.unpack(bref));
  HANDLE_CODE(sdap_hdr_ul.unpack(bref));
  HANDLE_CODE(bref.unpack(default_drb, 1));
  if (mapped_qos_flows_to_add_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mapped_qos_flows_to_add, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (mapped_qos_flows_to_release_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mapped_qos_flows_to_release, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }

  return SRSASN_SUCCESS;
}
void sdap_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdu-Session", pdu_session);
  j.write_str("sdap-HeaderDL", sdap_hdr_dl.to_string());
  j.write_str("sdap-HeaderUL", sdap_hdr_ul.to_string());
  j.write_bool("defaultDRB", default_drb);
  if (mapped_qos_flows_to_add.size() > 0) {
    j.start_array("mappedQoS-FlowsToAdd");
    for (const auto& e1 : mapped_qos_flows_to_add) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mapped_qos_flows_to_release.size() > 0) {
    j.start_array("mappedQoS-FlowsToRelease");
    for (const auto& e1 : mapped_qos_flows_to_release) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* sdap_cfg_s::sdap_hdr_dl_opts::to_string() const
{
  static const char* names[] = {"present", "absent"};
  return convert_enum_idx(names, 2, value, "sdap_cfg_s::sdap_hdr_dl_e_");
}

const char* sdap_cfg_s::sdap_hdr_ul_opts::to_string() const
{
  static const char* names[] = {"present", "absent"};
  return convert_enum_idx(names, 2, value, "sdap_cfg_s::sdap_hdr_ul_e_");
}

// TMGI-r17 ::= SEQUENCE
SRSASN_CODE tmgi_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r17.pack(bref));
  HANDLE_CODE(service_id_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tmgi_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r17.unpack(bref));
  HANDLE_CODE(service_id_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void tmgi_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Id-r17");
  plmn_id_r17.to_json(j);
  j.write_str("serviceId-r17", service_id_r17.to_string());
  j.end_obj();
}

void tmgi_r17_s::plmn_id_r17_c_::destroy_()
{
  switch (type_) {
    case types::explicit_value:
      c.destroy<plmn_id_s>();
      break;
    default:
      break;
  }
}
void tmgi_r17_s::plmn_id_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_idx:
      break;
    case types::explicit_value:
      c.init<plmn_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }
}
tmgi_r17_s::plmn_id_r17_c_::plmn_id_r17_c_(const tmgi_r17_s::plmn_id_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::explicit_value:
      c.init(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }
}
tmgi_r17_s::plmn_id_r17_c_& tmgi_r17_s::plmn_id_r17_c_::operator=(const tmgi_r17_s::plmn_id_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::explicit_value:
      c.set(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }

  return *this;
}
uint8_t& tmgi_r17_s::plmn_id_r17_c_::set_plmn_idx()
{
  set(types::plmn_idx);
  return c.get<uint8_t>();
}
plmn_id_s& tmgi_r17_s::plmn_id_r17_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c.get<plmn_id_s>();
}
void tmgi_r17_s::plmn_id_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_idx:
      j.write_int("plmn-Index", c.get<uint8_t>());
      break;
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.get<plmn_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE tmgi_r17_s::plmn_id_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)12u));
      break;
    case types::explicit_value:
      HANDLE_CODE(c.get<plmn_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tmgi_r17_s::plmn_id_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)12u));
      break;
    case types::explicit_value:
      HANDLE_CODE(c.get<plmn_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tmgi_r17_s::plmn_id_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* tmgi_r17_s::plmn_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"plmn-Index", "explicitValue"};
  return convert_enum_idx(names, 2, value, "tmgi_r17_s::plmn_id_r17_c_::types");
}

// DRB-ToAddMod ::= SEQUENCE
SRSASN_CODE drb_to_add_mod_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cn_assoc_present, 1));
  HANDLE_CODE(bref.pack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.pack(recover_pdcp_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_present, 1));

  if (cn_assoc_present) {
    HANDLE_CODE(cn_assoc.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= daps_cfg_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(daps_cfg_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cn_assoc_present, 1));
  HANDLE_CODE(bref.unpack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(recover_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_present, 1));

  if (cn_assoc_present) {
    HANDLE_CODE(cn_assoc.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(daps_cfg_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void drb_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cn_assoc_present) {
    j.write_fieldname("cnAssociation");
    cn_assoc.to_json(j);
  }
  j.write_int("drb-Identity", drb_id);
  if (reestablish_pdcp_present) {
    j.write_str("reestablishPDCP", "true");
  }
  if (recover_pdcp_present) {
    j.write_str("recoverPDCP", "true");
  }
  if (pdcp_cfg_present) {
    j.write_fieldname("pdcp-Config");
    pdcp_cfg.to_json(j);
  }
  if (ext) {
    if (daps_cfg_r16_present) {
      j.write_str("daps-Config-r16", "true");
    }
  }
  j.end_obj();
}

void drb_to_add_mod_s::cn_assoc_c_::destroy_()
{
  switch (type_) {
    case types::sdap_cfg:
      c.destroy<sdap_cfg_s>();
      break;
    default:
      break;
  }
}
void drb_to_add_mod_s::cn_assoc_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eps_bearer_id:
      break;
    case types::sdap_cfg:
      c.init<sdap_cfg_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }
}
drb_to_add_mod_s::cn_assoc_c_::cn_assoc_c_(const drb_to_add_mod_s::cn_assoc_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eps_bearer_id:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sdap_cfg:
      c.init(other.c.get<sdap_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }
}
drb_to_add_mod_s::cn_assoc_c_& drb_to_add_mod_s::cn_assoc_c_::operator=(const drb_to_add_mod_s::cn_assoc_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eps_bearer_id:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sdap_cfg:
      c.set(other.c.get<sdap_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }

  return *this;
}
uint8_t& drb_to_add_mod_s::cn_assoc_c_::set_eps_bearer_id()
{
  set(types::eps_bearer_id);
  return c.get<uint8_t>();
}
sdap_cfg_s& drb_to_add_mod_s::cn_assoc_c_::set_sdap_cfg()
{
  set(types::sdap_cfg);
  return c.get<sdap_cfg_s>();
}
void drb_to_add_mod_s::cn_assoc_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eps_bearer_id:
      j.write_int("eps-BearerIdentity", c.get<uint8_t>());
      break;
    case types::sdap_cfg:
      j.write_fieldname("sdap-Config");
      c.get<sdap_cfg_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
  }
  j.end_obj();
}
SRSASN_CODE drb_to_add_mod_s::cn_assoc_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eps_bearer_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sdap_cfg:
      HANDLE_CODE(c.get<sdap_cfg_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_s::cn_assoc_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eps_bearer_id:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sdap_cfg:
      HANDLE_CODE(c.get<sdap_cfg_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_s::cn_assoc_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drb_to_add_mod_s::cn_assoc_c_::types_opts::to_string() const
{
  static const char* names[] = {"eps-BearerIdentity", "sdap-Config"};
  return convert_enum_idx(names, 2, value, "drb_to_add_mod_s::cn_assoc_c_::types");
}

// MRB-ToAddMod-r17 ::= SEQUENCE
SRSASN_CODE mrb_to_add_mod_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbs_session_id_r17_present, 1));
  HANDLE_CODE(bref.pack(mrb_id_new_r17_present, 1));
  HANDLE_CODE(bref.pack(reestablish_pdcp_r17_present, 1));
  HANDLE_CODE(bref.pack(recover_pdcp_r17_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_r17_present, 1));

  if (mbs_session_id_r17_present) {
    HANDLE_CODE(mbs_session_id_r17.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, mrb_id_r17, (uint16_t)1u, (uint16_t)512u));
  if (mrb_id_new_r17_present) {
    HANDLE_CODE(pack_integer(bref, mrb_id_new_r17, (uint16_t)1u, (uint16_t)512u));
  }
  if (pdcp_cfg_r17_present) {
    HANDLE_CODE(pdcp_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_to_add_mod_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mbs_session_id_r17_present, 1));
  HANDLE_CODE(bref.unpack(mrb_id_new_r17_present, 1));
  HANDLE_CODE(bref.unpack(reestablish_pdcp_r17_present, 1));
  HANDLE_CODE(bref.unpack(recover_pdcp_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_r17_present, 1));

  if (mbs_session_id_r17_present) {
    HANDLE_CODE(mbs_session_id_r17.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(mrb_id_r17, bref, (uint16_t)1u, (uint16_t)512u));
  if (mrb_id_new_r17_present) {
    HANDLE_CODE(unpack_integer(mrb_id_new_r17, bref, (uint16_t)1u, (uint16_t)512u));
  }
  if (pdcp_cfg_r17_present) {
    HANDLE_CODE(pdcp_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrb_to_add_mod_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbs_session_id_r17_present) {
    j.write_fieldname("mbs-SessionId-r17");
    mbs_session_id_r17.to_json(j);
  }
  j.write_int("mrb-Identity-r17", mrb_id_r17);
  if (mrb_id_new_r17_present) {
    j.write_int("mrb-IdentityNew-r17", mrb_id_new_r17);
  }
  if (reestablish_pdcp_r17_present) {
    j.write_str("reestablishPDCP-r17", "true");
  }
  if (recover_pdcp_r17_present) {
    j.write_str("recoverPDCP-r17", "true");
  }
  if (pdcp_cfg_r17_present) {
    j.write_fieldname("pdcp-Config-r17");
    pdcp_cfg_r17.to_json(j);
  }
  j.end_obj();
}

// SRB-ToAddMod ::= SEQUENCE
SRSASN_CODE srb_to_add_mod_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.pack(discard_on_pdcp_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_present, 1));

  HANDLE_CODE(pack_integer(bref, srb_id, (uint8_t)1u, (uint8_t)3u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= srb_id_v1700_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srb_id_v1700_present, 1));
      if (srb_id_v1700_present) {
        HANDLE_CODE(pack_integer(bref, srb_id_v1700, (uint8_t)4u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(reestablish_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(discard_on_pdcp_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_present, 1));

  HANDLE_CODE(unpack_integer(srb_id, bref, (uint8_t)1u, (uint8_t)3u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(srb_id_v1700_present, 1));
      if (srb_id_v1700_present) {
        HANDLE_CODE(unpack_integer(srb_id_v1700, bref, (uint8_t)4u, (uint8_t)4u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srb_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srb-Identity", srb_id);
  if (reestablish_pdcp_present) {
    j.write_str("reestablishPDCP", "true");
  }
  if (discard_on_pdcp_present) {
    j.write_str("discardOnPDCP", "true");
  }
  if (pdcp_cfg_present) {
    j.write_fieldname("pdcp-Config");
    pdcp_cfg.to_json(j);
  }
  if (ext) {
    if (srb_id_v1700_present) {
      j.write_int("srb-Identity-v1700", srb_id_v1700);
    }
  }
  j.end_obj();
}

// SecurityAlgorithmConfig ::= SEQUENCE
SRSASN_CODE security_algorithm_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(integrity_prot_algorithm_present, 1));

  HANDLE_CODE(ciphering_algorithm.pack(bref));
  if (integrity_prot_algorithm_present) {
    HANDLE_CODE(integrity_prot_algorithm.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_algorithm_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(integrity_prot_algorithm_present, 1));

  HANDLE_CODE(ciphering_algorithm.unpack(bref));
  if (integrity_prot_algorithm_present) {
    HANDLE_CODE(integrity_prot_algorithm.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_algorithm_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cipheringAlgorithm", ciphering_algorithm.to_string());
  if (integrity_prot_algorithm_present) {
    j.write_str("integrityProtAlgorithm", integrity_prot_algorithm.to_string());
  }
  j.end_obj();
}

// SecurityConfig ::= SEQUENCE
SRSASN_CODE security_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(security_algorithm_cfg_present, 1));
  HANDLE_CODE(bref.pack(key_to_use_present, 1));

  if (security_algorithm_cfg_present) {
    HANDLE_CODE(security_algorithm_cfg.pack(bref));
  }
  if (key_to_use_present) {
    HANDLE_CODE(key_to_use.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(security_algorithm_cfg_present, 1));
  HANDLE_CODE(bref.unpack(key_to_use_present, 1));

  if (security_algorithm_cfg_present) {
    HANDLE_CODE(security_algorithm_cfg.unpack(bref));
  }
  if (key_to_use_present) {
    HANDLE_CODE(key_to_use.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (security_algorithm_cfg_present) {
    j.write_fieldname("securityAlgorithmConfig");
    security_algorithm_cfg.to_json(j);
  }
  if (key_to_use_present) {
    j.write_str("keyToUse", key_to_use.to_string());
  }
  j.end_obj();
}

const char* security_cfg_s::key_to_use_opts::to_string() const
{
  static const char* names[] = {"master", "secondary"};
  return convert_enum_idx(names, 2, value, "security_cfg_s::key_to_use_e_");
}

// RadioBearerConfig ::= SEQUENCE
SRSASN_CODE radio_bearer_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srb_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(srb3_to_release_present, 1));
  HANDLE_CODE(bref.pack(drb_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(security_cfg_present, 1));

  if (srb_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srb_to_add_mod_list, 1, 2));
  }
  if (drb_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_add_mod_list, 1, 29));
  }
  if (drb_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_release_list, 1, 29, integer_packer<uint8_t>(1, 32)));
  }
  if (security_cfg_present) {
    HANDLE_CODE(security_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mrb_to_add_mod_list_r17.is_present();
    group_flags[0] |= mrb_to_release_list_r17.is_present();
    group_flags[0] |= srb4_to_add_mod_r17.is_present();
    group_flags[0] |= srb4_to_release_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mrb_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(mrb_to_release_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(srb4_to_add_mod_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(srb4_to_release_r17_present, 1));
      if (mrb_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mrb_to_add_mod_list_r17, 1, 32));
      }
      if (mrb_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mrb_to_release_list_r17, 1, 32, integer_packer<uint16_t>(1, 512)));
      }
      if (srb4_to_add_mod_r17.is_present()) {
        HANDLE_CODE(srb4_to_add_mod_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE radio_bearer_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool srb_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(srb_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(srb3_to_release_present, 1));
  bool drb_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(drb_to_add_mod_list_present, 1));
  bool drb_to_release_list_present;
  HANDLE_CODE(bref.unpack(drb_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(security_cfg_present, 1));

  if (srb_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srb_to_add_mod_list, bref, 1, 2));
  }
  if (drb_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_add_mod_list, bref, 1, 29));
  }
  if (drb_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_release_list, bref, 1, 29, integer_packer<uint8_t>(1, 32)));
  }
  if (security_cfg_present) {
    HANDLE_CODE(security_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(mrb_to_add_mod_list_r17, bref);
      unpack_presence_flag(mrb_to_release_list_r17, bref);
      unpack_presence_flag(srb4_to_add_mod_r17, bref);
      HANDLE_CODE(bref.unpack(srb4_to_release_r17_present, 1));
      if (mrb_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mrb_to_add_mod_list_r17, bref, 1, 32));
      }
      if (mrb_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mrb_to_release_list_r17, bref, 1, 32, integer_packer<uint16_t>(1, 512)));
      }
      if (srb4_to_add_mod_r17.is_present()) {
        HANDLE_CODE(srb4_to_add_mod_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void radio_bearer_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srb_to_add_mod_list.size() > 0) {
    j.start_array("srb-ToAddModList");
    for (const auto& e1 : srb_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srb3_to_release_present) {
    j.write_str("srb3-ToRelease", "true");
  }
  if (drb_to_add_mod_list.size() > 0) {
    j.start_array("drb-ToAddModList");
    for (const auto& e1 : drb_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_release_list.size() > 0) {
    j.start_array("drb-ToReleaseList");
    for (const auto& e1 : drb_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (security_cfg_present) {
    j.write_fieldname("securityConfig");
    security_cfg.to_json(j);
  }
  if (ext) {
    if (mrb_to_add_mod_list_r17.is_present()) {
      j.start_array("mrb-ToAddModList-r17");
      for (const auto& e1 : *mrb_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (mrb_to_release_list_r17.is_present()) {
      j.start_array("mrb-ToReleaseList-r17");
      for (const auto& e1 : *mrb_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (srb4_to_add_mod_r17.is_present()) {
      j.write_fieldname("srb4-ToAddMod-r17");
      srb4_to_add_mod_r17->to_json(j);
    }
    if (srb4_to_release_r17_present) {
      j.write_str("srb4-ToRelease-r17", "true");
    }
  }
  j.end_obj();
}

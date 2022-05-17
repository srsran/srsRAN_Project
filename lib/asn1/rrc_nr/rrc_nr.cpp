/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PDCCH-ConfigSIB1 ::= SEQUENCE
SRSASN_CODE pdcch_cfg_sib1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, coreset_zero, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, search_space_zero, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_cfg_sib1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(coreset_zero, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(search_space_zero, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_cfg_sib1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("controlResourceSetZero", coreset_zero);
  j.write_int("searchSpaceZero", search_space_zero);
  j.end_obj();
}

// MIB ::= SEQUENCE
SRSASN_CODE mib_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sys_frame_num.pack(bref));
  HANDLE_CODE(sub_carrier_spacing_common.pack(bref));
  HANDLE_CODE(pack_integer(bref, ssb_subcarrier_offset, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(dmrs_type_a_position.pack(bref));
  HANDLE_CODE(pdcch_cfg_sib1.pack(bref));
  HANDLE_CODE(cell_barred.pack(bref));
  HANDLE_CODE(intra_freq_resel.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sys_frame_num.unpack(bref));
  HANDLE_CODE(sub_carrier_spacing_common.unpack(bref));
  HANDLE_CODE(unpack_integer(ssb_subcarrier_offset, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(dmrs_type_a_position.unpack(bref));
  HANDLE_CODE(pdcch_cfg_sib1.unpack(bref));
  HANDLE_CODE(cell_barred.unpack(bref));
  HANDLE_CODE(intra_freq_resel.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("systemFrameNumber", sys_frame_num.to_string());
  j.write_str("subCarrierSpacingCommon", sub_carrier_spacing_common.to_string());
  j.write_int("ssb-SubcarrierOffset", ssb_subcarrier_offset);
  j.write_str("dmrs-TypeA-Position", dmrs_type_a_position.to_string());
  j.write_fieldname("pdcch-ConfigSIB1");
  pdcch_cfg_sib1.to_json(j);
  j.write_str("cellBarred", cell_barred.to_string());
  j.write_str("intraFreqReselection", intra_freq_resel.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

const char* mib_s::sub_carrier_spacing_common_opts::to_string() const
{
  static const char* options[] = {"scs15or60", "scs30or120"};
  return convert_enum_idx(options, 2, value, "mib_s::sub_carrier_spacing_common_e_");
}
uint8_t mib_s::sub_carrier_spacing_common_opts::to_number() const
{
  static const uint8_t options[] = {15, 30};
  return map_enum_number(options, 2, value, "mib_s::sub_carrier_spacing_common_e_");
}

const char* mib_s::dmrs_type_a_position_opts::to_string() const
{
  static const char* options[] = {"pos2", "pos3"};
  return convert_enum_idx(options, 2, value, "mib_s::dmrs_type_a_position_e_");
}
uint8_t mib_s::dmrs_type_a_position_opts::to_number() const
{
  static const uint8_t options[] = {2, 3};
  return map_enum_number(options, 2, value, "mib_s::dmrs_type_a_position_e_");
}

const char* mib_s::cell_barred_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(options, 2, value, "mib_s::cell_barred_e_");
}

const char* mib_s::intra_freq_resel_opts::to_string() const
{
  static const char* options[] = {"allowed", "notAllowed"};
  return convert_enum_idx(options, 2, value, "mib_s::intra_freq_resel_e_");
}

// BCCH-BCH-MessageType ::= CHOICE
void bcch_bch_msg_type_c::set(types::options e)
{
  type_ = e;
}
mib_s& bcch_bch_msg_type_c::set_mib()
{
  set(types::mib);
  return c;
}
void bcch_bch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void bcch_bch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mib:
      j.write_fieldname("mib");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_bch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_bch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mib:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_bch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mib:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_bch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bcch_bch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"mib", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_bch_msg_type_c::types");
}

// BCCH-BCH-Message ::= SEQUENCE
SRSASN_CODE bcch_bch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_bch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-BCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

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

// EUTRA-Q-OffsetRange ::= ENUMERATED
const char* eutra_q_offset_range_opts::to_string() const
{
  static const char* options[] = {"dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10",
                                  "dB-8",  "dB-6",  "dB-5",  "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",
                                  "dB1",   "dB2",   "dB3",   "dB4",   "dB5",   "dB6",   "dB8",   "dB10",
                                  "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
  return convert_enum_idx(options, 31, value, "eutra_q_offset_range_e");
}
int8_t eutra_q_offset_range_opts::to_number() const
{
  static const int8_t options[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 31, value, "eutra_q_offset_range_e");
}

// Q-OffsetRange ::= ENUMERATED
const char* q_offset_range_opts::to_string() const
{
  static const char* options[] = {"dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10",
                                  "dB-8",  "dB-6",  "dB-5",  "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",
                                  "dB1",   "dB2",   "dB3",   "dB4",   "dB5",   "dB6",   "dB8",   "dB10",
                                  "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
  return convert_enum_idx(options, 31, value, "q_offset_range_e");
}
int8_t q_offset_range_opts::to_number() const
{
  static const int8_t options[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 31, value, "q_offset_range_e");
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
  static const char* options[] = {"n4",
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
  return convert_enum_idx(options, 16, value, "eutra_pci_range_s::range_e_");
}
uint16_t eutra_pci_range_s::range_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504};
  return map_enum_number(options, 14, value, "eutra_pci_range_s::range_e_");
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
  static const char* options[] = {"n4",
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
  return convert_enum_idx(options, 16, value, "pci_range_s::range_e_");
}
uint16_t pci_range_s::range_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504, 1008};
  return map_enum_number(options, 15, value, "pci_range_s::range_e_");
}

// CellReselectionSubPriority ::= ENUMERATED
const char* cell_resel_sub_prio_opts::to_string() const
{
  static const char* options[] = {"oDot2", "oDot4", "oDot6", "oDot8"};
  return convert_enum_idx(options, 4, value, "cell_resel_sub_prio_e");
}
float cell_resel_sub_prio_opts::to_number() const
{
  static const float options[] = {0.2, 0.4, 0.6, 0.8};
  return map_enum_number(options, 4, value, "cell_resel_sub_prio_e");
}
const char* cell_resel_sub_prio_opts::to_number_string() const
{
  static const char* options[] = {"0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(options, 4, value, "cell_resel_sub_prio_e");
}

const char* coreset_s::cce_reg_map_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"interleaved", "nonInterleaved"};
  return convert_enum_idx(options, 2, value, "coreset_s::cce_reg_map_type_c_::types");
}

// EUTRA-AllowedMeasBandwidth ::= ENUMERATED
const char* eutra_allowed_meas_bw_opts::to_string() const
{
  static const char* options[] = {"mbw6", "mbw15", "mbw25", "mbw50", "mbw75", "mbw100"};
  return convert_enum_idx(options, 6, value, "eutra_allowed_meas_bw_e");
}
uint8_t eutra_allowed_meas_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "eutra_allowed_meas_bw_e");
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
  static const char* options[] = {"sf5", "sf10", "sf20", "sf40", "sf80", "sf160"};
  return convert_enum_idx(options, 6, value, "ssb_mtc_s::periodicity_and_offset_c_::types");
}
uint8_t ssb_mtc_s::periodicity_and_offset_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(options, 6, value, "ssb_mtc_s::periodicity_and_offset_c_::types");
}

const char* ssb_mtc_s::dur_opts::to_string() const
{
  static const char* options[] = {"sf1", "sf2", "sf3", "sf4", "sf5"};
  return convert_enum_idx(options, 5, value, "ssb_mtc_s::dur_e_");
}
uint8_t ssb_mtc_s::dur_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5};
  return map_enum_number(options, 5, value, "ssb_mtc_s::dur_e_");
}

// SSB-ToMeasure ::= CHOICE
void ssb_to_measure_c::destroy_()
{
  switch (type_) {
    case types::short_bitmap:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::medium_bitmap:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::long_bitmap:
      c.destroy<fixed_bitstring<64> >();
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
      c.init<fixed_bitstring<4> >();
      break;
    case types::medium_bitmap:
      c.init<fixed_bitstring<8> >();
      break;
    case types::long_bitmap:
      c.init<fixed_bitstring<64> >();
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
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::medium_bitmap:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::long_bitmap:
      c.init(other.c.get<fixed_bitstring<64> >());
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
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::medium_bitmap:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::long_bitmap:
      c.set(other.c.get<fixed_bitstring<64> >());
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
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<8>& ssb_to_measure_c::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<64>& ssb_to_measure_c::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64> >();
}
void ssb_to_measure_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::short_bitmap:
      j.write_str("shortBitmap", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::medium_bitmap:
      j.write_str("mediumBitmap", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::long_bitmap:
      j.write_str("longBitmap", c.get<fixed_bitstring<64> >().to_string());
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
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().pack(bref));
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
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_to_measure_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ssb_to_measure_c::types_opts::to_string() const
{
  static const char* options[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
  return convert_enum_idx(options, 3, value, "ssb_to_measure_c::types");
}

const char* search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::to_string() const
{
  static const char* options[] = {"sl1",
                                  "sl2",
                                  "sl4",
                                  "sl5",
                                  "sl8",
                                  "sl10",
                                  "sl16",
                                  "sl20",
                                  "sl40",
                                  "sl80",
                                  "sl160",
                                  "sl320",
                                  "sl640",
                                  "sl1280",
                                  "sl2560"};
  return convert_enum_idx(options, 15, value, "search_space_s::monitoring_slot_periodicity_and_offset_c_::types");
}
uint16_t search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320, 640, 1280, 2560};
  return map_enum_number(options, 15, value, "search_space_s::monitoring_slot_periodicity_and_offset_c_::types");
}

const char* search_space_s::search_space_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"common", "ue-Specific"};
  return convert_enum_idx(options, 2, value, "search_space_s::search_space_type_c_::types");
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
  static const char* options[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
float speed_state_scale_factors_s::sf_medium_opts::to_number() const
{
  static const float options[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
const char* speed_state_scale_factors_s::sf_medium_opts::to_number_string() const
{
  static const char* options[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}

const char* speed_state_scale_factors_s::sf_high_opts::to_string() const
{
  static const char* options[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
float speed_state_scale_factors_s::sf_high_opts::to_number() const
{
  static const float options[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
const char* speed_state_scale_factors_s::sf_high_opts::to_number_string() const
{
  static const char* options[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
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

// CarrierFreqEUTRA ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_multi_band_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_black_cell_list.size() > 0, 1));
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
  if (eutra_black_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_black_cell_list, 1, 16));
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
  bool eutra_black_cell_list_present;
  HANDLE_CODE(bref.unpack(eutra_black_cell_list_present, 1));
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
  if (eutra_black_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_black_cell_list, bref, 1, 16));
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
  if (eutra_black_cell_list.size() > 0) {
    j.start_array("eutra-BlackCellList");
    for (const auto& e1 : eutra_black_cell_list) {
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
  HANDLE_CODE(bref.pack(inter_freq_black_cell_list.size() > 0, 1));

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
  if (inter_freq_black_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_black_cell_list, 1, 16));
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
  bool inter_freq_black_cell_list_present;
  HANDLE_CODE(bref.unpack(inter_freq_black_cell_list_present, 1));

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
  if (inter_freq_black_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_black_cell_list, bref, 1, 16));
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
  if (inter_freq_black_cell_list.size() > 0) {
    j.start_array("interFreqBlackCellList");
    for (const auto& e1 : inter_freq_black_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
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

const char* pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::types_opts::to_string() const
{
  static const char* options[] = {"sCS15KHZoneT",
                                  "sCS30KHZoneT-SCS15KHZhalfT",
                                  "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT",
                                  "sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                                  "sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                                  "sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                                  "sCS120KHZoneEighthT-SCS60KHZoneSixteenthT",
                                  "sCS120KHZoneSixteenthT"};
  return convert_enum_idx(options, 8, value, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::types");
}

// PLMN-Identity ::= SEQUENCE
SRSASN_CODE plmn_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, &(mcc)[0], mcc.size(), integer_packer<uint8_t>(0, 9)));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, mnc, 2, 3, integer_packer<uint8_t>(0, 9)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(unpack_fixed_seq_of(&(mcc)[0], bref, mcc.size(), integer_packer<uint8_t>(0, 9)));
  }
  HANDLE_CODE(unpack_dyn_seq_of(mnc, bref, 2, 3, integer_packer<uint8_t>(0, 9)));

  return SRSASN_SUCCESS;
}
void plmn_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mcc_present) {
    j.start_array("mcc");
    for (const auto& e1 : mcc) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.start_array("mnc");
  for (const auto& e1 : mnc) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types");
}
float rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types_opts::to_number() const
{
  static const float options[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(
      options, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types");
}
const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types_opts::to_number_string() const
{
  static const char* options[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types");
}

const char* rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::to_string() const
{
  static const char* options[] = {"l839", "l139"};
  return convert_enum_idx(options, 2, value, "rach_cfg_common_s::prach_root_seq_idx_c_::types");
}
uint16_t rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {839, 139};
  return map_enum_number(options, 2, value, "rach_cfg_common_s::prach_root_seq_idx_c_::types");
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
  static const char* options[] = {"sibType2",
                                  "sibType3",
                                  "sibType4",
                                  "sibType5",
                                  "sibType6",
                                  "sibType7",
                                  "sibType8",
                                  "sibType9",
                                  "spare8",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sib_type_info_s::type_e_");
}
uint8_t sib_type_info_s::type_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 9};
  return map_enum_number(options, 8, value, "sib_type_info_s::type_e_");
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
  static const char* options[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_state_params_s::t_eval_e_");
}
uint8_t mob_state_params_s::t_eval_opts::to_number() const
{
  static const uint8_t options[] = {30, 60, 120, 180, 240};
  return map_enum_number(options, 5, value, "mob_state_params_s::t_eval_e_");
}

const char* mob_state_params_s::t_hyst_normal_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_state_params_s::t_hyst_normal_e_");
}
uint8_t mob_state_params_s::t_hyst_normal_opts::to_number() const
{
  static const uint8_t options[] = {30, 60, 120, 180, 240};
  return map_enum_number(options, 5, value, "mob_state_params_s::t_hyst_normal_e_");
}

const char* pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneT", "halfT", "quarterT", "oneEighthT", "oneSixteenthT"};
  return convert_enum_idx(options, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
}
float pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_number() const
{
  static const float options[] = {1.0, 0.5, 0.25, 0.125, 0.0625};
  return map_enum_number(options, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
}
const char* pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_number_string() const
{
  static const char* options[] = {"1", "0.5", "0.25", "1/8", "1/16"};
  return convert_enum_idx(options, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
}

const char* pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types_opts::to_string() const
{
  static const char* options[] = {"sCS15KHZoneT",
                                  "sCS30KHZoneT-SCS15KHZhalfT",
                                  "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT",
                                  "sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                                  "sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                                  "sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                                  "sCS120KHZoneEighthT-SCS60KHZoneSixteenthT",
                                  "sCS120KHZoneSixteenthT"};
  return convert_enum_idx(options, 8, value, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types");
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
  j.end_obj();
}

const char* plmn_id_info_s::cell_reserved_for_oper_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_s::cell_reserved_for_oper_e_");
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
  static const char* options[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(options, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}
float si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number() const
{
  static const float options[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(options, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}
const char* si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* options[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(options, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}

const char* si_request_cfg_s::si_request_period_opts::to_string() const
{
  static const char* options[] = {"one", "two", "four", "six", "eight", "ten", "twelve", "sixteen"};
  return convert_enum_idx(options, 8, value, "si_request_cfg_s::si_request_period_e_");
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
  j.end_obj();
}

const char* sib2_s::cell_resel_info_common_s_::q_hyst_opts::to_string() const
{
  static const char* options[] = {"dB0",
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
  return convert_enum_idx(options, 16, value, "sib2_s::cell_resel_info_common_s_::q_hyst_e_");
}
uint8_t sib2_s::cell_resel_info_common_s_::q_hyst_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "sib2_s::cell_resel_info_common_s_::q_hyst_e_");
}

const char*
sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      options, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_number() const
{
  static const int8_t options[] = {-6, -4, -2, 0};
  return map_enum_number(
      options, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}

const char* sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      options, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_number() const
{
  static const int8_t options[] = {-6, -4, -2, 0};
  return map_enum_number(
      options, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
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
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t_resel_nr_sf.is_present(), 1));
      if (t_resel_nr_sf.is_present()) {
        HANDLE_CODE(t_resel_nr_sf->pack(bref));
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool t_resel_nr_sf_present;
      HANDLE_CODE(bref.unpack(t_resel_nr_sf_present, 1));
      t_resel_nr_sf.set_present(t_resel_nr_sf_present);
      if (t_resel_nr_sf.is_present()) {
        HANDLE_CODE(t_resel_nr_sf->unpack(bref));
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
  }
  j.end_obj();
}

// SIB3 ::= SEQUENCE
SRSASN_CODE sib3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(intra_freq_black_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (intra_freq_neigh_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_neigh_cell_list, 1, 16));
  }
  if (intra_freq_black_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_black_cell_list, 1, 16));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool intra_freq_neigh_cell_list_present;
  HANDLE_CODE(bref.unpack(intra_freq_neigh_cell_list_present, 1));
  bool intra_freq_black_cell_list_present;
  HANDLE_CODE(bref.unpack(intra_freq_black_cell_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (intra_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_neigh_cell_list, bref, 1, 16));
  }
  if (intra_freq_black_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_black_cell_list, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
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
  if (intra_freq_black_cell_list.size() > 0) {
    j.start_array("intraFreqBlackCellList");
    for (const auto& e1 : intra_freq_black_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
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
  static const char* options[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(options, 2, value, "sib7_s::warning_msg_segment_type_e_");
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
  static const char* options[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(options, 2, value, "sib8_s::warning_msg_segment_type_e_");
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
  static const char* options[] = {"broadcasting", "notBroadcasting"};
  return convert_enum_idx(options, 2, value, "sched_info_s::si_broadcast_status_e_");
}

const char* sched_info_s::si_periodicity_opts::to_string() const
{
  static const char* options[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(options, 7, value, "sched_info_s::si_periodicity_e_");
}
uint16_t sched_info_s::si_periodicity_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 7, value, "sched_info_s::si_periodicity_e_");
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
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}
float uac_barr_info_set_s::uac_barr_factor_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(options, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}
const char* uac_barr_info_set_s::uac_barr_factor_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}

const char* uac_barr_info_set_s::uac_barr_time_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "uac_barr_info_set_s::uac_barr_time_e_");
}
uint16_t uac_barr_info_set_s::uac_barr_time_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "uac_barr_info_set_s::uac_barr_time_e_");
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
                                    &(c.get<uac_implicit_ac_barr_list_l_>())[0],
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
      HANDLE_CODE(unpack_fixed_seq_of(&(c.get<uac_implicit_ac_barr_list_l_>())[0],
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
  static const char* options[] = {"uac-ImplicitACBarringList", "uac-ExplicitACBarringList"};
  return convert_enum_idx(options, 2, value, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types");
}

// CellAccessRelatedInfo ::= SEQUENCE
SRSASN_CODE cell_access_related_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_reserved_for_other_use_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list, 1, 12));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_reserved_for_other_use_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list, bref, 1, 12));

  return SRSASN_SUCCESS;
}
void cell_access_related_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList");
  for (const auto& e1 : plmn_id_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (cell_reserved_for_other_use_present) {
    j.write_str("cellReservedForOtherUse", "true");
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
  static const char* options[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(options, 4, value, "conn_est_fail_ctrl_s::conn_est_fail_count_e_");
}
uint8_t conn_est_fail_ctrl_s::conn_est_fail_count_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(options, 4, value, "conn_est_fail_ctrl_s::conn_est_fail_count_e_");
}

const char* conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s240", "s300", "s420", "s600", "s900"};
  return convert_enum_idx(options, 8, value, "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_");
}
uint16_t conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::to_number() const
{
  static const uint16_t options[] = {30, 60, 120, 240, 300, 420, 600, 900};
  return map_enum_number(options, 8, value, "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_");
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
  static const char* options[] = {"s5", "s10", "s20", "s40", "s80", "s160", "s320", "s640", "s1280"};
  return convert_enum_idx(options, 9, value, "si_sched_info_s::si_win_len_e_");
}
uint16_t si_sched_info_s::si_win_len_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 40, 80, 160, 320, 640, 1280};
  return map_enum_number(options, 9, value, "si_sched_info_s::si_win_len_e_");
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
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_info_ies_s::sib_type_and_info_item_c_::types_opts::to_string() const
{
  static const char* options[] = {"sib2", "sib3", "sib4", "sib5", "sib6", "sib7", "sib8", "sib9"};
  return convert_enum_idx(options, 8, value, "sys_info_ies_s::sib_type_and_info_item_c_::types");
}
uint8_t sys_info_ies_s::sib_type_and_info_item_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 9};
  return map_enum_number(options, 8, value, "sys_info_ies_s::sib_type_and_info_item_c_::types");
}

// UAC-AccessCategory1-SelectionAssistanceInfo ::= ENUMERATED
const char* uac_access_category1_sel_assist_info_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c"};
  return convert_enum_idx(options, 3, value, "uac_access_category1_sel_assist_info_e");
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
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t300_e_");
}
uint16_t ue_timers_and_consts_s::t300_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_e_");
}

const char* ue_timers_and_consts_s::t301_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t301_e_");
}
uint16_t ue_timers_and_consts_s::t301_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t301_e_");
}

const char* ue_timers_and_consts_s::t310_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_s::t310_e_");
}
uint16_t ue_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_s::t310_e_");
}

const char* ue_timers_and_consts_s::n310_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::n310_e_");
}
uint8_t ue_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::n310_e_");
}

const char* ue_timers_and_consts_s::t311_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_s::t311_e_");
}
uint16_t ue_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_s::t311_e_");
}

const char* ue_timers_and_consts_s::n311_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::n311_e_");
}
uint8_t ue_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::n311_e_");
}

const char* ue_timers_and_consts_s::t319_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t319_e_");
}
uint16_t ue_timers_and_consts_s::t319_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t319_e_");
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
    j.start_obj();
    j.end_obj();
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
  static const char* options[] = {"plmnCommon", "individualPLMNList"};
  return convert_enum_idx(
      options, 2, value, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types");
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

void sys_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
sys_info_ies_s& sys_info_s::crit_exts_c_::set_sys_info()
{
  set(types::sys_info);
  return c;
}
void sys_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void sys_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info:
      j.write_fieldname("systemInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
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
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
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
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "sys_info_s::crit_exts_c_::types");
}

// BCCH-DL-SCH-MessageType ::= CHOICE
void bcch_dl_sch_msg_type_c::set(types::options e)
{
  type_ = e;
}
bcch_dl_sch_msg_type_c::c1_c_& bcch_dl_sch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void bcch_dl_sch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void bcch_dl_sch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void bcch_dl_sch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::sys_info:
      c.destroy<sys_info_s>();
      break;
    case types::sib_type1:
      c.destroy<sib1_s>();
      break;
    default:
      break;
  }
}
void bcch_dl_sch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info:
      c.init<sys_info_s>();
      break;
    case types::sib_type1:
      c.init<sib1_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_c::c1_c_::c1_c_(const bcch_dl_sch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info:
      c.init(other.c.get<sys_info_s>());
      break;
    case types::sib_type1:
      c.init(other.c.get<sib1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_c::c1_c_& bcch_dl_sch_msg_type_c::c1_c_::operator=(const bcch_dl_sch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info:
      c.set(other.c.get<sys_info_s>());
      break;
    case types::sib_type1:
      c.set(other.c.get<sib1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }

  return *this;
}
sys_info_s& bcch_dl_sch_msg_type_c::c1_c_::set_sys_info()
{
  set(types::sys_info);
  return c.get<sys_info_s>();
}
sib1_s& bcch_dl_sch_msg_type_c::c1_c_::set_sib_type1()
{
  set(types::sib_type1);
  return c.get<sib1_s>();
}
void bcch_dl_sch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info:
      j.write_fieldname("systemInformation");
      c.get<sys_info_s>().to_json(j);
      break;
    case types::sib_type1:
      j.write_fieldname("systemInformationBlockType1");
      c.get<sib1_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_s>().pack(bref));
      break;
    case types::sib_type1:
      HANDLE_CODE(c.get<sib1_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_s>().unpack(bref));
      break;
    case types::sib_type1:
      HANDLE_CODE(c.get<sib1_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation", "systemInformationBlockType1"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_c::c1_c_::types");
}
uint8_t bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_number() const
{
  if (value == sib_type1) {
    return 1;
  }
  invalid_enum_number(value, "bcch_dl_sch_msg_type_c::c1_c_::types");
  return 0;
}

const char* bcch_dl_sch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_c::types");
}
uint8_t bcch_dl_sch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_c::types");
}

// BCCH-DL-SCH-Message ::= SEQUENCE
SRSASN_CODE bcch_dl_sch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_dl_sch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-DL-SCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// UL-DataSplitThreshold ::= ENUMERATED
const char* ul_data_split_thres_opts::to_string() const
{
  static const char* options[] = {"b0",       "b100",     "b200",     "b400",     "b800",     "b1600",    "b3200",
                                  "b6400",    "b12800",   "b25600",   "b51200",   "b102400",  "b204800",  "b409600",
                                  "b819200",  "b1228800", "b1638400", "b2457600", "b3276800", "b4096000", "b4915200",
                                  "b5734400", "b6553600", "infinity", "spare8",   "spare7",   "spare6",   "spare5",
                                  "spare4",   "spare3",   "spare2",   "spare1"};
  return convert_enum_idx(options, 32, value, "ul_data_split_thres_e");
}
int32_t ul_data_split_thres_opts::to_number() const
{
  static const int32_t options[] = {0,       100,     200,     400,     800,     1600,    3200,    6400,
                                    12800,   25600,   51200,   102400,  204800,  409600,  819200,  1228800,
                                    1638400, 2457600, 3276800, 4096000, 4915200, 5734400, 6553600, -1};
  return map_enum_number(options, 24, value, "ul_data_split_thres_e");
}

// CipheringAlgorithm ::= ENUMERATED
const char* ciphering_algorithm_opts::to_string() const
{
  static const char* options[] = {"nea0", "nea1", "nea2", "nea3", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ciphering_algorithm_e");
}
uint8_t ciphering_algorithm_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "ciphering_algorithm_e");
}

// IntegrityProtAlgorithm ::= ENUMERATED
const char* integrity_prot_algorithm_opts::to_string() const
{
  static const char* options[] = {"nia0", "nia1", "nia2", "nia3", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "integrity_prot_algorithm_e");
}
uint8_t integrity_prot_algorithm_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "integrity_prot_algorithm_e");
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
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ciphering_disabled_present, 1));
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ciphering_disabled_present, 1));
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
  }
  j.end_obj();
}

const char* pdcp_cfg_s::drb_s_::discard_timer_opts::to_string() const
{
  static const char* options[] = {"ms10",
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
  return convert_enum_idx(options, 16, value, "pdcp_cfg_s::drb_s_::discard_timer_e_");
}
int16_t pdcp_cfg_s::drb_s_::discard_timer_opts::to_number() const
{
  static const int16_t options[] = {10, 20, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(options, 16, value, "pdcp_cfg_s::drb_s_::discard_timer_e_");
}

const char* pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::to_string() const
{
  static const char* options[] = {"len12bits", "len18bits"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_");
}
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::to_number() const
{
  static const uint8_t options[] = {12, 18};
  return map_enum_number(options, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_");
}

const char* pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::to_string() const
{
  static const char* options[] = {"len12bits", "len18bits"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_");
}
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::to_number() const
{
  static const uint8_t options[] = {12, 18};
  return map_enum_number(options, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_");
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
      if (c.get<rohc_s_>().max_cid_present) {
        j.write_int("maxCID", c.get<rohc_s_>().max_cid);
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
      if (c.get<ul_only_rohc_s_>().max_cid_present) {
        j.write_int("maxCID", c.get<ul_only_rohc_s_>().max_cid);
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
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().max_cid_present, 1));
      HANDLE_CODE(bref.pack(c.get<rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<rohc_s_>().max_cid_present) {
        HANDLE_CODE(pack_integer(bref, c.get<rohc_s_>().max_cid, (uint16_t)1u, (uint16_t)16383u));
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
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().max_cid_present, 1));
      HANDLE_CODE(bref.pack(c.get<ul_only_rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<ul_only_rohc_s_>().max_cid_present) {
        HANDLE_CODE(pack_integer(bref, c.get<ul_only_rohc_s_>().max_cid, (uint16_t)1u, (uint16_t)16383u));
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
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().max_cid_present, 1));
      HANDLE_CODE(bref.unpack(c.get<rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<rohc_s_>().max_cid_present) {
        HANDLE_CODE(unpack_integer(c.get<rohc_s_>().max_cid, bref, (uint16_t)1u, (uint16_t)16383u));
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
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().max_cid_present, 1));
      HANDLE_CODE(bref.unpack(c.get<ul_only_rohc_s_>().drb_continue_rohc_present, 1));
      if (c.get<ul_only_rohc_s_>().max_cid_present) {
        HANDLE_CODE(unpack_integer(c.get<ul_only_rohc_s_>().max_cid, bref, (uint16_t)1u, (uint16_t)16383u));
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
  static const char* options[] = {"notUsed", "rohc", "uplinkOnlyROHC"};
  return convert_enum_idx(options, 3, value, "pdcp_cfg_s::drb_s_::hdr_compress_c_::types");
}

const char* pdcp_cfg_s::t_reordering_opts::to_string() const
{
  static const char* options[] = {
      "ms0",     "ms1",     "ms2",     "ms4",     "ms5",     "ms8",     "ms10",    "ms15",    "ms20",    "ms30",
      "ms40",    "ms50",    "ms60",    "ms80",    "ms100",   "ms120",   "ms140",   "ms160",   "ms180",   "ms200",
      "ms220",   "ms240",   "ms260",   "ms280",   "ms300",   "ms500",   "ms750",   "ms1000",  "ms1250",  "ms1500",
      "ms1750",  "ms2000",  "ms2250",  "ms2500",  "ms2750",  "ms3000",  "spare28", "spare27", "spare26", "spare25",
      "spare24", "spare23", "spare22", "spare21", "spare20", "spare19", "spare18", "spare17", "spare16", "spare15",
      "spare14", "spare13", "spare12", "spare11", "spare10", "spare09", "spare08", "spare07", "spare06", "spare05",
      "spare04", "spare03", "spare02", "spare01"};
  return convert_enum_idx(options, 64, value, "pdcp_cfg_s::t_reordering_e_");
}
uint16_t pdcp_cfg_s::t_reordering_opts::to_number() const
{
  static const uint16_t options[] = {0,   1,   2,   4,    5,    8,    10,   15,   20,   30,   40,   50,
                                     60,  80,  100, 120,  140,  160,  180,  200,  220,  240,  260,  280,
                                     300, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000};
  return map_enum_number(options, 36, value, "pdcp_cfg_s::t_reordering_e_");
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
  static const char* options[] = {"present", "absent"};
  return convert_enum_idx(options, 2, value, "sdap_cfg_s::sdap_hdr_dl_e_");
}

const char* sdap_cfg_s::sdap_hdr_ul_opts::to_string() const
{
  static const char* options[] = {"present", "absent"};
  return convert_enum_idx(options, 2, value, "sdap_cfg_s::sdap_hdr_ul_e_");
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
  static const char* options[] = {"eps-BearerIdentity", "sdap-Config"};
  return convert_enum_idx(options, 2, value, "drb_to_add_mod_s::cn_assoc_c_::types");
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
  static const char* options[] = {"master", "secondary"};
  return convert_enum_idx(options, 2, value, "security_cfg_s::key_to_use_e_");
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
  j.end_obj();
}

// RRCReject-IEs ::= SEQUENCE
SRSASN_CODE rrc_reject_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wait_time_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(pack_integer(bref, wait_time, (uint8_t)1u, (uint8_t)16u));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reject_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wait_time_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(unpack_integer(wait_time, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_reject_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wait_time_present) {
    j.write_int("waitTime", wait_time);
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

// RRCSetup-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(radio_bearer_cfg.pack(bref));
  HANDLE_CODE(master_cell_group.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(radio_bearer_cfg.unpack(bref));
  HANDLE_CODE(master_cell_group.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_setup_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("radioBearerConfig");
  radio_bearer_cfg.to_json(j);
  j.write_str("masterCellGroup", master_cell_group.to_string());
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

// RRCReject ::= SEQUENCE
SRSASN_CODE rrc_reject_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reject_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reject_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_reject_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_reject_ies_s& rrc_reject_s::crit_exts_c_::set_rrc_reject()
{
  set(types::rrc_reject);
  return c;
}
void rrc_reject_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_reject_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_reject:
      j.write_fieldname("rrcReject");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reject_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_reject_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reject_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reject_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reject_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_reject_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcReject", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_reject_s::crit_exts_c_::types");
}

// RRCSetup ::= SEQUENCE
SRSASN_CODE rrc_setup_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_setup_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_setup_ies_s& rrc_setup_s::crit_exts_c_::set_rrc_setup()
{
  set(types::rrc_setup);
  return c;
}
void rrc_setup_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_setup_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_setup:
      j.write_fieldname("rrcSetup");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_setup_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_setup:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_setup_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcSetup", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_setup_s::crit_exts_c_::types");
}

// DL-CCCH-MessageType ::= CHOICE
void dl_ccch_msg_type_c::set(types::options e)
{
  type_ = e;
}
dl_ccch_msg_type_c::c1_c_& dl_ccch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void dl_ccch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void dl_ccch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_ccch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_reject:
      c.destroy<rrc_reject_s>();
      break;
    case types::rrc_setup:
      c.destroy<rrc_setup_s>();
      break;
    default:
      break;
  }
}
void dl_ccch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_reject:
      c.init<rrc_reject_s>();
      break;
    case types::rrc_setup:
      c.init<rrc_setup_s>();
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
}
dl_ccch_msg_type_c::c1_c_::c1_c_(const dl_ccch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_reject:
      c.init(other.c.get<rrc_reject_s>());
      break;
    case types::rrc_setup:
      c.init(other.c.get<rrc_setup_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
}
dl_ccch_msg_type_c::c1_c_& dl_ccch_msg_type_c::c1_c_::operator=(const dl_ccch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_reject:
      c.set(other.c.get<rrc_reject_s>());
      break;
    case types::rrc_setup:
      c.set(other.c.get<rrc_setup_s>());
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }

  return *this;
}
rrc_reject_s& dl_ccch_msg_type_c::c1_c_::set_rrc_reject()
{
  set(types::rrc_reject);
  return c.get<rrc_reject_s>();
}
rrc_setup_s& dl_ccch_msg_type_c::c1_c_::set_rrc_setup()
{
  set(types::rrc_setup);
  return c.get<rrc_setup_s>();
}
void dl_ccch_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_ccch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void dl_ccch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_reject:
      j.write_fieldname("rrcReject");
      c.get<rrc_reject_s>().to_json(j);
      break;
    case types::rrc_setup:
      j.write_fieldname("rrcSetup");
      c.get<rrc_setup_s>().to_json(j);
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.get<rrc_reject_s>().pack(bref));
      break;
    case types::rrc_setup:
      HANDLE_CODE(c.get<rrc_setup_s>().pack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_reject:
      HANDLE_CODE(c.get<rrc_reject_s>().unpack(bref));
      break;
    case types::rrc_setup:
      HANDLE_CODE(c.get<rrc_setup_s>().unpack(bref));
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcReject", "rrcSetup", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "dl_ccch_msg_type_c::c1_c_::types");
}

const char* dl_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_ccch_msg_type_c::types");
}
uint8_t dl_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_ccch_msg_type_c::types");
}

// DL-CCCH-Message ::= SEQUENCE
SRSASN_CODE dl_ccch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void dl_ccch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("DL-CCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
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
  static const char* options[] = {"ms4", "ms5", "ms10", "ms20", "ms40"};
  return convert_enum_idx(options, 5, value, "csi_rs_res_mob_s::slot_cfg_c_::types");
}
uint8_t csi_rs_res_mob_s::slot_cfg_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {4, 5, 10, 20, 40};
  return map_enum_number(options, 5, value, "csi_rs_res_mob_s::slot_cfg_c_::types");
}

void csi_rs_res_mob_s::freq_domain_alloc_c_::destroy_()
{
  switch (type_) {
    case types::row1:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::row2:
      c.destroy<fixed_bitstring<12> >();
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
      c.init<fixed_bitstring<4> >();
      break;
    case types::row2:
      c.init<fixed_bitstring<12> >();
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
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::row2:
      c.init(other.c.get<fixed_bitstring<12> >());
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
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::row2:
      c.set(other.c.get<fixed_bitstring<12> >());
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
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<12>& csi_rs_res_mob_s::freq_domain_alloc_c_::set_row2()
{
  set(types::row2);
  return c.get<fixed_bitstring<12> >();
}
void csi_rs_res_mob_s::freq_domain_alloc_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::row1:
      j.write_str("row1", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::row2:
      j.write_str("row2", c.get<fixed_bitstring<12> >().to_string());
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
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::row2:
      HANDLE_CODE(c.get<fixed_bitstring<12> >().pack(bref));
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
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::row2:
      HANDLE_CODE(c.get<fixed_bitstring<12> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_mob_s::freq_domain_alloc_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_rs_res_mob_s::freq_domain_alloc_c_::types_opts::to_string() const
{
  static const char* options[] = {"row1", "row2"};
  return convert_enum_idx(options, 2, value, "csi_rs_res_mob_s::freq_domain_alloc_c_::types");
}
uint8_t csi_rs_res_mob_s::freq_domain_alloc_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "csi_rs_res_mob_s::freq_domain_alloc_c_::types");
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
  static const char* options[] = {"size24", "size48", "size96", "size192", "size264"};
  return convert_enum_idx(options, 5, value, "csi_rs_cell_mob_s::csi_rs_meas_bw_s_::nrof_prbs_e_");
}
uint16_t csi_rs_cell_mob_s::csi_rs_meas_bw_s_::nrof_prbs_opts::to_number() const
{
  static const uint16_t options[] = {24, 48, 96, 192, 264};
  return map_enum_number(options, 5, value, "csi_rs_cell_mob_s::csi_rs_meas_bw_s_::nrof_prbs_e_");
}

const char* csi_rs_cell_mob_s::density_opts::to_string() const
{
  static const char* options[] = {"d1", "d3"};
  return convert_enum_idx(options, 2, value, "csi_rs_cell_mob_s::density_e_");
}
uint8_t csi_rs_cell_mob_s::density_opts::to_number() const
{
  static const uint8_t options[] = {1, 3};
  return map_enum_number(options, 2, value, "csi_rs_cell_mob_s::density_e_");
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

// FilterCoefficient ::= ENUMERATED
const char* filt_coef_opts::to_string() const
{
  static const char* options[] = {"fc0",
                                  "fc1",
                                  "fc2",
                                  "fc3",
                                  "fc4",
                                  "fc5",
                                  "fc6",
                                  "fc7",
                                  "fc8",
                                  "fc9",
                                  "fc11",
                                  "fc13",
                                  "fc15",
                                  "fc17",
                                  "fc19",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "filt_coef_e");
}
uint8_t filt_coef_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19};
  return map_enum_number(options, 15, value, "filt_coef_e");
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
  static const char* options[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(options, 3, value, "meas_trigger_quant_c::types");
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
  static const char* options[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(options, 3, value, "meas_trigger_quant_eutra_c::types");
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
  static const char* options[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(options, 3, value, "meas_trigger_quant_offset_c::types");
}

// NR-RS-Type ::= ENUMERATED
const char* nr_rs_type_opts::to_string() const
{
  static const char* options[] = {"ssb", "csi-rs"};
  return convert_enum_idx(options, 2, value, "nr_rs_type_e");
}

// ReportInterval ::= ENUMERATED
const char* report_interv_opts::to_string() const
{
  static const char* options[] = {"ms120",
                                  "ms240",
                                  "ms480",
                                  "ms640",
                                  "ms1024",
                                  "ms2048",
                                  "ms5120",
                                  "ms10240",
                                  "ms20480",
                                  "ms40960",
                                  "min1",
                                  "min6",
                                  "min12",
                                  "min30"};
  return convert_enum_idx(options, 14, value, "report_interv_e");
}
uint16_t report_interv_opts::to_number() const
{
  static const uint16_t options[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 1, 6, 12, 30};
  return map_enum_number(options, 14, value, "report_interv_e");
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
  j.end_obj();
}

// TimeToTrigger ::= ENUMERATED
const char* time_to_trigger_opts::to_string() const
{
  static const char* options[] = {"ms0",
                                  "ms40",
                                  "ms64",
                                  "ms80",
                                  "ms100",
                                  "ms128",
                                  "ms160",
                                  "ms256",
                                  "ms320",
                                  "ms480",
                                  "ms512",
                                  "ms640",
                                  "ms1024",
                                  "ms1280",
                                  "ms2560",
                                  "ms5120"};
  return convert_enum_idx(options, 16, value, "time_to_trigger_e");
}
uint16_t time_to_trigger_opts::to_number() const
{
  static const uint16_t options[] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};
  return map_enum_number(options, 16, value, "time_to_trigger_e");
}

// EUTRA-BlackCell ::= SEQUENCE
SRSASN_CODE eutra_black_cell_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_idx_eutra, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_black_cell_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_idx_eutra, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pci_range.unpack(bref));

  return SRSASN_SUCCESS;
}
void eutra_black_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellIndexEUTRA", cell_idx_eutra);
  j.write_fieldname("physCellIdRange");
  pci_range.to_json(j);
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
      j.write_bool("useWhiteCellList", c.get<event_a3_s_>().use_white_cell_list);
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
      j.write_bool("useWhiteCellList", c.get<event_a4_s_>().use_white_cell_list);
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
      j.write_bool("useWhiteCellList", c.get<event_a5_s_>().use_white_cell_list);
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
      j.write_bool("useWhiteCellList", c.get<event_a6_s_>().use_white_cell_list);
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
      HANDLE_CODE(bref.pack(c.get<event_a3_s_>().use_white_cell_list, 1));
      break;
    case types::event_a4:
      HANDLE_CODE(c.get<event_a4_s_>().a4_thres.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a4_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a4_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a4_s_>().time_to_trigger.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a4_s_>().use_white_cell_list, 1));
      break;
    case types::event_a5:
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres1.pack(bref));
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres2.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a5_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a5_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a5_s_>().time_to_trigger.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a5_s_>().use_white_cell_list, 1));
      break;
    case types::event_a6:
      HANDLE_CODE(c.get<event_a6_s_>().a6_offset.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a6_s_>().report_on_leave, 1));
      HANDLE_CODE(pack_integer(bref, c.get<event_a6_s_>().hysteresis, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a6_s_>().time_to_trigger.pack(bref));
      HANDLE_CODE(bref.pack(c.get<event_a6_s_>().use_white_cell_list, 1));
      break;
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
      HANDLE_CODE(bref.unpack(c.get<event_a3_s_>().use_white_cell_list, 1));
      break;
    case types::event_a4:
      HANDLE_CODE(c.get<event_a4_s_>().a4_thres.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a4_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a4_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a4_s_>().time_to_trigger.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a4_s_>().use_white_cell_list, 1));
      break;
    case types::event_a5:
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres1.unpack(bref));
      HANDLE_CODE(c.get<event_a5_s_>().a5_thres2.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a5_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a5_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a5_s_>().time_to_trigger.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a5_s_>().use_white_cell_list, 1));
      break;
    case types::event_a6:
      HANDLE_CODE(c.get<event_a6_s_>().a6_offset.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a6_s_>().report_on_leave, 1));
      HANDLE_CODE(unpack_integer(c.get<event_a6_s_>().hysteresis, bref, (uint8_t)0u, (uint8_t)30u));
      HANDLE_CODE(c.get<event_a6_s_>().time_to_trigger.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<event_a6_s_>().use_white_cell_list, 1));
      break;
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_s::event_id_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* event_trigger_cfg_s::event_id_c_::types_opts::to_string() const
{
  static const char* options[] = {"eventA1", "eventA2", "eventA3", "eventA4", "eventA5", "eventA6"};
  return convert_enum_idx(options, 6, value, "event_trigger_cfg_s::event_id_c_::types");
}
uint8_t event_trigger_cfg_s::event_id_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 6, value, "event_trigger_cfg_s::event_id_c_::types");
}

const char* event_trigger_cfg_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "event_trigger_cfg_s::report_amount_e_");
}
int8_t event_trigger_cfg_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "event_trigger_cfg_s::report_amount_e_");
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
    default:
      log_invalid_choice_id(type_, "event_trigger_cfg_inter_rat_s::event_id_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* event_trigger_cfg_inter_rat_s::event_id_c_::types_opts::to_string() const
{
  static const char* options[] = {"eventB1", "eventB2"};
  return convert_enum_idx(options, 2, value, "event_trigger_cfg_inter_rat_s::event_id_c_::types");
}
uint8_t event_trigger_cfg_inter_rat_s::event_id_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "event_trigger_cfg_inter_rat_s::event_id_c_::types");
}

const char* event_trigger_cfg_inter_rat_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "event_trigger_cfg_inter_rat_s::report_amount_e_");
}
int8_t event_trigger_cfg_inter_rat_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "event_trigger_cfg_inter_rat_s::report_amount_e_");
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
  HANDLE_CODE(bref.pack(use_white_cell_list, 1));

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
  HANDLE_CODE(bref.unpack(use_white_cell_list, 1));

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
  j.write_bool("useWhiteCellList", use_white_cell_list);
  j.end_obj();
}

const char* periodical_report_cfg_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "periodical_report_cfg_s::report_amount_e_");
}
int8_t periodical_report_cfg_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "periodical_report_cfg_s::report_amount_e_");
}

// PeriodicalReportConfigInterRAT ::= SEQUENCE
SRSASN_CODE periodical_report_cfg_inter_rat_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(report_interv.pack(bref));
  HANDLE_CODE(report_amount.pack(bref));
  HANDLE_CODE(report_quant.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_cells, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE periodical_report_cfg_inter_rat_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(report_interv.unpack(bref));
  HANDLE_CODE(report_amount.unpack(bref));
  HANDLE_CODE(report_quant.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_cells, bref, (uint8_t)1u, (uint8_t)8u));

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
  j.end_obj();
}

const char* periodical_report_cfg_inter_rat_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "periodical_report_cfg_inter_rat_s::report_amount_e_");
}
int8_t periodical_report_cfg_inter_rat_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "periodical_report_cfg_inter_rat_s::report_amount_e_");
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

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cgi_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)1007u));

  return SRSASN_SUCCESS;
}
void report_cgi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellForWhichToReportCGI", cell_for_which_to_report_cgi);
  j.end_obj();
}

// ReportCGI-EUTRA ::= SEQUENCE
SRSASN_CODE report_cgi_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)503u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE report_cgi_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)503u));

  return SRSASN_SUCCESS;
}
void report_cgi_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellForWhichToReportCGI", cell_for_which_to_report_cgi);
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
      bool cells_for_which_to_report_sftd_present;
      HANDLE_CODE(bref.unpack(cells_for_which_to_report_sftd_present, 1));
      cells_for_which_to_report_sftd.set_present(cells_for_which_to_report_sftd_present);
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
  static const char* options[] = {"sf5", "sf10", "sf20", "sf40", "sf80", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ssb_mtc2_s::periodicity_e_");
}
uint8_t ssb_mtc2_s::periodicity_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40, 80};
  return map_enum_number(options, 5, value, "ssb_mtc2_s::periodicity_e_");
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
  static const char* options[] = {"nr-SCG", "eutra-SCG"};
  return convert_enum_idx(options, 2, value, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::types");
}

// MeasObjectEUTRA ::= SEQUENCE
SRSASN_CODE meas_obj_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cells_to_rem_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(cells_to_add_mod_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(black_cells_to_rem_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(black_cells_to_add_mod_list_eutran.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_q_offset_range_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw.pack(bref));
  if (cells_to_rem_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_rem_list_eutran, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_to_add_mod_list_eutran, 1, 32));
  }
  if (black_cells_to_rem_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_rem_list_eutran, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (black_cells_to_add_mod_list_eutran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_add_mod_list_eutran, 1, 32));
  }
  HANDLE_CODE(bref.pack(eutra_presence_ant_port1, 1));
  if (eutra_q_offset_range_present) {
    HANDLE_CODE(eutra_q_offset_range.pack(bref));
  }
  HANDLE_CODE(bref.pack(wideband_rsrq_meas, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_obj_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cells_to_rem_list_eutran_present;
  HANDLE_CODE(bref.unpack(cells_to_rem_list_eutran_present, 1));
  bool cells_to_add_mod_list_eutran_present;
  HANDLE_CODE(bref.unpack(cells_to_add_mod_list_eutran_present, 1));
  bool black_cells_to_rem_list_eutran_present;
  HANDLE_CODE(bref.unpack(black_cells_to_rem_list_eutran_present, 1));
  bool black_cells_to_add_mod_list_eutran_present;
  HANDLE_CODE(bref.unpack(black_cells_to_add_mod_list_eutran_present, 1));
  HANDLE_CODE(bref.unpack(eutra_q_offset_range_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(allowed_meas_bw.unpack(bref));
  if (cells_to_rem_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_rem_list_eutran, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cells_to_add_mod_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_to_add_mod_list_eutran, bref, 1, 32));
  }
  if (black_cells_to_rem_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_rem_list_eutran, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (black_cells_to_add_mod_list_eutran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_add_mod_list_eutran, bref, 1, 32));
  }
  HANDLE_CODE(bref.unpack(eutra_presence_ant_port1, 1));
  if (eutra_q_offset_range_present) {
    HANDLE_CODE(eutra_q_offset_range.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(wideband_rsrq_meas, 1));

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
  if (black_cells_to_rem_list_eutran.size() > 0) {
    j.start_array("blackCellsToRemoveListEUTRAN");
    for (const auto& e1 : black_cells_to_rem_list_eutran) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (black_cells_to_add_mod_list_eutran.size() > 0) {
    j.start_array("blackCellsToAddModListEUTRAN");
    for (const auto& e1 : black_cells_to_add_mod_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_bool("eutra-PresenceAntennaPort1", eutra_presence_ant_port1);
  if (eutra_q_offset_range_present) {
    j.write_str("eutra-Q-OffsetRange", eutra_q_offset_range.to_string());
  }
  j.write_bool("widebandRSRQ-Meas", wideband_rsrq_meas);
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
  HANDLE_CODE(bref.pack(black_cells_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(black_cells_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(white_cells_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(white_cells_to_add_mod_list.size() > 0, 1));

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
  if (black_cells_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_rem_list, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (black_cells_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, black_cells_to_add_mod_list, 1, 8));
  }
  if (white_cells_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, white_cells_to_rem_list, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (white_cells_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, white_cells_to_add_mod_list, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= freq_band_ind_nr_present;
    group_flags[0] |= meas_cycle_scell_present;
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
  bool black_cells_to_rem_list_present;
  HANDLE_CODE(bref.unpack(black_cells_to_rem_list_present, 1));
  bool black_cells_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(black_cells_to_add_mod_list_present, 1));
  bool white_cells_to_rem_list_present;
  HANDLE_CODE(bref.unpack(white_cells_to_rem_list_present, 1));
  bool white_cells_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(white_cells_to_add_mod_list_present, 1));

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
  if (black_cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_rem_list, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (black_cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(black_cells_to_add_mod_list, bref, 1, 8));
  }
  if (white_cells_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(white_cells_to_rem_list, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (white_cells_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(white_cells_to_add_mod_list, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
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
  if (black_cells_to_rem_list.size() > 0) {
    j.start_array("blackCellsToRemoveList");
    for (const auto& e1 : black_cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (black_cells_to_add_mod_list.size() > 0) {
    j.start_array("blackCellsToAddModList");
    for (const auto& e1 : black_cells_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (white_cells_to_rem_list.size() > 0) {
    j.start_array("whiteCellsToRemoveList");
    for (const auto& e1 : white_cells_to_rem_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (white_cells_to_add_mod_list.size() > 0) {
    j.start_array("whiteCellsToAddModList");
    for (const auto& e1 : white_cells_to_add_mod_list) {
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
  }
  j.end_obj();
}

const char* meas_obj_nr_s::meas_cycle_scell_opts::to_string() const
{
  static const char* options[] = {"sf160", "sf256", "sf320", "sf512", "sf640", "sf1024", "sf1280"};
  return convert_enum_idx(options, 7, value, "meas_obj_nr_s::meas_cycle_scell_e_");
}
uint16_t meas_obj_nr_s::meas_cycle_scell_opts::to_number() const
{
  static const uint16_t options[] = {160, 256, 320, 512, 640, 1024, 1280};
  return map_enum_number(options, 7, value, "meas_obj_nr_s::meas_cycle_scell_e_");
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
  static const char* options[] = {"s0",
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
  return convert_enum_idx(options, 16, value, "overheat_assist_cfg_s::overheat_ind_prohibit_timer_e_");
}
float overheat_assist_cfg_s::overheat_ind_prohibit_timer_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(options, 13, value, "overheat_assist_cfg_s::overheat_ind_prohibit_timer_e_");
}
const char* overheat_assist_cfg_s::overheat_ind_prohibit_timer_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(options, 16, value, "overheat_assist_cfg_s::overheat_ind_prohibit_timer_e_");
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
  static const char* options[] = {"periodical", "eventTriggered", "reportCGI", "reportSFTD"};
  return convert_enum_idx(options, 4, value, "report_cfg_inter_rat_s::report_type_c_::types");
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
    default:
      log_invalid_choice_id(type_, "report_cfg_nr_s::report_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* report_cfg_nr_s::report_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"periodical", "eventTriggered", "reportCGI", "reportSFTD"};
  return convert_enum_idx(options, 4, value, "report_cfg_nr_s::report_type_c_::types");
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
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ref_serv_cell_ind_present, 1));
      if (ref_serv_cell_ind_present) {
        HANDLE_CODE(ref_serv_cell_ind.pack(bref));
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ref_serv_cell_ind_present, 1));
      if (ref_serv_cell_ind_present) {
        HANDLE_CODE(ref_serv_cell_ind.unpack(bref));
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
  }
  j.end_obj();
}

const char* gap_cfg_s::mgl_opts::to_string() const
{
  static const char* options[] = {"ms1dot5", "ms3", "ms3dot5", "ms4", "ms5dot5", "ms6"};
  return convert_enum_idx(options, 6, value, "gap_cfg_s::mgl_e_");
}
float gap_cfg_s::mgl_opts::to_number() const
{
  static const float options[] = {1.5, 3.0, 3.5, 4.0, 5.5, 6.0};
  return map_enum_number(options, 6, value, "gap_cfg_s::mgl_e_");
}
const char* gap_cfg_s::mgl_opts::to_number_string() const
{
  static const char* options[] = {"1.5", "3", "3.5", "4", "5.5", "6"};
  return convert_enum_idx(options, 6, value, "gap_cfg_s::mgl_e_");
}

const char* gap_cfg_s::mgrp_opts::to_string() const
{
  static const char* options[] = {"ms20", "ms40", "ms80", "ms160"};
  return convert_enum_idx(options, 4, value, "gap_cfg_s::mgrp_e_");
}
uint8_t gap_cfg_s::mgrp_opts::to_number() const
{
  static const uint8_t options[] = {20, 40, 80, 160};
  return map_enum_number(options, 4, value, "gap_cfg_s::mgrp_e_");
}

const char* gap_cfg_s::mgta_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms0dot25", "ms0dot5"};
  return convert_enum_idx(options, 3, value, "gap_cfg_s::mgta_e_");
}
float gap_cfg_s::mgta_opts::to_number() const
{
  static const float options[] = {0.0, 0.25, 0.5};
  return map_enum_number(options, 3, value, "gap_cfg_s::mgta_e_");
}
const char* gap_cfg_s::mgta_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.25", "0.5"};
  return convert_enum_idx(options, 3, value, "gap_cfg_s::mgta_e_");
}

const char* gap_cfg_s::ref_serv_cell_ind_opts::to_string() const
{
  static const char* options[] = {"pCell", "pSCell", "mcg-FR2"};
  return convert_enum_idx(options, 3, value, "gap_cfg_s::ref_serv_cell_ind_e_");
}
uint8_t gap_cfg_s::ref_serv_cell_ind_opts::to_number() const
{
  if (value == mcg_fr2) {
    return 2;
  }
  invalid_enum_number(value, "gap_cfg_s::ref_serv_cell_ind_e_");
  return 0;
}

// MeasGapSharingScheme ::= ENUMERATED
const char* meas_gap_sharing_scheme_opts::to_string() const
{
  static const char* options[] = {"scheme00", "scheme01", "scheme10", "scheme11"};
  return convert_enum_idx(options, 4, value, "meas_gap_sharing_scheme_e");
}
float meas_gap_sharing_scheme_opts::to_number() const
{
  static const float options[] = {0.0, 0.1, 1.0, 1.1};
  return map_enum_number(options, 4, value, "meas_gap_sharing_scheme_e");
}
const char* meas_gap_sharing_scheme_opts::to_number_string() const
{
  static const char* options[] = {"0.0", "0.1", "1.0", "1.1"};
  return convert_enum_idx(options, 4, value, "meas_gap_sharing_scheme_e");
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
    default:
      log_invalid_choice_id(type_, "meas_obj_to_add_mod_s::meas_obj_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_obj_to_add_mod_s::meas_obj_c_::types_opts::to_string() const
{
  static const char* options[] = {"measObjectNR", "measObjectEUTRA"};
  return convert_enum_idx(options, 2, value, "meas_obj_to_add_mod_s::meas_obj_c_::types");
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

// RAT-Type ::= ENUMERATED
const char* rat_type_opts::to_string() const
{
  static const char* options[] = {"nr", "eutra-nr", "eutra", "spare1"};
  return convert_enum_idx(options, 4, value, "rat_type_e");
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
    default:
      log_invalid_choice_id(type_, "report_cfg_to_add_mod_s::report_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* report_cfg_to_add_mod_s::report_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"reportConfigNR", "reportConfigInterRAT"};
  return convert_enum_idx(options, 2, value, "report_cfg_to_add_mod_s::report_cfg_c_::types");
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool gap_fr1_present;
      HANDLE_CODE(bref.unpack(gap_fr1_present, 1));
      gap_fr1.set_present(gap_fr1_present);
      bool gap_ue_present;
      HANDLE_CODE(bref.unpack(gap_ue_present, 1));
      gap_ue.set_present(gap_ue_present);
      if (gap_fr1.is_present()) {
        HANDLE_CODE(gap_fr1->unpack(bref));
      }
      if (gap_ue.is_present()) {
        HANDLE_CODE(gap_ue->unpack(bref));
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

      bool gap_sharing_fr1_present;
      HANDLE_CODE(bref.unpack(gap_sharing_fr1_present, 1));
      gap_sharing_fr1.set_present(gap_sharing_fr1_present);
      bool gap_sharing_ue_present;
      HANDLE_CODE(bref.unpack(gap_sharing_ue_present, 1));
      gap_sharing_ue.set_present(gap_sharing_ue_present);
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
  static const char* options[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(
      options, 8, value, "other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_e_");
}
float other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(
      options, 8, value, "other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_e_");
}
const char*
other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(
      options, 8, value, "other_cfg_s::delay_budget_report_cfg_c_::setup_s_::delay_budget_report_prohibit_timer_e_");
}

const char* other_cfg_s::delay_budget_report_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"release", "setup"};
  return convert_enum_idx(options, 2, value, "other_cfg_s::delay_budget_report_cfg_c_::types");
}

// PeriodicRNAU-TimerValue ::= ENUMERATED
const char* periodic_rnau_timer_value_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min360", "min720"};
  return convert_enum_idx(options, 8, value, "periodic_rnau_timer_value_e");
}
uint16_t periodic_rnau_timer_value_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 30, 60, 120, 360, 720};
  return map_enum_number(options, 8, value, "periodic_rnau_timer_value_e");
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
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(quant_cfg_eutra.is_present(), 1));
      if (quant_cfg_eutra.is_present()) {
        HANDLE_CODE(quant_cfg_eutra->pack(bref));
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool quant_cfg_eutra_present;
      HANDLE_CODE(bref.unpack(quant_cfg_eutra_present, 1));
      quant_cfg_eutra.set_present(quant_cfg_eutra_present);
      if (quant_cfg_eutra.is_present()) {
        HANDLE_CODE(quant_cfg_eutra->unpack(bref));
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

const char* ran_notif_area_info_c::types_opts::to_string() const
{
  static const char* options[] = {"cellList", "ran-AreaConfigList"};
  return convert_enum_idx(options, 2, value, "ran_notif_area_info_c::types");
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
  static const char* options[] = {"epc", "fiveGC"};
  return convert_enum_idx(options, 2, value, "redirected_carrier_info_eutra_s::cn_type_e_");
}
uint8_t redirected_carrier_info_eutra_s::cn_type_opts::to_number() const
{
  if (value == five_gc) {
    return 5;
  }
  invalid_enum_number(value, "redirected_carrier_info_eutra_s::cn_type_e_");
  return 0;
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
  j.end_obj();
}

const char* cell_resel_priorities_s::t320_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min180", "spare1"};
  return convert_enum_idx(options, 8, value, "cell_resel_priorities_s::t320_e_");
}
uint8_t cell_resel_priorities_s::t320_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return map_enum_number(options, 7, value, "cell_resel_priorities_s::t320_e_");
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
  static const char* options[] = {"ssb-RSRP", "csi-RSRP"};
  return convert_enum_idx(options, 2, value, "meas_cfg_s::s_measure_cfg_c_::types");
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

// RRCRelease-v1540-IEs ::= SEQUENCE
SRSASN_CODE rrc_release_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wait_time_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (wait_time_present) {
    HANDLE_CODE(pack_integer(bref, wait_time, (uint8_t)1u, (uint8_t)16u));
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
    j.start_obj();
    j.end_obj();
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
  static const char* options[] = {"nr", "eutra"};
  return convert_enum_idx(options, 2, value, "redirected_carrier_info_c::types");
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
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
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
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* mob_from_nr_cmd_ies_s::target_rat_type_opts::to_string() const
{
  static const char* options[] = {"eutra", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "mob_from_nr_cmd_ies_s::target_rat_type_e_");
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
    j.start_obj();
    j.end_obj();
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
  static const char* options[] = {"frequency", "nr"};
  return convert_enum_idx(options, 2, value, "rrc_release_ies_s::depriorit_req_s_::depriorit_type_e_");
}

const char* rrc_release_ies_s::depriorit_req_s_::depriorit_timer_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min15", "min30"};
  return convert_enum_idx(options, 4, value, "rrc_release_ies_s::depriorit_req_s_::depriorit_timer_e_");
}
uint8_t rrc_release_ies_s::depriorit_req_s_::depriorit_timer_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 15, 30};
  return map_enum_number(options, 4, value, "rrc_release_ies_s::depriorit_req_s_::depriorit_timer_e_");
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
  static const char* options[] = {"counterCheck", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "counter_check_s::crit_exts_c_::types");
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
  static const char* options[] = {"dlInformationTransfer", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "dl_info_transfer_s::crit_exts_c_::types");
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
  static const char* options[] = {"mobilityFromNRCommand", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "mob_from_nr_cmd_s::crit_exts_c_::types");
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
  static const char* options[] = {"rrcReconfiguration", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_recfg_s::crit_exts_c_::types");
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
  static const char* options[] = {"rrcReestablishment", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_reest_s::crit_exts_c_::types");
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
  static const char* options[] = {"rrcRelease", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_release_s::crit_exts_c_::types");
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
  static const char* options[] = {"rrcResume", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_resume_s::crit_exts_c_::types");
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
  static const char* options[] = {"securityModeCommand", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "security_mode_cmd_s::crit_exts_c_::types");
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
  static const char* options[] = {"ueCapabilityEnquiry", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ue_cap_enquiry_s::crit_exts_c_::types");
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
void dl_dcch_msg_type_c::c1_c_::set_spare7()
{
  set(types::spare7);
}
void dl_dcch_msg_type_c::c1_c_::set_spare6()
{
  set(types::spare6);
}
void dl_dcch_msg_type_c::c1_c_::set_spare5()
{
  set(types::spare5);
}
void dl_dcch_msg_type_c::c1_c_::set_spare4()
{
  set(types::spare4);
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
      log_invalid_choice_id(type_, "dl_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcReconfiguration",
                                  "rrcResume",
                                  "rrcRelease",
                                  "rrcReestablishment",
                                  "securityModeCommand",
                                  "dlInformationTransfer",
                                  "ueCapabilityEnquiry",
                                  "counterCheck",
                                  "mobilityFromNRCommand",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "dl_dcch_msg_type_c::c1_c_::types");
}

const char* dl_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_dcch_msg_type_c::types");
}
uint8_t dl_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_dcch_msg_type_c::types");
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

// PagingUE-Identity ::= CHOICE
void paging_ue_id_c::destroy_()
{
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.destroy<fixed_bitstring<48> >();
      break;
    case types::full_i_rnti:
      c.destroy<fixed_bitstring<40> >();
      break;
    default:
      break;
  }
}
void paging_ue_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.init<fixed_bitstring<48> >();
      break;
    case types::full_i_rnti:
      c.init<fixed_bitstring<40> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
}
paging_ue_id_c::paging_ue_id_c(const paging_ue_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.init(other.c.get<fixed_bitstring<48> >());
      break;
    case types::full_i_rnti:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
}
paging_ue_id_c& paging_ue_id_c::operator=(const paging_ue_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.set(other.c.get<fixed_bitstring<48> >());
      break;
    case types::full_i_rnti:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }

  return *this;
}
fixed_bitstring<48>& paging_ue_id_c::set_ng_minus5_g_s_tmsi()
{
  set(types::ng_minus5_g_s_tmsi);
  return c.get<fixed_bitstring<48> >();
}
fixed_bitstring<40>& paging_ue_id_c::set_full_i_rnti()
{
  set(types::full_i_rnti);
  return c.get<fixed_bitstring<40> >();
}
void paging_ue_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      j.write_str("ng-5G-S-TMSI", c.get<fixed_bitstring<48> >().to_string());
      break;
    case types::full_i_rnti:
      j.write_str("fullI-RNTI", c.get<fixed_bitstring<40> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
  j.end_obj();
}
SRSASN_CODE paging_ue_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48> >().pack(bref));
      break;
    case types::full_i_rnti:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_ue_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48> >().unpack(bref));
      break;
    case types::full_i_rnti:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* paging_ue_id_c::types_opts::to_string() const
{
  static const char* options[] = {"ng-5G-S-TMSI", "fullI-RNTI"};
  return convert_enum_idx(options, 2, value, "paging_ue_id_c::types");
}
int8_t paging_ue_id_c::types_opts::to_number() const
{
  static const int8_t options[] = {-5};
  return map_enum_number(options, 1, value, "paging_ue_id_c::types");
}

// PagingRecord ::= SEQUENCE
SRSASN_CODE paging_record_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(access_type_present, 1));

  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_record_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(access_type_present, 1));

  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void paging_record_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  if (access_type_present) {
    j.write_str("accessType", "non3GPP");
  }
  j.end_obj();
}

// Paging ::= SEQUENCE
SRSASN_CODE paging_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_record_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (paging_record_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_record_list, 1, 32));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_s::unpack(cbit_ref& bref)
{
  bool paging_record_list_present;
  HANDLE_CODE(bref.unpack(paging_record_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (paging_record_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_record_list, bref, 1, 32));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_record_list.size() > 0) {
    j.start_array("pagingRecordList");
    for (const auto& e1 : paging_record_list) {
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

// PCCH-MessageType ::= CHOICE
void pcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
pcch_msg_type_c::c1_c_& pcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void pcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void pcch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "pcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "pcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pcch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
}
paging_s& pcch_msg_type_c::c1_c_::set_paging()
{
  set(types::paging);
  return c;
}
void pcch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void pcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::paging:
      j.write_fieldname("paging");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::paging:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::paging:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"paging", "spare1"};
  return convert_enum_idx(options, 2, value, "pcch_msg_type_c::c1_c_::types");
}

const char* pcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "pcch_msg_type_c::types");
}
uint8_t pcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "pcch_msg_type_c::types");
}

// PCCH-Message ::= SEQUENCE
SRSASN_CODE pcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void pcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("PCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// EstablishmentCause ::= ENUMERATED
const char* establishment_cause_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "mo-VoiceCall",
                                  "mo-VideoCall",
                                  "mo-SMS",
                                  "mps-PriorityAccess",
                                  "mcs-PriorityAccess",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "establishment_cause_e");
}

// InitialUE-Identity ::= CHOICE
void init_ue_id_c::destroy_()
{
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.destroy<fixed_bitstring<39> >();
      break;
    case types::random_value:
      c.destroy<fixed_bitstring<39> >();
      break;
    default:
      break;
  }
}
void init_ue_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.init<fixed_bitstring<39> >();
      break;
    case types::random_value:
      c.init<fixed_bitstring<39> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c::init_ue_id_c(const init_ue_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.init(other.c.get<fixed_bitstring<39> >());
      break;
    case types::random_value:
      c.init(other.c.get<fixed_bitstring<39> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c& init_ue_id_c::operator=(const init_ue_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.set(other.c.get<fixed_bitstring<39> >());
      break;
    case types::random_value:
      c.set(other.c.get<fixed_bitstring<39> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }

  return *this;
}
fixed_bitstring<39>& init_ue_id_c::set_ng_minus5_g_s_tmsi_part1()
{
  set(types::ng_minus5_g_s_tmsi_part1);
  return c.get<fixed_bitstring<39> >();
}
fixed_bitstring<39>& init_ue_id_c::set_random_value()
{
  set(types::random_value);
  return c.get<fixed_bitstring<39> >();
}
void init_ue_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      j.write_str("ng-5G-S-TMSI-Part1", c.get<fixed_bitstring<39> >().to_string());
      break;
    case types::random_value:
      j.write_str("randomValue", c.get<fixed_bitstring<39> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
  j.end_obj();
}
SRSASN_CODE init_ue_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<39> >().pack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<39> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE init_ue_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<39> >().unpack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<39> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* init_ue_id_c::types_opts::to_string() const
{
  static const char* options[] = {"ng-5G-S-TMSI-Part1", "randomValue"};
  return convert_enum_idx(options, 2, value, "init_ue_id_c::types");
}
int8_t init_ue_id_c::types_opts::to_number() const
{
  static const int8_t options[] = {-5};
  return map_enum_number(options, 1, value, "init_ue_id_c::types");
}

// ReestabUE-Identity ::= SEQUENCE
SRSASN_CODE reestab_ue_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, c_rnti, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(short_mac_i.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reestab_ue_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(short_mac_i.unpack(bref));

  return SRSASN_SUCCESS;
}
void reestab_ue_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("c-RNTI", c_rnti);
  j.write_int("physCellId", pci);
  j.write_str("shortMAC-I", short_mac_i.to_string());
  j.end_obj();
}

// ReestablishmentCause ::= ENUMERATED
const char* reest_cause_opts::to_string() const
{
  static const char* options[] = {"reconfigurationFailure", "handoverFailure", "otherFailure", "spare1"};
  return convert_enum_idx(options, 4, value, "reest_cause_e");
}

// ResumeCause ::= ENUMERATED
const char* resume_cause_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "mo-VoiceCall",
                                  "mo-VideoCall",
                                  "mo-SMS",
                                  "rna-Update",
                                  "mps-PriorityAccess",
                                  "mcs-PriorityAccess",
                                  "spare1",
                                  "spare2",
                                  "spare3",
                                  "spare4",
                                  "spare5"};
  return convert_enum_idx(options, 16, value, "resume_cause_e");
}

// RRCReestablishmentRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(reest_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(reest_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("reestablishmentCause", reest_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCResumeRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id.pack(bref));
  HANDLE_CODE(resume_mac_i.pack(bref));
  HANDLE_CODE(resume_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id.unpack(bref));
  HANDLE_CODE(resume_mac_i.unpack(bref));
  HANDLE_CODE(resume_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("resumeIdentity", resume_id.to_string());
  j.write_str("resumeMAC-I", resume_mac_i.to_string());
  j.write_str("resumeCause", resume_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCSetupRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(establishment_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(establishment_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("establishmentCause", establishment_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCSystemInfoRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_sys_info_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(requested_si_list.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(requested_si_list.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_sys_info_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("requested-SI-List", requested_si_list.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCReestablishmentRequest ::= SEQUENCE
SRSASN_CODE rrc_reest_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_reest_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_reest_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcReestablishmentRequest");
  rrc_reest_request.to_json(j);
  j.end_obj();
}

// RRCResumeRequest ::= SEQUENCE
SRSASN_CODE rrc_resume_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_resume_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_resume_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcResumeRequest");
  rrc_resume_request.to_json(j);
  j.end_obj();
}

// RRCSetupRequest ::= SEQUENCE
SRSASN_CODE rrc_setup_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_setup_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_setup_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcSetupRequest");
  rrc_setup_request.to_json(j);
  j.end_obj();
}

// RRCSystemInfoRequest ::= SEQUENCE
SRSASN_CODE rrc_sys_info_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_sys_info_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_sys_info_request_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_sys_info_request_ies_s& rrc_sys_info_request_s::crit_exts_c_::set_rrc_sys_info_request()
{
  set(types::rrc_sys_info_request);
  return c;
}
void rrc_sys_info_request_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_sys_info_request_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_sys_info_request:
      j.write_fieldname("rrcSystemInfoRequest");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_sys_info_request_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcSystemInfoRequest", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_sys_info_request_s::crit_exts_c_::types");
}

// UL-CCCH-MessageType ::= CHOICE
void ul_ccch_msg_type_c::set(types::options e)
{
  type_ = e;
}
ul_ccch_msg_type_c::c1_c_& ul_ccch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void ul_ccch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void ul_ccch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_setup_request:
      c.destroy<rrc_setup_request_s>();
      break;
    case types::rrc_resume_request:
      c.destroy<rrc_resume_request_s>();
      break;
    case types::rrc_reest_request:
      c.destroy<rrc_reest_request_s>();
      break;
    case types::rrc_sys_info_request:
      c.destroy<rrc_sys_info_request_s>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_setup_request:
      c.init<rrc_setup_request_s>();
      break;
    case types::rrc_resume_request:
      c.init<rrc_resume_request_s>();
      break;
    case types::rrc_reest_request:
      c.init<rrc_reest_request_s>();
      break;
    case types::rrc_sys_info_request:
      c.init<rrc_sys_info_request_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
}
ul_ccch_msg_type_c::c1_c_::c1_c_(const ul_ccch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_setup_request:
      c.init(other.c.get<rrc_setup_request_s>());
      break;
    case types::rrc_resume_request:
      c.init(other.c.get<rrc_resume_request_s>());
      break;
    case types::rrc_reest_request:
      c.init(other.c.get<rrc_reest_request_s>());
      break;
    case types::rrc_sys_info_request:
      c.init(other.c.get<rrc_sys_info_request_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
}
ul_ccch_msg_type_c::c1_c_& ul_ccch_msg_type_c::c1_c_::operator=(const ul_ccch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_setup_request:
      c.set(other.c.get<rrc_setup_request_s>());
      break;
    case types::rrc_resume_request:
      c.set(other.c.get<rrc_resume_request_s>());
      break;
    case types::rrc_reest_request:
      c.set(other.c.get<rrc_reest_request_s>());
      break;
    case types::rrc_sys_info_request:
      c.set(other.c.get<rrc_sys_info_request_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }

  return *this;
}
rrc_setup_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_setup_request()
{
  set(types::rrc_setup_request);
  return c.get<rrc_setup_request_s>();
}
rrc_resume_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_resume_request()
{
  set(types::rrc_resume_request);
  return c.get<rrc_resume_request_s>();
}
rrc_reest_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_reest_request()
{
  set(types::rrc_reest_request);
  return c.get<rrc_reest_request_s>();
}
rrc_sys_info_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_sys_info_request()
{
  set(types::rrc_sys_info_request);
  return c.get<rrc_sys_info_request_s>();
}
void ul_ccch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_setup_request:
      j.write_fieldname("rrcSetupRequest");
      c.get<rrc_setup_request_s>().to_json(j);
      break;
    case types::rrc_resume_request:
      j.write_fieldname("rrcResumeRequest");
      c.get<rrc_resume_request_s>().to_json(j);
      break;
    case types::rrc_reest_request:
      j.write_fieldname("rrcReestablishmentRequest");
      c.get<rrc_reest_request_s>().to_json(j);
      break;
    case types::rrc_sys_info_request:
      j.write_fieldname("rrcSystemInfoRequest");
      c.get<rrc_sys_info_request_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_setup_request:
      HANDLE_CODE(c.get<rrc_setup_request_s>().pack(bref));
      break;
    case types::rrc_resume_request:
      HANDLE_CODE(c.get<rrc_resume_request_s>().pack(bref));
      break;
    case types::rrc_reest_request:
      HANDLE_CODE(c.get<rrc_reest_request_s>().pack(bref));
      break;
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_setup_request:
      HANDLE_CODE(c.get<rrc_setup_request_s>().unpack(bref));
      break;
    case types::rrc_resume_request:
      HANDLE_CODE(c.get<rrc_resume_request_s>().unpack(bref));
      break;
    case types::rrc_reest_request:
      HANDLE_CODE(c.get<rrc_reest_request_s>().unpack(bref));
      break;
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "rrcSetupRequest", "rrcResumeRequest", "rrcReestablishmentRequest", "rrcSystemInfoRequest"};
  return convert_enum_idx(options, 4, value, "ul_ccch_msg_type_c::c1_c_::types");
}

const char* ul_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_c::types");
}
uint8_t ul_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_ccch_msg_type_c::types");
}

// UL-CCCH-Message ::= SEQUENCE
SRSASN_CODE ul_ccch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_ccch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-CCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// RRCResumeRequest1-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_request1_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id.pack(bref));
  HANDLE_CODE(resume_mac_i.pack(bref));
  HANDLE_CODE(resume_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request1_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id.unpack(bref));
  HANDLE_CODE(resume_mac_i.unpack(bref));
  HANDLE_CODE(resume_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request1_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("resumeIdentity", resume_id.to_string());
  j.write_str("resumeMAC-I", resume_mac_i.to_string());
  j.write_str("resumeCause", resume_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCResumeRequest1 ::= SEQUENCE
SRSASN_CODE rrc_resume_request1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_resume_request1.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_resume_request1.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcResumeRequest1");
  rrc_resume_request1.to_json(j);
  j.end_obj();
}

// UL-CCCH1-MessageType ::= CHOICE
void ul_ccch1_msg_type_c::set(types::options e)
{
  type_ = e;
}
ul_ccch1_msg_type_c::c1_c_& ul_ccch1_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void ul_ccch1_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void ul_ccch1_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch1_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch1_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch1_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_resume_request1_s& ul_ccch1_msg_type_c::c1_c_::set_rrc_resume_request1()
{
  set(types::rrc_resume_request1);
  return c;
}
void ul_ccch1_msg_type_c::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_ccch1_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_ccch1_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_ccch1_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_resume_request1:
      j.write_fieldname("rrcResumeRequest1");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch1_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_resume_request1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch1_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_resume_request1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_ccch1_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcResumeRequest1", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_ccch1_msg_type_c::c1_c_::types");
}
uint8_t ul_ccch1_msg_type_c::c1_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_ccch1_msg_type_c::c1_c_::types");
}

const char* ul_ccch1_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_ccch1_msg_type_c::types");
}
uint8_t ul_ccch1_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_ccch1_msg_type_c::types");
}

// UL-CCCH1-Message ::= SEQUENCE
SRSASN_CODE ul_ccch1_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch1_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_ccch1_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-CCCH1-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// MeasQuantityResults ::= SEQUENCE
SRSASN_CODE meas_quant_results_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_present, 1));
  HANDLE_CODE(bref.pack(rsrq_present, 1));
  HANDLE_CODE(bref.pack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(pack_integer(bref, rsrp, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrq_present) {
    HANDLE_CODE(pack_integer(bref, rsrq, (uint8_t)0u, (uint8_t)127u));
  }
  if (sinr_present) {
    HANDLE_CODE(pack_integer(bref, sinr, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_quant_results_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_present, 1));
  HANDLE_CODE(bref.unpack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(unpack_integer(rsrp, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrq_present) {
    HANDLE_CODE(unpack_integer(rsrq, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (sinr_present) {
    HANDLE_CODE(unpack_integer(sinr, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_quant_results_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_present) {
    j.write_int("rsrp", rsrp);
  }
  if (rsrq_present) {
    j.write_int("rsrq", rsrq);
  }
  if (sinr_present) {
    j.write_int("sinr", sinr);
  }
  j.end_obj();
}

// PLMN-Identity-EUTRA-5GC ::= CHOICE
void plmn_id_eutra_minus5_gc_c::destroy_()
{
  switch (type_) {
    case types::plmn_id_eutra_minus5_gc:
      c.destroy<plmn_id_s>();
      break;
    default:
      break;
  }
}
void plmn_id_eutra_minus5_gc_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_id_eutra_minus5_gc:
      c.init<plmn_id_s>();
      break;
    case types::plmn_idx:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_minus5_gc_c");
  }
}
plmn_id_eutra_minus5_gc_c::plmn_id_eutra_minus5_gc_c(const plmn_id_eutra_minus5_gc_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_id_eutra_minus5_gc:
      c.init(other.c.get<plmn_id_s>());
      break;
    case types::plmn_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_minus5_gc_c");
  }
}
plmn_id_eutra_minus5_gc_c& plmn_id_eutra_minus5_gc_c::operator=(const plmn_id_eutra_minus5_gc_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_id_eutra_minus5_gc:
      c.set(other.c.get<plmn_id_s>());
      break;
    case types::plmn_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_minus5_gc_c");
  }

  return *this;
}
plmn_id_s& plmn_id_eutra_minus5_gc_c::set_plmn_id_eutra_minus5_gc()
{
  set(types::plmn_id_eutra_minus5_gc);
  return c.get<plmn_id_s>();
}
uint8_t& plmn_id_eutra_minus5_gc_c::set_plmn_idx()
{
  set(types::plmn_idx);
  return c.get<uint8_t>();
}
void plmn_id_eutra_minus5_gc_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_id_eutra_minus5_gc:
      j.write_fieldname("plmn-Identity-EUTRA-5GC");
      c.get<plmn_id_s>().to_json(j);
      break;
    case types::plmn_idx:
      j.write_int("plmn-index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_minus5_gc_c");
  }
  j.end_obj();
}
SRSASN_CODE plmn_id_eutra_minus5_gc_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_id_eutra_minus5_gc:
      HANDLE_CODE(c.get<plmn_id_s>().pack(bref));
      break;
    case types::plmn_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_minus5_gc_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_eutra_minus5_gc_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_id_eutra_minus5_gc:
      HANDLE_CODE(c.get<plmn_id_s>().unpack(bref));
      break;
    case types::plmn_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_eutra_minus5_gc_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* plmn_id_eutra_minus5_gc_c::types_opts::to_string() const
{
  static const char* options[] = {"plmn-Identity-EUTRA-5GC", "plmn-index"};
  return convert_enum_idx(options, 2, value, "plmn_id_eutra_minus5_gc_c::types");
}
int8_t plmn_id_eutra_minus5_gc_c::types_opts::to_number() const
{
  static const int8_t options[] = {-5};
  return map_enum_number(options, 1, value, "plmn_id_eutra_minus5_gc_c::types");
}

// CellIdentity-EUTRA-5GC ::= CHOICE
void cell_id_eutra_minus5_gc_c::destroy_()
{
  switch (type_) {
    case types::cell_id_eutra:
      c.destroy<fixed_bitstring<28> >();
      break;
    default:
      break;
  }
}
void cell_id_eutra_minus5_gc_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_id_eutra:
      c.init<fixed_bitstring<28> >();
      break;
    case types::cell_id_idx:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_minus5_gc_c");
  }
}
cell_id_eutra_minus5_gc_c::cell_id_eutra_minus5_gc_c(const cell_id_eutra_minus5_gc_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_id_eutra:
      c.init(other.c.get<fixed_bitstring<28> >());
      break;
    case types::cell_id_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_minus5_gc_c");
  }
}
cell_id_eutra_minus5_gc_c& cell_id_eutra_minus5_gc_c::operator=(const cell_id_eutra_minus5_gc_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_id_eutra:
      c.set(other.c.get<fixed_bitstring<28> >());
      break;
    case types::cell_id_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_minus5_gc_c");
  }

  return *this;
}
fixed_bitstring<28>& cell_id_eutra_minus5_gc_c::set_cell_id_eutra()
{
  set(types::cell_id_eutra);
  return c.get<fixed_bitstring<28> >();
}
uint8_t& cell_id_eutra_minus5_gc_c::set_cell_id_idx()
{
  set(types::cell_id_idx);
  return c.get<uint8_t>();
}
void cell_id_eutra_minus5_gc_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_id_eutra:
      j.write_str("cellIdentity-EUTRA", c.get<fixed_bitstring<28> >().to_string());
      break;
    case types::cell_id_idx:
      j.write_int("cellId-index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_minus5_gc_c");
  }
  j.end_obj();
}
SRSASN_CODE cell_id_eutra_minus5_gc_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_id_eutra:
      HANDLE_CODE(c.get<fixed_bitstring<28> >().pack(bref));
      break;
    case types::cell_id_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_minus5_gc_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_id_eutra_minus5_gc_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_id_eutra:
      HANDLE_CODE(c.get<fixed_bitstring<28> >().unpack(bref));
      break;
    case types::cell_id_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)12u));
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_eutra_minus5_gc_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cell_id_eutra_minus5_gc_c::types_opts::to_string() const
{
  static const char* options[] = {"cellIdentity-EUTRA", "cellId-index"};
  return convert_enum_idx(options, 2, value, "cell_id_eutra_minus5_gc_c::types");
}

// ResultsPerCSI-RS-Index ::= SEQUENCE
SRSASN_CODE results_per_csi_rs_idx_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(csi_rs_results_present, 1));

  HANDLE_CODE(pack_integer(bref, csi_rs_idx, (uint8_t)0u, (uint8_t)95u));
  if (csi_rs_results_present) {
    HANDLE_CODE(csi_rs_results.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE results_per_csi_rs_idx_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(csi_rs_results_present, 1));

  HANDLE_CODE(unpack_integer(csi_rs_idx, bref, (uint8_t)0u, (uint8_t)95u));
  if (csi_rs_results_present) {
    HANDLE_CODE(csi_rs_results.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void results_per_csi_rs_idx_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS-Index", csi_rs_idx);
  if (csi_rs_results_present) {
    j.write_fieldname("csi-RS-Results");
    csi_rs_results.to_json(j);
  }
  j.end_obj();
}

// ResultsPerSSB-Index ::= SEQUENCE
SRSASN_CODE results_per_ssb_idx_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_results_present, 1));

  HANDLE_CODE(pack_integer(bref, ssb_idx, (uint8_t)0u, (uint8_t)63u));
  if (ssb_results_present) {
    HANDLE_CODE(ssb_results.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE results_per_ssb_idx_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_results_present, 1));

  HANDLE_CODE(unpack_integer(ssb_idx, bref, (uint8_t)0u, (uint8_t)63u));
  if (ssb_results_present) {
    HANDLE_CODE(ssb_results.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void results_per_ssb_idx_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb-Index", ssb_idx);
  if (ssb_results_present) {
    j.write_fieldname("ssb-Results");
    ssb_results.to_json(j);
  }
  j.end_obj();
}

// CGI-InfoNR ::= SEQUENCE
SRSASN_CODE cgi_info_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(plmn_id_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(no_sib1_present, 1));

  if (plmn_id_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_info_list, 1, 12));
  }
  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (no_sib1_present) {
    HANDLE_CODE(pack_integer(bref, no_sib1.ssb_subcarrier_offset, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(no_sib1.pdcch_cfg_sib1.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_info_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool plmn_id_info_list_present;
  HANDLE_CODE(bref.unpack(plmn_id_info_list_present, 1));
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  HANDLE_CODE(bref.unpack(no_sib1_present, 1));

  if (plmn_id_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_info_list, bref, 1, 12));
  }
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8, integer_packer<uint16_t>(1, 1024)));
  }
  if (no_sib1_present) {
    HANDLE_CODE(unpack_integer(no_sib1.ssb_subcarrier_offset, bref, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(no_sib1.pdcch_cfg_sib1.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cgi_info_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_info_list.size() > 0) {
    j.start_array("plmn-IdentityInfoList");
    for (const auto& e1 : plmn_id_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (no_sib1_present) {
    j.write_fieldname("noSIB1");
    j.start_obj();
    j.write_int("ssb-SubcarrierOffset", no_sib1.ssb_subcarrier_offset);
    j.write_fieldname("pdcch-ConfigSIB1");
    no_sib1.pdcch_cfg_sib1.to_json(j);
    j.end_obj();
  }
  j.end_obj();
}

// CellAccessRelatedInfo-EUTRA-5GC ::= SEQUENCE
SRSASN_CODE cell_access_related_info_eutra_minus5_gc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ranac_minus5gc_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_eutra_minus5gc, 1, 12));
  HANDLE_CODE(tac_eutra_minus5gc.pack(bref));
  if (ranac_minus5gc_present) {
    HANDLE_CODE(pack_integer(bref, ranac_minus5gc, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_eutra_minus5gc.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_eutra_minus5_gc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ranac_minus5gc_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_eutra_minus5gc, bref, 1, 12));
  HANDLE_CODE(tac_eutra_minus5gc.unpack(bref));
  if (ranac_minus5gc_present) {
    HANDLE_CODE(unpack_integer(ranac_minus5gc, bref, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(cell_id_eutra_minus5gc.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_access_related_info_eutra_minus5_gc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList-eutra-5gc");
  for (const auto& e1 : plmn_id_list_eutra_minus5gc) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-eutra-5gc", tac_eutra_minus5gc.to_string());
  if (ranac_minus5gc_present) {
    j.write_int("ranac-5gc", ranac_minus5gc);
  }
  j.write_fieldname("cellIdentity-eutra-5gc");
  cell_id_eutra_minus5gc.to_json(j);
  j.end_obj();
}

// CellAccessRelatedInfo-EUTRA-EPC ::= SEQUENCE
SRSASN_CODE cell_access_related_info_eutra_epc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_eutra_epc, 1, 12));
  HANDLE_CODE(tac_eutra_epc.pack(bref));
  HANDLE_CODE(cell_id_eutra_epc.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_eutra_epc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_eutra_epc, bref, 1, 12));
  HANDLE_CODE(tac_eutra_epc.unpack(bref));
  HANDLE_CODE(cell_id_eutra_epc.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_access_related_info_eutra_epc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList-eutra-epc");
  for (const auto& e1 : plmn_id_list_eutra_epc) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-eutra-epc", tac_eutra_epc.to_string());
  j.write_str("cellIdentity-eutra-epc", cell_id_eutra_epc.to_string());
  j.end_obj();
}

// CGI-InfoEUTRA ::= SEQUENCE
SRSASN_CODE cgi_info_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cgi_info_epc_present, 1));
  HANDLE_CODE(bref.pack(cgi_info_minus5_gc.size() > 0, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_ind_prio_present, 1));

  if (cgi_info_epc_present) {
    HANDLE_CODE(bref.pack(cgi_info_epc.cgi_info_epc_list.size() > 0, 1));
    HANDLE_CODE(cgi_info_epc.cgi_info_epc_legacy.pack(bref));
    if (cgi_info_epc.cgi_info_epc_list.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, cgi_info_epc.cgi_info_epc_list, 1, 12));
    }
  }
  if (cgi_info_minus5_gc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cgi_info_minus5_gc, 1, 12));
  }
  HANDLE_CODE(pack_integer(bref, freq_band_ind, (uint16_t)1u, (uint16_t)256u));
  if (multi_band_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_info_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cgi_info_epc_present, 1));
  bool cgi_info_minus5_gc_present;
  HANDLE_CODE(bref.unpack(cgi_info_minus5_gc_present, 1));
  bool multi_band_info_list_present;
  HANDLE_CODE(bref.unpack(multi_band_info_list_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_ind_prio_present, 1));

  if (cgi_info_epc_present) {
    bool cgi_info_epc_list_present;
    HANDLE_CODE(bref.unpack(cgi_info_epc_list_present, 1));
    HANDLE_CODE(cgi_info_epc.cgi_info_epc_legacy.unpack(bref));
    if (cgi_info_epc_list_present) {
      HANDLE_CODE(unpack_dyn_seq_of(cgi_info_epc.cgi_info_epc_list, bref, 1, 12));
    }
  }
  if (cgi_info_minus5_gc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cgi_info_minus5_gc, bref, 1, 12));
  }
  HANDLE_CODE(unpack_integer(freq_band_ind, bref, (uint16_t)1u, (uint16_t)256u));
  if (multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list, bref, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
void cgi_info_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cgi_info_epc_present) {
    j.write_fieldname("cgi-info-EPC");
    j.start_obj();
    j.write_fieldname("cgi-info-EPC-legacy");
    cgi_info_epc.cgi_info_epc_legacy.to_json(j);
    if (cgi_info_epc.cgi_info_epc_list.size() > 0) {
      j.start_array("cgi-info-EPC-list");
      for (const auto& e1 : cgi_info_epc.cgi_info_epc_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (cgi_info_minus5_gc.size() > 0) {
    j.start_array("cgi-info-5GC");
    for (const auto& e1 : cgi_info_minus5_gc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("freqBandIndicator", freq_band_ind);
  if (multi_band_info_list.size() > 0) {
    j.start_array("multiBandInfoList");
    for (const auto& e1 : multi_band_info_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (freq_band_ind_prio_present) {
    j.write_str("freqBandIndicatorPriority", "true");
  }
  j.end_obj();
}

// MeasQuantityResultsEUTRA ::= SEQUENCE
SRSASN_CODE meas_quant_results_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_present, 1));
  HANDLE_CODE(bref.pack(rsrq_present, 1));
  HANDLE_CODE(bref.pack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(pack_integer(bref, rsrp, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_present) {
    HANDLE_CODE(pack_integer(bref, rsrq, (uint8_t)0u, (uint8_t)34u));
  }
  if (sinr_present) {
    HANDLE_CODE(pack_integer(bref, sinr, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_quant_results_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_present, 1));
  HANDLE_CODE(bref.unpack(rsrq_present, 1));
  HANDLE_CODE(bref.unpack(sinr_present, 1));

  if (rsrp_present) {
    HANDLE_CODE(unpack_integer(rsrp, bref, (uint8_t)0u, (uint8_t)97u));
  }
  if (rsrq_present) {
    HANDLE_CODE(unpack_integer(rsrq, bref, (uint8_t)0u, (uint8_t)34u));
  }
  if (sinr_present) {
    HANDLE_CODE(unpack_integer(sinr, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_quant_results_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_present) {
    j.write_int("rsrp", rsrp);
  }
  if (rsrq_present) {
    j.write_int("rsrq", rsrq);
  }
  if (sinr_present) {
    j.write_int("sinr", sinr);
  }
  j.end_obj();
}

// MeasResultNR ::= SEQUENCE
SRSASN_CODE meas_result_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pci_present, 1));

  if (pci_present) {
    HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  }
  HANDLE_CODE(bref.pack(meas_result.rs_idx_results_present, 1));
  HANDLE_CODE(bref.pack(meas_result.cell_results.results_ssb_cell_present, 1));
  HANDLE_CODE(bref.pack(meas_result.cell_results.results_csi_rs_cell_present, 1));
  if (meas_result.cell_results.results_ssb_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_ssb_cell.pack(bref));
  }
  if (meas_result.cell_results.results_csi_rs_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_csi_rs_cell.pack(bref));
  }
  if (meas_result.rs_idx_results_present) {
    HANDLE_CODE(bref.pack(meas_result.rs_idx_results.results_ssb_idxes.size() > 0, 1));
    HANDLE_CODE(bref.pack(meas_result.rs_idx_results.results_csi_rs_idxes.size() > 0, 1));
    if (meas_result.rs_idx_results.results_ssb_idxes.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result.rs_idx_results.results_ssb_idxes, 1, 64));
    }
    if (meas_result.rs_idx_results.results_csi_rs_idxes.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, meas_result.rs_idx_results.results_csi_rs_idxes, 1, 64));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= cgi_info.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cgi_info.is_present(), 1));
      if (cgi_info.is_present()) {
        HANDLE_CODE(cgi_info->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pci_present, 1));

  if (pci_present) {
    HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  }
  HANDLE_CODE(bref.unpack(meas_result.rs_idx_results_present, 1));
  HANDLE_CODE(bref.unpack(meas_result.cell_results.results_ssb_cell_present, 1));
  HANDLE_CODE(bref.unpack(meas_result.cell_results.results_csi_rs_cell_present, 1));
  if (meas_result.cell_results.results_ssb_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_ssb_cell.unpack(bref));
  }
  if (meas_result.cell_results.results_csi_rs_cell_present) {
    HANDLE_CODE(meas_result.cell_results.results_csi_rs_cell.unpack(bref));
  }
  if (meas_result.rs_idx_results_present) {
    bool results_ssb_idxes_present;
    HANDLE_CODE(bref.unpack(results_ssb_idxes_present, 1));
    bool results_csi_rs_idxes_present;
    HANDLE_CODE(bref.unpack(results_csi_rs_idxes_present, 1));
    if (results_ssb_idxes_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result.rs_idx_results.results_ssb_idxes, bref, 1, 64));
    }
    if (results_csi_rs_idxes_present) {
      HANDLE_CODE(unpack_dyn_seq_of(meas_result.rs_idx_results.results_csi_rs_idxes, bref, 1, 64));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cgi_info_present;
      HANDLE_CODE(bref.unpack(cgi_info_present, 1));
      cgi_info.set_present(cgi_info_present);
      if (cgi_info.is_present()) {
        HANDLE_CODE(cgi_info->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_present) {
    j.write_int("physCellId", pci);
  }
  j.write_fieldname("measResult");
  j.start_obj();
  j.write_fieldname("cellResults");
  j.start_obj();
  if (meas_result.cell_results.results_ssb_cell_present) {
    j.write_fieldname("resultsSSB-Cell");
    meas_result.cell_results.results_ssb_cell.to_json(j);
  }
  if (meas_result.cell_results.results_csi_rs_cell_present) {
    j.write_fieldname("resultsCSI-RS-Cell");
    meas_result.cell_results.results_csi_rs_cell.to_json(j);
  }
  j.end_obj();
  if (meas_result.rs_idx_results_present) {
    j.write_fieldname("rsIndexResults");
    j.start_obj();
    if (meas_result.rs_idx_results.results_ssb_idxes.size() > 0) {
      j.start_array("resultsSSB-Indexes");
      for (const auto& e1 : meas_result.rs_idx_results.results_ssb_idxes) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result.rs_idx_results.results_csi_rs_idxes.size() > 0) {
      j.start_array("resultsCSI-RS-Indexes");
      for (const auto& e1 : meas_result.rs_idx_results.results_csi_rs_idxes) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  j.end_obj();
  if (ext) {
    if (cgi_info.is_present()) {
      j.write_fieldname("cgi-Info");
      cgi_info->to_json(j);
    }
  }
  j.end_obj();
}

// MeasResultEUTRA ::= SEQUENCE
SRSASN_CODE meas_result_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cgi_info_present, 1));

  HANDLE_CODE(pack_integer(bref, eutra_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(meas_result.pack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(cgi_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cgi_info_present, 1));

  HANDLE_CODE(unpack_integer(eutra_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(meas_result.unpack(bref));
  if (cgi_info_present) {
    HANDLE_CODE(cgi_info.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-PhysCellId", eutra_pci);
  j.write_fieldname("measResult");
  meas_result.to_json(j);
  if (cgi_info_present) {
    j.write_fieldname("cgi-Info");
    cgi_info.to_json(j);
  }
  j.end_obj();
}

// UplinkTxDirectCurrentBWP ::= SEQUENCE
SRSASN_CODE ul_tx_direct_current_bwp_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, bwp_id, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(bref.pack(shift7dot5k_hz, 1));
  HANDLE_CODE(pack_integer(bref, tx_direct_current_location, (uint16_t)0u, (uint16_t)3301u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_bwp_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(bref.unpack(shift7dot5k_hz, 1));
  HANDLE_CODE(unpack_integer(tx_direct_current_location, bref, (uint16_t)0u, (uint16_t)3301u));

  return SRSASN_SUCCESS;
}
void ul_tx_direct_current_bwp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bwp-Id", bwp_id);
  j.write_bool("shift7dot5kHz", shift7dot5k_hz);
  j.write_int("txDirectCurrentLocation", tx_direct_current_location);
  j.end_obj();
}

// EUTRA-RSTD-Info ::= SEQUENCE
SRSASN_CODE eutra_rstd_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, meas_prs_offset, (uint8_t)0u, (uint8_t)39u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_rstd_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(meas_prs_offset, bref, (uint8_t)0u, (uint8_t)39u));

  return SRSASN_SUCCESS;
}
void eutra_rstd_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_int("measPRS-Offset", meas_prs_offset);
  j.end_obj();
}

// MeasResult2EUTRA ::= SEQUENCE
SRSASN_CODE meas_result2_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_serving_cell_present, 1));
  HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.pack(bref));
  }
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_serving_cell_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.unpack(bref));
  }
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result2_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  if (meas_result_serving_cell_present) {
    j.write_fieldname("measResultServingCell");
    meas_result_serving_cell.to_json(j);
  }
  if (meas_result_best_neigh_cell_present) {
    j.write_fieldname("measResultBestNeighCell");
    meas_result_best_neigh_cell.to_json(j);
  }
  j.end_obj();
}

// MeasResult2NR ::= SEQUENCE
SRSASN_CODE meas_result2_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ssb_freq_present, 1));
  HANDLE_CODE(bref.pack(ref_freq_csi_rs_present, 1));
  HANDLE_CODE(bref.pack(meas_result_serving_cell_present, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cell_list_nr.size() > 0, 1));

  if (ssb_freq_present) {
    HANDLE_CODE(pack_integer(bref, ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ref_freq_csi_rs_present) {
    HANDLE_CODE(pack_integer(bref, ref_freq_csi_rs, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.pack(bref));
  }
  if (meas_result_neigh_cell_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cell_list_nr, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result2_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ssb_freq_present, 1));
  HANDLE_CODE(bref.unpack(ref_freq_csi_rs_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_serving_cell_present, 1));
  bool meas_result_neigh_cell_list_nr_present;
  HANDLE_CODE(bref.unpack(meas_result_neigh_cell_list_nr_present, 1));

  if (ssb_freq_present) {
    HANDLE_CODE(unpack_integer(ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (ref_freq_csi_rs_present) {
    HANDLE_CODE(unpack_integer(ref_freq_csi_rs, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (meas_result_serving_cell_present) {
    HANDLE_CODE(meas_result_serving_cell.unpack(bref));
  }
  if (meas_result_neigh_cell_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cell_list_nr, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_result2_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_freq_present) {
    j.write_int("ssbFrequency", ssb_freq);
  }
  if (ref_freq_csi_rs_present) {
    j.write_int("refFreqCSI-RS", ref_freq_csi_rs);
  }
  if (meas_result_serving_cell_present) {
    j.write_fieldname("measResultServingCell");
    meas_result_serving_cell.to_json(j);
  }
  if (meas_result_neigh_cell_list_nr.size() > 0) {
    j.start_array("measResultNeighCellListNR");
    for (const auto& e1 : meas_result_neigh_cell_list_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MeasResultCellSFTD-NR ::= SEQUENCE
SRSASN_CODE meas_result_cell_sftd_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_result_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(pack_integer(bref, sfn_offset_result, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, frame_boundary_offset_result, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_cell_sftd_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_result_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(unpack_integer(sfn_offset_result, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(frame_boundary_offset_result, bref, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(unpack_integer(rsrp_result, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_cell_sftd_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_int("sfn-OffsetResult", sfn_offset_result);
  j.write_int("frameBoundaryOffsetResult", frame_boundary_offset_result);
  if (rsrp_result_present) {
    j.write_int("rsrp-Result", rsrp_result);
  }
  j.end_obj();
}

// MeasResultServMO ::= SEQUENCE
SRSASN_CODE meas_result_serv_mo_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_id, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_result_serving_cell.pack(bref));
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serv_mo_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_best_neigh_cell_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_result_serving_cell.unpack(bref));
  if (meas_result_best_neigh_cell_present) {
    HANDLE_CODE(meas_result_best_neigh_cell.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_result_serv_mo_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId", serv_cell_id);
  j.write_fieldname("measResultServingCell");
  meas_result_serving_cell.to_json(j);
  if (meas_result_best_neigh_cell_present) {
    j.write_fieldname("measResultBestNeighCell");
    meas_result_best_neigh_cell.to_json(j);
  }
  j.end_obj();
}

// ReducedAggregatedBandwidth ::= ENUMERATED
const char* reduced_aggregated_bw_opts::to_string() const
{
  static const char* options[] = {
      "mhz0", "mhz10", "mhz20", "mhz30", "mhz40", "mhz50", "mhz60", "mhz80", "mhz100", "mhz200", "mhz300", "mhz400"};
  return convert_enum_idx(options, 12, value, "reduced_aggregated_bw_e");
}
uint16_t reduced_aggregated_bw_opts::to_number() const
{
  static const uint16_t options[] = {0, 10, 20, 30, 40, 50, 60, 80, 100, 200, 300, 400};
  return map_enum_number(options, 12, value, "reduced_aggregated_bw_e");
}

// UplinkTxDirectCurrentCell ::= SEQUENCE
SRSASN_CODE ul_tx_direct_current_cell_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_direct_current_bwp, 1, 4));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ul_direct_current_bwp_sul.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_direct_current_bwp_sul.is_present(), 1));
      if (ul_direct_current_bwp_sul.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ul_direct_current_bwp_sul, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_direct_current_cell_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_dyn_seq_of(ul_direct_current_bwp, bref, 1, 4));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_direct_current_bwp_sul_present;
      HANDLE_CODE(bref.unpack(ul_direct_current_bwp_sul_present, 1));
      ul_direct_current_bwp_sul.set_present(ul_direct_current_bwp_sul_present);
      if (ul_direct_current_bwp_sul.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_direct_current_bwp_sul, bref, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ul_tx_direct_current_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.start_array("uplinkDirectCurrentBWP");
  for (const auto& e1 : ul_direct_current_bwp) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (ul_direct_current_bwp_sul.is_present()) {
      j.start_array("uplinkDirectCurrentBWP-SUL");
      for (const auto& e1 : *ul_direct_current_bwp_sul) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// DRB-CountInfo ::= SEQUENCE
SRSASN_CODE drb_count_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(pack_integer(bref, count_ul, (uint64_t)0u, (uint64_t)4294967295u));
  HANDLE_CODE(pack_integer(bref, count_dl, (uint64_t)0u, (uint64_t)4294967295u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_count_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(unpack_integer(count_ul, bref, (uint64_t)0u, (uint64_t)4294967295u));
  HANDLE_CODE(unpack_integer(count_dl, bref, (uint64_t)0u, (uint64_t)4294967295u));

  return SRSASN_SUCCESS;
}
void drb_count_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Identity", drb_id);
  j.write_int("count-Uplink", count_ul);
  j.write_int("count-Downlink", count_dl);
  j.end_obj();
}

// MeasResultSFTD-EUTRA ::= SEQUENCE
SRSASN_CODE meas_result_sftd_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_result_present, 1));

  HANDLE_CODE(pack_integer(bref, eutra_pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, sfn_offset_result, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(pack_integer(bref, frame_boundary_offset_result, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_result, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_sftd_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_result_present, 1));

  HANDLE_CODE(unpack_integer(eutra_pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(sfn_offset_result, bref, (uint16_t)0u, (uint16_t)1023u));
  HANDLE_CODE(unpack_integer(frame_boundary_offset_result, bref, (int16_t)-30720, (int16_t)30719));
  if (rsrp_result_present) {
    HANDLE_CODE(unpack_integer(rsrp_result, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void meas_result_sftd_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-PhysCellId", eutra_pci);
  j.write_int("sfn-OffsetResult", sfn_offset_result);
  j.write_int("frameBoundaryOffsetResult", frame_boundary_offset_result);
  if (rsrp_result_present) {
    j.write_int("rsrp-Result", rsrp_result);
  }
  j.end_obj();
}

// OverheatingAssistance ::= SEQUENCE
SRSASN_CODE overheat_assist_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_max_ccs_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_bw_fr1_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_bw_fr2_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr1_present, 1));
  HANDLE_CODE(bref.pack(reduced_max_mimo_layers_fr2_present, 1));

  if (reduced_max_ccs_present) {
    HANDLE_CODE(pack_integer(bref, reduced_max_ccs.reduced_ccs_dl, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, reduced_max_ccs.reduced_ccs_ul, (uint8_t)0u, (uint8_t)31u));
  }
  if (reduced_max_bw_fr1_present) {
    HANDLE_CODE(reduced_max_bw_fr1.reduced_bw_fr1_dl.pack(bref));
    HANDLE_CODE(reduced_max_bw_fr1.reduced_bw_fr1_ul.pack(bref));
  }
  if (reduced_max_bw_fr2_present) {
    HANDLE_CODE(reduced_max_bw_fr2.reduced_bw_fr2_dl.pack(bref));
    HANDLE_CODE(reduced_max_bw_fr2.reduced_bw_fr2_ul.pack(bref));
  }
  if (reduced_max_mimo_layers_fr1_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_dl.pack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_ul.pack(bref));
  }
  if (reduced_max_mimo_layers_fr2_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_dl.pack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE overheat_assist_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_max_ccs_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr1_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_bw_fr2_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr1_present, 1));
  HANDLE_CODE(bref.unpack(reduced_max_mimo_layers_fr2_present, 1));

  if (reduced_max_ccs_present) {
    HANDLE_CODE(unpack_integer(reduced_max_ccs.reduced_ccs_dl, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(reduced_max_ccs.reduced_ccs_ul, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (reduced_max_bw_fr1_present) {
    HANDLE_CODE(reduced_max_bw_fr1.reduced_bw_fr1_dl.unpack(bref));
    HANDLE_CODE(reduced_max_bw_fr1.reduced_bw_fr1_ul.unpack(bref));
  }
  if (reduced_max_bw_fr2_present) {
    HANDLE_CODE(reduced_max_bw_fr2.reduced_bw_fr2_dl.unpack(bref));
    HANDLE_CODE(reduced_max_bw_fr2.reduced_bw_fr2_ul.unpack(bref));
  }
  if (reduced_max_mimo_layers_fr1_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_dl.unpack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_ul.unpack(bref));
  }
  if (reduced_max_mimo_layers_fr2_present) {
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_dl.unpack(bref));
    HANDLE_CODE(reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void overheat_assist_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_max_ccs_present) {
    j.write_fieldname("reducedMaxCCs");
    j.start_obj();
    j.write_int("reducedCCsDL", reduced_max_ccs.reduced_ccs_dl);
    j.write_int("reducedCCsUL", reduced_max_ccs.reduced_ccs_ul);
    j.end_obj();
  }
  if (reduced_max_bw_fr1_present) {
    j.write_fieldname("reducedMaxBW-FR1");
    j.start_obj();
    j.write_str("reducedBW-FR1-DL", reduced_max_bw_fr1.reduced_bw_fr1_dl.to_string());
    j.write_str("reducedBW-FR1-UL", reduced_max_bw_fr1.reduced_bw_fr1_ul.to_string());
    j.end_obj();
  }
  if (reduced_max_bw_fr2_present) {
    j.write_fieldname("reducedMaxBW-FR2");
    j.start_obj();
    j.write_str("reducedBW-FR2-DL", reduced_max_bw_fr2.reduced_bw_fr2_dl.to_string());
    j.write_str("reducedBW-FR2-UL", reduced_max_bw_fr2.reduced_bw_fr2_ul.to_string());
    j.end_obj();
  }
  if (reduced_max_mimo_layers_fr1_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR1");
    j.start_obj();
    j.write_str("reducedMIMO-LayersFR1-DL", reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_dl.to_string());
    j.write_str("reducedMIMO-LayersFR1-UL", reduced_max_mimo_layers_fr1.reduced_mimo_layers_fr1_ul.to_string());
    j.end_obj();
  }
  if (reduced_max_mimo_layers_fr2_present) {
    j.write_fieldname("reducedMaxMIMO-LayersFR2");
    j.start_obj();
    j.write_str("reducedMIMO-LayersFR2-DL", reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_dl.to_string());
    j.write_str("reducedMIMO-LayersFR2-UL", reduced_max_mimo_layers_fr2.reduced_mimo_layers_fr2_ul.to_string());
    j.end_obj();
  }
  j.end_obj();
}

// RRCReconfigurationComplete-v1560-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_resp_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_resp_present) {
    HANDLE_CODE(scg_resp.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_resp_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_resp_present) {
    HANDLE_CODE(scg_resp.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_resp_present) {
    j.write_fieldname("scg-Response");
    scg_resp.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

void rrc_recfg_complete_v1560_ies_s::scg_resp_c_::destroy_()
{
  switch (type_) {
    case types::nr_scg_resp:
      c.destroy<dyn_octstring>();
      break;
    case types::eutra_scg_resp:
      c.destroy<dyn_octstring>();
      break;
    default:
      break;
  }
}
void rrc_recfg_complete_v1560_ies_s::scg_resp_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_scg_resp:
      c.init<dyn_octstring>();
      break;
    case types::eutra_scg_resp:
      c.init<dyn_octstring>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }
}
rrc_recfg_complete_v1560_ies_s::scg_resp_c_::scg_resp_c_(const rrc_recfg_complete_v1560_ies_s::scg_resp_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_scg_resp:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_resp:
      c.init(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }
}
rrc_recfg_complete_v1560_ies_s::scg_resp_c_&
rrc_recfg_complete_v1560_ies_s::scg_resp_c_::operator=(const rrc_recfg_complete_v1560_ies_s::scg_resp_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_scg_resp:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::eutra_scg_resp:
      c.set(other.c.get<dyn_octstring>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }

  return *this;
}
dyn_octstring& rrc_recfg_complete_v1560_ies_s::scg_resp_c_::set_nr_scg_resp()
{
  set(types::nr_scg_resp);
  return c.get<dyn_octstring>();
}
dyn_octstring& rrc_recfg_complete_v1560_ies_s::scg_resp_c_::set_eutra_scg_resp()
{
  set(types::eutra_scg_resp);
  return c.get<dyn_octstring>();
}
void rrc_recfg_complete_v1560_ies_s::scg_resp_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_scg_resp:
      j.write_str("nr-SCG-Response", c.get<dyn_octstring>().to_string());
      break;
    case types::eutra_scg_resp:
      j.write_str("eutra-SCG-Response", c.get<dyn_octstring>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::scg_resp_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    case types::eutra_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1560_ies_s::scg_resp_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    case types::eutra_scg_resp:
      HANDLE_CODE(c.get<dyn_octstring>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_recfg_complete_v1560_ies_s::scg_resp_c_::types_opts::to_string() const
{
  static const char* options[] = {"nr-SCG-Response", "eutra-SCG-Response"};
  return convert_enum_idx(options, 2, value, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_::types");
}

// UE-CapabilityRAT-Container ::= SEQUENCE
SRSASN_CODE ue_cap_rat_container_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rat_type.pack(bref));
  HANDLE_CODE(ue_cap_rat_container.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_rat_container_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rat_type.unpack(bref));
  HANDLE_CODE(ue_cap_rat_container.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_rat_container_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rat-Type", rat_type.to_string());
  j.write_str("ue-CapabilityRAT-Container", ue_cap_rat_container.to_string());
  j.end_obj();
}

// DelayBudgetReport ::= CHOICE
void delay_budget_report_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("type1", c.to_string());
  j.end_obj();
}
SRSASN_CODE delay_budget_report_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE delay_budget_report_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "delay_budget_report_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* delay_budget_report_c::type1_opts::to_string() const
{
  static const char* options[] = {"msMinus1280",
                                  "msMinus640",
                                  "msMinus320",
                                  "msMinus160",
                                  "msMinus80",
                                  "msMinus60",
                                  "msMinus40",
                                  "msMinus20",
                                  "ms0",
                                  "ms20",
                                  "ms40",
                                  "ms60",
                                  "ms80",
                                  "ms160",
                                  "ms320",
                                  "ms640",
                                  "ms1280"};
  return convert_enum_idx(options, 17, value, "delay_budget_report_c::type1_e_");
}
int16_t delay_budget_report_c::type1_opts::to_number() const
{
  static const int16_t options[] = {
      -1280, -640, -320, -160, -80, -60, -40, -20, 0, 20, 40, 60, 80, 160, 320, 640, 1280};
  return map_enum_number(options, 17, value, "delay_budget_report_c::type1_e_");
}

const char* delay_budget_report_c::types_opts::to_string() const
{
  static const char* options[] = {"type1"};
  return convert_enum_idx(options, 1, value, "delay_budget_report_c::types");
}
uint8_t delay_budget_report_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "delay_budget_report_c::types");
}

// FailureInfoRLC-Bearer ::= SEQUENCE
SRSASN_CODE fail_info_rlc_bearer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_group_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, lc_ch_id, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(fail_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_rlc_bearer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_group_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(lc_ch_id, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(fail_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void fail_info_rlc_bearer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellGroupId", cell_group_id);
  j.write_int("logicalChannelIdentity", lc_ch_id);
  j.write_str("failureType", fail_type.to_string());
  j.end_obj();
}

const char* fail_info_rlc_bearer_s::fail_type_opts::to_string() const
{
  static const char* options[] = {"rlc-failure", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "fail_info_rlc_bearer_s::fail_type_e_");
}

// FailureReportSCG ::= SEQUENCE
SRSASN_CODE fail_report_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_freq_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_scg_fail.size() > 0, 1));

  HANDLE_CODE(fail_type.pack(bref));
  if (meas_result_freq_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list, 1, 8));
  }
  if (meas_result_scg_fail.size() > 0) {
    HANDLE_CODE(meas_result_scg_fail.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_result_freq_list_present;
  HANDLE_CODE(bref.unpack(meas_result_freq_list_present, 1));
  bool meas_result_scg_fail_present;
  HANDLE_CODE(bref.unpack(meas_result_scg_fail_present, 1));

  HANDLE_CODE(fail_type.unpack(bref));
  if (meas_result_freq_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list, bref, 1, 8));
  }
  if (meas_result_scg_fail_present) {
    HANDLE_CODE(meas_result_scg_fail.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void fail_report_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("failureType", fail_type.to_string());
  if (meas_result_freq_list.size() > 0) {
    j.start_array("measResultFreqList");
    for (const auto& e1 : meas_result_freq_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_scg_fail.size() > 0) {
    j.write_str("measResultSCG-Failure", meas_result_scg_fail.to_string());
  }
  j.end_obj();
}

const char* fail_report_scg_s::fail_type_opts::to_string() const
{
  static const char* options[] = {"t310-Expiry",
                                  "randomAccessProblem",
                                  "rlc-MaxNumRetx",
                                  "synchReconfigFailureSCG",
                                  "scg-ReconfigFailure",
                                  "srb3-IntegrityFailure",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "fail_report_scg_s::fail_type_e_");
}
uint16_t fail_report_scg_s::fail_type_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case srb3_integrity_fail:
      return 3;
    default:
      invalid_enum_number(value, "fail_report_scg_s::fail_type_e_");
  }
  return 0;
}

// FailureReportSCG-EUTRA ::= SEQUENCE
SRSASN_CODE fail_report_scg_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_freq_list_mrdc.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_scg_fail_mrdc.size() > 0, 1));

  HANDLE_CODE(fail_type.pack(bref));
  if (meas_result_freq_list_mrdc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list_mrdc, 1, 8));
  }
  if (meas_result_scg_fail_mrdc.size() > 0) {
    HANDLE_CODE(meas_result_scg_fail_mrdc.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_scg_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_result_freq_list_mrdc_present;
  HANDLE_CODE(bref.unpack(meas_result_freq_list_mrdc_present, 1));
  bool meas_result_scg_fail_mrdc_present;
  HANDLE_CODE(bref.unpack(meas_result_scg_fail_mrdc_present, 1));

  HANDLE_CODE(fail_type.unpack(bref));
  if (meas_result_freq_list_mrdc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list_mrdc, bref, 1, 8));
  }
  if (meas_result_scg_fail_mrdc_present) {
    HANDLE_CODE(meas_result_scg_fail_mrdc.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void fail_report_scg_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("failureType", fail_type.to_string());
  if (meas_result_freq_list_mrdc.size() > 0) {
    j.start_array("measResultFreqListMRDC");
    for (const auto& e1 : meas_result_freq_list_mrdc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_scg_fail_mrdc.size() > 0) {
    j.write_str("measResultSCG-FailureMRDC", meas_result_scg_fail_mrdc.to_string());
  }
  j.end_obj();
}

const char* fail_report_scg_eutra_s::fail_type_opts::to_string() const
{
  static const char* options[] = {"t313-Expiry",
                                  "randomAccessProblem",
                                  "rlc-MaxNumRetx",
                                  "scg-ChangeFailure",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "fail_report_scg_eutra_s::fail_type_e_");
}
uint16_t fail_report_scg_eutra_s::fail_type_opts::to_number() const
{
  static const uint16_t options[] = {313};
  return map_enum_number(options, 1, value, "fail_report_scg_eutra_s::fail_type_e_");
}

// LocationMeasurementInfo ::= CHOICE
void location_meas_info_c::set(types::options e)
{
  type_ = e;
}
eutra_rstd_info_list_l& location_meas_info_c::set_eutra_rstd()
{
  set(types::eutra_rstd);
  return c;
}
void location_meas_info_c::set_eutra_fine_timing_detection()
{
  set(types::eutra_fine_timing_detection);
}
void location_meas_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eutra_rstd:
      j.start_array("eutra-RSTD");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::eutra_fine_timing_detection:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
  }
  j.end_obj();
}
SRSASN_CODE location_meas_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eutra_rstd:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 3));
      break;
    case types::eutra_fine_timing_detection:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eutra_rstd:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 3));
      break;
    case types::eutra_fine_timing_detection:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* location_meas_info_c::types_opts::to_string() const
{
  static const char* options[] = {"eutra-RSTD", "eutra-FineTimingDetection"};
  return convert_enum_idx(options, 2, value, "location_meas_info_c::types");
}

// MeasResults ::= SEQUENCE
SRSASN_CODE meas_results_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_serving_mo_list, 1, 32));
  if (meas_result_neigh_cells_present) {
    HANDLE_CODE(meas_result_neigh_cells.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_result_serv_freq_list_eutra_scg.is_present();
    group_flags[0] |= meas_result_serv_freq_list_nr_scg.is_present();
    group_flags[0] |= meas_result_sftd_eutra.is_present();
    group_flags[0] |= meas_result_sftd_nr.is_present();
    group_flags[1] |= meas_result_cell_list_sftd_nr.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_eutra_scg.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_serv_freq_list_nr_scg.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_sftd_eutra.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_result_sftd_nr.is_present(), 1));
      if (meas_result_serv_freq_list_eutra_scg.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_eutra_scg, 1, 32));
      }
      if (meas_result_serv_freq_list_nr_scg.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_serv_freq_list_nr_scg, 1, 32));
      }
      if (meas_result_sftd_eutra.is_present()) {
        HANDLE_CODE(meas_result_sftd_eutra->pack(bref));
      }
      if (meas_result_sftd_nr.is_present()) {
        HANDLE_CODE(meas_result_sftd_nr->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_nr.is_present(), 1));
      if (meas_result_cell_list_sftd_nr.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *meas_result_cell_list_sftd_nr, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_present, 1));

  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_serving_mo_list, bref, 1, 32));
  if (meas_result_neigh_cells_present) {
    HANDLE_CODE(meas_result_neigh_cells.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_serv_freq_list_eutra_scg_present;
      HANDLE_CODE(bref.unpack(meas_result_serv_freq_list_eutra_scg_present, 1));
      meas_result_serv_freq_list_eutra_scg.set_present(meas_result_serv_freq_list_eutra_scg_present);
      bool meas_result_serv_freq_list_nr_scg_present;
      HANDLE_CODE(bref.unpack(meas_result_serv_freq_list_nr_scg_present, 1));
      meas_result_serv_freq_list_nr_scg.set_present(meas_result_serv_freq_list_nr_scg_present);
      bool meas_result_sftd_eutra_present;
      HANDLE_CODE(bref.unpack(meas_result_sftd_eutra_present, 1));
      meas_result_sftd_eutra.set_present(meas_result_sftd_eutra_present);
      bool meas_result_sftd_nr_present;
      HANDLE_CODE(bref.unpack(meas_result_sftd_nr_present, 1));
      meas_result_sftd_nr.set_present(meas_result_sftd_nr_present);
      if (meas_result_serv_freq_list_eutra_scg.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_eutra_scg, bref, 1, 32));
      }
      if (meas_result_serv_freq_list_nr_scg.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_serv_freq_list_nr_scg, bref, 1, 32));
      }
      if (meas_result_sftd_eutra.is_present()) {
        HANDLE_CODE(meas_result_sftd_eutra->unpack(bref));
      }
      if (meas_result_sftd_nr.is_present()) {
        HANDLE_CODE(meas_result_sftd_nr->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_result_cell_list_sftd_nr_present;
      HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_nr_present, 1));
      meas_result_cell_list_sftd_nr.set_present(meas_result_cell_list_sftd_nr_present);
      if (meas_result_cell_list_sftd_nr.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*meas_result_cell_list_sftd_nr, bref, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_results_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  j.start_array("measResultServingMOList");
  for (const auto& e1 : meas_result_serving_mo_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (meas_result_neigh_cells_present) {
    j.write_fieldname("measResultNeighCells");
    meas_result_neigh_cells.to_json(j);
  }
  if (ext) {
    if (meas_result_serv_freq_list_eutra_scg.is_present()) {
      j.start_array("measResultServFreqListEUTRA-SCG");
      for (const auto& e1 : *meas_result_serv_freq_list_eutra_scg) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_serv_freq_list_nr_scg.is_present()) {
      j.start_array("measResultServFreqListNR-SCG");
      for (const auto& e1 : *meas_result_serv_freq_list_nr_scg) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_result_sftd_eutra.is_present()) {
      j.write_fieldname("measResultSFTD-EUTRA");
      meas_result_sftd_eutra->to_json(j);
    }
    if (meas_result_sftd_nr.is_present()) {
      j.write_fieldname("measResultSFTD-NR");
      meas_result_sftd_nr->to_json(j);
    }
    if (meas_result_cell_list_sftd_nr.is_present()) {
      j.start_array("measResultCellListSFTD-NR");
      for (const auto& e1 : *meas_result_cell_list_sftd_nr) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

void meas_results_s::meas_result_neigh_cells_c_::destroy_()
{
  switch (type_) {
    case types::meas_result_list_nr:
      c.destroy<meas_result_list_nr_l>();
      break;
    case types::meas_result_list_eutra:
      c.destroy<meas_result_list_eutra_l>();
      break;
    default:
      break;
  }
}
void meas_results_s::meas_result_neigh_cells_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_result_list_nr:
      c.init<meas_result_list_nr_l>();
      break;
    case types::meas_result_list_eutra:
      c.init<meas_result_list_eutra_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
}
meas_results_s::meas_result_neigh_cells_c_::meas_result_neigh_cells_c_(
    const meas_results_s::meas_result_neigh_cells_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_result_list_nr:
      c.init(other.c.get<meas_result_list_nr_l>());
      break;
    case types::meas_result_list_eutra:
      c.init(other.c.get<meas_result_list_eutra_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
}
meas_results_s::meas_result_neigh_cells_c_&
meas_results_s::meas_result_neigh_cells_c_::operator=(const meas_results_s::meas_result_neigh_cells_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_result_list_nr:
      c.set(other.c.get<meas_result_list_nr_l>());
      break;
    case types::meas_result_list_eutra:
      c.set(other.c.get<meas_result_list_eutra_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }

  return *this;
}
meas_result_list_nr_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_nr()
{
  set(types::meas_result_list_nr);
  return c.get<meas_result_list_nr_l>();
}
meas_result_list_eutra_l& meas_results_s::meas_result_neigh_cells_c_::set_meas_result_list_eutra()
{
  set(types::meas_result_list_eutra);
  return c.get<meas_result_list_eutra_l>();
}
void meas_results_s::meas_result_neigh_cells_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_result_list_nr:
      j.start_array("measResultListNR");
      for (const auto& e1 : c.get<meas_result_list_nr_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::meas_result_list_eutra:
      j.start_array("measResultListEUTRA");
      for (const auto& e1 : c.get<meas_result_list_eutra_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_results_s::meas_result_neigh_cells_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_result_list_nr:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_nr_l>(), 1, 8));
      break;
    case types::meas_result_list_eutra: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<meas_result_list_eutra_l>(), 1, 8));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_results_s::meas_result_neigh_cells_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_result_list_nr:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_nr_l>(), bref, 1, 8));
      break;
    case types::meas_result_list_eutra: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(unpack_dyn_seq_of(c.get<meas_result_list_eutra_l>(), bref, 1, 8));
    } break;
    default:
      log_invalid_choice_id(type_, "meas_results_s::meas_result_neigh_cells_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_results_s::meas_result_neigh_cells_c_::types_opts::to_string() const
{
  static const char* options[] = {"measResultListNR", "measResultListEUTRA"};
  return convert_enum_idx(options, 2, value, "meas_results_s::meas_result_neigh_cells_c_::types");
}

// RRCReconfigurationComplete-v1530-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_tx_direct_current_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_list, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_v1530_ies_s::unpack(cbit_ref& bref)
{
  bool ul_tx_direct_current_list_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_list_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_tx_direct_current_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_list, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_tx_direct_current_list.size() > 0) {
    j.start_array("uplinkTxDirectCurrentList");
    for (const auto& e1 : ul_tx_direct_current_list) {
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

// RegisteredAMF ::= SEQUENCE
SRSASN_CODE registered_amf_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  HANDLE_CODE(amf_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE registered_amf_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));

  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  HANDLE_CODE(amf_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void registered_amf_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (plmn_id_present) {
    j.write_fieldname("plmn-Identity");
    plmn_id.to_json(j);
  }
  j.write_str("amf-Identifier", amf_id.to_string());
  j.end_obj();
}

// S-NSSAI ::= CHOICE
void s_nssai_c::destroy_()
{
  switch (type_) {
    case types::sst:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::sst_sd:
      c.destroy<fixed_bitstring<32> >();
      break;
    default:
      break;
  }
}
void s_nssai_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sst:
      c.init<fixed_bitstring<8> >();
      break;
    case types::sst_sd:
      c.init<fixed_bitstring<32> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }
}
s_nssai_c::s_nssai_c(const s_nssai_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sst:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::sst_sd:
      c.init(other.c.get<fixed_bitstring<32> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }
}
s_nssai_c& s_nssai_c::operator=(const s_nssai_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sst:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::sst_sd:
      c.set(other.c.get<fixed_bitstring<32> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }

  return *this;
}
fixed_bitstring<8>& s_nssai_c::set_sst()
{
  set(types::sst);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<32>& s_nssai_c::set_sst_sd()
{
  set(types::sst_sd);
  return c.get<fixed_bitstring<32> >();
}
void s_nssai_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sst:
      j.write_str("sst", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::sst_sd:
      j.write_str("sst-SD", c.get<fixed_bitstring<32> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
  }
  j.end_obj();
}
SRSASN_CODE s_nssai_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sst:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::sst_sd:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE s_nssai_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sst:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::sst_sd:
      HANDLE_CODE(c.get<fixed_bitstring<32> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "s_nssai_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* s_nssai_c::types_opts::to_string() const
{
  static const char* options[] = {"sst", "sst-SD"};
  return convert_enum_idx(options, 2, value, "s_nssai_c::types");
}

// SCGFailureInformation-v1590-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
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

// SCGFailureInformationEUTRA-v1590-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_eutra_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_eutra_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
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

// UEAssistanceInformation-v1540-IEs ::= SEQUENCE
SRSASN_CODE ueassist_info_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(overheat_assist_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (overheat_assist_present) {
    HANDLE_CODE(overheat_assist.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(overheat_assist_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (overheat_assist_present) {
    HANDLE_CODE(overheat_assist.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueassist_info_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (overheat_assist_present) {
    j.write_fieldname("overheatingAssistance");
    overheat_assist.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CounterCheckResponse-IEs ::= SEQUENCE
SRSASN_CODE counter_check_resp_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, drb_count_info_list, 0, 29));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(drb_count_info_list, bref, 0, 29));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void counter_check_resp_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("drb-CountInfoList");
  for (const auto& e1 : drb_count_info_list) {
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

// FailureInformation-IEs ::= SEQUENCE
SRSASN_CODE fail_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_info_rlc_bearer_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_info_rlc_bearer_present) {
    HANDLE_CODE(fail_info_rlc_bearer.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_info_rlc_bearer_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_info_rlc_bearer_present) {
    HANDLE_CODE(fail_info_rlc_bearer.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void fail_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_info_rlc_bearer_present) {
    j.write_fieldname("failureInfoRLC-Bearer");
    fail_info_rlc_bearer.to_json(j);
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

// LocationMeasurementIndication-IEs ::= SEQUENCE
SRSASN_CODE location_meas_ind_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_ind.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_ind_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_ind.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void location_meas_ind_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measurementIndication");
  meas_ind.to_json(j);
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

// MeasurementReport-IEs ::= SEQUENCE
SRSASN_CODE meas_report_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_results.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_results.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measResults");
  meas_results.to_json(j);
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

// RRCReconfigurationComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_recfg_complete_ies_s::unpack(cbit_ref& bref)
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
void rrc_recfg_complete_ies_s::to_json(json_writer& j) const
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

// RRCReestablishmentComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_complete_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_reest_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
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

// RRCResumeComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_nas_msg.size() > 0, 1));
  HANDLE_CODE(bref.pack(sel_plmn_id_present, 1));
  HANDLE_CODE(bref.pack(ul_tx_direct_current_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_nas_msg.size() > 0) {
    HANDLE_CODE(ded_nas_msg.pack(bref));
  }
  if (sel_plmn_id_present) {
    HANDLE_CODE(pack_integer(bref, sel_plmn_id, (uint8_t)1u, (uint8_t)12u));
  }
  if (ul_tx_direct_current_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_tx_direct_current_list, 1, 32));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_ies_s::unpack(cbit_ref& bref)
{
  bool ded_nas_msg_present;
  HANDLE_CODE(bref.unpack(ded_nas_msg_present, 1));
  HANDLE_CODE(bref.unpack(sel_plmn_id_present, 1));
  bool ul_tx_direct_current_list_present;
  HANDLE_CODE(bref.unpack(ul_tx_direct_current_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_nas_msg_present) {
    HANDLE_CODE(ded_nas_msg.unpack(bref));
  }
  if (sel_plmn_id_present) {
    HANDLE_CODE(unpack_integer(sel_plmn_id, bref, (uint8_t)1u, (uint8_t)12u));
  }
  if (ul_tx_direct_current_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_tx_direct_current_list, bref, 1, 32));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_nas_msg.size() > 0) {
    j.write_str("dedicatedNAS-Message", ded_nas_msg.to_string());
  }
  if (sel_plmn_id_present) {
    j.write_int("selectedPLMN-Identity", sel_plmn_id);
  }
  if (ul_tx_direct_current_list.size() > 0) {
    j.start_array("uplinkTxDirectCurrentList");
    for (const auto& e1 : ul_tx_direct_current_list) {
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

// RRCSetupComplete-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(registered_amf_present, 1));
  HANDLE_CODE(bref.pack(guami_type_present, 1));
  HANDLE_CODE(bref.pack(s_nssai_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ng_minus5_g_s_tmsi_value_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, sel_plmn_id, (uint8_t)1u, (uint8_t)12u));
  if (registered_amf_present) {
    HANDLE_CODE(registered_amf.pack(bref));
  }
  if (guami_type_present) {
    HANDLE_CODE(guami_type.pack(bref));
  }
  if (s_nssai_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, s_nssai_list, 1, 8));
  }
  HANDLE_CODE(ded_nas_msg.pack(bref));
  if (ng_minus5_g_s_tmsi_value_present) {
    HANDLE_CODE(ng_minus5_g_s_tmsi_value.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(registered_amf_present, 1));
  HANDLE_CODE(bref.unpack(guami_type_present, 1));
  bool s_nssai_list_present;
  HANDLE_CODE(bref.unpack(s_nssai_list_present, 1));
  HANDLE_CODE(bref.unpack(ng_minus5_g_s_tmsi_value_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(sel_plmn_id, bref, (uint8_t)1u, (uint8_t)12u));
  if (registered_amf_present) {
    HANDLE_CODE(registered_amf.unpack(bref));
  }
  if (guami_type_present) {
    HANDLE_CODE(guami_type.unpack(bref));
  }
  if (s_nssai_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(s_nssai_list, bref, 1, 8));
  }
  HANDLE_CODE(ded_nas_msg.unpack(bref));
  if (ng_minus5_g_s_tmsi_value_present) {
    HANDLE_CODE(ng_minus5_g_s_tmsi_value.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_setup_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("selectedPLMN-Identity", sel_plmn_id);
  if (registered_amf_present) {
    j.write_fieldname("registeredAMF");
    registered_amf.to_json(j);
  }
  if (guami_type_present) {
    j.write_str("guami-Type", guami_type.to_string());
  }
  if (s_nssai_list.size() > 0) {
    j.start_array("s-NSSAI-List");
    for (const auto& e1 : s_nssai_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("dedicatedNAS-Message", ded_nas_msg.to_string());
  if (ng_minus5_g_s_tmsi_value_present) {
    j.write_fieldname("ng-5G-S-TMSI-Value");
    ng_minus5_g_s_tmsi_value.to_json(j);
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

const char* rrc_setup_complete_ies_s::guami_type_opts::to_string() const
{
  static const char* options[] = {"native", "mapped"};
  return convert_enum_idx(options, 2, value, "rrc_setup_complete_ies_s::guami_type_e_");
}

void rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::destroy_()
{
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.destroy<fixed_bitstring<48> >();
      break;
    case types::ng_minus5_g_s_tmsi_part2:
      c.destroy<fixed_bitstring<9> >();
      break;
    default:
      break;
  }
}
void rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.init<fixed_bitstring<48> >();
      break;
    case types::ng_minus5_g_s_tmsi_part2:
      c.init<fixed_bitstring<9> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_");
  }
}
rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::ng_minus5_g_s_tmsi_value_c_(
    const rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.init(other.c.get<fixed_bitstring<48> >());
      break;
    case types::ng_minus5_g_s_tmsi_part2:
      c.init(other.c.get<fixed_bitstring<9> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_");
  }
}
rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_& rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::operator=(
    const rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      c.set(other.c.get<fixed_bitstring<48> >());
      break;
    case types::ng_minus5_g_s_tmsi_part2:
      c.set(other.c.get<fixed_bitstring<9> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_");
  }

  return *this;
}
fixed_bitstring<48>& rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::set_ng_minus5_g_s_tmsi()
{
  set(types::ng_minus5_g_s_tmsi);
  return c.get<fixed_bitstring<48> >();
}
fixed_bitstring<9>& rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::set_ng_minus5_g_s_tmsi_part2()
{
  set(types::ng_minus5_g_s_tmsi_part2);
  return c.get<fixed_bitstring<9> >();
}
void rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      j.write_str("ng-5G-S-TMSI", c.get<fixed_bitstring<48> >().to_string());
      break;
    case types::ng_minus5_g_s_tmsi_part2:
      j.write_str("ng-5G-S-TMSI-Part2", c.get<fixed_bitstring<9> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48> >().pack(bref));
      break;
    case types::ng_minus5_g_s_tmsi_part2:
      HANDLE_CODE(c.get<fixed_bitstring<9> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48> >().unpack(bref));
      break;
    case types::ng_minus5_g_s_tmsi_part2:
      HANDLE_CODE(c.get<fixed_bitstring<9> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::types_opts::to_string() const
{
  static const char* options[] = {"ng-5G-S-TMSI", "ng-5G-S-TMSI-Part2"};
  return convert_enum_idx(options, 2, value, "rrc_setup_complete_ies_s::ng_minus5_g_s_tmsi_value_c_::types");
}

// SCGFailureInformation-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_scg_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_scg_present) {
    HANDLE_CODE(fail_report_scg.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_scg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_scg_present) {
    HANDLE_CODE(fail_report_scg.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_scg_present) {
    j.write_fieldname("failureReportSCG");
    fail_report_scg.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCGFailureInformationEUTRA-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_eutra_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_scg_eutra_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_scg_eutra_present) {
    HANDLE_CODE(fail_report_scg_eutra.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_scg_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_scg_eutra_present) {
    HANDLE_CODE(fail_report_scg_eutra.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_eutra_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_scg_eutra_present) {
    j.write_fieldname("failureReportSCG-EUTRA");
    fail_report_scg_eutra.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SecurityModeComplete-IEs ::= SEQUENCE
SRSASN_CODE security_mode_complete_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_complete_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
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

// SecurityModeFailure-IEs ::= SEQUENCE
SRSASN_CODE security_mode_fail_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_mode_fail_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
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

// UEAssistanceInformation-IEs ::= SEQUENCE
SRSASN_CODE ueassist_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delay_budget_report_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (delay_budget_report_present) {
    HANDLE_CODE(delay_budget_report.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delay_budget_report_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (delay_budget_report_present) {
    HANDLE_CODE(delay_budget_report.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueassist_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (delay_budget_report_present) {
    j.write_fieldname("delayBudgetReport");
    delay_budget_report.to_json(j);
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

// UECapabilityInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_cap_rat_container_list_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_cap_rat_container_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_cap_rat_container_list, 0, 8));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ue_cap_rat_container_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_cap_rat_container_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ue_cap_rat_container_list, bref, 0, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_cap_rat_container_list_present) {
    j.start_array("ue-CapabilityRAT-ContainerList");
    for (const auto& e1 : ue_cap_rat_container_list) {
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

// ULInformationTransfer-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_ies_s::pack(bit_ref& bref) const
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

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_ies_s::unpack(cbit_ref& bref)
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

  return SRSASN_SUCCESS;
}
void ul_info_transfer_ies_s::to_json(json_writer& j) const
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
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// ULInformationTransferMRDC-IEs ::= SEQUENCE
SRSASN_CODE ul_info_transfer_mrdc_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_dcch_msg_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(ul_dcch_msg_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_dcch_msg_nr.size() > 0) {
    HANDLE_CODE(ul_dcch_msg_nr.pack(bref));
  }
  if (ul_dcch_msg_eutra.size() > 0) {
    HANDLE_CODE(ul_dcch_msg_eutra.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_ies_s::unpack(cbit_ref& bref)
{
  bool ul_dcch_msg_nr_present;
  HANDLE_CODE(bref.unpack(ul_dcch_msg_nr_present, 1));
  bool ul_dcch_msg_eutra_present;
  HANDLE_CODE(bref.unpack(ul_dcch_msg_eutra_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_dcch_msg_nr_present) {
    HANDLE_CODE(ul_dcch_msg_nr.unpack(bref));
  }
  if (ul_dcch_msg_eutra_present) {
    HANDLE_CODE(ul_dcch_msg_eutra.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_info_transfer_mrdc_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_dcch_msg_nr.size() > 0) {
    j.write_str("ul-DCCH-MessageNR", ul_dcch_msg_nr.to_string());
  }
  if (ul_dcch_msg_eutra.size() > 0) {
    j.write_str("ul-DCCH-MessageEUTRA", ul_dcch_msg_eutra.to_string());
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

// CounterCheckResponse ::= SEQUENCE
SRSASN_CODE counter_check_resp_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void counter_check_resp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void counter_check_resp_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
counter_check_resp_ies_s& counter_check_resp_s::crit_exts_c_::set_counter_check_resp()
{
  set(types::counter_check_resp);
  return c;
}
void counter_check_resp_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void counter_check_resp_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::counter_check_resp:
      j.write_fieldname("counterCheckResponse");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE counter_check_resp_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::counter_check_resp:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE counter_check_resp_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::counter_check_resp:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "counter_check_resp_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* counter_check_resp_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"counterCheckResponse", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "counter_check_resp_s::crit_exts_c_::types");
}

// FailureInformation ::= SEQUENCE
SRSASN_CODE fail_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void fail_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void fail_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
fail_info_ies_s& fail_info_s::crit_exts_c_::set_fail_info()
{
  set(types::fail_info);
  return c;
}
void fail_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void fail_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fail_info:
      j.write_fieldname("failureInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "fail_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE fail_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fail_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fail_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* fail_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"failureInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "fail_info_s::crit_exts_c_::types");
}

// LocationMeasurementIndication ::= SEQUENCE
SRSASN_CODE location_meas_ind_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_ind_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void location_meas_ind_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void location_meas_ind_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
location_meas_ind_ies_s& location_meas_ind_s::crit_exts_c_::set_location_meas_ind()
{
  set(types::location_meas_ind);
  return c;
}
void location_meas_ind_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void location_meas_ind_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::location_meas_ind:
      j.write_fieldname("locationMeasurementIndication");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_ind_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE location_meas_ind_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::location_meas_ind:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_ind_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_meas_ind_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::location_meas_ind:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "location_meas_ind_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* location_meas_ind_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"locationMeasurementIndication", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "location_meas_ind_s::crit_exts_c_::types");
}

// MeasurementReport ::= SEQUENCE
SRSASN_CODE meas_report_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_report_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void meas_report_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_report_ies_s& meas_report_s::crit_exts_c_::set_meas_report()
{
  set(types::meas_report);
  return c;
}
void meas_report_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void meas_report_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_report:
      j.write_fieldname("measurementReport");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_report_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_report_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_report_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"measurementReport", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "meas_report_s::crit_exts_c_::types");
}

// RRCReconfigurationComplete ::= SEQUENCE
SRSASN_CODE rrc_recfg_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_recfg_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_recfg_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_recfg_complete_ies_s& rrc_recfg_complete_s::crit_exts_c_::set_rrc_recfg_complete()
{
  set(types::rrc_recfg_complete);
  return c;
}
void rrc_recfg_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_recfg_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_recfg_complete:
      j.write_fieldname("rrcReconfigurationComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_recfg_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_recfg_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_recfg_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_recfg_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcReconfigurationComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_recfg_complete_s::crit_exts_c_::types");
}

// RRCReestablishmentComplete ::= SEQUENCE
SRSASN_CODE rrc_reest_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_reest_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_reest_complete_ies_s& rrc_reest_complete_s::crit_exts_c_::set_rrc_reest_complete()
{
  set(types::rrc_reest_complete);
  return c;
}
void rrc_reest_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_reest_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_reest_complete:
      j.write_fieldname("rrcReestablishmentComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_reest_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_reest_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_reest_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_reest_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_reest_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcReestablishmentComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_reest_complete_s::crit_exts_c_::types");
}

// RRCResumeComplete ::= SEQUENCE
SRSASN_CODE rrc_resume_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_resume_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_resume_complete_ies_s& rrc_resume_complete_s::crit_exts_c_::set_rrc_resume_complete()
{
  set(types::rrc_resume_complete);
  return c;
}
void rrc_resume_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_resume_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_resume_complete:
      j.write_fieldname("rrcResumeComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_resume_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_resume_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_resume_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_resume_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_resume_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcResumeComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_resume_complete_s::crit_exts_c_::types");
}

// RRCSetupComplete ::= SEQUENCE
SRSASN_CODE rrc_setup_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_setup_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_setup_complete_ies_s& rrc_setup_complete_s::crit_exts_c_::set_rrc_setup_complete()
{
  set(types::rrc_setup_complete);
  return c;
}
void rrc_setup_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_setup_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_setup_complete:
      j.write_fieldname("rrcSetupComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_setup_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_setup_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_setup_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_setup_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_setup_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcSetupComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_setup_complete_s::crit_exts_c_::types");
}

// SCGFailureInformation ::= SEQUENCE
SRSASN_CODE scg_fail_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_fail_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_fail_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_ies_s& scg_fail_info_s::crit_exts_c_::set_scg_fail_info()
{
  set(types::scg_fail_info);
  return c;
}
void scg_fail_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_fail_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_fail_info:
      j.write_fieldname("scgFailureInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_fail_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_fail_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* scg_fail_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"scgFailureInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "scg_fail_info_s::crit_exts_c_::types");
}

// SCGFailureInformationEUTRA ::= SEQUENCE
SRSASN_CODE scg_fail_info_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_fail_info_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_fail_info_eutra_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_fail_info_eutra_ies_s& scg_fail_info_eutra_s::crit_exts_c_::set_scg_fail_info_eutra()
{
  set(types::scg_fail_info_eutra);
  return c;
}
void scg_fail_info_eutra_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_fail_info_eutra_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_fail_info_eutra:
      j.write_fieldname("scgFailureInformationEUTRA");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_eutra_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_fail_info_eutra_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_eutra_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_eutra_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_fail_info_eutra_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* scg_fail_info_eutra_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"scgFailureInformationEUTRA", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "scg_fail_info_eutra_s::crit_exts_c_::types");
}

// SecurityModeComplete ::= SEQUENCE
SRSASN_CODE security_mode_complete_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_complete_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_complete_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_complete_ies_s& security_mode_complete_s::crit_exts_c_::set_security_mode_complete()
{
  set(types::security_mode_complete);
  return c;
}
void security_mode_complete_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_complete_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_complete:
      j.write_fieldname("securityModeComplete");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_complete_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_complete:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_complete_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_complete:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_complete_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* security_mode_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "security_mode_complete_s::crit_exts_c_::types");
}

// SecurityModeFailure ::= SEQUENCE
SRSASN_CODE security_mode_fail_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void security_mode_fail_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void security_mode_fail_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
security_mode_fail_ies_s& security_mode_fail_s::crit_exts_c_::set_security_mode_fail()
{
  set(types::security_mode_fail);
  return c;
}
void security_mode_fail_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void security_mode_fail_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::security_mode_fail:
      j.write_fieldname("securityModeFailure");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE security_mode_fail_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::security_mode_fail:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE security_mode_fail_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::security_mode_fail:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "security_mode_fail_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* security_mode_fail_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeFailure", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "security_mode_fail_s::crit_exts_c_::types");
}

// UEAssistanceInformation ::= SEQUENCE
SRSASN_CODE ueassist_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ueassist_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ueassist_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ueassist_info_ies_s& ueassist_info_s::crit_exts_c_::set_ue_assist_info()
{
  set(types::ue_assist_info);
  return c;
}
void ueassist_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ueassist_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_assist_info:
      j.write_fieldname("ueAssistanceInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ueassist_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ueassist_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_assist_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ueassist_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ueassist_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_assist_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ueassist_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ueassist_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueAssistanceInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ueassist_info_s::crit_exts_c_::types");
}

// UECapabilityInformation ::= SEQUENCE
SRSASN_CODE ue_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_cap_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_info_ies_s& ue_cap_info_s::crit_exts_c_::set_ue_cap_info()
{
  set(types::ue_cap_info);
  return c;
}
void ue_cap_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_cap_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_cap_info:
      j.write_fieldname("ueCapabilityInformation");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_cap_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_cap_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_cap_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueCapabilityInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ue_cap_info_s::crit_exts_c_::types");
}

// ULInformationTransfer ::= SEQUENCE
SRSASN_CODE ul_info_transfer_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_ies_s& ul_info_transfer_s::crit_exts_c_::set_ul_info_transfer()
{
  set(types::ul_info_transfer);
  return c;
}
void ul_info_transfer_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_info_transfer_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer:
      j.write_fieldname("ulInformationTransfer");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_info_transfer_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulInformationTransfer", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ul_info_transfer_s::crit_exts_c_::types");
}

// ULInformationTransferMRDC ::= SEQUENCE
SRSASN_CODE ul_info_transfer_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_info_transfer_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ul_info_transfer_mrdc_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_& ul_info_transfer_mrdc_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ul_info_transfer_mrdc_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ul_info_transfer_mrdc_ies_s& ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_ul_info_transfer_mrdc()
{
  set(types::ul_info_transfer_mrdc);
  return c;
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ul_info_transfer_mrdc:
      j.write_fieldname("ulInformationTransferMRDC");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulInformationTransferMRDC", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::types");
}

const char* ul_info_transfer_mrdc_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ul_info_transfer_mrdc_s::crit_exts_c_::types");
}
uint8_t ul_info_transfer_mrdc_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_info_transfer_mrdc_s::crit_exts_c_::types");
}

// UL-DCCH-MessageType ::= CHOICE
void ul_dcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
ul_dcch_msg_type_c::c1_c_& ul_dcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void ul_dcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void ul_dcch_msg_type_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_dcch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::meas_report:
      c.destroy<meas_report_s>();
      break;
    case types::rrc_recfg_complete:
      c.destroy<rrc_recfg_complete_s>();
      break;
    case types::rrc_setup_complete:
      c.destroy<rrc_setup_complete_s>();
      break;
    case types::rrc_reest_complete:
      c.destroy<rrc_reest_complete_s>();
      break;
    case types::rrc_resume_complete:
      c.destroy<rrc_resume_complete_s>();
      break;
    case types::security_mode_complete:
      c.destroy<security_mode_complete_s>();
      break;
    case types::security_mode_fail:
      c.destroy<security_mode_fail_s>();
      break;
    case types::ul_info_transfer:
      c.destroy<ul_info_transfer_s>();
      break;
    case types::location_meas_ind:
      c.destroy<location_meas_ind_s>();
      break;
    case types::ue_cap_info:
      c.destroy<ue_cap_info_s>();
      break;
    case types::counter_check_resp:
      c.destroy<counter_check_resp_s>();
      break;
    case types::ue_assist_info:
      c.destroy<ueassist_info_s>();
      break;
    case types::fail_info:
      c.destroy<fail_info_s>();
      break;
    case types::ul_info_transfer_mrdc:
      c.destroy<ul_info_transfer_mrdc_s>();
      break;
    case types::scg_fail_info:
      c.destroy<scg_fail_info_s>();
      break;
    case types::scg_fail_info_eutra:
      c.destroy<scg_fail_info_eutra_s>();
      break;
    default:
      break;
  }
}
void ul_dcch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_report:
      c.init<meas_report_s>();
      break;
    case types::rrc_recfg_complete:
      c.init<rrc_recfg_complete_s>();
      break;
    case types::rrc_setup_complete:
      c.init<rrc_setup_complete_s>();
      break;
    case types::rrc_reest_complete:
      c.init<rrc_reest_complete_s>();
      break;
    case types::rrc_resume_complete:
      c.init<rrc_resume_complete_s>();
      break;
    case types::security_mode_complete:
      c.init<security_mode_complete_s>();
      break;
    case types::security_mode_fail:
      c.init<security_mode_fail_s>();
      break;
    case types::ul_info_transfer:
      c.init<ul_info_transfer_s>();
      break;
    case types::location_meas_ind:
      c.init<location_meas_ind_s>();
      break;
    case types::ue_cap_info:
      c.init<ue_cap_info_s>();
      break;
    case types::counter_check_resp:
      c.init<counter_check_resp_s>();
      break;
    case types::ue_assist_info:
      c.init<ueassist_info_s>();
      break;
    case types::fail_info:
      c.init<fail_info_s>();
      break;
    case types::ul_info_transfer_mrdc:
      c.init<ul_info_transfer_mrdc_s>();
      break;
    case types::scg_fail_info:
      c.init<scg_fail_info_s>();
      break;
    case types::scg_fail_info_eutra:
      c.init<scg_fail_info_eutra_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
}
ul_dcch_msg_type_c::c1_c_::c1_c_(const ul_dcch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_report:
      c.init(other.c.get<meas_report_s>());
      break;
    case types::rrc_recfg_complete:
      c.init(other.c.get<rrc_recfg_complete_s>());
      break;
    case types::rrc_setup_complete:
      c.init(other.c.get<rrc_setup_complete_s>());
      break;
    case types::rrc_reest_complete:
      c.init(other.c.get<rrc_reest_complete_s>());
      break;
    case types::rrc_resume_complete:
      c.init(other.c.get<rrc_resume_complete_s>());
      break;
    case types::security_mode_complete:
      c.init(other.c.get<security_mode_complete_s>());
      break;
    case types::security_mode_fail:
      c.init(other.c.get<security_mode_fail_s>());
      break;
    case types::ul_info_transfer:
      c.init(other.c.get<ul_info_transfer_s>());
      break;
    case types::location_meas_ind:
      c.init(other.c.get<location_meas_ind_s>());
      break;
    case types::ue_cap_info:
      c.init(other.c.get<ue_cap_info_s>());
      break;
    case types::counter_check_resp:
      c.init(other.c.get<counter_check_resp_s>());
      break;
    case types::ue_assist_info:
      c.init(other.c.get<ueassist_info_s>());
      break;
    case types::fail_info:
      c.init(other.c.get<fail_info_s>());
      break;
    case types::ul_info_transfer_mrdc:
      c.init(other.c.get<ul_info_transfer_mrdc_s>());
      break;
    case types::scg_fail_info:
      c.init(other.c.get<scg_fail_info_s>());
      break;
    case types::scg_fail_info_eutra:
      c.init(other.c.get<scg_fail_info_eutra_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
}
ul_dcch_msg_type_c::c1_c_& ul_dcch_msg_type_c::c1_c_::operator=(const ul_dcch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_report:
      c.set(other.c.get<meas_report_s>());
      break;
    case types::rrc_recfg_complete:
      c.set(other.c.get<rrc_recfg_complete_s>());
      break;
    case types::rrc_setup_complete:
      c.set(other.c.get<rrc_setup_complete_s>());
      break;
    case types::rrc_reest_complete:
      c.set(other.c.get<rrc_reest_complete_s>());
      break;
    case types::rrc_resume_complete:
      c.set(other.c.get<rrc_resume_complete_s>());
      break;
    case types::security_mode_complete:
      c.set(other.c.get<security_mode_complete_s>());
      break;
    case types::security_mode_fail:
      c.set(other.c.get<security_mode_fail_s>());
      break;
    case types::ul_info_transfer:
      c.set(other.c.get<ul_info_transfer_s>());
      break;
    case types::location_meas_ind:
      c.set(other.c.get<location_meas_ind_s>());
      break;
    case types::ue_cap_info:
      c.set(other.c.get<ue_cap_info_s>());
      break;
    case types::counter_check_resp:
      c.set(other.c.get<counter_check_resp_s>());
      break;
    case types::ue_assist_info:
      c.set(other.c.get<ueassist_info_s>());
      break;
    case types::fail_info:
      c.set(other.c.get<fail_info_s>());
      break;
    case types::ul_info_transfer_mrdc:
      c.set(other.c.get<ul_info_transfer_mrdc_s>());
      break;
    case types::scg_fail_info:
      c.set(other.c.get<scg_fail_info_s>());
      break;
    case types::scg_fail_info_eutra:
      c.set(other.c.get<scg_fail_info_eutra_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }

  return *this;
}
meas_report_s& ul_dcch_msg_type_c::c1_c_::set_meas_report()
{
  set(types::meas_report);
  return c.get<meas_report_s>();
}
rrc_recfg_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_recfg_complete()
{
  set(types::rrc_recfg_complete);
  return c.get<rrc_recfg_complete_s>();
}
rrc_setup_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_setup_complete()
{
  set(types::rrc_setup_complete);
  return c.get<rrc_setup_complete_s>();
}
rrc_reest_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_reest_complete()
{
  set(types::rrc_reest_complete);
  return c.get<rrc_reest_complete_s>();
}
rrc_resume_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_resume_complete()
{
  set(types::rrc_resume_complete);
  return c.get<rrc_resume_complete_s>();
}
security_mode_complete_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_complete()
{
  set(types::security_mode_complete);
  return c.get<security_mode_complete_s>();
}
security_mode_fail_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_fail()
{
  set(types::security_mode_fail);
  return c.get<security_mode_fail_s>();
}
ul_info_transfer_s& ul_dcch_msg_type_c::c1_c_::set_ul_info_transfer()
{
  set(types::ul_info_transfer);
  return c.get<ul_info_transfer_s>();
}
location_meas_ind_s& ul_dcch_msg_type_c::c1_c_::set_location_meas_ind()
{
  set(types::location_meas_ind);
  return c.get<location_meas_ind_s>();
}
ue_cap_info_s& ul_dcch_msg_type_c::c1_c_::set_ue_cap_info()
{
  set(types::ue_cap_info);
  return c.get<ue_cap_info_s>();
}
counter_check_resp_s& ul_dcch_msg_type_c::c1_c_::set_counter_check_resp()
{
  set(types::counter_check_resp);
  return c.get<counter_check_resp_s>();
}
ueassist_info_s& ul_dcch_msg_type_c::c1_c_::set_ue_assist_info()
{
  set(types::ue_assist_info);
  return c.get<ueassist_info_s>();
}
fail_info_s& ul_dcch_msg_type_c::c1_c_::set_fail_info()
{
  set(types::fail_info);
  return c.get<fail_info_s>();
}
ul_info_transfer_mrdc_s& ul_dcch_msg_type_c::c1_c_::set_ul_info_transfer_mrdc()
{
  set(types::ul_info_transfer_mrdc);
  return c.get<ul_info_transfer_mrdc_s>();
}
scg_fail_info_s& ul_dcch_msg_type_c::c1_c_::set_scg_fail_info()
{
  set(types::scg_fail_info);
  return c.get<scg_fail_info_s>();
}
scg_fail_info_eutra_s& ul_dcch_msg_type_c::c1_c_::set_scg_fail_info_eutra()
{
  set(types::scg_fail_info_eutra);
  return c.get<scg_fail_info_eutra_s>();
}
void ul_dcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_report:
      j.write_fieldname("measurementReport");
      c.get<meas_report_s>().to_json(j);
      break;
    case types::rrc_recfg_complete:
      j.write_fieldname("rrcReconfigurationComplete");
      c.get<rrc_recfg_complete_s>().to_json(j);
      break;
    case types::rrc_setup_complete:
      j.write_fieldname("rrcSetupComplete");
      c.get<rrc_setup_complete_s>().to_json(j);
      break;
    case types::rrc_reest_complete:
      j.write_fieldname("rrcReestablishmentComplete");
      c.get<rrc_reest_complete_s>().to_json(j);
      break;
    case types::rrc_resume_complete:
      j.write_fieldname("rrcResumeComplete");
      c.get<rrc_resume_complete_s>().to_json(j);
      break;
    case types::security_mode_complete:
      j.write_fieldname("securityModeComplete");
      c.get<security_mode_complete_s>().to_json(j);
      break;
    case types::security_mode_fail:
      j.write_fieldname("securityModeFailure");
      c.get<security_mode_fail_s>().to_json(j);
      break;
    case types::ul_info_transfer:
      j.write_fieldname("ulInformationTransfer");
      c.get<ul_info_transfer_s>().to_json(j);
      break;
    case types::location_meas_ind:
      j.write_fieldname("locationMeasurementIndication");
      c.get<location_meas_ind_s>().to_json(j);
      break;
    case types::ue_cap_info:
      j.write_fieldname("ueCapabilityInformation");
      c.get<ue_cap_info_s>().to_json(j);
      break;
    case types::counter_check_resp:
      j.write_fieldname("counterCheckResponse");
      c.get<counter_check_resp_s>().to_json(j);
      break;
    case types::ue_assist_info:
      j.write_fieldname("ueAssistanceInformation");
      c.get<ueassist_info_s>().to_json(j);
      break;
    case types::fail_info:
      j.write_fieldname("failureInformation");
      c.get<fail_info_s>().to_json(j);
      break;
    case types::ul_info_transfer_mrdc:
      j.write_fieldname("ulInformationTransferMRDC");
      c.get<ul_info_transfer_mrdc_s>().to_json(j);
      break;
    case types::scg_fail_info:
      j.write_fieldname("scgFailureInformation");
      c.get<scg_fail_info_s>().to_json(j);
      break;
    case types::scg_fail_info_eutra:
      j.write_fieldname("scgFailureInformationEUTRA");
      c.get<scg_fail_info_eutra_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_dcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().pack(bref));
      break;
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.get<rrc_recfg_complete_s>().pack(bref));
      break;
    case types::rrc_setup_complete:
      HANDLE_CODE(c.get<rrc_setup_complete_s>().pack(bref));
      break;
    case types::rrc_reest_complete:
      HANDLE_CODE(c.get<rrc_reest_complete_s>().pack(bref));
      break;
    case types::rrc_resume_complete:
      HANDLE_CODE(c.get<rrc_resume_complete_s>().pack(bref));
      break;
    case types::security_mode_complete:
      HANDLE_CODE(c.get<security_mode_complete_s>().pack(bref));
      break;
    case types::security_mode_fail:
      HANDLE_CODE(c.get<security_mode_fail_s>().pack(bref));
      break;
    case types::ul_info_transfer:
      HANDLE_CODE(c.get<ul_info_transfer_s>().pack(bref));
      break;
    case types::location_meas_ind:
      HANDLE_CODE(c.get<location_meas_ind_s>().pack(bref));
      break;
    case types::ue_cap_info:
      HANDLE_CODE(c.get<ue_cap_info_s>().pack(bref));
      break;
    case types::counter_check_resp:
      HANDLE_CODE(c.get<counter_check_resp_s>().pack(bref));
      break;
    case types::ue_assist_info:
      HANDLE_CODE(c.get<ueassist_info_s>().pack(bref));
      break;
    case types::fail_info:
      HANDLE_CODE(c.get<fail_info_s>().pack(bref));
      break;
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.get<ul_info_transfer_mrdc_s>().pack(bref));
      break;
    case types::scg_fail_info:
      HANDLE_CODE(c.get<scg_fail_info_s>().pack(bref));
      break;
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.get<scg_fail_info_eutra_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_report:
      HANDLE_CODE(c.get<meas_report_s>().unpack(bref));
      break;
    case types::rrc_recfg_complete:
      HANDLE_CODE(c.get<rrc_recfg_complete_s>().unpack(bref));
      break;
    case types::rrc_setup_complete:
      HANDLE_CODE(c.get<rrc_setup_complete_s>().unpack(bref));
      break;
    case types::rrc_reest_complete:
      HANDLE_CODE(c.get<rrc_reest_complete_s>().unpack(bref));
      break;
    case types::rrc_resume_complete:
      HANDLE_CODE(c.get<rrc_resume_complete_s>().unpack(bref));
      break;
    case types::security_mode_complete:
      HANDLE_CODE(c.get<security_mode_complete_s>().unpack(bref));
      break;
    case types::security_mode_fail:
      HANDLE_CODE(c.get<security_mode_fail_s>().unpack(bref));
      break;
    case types::ul_info_transfer:
      HANDLE_CODE(c.get<ul_info_transfer_s>().unpack(bref));
      break;
    case types::location_meas_ind:
      HANDLE_CODE(c.get<location_meas_ind_s>().unpack(bref));
      break;
    case types::ue_cap_info:
      HANDLE_CODE(c.get<ue_cap_info_s>().unpack(bref));
      break;
    case types::counter_check_resp:
      HANDLE_CODE(c.get<counter_check_resp_s>().unpack(bref));
      break;
    case types::ue_assist_info:
      HANDLE_CODE(c.get<ueassist_info_s>().unpack(bref));
      break;
    case types::fail_info:
      HANDLE_CODE(c.get<fail_info_s>().unpack(bref));
      break;
    case types::ul_info_transfer_mrdc:
      HANDLE_CODE(c.get<ul_info_transfer_mrdc_s>().unpack(bref));
      break;
    case types::scg_fail_info:
      HANDLE_CODE(c.get<scg_fail_info_s>().unpack(bref));
      break;
    case types::scg_fail_info_eutra:
      HANDLE_CODE(c.get<scg_fail_info_eutra_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_dcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"measurementReport",
                                  "rrcReconfigurationComplete",
                                  "rrcSetupComplete",
                                  "rrcReestablishmentComplete",
                                  "rrcResumeComplete",
                                  "securityModeComplete",
                                  "securityModeFailure",
                                  "ulInformationTransfer",
                                  "locationMeasurementIndication",
                                  "ueCapabilityInformation",
                                  "counterCheckResponse",
                                  "ueAssistanceInformation",
                                  "failureInformation",
                                  "ulInformationTransferMRDC",
                                  "scgFailureInformation",
                                  "scgFailureInformationEUTRA"};
  return convert_enum_idx(options, 16, value, "ul_dcch_msg_type_c::c1_c_::types");
}

const char* ul_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_dcch_msg_type_c::types");
}
uint8_t ul_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_dcch_msg_type_c::types");
}

// UL-DCCH-Message ::= SEQUENCE
SRSASN_CODE ul_dcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_dcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_dcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-DCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::to_string() const
{
  static const char* options[] = {"row1", "row2", "row4", "other"};
  return convert_enum_idx(options, 4, value, "csi_rs_res_map_s::freq_domain_alloc_c_::types");
}
uint8_t csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "csi_rs_res_map_s::freq_domain_alloc_c_::types");
}

const char* csi_rs_res_map_s::density_c_::types_opts::to_string() const
{
  static const char* options[] = {"dot5", "one", "three", "spare"};
  return convert_enum_idx(options, 4, value, "csi_rs_res_map_s::density_c_::types");
}
uint8_t csi_rs_res_map_s::density_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {5, 1, 3};
  return map_enum_number(options, 3, value, "csi_rs_res_map_s::density_c_::types");
}

const char* csi_res_periodicity_and_offset_c::types_opts::to_string() const
{
  static const char* options[] = {"slots4",
                                  "slots5",
                                  "slots8",
                                  "slots10",
                                  "slots16",
                                  "slots20",
                                  "slots32",
                                  "slots40",
                                  "slots64",
                                  "slots80",
                                  "slots160",
                                  "slots320",
                                  "slots640"};
  return convert_enum_idx(options, 13, value, "csi_res_periodicity_and_offset_c::types");
}
uint16_t csi_res_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t options[] = {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640};
  return map_enum_number(options, 13, value, "csi_res_periodicity_and_offset_c::types");
}

const char* qcl_info_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* options[] = {"csi-rs", "ssb"};
  return convert_enum_idx(options, 2, value, "qcl_info_s::ref_sig_c_::types");
}

const char* radio_link_monitoring_rs_s::detection_res_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(options, 2, value, "radio_link_monitoring_rs_s::detection_res_c_::types");
}

const char* rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneSlot", "twoSlots"};
  return convert_enum_idx(
      options, 2, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types");
}
uint8_t rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options, 2, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types");
}

const char* rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n5", "n8", "n10", "n20", "n40"};
  return convert_enum_idx(
      options, 7, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types");
}
uint8_t rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 5, 8, 10, 20, 40};
  return map_enum_number(
      options, 7, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types");
}

const char* rate_match_pattern_s::pattern_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"bitmaps", "controlResourceSet"};
  return convert_enum_idx(options, 2, value, "rate_match_pattern_s::pattern_type_c_::types");
}

const char* rate_match_pattern_group_item_c_::types_opts::to_string() const
{
  static const char* options[] = {"cellLevel", "bwpLevel"};
  return convert_enum_idx(options, 2, value, "rate_match_pattern_group_item_c_::types");
}

const char* pdsch_cfg_s::prb_bundling_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"staticBundling", "dynamicBundling"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_s::prb_bundling_type_c_::types");
}

const char* pucch_pathloss_ref_rs_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(options, 2, value, "pucch_pathloss_ref_rs_s::ref_sig_c_::types");
}

const char* pusch_pathloss_ref_rs_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(options, 2, value, "pusch_pathloss_ref_rs_s::ref_sig_c_::types");
}

const char* srs_periodicity_and_offset_c::types_opts::to_string() const
{
  static const char* options[] = {"sl1",
                                  "sl2",
                                  "sl4",
                                  "sl5",
                                  "sl8",
                                  "sl10",
                                  "sl16",
                                  "sl20",
                                  "sl32",
                                  "sl40",
                                  "sl64",
                                  "sl80",
                                  "sl160",
                                  "sl320",
                                  "sl640",
                                  "sl1280",
                                  "sl2560"};
  return convert_enum_idx(options, 17, value, "srs_periodicity_and_offset_c::types");
}
uint16_t srs_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560};
  return map_enum_number(options, 17, value, "srs_periodicity_and_offset_c::types");
}

const char* srs_spatial_relation_info_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb-Index", "csi-RS-Index", "srs"};
  return convert_enum_idx(options, 3, value, "srs_spatial_relation_info_s::ref_sig_c_::types");
}

const char* cg_uci_on_pusch_c::types_opts::to_string() const
{
  static const char* options[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(options, 2, value, "cg_uci_on_pusch_c::types");
}

const char* prach_res_ded_bfr_c::types_opts::to_string() const
{
  static const char* options[] = {"ssb", "csi-RS"};
  return convert_enum_idx(options, 2, value, "prach_res_ded_bfr_c::types");
}

const char* pucch_res_s::format_c_::types_opts::to_string() const
{
  static const char* options[] = {"format0", "format1", "format2", "format3", "format4"};
  return convert_enum_idx(options, 5, value, "pucch_res_s::format_c_::types");
}
uint8_t pucch_res_s::format_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4};
  return map_enum_number(options, 5, value, "pucch_res_s::format_c_::types");
}

const char* pucch_spatial_relation_info_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb-Index", "csi-RS-Index", "srs"};
  return convert_enum_idx(options, 3, value, "pucch_spatial_relation_info_s::ref_sig_c_::types");
}

const char* srs_res_s::tx_comb_c_::types_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_res_s::tx_comb_c_::types");
}
uint8_t srs_res_s::tx_comb_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_res_s::tx_comb_c_::types");
}

const char* srs_res_s::res_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"aperiodic", "semi-persistent", "periodic"};
  return convert_enum_idx(options, 3, value, "srs_res_s::res_type_c_::types");
}

const char* srs_res_set_s::res_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"aperiodic", "semi-persistent", "periodic"};
  return convert_enum_idx(options, 3, value, "srs_res_set_s::res_type_c_::types");
}

const char* srs_res_set_s::pathloss_ref_rs_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(options, 2, value, "srs_res_set_s::pathloss_ref_rs_c_::types");
}

const char* sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::to_string() const
{
  static const char* options[] = {"sym2",
                                  "sym6or7",
                                  "sl1",
                                  "sl2",
                                  "sl4",
                                  "sl5",
                                  "sl8",
                                  "sl10",
                                  "sl16",
                                  "sl20",
                                  "sl40",
                                  "sl80",
                                  "sl160",
                                  "sl320",
                                  "sl640"};
  return convert_enum_idx(options, 15, value, "sched_request_res_cfg_s::periodicity_and_offset_c_::types");
}

const char* uci_on_pusch_s::beta_offsets_c_::types_opts::to_string() const
{
  static const char* options[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(options, 2, value, "uci_on_pusch_s::beta_offsets_c_::types");
}

const char* band_params_c::types_opts::to_string() const
{
  static const char* options[] = {"eutra", "nr"};
  return convert_enum_idx(options, 2, value, "band_params_c::types");
}

const char* band_params_v1540_s::srs_carrier_switch_c_::types_opts::to_string() const
{
  static const char* options[] = {"nr", "eutra"};
  return convert_enum_idx(options, 2, value, "band_params_v1540_s::srs_carrier_switch_c_::types");
}

// CA-ParametersEUTRA-v1570 ::= SEQUENCE
SRSASN_CODE ca_params_eutra_v1570_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_minus1024_qam_total_weighted_layers_present, 1));

  if (dl_minus1024_qam_total_weighted_layers_present) {
    HANDLE_CODE(pack_integer(bref, dl_minus1024_qam_total_weighted_layers, (uint8_t)0u, (uint8_t)10u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_eutra_v1570_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_minus1024_qam_total_weighted_layers_present, 1));

  if (dl_minus1024_qam_total_weighted_layers_present) {
    HANDLE_CODE(unpack_integer(dl_minus1024_qam_total_weighted_layers, bref, (uint8_t)0u, (uint8_t)10u));
  }

  return SRSASN_SUCCESS;
}
void ca_params_eutra_v1570_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_minus1024_qam_total_weighted_layers_present) {
    j.write_int("dl-1024QAM-TotalWeightedLayers", dl_minus1024_qam_total_weighted_layers);
  }
  j.end_obj();
}

// BandCombination-v1570 ::= SEQUENCE
SRSASN_CODE band_combination_v1570_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ca_params_eutra_v1570.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v1570_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ca_params_eutra_v1570.unpack(bref));

  return SRSASN_SUCCESS;
}
void band_combination_v1570_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ca-ParametersEUTRA-v1570");
  ca_params_eutra_v1570.to_json(j);
  j.end_obj();
}

// MRDC-Parameters-v1580 ::= SEQUENCE
SRSASN_CODE mrdc_params_v1580_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dynamic_pwr_sharing_nedc_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_params_v1580_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dynamic_pwr_sharing_nedc_present, 1));

  return SRSASN_SUCCESS;
}
void mrdc_params_v1580_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dynamic_pwr_sharing_nedc_present) {
    j.write_str("dynamicPowerSharingNEDC", "supported");
  }
  j.end_obj();
}

// BandCombination-v1580 ::= SEQUENCE
SRSASN_CODE band_combination_v1580_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mrdc_params_v1580.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v1580_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mrdc_params_v1580.unpack(bref));

  return SRSASN_SUCCESS;
}
void band_combination_v1580_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mrdc-Parameters-v1580");
  mrdc_params_v1580.to_json(j);
  j.end_obj();
}

// MRDC-Parameters-v1590 ::= SEQUENCE
SRSASN_CODE mrdc_params_v1590_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_band_contiguous_mrdc_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_params_v1590_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_band_contiguous_mrdc_present, 1));

  return SRSASN_SUCCESS;
}
void mrdc_params_v1590_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_band_contiguous_mrdc_present) {
    j.write_str("interBandContiguousMRDC", "supported");
  }
  j.end_obj();
}

// BandCombination-v1590 ::= SEQUENCE
SRSASN_CODE band_combination_v1590_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_bw_combination_set_intra_endc_present, 1));

  if (supported_bw_combination_set_intra_endc_present) {
    HANDLE_CODE(supported_bw_combination_set_intra_endc.pack(bref));
  }
  HANDLE_CODE(mrdc_params_v1590.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v1590_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_bw_combination_set_intra_endc_present, 1));

  if (supported_bw_combination_set_intra_endc_present) {
    HANDLE_CODE(supported_bw_combination_set_intra_endc.unpack(bref));
  }
  HANDLE_CODE(mrdc_params_v1590.unpack(bref));

  return SRSASN_SUCCESS;
}
void band_combination_v1590_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_bw_combination_set_intra_endc_present) {
    j.write_str("supportedBandwidthCombinationSetIntraENDC", supported_bw_combination_set_intra_endc.to_string());
  }
  j.write_fieldname("mrdc-Parameters-v1590");
  mrdc_params_v1590.to_json(j);
  j.end_obj();
}

const char* band_nr_s::ch_bws_dl_c_::types_opts::to_string() const
{
  static const char* options[] = {"fr1", "fr2"};
  return convert_enum_idx(options, 2, value, "band_nr_s::ch_bws_dl_c_::types");
}
uint8_t band_nr_s::ch_bws_dl_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "band_nr_s::ch_bws_dl_c_::types");
}

const char* band_nr_s::ch_bws_ul_c_::types_opts::to_string() const
{
  static const char* options[] = {"fr1", "fr2"};
  return convert_enum_idx(options, 2, value, "band_nr_s::ch_bws_ul_c_::types");
}
uint8_t band_nr_s::ch_bws_ul_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "band_nr_s::ch_bws_ul_c_::types");
}

const char* band_nr_s::ch_bws_dl_v1590_c_::types_opts::to_string() const
{
  static const char* options[] = {"fr1", "fr2"};
  return convert_enum_idx(options, 2, value, "band_nr_s::ch_bws_dl_v1590_c_::types");
}
uint8_t band_nr_s::ch_bws_dl_v1590_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "band_nr_s::ch_bws_dl_v1590_c_::types");
}

const char* band_nr_s::ch_bws_ul_v1590_c_::types_opts::to_string() const
{
  static const char* options[] = {"fr1", "fr2"};
  return convert_enum_idx(options, 2, value, "band_nr_s::ch_bws_ul_v1590_c_::types");
}
uint8_t band_nr_s::ch_bws_ul_v1590_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "band_nr_s::ch_bws_ul_v1590_c_::types");
}

// CFRA-CSIRS-Resource ::= SEQUENCE
SRSASN_CODE cfra_csirs_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_rs, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(pack_dyn_seq_of(bref, ra_occasion_list, 1, 64, integer_packer<uint16_t>(0, 511)));
  HANDLE_CODE(pack_integer(bref, ra_preamb_idx, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfra_csirs_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_rs, bref, (uint8_t)0u, (uint8_t)95u));
  HANDLE_CODE(unpack_dyn_seq_of(ra_occasion_list, bref, 1, 64, integer_packer<uint16_t>(0, 511)));
  HANDLE_CODE(unpack_integer(ra_preamb_idx, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void cfra_csirs_res_s::to_json(json_writer& j) const
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

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfra_ssb_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ssb, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(ra_preamb_idx, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void cfra_ssb_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb", ssb);
  j.write_int("ra-PreambleIndex", ra_preamb_idx);
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
  static const char* options[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(options, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
}
float cfra_s::occasions_s_::ssb_per_rach_occasion_opts::to_number() const
{
  static const float options[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(options, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
}
const char* cfra_s::occasions_s_::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* options[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(options, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
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

const char* cfra_s::res_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb", "csirs"};
  return convert_enum_idx(options, 2, value, "cfra_s::res_c_::types");
}

const char* csi_associated_report_cfg_info_s::res_for_ch_c_::types_opts::to_string() const
{
  static const char* options[] = {"nzp-CSI-RS", "csi-SSB-ResourceSet"};
  return convert_enum_idx(options, 2, value, "csi_associated_report_cfg_info_s::res_for_ch_c_::types");
}

const char* csi_im_res_s::csi_im_res_elem_pattern_c_::types_opts::to_string() const
{
  static const char* options[] = {"pattern0", "pattern1"};
  return convert_enum_idx(options, 2, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::types");
}
uint8_t csi_im_res_s::csi_im_res_elem_pattern_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::types");
}

const char* csi_report_periodicity_and_offset_c::types_opts::to_string() const
{
  static const char* options[] = {
      "slots4", "slots5", "slots8", "slots10", "slots16", "slots20", "slots40", "slots80", "slots160", "slots320"};
  return convert_enum_idx(options, 10, value, "csi_report_periodicity_and_offset_c::types");
}
uint16_t csi_report_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t options[] = {4, 5, 8, 10, 16, 20, 40, 80, 160, 320};
  return map_enum_number(options, 10, value, "csi_report_periodicity_and_offset_c::types");
}

const char* codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::types_opts::to_string() const
{
  static const char* options[] = {"two-one-TypeI-SinglePanel-Restriction",
                                  "two-two-TypeI-SinglePanel-Restriction",
                                  "four-one-TypeI-SinglePanel-Restriction",
                                  "three-two-TypeI-SinglePanel-Restriction",
                                  "six-one-TypeI-SinglePanel-Restriction",
                                  "four-two-TypeI-SinglePanel-Restriction",
                                  "eight-one-TypeI-SinglePanel-Restriction",
                                  "four-three-TypeI-SinglePanel-Restriction",
                                  "six-two-TypeI-SinglePanel-Restriction",
                                  "twelve-one-TypeI-SinglePanel-Restriction",
                                  "four-four-TypeI-SinglePanel-Restriction",
                                  "eight-two-TypeI-SinglePanel-Restriction",
                                  "sixteen-one-TypeI-SinglePanel-Restriction"};
  return convert_enum_idx(options,
                          13,
                          value,
                          "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_"
                          "ports_c_::more_than_two_s_::n1_n2_c_::types");
}

const char* codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    types_opts::to_string() const
{
  static const char* options[] = {"two", "moreThanTwo"};
  return convert_enum_idx(
      options,
      2,
      value,
      "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types");
}
uint8_t codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(
      options,
      1,
      value,
      "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types");
}

const char*
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types_opts::to_string()
    const
{
  static const char* options[] = {"two-two-one-TypeI-MultiPanel-Restriction",
                                  "two-four-one-TypeI-MultiPanel-Restriction",
                                  "four-two-one-TypeI-MultiPanel-Restriction",
                                  "two-two-two-TypeI-MultiPanel-Restriction",
                                  "two-eight-one-TypeI-MultiPanel-Restriction",
                                  "four-four-one-TypeI-MultiPanel-Restriction",
                                  "two-four-two-TypeI-MultiPanel-Restriction",
                                  "four-two-two-TypeI-MultiPanel-Restriction"};
  return convert_enum_idx(
      options,
      8,
      value,
      "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types");
}

const char* codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"typeI-SinglePanel", "typeI-MultiPanel"};
  return convert_enum_idx(options, 2, value, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types");
}

const char* codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::
    types_opts::to_string() const
{
  static const char* options[] = {"two-one",
                                  "two-two",
                                  "four-one",
                                  "three-two",
                                  "six-one",
                                  "four-two",
                                  "eight-one",
                                  "four-three",
                                  "six-two",
                                  "twelve-one",
                                  "four-four",
                                  "eight-two",
                                  "sixteen-one"};
  return convert_enum_idx(
      options,
      13,
      value,
      "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::types");
}

const char* codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"typeII", "typeII-PortSelection"};
  return convert_enum_idx(options, 2, value, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types");
}

const char* codebook_cfg_s::codebook_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"type1", "type2"};
  return convert_enum_idx(options, 2, value, "codebook_cfg_s::codebook_type_c_::types");
}
uint8_t codebook_cfg_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "codebook_cfg_s::codebook_type_c_::types");
}

const char* port_idx_for8_ranks_c::types_opts::to_string() const
{
  static const char* options[] = {"portIndex8", "portIndex4", "portIndex2", "portIndex1"};
  return convert_enum_idx(options, 4, value, "port_idx_for8_ranks_c::types");
}
uint8_t port_idx_for8_ranks_c::types_opts::to_number() const
{
  static const uint8_t options[] = {8, 4, 2, 1};
  return map_enum_number(options, 4, value, "port_idx_for8_ranks_c::types");
}

const char* csi_report_cfg_s::report_cfg_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"periodic", "semiPersistentOnPUCCH", "semiPersistentOnPUSCH", "aperiodic"};
  return convert_enum_idx(options, 4, value, "csi_report_cfg_s::report_cfg_type_c_::types");
}

const char* csi_report_cfg_s::report_quant_c_::types_opts::to_string() const
{
  static const char* options[] = {"none",
                                  "cri-RI-PMI-CQI",
                                  "cri-RI-i1",
                                  "cri-RI-i1-CQI",
                                  "cri-RI-CQI",
                                  "cri-RSRP",
                                  "ssb-Index-RSRP",
                                  "cri-RI-LI-PMI-CQI"};
  return convert_enum_idx(options, 8, value, "csi_report_cfg_s::report_quant_c_::types");
}

const char* csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types_opts::to_string() const
{
  static const char* options[] = {"subbands3",
                                  "subbands4",
                                  "subbands5",
                                  "subbands6",
                                  "subbands7",
                                  "subbands8",
                                  "subbands9",
                                  "subbands10",
                                  "subbands11",
                                  "subbands12",
                                  "subbands13",
                                  "subbands14",
                                  "subbands15",
                                  "subbands16",
                                  "subbands17",
                                  "subbands18",
                                  "subbands19-v1530"};
  return convert_enum_idx(options, 17, value, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types");
}
uint8_t csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  return map_enum_number(options, 17, value, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types");
}

const char* csi_report_cfg_s::group_based_beam_report_c_::types_opts::to_string() const
{
  static const char* options[] = {"enabled", "disabled"};
  return convert_enum_idx(options, 2, value, "csi_report_cfg_s::group_based_beam_report_c_::types");
}

const char* csi_res_cfg_s::csi_rs_res_set_list_c_::types_opts::to_string() const
{
  static const char* options[] = {"nzp-CSI-RS-SSB", "csi-IM-ResourceSetList"};
  return convert_enum_idx(options, 2, value, "csi_res_cfg_s::csi_rs_res_set_list_c_::types");
}

const char* eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneFrame", "fourFrames"};
  return convert_enum_idx(options, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types");
}
uint8_t eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 4};
  return map_enum_number(options, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types");
}

const char* eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneFrame", "fourFrames"};
  return convert_enum_idx(options, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types");
}
uint8_t eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 4};
  return map_enum_number(options, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types");
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
  j.end_obj();
}

// PollByte ::= ENUMERATED
const char* poll_byte_opts::to_string() const
{
  static const char* options[] = {
      "kB1",     "kB2",     "kB5",     "kB8",      "kB10",    "kB15",    "kB25",    "kB50",    "kB75",    "kB100",
      "kB125",   "kB250",   "kB375",   "kB500",    "kB750",   "kB1000",  "kB1250",  "kB1500",  "kB2000",  "kB3000",
      "kB4000",  "kB4500",  "kB5000",  "kB5500",   "kB6000",  "kB6500",  "kB7000",  "kB7500",  "mB8",     "mB9",
      "mB10",    "mB11",    "mB12",    "mB13",     "mB14",    "mB15",    "mB16",    "mB17",    "mB18",    "mB20",
      "mB25",    "mB30",    "mB40",    "infinity", "spare20", "spare19", "spare18", "spare17", "spare16", "spare15",
      "spare14", "spare13", "spare12", "spare11",  "spare10", "spare9",  "spare8",  "spare7",  "spare6",  "spare5",
      "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 64, value, "poll_byte_e");
}
int32_t poll_byte_opts::to_number() const
{
  static const int32_t options[] = {1,     2,     5,     8,     10,    15,    25,    50,    75,    100,   125,
                                    250,   375,   500,   750,   1000,  1250,  1500,  2000,  3000,  4000,  4500,
                                    5000,  5500,  6000,  6500,  7000,  7500,  8000,  9000,  10000, 11000, 12000,
                                    13000, 14000, 15000, 16000, 17000, 18000, 20000, 25000, 30000, 40000, -1};
  return map_enum_number(options, 44, value, "poll_byte_e");
}

// PollPDU ::= ENUMERATED
const char* poll_pdu_opts::to_string() const
{
  static const char* options[] = {"p4",     "p8",     "p16",    "p32",    "p64",    "p128",   "p256",   "p512",
                                  "p1024",  "p2048",  "p4096",  "p6144",  "p8192",  "p12288", "p16384", "p20480",
                                  "p24576", "p28672", "p32768", "p40960", "p49152", "p57344", "p65536", "infinity",
                                  "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "poll_pdu_e");
}
int32_t poll_pdu_opts::to_number() const
{
  static const int32_t options[] = {4,    8,     16,    32,    64,    128,   256,   512,   1024,  2048,  4096,  6144,
                                    8192, 12288, 16384, 20480, 24576, 28672, 32768, 40960, 49152, 57344, 65536, -1};
  return map_enum_number(options, 24, value, "poll_pdu_e");
}

// SN-FieldLengthAM ::= ENUMERATED
const char* sn_field_len_am_opts::to_string() const
{
  static const char* options[] = {"size12", "size18"};
  return convert_enum_idx(options, 2, value, "sn_field_len_am_e");
}
uint8_t sn_field_len_am_opts::to_number() const
{
  static const uint8_t options[] = {12, 18};
  return map_enum_number(options, 2, value, "sn_field_len_am_e");
}

// SN-FieldLengthUM ::= ENUMERATED
const char* sn_field_len_um_opts::to_string() const
{
  static const char* options[] = {"size6", "size12"};
  return convert_enum_idx(options, 2, value, "sn_field_len_um_e");
}
uint8_t sn_field_len_um_opts::to_number() const
{
  static const uint8_t options[] = {6, 12};
  return map_enum_number(options, 2, value, "sn_field_len_um_e");
}

const char* srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types_opts::to_string() const
{
  static const char* options[] = {"typeA", "typeB"};
  return convert_enum_idx(options, 2, value, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types");
}

// T-PollRetransmit ::= ENUMERATED
const char* t_poll_retx_opts::to_string() const
{
  static const char* options[] = {
      "ms5",   "ms10",   "ms15",   "ms20",   "ms25",   "ms30",   "ms35",   "ms40",   "ms45",  "ms50",  "ms55",
      "ms60",  "ms65",   "ms70",   "ms75",   "ms80",   "ms85",   "ms90",   "ms95",   "ms100", "ms105", "ms110",
      "ms115", "ms120",  "ms125",  "ms130",  "ms135",  "ms140",  "ms145",  "ms150",  "ms155", "ms160", "ms165",
      "ms170", "ms175",  "ms180",  "ms185",  "ms190",  "ms195",  "ms200",  "ms205",  "ms210", "ms215", "ms220",
      "ms225", "ms230",  "ms235",  "ms240",  "ms245",  "ms250",  "ms300",  "ms350",  "ms400", "ms450", "ms500",
      "ms800", "ms1000", "ms2000", "ms4000", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 64, value, "t_poll_retx_e");
}
uint16_t t_poll_retx_opts::to_number() const
{
  static const uint16_t options[] = {5,   10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,   65,   70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125, 130, 135,  140,  145, 150,
                                     155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210,  215,  220, 225,
                                     230, 235, 240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 2000, 4000};
  return map_enum_number(options, 59, value, "t_poll_retx_e");
}

// T-Reassembly ::= ENUMERATED
const char* t_reassembly_opts::to_string() const
{
  static const char* options[] = {"ms0",   "ms5",   "ms10",  "ms15",  "ms20",  "ms25",  "ms30",  "ms35",
                                  "ms40",  "ms45",  "ms50",  "ms55",  "ms60",  "ms65",  "ms70",  "ms75",
                                  "ms80",  "ms85",  "ms90",  "ms95",  "ms100", "ms110", "ms120", "ms130",
                                  "ms140", "ms150", "ms160", "ms170", "ms180", "ms190", "ms200", "spare1"};
  return convert_enum_idx(options, 32, value, "t_reassembly_e");
}
uint8_t t_reassembly_opts::to_number() const
{
  static const uint8_t options[] = {0,  5,  10, 15, 20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70, 75,
                                    80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
  return map_enum_number(options, 31, value, "t_reassembly_e");
}

// T-StatusProhibit ::= ENUMERATED
const char* t_status_prohibit_opts::to_string() const
{
  static const char* options[] = {
      "ms0",   "ms5",   "ms10",   "ms15",   "ms20",   "ms25",   "ms30",   "ms35",   "ms40",  "ms45",  "ms50",
      "ms55",  "ms60",  "ms65",   "ms70",   "ms75",   "ms80",   "ms85",   "ms90",   "ms95",  "ms100", "ms105",
      "ms110", "ms115", "ms120",  "ms125",  "ms130",  "ms135",  "ms140",  "ms145",  "ms150", "ms155", "ms160",
      "ms165", "ms170", "ms175",  "ms180",  "ms185",  "ms190",  "ms195",  "ms200",  "ms205", "ms210", "ms215",
      "ms220", "ms225", "ms230",  "ms235",  "ms240",  "ms245",  "ms250",  "ms300",  "ms350", "ms400", "ms450",
      "ms500", "ms800", "ms1000", "ms1200", "ms1600", "ms2000", "ms2400", "spare2", "spare1"};
  return convert_enum_idx(options, 64, value, "t_status_prohibit_e");
}
uint16_t t_status_prohibit_opts::to_number() const
{
  static const uint16_t options[] = {0,   5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,   65,  70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140,  145, 150, 155,
                                     160, 165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220,  225, 230, 235,
                                     240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 1200, 1600, 2000, 2400};
  return map_enum_number(options, 62, value, "t_status_prohibit_e");
}

const char* tdd_ul_dl_slot_cfg_s::symbols_c_::types_opts::to_string() const
{
  static const char* options[] = {"allDownlink", "allUplink", "explicit"};
  return convert_enum_idx(options, 3, value, "tdd_ul_dl_slot_cfg_s::symbols_c_::types");
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

const char* cross_carrier_sched_cfg_s::sched_cell_info_c_::types_opts::to_string() const
{
  static const char* options[] = {"own", "other"};
  return convert_enum_idx(options, 2, value, "cross_carrier_sched_cfg_s::sched_cell_info_c_::types");
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
  j.end_obj();
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
  static const char* options[] = {"ms1", "ms2", "ms4", "ms8", "ms16", "ms32", "ms64", "ms128"};
  return convert_enum_idx(options, 8, value, "sched_request_to_add_mod_s::sr_prohibit_timer_e_");
}
uint8_t sched_request_to_add_mod_s::sr_prohibit_timer_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(options, 8, value, "sched_request_to_add_mod_s::sr_prohibit_timer_e_");
}

const char* sched_request_to_add_mod_s::sr_trans_max_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sched_request_to_add_mod_s::sr_trans_max_e_");
}
uint8_t sched_request_to_add_mod_s::sr_trans_max_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 64};
  return map_enum_number(options, 5, value, "sched_request_to_add_mod_s::sr_trans_max_e_");
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
  j.end_obj();
}

const char* serving_cell_cfg_common_s::n_timing_advance_offset_opts::to_string() const
{
  static const char* options[] = {"n0", "n25600", "n39936"};
  return convert_enum_idx(options, 3, value, "serving_cell_cfg_common_s::n_timing_advance_offset_e_");
}
uint16_t serving_cell_cfg_common_s::n_timing_advance_offset_opts::to_number() const
{
  static const uint16_t options[] = {0, 25600, 39936};
  return map_enum_number(options, 3, value, "serving_cell_cfg_common_s::n_timing_advance_offset_e_");
}

void serving_cell_cfg_common_s::ssb_positions_in_burst_c_::destroy_()
{
  switch (type_) {
    case types::short_bitmap:
      c.destroy<fixed_bitstring<4> >();
      break;
    case types::medium_bitmap:
      c.destroy<fixed_bitstring<8> >();
      break;
    case types::long_bitmap:
      c.destroy<fixed_bitstring<64> >();
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
      c.init<fixed_bitstring<4> >();
      break;
    case types::medium_bitmap:
      c.init<fixed_bitstring<8> >();
      break;
    case types::long_bitmap:
      c.init<fixed_bitstring<64> >();
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
      c.init(other.c.get<fixed_bitstring<4> >());
      break;
    case types::medium_bitmap:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::long_bitmap:
      c.init(other.c.get<fixed_bitstring<64> >());
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
      c.set(other.c.get<fixed_bitstring<4> >());
      break;
    case types::medium_bitmap:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::long_bitmap:
      c.set(other.c.get<fixed_bitstring<64> >());
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
  return c.get<fixed_bitstring<4> >();
}
fixed_bitstring<8>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8> >();
}
fixed_bitstring<64>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64> >();
}
void serving_cell_cfg_common_s::ssb_positions_in_burst_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::short_bitmap:
      j.write_str("shortBitmap", c.get<fixed_bitstring<4> >().to_string());
      break;
    case types::medium_bitmap:
      j.write_str("mediumBitmap", c.get<fixed_bitstring<8> >().to_string());
      break;
    case types::long_bitmap:
      j.write_str("longBitmap", c.get<fixed_bitstring<64> >().to_string());
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
      HANDLE_CODE(c.get<fixed_bitstring<4> >().pack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().pack(bref));
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
      HANDLE_CODE(c.get<fixed_bitstring<4> >().unpack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types_opts::to_string() const
{
  static const char* options[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
  return convert_enum_idx(options, 3, value, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types");
}

const char* serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::to_string() const
{
  static const char* options[] = {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "serving_cell_cfg_common_s::ssb_periodicity_serving_cell_e_");
}
uint8_t serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(options, 6, value, "serving_cell_cfg_common_s::ssb_periodicity_serving_cell_e_");
}

const char* serving_cell_cfg_common_s::dmrs_type_a_position_opts::to_string() const
{
  static const char* options[] = {"pos2", "pos3"};
  return convert_enum_idx(options, 2, value, "serving_cell_cfg_common_s::dmrs_type_a_position_e_");
}
uint8_t serving_cell_cfg_common_s::dmrs_type_a_position_opts::to_number() const
{
  static const uint8_t options[] = {2, 3};
  return map_enum_number(options, 2, value, "serving_cell_cfg_common_s::dmrs_type_a_position_e_");
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
  static const char* options[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(options, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}
uint8_t ul_am_rlc_s::max_retx_thres_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(options, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
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

// BSR-Config ::= SEQUENCE
SRSASN_CODE bsr_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lc_ch_sr_delay_timer_present, 1));

  HANDLE_CODE(periodic_bsr_timer.pack(bref));
  HANDLE_CODE(retx_bsr_timer.pack(bref));
  if (lc_ch_sr_delay_timer_present) {
    HANDLE_CODE(lc_ch_sr_delay_timer.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bsr_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lc_ch_sr_delay_timer_present, 1));

  HANDLE_CODE(periodic_bsr_timer.unpack(bref));
  HANDLE_CODE(retx_bsr_timer.unpack(bref));
  if (lc_ch_sr_delay_timer_present) {
    HANDLE_CODE(lc_ch_sr_delay_timer.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bsr_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("periodicBSR-Timer", periodic_bsr_timer.to_string());
  j.write_str("retxBSR-Timer", retx_bsr_timer.to_string());
  if (lc_ch_sr_delay_timer_present) {
    j.write_str("logicalChannelSR-DelayTimer", lc_ch_sr_delay_timer.to_string());
  }
  j.end_obj();
}

const char* bsr_cfg_s::periodic_bsr_timer_opts::to_string() const
{
  static const char* options[] = {"sf1",
                                  "sf5",
                                  "sf10",
                                  "sf16",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "infinity"};
  return convert_enum_idx(options, 16, value, "bsr_cfg_s::periodic_bsr_timer_e_");
}
int16_t bsr_cfg_s::periodic_bsr_timer_opts::to_number() const
{
  static const int16_t options[] = {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return map_enum_number(options, 16, value, "bsr_cfg_s::periodic_bsr_timer_e_");
}

const char* bsr_cfg_s::retx_bsr_timer_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf40",
                                  "sf80",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "sf5120",
                                  "sf10240",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "bsr_cfg_s::retx_bsr_timer_e_");
}
uint16_t bsr_cfg_s::retx_bsr_timer_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(options, 11, value, "bsr_cfg_s::retx_bsr_timer_e_");
}

const char* bsr_cfg_s::lc_ch_sr_delay_timer_opts::to_string() const
{
  static const char* options[] = {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
  return convert_enum_idx(options, 8, value, "bsr_cfg_s::lc_ch_sr_delay_timer_e_");
}
uint16_t bsr_cfg_s::lc_ch_sr_delay_timer_opts::to_number() const
{
  static const uint16_t options[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(options, 7, value, "bsr_cfg_s::lc_ch_sr_delay_timer_e_");
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
  static const char* options[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                  "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                  "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                  "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_e_");
}
uint16_t drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t options[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(options, 24, value, "drx_cfg_s::drx_on_dur_timer_c_::milli_seconds_e_");
}

const char* drx_cfg_s::drx_on_dur_timer_c_::types_opts::to_string() const
{
  static const char* options[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(options, 2, value, "drx_cfg_s::drx_on_dur_timer_c_::types");
}

const char* drx_cfg_s::drx_inactivity_timer_opts::to_string() const
{
  static const char* options[] = {"ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",
                                  "ms10",   "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",
                                  "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare9",
                                  "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "drx_cfg_s::drx_inactivity_timer_e_");
}
uint16_t drx_cfg_s::drx_inactivity_timer_opts::to_number() const
{
  static const uint16_t options[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(options, 23, value, "drx_cfg_s::drx_inactivity_timer_e_");
}

const char* drx_cfg_s::drx_retx_timer_dl_opts::to_string() const
{
  static const char* options[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                  "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                  "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                  "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 32, value, "drx_cfg_s::drx_retx_timer_dl_e_");
}
uint16_t drx_cfg_s::drx_retx_timer_dl_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 17, value, "drx_cfg_s::drx_retx_timer_dl_e_");
}

const char* drx_cfg_s::drx_retx_timer_ul_opts::to_string() const
{
  static const char* options[] = {"sl0",    "sl1",     "sl2",     "sl4",     "sl6",     "sl8",     "sl16",    "sl24",
                                  "sl33",   "sl40",    "sl64",    "sl80",    "sl96",    "sl112",   "sl128",   "sl160",
                                  "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                  "spare8", "spare7",  "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 32, value, "drx_cfg_s::drx_retx_timer_ul_e_");
}
uint16_t drx_cfg_s::drx_retx_timer_ul_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 17, value, "drx_cfg_s::drx_retx_timer_ul_e_");
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

const char* drx_cfg_s::drx_long_cycle_start_offset_c_::types_opts::to_string() const
{
  static const char* options[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                  "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                  "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(options, 20, value, "drx_cfg_s::drx_long_cycle_start_offset_c_::types");
}
uint16_t drx_cfg_s::drx_long_cycle_start_offset_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(options, 20, value, "drx_cfg_s::drx_long_cycle_start_offset_c_::types");
}

const char* drx_cfg_s::short_drx_s_::drx_short_cycle_opts::to_string() const
{
  static const char* options[] = {"ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms7",    "ms8",    "ms10",
                                  "ms14",   "ms16",   "ms20",   "ms30",   "ms32",   "ms35",   "ms40",   "ms64",
                                  "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",  "spare9",
                                  "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "drx_cfg_s::short_drx_s_::drx_short_cycle_e_");
}
uint16_t drx_cfg_s::short_drx_s_::drx_short_cycle_opts::to_number() const
{
  static const uint16_t options[] = {2,  3,  4,  5,  6,  7,   8,   10,  14,  16,  20, 30,
                                     32, 35, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(options, 23, value, "drx_cfg_s::short_drx_s_::drx_short_cycle_e_");
}

// DataInactivityTimer ::= ENUMERATED
const char* data_inactivity_timer_opts::to_string() const
{
  static const char* options[] = {
      "s1", "s2", "s3", "s5", "s7", "s10", "s15", "s20", "s40", "s50", "s60", "s80", "s100", "s120", "s150", "s180"};
  return convert_enum_idx(options, 16, value, "data_inactivity_timer_e");
}
uint8_t data_inactivity_timer_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 5, 7, 10, 15, 20, 40, 50, 60, 80, 100, 120, 150, 180};
  return map_enum_number(options, 16, value, "data_inactivity_timer_e");
}

// LogicalChannelConfig ::= SEQUENCE
SRSASN_CODE lc_ch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_specific_params_present, 1));

  if (ul_specific_params_present) {
    HANDLE_CODE(ul_specific_params.pack(bref));
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

  return SRSASN_SUCCESS;
}
void lc_ch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_specific_params_present) {
    j.write_fieldname("ul-SpecificParameters");
    ul_specific_params.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE lc_ch_cfg_s::ul_specific_params_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(allowed_serving_cells.size() > 0, 1));
  HANDLE_CODE(bref.pack(allowed_scs_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(max_pusch_dur_present, 1));
  HANDLE_CODE(bref.pack(cfgured_grant_type1_allowed_present, 1));
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
    HANDLE_CODE(bref.pack(bit_rate_query_prohibit_timer_present, 1));

    if (bit_rate_query_prohibit_timer_present) {
      HANDLE_CODE(bit_rate_query_prohibit_timer.pack(bref));
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
  HANDLE_CODE(bref.unpack(cfgured_grant_type1_allowed_present, 1));
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
    HANDLE_CODE(bref.unpack(bit_rate_query_prohibit_timer_present, 1));

    if (bit_rate_query_prohibit_timer_present) {
      HANDLE_CODE(bit_rate_query_prohibit_timer.unpack(bref));
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
  if (cfgured_grant_type1_allowed_present) {
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
  }
  j.end_obj();
}

const char* lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_string() const
{
  static const char* options[] = {"kBps0",
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
  return convert_enum_idx(options, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}
int32_t lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_number() const
{
  static const int32_t options[] = {0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, -1};
  return map_enum_number(options, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_string() const
{
  static const char* options[] = {"ms5",
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
  return convert_enum_idx(options, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}
uint16_t lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 50, 100, 150, 300, 500, 1000};
  return map_enum_number(options, 9, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur_opts::to_string() const
{
  static const char* options[] = {"ms0p02", "ms0p04", "ms0p0625", "ms0p125", "ms0p25", "ms0p5", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_string() const
{
  static const char* options[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
}
float lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(options, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
}
const char* lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
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
  j.end_obj();
}

const char* phr_cfg_s::phr_periodic_timer_opts::to_string() const
{
  static const char* options[] = {"sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000", "infinity"};
  return convert_enum_idx(options, 8, value, "phr_cfg_s::phr_periodic_timer_e_");
}
int16_t phr_cfg_s::phr_periodic_timer_opts::to_number() const
{
  static const int16_t options[] = {10, 20, 50, 100, 200, 500, 1000, -1};
  return map_enum_number(options, 8, value, "phr_cfg_s::phr_periodic_timer_e_");
}

const char* phr_cfg_s::phr_prohibit_timer_opts::to_string() const
{
  static const char* options[] = {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
  return convert_enum_idx(options, 8, value, "phr_cfg_s::phr_prohibit_timer_e_");
}
uint16_t phr_cfg_s::phr_prohibit_timer_opts::to_number() const
{
  static const uint16_t options[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(options, 8, value, "phr_cfg_s::phr_prohibit_timer_e_");
}

const char* phr_cfg_s::phr_tx_pwr_factor_change_opts::to_string() const
{
  static const char* options[] = {"dB1", "dB3", "dB6", "infinity"};
  return convert_enum_idx(options, 4, value, "phr_cfg_s::phr_tx_pwr_factor_change_e_");
}
int8_t phr_cfg_s::phr_tx_pwr_factor_change_opts::to_number() const
{
  static const int8_t options[] = {1, 3, 6, -1};
  return map_enum_number(options, 4, value, "phr_cfg_s::phr_tx_pwr_factor_change_e_");
}

const char* phr_cfg_s::phr_mode_other_cg_opts::to_string() const
{
  static const char* options[] = {"real", "virtual"};
  return convert_enum_idx(options, 2, value, "phr_cfg_s::phr_mode_other_cg_e_");
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

const char* rlc_cfg_c::types_opts::to_string() const
{
  static const char* options[] = {"am", "um-Bi-Directional", "um-Uni-Directional-UL", "um-Uni-Directional-DL"};
  return convert_enum_idx(options, 4, value, "rlc_cfg_c::types");
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
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000", "ms4000", "ms6000"};
  return convert_enum_idx(options, 9, value, "rlf_timers_and_consts_s::t310_e_");
}
uint16_t rlf_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000, 4000, 6000};
  return map_enum_number(options, 9, value, "rlf_timers_and_consts_s::t310_e_");
}

const char* rlf_timers_and_consts_s::n310_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_s::n310_e_");
}
uint8_t rlf_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_s::n310_e_");
}

const char* rlf_timers_and_consts_s::n311_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_s::n311_e_");
}
uint8_t rlf_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_s::n311_e_");
}

const char* rlf_timers_and_consts_s::t311_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_s::t311_e_");
}
uint16_t rlf_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_s::t311_e_");
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
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc.is_present(), 1));
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->pack(bref));
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool smtc_present;
      HANDLE_CODE(bref.unpack(smtc_present, 1));
      smtc.set_present(smtc_present);
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->unpack(bref));
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
  }
  j.end_obj();
}

const char* recfg_with_sync_s::t304_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "ms10000"};
  return convert_enum_idx(options, 8, value, "recfg_with_sync_s::t304_e_");
}
uint16_t recfg_with_sync_s::t304_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return map_enum_number(options, 8, value, "recfg_with_sync_s::t304_e_");
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

const char* recfg_with_sync_s::rach_cfg_ded_c_::types_opts::to_string() const
{
  static const char* options[] = {"uplink", "supplementaryUplink"};
  return convert_enum_idx(options, 2, value, "recfg_with_sync_s::rach_cfg_ded_c_::types");
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
  HANDLE_CODE(bref.pack(skip_ul_tx_dynamic, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_mask_present;
    group_flags[0] |= data_inactivity_timer.is_present();
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
  HANDLE_CODE(bref.unpack(skip_ul_tx_dynamic, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(csi_mask_present, 1));
      bool data_inactivity_timer_present;
      HANDLE_CODE(bref.unpack(data_inactivity_timer_present, 1));
      data_inactivity_timer.set_present(data_inactivity_timer_present);
      if (csi_mask_present) {
        HANDLE_CODE(bref.unpack(csi_mask, 1));
      }
      if (data_inactivity_timer.is_present()) {
        HANDLE_CODE(data_inactivity_timer->unpack(bref));
      }
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
  j.write_bool("skipUplinkTxDynamic", skip_ul_tx_dynamic);
  if (ext) {
    if (csi_mask_present) {
      j.write_bool("csi-Mask", csi_mask);
    }
    if (data_inactivity_timer.is_present()) {
      j.write_fieldname("dataInactivityTimer");
      data_inactivity_timer->to_json(j);
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
    ext_groups_unpacker_guard group_flags(3);
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

      bool pdcch_blind_detection_present;
      HANDLE_CODE(bref.unpack(pdcch_blind_detection_present, 1));
      pdcch_blind_detection.set_present(pdcch_blind_detection_present);
      if (pdcch_blind_detection.is_present()) {
        HANDLE_CODE(pdcch_blind_detection->unpack(bref));
      }
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
  }
  j.end_obj();
}

const char* phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::to_string() const
{
  static const char* options[] = {"semiStatic", "dynamic"};
  return convert_enum_idx(options, 2, value, "phys_cell_group_cfg_s::pdsch_harq_ack_codebook_e_");
}

const char* phys_cell_group_cfg_s::xscale_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB6", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "phys_cell_group_cfg_s::xscale_e_");
}
uint8_t phys_cell_group_cfg_s::xscale_opts::to_number() const
{
  static const uint8_t options[] = {0, 6};
  return map_enum_number(options, 2, value, "phys_cell_group_cfg_s::xscale_e_");
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

const char* rlc_bearer_cfg_s::served_radio_bearer_c_::types_opts::to_string() const
{
  static const char* options[] = {"srb-Identity", "drb-Identity"};
  return convert_enum_idx(options, 2, value, "rlc_bearer_cfg_s::served_radio_bearer_c_::types");
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
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc.is_present(), 1));
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->pack(bref));
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool smtc_present;
      HANDLE_CODE(bref.unpack(smtc_present, 1));
      smtc.set_present(smtc_present);
      if (smtc.is_present()) {
        HANDLE_CODE(smtc->unpack(bref));
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
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_ul_tx_direct_current_present, 1));
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
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_ul_tx_direct_current_present, 1));
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
  }
  j.end_obj();
}

const char* feature_set_c::types_opts::to_string() const
{
  static const char* options[] = {"eutra", "nr"};
  return convert_enum_idx(options, 2, value, "feature_set_c::types");
}

const char* supported_bw_c::types_opts::to_string() const
{
  static const char* options[] = {"fr1", "fr2"};
  return convert_enum_idx(options, 2, value, "supported_bw_c::types");
}
uint8_t supported_bw_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "supported_bw_c::types");
}

const char* freq_band_info_c::types_opts::to_string() const
{
  static const char* options[] = {"bandInformationEUTRA", "bandInformationNR"};
  return convert_enum_idx(options, 2, value, "freq_band_info_c::types");
}

// MeasAndMobParametersMRDC-XDD-Diff-v1560 ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_xdd_diff_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sftd_meas_ps_cell_nedc_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_xdd_diff_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sftd_meas_ps_cell_nedc_present, 1));

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_xdd_diff_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sftd_meas_ps_cell_nedc_present) {
    j.write_str("sftd-MeasPSCell-NEDC", "supported");
  }
  j.end_obj();
}

// MeasAndMobParametersMRDC-v1560 ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    j.write_fieldname("measAndMobParametersMRDC-XDD-Diff-v1560");
    meas_and_mob_params_mrdc_xdd_diff_v1560.to_json(j);
  }
  j.end_obj();
}

// MeasResultSCG-Failure ::= SEQUENCE
SRSASN_CODE meas_result_scg_fail_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_per_mo_list, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_scg_fail_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_per_mo_list, bref, 1, 8));

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
  j.end_obj();
}

// NAICS-Capability-Entry ::= SEQUENCE
SRSASN_CODE naics_cap_entry_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, nof_naics_capable_cc, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(nof_aggregated_prb.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE naics_cap_entry_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(nof_naics_capable_cc, bref, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(nof_aggregated_prb.unpack(bref));

  return SRSASN_SUCCESS;
}
void naics_cap_entry_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("numberOfNAICS-CapableCC", nof_naics_capable_cc);
  j.write_str("numberOfAggregatedPRB", nof_aggregated_prb.to_string());
  j.end_obj();
}

const char* naics_cap_entry_s::nof_aggregated_prb_opts::to_string() const
{
  static const char* options[] = {"n50",
                                  "n75",
                                  "n100",
                                  "n125",
                                  "n150",
                                  "n175",
                                  "n200",
                                  "n225",
                                  "n250",
                                  "n275",
                                  "n300",
                                  "n350",
                                  "n400",
                                  "n450",
                                  "n500",
                                  "spare"};
  return convert_enum_idx(options, 16, value, "naics_cap_entry_s::nof_aggregated_prb_e_");
}
uint16_t naics_cap_entry_s::nof_aggregated_prb_opts::to_number() const
{
  static const uint16_t options[] = {50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500};
  return map_enum_number(options, 15, value, "naics_cap_entry_s::nof_aggregated_prb_e_");
}

// Phy-ParametersMRDC ::= SEQUENCE
SRSASN_CODE phy_params_mrdc_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(naics_cap_list.size() > 0, 1));

  if (naics_cap_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, naics_cap_list, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sp_cell_placement.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sp_cell_placement.is_present(), 1));
      if (sp_cell_placement.is_present()) {
        HANDLE_CODE(sp_cell_placement->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_mrdc_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool naics_cap_list_present;
  HANDLE_CODE(bref.unpack(naics_cap_list_present, 1));

  if (naics_cap_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(naics_cap_list, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sp_cell_placement_present;
      HANDLE_CODE(bref.unpack(sp_cell_placement_present, 1));
      sp_cell_placement.set_present(sp_cell_placement_present);
      if (sp_cell_placement.is_present()) {
        HANDLE_CODE(sp_cell_placement->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phy_params_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (naics_cap_list.size() > 0) {
    j.start_array("naics-Capability-List");
    for (const auto& e1 : naics_cap_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (sp_cell_placement.is_present()) {
      j.write_fieldname("spCellPlacement");
      sp_cell_placement->to_json(j);
    }
  }
  j.end_obj();
}

// RF-ParametersMRDC ::= SEQUENCE
SRSASN_CODE rf_params_mrdc_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(supported_band_combination_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(applied_freq_band_list_filt.size() > 0, 1));

  if (supported_band_combination_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list, 1, 65536));
  }
  if (applied_freq_band_list_filt.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, applied_freq_band_list_filt, 1, 1280));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= srs_switching_time_requested_present;
    group_flags[0] |= supported_band_combination_list_v1540.is_present();
    group_flags[1] |= supported_band_combination_list_v1550.is_present();
    group_flags[2] |= supported_band_combination_list_v1560.is_present();
    group_flags[2] |= supported_band_combination_list_nedc_only.is_present();
    group_flags[3] |= supported_band_combination_list_v1570.is_present();
    group_flags[4] |= supported_band_combination_list_v1580.is_present();
    group_flags[5] |= supported_band_combination_list_v1590.is_present();
    group_flags[6] |= supported_band_combination_list_nedc_only_v15a0.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srs_switching_time_requested_present, 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_v1540.is_present(), 1));
      if (supported_band_combination_list_v1540.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1540, 1, 65536));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1550.is_present(), 1));
      if (supported_band_combination_list_v1550.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1550, 1, 65536));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1560.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only.is_present(), 1));
      if (supported_band_combination_list_v1560.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1560, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_nedc_only, 1, 65536));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1570.is_present(), 1));
      if (supported_band_combination_list_v1570.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1570, 1, 65536));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1580.is_present(), 1));
      if (supported_band_combination_list_v1580.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1580, 1, 65536));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1590.is_present(), 1));
      if (supported_band_combination_list_v1590.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1590, 1, 65536));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v15a0.is_present(), 1));
      if (supported_band_combination_list_nedc_only_v15a0.is_present()) {
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590.size() > 0, 1));
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590, 1, 65536));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_mrdc_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool supported_band_combination_list_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_present, 1));
  bool applied_freq_band_list_filt_present;
  HANDLE_CODE(bref.unpack(applied_freq_band_list_filt_present, 1));

  if (supported_band_combination_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list, bref, 1, 65536));
  }
  if (applied_freq_band_list_filt_present) {
    HANDLE_CODE(unpack_dyn_seq_of(applied_freq_band_list_filt, bref, 1, 1280));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(srs_switching_time_requested_present, 1));
      bool supported_band_combination_list_v1540_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_v1540_present, 1));
      supported_band_combination_list_v1540.set_present(supported_band_combination_list_v1540_present);
      if (supported_band_combination_list_v1540.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1540, bref, 1, 65536));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool supported_band_combination_list_v1550_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_v1550_present, 1));
      supported_band_combination_list_v1550.set_present(supported_band_combination_list_v1550_present);
      if (supported_band_combination_list_v1550.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1550, bref, 1, 65536));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool supported_band_combination_list_v1560_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_v1560_present, 1));
      supported_band_combination_list_v1560.set_present(supported_band_combination_list_v1560_present);
      bool supported_band_combination_list_nedc_only_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_nedc_only_present, 1));
      supported_band_combination_list_nedc_only.set_present(supported_band_combination_list_nedc_only_present);
      if (supported_band_combination_list_v1560.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1560, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_nedc_only, bref, 1, 65536));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool supported_band_combination_list_v1570_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_v1570_present, 1));
      supported_band_combination_list_v1570.set_present(supported_band_combination_list_v1570_present);
      if (supported_band_combination_list_v1570.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1570, bref, 1, 65536));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool supported_band_combination_list_v1580_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_v1580_present, 1));
      supported_band_combination_list_v1580.set_present(supported_band_combination_list_v1580_present);
      if (supported_band_combination_list_v1580.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1580, bref, 1, 65536));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool supported_band_combination_list_v1590_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_v1590_present, 1));
      supported_band_combination_list_v1590.set_present(supported_band_combination_list_v1590_present);
      if (supported_band_combination_list_v1590.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1590, bref, 1, 65536));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool supported_band_combination_list_nedc_only_v15a0_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_nedc_only_v15a0_present, 1));
      supported_band_combination_list_nedc_only_v15a0.set_present(
          supported_band_combination_list_nedc_only_v15a0_present);
      if (supported_band_combination_list_nedc_only_v15a0.is_present()) {
        bool supported_band_combination_list_v1540_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1540_present, 1));
        bool supported_band_combination_list_v1560_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1560_present, 1));
        bool supported_band_combination_list_v1570_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1570_present, 1));
        bool supported_band_combination_list_v1580_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1580_present, 1));
        bool supported_band_combination_list_v1590_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1590_present, 1));
        if (supported_band_combination_list_v1540_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1560_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1570_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1580_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1590_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590, bref, 1, 65536));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rf_params_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_list.size() > 0) {
    j.start_array("supportedBandCombinationList");
    for (const auto& e1 : supported_band_combination_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (applied_freq_band_list_filt.size() > 0) {
    j.start_array("appliedFreqBandListFilter");
    for (const auto& e1 : applied_freq_band_list_filt) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (srs_switching_time_requested_present) {
      j.write_str("srs-SwitchingTimeRequested", "true");
    }
    if (supported_band_combination_list_v1540.is_present()) {
      j.start_array("supportedBandCombinationList-v1540");
      for (const auto& e1 : *supported_band_combination_list_v1540) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1550.is_present()) {
      j.start_array("supportedBandCombinationList-v1550");
      for (const auto& e1 : *supported_band_combination_list_v1550) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1560.is_present()) {
      j.start_array("supportedBandCombinationList-v1560");
      for (const auto& e1 : *supported_band_combination_list_v1560) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only.is_present()) {
      j.start_array("supportedBandCombinationListNEDC-Only");
      for (const auto& e1 : *supported_band_combination_list_nedc_only) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1570.is_present()) {
      j.start_array("supportedBandCombinationList-v1570");
      for (const auto& e1 : *supported_band_combination_list_v1570) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1580.is_present()) {
      j.start_array("supportedBandCombinationList-v1580");
      for (const auto& e1 : *supported_band_combination_list_v1580) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1590.is_present()) {
      j.start_array("supportedBandCombinationList-v1590");
      for (const auto& e1 : *supported_band_combination_list_v1590) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v15a0.is_present()) {
      j.write_fieldname("supportedBandCombinationListNEDC-Only-v15a0");
      j.start_obj();
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540.size() > 0) {
        j.start_array("supportedBandCombinationList-v1540");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560.size() > 0) {
        j.start_array("supportedBandCombinationList-v1560");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570.size() > 0) {
        j.start_array("supportedBandCombinationList-v1570");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580.size() > 0) {
        j.start_array("supportedBandCombinationList-v1580");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590.size() > 0) {
        j.start_array("supportedBandCombinationList-v1590");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
    }
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterNR-v1540 ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_nr_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_switching_time_request_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_nr_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_switching_time_request_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

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
    j.start_obj();
    j.end_obj();
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

// UE-MRDC-CapabilityAddXDD-Mode-v1560 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_add_xdd_mode_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_add_xdd_mode_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_add_xdd_mode_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    j.write_fieldname("measAndMobParametersMRDC-XDD-Diff-v1560");
    meas_and_mob_params_mrdc_xdd_diff_v1560.to_json(j);
  }
  j.end_obj();
}

// PDCP-ParametersMRDC ::= SEQUENCE
SRSASN_CODE pdcp_params_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
void pdcp_params_mrdc_s::to_json(json_writer& j) const
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

// UE-MRDC-Capability-v1560 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rx_filts.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_v1560_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rx_filts.size() > 0) {
    HANDLE_CODE(rx_filts.pack(bref));
  }
  if (meas_and_mob_params_mrdc_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1560.pack(bref));
  }
  if (fdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap_v1560.pack(bref));
  }
  if (tdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap_v1560.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_v1560_s::unpack(cbit_ref& bref)
{
  bool rx_filts_present;
  HANDLE_CODE(bref.unpack(rx_filts_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_v1560_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rx_filts_present) {
    HANDLE_CODE(rx_filts.unpack(bref));
  }
  if (meas_and_mob_params_mrdc_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1560.unpack(bref));
  }
  if (fdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap_v1560.unpack(bref));
  }
  if (tdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap_v1560.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rx_filts.size() > 0) {
    j.write_str("receivedFilters", rx_filts.to_string());
  }
  if (meas_and_mob_params_mrdc_v1560_present) {
    j.write_fieldname("measAndMobParametersMRDC-v1560");
    meas_and_mob_params_mrdc_v1560.to_json(j);
  }
  if (fdd_add_ue_mrdc_cap_v1560_present) {
    j.write_fieldname("fdd-Add-UE-MRDC-Capabilities-v1560");
    fdd_add_ue_mrdc_cap_v1560.to_json(j);
  }
  if (tdd_add_ue_mrdc_cap_v1560_present) {
    j.write_fieldname("tdd-Add-UE-MRDC-Capabilities-v1560");
    tdd_add_ue_mrdc_cap_v1560.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UE-MRDC-Capability ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_present, 1));
  HANDLE_CODE(bref.pack(phy_params_mrdc_v1530_present, 1));
  HANDLE_CODE(bref.pack(general_params_mrdc_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(fr1_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(fr2_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(feature_set_combinations.size() > 0, 1));
  HANDLE_CODE(bref.pack(pdcp_params_mrdc_v1530_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc.pack(bref));
  }
  if (phy_params_mrdc_v1530_present) {
    HANDLE_CODE(phy_params_mrdc_v1530.pack(bref));
  }
  HANDLE_CODE(rf_params_mrdc.pack(bref));
  if (general_params_mrdc_present) {
    HANDLE_CODE(general_params_mrdc.pack(bref));
  }
  if (fdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap.pack(bref));
  }
  if (tdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap.pack(bref));
  }
  if (fr1_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr1_add_ue_mrdc_cap.pack(bref));
  }
  if (fr2_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr2_add_ue_mrdc_cap.pack(bref));
  }
  if (feature_set_combinations.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                feature_set_combinations,
                                1,
                                1024,
                                SeqOfPacker<SeqOfPacker<Packer> >(1, 32, SeqOfPacker<Packer>(1, 128, Packer()))));
  }
  if (pdcp_params_mrdc_v1530_present) {
    HANDLE_CODE(pdcp_params_mrdc_v1530.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(phy_params_mrdc_v1530_present, 1));
  HANDLE_CODE(bref.unpack(general_params_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr1_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr2_add_ue_mrdc_cap_present, 1));
  bool feature_set_combinations_present;
  HANDLE_CODE(bref.unpack(feature_set_combinations_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_params_mrdc_v1530_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc.unpack(bref));
  }
  if (phy_params_mrdc_v1530_present) {
    HANDLE_CODE(phy_params_mrdc_v1530.unpack(bref));
  }
  HANDLE_CODE(rf_params_mrdc.unpack(bref));
  if (general_params_mrdc_present) {
    HANDLE_CODE(general_params_mrdc.unpack(bref));
  }
  if (fdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap.unpack(bref));
  }
  if (tdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap.unpack(bref));
  }
  if (fr1_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr1_add_ue_mrdc_cap.unpack(bref));
  }
  if (fr2_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr2_add_ue_mrdc_cap.unpack(bref));
  }
  if (feature_set_combinations_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_set_combinations,
                                  bref,
                                  1,
                                  1024,
                                  SeqOfPacker<SeqOfPacker<Packer> >(1, 32, SeqOfPacker<Packer>(1, 128, Packer()))));
  }
  if (pdcp_params_mrdc_v1530_present) {
    HANDLE_CODE(pdcp_params_mrdc_v1530.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_present) {
    j.write_fieldname("measAndMobParametersMRDC");
    meas_and_mob_params_mrdc.to_json(j);
  }
  if (phy_params_mrdc_v1530_present) {
    j.write_fieldname("phy-ParametersMRDC-v1530");
    phy_params_mrdc_v1530.to_json(j);
  }
  j.write_fieldname("rf-ParametersMRDC");
  rf_params_mrdc.to_json(j);
  if (general_params_mrdc_present) {
    j.write_fieldname("generalParametersMRDC");
    general_params_mrdc.to_json(j);
  }
  if (fdd_add_ue_mrdc_cap_present) {
    j.write_fieldname("fdd-Add-UE-MRDC-Capabilities");
    fdd_add_ue_mrdc_cap.to_json(j);
  }
  if (tdd_add_ue_mrdc_cap_present) {
    j.write_fieldname("tdd-Add-UE-MRDC-Capabilities");
    tdd_add_ue_mrdc_cap.to_json(j);
  }
  if (fr1_add_ue_mrdc_cap_present) {
    j.write_fieldname("fr1-Add-UE-MRDC-Capabilities");
    fr1_add_ue_mrdc_cap.to_json(j);
  }
  if (fr2_add_ue_mrdc_cap_present) {
    j.write_fieldname("fr2-Add-UE-MRDC-Capabilities");
    fr2_add_ue_mrdc_cap.to_json(j);
  }
  if (feature_set_combinations.size() > 0) {
    j.start_array("featureSetCombinations");
    for (const auto& e1 : feature_set_combinations) {
      j.start_array();
      for (const auto& e2 : e1) {
        j.start_array();
        for (const auto& e3 : e2) {
          e3.to_json(j);
        }
        j.end_array();
      }
      j.end_array();
    }
    j.end_array();
  }
  if (pdcp_params_mrdc_v1530_present) {
    j.write_fieldname("pdcp-ParametersMRDC-v1530");
    pdcp_params_mrdc_v1530.to_json(j);
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

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cap_request_filt_common_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cap_request_filt_common_present) {
    HANDLE_CODE(cap_request_filt_common.unpack(bref));
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
    j.start_obj();
    j.end_obj();
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
    group_flags[1] |= source_scg_cfgured_present;
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

      HANDLE_CODE(bref.pack(source_scg_cfgured_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(rrc_recfg.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
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

      HANDLE_CODE(bref.unpack(source_scg_cfgured_present, 1));
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
    if (source_scg_cfgured_present) {
      j.write_str("sourceSCG-Configured", "true");
    }
  }
  j.end_obj();
}

// BandCombinationInfo ::= SEQUENCE
SRSASN_CODE band_combination_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_combination_idx, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(pack_dyn_seq_of(bref, allowed_feature_sets_list, 1, 128, integer_packer<uint8_t>(1, 128)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_combination_idx, bref, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(unpack_dyn_seq_of(allowed_feature_sets_list, bref, 1, 128, integer_packer<uint8_t>(1, 128)));

  return SRSASN_SUCCESS;
}
void band_combination_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandCombinationIndex", band_combination_idx);
  j.start_array("allowedFeatureSetsList");
  for (const auto& e1 : allowed_feature_sets_list) {
    j.write_int(e1);
  }
  j.end_array();
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

// ConfigRestrictInfoSCG ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(allowed_bc_list_mrdc.size() > 0, 1));
  HANDLE_CODE(bref.pack(pwr_coordination_fr1_present, 1));
  HANDLE_CODE(bref.pack(serv_cell_idx_range_scg_present, 1));
  HANDLE_CODE(bref.pack(max_meas_freqs_scg_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));

  if (allowed_bc_list_mrdc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_bc_list_mrdc, 1, 65536));
  }
  if (pwr_coordination_fr1_present) {
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_nr_fr1_present, 1));
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_eutra_present, 1));
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_ue_fr1_present, 1));
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_nr_fr1, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_eutra, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_ue_fr1, (int8_t)-30, (int8_t)33));
    }
  }
  if (serv_cell_idx_range_scg_present) {
    HANDLE_CODE(pack_integer(bref, serv_cell_idx_range_scg.low_bound, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, serv_cell_idx_range_scg.up_bound, (uint8_t)0u, (uint8_t)31u));
  }
  if (max_meas_freqs_scg_present) {
    HANDLE_CODE(pack_integer(bref, max_meas_freqs_scg, (uint8_t)1u, (uint8_t)32u));
  }
  if (dummy_present) {
    HANDLE_CODE(pack_integer(bref, dummy, (uint8_t)1u, (uint8_t)62u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sel_band_entries_mn_list.is_present();
    group_flags[0] |= pdcch_blind_detection_scg_present;
    group_flags[0] |= max_num_rohc_context_sessions_sn_present;
    group_flags[1] |= max_intra_freq_meas_identities_scg_present;
    group_flags[1] |= max_inter_freq_meas_identities_scg_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sel_band_entries_mn_list.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.pack(max_num_rohc_context_sessions_sn_present, 1));
      if (sel_band_entries_mn_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref,
                                    *sel_band_entries_mn_list,
                                    1,
                                    65536,
                                    SeqOfPacker<integer_packer<uint8_t> >(1, 32, integer_packer<uint8_t>(0, 32))));
      }
      if (pdcch_blind_detection_scg_present) {
        HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_scg, (uint8_t)1u, (uint8_t)15u));
      }
      if (max_num_rohc_context_sessions_sn_present) {
        HANDLE_CODE(pack_integer(bref, max_num_rohc_context_sessions_sn, (uint16_t)0u, (uint16_t)16384u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_intra_freq_meas_identities_scg_present, 1));
      HANDLE_CODE(bref.pack(max_inter_freq_meas_identities_scg_present, 1));
      if (max_intra_freq_meas_identities_scg_present) {
        HANDLE_CODE(pack_integer(bref, max_intra_freq_meas_identities_scg, (uint8_t)1u, (uint8_t)62u));
      }
      if (max_inter_freq_meas_identities_scg_present) {
        HANDLE_CODE(pack_integer(bref, max_inter_freq_meas_identities_scg, (uint8_t)1u, (uint8_t)62u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool allowed_bc_list_mrdc_present;
  HANDLE_CODE(bref.unpack(allowed_bc_list_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(pwr_coordination_fr1_present, 1));
  HANDLE_CODE(bref.unpack(serv_cell_idx_range_scg_present, 1));
  HANDLE_CODE(bref.unpack(max_meas_freqs_scg_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  if (allowed_bc_list_mrdc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_bc_list_mrdc, bref, 1, 65536));
  }
  if (pwr_coordination_fr1_present) {
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_nr_fr1_present, 1));
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_eutra_present, 1));
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_ue_fr1_present, 1));
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_nr_fr1, bref, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_eutra, bref, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_ue_fr1, bref, (int8_t)-30, (int8_t)33));
    }
  }
  if (serv_cell_idx_range_scg_present) {
    HANDLE_CODE(unpack_integer(serv_cell_idx_range_scg.low_bound, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(serv_cell_idx_range_scg.up_bound, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (max_meas_freqs_scg_present) {
    HANDLE_CODE(unpack_integer(max_meas_freqs_scg, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (dummy_present) {
    HANDLE_CODE(unpack_integer(dummy, bref, (uint8_t)1u, (uint8_t)62u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sel_band_entries_mn_list_present;
      HANDLE_CODE(bref.unpack(sel_band_entries_mn_list_present, 1));
      sel_band_entries_mn_list.set_present(sel_band_entries_mn_list_present);
      HANDLE_CODE(bref.unpack(pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.unpack(max_num_rohc_context_sessions_sn_present, 1));
      if (sel_band_entries_mn_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sel_band_entries_mn_list,
                                      bref,
                                      1,
                                      65536,
                                      SeqOfPacker<integer_packer<uint8_t> >(1, 32, integer_packer<uint8_t>(0, 32))));
      }
      if (pdcch_blind_detection_scg_present) {
        HANDLE_CODE(unpack_integer(pdcch_blind_detection_scg, bref, (uint8_t)1u, (uint8_t)15u));
      }
      if (max_num_rohc_context_sessions_sn_present) {
        HANDLE_CODE(unpack_integer(max_num_rohc_context_sessions_sn, bref, (uint16_t)0u, (uint16_t)16384u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_intra_freq_meas_identities_scg_present, 1));
      HANDLE_CODE(bref.unpack(max_inter_freq_meas_identities_scg_present, 1));
      if (max_intra_freq_meas_identities_scg_present) {
        HANDLE_CODE(unpack_integer(max_intra_freq_meas_identities_scg, bref, (uint8_t)1u, (uint8_t)62u));
      }
      if (max_inter_freq_meas_identities_scg_present) {
        HANDLE_CODE(unpack_integer(max_inter_freq_meas_identities_scg, bref, (uint8_t)1u, (uint8_t)62u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cfg_restrict_info_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (allowed_bc_list_mrdc.size() > 0) {
    j.start_array("allowedBC-ListMRDC");
    for (const auto& e1 : allowed_bc_list_mrdc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pwr_coordination_fr1_present) {
    j.write_fieldname("powerCoordination-FR1");
    j.start_obj();
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      j.write_int("p-maxNR-FR1", pwr_coordination_fr1.p_max_nr_fr1);
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      j.write_int("p-maxEUTRA", pwr_coordination_fr1.p_max_eutra);
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      j.write_int("p-maxUE-FR1", pwr_coordination_fr1.p_max_ue_fr1);
    }
    j.end_obj();
  }
  if (serv_cell_idx_range_scg_present) {
    j.write_fieldname("servCellIndexRangeSCG");
    j.start_obj();
    j.write_int("lowBound", serv_cell_idx_range_scg.low_bound);
    j.write_int("upBound", serv_cell_idx_range_scg.up_bound);
    j.end_obj();
  }
  if (max_meas_freqs_scg_present) {
    j.write_int("maxMeasFreqsSCG", max_meas_freqs_scg);
  }
  if (dummy_present) {
    j.write_int("dummy", dummy);
  }
  if (ext) {
    if (sel_band_entries_mn_list.is_present()) {
      j.start_array("selectedBandEntriesMNList");
      for (const auto& e1 : *sel_band_entries_mn_list) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
    }
    if (pdcch_blind_detection_scg_present) {
      j.write_int("pdcch-BlindDetectionSCG", pdcch_blind_detection_scg);
    }
    if (max_num_rohc_context_sessions_sn_present) {
      j.write_int("maxNumberROHC-ContextSessionsSN", max_num_rohc_context_sessions_sn);
    }
    if (max_intra_freq_meas_identities_scg_present) {
      j.write_int("maxIntraFreqMeasIdentitiesSCG", max_intra_freq_meas_identities_scg);
    }
    if (max_inter_freq_meas_identities_scg_present) {
      j.write_int("maxInterFreqMeasIdentitiesSCG", max_inter_freq_meas_identities_scg);
    }
  }
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
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ran_notif_area_info_present;
      HANDLE_CODE(bref.unpack(ran_notif_area_info_present, 1));
      ran_notif_area_info.set_present(ran_notif_area_info_present);
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

      bool sel_band_combination_sn_present;
      HANDLE_CODE(bref.unpack(sel_band_combination_sn_present, 1));
      sel_band_combination_sn.set_present(sel_band_combination_sn_present);
      if (sel_band_combination_sn.is_present()) {
        HANDLE_CODE(sel_band_combination_sn->unpack(bref));
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
  }
  j.end_obj();
}

// VictimSystemType ::= SEQUENCE
SRSASN_CODE victim_sys_type_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gps_present, 1));
  HANDLE_CODE(bref.pack(glonass_present, 1));
  HANDLE_CODE(bref.pack(bds_present, 1));
  HANDLE_CODE(bref.pack(galileo_present, 1));
  HANDLE_CODE(bref.pack(wlan_present, 1));
  HANDLE_CODE(bref.pack(bluetooth_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE victim_sys_type_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gps_present, 1));
  HANDLE_CODE(bref.unpack(glonass_present, 1));
  HANDLE_CODE(bref.unpack(bds_present, 1));
  HANDLE_CODE(bref.unpack(galileo_present, 1));
  HANDLE_CODE(bref.unpack(wlan_present, 1));
  HANDLE_CODE(bref.unpack(bluetooth_present, 1));

  return SRSASN_SUCCESS;
}
void victim_sys_type_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gps_present) {
    j.write_str("gps", "true");
  }
  if (glonass_present) {
    j.write_str("glonass", "true");
  }
  if (bds_present) {
    j.write_str("bds", "true");
  }
  if (galileo_present) {
    j.write_str("galileo", "true");
  }
  if (wlan_present) {
    j.write_str("wlan", "true");
  }
  if (bluetooth_present) {
    j.write_str("bluetooth", "true");
  }
  j.end_obj();
}

// AffectedCarrierFreqCombInfoMRDC ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc_present, 1));

  HANDLE_CODE(victim_sys_type.pack(bref));
  HANDLE_CODE(interference_direction_mrdc.pack(bref));
  if (affected_carrier_freq_comb_mrdc_present) {
    HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0, 1));
    if (affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra,
                                  1,
                                  32,
                                  integer_packer<uint32_t>(0, 262143)));
    }
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr,
                                1,
                                32,
                                integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_mrdc_present, 1));

  HANDLE_CODE(victim_sys_type.unpack(bref));
  HANDLE_CODE(interference_direction_mrdc.unpack(bref));
  if (affected_carrier_freq_comb_mrdc_present) {
    bool affected_carrier_freq_comb_eutra_present;
    HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_eutra_present, 1));
    if (affected_carrier_freq_comb_eutra_present) {
      HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra,
                                    bref,
                                    1,
                                    32,
                                    integer_packer<uint32_t>(0, 262143)));
    }
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr,
                                  bref,
                                  1,
                                  32,
                                  integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_comb_info_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("victimSystemType");
  victim_sys_type.to_json(j);
  j.write_str("interferenceDirectionMRDC", interference_direction_mrdc.to_string());
  if (affected_carrier_freq_comb_mrdc_present) {
    j.write_fieldname("affectedCarrierFreqCombMRDC");
    j.start_obj();
    if (affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0) {
      j.start_array("affectedCarrierFreqCombEUTRA");
      for (const auto& e1 : affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra) {
        j.write_int(e1);
      }
      j.end_array();
    }
    j.start_array("affectedCarrierFreqCombNR");
    for (const auto& e1 : affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr) {
      j.write_int(e1);
    }
    j.end_array();
    j.end_obj();
  }
  j.end_obj();
}

const char* affected_carrier_freq_comb_info_mrdc_s::interference_direction_mrdc_opts::to_string() const
{
  static const char* options[] = {"eutra-nr", "nr", "other", "utra-nr-other", "nr-other", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "affected_carrier_freq_comb_info_mrdc_s::interference_direction_mrdc_e_");
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
  static const char* options[] = {"type1", "type3"};
  return convert_enum_idx(options, 2, value, "ph_ul_carrier_scg_s::ph_type1or3_e_");
}
uint8_t ph_ul_carrier_scg_s::ph_type1or3_opts::to_number() const
{
  static const uint8_t options[] = {1, 3};
  return map_enum_number(options, 2, value, "ph_ul_carrier_scg_s::ph_type1or3_e_");
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
    j.start_obj();
    j.end_obj();
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

// FR-Info ::= SEQUENCE
SRSASN_CODE fr_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(fr_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fr_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(fr_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void fr_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_str("fr-Type", fr_type.to_string());
  j.end_obj();
}

const char* fr_info_s::fr_type_opts::to_string() const
{
  static const char* options[] = {"fr1", "fr2"};
  return convert_enum_idx(options, 2, value, "fr_info_s::fr_type_e_");
}
uint8_t fr_info_s::fr_type_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "fr_info_s::fr_type_e_");
}

// NR-FreqInfo ::= SEQUENCE
SRSASN_CODE nr_freq_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_freq_present, 1));

  if (measured_freq_present) {
    HANDLE_CODE(pack_integer(bref, measured_freq, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_freq_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(measured_freq_present, 1));

  if (measured_freq_present) {
    HANDLE_CODE(unpack_integer(measured_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
void nr_freq_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_freq_present) {
    j.write_int("measuredFrequency", measured_freq);
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
    ext_groups_unpacker_guard group_flags(1);
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
  }
  j.end_obj();
}

// DRX-Info ::= SEQUENCE
SRSASN_CODE drx_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(short_drx_present, 1));

  HANDLE_CODE(drx_long_cycle_start_offset.pack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.pack(bref));
    HANDLE_CODE(pack_integer(bref, short_drx.drx_short_cycle_timer, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(short_drx_present, 1));

  HANDLE_CODE(drx_long_cycle_start_offset.unpack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.unpack(bref));
    HANDLE_CODE(unpack_integer(short_drx.drx_short_cycle_timer, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void drx_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-LongCycleStartOffset");
  drx_long_cycle_start_offset.to_json(j);
  if (short_drx_present) {
    j.write_fieldname("shortDRX");
    j.start_obj();
    j.write_str("drx-ShortCycle", short_drx.drx_short_cycle.to_string());
    j.write_int("drx-ShortCycleTimer", short_drx.drx_short_cycle_timer);
    j.end_obj();
  }
  j.end_obj();
}

void drx_info_s::drx_long_cycle_start_offset_c_::destroy_() {}
void drx_info_s::drx_long_cycle_start_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_info_s::drx_long_cycle_start_offset_c_::drx_long_cycle_start_offset_c_(
    const drx_info_s::drx_long_cycle_start_offset_c_& other)
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
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }
}
drx_info_s::drx_long_cycle_start_offset_c_&
drx_info_s::drx_long_cycle_start_offset_c_::operator=(const drx_info_s::drx_long_cycle_start_offset_c_& other)
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
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }

  return *this;
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
void drx_info_s::drx_long_cycle_start_offset_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_info_s::drx_long_cycle_start_offset_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info_s::drx_long_cycle_start_offset_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_info_s::drx_long_cycle_start_offset_c_::types_opts::to_string() const
{
  static const char* options[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                  "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                  "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(options, 20, value, "drx_info_s::drx_long_cycle_start_offset_c_::types");
}
uint16_t drx_info_s::drx_long_cycle_start_offset_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(options, 20, value, "drx_info_s::drx_long_cycle_start_offset_c_::types");
}

const char* drx_info_s::short_drx_s_::drx_short_cycle_opts::to_string() const
{
  static const char* options[] = {"ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms7",    "ms8",    "ms10",
                                  "ms14",   "ms16",   "ms20",   "ms30",   "ms32",   "ms35",   "ms40",   "ms64",
                                  "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",  "spare9",
                                  "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "drx_info_s::short_drx_s_::drx_short_cycle_e_");
}
uint16_t drx_info_s::short_drx_s_::drx_short_cycle_opts::to_number() const
{
  static const uint16_t options[] = {2,  3,  4,  5,  6,  7,   8,   10,  14,  16,  20, 30,
                                     32, 35, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(options, 23, value, "drx_info_s::short_drx_s_::drx_short_cycle_e_");
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
  static const char* options[] = {"cg-Config", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "cg_cfg_s::crit_exts_c_::c1_c_::types");
}

const char* cg_cfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "cg_cfg_s::crit_exts_c_::types");
}
uint8_t cg_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "cg_cfg_s::crit_exts_c_::types");
}

// CG-ConfigInfo-v1590-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_frequencies_mn_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (serv_frequencies_mn_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_frequencies_mn_nr, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool serv_frequencies_mn_nr_present;
  HANDLE_CODE(bref.unpack(serv_frequencies_mn_nr_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (serv_frequencies_mn_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_frequencies_mn_nr, bref, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_frequencies_mn_nr.size() > 0) {
    j.start_array("servFrequenciesMN-NR");
    for (const auto& e1 : serv_frequencies_mn_nr) {
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

// PH-UplinkCarrierMCG ::= SEQUENCE
SRSASN_CODE ph_ul_carrier_mcg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ph_type1or3.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_ul_carrier_mcg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ph_type1or3.unpack(bref));

  return SRSASN_SUCCESS;
}
void ph_ul_carrier_mcg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ph-Type1or3", ph_type1or3.to_string());
  j.end_obj();
}

const char* ph_ul_carrier_mcg_s::ph_type1or3_opts::to_string() const
{
  static const char* options[] = {"type1", "type3"};
  return convert_enum_idx(options, 2, value, "ph_ul_carrier_mcg_s::ph_type1or3_e_");
}
uint8_t ph_ul_carrier_mcg_s::ph_type1or3_opts::to_number() const
{
  static const uint8_t options[] = {1, 3};
  return map_enum_number(options, 2, value, "ph_ul_carrier_mcg_s::ph_type1or3_e_");
}

// CG-ConfigInfo-v1570-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1570_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sftd_freq_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(sftd_freq_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sftd_freq_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sftd_freq_list_nr, 1, 3, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sftd_freq_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sftd_freq_list_eutra, 1, 3, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1570_ies_s::unpack(cbit_ref& bref)
{
  bool sftd_freq_list_nr_present;
  HANDLE_CODE(bref.unpack(sftd_freq_list_nr_present, 1));
  bool sftd_freq_list_eutra_present;
  HANDLE_CODE(bref.unpack(sftd_freq_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sftd_freq_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sftd_freq_list_nr, bref, 1, 3, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sftd_freq_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sftd_freq_list_eutra, bref, 1, 3, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1570_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sftd_freq_list_nr.size() > 0) {
    j.start_array("sftdFrequencyList-NR");
    for (const auto& e1 : sftd_freq_list_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sftd_freq_list_eutra.size() > 0) {
    j.start_array("sftdFrequencyList-EUTRA");
    for (const auto& e1 : sftd_freq_list_eutra) {
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

// PH-InfoMCG ::= SEQUENCE
SRSASN_CODE ph_info_mcg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.pack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_info_mcg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.unpack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ph_info_mcg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_fieldname("ph-Uplink");
  ph_ul.to_json(j);
  if (ph_supplementary_ul_present) {
    j.write_fieldname("ph-SupplementaryUplink");
    ph_supplementary_ul.to_json(j);
  }
  j.end_obj();
}

// CG-ConfigInfo-v1560-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(candidate_cell_info_list_mn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(source_cfg_scg_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_eutra_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_mcg_present, 1));
  HANDLE_CODE(bref.pack(meas_result_report_cgi_eutra_present, 1));
  HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(fr_info_list_mcg.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_mn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_mn_eutra.pack(bref));
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.pack(bref));
  }
  if (source_cfg_scg_eutra.size() > 0) {
    HANDLE_CODE(source_cfg_scg_eutra.pack(bref));
  }
  if (scg_fail_info_eutra_present) {
    HANDLE_CODE(scg_fail_info_eutra.fail_type_eutra.pack(bref));
    HANDLE_CODE(scg_fail_info_eutra.meas_result_scg_eutra.pack(bref));
  }
  if (drx_cfg_mcg_present) {
    HANDLE_CODE(drx_cfg_mcg.pack(bref));
  }
  if (meas_result_report_cgi_eutra_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi_eutra.eutra_freq, (uint32_t)0u, (uint32_t)262143u));
    HANDLE_CODE(pack_integer(
        bref, meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(meas_result_report_cgi_eutra.cgi_info_eutra.pack(bref));
  }
  if (meas_result_cell_list_sftd_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_cell_list_sftd_eutra, 1, 3));
  }
  if (fr_info_list_mcg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, fr_info_list_mcg, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1560_ies_s::unpack(cbit_ref& bref)
{
  bool candidate_cell_info_list_mn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_mn_eutra_present, 1));
  bool candidate_cell_info_list_sn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_eutra_present, 1));
  bool source_cfg_scg_eutra_present;
  HANDLE_CODE(bref.unpack(source_cfg_scg_eutra_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_eutra_present, 1));
  HANDLE_CODE(bref.unpack(drx_cfg_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_report_cgi_eutra_present, 1));
  bool meas_result_cell_list_sftd_eutra_present;
  HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_eutra_present, 1));
  bool fr_info_list_mcg_present;
  HANDLE_CODE(bref.unpack(fr_info_list_mcg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_mn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_mn_eutra.unpack(bref));
  }
  if (candidate_cell_info_list_sn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.unpack(bref));
  }
  if (source_cfg_scg_eutra_present) {
    HANDLE_CODE(source_cfg_scg_eutra.unpack(bref));
  }
  if (scg_fail_info_eutra_present) {
    HANDLE_CODE(scg_fail_info_eutra.fail_type_eutra.unpack(bref));
    HANDLE_CODE(scg_fail_info_eutra.meas_result_scg_eutra.unpack(bref));
  }
  if (drx_cfg_mcg_present) {
    HANDLE_CODE(drx_cfg_mcg.unpack(bref));
  }
  if (meas_result_report_cgi_eutra_present) {
    HANDLE_CODE(unpack_integer(meas_result_report_cgi_eutra.eutra_freq, bref, (uint32_t)0u, (uint32_t)262143u));
    HANDLE_CODE(unpack_integer(
        meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(meas_result_report_cgi_eutra.cgi_info_eutra.unpack(bref));
  }
  if (meas_result_cell_list_sftd_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_cell_list_sftd_eutra, bref, 1, 3));
  }
  if (fr_info_list_mcg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(fr_info_list_mcg, bref, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_cell_info_list_mn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListMN-EUTRA", candidate_cell_info_list_mn_eutra.to_string());
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListSN-EUTRA", candidate_cell_info_list_sn_eutra.to_string());
  }
  if (source_cfg_scg_eutra.size() > 0) {
    j.write_str("sourceConfigSCG-EUTRA", source_cfg_scg_eutra.to_string());
  }
  if (scg_fail_info_eutra_present) {
    j.write_fieldname("scgFailureInfoEUTRA");
    j.start_obj();
    j.write_str("failureTypeEUTRA", scg_fail_info_eutra.fail_type_eutra.to_string());
    j.write_str("measResultSCG-EUTRA", scg_fail_info_eutra.meas_result_scg_eutra.to_string());
    j.end_obj();
  }
  if (drx_cfg_mcg_present) {
    j.write_fieldname("drx-ConfigMCG");
    drx_cfg_mcg.to_json(j);
  }
  if (meas_result_report_cgi_eutra_present) {
    j.write_fieldname("measResultReportCGI-EUTRA");
    j.start_obj();
    j.write_int("eutraFrequency", meas_result_report_cgi_eutra.eutra_freq);
    j.write_int("cellForWhichToReportCGI-EUTRA", meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra);
    j.write_fieldname("cgi-InfoEUTRA");
    meas_result_report_cgi_eutra.cgi_info_eutra.to_json(j);
    j.end_obj();
  }
  if (meas_result_cell_list_sftd_eutra.size() > 0) {
    j.start_array("measResultCellListSFTD-EUTRA");
    for (const auto& e1 : meas_result_cell_list_sftd_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (fr_info_list_mcg.size() > 0) {
    j.start_array("fr-InfoListMCG");
    for (const auto& e1 : fr_info_list_mcg) {
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

const char* cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_opts::to_string() const
{
  static const char* options[] = {"t313-Expiry", "randomAccessProblem", "rlc-MaxNumRetx", "scg-ChangeFailure"};
  return convert_enum_idx(options, 4, value, "cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_e_");
}
uint16_t cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_opts::to_number() const
{
  static const uint16_t options[] = {313};
  return map_enum_number(options, 1, value, "cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_e_");
}

// CG-ConfigInfo-v1540-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ph_info_mcg.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_report_cgi_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ph_info_mcg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ph_info_mcg, 1, 32));
  }
  if (meas_result_report_cgi_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi.ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi.cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)1007u));
    HANDLE_CODE(meas_result_report_cgi.cgi_info.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1540_ies_s::unpack(cbit_ref& bref)
{
  bool ph_info_mcg_present;
  HANDLE_CODE(bref.unpack(ph_info_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_report_cgi_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ph_info_mcg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ph_info_mcg, bref, 1, 32));
  }
  if (meas_result_report_cgi_present) {
    HANDLE_CODE(unpack_integer(meas_result_report_cgi.ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(
        unpack_integer(meas_result_report_cgi.cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)1007u));
    HANDLE_CODE(meas_result_report_cgi.cgi_info.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ph_info_mcg.size() > 0) {
    j.start_array("ph-InfoMCG");
    for (const auto& e1 : ph_info_mcg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_report_cgi_present) {
    j.write_fieldname("measResultReportCGI");
    j.start_obj();
    j.write_int("ssbFrequency", meas_result_report_cgi.ssb_freq);
    j.write_int("cellForWhichToReportCGI", meas_result_report_cgi.cell_for_which_to_report_cgi);
    j.write_fieldname("cgi-Info");
    meas_result_report_cgi.cgi_info.to_json(j);
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MRDC-AssistanceInfo ::= SEQUENCE
SRSASN_CODE mrdc_assist_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_comb_info_list_mrdc, 1, 128));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_assist_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_info_list_mrdc, bref, 1, 128));

  return SRSASN_SUCCESS;
}
void mrdc_assist_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("affectedCarrierFreqCombInfoListMRDC");
  for (const auto& e1 : affected_carrier_freq_comb_info_list_mrdc) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasConfigMN ::= SEQUENCE
SRSASN_CODE meas_cfg_mn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_frequencies_mn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.pack(gap_purpose_present, 1));

  if (measured_frequencies_mn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, measured_frequencies_mn, 1, 32));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.pack(bref));
  }
  if (gap_purpose_present) {
    HANDLE_CODE(gap_purpose.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_gap_cfg_fr2.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_gap_cfg_fr2.is_present(), 1));
      if (meas_gap_cfg_fr2.is_present()) {
        HANDLE_CODE(meas_gap_cfg_fr2->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_mn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool measured_frequencies_mn_present;
  HANDLE_CODE(bref.unpack(measured_frequencies_mn_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.unpack(gap_purpose_present, 1));

  if (measured_frequencies_mn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(measured_frequencies_mn, bref, 1, 32));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.unpack(bref));
  }
  if (gap_purpose_present) {
    HANDLE_CODE(gap_purpose.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_gap_cfg_fr2_present;
      HANDLE_CODE(bref.unpack(meas_gap_cfg_fr2_present, 1));
      meas_gap_cfg_fr2.set_present(meas_gap_cfg_fr2_present);
      if (meas_gap_cfg_fr2.is_present()) {
        HANDLE_CODE(meas_gap_cfg_fr2->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cfg_mn_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_frequencies_mn.size() > 0) {
    j.start_array("measuredFrequenciesMN");
    for (const auto& e1 : measured_frequencies_mn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_gap_cfg_present) {
    j.write_fieldname("measGapConfig");
    meas_gap_cfg.to_json(j);
  }
  if (gap_purpose_present) {
    j.write_str("gapPurpose", gap_purpose.to_string());
  }
  if (ext) {
    if (meas_gap_cfg_fr2.is_present()) {
      j.write_fieldname("measGapConfigFR2");
      meas_gap_cfg_fr2->to_json(j);
    }
  }
  j.end_obj();
}

const char* meas_cfg_mn_s::gap_purpose_opts::to_string() const
{
  static const char* options[] = {"perUE", "perFR1"};
  return convert_enum_idx(options, 2, value, "meas_cfg_mn_s::gap_purpose_e_");
}
uint8_t meas_cfg_mn_s::gap_purpose_opts::to_number() const
{
  if (value == per_fr1) {
    return 1;
  }
  invalid_enum_number(value, "meas_cfg_mn_s::gap_purpose_e_");
  return 0;
}

// CG-ConfigInfo-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_cap_info.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_mn.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_present, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_info_present, 1));
  HANDLE_CODE(bref.pack(drx_info_mcg_present, 1));
  HANDLE_CODE(bref.pack(meas_cfg_mn_present, 1));
  HANDLE_CODE(bref.pack(source_cfg_scg.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(mcg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(mrdc_assist_info_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_cap_info.size() > 0) {
    HANDLE_CODE(ue_cap_info.pack(bref));
  }
  if (candidate_cell_info_list_mn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_info_list_mn, 1, 8));
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn.pack(bref));
  }
  if (meas_result_cell_list_sftd_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_cell_list_sftd_nr, 1, 3));
  }
  if (scg_fail_info_present) {
    HANDLE_CODE(scg_fail_info.fail_type.pack(bref));
    HANDLE_CODE(scg_fail_info.meas_result_scg.pack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.pack(bref));
  }
  if (drx_info_mcg_present) {
    HANDLE_CODE(drx_info_mcg.pack(bref));
  }
  if (meas_cfg_mn_present) {
    HANDLE_CODE(meas_cfg_mn.pack(bref));
  }
  if (source_cfg_scg.size() > 0) {
    HANDLE_CODE(source_cfg_scg.pack(bref));
  }
  if (scg_rb_cfg.size() > 0) {
    HANDLE_CODE(scg_rb_cfg.pack(bref));
  }
  if (mcg_rb_cfg.size() > 0) {
    HANDLE_CODE(mcg_rb_cfg.pack(bref));
  }
  if (mrdc_assist_info_present) {
    HANDLE_CODE(mrdc_assist_info.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_ies_s::unpack(cbit_ref& bref)
{
  bool ue_cap_info_present;
  HANDLE_CODE(bref.unpack(ue_cap_info_present, 1));
  bool candidate_cell_info_list_mn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_mn_present, 1));
  bool candidate_cell_info_list_sn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_present, 1));
  bool meas_result_cell_list_sftd_nr_present;
  HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_nr_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_info_present, 1));
  HANDLE_CODE(bref.unpack(drx_info_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_mn_present, 1));
  bool source_cfg_scg_present;
  HANDLE_CODE(bref.unpack(source_cfg_scg_present, 1));
  bool scg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(scg_rb_cfg_present, 1));
  bool mcg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(mcg_rb_cfg_present, 1));
  HANDLE_CODE(bref.unpack(mrdc_assist_info_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_cap_info_present) {
    HANDLE_CODE(ue_cap_info.unpack(bref));
  }
  if (candidate_cell_info_list_mn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_info_list_mn, bref, 1, 8));
  }
  if (candidate_cell_info_list_sn_present) {
    HANDLE_CODE(candidate_cell_info_list_sn.unpack(bref));
  }
  if (meas_result_cell_list_sftd_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_cell_list_sftd_nr, bref, 1, 3));
  }
  if (scg_fail_info_present) {
    HANDLE_CODE(scg_fail_info.fail_type.unpack(bref));
    HANDLE_CODE(scg_fail_info.meas_result_scg.unpack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.unpack(bref));
  }
  if (drx_info_mcg_present) {
    HANDLE_CODE(drx_info_mcg.unpack(bref));
  }
  if (meas_cfg_mn_present) {
    HANDLE_CODE(meas_cfg_mn.unpack(bref));
  }
  if (source_cfg_scg_present) {
    HANDLE_CODE(source_cfg_scg.unpack(bref));
  }
  if (scg_rb_cfg_present) {
    HANDLE_CODE(scg_rb_cfg.unpack(bref));
  }
  if (mcg_rb_cfg_present) {
    HANDLE_CODE(mcg_rb_cfg.unpack(bref));
  }
  if (mrdc_assist_info_present) {
    HANDLE_CODE(mrdc_assist_info.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_cap_info.size() > 0) {
    j.write_str("ue-CapabilityInfo", ue_cap_info.to_string());
  }
  if (candidate_cell_info_list_mn.size() > 0) {
    j.start_array("candidateCellInfoListMN");
    for (const auto& e1 : candidate_cell_info_list_mn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    j.write_str("candidateCellInfoListSN", candidate_cell_info_list_sn.to_string());
  }
  if (meas_result_cell_list_sftd_nr.size() > 0) {
    j.start_array("measResultCellListSFTD-NR");
    for (const auto& e1 : meas_result_cell_list_sftd_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scg_fail_info_present) {
    j.write_fieldname("scgFailureInfo");
    j.start_obj();
    j.write_str("failureType", scg_fail_info.fail_type.to_string());
    j.write_str("measResultSCG", scg_fail_info.meas_result_scg.to_string());
    j.end_obj();
  }
  if (cfg_restrict_info_present) {
    j.write_fieldname("configRestrictInfo");
    cfg_restrict_info.to_json(j);
  }
  if (drx_info_mcg_present) {
    j.write_fieldname("drx-InfoMCG");
    drx_info_mcg.to_json(j);
  }
  if (meas_cfg_mn_present) {
    j.write_fieldname("measConfigMN");
    meas_cfg_mn.to_json(j);
  }
  if (source_cfg_scg.size() > 0) {
    j.write_str("sourceConfigSCG", source_cfg_scg.to_string());
  }
  if (scg_rb_cfg.size() > 0) {
    j.write_str("scg-RB-Config", scg_rb_cfg.to_string());
  }
  if (mcg_rb_cfg.size() > 0) {
    j.write_str("mcg-RB-Config", mcg_rb_cfg.to_string());
  }
  if (mrdc_assist_info_present) {
    j.write_fieldname("mrdc-AssistanceInfo");
    mrdc_assist_info.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_opts::to_string() const
{
  static const char* options[] = {"t310-Expiry",
                                  "randomAccessProblem",
                                  "rlc-MaxNumRetx",
                                  "synchReconfigFailure-SCG",
                                  "scg-reconfigFailure",
                                  "srb3-IntegrityFailure"};
  return convert_enum_idx(options, 6, value, "cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_e_");
}
uint16_t cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case srb3_integrity_fail:
      return 3;
    default:
      invalid_enum_number(value, "cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_e_");
  }
  return 0;
}

// CG-ConfigInfo ::= SEQUENCE
SRSASN_CODE cg_cfg_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_cfg_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void cg_cfg_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_info_s::crit_exts_c_::c1_c_& cg_cfg_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_cfg_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void cg_cfg_info_s::crit_exts_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cg_cfg_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_info_ies_s& cg_cfg_info_s::crit_exts_c_::c1_c_::set_cg_cfg_info()
{
  set(types::cg_cfg_info);
  return c;
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cg_cfg_info:
      j.write_fieldname("cg-ConfigInfo");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cg_cfg_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cg_cfg_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cg_cfg_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"cg-ConfigInfo", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "cg_cfg_info_s::crit_exts_c_::c1_c_::types");
}

const char* cg_cfg_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "cg_cfg_info_s::crit_exts_c_::types");
}
uint8_t cg_cfg_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "cg_cfg_info_s::crit_exts_c_::types");
}

void cells_triggered_list_item_c_::destroy_() {}
void cells_triggered_list_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
cells_triggered_list_item_c_::cells_triggered_list_item_c_(const cells_triggered_list_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pci:
      c.init(other.c.get<uint16_t>());
      break;
    case types::pci_eutra:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }
}
cells_triggered_list_item_c_& cells_triggered_list_item_c_::operator=(const cells_triggered_list_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pci:
      c.set(other.c.get<uint16_t>());
      break;
    case types::pci_eutra:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }

  return *this;
}
uint16_t& cells_triggered_list_item_c_::set_pci()
{
  set(types::pci);
  return c.get<uint16_t>();
}
uint16_t& cells_triggered_list_item_c_::set_pci_eutra()
{
  set(types::pci_eutra);
  return c.get<uint16_t>();
}
void cells_triggered_list_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pci:
      j.write_int("physCellId", c.get<uint16_t>());
      break;
    case types::pci_eutra:
      j.write_int("physCellIdEUTRA", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE cells_triggered_list_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pci:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1007u));
      break;
    case types::pci_eutra:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)503u));
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_triggered_list_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pci:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1007u));
      break;
    case types::pci_eutra:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)503u));
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cells_triggered_list_item_c_::types_opts::to_string() const
{
  static const char* options[] = {"physCellId", "physCellIdEUTRA"};
  return convert_enum_idx(options, 2, value, "cells_triggered_list_item_c_::types");
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
  static const char* options[] = {"handoverCommand", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ho_cmd_s::crit_exts_c_::c1_c_::types");
}

const char* ho_cmd_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ho_cmd_s::crit_exts_c_::types");
}
uint8_t ho_cmd_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ho_cmd_s::crit_exts_c_::types");
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

      bool candidate_cell_info_list_sn_eutra_present;
      HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_eutra_present, 1));
      candidate_cell_info_list_sn_eutra.set_present(candidate_cell_info_list_sn_eutra_present);
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
  static const char* options[] = {
      "s1",    "s2",       "s3",    "s5",           "s7",      "s10",   "s15",     "s20",      "s25",     "s30",
      "s40",   "s50",      "min1",  "min1s20",      "min1s40", "min2",  "min2s30", "min3",     "min3s30", "min4",
      "min5",  "min6",     "min7",  "min8",         "min9",    "min10", "min12",   "min14",    "min17",   "min20",
      "min24", "min28",    "min33", "min38",        "min44",   "min50", "hr1",     "hr1min30", "hr2",     "hr2min30",
      "hr3",   "hr3min30", "hr4",   "hr5",          "hr6",     "hr8",   "hr10",    "hr13",     "hr16",    "hr20",
      "day1",  "day1hr12", "day2",  "day2hr12",     "day3",    "day4",  "day5",    "day7",     "day10",   "day14",
      "day19", "day24",    "day30", "dayMoreThan30"};
  return convert_enum_idx(options, 64, value, "rrm_cfg_s::ue_inactive_time_e_");
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
  static const char* options[] = {"handoverPreparationInformation", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ho_prep_info_s::crit_exts_c_::c1_c_::types");
}

const char* ho_prep_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ho_prep_info_s::crit_exts_c_::types");
}
uint8_t ho_prep_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ho_prep_info_s::crit_exts_c_::types");
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

      bool ssb_to_measure_present;
      HANDLE_CODE(bref.unpack(ssb_to_measure_present, 1));
      ssb_to_measure.set_present(ssb_to_measure_present);
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

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_v1550_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(camp_on_first_ssb, 1));
  HANDLE_CODE(bref.unpack(ps_cell_only_on_first_ssb, 1));

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_v1550_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("campOnFirstSSB", camp_on_first_ssb);
  j.write_bool("psCellOnlyOnFirstSSB", ps_cell_only_on_first_ssb);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
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
  static const char* options[] = {"measTimingConf", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "meas_timing_cfg_s::crit_exts_c_::c1_c_::types");
}

const char* meas_timing_cfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "meas_timing_cfg_s::crit_exts_c_::types");
}
uint8_t meas_timing_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "meas_timing_cfg_s::crit_exts_c_::types");
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
  static const char* options[] = {
      "ueRadioAccessCapabilityInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_access_cap_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ue_radio_access_cap_info_s::crit_exts_c_::types");
}
uint8_t ue_radio_access_cap_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ue_radio_access_cap_info_s::crit_exts_c_::types");
}

// UERadioPagingInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_nr_for_paging.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (supported_band_list_nr_for_paging.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_nr_for_paging, 1, 1024, integer_packer<uint16_t>(1, 1024)));
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
    j.start_obj();
    j.end_obj();
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
  static const char* options[] = {
      "ueRadioPagingInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_radio_paging_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_paging_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "ue_radio_paging_info_s::crit_exts_c_::types");
}
uint8_t ue_radio_paging_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ue_radio_paging_info_s::crit_exts_c_::types");
}

// VarMeasConfig ::= SEQUENCE
SRSASN_CODE var_meas_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_id_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_obj_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(report_cfg_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(quant_cfg_present, 1));
  HANDLE_CODE(bref.pack(s_measure_cfg_present, 1));

  if (meas_id_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_id_list, 1, 64));
  }
  if (meas_obj_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_obj_list, 1, 64));
  }
  if (report_cfg_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, report_cfg_list, 1, 64));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.pack(bref));
  }
  if (s_measure_cfg_present) {
    HANDLE_CODE(s_measure_cfg.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_cfg_s::unpack(cbit_ref& bref)
{
  bool meas_id_list_present;
  HANDLE_CODE(bref.unpack(meas_id_list_present, 1));
  bool meas_obj_list_present;
  HANDLE_CODE(bref.unpack(meas_obj_list_present, 1));
  bool report_cfg_list_present;
  HANDLE_CODE(bref.unpack(report_cfg_list_present, 1));
  HANDLE_CODE(bref.unpack(quant_cfg_present, 1));
  HANDLE_CODE(bref.unpack(s_measure_cfg_present, 1));

  if (meas_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_id_list, bref, 1, 64));
  }
  if (meas_obj_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_obj_list, bref, 1, 64));
  }
  if (report_cfg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(report_cfg_list, bref, 1, 64));
  }
  if (quant_cfg_present) {
    HANDLE_CODE(quant_cfg.unpack(bref));
  }
  if (s_measure_cfg_present) {
    HANDLE_CODE(s_measure_cfg.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void var_meas_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_id_list.size() > 0) {
    j.start_array("measIdList");
    for (const auto& e1 : meas_id_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_obj_list.size() > 0) {
    j.start_array("measObjectList");
    for (const auto& e1 : meas_obj_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (report_cfg_list.size() > 0) {
    j.start_array("reportConfigList");
    for (const auto& e1 : report_cfg_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (quant_cfg_present) {
    j.write_fieldname("quantityConfig");
    quant_cfg.to_json(j);
  }
  if (s_measure_cfg_present) {
    j.write_fieldname("s-MeasureConfig");
    s_measure_cfg.to_json(j);
  }
  j.end_obj();
}

void var_meas_cfg_s::s_measure_cfg_c_::destroy_() {}
void var_meas_cfg_s::s_measure_cfg_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
var_meas_cfg_s::s_measure_cfg_c_::s_measure_cfg_c_(const var_meas_cfg_s::s_measure_cfg_c_& other)
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
  }
}
var_meas_cfg_s::s_measure_cfg_c_&
var_meas_cfg_s::s_measure_cfg_c_::operator=(const var_meas_cfg_s::s_measure_cfg_c_& other)
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
  }

  return *this;
}
uint8_t& var_meas_cfg_s::s_measure_cfg_c_::set_ssb_rsrp()
{
  set(types::ssb_rsrp);
  return c.get<uint8_t>();
}
uint8_t& var_meas_cfg_s::s_measure_cfg_c_::set_csi_rsrp()
{
  set(types::csi_rsrp);
  return c.get<uint8_t>();
}
void var_meas_cfg_s::s_measure_cfg_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE var_meas_cfg_s::s_measure_cfg_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_cfg_s::s_measure_cfg_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "var_meas_cfg_s::s_measure_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* var_meas_cfg_s::s_measure_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"ssb-RSRP", "csi-RSRP"};
  return convert_enum_idx(options, 2, value, "var_meas_cfg_s::s_measure_cfg_c_::types");
}

// VarMeasReport ::= SEQUENCE
SRSASN_CODE var_meas_report_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cells_triggered_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)64u));
  if (cells_triggered_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_triggered_list, 1, 32));
  }
  HANDLE_CODE(pack_unconstrained_integer(bref, nof_reports_sent));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_report_s::unpack(cbit_ref& bref)
{
  bool cells_triggered_list_present;
  HANDLE_CODE(bref.unpack(cells_triggered_list_present, 1));

  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)64u));
  if (cells_triggered_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_triggered_list, bref, 1, 32));
  }
  HANDLE_CODE(unpack_unconstrained_integer(nof_reports_sent, bref));

  return SRSASN_SUCCESS;
}
void var_meas_report_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  if (cells_triggered_list.size() > 0) {
    j.start_array("cellsTriggeredList");
    for (const auto& e1 : cells_triggered_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("numberOfReportsSent", nof_reports_sent);
  j.end_obj();
}

// VarResumeMAC-Input ::= SEQUENCE
SRSASN_CODE var_resume_mac_input_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, source_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, source_c_rnti, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_resume_mac_input_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(source_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.unpack(bref));
  HANDLE_CODE(unpack_integer(source_c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
void var_resume_mac_input_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId", source_pci);
  j.write_str("targetCellIdentity", target_cell_id.to_string());
  j.write_int("source-c-RNTI", source_c_rnti);
  j.end_obj();
}

// VarShortMAC-Input ::= SEQUENCE
SRSASN_CODE var_short_mac_input_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, source_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, source_c_rnti, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_short_mac_input_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(source_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_id.unpack(bref));
  HANDLE_CODE(unpack_integer(source_c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));

  return SRSASN_SUCCESS;
}
void var_short_mac_input_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId", source_pci);
  j.write_str("targetCellIdentity", target_cell_id.to_string());
  j.write_int("source-c-RNTI", source_c_rnti);
  j.end_obj();
}

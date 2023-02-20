/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/asn1/rrc_nr/ue_cap.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MIMO-LayersDL ::= ENUMERATED
const char* mimo_layers_dl_opts::to_string() const
{
  static const char* names[] = {"twoLayers", "fourLayers", "eightLayers"};
  return convert_enum_idx(names, 3, value, "mimo_layers_dl_e");
}
uint8_t mimo_layers_dl_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "mimo_layers_dl_e");
}

// MIMO-LayersUL ::= ENUMERATED
const char* mimo_layers_ul_opts::to_string() const
{
  static const char* names[] = {"oneLayer", "twoLayers", "fourLayers"};
  return convert_enum_idx(names, 3, value, "mimo_layers_ul_e");
}
uint8_t mimo_layers_ul_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "mimo_layers_ul_e");
}

// CA-BandwidthClassEUTRA ::= ENUMERATED
const char* ca_bw_class_eutra_opts::to_string() const
{
  static const char* names[] = {"a", "b", "c", "d", "e", "f"};
  return convert_enum_idx(names, 6, value, "ca_bw_class_eutra_e");
}

// CA-BandwidthClassNR ::= ENUMERATED
const char* ca_bw_class_nr_opts::to_string() const
{
  static const char* names[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q"};
  return convert_enum_idx(names, 17, value, "ca_bw_class_nr_e");
}

// BandParameters ::= CHOICE
void band_params_c::destroy_()
{
  switch (type_) {
    case types::eutra:
      c.destroy<eutra_s_>();
      break;
    case types::nr:
      c.destroy<nr_s_>();
      break;
    default:
      break;
  }
}
void band_params_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eutra:
      c.init<eutra_s_>();
      break;
    case types::nr:
      c.init<nr_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_params_c");
  }
}
band_params_c::band_params_c(const band_params_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eutra:
      c.init(other.c.get<eutra_s_>());
      break;
    case types::nr:
      c.init(other.c.get<nr_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_params_c");
  }
}
band_params_c& band_params_c::operator=(const band_params_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eutra:
      c.set(other.c.get<eutra_s_>());
      break;
    case types::nr:
      c.set(other.c.get<nr_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_params_c");
  }

  return *this;
}
band_params_c::eutra_s_& band_params_c::set_eutra()
{
  set(types::eutra);
  return c.get<eutra_s_>();
}
band_params_c::nr_s_& band_params_c::set_nr()
{
  set(types::nr);
  return c.get<nr_s_>();
}
void band_params_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eutra:
      j.write_fieldname("eutra");
      j.start_obj();
      j.write_int("bandEUTRA", c.get<eutra_s_>().band_eutra);
      if (c.get<eutra_s_>().ca_bw_class_dl_eutra_present) {
        j.write_str("ca-BandwidthClassDL-EUTRA", c.get<eutra_s_>().ca_bw_class_dl_eutra.to_string());
      }
      if (c.get<eutra_s_>().ca_bw_class_ul_eutra_present) {
        j.write_str("ca-BandwidthClassUL-EUTRA", c.get<eutra_s_>().ca_bw_class_ul_eutra.to_string());
      }
      j.end_obj();
      break;
    case types::nr:
      j.write_fieldname("nr");
      j.start_obj();
      j.write_int("bandNR", c.get<nr_s_>().band_nr);
      if (c.get<nr_s_>().ca_bw_class_dl_nr_present) {
        j.write_str("ca-BandwidthClassDL-NR", c.get<nr_s_>().ca_bw_class_dl_nr.to_string());
      }
      if (c.get<nr_s_>().ca_bw_class_ul_nr_present) {
        j.write_str("ca-BandwidthClassUL-NR", c.get<nr_s_>().ca_bw_class_ul_nr.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "band_params_c");
  }
  j.end_obj();
}
SRSASN_CODE band_params_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eutra:
      HANDLE_CODE(bref.pack(c.get<eutra_s_>().ca_bw_class_dl_eutra_present, 1));
      HANDLE_CODE(bref.pack(c.get<eutra_s_>().ca_bw_class_ul_eutra_present, 1));
      HANDLE_CODE(pack_integer(bref, c.get<eutra_s_>().band_eutra, (uint16_t)1u, (uint16_t)256u));
      if (c.get<eutra_s_>().ca_bw_class_dl_eutra_present) {
        HANDLE_CODE(c.get<eutra_s_>().ca_bw_class_dl_eutra.pack(bref));
      }
      if (c.get<eutra_s_>().ca_bw_class_ul_eutra_present) {
        HANDLE_CODE(c.get<eutra_s_>().ca_bw_class_ul_eutra.pack(bref));
      }
      break;
    case types::nr:
      HANDLE_CODE(bref.pack(c.get<nr_s_>().ca_bw_class_dl_nr_present, 1));
      HANDLE_CODE(bref.pack(c.get<nr_s_>().ca_bw_class_ul_nr_present, 1));
      HANDLE_CODE(pack_integer(bref, c.get<nr_s_>().band_nr, (uint16_t)1u, (uint16_t)1024u));
      if (c.get<nr_s_>().ca_bw_class_dl_nr_present) {
        HANDLE_CODE(c.get<nr_s_>().ca_bw_class_dl_nr.pack(bref));
      }
      if (c.get<nr_s_>().ca_bw_class_ul_nr_present) {
        HANDLE_CODE(c.get<nr_s_>().ca_bw_class_ul_nr.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_params_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eutra:
      HANDLE_CODE(bref.unpack(c.get<eutra_s_>().ca_bw_class_dl_eutra_present, 1));
      HANDLE_CODE(bref.unpack(c.get<eutra_s_>().ca_bw_class_ul_eutra_present, 1));
      HANDLE_CODE(unpack_integer(c.get<eutra_s_>().band_eutra, bref, (uint16_t)1u, (uint16_t)256u));
      if (c.get<eutra_s_>().ca_bw_class_dl_eutra_present) {
        HANDLE_CODE(c.get<eutra_s_>().ca_bw_class_dl_eutra.unpack(bref));
      }
      if (c.get<eutra_s_>().ca_bw_class_ul_eutra_present) {
        HANDLE_CODE(c.get<eutra_s_>().ca_bw_class_ul_eutra.unpack(bref));
      }
      break;
    case types::nr:
      HANDLE_CODE(bref.unpack(c.get<nr_s_>().ca_bw_class_dl_nr_present, 1));
      HANDLE_CODE(bref.unpack(c.get<nr_s_>().ca_bw_class_ul_nr_present, 1));
      HANDLE_CODE(unpack_integer(c.get<nr_s_>().band_nr, bref, (uint16_t)1u, (uint16_t)1024u));
      if (c.get<nr_s_>().ca_bw_class_dl_nr_present) {
        HANDLE_CODE(c.get<nr_s_>().ca_bw_class_dl_nr.unpack(bref));
      }
      if (c.get<nr_s_>().ca_bw_class_ul_nr_present) {
        HANDLE_CODE(c.get<nr_s_>().ca_bw_class_ul_nr.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_params_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CA-ParametersEUTRA ::= SEQUENCE
SRSASN_CODE ca_params_eutra_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(multiple_timing_advance_present, 1));
  HANDLE_CODE(bref.pack(simul_rx_tx_present, 1));
  HANDLE_CODE(bref.pack(supported_naics_2_crs_ap_present, 1));
  HANDLE_CODE(bref.pack(add_rx_tx_performance_req_present, 1));
  HANDLE_CODE(bref.pack(ue_ca_pwr_class_n_present, 1));
  HANDLE_CODE(bref.pack(supported_bw_combination_set_eutra_v1530_present, 1));

  if (supported_naics_2_crs_ap_present) {
    HANDLE_CODE(supported_naics_2_crs_ap.pack(bref));
  }
  if (supported_bw_combination_set_eutra_v1530_present) {
    HANDLE_CODE(supported_bw_combination_set_eutra_v1530.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_eutra_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(multiple_timing_advance_present, 1));
  HANDLE_CODE(bref.unpack(simul_rx_tx_present, 1));
  HANDLE_CODE(bref.unpack(supported_naics_2_crs_ap_present, 1));
  HANDLE_CODE(bref.unpack(add_rx_tx_performance_req_present, 1));
  HANDLE_CODE(bref.unpack(ue_ca_pwr_class_n_present, 1));
  HANDLE_CODE(bref.unpack(supported_bw_combination_set_eutra_v1530_present, 1));

  if (supported_naics_2_crs_ap_present) {
    HANDLE_CODE(supported_naics_2_crs_ap.unpack(bref));
  }
  if (supported_bw_combination_set_eutra_v1530_present) {
    HANDLE_CODE(supported_bw_combination_set_eutra_v1530.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ca_params_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multiple_timing_advance_present) {
    j.write_str("multipleTimingAdvance", "supported");
  }
  if (simul_rx_tx_present) {
    j.write_str("simultaneousRx-Tx", "supported");
  }
  if (supported_naics_2_crs_ap_present) {
    j.write_str("supportedNAICS-2CRS-AP", supported_naics_2_crs_ap.to_string());
  }
  if (add_rx_tx_performance_req_present) {
    j.write_str("additionalRx-Tx-PerformanceReq", "supported");
  }
  if (ue_ca_pwr_class_n_present) {
    j.write_str("ue-CA-PowerClass-N", "class2");
  }
  if (supported_bw_combination_set_eutra_v1530_present) {
    j.write_str("supportedBandwidthCombinationSetEUTRA-v1530", supported_bw_combination_set_eutra_v1530.to_string());
  }
  j.end_obj();
}

// CA-ParametersNR ::= SEQUENCE
SRSASN_CODE ca_params_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(parallel_tx_srs_pucch_pusch_present, 1));
  HANDLE_CODE(bref.pack(parallel_tx_prach_srs_pucch_pusch_present, 1));
  HANDLE_CODE(bref.pack(simul_rx_tx_inter_band_ca_present, 1));
  HANDLE_CODE(bref.pack(simul_rx_tx_sul_present, 1));
  HANDLE_CODE(bref.pack(diff_numerology_across_pucch_group_present, 1));
  HANDLE_CODE(bref.pack(diff_numerology_within_pucch_group_smaller_scs_present, 1));
  HANDLE_CODE(bref.pack(supported_num_tag_present, 1));

  if (supported_num_tag_present) {
    HANDLE_CODE(supported_num_tag.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(parallel_tx_srs_pucch_pusch_present, 1));
  HANDLE_CODE(bref.unpack(parallel_tx_prach_srs_pucch_pusch_present, 1));
  HANDLE_CODE(bref.unpack(simul_rx_tx_inter_band_ca_present, 1));
  HANDLE_CODE(bref.unpack(simul_rx_tx_sul_present, 1));
  HANDLE_CODE(bref.unpack(diff_numerology_across_pucch_group_present, 1));
  HANDLE_CODE(bref.unpack(diff_numerology_within_pucch_group_smaller_scs_present, 1));
  HANDLE_CODE(bref.unpack(supported_num_tag_present, 1));

  if (supported_num_tag_present) {
    HANDLE_CODE(supported_num_tag.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ca_params_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dummy_present) {
    j.write_str("dummy", "supported");
  }
  if (parallel_tx_srs_pucch_pusch_present) {
    j.write_str("parallelTxSRS-PUCCH-PUSCH", "supported");
  }
  if (parallel_tx_prach_srs_pucch_pusch_present) {
    j.write_str("parallelTxPRACH-SRS-PUCCH-PUSCH", "supported");
  }
  if (simul_rx_tx_inter_band_ca_present) {
    j.write_str("simultaneousRxTxInterBandCA", "supported");
  }
  if (simul_rx_tx_sul_present) {
    j.write_str("simultaneousRxTxSUL", "supported");
  }
  if (diff_numerology_across_pucch_group_present) {
    j.write_str("diffNumerologyAcrossPUCCH-Group", "supported");
  }
  if (diff_numerology_within_pucch_group_smaller_scs_present) {
    j.write_str("diffNumerologyWithinPUCCH-GroupSmallerSCS", "supported");
  }
  if (supported_num_tag_present) {
    j.write_str("supportedNumberTAG", supported_num_tag.to_string());
  }
  j.end_obj();
}

const char* ca_params_nr_s::supported_num_tag_opts::to_string() const
{
  static const char* names[] = {"n2", "n3", "n4"};
  return convert_enum_idx(names, 3, value, "ca_params_nr_s::supported_num_tag_e_");
}
uint8_t ca_params_nr_s::supported_num_tag_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4};
  return map_enum_number(numbers, 3, value, "ca_params_nr_s::supported_num_tag_e_");
}

// MRDC-Parameters ::= SEQUENCE
SRSASN_CODE mrdc_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(single_ul_tx_present, 1));
  HANDLE_CODE(bref.pack(dyn_pwr_sharing_endc_present, 1));
  HANDLE_CODE(bref.pack(tdm_pattern_present, 1));
  HANDLE_CODE(bref.pack(ul_sharing_eutra_nr_present, 1));
  HANDLE_CODE(bref.pack(ul_switching_time_eutra_nr_present, 1));
  HANDLE_CODE(bref.pack(simul_rx_tx_inter_band_endc_present, 1));
  HANDLE_CODE(bref.pack(async_intra_band_endc_present, 1));

  if (ul_sharing_eutra_nr_present) {
    HANDLE_CODE(ul_sharing_eutra_nr.pack(bref));
  }
  if (ul_switching_time_eutra_nr_present) {
    HANDLE_CODE(ul_switching_time_eutra_nr.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dual_pa_architecture_present;
    group_flags[0] |= intra_band_endc_support_present;
    group_flags[0] |= ul_timing_align_eutra_nr_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dual_pa_architecture_present, 1));
      HANDLE_CODE(bref.pack(intra_band_endc_support_present, 1));
      HANDLE_CODE(bref.pack(ul_timing_align_eutra_nr_present, 1));
      if (intra_band_endc_support_present) {
        HANDLE_CODE(intra_band_endc_support.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(single_ul_tx_present, 1));
  HANDLE_CODE(bref.unpack(dyn_pwr_sharing_endc_present, 1));
  HANDLE_CODE(bref.unpack(tdm_pattern_present, 1));
  HANDLE_CODE(bref.unpack(ul_sharing_eutra_nr_present, 1));
  HANDLE_CODE(bref.unpack(ul_switching_time_eutra_nr_present, 1));
  HANDLE_CODE(bref.unpack(simul_rx_tx_inter_band_endc_present, 1));
  HANDLE_CODE(bref.unpack(async_intra_band_endc_present, 1));

  if (ul_sharing_eutra_nr_present) {
    HANDLE_CODE(ul_sharing_eutra_nr.unpack(bref));
  }
  if (ul_switching_time_eutra_nr_present) {
    HANDLE_CODE(ul_switching_time_eutra_nr.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dual_pa_architecture_present, 1));
      HANDLE_CODE(bref.unpack(intra_band_endc_support_present, 1));
      HANDLE_CODE(bref.unpack(ul_timing_align_eutra_nr_present, 1));
      if (intra_band_endc_support_present) {
        HANDLE_CODE(intra_band_endc_support.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mrdc_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (single_ul_tx_present) {
    j.write_str("singleUL-Transmission", "supported");
  }
  if (dyn_pwr_sharing_endc_present) {
    j.write_str("dynamicPowerSharingENDC", "supported");
  }
  if (tdm_pattern_present) {
    j.write_str("tdm-Pattern", "supported");
  }
  if (ul_sharing_eutra_nr_present) {
    j.write_str("ul-SharingEUTRA-NR", ul_sharing_eutra_nr.to_string());
  }
  if (ul_switching_time_eutra_nr_present) {
    j.write_str("ul-SwitchingTimeEUTRA-NR", ul_switching_time_eutra_nr.to_string());
  }
  if (simul_rx_tx_inter_band_endc_present) {
    j.write_str("simultaneousRxTxInterBandENDC", "supported");
  }
  if (async_intra_band_endc_present) {
    j.write_str("asyncIntraBandENDC", "supported");
  }
  if (ext) {
    if (dual_pa_architecture_present) {
      j.write_str("dualPA-Architecture", "supported");
    }
    if (intra_band_endc_support_present) {
      j.write_str("intraBandENDC-Support", intra_band_endc_support.to_string());
    }
    if (ul_timing_align_eutra_nr_present) {
      j.write_str("ul-TimingAlignmentEUTRA-NR", "required");
    }
  }
  j.end_obj();
}

const char* mrdc_params_s::ul_sharing_eutra_nr_opts::to_string() const
{
  static const char* names[] = {"tdm", "fdm", "both"};
  return convert_enum_idx(names, 3, value, "mrdc_params_s::ul_sharing_eutra_nr_e_");
}

const char* mrdc_params_s::ul_switching_time_eutra_nr_opts::to_string() const
{
  static const char* names[] = {"type1", "type2"};
  return convert_enum_idx(names, 2, value, "mrdc_params_s::ul_switching_time_eutra_nr_e_");
}
uint8_t mrdc_params_s::ul_switching_time_eutra_nr_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "mrdc_params_s::ul_switching_time_eutra_nr_e_");
}

const char* mrdc_params_s::intra_band_endc_support_opts::to_string() const
{
  static const char* names[] = {"non-contiguous", "both"};
  return convert_enum_idx(names, 2, value, "mrdc_params_s::intra_band_endc_support_e_");
}

// BandCombination ::= SEQUENCE
SRSASN_CODE band_combination_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_params_eutra_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nr_present, 1));
  HANDLE_CODE(bref.pack(mrdc_params_present, 1));
  HANDLE_CODE(bref.pack(supported_bw_combination_set_present, 1));
  HANDLE_CODE(bref.pack(pwr_class_v1530_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, band_list, 1, 32));
  HANDLE_CODE(pack_integer(bref, feature_set_combination, (uint16_t)0u, (uint16_t)1024u));
  if (ca_params_eutra_present) {
    HANDLE_CODE(ca_params_eutra.pack(bref));
  }
  if (ca_params_nr_present) {
    HANDLE_CODE(ca_params_nr.pack(bref));
  }
  if (mrdc_params_present) {
    HANDLE_CODE(mrdc_params.pack(bref));
  }
  if (supported_bw_combination_set_present) {
    HANDLE_CODE(supported_bw_combination_set.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_params_eutra_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nr_present, 1));
  HANDLE_CODE(bref.unpack(mrdc_params_present, 1));
  HANDLE_CODE(bref.unpack(supported_bw_combination_set_present, 1));
  HANDLE_CODE(bref.unpack(pwr_class_v1530_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(band_list, bref, 1, 32));
  HANDLE_CODE(unpack_integer(feature_set_combination, bref, (uint16_t)0u, (uint16_t)1024u));
  if (ca_params_eutra_present) {
    HANDLE_CODE(ca_params_eutra.unpack(bref));
  }
  if (ca_params_nr_present) {
    HANDLE_CODE(ca_params_nr.unpack(bref));
  }
  if (mrdc_params_present) {
    HANDLE_CODE(mrdc_params.unpack(bref));
  }
  if (supported_bw_combination_set_present) {
    HANDLE_CODE(supported_bw_combination_set.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandList");
  for (const auto& e1 : band_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("featureSetCombination", feature_set_combination);
  if (ca_params_eutra_present) {
    j.write_fieldname("ca-ParametersEUTRA");
    ca_params_eutra.to_json(j);
  }
  if (ca_params_nr_present) {
    j.write_fieldname("ca-ParametersNR");
    ca_params_nr.to_json(j);
  }
  if (mrdc_params_present) {
    j.write_fieldname("mrdc-Parameters");
    mrdc_params.to_json(j);
  }
  if (supported_bw_combination_set_present) {
    j.write_str("supportedBandwidthCombinationSet", supported_bw_combination_set.to_string());
  }
  if (pwr_class_v1530_present) {
    j.write_str("powerClass-v1530", "pc2");
  }
  j.end_obj();
}

// SRS-SwitchingTimeEUTRA ::= SEQUENCE
SRSASN_CODE srs_switching_time_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(switching_time_dl_present, 1));
  HANDLE_CODE(bref.pack(switching_time_ul_present, 1));

  if (switching_time_dl_present) {
    HANDLE_CODE(switching_time_dl.pack(bref));
  }
  if (switching_time_ul_present) {
    HANDLE_CODE(switching_time_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_switching_time_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(switching_time_dl_present, 1));
  HANDLE_CODE(bref.unpack(switching_time_ul_present, 1));

  if (switching_time_dl_present) {
    HANDLE_CODE(switching_time_dl.unpack(bref));
  }
  if (switching_time_ul_present) {
    HANDLE_CODE(switching_time_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_switching_time_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (switching_time_dl_present) {
    j.write_str("switchingTimeDL", switching_time_dl.to_string());
  }
  if (switching_time_ul_present) {
    j.write_str("switchingTimeUL", switching_time_ul.to_string());
  }
  j.end_obj();
}

const char* srs_switching_time_eutra_s::switching_time_dl_opts::to_string() const
{
  static const char* names[] = {"n0",
                                "n0dot5",
                                "n1",
                                "n1dot5",
                                "n2",
                                "n2dot5",
                                "n3",
                                "n3dot5",
                                "n4",
                                "n4dot5",
                                "n5",
                                "n5dot5",
                                "n6",
                                "n6dot5",
                                "n7"};
  return convert_enum_idx(names, 15, value, "srs_switching_time_eutra_s::switching_time_dl_e_");
}
float srs_switching_time_eutra_s::switching_time_dl_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return map_enum_number(numbers, 15, value, "srs_switching_time_eutra_s::switching_time_dl_e_");
}
const char* srs_switching_time_eutra_s::switching_time_dl_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0", "0.5", "1", "1.5", "2", "2.5", "3", "3.5", "4", "4.5", "5", "5.5", "6", "6.5", "7"};
  return convert_enum_idx(number_strs, 15, value, "srs_switching_time_eutra_s::switching_time_dl_e_");
}

const char* srs_switching_time_eutra_s::switching_time_ul_opts::to_string() const
{
  static const char* names[] = {"n0",
                                "n0dot5",
                                "n1",
                                "n1dot5",
                                "n2",
                                "n2dot5",
                                "n3",
                                "n3dot5",
                                "n4",
                                "n4dot5",
                                "n5",
                                "n5dot5",
                                "n6",
                                "n6dot5",
                                "n7"};
  return convert_enum_idx(names, 15, value, "srs_switching_time_eutra_s::switching_time_ul_e_");
}
float srs_switching_time_eutra_s::switching_time_ul_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return map_enum_number(numbers, 15, value, "srs_switching_time_eutra_s::switching_time_ul_e_");
}
const char* srs_switching_time_eutra_s::switching_time_ul_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0", "0.5", "1", "1.5", "2", "2.5", "3", "3.5", "4", "4.5", "5", "5.5", "6", "6.5", "7"};
  return convert_enum_idx(number_strs, 15, value, "srs_switching_time_eutra_s::switching_time_ul_e_");
}

// SRS-SwitchingTimeNR ::= SEQUENCE
SRSASN_CODE srs_switching_time_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(switching_time_dl_present, 1));
  HANDLE_CODE(bref.pack(switching_time_ul_present, 1));

  if (switching_time_dl_present) {
    HANDLE_CODE(switching_time_dl.pack(bref));
  }
  if (switching_time_ul_present) {
    HANDLE_CODE(switching_time_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_switching_time_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(switching_time_dl_present, 1));
  HANDLE_CODE(bref.unpack(switching_time_ul_present, 1));

  if (switching_time_dl_present) {
    HANDLE_CODE(switching_time_dl.unpack(bref));
  }
  if (switching_time_ul_present) {
    HANDLE_CODE(switching_time_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_switching_time_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (switching_time_dl_present) {
    j.write_str("switchingTimeDL", switching_time_dl.to_string());
  }
  if (switching_time_ul_present) {
    j.write_str("switchingTimeUL", switching_time_ul.to_string());
  }
  j.end_obj();
}

const char* srs_switching_time_nr_s::switching_time_dl_opts::to_string() const
{
  static const char* names[] = {"n0us", "n30us", "n100us", "n140us", "n200us", "n300us", "n500us", "n900us"};
  return convert_enum_idx(names, 8, value, "srs_switching_time_nr_s::switching_time_dl_e_");
}
uint16_t srs_switching_time_nr_s::switching_time_dl_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 30, 100, 140, 200, 300, 500, 900};
  return map_enum_number(numbers, 8, value, "srs_switching_time_nr_s::switching_time_dl_e_");
}

const char* srs_switching_time_nr_s::switching_time_ul_opts::to_string() const
{
  static const char* names[] = {"n0us", "n30us", "n100us", "n140us", "n200us", "n300us", "n500us", "n900us"};
  return convert_enum_idx(names, 8, value, "srs_switching_time_nr_s::switching_time_ul_e_");
}
uint16_t srs_switching_time_nr_s::switching_time_ul_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 30, 100, 140, 200, 300, 500, 900};
  return map_enum_number(numbers, 8, value, "srs_switching_time_nr_s::switching_time_ul_e_");
}

// BandParameters-v1540 ::= SEQUENCE
SRSASN_CODE band_params_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_carrier_switch_present, 1));
  HANDLE_CODE(bref.pack(srs_tx_switch_present, 1));

  if (srs_carrier_switch_present) {
    HANDLE_CODE(srs_carrier_switch.pack(bref));
  }
  if (srs_tx_switch_present) {
    HANDLE_CODE(bref.pack(srs_tx_switch.tx_switch_impact_to_rx_present, 1));
    HANDLE_CODE(bref.pack(srs_tx_switch.tx_switch_with_another_band_present, 1));
    HANDLE_CODE(srs_tx_switch.supported_srs_tx_port_switch.pack(bref));
    if (srs_tx_switch.tx_switch_impact_to_rx_present) {
      HANDLE_CODE(pack_integer(bref, srs_tx_switch.tx_switch_impact_to_rx, (uint8_t)1u, (uint8_t)32u));
    }
    if (srs_tx_switch.tx_switch_with_another_band_present) {
      HANDLE_CODE(pack_integer(bref, srs_tx_switch.tx_switch_with_another_band, (uint8_t)1u, (uint8_t)32u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_carrier_switch_present, 1));
  HANDLE_CODE(bref.unpack(srs_tx_switch_present, 1));

  if (srs_carrier_switch_present) {
    HANDLE_CODE(srs_carrier_switch.unpack(bref));
  }
  if (srs_tx_switch_present) {
    HANDLE_CODE(bref.unpack(srs_tx_switch.tx_switch_impact_to_rx_present, 1));
    HANDLE_CODE(bref.unpack(srs_tx_switch.tx_switch_with_another_band_present, 1));
    HANDLE_CODE(srs_tx_switch.supported_srs_tx_port_switch.unpack(bref));
    if (srs_tx_switch.tx_switch_impact_to_rx_present) {
      HANDLE_CODE(unpack_integer(srs_tx_switch.tx_switch_impact_to_rx, bref, (uint8_t)1u, (uint8_t)32u));
    }
    if (srs_tx_switch.tx_switch_with_another_band_present) {
      HANDLE_CODE(unpack_integer(srs_tx_switch.tx_switch_with_another_band, bref, (uint8_t)1u, (uint8_t)32u));
    }
  }

  return SRSASN_SUCCESS;
}
void band_params_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_carrier_switch_present) {
    j.write_fieldname("srs-CarrierSwitch");
    srs_carrier_switch.to_json(j);
  }
  if (srs_tx_switch_present) {
    j.write_fieldname("srs-TxSwitch");
    j.start_obj();
    j.write_str("supportedSRS-TxPortSwitch", srs_tx_switch.supported_srs_tx_port_switch.to_string());
    if (srs_tx_switch.tx_switch_impact_to_rx_present) {
      j.write_int("txSwitchImpactToRx", srs_tx_switch.tx_switch_impact_to_rx);
    }
    if (srs_tx_switch.tx_switch_with_another_band_present) {
      j.write_int("txSwitchWithAnotherBand", srs_tx_switch.tx_switch_with_another_band);
    }
    j.end_obj();
  }
  j.end_obj();
}

void band_params_v1540_s::srs_carrier_switch_c_::destroy_()
{
  switch (type_) {
    case types::nr:
      c.destroy<nr_s_>();
      break;
    case types::eutra:
      c.destroy<eutra_s_>();
      break;
    default:
      break;
  }
}
void band_params_v1540_s::srs_carrier_switch_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr:
      c.init<nr_s_>();
      break;
    case types::eutra:
      c.init<eutra_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_params_v1540_s::srs_carrier_switch_c_");
  }
}
band_params_v1540_s::srs_carrier_switch_c_::srs_carrier_switch_c_(
    const band_params_v1540_s::srs_carrier_switch_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr:
      c.init(other.c.get<nr_s_>());
      break;
    case types::eutra:
      c.init(other.c.get<eutra_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_params_v1540_s::srs_carrier_switch_c_");
  }
}
band_params_v1540_s::srs_carrier_switch_c_&
band_params_v1540_s::srs_carrier_switch_c_::operator=(const band_params_v1540_s::srs_carrier_switch_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr:
      c.set(other.c.get<nr_s_>());
      break;
    case types::eutra:
      c.set(other.c.get<eutra_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_params_v1540_s::srs_carrier_switch_c_");
  }

  return *this;
}
band_params_v1540_s::srs_carrier_switch_c_::nr_s_& band_params_v1540_s::srs_carrier_switch_c_::set_nr()
{
  set(types::nr);
  return c.get<nr_s_>();
}
band_params_v1540_s::srs_carrier_switch_c_::eutra_s_& band_params_v1540_s::srs_carrier_switch_c_::set_eutra()
{
  set(types::eutra);
  return c.get<eutra_s_>();
}
void band_params_v1540_s::srs_carrier_switch_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr:
      j.write_fieldname("nr");
      j.start_obj();
      j.start_array("srs-SwitchingTimesListNR");
      for (const auto& e1 : c.get<nr_s_>().srs_switching_times_list_nr) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    case types::eutra:
      j.write_fieldname("eutra");
      j.start_obj();
      j.start_array("srs-SwitchingTimesListEUTRA");
      for (const auto& e1 : c.get<eutra_s_>().srs_switching_times_list_eutra) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "band_params_v1540_s::srs_carrier_switch_c_");
  }
  j.end_obj();
}
SRSASN_CODE band_params_v1540_s::srs_carrier_switch_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<nr_s_>().srs_switching_times_list_nr, 1, 32));
      break;
    case types::eutra:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<eutra_s_>().srs_switching_times_list_eutra, 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "band_params_v1540_s::srs_carrier_switch_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_params_v1540_s::srs_carrier_switch_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<nr_s_>().srs_switching_times_list_nr, bref, 1, 32));
      break;
    case types::eutra:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<eutra_s_>().srs_switching_times_list_eutra, bref, 1, 32));
      break;
    default:
      log_invalid_choice_id(type_, "band_params_v1540_s::srs_carrier_switch_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* band_params_v1540_s::srs_tx_switch_s_::supported_srs_tx_port_switch_opts::to_string() const
{
  static const char* names[] = {"t1r2", "t1r4", "t2r4", "t1r4-t2r4", "t1r1", "t2r2", "t4r4", "notSupported"};
  return convert_enum_idx(names, 8, value, "band_params_v1540_s::srs_tx_switch_s_::supported_srs_tx_port_switch_e_");
}

// CA-ParametersNR-v1540 ::= SEQUENCE
SRSASN_CODE ca_params_nr_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(simul_srs_assoc_csi_rs_all_cc_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_im_reception_for_feedback_per_band_comb_present, 1));
  HANDLE_CODE(bref.pack(simul_csi_reports_all_cc_present, 1));
  HANDLE_CODE(bref.pack(dual_pa_architecture_present, 1));

  if (simul_srs_assoc_csi_rs_all_cc_present) {
    HANDLE_CODE(pack_integer(bref, simul_srs_assoc_csi_rs_all_cc, (uint8_t)5u, (uint8_t)32u));
  }
  if (csi_rs_im_reception_for_feedback_per_band_comb_present) {
    HANDLE_CODE(
        bref.pack(csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc_present, 1));
    HANDLE_CODE(bref.pack(
        csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present, 1));
    if (csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc_present) {
      HANDLE_CODE(pack_integer(bref,
                               csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc,
                               (uint8_t)1u,
                               (uint8_t)64u));
    }
    if (csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present) {
      HANDLE_CODE(
          pack_integer(bref,
                       csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc,
                       (uint16_t)2u,
                       (uint16_t)256u));
    }
  }
  if (simul_csi_reports_all_cc_present) {
    HANDLE_CODE(pack_integer(bref, simul_csi_reports_all_cc, (uint8_t)5u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nr_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(simul_srs_assoc_csi_rs_all_cc_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_im_reception_for_feedback_per_band_comb_present, 1));
  HANDLE_CODE(bref.unpack(simul_csi_reports_all_cc_present, 1));
  HANDLE_CODE(bref.unpack(dual_pa_architecture_present, 1));

  if (simul_srs_assoc_csi_rs_all_cc_present) {
    HANDLE_CODE(unpack_integer(simul_srs_assoc_csi_rs_all_cc, bref, (uint8_t)5u, (uint8_t)32u));
  }
  if (csi_rs_im_reception_for_feedback_per_band_comb_present) {
    HANDLE_CODE(
        bref.unpack(csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc_present, 1));
    HANDLE_CODE(bref.unpack(
        csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present, 1));
    if (csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc_present) {
      HANDLE_CODE(unpack_integer(csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc,
                                 bref,
                                 (uint8_t)1u,
                                 (uint8_t)64u));
    }
    if (csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present) {
      HANDLE_CODE(
          unpack_integer(csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc,
                         bref,
                         (uint16_t)2u,
                         (uint16_t)256u));
    }
  }
  if (simul_csi_reports_all_cc_present) {
    HANDLE_CODE(unpack_integer(simul_csi_reports_all_cc, bref, (uint8_t)5u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void ca_params_nr_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (simul_srs_assoc_csi_rs_all_cc_present) {
    j.write_int("simultaneousSRS-AssocCSI-RS-AllCC", simul_srs_assoc_csi_rs_all_cc);
  }
  if (csi_rs_im_reception_for_feedback_per_band_comb_present) {
    j.write_fieldname("csi-RS-IM-ReceptionForFeedbackPerBandComb");
    j.start_obj();
    if (csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc_present) {
      j.write_int("maxNumberSimultaneousNZP-CSI-RS-ActBWP-AllCC",
                  csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc);
    }
    if (csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present) {
      j.write_int("totalNumberPortsSimultaneousNZP-CSI-RS-ActBWP-AllCC",
                  csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc);
    }
    j.end_obj();
  }
  if (simul_csi_reports_all_cc_present) {
    j.write_int("simultaneousCSI-ReportsAllCC", simul_csi_reports_all_cc);
  }
  if (dual_pa_architecture_present) {
    j.write_str("dualPA-Architecture", "supported");
  }
  j.end_obj();
}

// BandCombination-v1540 ::= SEQUENCE
SRSASN_CODE band_combination_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_params_nr_v1540_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, band_list_v1540, 1, 32));
  if (ca_params_nr_v1540_present) {
    HANDLE_CODE(ca_params_nr_v1540.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_params_nr_v1540_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(band_list_v1540, bref, 1, 32));
  if (ca_params_nr_v1540_present) {
    HANDLE_CODE(ca_params_nr_v1540.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandList-v1540");
  for (const auto& e1 : band_list_v1540) {
    e1.to_json(j);
  }
  j.end_array();
  if (ca_params_nr_v1540_present) {
    j.write_fieldname("ca-ParametersNR-v1540");
    ca_params_nr_v1540.to_json(j);
  }
  j.end_obj();
}

// CA-ParametersNR-v1550 ::= SEQUENCE
SRSASN_CODE ca_params_nr_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dummy_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nr_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  return SRSASN_SUCCESS;
}
void ca_params_nr_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dummy_present) {
    j.write_str("dummy", "supported");
  }
  j.end_obj();
}

// BandCombination-v1550 ::= SEQUENCE
SRSASN_CODE band_combination_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ca_params_nr_v1550.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ca_params_nr_v1550.unpack(bref));

  return SRSASN_SUCCESS;
}
void band_combination_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ca-ParametersNR-v1550");
  ca_params_nr_v1550.to_json(j);
  j.end_obj();
}

// CA-ParametersNR-v1560 ::= SEQUENCE
SRSASN_CODE ca_params_nr_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(diff_numerology_within_pucch_group_larger_scs_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nr_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(diff_numerology_within_pucch_group_larger_scs_present, 1));

  return SRSASN_SUCCESS;
}
void ca_params_nr_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (diff_numerology_within_pucch_group_larger_scs_present) {
    j.write_str("diffNumerologyWithinPUCCH-GroupLargerSCS", "supported");
  }
  j.end_obj();
}

// CA-ParametersEUTRA-v1560 ::= SEQUENCE
SRSASN_CODE ca_params_eutra_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fd_mimo_total_weighted_layers_present, 1));

  if (fd_mimo_total_weighted_layers_present) {
    HANDLE_CODE(pack_integer(bref, fd_mimo_total_weighted_layers, (uint8_t)2u, (uint8_t)128u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_eutra_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fd_mimo_total_weighted_layers_present, 1));

  if (fd_mimo_total_weighted_layers_present) {
    HANDLE_CODE(unpack_integer(fd_mimo_total_weighted_layers, bref, (uint8_t)2u, (uint8_t)128u));
  }

  return SRSASN_SUCCESS;
}
void ca_params_eutra_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fd_mimo_total_weighted_layers_present) {
    j.write_int("fd-MIMO-TotalWeightedLayers", fd_mimo_total_weighted_layers);
  }
  j.end_obj();
}

// CA-ParametersNRDC ::= SEQUENCE
SRSASN_CODE ca_params_nrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_params_nr_for_dc_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nr_for_dc_v1540_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nr_for_dc_v1550_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nr_for_dc_v1560_present, 1));
  HANDLE_CODE(bref.pack(feature_set_combination_dc_present, 1));

  if (ca_params_nr_for_dc_present) {
    HANDLE_CODE(ca_params_nr_for_dc.pack(bref));
  }
  if (ca_params_nr_for_dc_v1540_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v1540.pack(bref));
  }
  if (ca_params_nr_for_dc_v1550_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v1550.pack(bref));
  }
  if (ca_params_nr_for_dc_v1560_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v1560.pack(bref));
  }
  if (feature_set_combination_dc_present) {
    HANDLE_CODE(pack_integer(bref, feature_set_combination_dc, (uint16_t)0u, (uint16_t)1024u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_params_nr_for_dc_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nr_for_dc_v1540_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nr_for_dc_v1550_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nr_for_dc_v1560_present, 1));
  HANDLE_CODE(bref.unpack(feature_set_combination_dc_present, 1));

  if (ca_params_nr_for_dc_present) {
    HANDLE_CODE(ca_params_nr_for_dc.unpack(bref));
  }
  if (ca_params_nr_for_dc_v1540_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v1540.unpack(bref));
  }
  if (ca_params_nr_for_dc_v1550_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v1550.unpack(bref));
  }
  if (ca_params_nr_for_dc_v1560_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v1560.unpack(bref));
  }
  if (feature_set_combination_dc_present) {
    HANDLE_CODE(unpack_integer(feature_set_combination_dc, bref, (uint16_t)0u, (uint16_t)1024u));
  }

  return SRSASN_SUCCESS;
}
void ca_params_nrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ca_params_nr_for_dc_present) {
    j.write_fieldname("ca-ParametersNR-ForDC");
    ca_params_nr_for_dc.to_json(j);
  }
  if (ca_params_nr_for_dc_v1540_present) {
    j.write_fieldname("ca-ParametersNR-ForDC-v1540");
    ca_params_nr_for_dc_v1540.to_json(j);
  }
  if (ca_params_nr_for_dc_v1550_present) {
    j.write_fieldname("ca-ParametersNR-ForDC-v1550");
    ca_params_nr_for_dc_v1550.to_json(j);
  }
  if (ca_params_nr_for_dc_v1560_present) {
    j.write_fieldname("ca-ParametersNR-ForDC-v1560");
    ca_params_nr_for_dc_v1560.to_json(j);
  }
  if (feature_set_combination_dc_present) {
    j.write_int("featureSetCombinationDC", feature_set_combination_dc);
  }
  j.end_obj();
}

// BandCombination-v1560 ::= SEQUENCE
SRSASN_CODE band_combination_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ne_dc_bc_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nrdc_present, 1));
  HANDLE_CODE(bref.pack(ca_params_eutra_v1560_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nr_v1560_present, 1));

  if (ca_params_nrdc_present) {
    HANDLE_CODE(ca_params_nrdc.pack(bref));
  }
  if (ca_params_eutra_v1560_present) {
    HANDLE_CODE(ca_params_eutra_v1560.pack(bref));
  }
  if (ca_params_nr_v1560_present) {
    HANDLE_CODE(ca_params_nr_v1560.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ne_dc_bc_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nrdc_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_eutra_v1560_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nr_v1560_present, 1));

  if (ca_params_nrdc_present) {
    HANDLE_CODE(ca_params_nrdc.unpack(bref));
  }
  if (ca_params_eutra_v1560_present) {
    HANDLE_CODE(ca_params_eutra_v1560.unpack(bref));
  }
  if (ca_params_nr_v1560_present) {
    HANDLE_CODE(ca_params_nr_v1560.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ne_dc_bc_present) {
    j.write_str("ne-DC-BC", "supported");
  }
  if (ca_params_nrdc_present) {
    j.write_fieldname("ca-ParametersNRDC");
    ca_params_nrdc.to_json(j);
  }
  if (ca_params_eutra_v1560_present) {
    j.write_fieldname("ca-ParametersEUTRA-v1560");
    ca_params_eutra_v1560.to_json(j);
  }
  if (ca_params_nr_v1560_present) {
    j.write_fieldname("ca-ParametersNR-v1560");
    ca_params_nr_v1560.to_json(j);
  }
  j.end_obj();
}

// SupportedCSI-RS-Resource ::= SEQUENCE
SRSASN_CODE supported_csi_rs_res_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_tx_ports_per_res.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_res_per_band, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, total_num_tx_ports_per_band, (uint16_t)2u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_csi_rs_res_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_tx_ports_per_res.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_res_per_band, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(total_num_tx_ports_per_band, bref, (uint16_t)2u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
void supported_csi_rs_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberTxPortsPerResource", max_num_tx_ports_per_res.to_string());
  j.write_int("maxNumberResourcesPerBand", max_num_res_per_band);
  j.write_int("totalNumberTxPortsPerBand", total_num_tx_ports_per_band);
  j.end_obj();
}

const char* supported_csi_rs_res_s::max_num_tx_ports_per_res_opts::to_string() const
{
  static const char* names[] = {"p2", "p4", "p8", "p12", "p16", "p24", "p32"};
  return convert_enum_idx(names, 7, value, "supported_csi_rs_res_s::max_num_tx_ports_per_res_e_");
}
uint8_t supported_csi_rs_res_s::max_num_tx_ports_per_res_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 12, 16, 24, 32};
  return map_enum_number(numbers, 7, value, "supported_csi_rs_res_s::max_num_tx_ports_per_res_e_");
}

// BeamManagementSSB-CSI-RS ::= SEQUENCE
SRSASN_CODE beam_management_ssb_csi_rs_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_csi_rs_density_present, 1));

  HANDLE_CODE(max_num_ssb_csi_rs_res_one_tx.pack(bref));
  HANDLE_CODE(max_num_csi_rs_res.pack(bref));
  HANDLE_CODE(max_num_csi_rs_res_two_tx.pack(bref));
  if (supported_csi_rs_density_present) {
    HANDLE_CODE(supported_csi_rs_density.pack(bref));
  }
  HANDLE_CODE(max_num_aperiodic_csi_rs_res.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE beam_management_ssb_csi_rs_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_csi_rs_density_present, 1));

  HANDLE_CODE(max_num_ssb_csi_rs_res_one_tx.unpack(bref));
  HANDLE_CODE(max_num_csi_rs_res.unpack(bref));
  HANDLE_CODE(max_num_csi_rs_res_two_tx.unpack(bref));
  if (supported_csi_rs_density_present) {
    HANDLE_CODE(supported_csi_rs_density.unpack(bref));
  }
  HANDLE_CODE(max_num_aperiodic_csi_rs_res.unpack(bref));

  return SRSASN_SUCCESS;
}
void beam_management_ssb_csi_rs_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberSSB-CSI-RS-ResourceOneTx", max_num_ssb_csi_rs_res_one_tx.to_string());
  j.write_str("maxNumberCSI-RS-Resource", max_num_csi_rs_res.to_string());
  j.write_str("maxNumberCSI-RS-ResourceTwoTx", max_num_csi_rs_res_two_tx.to_string());
  if (supported_csi_rs_density_present) {
    j.write_str("supportedCSI-RS-Density", supported_csi_rs_density.to_string());
  }
  j.write_str("maxNumberAperiodicCSI-RS-Resource", max_num_aperiodic_csi_rs_res.to_string());
  j.end_obj();
}

const char* beam_management_ssb_csi_rs_s::max_num_ssb_csi_rs_res_one_tx_opts::to_string() const
{
  static const char* names[] = {"n0", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 5, value, "beam_management_ssb_csi_rs_s::max_num_ssb_csi_rs_res_one_tx_e_");
}
uint8_t beam_management_ssb_csi_rs_s::max_num_ssb_csi_rs_res_one_tx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 8, 16, 32, 64};
  return map_enum_number(numbers, 5, value, "beam_management_ssb_csi_rs_s::max_num_ssb_csi_rs_res_one_tx_e_");
}

const char* beam_management_ssb_csi_rs_s::max_num_csi_rs_res_opts::to_string() const
{
  static const char* names[] = {"n0", "n4", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 6, value, "beam_management_ssb_csi_rs_s::max_num_csi_rs_res_e_");
}
uint8_t beam_management_ssb_csi_rs_s::max_num_csi_rs_res_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 6, value, "beam_management_ssb_csi_rs_s::max_num_csi_rs_res_e_");
}

const char* beam_management_ssb_csi_rs_s::max_num_csi_rs_res_two_tx_opts::to_string() const
{
  static const char* names[] = {"n0", "n4", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 6, value, "beam_management_ssb_csi_rs_s::max_num_csi_rs_res_two_tx_e_");
}
uint8_t beam_management_ssb_csi_rs_s::max_num_csi_rs_res_two_tx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 6, value, "beam_management_ssb_csi_rs_s::max_num_csi_rs_res_two_tx_e_");
}

const char* beam_management_ssb_csi_rs_s::supported_csi_rs_density_opts::to_string() const
{
  static const char* names[] = {"one", "three", "oneAndThree"};
  return convert_enum_idx(names, 3, value, "beam_management_ssb_csi_rs_s::supported_csi_rs_density_e_");
}

const char* beam_management_ssb_csi_rs_s::max_num_aperiodic_csi_rs_res_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n4", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 7, value, "beam_management_ssb_csi_rs_s::max_num_aperiodic_csi_rs_res_e_");
}
uint8_t beam_management_ssb_csi_rs_s::max_num_aperiodic_csi_rs_res_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "beam_management_ssb_csi_rs_s::max_num_aperiodic_csi_rs_res_e_");
}

// CSI-RS-ForTracking ::= SEQUENCE
SRSASN_CODE csi_rs_for_tracking_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, max_burst_len, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(pack_integer(bref, max_simul_res_sets_per_cc, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, max_cfg_res_sets_per_cc, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, max_cfg_res_sets_all_cc, (uint16_t)1u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_for_tracking_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(max_burst_len, bref, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(unpack_integer(max_simul_res_sets_per_cc, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(max_cfg_res_sets_per_cc, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(max_cfg_res_sets_all_cc, bref, (uint16_t)1u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
void csi_rs_for_tracking_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxBurstLength", max_burst_len);
  j.write_int("maxSimultaneousResourceSetsPerCC", max_simul_res_sets_per_cc);
  j.write_int("maxConfiguredResourceSetsPerCC", max_cfg_res_sets_per_cc);
  j.write_int("maxConfiguredResourceSetsAllCC", max_cfg_res_sets_all_cc);
  j.end_obj();
}

// CSI-RS-IM-ReceptionForFeedback ::= SEQUENCE
SRSASN_CODE csi_rs_im_reception_for_feedback_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, max_cfg_num_nzp_csi_rs_per_cc, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, max_cfg_num_ports_across_nzp_csi_rs_per_cc, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(max_cfg_num_csi_im_per_cc.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_simul_nzp_csi_rs_per_cc, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, total_num_ports_simul_nzp_csi_rs_per_cc, (uint16_t)2u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_im_reception_for_feedback_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(max_cfg_num_nzp_csi_rs_per_cc, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(max_cfg_num_ports_across_nzp_csi_rs_per_cc, bref, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(max_cfg_num_csi_im_per_cc.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_simul_nzp_csi_rs_per_cc, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(total_num_ports_simul_nzp_csi_rs_per_cc, bref, (uint16_t)2u, (uint16_t)256u));

  return SRSASN_SUCCESS;
}
void csi_rs_im_reception_for_feedback_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxConfigNumberNZP-CSI-RS-PerCC", max_cfg_num_nzp_csi_rs_per_cc);
  j.write_int("maxConfigNumberPortsAcrossNZP-CSI-RS-PerCC", max_cfg_num_ports_across_nzp_csi_rs_per_cc);
  j.write_str("maxConfigNumberCSI-IM-PerCC", max_cfg_num_csi_im_per_cc.to_string());
  j.write_int("maxNumberSimultaneousNZP-CSI-RS-PerCC", max_num_simul_nzp_csi_rs_per_cc);
  j.write_int("totalNumberPortsSimultaneousNZP-CSI-RS-PerCC", total_num_ports_simul_nzp_csi_rs_per_cc);
  j.end_obj();
}

const char* csi_rs_im_reception_for_feedback_s::max_cfg_num_csi_im_per_cc_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n16", "n32"};
  return convert_enum_idx(names, 6, value, "csi_rs_im_reception_for_feedback_s::max_cfg_num_csi_im_per_cc_e_");
}
uint8_t csi_rs_im_reception_for_feedback_s::max_cfg_num_csi_im_per_cc_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "csi_rs_im_reception_for_feedback_s::max_cfg_num_csi_im_per_cc_e_");
}

// CSI-RS-ProcFrameworkForSRS ::= SEQUENCE
SRSASN_CODE csi_rs_proc_framework_for_srs_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, max_num_periodic_srs_assoc_csi_rs_per_bwp, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_aperiodic_srs_assoc_csi_rs_per_bwp, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_sp_srs_assoc_csi_rs_per_bwp, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, simul_srs_assoc_csi_rs_per_cc, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_proc_framework_for_srs_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(max_num_periodic_srs_assoc_csi_rs_per_bwp, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_aperiodic_srs_assoc_csi_rs_per_bwp, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_sp_srs_assoc_csi_rs_per_bwp, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(simul_srs_assoc_csi_rs_per_cc, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void csi_rs_proc_framework_for_srs_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxNumberPeriodicSRS-AssocCSI-RS-PerBWP", max_num_periodic_srs_assoc_csi_rs_per_bwp);
  j.write_int("maxNumberAperiodicSRS-AssocCSI-RS-PerBWP", max_num_aperiodic_srs_assoc_csi_rs_per_bwp);
  j.write_int("maxNumberSP-SRS-AssocCSI-RS-PerBWP", max_num_sp_srs_assoc_csi_rs_per_bwp);
  j.write_int("simultaneousSRS-AssocCSI-RS-PerCC", simul_srs_assoc_csi_rs_per_cc);
  j.end_obj();
}

// CSI-ReportFramework ::= SEQUENCE
SRSASN_CODE csi_report_framework_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, max_num_periodic_csi_per_bwp_for_csi_report, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_aperiodic_csi_per_bwp_for_csi_report, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_semi_persistent_csi_per_bwp_for_csi_report, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_periodic_csi_per_bwp_for_beam_report, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_aperiodic_csi_per_bwp_for_beam_report, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(max_num_aperiodic_csi_trigger_state_per_cc.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_semi_persistent_csi_per_bwp_for_beam_report, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, simul_csi_reports_per_cc, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_framework_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(max_num_periodic_csi_per_bwp_for_csi_report, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_aperiodic_csi_per_bwp_for_csi_report, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_semi_persistent_csi_per_bwp_for_csi_report, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_periodic_csi_per_bwp_for_beam_report, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_aperiodic_csi_per_bwp_for_beam_report, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(max_num_aperiodic_csi_trigger_state_per_cc.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_semi_persistent_csi_per_bwp_for_beam_report, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(simul_csi_reports_per_cc, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void csi_report_framework_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxNumberPeriodicCSI-PerBWP-ForCSI-Report", max_num_periodic_csi_per_bwp_for_csi_report);
  j.write_int("maxNumberAperiodicCSI-PerBWP-ForCSI-Report", max_num_aperiodic_csi_per_bwp_for_csi_report);
  j.write_int("maxNumberSemiPersistentCSI-PerBWP-ForCSI-Report", max_num_semi_persistent_csi_per_bwp_for_csi_report);
  j.write_int("maxNumberPeriodicCSI-PerBWP-ForBeamReport", max_num_periodic_csi_per_bwp_for_beam_report);
  j.write_int("maxNumberAperiodicCSI-PerBWP-ForBeamReport", max_num_aperiodic_csi_per_bwp_for_beam_report);
  j.write_str("maxNumberAperiodicCSI-triggeringStatePerCC", max_num_aperiodic_csi_trigger_state_per_cc.to_string());
  j.write_int("maxNumberSemiPersistentCSI-PerBWP-ForBeamReport", max_num_semi_persistent_csi_per_bwp_for_beam_report);
  j.write_int("simultaneousCSI-ReportsPerCC", simul_csi_reports_per_cc);
  j.end_obj();
}

const char* csi_report_framework_s::max_num_aperiodic_csi_trigger_state_per_cc_opts::to_string() const
{
  static const char* names[] = {"n3", "n7", "n15", "n31", "n63", "n128"};
  return convert_enum_idx(names, 6, value, "csi_report_framework_s::max_num_aperiodic_csi_trigger_state_per_cc_e_");
}
uint8_t csi_report_framework_s::max_num_aperiodic_csi_trigger_state_per_cc_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 7, 15, 31, 63, 128};
  return map_enum_number(numbers, 6, value, "csi_report_framework_s::max_num_aperiodic_csi_trigger_state_per_cc_e_");
}

// CodebookParameters ::= SEQUENCE
SRSASN_CODE codebook_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(type2_present, 1));
  HANDLE_CODE(bref.pack(type2_port_sel_present, 1));

  HANDLE_CODE(bref.pack(type1.multi_panel_present, 1));
  HANDLE_CODE(pack_dyn_seq_of(bref, type1.single_panel.supported_csi_rs_res_list, 1, 7));
  HANDLE_CODE(type1.single_panel.modes.pack(bref));
  HANDLE_CODE(pack_integer(bref, type1.single_panel.max_num_csi_rs_per_res_set, (uint8_t)1u, (uint8_t)8u));
  if (type1.multi_panel_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, type1.multi_panel.supported_csi_rs_res_list, 1, 7));
    HANDLE_CODE(type1.multi_panel.modes.pack(bref));
    HANDLE_CODE(type1.multi_panel.nrof_panels.pack(bref));
    HANDLE_CODE(pack_integer(bref, type1.multi_panel.max_num_csi_rs_per_res_set, (uint8_t)1u, (uint8_t)8u));
  }
  if (type2_present) {
    HANDLE_CODE(bref.pack(type2.amplitude_subset_restrict_present, 1));
    HANDLE_CODE(pack_dyn_seq_of(bref, type2.supported_csi_rs_res_list, 1, 7));
    HANDLE_CODE(pack_integer(bref, type2.param_lx, (uint8_t)2u, (uint8_t)4u));
    HANDLE_CODE(type2.amplitude_scaling_type.pack(bref));
  }
  if (type2_port_sel_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, type2_port_sel.supported_csi_rs_res_list, 1, 7));
    HANDLE_CODE(pack_integer(bref, type2_port_sel.param_lx, (uint8_t)2u, (uint8_t)4u));
    HANDLE_CODE(type2_port_sel.amplitude_scaling_type.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(type2_present, 1));
  HANDLE_CODE(bref.unpack(type2_port_sel_present, 1));

  HANDLE_CODE(bref.unpack(type1.multi_panel_present, 1));
  HANDLE_CODE(unpack_dyn_seq_of(type1.single_panel.supported_csi_rs_res_list, bref, 1, 7));
  HANDLE_CODE(type1.single_panel.modes.unpack(bref));
  HANDLE_CODE(unpack_integer(type1.single_panel.max_num_csi_rs_per_res_set, bref, (uint8_t)1u, (uint8_t)8u));
  if (type1.multi_panel_present) {
    HANDLE_CODE(unpack_dyn_seq_of(type1.multi_panel.supported_csi_rs_res_list, bref, 1, 7));
    HANDLE_CODE(type1.multi_panel.modes.unpack(bref));
    HANDLE_CODE(type1.multi_panel.nrof_panels.unpack(bref));
    HANDLE_CODE(unpack_integer(type1.multi_panel.max_num_csi_rs_per_res_set, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (type2_present) {
    HANDLE_CODE(bref.unpack(type2.amplitude_subset_restrict_present, 1));
    HANDLE_CODE(unpack_dyn_seq_of(type2.supported_csi_rs_res_list, bref, 1, 7));
    HANDLE_CODE(unpack_integer(type2.param_lx, bref, (uint8_t)2u, (uint8_t)4u));
    HANDLE_CODE(type2.amplitude_scaling_type.unpack(bref));
  }
  if (type2_port_sel_present) {
    HANDLE_CODE(unpack_dyn_seq_of(type2_port_sel.supported_csi_rs_res_list, bref, 1, 7));
    HANDLE_CODE(unpack_integer(type2_port_sel.param_lx, bref, (uint8_t)2u, (uint8_t)4u));
    HANDLE_CODE(type2_port_sel.amplitude_scaling_type.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void codebook_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("type1");
  j.start_obj();
  j.write_fieldname("singlePanel");
  j.start_obj();
  j.start_array("supportedCSI-RS-ResourceList");
  for (const auto& e1 : type1.single_panel.supported_csi_rs_res_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("modes", type1.single_panel.modes.to_string());
  j.write_int("maxNumberCSI-RS-PerResourceSet", type1.single_panel.max_num_csi_rs_per_res_set);
  j.end_obj();
  if (type1.multi_panel_present) {
    j.write_fieldname("multiPanel");
    j.start_obj();
    j.start_array("supportedCSI-RS-ResourceList");
    for (const auto& e1 : type1.multi_panel.supported_csi_rs_res_list) {
      e1.to_json(j);
    }
    j.end_array();
    j.write_str("modes", type1.multi_panel.modes.to_string());
    j.write_str("nrofPanels", type1.multi_panel.nrof_panels.to_string());
    j.write_int("maxNumberCSI-RS-PerResourceSet", type1.multi_panel.max_num_csi_rs_per_res_set);
    j.end_obj();
  }
  j.end_obj();
  if (type2_present) {
    j.write_fieldname("type2");
    j.start_obj();
    j.start_array("supportedCSI-RS-ResourceList");
    for (const auto& e1 : type2.supported_csi_rs_res_list) {
      e1.to_json(j);
    }
    j.end_array();
    j.write_int("parameterLx", type2.param_lx);
    j.write_str("amplitudeScalingType", type2.amplitude_scaling_type.to_string());
    if (type2.amplitude_subset_restrict_present) {
      j.write_str("amplitudeSubsetRestriction", "supported");
    }
    j.end_obj();
  }
  if (type2_port_sel_present) {
    j.write_fieldname("type2-PortSelection");
    j.start_obj();
    j.start_array("supportedCSI-RS-ResourceList");
    for (const auto& e1 : type2_port_sel.supported_csi_rs_res_list) {
      e1.to_json(j);
    }
    j.end_array();
    j.write_int("parameterLx", type2_port_sel.param_lx);
    j.write_str("amplitudeScalingType", type2_port_sel.amplitude_scaling_type.to_string());
    j.end_obj();
  }
  j.end_obj();
}

const char* codebook_params_s::type1_s_::single_panel_s_::modes_opts::to_string() const
{
  static const char* names[] = {"mode1", "mode1andMode2"};
  return convert_enum_idx(names, 2, value, "codebook_params_s::type1_s_::single_panel_s_::modes_e_");
}

const char* codebook_params_s::type1_s_::multi_panel_s_::modes_opts::to_string() const
{
  static const char* names[] = {"mode1", "mode2", "both"};
  return convert_enum_idx(names, 3, value, "codebook_params_s::type1_s_::multi_panel_s_::modes_e_");
}
uint8_t codebook_params_s::type1_s_::multi_panel_s_::modes_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "codebook_params_s::type1_s_::multi_panel_s_::modes_e_");
}

const char* codebook_params_s::type1_s_::multi_panel_s_::nrof_panels_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(names, 2, value, "codebook_params_s::type1_s_::multi_panel_s_::nrof_panels_e_");
}
uint8_t codebook_params_s::type1_s_::multi_panel_s_::nrof_panels_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "codebook_params_s::type1_s_::multi_panel_s_::nrof_panels_e_");
}

const char* codebook_params_s::type2_s_::amplitude_scaling_type_opts::to_string() const
{
  static const char* names[] = {"wideband", "widebandAndSubband"};
  return convert_enum_idx(names, 2, value, "codebook_params_s::type2_s_::amplitude_scaling_type_e_");
}

const char* codebook_params_s::type2_port_sel_s_::amplitude_scaling_type_opts::to_string() const
{
  static const char* names[] = {"wideband", "widebandAndSubband"};
  return convert_enum_idx(names, 2, value, "codebook_params_s::type2_port_sel_s_::amplitude_scaling_type_e_");
}

// DummyG ::= SEQUENCE
SRSASN_CODE dummy_g_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_ssb_csi_rs_res_one_tx.pack(bref));
  HANDLE_CODE(max_num_ssb_csi_rs_res_two_tx.pack(bref));
  HANDLE_CODE(supported_csi_rs_density.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_g_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_ssb_csi_rs_res_one_tx.unpack(bref));
  HANDLE_CODE(max_num_ssb_csi_rs_res_two_tx.unpack(bref));
  HANDLE_CODE(supported_csi_rs_density.unpack(bref));

  return SRSASN_SUCCESS;
}
void dummy_g_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberSSB-CSI-RS-ResourceOneTx", max_num_ssb_csi_rs_res_one_tx.to_string());
  j.write_str("maxNumberSSB-CSI-RS-ResourceTwoTx", max_num_ssb_csi_rs_res_two_tx.to_string());
  j.write_str("supportedCSI-RS-Density", supported_csi_rs_density.to_string());
  j.end_obj();
}

const char* dummy_g_s::max_num_ssb_csi_rs_res_one_tx_opts::to_string() const
{
  static const char* names[] = {"n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 4, value, "dummy_g_s::max_num_ssb_csi_rs_res_one_tx_e_");
}
uint8_t dummy_g_s::max_num_ssb_csi_rs_res_one_tx_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 16, 32, 64};
  return map_enum_number(numbers, 4, value, "dummy_g_s::max_num_ssb_csi_rs_res_one_tx_e_");
}

const char* dummy_g_s::max_num_ssb_csi_rs_res_two_tx_opts::to_string() const
{
  static const char* names[] = {"n0", "n4", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 6, value, "dummy_g_s::max_num_ssb_csi_rs_res_two_tx_e_");
}
uint8_t dummy_g_s::max_num_ssb_csi_rs_res_two_tx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 6, value, "dummy_g_s::max_num_ssb_csi_rs_res_two_tx_e_");
}

const char* dummy_g_s::supported_csi_rs_density_opts::to_string() const
{
  static const char* names[] = {"one", "three", "oneAndThree"};
  return convert_enum_idx(names, 3, value, "dummy_g_s::supported_csi_rs_density_e_");
}

// DummyH ::= SEQUENCE
SRSASN_CODE dummy_h_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, burst_len, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(pack_integer(bref, max_simul_res_sets_per_cc, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, max_cfg_res_sets_per_cc, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, max_cfg_res_sets_all_cc, (uint8_t)1u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_h_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(burst_len, bref, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(unpack_integer(max_simul_res_sets_per_cc, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(max_cfg_res_sets_per_cc, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(max_cfg_res_sets_all_cc, bref, (uint8_t)1u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
void dummy_h_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("burstLength", burst_len);
  j.write_int("maxSimultaneousResourceSetsPerCC", max_simul_res_sets_per_cc);
  j.write_int("maxConfiguredResourceSetsPerCC", max_cfg_res_sets_per_cc);
  j.write_int("maxConfiguredResourceSetsAllCC", max_cfg_res_sets_all_cc);
  j.end_obj();
}

// PTRS-DensityRecommendationDL ::= SEQUENCE
SRSASN_CODE ptrs_density_recommendation_dl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, freq_density1, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, freq_density2, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, time_density1, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(pack_integer(bref, time_density2, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(pack_integer(bref, time_density3, (uint8_t)0u, (uint8_t)29u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ptrs_density_recommendation_dl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(freq_density1, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(freq_density2, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(time_density1, bref, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(unpack_integer(time_density2, bref, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(unpack_integer(time_density3, bref, (uint8_t)0u, (uint8_t)29u));

  return SRSASN_SUCCESS;
}
void ptrs_density_recommendation_dl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("frequencyDensity1", freq_density1);
  j.write_int("frequencyDensity2", freq_density2);
  j.write_int("timeDensity1", time_density1);
  j.write_int("timeDensity2", time_density2);
  j.write_int("timeDensity3", time_density3);
  j.end_obj();
}

// PTRS-DensityRecommendationUL ::= SEQUENCE
SRSASN_CODE ptrs_density_recommendation_ul_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, freq_density1, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, freq_density2, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, time_density1, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(pack_integer(bref, time_density2, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(pack_integer(bref, time_density3, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(pack_integer(bref, sample_density1, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, sample_density2, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, sample_density3, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, sample_density4, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, sample_density5, (uint16_t)1u, (uint16_t)276u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ptrs_density_recommendation_ul_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(freq_density1, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(freq_density2, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(time_density1, bref, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(unpack_integer(time_density2, bref, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(unpack_integer(time_density3, bref, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(unpack_integer(sample_density1, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(sample_density2, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(sample_density3, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(sample_density4, bref, (uint16_t)1u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(sample_density5, bref, (uint16_t)1u, (uint16_t)276u));

  return SRSASN_SUCCESS;
}
void ptrs_density_recommendation_ul_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("frequencyDensity1", freq_density1);
  j.write_int("frequencyDensity2", freq_density2);
  j.write_int("timeDensity1", time_density1);
  j.write_int("timeDensity2", time_density2);
  j.write_int("timeDensity3", time_density3);
  j.write_int("sampleDensity1", sample_density1);
  j.write_int("sampleDensity2", sample_density2);
  j.write_int("sampleDensity3", sample_density3);
  j.write_int("sampleDensity4", sample_density4);
  j.write_int("sampleDensity5", sample_density5);
  j.end_obj();
}

// SpatialRelations ::= SEQUENCE
SRSASN_CODE spatial_relations_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_active_spatial_relation_pucch_present, 1));

  HANDLE_CODE(max_num_cfg_spatial_relations.pack(bref));
  HANDLE_CODE(max_num_active_spatial_relations.pack(bref));
  HANDLE_CODE(max_num_dl_rs_qcl_type_d.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE spatial_relations_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_active_spatial_relation_pucch_present, 1));

  HANDLE_CODE(max_num_cfg_spatial_relations.unpack(bref));
  HANDLE_CODE(max_num_active_spatial_relations.unpack(bref));
  HANDLE_CODE(max_num_dl_rs_qcl_type_d.unpack(bref));

  return SRSASN_SUCCESS;
}
void spatial_relations_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberConfiguredSpatialRelations", max_num_cfg_spatial_relations.to_string());
  j.write_str("maxNumberActiveSpatialRelations", max_num_active_spatial_relations.to_string());
  if (add_active_spatial_relation_pucch_present) {
    j.write_str("additionalActiveSpatialRelationPUCCH", "supported");
  }
  j.write_str("maxNumberDL-RS-QCL-TypeD", max_num_dl_rs_qcl_type_d.to_string());
  j.end_obj();
}

const char* spatial_relations_s::max_num_cfg_spatial_relations_opts::to_string() const
{
  static const char* names[] = {"n4", "n8", "n16", "n32", "n64", "n96"};
  return convert_enum_idx(names, 6, value, "spatial_relations_s::max_num_cfg_spatial_relations_e_");
}
uint8_t spatial_relations_s::max_num_cfg_spatial_relations_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16, 32, 64, 96};
  return map_enum_number(numbers, 6, value, "spatial_relations_s::max_num_cfg_spatial_relations_e_");
}

const char* spatial_relations_s::max_num_active_spatial_relations_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n14"};
  return convert_enum_idx(names, 5, value, "spatial_relations_s::max_num_active_spatial_relations_e_");
}
uint8_t spatial_relations_s::max_num_active_spatial_relations_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 14};
  return map_enum_number(numbers, 5, value, "spatial_relations_s::max_num_active_spatial_relations_e_");
}

const char* spatial_relations_s::max_num_dl_rs_qcl_type_d_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n14"};
  return convert_enum_idx(names, 5, value, "spatial_relations_s::max_num_dl_rs_qcl_type_d_e_");
}
uint8_t spatial_relations_s::max_num_dl_rs_qcl_type_d_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 14};
  return map_enum_number(numbers, 5, value, "spatial_relations_s::max_num_dl_rs_qcl_type_d_e_");
}

// MIMO-ParametersPerBand ::= SEQUENCE
SRSASN_CODE mimo_params_per_band_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tci_state_pdsch_present, 1));
  HANDLE_CODE(bref.pack(add_active_tci_state_pdcch_present, 1));
  HANDLE_CODE(bref.pack(pusch_trans_coherence_present, 1));
  HANDLE_CODE(bref.pack(beam_correspondence_without_ul_beam_sweeping_present, 1));
  HANDLE_CODE(bref.pack(periodic_beam_report_present, 1));
  HANDLE_CODE(bref.pack(aperiodic_beam_report_present, 1));
  HANDLE_CODE(bref.pack(sp_beam_report_pucch_present, 1));
  HANDLE_CODE(bref.pack(sp_beam_report_pusch_present, 1));
  HANDLE_CODE(bref.pack(dummy1_present, 1));
  HANDLE_CODE(bref.pack(max_num_rx_beam_present, 1));
  HANDLE_CODE(bref.pack(max_num_rx_tx_beam_switch_dl_present, 1));
  HANDLE_CODE(bref.pack(max_num_non_group_beam_report_present, 1));
  HANDLE_CODE(bref.pack(group_beam_report_present, 1));
  HANDLE_CODE(bref.pack(ul_beam_management_present, 1));
  HANDLE_CODE(bref.pack(max_num_csi_rs_bfd_present, 1));
  HANDLE_CODE(bref.pack(max_num_ssb_bfd_present, 1));
  HANDLE_CODE(bref.pack(max_num_csi_rs_ssb_cbd_present, 1));
  HANDLE_CODE(bref.pack(dummy2_present, 1));
  HANDLE_CODE(bref.pack(two_ports_ptrs_ul_present, 1));
  HANDLE_CODE(bref.pack(dummy5_present, 1));
  HANDLE_CODE(bref.pack(dummy3_present, 1));
  HANDLE_CODE(bref.pack(beam_report_timing_present, 1));
  HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_dl_present, 1));
  HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_ul_present, 1));
  HANDLE_CODE(bref.pack(dummy4_present, 1));
  HANDLE_CODE(bref.pack(aperiodic_trs_present, 1));

  if (tci_state_pdsch_present) {
    HANDLE_CODE(bref.pack(tci_state_pdsch.max_num_cfg_tci_states_per_cc_present, 1));
    HANDLE_CODE(bref.pack(tci_state_pdsch.max_num_active_tci_per_bwp_present, 1));
    if (tci_state_pdsch.max_num_cfg_tci_states_per_cc_present) {
      HANDLE_CODE(tci_state_pdsch.max_num_cfg_tci_states_per_cc.pack(bref));
    }
    if (tci_state_pdsch.max_num_active_tci_per_bwp_present) {
      HANDLE_CODE(tci_state_pdsch.max_num_active_tci_per_bwp.pack(bref));
    }
  }
  if (pusch_trans_coherence_present) {
    HANDLE_CODE(pusch_trans_coherence.pack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.pack(bref));
  }
  if (max_num_rx_beam_present) {
    HANDLE_CODE(pack_integer(bref, max_num_rx_beam, (uint8_t)2u, (uint8_t)8u));
  }
  if (max_num_rx_tx_beam_switch_dl_present) {
    HANDLE_CODE(bref.pack(max_num_rx_tx_beam_switch_dl.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(max_num_rx_tx_beam_switch_dl.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(max_num_rx_tx_beam_switch_dl.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(max_num_rx_tx_beam_switch_dl.scs_120k_hz_present, 1));
    HANDLE_CODE(bref.pack(max_num_rx_tx_beam_switch_dl.scs_240k_hz_present, 1));
    if (max_num_rx_tx_beam_switch_dl.scs_15k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_15k_hz.pack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_30k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_30k_hz.pack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_60k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_60k_hz.pack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_120k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_120k_hz.pack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_240k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_240k_hz.pack(bref));
    }
  }
  if (max_num_non_group_beam_report_present) {
    HANDLE_CODE(max_num_non_group_beam_report.pack(bref));
  }
  if (ul_beam_management_present) {
    HANDLE_CODE(ul_beam_management.max_num_srs_res_per_set_bm.pack(bref));
    HANDLE_CODE(pack_integer(bref, ul_beam_management.max_num_srs_res_set, (uint8_t)1u, (uint8_t)8u));
  }
  if (max_num_csi_rs_bfd_present) {
    HANDLE_CODE(pack_integer(bref, max_num_csi_rs_bfd, (uint8_t)1u, (uint8_t)64u));
  }
  if (max_num_ssb_bfd_present) {
    HANDLE_CODE(pack_integer(bref, max_num_ssb_bfd, (uint8_t)1u, (uint8_t)64u));
  }
  if (max_num_csi_rs_ssb_cbd_present) {
    HANDLE_CODE(pack_integer(bref, max_num_csi_rs_ssb_cbd, (uint16_t)1u, (uint16_t)256u));
  }
  if (dummy5_present) {
    HANDLE_CODE(dummy5.pack(bref));
  }
  if (dummy3_present) {
    HANDLE_CODE(pack_integer(bref, dummy3, (uint8_t)1u, (uint8_t)4u));
  }
  if (beam_report_timing_present) {
    HANDLE_CODE(bref.pack(beam_report_timing.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(beam_report_timing.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(beam_report_timing.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(beam_report_timing.scs_120k_hz_present, 1));
    if (beam_report_timing.scs_15k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_15k_hz.pack(bref));
    }
    if (beam_report_timing.scs_30k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_30k_hz.pack(bref));
    }
    if (beam_report_timing.scs_60k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_60k_hz.pack(bref));
    }
    if (beam_report_timing.scs_120k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_120k_hz.pack(bref));
    }
  }
  if (ptrs_density_recommendation_set_dl_present) {
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_dl.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_dl.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_dl.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_dl.scs_120k_hz_present, 1));
    if (ptrs_density_recommendation_set_dl.scs_15k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_15k_hz.pack(bref));
    }
    if (ptrs_density_recommendation_set_dl.scs_30k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_30k_hz.pack(bref));
    }
    if (ptrs_density_recommendation_set_dl.scs_60k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_60k_hz.pack(bref));
    }
    if (ptrs_density_recommendation_set_dl.scs_120k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_120k_hz.pack(bref));
    }
  }
  if (ptrs_density_recommendation_set_ul_present) {
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_ul.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_ul.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_ul.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(ptrs_density_recommendation_set_ul.scs_120k_hz_present, 1));
    if (ptrs_density_recommendation_set_ul.scs_15k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_15k_hz.pack(bref));
    }
    if (ptrs_density_recommendation_set_ul.scs_30k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_30k_hz.pack(bref));
    }
    if (ptrs_density_recommendation_set_ul.scs_60k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_60k_hz.pack(bref));
    }
    if (ptrs_density_recommendation_set_ul.scs_120k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_120k_hz.pack(bref));
    }
  }
  if (dummy4_present) {
    HANDLE_CODE(dummy4.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dummy6_present;
    group_flags[0] |= beam_management_ssb_csi_rs.is_present();
    group_flags[0] |= beam_switch_timing.is_present();
    group_flags[0] |= codebook_params.is_present();
    group_flags[0] |= csi_rs_im_reception_for_feedback.is_present();
    group_flags[0] |= csi_rs_proc_framework_for_srs.is_present();
    group_flags[0] |= csi_report_framework.is_present();
    group_flags[0] |= csi_rs_for_tracking.is_present();
    group_flags[0] |= srs_assoc_csi_rs.is_present();
    group_flags[0] |= spatial_relations.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dummy6_present, 1));
      HANDLE_CODE(bref.pack(beam_management_ssb_csi_rs.is_present(), 1));
      HANDLE_CODE(bref.pack(beam_switch_timing.is_present(), 1));
      HANDLE_CODE(bref.pack(codebook_params.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_im_reception_for_feedback.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_proc_framework_for_srs.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_report_framework.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_for_tracking.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_assoc_csi_rs.is_present(), 1));
      HANDLE_CODE(bref.pack(spatial_relations.is_present(), 1));
      if (beam_management_ssb_csi_rs.is_present()) {
        HANDLE_CODE(beam_management_ssb_csi_rs->pack(bref));
      }
      if (beam_switch_timing.is_present()) {
        HANDLE_CODE(bref.pack(beam_switch_timing->scs_60k_hz_present, 1));
        HANDLE_CODE(bref.pack(beam_switch_timing->scs_120k_hz_present, 1));
        if (beam_switch_timing->scs_60k_hz_present) {
          HANDLE_CODE(beam_switch_timing->scs_60k_hz.pack(bref));
        }
        if (beam_switch_timing->scs_120k_hz_present) {
          HANDLE_CODE(beam_switch_timing->scs_120k_hz.pack(bref));
        }
      }
      if (codebook_params.is_present()) {
        HANDLE_CODE(codebook_params->pack(bref));
      }
      if (csi_rs_im_reception_for_feedback.is_present()) {
        HANDLE_CODE(csi_rs_im_reception_for_feedback->pack(bref));
      }
      if (csi_rs_proc_framework_for_srs.is_present()) {
        HANDLE_CODE(csi_rs_proc_framework_for_srs->pack(bref));
      }
      if (csi_report_framework.is_present()) {
        HANDLE_CODE(csi_report_framework->pack(bref));
      }
      if (csi_rs_for_tracking.is_present()) {
        HANDLE_CODE(csi_rs_for_tracking->pack(bref));
      }
      if (srs_assoc_csi_rs.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_assoc_csi_rs, 1, 7));
      }
      if (spatial_relations.is_present()) {
        HANDLE_CODE(spatial_relations->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_params_per_band_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tci_state_pdsch_present, 1));
  HANDLE_CODE(bref.unpack(add_active_tci_state_pdcch_present, 1));
  HANDLE_CODE(bref.unpack(pusch_trans_coherence_present, 1));
  HANDLE_CODE(bref.unpack(beam_correspondence_without_ul_beam_sweeping_present, 1));
  HANDLE_CODE(bref.unpack(periodic_beam_report_present, 1));
  HANDLE_CODE(bref.unpack(aperiodic_beam_report_present, 1));
  HANDLE_CODE(bref.unpack(sp_beam_report_pucch_present, 1));
  HANDLE_CODE(bref.unpack(sp_beam_report_pusch_present, 1));
  HANDLE_CODE(bref.unpack(dummy1_present, 1));
  HANDLE_CODE(bref.unpack(max_num_rx_beam_present, 1));
  HANDLE_CODE(bref.unpack(max_num_rx_tx_beam_switch_dl_present, 1));
  HANDLE_CODE(bref.unpack(max_num_non_group_beam_report_present, 1));
  HANDLE_CODE(bref.unpack(group_beam_report_present, 1));
  HANDLE_CODE(bref.unpack(ul_beam_management_present, 1));
  HANDLE_CODE(bref.unpack(max_num_csi_rs_bfd_present, 1));
  HANDLE_CODE(bref.unpack(max_num_ssb_bfd_present, 1));
  HANDLE_CODE(bref.unpack(max_num_csi_rs_ssb_cbd_present, 1));
  HANDLE_CODE(bref.unpack(dummy2_present, 1));
  HANDLE_CODE(bref.unpack(two_ports_ptrs_ul_present, 1));
  HANDLE_CODE(bref.unpack(dummy5_present, 1));
  HANDLE_CODE(bref.unpack(dummy3_present, 1));
  HANDLE_CODE(bref.unpack(beam_report_timing_present, 1));
  HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_dl_present, 1));
  HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_ul_present, 1));
  HANDLE_CODE(bref.unpack(dummy4_present, 1));
  HANDLE_CODE(bref.unpack(aperiodic_trs_present, 1));

  if (tci_state_pdsch_present) {
    HANDLE_CODE(bref.unpack(tci_state_pdsch.max_num_cfg_tci_states_per_cc_present, 1));
    HANDLE_CODE(bref.unpack(tci_state_pdsch.max_num_active_tci_per_bwp_present, 1));
    if (tci_state_pdsch.max_num_cfg_tci_states_per_cc_present) {
      HANDLE_CODE(tci_state_pdsch.max_num_cfg_tci_states_per_cc.unpack(bref));
    }
    if (tci_state_pdsch.max_num_active_tci_per_bwp_present) {
      HANDLE_CODE(tci_state_pdsch.max_num_active_tci_per_bwp.unpack(bref));
    }
  }
  if (pusch_trans_coherence_present) {
    HANDLE_CODE(pusch_trans_coherence.unpack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.unpack(bref));
  }
  if (max_num_rx_beam_present) {
    HANDLE_CODE(unpack_integer(max_num_rx_beam, bref, (uint8_t)2u, (uint8_t)8u));
  }
  if (max_num_rx_tx_beam_switch_dl_present) {
    HANDLE_CODE(bref.unpack(max_num_rx_tx_beam_switch_dl.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(max_num_rx_tx_beam_switch_dl.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(max_num_rx_tx_beam_switch_dl.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(max_num_rx_tx_beam_switch_dl.scs_120k_hz_present, 1));
    HANDLE_CODE(bref.unpack(max_num_rx_tx_beam_switch_dl.scs_240k_hz_present, 1));
    if (max_num_rx_tx_beam_switch_dl.scs_15k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_15k_hz.unpack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_30k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_30k_hz.unpack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_60k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_60k_hz.unpack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_120k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_120k_hz.unpack(bref));
    }
    if (max_num_rx_tx_beam_switch_dl.scs_240k_hz_present) {
      HANDLE_CODE(max_num_rx_tx_beam_switch_dl.scs_240k_hz.unpack(bref));
    }
  }
  if (max_num_non_group_beam_report_present) {
    HANDLE_CODE(max_num_non_group_beam_report.unpack(bref));
  }
  if (ul_beam_management_present) {
    HANDLE_CODE(ul_beam_management.max_num_srs_res_per_set_bm.unpack(bref));
    HANDLE_CODE(unpack_integer(ul_beam_management.max_num_srs_res_set, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (max_num_csi_rs_bfd_present) {
    HANDLE_CODE(unpack_integer(max_num_csi_rs_bfd, bref, (uint8_t)1u, (uint8_t)64u));
  }
  if (max_num_ssb_bfd_present) {
    HANDLE_CODE(unpack_integer(max_num_ssb_bfd, bref, (uint8_t)1u, (uint8_t)64u));
  }
  if (max_num_csi_rs_ssb_cbd_present) {
    HANDLE_CODE(unpack_integer(max_num_csi_rs_ssb_cbd, bref, (uint16_t)1u, (uint16_t)256u));
  }
  if (dummy5_present) {
    HANDLE_CODE(dummy5.unpack(bref));
  }
  if (dummy3_present) {
    HANDLE_CODE(unpack_integer(dummy3, bref, (uint8_t)1u, (uint8_t)4u));
  }
  if (beam_report_timing_present) {
    HANDLE_CODE(bref.unpack(beam_report_timing.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(beam_report_timing.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(beam_report_timing.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(beam_report_timing.scs_120k_hz_present, 1));
    if (beam_report_timing.scs_15k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_15k_hz.unpack(bref));
    }
    if (beam_report_timing.scs_30k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_30k_hz.unpack(bref));
    }
    if (beam_report_timing.scs_60k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_60k_hz.unpack(bref));
    }
    if (beam_report_timing.scs_120k_hz_present) {
      HANDLE_CODE(beam_report_timing.scs_120k_hz.unpack(bref));
    }
  }
  if (ptrs_density_recommendation_set_dl_present) {
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_dl.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_dl.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_dl.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_dl.scs_120k_hz_present, 1));
    if (ptrs_density_recommendation_set_dl.scs_15k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_15k_hz.unpack(bref));
    }
    if (ptrs_density_recommendation_set_dl.scs_30k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_30k_hz.unpack(bref));
    }
    if (ptrs_density_recommendation_set_dl.scs_60k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_60k_hz.unpack(bref));
    }
    if (ptrs_density_recommendation_set_dl.scs_120k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_dl.scs_120k_hz.unpack(bref));
    }
  }
  if (ptrs_density_recommendation_set_ul_present) {
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_ul.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_ul.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_ul.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(ptrs_density_recommendation_set_ul.scs_120k_hz_present, 1));
    if (ptrs_density_recommendation_set_ul.scs_15k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_15k_hz.unpack(bref));
    }
    if (ptrs_density_recommendation_set_ul.scs_30k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_30k_hz.unpack(bref));
    }
    if (ptrs_density_recommendation_set_ul.scs_60k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_60k_hz.unpack(bref));
    }
    if (ptrs_density_recommendation_set_ul.scs_120k_hz_present) {
      HANDLE_CODE(ptrs_density_recommendation_set_ul.scs_120k_hz.unpack(bref));
    }
  }
  if (dummy4_present) {
    HANDLE_CODE(dummy4.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dummy6_present, 1));
      bool beam_management_ssb_csi_rs_present;
      HANDLE_CODE(bref.unpack(beam_management_ssb_csi_rs_present, 1));
      beam_management_ssb_csi_rs.set_present(beam_management_ssb_csi_rs_present);
      bool beam_switch_timing_present;
      HANDLE_CODE(bref.unpack(beam_switch_timing_present, 1));
      beam_switch_timing.set_present(beam_switch_timing_present);
      bool codebook_params_present;
      HANDLE_CODE(bref.unpack(codebook_params_present, 1));
      codebook_params.set_present(codebook_params_present);
      bool csi_rs_im_reception_for_feedback_present;
      HANDLE_CODE(bref.unpack(csi_rs_im_reception_for_feedback_present, 1));
      csi_rs_im_reception_for_feedback.set_present(csi_rs_im_reception_for_feedback_present);
      bool csi_rs_proc_framework_for_srs_present;
      HANDLE_CODE(bref.unpack(csi_rs_proc_framework_for_srs_present, 1));
      csi_rs_proc_framework_for_srs.set_present(csi_rs_proc_framework_for_srs_present);
      bool csi_report_framework_present;
      HANDLE_CODE(bref.unpack(csi_report_framework_present, 1));
      csi_report_framework.set_present(csi_report_framework_present);
      bool csi_rs_for_tracking_present;
      HANDLE_CODE(bref.unpack(csi_rs_for_tracking_present, 1));
      csi_rs_for_tracking.set_present(csi_rs_for_tracking_present);
      bool srs_assoc_csi_rs_present;
      HANDLE_CODE(bref.unpack(srs_assoc_csi_rs_present, 1));
      srs_assoc_csi_rs.set_present(srs_assoc_csi_rs_present);
      bool spatial_relations_present;
      HANDLE_CODE(bref.unpack(spatial_relations_present, 1));
      spatial_relations.set_present(spatial_relations_present);
      if (beam_management_ssb_csi_rs.is_present()) {
        HANDLE_CODE(beam_management_ssb_csi_rs->unpack(bref));
      }
      if (beam_switch_timing.is_present()) {
        HANDLE_CODE(bref.unpack(beam_switch_timing->scs_60k_hz_present, 1));
        HANDLE_CODE(bref.unpack(beam_switch_timing->scs_120k_hz_present, 1));
        if (beam_switch_timing->scs_60k_hz_present) {
          HANDLE_CODE(beam_switch_timing->scs_60k_hz.unpack(bref));
        }
        if (beam_switch_timing->scs_120k_hz_present) {
          HANDLE_CODE(beam_switch_timing->scs_120k_hz.unpack(bref));
        }
      }
      if (codebook_params.is_present()) {
        HANDLE_CODE(codebook_params->unpack(bref));
      }
      if (csi_rs_im_reception_for_feedback.is_present()) {
        HANDLE_CODE(csi_rs_im_reception_for_feedback->unpack(bref));
      }
      if (csi_rs_proc_framework_for_srs.is_present()) {
        HANDLE_CODE(csi_rs_proc_framework_for_srs->unpack(bref));
      }
      if (csi_report_framework.is_present()) {
        HANDLE_CODE(csi_report_framework->unpack(bref));
      }
      if (csi_rs_for_tracking.is_present()) {
        HANDLE_CODE(csi_rs_for_tracking->unpack(bref));
      }
      if (srs_assoc_csi_rs.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_assoc_csi_rs, bref, 1, 7));
      }
      if (spatial_relations.is_present()) {
        HANDLE_CODE(spatial_relations->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mimo_params_per_band_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tci_state_pdsch_present) {
    j.write_fieldname("tci-StatePDSCH");
    j.start_obj();
    if (tci_state_pdsch.max_num_cfg_tci_states_per_cc_present) {
      j.write_str("maxNumberConfiguredTCIstatesPerCC", tci_state_pdsch.max_num_cfg_tci_states_per_cc.to_string());
    }
    if (tci_state_pdsch.max_num_active_tci_per_bwp_present) {
      j.write_str("maxNumberActiveTCI-PerBWP", tci_state_pdsch.max_num_active_tci_per_bwp.to_string());
    }
    j.end_obj();
  }
  if (add_active_tci_state_pdcch_present) {
    j.write_str("additionalActiveTCI-StatePDCCH", "supported");
  }
  if (pusch_trans_coherence_present) {
    j.write_str("pusch-TransCoherence", pusch_trans_coherence.to_string());
  }
  if (beam_correspondence_without_ul_beam_sweeping_present) {
    j.write_str("beamCorrespondenceWithoutUL-BeamSweeping", "supported");
  }
  if (periodic_beam_report_present) {
    j.write_str("periodicBeamReport", "supported");
  }
  if (aperiodic_beam_report_present) {
    j.write_str("aperiodicBeamReport", "supported");
  }
  if (sp_beam_report_pucch_present) {
    j.write_str("sp-BeamReportPUCCH", "supported");
  }
  if (sp_beam_report_pusch_present) {
    j.write_str("sp-BeamReportPUSCH", "supported");
  }
  if (dummy1_present) {
    j.write_fieldname("dummy1");
    dummy1.to_json(j);
  }
  if (max_num_rx_beam_present) {
    j.write_int("maxNumberRxBeam", max_num_rx_beam);
  }
  if (max_num_rx_tx_beam_switch_dl_present) {
    j.write_fieldname("maxNumberRxTxBeamSwitchDL");
    j.start_obj();
    if (max_num_rx_tx_beam_switch_dl.scs_15k_hz_present) {
      j.write_str("scs-15kHz", max_num_rx_tx_beam_switch_dl.scs_15k_hz.to_string());
    }
    if (max_num_rx_tx_beam_switch_dl.scs_30k_hz_present) {
      j.write_str("scs-30kHz", max_num_rx_tx_beam_switch_dl.scs_30k_hz.to_string());
    }
    if (max_num_rx_tx_beam_switch_dl.scs_60k_hz_present) {
      j.write_str("scs-60kHz", max_num_rx_tx_beam_switch_dl.scs_60k_hz.to_string());
    }
    if (max_num_rx_tx_beam_switch_dl.scs_120k_hz_present) {
      j.write_str("scs-120kHz", max_num_rx_tx_beam_switch_dl.scs_120k_hz.to_string());
    }
    if (max_num_rx_tx_beam_switch_dl.scs_240k_hz_present) {
      j.write_str("scs-240kHz", max_num_rx_tx_beam_switch_dl.scs_240k_hz.to_string());
    }
    j.end_obj();
  }
  if (max_num_non_group_beam_report_present) {
    j.write_str("maxNumberNonGroupBeamReporting", max_num_non_group_beam_report.to_string());
  }
  if (group_beam_report_present) {
    j.write_str("groupBeamReporting", "supported");
  }
  if (ul_beam_management_present) {
    j.write_fieldname("uplinkBeamManagement");
    j.start_obj();
    j.write_str("maxNumberSRS-ResourcePerSet-BM", ul_beam_management.max_num_srs_res_per_set_bm.to_string());
    j.write_int("maxNumberSRS-ResourceSet", ul_beam_management.max_num_srs_res_set);
    j.end_obj();
  }
  if (max_num_csi_rs_bfd_present) {
    j.write_int("maxNumberCSI-RS-BFD", max_num_csi_rs_bfd);
  }
  if (max_num_ssb_bfd_present) {
    j.write_int("maxNumberSSB-BFD", max_num_ssb_bfd);
  }
  if (max_num_csi_rs_ssb_cbd_present) {
    j.write_int("maxNumberCSI-RS-SSB-CBD", max_num_csi_rs_ssb_cbd);
  }
  if (dummy2_present) {
    j.write_str("dummy2", "supported");
  }
  if (two_ports_ptrs_ul_present) {
    j.write_str("twoPortsPTRS-UL", "supported");
  }
  if (dummy5_present) {
    j.write_fieldname("dummy5");
    dummy5.to_json(j);
  }
  if (dummy3_present) {
    j.write_int("dummy3", dummy3);
  }
  if (beam_report_timing_present) {
    j.write_fieldname("beamReportTiming");
    j.start_obj();
    if (beam_report_timing.scs_15k_hz_present) {
      j.write_str("scs-15kHz", beam_report_timing.scs_15k_hz.to_string());
    }
    if (beam_report_timing.scs_30k_hz_present) {
      j.write_str("scs-30kHz", beam_report_timing.scs_30k_hz.to_string());
    }
    if (beam_report_timing.scs_60k_hz_present) {
      j.write_str("scs-60kHz", beam_report_timing.scs_60k_hz.to_string());
    }
    if (beam_report_timing.scs_120k_hz_present) {
      j.write_str("scs-120kHz", beam_report_timing.scs_120k_hz.to_string());
    }
    j.end_obj();
  }
  if (ptrs_density_recommendation_set_dl_present) {
    j.write_fieldname("ptrs-DensityRecommendationSetDL");
    j.start_obj();
    if (ptrs_density_recommendation_set_dl.scs_15k_hz_present) {
      j.write_fieldname("scs-15kHz");
      ptrs_density_recommendation_set_dl.scs_15k_hz.to_json(j);
    }
    if (ptrs_density_recommendation_set_dl.scs_30k_hz_present) {
      j.write_fieldname("scs-30kHz");
      ptrs_density_recommendation_set_dl.scs_30k_hz.to_json(j);
    }
    if (ptrs_density_recommendation_set_dl.scs_60k_hz_present) {
      j.write_fieldname("scs-60kHz");
      ptrs_density_recommendation_set_dl.scs_60k_hz.to_json(j);
    }
    if (ptrs_density_recommendation_set_dl.scs_120k_hz_present) {
      j.write_fieldname("scs-120kHz");
      ptrs_density_recommendation_set_dl.scs_120k_hz.to_json(j);
    }
    j.end_obj();
  }
  if (ptrs_density_recommendation_set_ul_present) {
    j.write_fieldname("ptrs-DensityRecommendationSetUL");
    j.start_obj();
    if (ptrs_density_recommendation_set_ul.scs_15k_hz_present) {
      j.write_fieldname("scs-15kHz");
      ptrs_density_recommendation_set_ul.scs_15k_hz.to_json(j);
    }
    if (ptrs_density_recommendation_set_ul.scs_30k_hz_present) {
      j.write_fieldname("scs-30kHz");
      ptrs_density_recommendation_set_ul.scs_30k_hz.to_json(j);
    }
    if (ptrs_density_recommendation_set_ul.scs_60k_hz_present) {
      j.write_fieldname("scs-60kHz");
      ptrs_density_recommendation_set_ul.scs_60k_hz.to_json(j);
    }
    if (ptrs_density_recommendation_set_ul.scs_120k_hz_present) {
      j.write_fieldname("scs-120kHz");
      ptrs_density_recommendation_set_ul.scs_120k_hz.to_json(j);
    }
    j.end_obj();
  }
  if (dummy4_present) {
    j.write_fieldname("dummy4");
    dummy4.to_json(j);
  }
  if (aperiodic_trs_present) {
    j.write_str("aperiodicTRS", "supported");
  }
  if (ext) {
    if (dummy6_present) {
      j.write_str("dummy6", "true");
    }
    if (beam_management_ssb_csi_rs.is_present()) {
      j.write_fieldname("beamManagementSSB-CSI-RS");
      beam_management_ssb_csi_rs->to_json(j);
    }
    if (beam_switch_timing.is_present()) {
      j.write_fieldname("beamSwitchTiming");
      j.start_obj();
      if (beam_switch_timing->scs_60k_hz_present) {
        j.write_str("scs-60kHz", beam_switch_timing->scs_60k_hz.to_string());
      }
      if (beam_switch_timing->scs_120k_hz_present) {
        j.write_str("scs-120kHz", beam_switch_timing->scs_120k_hz.to_string());
      }
      j.end_obj();
    }
    if (codebook_params.is_present()) {
      j.write_fieldname("codebookParameters");
      codebook_params->to_json(j);
    }
    if (csi_rs_im_reception_for_feedback.is_present()) {
      j.write_fieldname("csi-RS-IM-ReceptionForFeedback");
      csi_rs_im_reception_for_feedback->to_json(j);
    }
    if (csi_rs_proc_framework_for_srs.is_present()) {
      j.write_fieldname("csi-RS-ProcFrameworkForSRS");
      csi_rs_proc_framework_for_srs->to_json(j);
    }
    if (csi_report_framework.is_present()) {
      j.write_fieldname("csi-ReportFramework");
      csi_report_framework->to_json(j);
    }
    if (csi_rs_for_tracking.is_present()) {
      j.write_fieldname("csi-RS-ForTracking");
      csi_rs_for_tracking->to_json(j);
    }
    if (srs_assoc_csi_rs.is_present()) {
      j.start_array("srs-AssocCSI-RS");
      for (const auto& e1 : *srs_assoc_csi_rs) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (spatial_relations.is_present()) {
      j.write_fieldname("spatialRelations");
      spatial_relations->to_json(j);
    }
  }
  j.end_obj();
}

const char* mimo_params_per_band_s::tci_state_pdsch_s_::max_num_cfg_tci_states_per_cc_opts::to_string() const
{
  static const char* names[] = {"n4", "n8", "n16", "n32", "n64", "n128"};
  return convert_enum_idx(
      names, 6, value, "mimo_params_per_band_s::tci_state_pdsch_s_::max_num_cfg_tci_states_per_cc_e_");
}
uint8_t mimo_params_per_band_s::tci_state_pdsch_s_::max_num_cfg_tci_states_per_cc_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16, 32, 64, 128};
  return map_enum_number(
      numbers, 6, value, "mimo_params_per_band_s::tci_state_pdsch_s_::max_num_cfg_tci_states_per_cc_e_");
}

const char* mimo_params_per_band_s::tci_state_pdsch_s_::max_num_active_tci_per_bwp_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(names, 4, value, "mimo_params_per_band_s::tci_state_pdsch_s_::max_num_active_tci_per_bwp_e_");
}
uint8_t mimo_params_per_band_s::tci_state_pdsch_s_::max_num_active_tci_per_bwp_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(
      numbers, 4, value, "mimo_params_per_band_s::tci_state_pdsch_s_::max_num_active_tci_per_bwp_e_");
}

const char* mimo_params_per_band_s::pusch_trans_coherence_opts::to_string() const
{
  static const char* names[] = {"nonCoherent", "partialCoherent", "fullCoherent"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::pusch_trans_coherence_e_");
}

const char* mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_15k_hz_opts::to_string() const
{
  static const char* names[] = {"n4", "n7", "n14"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_15k_hz_e_");
}
uint8_t mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_15k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 7, 14};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_15k_hz_e_");
}

const char* mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_30k_hz_opts::to_string() const
{
  static const char* names[] = {"n4", "n7", "n14"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_30k_hz_e_");
}
uint8_t mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_30k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 7, 14};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_30k_hz_e_");
}

const char* mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_60k_hz_opts::to_string() const
{
  static const char* names[] = {"n4", "n7", "n14"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_60k_hz_e_");
}
uint8_t mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_60k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 7, 14};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_60k_hz_e_");
}

const char* mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_120k_hz_opts::to_string() const
{
  static const char* names[] = {"n4", "n7", "n14"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_120k_hz_e_");
}
uint8_t mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_120k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 7, 14};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_120k_hz_e_");
}

const char* mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_240k_hz_opts::to_string() const
{
  static const char* names[] = {"n4", "n7", "n14"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_240k_hz_e_");
}
uint8_t mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_240k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 7, 14};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::max_num_rx_tx_beam_switch_dl_s_::scs_240k_hz_e_");
}

const char* mimo_params_per_band_s::max_num_non_group_beam_report_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::max_num_non_group_beam_report_e_");
}
uint8_t mimo_params_per_band_s::max_num_non_group_beam_report_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::max_num_non_group_beam_report_e_");
}

const char* mimo_params_per_band_s::ul_beam_management_s_::max_num_srs_res_per_set_bm_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8", "n16"};
  return convert_enum_idx(
      names, 4, value, "mimo_params_per_band_s::ul_beam_management_s_::max_num_srs_res_per_set_bm_e_");
}
uint8_t mimo_params_per_band_s::ul_beam_management_s_::max_num_srs_res_per_set_bm_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 16};
  return map_enum_number(
      numbers, 4, value, "mimo_params_per_band_s::ul_beam_management_s_::max_num_srs_res_per_set_bm_e_");
}

const char* mimo_params_per_band_s::beam_report_timing_s_::scs_15k_hz_opts::to_string() const
{
  static const char* names[] = {"sym2", "sym4", "sym8"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_15k_hz_e_");
}
uint8_t mimo_params_per_band_s::beam_report_timing_s_::scs_15k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_15k_hz_e_");
}

const char* mimo_params_per_band_s::beam_report_timing_s_::scs_30k_hz_opts::to_string() const
{
  static const char* names[] = {"sym4", "sym8", "sym14", "sym28"};
  return convert_enum_idx(names, 4, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_30k_hz_e_");
}
uint8_t mimo_params_per_band_s::beam_report_timing_s_::scs_30k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 14, 28};
  return map_enum_number(numbers, 4, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_30k_hz_e_");
}

const char* mimo_params_per_band_s::beam_report_timing_s_::scs_60k_hz_opts::to_string() const
{
  static const char* names[] = {"sym8", "sym14", "sym28"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_60k_hz_e_");
}
uint8_t mimo_params_per_band_s::beam_report_timing_s_::scs_60k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 14, 28};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_60k_hz_e_");
}

const char* mimo_params_per_band_s::beam_report_timing_s_::scs_120k_hz_opts::to_string() const
{
  static const char* names[] = {"sym14", "sym28", "sym56"};
  return convert_enum_idx(names, 3, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_120k_hz_e_");
}
uint8_t mimo_params_per_band_s::beam_report_timing_s_::scs_120k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {14, 28, 56};
  return map_enum_number(numbers, 3, value, "mimo_params_per_band_s::beam_report_timing_s_::scs_120k_hz_e_");
}

const char* mimo_params_per_band_s::beam_switch_timing_s_::scs_60k_hz_opts::to_string() const
{
  static const char* names[] = {"sym14", "sym28", "sym48", "sym224", "sym336"};
  return convert_enum_idx(names, 5, value, "mimo_params_per_band_s::beam_switch_timing_s_::scs_60k_hz_e_");
}
uint16_t mimo_params_per_band_s::beam_switch_timing_s_::scs_60k_hz_opts::to_number() const
{
  static const uint16_t numbers[] = {14, 28, 48, 224, 336};
  return map_enum_number(numbers, 5, value, "mimo_params_per_band_s::beam_switch_timing_s_::scs_60k_hz_e_");
}

const char* mimo_params_per_band_s::beam_switch_timing_s_::scs_120k_hz_opts::to_string() const
{
  static const char* names[] = {"sym14", "sym28", "sym48", "sym224", "sym336"};
  return convert_enum_idx(names, 5, value, "mimo_params_per_band_s::beam_switch_timing_s_::scs_120k_hz_e_");
}
uint16_t mimo_params_per_band_s::beam_switch_timing_s_::scs_120k_hz_opts::to_number() const
{
  static const uint16_t numbers[] = {14, 28, 48, 224, 336};
  return map_enum_number(numbers, 5, value, "mimo_params_per_band_s::beam_switch_timing_s_::scs_120k_hz_e_");
}

// BandNR ::= SEQUENCE
SRSASN_CODE band_nr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(modified_mpr_behaviour_present, 1));
  HANDLE_CODE(bref.pack(mimo_params_per_band_present, 1));
  HANDLE_CODE(bref.pack(extended_cp_present, 1));
  HANDLE_CODE(bref.pack(multiple_tci_present, 1));
  HANDLE_CODE(bref.pack(bwp_without_restrict_present, 1));
  HANDLE_CODE(bref.pack(bwp_same_numerology_present, 1));
  HANDLE_CODE(bref.pack(bwp_diff_numerology_present, 1));
  HANDLE_CODE(bref.pack(cross_carrier_sched_same_scs_present, 1));
  HANDLE_CODE(bref.pack(pdsch_256_qam_fr2_present, 1));
  HANDLE_CODE(bref.pack(pusch_256_qam_present, 1));
  HANDLE_CODE(bref.pack(ue_pwr_class_present, 1));
  HANDLE_CODE(bref.pack(rate_matching_lte_crs_present, 1));
  HANDLE_CODE(bref.pack(ch_bws_dl_present, 1));
  HANDLE_CODE(bref.pack(ch_bws_ul_present, 1));

  HANDLE_CODE(pack_integer(bref, band_nr, (uint16_t)1u, (uint16_t)1024u));
  if (modified_mpr_behaviour_present) {
    HANDLE_CODE(modified_mpr_behaviour.pack(bref));
  }
  if (mimo_params_per_band_present) {
    HANDLE_CODE(mimo_params_per_band.pack(bref));
  }
  if (bwp_same_numerology_present) {
    HANDLE_CODE(bwp_same_numerology.pack(bref));
  }
  if (ue_pwr_class_present) {
    HANDLE_CODE(ue_pwr_class.pack(bref));
  }
  if (ch_bws_dl_present) {
    HANDLE_CODE(ch_bws_dl.pack(bref));
  }
  if (ch_bws_ul_present) {
    HANDLE_CODE(ch_bws_ul.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= max_ul_duty_cycle_pc2_fr1_present;
    group_flags[1] |= pucch_spatial_rel_info_mac_ce_present;
    group_flags[1] |= pwr_boosting_pi2_bpsk_present;
    group_flags[2] |= max_ul_duty_cycle_fr2_present;
    group_flags[3] |= ch_bws_dl_v1590.is_present();
    group_flags[3] |= ch_bws_ul_v1590.is_present();
    group_flags[4] |= asymmetric_bw_combination_set_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_ul_duty_cycle_pc2_fr1_present, 1));
      if (max_ul_duty_cycle_pc2_fr1_present) {
        HANDLE_CODE(max_ul_duty_cycle_pc2_fr1.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pucch_spatial_rel_info_mac_ce_present, 1));
      HANDLE_CODE(bref.pack(pwr_boosting_pi2_bpsk_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_ul_duty_cycle_fr2_present, 1));
      if (max_ul_duty_cycle_fr2_present) {
        HANDLE_CODE(max_ul_duty_cycle_fr2.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ch_bws_dl_v1590.is_present(), 1));
      HANDLE_CODE(bref.pack(ch_bws_ul_v1590.is_present(), 1));
      if (ch_bws_dl_v1590.is_present()) {
        HANDLE_CODE(ch_bws_dl_v1590->pack(bref));
      }
      if (ch_bws_ul_v1590.is_present()) {
        HANDLE_CODE(ch_bws_ul_v1590->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(asymmetric_bw_combination_set_present, 1));
      if (asymmetric_bw_combination_set_present) {
        HANDLE_CODE(asymmetric_bw_combination_set.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_nr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(modified_mpr_behaviour_present, 1));
  HANDLE_CODE(bref.unpack(mimo_params_per_band_present, 1));
  HANDLE_CODE(bref.unpack(extended_cp_present, 1));
  HANDLE_CODE(bref.unpack(multiple_tci_present, 1));
  HANDLE_CODE(bref.unpack(bwp_without_restrict_present, 1));
  HANDLE_CODE(bref.unpack(bwp_same_numerology_present, 1));
  HANDLE_CODE(bref.unpack(bwp_diff_numerology_present, 1));
  HANDLE_CODE(bref.unpack(cross_carrier_sched_same_scs_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_256_qam_fr2_present, 1));
  HANDLE_CODE(bref.unpack(pusch_256_qam_present, 1));
  HANDLE_CODE(bref.unpack(ue_pwr_class_present, 1));
  HANDLE_CODE(bref.unpack(rate_matching_lte_crs_present, 1));
  HANDLE_CODE(bref.unpack(ch_bws_dl_present, 1));
  HANDLE_CODE(bref.unpack(ch_bws_ul_present, 1));

  HANDLE_CODE(unpack_integer(band_nr, bref, (uint16_t)1u, (uint16_t)1024u));
  if (modified_mpr_behaviour_present) {
    HANDLE_CODE(modified_mpr_behaviour.unpack(bref));
  }
  if (mimo_params_per_band_present) {
    HANDLE_CODE(mimo_params_per_band.unpack(bref));
  }
  if (bwp_same_numerology_present) {
    HANDLE_CODE(bwp_same_numerology.unpack(bref));
  }
  if (ue_pwr_class_present) {
    HANDLE_CODE(ue_pwr_class.unpack(bref));
  }
  if (ch_bws_dl_present) {
    HANDLE_CODE(ch_bws_dl.unpack(bref));
  }
  if (ch_bws_ul_present) {
    HANDLE_CODE(ch_bws_ul.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_ul_duty_cycle_pc2_fr1_present, 1));
      if (max_ul_duty_cycle_pc2_fr1_present) {
        HANDLE_CODE(max_ul_duty_cycle_pc2_fr1.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pucch_spatial_rel_info_mac_ce_present, 1));
      HANDLE_CODE(bref.unpack(pwr_boosting_pi2_bpsk_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_ul_duty_cycle_fr2_present, 1));
      if (max_ul_duty_cycle_fr2_present) {
        HANDLE_CODE(max_ul_duty_cycle_fr2.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ch_bws_dl_v1590_present;
      HANDLE_CODE(bref.unpack(ch_bws_dl_v1590_present, 1));
      ch_bws_dl_v1590.set_present(ch_bws_dl_v1590_present);
      bool ch_bws_ul_v1590_present;
      HANDLE_CODE(bref.unpack(ch_bws_ul_v1590_present, 1));
      ch_bws_ul_v1590.set_present(ch_bws_ul_v1590_present);
      if (ch_bws_dl_v1590.is_present()) {
        HANDLE_CODE(ch_bws_dl_v1590->unpack(bref));
      }
      if (ch_bws_ul_v1590.is_present()) {
        HANDLE_CODE(ch_bws_ul_v1590->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(asymmetric_bw_combination_set_present, 1));
      if (asymmetric_bw_combination_set_present) {
        HANDLE_CODE(asymmetric_bw_combination_set.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void band_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandNR", band_nr);
  if (modified_mpr_behaviour_present) {
    j.write_str("modifiedMPR-Behaviour", modified_mpr_behaviour.to_string());
  }
  if (mimo_params_per_band_present) {
    j.write_fieldname("mimo-ParametersPerBand");
    mimo_params_per_band.to_json(j);
  }
  if (extended_cp_present) {
    j.write_str("extendedCP", "supported");
  }
  if (multiple_tci_present) {
    j.write_str("multipleTCI", "supported");
  }
  if (bwp_without_restrict_present) {
    j.write_str("bwp-WithoutRestriction", "supported");
  }
  if (bwp_same_numerology_present) {
    j.write_str("bwp-SameNumerology", bwp_same_numerology.to_string());
  }
  if (bwp_diff_numerology_present) {
    j.write_str("bwp-DiffNumerology", "upto4");
  }
  if (cross_carrier_sched_same_scs_present) {
    j.write_str("crossCarrierScheduling-SameSCS", "supported");
  }
  if (pdsch_256_qam_fr2_present) {
    j.write_str("pdsch-256QAM-FR2", "supported");
  }
  if (pusch_256_qam_present) {
    j.write_str("pusch-256QAM", "supported");
  }
  if (ue_pwr_class_present) {
    j.write_str("ue-PowerClass", ue_pwr_class.to_string());
  }
  if (rate_matching_lte_crs_present) {
    j.write_str("rateMatchingLTE-CRS", "supported");
  }
  if (ch_bws_dl_present) {
    j.write_fieldname("channelBWs-DL");
    ch_bws_dl.to_json(j);
  }
  if (ch_bws_ul_present) {
    j.write_fieldname("channelBWs-UL");
    ch_bws_ul.to_json(j);
  }
  if (ext) {
    if (max_ul_duty_cycle_pc2_fr1_present) {
      j.write_str("maxUplinkDutyCycle-PC2-FR1", max_ul_duty_cycle_pc2_fr1.to_string());
    }
    if (pucch_spatial_rel_info_mac_ce_present) {
      j.write_str("pucch-SpatialRelInfoMAC-CE", "supported");
    }
    if (pwr_boosting_pi2_bpsk_present) {
      j.write_str("powerBoosting-pi2BPSK", "supported");
    }
    if (max_ul_duty_cycle_fr2_present) {
      j.write_str("maxUplinkDutyCycle-FR2", max_ul_duty_cycle_fr2.to_string());
    }
    if (ch_bws_dl_v1590.is_present()) {
      j.write_fieldname("channelBWs-DL-v1590");
      ch_bws_dl_v1590->to_json(j);
    }
    if (ch_bws_ul_v1590.is_present()) {
      j.write_fieldname("channelBWs-UL-v1590");
      ch_bws_ul_v1590->to_json(j);
    }
    if (asymmetric_bw_combination_set_present) {
      j.write_str("asymmetricBandwidthCombinationSet", asymmetric_bw_combination_set.to_string());
    }
  }
  j.end_obj();
}

const char* band_nr_s::bwp_same_numerology_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4"};
  return convert_enum_idx(names, 2, value, "band_nr_s::bwp_same_numerology_e_");
}
uint8_t band_nr_s::bwp_same_numerology_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "band_nr_s::bwp_same_numerology_e_");
}

const char* band_nr_s::ue_pwr_class_opts::to_string() const
{
  static const char* names[] = {"pc1", "pc2", "pc3", "pc4"};
  return convert_enum_idx(names, 4, value, "band_nr_s::ue_pwr_class_e_");
}
uint8_t band_nr_s::ue_pwr_class_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "band_nr_s::ue_pwr_class_e_");
}

void band_nr_s::ch_bws_dl_c_::destroy_()
{
  switch (type_) {
    case types::fr1:
      c.destroy<fr1_s_>();
      break;
    case types::fr2:
      c.destroy<fr2_s_>();
      break;
    default:
      break;
  }
}
void band_nr_s::ch_bws_dl_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::fr1:
      c.init<fr1_s_>();
      break;
    case types::fr2:
      c.init<fr2_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_c_");
  }
}
band_nr_s::ch_bws_dl_c_::ch_bws_dl_c_(const band_nr_s::ch_bws_dl_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fr1:
      c.init(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.init(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_c_");
  }
}
band_nr_s::ch_bws_dl_c_& band_nr_s::ch_bws_dl_c_::operator=(const band_nr_s::ch_bws_dl_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fr1:
      c.set(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.set(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_c_");
  }

  return *this;
}
band_nr_s::ch_bws_dl_c_::fr1_s_& band_nr_s::ch_bws_dl_c_::set_fr1()
{
  set(types::fr1);
  return c.get<fr1_s_>();
}
band_nr_s::ch_bws_dl_c_::fr2_s_& band_nr_s::ch_bws_dl_c_::set_fr2()
{
  set(types::fr2);
  return c.get<fr2_s_>();
}
void band_nr_s::ch_bws_dl_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fr1:
      j.write_fieldname("fr1");
      j.start_obj();
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        j.write_str("scs-15kHz", c.get<fr1_s_>().scs_15k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        j.write_str("scs-30kHz", c.get<fr1_s_>().scs_30k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr1_s_>().scs_60k_hz.to_string());
      }
      j.end_obj();
      break;
    case types::fr2:
      j.write_fieldname("fr2");
      j.start_obj();
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr2_s_>().scs_60k_hz.to_string());
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        j.write_str("scs-120kHz", c.get<fr2_s_>().scs_120k_hz.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_c_");
  }
  j.end_obj();
}
SRSASN_CODE band_nr_s::ch_bws_dl_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.pack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.pack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_nr_s::ch_bws_dl_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.unpack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.unpack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void band_nr_s::ch_bws_ul_c_::destroy_()
{
  switch (type_) {
    case types::fr1:
      c.destroy<fr1_s_>();
      break;
    case types::fr2:
      c.destroy<fr2_s_>();
      break;
    default:
      break;
  }
}
void band_nr_s::ch_bws_ul_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::fr1:
      c.init<fr1_s_>();
      break;
    case types::fr2:
      c.init<fr2_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_c_");
  }
}
band_nr_s::ch_bws_ul_c_::ch_bws_ul_c_(const band_nr_s::ch_bws_ul_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fr1:
      c.init(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.init(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_c_");
  }
}
band_nr_s::ch_bws_ul_c_& band_nr_s::ch_bws_ul_c_::operator=(const band_nr_s::ch_bws_ul_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fr1:
      c.set(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.set(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_c_");
  }

  return *this;
}
band_nr_s::ch_bws_ul_c_::fr1_s_& band_nr_s::ch_bws_ul_c_::set_fr1()
{
  set(types::fr1);
  return c.get<fr1_s_>();
}
band_nr_s::ch_bws_ul_c_::fr2_s_& band_nr_s::ch_bws_ul_c_::set_fr2()
{
  set(types::fr2);
  return c.get<fr2_s_>();
}
void band_nr_s::ch_bws_ul_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fr1:
      j.write_fieldname("fr1");
      j.start_obj();
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        j.write_str("scs-15kHz", c.get<fr1_s_>().scs_15k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        j.write_str("scs-30kHz", c.get<fr1_s_>().scs_30k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr1_s_>().scs_60k_hz.to_string());
      }
      j.end_obj();
      break;
    case types::fr2:
      j.write_fieldname("fr2");
      j.start_obj();
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr2_s_>().scs_60k_hz.to_string());
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        j.write_str("scs-120kHz", c.get<fr2_s_>().scs_120k_hz.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_c_");
  }
  j.end_obj();
}
SRSASN_CODE band_nr_s::ch_bws_ul_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.pack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.pack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_nr_s::ch_bws_ul_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.unpack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.unpack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* band_nr_s::max_ul_duty_cycle_pc2_fr1_opts::to_string() const
{
  static const char* names[] = {"n60", "n70", "n80", "n90", "n100"};
  return convert_enum_idx(names, 5, value, "band_nr_s::max_ul_duty_cycle_pc2_fr1_e_");
}
uint8_t band_nr_s::max_ul_duty_cycle_pc2_fr1_opts::to_number() const
{
  static const uint8_t numbers[] = {60, 70, 80, 90, 100};
  return map_enum_number(numbers, 5, value, "band_nr_s::max_ul_duty_cycle_pc2_fr1_e_");
}

const char* band_nr_s::max_ul_duty_cycle_fr2_opts::to_string() const
{
  static const char* names[] = {"n15", "n20", "n25", "n30", "n40", "n50", "n60", "n70", "n80", "n90", "n100"};
  return convert_enum_idx(names, 11, value, "band_nr_s::max_ul_duty_cycle_fr2_e_");
}
uint8_t band_nr_s::max_ul_duty_cycle_fr2_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100};
  return map_enum_number(numbers, 11, value, "band_nr_s::max_ul_duty_cycle_fr2_e_");
}

void band_nr_s::ch_bws_dl_v1590_c_::destroy_()
{
  switch (type_) {
    case types::fr1:
      c.destroy<fr1_s_>();
      break;
    case types::fr2:
      c.destroy<fr2_s_>();
      break;
    default:
      break;
  }
}
void band_nr_s::ch_bws_dl_v1590_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::fr1:
      c.init<fr1_s_>();
      break;
    case types::fr2:
      c.init<fr2_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_v1590_c_");
  }
}
band_nr_s::ch_bws_dl_v1590_c_::ch_bws_dl_v1590_c_(const band_nr_s::ch_bws_dl_v1590_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fr1:
      c.init(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.init(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_v1590_c_");
  }
}
band_nr_s::ch_bws_dl_v1590_c_& band_nr_s::ch_bws_dl_v1590_c_::operator=(const band_nr_s::ch_bws_dl_v1590_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fr1:
      c.set(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.set(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_v1590_c_");
  }

  return *this;
}
band_nr_s::ch_bws_dl_v1590_c_::fr1_s_& band_nr_s::ch_bws_dl_v1590_c_::set_fr1()
{
  set(types::fr1);
  return c.get<fr1_s_>();
}
band_nr_s::ch_bws_dl_v1590_c_::fr2_s_& band_nr_s::ch_bws_dl_v1590_c_::set_fr2()
{
  set(types::fr2);
  return c.get<fr2_s_>();
}
void band_nr_s::ch_bws_dl_v1590_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fr1:
      j.write_fieldname("fr1");
      j.start_obj();
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        j.write_str("scs-15kHz", c.get<fr1_s_>().scs_15k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        j.write_str("scs-30kHz", c.get<fr1_s_>().scs_30k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr1_s_>().scs_60k_hz.to_string());
      }
      j.end_obj();
      break;
    case types::fr2:
      j.write_fieldname("fr2");
      j.start_obj();
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr2_s_>().scs_60k_hz.to_string());
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        j.write_str("scs-120kHz", c.get<fr2_s_>().scs_120k_hz.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_v1590_c_");
  }
  j.end_obj();
}
SRSASN_CODE band_nr_s::ch_bws_dl_v1590_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.pack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.pack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_v1590_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_nr_s::ch_bws_dl_v1590_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.unpack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.unpack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_dl_v1590_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void band_nr_s::ch_bws_ul_v1590_c_::destroy_()
{
  switch (type_) {
    case types::fr1:
      c.destroy<fr1_s_>();
      break;
    case types::fr2:
      c.destroy<fr2_s_>();
      break;
    default:
      break;
  }
}
void band_nr_s::ch_bws_ul_v1590_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::fr1:
      c.init<fr1_s_>();
      break;
    case types::fr2:
      c.init<fr2_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_v1590_c_");
  }
}
band_nr_s::ch_bws_ul_v1590_c_::ch_bws_ul_v1590_c_(const band_nr_s::ch_bws_ul_v1590_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fr1:
      c.init(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.init(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_v1590_c_");
  }
}
band_nr_s::ch_bws_ul_v1590_c_& band_nr_s::ch_bws_ul_v1590_c_::operator=(const band_nr_s::ch_bws_ul_v1590_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fr1:
      c.set(other.c.get<fr1_s_>());
      break;
    case types::fr2:
      c.set(other.c.get<fr2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_v1590_c_");
  }

  return *this;
}
band_nr_s::ch_bws_ul_v1590_c_::fr1_s_& band_nr_s::ch_bws_ul_v1590_c_::set_fr1()
{
  set(types::fr1);
  return c.get<fr1_s_>();
}
band_nr_s::ch_bws_ul_v1590_c_::fr2_s_& band_nr_s::ch_bws_ul_v1590_c_::set_fr2()
{
  set(types::fr2);
  return c.get<fr2_s_>();
}
void band_nr_s::ch_bws_ul_v1590_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fr1:
      j.write_fieldname("fr1");
      j.start_obj();
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        j.write_str("scs-15kHz", c.get<fr1_s_>().scs_15k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        j.write_str("scs-30kHz", c.get<fr1_s_>().scs_30k_hz.to_string());
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr1_s_>().scs_60k_hz.to_string());
      }
      j.end_obj();
      break;
    case types::fr2:
      j.write_fieldname("fr2");
      j.start_obj();
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        j.write_str("scs-60kHz", c.get<fr2_s_>().scs_60k_hz.to_string());
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        j.write_str("scs-120kHz", c.get<fr2_s_>().scs_120k_hz.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_v1590_c_");
  }
  j.end_obj();
}
SRSASN_CODE band_nr_s::ch_bws_ul_v1590_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.pack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.pack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.pack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.pack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_v1590_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE band_nr_s::ch_bws_ul_v1590_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_15k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_30k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr1_s_>().scs_60k_hz_present, 1));
      if (c.get<fr1_s_>().scs_15k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_15k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_30k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_30k_hz.unpack(bref));
      }
      if (c.get<fr1_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr1_s_>().scs_60k_hz.unpack(bref));
      }
      break;
    case types::fr2:
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_60k_hz_present, 1));
      HANDLE_CODE(bref.unpack(c.get<fr2_s_>().scs_120k_hz_present, 1));
      if (c.get<fr2_s_>().scs_60k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_60k_hz.unpack(bref));
      }
      if (c.get<fr2_s_>().scs_120k_hz_present) {
        HANDLE_CODE(c.get<fr2_s_>().scs_120k_hz.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "band_nr_s::ch_bws_ul_v1590_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// EUTRA-ParametersCommon ::= SEQUENCE
SRSASN_CODE eutra_params_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mfbi_eutra_present, 1));
  HANDLE_CODE(bref.pack(modified_mpr_behavior_eutra_present, 1));
  HANDLE_CODE(bref.pack(multi_ns_pmax_eutra_present, 1));
  HANDLE_CODE(bref.pack(rs_sinr_meas_eutra_present, 1));

  if (modified_mpr_behavior_eutra_present) {
    HANDLE_CODE(modified_mpr_behavior_eutra.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ne_dc_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ne_dc_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_params_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mfbi_eutra_present, 1));
  HANDLE_CODE(bref.unpack(modified_mpr_behavior_eutra_present, 1));
  HANDLE_CODE(bref.unpack(multi_ns_pmax_eutra_present, 1));
  HANDLE_CODE(bref.unpack(rs_sinr_meas_eutra_present, 1));

  if (modified_mpr_behavior_eutra_present) {
    HANDLE_CODE(modified_mpr_behavior_eutra.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ne_dc_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void eutra_params_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mfbi_eutra_present) {
    j.write_str("mfbi-EUTRA", "supported");
  }
  if (modified_mpr_behavior_eutra_present) {
    j.write_str("modifiedMPR-BehaviorEUTRA", modified_mpr_behavior_eutra.to_string());
  }
  if (multi_ns_pmax_eutra_present) {
    j.write_str("multiNS-Pmax-EUTRA", "supported");
  }
  if (rs_sinr_meas_eutra_present) {
    j.write_str("rs-SINR-MeasEUTRA", "supported");
  }
  if (ext) {
    if (ne_dc_present) {
      j.write_str("ne-DC", "supported");
    }
  }
  j.end_obj();
}

// EUTRA-ParametersXDD-Diff ::= SEQUENCE
SRSASN_CODE eutra_params_xdd_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rsrq_meas_wideband_eutra_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_params_xdd_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rsrq_meas_wideband_eutra_present, 1));

  return SRSASN_SUCCESS;
}
void eutra_params_xdd_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrq_meas_wideband_eutra_present) {
    j.write_str("rsrqMeasWidebandEUTRA", "supported");
  }
  j.end_obj();
}

// EUTRA-Parameters ::= SEQUENCE
SRSASN_CODE eutra_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(eutra_params_common_present, 1));
  HANDLE_CODE(bref.pack(eutra_params_xdd_diff_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_eutra, 1, 256, integer_packer<uint16_t>(1, 256)));
  if (eutra_params_common_present) {
    HANDLE_CODE(eutra_params_common.pack(bref));
  }
  if (eutra_params_xdd_diff_present) {
    HANDLE_CODE(eutra_params_xdd_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(eutra_params_common_present, 1));
  HANDLE_CODE(bref.unpack(eutra_params_xdd_diff_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_eutra, bref, 1, 256, integer_packer<uint16_t>(1, 256)));
  if (eutra_params_common_present) {
    HANDLE_CODE(eutra_params_common.unpack(bref));
  }
  if (eutra_params_xdd_diff_present) {
    HANDLE_CODE(eutra_params_xdd_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void eutra_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListEUTRA");
  for (const auto& e1 : supported_band_list_eutra) {
    j.write_int(e1);
  }
  j.end_array();
  if (eutra_params_common_present) {
    j.write_fieldname("eutra-ParametersCommon");
    eutra_params_common.to_json(j);
  }
  if (eutra_params_xdd_diff_present) {
    j.write_fieldname("eutra-ParametersXDD-Diff");
    eutra_params_xdd_diff.to_json(j);
  }
  j.end_obj();
}

// FeatureSet ::= CHOICE
void feature_set_c::destroy_()
{
  switch (type_) {
    case types::eutra:
      c.destroy<eutra_s_>();
      break;
    case types::nr:
      c.destroy<nr_s_>();
      break;
    default:
      break;
  }
}
void feature_set_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eutra:
      c.init<eutra_s_>();
      break;
    case types::nr:
      c.init<nr_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "feature_set_c");
  }
}
feature_set_c::feature_set_c(const feature_set_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eutra:
      c.init(other.c.get<eutra_s_>());
      break;
    case types::nr:
      c.init(other.c.get<nr_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "feature_set_c");
  }
}
feature_set_c& feature_set_c::operator=(const feature_set_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eutra:
      c.set(other.c.get<eutra_s_>());
      break;
    case types::nr:
      c.set(other.c.get<nr_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "feature_set_c");
  }

  return *this;
}
feature_set_c::eutra_s_& feature_set_c::set_eutra()
{
  set(types::eutra);
  return c.get<eutra_s_>();
}
feature_set_c::nr_s_& feature_set_c::set_nr()
{
  set(types::nr);
  return c.get<nr_s_>();
}
void feature_set_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eutra:
      j.write_fieldname("eutra");
      j.start_obj();
      j.write_int("downlinkSetEUTRA", c.get<eutra_s_>().dl_set_eutra);
      j.write_int("uplinkSetEUTRA", c.get<eutra_s_>().ul_set_eutra);
      j.end_obj();
      break;
    case types::nr:
      j.write_fieldname("nr");
      j.start_obj();
      j.write_int("downlinkSetNR", c.get<nr_s_>().dl_set_nr);
      j.write_int("uplinkSetNR", c.get<nr_s_>().ul_set_nr);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "feature_set_c");
  }
  j.end_obj();
}
SRSASN_CODE feature_set_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eutra:
      HANDLE_CODE(pack_integer(bref, c.get<eutra_s_>().dl_set_eutra, (uint16_t)0u, (uint16_t)256u));
      HANDLE_CODE(pack_integer(bref, c.get<eutra_s_>().ul_set_eutra, (uint16_t)0u, (uint16_t)256u));
      break;
    case types::nr:
      HANDLE_CODE(pack_integer(bref, c.get<nr_s_>().dl_set_nr, (uint16_t)0u, (uint16_t)1024u));
      HANDLE_CODE(pack_integer(bref, c.get<nr_s_>().ul_set_nr, (uint16_t)0u, (uint16_t)1024u));
      break;
    default:
      log_invalid_choice_id(type_, "feature_set_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eutra:
      HANDLE_CODE(unpack_integer(c.get<eutra_s_>().dl_set_eutra, bref, (uint16_t)0u, (uint16_t)256u));
      HANDLE_CODE(unpack_integer(c.get<eutra_s_>().ul_set_eutra, bref, (uint16_t)0u, (uint16_t)256u));
      break;
    case types::nr:
      HANDLE_CODE(unpack_integer(c.get<nr_s_>().dl_set_nr, bref, (uint16_t)0u, (uint16_t)1024u));
      HANDLE_CODE(unpack_integer(c.get<nr_s_>().ul_set_nr, bref, (uint16_t)0u, (uint16_t)1024u));
      break;
    default:
      log_invalid_choice_id(type_, "feature_set_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DummyA ::= SEQUENCE
SRSASN_CODE dummy_a_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, max_num_nzp_csi_rs_per_cc, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(max_num_ports_across_nzp_csi_rs_per_cc.pack(bref));
  HANDLE_CODE(max_num_cs_im_per_cc.pack(bref));
  HANDLE_CODE(max_num_simul_csi_rs_act_bwp_all_cc.pack(bref));
  HANDLE_CODE(total_num_ports_simul_csi_rs_act_bwp_all_cc.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_a_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(max_num_nzp_csi_rs_per_cc, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(max_num_ports_across_nzp_csi_rs_per_cc.unpack(bref));
  HANDLE_CODE(max_num_cs_im_per_cc.unpack(bref));
  HANDLE_CODE(max_num_simul_csi_rs_act_bwp_all_cc.unpack(bref));
  HANDLE_CODE(total_num_ports_simul_csi_rs_act_bwp_all_cc.unpack(bref));

  return SRSASN_SUCCESS;
}
void dummy_a_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxNumberNZP-CSI-RS-PerCC", max_num_nzp_csi_rs_per_cc);
  j.write_str("maxNumberPortsAcrossNZP-CSI-RS-PerCC", max_num_ports_across_nzp_csi_rs_per_cc.to_string());
  j.write_str("maxNumberCS-IM-PerCC", max_num_cs_im_per_cc.to_string());
  j.write_str("maxNumberSimultaneousCSI-RS-ActBWP-AllCC", max_num_simul_csi_rs_act_bwp_all_cc.to_string());
  j.write_str("totalNumberPortsSimultaneousCSI-RS-ActBWP-AllCC",
              total_num_ports_simul_csi_rs_act_bwp_all_cc.to_string());
  j.end_obj();
}

const char* dummy_a_s::max_num_ports_across_nzp_csi_rs_per_cc_opts::to_string() const
{
  static const char* names[] = {"p2",   "p4",   "p8",   "p12",  "p16",  "p24",  "p32",  "p40",  "p48",
                                "p56",  "p64",  "p72",  "p80",  "p88",  "p96",  "p104", "p112", "p120",
                                "p128", "p136", "p144", "p152", "p160", "p168", "p176", "p184", "p192",
                                "p200", "p208", "p216", "p224", "p232", "p240", "p248", "p256"};
  return convert_enum_idx(names, 35, value, "dummy_a_s::max_num_ports_across_nzp_csi_rs_per_cc_e_");
}
uint16_t dummy_a_s::max_num_ports_across_nzp_csi_rs_per_cc_opts::to_number() const
{
  static const uint16_t numbers[] = {2,   4,   8,   12,  16,  24,  32,  40,  48,  56,  64,  72,
                                     80,  88,  96,  104, 112, 120, 128, 136, 144, 152, 160, 168,
                                     176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256};
  return map_enum_number(numbers, 35, value, "dummy_a_s::max_num_ports_across_nzp_csi_rs_per_cc_e_");
}

const char* dummy_a_s::max_num_cs_im_per_cc_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n16", "n32"};
  return convert_enum_idx(names, 6, value, "dummy_a_s::max_num_cs_im_per_cc_e_");
}
uint8_t dummy_a_s::max_num_cs_im_per_cc_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "dummy_a_s::max_num_cs_im_per_cc_e_");
}

const char* dummy_a_s::max_num_simul_csi_rs_act_bwp_all_cc_opts::to_string() const
{
  static const char* names[] = {"n5",  "n6",  "n7",  "n8",  "n9",  "n10", "n12", "n14", "n16", "n18", "n20",
                                "n22", "n24", "n26", "n28", "n30", "n32", "n34", "n36", "n38", "n40", "n42",
                                "n44", "n46", "n48", "n50", "n52", "n54", "n56", "n58", "n60", "n62", "n64"};
  return convert_enum_idx(names, 33, value, "dummy_a_s::max_num_simul_csi_rs_act_bwp_all_cc_e_");
}
uint8_t dummy_a_s::max_num_simul_csi_rs_act_bwp_all_cc_opts::to_number() const
{
  static const uint8_t numbers[] = {5,  6,  7,  8,  9,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
                                    34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64};
  return map_enum_number(numbers, 33, value, "dummy_a_s::max_num_simul_csi_rs_act_bwp_all_cc_e_");
}

const char* dummy_a_s::total_num_ports_simul_csi_rs_act_bwp_all_cc_opts::to_string() const
{
  static const char* names[] = {"p8",   "p12",  "p16",  "p24",  "p32",  "p40",  "p48",  "p56",  "p64",  "p72",  "p80",
                                "p88",  "p96",  "p104", "p112", "p120", "p128", "p136", "p144", "p152", "p160", "p168",
                                "p176", "p184", "p192", "p200", "p208", "p216", "p224", "p232", "p240", "p248", "p256"};
  return convert_enum_idx(names, 33, value, "dummy_a_s::total_num_ports_simul_csi_rs_act_bwp_all_cc_e_");
}
uint16_t dummy_a_s::total_num_ports_simul_csi_rs_act_bwp_all_cc_opts::to_number() const
{
  static const uint16_t numbers[] = {8,   12,  16,  24,  32,  40,  48,  56,  64,  72,  80,
                                     88,  96,  104, 112, 120, 128, 136, 144, 152, 160, 168,
                                     176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256};
  return map_enum_number(numbers, 33, value, "dummy_a_s::total_num_ports_simul_csi_rs_act_bwp_all_cc_e_");
}

// DummyB ::= SEQUENCE
SRSASN_CODE dummy_b_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_tx_ports_per_res.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_res, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, total_num_tx_ports, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(supported_codebook_mode.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_csi_rs_per_res_set, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_b_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_tx_ports_per_res.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_res, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(total_num_tx_ports, bref, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(supported_codebook_mode.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_csi_rs_per_res_set, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void dummy_b_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberTxPortsPerResource", max_num_tx_ports_per_res.to_string());
  j.write_int("maxNumberResources", max_num_res);
  j.write_int("totalNumberTxPorts", total_num_tx_ports);
  j.write_str("supportedCodebookMode", supported_codebook_mode.to_string());
  j.write_int("maxNumberCSI-RS-PerResourceSet", max_num_csi_rs_per_res_set);
  j.end_obj();
}

const char* dummy_b_s::max_num_tx_ports_per_res_opts::to_string() const
{
  static const char* names[] = {"p2", "p4", "p8", "p12", "p16", "p24", "p32"};
  return convert_enum_idx(names, 7, value, "dummy_b_s::max_num_tx_ports_per_res_e_");
}
uint8_t dummy_b_s::max_num_tx_ports_per_res_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 12, 16, 24, 32};
  return map_enum_number(numbers, 7, value, "dummy_b_s::max_num_tx_ports_per_res_e_");
}

const char* dummy_b_s::supported_codebook_mode_opts::to_string() const
{
  static const char* names[] = {"mode1", "mode1AndMode2"};
  return convert_enum_idx(names, 2, value, "dummy_b_s::supported_codebook_mode_e_");
}

// DummyC ::= SEQUENCE
SRSASN_CODE dummy_c_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_tx_ports_per_res.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_res, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, total_num_tx_ports, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(supported_codebook_mode.pack(bref));
  HANDLE_CODE(supported_num_panels.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_csi_rs_per_res_set, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_c_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_tx_ports_per_res.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_res, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(total_num_tx_ports, bref, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(supported_codebook_mode.unpack(bref));
  HANDLE_CODE(supported_num_panels.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_csi_rs_per_res_set, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void dummy_c_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberTxPortsPerResource", max_num_tx_ports_per_res.to_string());
  j.write_int("maxNumberResources", max_num_res);
  j.write_int("totalNumberTxPorts", total_num_tx_ports);
  j.write_str("supportedCodebookMode", supported_codebook_mode.to_string());
  j.write_str("supportedNumberPanels", supported_num_panels.to_string());
  j.write_int("maxNumberCSI-RS-PerResourceSet", max_num_csi_rs_per_res_set);
  j.end_obj();
}

const char* dummy_c_s::max_num_tx_ports_per_res_opts::to_string() const
{
  static const char* names[] = {"p8", "p16", "p32"};
  return convert_enum_idx(names, 3, value, "dummy_c_s::max_num_tx_ports_per_res_e_");
}
uint8_t dummy_c_s::max_num_tx_ports_per_res_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 16, 32};
  return map_enum_number(numbers, 3, value, "dummy_c_s::max_num_tx_ports_per_res_e_");
}

const char* dummy_c_s::supported_codebook_mode_opts::to_string() const
{
  static const char* names[] = {"mode1", "mode2", "both"};
  return convert_enum_idx(names, 3, value, "dummy_c_s::supported_codebook_mode_e_");
}
uint8_t dummy_c_s::supported_codebook_mode_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "dummy_c_s::supported_codebook_mode_e_");
}

const char* dummy_c_s::supported_num_panels_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(names, 2, value, "dummy_c_s::supported_num_panels_e_");
}
uint8_t dummy_c_s::supported_num_panels_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "dummy_c_s::supported_num_panels_e_");
}

// DummyD ::= SEQUENCE
SRSASN_CODE dummy_d_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(amplitude_subset_restrict_present, 1));

  HANDLE_CODE(max_num_tx_ports_per_res.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_res, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, total_num_tx_ports, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(pack_integer(bref, param_lx, (uint8_t)2u, (uint8_t)4u));
  HANDLE_CODE(amplitude_scaling_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_csi_rs_per_res_set, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_d_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(amplitude_subset_restrict_present, 1));

  HANDLE_CODE(max_num_tx_ports_per_res.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_res, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(total_num_tx_ports, bref, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(unpack_integer(param_lx, bref, (uint8_t)2u, (uint8_t)4u));
  HANDLE_CODE(amplitude_scaling_type.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_csi_rs_per_res_set, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void dummy_d_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberTxPortsPerResource", max_num_tx_ports_per_res.to_string());
  j.write_int("maxNumberResources", max_num_res);
  j.write_int("totalNumberTxPorts", total_num_tx_ports);
  j.write_int("parameterLx", param_lx);
  j.write_str("amplitudeScalingType", amplitude_scaling_type.to_string());
  if (amplitude_subset_restrict_present) {
    j.write_str("amplitudeSubsetRestriction", "supported");
  }
  j.write_int("maxNumberCSI-RS-PerResourceSet", max_num_csi_rs_per_res_set);
  j.end_obj();
}

const char* dummy_d_s::max_num_tx_ports_per_res_opts::to_string() const
{
  static const char* names[] = {"p4", "p8", "p12", "p16", "p24", "p32"};
  return convert_enum_idx(names, 6, value, "dummy_d_s::max_num_tx_ports_per_res_e_");
}
uint8_t dummy_d_s::max_num_tx_ports_per_res_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 12, 16, 24, 32};
  return map_enum_number(numbers, 6, value, "dummy_d_s::max_num_tx_ports_per_res_e_");
}

const char* dummy_d_s::amplitude_scaling_type_opts::to_string() const
{
  static const char* names[] = {"wideband", "widebandAndSubband"};
  return convert_enum_idx(names, 2, value, "dummy_d_s::amplitude_scaling_type_e_");
}

// DummyE ::= SEQUENCE
SRSASN_CODE dummy_e_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_tx_ports_per_res.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_res, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(pack_integer(bref, total_num_tx_ports, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(pack_integer(bref, param_lx, (uint8_t)2u, (uint8_t)4u));
  HANDLE_CODE(amplitude_scaling_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_num_csi_rs_per_res_set, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_e_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_tx_ports_per_res.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_res, bref, (uint8_t)1u, (uint8_t)64u));
  HANDLE_CODE(unpack_integer(total_num_tx_ports, bref, (uint16_t)2u, (uint16_t)256u));
  HANDLE_CODE(unpack_integer(param_lx, bref, (uint8_t)2u, (uint8_t)4u));
  HANDLE_CODE(amplitude_scaling_type.unpack(bref));
  HANDLE_CODE(unpack_integer(max_num_csi_rs_per_res_set, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void dummy_e_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberTxPortsPerResource", max_num_tx_ports_per_res.to_string());
  j.write_int("maxNumberResources", max_num_res);
  j.write_int("totalNumberTxPorts", total_num_tx_ports);
  j.write_int("parameterLx", param_lx);
  j.write_str("amplitudeScalingType", amplitude_scaling_type.to_string());
  j.write_int("maxNumberCSI-RS-PerResourceSet", max_num_csi_rs_per_res_set);
  j.end_obj();
}

const char* dummy_e_s::max_num_tx_ports_per_res_opts::to_string() const
{
  static const char* names[] = {"p4", "p8", "p12", "p16", "p24", "p32"};
  return convert_enum_idx(names, 6, value, "dummy_e_s::max_num_tx_ports_per_res_e_");
}
uint8_t dummy_e_s::max_num_tx_ports_per_res_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 12, 16, 24, 32};
  return map_enum_number(numbers, 6, value, "dummy_e_s::max_num_tx_ports_per_res_e_");
}

const char* dummy_e_s::amplitude_scaling_type_opts::to_string() const
{
  static const char* names[] = {"wideband", "widebandAndSubband"};
  return convert_enum_idx(names, 2, value, "dummy_e_s::amplitude_scaling_type_e_");
}

// FreqSeparationClass ::= ENUMERATED
const char* freq_separation_class_opts::to_string() const
{
  static const char* names[] = {"c1", "c2", "c3"};
  return convert_enum_idx(names, 3, value, "freq_separation_class_e");
}
uint8_t freq_separation_class_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "freq_separation_class_e");
}

// FeatureSetDownlink ::= SEQUENCE
SRSASN_CODE feature_set_dl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(intra_band_freq_separation_dl_present, 1));
  HANDLE_CODE(bref.pack(scaling_factor_present, 1));
  HANDLE_CODE(bref.pack(cross_carrier_sched_other_scs_present, 1));
  HANDLE_CODE(bref.pack(scell_without_ssb_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_meas_scell_without_ssb_present, 1));
  HANDLE_CODE(bref.pack(dummy1_present, 1));
  HANDLE_CODE(bref.pack(type1_3_css_present, 1));
  HANDLE_CODE(bref.pack(pdcch_monitoring_any_occasions_present, 1));
  HANDLE_CODE(bref.pack(dummy2_present, 1));
  HANDLE_CODE(bref.pack(ue_specific_ul_dl_assign_present, 1));
  HANDLE_CODE(bref.pack(search_space_sharing_ca_dl_present, 1));
  HANDLE_CODE(bref.pack(time_dur_for_qcl_present, 1));
  HANDLE_CODE(bref.pack(pdsch_processing_type1_different_tb_per_slot_present, 1));
  HANDLE_CODE(bref.pack(dummy3_present, 1));
  HANDLE_CODE(bref.pack(dummy4.size() > 0, 1));
  HANDLE_CODE(bref.pack(dummy5.size() > 0, 1));
  HANDLE_CODE(bref.pack(dummy6.size() > 0, 1));
  HANDLE_CODE(bref.pack(dummy7.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, feature_set_list_per_dl_cc, 1, 32, integer_packer<uint16_t>(1, 1024)));
  if (intra_band_freq_separation_dl_present) {
    HANDLE_CODE(intra_band_freq_separation_dl.pack(bref));
  }
  if (scaling_factor_present) {
    HANDLE_CODE(scaling_factor.pack(bref));
  }
  if (pdcch_monitoring_any_occasions_present) {
    HANDLE_CODE(pdcch_monitoring_any_occasions.pack(bref));
  }
  if (time_dur_for_qcl_present) {
    HANDLE_CODE(bref.pack(time_dur_for_qcl.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(time_dur_for_qcl.scs_120k_hz_present, 1));
    if (time_dur_for_qcl.scs_60k_hz_present) {
      HANDLE_CODE(time_dur_for_qcl.scs_60k_hz.pack(bref));
    }
    if (time_dur_for_qcl.scs_120k_hz_present) {
      HANDLE_CODE(time_dur_for_qcl.scs_120k_hz.pack(bref));
    }
  }
  if (pdsch_processing_type1_different_tb_per_slot_present) {
    HANDLE_CODE(bref.pack(pdsch_processing_type1_different_tb_per_slot.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdsch_processing_type1_different_tb_per_slot.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdsch_processing_type1_different_tb_per_slot.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdsch_processing_type1_different_tb_per_slot.scs_120k_hz_present, 1));
    if (pdsch_processing_type1_different_tb_per_slot.scs_15k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_15k_hz.pack(bref));
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_30k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_30k_hz.pack(bref));
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_60k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_60k_hz.pack(bref));
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_120k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_120k_hz.pack(bref));
    }
  }
  if (dummy3_present) {
    HANDLE_CODE(dummy3.pack(bref));
  }
  if (dummy4.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dummy4, 1, 16));
  }
  if (dummy5.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dummy5, 1, 16));
  }
  if (dummy6.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dummy6, 1, 16));
  }
  if (dummy7.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dummy7, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_dl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(intra_band_freq_separation_dl_present, 1));
  HANDLE_CODE(bref.unpack(scaling_factor_present, 1));
  HANDLE_CODE(bref.unpack(cross_carrier_sched_other_scs_present, 1));
  HANDLE_CODE(bref.unpack(scell_without_ssb_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_meas_scell_without_ssb_present, 1));
  HANDLE_CODE(bref.unpack(dummy1_present, 1));
  HANDLE_CODE(bref.unpack(type1_3_css_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_monitoring_any_occasions_present, 1));
  HANDLE_CODE(bref.unpack(dummy2_present, 1));
  HANDLE_CODE(bref.unpack(ue_specific_ul_dl_assign_present, 1));
  HANDLE_CODE(bref.unpack(search_space_sharing_ca_dl_present, 1));
  HANDLE_CODE(bref.unpack(time_dur_for_qcl_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_processing_type1_different_tb_per_slot_present, 1));
  HANDLE_CODE(bref.unpack(dummy3_present, 1));
  bool dummy4_present;
  HANDLE_CODE(bref.unpack(dummy4_present, 1));
  bool dummy5_present;
  HANDLE_CODE(bref.unpack(dummy5_present, 1));
  bool dummy6_present;
  HANDLE_CODE(bref.unpack(dummy6_present, 1));
  bool dummy7_present;
  HANDLE_CODE(bref.unpack(dummy7_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(feature_set_list_per_dl_cc, bref, 1, 32, integer_packer<uint16_t>(1, 1024)));
  if (intra_band_freq_separation_dl_present) {
    HANDLE_CODE(intra_band_freq_separation_dl.unpack(bref));
  }
  if (scaling_factor_present) {
    HANDLE_CODE(scaling_factor.unpack(bref));
  }
  if (pdcch_monitoring_any_occasions_present) {
    HANDLE_CODE(pdcch_monitoring_any_occasions.unpack(bref));
  }
  if (time_dur_for_qcl_present) {
    HANDLE_CODE(bref.unpack(time_dur_for_qcl.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(time_dur_for_qcl.scs_120k_hz_present, 1));
    if (time_dur_for_qcl.scs_60k_hz_present) {
      HANDLE_CODE(time_dur_for_qcl.scs_60k_hz.unpack(bref));
    }
    if (time_dur_for_qcl.scs_120k_hz_present) {
      HANDLE_CODE(time_dur_for_qcl.scs_120k_hz.unpack(bref));
    }
  }
  if (pdsch_processing_type1_different_tb_per_slot_present) {
    HANDLE_CODE(bref.unpack(pdsch_processing_type1_different_tb_per_slot.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdsch_processing_type1_different_tb_per_slot.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdsch_processing_type1_different_tb_per_slot.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdsch_processing_type1_different_tb_per_slot.scs_120k_hz_present, 1));
    if (pdsch_processing_type1_different_tb_per_slot.scs_15k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_15k_hz.unpack(bref));
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_30k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_30k_hz.unpack(bref));
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_60k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_60k_hz.unpack(bref));
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_120k_hz_present) {
      HANDLE_CODE(pdsch_processing_type1_different_tb_per_slot.scs_120k_hz.unpack(bref));
    }
  }
  if (dummy3_present) {
    HANDLE_CODE(dummy3.unpack(bref));
  }
  if (dummy4_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dummy4, bref, 1, 16));
  }
  if (dummy5_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dummy5, bref, 1, 16));
  }
  if (dummy6_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dummy6, bref, 1, 16));
  }
  if (dummy7_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dummy7, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void feature_set_dl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("featureSetListPerDownlinkCC");
  for (const auto& e1 : feature_set_list_per_dl_cc) {
    j.write_int(e1);
  }
  j.end_array();
  if (intra_band_freq_separation_dl_present) {
    j.write_str("intraBandFreqSeparationDL", intra_band_freq_separation_dl.to_string());
  }
  if (scaling_factor_present) {
    j.write_str("scalingFactor", scaling_factor.to_string());
  }
  if (cross_carrier_sched_other_scs_present) {
    j.write_str("crossCarrierScheduling-OtherSCS", "supported");
  }
  if (scell_without_ssb_present) {
    j.write_str("scellWithoutSSB", "supported");
  }
  if (csi_rs_meas_scell_without_ssb_present) {
    j.write_str("csi-RS-MeasSCellWithoutSSB", "supported");
  }
  if (dummy1_present) {
    j.write_str("dummy1", "supported");
  }
  if (type1_3_css_present) {
    j.write_str("type1-3-CSS", "supported");
  }
  if (pdcch_monitoring_any_occasions_present) {
    j.write_str("pdcch-MonitoringAnyOccasions", pdcch_monitoring_any_occasions.to_string());
  }
  if (dummy2_present) {
    j.write_str("dummy2", "supported");
  }
  if (ue_specific_ul_dl_assign_present) {
    j.write_str("ue-SpecificUL-DL-Assignment", "supported");
  }
  if (search_space_sharing_ca_dl_present) {
    j.write_str("searchSpaceSharingCA-DL", "supported");
  }
  if (time_dur_for_qcl_present) {
    j.write_fieldname("timeDurationForQCL");
    j.start_obj();
    if (time_dur_for_qcl.scs_60k_hz_present) {
      j.write_str("scs-60kHz", time_dur_for_qcl.scs_60k_hz.to_string());
    }
    if (time_dur_for_qcl.scs_120k_hz_present) {
      j.write_str("scs-120kHz", time_dur_for_qcl.scs_120k_hz.to_string());
    }
    j.end_obj();
  }
  if (pdsch_processing_type1_different_tb_per_slot_present) {
    j.write_fieldname("pdsch-ProcessingType1-DifferentTB-PerSlot");
    j.start_obj();
    if (pdsch_processing_type1_different_tb_per_slot.scs_15k_hz_present) {
      j.write_str("scs-15kHz", pdsch_processing_type1_different_tb_per_slot.scs_15k_hz.to_string());
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_30k_hz_present) {
      j.write_str("scs-30kHz", pdsch_processing_type1_different_tb_per_slot.scs_30k_hz.to_string());
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_60k_hz_present) {
      j.write_str("scs-60kHz", pdsch_processing_type1_different_tb_per_slot.scs_60k_hz.to_string());
    }
    if (pdsch_processing_type1_different_tb_per_slot.scs_120k_hz_present) {
      j.write_str("scs-120kHz", pdsch_processing_type1_different_tb_per_slot.scs_120k_hz.to_string());
    }
    j.end_obj();
  }
  if (dummy3_present) {
    j.write_fieldname("dummy3");
    dummy3.to_json(j);
  }
  if (dummy4.size() > 0) {
    j.start_array("dummy4");
    for (const auto& e1 : dummy4) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (dummy5.size() > 0) {
    j.start_array("dummy5");
    for (const auto& e1 : dummy5) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (dummy6.size() > 0) {
    j.start_array("dummy6");
    for (const auto& e1 : dummy6) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (dummy7.size() > 0) {
    j.start_array("dummy7");
    for (const auto& e1 : dummy7) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* feature_set_dl_s::scaling_factor_opts::to_string() const
{
  static const char* names[] = {"f0p4", "f0p75", "f0p8"};
  return convert_enum_idx(names, 3, value, "feature_set_dl_s::scaling_factor_e_");
}

const char* feature_set_dl_s::pdcch_monitoring_any_occasions_opts::to_string() const
{
  static const char* names[] = {"withoutDCI-Gap", "withDCI-Gap"};
  return convert_enum_idx(names, 2, value, "feature_set_dl_s::pdcch_monitoring_any_occasions_e_");
}

const char* feature_set_dl_s::time_dur_for_qcl_s_::scs_60k_hz_opts::to_string() const
{
  static const char* names[] = {"s7", "s14", "s28"};
  return convert_enum_idx(names, 3, value, "feature_set_dl_s::time_dur_for_qcl_s_::scs_60k_hz_e_");
}
uint8_t feature_set_dl_s::time_dur_for_qcl_s_::scs_60k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 14, 28};
  return map_enum_number(numbers, 3, value, "feature_set_dl_s::time_dur_for_qcl_s_::scs_60k_hz_e_");
}

const char* feature_set_dl_s::time_dur_for_qcl_s_::scs_120k_hz_opts::to_string() const
{
  static const char* names[] = {"s14", "s28"};
  return convert_enum_idx(names, 2, value, "feature_set_dl_s::time_dur_for_qcl_s_::scs_120k_hz_e_");
}
uint8_t feature_set_dl_s::time_dur_for_qcl_s_::scs_120k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {14, 28};
  return map_enum_number(numbers, 2, value, "feature_set_dl_s::time_dur_for_qcl_s_::scs_120k_hz_e_");
}

const char* feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_e_");
}
uint8_t feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_e_");
}

const char* feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_e_");
}
uint8_t feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_e_");
}

const char* feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_e_");
}
uint8_t feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_e_");
}

const char* feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_e_");
}
uint8_t feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_s::pdsch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_e_");
}

// ProcessingParameters ::= SEQUENCE
SRSASN_CODE processing_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(different_tb_per_slot_present, 1));

  HANDLE_CODE(fallback.pack(bref));
  if (different_tb_per_slot_present) {
    HANDLE_CODE(bref.pack(different_tb_per_slot.upto1_present, 1));
    HANDLE_CODE(bref.pack(different_tb_per_slot.upto2_present, 1));
    HANDLE_CODE(bref.pack(different_tb_per_slot.upto4_present, 1));
    HANDLE_CODE(bref.pack(different_tb_per_slot.upto7_present, 1));
    if (different_tb_per_slot.upto1_present) {
      HANDLE_CODE(pack_integer(bref, different_tb_per_slot.upto1, (uint8_t)1u, (uint8_t)16u));
    }
    if (different_tb_per_slot.upto2_present) {
      HANDLE_CODE(pack_integer(bref, different_tb_per_slot.upto2, (uint8_t)1u, (uint8_t)16u));
    }
    if (different_tb_per_slot.upto4_present) {
      HANDLE_CODE(pack_integer(bref, different_tb_per_slot.upto4, (uint8_t)1u, (uint8_t)16u));
    }
    if (different_tb_per_slot.upto7_present) {
      HANDLE_CODE(pack_integer(bref, different_tb_per_slot.upto7, (uint8_t)1u, (uint8_t)16u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE processing_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(different_tb_per_slot_present, 1));

  HANDLE_CODE(fallback.unpack(bref));
  if (different_tb_per_slot_present) {
    HANDLE_CODE(bref.unpack(different_tb_per_slot.upto1_present, 1));
    HANDLE_CODE(bref.unpack(different_tb_per_slot.upto2_present, 1));
    HANDLE_CODE(bref.unpack(different_tb_per_slot.upto4_present, 1));
    HANDLE_CODE(bref.unpack(different_tb_per_slot.upto7_present, 1));
    if (different_tb_per_slot.upto1_present) {
      HANDLE_CODE(unpack_integer(different_tb_per_slot.upto1, bref, (uint8_t)1u, (uint8_t)16u));
    }
    if (different_tb_per_slot.upto2_present) {
      HANDLE_CODE(unpack_integer(different_tb_per_slot.upto2, bref, (uint8_t)1u, (uint8_t)16u));
    }
    if (different_tb_per_slot.upto4_present) {
      HANDLE_CODE(unpack_integer(different_tb_per_slot.upto4, bref, (uint8_t)1u, (uint8_t)16u));
    }
    if (different_tb_per_slot.upto7_present) {
      HANDLE_CODE(unpack_integer(different_tb_per_slot.upto7, bref, (uint8_t)1u, (uint8_t)16u));
    }
  }

  return SRSASN_SUCCESS;
}
void processing_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("fallback", fallback.to_string());
  if (different_tb_per_slot_present) {
    j.write_fieldname("differentTB-PerSlot");
    j.start_obj();
    if (different_tb_per_slot.upto1_present) {
      j.write_int("upto1", different_tb_per_slot.upto1);
    }
    if (different_tb_per_slot.upto2_present) {
      j.write_int("upto2", different_tb_per_slot.upto2);
    }
    if (different_tb_per_slot.upto4_present) {
      j.write_int("upto4", different_tb_per_slot.upto4);
    }
    if (different_tb_per_slot.upto7_present) {
      j.write_int("upto7", different_tb_per_slot.upto7);
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* processing_params_s::fallback_opts::to_string() const
{
  static const char* names[] = {"sc", "cap1-only"};
  return convert_enum_idx(names, 2, value, "processing_params_s::fallback_e_");
}
uint8_t processing_params_s::fallback_opts::to_number() const
{
  if (value == cap1_only) {
    return 1;
  }
  invalid_enum_number(value, "processing_params_s::fallback_e_");
  return 0;
}

// FeatureSetDownlink-v1540 ::= SEQUENCE
SRSASN_CODE feature_set_dl_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(one_fl_dmrs_two_add_dmrs_dl_present, 1));
  HANDLE_CODE(bref.pack(add_dmrs_dl_alt_present, 1));
  HANDLE_CODE(bref.pack(two_fl_dmrs_two_add_dmrs_dl_present, 1));
  HANDLE_CODE(bref.pack(one_fl_dmrs_three_add_dmrs_dl_present, 1));
  HANDLE_CODE(bref.pack(pdcch_monitoring_any_occasions_with_span_gap_present, 1));
  HANDLE_CODE(bref.pack(pdsch_separation_with_gap_present, 1));
  HANDLE_CODE(bref.pack(pdsch_processing_type2_present, 1));
  HANDLE_CODE(bref.pack(pdsch_processing_type2_limited_present, 1));
  HANDLE_CODE(bref.pack(dl_mcs_table_alt_dyn_ind_present, 1));

  if (pdcch_monitoring_any_occasions_with_span_gap_present) {
    HANDLE_CODE(bref.pack(pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz_present, 1));
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz.pack(bref));
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz.pack(bref));
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz.pack(bref));
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz.pack(bref));
    }
  }
  if (pdsch_processing_type2_present) {
    HANDLE_CODE(bref.pack(pdsch_processing_type2.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdsch_processing_type2.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(pdsch_processing_type2.scs_60k_hz_present, 1));
    if (pdsch_processing_type2.scs_15k_hz_present) {
      HANDLE_CODE(pdsch_processing_type2.scs_15k_hz.pack(bref));
    }
    if (pdsch_processing_type2.scs_30k_hz_present) {
      HANDLE_CODE(pdsch_processing_type2.scs_30k_hz.pack(bref));
    }
    if (pdsch_processing_type2.scs_60k_hz_present) {
      HANDLE_CODE(pdsch_processing_type2.scs_60k_hz.pack(bref));
    }
  }
  if (pdsch_processing_type2_limited_present) {
    HANDLE_CODE(pdsch_processing_type2_limited.different_tb_per_slot_scs_30k_hz.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_dl_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(one_fl_dmrs_two_add_dmrs_dl_present, 1));
  HANDLE_CODE(bref.unpack(add_dmrs_dl_alt_present, 1));
  HANDLE_CODE(bref.unpack(two_fl_dmrs_two_add_dmrs_dl_present, 1));
  HANDLE_CODE(bref.unpack(one_fl_dmrs_three_add_dmrs_dl_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_monitoring_any_occasions_with_span_gap_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_separation_with_gap_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_processing_type2_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_processing_type2_limited_present, 1));
  HANDLE_CODE(bref.unpack(dl_mcs_table_alt_dyn_ind_present, 1));

  if (pdcch_monitoring_any_occasions_with_span_gap_present) {
    HANDLE_CODE(bref.unpack(pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz_present, 1));
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz.unpack(bref));
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz.unpack(bref));
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz.unpack(bref));
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz_present) {
      HANDLE_CODE(pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz.unpack(bref));
    }
  }
  if (pdsch_processing_type2_present) {
    HANDLE_CODE(bref.unpack(pdsch_processing_type2.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdsch_processing_type2.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pdsch_processing_type2.scs_60k_hz_present, 1));
    if (pdsch_processing_type2.scs_15k_hz_present) {
      HANDLE_CODE(pdsch_processing_type2.scs_15k_hz.unpack(bref));
    }
    if (pdsch_processing_type2.scs_30k_hz_present) {
      HANDLE_CODE(pdsch_processing_type2.scs_30k_hz.unpack(bref));
    }
    if (pdsch_processing_type2.scs_60k_hz_present) {
      HANDLE_CODE(pdsch_processing_type2.scs_60k_hz.unpack(bref));
    }
  }
  if (pdsch_processing_type2_limited_present) {
    HANDLE_CODE(pdsch_processing_type2_limited.different_tb_per_slot_scs_30k_hz.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void feature_set_dl_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (one_fl_dmrs_two_add_dmrs_dl_present) {
    j.write_str("oneFL-DMRS-TwoAdditionalDMRS-DL", "supported");
  }
  if (add_dmrs_dl_alt_present) {
    j.write_str("additionalDMRS-DL-Alt", "supported");
  }
  if (two_fl_dmrs_two_add_dmrs_dl_present) {
    j.write_str("twoFL-DMRS-TwoAdditionalDMRS-DL", "supported");
  }
  if (one_fl_dmrs_three_add_dmrs_dl_present) {
    j.write_str("oneFL-DMRS-ThreeAdditionalDMRS-DL", "supported");
  }
  if (pdcch_monitoring_any_occasions_with_span_gap_present) {
    j.write_fieldname("pdcch-MonitoringAnyOccasionsWithSpanGap");
    j.start_obj();
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz_present) {
      j.write_str("scs-15kHz", pdcch_monitoring_any_occasions_with_span_gap.scs_15k_hz.to_string());
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz_present) {
      j.write_str("scs-30kHz", pdcch_monitoring_any_occasions_with_span_gap.scs_30k_hz.to_string());
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz_present) {
      j.write_str("scs-60kHz", pdcch_monitoring_any_occasions_with_span_gap.scs_60k_hz.to_string());
    }
    if (pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz_present) {
      j.write_str("scs-120kHz", pdcch_monitoring_any_occasions_with_span_gap.scs_120k_hz.to_string());
    }
    j.end_obj();
  }
  if (pdsch_separation_with_gap_present) {
    j.write_str("pdsch-SeparationWithGap", "supported");
  }
  if (pdsch_processing_type2_present) {
    j.write_fieldname("pdsch-ProcessingType2");
    j.start_obj();
    if (pdsch_processing_type2.scs_15k_hz_present) {
      j.write_fieldname("scs-15kHz");
      pdsch_processing_type2.scs_15k_hz.to_json(j);
    }
    if (pdsch_processing_type2.scs_30k_hz_present) {
      j.write_fieldname("scs-30kHz");
      pdsch_processing_type2.scs_30k_hz.to_json(j);
    }
    if (pdsch_processing_type2.scs_60k_hz_present) {
      j.write_fieldname("scs-60kHz");
      pdsch_processing_type2.scs_60k_hz.to_json(j);
    }
    j.end_obj();
  }
  if (pdsch_processing_type2_limited_present) {
    j.write_fieldname("pdsch-ProcessingType2-Limited");
    j.start_obj();
    j.write_str("differentTB-PerSlot-SCS-30kHz",
                pdsch_processing_type2_limited.different_tb_per_slot_scs_30k_hz.to_string());
    j.end_obj();
  }
  if (dl_mcs_table_alt_dyn_ind_present) {
    j.write_str("dl-MCS-TableAlt-DynamicIndication", "supported");
  }
  j.end_obj();
}

const char* feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_15k_hz_opts::to_string() const
{
  static const char* names[] = {"set1", "set2", "set3"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_15k_hz_e_");
}
uint8_t feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_15k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_15k_hz_e_");
}

const char* feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_30k_hz_opts::to_string() const
{
  static const char* names[] = {"set1", "set2", "set3"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_30k_hz_e_");
}
uint8_t feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_30k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_30k_hz_e_");
}

const char* feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_60k_hz_opts::to_string() const
{
  static const char* names[] = {"set1", "set2", "set3"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_60k_hz_e_");
}
uint8_t feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_60k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_60k_hz_e_");
}

const char* feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_120k_hz_opts::to_string() const
{
  static const char* names[] = {"set1", "set2", "set3"};
  return convert_enum_idx(
      names, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_120k_hz_e_");
}
uint8_t feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_120k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(
      numbers, 3, value, "feature_set_dl_v1540_s::pdcch_monitoring_any_occasions_with_span_gap_s_::scs_120k_hz_e_");
}

const char*
feature_set_dl_v1540_s::pdsch_processing_type2_limited_s_::different_tb_per_slot_scs_30k_hz_opts::to_string() const
{
  static const char* names[] = {"upto1", "upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names,
      4,
      value,
      "feature_set_dl_v1540_s::pdsch_processing_type2_limited_s_::different_tb_per_slot_scs_30k_hz_e_");
}
uint8_t
feature_set_dl_v1540_s::pdsch_processing_type2_limited_s_::different_tb_per_slot_scs_30k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 7};
  return map_enum_number(
      numbers,
      4,
      value,
      "feature_set_dl_v1540_s::pdsch_processing_type2_limited_s_::different_tb_per_slot_scs_30k_hz_e_");
}

// FeatureSetDownlink-v15a0 ::= SEQUENCE
SRSASN_CODE feature_set_dl_v15a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_srs_res_present, 1));

  if (supported_srs_res_present) {
    HANDLE_CODE(supported_srs_res.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_dl_v15a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_srs_res_present, 1));

  if (supported_srs_res_present) {
    HANDLE_CODE(supported_srs_res.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void feature_set_dl_v15a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_srs_res_present) {
    j.write_fieldname("supportedSRS-Resources");
    supported_srs_res.to_json(j);
  }
  j.end_obj();
}

// ModulationOrder ::= ENUMERATED
const char* mod_order_opts::to_string() const
{
  static const char* names[] = {"bpsk-halfpi", "bpsk", "qpsk", "qam16", "qam64", "qam256"};
  return convert_enum_idx(names, 6, value, "mod_order_e");
}
float mod_order_opts::to_number() const
{
  switch (value) {
    case bpsk_halfpi:
      return 0.5;
    case qam16:
      return 16.0;
    case qam64:
      return 64.0;
    case qam256:
      return 256.0;
    default:
      invalid_enum_number(value, "mod_order_e");
  }
  return 0;
}
const char* mod_order_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "16", "64", "256"};
  return convert_enum_idx(number_strs, 6, value, "mod_order_e");
}

// SupportedBandwidth ::= CHOICE
void supported_bw_c::destroy_() {}
void supported_bw_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
supported_bw_c::supported_bw_c(const supported_bw_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fr1:
      c.init(other.c.get<fr1_e_>());
      break;
    case types::fr2:
      c.init(other.c.get<fr2_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "supported_bw_c");
  }
}
supported_bw_c& supported_bw_c::operator=(const supported_bw_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fr1:
      c.set(other.c.get<fr1_e_>());
      break;
    case types::fr2:
      c.set(other.c.get<fr2_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "supported_bw_c");
  }

  return *this;
}
supported_bw_c::fr1_e_& supported_bw_c::set_fr1()
{
  set(types::fr1);
  return c.get<fr1_e_>();
}
supported_bw_c::fr2_e_& supported_bw_c::set_fr2()
{
  set(types::fr2);
  return c.get<fr2_e_>();
}
void supported_bw_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fr1:
      j.write_str("fr1", c.get<fr1_e_>().to_string());
      break;
    case types::fr2:
      j.write_str("fr2", c.get<fr2_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "supported_bw_c");
  }
  j.end_obj();
}
SRSASN_CODE supported_bw_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(c.get<fr1_e_>().pack(bref));
      break;
    case types::fr2:
      HANDLE_CODE(c.get<fr2_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "supported_bw_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_bw_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fr1:
      HANDLE_CODE(c.get<fr1_e_>().unpack(bref));
      break;
    case types::fr2:
      HANDLE_CODE(c.get<fr2_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "supported_bw_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* supported_bw_c::fr1_opts::to_string() const
{
  static const char* names[] = {
      "mhz5", "mhz10", "mhz15", "mhz20", "mhz25", "mhz30", "mhz40", "mhz50", "mhz60", "mhz80", "mhz100"};
  return convert_enum_idx(names, 11, value, "supported_bw_c::fr1_e_");
}
uint8_t supported_bw_c::fr1_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 15, 20, 25, 30, 40, 50, 60, 80, 100};
  return map_enum_number(numbers, 11, value, "supported_bw_c::fr1_e_");
}

const char* supported_bw_c::fr2_opts::to_string() const
{
  static const char* names[] = {"mhz50", "mhz100", "mhz200", "mhz400"};
  return convert_enum_idx(names, 4, value, "supported_bw_c::fr2_e_");
}
uint16_t supported_bw_c::fr2_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 200, 400};
  return map_enum_number(numbers, 4, value, "supported_bw_c::fr2_e_");
}

// FeatureSetDownlinkPerCC ::= SEQUENCE
SRSASN_CODE feature_set_dl_per_cc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ch_bw_90mhz_present, 1));
  HANDLE_CODE(bref.pack(max_num_mimo_layers_pdsch_present, 1));
  HANDLE_CODE(bref.pack(supported_mod_order_dl_present, 1));

  HANDLE_CODE(supported_subcarrier_spacing_dl.pack(bref));
  HANDLE_CODE(supported_bw_dl.pack(bref));
  if (max_num_mimo_layers_pdsch_present) {
    HANDLE_CODE(max_num_mimo_layers_pdsch.pack(bref));
  }
  if (supported_mod_order_dl_present) {
    HANDLE_CODE(supported_mod_order_dl.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_dl_per_cc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ch_bw_90mhz_present, 1));
  HANDLE_CODE(bref.unpack(max_num_mimo_layers_pdsch_present, 1));
  HANDLE_CODE(bref.unpack(supported_mod_order_dl_present, 1));

  HANDLE_CODE(supported_subcarrier_spacing_dl.unpack(bref));
  HANDLE_CODE(supported_bw_dl.unpack(bref));
  if (max_num_mimo_layers_pdsch_present) {
    HANDLE_CODE(max_num_mimo_layers_pdsch.unpack(bref));
  }
  if (supported_mod_order_dl_present) {
    HANDLE_CODE(supported_mod_order_dl.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void feature_set_dl_per_cc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("supportedSubcarrierSpacingDL", supported_subcarrier_spacing_dl.to_string());
  j.write_fieldname("supportedBandwidthDL");
  supported_bw_dl.to_json(j);
  if (ch_bw_90mhz_present) {
    j.write_str("channelBW-90mhz", "supported");
  }
  if (max_num_mimo_layers_pdsch_present) {
    j.write_str("maxNumberMIMO-LayersPDSCH", max_num_mimo_layers_pdsch.to_string());
  }
  if (supported_mod_order_dl_present) {
    j.write_str("supportedModulationOrderDL", supported_mod_order_dl.to_string());
  }
  j.end_obj();
}

// DummyF ::= SEQUENCE
SRSASN_CODE dummy_f_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, max_num_periodic_csi_report_per_bwp, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_aperiodic_csi_report_per_bwp, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, max_num_semi_persistent_csi_report_per_bwp, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, simul_csi_reports_all_cc, (uint8_t)5u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_f_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(max_num_periodic_csi_report_per_bwp, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_aperiodic_csi_report_per_bwp, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(max_num_semi_persistent_csi_report_per_bwp, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(simul_csi_reports_all_cc, bref, (uint8_t)5u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
void dummy_f_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxNumberPeriodicCSI-ReportPerBWP", max_num_periodic_csi_report_per_bwp);
  j.write_int("maxNumberAperiodicCSI-ReportPerBWP", max_num_aperiodic_csi_report_per_bwp);
  j.write_int("maxNumberSemiPersistentCSI-ReportPerBWP", max_num_semi_persistent_csi_report_per_bwp);
  j.write_int("simultaneousCSI-ReportsAllCC", simul_csi_reports_all_cc);
  j.end_obj();
}

// DummyI ::= SEQUENCE
SRSASN_CODE dummy_i_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_switch_impact_to_rx_present, 1));

  HANDLE_CODE(supported_srs_tx_port_switch.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_i_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_switch_impact_to_rx_present, 1));

  HANDLE_CODE(supported_srs_tx_port_switch.unpack(bref));

  return SRSASN_SUCCESS;
}
void dummy_i_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("supportedSRS-TxPortSwitch", supported_srs_tx_port_switch.to_string());
  if (tx_switch_impact_to_rx_present) {
    j.write_str("txSwitchImpactToRx", "true");
  }
  j.end_obj();
}

const char* dummy_i_s::supported_srs_tx_port_switch_opts::to_string() const
{
  static const char* names[] = {"t1r2", "t1r4", "t2r4", "t1r4-t2r4", "tr-equal"};
  return convert_enum_idx(names, 5, value, "dummy_i_s::supported_srs_tx_port_switch_e_");
}

// FeatureSetUplink ::= SEQUENCE
SRSASN_CODE feature_set_ul_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scaling_factor_present, 1));
  HANDLE_CODE(bref.pack(cross_carrier_sched_other_scs_present, 1));
  HANDLE_CODE(bref.pack(intra_band_freq_separation_ul_present, 1));
  HANDLE_CODE(bref.pack(search_space_sharing_ca_ul_present, 1));
  HANDLE_CODE(bref.pack(dummy1_present, 1));
  HANDLE_CODE(bref.pack(supported_srs_res_present, 1));
  HANDLE_CODE(bref.pack(two_pucch_group_present, 1));
  HANDLE_CODE(bref.pack(dyn_switch_sul_present, 1));
  HANDLE_CODE(bref.pack(simul_tx_sul_non_sul_present, 1));
  HANDLE_CODE(bref.pack(pusch_processing_type1_different_tb_per_slot_present, 1));
  HANDLE_CODE(bref.pack(dummy2_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, feature_set_list_per_ul_cc, 1, 32, integer_packer<uint16_t>(1, 1024)));
  if (scaling_factor_present) {
    HANDLE_CODE(scaling_factor.pack(bref));
  }
  if (intra_band_freq_separation_ul_present) {
    HANDLE_CODE(intra_band_freq_separation_ul.pack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.pack(bref));
  }
  if (supported_srs_res_present) {
    HANDLE_CODE(supported_srs_res.pack(bref));
  }
  if (pusch_processing_type1_different_tb_per_slot_present) {
    HANDLE_CODE(bref.pack(pusch_processing_type1_different_tb_per_slot.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(pusch_processing_type1_different_tb_per_slot.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(pusch_processing_type1_different_tb_per_slot.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.pack(pusch_processing_type1_different_tb_per_slot.scs_120k_hz_present, 1));
    if (pusch_processing_type1_different_tb_per_slot.scs_15k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_15k_hz.pack(bref));
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_30k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_30k_hz.pack(bref));
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_60k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_60k_hz.pack(bref));
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_120k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_120k_hz.pack(bref));
    }
  }
  if (dummy2_present) {
    HANDLE_CODE(dummy2.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_ul_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scaling_factor_present, 1));
  HANDLE_CODE(bref.unpack(cross_carrier_sched_other_scs_present, 1));
  HANDLE_CODE(bref.unpack(intra_band_freq_separation_ul_present, 1));
  HANDLE_CODE(bref.unpack(search_space_sharing_ca_ul_present, 1));
  HANDLE_CODE(bref.unpack(dummy1_present, 1));
  HANDLE_CODE(bref.unpack(supported_srs_res_present, 1));
  HANDLE_CODE(bref.unpack(two_pucch_group_present, 1));
  HANDLE_CODE(bref.unpack(dyn_switch_sul_present, 1));
  HANDLE_CODE(bref.unpack(simul_tx_sul_non_sul_present, 1));
  HANDLE_CODE(bref.unpack(pusch_processing_type1_different_tb_per_slot_present, 1));
  HANDLE_CODE(bref.unpack(dummy2_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(feature_set_list_per_ul_cc, bref, 1, 32, integer_packer<uint16_t>(1, 1024)));
  if (scaling_factor_present) {
    HANDLE_CODE(scaling_factor.unpack(bref));
  }
  if (intra_band_freq_separation_ul_present) {
    HANDLE_CODE(intra_band_freq_separation_ul.unpack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.unpack(bref));
  }
  if (supported_srs_res_present) {
    HANDLE_CODE(supported_srs_res.unpack(bref));
  }
  if (pusch_processing_type1_different_tb_per_slot_present) {
    HANDLE_CODE(bref.unpack(pusch_processing_type1_different_tb_per_slot.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pusch_processing_type1_different_tb_per_slot.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pusch_processing_type1_different_tb_per_slot.scs_60k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pusch_processing_type1_different_tb_per_slot.scs_120k_hz_present, 1));
    if (pusch_processing_type1_different_tb_per_slot.scs_15k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_15k_hz.unpack(bref));
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_30k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_30k_hz.unpack(bref));
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_60k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_60k_hz.unpack(bref));
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_120k_hz_present) {
      HANDLE_CODE(pusch_processing_type1_different_tb_per_slot.scs_120k_hz.unpack(bref));
    }
  }
  if (dummy2_present) {
    HANDLE_CODE(dummy2.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void feature_set_ul_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("featureSetListPerUplinkCC");
  for (const auto& e1 : feature_set_list_per_ul_cc) {
    j.write_int(e1);
  }
  j.end_array();
  if (scaling_factor_present) {
    j.write_str("scalingFactor", scaling_factor.to_string());
  }
  if (cross_carrier_sched_other_scs_present) {
    j.write_str("crossCarrierScheduling-OtherSCS", "supported");
  }
  if (intra_band_freq_separation_ul_present) {
    j.write_str("intraBandFreqSeparationUL", intra_band_freq_separation_ul.to_string());
  }
  if (search_space_sharing_ca_ul_present) {
    j.write_str("searchSpaceSharingCA-UL", "supported");
  }
  if (dummy1_present) {
    j.write_fieldname("dummy1");
    dummy1.to_json(j);
  }
  if (supported_srs_res_present) {
    j.write_fieldname("supportedSRS-Resources");
    supported_srs_res.to_json(j);
  }
  if (two_pucch_group_present) {
    j.write_str("twoPUCCH-Group", "supported");
  }
  if (dyn_switch_sul_present) {
    j.write_str("dynamicSwitchSUL", "supported");
  }
  if (simul_tx_sul_non_sul_present) {
    j.write_str("simultaneousTxSUL-NonSUL", "supported");
  }
  if (pusch_processing_type1_different_tb_per_slot_present) {
    j.write_fieldname("pusch-ProcessingType1-DifferentTB-PerSlot");
    j.start_obj();
    if (pusch_processing_type1_different_tb_per_slot.scs_15k_hz_present) {
      j.write_str("scs-15kHz", pusch_processing_type1_different_tb_per_slot.scs_15k_hz.to_string());
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_30k_hz_present) {
      j.write_str("scs-30kHz", pusch_processing_type1_different_tb_per_slot.scs_30k_hz.to_string());
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_60k_hz_present) {
      j.write_str("scs-60kHz", pusch_processing_type1_different_tb_per_slot.scs_60k_hz.to_string());
    }
    if (pusch_processing_type1_different_tb_per_slot.scs_120k_hz_present) {
      j.write_str("scs-120kHz", pusch_processing_type1_different_tb_per_slot.scs_120k_hz.to_string());
    }
    j.end_obj();
  }
  if (dummy2_present) {
    j.write_fieldname("dummy2");
    dummy2.to_json(j);
  }
  j.end_obj();
}

const char* feature_set_ul_s::scaling_factor_opts::to_string() const
{
  static const char* names[] = {"f0p4", "f0p75", "f0p8"};
  return convert_enum_idx(names, 3, value, "feature_set_ul_s::scaling_factor_e_");
}

const char* feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_e_");
}
uint8_t feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_15k_hz_e_");
}

const char* feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_e_");
}
uint8_t feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_30k_hz_e_");
}

const char* feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_e_");
}
uint8_t feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_60k_hz_e_");
}

const char* feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_opts::to_string() const
{
  static const char* names[] = {"upto2", "upto4", "upto7"};
  return convert_enum_idx(
      names, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_e_");
}
uint8_t feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 7};
  return map_enum_number(
      numbers, 3, value, "feature_set_ul_s::pusch_processing_type1_different_tb_per_slot_s_::scs_120k_hz_e_");
}

// FeatureSetUplink-v1540 ::= SEQUENCE
SRSASN_CODE feature_set_ul_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(zero_slot_offset_aperiodic_srs_present, 1));
  HANDLE_CODE(bref.pack(pa_phase_discontinuity_impacts_present, 1));
  HANDLE_CODE(bref.pack(pusch_separation_with_gap_present, 1));
  HANDLE_CODE(bref.pack(pusch_processing_type2_present, 1));
  HANDLE_CODE(bref.pack(ul_mcs_table_alt_dyn_ind_present, 1));

  if (pusch_processing_type2_present) {
    HANDLE_CODE(bref.pack(pusch_processing_type2.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.pack(pusch_processing_type2.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.pack(pusch_processing_type2.scs_60k_hz_present, 1));
    if (pusch_processing_type2.scs_15k_hz_present) {
      HANDLE_CODE(pusch_processing_type2.scs_15k_hz.pack(bref));
    }
    if (pusch_processing_type2.scs_30k_hz_present) {
      HANDLE_CODE(pusch_processing_type2.scs_30k_hz.pack(bref));
    }
    if (pusch_processing_type2.scs_60k_hz_present) {
      HANDLE_CODE(pusch_processing_type2.scs_60k_hz.pack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_ul_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(zero_slot_offset_aperiodic_srs_present, 1));
  HANDLE_CODE(bref.unpack(pa_phase_discontinuity_impacts_present, 1));
  HANDLE_CODE(bref.unpack(pusch_separation_with_gap_present, 1));
  HANDLE_CODE(bref.unpack(pusch_processing_type2_present, 1));
  HANDLE_CODE(bref.unpack(ul_mcs_table_alt_dyn_ind_present, 1));

  if (pusch_processing_type2_present) {
    HANDLE_CODE(bref.unpack(pusch_processing_type2.scs_15k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pusch_processing_type2.scs_30k_hz_present, 1));
    HANDLE_CODE(bref.unpack(pusch_processing_type2.scs_60k_hz_present, 1));
    if (pusch_processing_type2.scs_15k_hz_present) {
      HANDLE_CODE(pusch_processing_type2.scs_15k_hz.unpack(bref));
    }
    if (pusch_processing_type2.scs_30k_hz_present) {
      HANDLE_CODE(pusch_processing_type2.scs_30k_hz.unpack(bref));
    }
    if (pusch_processing_type2.scs_60k_hz_present) {
      HANDLE_CODE(pusch_processing_type2.scs_60k_hz.unpack(bref));
    }
  }

  return SRSASN_SUCCESS;
}
void feature_set_ul_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (zero_slot_offset_aperiodic_srs_present) {
    j.write_str("zeroSlotOffsetAperiodicSRS", "supported");
  }
  if (pa_phase_discontinuity_impacts_present) {
    j.write_str("pa-PhaseDiscontinuityImpacts", "supported");
  }
  if (pusch_separation_with_gap_present) {
    j.write_str("pusch-SeparationWithGap", "supported");
  }
  if (pusch_processing_type2_present) {
    j.write_fieldname("pusch-ProcessingType2");
    j.start_obj();
    if (pusch_processing_type2.scs_15k_hz_present) {
      j.write_fieldname("scs-15kHz");
      pusch_processing_type2.scs_15k_hz.to_json(j);
    }
    if (pusch_processing_type2.scs_30k_hz_present) {
      j.write_fieldname("scs-30kHz");
      pusch_processing_type2.scs_30k_hz.to_json(j);
    }
    if (pusch_processing_type2.scs_60k_hz_present) {
      j.write_fieldname("scs-60kHz");
      pusch_processing_type2.scs_60k_hz.to_json(j);
    }
    j.end_obj();
  }
  if (ul_mcs_table_alt_dyn_ind_present) {
    j.write_str("ul-MCS-TableAlt-DynamicIndication", "supported");
  }
  j.end_obj();
}

// FeatureSetUplinkPerCC ::= SEQUENCE
SRSASN_CODE feature_set_ul_per_cc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ch_bw_90mhz_present, 1));
  HANDLE_CODE(bref.pack(mimo_cb_pusch_present, 1));
  HANDLE_CODE(bref.pack(max_num_mimo_layers_non_cb_pusch_present, 1));
  HANDLE_CODE(bref.pack(supported_mod_order_ul_present, 1));

  HANDLE_CODE(supported_subcarrier_spacing_ul.pack(bref));
  HANDLE_CODE(supported_bw_ul.pack(bref));
  if (mimo_cb_pusch_present) {
    HANDLE_CODE(bref.pack(mimo_cb_pusch.max_num_mimo_layers_cb_pusch_present, 1));
    if (mimo_cb_pusch.max_num_mimo_layers_cb_pusch_present) {
      HANDLE_CODE(mimo_cb_pusch.max_num_mimo_layers_cb_pusch.pack(bref));
    }
    HANDLE_CODE(pack_integer(bref, mimo_cb_pusch.max_num_srs_res_per_set, (uint8_t)1u, (uint8_t)2u));
  }
  if (max_num_mimo_layers_non_cb_pusch_present) {
    HANDLE_CODE(max_num_mimo_layers_non_cb_pusch.pack(bref));
  }
  if (supported_mod_order_ul_present) {
    HANDLE_CODE(supported_mod_order_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_ul_per_cc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ch_bw_90mhz_present, 1));
  HANDLE_CODE(bref.unpack(mimo_cb_pusch_present, 1));
  HANDLE_CODE(bref.unpack(max_num_mimo_layers_non_cb_pusch_present, 1));
  HANDLE_CODE(bref.unpack(supported_mod_order_ul_present, 1));

  HANDLE_CODE(supported_subcarrier_spacing_ul.unpack(bref));
  HANDLE_CODE(supported_bw_ul.unpack(bref));
  if (mimo_cb_pusch_present) {
    HANDLE_CODE(bref.unpack(mimo_cb_pusch.max_num_mimo_layers_cb_pusch_present, 1));
    if (mimo_cb_pusch.max_num_mimo_layers_cb_pusch_present) {
      HANDLE_CODE(mimo_cb_pusch.max_num_mimo_layers_cb_pusch.unpack(bref));
    }
    HANDLE_CODE(unpack_integer(mimo_cb_pusch.max_num_srs_res_per_set, bref, (uint8_t)1u, (uint8_t)2u));
  }
  if (max_num_mimo_layers_non_cb_pusch_present) {
    HANDLE_CODE(max_num_mimo_layers_non_cb_pusch.unpack(bref));
  }
  if (supported_mod_order_ul_present) {
    HANDLE_CODE(supported_mod_order_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void feature_set_ul_per_cc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("supportedSubcarrierSpacingUL", supported_subcarrier_spacing_ul.to_string());
  j.write_fieldname("supportedBandwidthUL");
  supported_bw_ul.to_json(j);
  if (ch_bw_90mhz_present) {
    j.write_str("channelBW-90mhz", "supported");
  }
  if (mimo_cb_pusch_present) {
    j.write_fieldname("mimo-CB-PUSCH");
    j.start_obj();
    if (mimo_cb_pusch.max_num_mimo_layers_cb_pusch_present) {
      j.write_str("maxNumberMIMO-LayersCB-PUSCH", mimo_cb_pusch.max_num_mimo_layers_cb_pusch.to_string());
    }
    j.write_int("maxNumberSRS-ResourcePerSet", mimo_cb_pusch.max_num_srs_res_per_set);
    j.end_obj();
  }
  if (max_num_mimo_layers_non_cb_pusch_present) {
    j.write_str("maxNumberMIMO-LayersNonCB-PUSCH", max_num_mimo_layers_non_cb_pusch.to_string());
  }
  if (supported_mod_order_ul_present) {
    j.write_str("supportedModulationOrderUL", supported_mod_order_ul.to_string());
  }
  j.end_obj();
}

// FeatureSetUplinkPerCC-v1540 ::= SEQUENCE
SRSASN_CODE feature_set_ul_per_cc_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mimo_non_cb_pusch_present, 1));

  if (mimo_non_cb_pusch_present) {
    HANDLE_CODE(pack_integer(bref, mimo_non_cb_pusch.max_num_srs_res_per_set, (uint8_t)1u, (uint8_t)4u));
    HANDLE_CODE(pack_integer(bref, mimo_non_cb_pusch.max_num_simul_srs_res_tx, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_set_ul_per_cc_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mimo_non_cb_pusch_present, 1));

  if (mimo_non_cb_pusch_present) {
    HANDLE_CODE(unpack_integer(mimo_non_cb_pusch.max_num_srs_res_per_set, bref, (uint8_t)1u, (uint8_t)4u));
    HANDLE_CODE(unpack_integer(mimo_non_cb_pusch.max_num_simul_srs_res_tx, bref, (uint8_t)1u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
void feature_set_ul_per_cc_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mimo_non_cb_pusch_present) {
    j.write_fieldname("mimo-NonCB-PUSCH");
    j.start_obj();
    j.write_int("maxNumberSRS-ResourcePerSet", mimo_non_cb_pusch.max_num_srs_res_per_set);
    j.write_int("maxNumberSimultaneousSRS-ResourceTx", mimo_non_cb_pusch.max_num_simul_srs_res_tx);
    j.end_obj();
  }
  j.end_obj();
}

// FeatureSets ::= SEQUENCE
SRSASN_CODE feature_sets_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(feature_sets_dl.size() > 0, 1));
  HANDLE_CODE(bref.pack(feature_sets_dl_per_cc.size() > 0, 1));
  HANDLE_CODE(bref.pack(feature_sets_ul.size() > 0, 1));
  HANDLE_CODE(bref.pack(feature_sets_ul_per_cc.size() > 0, 1));

  if (feature_sets_dl.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_dl, 1, 1024));
  }
  if (feature_sets_dl_per_cc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_dl_per_cc, 1, 1024));
  }
  if (feature_sets_ul.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_ul, 1, 1024));
  }
  if (feature_sets_ul_per_cc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, feature_sets_ul_per_cc, 1, 1024));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= feature_sets_dl_v1540.is_present();
    group_flags[0] |= feature_sets_ul_v1540.is_present();
    group_flags[0] |= feature_sets_ul_per_cc_v1540.is_present();
    group_flags[1] |= feature_sets_dl_v15a0.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(feature_sets_dl_v1540.is_present(), 1));
      HANDLE_CODE(bref.pack(feature_sets_ul_v1540.is_present(), 1));
      HANDLE_CODE(bref.pack(feature_sets_ul_per_cc_v1540.is_present(), 1));
      if (feature_sets_dl_v1540.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *feature_sets_dl_v1540, 1, 1024));
      }
      if (feature_sets_ul_v1540.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *feature_sets_ul_v1540, 1, 1024));
      }
      if (feature_sets_ul_per_cc_v1540.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *feature_sets_ul_per_cc_v1540, 1, 1024));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(feature_sets_dl_v15a0.is_present(), 1));
      if (feature_sets_dl_v15a0.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *feature_sets_dl_v15a0, 1, 1024));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE feature_sets_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool feature_sets_dl_present;
  HANDLE_CODE(bref.unpack(feature_sets_dl_present, 1));
  bool feature_sets_dl_per_cc_present;
  HANDLE_CODE(bref.unpack(feature_sets_dl_per_cc_present, 1));
  bool feature_sets_ul_present;
  HANDLE_CODE(bref.unpack(feature_sets_ul_present, 1));
  bool feature_sets_ul_per_cc_present;
  HANDLE_CODE(bref.unpack(feature_sets_ul_per_cc_present, 1));

  if (feature_sets_dl_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_dl, bref, 1, 1024));
  }
  if (feature_sets_dl_per_cc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_dl_per_cc, bref, 1, 1024));
  }
  if (feature_sets_ul_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_ul, bref, 1, 1024));
  }
  if (feature_sets_ul_per_cc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_sets_ul_per_cc, bref, 1, 1024));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool feature_sets_dl_v1540_present;
      HANDLE_CODE(bref.unpack(feature_sets_dl_v1540_present, 1));
      feature_sets_dl_v1540.set_present(feature_sets_dl_v1540_present);
      bool feature_sets_ul_v1540_present;
      HANDLE_CODE(bref.unpack(feature_sets_ul_v1540_present, 1));
      feature_sets_ul_v1540.set_present(feature_sets_ul_v1540_present);
      bool feature_sets_ul_per_cc_v1540_present;
      HANDLE_CODE(bref.unpack(feature_sets_ul_per_cc_v1540_present, 1));
      feature_sets_ul_per_cc_v1540.set_present(feature_sets_ul_per_cc_v1540_present);
      if (feature_sets_dl_v1540.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*feature_sets_dl_v1540, bref, 1, 1024));
      }
      if (feature_sets_ul_v1540.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*feature_sets_ul_v1540, bref, 1, 1024));
      }
      if (feature_sets_ul_per_cc_v1540.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*feature_sets_ul_per_cc_v1540, bref, 1, 1024));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool feature_sets_dl_v15a0_present;
      HANDLE_CODE(bref.unpack(feature_sets_dl_v15a0_present, 1));
      feature_sets_dl_v15a0.set_present(feature_sets_dl_v15a0_present);
      if (feature_sets_dl_v15a0.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*feature_sets_dl_v15a0, bref, 1, 1024));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void feature_sets_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (feature_sets_dl.size() > 0) {
    j.start_array("featureSetsDownlink");
    for (const auto& e1 : feature_sets_dl) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (feature_sets_dl_per_cc.size() > 0) {
    j.start_array("featureSetsDownlinkPerCC");
    for (const auto& e1 : feature_sets_dl_per_cc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (feature_sets_ul.size() > 0) {
    j.start_array("featureSetsUplink");
    for (const auto& e1 : feature_sets_ul) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (feature_sets_ul_per_cc.size() > 0) {
    j.start_array("featureSetsUplinkPerCC");
    for (const auto& e1 : feature_sets_ul_per_cc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (feature_sets_dl_v1540.is_present()) {
      j.start_array("featureSetsDownlink-v1540");
      for (const auto& e1 : *feature_sets_dl_v1540) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (feature_sets_ul_v1540.is_present()) {
      j.start_array("featureSetsUplink-v1540");
      for (const auto& e1 : *feature_sets_ul_v1540) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (feature_sets_ul_per_cc_v1540.is_present()) {
      j.start_array("featureSetsUplinkPerCC-v1540");
      for (const auto& e1 : *feature_sets_ul_per_cc_v1540) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (feature_sets_dl_v15a0.is_present()) {
      j.start_array("featureSetsDownlink-v15a0");
      for (const auto& e1 : *feature_sets_dl_v15a0) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// AggregatedBandwidth ::= ENUMERATED
const char* aggr_bw_opts::to_string() const
{
  static const char* names[] = {"mhz50",
                                "mhz100",
                                "mhz150",
                                "mhz200",
                                "mhz250",
                                "mhz300",
                                "mhz350",
                                "mhz400",
                                "mhz450",
                                "mhz500",
                                "mhz550",
                                "mhz600",
                                "mhz650",
                                "mhz700",
                                "mhz750",
                                "mhz800"};
  return convert_enum_idx(names, 16, value, "aggr_bw_e");
}
uint16_t aggr_bw_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800};
  return map_enum_number(numbers, 16, value, "aggr_bw_e");
}

// FreqBandInformationEUTRA ::= SEQUENCE
SRSASN_CODE freq_band_info_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_bw_class_dl_eutra_present, 1));
  HANDLE_CODE(bref.pack(ca_bw_class_ul_eutra_present, 1));

  HANDLE_CODE(pack_integer(bref, band_eutra, (uint16_t)1u, (uint16_t)256u));
  if (ca_bw_class_dl_eutra_present) {
    HANDLE_CODE(ca_bw_class_dl_eutra.pack(bref));
  }
  if (ca_bw_class_ul_eutra_present) {
    HANDLE_CODE(ca_bw_class_ul_eutra.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_band_info_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_bw_class_dl_eutra_present, 1));
  HANDLE_CODE(bref.unpack(ca_bw_class_ul_eutra_present, 1));

  HANDLE_CODE(unpack_integer(band_eutra, bref, (uint16_t)1u, (uint16_t)256u));
  if (ca_bw_class_dl_eutra_present) {
    HANDLE_CODE(ca_bw_class_dl_eutra.unpack(bref));
  }
  if (ca_bw_class_ul_eutra_present) {
    HANDLE_CODE(ca_bw_class_ul_eutra.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void freq_band_info_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandEUTRA", band_eutra);
  if (ca_bw_class_dl_eutra_present) {
    j.write_str("ca-BandwidthClassDL-EUTRA", ca_bw_class_dl_eutra.to_string());
  }
  if (ca_bw_class_ul_eutra_present) {
    j.write_str("ca-BandwidthClassUL-EUTRA", ca_bw_class_ul_eutra.to_string());
  }
  j.end_obj();
}

// FreqBandInformationNR ::= SEQUENCE
SRSASN_CODE freq_band_info_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_bw_requested_dl_present, 1));
  HANDLE_CODE(bref.pack(max_bw_requested_ul_present, 1));
  HANDLE_CODE(bref.pack(max_carriers_requested_dl_present, 1));
  HANDLE_CODE(bref.pack(max_carriers_requested_ul_present, 1));

  HANDLE_CODE(pack_integer(bref, band_nr, (uint16_t)1u, (uint16_t)1024u));
  if (max_bw_requested_dl_present) {
    HANDLE_CODE(max_bw_requested_dl.pack(bref));
  }
  if (max_bw_requested_ul_present) {
    HANDLE_CODE(max_bw_requested_ul.pack(bref));
  }
  if (max_carriers_requested_dl_present) {
    HANDLE_CODE(pack_integer(bref, max_carriers_requested_dl, (uint8_t)1u, (uint8_t)32u));
  }
  if (max_carriers_requested_ul_present) {
    HANDLE_CODE(pack_integer(bref, max_carriers_requested_ul, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_band_info_nr_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_bw_requested_dl_present, 1));
  HANDLE_CODE(bref.unpack(max_bw_requested_ul_present, 1));
  HANDLE_CODE(bref.unpack(max_carriers_requested_dl_present, 1));
  HANDLE_CODE(bref.unpack(max_carriers_requested_ul_present, 1));

  HANDLE_CODE(unpack_integer(band_nr, bref, (uint16_t)1u, (uint16_t)1024u));
  if (max_bw_requested_dl_present) {
    HANDLE_CODE(max_bw_requested_dl.unpack(bref));
  }
  if (max_bw_requested_ul_present) {
    HANDLE_CODE(max_bw_requested_ul.unpack(bref));
  }
  if (max_carriers_requested_dl_present) {
    HANDLE_CODE(unpack_integer(max_carriers_requested_dl, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (max_carriers_requested_ul_present) {
    HANDLE_CODE(unpack_integer(max_carriers_requested_ul, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void freq_band_info_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandNR", band_nr);
  if (max_bw_requested_dl_present) {
    j.write_str("maxBandwidthRequestedDL", max_bw_requested_dl.to_string());
  }
  if (max_bw_requested_ul_present) {
    j.write_str("maxBandwidthRequestedUL", max_bw_requested_ul.to_string());
  }
  if (max_carriers_requested_dl_present) {
    j.write_int("maxCarriersRequestedDL", max_carriers_requested_dl);
  }
  if (max_carriers_requested_ul_present) {
    j.write_int("maxCarriersRequestedUL", max_carriers_requested_ul);
  }
  j.end_obj();
}

// FreqBandInformation ::= CHOICE
void freq_band_info_c::destroy_()
{
  switch (type_) {
    case types::band_info_eutra:
      c.destroy<freq_band_info_eutra_s>();
      break;
    case types::band_info_nr:
      c.destroy<freq_band_info_nr_s>();
      break;
    default:
      break;
  }
}
void freq_band_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::band_info_eutra:
      c.init<freq_band_info_eutra_s>();
      break;
    case types::band_info_nr:
      c.init<freq_band_info_nr_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_band_info_c");
  }
}
freq_band_info_c::freq_band_info_c(const freq_band_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::band_info_eutra:
      c.init(other.c.get<freq_band_info_eutra_s>());
      break;
    case types::band_info_nr:
      c.init(other.c.get<freq_band_info_nr_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_band_info_c");
  }
}
freq_band_info_c& freq_band_info_c::operator=(const freq_band_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::band_info_eutra:
      c.set(other.c.get<freq_band_info_eutra_s>());
      break;
    case types::band_info_nr:
      c.set(other.c.get<freq_band_info_nr_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_band_info_c");
  }

  return *this;
}
freq_band_info_eutra_s& freq_band_info_c::set_band_info_eutra()
{
  set(types::band_info_eutra);
  return c.get<freq_band_info_eutra_s>();
}
freq_band_info_nr_s& freq_band_info_c::set_band_info_nr()
{
  set(types::band_info_nr);
  return c.get<freq_band_info_nr_s>();
}
void freq_band_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::band_info_eutra:
      j.write_fieldname("bandInformationEUTRA");
      c.get<freq_band_info_eutra_s>().to_json(j);
      break;
    case types::band_info_nr:
      j.write_fieldname("bandInformationNR");
      c.get<freq_band_info_nr_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "freq_band_info_c");
  }
  j.end_obj();
}
SRSASN_CODE freq_band_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::band_info_eutra:
      HANDLE_CODE(c.get<freq_band_info_eutra_s>().pack(bref));
      break;
    case types::band_info_nr:
      HANDLE_CODE(c.get<freq_band_info_nr_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_band_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_band_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::band_info_eutra:
      HANDLE_CODE(c.get<freq_band_info_eutra_s>().unpack(bref));
      break;
    case types::band_info_nr:
      HANDLE_CODE(c.get<freq_band_info_nr_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_band_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// IMS-ParametersCommon ::= SEQUENCE
SRSASN_CODE ims_params_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(voice_over_eutra_5_gc_present, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= voice_over_scg_bearer_eutra_5_gc_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(voice_over_scg_bearer_eutra_5_gc_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ims_params_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(voice_over_eutra_5_gc_present, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(voice_over_scg_bearer_eutra_5_gc_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void ims_params_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (voice_over_eutra_5_gc_present) {
    j.write_str("voiceOverEUTRA-5GC", "supported");
  }
  if (ext) {
    if (voice_over_scg_bearer_eutra_5_gc_present) {
      j.write_str("voiceOverSCG-BearerEUTRA-5GC", "supported");
    }
  }
  j.end_obj();
}

// IMS-ParametersFRX-Diff ::= SEQUENCE
SRSASN_CODE ims_params_frx_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(voice_over_nr_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ims_params_frx_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(voice_over_nr_present, 1));

  return SRSASN_SUCCESS;
}
void ims_params_frx_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (voice_over_nr_present) {
    j.write_str("voiceOverNR", "supported");
  }
  j.end_obj();
}

// IMS-Parameters ::= SEQUENCE
SRSASN_CODE ims_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ims_params_common_present, 1));
  HANDLE_CODE(bref.pack(ims_params_frx_diff_present, 1));

  if (ims_params_common_present) {
    HANDLE_CODE(ims_params_common.pack(bref));
  }
  if (ims_params_frx_diff_present) {
    HANDLE_CODE(ims_params_frx_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ims_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ims_params_common_present, 1));
  HANDLE_CODE(bref.unpack(ims_params_frx_diff_present, 1));

  if (ims_params_common_present) {
    HANDLE_CODE(ims_params_common.unpack(bref));
  }
  if (ims_params_frx_diff_present) {
    HANDLE_CODE(ims_params_frx_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ims_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ims_params_common_present) {
    j.write_fieldname("ims-ParametersCommon");
    ims_params_common.to_json(j);
  }
  if (ims_params_frx_diff_present) {
    j.write_fieldname("ims-ParametersFRX-Diff");
    ims_params_frx_diff.to_json(j);
  }
  j.end_obj();
}

// InterRAT-Parameters ::= SEQUENCE
SRSASN_CODE inter_rat_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(eutra_present, 1));

  if (eutra_present) {
    HANDLE_CODE(eutra.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_rat_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(eutra_present, 1));

  if (eutra_present) {
    HANDLE_CODE(eutra.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_rat_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_present) {
    j.write_fieldname("eutra");
    eutra.to_json(j);
  }
  j.end_obj();
}

// MAC-ParametersCommon ::= SEQUENCE
SRSASN_CODE mac_params_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lcp_restrict_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(lch_to_scell_restrict_present, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= recommended_bit_rate_present;
    group_flags[0] |= recommended_bit_rate_query_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(recommended_bit_rate_present, 1));
      HANDLE_CODE(bref.pack(recommended_bit_rate_query_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lcp_restrict_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(lch_to_scell_restrict_present, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(recommended_bit_rate_present, 1));
      HANDLE_CODE(bref.unpack(recommended_bit_rate_query_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void mac_params_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (lcp_restrict_present) {
    j.write_str("lcp-Restriction", "supported");
  }
  if (dummy_present) {
    j.write_str("dummy", "supported");
  }
  if (lch_to_scell_restrict_present) {
    j.write_str("lch-ToSCellRestriction", "supported");
  }
  if (ext) {
    if (recommended_bit_rate_present) {
      j.write_str("recommendedBitRate", "supported");
    }
    if (recommended_bit_rate_query_present) {
      j.write_str("recommendedBitRateQuery", "supported");
    }
  }
  j.end_obj();
}

// MAC-ParametersXDD-Diff ::= SEQUENCE
SRSASN_CODE mac_params_xdd_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(skip_ul_tx_dyn_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_sr_delay_timer_present, 1));
  HANDLE_CODE(bref.pack(long_drx_cycle_present, 1));
  HANDLE_CODE(bref.pack(short_drx_cycle_present, 1));
  HANDLE_CODE(bref.pack(multiple_sr_cfgs_present, 1));
  HANDLE_CODE(bref.pack(multiple_cfg_grants_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_xdd_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(skip_ul_tx_dyn_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_sr_delay_timer_present, 1));
  HANDLE_CODE(bref.unpack(long_drx_cycle_present, 1));
  HANDLE_CODE(bref.unpack(short_drx_cycle_present, 1));
  HANDLE_CODE(bref.unpack(multiple_sr_cfgs_present, 1));
  HANDLE_CODE(bref.unpack(multiple_cfg_grants_present, 1));

  return SRSASN_SUCCESS;
}
void mac_params_xdd_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (skip_ul_tx_dyn_present) {
    j.write_str("skipUplinkTxDynamic", "supported");
  }
  if (lc_ch_sr_delay_timer_present) {
    j.write_str("logicalChannelSR-DelayTimer", "supported");
  }
  if (long_drx_cycle_present) {
    j.write_str("longDRX-Cycle", "supported");
  }
  if (short_drx_cycle_present) {
    j.write_str("shortDRX-Cycle", "supported");
  }
  if (multiple_sr_cfgs_present) {
    j.write_str("multipleSR-Configurations", "supported");
  }
  if (multiple_cfg_grants_present) {
    j.write_str("multipleConfiguredGrants", "supported");
  }
  j.end_obj();
}

// MAC-Parameters ::= SEQUENCE
SRSASN_CODE mac_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mac_params_common_present, 1));
  HANDLE_CODE(bref.pack(mac_params_xdd_diff_present, 1));

  if (mac_params_common_present) {
    HANDLE_CODE(mac_params_common.pack(bref));
  }
  if (mac_params_xdd_diff_present) {
    HANDLE_CODE(mac_params_xdd_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mac_params_common_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_xdd_diff_present, 1));

  if (mac_params_common_present) {
    HANDLE_CODE(mac_params_common.unpack(bref));
  }
  if (mac_params_xdd_diff_present) {
    HANDLE_CODE(mac_params_xdd_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mac_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mac_params_common_present) {
    j.write_fieldname("mac-ParametersCommon");
    mac_params_common.to_json(j);
  }
  if (mac_params_xdd_diff_present) {
    j.write_fieldname("mac-ParametersXDD-Diff");
    mac_params_xdd_diff.to_json(j);
  }
  j.end_obj();
}

// MeasAndMobParametersCommon ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(supported_gap_pattern_present, 1));
  HANDLE_CODE(bref.pack(ssb_rlm_present, 1));
  HANDLE_CODE(bref.pack(ssb_and_csi_rs_rlm_present, 1));

  if (supported_gap_pattern_present) {
    HANDLE_CODE(supported_gap_pattern.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= event_b_meas_and_report_present;
    group_flags[0] |= ho_fdd_tdd_present;
    group_flags[0] |= eutra_cgi_report_present;
    group_flags[0] |= nr_cgi_report_present;
    group_flags[1] |= independent_gap_cfg_present;
    group_flags[1] |= periodic_eutra_meas_and_report_present;
    group_flags[1] |= ho_fr1_fr2_present;
    group_flags[1] |= max_num_csi_rs_rrm_rs_sinr_present;
    group_flags[2] |= nr_cgi_report_endc_present;
    group_flags[3] |= eutra_cgi_report_nedc_present;
    group_flags[3] |= eutra_cgi_report_nrdc_present;
    group_flags[3] |= nr_cgi_report_nedc_present;
    group_flags[3] |= nr_cgi_report_nrdc_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(event_b_meas_and_report_present, 1));
      HANDLE_CODE(bref.pack(ho_fdd_tdd_present, 1));
      HANDLE_CODE(bref.pack(eutra_cgi_report_present, 1));
      HANDLE_CODE(bref.pack(nr_cgi_report_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(independent_gap_cfg_present, 1));
      HANDLE_CODE(bref.pack(periodic_eutra_meas_and_report_present, 1));
      HANDLE_CODE(bref.pack(ho_fr1_fr2_present, 1));
      HANDLE_CODE(bref.pack(max_num_csi_rs_rrm_rs_sinr_present, 1));
      if (max_num_csi_rs_rrm_rs_sinr_present) {
        HANDLE_CODE(max_num_csi_rs_rrm_rs_sinr.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nr_cgi_report_endc_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(eutra_cgi_report_nedc_present, 1));
      HANDLE_CODE(bref.pack(eutra_cgi_report_nrdc_present, 1));
      HANDLE_CODE(bref.pack(nr_cgi_report_nedc_present, 1));
      HANDLE_CODE(bref.pack(nr_cgi_report_nrdc_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(supported_gap_pattern_present, 1));
  HANDLE_CODE(bref.unpack(ssb_rlm_present, 1));
  HANDLE_CODE(bref.unpack(ssb_and_csi_rs_rlm_present, 1));

  if (supported_gap_pattern_present) {
    HANDLE_CODE(supported_gap_pattern.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(event_b_meas_and_report_present, 1));
      HANDLE_CODE(bref.unpack(ho_fdd_tdd_present, 1));
      HANDLE_CODE(bref.unpack(eutra_cgi_report_present, 1));
      HANDLE_CODE(bref.unpack(nr_cgi_report_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(independent_gap_cfg_present, 1));
      HANDLE_CODE(bref.unpack(periodic_eutra_meas_and_report_present, 1));
      HANDLE_CODE(bref.unpack(ho_fr1_fr2_present, 1));
      HANDLE_CODE(bref.unpack(max_num_csi_rs_rrm_rs_sinr_present, 1));
      if (max_num_csi_rs_rrm_rs_sinr_present) {
        HANDLE_CODE(max_num_csi_rs_rrm_rs_sinr.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(nr_cgi_report_endc_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(eutra_cgi_report_nedc_present, 1));
      HANDLE_CODE(bref.unpack(eutra_cgi_report_nrdc_present, 1));
      HANDLE_CODE(bref.unpack(nr_cgi_report_nedc_present, 1));
      HANDLE_CODE(bref.unpack(nr_cgi_report_nrdc_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_and_mob_params_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_gap_pattern_present) {
    j.write_str("supportedGapPattern", supported_gap_pattern.to_string());
  }
  if (ssb_rlm_present) {
    j.write_str("ssb-RLM", "supported");
  }
  if (ssb_and_csi_rs_rlm_present) {
    j.write_str("ssb-AndCSI-RS-RLM", "supported");
  }
  if (ext) {
    if (event_b_meas_and_report_present) {
      j.write_str("eventB-MeasAndReport", "supported");
    }
    if (ho_fdd_tdd_present) {
      j.write_str("handoverFDD-TDD", "supported");
    }
    if (eutra_cgi_report_present) {
      j.write_str("eutra-CGI-Reporting", "supported");
    }
    if (nr_cgi_report_present) {
      j.write_str("nr-CGI-Reporting", "supported");
    }
    if (independent_gap_cfg_present) {
      j.write_str("independentGapConfig", "supported");
    }
    if (periodic_eutra_meas_and_report_present) {
      j.write_str("periodicEUTRA-MeasAndReport", "supported");
    }
    if (ho_fr1_fr2_present) {
      j.write_str("handoverFR1-FR2", "supported");
    }
    if (max_num_csi_rs_rrm_rs_sinr_present) {
      j.write_str("maxNumberCSI-RS-RRM-RS-SINR", max_num_csi_rs_rrm_rs_sinr.to_string());
    }
    if (nr_cgi_report_endc_present) {
      j.write_str("nr-CGI-Reporting-ENDC", "supported");
    }
    if (eutra_cgi_report_nedc_present) {
      j.write_str("eutra-CGI-Reporting-NEDC", "supported");
    }
    if (eutra_cgi_report_nrdc_present) {
      j.write_str("eutra-CGI-Reporting-NRDC", "supported");
    }
    if (nr_cgi_report_nedc_present) {
      j.write_str("nr-CGI-Reporting-NEDC", "supported");
    }
    if (nr_cgi_report_nrdc_present) {
      j.write_str("nr-CGI-Reporting-NRDC", "supported");
    }
  }
  j.end_obj();
}

const char* meas_and_mob_params_common_s::max_num_csi_rs_rrm_rs_sinr_opts::to_string() const
{
  static const char* names[] = {"n4", "n8", "n16", "n32", "n64", "n96"};
  return convert_enum_idx(names, 6, value, "meas_and_mob_params_common_s::max_num_csi_rs_rrm_rs_sinr_e_");
}
uint8_t meas_and_mob_params_common_s::max_num_csi_rs_rrm_rs_sinr_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16, 32, 64, 96};
  return map_enum_number(numbers, 6, value, "meas_and_mob_params_common_s::max_num_csi_rs_rrm_rs_sinr_e_");
}

// MeasAndMobParametersFRX-Diff ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_frx_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ss_sinr_meas_present, 1));
  HANDLE_CODE(bref.pack(csi_rsrp_and_rsrq_meas_with_ssb_present, 1));
  HANDLE_CODE(bref.pack(csi_rsrp_and_rsrq_meas_without_ssb_present, 1));
  HANDLE_CODE(bref.pack(csi_sinr_meas_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_rlm_present, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ho_inter_f_present;
    group_flags[0] |= ho_lte_epc_present;
    group_flags[0] |= ho_lte_5_gc_present;
    group_flags[1] |= max_num_res_csi_rs_rlm_present;
    group_flags[2] |= simul_rx_data_ssb_diff_numerology_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ho_inter_f_present, 1));
      HANDLE_CODE(bref.pack(ho_lte_epc_present, 1));
      HANDLE_CODE(bref.pack(ho_lte_5_gc_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_num_res_csi_rs_rlm_present, 1));
      if (max_num_res_csi_rs_rlm_present) {
        HANDLE_CODE(max_num_res_csi_rs_rlm.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(simul_rx_data_ssb_diff_numerology_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_frx_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ss_sinr_meas_present, 1));
  HANDLE_CODE(bref.unpack(csi_rsrp_and_rsrq_meas_with_ssb_present, 1));
  HANDLE_CODE(bref.unpack(csi_rsrp_and_rsrq_meas_without_ssb_present, 1));
  HANDLE_CODE(bref.unpack(csi_sinr_meas_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_rlm_present, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ho_inter_f_present, 1));
      HANDLE_CODE(bref.unpack(ho_lte_epc_present, 1));
      HANDLE_CODE(bref.unpack(ho_lte_5_gc_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_num_res_csi_rs_rlm_present, 1));
      if (max_num_res_csi_rs_rlm_present) {
        HANDLE_CODE(max_num_res_csi_rs_rlm.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(simul_rx_data_ssb_diff_numerology_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_and_mob_params_frx_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ss_sinr_meas_present) {
    j.write_str("ss-SINR-Meas", "supported");
  }
  if (csi_rsrp_and_rsrq_meas_with_ssb_present) {
    j.write_str("csi-RSRP-AndRSRQ-MeasWithSSB", "supported");
  }
  if (csi_rsrp_and_rsrq_meas_without_ssb_present) {
    j.write_str("csi-RSRP-AndRSRQ-MeasWithoutSSB", "supported");
  }
  if (csi_sinr_meas_present) {
    j.write_str("csi-SINR-Meas", "supported");
  }
  if (csi_rs_rlm_present) {
    j.write_str("csi-RS-RLM", "supported");
  }
  if (ext) {
    if (ho_inter_f_present) {
      j.write_str("handoverInterF", "supported");
    }
    if (ho_lte_epc_present) {
      j.write_str("handoverLTE-EPC", "supported");
    }
    if (ho_lte_5_gc_present) {
      j.write_str("handoverLTE-5GC", "supported");
    }
    if (max_num_res_csi_rs_rlm_present) {
      j.write_str("maxNumberResource-CSI-RS-RLM", max_num_res_csi_rs_rlm.to_string());
    }
    if (simul_rx_data_ssb_diff_numerology_present) {
      j.write_str("simultaneousRxDataSSB-DiffNumerology", "supported");
    }
  }
  j.end_obj();
}

const char* meas_and_mob_params_frx_diff_s::max_num_res_csi_rs_rlm_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n6", "n8"};
  return convert_enum_idx(names, 4, value, "meas_and_mob_params_frx_diff_s::max_num_res_csi_rs_rlm_e_");
}
uint8_t meas_and_mob_params_frx_diff_s::max_num_res_csi_rs_rlm_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8};
  return map_enum_number(numbers, 4, value, "meas_and_mob_params_frx_diff_s::max_num_res_csi_rs_rlm_e_");
}

// MeasAndMobParametersXDD-Diff ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_xdd_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(intra_and_inter_f_meas_and_report_present, 1));
  HANDLE_CODE(bref.pack(event_a_meas_and_report_present, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ho_inter_f_present;
    group_flags[0] |= ho_lte_epc_present;
    group_flags[0] |= ho_lte_5_gc_present;
    group_flags[1] |= sftd_meas_nr_neigh_present;
    group_flags[1] |= sftd_meas_nr_neigh_drx_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ho_inter_f_present, 1));
      HANDLE_CODE(bref.pack(ho_lte_epc_present, 1));
      HANDLE_CODE(bref.pack(ho_lte_5_gc_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sftd_meas_nr_neigh_present, 1));
      HANDLE_CODE(bref.pack(sftd_meas_nr_neigh_drx_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_xdd_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(intra_and_inter_f_meas_and_report_present, 1));
  HANDLE_CODE(bref.unpack(event_a_meas_and_report_present, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ho_inter_f_present, 1));
      HANDLE_CODE(bref.unpack(ho_lte_epc_present, 1));
      HANDLE_CODE(bref.unpack(ho_lte_5_gc_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sftd_meas_nr_neigh_present, 1));
      HANDLE_CODE(bref.unpack(sftd_meas_nr_neigh_drx_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_and_mob_params_xdd_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_and_inter_f_meas_and_report_present) {
    j.write_str("intraAndInterF-MeasAndReport", "supported");
  }
  if (event_a_meas_and_report_present) {
    j.write_str("eventA-MeasAndReport", "supported");
  }
  if (ext) {
    if (ho_inter_f_present) {
      j.write_str("handoverInterF", "supported");
    }
    if (ho_lte_epc_present) {
      j.write_str("handoverLTE-EPC", "supported");
    }
    if (ho_lte_5_gc_present) {
      j.write_str("handoverLTE-5GC", "supported");
    }
    if (sftd_meas_nr_neigh_present) {
      j.write_str("sftd-MeasNR-Neigh", "supported");
    }
    if (sftd_meas_nr_neigh_drx_present) {
      j.write_str("sftd-MeasNR-Neigh-DRX", "supported");
    }
  }
  j.end_obj();
}

// MeasAndMobParameters ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_common_present, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_frx_diff_present, 1));

  if (meas_and_mob_params_common_present) {
    HANDLE_CODE(meas_and_mob_params_common.pack(bref));
  }
  if (meas_and_mob_params_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_xdd_diff.pack(bref));
  }
  if (meas_and_mob_params_frx_diff_present) {
    HANDLE_CODE(meas_and_mob_params_frx_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_common_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_frx_diff_present, 1));

  if (meas_and_mob_params_common_present) {
    HANDLE_CODE(meas_and_mob_params_common.unpack(bref));
  }
  if (meas_and_mob_params_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_xdd_diff.unpack(bref));
  }
  if (meas_and_mob_params_frx_diff_present) {
    HANDLE_CODE(meas_and_mob_params_frx_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_common_present) {
    j.write_fieldname("measAndMobParametersCommon");
    meas_and_mob_params_common.to_json(j);
  }
  if (meas_and_mob_params_xdd_diff_present) {
    j.write_fieldname("measAndMobParametersXDD-Diff");
    meas_and_mob_params_xdd_diff.to_json(j);
  }
  if (meas_and_mob_params_frx_diff_present) {
    j.write_fieldname("measAndMobParametersFRX-Diff");
    meas_and_mob_params_frx_diff.to_json(j);
  }
  j.end_obj();
}

// MeasAndMobParametersMRDC-Common ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_common_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(independent_gap_cfg_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_common_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(independent_gap_cfg_present, 1));

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (independent_gap_cfg_present) {
    j.write_str("independentGapConfig", "supported");
  }
  j.end_obj();
}

// MeasAndMobParametersMRDC-FRX-Diff ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_frx_diff_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(simul_rx_data_ssb_diff_numerology_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_frx_diff_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(simul_rx_data_ssb_diff_numerology_present, 1));

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_frx_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (simul_rx_data_ssb_diff_numerology_present) {
    j.write_str("simultaneousRxDataSSB-DiffNumerology", "supported");
  }
  j.end_obj();
}

// MeasAndMobParametersMRDC-XDD-Diff ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_xdd_diff_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sftd_meas_pscell_present, 1));
  HANDLE_CODE(bref.pack(sftd_meas_nr_cell_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_xdd_diff_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sftd_meas_pscell_present, 1));
  HANDLE_CODE(bref.unpack(sftd_meas_nr_cell_present, 1));

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_xdd_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sftd_meas_pscell_present) {
    j.write_str("sftd-MeasPSCell", "supported");
  }
  if (sftd_meas_nr_cell_present) {
    j.write_str("sftd-MeasNR-Cell", "supported");
  }
  j.end_obj();
}

// MeasAndMobParametersMRDC ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_common_present, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_xdd_diff_present, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_frx_diff_present, 1));

  if (meas_and_mob_params_mrdc_common_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_common.pack(bref));
  }
  if (meas_and_mob_params_mrdc_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff.pack(bref));
  }
  if (meas_and_mob_params_mrdc_frx_diff_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_frx_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_common_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_xdd_diff_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_frx_diff_present, 1));

  if (meas_and_mob_params_mrdc_common_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_common.unpack(bref));
  }
  if (meas_and_mob_params_mrdc_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff.unpack(bref));
  }
  if (meas_and_mob_params_mrdc_frx_diff_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_frx_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_common_present) {
    j.write_fieldname("measAndMobParametersMRDC-Common");
    meas_and_mob_params_mrdc_common.to_json(j);
  }
  if (meas_and_mob_params_mrdc_xdd_diff_present) {
    j.write_fieldname("measAndMobParametersMRDC-XDD-Diff");
    meas_and_mob_params_mrdc_xdd_diff.to_json(j);
  }
  if (meas_and_mob_params_mrdc_frx_diff_present) {
    j.write_fieldname("measAndMobParametersMRDC-FRX-Diff");
    meas_and_mob_params_mrdc_frx_diff.to_json(j);
  }
  j.end_obj();
}

// GeneralParametersMRDC-XDD-Diff ::= SEQUENCE
SRSASN_CODE general_params_mrdc_xdd_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(split_srb_with_one_ul_path_present, 1));
  HANDLE_CODE(bref.pack(split_drb_with_ul_both_mcg_scg_present, 1));
  HANDLE_CODE(bref.pack(srb3_present, 1));
  HANDLE_CODE(bref.pack(v2x_eutra_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE general_params_mrdc_xdd_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(split_srb_with_one_ul_path_present, 1));
  HANDLE_CODE(bref.unpack(split_drb_with_ul_both_mcg_scg_present, 1));
  HANDLE_CODE(bref.unpack(srb3_present, 1));
  HANDLE_CODE(bref.unpack(v2x_eutra_present, 1));

  return SRSASN_SUCCESS;
}
void general_params_mrdc_xdd_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (split_srb_with_one_ul_path_present) {
    j.write_str("splitSRB-WithOneUL-Path", "supported");
  }
  if (split_drb_with_ul_both_mcg_scg_present) {
    j.write_str("splitDRB-withUL-Both-MCG-SCG", "supported");
  }
  if (srb3_present) {
    j.write_str("srb3", "supported");
  }
  if (v2x_eutra_present) {
    j.write_str("v2x-EUTRA", "supported");
  }
  j.end_obj();
}

// UE-MRDC-CapabilityAddFRX-Mode ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_add_frx_mode_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(meas_and_mob_params_mrdc_frx_diff.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_add_frx_mode_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(meas_and_mob_params_mrdc_frx_diff.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_add_frx_mode_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measAndMobParametersMRDC-FRX-Diff");
  meas_and_mob_params_mrdc_frx_diff.to_json(j);
  j.end_obj();
}

// UE-MRDC-CapabilityAddXDD-Mode ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_add_xdd_mode_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_xdd_diff_present, 1));
  HANDLE_CODE(bref.pack(general_params_mrdc_xdd_diff_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff.pack(bref));
  }
  if (general_params_mrdc_xdd_diff_present) {
    HANDLE_CODE(general_params_mrdc_xdd_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_add_xdd_mode_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_xdd_diff_present, 1));
  HANDLE_CODE(bref.unpack(general_params_mrdc_xdd_diff_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff.unpack(bref));
  }
  if (general_params_mrdc_xdd_diff_present) {
    HANDLE_CODE(general_params_mrdc_xdd_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_add_xdd_mode_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_xdd_diff_present) {
    j.write_fieldname("measAndMobParametersMRDC-XDD-Diff");
    meas_and_mob_params_mrdc_xdd_diff.to_json(j);
  }
  if (general_params_mrdc_xdd_diff_present) {
    j.write_fieldname("generalParametersMRDC-XDD-Diff");
    general_params_mrdc_xdd_diff.to_json(j);
  }
  j.end_obj();
}

// NRDC-Parameters ::= SEQUENCE
SRSASN_CODE nrdc_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_nrdc_present, 1));
  HANDLE_CODE(bref.pack(general_params_nrdc_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_nrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_nrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(fr1_add_ue_nrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(fr2_add_ue_nrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));

  if (meas_and_mob_params_nrdc_present) {
    HANDLE_CODE(meas_and_mob_params_nrdc.pack(bref));
  }
  if (general_params_nrdc_present) {
    HANDLE_CODE(general_params_nrdc.pack(bref));
  }
  if (fdd_add_ue_nrdc_cap_present) {
    HANDLE_CODE(fdd_add_ue_nrdc_cap.pack(bref));
  }
  if (tdd_add_ue_nrdc_cap_present) {
    HANDLE_CODE(tdd_add_ue_nrdc_cap.pack(bref));
  }
  if (fr1_add_ue_nrdc_cap_present) {
    HANDLE_CODE(fr1_add_ue_nrdc_cap.pack(bref));
  }
  if (fr2_add_ue_nrdc_cap_present) {
    HANDLE_CODE(fr2_add_ue_nrdc_cap.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nrdc_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_nrdc_present, 1));
  HANDLE_CODE(bref.unpack(general_params_nrdc_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_nrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_nrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr1_add_ue_nrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr2_add_ue_nrdc_cap_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  if (meas_and_mob_params_nrdc_present) {
    HANDLE_CODE(meas_and_mob_params_nrdc.unpack(bref));
  }
  if (general_params_nrdc_present) {
    HANDLE_CODE(general_params_nrdc.unpack(bref));
  }
  if (fdd_add_ue_nrdc_cap_present) {
    HANDLE_CODE(fdd_add_ue_nrdc_cap.unpack(bref));
  }
  if (tdd_add_ue_nrdc_cap_present) {
    HANDLE_CODE(tdd_add_ue_nrdc_cap.unpack(bref));
  }
  if (fr1_add_ue_nrdc_cap_present) {
    HANDLE_CODE(fr1_add_ue_nrdc_cap.unpack(bref));
  }
  if (fr2_add_ue_nrdc_cap_present) {
    HANDLE_CODE(fr2_add_ue_nrdc_cap.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nrdc_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_nrdc_present) {
    j.write_fieldname("measAndMobParametersNRDC");
    meas_and_mob_params_nrdc.to_json(j);
  }
  if (general_params_nrdc_present) {
    j.write_fieldname("generalParametersNRDC");
    general_params_nrdc.to_json(j);
  }
  if (fdd_add_ue_nrdc_cap_present) {
    j.write_fieldname("fdd-Add-UE-NRDC-Capabilities");
    fdd_add_ue_nrdc_cap.to_json(j);
  }
  if (tdd_add_ue_nrdc_cap_present) {
    j.write_fieldname("tdd-Add-UE-NRDC-Capabilities");
    tdd_add_ue_nrdc_cap.to_json(j);
  }
  if (fr1_add_ue_nrdc_cap_present) {
    j.write_fieldname("fr1-Add-UE-NRDC-Capabilities");
    fr1_add_ue_nrdc_cap.to_json(j);
  }
  if (fr2_add_ue_nrdc_cap_present) {
    j.write_fieldname("fr2-Add-UE-NRDC-Capabilities");
    fr2_add_ue_nrdc_cap.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (dummy_present) {
    j.write_fieldname("dummy");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CarrierAggregationVariant ::= SEQUENCE
SRSASN_CODE carrier_aggregation_variant_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_fdd_present, 1));
  HANDLE_CODE(bref.pack(fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_tdd_present, 1));
  HANDLE_CODE(bref.pack(fr1fdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present, 1));
  HANDLE_CODE(bref.pack(fr1fdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present, 1));
  HANDLE_CODE(bref.pack(fr1tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present, 1));
  HANDLE_CODE(bref.pack(fr1tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present, 1));
  HANDLE_CODE(bref.pack(fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present, 1));
  HANDLE_CODE(bref.pack(fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present, 1));
  HANDLE_CODE(bref.pack(fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_aggregation_variant_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_fdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_tdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1fdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1fdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present, 1));
  HANDLE_CODE(bref.unpack(fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present, 1));

  return SRSASN_SUCCESS;
}
void carrier_aggregation_variant_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_fdd_present) {
    j.write_str("fr1fdd-FR1TDD-CA-SpCellOnFR1FDD", "supported");
  }
  if (fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_tdd_present) {
    j.write_str("fr1fdd-FR1TDD-CA-SpCellOnFR1TDD", "supported");
  }
  if (fr1fdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present) {
    j.write_str("fr1fdd-FR2TDD-CA-SpCellOnFR1FDD", "supported");
  }
  if (fr1fdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present) {
    j.write_str("fr1fdd-FR2TDD-CA-SpCellOnFR2TDD", "supported");
  }
  if (fr1tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present) {
    j.write_str("fr1tdd-FR2TDD-CA-SpCellOnFR1TDD", "supported");
  }
  if (fr1tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present) {
    j.write_str("fr1tdd-FR2TDD-CA-SpCellOnFR2TDD", "supported");
  }
  if (fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present) {
    j.write_str("fr1fdd-FR1TDD-FR2TDD-CA-SpCellOnFR1FDD", "supported");
  }
  if (fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present) {
    j.write_str("fr1fdd-FR1TDD-FR2TDD-CA-SpCellOnFR1TDD", "supported");
  }
  if (fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present) {
    j.write_str("fr1fdd-FR1TDD-FR2TDD-CA-SpCellOnFR2TDD", "supported");
  }
  j.end_obj();
}

// Phy-ParametersCommon ::= SEQUENCE
SRSASN_CODE phy_params_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(csi_rs_cfra_for_ho_present, 1));
  HANDLE_CODE(bref.pack(dyn_prb_bundling_dl_present, 1));
  HANDLE_CODE(bref.pack(sp_csi_report_pucch_present, 1));
  HANDLE_CODE(bref.pack(sp_csi_report_pusch_present, 1));
  HANDLE_CODE(bref.pack(nzp_csi_rs_intef_mgmt_present, 1));
  HANDLE_CODE(bref.pack(type2_sp_csi_feedback_long_pucch_present, 1));
  HANDLE_CODE(bref.pack(precoder_granularity_coreset_present, 1));
  HANDLE_CODE(bref.pack(dyn_harq_ack_codebook_present, 1));
  HANDLE_CODE(bref.pack(semi_static_harq_ack_codebook_present, 1));
  HANDLE_CODE(bref.pack(spatial_bundling_harq_ack_present, 1));
  HANDLE_CODE(bref.pack(dyn_beta_offset_ind_harq_ack_csi_present, 1));
  HANDLE_CODE(bref.pack(pucch_repeat_f1_3_4_present, 1));
  HANDLE_CODE(bref.pack(ra_type0_pusch_present, 1));
  HANDLE_CODE(bref.pack(dyn_switch_ra_type0_1_pdsch_present, 1));
  HANDLE_CODE(bref.pack(dyn_switch_ra_type0_1_pusch_present, 1));
  HANDLE_CODE(bref.pack(pdsch_map_type_a_present, 1));
  HANDLE_CODE(bref.pack(pdsch_map_type_b_present, 1));
  HANDLE_CODE(bref.pack(interleaving_vrb_to_prb_pdsch_present, 1));
  HANDLE_CODE(bref.pack(inter_slot_freq_hop_pusch_present, 1));
  HANDLE_CODE(bref.pack(type1_pusch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.pack(type2_pusch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.pack(pusch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.pack(pdsch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.pack(dl_sps_present, 1));
  HANDLE_CODE(bref.pack(cfg_ul_grant_type1_present, 1));
  HANDLE_CODE(bref.pack(cfg_ul_grant_type2_present, 1));
  HANDLE_CODE(bref.pack(pre_empt_ind_dl_present, 1));
  HANDLE_CODE(bref.pack(cbg_trans_ind_dl_present, 1));
  HANDLE_CODE(bref.pack(cbg_trans_ind_ul_present, 1));
  HANDLE_CODE(bref.pack(cbg_flush_ind_dl_present, 1));
  HANDLE_CODE(bref.pack(dyn_harq_ack_code_b_cbg_retx_dl_present, 1));
  HANDLE_CODE(bref.pack(rate_matching_resrc_set_semi_static_present, 1));
  HANDLE_CODE(bref.pack(rate_matching_resrc_set_dyn_present, 1));
  HANDLE_CODE(bref.pack(bwp_switching_delay_present, 1));

  if (bwp_switching_delay_present) {
    HANDLE_CODE(bwp_switching_delay.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dummy_present;
    group_flags[1] |= max_num_search_spaces_present;
    group_flags[1] |= rate_matching_ctrl_resrc_set_dyn_present;
    group_flags[1] |= max_layers_mimo_ind_present;
    group_flags[2] |= sp_cell_placement.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dummy_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_num_search_spaces_present, 1));
      HANDLE_CODE(bref.pack(rate_matching_ctrl_resrc_set_dyn_present, 1));
      HANDLE_CODE(bref.pack(max_layers_mimo_ind_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sp_cell_placement.is_present(), 1));
      if (sp_cell_placement.is_present()) {
        HANDLE_CODE(sp_cell_placement->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(csi_rs_cfra_for_ho_present, 1));
  HANDLE_CODE(bref.unpack(dyn_prb_bundling_dl_present, 1));
  HANDLE_CODE(bref.unpack(sp_csi_report_pucch_present, 1));
  HANDLE_CODE(bref.unpack(sp_csi_report_pusch_present, 1));
  HANDLE_CODE(bref.unpack(nzp_csi_rs_intef_mgmt_present, 1));
  HANDLE_CODE(bref.unpack(type2_sp_csi_feedback_long_pucch_present, 1));
  HANDLE_CODE(bref.unpack(precoder_granularity_coreset_present, 1));
  HANDLE_CODE(bref.unpack(dyn_harq_ack_codebook_present, 1));
  HANDLE_CODE(bref.unpack(semi_static_harq_ack_codebook_present, 1));
  HANDLE_CODE(bref.unpack(spatial_bundling_harq_ack_present, 1));
  HANDLE_CODE(bref.unpack(dyn_beta_offset_ind_harq_ack_csi_present, 1));
  HANDLE_CODE(bref.unpack(pucch_repeat_f1_3_4_present, 1));
  HANDLE_CODE(bref.unpack(ra_type0_pusch_present, 1));
  HANDLE_CODE(bref.unpack(dyn_switch_ra_type0_1_pdsch_present, 1));
  HANDLE_CODE(bref.unpack(dyn_switch_ra_type0_1_pusch_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_map_type_a_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_map_type_b_present, 1));
  HANDLE_CODE(bref.unpack(interleaving_vrb_to_prb_pdsch_present, 1));
  HANDLE_CODE(bref.unpack(inter_slot_freq_hop_pusch_present, 1));
  HANDLE_CODE(bref.unpack(type1_pusch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.unpack(type2_pusch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.unpack(pusch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_repeat_multi_slots_present, 1));
  HANDLE_CODE(bref.unpack(dl_sps_present, 1));
  HANDLE_CODE(bref.unpack(cfg_ul_grant_type1_present, 1));
  HANDLE_CODE(bref.unpack(cfg_ul_grant_type2_present, 1));
  HANDLE_CODE(bref.unpack(pre_empt_ind_dl_present, 1));
  HANDLE_CODE(bref.unpack(cbg_trans_ind_dl_present, 1));
  HANDLE_CODE(bref.unpack(cbg_trans_ind_ul_present, 1));
  HANDLE_CODE(bref.unpack(cbg_flush_ind_dl_present, 1));
  HANDLE_CODE(bref.unpack(dyn_harq_ack_code_b_cbg_retx_dl_present, 1));
  HANDLE_CODE(bref.unpack(rate_matching_resrc_set_semi_static_present, 1));
  HANDLE_CODE(bref.unpack(rate_matching_resrc_set_dyn_present, 1));
  HANDLE_CODE(bref.unpack(bwp_switching_delay_present, 1));

  if (bwp_switching_delay_present) {
    HANDLE_CODE(bwp_switching_delay.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dummy_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_num_search_spaces_present, 1));
      HANDLE_CODE(bref.unpack(rate_matching_ctrl_resrc_set_dyn_present, 1));
      HANDLE_CODE(bref.unpack(max_layers_mimo_ind_present, 1));
    }
    if (group_flags[2]) {
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
void phy_params_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (csi_rs_cfra_for_ho_present) {
    j.write_str("csi-RS-CFRA-ForHO", "supported");
  }
  if (dyn_prb_bundling_dl_present) {
    j.write_str("dynamicPRB-BundlingDL", "supported");
  }
  if (sp_csi_report_pucch_present) {
    j.write_str("sp-CSI-ReportPUCCH", "supported");
  }
  if (sp_csi_report_pusch_present) {
    j.write_str("sp-CSI-ReportPUSCH", "supported");
  }
  if (nzp_csi_rs_intef_mgmt_present) {
    j.write_str("nzp-CSI-RS-IntefMgmt", "supported");
  }
  if (type2_sp_csi_feedback_long_pucch_present) {
    j.write_str("type2-SP-CSI-Feedback-LongPUCCH", "supported");
  }
  if (precoder_granularity_coreset_present) {
    j.write_str("precoderGranularityCORESET", "supported");
  }
  if (dyn_harq_ack_codebook_present) {
    j.write_str("dynamicHARQ-ACK-Codebook", "supported");
  }
  if (semi_static_harq_ack_codebook_present) {
    j.write_str("semiStaticHARQ-ACK-Codebook", "supported");
  }
  if (spatial_bundling_harq_ack_present) {
    j.write_str("spatialBundlingHARQ-ACK", "supported");
  }
  if (dyn_beta_offset_ind_harq_ack_csi_present) {
    j.write_str("dynamicBetaOffsetInd-HARQ-ACK-CSI", "supported");
  }
  if (pucch_repeat_f1_3_4_present) {
    j.write_str("pucch-Repetition-F1-3-4", "supported");
  }
  if (ra_type0_pusch_present) {
    j.write_str("ra-Type0-PUSCH", "supported");
  }
  if (dyn_switch_ra_type0_1_pdsch_present) {
    j.write_str("dynamicSwitchRA-Type0-1-PDSCH", "supported");
  }
  if (dyn_switch_ra_type0_1_pusch_present) {
    j.write_str("dynamicSwitchRA-Type0-1-PUSCH", "supported");
  }
  if (pdsch_map_type_a_present) {
    j.write_str("pdsch-MappingTypeA", "supported");
  }
  if (pdsch_map_type_b_present) {
    j.write_str("pdsch-MappingTypeB", "supported");
  }
  if (interleaving_vrb_to_prb_pdsch_present) {
    j.write_str("interleavingVRB-ToPRB-PDSCH", "supported");
  }
  if (inter_slot_freq_hop_pusch_present) {
    j.write_str("interSlotFreqHopping-PUSCH", "supported");
  }
  if (type1_pusch_repeat_multi_slots_present) {
    j.write_str("type1-PUSCH-RepetitionMultiSlots", "supported");
  }
  if (type2_pusch_repeat_multi_slots_present) {
    j.write_str("type2-PUSCH-RepetitionMultiSlots", "supported");
  }
  if (pusch_repeat_multi_slots_present) {
    j.write_str("pusch-RepetitionMultiSlots", "supported");
  }
  if (pdsch_repeat_multi_slots_present) {
    j.write_str("pdsch-RepetitionMultiSlots", "supported");
  }
  if (dl_sps_present) {
    j.write_str("downlinkSPS", "supported");
  }
  if (cfg_ul_grant_type1_present) {
    j.write_str("configuredUL-GrantType1", "supported");
  }
  if (cfg_ul_grant_type2_present) {
    j.write_str("configuredUL-GrantType2", "supported");
  }
  if (pre_empt_ind_dl_present) {
    j.write_str("pre-EmptIndication-DL", "supported");
  }
  if (cbg_trans_ind_dl_present) {
    j.write_str("cbg-TransIndication-DL", "supported");
  }
  if (cbg_trans_ind_ul_present) {
    j.write_str("cbg-TransIndication-UL", "supported");
  }
  if (cbg_flush_ind_dl_present) {
    j.write_str("cbg-FlushIndication-DL", "supported");
  }
  if (dyn_harq_ack_code_b_cbg_retx_dl_present) {
    j.write_str("dynamicHARQ-ACK-CodeB-CBG-Retx-DL", "supported");
  }
  if (rate_matching_resrc_set_semi_static_present) {
    j.write_str("rateMatchingResrcSetSemi-Static", "supported");
  }
  if (rate_matching_resrc_set_dyn_present) {
    j.write_str("rateMatchingResrcSetDynamic", "supported");
  }
  if (bwp_switching_delay_present) {
    j.write_str("bwp-SwitchingDelay", bwp_switching_delay.to_string());
  }
  if (ext) {
    if (dummy_present) {
      j.write_str("dummy", "supported");
    }
    if (max_num_search_spaces_present) {
      j.write_str("maxNumberSearchSpaces", "n10");
    }
    if (rate_matching_ctrl_resrc_set_dyn_present) {
      j.write_str("rateMatchingCtrlResrcSetDynamic", "supported");
    }
    if (max_layers_mimo_ind_present) {
      j.write_str("maxLayersMIMO-Indication", "supported");
    }
    if (sp_cell_placement.is_present()) {
      j.write_fieldname("spCellPlacement");
      sp_cell_placement->to_json(j);
    }
  }
  j.end_obj();
}

const char* phy_params_common_s::bwp_switching_delay_opts::to_string() const
{
  static const char* names[] = {"type1", "type2"};
  return convert_enum_idx(names, 2, value, "phy_params_common_s::bwp_switching_delay_e_");
}
uint8_t phy_params_common_s::bwp_switching_delay_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "phy_params_common_s::bwp_switching_delay_e_");
}

// Phy-ParametersFR1 ::= SEQUENCE
SRSASN_CODE phy_params_fr1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdcch_monitoring_single_occasion_present, 1));
  HANDLE_CODE(bref.pack(scs_60k_hz_present, 1));
  HANDLE_CODE(bref.pack(pdsch_256_qam_fr1_present, 1));
  HANDLE_CODE(bref.pack(pdsch_re_map_fr1_per_symbol_present, 1));

  if (pdsch_re_map_fr1_per_symbol_present) {
    HANDLE_CODE(pdsch_re_map_fr1_per_symbol.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pdsch_re_map_fr1_per_slot_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_re_map_fr1_per_slot_present, 1));
      if (pdsch_re_map_fr1_per_slot_present) {
        HANDLE_CODE(pdsch_re_map_fr1_per_slot.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_fr1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdcch_monitoring_single_occasion_present, 1));
  HANDLE_CODE(bref.unpack(scs_60k_hz_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_256_qam_fr1_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_re_map_fr1_per_symbol_present, 1));

  if (pdsch_re_map_fr1_per_symbol_present) {
    HANDLE_CODE(pdsch_re_map_fr1_per_symbol.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pdsch_re_map_fr1_per_slot_present, 1));
      if (pdsch_re_map_fr1_per_slot_present) {
        HANDLE_CODE(pdsch_re_map_fr1_per_slot.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phy_params_fr1_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcch_monitoring_single_occasion_present) {
    j.write_str("pdcch-MonitoringSingleOccasion", "supported");
  }
  if (scs_60k_hz_present) {
    j.write_str("scs-60kHz", "supported");
  }
  if (pdsch_256_qam_fr1_present) {
    j.write_str("pdsch-256QAM-FR1", "supported");
  }
  if (pdsch_re_map_fr1_per_symbol_present) {
    j.write_str("pdsch-RE-MappingFR1-PerSymbol", pdsch_re_map_fr1_per_symbol.to_string());
  }
  if (ext) {
    if (pdsch_re_map_fr1_per_slot_present) {
      j.write_str("pdsch-RE-MappingFR1-PerSlot", pdsch_re_map_fr1_per_slot.to_string());
    }
  }
  j.end_obj();
}

const char* phy_params_fr1_s::pdsch_re_map_fr1_per_symbol_opts::to_string() const
{
  static const char* names[] = {"n10", "n20"};
  return convert_enum_idx(names, 2, value, "phy_params_fr1_s::pdsch_re_map_fr1_per_symbol_e_");
}
uint8_t phy_params_fr1_s::pdsch_re_map_fr1_per_symbol_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20};
  return map_enum_number(numbers, 2, value, "phy_params_fr1_s::pdsch_re_map_fr1_per_symbol_e_");
}

const char* phy_params_fr1_s::pdsch_re_map_fr1_per_slot_opts::to_string() const
{
  static const char* names[] = {"n16",
                                "n32",
                                "n48",
                                "n64",
                                "n80",
                                "n96",
                                "n112",
                                "n128",
                                "n144",
                                "n160",
                                "n176",
                                "n192",
                                "n208",
                                "n224",
                                "n240",
                                "n256"};
  return convert_enum_idx(names, 16, value, "phy_params_fr1_s::pdsch_re_map_fr1_per_slot_e_");
}
uint16_t phy_params_fr1_s::pdsch_re_map_fr1_per_slot_opts::to_number() const
{
  static const uint16_t numbers[] = {16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256};
  return map_enum_number(numbers, 16, value, "phy_params_fr1_s::pdsch_re_map_fr1_per_slot_e_");
}

// Phy-ParametersFR2 ::= SEQUENCE
SRSASN_CODE phy_params_fr2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(pdsch_re_map_fr2_per_symbol_present, 1));

  if (pdsch_re_map_fr2_per_symbol_present) {
    HANDLE_CODE(pdsch_re_map_fr2_per_symbol.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pcell_fr2_present;
    group_flags[0] |= pdsch_re_map_fr2_per_slot_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pcell_fr2_present, 1));
      HANDLE_CODE(bref.pack(pdsch_re_map_fr2_per_slot_present, 1));
      if (pdsch_re_map_fr2_per_slot_present) {
        HANDLE_CODE(pdsch_re_map_fr2_per_slot.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_fr2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_re_map_fr2_per_symbol_present, 1));

  if (pdsch_re_map_fr2_per_symbol_present) {
    HANDLE_CODE(pdsch_re_map_fr2_per_symbol.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pcell_fr2_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_re_map_fr2_per_slot_present, 1));
      if (pdsch_re_map_fr2_per_slot_present) {
        HANDLE_CODE(pdsch_re_map_fr2_per_slot.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phy_params_fr2_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dummy_present) {
    j.write_str("dummy", "supported");
  }
  if (pdsch_re_map_fr2_per_symbol_present) {
    j.write_str("pdsch-RE-MappingFR2-PerSymbol", pdsch_re_map_fr2_per_symbol.to_string());
  }
  if (ext) {
    if (pcell_fr2_present) {
      j.write_str("pCell-FR2", "supported");
    }
    if (pdsch_re_map_fr2_per_slot_present) {
      j.write_str("pdsch-RE-MappingFR2-PerSlot", pdsch_re_map_fr2_per_slot.to_string());
    }
  }
  j.end_obj();
}

const char* phy_params_fr2_s::pdsch_re_map_fr2_per_symbol_opts::to_string() const
{
  static const char* names[] = {"n6", "n20"};
  return convert_enum_idx(names, 2, value, "phy_params_fr2_s::pdsch_re_map_fr2_per_symbol_e_");
}
uint8_t phy_params_fr2_s::pdsch_re_map_fr2_per_symbol_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 20};
  return map_enum_number(numbers, 2, value, "phy_params_fr2_s::pdsch_re_map_fr2_per_symbol_e_");
}

const char* phy_params_fr2_s::pdsch_re_map_fr2_per_slot_opts::to_string() const
{
  static const char* names[] = {"n16",
                                "n32",
                                "n48",
                                "n64",
                                "n80",
                                "n96",
                                "n112",
                                "n128",
                                "n144",
                                "n160",
                                "n176",
                                "n192",
                                "n208",
                                "n224",
                                "n240",
                                "n256"};
  return convert_enum_idx(names, 16, value, "phy_params_fr2_s::pdsch_re_map_fr2_per_slot_e_");
}
uint16_t phy_params_fr2_s::pdsch_re_map_fr2_per_slot_opts::to_number() const
{
  static const uint16_t numbers[] = {16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256};
  return map_enum_number(numbers, 16, value, "phy_params_fr2_s::pdsch_re_map_fr2_per_slot_e_");
}

// Phy-ParametersFRX-Diff ::= SEQUENCE
SRSASN_CODE phy_params_frx_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dyn_sfi_present, 1));
  HANDLE_CODE(bref.pack(dummy1_present, 1));
  HANDLE_CODE(bref.pack(two_fl_dmrs_present, 1));
  HANDLE_CODE(bref.pack(dummy2_present, 1));
  HANDLE_CODE(bref.pack(dummy3_present, 1));
  HANDLE_CODE(bref.pack(supported_dmrs_type_dl_present, 1));
  HANDLE_CODE(bref.pack(supported_dmrs_type_ul_present, 1));
  HANDLE_CODE(bref.pack(semi_open_loop_csi_present, 1));
  HANDLE_CODE(bref.pack(csi_report_without_pmi_present, 1));
  HANDLE_CODE(bref.pack(csi_report_without_cqi_present, 1));
  HANDLE_CODE(bref.pack(one_ports_ptrs_present, 1));
  HANDLE_CODE(bref.pack(two_pucch_f0_2_consec_symbols_present, 1));
  HANDLE_CODE(bref.pack(pucch_f2_with_fh_present, 1));
  HANDLE_CODE(bref.pack(pucch_f3_with_fh_present, 1));
  HANDLE_CODE(bref.pack(pucch_f4_with_fh_present, 1));
  HANDLE_CODE(bref.pack(pucch_f0_2_without_fh_present, 1));
  HANDLE_CODE(bref.pack(pucch_f1_3_4_without_fh_present, 1));
  HANDLE_CODE(bref.pack(mux_sr_harq_ack_csi_pucch_multi_per_slot_present, 1));
  HANDLE_CODE(bref.pack(uci_code_block_segmentation_present, 1));
  HANDLE_CODE(bref.pack(one_pucch_long_and_short_format_present, 1));
  HANDLE_CODE(bref.pack(two_pucch_any_others_in_slot_present, 1));
  HANDLE_CODE(bref.pack(intra_slot_freq_hop_pusch_present, 1));
  HANDLE_CODE(bref.pack(pusch_lbrm_present, 1));
  HANDLE_CODE(bref.pack(pdcch_blind_detection_ca_present, 1));
  HANDLE_CODE(bref.pack(tpc_pusch_rnti_present, 1));
  HANDLE_CODE(bref.pack(tpc_pucch_rnti_present, 1));
  HANDLE_CODE(bref.pack(tpc_srs_rnti_present, 1));
  HANDLE_CODE(bref.pack(absolute_tpc_cmd_present, 1));
  HANDLE_CODE(bref.pack(two_different_tpc_loop_pusch_present, 1));
  HANDLE_CODE(bref.pack(two_different_tpc_loop_pucch_present, 1));
  HANDLE_CODE(bref.pack(pusch_half_pi_bpsk_present, 1));
  HANDLE_CODE(bref.pack(pucch_f3_4_half_pi_bpsk_present, 1));
  HANDLE_CODE(bref.pack(almost_contiguous_cp_ofdm_ul_present, 1));
  HANDLE_CODE(bref.pack(sp_csi_rs_present, 1));
  HANDLE_CODE(bref.pack(sp_csi_im_present, 1));
  HANDLE_CODE(bref.pack(tdd_multi_dl_ul_switch_per_slot_present, 1));
  HANDLE_CODE(bref.pack(multiple_coreset_present, 1));

  if (dummy1_present) {
    HANDLE_CODE(dummy1.pack(bref));
  }
  if (two_fl_dmrs_present) {
    HANDLE_CODE(two_fl_dmrs.pack(bref));
  }
  if (dummy2_present) {
    HANDLE_CODE(dummy2.pack(bref));
  }
  if (dummy3_present) {
    HANDLE_CODE(dummy3.pack(bref));
  }
  if (supported_dmrs_type_dl_present) {
    HANDLE_CODE(supported_dmrs_type_dl.pack(bref));
  }
  if (supported_dmrs_type_ul_present) {
    HANDLE_CODE(supported_dmrs_type_ul.pack(bref));
  }
  if (one_ports_ptrs_present) {
    HANDLE_CODE(one_ports_ptrs.pack(bref));
  }
  if (pdcch_blind_detection_ca_present) {
    HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca, (uint8_t)4u, (uint8_t)16u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_rs_im_reception_for_feedback.is_present();
    group_flags[0] |= csi_rs_proc_framework_for_srs.is_present();
    group_flags[0] |= csi_report_framework.is_present();
    group_flags[0] |= mux_sr_harq_ack_csi_pucch_once_per_slot.is_present();
    group_flags[0] |= mux_sr_harq_ack_pucch_present;
    group_flags[0] |= mux_multiple_group_ctrl_ch_overlap_present;
    group_flags[0] |= dl_sched_offset_pdsch_type_a_present;
    group_flags[0] |= dl_sched_offset_pdsch_type_b_present;
    group_flags[0] |= ul_sched_offset_present;
    group_flags[0] |= dl_64_qam_mcs_table_alt_present;
    group_flags[0] |= ul_64_qam_mcs_table_alt_present;
    group_flags[0] |= cqi_table_alt_present;
    group_flags[0] |= one_fl_dmrs_two_add_dmrs_ul_present;
    group_flags[0] |= two_fl_dmrs_two_add_dmrs_ul_present;
    group_flags[0] |= one_fl_dmrs_three_add_dmrs_ul_present;
    group_flags[1] |= pdcch_blind_detection_nrdc.is_present();
    group_flags[1] |= mux_harq_ack_pusch_diff_symbol_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_im_reception_for_feedback.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_proc_framework_for_srs.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_report_framework.is_present(), 1));
      HANDLE_CODE(bref.pack(mux_sr_harq_ack_csi_pucch_once_per_slot.is_present(), 1));
      HANDLE_CODE(bref.pack(mux_sr_harq_ack_pucch_present, 1));
      HANDLE_CODE(bref.pack(mux_multiple_group_ctrl_ch_overlap_present, 1));
      HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_present, 1));
      HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_present, 1));
      HANDLE_CODE(bref.pack(ul_sched_offset_present, 1));
      HANDLE_CODE(bref.pack(dl_64_qam_mcs_table_alt_present, 1));
      HANDLE_CODE(bref.pack(ul_64_qam_mcs_table_alt_present, 1));
      HANDLE_CODE(bref.pack(cqi_table_alt_present, 1));
      HANDLE_CODE(bref.pack(one_fl_dmrs_two_add_dmrs_ul_present, 1));
      HANDLE_CODE(bref.pack(two_fl_dmrs_two_add_dmrs_ul_present, 1));
      HANDLE_CODE(bref.pack(one_fl_dmrs_three_add_dmrs_ul_present, 1));
      if (csi_rs_im_reception_for_feedback.is_present()) {
        HANDLE_CODE(csi_rs_im_reception_for_feedback->pack(bref));
      }
      if (csi_rs_proc_framework_for_srs.is_present()) {
        HANDLE_CODE(csi_rs_proc_framework_for_srs->pack(bref));
      }
      if (csi_report_framework.is_present()) {
        HANDLE_CODE(csi_report_framework->pack(bref));
      }
      if (mux_sr_harq_ack_csi_pucch_once_per_slot.is_present()) {
        HANDLE_CODE(bref.pack(mux_sr_harq_ack_csi_pucch_once_per_slot->same_symbol_present, 1));
        HANDLE_CODE(bref.pack(mux_sr_harq_ack_csi_pucch_once_per_slot->diff_symbol_present, 1));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdcch_blind_detection_nrdc.is_present(), 1));
      HANDLE_CODE(bref.pack(mux_harq_ack_pusch_diff_symbol_present, 1));
      if (pdcch_blind_detection_nrdc.is_present()) {
        HANDLE_CODE(
            pack_integer(bref, pdcch_blind_detection_nrdc->pdcch_blind_detection_mcg_ue, (uint8_t)1u, (uint8_t)15u));
        HANDLE_CODE(
            pack_integer(bref, pdcch_blind_detection_nrdc->pdcch_blind_detection_scg_ue, (uint8_t)1u, (uint8_t)15u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_frx_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dyn_sfi_present, 1));
  HANDLE_CODE(bref.unpack(dummy1_present, 1));
  HANDLE_CODE(bref.unpack(two_fl_dmrs_present, 1));
  HANDLE_CODE(bref.unpack(dummy2_present, 1));
  HANDLE_CODE(bref.unpack(dummy3_present, 1));
  HANDLE_CODE(bref.unpack(supported_dmrs_type_dl_present, 1));
  HANDLE_CODE(bref.unpack(supported_dmrs_type_ul_present, 1));
  HANDLE_CODE(bref.unpack(semi_open_loop_csi_present, 1));
  HANDLE_CODE(bref.unpack(csi_report_without_pmi_present, 1));
  HANDLE_CODE(bref.unpack(csi_report_without_cqi_present, 1));
  HANDLE_CODE(bref.unpack(one_ports_ptrs_present, 1));
  HANDLE_CODE(bref.unpack(two_pucch_f0_2_consec_symbols_present, 1));
  HANDLE_CODE(bref.unpack(pucch_f2_with_fh_present, 1));
  HANDLE_CODE(bref.unpack(pucch_f3_with_fh_present, 1));
  HANDLE_CODE(bref.unpack(pucch_f4_with_fh_present, 1));
  HANDLE_CODE(bref.unpack(pucch_f0_2_without_fh_present, 1));
  HANDLE_CODE(bref.unpack(pucch_f1_3_4_without_fh_present, 1));
  HANDLE_CODE(bref.unpack(mux_sr_harq_ack_csi_pucch_multi_per_slot_present, 1));
  HANDLE_CODE(bref.unpack(uci_code_block_segmentation_present, 1));
  HANDLE_CODE(bref.unpack(one_pucch_long_and_short_format_present, 1));
  HANDLE_CODE(bref.unpack(two_pucch_any_others_in_slot_present, 1));
  HANDLE_CODE(bref.unpack(intra_slot_freq_hop_pusch_present, 1));
  HANDLE_CODE(bref.unpack(pusch_lbrm_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_blind_detection_ca_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pusch_rnti_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pucch_rnti_present, 1));
  HANDLE_CODE(bref.unpack(tpc_srs_rnti_present, 1));
  HANDLE_CODE(bref.unpack(absolute_tpc_cmd_present, 1));
  HANDLE_CODE(bref.unpack(two_different_tpc_loop_pusch_present, 1));
  HANDLE_CODE(bref.unpack(two_different_tpc_loop_pucch_present, 1));
  HANDLE_CODE(bref.unpack(pusch_half_pi_bpsk_present, 1));
  HANDLE_CODE(bref.unpack(pucch_f3_4_half_pi_bpsk_present, 1));
  HANDLE_CODE(bref.unpack(almost_contiguous_cp_ofdm_ul_present, 1));
  HANDLE_CODE(bref.unpack(sp_csi_rs_present, 1));
  HANDLE_CODE(bref.unpack(sp_csi_im_present, 1));
  HANDLE_CODE(bref.unpack(tdd_multi_dl_ul_switch_per_slot_present, 1));
  HANDLE_CODE(bref.unpack(multiple_coreset_present, 1));

  if (dummy1_present) {
    HANDLE_CODE(dummy1.unpack(bref));
  }
  if (two_fl_dmrs_present) {
    HANDLE_CODE(two_fl_dmrs.unpack(bref));
  }
  if (dummy2_present) {
    HANDLE_CODE(dummy2.unpack(bref));
  }
  if (dummy3_present) {
    HANDLE_CODE(dummy3.unpack(bref));
  }
  if (supported_dmrs_type_dl_present) {
    HANDLE_CODE(supported_dmrs_type_dl.unpack(bref));
  }
  if (supported_dmrs_type_ul_present) {
    HANDLE_CODE(supported_dmrs_type_ul.unpack(bref));
  }
  if (one_ports_ptrs_present) {
    HANDLE_CODE(one_ports_ptrs.unpack(bref));
  }
  if (pdcch_blind_detection_ca_present) {
    HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca, bref, (uint8_t)4u, (uint8_t)16u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool csi_rs_im_reception_for_feedback_present;
      HANDLE_CODE(bref.unpack(csi_rs_im_reception_for_feedback_present, 1));
      csi_rs_im_reception_for_feedback.set_present(csi_rs_im_reception_for_feedback_present);
      bool csi_rs_proc_framework_for_srs_present;
      HANDLE_CODE(bref.unpack(csi_rs_proc_framework_for_srs_present, 1));
      csi_rs_proc_framework_for_srs.set_present(csi_rs_proc_framework_for_srs_present);
      bool csi_report_framework_present;
      HANDLE_CODE(bref.unpack(csi_report_framework_present, 1));
      csi_report_framework.set_present(csi_report_framework_present);
      bool mux_sr_harq_ack_csi_pucch_once_per_slot_present;
      HANDLE_CODE(bref.unpack(mux_sr_harq_ack_csi_pucch_once_per_slot_present, 1));
      mux_sr_harq_ack_csi_pucch_once_per_slot.set_present(mux_sr_harq_ack_csi_pucch_once_per_slot_present);
      HANDLE_CODE(bref.unpack(mux_sr_harq_ack_pucch_present, 1));
      HANDLE_CODE(bref.unpack(mux_multiple_group_ctrl_ch_overlap_present, 1));
      HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_present, 1));
      HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_present, 1));
      HANDLE_CODE(bref.unpack(ul_sched_offset_present, 1));
      HANDLE_CODE(bref.unpack(dl_64_qam_mcs_table_alt_present, 1));
      HANDLE_CODE(bref.unpack(ul_64_qam_mcs_table_alt_present, 1));
      HANDLE_CODE(bref.unpack(cqi_table_alt_present, 1));
      HANDLE_CODE(bref.unpack(one_fl_dmrs_two_add_dmrs_ul_present, 1));
      HANDLE_CODE(bref.unpack(two_fl_dmrs_two_add_dmrs_ul_present, 1));
      HANDLE_CODE(bref.unpack(one_fl_dmrs_three_add_dmrs_ul_present, 1));
      if (csi_rs_im_reception_for_feedback.is_present()) {
        HANDLE_CODE(csi_rs_im_reception_for_feedback->unpack(bref));
      }
      if (csi_rs_proc_framework_for_srs.is_present()) {
        HANDLE_CODE(csi_rs_proc_framework_for_srs->unpack(bref));
      }
      if (csi_report_framework.is_present()) {
        HANDLE_CODE(csi_report_framework->unpack(bref));
      }
      if (mux_sr_harq_ack_csi_pucch_once_per_slot.is_present()) {
        HANDLE_CODE(bref.unpack(mux_sr_harq_ack_csi_pucch_once_per_slot->same_symbol_present, 1));
        HANDLE_CODE(bref.unpack(mux_sr_harq_ack_csi_pucch_once_per_slot->diff_symbol_present, 1));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pdcch_blind_detection_nrdc_present;
      HANDLE_CODE(bref.unpack(pdcch_blind_detection_nrdc_present, 1));
      pdcch_blind_detection_nrdc.set_present(pdcch_blind_detection_nrdc_present);
      HANDLE_CODE(bref.unpack(mux_harq_ack_pusch_diff_symbol_present, 1));
      if (pdcch_blind_detection_nrdc.is_present()) {
        HANDLE_CODE(
            unpack_integer(pdcch_blind_detection_nrdc->pdcch_blind_detection_mcg_ue, bref, (uint8_t)1u, (uint8_t)15u));
        HANDLE_CODE(
            unpack_integer(pdcch_blind_detection_nrdc->pdcch_blind_detection_scg_ue, bref, (uint8_t)1u, (uint8_t)15u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phy_params_frx_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dyn_sfi_present) {
    j.write_str("dynamicSFI", "supported");
  }
  if (dummy1_present) {
    j.write_str("dummy1", dummy1.to_string());
  }
  if (two_fl_dmrs_present) {
    j.write_str("twoFL-DMRS", two_fl_dmrs.to_string());
  }
  if (dummy2_present) {
    j.write_str("dummy2", dummy2.to_string());
  }
  if (dummy3_present) {
    j.write_str("dummy3", dummy3.to_string());
  }
  if (supported_dmrs_type_dl_present) {
    j.write_str("supportedDMRS-TypeDL", supported_dmrs_type_dl.to_string());
  }
  if (supported_dmrs_type_ul_present) {
    j.write_str("supportedDMRS-TypeUL", supported_dmrs_type_ul.to_string());
  }
  if (semi_open_loop_csi_present) {
    j.write_str("semiOpenLoopCSI", "supported");
  }
  if (csi_report_without_pmi_present) {
    j.write_str("csi-ReportWithoutPMI", "supported");
  }
  if (csi_report_without_cqi_present) {
    j.write_str("csi-ReportWithoutCQI", "supported");
  }
  if (one_ports_ptrs_present) {
    j.write_str("onePortsPTRS", one_ports_ptrs.to_string());
  }
  if (two_pucch_f0_2_consec_symbols_present) {
    j.write_str("twoPUCCH-F0-2-ConsecSymbols", "supported");
  }
  if (pucch_f2_with_fh_present) {
    j.write_str("pucch-F2-WithFH", "supported");
  }
  if (pucch_f3_with_fh_present) {
    j.write_str("pucch-F3-WithFH", "supported");
  }
  if (pucch_f4_with_fh_present) {
    j.write_str("pucch-F4-WithFH", "supported");
  }
  if (pucch_f0_2_without_fh_present) {
    j.write_str("pucch-F0-2WithoutFH", "notSupported");
  }
  if (pucch_f1_3_4_without_fh_present) {
    j.write_str("pucch-F1-3-4WithoutFH", "notSupported");
  }
  if (mux_sr_harq_ack_csi_pucch_multi_per_slot_present) {
    j.write_str("mux-SR-HARQ-ACK-CSI-PUCCH-MultiPerSlot", "supported");
  }
  if (uci_code_block_segmentation_present) {
    j.write_str("uci-CodeBlockSegmentation", "supported");
  }
  if (one_pucch_long_and_short_format_present) {
    j.write_str("onePUCCH-LongAndShortFormat", "supported");
  }
  if (two_pucch_any_others_in_slot_present) {
    j.write_str("twoPUCCH-AnyOthersInSlot", "supported");
  }
  if (intra_slot_freq_hop_pusch_present) {
    j.write_str("intraSlotFreqHopping-PUSCH", "supported");
  }
  if (pusch_lbrm_present) {
    j.write_str("pusch-LBRM", "supported");
  }
  if (pdcch_blind_detection_ca_present) {
    j.write_int("pdcch-BlindDetectionCA", pdcch_blind_detection_ca);
  }
  if (tpc_pusch_rnti_present) {
    j.write_str("tpc-PUSCH-RNTI", "supported");
  }
  if (tpc_pucch_rnti_present) {
    j.write_str("tpc-PUCCH-RNTI", "supported");
  }
  if (tpc_srs_rnti_present) {
    j.write_str("tpc-SRS-RNTI", "supported");
  }
  if (absolute_tpc_cmd_present) {
    j.write_str("absoluteTPC-Command", "supported");
  }
  if (two_different_tpc_loop_pusch_present) {
    j.write_str("twoDifferentTPC-Loop-PUSCH", "supported");
  }
  if (two_different_tpc_loop_pucch_present) {
    j.write_str("twoDifferentTPC-Loop-PUCCH", "supported");
  }
  if (pusch_half_pi_bpsk_present) {
    j.write_str("pusch-HalfPi-BPSK", "supported");
  }
  if (pucch_f3_4_half_pi_bpsk_present) {
    j.write_str("pucch-F3-4-HalfPi-BPSK", "supported");
  }
  if (almost_contiguous_cp_ofdm_ul_present) {
    j.write_str("almostContiguousCP-OFDM-UL", "supported");
  }
  if (sp_csi_rs_present) {
    j.write_str("sp-CSI-RS", "supported");
  }
  if (sp_csi_im_present) {
    j.write_str("sp-CSI-IM", "supported");
  }
  if (tdd_multi_dl_ul_switch_per_slot_present) {
    j.write_str("tdd-MultiDL-UL-SwitchPerSlot", "supported");
  }
  if (multiple_coreset_present) {
    j.write_str("multipleCORESET", "supported");
  }
  if (ext) {
    if (csi_rs_im_reception_for_feedback.is_present()) {
      j.write_fieldname("csi-RS-IM-ReceptionForFeedback");
      csi_rs_im_reception_for_feedback->to_json(j);
    }
    if (csi_rs_proc_framework_for_srs.is_present()) {
      j.write_fieldname("csi-RS-ProcFrameworkForSRS");
      csi_rs_proc_framework_for_srs->to_json(j);
    }
    if (csi_report_framework.is_present()) {
      j.write_fieldname("csi-ReportFramework");
      csi_report_framework->to_json(j);
    }
    if (mux_sr_harq_ack_csi_pucch_once_per_slot.is_present()) {
      j.write_fieldname("mux-SR-HARQ-ACK-CSI-PUCCH-OncePerSlot");
      j.start_obj();
      if (mux_sr_harq_ack_csi_pucch_once_per_slot->same_symbol_present) {
        j.write_str("sameSymbol", "supported");
      }
      if (mux_sr_harq_ack_csi_pucch_once_per_slot->diff_symbol_present) {
        j.write_str("diffSymbol", "supported");
      }
      j.end_obj();
    }
    if (mux_sr_harq_ack_pucch_present) {
      j.write_str("mux-SR-HARQ-ACK-PUCCH", "supported");
    }
    if (mux_multiple_group_ctrl_ch_overlap_present) {
      j.write_str("mux-MultipleGroupCtrlCH-Overlap", "supported");
    }
    if (dl_sched_offset_pdsch_type_a_present) {
      j.write_str("dl-SchedulingOffset-PDSCH-TypeA", "supported");
    }
    if (dl_sched_offset_pdsch_type_b_present) {
      j.write_str("dl-SchedulingOffset-PDSCH-TypeB", "supported");
    }
    if (ul_sched_offset_present) {
      j.write_str("ul-SchedulingOffset", "supported");
    }
    if (dl_64_qam_mcs_table_alt_present) {
      j.write_str("dl-64QAM-MCS-TableAlt", "supported");
    }
    if (ul_64_qam_mcs_table_alt_present) {
      j.write_str("ul-64QAM-MCS-TableAlt", "supported");
    }
    if (cqi_table_alt_present) {
      j.write_str("cqi-TableAlt", "supported");
    }
    if (one_fl_dmrs_two_add_dmrs_ul_present) {
      j.write_str("oneFL-DMRS-TwoAdditionalDMRS-UL", "supported");
    }
    if (two_fl_dmrs_two_add_dmrs_ul_present) {
      j.write_str("twoFL-DMRS-TwoAdditionalDMRS-UL", "supported");
    }
    if (one_fl_dmrs_three_add_dmrs_ul_present) {
      j.write_str("oneFL-DMRS-ThreeAdditionalDMRS-UL", "supported");
    }
    if (pdcch_blind_detection_nrdc.is_present()) {
      j.write_fieldname("pdcch-BlindDetectionNRDC");
      j.start_obj();
      j.write_int("pdcch-BlindDetectionMCG-UE", pdcch_blind_detection_nrdc->pdcch_blind_detection_mcg_ue);
      j.write_int("pdcch-BlindDetectionSCG-UE", pdcch_blind_detection_nrdc->pdcch_blind_detection_scg_ue);
      j.end_obj();
    }
    if (mux_harq_ack_pusch_diff_symbol_present) {
      j.write_str("mux-HARQ-ACK-PUSCH-DiffSymbol", "supported");
    }
  }
  j.end_obj();
}

const char* phy_params_frx_diff_s::supported_dmrs_type_dl_opts::to_string() const
{
  static const char* names[] = {"type1", "type1And2"};
  return convert_enum_idx(names, 2, value, "phy_params_frx_diff_s::supported_dmrs_type_dl_e_");
}

const char* phy_params_frx_diff_s::supported_dmrs_type_ul_opts::to_string() const
{
  static const char* names[] = {"type1", "type1And2"};
  return convert_enum_idx(names, 2, value, "phy_params_frx_diff_s::supported_dmrs_type_ul_e_");
}

// Phy-ParametersXDD-Diff ::= SEQUENCE
SRSASN_CODE phy_params_xdd_diff_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dyn_sfi_present, 1));
  HANDLE_CODE(bref.pack(two_pucch_f0_2_consec_symbols_present, 1));
  HANDLE_CODE(bref.pack(two_different_tpc_loop_pusch_present, 1));
  HANDLE_CODE(bref.pack(two_different_tpc_loop_pucch_present, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dl_sched_offset_pdsch_type_a_present;
    group_flags[0] |= dl_sched_offset_pdsch_type_b_present;
    group_flags[0] |= ul_sched_offset_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_present, 1));
      HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_present, 1));
      HANDLE_CODE(bref.pack(ul_sched_offset_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_xdd_diff_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dyn_sfi_present, 1));
  HANDLE_CODE(bref.unpack(two_pucch_f0_2_consec_symbols_present, 1));
  HANDLE_CODE(bref.unpack(two_different_tpc_loop_pusch_present, 1));
  HANDLE_CODE(bref.unpack(two_different_tpc_loop_pucch_present, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_present, 1));
      HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_present, 1));
      HANDLE_CODE(bref.unpack(ul_sched_offset_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void phy_params_xdd_diff_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dyn_sfi_present) {
    j.write_str("dynamicSFI", "supported");
  }
  if (two_pucch_f0_2_consec_symbols_present) {
    j.write_str("twoPUCCH-F0-2-ConsecSymbols", "supported");
  }
  if (two_different_tpc_loop_pusch_present) {
    j.write_str("twoDifferentTPC-Loop-PUSCH", "supported");
  }
  if (two_different_tpc_loop_pucch_present) {
    j.write_str("twoDifferentTPC-Loop-PUCCH", "supported");
  }
  if (ext) {
    if (dl_sched_offset_pdsch_type_a_present) {
      j.write_str("dl-SchedulingOffset-PDSCH-TypeA", "supported");
    }
    if (dl_sched_offset_pdsch_type_b_present) {
      j.write_str("dl-SchedulingOffset-PDSCH-TypeB", "supported");
    }
    if (ul_sched_offset_present) {
      j.write_str("ul-SchedulingOffset", "supported");
    }
  }
  j.end_obj();
}

// Phy-Parameters ::= SEQUENCE
SRSASN_CODE phy_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_params_common_present, 1));
  HANDLE_CODE(bref.pack(phy_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.pack(phy_params_frx_diff_present, 1));
  HANDLE_CODE(bref.pack(phy_params_fr1_present, 1));
  HANDLE_CODE(bref.pack(phy_params_fr2_present, 1));

  if (phy_params_common_present) {
    HANDLE_CODE(phy_params_common.pack(bref));
  }
  if (phy_params_xdd_diff_present) {
    HANDLE_CODE(phy_params_xdd_diff.pack(bref));
  }
  if (phy_params_frx_diff_present) {
    HANDLE_CODE(phy_params_frx_diff.pack(bref));
  }
  if (phy_params_fr1_present) {
    HANDLE_CODE(phy_params_fr1.pack(bref));
  }
  if (phy_params_fr2_present) {
    HANDLE_CODE(phy_params_fr2.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_params_common_present, 1));
  HANDLE_CODE(bref.unpack(phy_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.unpack(phy_params_frx_diff_present, 1));
  HANDLE_CODE(bref.unpack(phy_params_fr1_present, 1));
  HANDLE_CODE(bref.unpack(phy_params_fr2_present, 1));

  if (phy_params_common_present) {
    HANDLE_CODE(phy_params_common.unpack(bref));
  }
  if (phy_params_xdd_diff_present) {
    HANDLE_CODE(phy_params_xdd_diff.unpack(bref));
  }
  if (phy_params_frx_diff_present) {
    HANDLE_CODE(phy_params_frx_diff.unpack(bref));
  }
  if (phy_params_fr1_present) {
    HANDLE_CODE(phy_params_fr1.unpack(bref));
  }
  if (phy_params_fr2_present) {
    HANDLE_CODE(phy_params_fr2.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phy_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_params_common_present) {
    j.write_fieldname("phy-ParametersCommon");
    phy_params_common.to_json(j);
  }
  if (phy_params_xdd_diff_present) {
    j.write_fieldname("phy-ParametersXDD-Diff");
    phy_params_xdd_diff.to_json(j);
  }
  if (phy_params_frx_diff_present) {
    j.write_fieldname("phy-ParametersFRX-Diff");
    phy_params_frx_diff.to_json(j);
  }
  if (phy_params_fr1_present) {
    j.write_fieldname("phy-ParametersFR1");
    phy_params_fr1.to_json(j);
  }
  if (phy_params_fr2_present) {
    j.write_fieldname("phy-ParametersFR2");
    phy_params_fr2.to_json(j);
  }
  j.end_obj();
}

// RF-Parameters ::= SEQUENCE
SRSASN_CODE rf_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(supported_band_combination_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(applied_freq_band_list_filt.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_nr, 1, 1024));
  if (supported_band_combination_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list, 1, 65536));
  }
  if (applied_freq_band_list_filt.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, applied_freq_band_list_filt, 1, 1280));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= supported_band_combination_list_v1540.is_present();
    group_flags[0] |= srs_switching_time_requested_present;
    group_flags[1] |= supported_band_combination_list_v1550.is_present();
    group_flags[2] |= supported_band_combination_list_v1560.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1540.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_switching_time_requested_present, 1));
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
      if (supported_band_combination_list_v1560.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1560, 1, 65536));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool supported_band_combination_list_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_present, 1));
  bool applied_freq_band_list_filt_present;
  HANDLE_CODE(bref.unpack(applied_freq_band_list_filt_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_nr, bref, 1, 1024));
  if (supported_band_combination_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list, bref, 1, 65536));
  }
  if (applied_freq_band_list_filt_present) {
    HANDLE_CODE(unpack_dyn_seq_of(applied_freq_band_list_filt, bref, 1, 1280));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool supported_band_combination_list_v1540_present;
      HANDLE_CODE(bref.unpack(supported_band_combination_list_v1540_present, 1));
      supported_band_combination_list_v1540.set_present(supported_band_combination_list_v1540_present);
      HANDLE_CODE(bref.unpack(srs_switching_time_requested_present, 1));
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
      if (supported_band_combination_list_v1560.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1560, bref, 1, 65536));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rf_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListNR");
  for (const auto& e1 : supported_band_list_nr) {
    e1.to_json(j);
  }
  j.end_array();
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
    if (supported_band_combination_list_v1540.is_present()) {
      j.start_array("supportedBandCombinationList-v1540");
      for (const auto& e1 : *supported_band_combination_list_v1540) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srs_switching_time_requested_present) {
      j.write_str("srs-SwitchingTimeRequested", "true");
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
  }
  j.end_obj();
}

// NRDC-Parameters-v1570 ::= SEQUENCE
SRSASN_CODE nrdc_params_v1570_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sfn_sync_nrdc_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nrdc_params_v1570_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sfn_sync_nrdc_present, 1));

  return SRSASN_SUCCESS;
}
void nrdc_params_v1570_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sfn_sync_nrdc_present) {
    j.write_str("sfn-SyncNRDC", "supported");
  }
  j.end_obj();
}

// UE-NR-Capability-v1570 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v1570_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nrdc_params_v1570_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (nrdc_params_v1570_present) {
    HANDLE_CODE(nrdc_params_v1570.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v1570_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nrdc_params_v1570_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (nrdc_params_v1570_present) {
    HANDLE_CODE(nrdc_params_v1570.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v1570_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nrdc_params_v1570_present) {
    j.write_fieldname("nrdc-Parameters-v1570");
    nrdc_params_v1570.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UE-NR-Capability-v1560 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nrdc_params_present, 1));
  HANDLE_CODE(bref.pack(rx_filts.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (nrdc_params_present) {
    HANDLE_CODE(nrdc_params.pack(bref));
  }
  if (rx_filts.size() > 0) {
    HANDLE_CODE(rx_filts.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nrdc_params_present, 1));
  bool rx_filts_present;
  HANDLE_CODE(bref.unpack(rx_filts_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (nrdc_params_present) {
    HANDLE_CODE(nrdc_params.unpack(bref));
  }
  if (rx_filts_present) {
    HANDLE_CODE(rx_filts.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nrdc_params_present) {
    j.write_fieldname("nrdc-Parameters");
    nrdc_params.to_json(j);
  }
  if (rx_filts.size() > 0) {
    j.write_str("receivedFilters", rx_filts.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SDAP-Parameters ::= SEQUENCE
SRSASN_CODE sdap_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(as_reflective_qos_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdap_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(as_reflective_qos_present, 1));

  return SRSASN_SUCCESS;
}
void sdap_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (as_reflective_qos_present) {
    j.write_str("as-ReflectiveQoS", "true");
  }
  j.end_obj();
}

// UE-NR-Capability-v1550 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v1550_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_cp_latency_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v1550_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_cp_latency_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v1550_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_cp_latency_present) {
    j.write_str("reducedCP-Latency", "supported");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-NR-CapabilityAddFRX-Mode ::= SEQUENCE
SRSASN_CODE ue_nr_cap_add_frx_mode_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_params_frx_diff_present, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_frx_diff_present, 1));

  if (phy_params_frx_diff_present) {
    HANDLE_CODE(phy_params_frx_diff.pack(bref));
  }
  if (meas_and_mob_params_frx_diff_present) {
    HANDLE_CODE(meas_and_mob_params_frx_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_add_frx_mode_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_params_frx_diff_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_frx_diff_present, 1));

  if (phy_params_frx_diff_present) {
    HANDLE_CODE(phy_params_frx_diff.unpack(bref));
  }
  if (meas_and_mob_params_frx_diff_present) {
    HANDLE_CODE(meas_and_mob_params_frx_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_add_frx_mode_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_params_frx_diff_present) {
    j.write_fieldname("phy-ParametersFRX-Diff");
    phy_params_frx_diff.to_json(j);
  }
  if (meas_and_mob_params_frx_diff_present) {
    j.write_fieldname("measAndMobParametersFRX-Diff");
    meas_and_mob_params_frx_diff.to_json(j);
  }
  j.end_obj();
}

// UE-NR-CapabilityAddFRX-Mode-v1540 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_add_frx_mode_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ims_params_frx_diff_present, 1));

  if (ims_params_frx_diff_present) {
    HANDLE_CODE(ims_params_frx_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_add_frx_mode_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ims_params_frx_diff_present, 1));

  if (ims_params_frx_diff_present) {
    HANDLE_CODE(ims_params_frx_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_add_frx_mode_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ims_params_frx_diff_present) {
    j.write_fieldname("ims-ParametersFRX-Diff");
    ims_params_frx_diff.to_json(j);
  }
  j.end_obj();
}

// UE-NR-Capability-v1540 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sdap_params_present, 1));
  HANDLE_CODE(bref.pack(overheat_ind_present, 1));
  HANDLE_CODE(bref.pack(ims_params_present, 1));
  HANDLE_CODE(bref.pack(fr1_add_ue_nr_cap_v1540_present, 1));
  HANDLE_CODE(bref.pack(fr2_add_ue_nr_cap_v1540_present, 1));
  HANDLE_CODE(bref.pack(fr1_fr2_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sdap_params_present) {
    HANDLE_CODE(sdap_params.pack(bref));
  }
  if (ims_params_present) {
    HANDLE_CODE(ims_params.pack(bref));
  }
  if (fr1_add_ue_nr_cap_v1540_present) {
    HANDLE_CODE(fr1_add_ue_nr_cap_v1540.pack(bref));
  }
  if (fr2_add_ue_nr_cap_v1540_present) {
    HANDLE_CODE(fr2_add_ue_nr_cap_v1540.pack(bref));
  }
  if (fr1_fr2_add_ue_nr_cap_present) {
    HANDLE_CODE(fr1_fr2_add_ue_nr_cap.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sdap_params_present, 1));
  HANDLE_CODE(bref.unpack(overheat_ind_present, 1));
  HANDLE_CODE(bref.unpack(ims_params_present, 1));
  HANDLE_CODE(bref.unpack(fr1_add_ue_nr_cap_v1540_present, 1));
  HANDLE_CODE(bref.unpack(fr2_add_ue_nr_cap_v1540_present, 1));
  HANDLE_CODE(bref.unpack(fr1_fr2_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sdap_params_present) {
    HANDLE_CODE(sdap_params.unpack(bref));
  }
  if (ims_params_present) {
    HANDLE_CODE(ims_params.unpack(bref));
  }
  if (fr1_add_ue_nr_cap_v1540_present) {
    HANDLE_CODE(fr1_add_ue_nr_cap_v1540.unpack(bref));
  }
  if (fr2_add_ue_nr_cap_v1540_present) {
    HANDLE_CODE(fr2_add_ue_nr_cap_v1540.unpack(bref));
  }
  if (fr1_fr2_add_ue_nr_cap_present) {
    HANDLE_CODE(fr1_fr2_add_ue_nr_cap.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sdap_params_present) {
    j.write_fieldname("sdap-Parameters");
    sdap_params.to_json(j);
  }
  if (overheat_ind_present) {
    j.write_str("overheatingInd", "supported");
  }
  if (ims_params_present) {
    j.write_fieldname("ims-Parameters");
    ims_params.to_json(j);
  }
  if (fr1_add_ue_nr_cap_v1540_present) {
    j.write_fieldname("fr1-Add-UE-NR-Capabilities-v1540");
    fr1_add_ue_nr_cap_v1540.to_json(j);
  }
  if (fr2_add_ue_nr_cap_v1540_present) {
    j.write_fieldname("fr2-Add-UE-NR-Capabilities-v1540");
    fr2_add_ue_nr_cap_v1540.to_json(j);
  }
  if (fr1_fr2_add_ue_nr_cap_present) {
    j.write_fieldname("fr1-fr2-Add-UE-NR-Capabilities");
    fr1_fr2_add_ue_nr_cap.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-NR-CapabilityAddXDD-Mode-v1530 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_add_xdd_mode_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(eutra_params_xdd_diff.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_add_xdd_mode_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(eutra_params_xdd_diff.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_nr_cap_add_xdd_mode_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eutra-ParametersXDD-Diff");
  eutra_params_xdd_diff.to_json(j);
  j.end_obj();
}

// AccessStratumRelease ::= ENUMERATED
const char* access_stratum_release_opts::to_string() const
{
  static const char* names[] = {"rel15", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "access_stratum_release_e");
}
uint8_t access_stratum_release_opts::to_number() const
{
  static const uint8_t numbers[] = {15};
  return map_enum_number(numbers, 1, value, "access_stratum_release_e");
}

// PDCP-Parameters ::= SEQUENCE
SRSASN_CODE pdcp_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_only_rohc_profiles_present, 1));
  HANDLE_CODE(bref.pack(continue_rohc_context_present, 1));
  HANDLE_CODE(bref.pack(out_of_order_delivery_present, 1));
  HANDLE_CODE(bref.pack(short_sn_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_srb_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_mcg_or_scg_drb_present, 1));

  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0000, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0001, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0002, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0003, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0004, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0006, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0101, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0102, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0103, 1));
  HANDLE_CODE(bref.pack(supported_rohc_profiles.profile0x0104, 1));
  HANDLE_CODE(max_num_rohc_context_sessions.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_only_rohc_profiles_present, 1));
  HANDLE_CODE(bref.unpack(continue_rohc_context_present, 1));
  HANDLE_CODE(bref.unpack(out_of_order_delivery_present, 1));
  HANDLE_CODE(bref.unpack(short_sn_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_srb_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_mcg_or_scg_drb_present, 1));

  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0000, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0001, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0002, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0003, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0004, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0006, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0101, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0102, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0103, 1));
  HANDLE_CODE(bref.unpack(supported_rohc_profiles.profile0x0104, 1));
  HANDLE_CODE(max_num_rohc_context_sessions.unpack(bref));

  return SRSASN_SUCCESS;
}
void pdcp_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("supportedROHC-Profiles");
  j.start_obj();
  j.write_bool("profile0x0000", supported_rohc_profiles.profile0x0000);
  j.write_bool("profile0x0001", supported_rohc_profiles.profile0x0001);
  j.write_bool("profile0x0002", supported_rohc_profiles.profile0x0002);
  j.write_bool("profile0x0003", supported_rohc_profiles.profile0x0003);
  j.write_bool("profile0x0004", supported_rohc_profiles.profile0x0004);
  j.write_bool("profile0x0006", supported_rohc_profiles.profile0x0006);
  j.write_bool("profile0x0101", supported_rohc_profiles.profile0x0101);
  j.write_bool("profile0x0102", supported_rohc_profiles.profile0x0102);
  j.write_bool("profile0x0103", supported_rohc_profiles.profile0x0103);
  j.write_bool("profile0x0104", supported_rohc_profiles.profile0x0104);
  j.end_obj();
  j.write_str("maxNumberROHC-ContextSessions", max_num_rohc_context_sessions.to_string());
  if (ul_only_rohc_profiles_present) {
    j.write_str("uplinkOnlyROHC-Profiles", "supported");
  }
  if (continue_rohc_context_present) {
    j.write_str("continueROHC-Context", "supported");
  }
  if (out_of_order_delivery_present) {
    j.write_str("outOfOrderDelivery", "supported");
  }
  if (short_sn_present) {
    j.write_str("shortSN", "supported");
  }
  if (pdcp_dupl_srb_present) {
    j.write_str("pdcp-DuplicationSRB", "supported");
  }
  if (pdcp_dupl_mcg_or_scg_drb_present) {
    j.write_str("pdcp-DuplicationMCG-OrSCG-DRB", "supported");
  }
  j.end_obj();
}

const char* pdcp_params_s::max_num_rohc_context_sessions_opts::to_string() const
{
  static const char* names[] = {"cs2",
                                "cs4",
                                "cs8",
                                "cs12",
                                "cs16",
                                "cs24",
                                "cs32",
                                "cs48",
                                "cs64",
                                "cs128",
                                "cs256",
                                "cs512",
                                "cs1024",
                                "cs16384",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
}
uint16_t pdcp_params_s::max_num_rohc_context_sessions_opts::to_number() const
{
  static const uint16_t numbers[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return map_enum_number(numbers, 14, value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
}

// RLC-Parameters ::= SEQUENCE
SRSASN_CODE rlc_params_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(am_with_short_sn_present, 1));
  HANDLE_CODE(bref.pack(um_with_short_sn_present, 1));
  HANDLE_CODE(bref.pack(um_with_long_sn_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_params_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(am_with_short_sn_present, 1));
  HANDLE_CODE(bref.unpack(um_with_short_sn_present, 1));
  HANDLE_CODE(bref.unpack(um_with_long_sn_present, 1));

  return SRSASN_SUCCESS;
}
void rlc_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (am_with_short_sn_present) {
    j.write_str("am-WithShortSN", "supported");
  }
  if (um_with_short_sn_present) {
    j.write_str("um-WithShortSN", "supported");
  }
  if (um_with_long_sn_present) {
    j.write_str("um-WithLongSN", "supported");
  }
  j.end_obj();
}

// UE-NR-Capability-v1530 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fdd_add_ue_nr_cap_v1530_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_nr_cap_v1530_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(inter_rat_params_present, 1));
  HANDLE_CODE(bref.pack(inactive_state_present, 1));
  HANDLE_CODE(bref.pack(delay_budget_report_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fdd_add_ue_nr_cap_v1530_present) {
    HANDLE_CODE(fdd_add_ue_nr_cap_v1530.pack(bref));
  }
  if (tdd_add_ue_nr_cap_v1530_present) {
    HANDLE_CODE(tdd_add_ue_nr_cap_v1530.pack(bref));
  }
  if (inter_rat_params_present) {
    HANDLE_CODE(inter_rat_params.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fdd_add_ue_nr_cap_v1530_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_nr_cap_v1530_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(inter_rat_params_present, 1));
  HANDLE_CODE(bref.unpack(inactive_state_present, 1));
  HANDLE_CODE(bref.unpack(delay_budget_report_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fdd_add_ue_nr_cap_v1530_present) {
    HANDLE_CODE(fdd_add_ue_nr_cap_v1530.unpack(bref));
  }
  if (tdd_add_ue_nr_cap_v1530_present) {
    HANDLE_CODE(tdd_add_ue_nr_cap_v1530.unpack(bref));
  }
  if (inter_rat_params_present) {
    HANDLE_CODE(inter_rat_params.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fdd_add_ue_nr_cap_v1530_present) {
    j.write_fieldname("fdd-Add-UE-NR-Capabilities-v1530");
    fdd_add_ue_nr_cap_v1530.to_json(j);
  }
  if (tdd_add_ue_nr_cap_v1530_present) {
    j.write_fieldname("tdd-Add-UE-NR-Capabilities-v1530");
    tdd_add_ue_nr_cap_v1530.to_json(j);
  }
  if (dummy_present) {
    j.write_str("dummy", "supported");
  }
  if (inter_rat_params_present) {
    j.write_fieldname("interRAT-Parameters");
    inter_rat_params.to_json(j);
  }
  if (inactive_state_present) {
    j.write_str("inactiveState", "supported");
  }
  if (delay_budget_report_present) {
    j.write_str("delayBudgetReporting", "supported");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-NR-CapabilityAddXDD-Mode ::= SEQUENCE
SRSASN_CODE ue_nr_cap_add_xdd_mode_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.pack(mac_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_xdd_diff_present, 1));

  if (phy_params_xdd_diff_present) {
    HANDLE_CODE(phy_params_xdd_diff.pack(bref));
  }
  if (mac_params_xdd_diff_present) {
    HANDLE_CODE(mac_params_xdd_diff.pack(bref));
  }
  if (meas_and_mob_params_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_xdd_diff.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_add_xdd_mode_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_xdd_diff_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_xdd_diff_present, 1));

  if (phy_params_xdd_diff_present) {
    HANDLE_CODE(phy_params_xdd_diff.unpack(bref));
  }
  if (mac_params_xdd_diff_present) {
    HANDLE_CODE(mac_params_xdd_diff.unpack(bref));
  }
  if (meas_and_mob_params_xdd_diff_present) {
    HANDLE_CODE(meas_and_mob_params_xdd_diff.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_add_xdd_mode_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_params_xdd_diff_present) {
    j.write_fieldname("phy-ParametersXDD-Diff");
    phy_params_xdd_diff.to_json(j);
  }
  if (mac_params_xdd_diff_present) {
    j.write_fieldname("mac-ParametersXDD-Diff");
    mac_params_xdd_diff.to_json(j);
  }
  if (meas_and_mob_params_xdd_diff_present) {
    j.write_fieldname("measAndMobParametersXDD-Diff");
    meas_and_mob_params_xdd_diff.to_json(j);
  }
  j.end_obj();
}

// UE-NR-Capability ::= SEQUENCE
SRSASN_CODE ue_nr_cap_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rlc_params_present, 1));
  HANDLE_CODE(bref.pack(mac_params_present, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.pack(fr1_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.pack(fr2_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.pack(feature_sets_present, 1));
  HANDLE_CODE(bref.pack(feature_set_combinations.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(access_stratum_release.pack(bref));
  HANDLE_CODE(pdcp_params.pack(bref));
  if (rlc_params_present) {
    HANDLE_CODE(rlc_params.pack(bref));
  }
  if (mac_params_present) {
    HANDLE_CODE(mac_params.pack(bref));
  }
  HANDLE_CODE(phy_params.pack(bref));
  HANDLE_CODE(rf_params.pack(bref));
  if (meas_and_mob_params_present) {
    HANDLE_CODE(meas_and_mob_params.pack(bref));
  }
  if (fdd_add_ue_nr_cap_present) {
    HANDLE_CODE(fdd_add_ue_nr_cap.pack(bref));
  }
  if (tdd_add_ue_nr_cap_present) {
    HANDLE_CODE(tdd_add_ue_nr_cap.pack(bref));
  }
  if (fr1_add_ue_nr_cap_present) {
    HANDLE_CODE(fr1_add_ue_nr_cap.pack(bref));
  }
  if (fr2_add_ue_nr_cap_present) {
    HANDLE_CODE(fr2_add_ue_nr_cap.pack(bref));
  }
  if (feature_sets_present) {
    HANDLE_CODE(feature_sets.pack(bref));
  }
  if (feature_set_combinations.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                feature_set_combinations,
                                1,
                                1024,
                                SeqOfPacker<SeqOfPacker<Packer>>(1, 32, SeqOfPacker<Packer>(1, 128, Packer()))));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rlc_params_present, 1));
  HANDLE_CODE(bref.unpack(mac_params_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr1_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr2_add_ue_nr_cap_present, 1));
  HANDLE_CODE(bref.unpack(feature_sets_present, 1));
  bool feature_set_combinations_present;
  HANDLE_CODE(bref.unpack(feature_set_combinations_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(access_stratum_release.unpack(bref));
  HANDLE_CODE(pdcp_params.unpack(bref));
  if (rlc_params_present) {
    HANDLE_CODE(rlc_params.unpack(bref));
  }
  if (mac_params_present) {
    HANDLE_CODE(mac_params.unpack(bref));
  }
  HANDLE_CODE(phy_params.unpack(bref));
  HANDLE_CODE(rf_params.unpack(bref));
  if (meas_and_mob_params_present) {
    HANDLE_CODE(meas_and_mob_params.unpack(bref));
  }
  if (fdd_add_ue_nr_cap_present) {
    HANDLE_CODE(fdd_add_ue_nr_cap.unpack(bref));
  }
  if (tdd_add_ue_nr_cap_present) {
    HANDLE_CODE(tdd_add_ue_nr_cap.unpack(bref));
  }
  if (fr1_add_ue_nr_cap_present) {
    HANDLE_CODE(fr1_add_ue_nr_cap.unpack(bref));
  }
  if (fr2_add_ue_nr_cap_present) {
    HANDLE_CODE(fr2_add_ue_nr_cap.unpack(bref));
  }
  if (feature_sets_present) {
    HANDLE_CODE(feature_sets.unpack(bref));
  }
  if (feature_set_combinations_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_set_combinations,
                                  bref,
                                  1,
                                  1024,
                                  SeqOfPacker<SeqOfPacker<Packer>>(1, 32, SeqOfPacker<Packer>(1, 128, Packer()))));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("accessStratumRelease", access_stratum_release.to_string());
  j.write_fieldname("pdcp-Parameters");
  pdcp_params.to_json(j);
  if (rlc_params_present) {
    j.write_fieldname("rlc-Parameters");
    rlc_params.to_json(j);
  }
  if (mac_params_present) {
    j.write_fieldname("mac-Parameters");
    mac_params.to_json(j);
  }
  j.write_fieldname("phy-Parameters");
  phy_params.to_json(j);
  j.write_fieldname("rf-Parameters");
  rf_params.to_json(j);
  if (meas_and_mob_params_present) {
    j.write_fieldname("measAndMobParameters");
    meas_and_mob_params.to_json(j);
  }
  if (fdd_add_ue_nr_cap_present) {
    j.write_fieldname("fdd-Add-UE-NR-Capabilities");
    fdd_add_ue_nr_cap.to_json(j);
  }
  if (tdd_add_ue_nr_cap_present) {
    j.write_fieldname("tdd-Add-UE-NR-Capabilities");
    tdd_add_ue_nr_cap.to_json(j);
  }
  if (fr1_add_ue_nr_cap_present) {
    j.write_fieldname("fr1-Add-UE-NR-Capabilities");
    fr1_add_ue_nr_cap.to_json(j);
  }
  if (fr2_add_ue_nr_cap_present) {
    j.write_fieldname("fr2-Add-UE-NR-Capabilities");
    fr2_add_ue_nr_cap.to_json(j);
  }
  if (feature_sets_present) {
    j.write_fieldname("featureSets");
    feature_sets.to_json(j);
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
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

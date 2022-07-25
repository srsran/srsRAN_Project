/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/pdsch.h"

using namespace srsgnb;
using namespace fapi_adaptor;
using namespace fapi;

/// Fills the RE patterns parameter of the PDSCH PDU.
static void fill_reserved_re_pattern(pdsch_processor::pdu_t& proc_pdu, const dl_pdsch_pdu& fapi_pdu)
{
  // :TODO: Implement me!
}

/// Fills the codewords.
static void fill_codewords(pdsch_processor::pdu_t& proc_pdu, const dl_pdsch_pdu& fapi_pdu)
{
  for (const auto& cw : fapi_pdu.cws) {
    proc_pdu.codewords.push_back(
        pdsch_processor::codeword_description{static_cast<modulation_scheme>(cw.qam_mod_order), cw.rv_index});
  }
}

static float get_power_control_offset_ss_dB(nzp_csi_rs_epre_to_ssb power_control_offset_ss_profile_nr)
{
  switch (power_control_offset_ss_profile_nr) {
    case nzp_csi_rs_epre_to_ssb::dB_minus_3:
      return -3.0F;
    case nzp_csi_rs_epre_to_ssb::dB0:
      return +0.0F;
    case nzp_csi_rs_epre_to_ssb::dB3:
      return +3.0F;
    case nzp_csi_rs_epre_to_ssb::dB6:
    default:
      break;
  }
  return +6.0F;
}

/// Fills the power related parameters in the PDSCH PDU.
static void fill_power_values(pdsch_processor::pdu_t& proc_pdu, const dl_pdsch_pdu& fapi_pdu)
{
  // Determine if ProfileNR is enabled.
  bool use_profileNR = (fapi_pdu.power_control_offset_ss_profile_nr != nzp_csi_rs_epre_to_ssb::L1_use_profile_sss);

  // Depending on the profile to use.
  if (use_profileNR) {
    // Load Data to SSS ratio from NR profile.
    srsran_assert(fapi_pdu.power_control_offset_profile_nr != std::numeric_limits<uint8_t>::max(),
                  "Expected SSS profile.");

    // Calculate the power offset between NZP-CSI-RS to PDSCH data.
    float power_control_offset_dB = static_cast<float>(fapi_pdu.power_control_offset_profile_nr - 8);

    // Calculate the power offset between NZP-CSI-RS to SSS.
    float power_control_offset_ss_dB = get_power_control_offset_ss_dB(fapi_pdu.power_control_offset_ss_profile_nr);

    proc_pdu.ratio_pdsch_data_to_sss_dB = power_control_offset_dB + power_control_offset_ss_dB;
  } else {
    // Load Data to SSS ratio from SSS profile.
    srsran_assert(fapi_pdu.power_control_offset_profile_nr == std::numeric_limits<uint8_t>::max(),
                  "Expected SSS profile.");
    srsran_assert(fapi_pdu.power_control_offset_ss_profile_nr == nzp_csi_rs_epre_to_ssb::L1_use_profile_sss,
                  "Expected SSS profile.");
    srsran_assert(fapi_pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss !=
                      std::numeric_limits<int16_t>::min(),
                  "Expected SSS profile.");
    proc_pdu.ratio_pdsch_data_to_sss_dB =
        static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss) * 0.001F;
  }

  // Use direct value if SSS profile is used.
  if (fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss != std::numeric_limits<int16_t>::min()) {
    proc_pdu.ratio_pdsch_dmrs_to_sss_dB =
        static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss) * 0.001F;
  } else {
    // Otherwise, determines the PDSCH DMRS power from the PDSCH data power as per TS38.214 Table 4.1-1.
    srsran_assert(fapi_pdu.num_dmrs_cdm_grps_no_data > 0 && fapi_pdu.num_dmrs_cdm_grps_no_data < 4,
                  "Invalid number of DMRS CDM groups without data.");
    static constexpr std::array<float, 4> beta_dmrs_values = {NAN, 0, -3, -4.77};

    proc_pdu.ratio_pdsch_dmrs_to_sss_dB =
        proc_pdu.ratio_pdsch_data_to_sss_dB + beta_dmrs_values[fapi_pdu.num_dmrs_cdm_grps_no_data];
  }
}

static unsigned get_interleaver_size(vrb_to_prb_mapping_type vrb_to_prb_mapping)
{
  switch (vrb_to_prb_mapping) {
    case vrb_to_prb_mapping_type::interleaved_rb_size2:
      return 2;
    case vrb_to_prb_mapping_type::interleaved_rb_size4:
      return 4;
    case vrb_to_prb_mapping_type::non_interleaved:
      break;
  }
  return 0;
}

/// Constructs the VRB-to-PRB mapper in function of the transmission type parameter of the PDSCH PDU.
static vrb_to_prb_mapper make_vrb_to_prb_mapper(const dl_pdsch_pdu& fapi_pdu)
{
  // BWP i start.
  unsigned N_bwp_i_start = fapi_pdu.bwp_start;
  // BWP i size.
  unsigned N_bwp_i_size = fapi_pdu.bwp_size;
  // CORESET first VRB index.
  unsigned N_start_coreset = fapi_pdu.pdsch_maintenance_v3.coreset_start_point - N_bwp_i_start;
  // Initial BWP size.
  unsigned N_bwp_init_size = fapi_pdu.pdsch_maintenance_v3.initial_dl_bwp_size;
  // Bundle i size.
  unsigned L_i = get_interleaver_size(fapi_pdu.vrb_to_prb_mapping);

  switch (fapi_pdu.pdsch_maintenance_v3.trans_type) {
    case pdsch_trans_type::non_interleaved_common_ss:
      return vrb_to_prb_mapper::create_non_interleaved_common_ss(N_start_coreset);
    case pdsch_trans_type::interleaved_common_type0_coreset0:
      return vrb_to_prb_mapper::create_interleaved_coreset0(N_start_coreset, N_bwp_init_size);
    case pdsch_trans_type::interleaved_common_any_coreset0_present:
      return vrb_to_prb_mapper::create_interleaved_common(N_start_coreset, N_bwp_i_start, N_bwp_init_size);
    case pdsch_trans_type::interleaved_common_any_coreset0_not_present:
      return vrb_to_prb_mapper::create_interleaved_common(N_start_coreset, N_bwp_i_start, N_bwp_i_size);
    case pdsch_trans_type::interleaved_other:
      return vrb_to_prb_mapper::create_interleaved_other(N_bwp_i_start, N_bwp_i_size, L_i);
    default:
    case pdsch_trans_type::non_interleaved_other:
      break;
  }
  return vrb_to_prb_mapper::create_non_interleaved_other();
}

/// Fills the rb_allocation parameter of the PDSCH PDU.
static void fill_rb_allocation(pdsch_processor::pdu_t& proc_pdu, const dl_pdsch_pdu& fapi_pdu)
{
  vrb_to_prb_mapper mapper = make_vrb_to_prb_mapper(fapi_pdu);

  if (fapi_pdu.resource_alloc == resource_allocation_type::type_1) {
    proc_pdu.freq_alloc = rb_allocation::make_type1(fapi_pdu.rb_start, fapi_pdu.rb_size, mapper);
    return;
  }

  // Unpack the VRB bitmap. LSB of byte 0 of the bitmap represents the VRB 0.
  bounded_bitset<MAX_RB> vrb_bitmap(fapi_pdu.bwp_size);
  for (unsigned vrb_index = 0, vrb_index_end = fapi_pdu.bwp_size; vrb_index != vrb_index_end; ++vrb_index) {
    unsigned byte = vrb_index / 8;
    unsigned bit  = vrb_index % 8;
    if ((fapi_pdu.rb_bitmap[byte] >> bit) & 1U) {
      vrb_bitmap.set(vrb_index);
    }
  }

  proc_pdu.freq_alloc = rb_allocation::make_type0(vrb_bitmap, mapper);
}

void srsgnb::fapi_adaptor::convert_pdsch_fapi_to_phy(pdsch_processor::pdu_t& proc_pdu,
                                                     const dl_pdsch_pdu&     fapi_pdu,
                                                     uint16_t                sfn,
                                                     uint16_t                slot)
{
  proc_pdu.slot         = slot_point(static_cast<uint32_t>(fapi_pdu.scs), sfn, slot);
  proc_pdu.rnti         = fapi_pdu.rnti;
  proc_pdu.bwp_size_rb  = fapi_pdu.bwp_size;
  proc_pdu.bwp_start_rb = fapi_pdu.bwp_start;
  proc_pdu.cp =
      cyclic_prefix((fapi_pdu.cyclic_prefix == cyclic_prefix_type::normal) ? cyclic_prefix::options::NORMAL
                                                                           : cyclic_prefix::options::EXTENDED);

  fill_codewords(proc_pdu, fapi_pdu);

  proc_pdu.n_id = fapi_pdu.nid_pdsch;

  // This value is defined in field DL DMRS symbol position in SCF-222 v4.0 Section 3.4.2.2, in table PDSCH PDU, DMRS
  // subsection.
  static constexpr unsigned DL_DMRS_SYMBOL_POS_SIZE = 14U;
  for (unsigned i = 0; i != DL_DMRS_SYMBOL_POS_SIZE; ++i) {
    proc_pdu.dmrs_symbol_mask[i] = ((static_cast<unsigned>(fapi_pdu.dl_dmrs_symb_pos >> i) & 1U) == 1U);
  }

  proc_pdu.ref_point = (fapi_pdu.ref_point == pdsch_ref_point_type::point_a) ? pdsch_processor::pdu_t::CRB0
                                                                             : pdsch_processor::pdu_t::PRB0;

  proc_pdu.dmrs =
      fapi_pdu.dmrs_type == dmrs_config_type::type_1 ? dmrs_type::options::TYPE1 : dmrs_type::options::TYPE2;
  proc_pdu.scrambling_id               = fapi_pdu.pdsch_dmrs_scrambling_id;
  proc_pdu.n_scid                      = fapi_pdu.nscid == 1U;
  proc_pdu.nof_cdm_groups_without_data = fapi_pdu.num_dmrs_cdm_grps_no_data;
  proc_pdu.start_symbol_index          = fapi_pdu.start_symbol_index;
  proc_pdu.nof_symbols                 = fapi_pdu.nr_of_symbols;

  fill_rb_allocation(proc_pdu, fapi_pdu);

  fill_power_values(proc_pdu, fapi_pdu);

  proc_pdu.ldpc_base_graph = fapi_pdu.pdsch_maintenance_v3.ldpc_base_graph;
  proc_pdu.tbs_lbrm_bytes  = fapi_pdu.pdsch_maintenance_v3.tb_size_lbrm_bytes;

  fill_reserved_re_pattern(proc_pdu, fapi_pdu);

  // :TODO: add the ports.
  proc_pdu.ports = {0};
}

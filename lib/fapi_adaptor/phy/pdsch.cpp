/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/pdsch.h"

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
    proc_pdu.codewords.push_back(pdsch_processor::codeword_description{
        float(cw.target_code_rate) * 0.1F,
        static_cast<modulation_scheme>(cw.qam_mod_order),
        cw.mcs_index,
        static_cast<pdsch_processor::codeword_description::mcs_table_type>(cw.mcs_table),
        cw.rv_index});
  }
}

/// Fills the power related parameters in the PDSCH PDU.
static void fill_power_values(pdsch_processor::pdu_t& proc_pdu, const dl_pdsch_pdu& fapi_pdu)
{
  proc_pdu.ratio_pdsch_to_ssb_dB =
      fapi_pdu.power_control_offset_profile_nr == std::numeric_limits<uint8_t>::max()
          ? static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss) * 0.001F
          : static_cast<float>(fapi_pdu.power_control_offset_profile_nr - 8);

  proc_pdu.ratio_pdsch_dmrs_to_sss_dB =
      fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss == std::numeric_limits<int16_t>::min()
          ? proc_pdu.ratio_pdsch_to_ssb_dB
          : static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss) * 0.001F;

  proc_pdu.ratio_pdsch_data_to_sss_dB =
      fapi_pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss == std::numeric_limits<int16_t>::min()
          ? proc_pdu.ratio_pdsch_dmrs_to_sss_dB
          : static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss) * 0.001F;

  switch (fapi_pdu.power_control_offset_ss_profile_nr) {
    case pdsch_ss_profile_nr_type::dB_minus_3:
      proc_pdu.ratio_nzp_csi_rs_to_ssb_dB = -3.F;
      break;
    case pdsch_ss_profile_nr_type::dB0:
      proc_pdu.ratio_nzp_csi_rs_to_ssb_dB = 0.F;
      break;
    case pdsch_ss_profile_nr_type::dB3:
      proc_pdu.ratio_nzp_csi_rs_to_ssb_dB = 3.F;
      break;
    case pdsch_ss_profile_nr_type::dB6:
      proc_pdu.ratio_nzp_csi_rs_to_ssb_dB = 6.F;
      break;
    case pdsch_ss_profile_nr_type::L1_use_profile_sss:
      proc_pdu.ratio_nzp_csi_rs_to_ssb_dB =
          static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss) * 0.001F;
      break;
  }
}

/// Fills the transmission type parameter of the PDSCH PDU.
static void fill_transmission_type(pdsch_processor::pdu_t& proc_pdu, const dl_pdsch_pdu& fapi_pdu)
{
  switch (fapi_pdu.pdsch_maintenance_v3.trans_type) {
    case pdsch_trans_type::non_interleaved_common_ss:
      proc_pdu.transmission_type = pdsch_processor::pdu_t::NON_INTERLEAVED_COMMON_SS;
      break;
    case pdsch_trans_type::non_interleaved_other:
      proc_pdu.transmission_type = pdsch_processor::pdu_t::NON_INTERLEAVED_OTHER;
      break;
    case pdsch_trans_type::interleaved_common_type0_coreset0:
      proc_pdu.transmission_type = pdsch_processor::pdu_t::INTERLEAVED_COMMON_TYPE0_CORESET0;
      break;
    case pdsch_trans_type::interleaved_common_any_coreset0_present:
      proc_pdu.transmission_type = pdsch_processor::pdu_t::INTERLEAVED_COMMON_ANY_CORESET0_PRESENT;
      break;
    case pdsch_trans_type::interleaved_common_any_coreset0_not_present:
      proc_pdu.transmission_type = pdsch_processor::pdu_t::INTERLEAVED_COMMON_ANY_CORESET0_NOT_PRESENT;
      break;
    case pdsch_trans_type::interleaved_other:
      proc_pdu.transmission_type = pdsch_processor::pdu_t::INTERLEAVED_OTHER;
      break;
    default:
      srsran_assert(0, "InvalidPDSCH transmission type");
      break;
  }
}

/// Fills the rb_allocation parameter of the PDSCH PDU.
static void fill_rb_allocation(pdsch_processor::pdu_t& proc_pdu, const dl_pdsch_pdu& fapi_pdu)
{
  if (fapi_pdu.resource_alloc == pdsch_resource_allocation_type::type_0) {
    proc_pdu.freq_alloc =
        rb_allocation(fapi_pdu.rb_bitmap, static_cast<vrb_to_prb_mapping_type>(fapi_pdu.vrb_to_prb_mapping));

    return;
  }

  proc_pdu.freq_alloc = rb_allocation(
      fapi_pdu.rb_start, fapi_pdu.rb_size, static_cast<vrb_to_prb_mapping_type>(fapi_pdu.vrb_to_prb_mapping));
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

  // This value is defined in field DL DMRS symbol position in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU, DMRS
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

  fill_transmission_type(proc_pdu, fapi_pdu);

  proc_pdu.coreset_start_rb    = fapi_pdu.pdsch_maintenance_v3.coreset_start_point;
  proc_pdu.initial_dl_bwp_size = fapi_pdu.pdsch_maintenance_v3.initial_dl_bwp_size;
  proc_pdu.ldpc_base_graph     = (fapi_pdu.pdsch_maintenance_v3.ldpc_base_graph == ldpc_base_graph_type::bg_1)
                                     ? ldpc::base_graph_t::BG1
                                     : ldpc::base_graph_t::BG2;
  proc_pdu.tbs_lbrm_bytes      = fapi_pdu.pdsch_maintenance_v3.tb_size_lbrm_bytes;

  fill_reserved_re_pattern(proc_pdu, fapi_pdu);

  // :TODO: add the ports.
  proc_pdu.ports = {0};
}

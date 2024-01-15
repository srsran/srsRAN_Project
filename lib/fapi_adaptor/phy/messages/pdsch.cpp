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

#include "srsran/fapi_adaptor/phy/messages/pdsch.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/ran/sch_dmrs_power.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Fills the reserved RE pattern list field in a PDSCH PDU.
static void fill_reserved_re_pattern(pdsch_processor::pdu_t&     proc_pdu,
                                     const fapi::dl_pdsch_pdu&   fapi_pdu,
                                     span<const re_pattern_list> csi_re_pattern_list)
{
  for (auto csi_index : fapi_pdu.pdsch_maintenance_v3.csi_for_rm) {
    srsran_assert(csi_index < csi_re_pattern_list.size(),
                  "CSI-RS PDU index={} value out of bounds CSI RE patterns={}",
                  csi_index,
                  csi_re_pattern_list.size());

    proc_pdu.reserved.merge(csi_re_pattern_list[csi_index]);
  }
}

/// Fills the codewords.
static void fill_codewords(pdsch_processor::pdu_t& proc_pdu, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  for (const auto& cw : fapi_pdu.cws) {
    pdsch_processor::codeword_description codeword_descr;
    codeword_descr.modulation = static_cast<modulation_scheme>(cw.qam_mod_order);
    codeword_descr.rv         = cw.rv_index;
    if (fapi_pdu.context.has_value()) {
      codeword_descr.new_data = fapi_pdu.context->is_new_data();
    } else {
      codeword_descr.new_data = true;
    }
    proc_pdu.codewords.push_back(codeword_descr);
  }
}

static float get_power_control_offset_ss_dB(fapi::nzp_csi_rs_epre_to_ssb power_control_offset_ss_profile_nr)
{
  switch (power_control_offset_ss_profile_nr) {
    case fapi::nzp_csi_rs_epre_to_ssb::dB_minus_3:
      return -3.0F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB0:
      return +0.0F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB3:
      return +3.0F;
    case fapi::nzp_csi_rs_epre_to_ssb::dB6:
    default:
      break;
  }
  return +6.0F;
}

/// Fills the power related parameters in the PDSCH PDU.
static void fill_power_values(pdsch_processor::pdu_t& proc_pdu, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  // Determine if ProfileNR is enabled.
  bool use_profileNR =
      (fapi_pdu.power_control_offset_ss_profile_nr != fapi::nzp_csi_rs_epre_to_ssb::L1_use_profile_sss);

  // Depending on the profile to use.
  if (use_profileNR) {
    // Load Data to SSS ratio from NR profile.
    srsran_assert(fapi_pdu.power_control_offset_profile_nr !=
                      std::numeric_limits<decltype(fapi_pdu.power_control_offset_profile_nr)>::max(),
                  "Expected SSS profile.");

    // Calculate the power offset between NZP-CSI-RS to PDSCH data.
    float power_control_offset_dB = static_cast<float>(fapi_pdu.power_control_offset_profile_nr - 8);

    // Calculate the power offset between NZP-CSI-RS to SSS.
    float power_control_offset_ss_dB = get_power_control_offset_ss_dB(fapi_pdu.power_control_offset_ss_profile_nr);

    proc_pdu.ratio_pdsch_data_to_sss_dB = power_control_offset_dB + power_control_offset_ss_dB;
  } else {
    // Load Data to SSS ratio from SSS profile.
    srsran_assert(fapi_pdu.power_control_offset_profile_nr ==
                      std::numeric_limits<decltype(fapi_pdu.power_control_offset_profile_nr)>::max(),
                  "Expected SSS profile.");
    srsran_assert(fapi_pdu.power_control_offset_ss_profile_nr == fapi::nzp_csi_rs_epre_to_ssb::L1_use_profile_sss,
                  "Expected SSS profile.");
    srsran_assert(
        fapi_pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss !=
            std::numeric_limits<decltype(fapi_pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss)>::min(),
        "Expected SSS profile.");
    proc_pdu.ratio_pdsch_data_to_sss_dB =
        static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss) * 0.001F;
  }

  // Use direct value if SSS profile is used.
  if (fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss !=
      std::numeric_limits<decltype(fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss)>::min()) {
    proc_pdu.ratio_pdsch_dmrs_to_sss_dB =
        static_cast<float>(fapi_pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss) * 0.001F;
  } else {
    // Otherwise, determines the PDSCH DMRS power from the PDSCH data power as per TS38.214 Table 4.1-1.
    proc_pdu.ratio_pdsch_dmrs_to_sss_dB =
        proc_pdu.ratio_pdsch_data_to_sss_dB + get_sch_to_dmrs_ratio_dB(fapi_pdu.num_dmrs_cdm_grps_no_data);
  }
}

static unsigned get_interleaver_size(fapi::vrb_to_prb_mapping_type vrb_to_prb_mapping)
{
  switch (vrb_to_prb_mapping) {
    case fapi::vrb_to_prb_mapping_type::interleaved_rb_size2:
      return 2;
    case fapi::vrb_to_prb_mapping_type::interleaved_rb_size4:
      return 4;
    case fapi::vrb_to_prb_mapping_type::non_interleaved:
      break;
  }
  return 0;
}

/// Constructs the VRB-to-PRB mapper in function of the transmission type parameter of the PDSCH PDU.
static vrb_to_prb_mapper make_vrb_to_prb_mapper(const fapi::dl_pdsch_pdu& fapi_pdu)
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
    case fapi::pdsch_trans_type::non_interleaved_common_ss:
      return vrb_to_prb_mapper::create_non_interleaved_common_ss(N_start_coreset);
    case fapi::pdsch_trans_type::interleaved_common_type0_coreset0:
      return vrb_to_prb_mapper::create_interleaved_coreset0(N_start_coreset, N_bwp_init_size);
    case fapi::pdsch_trans_type::interleaved_common_any_coreset0_present:
      return vrb_to_prb_mapper::create_interleaved_common(N_start_coreset, N_bwp_i_start, N_bwp_init_size);
    case fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present:
      return vrb_to_prb_mapper::create_interleaved_common(N_start_coreset, N_bwp_i_start, N_bwp_i_size);
    case fapi::pdsch_trans_type::interleaved_other:
      return vrb_to_prb_mapper::create_interleaved_other(N_bwp_i_start, N_bwp_i_size, L_i);
    default:
    case fapi::pdsch_trans_type::non_interleaved_other:
      break;
  }
  return vrb_to_prb_mapper::create_non_interleaved_other();
}

/// Fills the rb_allocation parameter of the PDSCH PDU.
static void fill_rb_allocation(pdsch_processor::pdu_t& proc_pdu, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  vrb_to_prb_mapper mapper = make_vrb_to_prb_mapper(fapi_pdu);

  if (fapi_pdu.resource_alloc == fapi::resource_allocation_type::type_1) {
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

void srsran::fapi_adaptor::convert_pdsch_fapi_to_phy(pdsch_processor::pdu_t&            proc_pdu,
                                                     const fapi::dl_pdsch_pdu&          fapi_pdu,
                                                     uint16_t                           sfn,
                                                     uint16_t                           slot,
                                                     span<const re_pattern_list>        csi_re_pattern_list,
                                                     const precoding_matrix_repository& pm_repo)
{
  proc_pdu.slot         = slot_point(fapi_pdu.scs, sfn, slot);
  proc_pdu.rnti         = to_value(fapi_pdu.rnti);
  proc_pdu.bwp_size_rb  = fapi_pdu.bwp_size;
  proc_pdu.bwp_start_rb = fapi_pdu.bwp_start;
  proc_pdu.cp           = fapi_pdu.cp;

  fill_codewords(proc_pdu, fapi_pdu);

  proc_pdu.n_id = fapi_pdu.nid_pdsch;

  // This value is defined in field DL DM-RS symbol position in SCF-222 v4.0 Section 3.4.2.2, in table PDSCH PDU, DM-RS
  // subsection.
  static constexpr unsigned DL_DMRS_SYMBOL_POS_SIZE = 14U;
  proc_pdu.dmrs_symbol_mask.resize(DL_DMRS_SYMBOL_POS_SIZE);
  proc_pdu.dmrs_symbol_mask.reset();
  for (unsigned i = 0; i != DL_DMRS_SYMBOL_POS_SIZE; ++i) {
    proc_pdu.dmrs_symbol_mask.set(i, (static_cast<unsigned>(fapi_pdu.dl_dmrs_symb_pos >> i) & 1U) == 1U);
  }

  proc_pdu.ref_point = (fapi_pdu.ref_point == fapi::pdsch_ref_point_type::point_a) ? pdsch_processor::pdu_t::CRB0
                                                                                   : pdsch_processor::pdu_t::PRB0;

  proc_pdu.dmrs =
      fapi_pdu.dmrs_type == fapi::dmrs_cfg_type::type_1 ? dmrs_type::options::TYPE1 : dmrs_type::options::TYPE2;
  proc_pdu.scrambling_id               = fapi_pdu.pdsch_dmrs_scrambling_id;
  proc_pdu.n_scid                      = fapi_pdu.nscid == 1U;
  proc_pdu.nof_cdm_groups_without_data = fapi_pdu.num_dmrs_cdm_grps_no_data;
  proc_pdu.start_symbol_index          = fapi_pdu.start_symbol_index;
  proc_pdu.nof_symbols                 = fapi_pdu.nr_of_symbols;

  fill_rb_allocation(proc_pdu, fapi_pdu);

  fill_power_values(proc_pdu, fapi_pdu);

  proc_pdu.ldpc_base_graph = fapi_pdu.pdsch_maintenance_v3.ldpc_base_graph;
  proc_pdu.tbs_lbrm_bytes  = fapi_pdu.pdsch_maintenance_v3.tb_size_lbrm_bytes.value();

  fill_reserved_re_pattern(proc_pdu, fapi_pdu, csi_re_pattern_list);

  srsran_assert(fapi_pdu.precoding_and_beamforming.prgs.size() == 1U,
                "Unsupported number of PRGs={}",
                fapi_pdu.precoding_and_beamforming.prgs.size());
  proc_pdu.precoding = precoding_configuration::make_wideband(
      pm_repo.get_precoding_matrix(fapi_pdu.precoding_and_beamforming.prgs.front().pm_index));

  // Fill PDSCH context for logging.
  proc_pdu.context = fapi_pdu.context;
}

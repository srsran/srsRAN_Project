/*
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_dci_builder.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/csi_rs/csi_meas_config.h"
#include "srsran/ran/pdcch/dci_packing.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/physical_cell_group.h"
#include "srsran/ran/pucch/srs_configuration.h"
#include "srsran/ran/pusch/pusch_configuration.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include <algorithm>

using namespace srsran;

/// Get PDSCH-to-HARQ-timing-indicator field as per TS38.213, 9.2.3.
static unsigned get_dci_1_0_pdsch_to_harq_timing_indicator(unsigned k1)
{
  // PDSCH-to-HARQ-timing-indicator maps to {1,2,3,4,5,6,7,8} for DCI 1_0.
  srsran_assert(k1 <= 8, "Invalid k1 value");
  return k1 - 1;
}

/// Get PDSCH-to-HARQ-timing-indicator field as per TS38.213, 9.2.3.
static unsigned get_dci_1_1_pdsch_to_harq_timing_indicator(unsigned k1, span<const uint8_t> dl_data_to_ul_ack)
{
  // PDSCH-to-HARQ-timing-indicator maps to dl-DataToUL-ACK for DCI 1_1.
  const auto* it = std::find_if(
      dl_data_to_ul_ack.begin(), dl_data_to_ul_ack.end(), [k1](const auto& k1_cfg) { return k1 == k1_cfg; });
  srsran_assert(it != dl_data_to_ul_ack.end(), "Invalid k1 value");
  return it - dl_data_to_ul_ack.begin();
}

/// \brief Determines whether the given DCI format is monitored in UE specific SS or not.
/// \param active_dl_bwp_cmn Active DL BWP common configuration.
/// \param active_dl_bwp_ded Active DL BWP dedicated configuration.
/// \param check_for_fallback_dci_formats Boolean denoting a fallback or non-fallback DCI format.
/// Returns whether the given DCI format are monitored in UE specific SS or not.
static bool is_dci_format_monitored_in_ue_ss(const bwp_downlink_common&    active_dl_bwp_cmn,
                                             const bwp_downlink_dedicated* active_dl_bwp_ded,
                                             bool                          check_for_fallback_dci_formats)
{
  const auto dci_format   = check_for_fallback_dci_formats
                                ? search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0
                                : search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;
  auto       it           = std::find_if(active_dl_bwp_cmn.pdcch_common.search_spaces.begin(),
                         active_dl_bwp_cmn.pdcch_common.search_spaces.end(),
                         [dci_format](const search_space_configuration& ss) {
                           return ss.type == search_space_configuration::type_t::ue_dedicated and
                                  ss.ue_specific == dci_format;
                         });
  bool       is_monitired = it != active_dl_bwp_cmn.pdcch_common.search_spaces.end();
  if (active_dl_bwp_ded != nullptr and active_dl_bwp_ded->pdcch_cfg.has_value()) {
    it           = std::find_if(active_dl_bwp_ded->pdcch_cfg.value().search_spaces.begin(),
                      active_dl_bwp_ded->pdcch_cfg.value().search_spaces.end(),
                      [dci_format](const search_space_configuration& ss) {
                        return ss.type == search_space_configuration::type_t::ue_dedicated and
                               ss.ue_specific == dci_format;
                      });
    is_monitired = it != active_dl_bwp_ded->pdcch_cfg.value().search_spaces.end();
  }
  return is_monitired;
}

void srsran::build_dci_f1_0_tc_rnti(dci_dl_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    prb_interval               prbs,
                                    unsigned                   time_resource,
                                    unsigned                   k1,
                                    unsigned                   pucch_res_indicator,
                                    sch_mcs_index              mcs_index,
                                    const dl_harq_process&     h_dl)
{
  static constexpr unsigned tb_idx = 0;

  dci.type                            = srsran::dci_dl_rnti_config_type::tc_rnti_f1_0;
  dci.tc_rnti_f1_0                    = {};
  dci_1_0_tc_rnti_configuration& f1_0 = dci.tc_rnti_f1_0;

  f1_0.tpc_command        = 1;
  f1_0.vrb_to_prb_mapping = 0;

  // PDSCH resources.
  // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB is the size of CORESET 0 for TC-RNTI.
  f1_0.N_rb_dl_bwp = init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length();
  f1_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{f1_0.N_rb_dl_bwp, prbs.start(), prbs.length()});
  f1_0.time_resource = time_resource;

  // UCI resources.
  f1_0.pucch_resource_indicator       = pucch_res_indicator;
  f1_0.pdsch_harq_fb_timing_indicator = get_dci_1_0_pdsch_to_harq_timing_indicator(k1);

  f1_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f1_0.harq_process_number = h_dl.id;
  f1_0.new_data_indicator  = h_dl.tb(tb_idx).ndi;
  f1_0.redundancy_version  = get_redundancy_version(h_dl.tb(tb_idx).nof_retxs);
}

void srsran::build_dci_f1_0_c_rnti(dci_dl_info&                       dci,
                                   const bwp_downlink_common&         init_dl_bwp,
                                   const bwp_downlink_common&         active_dl_bwp,
                                   const bwp_downlink_dedicated*      active_dl_bwp_ded,
                                   search_space_configuration::type_t ss_type,
                                   prb_interval                       prbs,
                                   unsigned                           time_resource,
                                   unsigned                           k1,
                                   unsigned                           pucch_res_indicator,
                                   unsigned                           dai,
                                   sch_mcs_index                      mcs_index,
                                   const dl_harq_process&             h_dl)
{
  dci.type                           = srsran::dci_dl_rnti_config_type::c_rnti_f1_0;
  dci.c_rnti_f1_0                    = {};
  dci_1_0_c_rnti_configuration& f1_0 = dci.c_rnti_f1_0;

  f1_0.tpc_command        = 1;
  f1_0.vrb_to_prb_mapping = 0;

  // PDSCH resources.
  // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB for C-RNTI.
  unsigned N_rb_dl_bwp = 0;
  if (ss_type == srsran::search_space_configuration::type_t::common) {
    N_rb_dl_bwp = init_dl_bwp.pdcch_common.coreset0.has_value()
                      ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()
                      : init_dl_bwp.generic_params.crbs.length();
  } else {
    N_rb_dl_bwp = active_dl_bwp.generic_params.crbs.length();
  }
  f1_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{N_rb_dl_bwp, prbs.start(), prbs.length()});
  f1_0.time_resource = time_resource;

  // PUSCH params.
  const auto dci_sz_cfg = dci_size_config{
      is_dci_format_monitored_in_ue_ss(active_dl_bwp, active_dl_bwp_ded, true),
      is_dci_format_monitored_in_ue_ss(active_dl_bwp, active_dl_bwp_ded, false),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.generic_params.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.generic_params.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0,
      false};
  srsran_assert(validate_dci_size_config(dci_sz_cfg), "Invalid DCI size configuration for DCI Format 1_0 (C-RNTI)");
  dci_sizes dci_sz  = get_dci_sizes(dci_sz_cfg);
  f1_0.payload_size = ss_type == search_space_configuration::type_t::ue_dedicated ? dci_sz.format1_0_ue_size.value()
                                                                                  : dci_sz.format1_0_common_size;

  // UCI resources.
  f1_0.pucch_resource_indicator       = pucch_res_indicator;
  f1_0.pdsch_harq_fb_timing_indicator = get_dci_1_0_pdsch_to_harq_timing_indicator(k1);
  f1_0.dl_assignment_index            = dai;

  f1_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f1_0.harq_process_number = h_dl.id;
  f1_0.new_data_indicator  = h_dl.tb(0).ndi;
  f1_0.redundancy_version  = get_redundancy_version(h_dl.tb(0).nof_retxs);
}

void srsran::build_dci_f1_1_c_rnti(dci_dl_info&                 dci,
                                   const ue_cell_configuration& ue_cell_cfg,
                                   bool                         is_ue_configured_multiple_serving_cells,
                                   bwp_id_t                     active_bwp_id,
                                   search_space_id              ss_id,
                                   prb_interval                 prbs,
                                   unsigned                     time_resource,
                                   unsigned                     k1,
                                   unsigned                     pucch_res_indicator,
                                   unsigned                     dai,
                                   sch_mcs_index                tb1_mcs_index,
                                   const dl_harq_process&       h_dl)
{
  const search_space_configuration* ss_cfg = ue_cell_cfg.find_search_space(ss_id);
  srsran_assert(ss_cfg != nullptr, "No valid SearchSpace found");
  srsran_assert(ss_cfg->type == search_space_configuration::type_t::ue_dedicated,
                "SearchSpace must be of type UE-Specific SearchSpace");

  const bwp_downlink_common&    init_dl_bwp       = ue_cell_cfg.dl_bwp_common(to_bwp_id(0));
  const bwp_downlink_common&    active_dl_bwp_cmn = ue_cell_cfg.dl_bwp_common(active_bwp_id);
  const bwp_downlink_dedicated* active_dl_bwp_ded = ue_cell_cfg.find_dl_bwp_ded(active_bwp_id);
  const bwp_configuration&      active_dl_bwp     = active_dl_bwp_cmn.generic_params;
  const bwp_uplink_common&      init_ul_bwp       = ue_cell_cfg.ul_bwp_common(to_bwp_id(0));
  const bwp_configuration&      active_ul_bwp     = ue_cell_cfg.ul_bwp_common(active_bwp_id).generic_params;
  const auto                    k1_candidates     = ue_cell_cfg.get_k1_candidates(dci_dl_rnti_config_type::c_rnti_f1_1);
  const auto&                   opt_pdsch_cfg     = ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg;
  const auto&                   opt_pdsch_serving_cell_cfg = ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg;

  dci.type                    = srsran::dci_dl_rnti_config_type::c_rnti_f1_1;
  dci.c_rnti_f1_1             = {};
  dci_1_1_configuration& f1_1 = dci.c_rnti_f1_1;

  f1_1.tpc_command             = 1;
  f1_1.srs_request             = 0;
  f1_1.dmrs_seq_initialization = 0;
  // TODO: Set proper value based on nof. layers used. See TS 38.212, clause 7.3.1.2.2.
  f1_1.antenna_ports = 0;

  // See 38.212, clause 7.3.1.2.2 - N^{DL,BWP}_RB for C-RNTI.
  f1_1.frequency_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{active_dl_bwp.crbs.length(), prbs.start(), prbs.length()});
  f1_1.time_resource = time_resource;

  dci_size_config dci_sz_cfg = dci_size_config{
      is_dci_format_monitored_in_ue_ss(active_dl_bwp_cmn, active_dl_bwp_ded, true),
      is_dci_format_monitored_in_ue_ss(active_dl_bwp_cmn, active_dl_bwp_ded, false),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_ul_bwp.generic_params.crbs.length(),
      active_ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0};
  dci_sz_cfg.cross_carrier_configured = false;
  dci_sz_cfg.sul_configured           = false;
  // TODO: Need to fetch from physical_cell_group_config.
  dci_sz_cfg.pdsch_harq_ack_cb           = pdsch_harq_ack_codebook::dynamic;
  dci_sz_cfg.nof_dl_bwp_rrc              = ue_cell_cfg.cfg_dedicated().dl_bwps.size();
  dci_sz_cfg.nof_dl_time_domain_res      = ue_cell_cfg.get_pdsch_time_domain_list(ss_id).size();
  dci_sz_cfg.nof_aperiodic_zp_csi        = 0;
  dci_sz_cfg.nof_pdsch_ack_timings       = k1_candidates.size();
  dci_sz_cfg.dynamic_prb_bundling        = false;
  dci_sz_cfg.rm_pattern_group1           = false;
  dci_sz_cfg.rm_pattern_group2           = false;
  dci_sz_cfg.pdsch_two_codewords         = false;
  dci_sz_cfg.pdsch_res_allocation_type   = resource_allocation::resource_allocation_type_1;
  dci_sz_cfg.interleaved_vrb_prb_mapping = false;
  if (opt_pdsch_cfg.has_value()) {
    dci_sz_cfg.dynamic_prb_bundling =
        variant_holds_alternative<prb_bundling::dynamic_bundling>(opt_pdsch_cfg.value().prb_bndlg.bundling);
    dci_sz_cfg.pdsch_two_codewords = opt_pdsch_cfg.value().is_max_cw_sched_by_dci_is_two;
    switch (opt_pdsch_cfg.value().res_alloc) {
      case pdsch_config::resource_allocation::resource_allocation_type_0: {
        dci_sz_cfg.pdsch_res_allocation_type = resource_allocation::resource_allocation_type_0;
        dci_sz_cfg.nof_dl_rb_groups          = static_cast<unsigned>(
            get_nominal_rbg_size(active_dl_bwp.crbs.length(), opt_pdsch_cfg.value().rbg_sz == rbg_size::config1));
        dci_sz_cfg.interleaved_vrb_prb_mapping = opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value();
        break;
      }
      case pdsch_config::resource_allocation::resource_allocation_type_1: {
        dci_sz_cfg.pdsch_res_allocation_type = resource_allocation::resource_allocation_type_1;
        break;
      }
      case pdsch_config::resource_allocation::dynamic_switch: {
        dci_sz_cfg.pdsch_res_allocation_type = resource_allocation::dynamic_switch;
        dci_sz_cfg.nof_dl_rb_groups          = static_cast<unsigned>(
            get_nominal_rbg_size(active_dl_bwp.crbs.length(), opt_pdsch_cfg.value().rbg_sz == rbg_size::config1));
        dci_sz_cfg.interleaved_vrb_prb_mapping = opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value();
        break;
      }
    }
    if (opt_pdsch_cfg.value().pdsch_mapping_type_a_dmrs.has_value()) {
      dci_sz_cfg.pdsch_dmrs_A_type    = opt_pdsch_cfg.value().pdsch_mapping_type_a_dmrs.value().type;
      dci_sz_cfg.pdsch_dmrs_A_max_len = opt_pdsch_cfg.value().pdsch_mapping_type_a_dmrs.value().max_length;
    }
    if (opt_pdsch_cfg.value().pdsch_mapping_type_b_dmrs.has_value()) {
      dci_sz_cfg.pdsch_dmrs_B_type    = opt_pdsch_cfg.value().pdsch_mapping_type_b_dmrs.value().type;
      dci_sz_cfg.pdsch_dmrs_B_max_len = opt_pdsch_cfg.value().pdsch_mapping_type_b_dmrs.value().max_length;
    }
  }
  dci_sz_cfg.multiple_scells     = is_ue_configured_multiple_serving_cells;
  dci_sz_cfg.pdsch_tci           = false;
  dci_sz_cfg.cbg_flush_indicator = false;
  if (opt_pdsch_serving_cell_cfg.has_value()) {
    dci_sz_cfg.cbg_flush_indicator = false;
    if (opt_pdsch_serving_cell_cfg.value().code_block_group_tx.has_value()) {
      dci_sz_cfg.cbg_flush_indicator =
          opt_pdsch_serving_cell_cfg.value().code_block_group_tx.value().code_block_group_flush_indicator;
      dci_sz_cfg.max_cbg_tb_pdsch =
          static_cast<unsigned>(opt_pdsch_serving_cell_cfg.value().code_block_group_tx.value().max_cbg_per_tb);
    }
  }
  srsran_assert(validate_dci_size_config(dci_sz_cfg), "Invalid DCI size configuration for DCI Format 1_1 (C-RNTI)");

  // Compute DCI size.
  dci_sizes dci_sz  = get_dci_sizes(dci_sz_cfg);
  f1_1.payload_size = dci_sz.format1_1_ue_size.value();

  if (dci_sz_cfg.interleaved_vrb_prb_mapping) {
    f1_1.vrb_prb_mapping = static_cast<unsigned>(opt_pdsch_cfg.value().vrb_to_prb_itlvr.value());
  }

  // TODO:
  //  1. When Dynamic PRB bundling is used, set prb_bundling_size_indicator.
  //  2. When Maximum number of code-block-groups (CBGs) is configured, set cbg_transmission_info.
  //  3. When DMRS configuration is set in PDSCH-Config, set dmrs_seq_initialization based on whether scramblingID0 or
  //  scramblingID1 to be used.

  // UCI resources.
  f1_1.pucch_resource_indicator       = pucch_res_indicator;
  f1_1.pdsch_harq_fb_timing_indicator = get_dci_1_1_pdsch_to_harq_timing_indicator(k1, k1_candidates);
  f1_1.downlink_assignment_index      = dai;

  f1_1.tb1_modulation_coding_scheme = tb1_mcs_index.to_uint();

  // HARQ params.
  f1_1.harq_process_number    = h_dl.id;
  f1_1.tb1_new_data_indicator = h_dl.tb(0).ndi;
  f1_1.tb1_redundancy_version = get_redundancy_version(h_dl.tb(0).nof_retxs);
}

void srsran::build_dci_f0_0_tc_rnti(dci_ul_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    const bwp_configuration&   ul_bwp,
                                    const prb_interval&        prbs,
                                    unsigned                   time_resource,
                                    sch_mcs_index              mcs_index,
                                    const ul_harq_process&     h_ul)
{
  // See TS38.321, 5.4.2.1 - "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is used."
  srsran_assert(h_ul.id == 0, "UL HARQ process used for Msg3 must have id=0");

  dci.type                            = srsran::dci_ul_rnti_config_type::tc_rnti_f0_0;
  dci.tc_rnti_f0_0                    = {};
  dci_0_0_tc_rnti_configuration& f0_0 = dci.tc_rnti_f0_0;

  // TODO.
  f0_0.N_ul_hop               = 0;
  f0_0.hopping_offset         = 0;
  f0_0.frequency_hopping_flag = 0;
  f0_0.tpc_command            = 1;

  // PUSCH params.
  const auto dci_sz_cfg = dci_size_config{
      false,
      false,
      init_dl_bwp.generic_params.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      ul_bwp.crbs.length(),
      ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0,
      false};
  srsran_assert(validate_dci_size_config(dci_sz_cfg), "Invalid DCI size configuration for DCI Format 0_0 (TC-RNTI)");
  dci_sizes dci_sz  = get_dci_sizes(dci_sz_cfg);
  f0_0.payload_size = dci_sz.format0_0_common_size;
  f0_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{ul_bwp.crbs.length(), prbs.start(), prbs.length()});
  f0_0.time_resource = time_resource;

  f0_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_0.redundancy_version = get_redundancy_version(h_ul.tb().nof_retxs);
}

void srsran::build_dci_f0_0_c_rnti(dci_ul_info&                       dci,
                                   const bwp_downlink_common&         init_dl_bwp,
                                   const bwp_downlink_common&         active_dl_bwp,
                                   const bwp_downlink_dedicated*      active_dl_bwp_ded,
                                   const bwp_configuration&           init_ul_bwp,
                                   const bwp_configuration&           active_ul_bwp,
                                   search_space_configuration::type_t ss_type,
                                   const prb_interval&                prbs,
                                   unsigned                           time_resource,
                                   sch_mcs_index                      mcs_index,
                                   const ul_harq_process&             h_ul)
{
  dci.type                           = dci_ul_rnti_config_type::c_rnti_f0_0;
  dci.c_rnti_f0_0                    = {};
  dci_0_0_c_rnti_configuration& f0_0 = dci.c_rnti_f0_0;

  // TODO.
  f0_0.N_ul_hop               = 0;
  f0_0.hopping_offset         = 0;
  f0_0.frequency_hopping_flag = 0;
  f0_0.tpc_command            = 1;
  f0_0.ul_sul_indicator       = {};

  // PUSCH params.
  dci_size_config dci_sz_cfg;
  dci_sz_cfg.dci_0_0_and_1_0_ue_ss = is_dci_format_monitored_in_ue_ss(active_dl_bwp, active_dl_bwp_ded, true);
  dci_sz_cfg.dci_0_1_and_1_1_ue_ss = is_dci_format_monitored_in_ue_ss(active_dl_bwp, active_dl_bwp_ded, false);
  dci_sz_cfg.dl_bwp_initial_bw     = init_dl_bwp.generic_params.crbs.length();
  dci_sz_cfg.dl_bwp_active_bw      = active_dl_bwp.generic_params.crbs.length();
  dci_sz_cfg.ul_bwp_initial_bw     = init_ul_bwp.crbs.length();
  dci_sz_cfg.ul_bwp_active_bw      = active_ul_bwp.crbs.length();
  dci_sz_cfg.coreset0_bw =
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0;
  dci_sz_cfg.sul_configured = false;
  srsran_assert(validate_dci_size_config(dci_sz_cfg), "Invalid DCI size configuration for DCI Format 0_0 (C-RNTI)");

  dci_sizes dci_sz  = get_dci_sizes(dci_sz_cfg);
  f0_0.payload_size = ss_type == search_space_configuration::type_t::ue_dedicated ? dci_sz.format0_0_ue_size.value()
                                                                                  : dci_sz.format0_0_common_size;

  // PUSCH resources.
  // See 38.212, clause 7.3.1.1.1 - N^{UL,BWP}_RB for C-RNTI.
  if (ss_type == srsran::search_space_configuration::type_t::common) {
    f0_0.frequency_resource = ra_frequency_type1_get_riv(
        ra_frequency_type1_configuration{init_ul_bwp.crbs.length(), prbs.start(), prbs.length()});
  } else {
    f0_0.frequency_resource = ra_frequency_type1_get_riv(
        ra_frequency_type1_configuration{active_ul_bwp.crbs.length(), prbs.start(), prbs.length()});
  }

  f0_0.time_resource = time_resource;

  f0_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_0.harq_process_number = h_ul.id;
  f0_0.new_data_indicator  = h_ul.tb().ndi;
  f0_0.redundancy_version  = get_redundancy_version(h_ul.tb().nof_retxs);
}

void srsran::build_dci_f0_1_c_rnti(dci_ul_info&                 dci,
                                   const ue_cell_configuration& ue_cell_cfg,
                                   bwp_id_t                     active_bwp_id,
                                   search_space_id              ss_id,
                                   const prb_interval&          prbs,
                                   unsigned                     time_resource,
                                   sch_mcs_index                mcs_index,
                                   const ul_harq_process&       h_ul)
{
  const search_space_configuration* ss_cfg = ue_cell_cfg.find_search_space(ss_id);
  srsran_assert(ss_cfg != nullptr, "No valid SearchSpace found");
  srsran_assert(ss_cfg->type == search_space_configuration::type_t::ue_dedicated,
                "SearchSpace must be of type UE-Specific SearchSpace");

  const bwp_downlink_common&          init_dl_bwp       = ue_cell_cfg.dl_bwp_common(to_bwp_id(0));
  const bwp_downlink_common&          active_dl_bwp_cmn = ue_cell_cfg.dl_bwp_common(active_bwp_id);
  const bwp_downlink_dedicated*       active_dl_bwp_ded = ue_cell_cfg.find_dl_bwp_ded(active_bwp_id);
  const bwp_configuration&            active_dl_bwp     = active_dl_bwp_cmn.generic_params;
  const bwp_uplink_common&            init_ul_bwp       = ue_cell_cfg.ul_bwp_common(to_bwp_id(0));
  const bwp_configuration&            active_ul_bwp     = ue_cell_cfg.ul_bwp_common(active_bwp_id).generic_params;
  const optional<rach_config_common>& opt_rach_cfg      = ue_cell_cfg.ul_bwp_common(active_bwp_id).rach_cfg_common;
  const optional<csi_meas_config>&    opt_csi_meas_cfg  = ue_cell_cfg.cfg_dedicated().csi_meas_cfg;
  const optional<uplink_config>&      opt_ul_cfg        = ue_cell_cfg.cfg_dedicated().ul_config;

  dci.type                    = dci_ul_rnti_config_type::c_rnti_f0_1;
  dci.c_rnti_f0_1             = {};
  dci_0_1_configuration& f0_1 = dci.c_rnti_f0_1;

  dci_size_config dci_sz_cfg = dci_size_config{
      is_dci_format_monitored_in_ue_ss(active_dl_bwp_cmn, active_dl_bwp_ded, true),
      is_dci_format_monitored_in_ue_ss(active_dl_bwp_cmn, active_dl_bwp_ded, false),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_ul_bwp.generic_params.crbs.length(),
      active_ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0};
  dci_sz_cfg.cross_carrier_configured = false;
  dci_sz_cfg.sul_configured           = false;
  // TODO: Need to fetch from physical_cell_group_config.
  dci_sz_cfg.pdsch_harq_ack_cb = pdsch_harq_ack_codebook::dynamic;
  // TODO: Once additional UL BWPs are added to uplinkConfig, populate below field from configs.
  dci_sz_cfg.nof_ul_bwp_rrc         = 0;
  dci_sz_cfg.nof_ul_time_domain_res = ue_cell_cfg.get_pusch_time_domain_list(ss_id).size();
  dci_sz_cfg.report_trigger_size    = 0;
  if (opt_csi_meas_cfg.has_value() and opt_csi_meas_cfg.value().report_trigger_size.has_value()) {
    dci_sz_cfg.report_trigger_size = opt_csi_meas_cfg.value().report_trigger_size.value();
  }
  dci_sz_cfg.frequency_hopping_configured = false;
  dci_sz_cfg.tx_config_non_codebook       = false;
  dci_sz_cfg.ptrs_uplink_configured       = false;
  dci_sz_cfg.dynamic_beta_offsets         = false;
  if (opt_rach_cfg.has_value()) {
    dci_sz_cfg.transform_precoding_enabled = opt_rach_cfg.value().msg3_transform_precoder;
  }
  dci_sz_cfg.pusch_res_allocation_type = resource_allocation::resource_allocation_type_1;
  if (opt_ul_cfg.has_value()) {
    const optional<pusch_config>&              opt_pusch_cfg    = opt_ul_cfg.value().init_ul_bwp.pusch_cfg;
    const optional<srs_config>&                opt_srs_cfg      = opt_ul_cfg.value().init_ul_bwp.srs_cfg;
    const optional<pusch_serving_cell_config>& opt_pusch_sc_cfg = opt_ul_cfg.value().pusch_scell_cfg;
    if (opt_pusch_cfg.has_value()) {
      dci_sz_cfg.tx_config_non_codebook = opt_pusch_cfg.value().tx_cfg != pusch_config::tx_config::not_set and
                                          opt_pusch_cfg.value().tx_cfg == pusch_config::tx_config::non_codebook;
      dci_sz_cfg.transform_precoding_enabled =
          opt_pusch_cfg.value().trans_precoder != pusch_config::transform_precoder::not_set;
      if ((opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.has_value() and
           opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.value().ptrs.has_value()) or
          (opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.has_value() and
           opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.value().ptrs.has_value())) {
        dci_sz_cfg.ptrs_uplink_configured = true;
      }
      if (opt_pusch_cfg.value().uci_cfg.has_value() and
          opt_pusch_cfg.value().uci_cfg.value().beta_offsets_cfg.has_value() and
          variant_holds_alternative<uci_on_pusch::beta_offsets_dynamic>(
              opt_pusch_cfg.value().uci_cfg.value().beta_offsets_cfg.value())) {
        dci_sz_cfg.dynamic_beta_offsets = true;
      }
      switch (opt_pusch_cfg.value().res_alloc) {
        case pusch_config::resource_allocation::resource_allocation_type_0: {
          dci_sz_cfg.pusch_res_allocation_type = resource_allocation::resource_allocation_type_0;
          // TODO: Once rbg-Size is declared inn PUSCH-Config, use the value from config.
          dci_sz_cfg.nof_ul_rb_groups = static_cast<unsigned>(get_nominal_rbg_size(active_ul_bwp.crbs.length(), true));
          break;
        }
        case pusch_config::resource_allocation::resource_allocation_type_1: {
          dci_sz_cfg.pusch_res_allocation_type = resource_allocation::resource_allocation_type_1;
          break;
        }
        case pusch_config::resource_allocation::dynamic_switch: {
          dci_sz_cfg.pusch_res_allocation_type = resource_allocation::dynamic_switch;
          // TODO: Once rbg-Size is defined in PUSCH-Config, use the value from config.
          dci_sz_cfg.nof_ul_rb_groups = static_cast<unsigned>(get_nominal_rbg_size(active_ul_bwp.crbs.length(), true));
          break;
        }
      }
      switch (opt_pusch_cfg.value().cb_subset) {
        case pusch_config::codebook_subset::fully_and_partial_and_non_coherent:
          dci_sz_cfg.cb_subset = tx_scheme_codebook_subset::fully_and_partial_and_non_coherent;
          break;
        case pusch_config::codebook_subset::partial_and_non_coherent:
          dci_sz_cfg.cb_subset = tx_scheme_codebook_subset::partial_and_non_coherent;
          break;
        case pusch_config::codebook_subset::non_coherent:
          dci_sz_cfg.cb_subset = tx_scheme_codebook_subset::non_coherent;
          break;
        default:
          break;
      }
      if (dci_sz_cfg.ptrs_uplink_configured and dci_sz_cfg.cb_subset.has_value() and
          not dci_sz_cfg.tx_config_non_codebook) {
        srsran_assert(opt_pusch_cfg.value().max_rank.has_value(), "No valid max. rank found in PUSCH config");
        dci_sz_cfg.max_rank = opt_pusch_cfg.value().max_rank.value();
      }
      if (opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.has_value()) {
        dci_sz_cfg.pusch_dmrs_A_type    = opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.value().is_dmrs_type2
                                              ? dmrs_config_type::type2
                                              : dmrs_config_type::type1;
        dci_sz_cfg.pusch_dmrs_A_max_len = opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.value().is_max_length_len2
                                              ? dmrs_max_length::len2
                                              : dmrs_max_length::len1;
      }
      if (opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.has_value()) {
        dci_sz_cfg.pusch_dmrs_B_type    = opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.value().is_dmrs_type2
                                              ? dmrs_config_type::type2
                                              : dmrs_config_type::type1;
        dci_sz_cfg.pusch_dmrs_B_max_len = opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.value().is_max_length_len2
                                              ? dmrs_max_length::len2
                                              : dmrs_max_length::len1;
      }
    }
    if (opt_srs_cfg.has_value()) {
      if (not dci_sz_cfg.tx_config_non_codebook) {
        // See TS 38.214, clause 6.1.1.1.
        const auto* srs_res_set =
            std::find_if(opt_srs_cfg.value().srs_res_set.begin(),
                         opt_srs_cfg.value().srs_res_set.end(),
                         [](const srs_config::srs_resource_set& res_set) {
                           return res_set.srs_res_set_usage == srs_config::srs_resource_set::usage::codebook;
                         });
        srsran_assert(srs_res_set != opt_srs_cfg.value().srs_res_set.end(), "No valid SRS resource set found");
        srsran_assert(not srs_res_set->srs_res_id_list.empty(), "No SRS resource configured in SRS resource set");
        const auto  srs_resource_id = srs_res_set->srs_res_id_list.front();
        const auto* srs_res =
            std::find_if(opt_srs_cfg.value().srs_res.begin(),
                         opt_srs_cfg.value().srs_res.end(),
                         [srs_resource_id](const srs_config::srs_resource& res) { return res.id == srs_resource_id; });
        srsran_assert(srs_res != opt_srs_cfg.value().srs_res.end(), "No valid SRS resource found");
        dci_sz_cfg.nof_srs_ports = static_cast<unsigned>(srs_res->nof_ports);
      } else {
        // See TS 38.214, clause 6.1.1.1.
        const auto* srs_res_set =
            std::find_if(opt_srs_cfg.value().srs_res_set.begin(),
                         opt_srs_cfg.value().srs_res_set.end(),
                         [](const srs_config::srs_resource_set& res_set) {
                           return res_set.srs_res_set_usage == srs_config::srs_resource_set::usage::non_codebook;
                         });
        srsran_assert(srs_res_set != opt_srs_cfg.value().srs_res_set.end(), "No valid SRS resource set found");
        srsran_assert(not srs_res_set->srs_res_id_list.empty(), "No SRS resource configured in SRS resource set");
        dci_sz_cfg.nof_srs_resources = srs_res_set->srs_res_id_list.size();
      }
    }
    if (opt_pusch_sc_cfg.has_value() and opt_pusch_sc_cfg.value().cbg_tx.has_value()) {
      dci_sz_cfg.max_cbg_tb_pusch = static_cast<unsigned>(opt_pusch_sc_cfg.value().cbg_tx.value().max_cgb_per_tb);
    }
    if (dci_sz_cfg.tx_config_non_codebook) {
      // TODO: Set value based on maxMIMO-Layers config in PUSCH-ServingCellConfig or UE capability.
      dci_sz_cfg.pusch_max_layers = 1;
    }
  }
  if (dci_sz_cfg.pdsch_harq_ack_cb == pdsch_harq_ack_codebook::dynamic) {
    // TODO: Set value based on config.
    dci_sz_cfg.dynamic_dual_harq_ack_cb = false;
  }
  srsran_assert(validate_dci_size_config(dci_sz_cfg), "Invalid DCI size configuration for DCI Format 0_1 (C-RNTI)");

  dci_sizes dci_sz  = get_dci_sizes(dci_sz_cfg);
  f0_1.payload_size = dci_sz.format0_1_ue_size.value();

  f0_1.tpc_command             = 1;
  f0_1.srs_request             = 0;
  f0_1.dmrs_seq_initialization = 0;
  // TODO: Set proper value based on nof. layers used. See TS 38.212, clause 7.3.1.1.2.
  f0_1.antenna_ports = 0;

  // See 38.212, clause 7.3.1.1.2 - N^{UL,BWP}_RB for C-RNTI.
  f0_1.frequency_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{active_ul_bwp.crbs.length(), prbs.start(), prbs.length()});
  f0_1.time_resource = time_resource;

  f0_1.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_1.harq_process_number = h_ul.id;
  f0_1.new_data_indicator  = h_ul.tb().ndi;
  f0_1.redundancy_version  = get_redundancy_version(h_ul.tb().nof_retxs);

  // TODO: Set values for -
  //  1. first_dl_assignment_index - not optional
  //  2. srs_resource_indicator
  //  3. precoding_info_nof_layers
  //  4. beta_offset_indicator
  //  5. ptrs_dmrs_association
  //  6. cbg_transmission_info

  f0_1.ul_sch_indicator = 1;
}

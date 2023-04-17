/*
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_dci_builder.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/pdcch/dci_packing.h"
#include "srsran/ran/physical_cell_group.h"
#include "srsran/scheduler/config/bwp_configuration.h"
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
                                   const bwp_configuration&           active_dl_bwp,
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
    N_rb_dl_bwp = active_dl_bwp.crbs.length();
  }
  f1_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{N_rb_dl_bwp, prbs.start(), prbs.length()});
  f1_0.time_resource = time_resource;

  // PUSCH params.
  dci_sizes dci_sz  = get_dci_sizes(dci_size_config{
      ss_type == search_space_configuration::type_t::ue_dedicated,
      false,
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0,
      false});
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

  const bwp_downlink_common& init_dl_bwp   = ue_cell_cfg.dl_bwp_common(to_bwp_id(0));
  const bwp_configuration&   active_dl_bwp = ue_cell_cfg.dl_bwp_common(active_bwp_id).generic_params;
  const auto                 k1_candidates = ue_cell_cfg.get_k1_candidates(dci_dl_rnti_config_type::c_rnti_f1_1);
  const auto&                opt_pdsch_cfg = ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg;
  const auto&                opt_pdsch_serving_cell_cfg = ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg;

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
      false,
      true,
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
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
        dci_sz_cfg.pdsch_res_allocation_type   = resource_allocation::resource_allocation_type_0;
        dci_sz_cfg.interleaved_vrb_prb_mapping = opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value();
        break;
      }
      case pdsch_config::resource_allocation::resource_allocation_type_1: {
        dci_sz_cfg.pdsch_res_allocation_type = resource_allocation::resource_allocation_type_1;
        dci_sz_cfg.nof_dl_rb_groups          = static_cast<unsigned>(
            get_nominal_rbg_size(active_dl_bwp.crbs.length(), opt_pdsch_cfg.value().rbg_sz == rbg_size::config1));
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
  }
  dci_sz_cfg.multiple_scells     = is_ue_configured_multiple_serving_cells;
  dci_sz_cfg.pdsch_tci           = false;
  dci_sz_cfg.cbg_flush_indicator = false;
  if (opt_pdsch_serving_cell_cfg.has_value()) {
    dci_sz_cfg.cbg_flush_indicator =
        opt_pdsch_serving_cell_cfg.value().code_block_group_tx.has_value()
            ? opt_pdsch_serving_cell_cfg.value().code_block_group_tx.value().code_block_group_flush_indicator
            : false;
  }

  // Compute DCI size.
  dci_sizes dci_sz  = get_dci_sizes(dci_sz_cfg);
  f1_1.payload_size = dci_sz.format1_1_ue_size.value();

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
  dci_sizes dci_sz  = get_dci_sizes(dci_size_config{
      false,
      false,
      init_dl_bwp.generic_params.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      ul_bwp.crbs.length(),
      ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0,
      false});
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
                                   const bwp_configuration&           active_dl_bwp,
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
  dci_size_config dci_config;
  dci_config.dci_0_0_and_1_0_ue_ss = ss_type != srsran::search_space_configuration::type_t::common;
  dci_config.dci_0_1_and_1_1_ue_ss = false;
  dci_config.dl_bwp_initial_bw     = init_dl_bwp.generic_params.crbs.length();
  dci_config.dl_bwp_active_bw      = active_dl_bwp.crbs.length();
  dci_config.ul_bwp_initial_bw     = init_ul_bwp.crbs.length();
  dci_config.ul_bwp_active_bw      = active_ul_bwp.crbs.length();
  dci_config.coreset0_bw =
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0;
  dci_config.sul_configured = false;

  dci_sizes dci_sz  = get_dci_sizes(dci_config);
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

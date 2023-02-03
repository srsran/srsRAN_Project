/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_dci_builder.h"

using namespace srsgnb;

/// Get PDSCH-to-HARQ-timing-indicator field as per TS38.213, 9.2.3.
static unsigned get_dci_1_0_pdsch_to_harq_timing_indicator(unsigned k1)
{
  // PDSCH-to-HARQ-timing-indicator maps to {1,2,3,4,5,6,7,8} for DCI 1_0.
  srsgnb_assert(k1 <= 8, "Invalid k1 value");
  return k1 - 1;
}

void srsgnb::build_dci_f1_0_tc_rnti(dci_dl_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    prb_interval               prbs,
                                    unsigned                   time_resource,
                                    unsigned                   k1,
                                    unsigned                   pucch_res_indicator,
                                    sch_mcs_index              mcs_index,
                                    const dl_harq_process&     h_dl)
{
  static constexpr unsigned tb_idx = 0;

  dci.type                            = srsgnb::dci_dl_rnti_config_type::tc_rnti_f1_0;
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

void srsgnb::build_dci_f1_0_c_rnti(dci_dl_info&                       dci,
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
  dci.type                           = srsgnb::dci_dl_rnti_config_type::c_rnti_f1_0;
  dci.c_rnti_f1_0                    = {};
  dci_1_0_c_rnti_configuration& f1_0 = dci.c_rnti_f1_0;

  f1_0.tpc_command        = 1;
  f1_0.vrb_to_prb_mapping = 0;

  // PDSCH resources.
  // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB for C-RNTI.
  if (ss_type == srsgnb::search_space_configuration::type_t::common) {
    f1_0.N_rb_dl_bwp = init_dl_bwp.pdcch_common.coreset0.has_value()
                           ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()
                           : init_dl_bwp.generic_params.crbs.length();
  } else {
    f1_0.N_rb_dl_bwp = active_dl_bwp.crbs.length();
  }
  f1_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{f1_0.N_rb_dl_bwp, prbs.start(), prbs.length()});
  f1_0.time_resource = time_resource;

  // PUSCH params.
  dci_sizes dci_sz  = get_dci_sizes(dci_size_config{
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0});
  f1_0.payload_size = ss_type == search_space_configuration::type_t::ue_dedicated ? dci_sz.format1_0_ue_size
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

void srsgnb::build_dci_f0_0_tc_rnti(dci_ul_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    const bwp_configuration&   ul_bwp,
                                    const prb_interval&        prbs,
                                    unsigned                   time_resource,
                                    sch_mcs_index              mcs_index,
                                    const ul_harq_process&     h_ul)
{
  // See TS38.321, 5.4.2.1 - "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is used."
  srsgnb_assert(h_ul.id == 0, "UL HARQ process used for Msg3 must have id=0");

  dci.type                            = srsgnb::dci_ul_rnti_config_type::tc_rnti_f0_0;
  dci.tc_rnti_f0_0                    = {};
  dci_0_0_tc_rnti_configuration& f0_0 = dci.tc_rnti_f0_0;

  // TODO.
  f0_0.N_ul_hop               = 0;
  f0_0.hopping_offset         = 0;
  f0_0.frequency_hopping_flag = 0;
  f0_0.tpc_command            = 1;

  // PUSCH params.
  dci_sizes dci_sz  = get_dci_sizes(dci_size_config{
      init_dl_bwp.generic_params.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      ul_bwp.crbs.length(),
      ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0});
  f0_0.payload_size = dci_sz.format0_0_common_size;
  f0_0.N_rb_ul_bwp  = ul_bwp.crbs.length();
  f0_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{ul_bwp.crbs.length(), prbs.start(), prbs.length()});
  f0_0.time_resource = time_resource;

  f0_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_0.redundancy_version = get_redundancy_version(h_ul.tb().nof_retxs);
}

void srsgnb::build_dci_f0_0_c_rnti(dci_ul_info&                       dci,
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
  dci_sizes dci_sz  = get_dci_sizes(dci_size_config{
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_ul_bwp.crbs.length(),
      active_ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0});
  f0_0.payload_size = ss_type == search_space_configuration::type_t::ue_dedicated ? dci_sz.format0_0_ue_size
                                                                                  : dci_sz.format0_0_common_size;

  // PUSCH resources.
  // See 38.212, clause 7.3.1.1.1 - N^{UL,BWP}_RB for C-RNTI.
  if (ss_type == srsgnb::search_space_configuration::type_t::common) {
    f0_0.N_rb_ul_bwp = init_ul_bwp.crbs.length();
  } else {
    f0_0.N_rb_ul_bwp = active_ul_bwp.crbs.length();
  }

  f0_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{f0_0.N_rb_ul_bwp, prbs.start(), prbs.length()});
  f0_0.time_resource = time_resource;

  f0_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_0.harq_process_number = h_ul.id;
  f0_0.new_data_indicator  = h_ul.tb().ndi;
  f0_0.redundancy_version  = get_redundancy_version(h_ul.tb().nof_retxs);
}

/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/adt/optional.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/ofdm_symbol_range.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/config/csi_helper.h"
#include "srsran/srslog/srslog.h"
#include <set>
#include <vector>

using namespace srsran;
using namespace srsran::config_helpers;

static nr_band get_band(const cell_config_builder_params& params)
{
  return params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
}

static unsigned cell_nof_crbs(const cell_config_builder_params& params)
{
  return band_helper::get_n_rbs_from_bw(params.channel_bw_mhz,
                                        params.scs_common,
                                        params.band.has_value() ? band_helper::get_freq_range(params.band.value())
                                                                : frequency_range::FR1);
}

static carrier_configuration make_default_carrier_configuration(const cell_config_builder_params& params, bool is_dl)
{
  carrier_configuration cfg{};
  cfg.carrier_bw_mhz = bs_channel_bandwidth_to_MHz(params.channel_bw_mhz);
  cfg.band           = get_band(params);
  if (is_dl) {
    cfg.arfcn   = params.dl_arfcn;
    cfg.nof_ant = params.nof_dl_ports;
  } else {
    cfg.arfcn   = band_helper::get_ul_arfcn_from_dl_arfcn(params.dl_arfcn, cfg.band);
    cfg.nof_ant = 1;
  }
  const min_channel_bandwidth min_channel_bw = band_helper::get_min_channel_bw(cfg.band, params.scs_common);
  srsran_assert(cfg.carrier_bw_mhz >= min_channel_bandwidth_to_MHz(min_channel_bw),
                "Carrier BW {}Mhz must be greater than or equal to minimum channel BW {}Mhz",
                cfg.carrier_bw_mhz,
                min_channel_bandwidth_to_MHz(min_channel_bw));
  return cfg;
}

static_vector<uint8_t, 8> srsran::config_helpers::generate_k1_candidates(const tdd_ul_dl_config_common& tdd_cfg)
{
  // TODO: Fetch cyclic prefix from other configuration.
  static const cyclic_prefix cp = cyclic_prefix::NORMAL;
  static_vector<uint8_t, 8>  result;
  for (unsigned idx = 0; idx < nof_slots_per_tdd_period(tdd_cfg); ++idx) {
    // For every slot containing DL symbols check for corresponding k1 value.
    if (get_active_tdd_dl_symbols(tdd_cfg, idx, cp).length() > 0) {
      for (unsigned k1 = SCHEDULER_MIN_K1; k1 <= SCHEDULER_MAX_K1; ++k1) {
        // TODO: Consider partial UL slots when scheduler supports it.
        if (not result.full() and get_active_tdd_ul_symbols(tdd_cfg, idx + k1, cp).length() == get_nsymb_per_slot(cp)) {
          if (std::find(result.begin(), result.end(), k1) == result.end()) {
            result.emplace_back(k1);
          }
          break;
        }
      }
    }
  }
  // Sorting in ascending order is performed to reduce the latency with which UCI is scheduled.
  std::sort(result.begin(), result.end());
  return result;
}

carrier_configuration
srsran::config_helpers::make_default_dl_carrier_configuration(const cell_config_builder_params& params)
{
  return make_default_carrier_configuration(params, true);
}

carrier_configuration
srsran::config_helpers::make_default_ul_carrier_configuration(const cell_config_builder_params& params)
{
  return make_default_carrier_configuration(params, false);
}

tdd_ul_dl_config_common
srsran::config_helpers::make_default_tdd_ul_dl_config_common(const cell_config_builder_params& params)
{
  tdd_ul_dl_config_common cfg{};
  cfg.ref_scs                            = params.scs_common;
  cfg.pattern1.dl_ul_tx_period_nof_slots = 10;
  cfg.pattern1.nof_dl_slots              = 6;
  cfg.pattern1.nof_dl_symbols            = 0;
  cfg.pattern1.nof_ul_slots              = 3;
  cfg.pattern1.nof_ul_symbols            = 0;
  return cfg;
}

coreset_configuration srsran::config_helpers::make_default_coreset_config(const cell_config_builder_params& params)
{
  coreset_configuration cfg{};
  cfg.id = to_coreset_id(1);
  // PRBs spanning the maximnum number of CRBs possible.
  freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
  const unsigned       coreset_nof_resources = cell_nof_crbs(params) / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE;
  freq_resources.fill(0, coreset_nof_resources, true);
  cfg.set_freq_domain_resources(freq_resources);
  const nr_band band =
      params.band.has_value() ? params.band.value() : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  // Number of symbols equal to max(CORESET#0, 2).
  const pdcch_type0_css_coreset_description desc =
      pdcch_type0_css_coreset_get(band, params.scs_common, params.scs_common, params.coreset0_index, 0);
  cfg.duration             = std::max(2U, static_cast<unsigned>(desc.nof_symb_coreset));
  cfg.precoder_granurality = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
  return cfg;
}

/// Generates a default CORESET#0 configuration. The default CORESET#0 table index value used is equal to 6.
/// \remark See TS 38.213, Table 13-1.
coreset_configuration srsran::config_helpers::make_default_coreset0_config(const cell_config_builder_params& params)
{
  coreset_configuration cfg{};
  cfg.id = to_coreset_id(0);
  const nr_band band =
      params.band.has_value() ? params.band.value() : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  const pdcch_type0_css_coreset_description desc =
      pdcch_type0_css_coreset_get(band, params.scs_common, params.scs_common, params.coreset0_index, 0);

  cfg.duration       = static_cast<unsigned>(desc.nof_symb_coreset);
  const int rb_start = params.scs_common == subcarrier_spacing::kHz15
                           ? static_cast<int>(params.offset_to_point_a.to_uint()) - desc.offset
                           : static_cast<int>(params.offset_to_point_a.to_uint() / 2) - desc.offset;
  if (rb_start < 0) {
    report_error("Coreset#0 CRB starts before pointA.\n");
  }
  cfg.set_coreset0_crbs({static_cast<unsigned>(rb_start), static_cast<unsigned>(rb_start) + desc.nof_rb_coreset});
  // Implicit CORESET#0 parameters as per TS38.211-7.3.2.2.
  cfg.interleaved.emplace();
  cfg.interleaved->interleaver_sz = 2;
  cfg.interleaved->reg_bundle_sz  = 6;
  cfg.interleaved->shift_index    = params.pci;
  cfg.precoder_granurality        = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;

  return cfg;
}

search_space_configuration
srsran::config_helpers::make_default_search_space_zero_config(const cell_config_builder_params& params)
{
  return search_space_configuration{
      params.dl_arfcn, params.scs_common, params.scs_common, params.coreset0_index, params.search_space0_index};
}

search_space_configuration
srsran::config_helpers::make_default_common_search_space_config(const cell_config_builder_params& params)
{
  search_space_configuration::monitoring_symbols_within_slot_t monitoring_symbols_within_slot(
      NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
  monitoring_symbols_within_slot.set(0, true);
  return search_space_configuration{to_search_space_id(1),
                                    to_coreset_id(0),
                                    {0, 0, 1, 0, 0},
                                    search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true},
                                    1,
                                    0,
                                    params.scs_common,
                                    1,
                                    monitoring_symbols_within_slot};
}

search_space_configuration
srsran::config_helpers::make_default_ue_search_space_config(const cell_config_builder_params& params)
{
  search_space_configuration cfg = make_default_common_search_space_config(params);
  cfg.set_non_ss0_coreset_id(to_coreset_id(1));
  cfg.set_non_ss0_id(to_search_space_id(2));
  cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);
  return cfg;
}

bwp_configuration srsran::config_helpers::make_default_init_bwp(const cell_config_builder_params& params)
{
  bwp_configuration cfg{};
  cfg.scs  = params.scs_common;
  cfg.crbs = {0, cell_nof_crbs(params)};
  cfg.cp   = cyclic_prefix::NORMAL;
  return cfg;
}

dl_config_common srsran::config_helpers::make_default_dl_config_common(const cell_config_builder_params& params)
{
  dl_config_common cfg{};

  // Configure FrequencyInfoDL.
  cfg.freq_info_dl.freq_band_list.emplace_back();
  cfg.freq_info_dl.freq_band_list.back().band =
      params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  cfg.freq_info_dl.offset_to_point_a = params.offset_to_point_a.to_uint();
  cfg.freq_info_dl.scs_carrier_list.emplace_back();
  cfg.freq_info_dl.scs_carrier_list.back().scs               = params.scs_common;
  cfg.freq_info_dl.scs_carrier_list.back().offset_to_carrier = 0;

  cfg.freq_info_dl.scs_carrier_list.back().carrier_bandwidth = cell_nof_crbs(params);

  // Configure initial DL BWP.
  cfg.init_dl_bwp.generic_params = make_default_init_bwp(params);
  cfg.init_dl_bwp.pdcch_common.coreset0.emplace(make_default_coreset0_config(params));
  cfg.init_dl_bwp.pdcch_common.search_spaces.push_back(make_default_search_space_zero_config(params));
  cfg.init_dl_bwp.pdcch_common.search_spaces.push_back(make_default_common_search_space_config(params));
  cfg.init_dl_bwp.pdcch_common.sib1_search_space_id     = to_search_space_id(0);
  cfg.init_dl_bwp.pdcch_common.other_si_search_space_id = MAX_NOF_SEARCH_SPACES;
  cfg.init_dl_bwp.pdcch_common.paging_search_space_id   = to_search_space_id(1);
  cfg.init_dl_bwp.pdcch_common.ra_search_space_id       = to_search_space_id(1);
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list      = make_pdsch_time_domain_resource(
      params.search_space0_index,
      cfg.init_dl_bwp.pdcch_common,
      nullopt,
      band_helper::get_duplex_mode(cfg.freq_info_dl.freq_band_list.back().band) == duplex_mode::TDD
               ? make_default_tdd_ul_dl_config_common(params)
               : optional<tdd_ul_dl_config_common>{});

  // Configure PCCH.
  cfg.pcch_cfg.default_paging_cycle = paging_cycle::rf128;
  cfg.pcch_cfg.nof_pf               = pcch_config::nof_pf_per_drx_cycle::oneT;
  cfg.pcch_cfg.paging_frame_offset  = 0;
  cfg.pcch_cfg.ns                   = pcch_config::nof_po_per_pf::one;

  return cfg;
}

ul_config_common srsran::config_helpers::make_default_ul_config_common(const cell_config_builder_params& params)
{
  ul_config_common cfg{};
  // This is the ARFCN of the UL f_ref, as per TS 38.104, Section 5.4.2.1.
  const uint32_t ul_arfcn = band_helper::get_ul_arfcn_from_dl_arfcn(params.dl_arfcn, params.band);
  // This is f_ref frequency for UL, expressed in Hz and obtained from the corresponding ARFCN.

  const frequency_range freq_range =
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1;
  const duplex_mode duplex = band_helper::get_duplex_mode(
      params.band.has_value() ? params.band.value() : band_helper::get_band_from_dl_arfcn(params.dl_arfcn));

  const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(params.channel_bw_mhz, params.scs_common, freq_range);

  const double ul_f_ref = band_helper::get_abs_freq_point_a_from_f_ref(
      band_helper::nr_arfcn_to_freq(ul_arfcn), nof_crbs, params.scs_common);
  // absolute_freq_point_a needs to be expressed as in ARFCN, as per \c absoluteFrequencyPointA definition in 38.211,
  // Section 4.4.4.2.
  cfg.freq_info_ul.absolute_freq_point_a = band_helper::freq_to_nr_arfcn(ul_f_ref);
  cfg.freq_info_ul.scs_carrier_list.resize(1);
  cfg.freq_info_ul.scs_carrier_list[0].scs               = params.scs_common;
  cfg.freq_info_ul.scs_carrier_list[0].offset_to_carrier = 0;
  cfg.freq_info_ul.scs_carrier_list[0].carrier_bandwidth = nof_crbs;
  cfg.freq_info_ul.freq_band_list.emplace_back();
  cfg.freq_info_ul.freq_band_list.back().band =
      params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  cfg.init_ul_bwp.generic_params = make_default_init_bwp(params);
  cfg.init_ul_bwp.rach_cfg_common.emplace();
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index = 1;
  // Although this is not specified in the TS, from our tests, the UE espects Msg1-SCS to be given when using short
  // PRACH Preambles formats. With long formats, we can set Msg1-SCS as \c invalid, in which case the UE derives the
  // PRACH SCS from \c prach-ConfigurationIndex in RACH-ConfigGeneric.
  cfg.init_ul_bwp.rach_cfg_common->msg1_scs =
      is_long_preamble(prach_configuration_get(
                           freq_range, duplex, cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)
                           .format)
          ? subcarrier_spacing::invalid
          : params.scs_common;
  cfg.init_ul_bwp.rach_cfg_common->restricted_set = restricted_set_config::UNRESTRICTED;
  // Set l839 for long preamble formats, l139 for short preamble formats, as per Tables 6.3.3.1-1 and 6.3.3.1-2,
  // TS 38.211
  cfg.init_ul_bwp.rach_cfg_common->is_prach_root_seq_index_l839 = is_long_preamble(
      prach_configuration_get(freq_range, duplex, cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)
          .format);
  cfg.init_ul_bwp.rach_cfg_common->prach_root_seq_index      = 1;
  cfg.init_ul_bwp.rach_cfg_common->msg3_transform_precoder   = false;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_fdm = 1;
  // Add +3 PRBS to the MSG1 frequency start, which act as a guardband between the PUCCH and PRACH.
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start         = 6;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.zero_correlation_zone_config = 15;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window = 10U << to_numerology_value(params.scs_common);
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.preamble_rx_target_pw = -100;
  cfg.init_ul_bwp.pusch_cfg_common.emplace();
  auto to_pusch_td_list = [](const std::initializer_list<unsigned>& k2s) {
    std::vector<pusch_time_domain_resource_allocation> vec;
    vec.reserve(k2s.size());
    for (const unsigned k2 : k2s) {
      vec.push_back(pusch_time_domain_resource_allocation{
          .k2 = k2, .map_type = sch_mapping_type::typeA, .symbols = ofdm_symbol_range{0, 14}});
    }
    return vec;
  };
  if (band_helper::get_duplex_mode(get_band(params)) == duplex_mode::FDD) {
    cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list = to_pusch_td_list({4});
  } else {
    // TDD
    // - [Implementation-defined] Ensure k2 value which is less than or equal to minimum value of k1(s) exist in the
    // first entry of list. This way PDSCH(s) are scheduled before PUSCH and all DL slots are filled with PDSCH and all
    // UL slots are filled with PUSCH under heavy load. It also ensures that correct DAI value goes in the UL PDCCH of
    // DCI Format 0_1.
    cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list = to_pusch_td_list({4, 5, 6, 7});
  }
  cfg.init_ul_bwp.pucch_cfg_common.emplace();
  cfg.init_ul_bwp.pucch_cfg_common->pucch_resource_common        = 11;
  cfg.init_ul_bwp.pucch_cfg_common->group_hopping                = pucch_group_hopping::NEITHER;
  cfg.init_ul_bwp.pucch_cfg_common->p0_nominal                   = -90;
  cfg.init_ul_bwp.pusch_cfg_common.value().p0_nominal_with_grant = -76;
  cfg.init_ul_bwp.pusch_cfg_common.value().msg3_delta_preamble   = 6;
  cfg.init_ul_bwp.pusch_cfg_common.value().msg3_delta_power      = 8;

  return cfg;
}

ssb_configuration srsran::config_helpers::make_default_ssb_config(const cell_config_builder_params& params)
{
  ssb_configuration cfg{};

  cfg.scs = band_helper::get_most_suitable_ssb_scs(
      params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn), params.scs_common);
  cfg.offset_to_point_A = ssb_offset_to_pointA{params.offset_to_point_a};
  cfg.ssb_period        = ssb_periodicity::ms10;
  cfg.k_ssb             = params.k_ssb;

  const unsigned beam_index = 63;
  cfg.ssb_bitmap            = uint64_t(1) << beam_index;
  cfg.beam_ids[beam_index]  = 0;

  // The values we assign to these parameters are implementation-defined.
  cfg.ssb_block_power = -16;
  cfg.pss_to_sss_epre = ssb_pss_to_sss_epre::dB_0;

  return cfg;
}

pusch_config srsran::config_helpers::make_default_pusch_config(const cell_config_builder_params& params)
{
  pusch_config cfg{};
  // TODO: Verify whether its the correct Transmission Configuration we want to support.
  cfg.tx_cfg = pusch_config::tx_config::codebook;
  cfg.pusch_mapping_type_a_dmrs.emplace();
  cfg.pusch_mapping_type_a_dmrs.value().trans_precoder_disabled.emplace();
  cfg.pusch_mapping_type_a_dmrs.value().additional_positions = dmrs_additional_positions::pos2;

  cfg.pusch_pwr_ctrl = pusch_config::pusch_power_control{.msg3_alpha               = alpha::alpha1,
                                                         .p0_nominal_without_grant = -76,
                                                         .p0_alphasets             = {},
                                                         .pathloss_ref_rs          = {},
                                                         .sri_pusch_mapping        = {}};
  cfg.pusch_pwr_ctrl.value().p0_alphasets.push_back(pusch_config::pusch_power_control::p0_pusch_alphaset{
      .id = static_cast<p0_pusch_alphaset_id>(0), .p0 = 0, .p0_pusch_alpha = alpha::alpha1});
  cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.push_back(pusch_config::pusch_power_control::pusch_pathloss_ref_rs{
      .id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .rs = static_cast<ssb_id_t>(0)});
  cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.push_back(pusch_config::pusch_power_control::sri_pusch_pwr_ctrl{
      .id                           = static_cast<pusch_config::pusch_power_control::sri_pusch_pwr_ctrl_id>(0),
      .sri_pusch_pathloss_ref_rs_id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .sri_p0_pusch_alphaset_id     = static_cast<p0_pusch_alphaset_id>(0),
      .closed_loop_idx = pusch_config::pusch_power_control::sri_pusch_pwr_ctrl::sri_pusch_closed_loop_index::i0});
  cfg.res_alloc      = pusch_config::resource_allocation::resource_allocation_type_1;
  cfg.trans_precoder = pusch_config::transform_precoder::disabled;
  cfg.cb_subset      = pusch_config::codebook_subset::non_coherent;
  cfg.max_rank       = 1;

  cfg.uci_cfg.emplace();
  auto& uci_cfg   = cfg.uci_cfg.value();
  uci_cfg.scaling = alpha_scaling_opt::f1;
  beta_offsets b_offset{};
  b_offset.beta_offset_ack_idx_1    = 9;
  b_offset.beta_offset_ack_idx_2    = 9;
  b_offset.beta_offset_ack_idx_3    = 9;
  b_offset.beta_offset_csi_p1_idx_1 = 9;
  b_offset.beta_offset_csi_p1_idx_2 = 9;
  b_offset.beta_offset_csi_p2_idx_1 = 9;
  b_offset.beta_offset_csi_p2_idx_2 = 9;
  uci_cfg.beta_offsets_cfg          = uci_on_pusch::beta_offsets_semi_static{b_offset};

  return cfg;
}

srs_config srsran::config_helpers::make_default_srs_config(const cell_config_builder_params& params)
{
  srs_config cfg{};

  cfg.srs_res.emplace_back();
  // TODO: Verify correctness of the config based on what we support.
  srs_config::srs_resource& res = cfg.srs_res.back();
  res.id                        = static_cast<srs_config::srs_res_id>(0);
  res.nof_ports                 = srs_config::srs_resource::nof_srs_ports::port1;
  res.trans_comb_value          = 2;
  res.trans_comb_offset         = 0;
  res.trans_comb_cyclic_shift   = 0;
  res.res_mapping.start_pos     = 0;
  res.res_mapping.nof_symb      = srs_config::srs_resource::resource_mapping::nof_symbols::n1;
  res.res_mapping.re_factor     = srs_config::srs_resource::resource_mapping::repetition_factor::n1;
  res.freq_domain_pos           = 0;
  res.freq_domain_shift         = 5;
  // NOTE: C_SRS, B_SRS and B_hop are chosen to disable SRS frequency hopping and to monitor SRS over smallest
  // possible BW i.e. 4 RBs. See TS 38.211, Table 6.4.1.4.3-1.
  // This is done to cater to setups of all BWs until SRS is supported in scheduler.
  // TODO: Support SRS in scheduler.
  res.freq_hop.c_srs = 0;
  res.freq_hop.b_srs = 0;
  res.freq_hop.b_hop = 0;
  res.grp_or_seq_hop = srs_config::srs_resource::group_or_sequence_hopping::neither;
  res.res_type       = srs_config::srs_resource::aperiodic;
  res.sequence_id    = params.pci;

  cfg.srs_res_set.emplace_back();
  // TODO: Verify correctness of the config based on what we support.
  srs_config::srs_resource_set& res_set = cfg.srs_res_set.back();
  res_set.id                            = static_cast<srs_config::srs_res_set_id>(0);
  res_set.srs_res_id_list.emplace_back(static_cast<srs_config::srs_res_id>(0));
  res_set.res_type =
      srs_config::srs_resource_set::aperiodic_resource_type{.aperiodic_srs_res_trigger = 1, .slot_offset = 7};
  res_set.srs_res_set_usage = srs_config::srs_resource_set::usage::codebook;
  res_set.p0                = -84;
  res_set.pathloss_ref_rs   = static_cast<ssb_id_t>(0);

  return cfg;
}

uplink_config srsran::config_helpers::make_default_ue_uplink_config(const cell_config_builder_params& params)
{
  // > UL Config.
  uplink_config ul_config{};
  ul_config.init_ul_bwp.pucch_cfg.emplace();

  // >> PUCCH.
  auto& pucch_cfg = ul_config.init_ul_bwp.pucch_cfg.value();

  // PUCCH Resource Set ID 0. This is for PUCCH Format 1 only (Format 0 not yet supported), used for HARQ-ACK only.
  auto& pucch_res_set_0            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_0.pucch_res_set_id = 0;
  pucch_res_set_0.pucch_res_id_list.emplace_back(0);
  pucch_res_set_0.pucch_res_id_list.emplace_back(1);
  pucch_res_set_0.pucch_res_id_list.emplace_back(2);

  // PUCCH Resource Set ID 1. This is for PUCCH Format 2 only and used for HARQ-ACK + optionally SR and/or CSI.
  auto& pucch_res_set_1            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_1.pucch_res_set_id = 1;
  pucch_res_set_1.pucch_res_id_list.emplace_back(3);
  pucch_res_set_1.pucch_res_id_list.emplace_back(4);
  pucch_res_set_1.pucch_res_id_list.emplace_back(5);
  pucch_res_set_1.pucch_res_id_list.emplace_back(6);
  pucch_res_set_1.pucch_res_id_list.emplace_back(7);
  pucch_res_set_1.pucch_res_id_list.emplace_back(8);

  const unsigned nof_rbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz,
      params.scs_common,
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);

  // PUCCH resource format 1, for HARQ-ACK.
  // >>> PUCCH resource 0.
  pucch_resource res_basic{.res_id = 0, .starting_prb = nof_rbs - 1, .format = pucch_format::FORMAT_1};
  res_basic.format_params.emplace<pucch_format_1_cfg>(
      pucch_format_1_cfg{.initial_cyclic_shift = 0, .nof_symbols = 14, .starting_sym_idx = 0, .time_domain_occ = 0});
  pucch_cfg.pucch_res_list.push_back(res_basic);
  // >>> PUCCH resource 1.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res1 = pucch_cfg.pucch_res_list.back();
  res1.res_id          = 1;
  res1.starting_prb    = 1;
  // >>> PUCCH resource 2.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res2 = pucch_cfg.pucch_res_list.back();
  res2.res_id          = 2;
  res2.starting_prb    = nof_rbs - 2;

  // PUCCH resource format 2, for HARQ-ACK + optionally SR and/or CSI.
  // >>> PUCCH resource 3.
  pucch_resource res_basic_f2{.starting_prb = 2, .format = pucch_format::FORMAT_2};
  res_basic_f2.res_id = 3;
  res_basic_f2.format_params.emplace<pucch_format_2_3_cfg>(
      pucch_format_2_3_cfg{.nof_prbs = 1, .nof_symbols = 2, .starting_sym_idx = 0});
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  // >>> PUCCH resource 4.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res4                                                   = pucch_cfg.pucch_res_list.back();
  res4.res_id                                                            = 4;
  variant_get<pucch_format_2_3_cfg>(res4.format_params).starting_sym_idx = 2;
  // >>> PUCCH resource 5.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res5                                                   = pucch_cfg.pucch_res_list.back();
  res5.res_id                                                            = 5;
  variant_get<pucch_format_2_3_cfg>(res5.format_params).starting_sym_idx = 4;
  // >>> PUCCH resource 6.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res6                                                   = pucch_cfg.pucch_res_list.back();
  res6.res_id                                                            = 6;
  variant_get<pucch_format_2_3_cfg>(res6.format_params).starting_sym_idx = 6;
  // >>> PUCCH resource 7.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res7                                                   = pucch_cfg.pucch_res_list.back();
  res7.res_id                                                            = 7;
  variant_get<pucch_format_2_3_cfg>(res7.format_params).starting_sym_idx = 8;
  // >>> PUCCH resource 8.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res8                                                   = pucch_cfg.pucch_res_list.back();
  res8.res_id                                                            = 8;
  variant_get<pucch_format_2_3_cfg>(res8.format_params).starting_sym_idx = 10;

  // PUCCH resource format 2, for CSI and optionally for SR.
  // >>> PUCCH resource 9.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res9                                                   = pucch_cfg.pucch_res_list.back();
  res9.res_id                                                            = 9;
  variant_get<pucch_format_2_3_cfg>(res9.format_params).starting_sym_idx = 12;

  // PUCCH resource format 1, for SR only.
  // >>> PUCCH resource 10.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res10 = pucch_cfg.pucch_res_list.back();
  res10.res_id          = 10;
  res10.starting_prb    = 0;
  res10.second_hop_prb  = nof_rbs - 1;

  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res11 = pucch_cfg.pucch_res_list.back();
  res11.res_id          = 11;
  res11.starting_prb    = nof_rbs - 3;

  // TODO: add more PUCCH resources.

  // >>> SR Resources.
  // Use 40msec SR period by default.
  const unsigned sr_period = get_nof_slots_per_subframe(params.scs_common) * 40;
  pucch_cfg.sr_res_list.push_back(
      scheduling_request_resource_config{.sr_res_id    = 1,
                                         .sr_id        = uint_to_sched_req_id(0),
                                         .period       = (sr_periodicity)sr_period,
                                         .offset       = 0,
                                         .pucch_res_id = (unsigned)pucch_cfg.pucch_res_list.size() - 1U});

  pucch_cfg.format_1_common_param.emplace();
  pucch_cfg.format_2_common_param.emplace(
      pucch_common_all_formats{.max_c_rate = max_pucch_code_rate::dot_25, .simultaneous_harq_ack_csi = true});

  // >>> dl-DataToUl-Ack
  // TS38.213, 9.1.2.1 - "If a UE is provided dl-DataToUL-ACK, the UE does not expect to be indicated by DCI format 1_0
  // a slot timing value for transmission of HARQ-ACK information that does not belong to the intersection of the set
  // of slot timing values {1, 2, 3, 4, 5, 6, 7, 8} and the set of slot timing values provided by dl-DataToUL-ACK for
  // the active DL BWP of a corresponding serving cell.
  // Inactive for format1_0."
  // Note2: Only k1 >= 4 supported.
  if (band_helper::get_duplex_mode(get_band(params)) == duplex_mode::FDD) {
    pucch_cfg.dl_data_to_ul_ack = {SCHEDULER_MIN_K1};
  } else {
    // TDD
    pucch_cfg.dl_data_to_ul_ack = generate_k1_candidates(make_default_tdd_ul_dl_config_common(params));
  }

  // > PUSCH config.
  ul_config.init_ul_bwp.pusch_cfg.emplace(make_default_pusch_config());

  // > SRS config.
  ul_config.init_ul_bwp.srs_cfg.emplace(make_default_srs_config(params));

  return ul_config;
}

pdsch_serving_cell_config srsran::config_helpers::make_default_pdsch_serving_cell_config()
{
  pdsch_serving_cell_config serv_cell;
  serv_cell.nof_harq_proc   = pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n16;
  serv_cell.max_mimo_layers = 1;

  return serv_cell;
}

static csi_helper::csi_builder_params make_default_csi_builder_params(const cell_config_builder_params& params)
{
  // Parameters used to generate list of CSI resources.
  csi_helper::csi_builder_params csi_params{};
  csi_params.pci           = params.pci;
  csi_params.nof_rbs       = cell_nof_crbs(params);
  csi_params.nof_ports     = params.nof_dl_ports;
  csi_params.csi_rs_period = csi_helper::get_max_csi_rs_period(params.scs_common);
  return csi_params;
}

pdsch_config srsran::config_helpers::make_default_pdsch_config(const cell_config_builder_params& params)
{
  pdsch_config pdsch_cfg;
  pdsch_cfg.pdsch_mapping_type_a_dmrs.emplace();
  dmrs_downlink_config& dmrs_type_a = pdsch_cfg.pdsch_mapping_type_a_dmrs.value();
  dmrs_type_a.additional_positions  = dmrs_additional_positions::pos1;
  pdsch_cfg.tci_states.push_back(tci_state{
      .state_id  = static_cast<tci_state_id_t>(0),
      .qcl_type1 = {.ref_sig  = {.type = qcl_info::reference_signal::reference_signal_type::ssb,
                                 .ssb  = static_cast<ssb_id_t>(0)},
                    .qcl_type = qcl_info::qcl_type::type_d},
  });
  pdsch_cfg.res_alloc = pdsch_config::resource_allocation::resource_allocation_type_1;
  pdsch_cfg.rbg_sz    = rbg_size::config1;
  pdsch_cfg.prb_bndlg.bundling.emplace<prb_bundling::static_bundling>(
      prb_bundling::static_bundling({.sz = prb_bundling::static_bundling::bundling_size::wideband}));

  if (params.csi_rs_enabled) {
    const csi_helper::csi_builder_params csi_params = make_default_csi_builder_params(params);

    // zp-CSI-RS resources.
    pdsch_cfg.zp_csi_rs_res_list = csi_helper::make_periodic_zp_csi_rs_resource_list(csi_params);

    // periodic set of zp-CSI-RS resources.
    pdsch_cfg.p_zp_csi_rs_res = csi_helper::make_periodic_zp_csi_rs_resource_set(csi_params);
  }

  return pdsch_cfg;
}

serving_cell_config
srsran::config_helpers::create_default_initial_ue_serving_cell_config(const cell_config_builder_params& params)
{
  serving_cell_config serv_cell;
  serv_cell.cell_index = to_du_cell_index(0);

  // > PDCCH-Config.
  serv_cell.init_dl_bwp.pdcch_cfg.emplace();
  pdcch_config& pdcch_cfg = serv_cell.init_dl_bwp.pdcch_cfg.value();
  // >> Add CORESET#1.
  pdcch_cfg.coresets.push_back(make_default_coreset_config(params));
  pdcch_cfg.coresets[0].id = to_coreset_id(1);
  // >> Add SearchSpace#2.
  pdcch_cfg.search_spaces.push_back(make_default_ue_search_space_config());
  pdcch_cfg.search_spaces[0].set_non_ss0_nof_candidates(
      {0, 0, compute_max_nof_candidates(aggregation_level::n4, pdcch_cfg.coresets[0]), 0, 0});

  // > PDSCH-Config.
  serv_cell.init_dl_bwp.pdsch_cfg = make_default_pdsch_config(params);

  // > UL Config.
  serv_cell.ul_config.emplace(make_default_ue_uplink_config(params));

  // > pdsch-ServingCellConfig.
  serv_cell.pdsch_serv_cell_cfg.emplace(make_default_pdsch_serving_cell_config());

  // > CSI-MeasConfig.
  if (params.csi_rs_enabled) {
    // Parameters used to generate list of CSI resources.
    const csi_helper::csi_builder_params csi_params = make_default_csi_builder_params(params);

    // Generate CSI resources.
    serv_cell.csi_meas_cfg = csi_helper::make_csi_meas_config(csi_params);
  }

  return serv_cell;
}

cell_config_dedicated
srsran::config_helpers::create_default_initial_ue_spcell_cell_config(const cell_config_builder_params& params)
{
  cell_config_dedicated cfg;
  cfg.serv_cell_idx = to_serv_cell_index(0);
  cfg.serv_cell_cfg = create_default_initial_ue_serving_cell_config(params);
  return cfg;
}

uint8_t srsran::config_helpers::compute_max_nof_candidates(aggregation_level            aggr_lvl,
                                                           const coreset_configuration& cs_cfg)
{
  // 1 REG = 1 RB and 1 symbol.
  // 1 CCE = 6 {PRB, symbol}. e.g. 3 PRBs over 2 symbols or 6 PRBs over 1 symbol, etc.
  // Example: 3 Frequency domain resources, 2 symbol duration contains 6 CCEs.
  const unsigned max_coreset_cces   = cs_cfg.freq_domain_resources().count() * cs_cfg.duration;
  unsigned       max_nof_candidates = max_coreset_cces / to_nof_cces(aggr_lvl);
  // See TS 38.331, SearchSpace IE.
  // aggregationLevelX - ENUMERATED {n0, n1, n2, n3, n4, n5, n6, n8}.
  if (max_nof_candidates == 7) {
    max_nof_candidates = 6;
  }
  return max_nof_candidates > PDCCH_MAX_NOF_CANDIDATES_SS ? PDCCH_MAX_NOF_CANDIDATES_SS : max_nof_candidates;
}

std::vector<pdsch_time_domain_resource_allocation>
srsran::config_helpers::make_pdsch_time_domain_resource(uint8_t                           ss0_idx,
                                                        const pdcch_config_common&        common_pdcch_cfg,
                                                        optional<pdcch_config>            ded_pdcch_cfg,
                                                        optional<tdd_ul_dl_config_common> tdd_cfg)
{
  const optional<coreset_configuration> coreset0                                = common_pdcch_cfg.coreset0;
  const optional<coreset_configuration> common_coreset                          = common_pdcch_cfg.common_coreset;
  unsigned                              pattern1_nof_dl_symbols_in_special_slot = 0;
  unsigned                              pattern2_nof_dl_symbols_in_special_slot = 0;
  if (tdd_cfg.has_value()) {
    pattern1_nof_dl_symbols_in_special_slot = tdd_cfg->pattern1.nof_dl_symbols;
    if (tdd_cfg->pattern2.has_value()) {
      pattern2_nof_dl_symbols_in_special_slot = tdd_cfg->pattern2->nof_dl_symbols;
    }
  }
  std::set<ofdm_symbol_range> pdsch_symbols;

  // See TS 38.214, Table 5.1.2.1-1: Valid S and L combinations.
  static const unsigned pdsch_mapping_typeA_min_L_value = 3;

  // TODO: Consider SearchSpace periodicity while generating PDSCH OFDM symbol range. If there is no PDCCH, there is no
  //  PDSCH since we dont support k0 != 0 yet.

  // NOTE1: Number of DL PDSCH symbols must be atleast greater than SearchSpace monitoring symbol index + CORESET
  // duration for PDSCH allocation in partial slot. Otherwise, it can be used only for UL PDCCH allocations.
  // NOTE2: We don't support multiple monitoring occasions in a slot belonging to a SearchSpace.
  // NOTE3: It is assumed that a validator has ensured that a CORESET exists corresponding to the CORESET Id set in
  // SearchSpace configuration.
  optional<unsigned> cs_duration;
  unsigned           ss_start_symbol_idx;
  // SearchSpaces in Common PDCCH configuration.
  for (const search_space_configuration& ss_cfg : common_pdcch_cfg.search_spaces) {
    cs_duration         = {};
    ss_start_symbol_idx = 0;
    if (coreset0.has_value()) {
      if (ss_cfg.is_search_space0() and ss_cfg.get_coreset_id() == coreset0->id) {
        cs_duration = coreset0->duration;
        // Fetch SearchSpace#0 configuration.
        const pdcch_type0_css_occasion_pattern1_description ss0_occasion =
            pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
                .is_fr2 = false, .ss_zero_index = ss0_idx, .nof_symb_coreset = cs_duration.value()});
        // Consider the starting index of last PDCCH monitoring occasion to account for all SSB beams.
        ss_start_symbol_idx = *std::max_element(ss0_occasion.start_symbol.begin(), ss0_occasion.start_symbol.end());
      } else if (not ss_cfg.is_search_space0() and ss_cfg.get_coreset_id() == coreset0->id) {
        cs_duration         = coreset0->duration;
        ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
      }
    }
    if (common_coreset.has_value()) {
      if (ss_cfg.is_search_space0() and ss_cfg.get_coreset_id() == common_coreset->id) {
        cs_duration = common_coreset->duration;
        // Fetch SearchSpace#0 configuration.
        const pdcch_type0_css_occasion_pattern1_description ss0_occasion =
            pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
                .is_fr2 = false, .ss_zero_index = ss0_idx, .nof_symb_coreset = cs_duration.value()});
        // Consider the starting index of last PDCCH monitoring occasion to account for all SSB beams.
        ss_start_symbol_idx = *std::max_element(ss0_occasion.start_symbol.begin(), ss0_occasion.start_symbol.end());
      } else if (not ss_cfg.is_search_space0() and ss_cfg.get_coreset_id() == common_coreset->id) {
        cs_duration         = common_coreset->duration;
        ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
      }
    }

    // For slots with all DL symbols. i.e. full DL slot.
    pdsch_symbols.insert({ss_start_symbol_idx + cs_duration.value(), NOF_OFDM_SYM_PER_SLOT_NORMAL_CP});
    // For special slots with DL symbols in TDD pattern 1 if configured.
    if (pattern1_nof_dl_symbols_in_special_slot > 0 and
        (pattern1_nof_dl_symbols_in_special_slot > (ss_start_symbol_idx + cs_duration.value())) and
        (pattern1_nof_dl_symbols_in_special_slot - ss_start_symbol_idx - cs_duration.value()) >=
            pdsch_mapping_typeA_min_L_value) {
      pdsch_symbols.insert({ss_start_symbol_idx + cs_duration.value(), pattern1_nof_dl_symbols_in_special_slot});
    }
    // For special slots with DL symbols in TDD pattern 2 if configured.
    if (pattern2_nof_dl_symbols_in_special_slot > 0 and
        (pattern2_nof_dl_symbols_in_special_slot > (ss_start_symbol_idx + cs_duration.value())) and
        (pattern2_nof_dl_symbols_in_special_slot - ss_start_symbol_idx - cs_duration.value()) >=
            pdsch_mapping_typeA_min_L_value) {
      pdsch_symbols.insert({ss_start_symbol_idx + cs_duration.value(), pattern2_nof_dl_symbols_in_special_slot});
    }
  }
  // SearchSpaces in Dedicated PDCCH configuration.
  if (ded_pdcch_cfg.has_value()) {
    for (const search_space_configuration& ss_cfg : ded_pdcch_cfg->search_spaces) {
      cs_duration         = {};
      ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
      if (coreset0.has_value()) {
        if (ss_cfg.get_coreset_id() == coreset0->id) {
          cs_duration = coreset0->duration;
        }
      }
      if ((not cs_duration.has_value()) and common_coreset.has_value()) {
        if (ss_cfg.get_coreset_id() == common_coreset->id) {
          cs_duration = common_coreset->duration;
        }
      }

      if (not cs_duration.has_value()) {
        for (const coreset_configuration& cs_cfg : ded_pdcch_cfg->coresets) {
          if (ss_cfg.get_coreset_id() == cs_cfg.id) {
            cs_duration = cs_cfg.duration;
            break;
          }
        }
      }

      // For slots with all DL symbols. i.e. full DL slot.
      pdsch_symbols.insert({ss_start_symbol_idx + cs_duration.value(), NOF_OFDM_SYM_PER_SLOT_NORMAL_CP});
      // For special slots with DL symbols in TDD pattern 1 if configured.
      if (pattern1_nof_dl_symbols_in_special_slot > 0 and
          (pattern1_nof_dl_symbols_in_special_slot > (ss_start_symbol_idx + cs_duration.value())) and
          (pattern1_nof_dl_symbols_in_special_slot - ss_start_symbol_idx - cs_duration.value()) >=
              pdsch_mapping_typeA_min_L_value) {
        pdsch_symbols.insert({ss_start_symbol_idx + cs_duration.value(), pattern1_nof_dl_symbols_in_special_slot});
      }
      // For special slots with DL symbols in TDD pattern 2 if configured.
      if (pattern2_nof_dl_symbols_in_special_slot > 0 and
          (pattern2_nof_dl_symbols_in_special_slot > (ss_start_symbol_idx + cs_duration.value())) and
          (pattern2_nof_dl_symbols_in_special_slot - ss_start_symbol_idx - cs_duration.value()) >=
              pdsch_mapping_typeA_min_L_value) {
        pdsch_symbols.insert({ss_start_symbol_idx + cs_duration.value(), pattern2_nof_dl_symbols_in_special_slot});
      }
    }
  }

  // Make PDSCH time domain resource allocation.
  std::vector<pdsch_time_domain_resource_allocation> result;
  for (const auto& symbs : pdsch_symbols) {
    result.push_back(
        pdsch_time_domain_resource_allocation{.k0 = 0, .map_type = sch_mapping_type::typeA, .symbols = symbs});
  }

  // Sort PDSCH time domain resource allocations in descending order of OFDM symbol range length to always choose
  // the resource which occupies most of the DL symbols in a slot.
  std::sort(result.begin(),
            result.end(),
            [](const pdsch_time_domain_resource_allocation& res_alloc_a,
               const pdsch_time_domain_resource_allocation& res_alloc_b) {
              return res_alloc_a.symbols.length() > res_alloc_b.symbols.length();
            });

  return result;
}

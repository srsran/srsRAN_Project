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

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

struct cu_cp_periodicity_and_offset {
  optional<uint8_t> sf5;
  optional<uint8_t> sf10;
  optional<uint8_t> sf20;
  optional<uint8_t> sf40;
  optional<uint8_t> sf80;
  optional<uint8_t> sf160;
};

struct cu_cp_ssb_mtc {
  cu_cp_periodicity_and_offset periodicity_and_offset;
  uint8_t                      dur;
};

struct cu_cp_ssb_mtc2 {
  std::vector<pci_t> pci_list;
  uint8_t            periodicity;
};

struct cu_cp_ssb_to_measure {
  optional<uint8_t>  short_bitmap;
  optional<uint8_t>  medium_bitmap;
  optional<uint64_t> long_bitmap;
};

struct cu_cp_ssb_to_measure_setup_release {
  bool                           is_release;
  optional<cu_cp_ssb_to_measure> setup;
};

struct cu_cp_ss_rssi_meas {
  uint64_t meas_slots;
  uint8_t  end_symbol;
};

struct cu_cp_ssb_cfg_mob {
  optional<cu_cp_ssb_to_measure_setup_release> ssb_to_measure;
  bool                                         derive_ssb_idx_from_cell;
  optional<cu_cp_ss_rssi_meas>                 ss_rssi_meas;
};

struct cu_cp_csi_rs_meas_bw {
  uint16_t nrof_prbs;
  uint16_t start_prb;
};

struct cu_cp_slot_cfg {
  optional<uint8_t>  ms4;
  optional<uint8_t>  ms5;
  optional<uint8_t>  ms10;
  optional<uint8_t>  ms20;
  optional<uint16_t> ms40;
};

struct cu_cp_associated_ssb {
  uint8_t ssb_idx;
  bool    is_quasi_colocated;
};

struct cu_cp_freq_domain_alloc {
  optional<uint8_t>  row1;
  optional<uint16_t> row2;
};

struct cu_cp_csi_rs_res_mob {
  uint8_t                        csi_rs_idx;
  cu_cp_slot_cfg                 slot_cfg;
  optional<cu_cp_associated_ssb> associated_ssb;
  cu_cp_freq_domain_alloc        freq_domain_alloc;
  uint8_t                        first_ofdm_symbol_in_time_domain;
  uint16_t                       seq_generation_cfg;
};

struct cu_cp_csi_rs_cell_mob {
  uint16_t                          cell_id;
  cu_cp_csi_rs_meas_bw              csi_rs_meas_bw;
  optional<uint8_t>                 density;
  std::vector<cu_cp_csi_rs_res_mob> csi_rs_res_list_mob;
};

struct cu_cp_csi_rs_res_cfg_mob {
  subcarrier_spacing                 sc_spacing;
  std::vector<cu_cp_csi_rs_cell_mob> csi_rs_cell_list_mob;
};

struct cu_cp_csi_rs_res_cfg_mob_setup_release {
  bool                               is_release;
  optional<cu_cp_csi_rs_res_cfg_mob> setup;
};

struct cu_cp_ref_sig_cfg {
  optional<cu_cp_ssb_cfg_mob>                      ssb_cfg_mob;
  optional<cu_cp_csi_rs_res_cfg_mob_setup_release> csi_rs_res_cfg_mob;
};

struct cu_cp_thres_nr {
  optional<uint8_t> thres_rsrp;
  optional<uint8_t> thres_rsrq;
  optional<uint8_t> thres_sinr;
};

struct cu_cp_q_offset_range_list {
  optional<int8_t> rsrp_offset_ssb;
  optional<int8_t> rsrq_offset_ssb;
  optional<int8_t> sinr_offset_ssb;
  optional<int8_t> rsrp_offset_csi_rs;
  optional<int8_t> rsrq_offset_csi_rs;
  optional<int8_t> sinr_offset_csi_rs;
};

struct cu_cp_cells_to_add_mod {
  pci_t                     pci;
  cu_cp_q_offset_range_list cell_individual_offset;
};

struct cu_cp_pci_range {
  uint16_t           start;
  optional<uint16_t> range;
};

struct cu_cp_pci_range_elem {
  uint8_t         pci_range_idx;
  cu_cp_pci_range pci_range;
};

struct cu_cp_meas_obj_nr {
  optional<uint32_t>                  ssb_freq;
  optional<subcarrier_spacing>        ssb_subcarrier_spacing;
  optional<cu_cp_ssb_mtc>             smtc1;
  optional<cu_cp_ssb_mtc2>            smtc2;
  optional<uint32_t>                  ref_freq_csi_rs;
  cu_cp_ref_sig_cfg                   ref_sig_cfg;
  optional<cu_cp_thres_nr>            abs_thresh_ss_blocks_consolidation;
  optional<cu_cp_thres_nr>            abs_thresh_csi_rs_consolidation;
  optional<uint8_t>                   nrof_ss_blocks_to_average;
  optional<uint8_t>                   nrof_csi_rs_res_to_average;
  uint8_t                             quant_cfg_idx;
  cu_cp_q_offset_range_list           offset_mo;
  std::vector<pci_t>                  cells_to_rem_list; // max size = 32
  std::vector<cu_cp_cells_to_add_mod> cells_to_add_mod_list;
  std::vector<uint8_t>                excluded_cells_to_rem_list; // max size = 8
  std::vector<cu_cp_pci_range_elem>   excluded_cells_to_add_mod_list;
  std::vector<uint8_t>                allowed_cells_to_rem_list; // max size = 8
  std::vector<cu_cp_pci_range_elem>   allowed_cells_to_add_mod_list;
  // group 0
  optional<uint16_t> freq_band_ind_nr;
  optional<uint16_t> meas_cycle_scell;
};

struct cu_cp_n2 {
  uint8_t comb_offset_n2;
  uint8_t cyclic_shift_n2;
};

struct cu_cp_n4 {
  uint8_t comb_offset_n4;
  uint8_t cyclic_shift_n4;
};

struct cu_cp_tx_comb {
  optional<cu_cp_n2> n2;
  optional<cu_cp_n4> n4;
};

struct cu_cp_res_map {
  uint8_t start_position;
  uint8_t nrof_symbols;
  uint8_t repeat_factor;
};

struct cu_cp_freq_hop {
  uint8_t c_srs;
  uint8_t b_srs;
  uint8_t b_hop;
};

struct cu_cp_srs_periodicity_and_offset {
  bool               is_sl1;
  optional<uint8_t>  sl2;
  optional<uint8_t>  sl4;
  optional<uint8_t>  sl5;
  optional<uint8_t>  sl8;
  optional<uint8_t>  sl10;
  optional<uint8_t>  sl16;
  optional<uint8_t>  sl20;
  optional<uint8_t>  sl32;
  optional<uint8_t>  sl40;
  optional<uint8_t>  sl64;
  optional<uint8_t>  sl80;
  optional<uint8_t>  sl160;
  optional<uint16_t> sl320;
  optional<uint16_t> sl640;
  optional<uint16_t> sl1280;
  optional<uint16_t> sl2560;
};

struct cu_cp_semi_persistent_periodic {
  cu_cp_srs_periodicity_and_offset periodicity_and_offset_sp_p;
};

struct cu_cp_res_type {
  bool                                     is_aperiodic;
  optional<cu_cp_semi_persistent_periodic> semi_persistent;
  optional<cu_cp_semi_persistent_periodic> periodic;
};

struct cu_cp_srs {
  uint8_t res_id;
  uint8_t ul_bwp;
};

struct cu_cp_srs_spatial_relation_info {
  struct cu_cp_ref_sig_ {
    optional<uint8_t>   ssb_idx;
    optional<uint8_t>   csi_rs_idx;
    optional<cu_cp_srs> srs;
  };

  optional<uint8_t> serving_cell_id;
  cu_cp_ref_sig_    ref_sig;
};

struct cu_cp_srs_res {
  uint8_t                                   srs_res_id;
  uint8_t                                   nrof_srs_ports;
  optional<uint8_t>                         ptrs_port_idx;
  cu_cp_tx_comb                             tx_comb;
  cu_cp_res_map                             res_map;
  uint8_t                                   freq_domain_position;
  uint16_t                                  freq_domain_shift;
  cu_cp_freq_hop                            freq_hop;
  std::string                               group_or_seq_hop;
  cu_cp_res_type                            res_type;
  uint16_t                                  seq_id;
  optional<cu_cp_srs_spatial_relation_info> spatial_relation_info;
};

struct cu_cp_meas_obj_to_add_mod {
  uint8_t                     meas_obj_id;
  optional<cu_cp_meas_obj_nr> meas_obj_nr;
};

struct cu_cp_meas_report_quant {
  bool rsrp;
  bool rsrq;
  bool sinr;
};

struct cu_cp_periodical_report_cfg {
  std::string                       rs_type;
  uint16_t                          report_interv;
  int8_t                            report_amount;
  cu_cp_meas_report_quant           report_quant_cell;
  uint8_t                           max_report_cells;
  optional<cu_cp_meas_report_quant> report_quant_rs_idxes;
  optional<uint8_t>                 max_nrof_rs_idxes_to_report;
  bool                              include_beam_meass;
  bool                              use_allowed_cell_list;
};

struct cu_cp_meas_trigger_quant {
  optional<uint8_t> rsrp;
  optional<uint8_t> rsrq;
  optional<uint8_t> sinr;
};

struct cu_cp_event_a1 {
  cu_cp_meas_trigger_quant a1_thres;
  bool                     report_on_leave;
  uint8_t                  hysteresis;
  uint16_t                 time_to_trigger;
};

struct cu_cp_event_a2 {
  cu_cp_meas_trigger_quant a2_thres;
  bool                     report_on_leave;
  uint8_t                  hysteresis;
  uint16_t                 time_to_trigger;
};

using cu_cp_meas_trigger_quant_offset = cu_cp_meas_trigger_quant;

struct cu_cp_event_a3 {
  cu_cp_meas_trigger_quant_offset a3_offset;
  bool                            report_on_leave;
  uint8_t                         hysteresis;
  uint16_t                        time_to_trigger;
  bool                            use_allowed_cell_list;
};

struct cu_cp_event_a4 {
  cu_cp_meas_trigger_quant a4_thres;
  bool                     report_on_leave;
  uint8_t                  hysteresis;
  uint16_t                 time_to_trigger;
  bool                     use_allowed_cell_list;
};

struct cu_cp_event_a5 {
  cu_cp_meas_trigger_quant a5_thres_1;
  cu_cp_meas_trigger_quant a5_thres_2;
  bool                     report_on_leave;
  uint8_t                  hysteresis;
  uint16_t                 time_to_trigger;
  bool                     use_allowed_cell_list;
};

struct cu_cp_event_a6 {
  cu_cp_meas_trigger_quant_offset a6_offset;
  bool                            report_on_leave;
  uint8_t                         hysteresis;
  uint16_t                        time_to_trigger;
  bool                            use_allowed_cell_list;
};

struct cu_cp_event_id {
  optional<cu_cp_event_a1> event_a1;
  optional<cu_cp_event_a2> event_a2;
  optional<cu_cp_event_a3> event_a3;
  optional<cu_cp_event_a4> event_a4;
  optional<cu_cp_event_a5> event_a5;
  optional<cu_cp_event_a6> event_a6;
};

struct cu_cp_event_trigger_cfg {
  bool                              report_add_neigh_meas_present;
  cu_cp_event_id                    event_id;
  std::string                       rs_type;
  uint16_t                          report_interv;
  int8_t                            report_amount;
  cu_cp_meas_report_quant           report_quant_cell;
  uint8_t                           max_report_cells;
  optional<cu_cp_meas_report_quant> report_quant_rs_idxes;
  optional<uint8_t>                 max_nrof_rs_idxes_to_report;
  bool                              include_beam_meass;
};

struct cu_cp_report_cgi {
  uint16_t cell_for_which_to_report_cgi;
};

struct cu_cp_report_sftd_nr {
  bool report_sftd_meas;
  bool report_rsrp;
};

struct cu_cp_cond_event_a3 {
  cu_cp_meas_trigger_quant_offset a3_offset;
  uint8_t                         hysteresis;
  uint16_t                        time_to_trigger;
};

struct cu_cp_cond_event_a5 {
  cu_cp_meas_trigger_quant a5_thres1;
  cu_cp_meas_trigger_quant a5_thres2;
  uint8_t                  hysteresis;
  uint16_t                 time_to_trigger;
};

struct cu_cp_report_cfg_nr {
  optional<cu_cp_periodical_report_cfg> periodical;
  optional<cu_cp_event_trigger_cfg>     event_triggered;
  optional<cu_cp_report_cgi>            report_cgi;
  optional<cu_cp_report_sftd_nr>        report_sftd;
};

struct cu_cp_report_cfg {
  optional<cu_cp_report_cfg_nr> report_cfg_nr;
};

struct cu_cp_report_cfg_to_add_mod {
  uint8_t          report_cfg_id;
  cu_cp_report_cfg report_cfg;
};

struct cu_cp_meas_id_to_add_mod {
  uint8_t meas_id;
  uint8_t meas_obj_id;
  uint8_t report_cfg_id;
};

struct cu_cp_s_measure_cfg {
  optional<uint8_t> ssb_rsrp;
  optional<uint8_t> csi_rsrp;
};

struct cu_cp_filt_cfg {
  optional<uint8_t> filt_coef_rsrp;
  optional<uint8_t> filt_coef_rsrq;
  optional<uint8_t> filt_coef_rs_sinr;
};

struct cu_cp_quant_cfg_rs {
  cu_cp_filt_cfg ssb_filt_cfg;
  cu_cp_filt_cfg csi_rs_filt_cfg;
};

struct cu_cp_quant_cfg_nr {
  cu_cp_quant_cfg_rs           quant_cfg_cell;
  optional<cu_cp_quant_cfg_rs> quant_cfg_rs_idx;
};

struct cu_cp_quant_cfg {
  std::vector<cu_cp_quant_cfg_nr> quant_cfg_nr_list;
};

struct cu_cp_gap_cfg {
  uint8_t gap_offset;
  uint8_t mgl;
  uint8_t mgrp;
  float   mgta;
};

struct cu_cp_gap_cfg_setup_release {
  bool                    is_release;
  optional<cu_cp_gap_cfg> setup;
};

struct cu_cp_measg_gap_cfg {
  optional<cu_cp_gap_cfg_setup_release> gap_fr2;
};

struct cu_cp_meas_gap_sharing_scheme_setup_release {
  bool                  is_release;
  optional<std::string> setup;
};

struct cu_cp_meas_gap_sharing_cfg {
  optional<cu_cp_meas_gap_sharing_scheme_setup_release> gap_sharing_fr2;
};

struct cu_cp_meas_cfg {
  std::vector<uint8_t>                     meas_obj_to_rem_list;
  std::vector<cu_cp_meas_obj_to_add_mod>   meas_obj_to_add_mod_list;
  std::vector<uint8_t>                     report_cfg_to_rem_list;
  std::vector<cu_cp_report_cfg_to_add_mod> report_cfg_to_add_mod_list;
  std::vector<uint8_t>                     meas_id_to_rem_list;
  std::vector<cu_cp_meas_id_to_add_mod>    meas_id_to_add_mod_list;
  optional<cu_cp_s_measure_cfg>            s_measure_cfg;
  optional<cu_cp_quant_cfg>                quant_cfg;
  optional<cu_cp_measg_gap_cfg>            meas_gap_cfg;
  optional<cu_cp_meas_gap_sharing_cfg>     meas_gap_sharing_cfg;
};

struct cu_cp_meas_quant_results {
  optional<uint8_t> rsrp;
  optional<uint8_t> rsrq;
  optional<uint8_t> sinr;
};

struct cu_cp_results_per_ssb_idx {
  uint8_t                            ssb_idx;
  optional<cu_cp_meas_quant_results> ssb_results;
};

struct cu_cp_results_per_csi_rs_idx {
  uint8_t                            csi_rs_idx;
  optional<cu_cp_meas_quant_results> csi_rs_results;
};

struct cu_cp_meas_result_nr {
  struct cell_results_ {
    optional<cu_cp_meas_quant_results> results_ssb_cell;
    optional<cu_cp_meas_quant_results> results_csi_rs_cell;
  };

  struct rs_idx_results_ {
    slotted_id_vector<uint8_t, cu_cp_results_per_ssb_idx>    results_ssb_idxes;    // indexed by ssb_idx
    slotted_id_vector<uint8_t, cu_cp_results_per_csi_rs_idx> results_csi_rs_idxes; // indexed by csi_rs_idx
  };

  optional<pci_t>           pci;
  cell_results_             cell_results;
  optional<rs_idx_results_> rs_idx_results;
};

struct cu_cp_meas_result_serv_mo {
  uint8_t                        serv_cell_id;
  cu_cp_meas_result_nr           meas_result_serving_cell;
  optional<cu_cp_meas_result_nr> meas_result_best_neigh_cell;
};

struct cu_cp_meas_result_neigh_cells {
  // choice
  std::vector<cu_cp_meas_result_nr> meas_result_list_nr;
};

struct cu_cp_meas_results {
  uint8_t                                               meas_id;
  slotted_id_vector<uint8_t, cu_cp_meas_result_serv_mo> meas_result_serving_mo_list; // indexed by serv_cell_id
  optional<cu_cp_meas_result_neigh_cells>               meas_result_neigh_cells;
};

} // namespace srs_cu_cp
} // namespace srsran

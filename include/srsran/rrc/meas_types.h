/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <string>
#include <variant>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// Maximum number of measurements.
const uint8_t MAX_NOF_MEAS = 64;

/// Meas index.
enum class meas_id_t : uint8_t { min = 1, max = MAX_NOF_MEAS, invalid = MAX_NOF_MEAS + 1 };

/// Convert meas_id type to integer.
inline uint8_t meas_id_to_uint(meas_id_t id)
{
  return static_cast<uint8_t>(id);
}

/// Convert integer to meas_id type.
inline meas_id_t uint_to_meas_id(uint8_t id)
{
  return static_cast<meas_id_t>(id);
}

/// Maximum number of measurement objects.
const uint8_t MAX_NOF_MEAS_OBJ = 64;

/// Meas object index.
enum class meas_obj_id_t : uint8_t { min = 1, max = MAX_NOF_MEAS_OBJ, invalid = MAX_NOF_MEAS_OBJ + 1 };

/// Convert meas_obj_id type to integer.
inline uint8_t meas_obj_id_to_uint(meas_obj_id_t id)
{
  return static_cast<uint8_t>(id);
}

/// Convert integer to meas_obj_id type.
inline meas_obj_id_t uint_to_meas_obj_id(uint8_t id)
{
  return static_cast<meas_obj_id_t>(id);
}

/// Maximum number of report configs.
const uint8_t MAX_NOF_REPORT_CFG = 64;

/// Report config index.
enum class report_cfg_id_t : uint8_t { min = 0, max = MAX_NOF_REPORT_CFG - 1, invalid = MAX_NOF_REPORT_CFG };

/// Convert report_cfg_id type to integer.
inline uint8_t report_cfg_id_to_uint(report_cfg_id_t id)
{
  return static_cast<uint8_t>(id);
}

/// Convert integer to report_cfg_id type.
inline report_cfg_id_t uint_to_report_cfg_id(uint8_t id)
{
  return static_cast<report_cfg_id_t>(id);
}

struct rrc_periodicity_and_offset {
  enum class periodicity_t : uint8_t { sf5 = 5, sf10 = 10, sf20 = 20, sf40 = 40, sf80 = 80, sf160 = 160 };
  periodicity_t periodicity;
  uint8_t       offset;
};

inline bool operator==(const rrc_periodicity_and_offset& lhs, const rrc_periodicity_and_offset& rhs)
{
  return lhs.periodicity == rhs.periodicity && lhs.offset == rhs.offset;
}

struct rrc_ssb_mtc {
  rrc_periodicity_and_offset periodicity_and_offset;
  uint8_t                    dur;
};

inline bool operator==(const rrc_ssb_mtc& lhs, const rrc_ssb_mtc& rhs)
{
  return lhs.periodicity_and_offset == rhs.periodicity_and_offset && lhs.dur == rhs.dur;
}

struct rrc_ss_rssi_meas {
  uint64_t meas_slots;
  uint8_t  end_symbol;
};

struct rrc_meas_timing {
  struct rrc_freq_and_timing_ {
    uint32_t                        carrier_freq;
    subcarrier_spacing              ssb_subcarrier_spacing;
    rrc_ssb_mtc                     ssb_meas_timing_cfg;
    std::optional<rrc_ss_rssi_meas> ss_rssi_meas;
  };

  std::optional<rrc_freq_and_timing_> freq_and_timing;
};

struct rrc_ssb_mtc2 {
  std::vector<pci_t> pci_list;
  uint8_t            periodicity;
};

inline bool operator==(const rrc_ssb_mtc2& lhs, const rrc_ssb_mtc2& rhs)
{
  return lhs.periodicity == rhs.periodicity && lhs.pci_list == rhs.pci_list;
}

struct rrc_ssb_to_measure {
  enum class bitmap_type_t : uint8_t { /* uint8_t */ short_bitmap,
                                       /* uint8_t */ medium_bitmap,
                                       /* uint64_t */ long_bitmap };

  bitmap_type_t type;
  uint64_t      bitmap;
};

struct rrc_ssb_to_measure_setup_release {
  bool                              is_release;
  std::optional<rrc_ssb_to_measure> setup;
};

struct rrc_ssb_cfg_mob {
  std::optional<rrc_ssb_to_measure_setup_release> ssb_to_measure;
  bool                                            derive_ssb_idx_from_cell;
  std::optional<rrc_ss_rssi_meas>                 ss_rssi_meas;
};

struct rrc_csi_rs_meas_bw {
  uint16_t nrof_prbs;
  uint16_t start_prb;
};

struct rrc_slot_cfg {
  enum class period_t : uint8_t { ms4, ms5, ms10, ms20, ms40 };

  period_t period;
  uint16_t offset;
};

struct rrc_associated_ssb {
  uint8_t ssb_idx;
  bool    is_quasi_colocated;
};

using rrc_freq_domain_alloc_row1 = uint8_t;
using rrc_freq_domain_alloc_row2 = uint16_t;

using rrc_freq_domain_alloc = std::variant<rrc_freq_domain_alloc_row1, rrc_freq_domain_alloc_row2>;

struct rrc_csi_rs_res_mob {
  uint8_t                           csi_rs_idx;
  rrc_slot_cfg                      slot_cfg;
  std::optional<rrc_associated_ssb> associated_ssb;
  rrc_freq_domain_alloc             freq_domain_alloc;
  uint8_t                           first_ofdm_symbol_in_time_domain;
  uint16_t                          seq_generation_cfg;
};

struct rrc_csi_rs_cell_mob {
  uint16_t                        cell_id;
  rrc_csi_rs_meas_bw              csi_rs_meas_bw;
  std::optional<uint8_t>          density;
  std::vector<rrc_csi_rs_res_mob> csi_rs_res_list_mob;
};

struct rrc_csi_rs_res_cfg_mob {
  subcarrier_spacing               sc_spacing;
  std::vector<rrc_csi_rs_cell_mob> csi_rs_cell_list_mob;
};

struct rrc_csi_rs_res_cfg_mob_setup_release {
  bool                                  is_release;
  std::optional<rrc_csi_rs_res_cfg_mob> setup;
};

struct rrc_ref_sig_cfg {
  std::optional<rrc_ssb_cfg_mob>                      ssb_cfg_mob;
  std::optional<rrc_csi_rs_res_cfg_mob_setup_release> csi_rs_res_cfg_mob;
};

struct rrc_thres_nr {
  std::optional<uint8_t> thres_rsrp;
  std::optional<uint8_t> thres_rsrq;
  std::optional<uint8_t> thres_sinr;
};

struct rrc_q_offset_range_list {
  std::optional<int8_t> rsrp_offset_ssb;
  std::optional<int8_t> rsrq_offset_ssb;
  std::optional<int8_t> sinr_offset_ssb;
  std::optional<int8_t> rsrp_offset_csi_rs;
  std::optional<int8_t> rsrq_offset_csi_rs;
  std::optional<int8_t> sinr_offset_csi_rs;
};

struct rrc_cells_to_add_mod {
  pci_t                   pci;
  rrc_q_offset_range_list cell_individual_offset;
};

struct rrc_pci_range {
  uint16_t                start;
  std::optional<uint16_t> range;
};

struct rrc_pci_range_elem {
  uint8_t       pci_range_idx;
  rrc_pci_range pci_range;
};

using ssb_frequency_t = uint32_t;

struct rrc_meas_obj_nr {
  std::optional<ssb_frequency_t>    ssb_freq; // SSB ARFCN value
  std::optional<subcarrier_spacing> ssb_subcarrier_spacing;
  std::optional<rrc_ssb_mtc>        smtc1;
  std::optional<rrc_ssb_mtc2>       smtc2;
  std::optional<uint32_t>           ref_freq_csi_rs;
  rrc_ref_sig_cfg                   ref_sig_cfg;
  std::optional<rrc_thres_nr>       abs_thresh_ss_blocks_consolidation;
  std::optional<rrc_thres_nr>       abs_thresh_csi_rs_consolidation;
  std::optional<uint8_t>            nrof_ss_blocks_to_average;
  std::optional<uint8_t>            nrof_csi_rs_res_to_average;
  uint8_t                           quant_cfg_idx;
  rrc_q_offset_range_list           offset_mo;
  std::vector<pci_t>                cells_to_rem_list; // max size = 32
  std::vector<rrc_cells_to_add_mod> cells_to_add_mod_list;
  std::vector<uint8_t>              excluded_cells_to_rem_list; // max size = 8
  std::vector<rrc_pci_range_elem>   excluded_cells_to_add_mod_list;
  std::vector<uint8_t>              allowed_cells_to_rem_list; // max size = 8
  std::vector<rrc_pci_range_elem>   allowed_cells_to_add_mod_list;
  // group 0
  std::optional<uint16_t> freq_band_ind_nr;
  std::optional<uint16_t> meas_cycle_scell;
};

struct rrc_n2 {
  uint8_t comb_offset_n2;
  uint8_t cyclic_shift_n2;
};

struct rrc_n4 {
  uint8_t comb_offset_n4;
  uint8_t cyclic_shift_n4;
};

using rrc_tx_comb = std::variant<rrc_n2, rrc_n4>;

struct rrc_res_map {
  uint8_t start_position;
  uint8_t nrof_symbols;
  uint8_t repeat_factor;
};

struct rrc_freq_hop {
  uint8_t c_srs;
  uint8_t b_srs;
  uint8_t b_hop;
};

struct rrc_srs_periodicity_and_offset {
  enum class period_t : uint16_t {
    ms1,
    ms2,
    ms4,
    ms5,
    ms8,
    ms10,
    ms16,
    ms20,
    ms32,
    ms40,
    ms64,
    ms80,
    ms160,
    ms320,
    ms640,
    ms1280,
    ms2560
  };

  period_t period;
  uint16_t offset;
};

struct rrc_periodic {
  rrc_srs_periodicity_and_offset periodicity_and_offset_sp_p;
};

struct rrc_semi_persistent {
  rrc_srs_periodicity_and_offset periodicity_and_offset_sp_p;
};

using rrc_res_type = std::variant<bool, rrc_semi_persistent, rrc_periodic>;

struct rrc_srs {
  uint8_t res_id;
  uint8_t ul_bwp;
};

struct rrc_srs_spatial_relation_info {
  enum class ssb_idx_t : uint8_t;
  enum class csi_rs_idx_t : uint8_t;

  using rrc_ref_sig_ = std::variant<ssb_idx_t, csi_rs_idx_t, rrc_srs>;

  std::optional<uint8_t> serving_cell_id;
  rrc_ref_sig_           ref_sig;
};

struct rrc_srs_res {
  uint8_t                                      srs_res_id;
  uint8_t                                      nrof_srs_ports;
  std::optional<uint8_t>                       ptrs_port_idx;
  rrc_tx_comb                                  tx_comb;
  rrc_res_map                                  res_map;
  uint8_t                                      freq_domain_position;
  uint16_t                                     freq_domain_shift;
  rrc_freq_hop                                 freq_hop;
  std::string                                  group_or_seq_hop;
  rrc_res_type                                 res_type;
  uint16_t                                     seq_id;
  std::optional<rrc_srs_spatial_relation_info> spatial_relation_info;
};

struct rrc_meas_obj_to_add_mod {
  meas_obj_id_t                  meas_obj_id;
  std::optional<rrc_meas_obj_nr> meas_obj_nr;
};

enum class rrc_nr_rs_type { ssb = 0, csi_rs };

struct rrc_meas_report_quant {
  bool rsrp;
  bool rsrq;
  bool sinr;
};

struct rrc_periodical_report_cfg {
  rrc_nr_rs_type                       rs_type;
  uint32_t                             report_interv;
  int8_t                               report_amount;
  rrc_meas_report_quant                report_quant_cell;
  uint8_t                              max_report_cells;
  std::optional<rrc_meas_report_quant> report_quant_rs_idxes;
  std::optional<uint8_t>               max_nrof_rs_idxes_to_report;
  bool                                 include_beam_meass;
  bool                                 use_allowed_cell_list;
  // Implementation specific config value.
  int8_t periodic_ho_rsrp_offset;
};

struct rrc_meas_trigger_quant {
  std::optional<uint8_t> rsrp;
  std::optional<uint8_t> rsrq;
  std::optional<uint8_t> sinr;
};

using rrc_meas_trigger_quant_offset = rrc_meas_trigger_quant;

struct rrc_event_id {
  enum class event_id_t : uint8_t { a1, a2, a3, a4, a5, a6 };
  // common parameters
  event_id_t id;

  bool     report_on_leave;
  uint8_t  hysteresis;
  uint16_t time_to_trigger;

  // event A1/A2/A4/A5
  std::optional<rrc_meas_trigger_quant>
      meas_trigger_quant_thres_or_offset; ///< Threshold for the measurement trigger quantity of event A1/A2/A4/A5 or
                                          ///< offset for event A3/A6
  std::optional<rrc_meas_trigger_quant>
      meas_trigger_quant_thres_2; ///< Threshold 2 for the measurement trigger quantity of event A5

  std::optional<bool> use_allowed_cell_list; ///< For event A3/A4/A5/A6
};

struct rrc_event_trigger_cfg {
  bool                                 report_add_neigh_meas_present;
  rrc_event_id                         event_id;
  rrc_nr_rs_type                       rs_type;
  uint16_t                             report_interv;
  int8_t                               report_amount;
  rrc_meas_report_quant                report_quant_cell;
  uint8_t                              max_report_cells;
  std::optional<rrc_meas_report_quant> report_quant_rs_idxes;
  std::optional<uint8_t>               max_nrof_rs_idxes_to_report;
  bool                                 include_beam_meass;
};

struct rrc_report_cgi {
  uint16_t cell_for_which_to_report_cgi;
};

struct rrc_report_sftd_nr {
  bool report_sftd_meas;
  bool report_rsrp;
};

struct rrc_cond_event_a3 {
  rrc_meas_trigger_quant_offset a3_offset;
  uint8_t                       hysteresis;
  uint16_t                      time_to_trigger;
};

struct rrc_cond_event_a5 {
  rrc_meas_trigger_quant a5_thres1;
  rrc_meas_trigger_quant a5_thres2;
  uint8_t                hysteresis;
  uint16_t               time_to_trigger;
};

using rrc_report_cfg_nr =
    std::variant<rrc_periodical_report_cfg, rrc_event_trigger_cfg, rrc_report_cgi, rrc_report_sftd_nr>;

struct rrc_report_cfg_to_add_mod {
  report_cfg_id_t   report_cfg_id;
  rrc_report_cfg_nr report_cfg;
};

struct rrc_meas_id_to_add_mod {
  meas_id_t       meas_id;
  meas_obj_id_t   meas_obj_id;
  report_cfg_id_t report_cfg_id;
};

struct rrc_s_measure_cfg {
  enum class measure_type_t : uint8_t { ssb_rsrp, csi_rsrp };

  measure_type_t type;
  uint8_t        measure_config;
};

struct rrc_filt_cfg {
  std::optional<uint8_t> filt_coef_rsrp;
  std::optional<uint8_t> filt_coef_rsrq;
  std::optional<uint8_t> filt_coef_rs_sinr;
};

struct rrc_quant_cfg_rs {
  rrc_filt_cfg ssb_filt_cfg;
  rrc_filt_cfg csi_rs_filt_cfg;
};

struct rrc_quant_cfg_nr {
  rrc_quant_cfg_rs                quant_cfg_cell;
  std::optional<rrc_quant_cfg_rs> quant_cfg_rs_idx;
};

struct rrc_quant_cfg {
  std::vector<rrc_quant_cfg_nr> quant_cfg_nr_list;
};

struct rrc_meas_cfg {
  std::vector<meas_obj_id_t>             meas_obj_to_rem_list;
  std::vector<rrc_meas_obj_to_add_mod>   meas_obj_to_add_mod_list;
  std::vector<report_cfg_id_t>           report_cfg_to_rem_list;
  std::vector<rrc_report_cfg_to_add_mod> report_cfg_to_add_mod_list;
  std::vector<meas_id_t>                 meas_id_to_rem_list;
  std::vector<rrc_meas_id_to_add_mod>    meas_id_to_add_mod_list;
  std::optional<rrc_s_measure_cfg>       s_measure_cfg;
  std::optional<rrc_quant_cfg>           quant_cfg;
  // The meas gap config is excluded from the common type and stored in the RRC UE context.
};

struct rrc_meas_quant_results {
  std::optional<uint8_t> rsrp;
  std::optional<uint8_t> rsrq;
  std::optional<uint8_t> sinr;
};

struct rrc_results_per_ssb_idx {
  uint8_t                               ssb_idx;
  std::optional<rrc_meas_quant_results> ssb_results;
};

struct rrc_results_per_csi_rs_idx {
  uint8_t                               csi_rs_idx;
  std::optional<rrc_meas_quant_results> csi_rs_results;
};

struct rrc_meas_result_nr {
  struct cell_results_ {
    std::optional<rrc_meas_quant_results> results_ssb_cell;
    std::optional<rrc_meas_quant_results> results_csi_rs_cell;
  };

  struct rs_idx_results_ {
    slotted_id_vector<uint8_t, rrc_results_per_ssb_idx>    results_ssb_idxes;    // indexed by ssb_idx
    slotted_id_vector<uint8_t, rrc_results_per_csi_rs_idx> results_csi_rs_idxes; // indexed by csi_rs_idx
  };

  std::optional<pci_t>           pci;
  cell_results_                  cell_results;
  std::optional<rs_idx_results_> rs_idx_results;
};

struct rrc_meas_result_serv_mo {
  uint8_t                           serv_cell_id;
  rrc_meas_result_nr                meas_result_serving_cell;
  std::optional<rrc_meas_result_nr> meas_result_best_neigh_cell;
};

struct rrc_meas_result_neigh_cells {
  std::vector<rrc_meas_result_nr> meas_result_list_nr;
};

struct rrc_meas_results {
  meas_id_t                                           meas_id;
  slotted_id_vector<uint8_t, rrc_meas_result_serv_mo> meas_result_serving_mo_list; // indexed by serv_cell_id
  std::optional<rrc_meas_result_neigh_cells>          meas_result_neigh_cells;
};

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

// Cell meas config formatter
template <>
struct formatter<srsran::srs_cu_cp::rrc_meas_obj_nr> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::rrc_meas_obj_nr& meas_object, FormatContext& ctx) const
  {
    std::string smtc1_str;
    std::string smtc2_str;

    if (meas_object.smtc1.has_value()) {
      smtc1_str = fmt::format(" smtc1: periodicity={} offset={} dur={}",
                              fmt::underlying(meas_object.smtc1.value().periodicity_and_offset.periodicity),
                              meas_object.smtc1.value().periodicity_and_offset.offset,
                              meas_object.smtc1.value().dur);
    }

    if (meas_object.smtc2.has_value()) {
      smtc2_str = " smtc2: pci_list=[ ";
      for (const auto& pci : meas_object.smtc2.value().pci_list) {
        smtc2_str = smtc2_str + std::to_string(pci) + " ";
      }
      smtc2_str = fmt::format("{}] periodicity={}", smtc2_str, meas_object.smtc2.value().periodicity);
    }

    return format_to(
        ctx.out(),
        "ssb_freq={} ssb_scs={}{}{}",
        meas_object.ssb_freq.has_value() ? fmt::to_string(meas_object.ssb_freq.value()) : "?",
        meas_object.ssb_subcarrier_spacing.has_value() ? to_string(meas_object.ssb_subcarrier_spacing.value()) : "?",
        smtc1_str,
        smtc2_str);
  }
};

} // namespace fmt

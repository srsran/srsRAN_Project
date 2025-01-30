/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/positioning/common.h"
#include "srsran/ran/positioning/measurement_information.h"
#include <cstdint>
#include <variant>

namespace srsran {

struct res_type_periodic_t {
  uint16_t periodicity;
  uint16_t offset;
};

struct res_type_semi_persistent_t {
  uint16_t periodicity;
  uint16_t offset;
};

struct res_type_aperiodic_t {
  bool aperiodic_res_type;
};

using res_type_t = std::variant<res_type_periodic_t, res_type_semi_persistent_t, res_type_aperiodic_t>;

using bw_srs_t = std::variant<uint8_t, uint16_t>;

struct nzp_csi_rs_t {
  uint8_t value;
};

struct srs_t {
  uint8_t value;
};

struct positioning_srs_t {
  uint8_t value;
};

struct dl_prs_t {
  uint16_t               prs_id;
  uint8_t                dl_prs_res_set_id;
  std::optional<uint8_t> dl_prs_res_id;
};

using ref_sig_t = std::variant<nzp_csi_rs_t, ssb_t, srs_t, positioning_srs_t, dl_prs_t>;

struct spatial_relation_for_res_id_item_t {
  ref_sig_t ref_sig;
};

struct spatial_relation_info_t {
  std::vector<spatial_relation_for_res_id_item_t> spatial_relation_for_res_id;
};

using pathloss_ref_sig_t = std::variant<ssb_t, dl_prs_t>;

struct pathloss_ref_info_t {
  pathloss_ref_sig_t pathloss_ref_sig;
};

struct spatial_relation_per_srs_res_item_t {
  ref_sig_t ref_sig;
};

struct spatial_relation_per_srs_res_t {
  std::vector<spatial_relation_per_srs_res_item_t> spatial_relation_per_srs_res_list;
};

struct periodicity_item_t {
  enum class value_t {
    ms0dot125 = 0,
    ms0dot25,
    ms0dot5,
    ms0dot625,
    ms1,
    ms1dot25,
    ms2,
    ms2dot5,
    ms4dot,
    ms5,
    ms8,
    ms10,
    ms16,
    ms20,
    ms32,
    ms40,
    ms64,
    ms80m,
    ms160,
    ms320,
    ms640m,
    ms1280,
    ms2560,
    ms5120,
    ms10240,
  };

  static float to_float(const periodicity_item_t::value_t& enum_val)
  {
    static const float numbers[] = {0.125, 0.25,  0.5,   0.625,  1.0,    1.25,   2.0,    2.5,  4.0,
                                    5.0,   8.0,   10.0,  16.0,   20.0,   32.0,   40.0,   64.0, 80.0,
                                    160.0, 320.0, 640.0, 1280.0, 2560.0, 5120.0, 10240.0};

    if ((unsigned)enum_val >= sizeof(numbers)) {
      return (float)0;
    }
    return numbers[(uint8_t)enum_val];
  }

  value_t value;
};

struct srs_res_set_item_t {
  std::optional<uint8_t>                      nof_srs_res_per_set;
  std::vector<periodicity_item_t>             periodicity_list;
  std::optional<spatial_relation_info_t>      spatial_relation_info;
  std::optional<pathloss_ref_info_t>          pathloss_ref_info;
  std::vector<spatial_relation_per_srs_res_t> spatial_relation_per_srs_res_list;
};

struct requested_srs_tx_characteristics_t {
  enum class res_type_t { periodic, semi_persistent, aperiodic };

  std::optional<uint16_t>         nof_txs;
  res_type_t                      res_type;
  bw_srs_t                        bw;
  std::vector<srs_res_set_item_t> list_of_srs_res_set;
  std::optional<ssb_info_t>       ssb_info;
  std::vector<uint32_t>           srs_freqs;
};

struct ue_report_info_t {
  uint8_t report_amount;
  uint8_t report_interv;
};

enum class ue_teg_info_request_t { on_demand, periodic, stop };

struct scs_specific_carrier_t {
  uint16_t           offset_to_carrier;
  subcarrier_spacing scs;
  uint16_t           carrier_bw;
};

struct n2_t {
  uint8_t comb_offset_n2;
  uint8_t cyclic_shift_n2;
};

struct n4_t {
  uint8_t comb_offset_n4;
  uint8_t cyclic_shift_n4;
};

using tx_comb_t = std::variant<n2_t, n4_t>;

enum class group_or_seq_hop_t { neither, group_hop, seq_hop };

struct srs_res_t {
  uint8_t            srs_res_id;
  uint8_t            nrof_srs_ports;
  tx_comb_t          tx_comb;
  uint8_t            start_position;
  uint8_t            nrof_symbols;
  uint8_t            repeat_factor;
  uint8_t            freq_domain_position;
  uint16_t           freq_domain_shift;
  uint8_t            c_srs;
  uint8_t            b_srs;
  uint8_t            b_hop;
  group_or_seq_hop_t group_or_seq_hop;
  res_type_t         res_type;
  uint16_t           seq_id;
};

struct n8_t {
  uint8_t comb_offset_n8;
  uint8_t cyclic_shift_n8;
};

using tx_comb_pos_t = std::variant<n2_t, n4_t, n8_t>;

struct res_type_periodic_pos_t {
  uint32_t periodicity;
  uint32_t offset;
};

struct res_type_semi_persistent_pos_t {
  uint32_t periodicity;
  uint32_t offset;
};

struct res_type_aperiodic_pos_t {
  uint8_t slot_offset;
};

using res_type_pos_t = std::variant<res_type_periodic_pos_t, res_type_semi_persistent_pos_t, res_type_aperiodic_pos_t>;

struct prs_information_pos_t {
  uint16_t               prs_id_pos;
  uint8_t                prs_res_set_id_pos;
  std::optional<uint8_t> prs_res_id_pos;
};

using spatial_relation_pos_t = std::variant<ssb_t, prs_information_pos_t>;

struct pos_srs_res_item_t {
  uint8_t                               srs_pos_res_id;
  tx_comb_pos_t                         tx_comb_pos;
  uint8_t                               start_position;
  uint8_t                               nrof_symbols;
  uint16_t                              freq_domain_shift;
  uint8_t                               c_srs;
  group_or_seq_hop_t                    group_or_seq_hop;
  res_type_pos_t                        res_type_pos;
  uint32_t                              seq_id;
  std::optional<spatial_relation_pos_t> spatial_relation_pos;
};

struct res_set_type_periodic_t {
  bool periodic_set;
};

struct res_set_type_semi_persistent_t {
  bool semi_persistent_set;
};

struct res_set_type_aperiodic_t {
  uint8_t srs_res_trigger_list;
  uint8_t slot_offset;
};

using res_set_type_t = std::variant<res_set_type_periodic_t, res_set_type_semi_persistent_t, res_set_type_aperiodic_t>;

struct srs_res_set_t {
  uint8_t              srs_res_set_id;
  std::vector<uint8_t> srs_res_id_list;
  res_set_type_t       res_set_type;
};

struct pos_res_set_type_periodic_t {
  bool pos_periodic_set;
};

struct pos_res_set_type_semi_persistent_t {
  bool pos_semi_persistent_set;
};

struct pos_res_set_type_aperiodic_t {
  uint8_t srs_res_trigger_list;
};

using pos_res_set_type_t =
    std::variant<pos_res_set_type_periodic_t, pos_res_set_type_semi_persistent_t, pos_res_set_type_aperiodic_t>;

struct pos_srs_res_set_item_t {
  uint8_t              pos_srs_res_set_id;
  std::vector<uint8_t> pos_srs_res_id_per_set_list;
  pos_res_set_type_t   pos_res_set_type;
};

struct srs_config_t {
  std::vector<srs_res_t>              srs_res_list;
  std::vector<pos_srs_res_item_t>     pos_srs_res_list;
  std::vector<srs_res_set_t>          srs_res_set_list;
  std::vector<pos_srs_res_set_item_t> pos_srs_res_set_list;
};

struct active_ul_bwp_t {
  uint16_t            location_and_bw;
  subcarrier_spacing  scs;
  cyclic_prefix       cp;
  uint16_t            tx_direct_current_location;
  std::optional<bool> shift7dot5k_hz;
  srs_config_t        srs_cfg;
};

struct srs_carrier_list_item_t {
  uint32_t                            point_a;
  std::vector<scs_specific_carrier_t> ul_ch_bw_per_scs_list;
  active_ul_bwp_t                     active_ul_bwp;
  std::optional<pci_t>                pci_nr;
};

struct srs_configuration_t {
  std::vector<srs_carrier_list_item_t> srs_carrier_list;
};

struct carrier_freq_t {
  uint32_t point_a;
  uint16_t offset_to_carrier;
};

struct ue_tx_teg_assoc_item_t {
  uint8_t                       ue_tx_teg_id;
  std::vector<uint8_t>          pos_srs_res_id_list;
  time_stamp_t                  time_stamp;
  std::optional<carrier_freq_t> carrier_freq;
  std::vector<uint8_t>          timing_error_margins;
};

} // namespace srsran

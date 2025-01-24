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

#include "../cyclic_prefix.h"
#include "common.h"
#include "positioning_ids.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <variant>
#include <vector>

namespace srsran {

enum class trp_information_type_item_t {
  nr_pci,
  ng_ran_cgi,
  arfcn,
  prs_cfg,
  ssb_info,
  sfn_init_time,
  spatial_direct_info,
  geo_coord,
  trp_type,
  ondemand_prs_info,
  trp_tx_teg,
  beam_ant_info
};

struct dl_prs_muting_pattern_t {
  uint8_t  length;
  uint32_t value;
};

struct prs_muting_option1_t {
  dl_prs_muting_pattern_t muting_pattern;
  uint8_t                 muting_bit_repeat_factor;
};

struct prs_muting_option2_t {
  dl_prs_muting_pattern_t muting_pattern;
};

struct prs_muting_t {
  std::optional<prs_muting_option1_t> prs_muting_option1;
  std::optional<prs_muting_option2_t> prs_muting_option2;
};

struct prs_resource_qcl_source_ssb_t {
  pci_t                  pci_nr;
  std::optional<uint8_t> ssb_idx;
};

struct prs_resource_qcl_source_prs_t {
  uint8_t                qcl_source_prs_res_set_id;
  std::optional<uint8_t> qcl_source_prs_res_id;
};

using prs_resource_qcl_info_t = std::variant<prs_resource_qcl_source_ssb_t, prs_resource_qcl_source_prs_t>;

struct prs_res_item_t {
  uint8_t                                prs_res_id;
  uint16_t                               seq_id;
  uint8_t                                re_offset;
  uint16_t                               res_slot_offset;
  uint8_t                                res_symbol_offset;
  std::optional<prs_resource_qcl_info_t> qcl_info;
};

struct prs_resource_set_item_t {
  uint8_t                     prs_res_set_id;
  subcarrier_spacing          scs;
  uint8_t                     prs_bw;
  uint16_t                    start_prb;
  uint32_t                    point_a;
  uint8_t                     comb_size;
  cyclic_prefix               cp_type;
  uint32_t                    res_set_periodicity;
  uint32_t                    res_set_slot_offset;
  uint8_t                     res_repeat_factor;
  uint8_t                     res_time_gap;
  uint8_t                     res_numof_symbols;
  std::optional<prs_muting_t> prs_muting;
  int8_t                      prs_res_tx_pwr;
  std::vector<prs_res_item_t> prs_res_list;
};

struct prs_cfg_t {
  std::vector<prs_resource_set_item_t> prs_res_set_list;
};

struct ssb_burst_position_t {
  enum class bitmap_type_t : uint8_t { /* uint8_t */ short_bitmap,
                                       /* uint8_t */ medium_bitmap,
                                       /* uint64_t */ long_bitmap };

  bitmap_type_t type;
  uint64_t      bitmap;
};

struct tf_config_t {
  uint32_t                            ssb_freq;
  subcarrier_spacing                  ssb_subcarrier_spacing;
  int8_t                              ssb_tx_pwr;
  uint8_t                             ssb_periodicity;
  uint8_t                             ssb_half_frame_offset;
  uint8_t                             ssb_sfn_offset;
  std::optional<ssb_burst_position_t> ssb_burst_position;
  std::optional<uint64_t>             sfn_initialization_time;
};

struct ssb_info_item_t {
  tf_config_t ssb_cfg;
  pci_t       pci_nr;
};

struct ssb_info_t {
  std::vector<ssb_info_item_t> list_of_ssb_info;
};

struct prs_angle_item_ext_ies_t {
  uint8_t prs_res_id;
};

struct prs_angle_item_t {
  uint16_t                              nr_prs_azimuth;
  std::optional<uint8_t>                nr_prs_azimuth_fine;
  std::optional<uint8_t>                nr_prs_elevation;
  std::optional<uint8_t>                nr_prs_elevation_fine;
  std::vector<prs_angle_item_ext_ies_t> ie_exts;
};

struct nr_prs_beam_info_item_t {
  uint8_t                       pr_sres_set_id;
  std::vector<prs_angle_item_t> prs_angle;
};

struct lcs_to_gcs_translation_item_t {
  uint16_t               alpha;
  std::optional<uint8_t> alpha_fine;
  uint16_t               beta;
  std::optional<uint8_t> beta_fine;
  uint16_t               gamma;
  std::optional<uint8_t> gamma_fine;
};

struct nr_prs_beam_info_t {
  std::vector<nr_prs_beam_info_item_t>       nr_prs_beam_info_list;
  std::vector<lcs_to_gcs_translation_item_t> lcs_to_gcs_translation_list;
};

struct spatial_direction_info_t {
  nr_prs_beam_info_t nr_prs_beam_info;
};

struct ng_ran_high_accuracy_access_point_position_t {
  int64_t  latitude;
  int64_t  longitude;
  int32_t  altitude;
  uint16_t uncertainty_semi_major;
  uint16_t uncertainty_semi_minor;
  uint8_t  orientation_of_major_axis;
  uint8_t  horizontal_confidence;
  uint16_t uncertainty_altitude;
  uint8_t  vertical_confidence;
};

using trp_position_direct_accuracy_t =
    std::variant<ng_ran_access_point_position_t, ng_ran_high_accuracy_access_point_position_t>;

struct trp_position_direct_t {
  trp_position_direct_accuracy_t accuracy;
};

using ref_point_t =
    std::variant<uint16_t, ng_ran_access_point_position_t, ng_ran_high_accuracy_access_point_position_t>;

enum class milli_arc_second_units_t { zerodot03, zerodot3, three };

enum class height_units_t { mm, cm, m };

struct location_uncertainty_t {
  uint16_t horizontal_uncertainty;
  uint8_t  horizontal_confidence;
  uint16_t vertical_uncertainty;
  uint8_t  vertical_confidence;
};

struct relative_geodetic_location_t {
  milli_arc_second_units_t milli_arc_second_units;
  height_units_t           height_units;
  int16_t                  delta_latitude;
  int16_t                  delta_longitude;
  int16_t                  delta_height;
  location_uncertainty_t   location_uncertainty;
};

enum class xyz_unit_t { mm, cm, dm };

struct relative_cartesian_location_t {
  xyz_unit_t             xyz_unit;
  int32_t                xvalue;
  int32_t                yvalue;
  int32_t                zvalue;
  location_uncertainty_t location_uncertainty;
};

using trp_ref_point_type_t = std::variant<relative_geodetic_location_t, relative_cartesian_location_t>;

struct trp_position_refd_t {
  ref_point_t          ref_point;
  trp_ref_point_type_t ref_point_type;
};

using trp_position_definition_type_t = std::variant<trp_position_direct_t, trp_position_refd_t>;

using dl_prs_res_set_arp_location_t = std::variant<relative_geodetic_location_t, relative_cartesian_location_t>;

using dl_prs_res_arp_location_t = std::variant<relative_geodetic_location_t, relative_cartesian_location_t>;

struct dl_prs_res_arp_t {
  uint8_t                   dl_prs_res_id;
  dl_prs_res_arp_location_t dl_prs_res_arp_location;
};

struct dl_prs_res_set_arp_t {
  uint8_t                       dl_prs_res_set_id;
  dl_prs_res_set_arp_location_t dl_prs_res_set_arp_location;
  std::vector<dl_prs_res_arp_t> listof_dl_prs_res_arp;
};

struct dl_prs_res_coordinates_t {
  std::vector<dl_prs_res_set_arp_t> listof_dl_prs_res_set_arp;
};

using arp_location_type_t = std::variant<relative_geodetic_location_t, relative_cartesian_location_t>;

struct arp_location_info_item_t {
  uint8_t             arp_id;
  arp_location_type_t arp_location_type;
};

struct geographical_coordinates_ext_ies_t {
  std::vector<arp_location_info_item_t> arp_location_info;
};

struct geographical_coordinates_t {
  trp_position_definition_type_t                  trp_position_definition_type;
  std::optional<dl_prs_res_coordinates_t>         dl_prs_res_coordinates;
  std::vector<geographical_coordinates_ext_ies_t> ie_exts;
};

enum class trp_type_t { prs_only_tp, srs_only_rp, tp, rp, trp };

struct on_demand_prs_info_t {
  uint16_t                on_demand_prs_request_allowed;
  std::optional<uint32_t> allowed_res_set_periodicity_values;
  std::optional<uint64_t> allowed_prs_bw_values;
  std::optional<uint8_t>  allowed_res_repeat_factor_values;
  std::optional<uint8_t>  allowed_res_nof_symbols_values;
  std::optional<uint8_t>  allowed_comb_size_values;
};

struct trp_tx_teg_info_t {
  uint8_t trp_tx_teg_id;
  uint8_t trp_tx_timing_error_margin;
};

struct dl_prs_res_id_item_t {
  uint8_t dl_prs_res_id;
};

struct trpteg_item_t {
  trp_tx_teg_info_t                 trp_tx_teg_info;
  uint8_t                           dl_prs_res_set_id;
  std::vector<dl_prs_res_id_item_t> dl_prs_res_id_list;
};

struct trp_beam_pwr_item_t {
  std::optional<uint8_t> prs_res_set_id;
  uint8_t                prs_res_id;
  uint8_t                relative_pwr;
  std::optional<uint8_t> relative_pwr_fine;
};

struct trp_elevation_angle_list_item_t {
  uint8_t                          trp_elevation_angle;
  std::optional<uint8_t>           trp_elevation_angle_fine;
  std::vector<trp_beam_pwr_item_t> trp_beam_pwr_list;
};

struct trp_beam_ant_angles_list_item_t {
  uint16_t                                     trp_azimuth_angle;
  std::optional<uint8_t>                       trp_azimuth_angle_fine;
  std::vector<trp_elevation_angle_list_item_t> trp_elevation_angle_list;
};

struct trp_beam_ant_explicit_info_t {
  std::vector<trp_beam_ant_angles_list_item_t> trp_beam_ant_angles;
  std::optional<lcs_to_gcs_translation_t>      lcs_to_gcs_translation;
};

using choice_trp_beam_ant_info_item_t = std::variant<uint32_t, trp_beam_ant_explicit_info_t>;

struct trp_beam_ant_info_t {
  choice_trp_beam_ant_info_item_t choice_trp_beam_ant_info_item;
};

using trp_info_type_resp_item_ext_ies_t =
    std::variant<trp_type_t, on_demand_prs_info_t, std::vector<trpteg_item_t>, trp_beam_ant_info_t>;

using trp_information_type_response_item_t = std::variant<pci_t,
                                                          nr_cell_global_id_t,
                                                          uint32_t,
                                                          prs_cfg_t,
                                                          ssb_info_t,
                                                          uint64_t,
                                                          spatial_direction_info_t,
                                                          geographical_coordinates_t,
                                                          trp_info_type_resp_item_ext_ies_t>;

struct trp_info_t {
  trp_id_t                                          trp_id;
  std::vector<trp_information_type_response_item_t> trp_info_type_resp_list;
};

struct trp_information_list_trp_response_item_t {
  trp_info_t trp_info;
};

} // namespace srsran

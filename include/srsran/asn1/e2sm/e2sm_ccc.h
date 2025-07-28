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

/*******************************************************************************
 *
 *                     3GPP TS ASN1 E2SM CCC v06.00 (2025)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_ap_utils.h"
#include "srsran/asn1/asn1_json_utils.h"

namespace asn1 {
namespace e2sm_ccc {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E2SM_CCC_MAXNOOF_ELEMS 64

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// M-5QiList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..255)
using m_5qi_list_l = dyn_array<uint16_t>;

// PlmnId ::= SEQUENCE
struct plmn_id_s {
  bool                               mcc_present = false;
  bool                               mnc_present = false;
  printable_string<3, 3, true, true> mcc;
  printable_string<2, 3, true, true> mnc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const plmn_id_s& obj);
void from_json(const nlohmann::ordered_json& in_j, plmn_id_s& obj);

// Snssai ::= SEQUENCE
struct snssai_s {
  bool                     sst_present = false;
  bool                     sd_present  = false;
  uint16_t                 sst         = 0;
  fixed_octstring<3, true> sd;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const snssai_s& obj);
void from_json(const nlohmann::ordered_json& in_j, snssai_s& obj);

struct partition_flow_list_item_s_ {
  bool         snssai_present  = false;
  bool         plmn_id_present = false;
  snssai_s     snssai;
  plmn_id_s    plmn_id;
  m_5qi_list_l n5qi_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const partition_flow_list_item_s_& obj);
void from_json(const nlohmann::ordered_json& in_j, partition_flow_list_item_s_& obj);

// PlmnInfo ::= SEQUENCE
struct plmn_info_s {
  bool      plmn_id_present = false;
  bool      snssai_present  = false;
  plmn_id_s plmn_id;
  snssai_s  snssai;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const plmn_info_s& obj);
void from_json(const nlohmann::ordered_json& in_j, plmn_info_s& obj);

// RPolarizationItem ::= SEQUENCE
struct r_polarization_item_s {
  struct rpolarization_type_opts {
    enum options { minus_neg45, zero, plus_neg45, plus_neg90, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using rpolarization_type_e_ = enumerated<rpolarization_type_opts>;

  // member variables
  uint16_t              rpolarization_idx = 0;
  rpolarization_type_e_ rpolarization_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const r_polarization_item_s& obj);
void from_json(const nlohmann::ordered_json& in_j, r_polarization_item_s& obj);

// SleepMode ::= SEQUENCE
struct sleep_mode_s {
  struct sleep_mode_type_opts {
    enum options { sleep_mode0, sleep_mode1, sleep_mode2, sleep_mode3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using sleep_mode_type_e_ = enumerated<sleep_mode_type_opts>;

  // member variables
  sleep_mode_type_e_ sleep_mode_type;
  int64_t            wakeup_dur            = 0;
  bool               wakeup_dur_guaranteed = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const sleep_mode_s& obj);
void from_json(const nlohmann::ordered_json& in_j, sleep_mode_s& obj);

// SupportedTrxControlMask ::= SEQUENCE
struct supported_trx_ctrl_mask_s {
  printable_string<1, 150, true, true> mask_name;
  int64_t                              ant_mask = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const supported_trx_ctrl_mask_s& obj);
void from_json(const nlohmann::ordered_json& in_j, supported_trx_ctrl_mask_s& obj);

// TPolarizationItem ::= SEQUENCE
struct t_polarization_item_s {
  struct tpolarization_type_opts {
    enum options { minus_neg45, zero, plus_neg45, plus_neg90, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using tpolarization_type_e_ = enumerated<tpolarization_type_opts>;

  // member variables
  uint16_t              tpolarization_idx = 0;
  tpolarization_type_e_ tpolarization_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const t_polarization_item_s& obj);
void from_json(const nlohmann::ordered_json& in_j, t_polarization_item_s& obj);

// PartitionFlowList ::= SEQUENCE (SIZE (1..64)) OF PartitionFlowList-item
using partition_flow_list_l = dyn_array<partition_flow_list_item_s_>;

// PlmnInfoList ::= SEQUENCE (SIZE (1..64)) OF PlmnInfo
using plmn_info_list_l = dyn_array<plmn_info_s>;

// RxArrayItem ::= SEQUENCE
struct rx_array_item_s {
  struct rnormal_vector_direction_s_ {
    bool   razimuth_angle_present = false;
    bool   rzenith_angle_present  = false;
    real_s razimuth_angle;
    real_s rzenith_angle;
  };
  struct rleftmost_bottom_array_elem_position_s_ {
    bool   rx_axis_present = false;
    bool   ry_axis_present = false;
    bool   rz_axis_present = false;
    real_s rx_axis;
    real_s ry_axis;
    real_s rz_axis;
  };
  using rpolarization_list_l_ = dyn_array<r_polarization_item_s>;
  struct rgain_correction_range_s_ {
    bool   rmax_present = false;
    bool   rmin_present = false;
    real_s rmax;
    real_s rmin;
  };

  // member variables
  bool                                    rhorizontal_spacing_present                  = false;
  bool                                    rvertical_spacing_present                    = false;
  bool                                    rnormal_vector_direction_present             = false;
  bool                                    rleftmost_bottom_array_elem_position_present = false;
  printable_string<1, 150, true, true>    rname;
  uint32_t                                rnof_rows         = 1;
  uint32_t                                rnof_columns      = 1;
  uint16_t                                rnof_array_layers = 1;
  real_s                                  rhorizontal_spacing;
  real_s                                  rvertical_spacing;
  rnormal_vector_direction_s_             rnormal_vector_direction;
  rleftmost_bottom_array_elem_position_s_ rleftmost_bottom_array_elem_position;
  rpolarization_list_l_                   rpolarization_list;
  uint16_t                                rband_num = 1;
  rgain_correction_range_s_               rgain_correction_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const rx_array_item_s& obj);
void from_json(const nlohmann::ordered_json& in_j, rx_array_item_s& obj);

// SleepModes ::= SEQUENCE (SIZE (1..64)) OF SleepMode
using sleep_modes_l = dyn_array<sleep_mode_s>;

// SupportedTrxControlMasks ::= SEQUENCE (SIZE (1..64)) OF SupportedTrxControlMask
using supported_trx_ctrl_masks_l = dyn_array<supported_trx_ctrl_mask_s>;

// TxArrayItem ::= SEQUENCE
struct tx_array_item_s {
  struct tnormal_vector_direction_s_ {
    bool   tazimuth_angle_present = false;
    bool   tzenith_angle_present  = false;
    real_s tazimuth_angle;
    real_s tzenith_angle;
  };
  struct tleftmost_bottom_array_elem_position_s_ {
    bool   tx_axis_present = false;
    bool   ty_axis_present = false;
    bool   tz_axis_present = false;
    real_s tx_axis;
    real_s ty_axis;
    real_s tz_axis;
  };
  using tpolarization_list_l_ = dyn_array<t_polarization_item_s>;

  // member variables
  bool                                    thorizontal_spacing_present                  = false;
  bool                                    tvertical_spacing_present                    = false;
  bool                                    tnormal_vector_direction_present             = false;
  bool                                    tleftmost_bottom_array_elem_position_present = false;
  bool                                    tmin_gain_present                            = false;
  printable_string<1, 150, true, true>    tname;
  uint32_t                                tnof_rows         = 1;
  uint32_t                                tnof_columns      = 1;
  uint16_t                                tnof_array_layers = 1;
  real_s                                  thorizontal_spacing;
  real_s                                  tvertical_spacing;
  tnormal_vector_direction_s_             tnormal_vector_direction;
  tleftmost_bottom_array_elem_position_s_ tleftmost_bottom_array_elem_position;
  tpolarization_list_l_                   tpolarization_list;
  uint16_t                                tband_num = 1;
  real_s                                  tmax_gain;
  real_s                                  tmin_gain;
  bool                                    tindependent_pwr_budget = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const tx_array_item_s& obj);
void from_json(const nlohmann::ordered_json& in_j, tx_array_item_s& obj);

// AsmCapabilityInfo ::= SEQUENCE
struct asm_cap_info_s {
  sleep_modes_l sleep_modes;
  bool          defined_dur_sleep_supported   = false;
  bool          undefined_dur_sleep_supported = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const asm_cap_info_s& obj);
void from_json(const nlohmann::ordered_json& in_j, asm_cap_info_s& obj);

// BwpContext ::= ENUMERATED
struct bwp_context_opts {
  enum options { dl, ul, sul, nulltype } value;

  const char* to_string() const;
  bool        from_string(const std::string& str);
};
using bwp_context_e = enumerated<bwp_context_opts>;

// CyclicPrefix ::= ENUMERATED
struct cp_opts {
  enum options { n15, n30, n60, n120, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  bool        from_string(const std::string& str);
  uint8_t     to_number() const;
};
using cp_e = enumerated<cp_opts>;

// EnergySavingCapabilityCommonInfo ::= SEQUENCE
struct energy_saving_cap_common_info_s {
  bool sleep_dur_ext_supported_present = false;
  bool st8_ready_msg_supported         = false;
  bool sleep_dur_ext_supported         = false;
  bool emergency_wake_up_cmd_supported = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const energy_saving_cap_common_info_s& obj);
void from_json(const nlohmann::ordered_json& in_j, energy_saving_cap_common_info_s& obj);

// EventTriggerStyleType ::= SEQUENCE
struct event_trigger_style_type_s {
  int64_t event_trigger_style_type = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const event_trigger_style_type_s& obj);
void from_json(const nlohmann::ordered_json& in_j, event_trigger_style_type_s& obj);

// IsInitialBwp ::= ENUMERATED
struct is_init_bwp_opts {
  enum options { init, other, sul, nulltype } value;

  const char* to_string() const;
  bool        from_string(const std::string& str);
};
using is_init_bwp_e = enumerated<is_init_bwp_opts>;

struct partition_list_item_s_ {
  bool                  poffset_to_point_a_present = false;
  bool                  pnof_rbs_present           = false;
  int64_t               poffset_to_point_a         = 0;
  int64_t               pnof_rbs                   = 0;
  partition_flow_list_l partition_flow_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const partition_list_item_s_& obj);
void from_json(const nlohmann::ordered_json& in_j, partition_list_item_s_& obj);

// PerfObjective ::= SEQUENCE
struct perf_objive_s {
  bool                                 max_ngbr_flow_br_present = false;
  bool                                 flow_br_avg_win_present  = false;
  bool                                 max_pd_present           = false;
  bool                                 target_pd_present        = false;
  plmn_info_list_l                     plmn_info_list;
  uint16_t                             five_qi_value = 0;
  printable_string<0, 400, true, true> max_ngbr_flow_br;
  uint16_t                             flow_br_avg_win = 0;
  uint16_t                             max_pd          = 0;
  uint16_t                             target_pd       = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const perf_objive_s& obj);
void from_json(const nlohmann::ordered_json& in_j, perf_objive_s& obj);

// PrbBlankingConfigItem ::= SEQUENCE
struct prb_blanking_cfg_item_s {
  bool     bwp_idx_present   = false;
  bool     frame_num_present = false;
  bool     slot_num_present  = false;
  bool     prb_idx_present   = false;
  uint8_t  bwp_idx           = 0;
  uint16_t frame_num         = 0;
  uint16_t slot_num          = 0;
  uint16_t prb_idx           = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const prb_blanking_cfg_item_s& obj);
void from_json(const nlohmann::ordered_json& in_j, prb_blanking_cfg_item_s& obj);

// RrmPolicyMember ::= SEQUENCE
struct rrm_policy_member_s {
  bool      plmn_id_present = false;
  bool      snssai_present  = false;
  plmn_id_s plmn_id;
  snssai_s  snssai;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const rrm_policy_member_s& obj);
void from_json(const nlohmann::ordered_json& in_j, rrm_policy_member_s& obj);

// RxArrayList ::= SEQUENCE (SIZE (1..64)) OF RxArrayItem
using rx_array_list_l = dyn_array<rx_array_item_s>;

// TrxControlCapabilityInfo ::= SEQUENCE
struct trx_ctrl_cap_info_s {
  supported_trx_ctrl_masks_l supported_trx_ctrl_masks;
  sleep_modes_l              sleep_modes;
  bool                       defined_dur_sleep_supported   = false;
  bool                       undefined_dur_sleep_supported = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const trx_ctrl_cap_info_s& obj);
void from_json(const nlohmann::ordered_json& in_j, trx_ctrl_cap_info_s& obj);

// TxArrayList ::= SEQUENCE (SIZE (1..64)) OF TxArrayItem
using tx_array_list_l = dyn_array<tx_array_item_s>;

// AdministrativeState ::= ENUMERATED
struct administrative_state_opts {
  enum options { locked, shuttingdown, unlocked, nulltype } value;

  const char* to_string() const;
  bool        from_string(const std::string& str);
};
using administrative_state_e = enumerated<administrative_state_opts>;

// CellState ::= ENUMERATED
struct cell_state_opts {
  enum options { id_le, inactive, active, nulltype } value;

  const char* to_string() const;
  bool        from_string(const std::string& str);
};
using cell_state_e = enumerated<cell_state_opts>;

// ControlStyle ::= SEQUENCE
struct ctrl_style_s {
  bool                                 ric_call_process_id_format_type_present = false;
  int64_t                              ctrl_service_style_type                 = 0;
  printable_string<1, 150, true, true> ctrl_service_style_name;
  int64_t                              ctrl_service_hdr_format_type          = 0;
  int64_t                              ctrl_service_msg_format_type          = 0;
  int64_t                              ric_call_process_id_format_type       = 0;
  int64_t                              ctrl_service_ctrl_outcome_format_type = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ctrl_style_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ctrl_style_s& obj);

// CycleStartOffset ::= SEQUENCE
struct cycle_start_offset_s {
  struct periodicity_opts {
    enum options {
      ms10,
      ms20,
      ms32,
      ms40,
      ms60,
      ms64,
      ms70,
      ms80,
      ms128,
      ms160,
      ms256,
      ms320,
      ms512,
      ms640,
      ms1024,
      ms1280,
      ms2048,
      ms2560,
      ms5120,
      ms10240,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint16_t    to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts>;

  // member variables
  bool           periodicity_present = false;
  bool           offset_present      = false;
  periodicity_e_ periodicity;
  uint16_t       offset = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cycle_start_offset_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cycle_start_offset_s& obj);

// EsObjective ::= CHOICE
struct es_objive_c {
  struct target_ec_s_ {
    bool   target_ec_present = false;
    real_s target_ec;
  };
  struct es_percentage_s_ {
    bool    es_percentage_present = false;
    uint8_t es_percentage         = 0;
  };
  struct types_opts {
    enum options { target_ec, es_percentage, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using types = enumerated<types_opts>;

  // choice methods
  es_objive_c() = default;
  es_objive_c(const es_objive_c& other);
  es_objive_c& operator=(const es_objive_c& other);
  ~es_objive_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  target_ec_s_& target_ec()
  {
    assert_choice_type(types::target_ec, type_, "EsObjective");
    return c.get<target_ec_s_>();
  }
  es_percentage_s_& es_percentage()
  {
    assert_choice_type(types::es_percentage, type_, "EsObjective");
    return c.get<es_percentage_s_>();
  }
  const target_ec_s_& target_ec() const
  {
    assert_choice_type(types::target_ec, type_, "EsObjective");
    return c.get<target_ec_s_>();
  }
  const es_percentage_s_& es_percentage() const
  {
    assert_choice_type(types::es_percentage, type_, "EsObjective");
    return c.get<es_percentage_s_>();
  }
  target_ec_s_&     set_target_ec();
  es_percentage_s_& set_es_percentage();

private:
  types                                           type_;
  choice_buffer_t<es_percentage_s_, target_ec_s_> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const es_objive_c& obj);
void from_json(const nlohmann::ordered_json& in_j, es_objive_c& obj);

// EventTriggerStyle ::= SEQUENCE
struct event_trigger_style_s {
  int64_t                              event_trigger_style_type = 0;
  printable_string<1, 150, true, true> event_trigger_style_name;
  int64_t                              event_trigger_format_type = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const event_trigger_style_s& obj);
void from_json(const nlohmann::ordered_json& in_j, event_trigger_style_s& obj);

// GEnbIdList ::= SEQUENCE (SIZE (1..64)) OF PrintableString (SIZE (10..17,...))
using g_enb_id_list_l = dyn_array<printable_string<10, 17, true, true>>;

// GGnbIdList ::= SEQUENCE (SIZE (1..64)) OF PrintableString (SIZE (10..20,...))
using g_gnb_id_list_l = dyn_array<printable_string<10, 20, true, true>>;

// O-Bwp ::= SEQUENCE
struct o_bwp_s {
  bool          bwp_context_present         = false;
  bool          is_init_bwp_present         = false;
  bool          sub_carrier_spacing_present = false;
  bool          cp_present                  = false;
  bool          start_rb_present            = false;
  bool          nof_rbs_present             = false;
  bwp_context_e bwp_context;
  is_init_bwp_e is_init_bwp;
  int64_t       sub_carrier_spacing = 0;
  cp_e          cp;
  int64_t       start_rb = 0;
  int64_t       nof_rbs  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_bwp_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_bwp_s& obj);

// OnDurationTimer ::= CHOICE
struct on_dur_timer_c {
  struct timeout_opts {
    enum options {
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms400,
      ms500,
      ms600,
      ms800,
      ms1000,
      ms1200,
      ms1600,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint16_t    to_number() const;
  };
  using timeout_e_ = enumerated<timeout_opts>;
  struct types_opts {
    enum options { integer, timeout, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using types = enumerated<types_opts>;

  // choice methods
  on_dur_timer_c() = default;
  on_dur_timer_c(const on_dur_timer_c& other);
  on_dur_timer_c& operator=(const on_dur_timer_c& other);
  ~on_dur_timer_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& integer()
  {
    assert_choice_type(types::integer, type_, "OnDurationTimer");
    return c.get<uint8_t>();
  }
  timeout_e_& timeout()
  {
    assert_choice_type(types::timeout, type_, "OnDurationTimer");
    return c.get<timeout_e_>();
  }
  const uint8_t& integer() const
  {
    assert_choice_type(types::integer, type_, "OnDurationTimer");
    return c.get<uint8_t>();
  }
  const timeout_e_& timeout() const
  {
    assert_choice_type(types::timeout, type_, "OnDurationTimer");
    return c.get<timeout_e_>();
  }
  uint8_t&    set_integer();
  timeout_e_& set_timeout();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const on_dur_timer_c& obj);
void from_json(const nlohmann::ordered_json& in_j, on_dur_timer_c& obj);

// OperationalState ::= ENUMERATED
struct operational_state_opts {
  enum options { enabled, disabled, nulltype } value;

  const char* to_string() const;
  bool        from_string(const std::string& str);
};
using operational_state_e = enumerated<operational_state_opts>;

// OruCapabilities ::= SEQUENCE
struct oru_cap_s {
  bool                            energy_saving_cap_common_info_present = false;
  bool                            asm_cap_info_present                  = false;
  bool                            trx_ctrl_cap_info_present             = false;
  energy_saving_cap_common_info_s energy_saving_cap_common_info;
  asm_cap_info_s                  asm_cap_info;
  trx_ctrl_cap_info_s             trx_ctrl_cap_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const oru_cap_s& obj);
void from_json(const nlohmann::ordered_json& in_j, oru_cap_s& obj);

// OruUserPlaneConfiguration ::= SEQUENCE
struct oru_user_plane_cfg_s {
  tx_array_list_l tx_array_list;
  rx_array_list_l rx_array_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const oru_user_plane_cfg_s& obj);
void from_json(const nlohmann::ordered_json& in_j, oru_user_plane_cfg_s& obj);

// PartitionList ::= SEQUENCE (SIZE (1..64)) OF PartitionList-item
using partition_list_l = dyn_array<partition_list_item_s_>;

// PerfObjectiveList ::= SEQUENCE (SIZE (1..64)) OF PerfObjective
using perf_objive_list_l = dyn_array<perf_objive_s>;

// PrbBlankingConfigurationList ::= SEQUENCE (SIZE (1..64)) OF PrbBlankingConfigItem
using prb_blanking_cfg_list_l = dyn_array<prb_blanking_cfg_item_s>;

// QueryStyle ::= SEQUENCE
struct query_style_s {
  int64_t                              query_service_style_type = 0;
  printable_string<0, 150, true, true> query_service_style_name;
  int64_t                              query_service_hdr_format_type        = 0;
  int64_t                              query_service_definition_format_type = 0;
  int64_t                              query_service_outcome_format_type    = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const query_style_s& obj);
void from_json(const nlohmann::ordered_json& in_j, query_style_s& obj);

// ReportStyle ::= SEQUENCE
struct report_style_s {
  using list_of_supported_event_trigger_styles_for_report_style_l_ = dyn_array<event_trigger_style_type_s>;

  // member variables
  int64_t                                                    report_service_style_type = 0;
  printable_string<1, 150, true, true>                       report_service_style_name;
  list_of_supported_event_trigger_styles_for_report_style_l_ list_of_supported_event_trigger_styles_for_report_style;
  int64_t                                                    report_service_action_definition_format_type = 0;
  int64_t                                                    report_service_ind_hdr_format_type           = 0;
  int64_t                                                    report_service_ind_msg_format_type           = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const report_style_s& obj);
void from_json(const nlohmann::ordered_json& in_j, report_style_s& obj);

// ResourceType ::= ENUMERATED
struct res_type_opts {
  enum options { prb_dl, prb_ul, drb, rrc, nulltype } value;

  const char* to_string() const;
  bool        from_string(const std::string& str);
};
using res_type_e = enumerated<res_type_opts>;

// RrmPolicyMemberList ::= SEQUENCE (SIZE (1..64)) OF RrmPolicyMember
using rrm_policy_member_list_l = dyn_array<rrm_policy_member_s>;

// ListOfSupportedControlStyles ::= SEQUENCE (SIZE (1..64)) OF ControlStyle
using list_of_supported_ctrl_styles_l = dyn_array<ctrl_style_s>;

// ListOfSupportedEventTriggerStyles ::= SEQUENCE (SIZE (1..64)) OF EventTriggerStyle
using list_of_supported_event_trigger_styles_l = dyn_array<event_trigger_style_s>;

// ListOfSupportedQueryStyles ::= SEQUENCE (SIZE (1..64)) OF QueryStyle
using list_of_supported_query_styles_l = dyn_array<query_style_s>;

// ListOfSupportedReportStyles ::= SEQUENCE (SIZE (1..64)) OF ReportStyle
using list_of_supported_report_styles_l = dyn_array<report_style_s>;

// O-CellDTXDRXConfig ::= SEQUENCE
struct o_cell_dtxdrx_cfg_s {
  struct cfg_type_opts {
    enum options { dtx, drx, dtxdrx, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using cfg_type_e_ = enumerated<cfg_type_opts>;
  struct activation_status_opts {
    enum options { activ, deactiv, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using activation_status_e_ = enumerated<activation_status_opts>;
  struct l1_activation_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using l1_activation_e_ = enumerated<l1_activation_opts>;

  // member variables
  bool                 on_dur_timer_present       = false;
  bool                 cycle_start_offset_present = false;
  bool                 slot_offset_present        = false;
  bool                 cfg_type_present           = false;
  bool                 activation_status_present  = false;
  bool                 l1_activation_present      = false;
  on_dur_timer_c       on_dur_timer;
  cycle_start_offset_s cycle_start_offset;
  uint8_t              slot_offset = 0;
  cfg_type_e_          cfg_type;
  activation_status_e_ activation_status;
  l1_activation_e_     l1_activation;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_cell_dtxdrx_cfg_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_cell_dtxdrx_cfg_s& obj);

// O-CESManagementFunction ::= SEQUENCE
struct o_ces_management_function_s {
  struct energy_saving_state_opts {
    enum options { is_not_energy_saving, is_energy_saving, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using energy_saving_state_e_ = enumerated<energy_saving_state_opts>;
  struct energy_saving_ctrl_opts {
    enum options { to_be_energy_saving, to_be_not_energy_saving, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using energy_saving_ctrl_e_ = enumerated<energy_saving_ctrl_opts>;

  // member variables
  bool                   ces_switch_present          = false;
  bool                   energy_saving_state_present = false;
  bool                   energy_saving_ctrl_present  = false;
  bool                   ces_switch                  = false;
  energy_saving_state_e_ energy_saving_state;
  energy_saving_ctrl_e_  energy_saving_ctrl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_ces_management_function_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_ces_management_function_s& obj);

// O-GnbCuCpFunction ::= SEQUENCE
struct o_gnb_cu_cp_function_s {
  bool                                 gnb_id_present      = false;
  bool                                 gnb_id_len_present  = false;
  bool                                 gnb_cu_name_present = false;
  bool                                 plmn_id_present     = false;
  uint64_t                             gnb_id              = 0;
  uint8_t                              gnb_id_len          = 22;
  printable_string<1, 150, true, true> gnb_cu_name;
  plmn_id_s                            plmn_id;
  g_enb_id_list_l                      x2_block_list;
  g_gnb_id_list_l                      xn_block_list;
  g_enb_id_list_l                      x2_allow_list;
  g_gnb_id_list_l                      xn_allow_list;
  g_enb_id_list_l                      x2_ho_block_list;
  g_gnb_id_list_l                      xn_ho_block_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_gnb_cu_cp_function_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_gnb_cu_cp_function_s& obj);

// O-GnbCuUpFunction ::= SEQUENCE
struct o_gnb_cu_up_function_s {
  bool             gnb_id_present       = false;
  bool             gnb_id_len_present   = false;
  bool             gnb_cu_up_id_present = false;
  uint64_t         gnb_id               = 0;
  uint8_t          gnb_id_len           = 22;
  uint64_t         gnb_cu_up_id         = 0;
  plmn_info_list_l plmn_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_gnb_cu_up_function_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_gnb_cu_up_function_s& obj);

// O-GnbDuFunction ::= SEQUENCE
struct o_gnb_du_function_s {
  bool                                 gnb_du_id_present   = false;
  bool                                 gnb_du_name_present = false;
  bool                                 gnb_id_present      = false;
  bool                                 gnb_id_len_present  = false;
  uint64_t                             gnb_du_id           = 0;
  printable_string<1, 150, true, true> gnb_du_name;
  uint64_t                             gnb_id     = 0;
  uint8_t                              gnb_id_len = 22;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_gnb_du_function_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_gnb_du_function_s& obj);

// O-NESPolicy ::= SEQUENCE
struct o_nes_policy_s {
  struct policy_type_opts {
    enum options { ASM, trx, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using policy_type_e_ = enumerated<policy_type_opts>;
  struct sleep_mode_opts {
    enum options { sleep_mode0, sleep_mode1, sleep_mode2, sleep_mode3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using sleep_mode_e_ = enumerated<sleep_mode_opts>;
  struct data_dir_opts {
    enum options { dl, ul, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using data_dir_e_ = enumerated<data_dir_opts>;

  // member variables
  bool                                 policy_type_present   = false;
  bool                                 ant_mask_name_present = false;
  bool                                 ant_mask_present      = false;
  bool                                 sleep_mode_present    = false;
  bool                                 data_dir_present      = false;
  bool                                 symbol_mask_present   = false;
  bool                                 slot_mask_present     = false;
  bool                                 valid_dur_present     = false;
  bool                                 es_objive_present     = false;
  policy_type_e_                       policy_type;
  printable_string<1, 150, true, true> ant_mask_name;
  int64_t                              ant_mask = 0;
  sleep_mode_e_                        sleep_mode;
  data_dir_e_                          data_dir;
  uint16_t                             symbol_mask = 0;
  int64_t                              slot_mask   = 0;
  int64_t                              valid_dur   = 0;
  es_objive_c                          es_objive;
  perf_objive_list_l                   perf_objive_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_nes_policy_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_nes_policy_s& obj);

// O-NrCellCu ::= SEQUENCE
struct o_nr_cell_cu_s {
  bool             cell_local_id_present = false;
  int64_t          cell_local_id         = 0;
  plmn_info_list_l plmn_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_nr_cell_cu_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_nr_cell_cu_s& obj);

// O-NrCellDu ::= SEQUENCE
struct o_nr_cell_du_s {
  using bwp_list_l_ = dyn_array<o_bwp_s>;

  // member variables
  bool                   cell_local_id_present           = false;
  bool                   operational_state_present       = false;
  bool                   administrative_state_present    = false;
  bool                   cell_state_present              = false;
  bool                   nr_pci_present                  = false;
  bool                   nr_tac_present                  = false;
  bool                   arfcn_dl_present                = false;
  bool                   arfcn_ul_present                = false;
  bool                   arfcn_sul_present               = false;
  bool                   bs_ch_bw_dl_present             = false;
  bool                   ssb_freq_present                = false;
  bool                   ssb_periodicity_present         = false;
  bool                   ssb_sub_carrier_spacing_present = false;
  bool                   ssb_offset_present              = false;
  bool                   ssb_dur_present                 = false;
  bool                   bs_ch_bw_ul_present             = false;
  bool                   bs_ch_bw_sul_present            = false;
  int64_t                cell_local_id                   = 0;
  operational_state_e    operational_state;
  administrative_state_e administrative_state;
  cell_state_e           cell_state;
  plmn_info_list_l       plmn_info_list;
  uint16_t               nr_pci                  = 0;
  uint32_t               nr_tac                  = 0;
  int64_t                arfcn_dl                = 0;
  int64_t                arfcn_ul                = 0;
  int64_t                arfcn_sul               = 0;
  int64_t                bs_ch_bw_dl             = 0;
  uint32_t               ssb_freq                = 0;
  int64_t                ssb_periodicity         = 0;
  int64_t                ssb_sub_carrier_spacing = 0;
  uint8_t                ssb_offset              = 0;
  int64_t                ssb_dur                 = 0;
  int64_t                bs_ch_bw_ul             = 0;
  int64_t                bs_ch_bw_sul            = 0;
  bwp_list_l_            bwp_list;
  partition_list_l       partition_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_nr_cell_du_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_nr_cell_du_s& obj);

// O-PRBBlankingPolicy ::= SEQUENCE
struct o_prb_blanking_policy_s {
  struct policy_category_opts {
    enum options { prb_blanking_cfg, prb_blanking_objive, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using policy_category_e_ = enumerated<policy_category_opts>;
  struct link_dir_opts {
    enum options { dl, ul, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using link_dir_e_ = enumerated<link_dir_opts>;

  // member variables
  bool                     policy_category_present  = false;
  bool                     link_dir_present         = false;
  bool                     start_time_present       = false;
  bool                     end_time_present         = false;
  bool                     prb_blanking_obj_present = false;
  policy_category_e_       policy_category;
  link_dir_e_              link_dir;
  fixed_octstring<8, true> start_time;
  fixed_octstring<8, true> end_time;
  prb_blanking_cfg_list_l  prb_blanking_cfg_list;
  real_s                   prb_blanking_obj;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_prb_blanking_policy_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_prb_blanking_policy_s& obj);

// O-RRMPolicyRatio ::= SEQUENCE
struct o_rrm_policy_ratio_s {
  bool                     res_type_present             = false;
  bool                     rrm_policy_max_ratio_present = false;
  bool                     rrm_policy_min_ratio_present = false;
  bool                     rrm_policy_ded_ratio_present = false;
  res_type_e               res_type;
  rrm_policy_member_list_l rrm_policy_member_list;
  int64_t                  rrm_policy_max_ratio = 0;
  int64_t                  rrm_policy_min_ratio = 0;
  int64_t                  rrm_policy_ded_ratio = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_rrm_policy_ratio_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_rrm_policy_ratio_s& obj);

// O-RUInfo ::= SEQUENCE
struct o_ru_info_s {
  bool                 oru_user_plane_cfg_present = false;
  bool                 oru_cap_present            = false;
  oru_user_plane_cfg_s oru_user_plane_cfg;
  oru_cap_s            oru_cap;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const o_ru_info_s& obj);
void from_json(const nlohmann::ordered_json& in_j, o_ru_info_s& obj);

// ControlService ::= SEQUENCE
struct ctrl_service_s {
  list_of_supported_ctrl_styles_l list_of_supported_ctrl_styles;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ctrl_service_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ctrl_service_s& obj);

// E2SM-CCC-RAN-Configuration-Structure ::= CHOICE
struct e2_sm_ccc_ran_cfg_structure_c {
  struct types_opts {
    enum options {
      o_gnb_cu_cp_function,
      o_gnb_cu_up_function,
      o_gnb_du_function,
      o_nr_cell_cu,
      o_nr_cell_du,
      o_rrm_policy_ratio,
      o_bwp,
      o_ces_management_function,
      o_nes_policy,
      o_cell_dtxdrx_cfg,
      o_ru_info,
      o_prb_blanking_policy,
      nulltype
    } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using types = enumerated<types_opts>;

  // choice methods
  e2_sm_ccc_ran_cfg_structure_c() = default;
  e2_sm_ccc_ran_cfg_structure_c(const e2_sm_ccc_ran_cfg_structure_c& other);
  e2_sm_ccc_ran_cfg_structure_c& operator=(const e2_sm_ccc_ran_cfg_structure_c& other);
  ~e2_sm_ccc_ran_cfg_structure_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  o_gnb_cu_cp_function_s& o_gnb_cu_cp_function()
  {
    assert_choice_type(types::o_gnb_cu_cp_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_cp_function_s>();
  }
  o_gnb_cu_up_function_s& o_gnb_cu_up_function()
  {
    assert_choice_type(types::o_gnb_cu_up_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_up_function_s>();
  }
  o_gnb_du_function_s& o_gnb_du_function()
  {
    assert_choice_type(types::o_gnb_du_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_du_function_s>();
  }
  o_nr_cell_cu_s& o_nr_cell_cu()
  {
    assert_choice_type(types::o_nr_cell_cu, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_cu_s>();
  }
  o_nr_cell_du_s& o_nr_cell_du()
  {
    assert_choice_type(types::o_nr_cell_du, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_du_s>();
  }
  o_rrm_policy_ratio_s& o_rrm_policy_ratio()
  {
    assert_choice_type(types::o_rrm_policy_ratio, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_rrm_policy_ratio_s>();
  }
  o_bwp_s& o_bwp()
  {
    assert_choice_type(types::o_bwp, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_bwp_s>();
  }
  o_ces_management_function_s& o_ces_management_function()
  {
    assert_choice_type(types::o_ces_management_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ces_management_function_s>();
  }
  o_nes_policy_s& o_nes_policy()
  {
    assert_choice_type(types::o_nes_policy, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nes_policy_s>();
  }
  o_cell_dtxdrx_cfg_s& o_cell_dtxdrx_cfg()
  {
    assert_choice_type(types::o_cell_dtxdrx_cfg, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_cell_dtxdrx_cfg_s>();
  }
  o_ru_info_s& o_ru_info()
  {
    assert_choice_type(types::o_ru_info, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ru_info_s>();
  }
  o_prb_blanking_policy_s& o_prb_blanking_policy()
  {
    assert_choice_type(types::o_prb_blanking_policy, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_prb_blanking_policy_s>();
  }
  const o_gnb_cu_cp_function_s& o_gnb_cu_cp_function() const
  {
    assert_choice_type(types::o_gnb_cu_cp_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_cp_function_s>();
  }
  const o_gnb_cu_up_function_s& o_gnb_cu_up_function() const
  {
    assert_choice_type(types::o_gnb_cu_up_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_up_function_s>();
  }
  const o_gnb_du_function_s& o_gnb_du_function() const
  {
    assert_choice_type(types::o_gnb_du_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_du_function_s>();
  }
  const o_nr_cell_cu_s& o_nr_cell_cu() const
  {
    assert_choice_type(types::o_nr_cell_cu, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_cu_s>();
  }
  const o_nr_cell_du_s& o_nr_cell_du() const
  {
    assert_choice_type(types::o_nr_cell_du, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_du_s>();
  }
  const o_rrm_policy_ratio_s& o_rrm_policy_ratio() const
  {
    assert_choice_type(types::o_rrm_policy_ratio, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_rrm_policy_ratio_s>();
  }
  const o_bwp_s& o_bwp() const
  {
    assert_choice_type(types::o_bwp, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_bwp_s>();
  }
  const o_ces_management_function_s& o_ces_management_function() const
  {
    assert_choice_type(types::o_ces_management_function, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ces_management_function_s>();
  }
  const o_nes_policy_s& o_nes_policy() const
  {
    assert_choice_type(types::o_nes_policy, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nes_policy_s>();
  }
  const o_cell_dtxdrx_cfg_s& o_cell_dtxdrx_cfg() const
  {
    assert_choice_type(types::o_cell_dtxdrx_cfg, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_cell_dtxdrx_cfg_s>();
  }
  const o_ru_info_s& o_ru_info() const
  {
    assert_choice_type(types::o_ru_info, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ru_info_s>();
  }
  const o_prb_blanking_policy_s& o_prb_blanking_policy() const
  {
    assert_choice_type(types::o_prb_blanking_policy, type_, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_prb_blanking_policy_s>();
  }
  o_gnb_cu_cp_function_s&      set_o_gnb_cu_cp_function();
  o_gnb_cu_up_function_s&      set_o_gnb_cu_up_function();
  o_gnb_du_function_s&         set_o_gnb_du_function();
  o_nr_cell_cu_s&              set_o_nr_cell_cu();
  o_nr_cell_du_s&              set_o_nr_cell_du();
  o_rrm_policy_ratio_s&        set_o_rrm_policy_ratio();
  o_bwp_s&                     set_o_bwp();
  o_ces_management_function_s& set_o_ces_management_function();
  o_nes_policy_s&              set_o_nes_policy();
  o_cell_dtxdrx_cfg_s&         set_o_cell_dtxdrx_cfg();
  o_ru_info_s&                 set_o_ru_info();
  o_prb_blanking_policy_s&     set_o_prb_blanking_policy();

private:
  types type_;
  choice_buffer_t<o_bwp_s,
                  o_cell_dtxdrx_cfg_s,
                  o_ces_management_function_s,
                  o_gnb_cu_cp_function_s,
                  o_gnb_cu_up_function_s,
                  o_gnb_du_function_s,
                  o_nes_policy_s,
                  o_nr_cell_cu_s,
                  o_nr_cell_du_s,
                  o_prb_blanking_policy_s,
                  o_rrm_policy_ratio_s,
                  o_ru_info_s>
      c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ran_cfg_structure_c& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ran_cfg_structure_c& obj);

// EventTrigger ::= SEQUENCE
struct event_trigger_s {
  list_of_supported_event_trigger_styles_l list_of_supported_event_trigger_styles;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const event_trigger_s& obj);
void from_json(const nlohmann::ordered_json& in_j, event_trigger_s& obj);

// QueryService ::= SEQUENCE
struct query_service_s {
  list_of_supported_query_styles_l list_of_supported_query_styles;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const query_service_s& obj);
void from_json(const nlohmann::ordered_json& in_j, query_service_s& obj);

// ReportService ::= SEQUENCE
struct report_service_s {
  list_of_supported_report_styles_l list_of_supported_report_styles;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const report_service_s& obj);
void from_json(const nlohmann::ordered_json& in_j, report_service_s& obj);

// AttributeName ::= SEQUENCE
struct attribute_name_s {
  printable_string<1, 150, true, true> attribute_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const attribute_name_s& obj);
void from_json(const nlohmann::ordered_json& in_j, attribute_name_s& obj);

// Cause ::= ENUMERATED
struct cause_opts {
  enum options {
    not_supported,
    not_available,
    incompatible_state,
    json_error,
    semantic_error,
    unspecified,
    nulltype
  } value;

  const char* to_string() const;
  bool        from_string(const std::string& str);
};
using cause_e = enumerated<cause_opts>;

// RICServices ::= SEQUENCE
struct ric_services_s {
  bool             event_trigger_present  = false;
  bool             report_service_present = false;
  bool             insert_service_present = false;
  bool             ctrl_service_present   = false;
  bool             policy_service_present = false;
  bool             query_service_present  = false;
  event_trigger_s  event_trigger;
  report_service_s report_service;
  int64_t          insert_service = 0;
  ctrl_service_s   ctrl_service;
  int64_t          policy_service = 0;
  query_service_s  query_service;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_services_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_services_s& obj);

// ValuesOfAttributes ::= SEQUENCE
struct values_of_attributes_s {
  e2_sm_ccc_ran_cfg_structure_c ran_cfg_structure;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const values_of_attributes_s& obj);
void from_json(const nlohmann::ordered_json& in_j, values_of_attributes_s& obj);

// Attribute ::= SEQUENCE
struct attribute_s {
  printable_string<1, 150, true, true> attribute_name;
  ric_services_s                       supported_services;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const attribute_s& obj);
void from_json(const nlohmann::ordered_json& in_j, attribute_s& obj);

// CelllevelConfigurationStructuresForQuery ::= SEQUENCE
struct celllevel_cfg_structures_for_query_s {
  using list_of_attributes_l_ = dyn_array<attribute_name_s>;

  // member variables
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const celllevel_cfg_structures_for_query_s& obj);
void from_json(const nlohmann::ordered_json& in_j, celllevel_cfg_structures_for_query_s& obj);

// ConfigurationStructureAccepted ::= SEQUENCE
struct cfg_structure_accepted_s {
  bool                                 applied_timestamp_present = false;
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               old_values_of_attributes;
  values_of_attributes_s               current_values_of_attributes;
  fixed_octstring<8, true>             applied_timestamp;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cfg_structure_accepted_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cfg_structure_accepted_s& obj);

// ConfigurationStructureFailed ::= SEQUENCE
struct cfg_structure_failed_s {
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               old_values_of_attributes;
  values_of_attributes_s               requested_values_of_attributes;
  cause_e                              cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cfg_structure_failed_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cfg_structure_failed_s& obj);

// ConfigurationStructure ::= SEQUENCE
struct cfg_structure_s {
  struct change_type_opts {
    enum options { none, mod, addition, deletion, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using change_type_e_ = enumerated<change_type_opts>;

  // member variables
  bool                                 old_values_of_attributes_present = false;
  change_type_e_                       change_type;
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               values_of_attributes;
  values_of_attributes_s               old_values_of_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cfg_structure_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cfg_structure_s& obj);

// ConfigurationStructureWrite ::= SEQUENCE
struct cfg_structure_write_s {
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               old_values_of_attributes;
  values_of_attributes_s               new_values_of_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cfg_structure_write_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cfg_structure_write_s& obj);

// ConfigurationStructuresToBeReportedForQuery ::= SEQUENCE
struct cfg_structures_to_be_reported_for_query_s {
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               values_of_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cfg_structures_to_be_reported_for_query_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cfg_structures_to_be_reported_for_query_s& obj);

// EUTRA-CGI ::= SEQUENCE
struct eutra_cgi_s {
  plmn_id_s                        plmn_id;
  fixed_bitstring<28, false, true> eutra_cell_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const eutra_cgi_s& obj);
void from_json(const nlohmann::ordered_json& in_j, eutra_cgi_s& obj);

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  plmn_id_s                        plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const nr_cgi_s& obj);
void from_json(const nlohmann::ordered_json& in_j, nr_cgi_s& obj);

// RANConfigurationStructureForADF ::= SEQUENCE
struct ran_cfg_structure_for_adf_s {
  struct report_type_opts {
    enum options { all, change, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using report_type_e_        = enumerated<report_type_opts>;
  using list_of_attributes_l_ = dyn_array<attribute_name_s>;

  // member variables
  report_type_e_                       report_type;
  printable_string<0, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ran_cfg_structure_for_adf_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ran_cfg_structure_for_adf_s& obj);

// RANConfigurationStructureForEventTrigger ::= SEQUENCE
struct ran_cfg_structure_for_event_trigger_s {
  using list_of_attributes_l_ = dyn_array<printable_string<0, 150, true, true>>;

  // member variables
  printable_string<0, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ran_cfg_structure_for_event_trigger_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ran_cfg_structure_for_event_trigger_s& obj);

// CellGlobalId ::= CHOICE
struct cell_global_id_c {
  struct types_opts {
    enum options { nr_cgi, eutra_cgi, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using types = enumerated<types_opts>;

  // choice methods
  cell_global_id_c() = default;
  cell_global_id_c(const cell_global_id_c& other);
  cell_global_id_c& operator=(const cell_global_id_c& other);
  ~cell_global_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_cgi_s& nr_cgi()
  {
    assert_choice_type(types::nr_cgi, type_, "CellGlobalId");
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& eutra_cgi()
  {
    assert_choice_type(types::eutra_cgi, type_, "CellGlobalId");
    return c.get<eutra_cgi_s>();
  }
  const nr_cgi_s& nr_cgi() const
  {
    assert_choice_type(types::nr_cgi, type_, "CellGlobalId");
    return c.get<nr_cgi_s>();
  }
  const eutra_cgi_s& eutra_cgi() const
  {
    assert_choice_type(types::eutra_cgi, type_, "CellGlobalId");
    return c.get<eutra_cgi_s>();
  }
  nr_cgi_s&    set_nr_cgi();
  eutra_cgi_s& set_eutra_cgi();

private:
  types                                  type_;
  choice_buffer_t<eutra_cgi_s, nr_cgi_s> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const cell_global_id_c& obj);
void from_json(const nlohmann::ordered_json& in_j, cell_global_id_c& obj);

// ListOfCelllevelConfigurationStructuresForQuery ::= SEQUENCE (SIZE (1..64)) OF
// CelllevelConfigurationStructuresForQuery
using list_of_celllevel_cfg_structures_for_query_l = dyn_array<celllevel_cfg_structures_for_query_s>;

// ListOfConfigurationStructures ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructureWrite
using list_of_cfg_structures_l = dyn_array<cfg_structure_write_s>;

// ListOfConfigurationStructuresToBeReportedForQuery ::= SEQUENCE (SIZE (1..64)) OF
// ConfigurationStructuresToBeReportedForQuery
using list_of_cfg_structures_to_be_reported_for_query_l = dyn_array<cfg_structures_to_be_reported_for_query_s>;

// ListOfConfigurationsReported ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructure
using list_of_cfgs_reported_l = dyn_array<cfg_structure_s>;

// ListOfRANConfigurationStructuresForADF ::= SEQUENCE (SIZE (1..64)) OF RANConfigurationStructureForADF
using list_of_ran_cfg_structures_for_adf_l = dyn_array<ran_cfg_structure_for_adf_s>;

// ListOfRANConfigurationStructuresForEventTrigger ::= SEQUENCE (SIZE (1..64)) OF
// RANConfigurationStructureForEventTrigger
using list_of_ran_cfg_structures_for_event_trigger_l = dyn_array<ran_cfg_structure_for_event_trigger_s>;

// ListOfSupportedAttributes ::= SEQUENCE (SIZE (1..64)) OF Attribute
using list_of_supported_attributes_l = dyn_array<attribute_s>;

// RanConfigurationStructuresAcceptedList ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructureAccepted
using ran_cfg_structures_accepted_list_l = dyn_array<cfg_structure_accepted_s>;

// RanConfigurationStructuresFailedList ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructureFailed
using ran_cfg_structures_failed_list_l = dyn_array<cfg_structure_failed_s>;

// CellConfigurationToBeReportedForADF ::= SEQUENCE
struct cell_cfg_to_be_reported_for_adf_s {
  bool                                 cell_global_id_present = false;
  cell_global_id_c                     cell_global_id;
  list_of_ran_cfg_structures_for_adf_l list_of_cell_level_ran_cfg_structures_for_adf;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cell_cfg_to_be_reported_for_adf_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cell_cfg_to_be_reported_for_adf_s& obj);

// CellControlOutcome ::= SEQUENCE
struct cell_ctrl_outcome_s {
  cell_global_id_c                   cell_global_id;
  ran_cfg_structures_accepted_list_l ran_cfg_structures_accepted_list;
  ran_cfg_structures_failed_list_l   ran_cfg_structures_failed_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cell_ctrl_outcome_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cell_ctrl_outcome_s& obj);

// CellControlled ::= SEQUENCE
struct cell_ctrl_s {
  cell_global_id_c         cell_global_id;
  list_of_cfg_structures_l list_of_cfg_structures;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cell_ctrl_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cell_ctrl_s& obj);

// CellLevelConfigurationStructureForEventTrigger ::= SEQUENCE
struct cell_level_cfg_structure_for_event_trigger_s {
  bool                                           cell_global_id_present = false;
  cell_global_id_c                               cell_global_id;
  list_of_ran_cfg_structures_for_event_trigger_l list_of_ran_cfg_structures_for_event_trigger;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cell_level_cfg_structure_for_event_trigger_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cell_level_cfg_structure_for_event_trigger_s& obj);

// CellReported ::= SEQUENCE
struct cell_reported_s {
  cell_global_id_c        cell_global_id;
  list_of_cfgs_reported_l list_of_cfg_structures_reported;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cell_reported_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cell_reported_s& obj);

// CellsToBeQueried ::= SEQUENCE
struct cells_to_be_queried_s {
  bool                                         cell_global_id_present = false;
  cell_global_id_c                             cell_global_id;
  list_of_celllevel_cfg_structures_for_query_l list_of_celllevel_cfg_structures_for_query;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cells_to_be_queried_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cells_to_be_queried_s& obj);

// CellsToBeReportedForQuery ::= SEQUENCE
struct cells_to_be_reported_for_query_s {
  cell_global_id_c                                  cell_global_id;
  list_of_cfg_structures_to_be_reported_for_query_l list_of_cfg_structures_to_be_reported_for_query;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cells_to_be_reported_for_query_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cells_to_be_reported_for_query_s& obj);

// NodelevelRanConfigurationStructuresForQuery ::= SEQUENCE
struct nodelevel_ran_cfg_structures_for_query_s {
  using list_of_attributes_l_ = dyn_array<attribute_name_s>;

  // member variables
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const nodelevel_ran_cfg_structures_for_query_s& obj);
void from_json(const nlohmann::ordered_json& in_j, nodelevel_ran_cfg_structures_for_query_s& obj);

// RANConfigurationStructure ::= SEQUENCE
struct ran_cfg_structure_s {
  printable_string<0, 150, true, true> ran_cfg_structure_name;
  list_of_supported_attributes_l       list_of_supported_attributes;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ran_cfg_structure_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ran_cfg_structure_s& obj);

// ListOfCellConfigurationsToBeReportedForADF ::= SEQUENCE (SIZE (1..64)) OF CellConfigurationToBeReportedForADF
using list_of_cell_cfgs_to_be_reported_for_adf_l = dyn_array<cell_cfg_to_be_reported_for_adf_s>;

// ListOfCellLevelConfigurationStructuresForEventTrigger ::= SEQUENCE (SIZE (1..64)) OF
// CellLevelConfigurationStructureForEventTrigger
using list_of_cell_level_cfg_structures_for_event_trigger_l = dyn_array<cell_level_cfg_structure_for_event_trigger_s>;

// ListOfCellsControlled ::= SEQUENCE (SIZE (1..64)) OF CellControlled
using list_of_cells_ctrl_l = dyn_array<cell_ctrl_s>;

// ListOfCellsForControlOutcome ::= SEQUENCE (SIZE (1..64)) OF CellControlOutcome
using list_of_cells_for_ctrl_outcome_l = dyn_array<cell_ctrl_outcome_s>;

// ListOfCellsReported ::= SEQUENCE (SIZE (1..64)) OF CellReported
using list_of_cells_reported_l = dyn_array<cell_reported_s>;

// ListOfCellsToBeQueriedForQuery ::= SEQUENCE (SIZE (1..64)) OF CellsToBeQueried
using list_of_cells_to_be_queried_for_query_l = dyn_array<cells_to_be_queried_s>;

// ListOfCellsToBeReportedForQuery ::= SEQUENCE (SIZE (1..64)) OF CellsToBeReportedForQuery
using list_of_cells_to_be_reported_for_query_l = dyn_array<cells_to_be_reported_for_query_s>;

// ListOfNodelevelRanConfigurationStructuresForQuery ::= SEQUENCE (SIZE (1..64)) OF
// NodelevelRanConfigurationStructuresForQuery
using list_of_nodelevel_ran_cfg_structures_for_query_l = dyn_array<nodelevel_ran_cfg_structures_for_query_s>;

// ListOfSupportedRANConfigurationStructures ::= SEQUENCE (SIZE (1..64)) OF RANConfigurationStructure
using list_of_supported_ran_cfg_structures_l = dyn_array<ran_cfg_structure_s>;

// CellForRANFunctionDefinition ::= SEQUENCE
struct cell_for_ran_function_definition_s {
  cell_global_id_c                       cell_global_id;
  list_of_supported_ran_cfg_structures_l list_of_supported_cell_level_ran_cfg_structures;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const cell_for_ran_function_definition_s& obj);
void from_json(const nlohmann::ordered_json& in_j, cell_for_ran_function_definition_s& obj);

// E2SM-CCC-ActionDefinitionFormat1 ::= SEQUENCE
struct e2_sm_ccc_action_definition_format1_s {
  list_of_ran_cfg_structures_for_adf_l list_of_node_level_ran_cfg_structures_for_adf;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_action_definition_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_action_definition_format1_s& obj);

// E2SM-CCC-ActionDefinitionFormat2 ::= SEQUENCE
struct e2_sm_ccc_action_definition_format2_s {
  list_of_cell_cfgs_to_be_reported_for_adf_l list_of_cell_cfgs_to_be_reported_for_adf;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_action_definition_format2_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_action_definition_format2_s& obj);

// E2SM-CCC-ControlHeaderFormat1 ::= SEQUENCE
struct e2_sm_ccc_ctrl_hdr_format1_s {
  int64_t ric_style_type = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_hdr_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_hdr_format1_s& obj);

// E2SM-CCC-ControlMessageFormat1 ::= SEQUENCE
struct e2_sm_ccc_ctrl_msg_format1_s {
  list_of_cfg_structures_l list_of_cfg_structures;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_msg_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_msg_format1_s& obj);

// E2SM-CCC-ControlMessageFormat2 ::= SEQUENCE
struct e2_sm_ccc_ctrl_msg_format2_s {
  list_of_cells_ctrl_l list_of_cells_ctrl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_msg_format2_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_msg_format2_s& obj);

// E2SM-CCC-ControlOutcomeFormat1 ::= SEQUENCE
struct e2_sm_ccc_ctrl_outcome_format1_s {
  bool                               rx_timestamp_present = false;
  fixed_octstring<8, true>           rx_timestamp;
  ran_cfg_structures_accepted_list_l ran_cfg_structures_accepted_list;
  ran_cfg_structures_failed_list_l   ran_cfg_structures_failed_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_outcome_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_outcome_format1_s& obj);

// E2SM-CCC-ControlOutcomeFormat2 ::= SEQUENCE
struct e2_sm_ccc_ctrl_outcome_format2_s {
  bool                             rx_timestamp_present = false;
  fixed_octstring<8, true>         rx_timestamp;
  list_of_cells_for_ctrl_outcome_l list_of_cells_for_ctrl_outcome;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ctrl_outcome_format2_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ctrl_outcome_format2_s& obj);

// E2SM-CCC-EventTriggerDefinition-Format1 ::= SEQUENCE
struct e2_sm_ccc_event_trigger_definition_format1_s {
  list_of_ran_cfg_structures_for_event_trigger_l list_of_node_level_cfg_structures_for_event_trigger;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_event_trigger_definition_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_event_trigger_definition_format1_s& obj);

// E2SM-CCC-EventTriggerDefinition-Format2 ::= SEQUENCE
struct e2_sm_ccc_event_trigger_definition_format2_s {
  list_of_cell_level_cfg_structures_for_event_trigger_l list_of_cell_level_cfg_structures_for_event_trigger;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_event_trigger_definition_format2_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_event_trigger_definition_format2_s& obj);

// E2SM-CCC-EventTriggerDefinition-Format3 ::= SEQUENCE
struct e2_sm_ccc_event_trigger_definition_format3_s {
  int64_t period = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_event_trigger_definition_format3_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_event_trigger_definition_format3_s& obj);

// E2SM-CCC-IndicationHeaderFormat1 ::= SEQUENCE
struct e2_sm_ccc_ind_hdr_format1_s {
  struct ind_reason_opts {
    enum options { upon_sub, upon_change, periodic, nulltype } value;

    const char* to_string() const;
    bool        from_string(const std::string& str);
  };
  using ind_reason_e_ = enumerated<ind_reason_opts>;

  // member variables
  ind_reason_e_            ind_reason;
  fixed_octstring<8, true> event_time;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ind_hdr_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ind_hdr_format1_s& obj);

// E2SM-CCC-IndicationMessageFormat1 ::= SEQUENCE
struct e2_sm_ccc_ind_msg_format1_s {
  list_of_cfgs_reported_l list_of_cfg_structures_reported;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ind_msg_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ind_msg_format1_s& obj);

// E2SM-CCC-IndicationMessageFormat2 ::= SEQUENCE
struct e2_sm_ccc_ind_msg_format2_s {
  list_of_cells_reported_l list_of_cells_reported;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_ind_msg_format2_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_ind_msg_format2_s& obj);

// E2SM-CCC-QueryDefinitionFormat1 ::= SEQUENCE
struct e2_sm_ccc_query_definition_format1_s {
  list_of_nodelevel_ran_cfg_structures_for_query_l list_of_nodelevel_ran_cfg_structures_for_query;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_definition_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_definition_format1_s& obj);

// E2SM-CCC-QueryDefinitionFormat2 ::= SEQUENCE
struct e2_sm_ccc_query_definition_format2_s {
  list_of_cells_to_be_queried_for_query_l list_of_cells_to_be_queried_for_query;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_definition_format2_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_definition_format2_s& obj);

// E2SM-CCC-QueryHeaderFormat1 ::= SEQUENCE
struct e2_sm_ccc_query_hdr_format1_s {
  int64_t ric_style_type = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_hdr_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_hdr_format1_s& obj);

// E2SM-CCC-QueryOutcomeFormat1 ::= SEQUENCE
struct e2_sm_ccc_query_outcome_format1_s {
  list_of_cfg_structures_to_be_reported_for_query_l list_of_cfg_structures_to_be_reported_for_query;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_outcome_format1_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_outcome_format1_s& obj);

// E2SM-CCC-QueryOutcomeFormat2 ::= SEQUENCE
struct e2_sm_ccc_query_outcome_format2_s {
  list_of_cells_to_be_reported_for_query_l list_of_cells_to_be_reported_for_query;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const e2_sm_ccc_query_outcome_format2_s& obj);
void from_json(const nlohmann::ordered_json& in_j, e2_sm_ccc_query_outcome_format2_s& obj);

// ActionDefinitionFormat ::= CHOICE
struct action_definition_format_c {
  struct types_opts {
    enum options { action_definition_format1, action_definition_format2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  action_definition_format_c() = default;
  action_definition_format_c(const action_definition_format_c& other);
  action_definition_format_c& operator=(const action_definition_format_c& other);
  ~action_definition_format_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_action_definition_format1_s& action_definition_format1()
  {
    assert_choice_type(types::action_definition_format1, type_, "ActionDefinitionFormat");
    return c.get<e2_sm_ccc_action_definition_format1_s>();
  }
  e2_sm_ccc_action_definition_format2_s& action_definition_format2()
  {
    assert_choice_type(types::action_definition_format2, type_, "ActionDefinitionFormat");
    return c.get<e2_sm_ccc_action_definition_format2_s>();
  }
  const e2_sm_ccc_action_definition_format1_s& action_definition_format1() const
  {
    assert_choice_type(types::action_definition_format1, type_, "ActionDefinitionFormat");
    return c.get<e2_sm_ccc_action_definition_format1_s>();
  }
  const e2_sm_ccc_action_definition_format2_s& action_definition_format2() const
  {
    assert_choice_type(types::action_definition_format2, type_, "ActionDefinitionFormat");
    return c.get<e2_sm_ccc_action_definition_format2_s>();
  }
  e2_sm_ccc_action_definition_format1_s& set_action_definition_format1();
  e2_sm_ccc_action_definition_format2_s& set_action_definition_format2();

private:
  types                                                                                         type_;
  choice_buffer_t<e2_sm_ccc_action_definition_format1_s, e2_sm_ccc_action_definition_format2_s> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const action_definition_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, action_definition_format_c& obj);

// ControlHeaderFormat ::= CHOICE
struct ctrl_hdr_format_c {
  struct types_opts {
    enum options { ctrl_hdr_format1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  types       type() const { return types::ctrl_hdr_format1; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_ctrl_hdr_format1_s&       ctrl_hdr_format1() { return c; }
  const e2_sm_ccc_ctrl_hdr_format1_s& ctrl_hdr_format1() const { return c; }

private:
  e2_sm_ccc_ctrl_hdr_format1_s c;
};

void to_json(nlohmann::ordered_json& out_j, const ctrl_hdr_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, ctrl_hdr_format_c& obj);

// ControlMessageFormat ::= CHOICE
struct ctrl_msg_format_c {
  struct types_opts {
    enum options { ctrl_msg_format1, ctrl_msg_format2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ctrl_msg_format_c() = default;
  ctrl_msg_format_c(const ctrl_msg_format_c& other);
  ctrl_msg_format_c& operator=(const ctrl_msg_format_c& other);
  ~ctrl_msg_format_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_ctrl_msg_format1_s& ctrl_msg_format1()
  {
    assert_choice_type(types::ctrl_msg_format1, type_, "ControlMessageFormat");
    return c.get<e2_sm_ccc_ctrl_msg_format1_s>();
  }
  e2_sm_ccc_ctrl_msg_format2_s& ctrl_msg_format2()
  {
    assert_choice_type(types::ctrl_msg_format2, type_, "ControlMessageFormat");
    return c.get<e2_sm_ccc_ctrl_msg_format2_s>();
  }
  const e2_sm_ccc_ctrl_msg_format1_s& ctrl_msg_format1() const
  {
    assert_choice_type(types::ctrl_msg_format1, type_, "ControlMessageFormat");
    return c.get<e2_sm_ccc_ctrl_msg_format1_s>();
  }
  const e2_sm_ccc_ctrl_msg_format2_s& ctrl_msg_format2() const
  {
    assert_choice_type(types::ctrl_msg_format2, type_, "ControlMessageFormat");
    return c.get<e2_sm_ccc_ctrl_msg_format2_s>();
  }
  e2_sm_ccc_ctrl_msg_format1_s& set_ctrl_msg_format1();
  e2_sm_ccc_ctrl_msg_format2_s& set_ctrl_msg_format2();

private:
  types                                                                       type_;
  choice_buffer_t<e2_sm_ccc_ctrl_msg_format1_s, e2_sm_ccc_ctrl_msg_format2_s> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const ctrl_msg_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, ctrl_msg_format_c& obj);

// ControlOutcomeFormat ::= CHOICE
struct ctrl_outcome_format_c {
  struct types_opts {
    enum options { ctrl_outcome_format1, ctrl_outcome_format2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ctrl_outcome_format_c() = default;
  ctrl_outcome_format_c(const ctrl_outcome_format_c& other);
  ctrl_outcome_format_c& operator=(const ctrl_outcome_format_c& other);
  ~ctrl_outcome_format_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_ctrl_outcome_format1_s& ctrl_outcome_format1()
  {
    assert_choice_type(types::ctrl_outcome_format1, type_, "ControlOutcomeFormat");
    return c.get<e2_sm_ccc_ctrl_outcome_format1_s>();
  }
  e2_sm_ccc_ctrl_outcome_format2_s& ctrl_outcome_format2()
  {
    assert_choice_type(types::ctrl_outcome_format2, type_, "ControlOutcomeFormat");
    return c.get<e2_sm_ccc_ctrl_outcome_format2_s>();
  }
  const e2_sm_ccc_ctrl_outcome_format1_s& ctrl_outcome_format1() const
  {
    assert_choice_type(types::ctrl_outcome_format1, type_, "ControlOutcomeFormat");
    return c.get<e2_sm_ccc_ctrl_outcome_format1_s>();
  }
  const e2_sm_ccc_ctrl_outcome_format2_s& ctrl_outcome_format2() const
  {
    assert_choice_type(types::ctrl_outcome_format2, type_, "ControlOutcomeFormat");
    return c.get<e2_sm_ccc_ctrl_outcome_format2_s>();
  }
  e2_sm_ccc_ctrl_outcome_format1_s& set_ctrl_outcome_format1();
  e2_sm_ccc_ctrl_outcome_format2_s& set_ctrl_outcome_format2();

private:
  types                                                                               type_;
  choice_buffer_t<e2_sm_ccc_ctrl_outcome_format1_s, e2_sm_ccc_ctrl_outcome_format2_s> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const ctrl_outcome_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, ctrl_outcome_format_c& obj);

// EventTriggerDefinitionFormat ::= CHOICE
struct event_trigger_definition_format_c {
  struct types_opts {
    enum options {
      event_trigger_definition_format1,
      event_trigger_definition_format2,
      event_trigger_definition_format3,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  event_trigger_definition_format_c() = default;
  event_trigger_definition_format_c(const event_trigger_definition_format_c& other);
  event_trigger_definition_format_c& operator=(const event_trigger_definition_format_c& other);
  ~event_trigger_definition_format_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_event_trigger_definition_format1_s& event_trigger_definition_format1()
  {
    assert_choice_type(types::event_trigger_definition_format1, type_, "EventTriggerDefinitionFormat");
    return c.get<e2_sm_ccc_event_trigger_definition_format1_s>();
  }
  e2_sm_ccc_event_trigger_definition_format2_s& event_trigger_definition_format2()
  {
    assert_choice_type(types::event_trigger_definition_format2, type_, "EventTriggerDefinitionFormat");
    return c.get<e2_sm_ccc_event_trigger_definition_format2_s>();
  }
  e2_sm_ccc_event_trigger_definition_format3_s& event_trigger_definition_format3()
  {
    assert_choice_type(types::event_trigger_definition_format3, type_, "EventTriggerDefinitionFormat");
    return c.get<e2_sm_ccc_event_trigger_definition_format3_s>();
  }
  const e2_sm_ccc_event_trigger_definition_format1_s& event_trigger_definition_format1() const
  {
    assert_choice_type(types::event_trigger_definition_format1, type_, "EventTriggerDefinitionFormat");
    return c.get<e2_sm_ccc_event_trigger_definition_format1_s>();
  }
  const e2_sm_ccc_event_trigger_definition_format2_s& event_trigger_definition_format2() const
  {
    assert_choice_type(types::event_trigger_definition_format2, type_, "EventTriggerDefinitionFormat");
    return c.get<e2_sm_ccc_event_trigger_definition_format2_s>();
  }
  const e2_sm_ccc_event_trigger_definition_format3_s& event_trigger_definition_format3() const
  {
    assert_choice_type(types::event_trigger_definition_format3, type_, "EventTriggerDefinitionFormat");
    return c.get<e2_sm_ccc_event_trigger_definition_format3_s>();
  }
  e2_sm_ccc_event_trigger_definition_format1_s& set_event_trigger_definition_format1();
  e2_sm_ccc_event_trigger_definition_format2_s& set_event_trigger_definition_format2();
  e2_sm_ccc_event_trigger_definition_format3_s& set_event_trigger_definition_format3();

private:
  types type_;
  choice_buffer_t<e2_sm_ccc_event_trigger_definition_format1_s,
                  e2_sm_ccc_event_trigger_definition_format2_s,
                  e2_sm_ccc_event_trigger_definition_format3_s>
      c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const event_trigger_definition_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, event_trigger_definition_format_c& obj);

// IndicationHeaderFormat ::= CHOICE
struct ind_hdr_format_c {
  struct types_opts {
    enum options { ind_hdr_format1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  types       type() const { return types::ind_hdr_format1; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_ind_hdr_format1_s&       ind_hdr_format1() { return c; }
  const e2_sm_ccc_ind_hdr_format1_s& ind_hdr_format1() const { return c; }

private:
  e2_sm_ccc_ind_hdr_format1_s c;
};

void to_json(nlohmann::ordered_json& out_j, const ind_hdr_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, ind_hdr_format_c& obj);

// IndicationMessageFormat ::= CHOICE
struct ind_msg_format_c {
  struct types_opts {
    enum options { ind_msg_format1, ind_msg_format2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ind_msg_format_c() = default;
  ind_msg_format_c(const ind_msg_format_c& other);
  ind_msg_format_c& operator=(const ind_msg_format_c& other);
  ~ind_msg_format_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_ind_msg_format1_s& ind_msg_format1()
  {
    assert_choice_type(types::ind_msg_format1, type_, "IndicationMessageFormat");
    return c.get<e2_sm_ccc_ind_msg_format1_s>();
  }
  e2_sm_ccc_ind_msg_format2_s& ind_msg_format2()
  {
    assert_choice_type(types::ind_msg_format2, type_, "IndicationMessageFormat");
    return c.get<e2_sm_ccc_ind_msg_format2_s>();
  }
  const e2_sm_ccc_ind_msg_format1_s& ind_msg_format1() const
  {
    assert_choice_type(types::ind_msg_format1, type_, "IndicationMessageFormat");
    return c.get<e2_sm_ccc_ind_msg_format1_s>();
  }
  const e2_sm_ccc_ind_msg_format2_s& ind_msg_format2() const
  {
    assert_choice_type(types::ind_msg_format2, type_, "IndicationMessageFormat");
    return c.get<e2_sm_ccc_ind_msg_format2_s>();
  }
  e2_sm_ccc_ind_msg_format1_s& set_ind_msg_format1();
  e2_sm_ccc_ind_msg_format2_s& set_ind_msg_format2();

private:
  types                                                                     type_;
  choice_buffer_t<e2_sm_ccc_ind_msg_format1_s, e2_sm_ccc_ind_msg_format2_s> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const ind_msg_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, ind_msg_format_c& obj);

// ListOfCellsForRANFunctionDefinition ::= SEQUENCE (SIZE (1..64)) OF CellForRANFunctionDefinition
using list_of_cells_for_ran_function_definition_l = dyn_array<cell_for_ran_function_definition_s>;

// QueryDefinitionFormat ::= CHOICE
struct query_definition_format_c {
  struct types_opts {
    enum options { query_definition_format1, query_definition_format2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  query_definition_format_c() = default;
  query_definition_format_c(const query_definition_format_c& other);
  query_definition_format_c& operator=(const query_definition_format_c& other);
  ~query_definition_format_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_query_definition_format1_s& query_definition_format1()
  {
    assert_choice_type(types::query_definition_format1, type_, "QueryDefinitionFormat");
    return c.get<e2_sm_ccc_query_definition_format1_s>();
  }
  e2_sm_ccc_query_definition_format2_s& query_definition_format2()
  {
    assert_choice_type(types::query_definition_format2, type_, "QueryDefinitionFormat");
    return c.get<e2_sm_ccc_query_definition_format2_s>();
  }
  const e2_sm_ccc_query_definition_format1_s& query_definition_format1() const
  {
    assert_choice_type(types::query_definition_format1, type_, "QueryDefinitionFormat");
    return c.get<e2_sm_ccc_query_definition_format1_s>();
  }
  const e2_sm_ccc_query_definition_format2_s& query_definition_format2() const
  {
    assert_choice_type(types::query_definition_format2, type_, "QueryDefinitionFormat");
    return c.get<e2_sm_ccc_query_definition_format2_s>();
  }
  e2_sm_ccc_query_definition_format1_s& set_query_definition_format1();
  e2_sm_ccc_query_definition_format2_s& set_query_definition_format2();

private:
  types                                                                                       type_;
  choice_buffer_t<e2_sm_ccc_query_definition_format1_s, e2_sm_ccc_query_definition_format2_s> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const query_definition_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, query_definition_format_c& obj);

// QueryHeaderFormat ::= CHOICE
struct query_hdr_format_c {
  struct types_opts {
    enum options { query_hdr_format1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  types       type() const { return types::query_hdr_format1; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_query_hdr_format1_s&       query_hdr_format1() { return c; }
  const e2_sm_ccc_query_hdr_format1_s& query_hdr_format1() const { return c; }

private:
  e2_sm_ccc_query_hdr_format1_s c;
};

void to_json(nlohmann::ordered_json& out_j, const query_hdr_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, query_hdr_format_c& obj);

// QueryOutcomeFormat ::= CHOICE
struct query_outcome_format_c {
  struct types_opts {
    enum options { query_outcome_format1, query_outcome_format2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    bool        from_string(const std::string& str);
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  query_outcome_format_c() = default;
  query_outcome_format_c(const query_outcome_format_c& other);
  query_outcome_format_c& operator=(const query_outcome_format_c& other);
  ~query_outcome_format_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2_sm_ccc_query_outcome_format1_s& query_outcome_format1()
  {
    assert_choice_type(types::query_outcome_format1, type_, "QueryOutcomeFormat");
    return c.get<e2_sm_ccc_query_outcome_format1_s>();
  }
  e2_sm_ccc_query_outcome_format2_s& query_outcome_format2()
  {
    assert_choice_type(types::query_outcome_format2, type_, "QueryOutcomeFormat");
    return c.get<e2_sm_ccc_query_outcome_format2_s>();
  }
  const e2_sm_ccc_query_outcome_format1_s& query_outcome_format1() const
  {
    assert_choice_type(types::query_outcome_format1, type_, "QueryOutcomeFormat");
    return c.get<e2_sm_ccc_query_outcome_format1_s>();
  }
  const e2_sm_ccc_query_outcome_format2_s& query_outcome_format2() const
  {
    assert_choice_type(types::query_outcome_format2, type_, "QueryOutcomeFormat");
    return c.get<e2_sm_ccc_query_outcome_format2_s>();
  }
  e2_sm_ccc_query_outcome_format1_s& set_query_outcome_format1();
  e2_sm_ccc_query_outcome_format2_s& set_query_outcome_format2();

private:
  types                                                                                 type_;
  choice_buffer_t<e2_sm_ccc_query_outcome_format1_s, e2_sm_ccc_query_outcome_format2_s> c;

  void destroy_();
};

void to_json(nlohmann::ordered_json& out_j, const query_outcome_format_c& obj);
void from_json(const nlohmann::ordered_json& in_j, query_outcome_format_c& obj);

// RANFunctionName ::= SEQUENCE
struct ran_function_name_s {
  bool                                  ran_function_instance_present = false;
  printable_string<0, 150, true, true>  ran_function_short_name;
  printable_string<1, 1000, true, true> ran_function_service_model_o_id;
  printable_string<1, 150, true, true>  ran_function_description;
  int64_t                               ran_function_instance = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ran_function_name_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ran_function_name_s& obj);

// PlmnIdList ::= SEQUENCE (SIZE (1..64)) OF PlmnId
using plmn_id_list_l = dyn_array<plmn_id_s>;

// RANFunctionDefinition ::= SEQUENCE
struct ran_function_definition_s {
  ran_function_name_s                         ran_function_name;
  list_of_supported_ran_cfg_structures_l      list_of_supported_node_level_cfg_structures;
  list_of_cells_for_ran_function_definition_l list_of_cells_for_ran_function_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ran_function_definition_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ran_function_definition_s& obj);

// RICActionDefinition ::= SEQUENCE
struct ric_action_definition_s {
  int64_t                    ric_style_type = 0;
  action_definition_format_c action_definition_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_action_definition_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_action_definition_s& obj);

// RIC-Control-Header ::= SEQUENCE
struct ric_ctrl_hdr_s {
  ctrl_hdr_format_c ctrl_hdr_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_ctrl_hdr_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_ctrl_hdr_s& obj);

// RIC-Control-Message ::= SEQUENCE
struct ric_ctrl_msg_s {
  ctrl_msg_format_c ctrl_msg_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_ctrl_msg_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_ctrl_msg_s& obj);

// RIC-Control-Outcome ::= SEQUENCE
struct ric_ctrl_outcome_s {
  ctrl_outcome_format_c ctrl_outcome_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_ctrl_outcome_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_ctrl_outcome_s& obj);

// RICEventTriggerDefinition ::= SEQUENCE
struct ric_event_trigger_definition_s {
  event_trigger_definition_format_c event_trigger_definition_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_event_trigger_definition_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_event_trigger_definition_s& obj);

// RIC-Indication-Header ::= SEQUENCE
struct ric_ind_hdr_s {
  ind_hdr_format_c ind_hdr_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_ind_hdr_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_ind_hdr_s& obj);

// RIC-Indication-Message ::= SEQUENCE
struct ric_ind_msg_s {
  ind_msg_format_c ind_msg_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_ind_msg_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_ind_msg_s& obj);

// RIC-Query-Definition ::= SEQUENCE
struct ric_query_definition_s {
  query_definition_format_c query_definition_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_query_definition_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_query_definition_s& obj);

// RIC-Query-Header ::= SEQUENCE
struct ric_query_hdr_s {
  query_hdr_format_c query_hdr_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_query_hdr_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_query_hdr_s& obj);

// RIC-Query-Outcome ::= SEQUENCE
struct ric_query_outcome_s {
  query_outcome_format_c query_outcome_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

void to_json(nlohmann::ordered_json& out_j, const ric_query_outcome_s& obj);
void from_json(const nlohmann::ordered_json& in_j, ric_query_outcome_s& obj);

// SnssaiList ::= SEQUENCE (SIZE (1..64)) OF Snssai
using snssai_list_l = dyn_array<snssai_s>;

} // namespace e2sm_ccc
} // namespace asn1

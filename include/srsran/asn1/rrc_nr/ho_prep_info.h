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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "ul_dcch_msg_ies.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// ReestabNCellInfo ::= SEQUENCE
struct reestab_ncell_info_s {
  fixed_bitstring<36>  cell_id;
  fixed_bitstring<256> key_g_node_b_star;
  fixed_bitstring<16>  short_mac_i;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestabNCellInfoList ::= SEQUENCE (SIZE (1..32)) OF ReestabNCellInfo
using reestab_ncell_info_list_l = dyn_array<reestab_ncell_info_s>;

// BandCombinationInfoSN ::= SEQUENCE
struct band_combination_info_sn_s {
  uint32_t band_combination_idx   = 1;
  uint8_t  requested_feature_sets = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfigRestrictInfoDAPS-r16 ::= SEQUENCE
struct cfg_restrict_info_daps_r16_s {
  struct pwr_coordination_r16_s_ {
    struct ul_pwr_sharing_daps_mode_r16_opts {
      enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using ul_pwr_sharing_daps_mode_r16_e_ = enumerated<ul_pwr_sharing_daps_mode_r16_opts>;

    // member variables
    int8_t                          p_daps_source_r16 = -30;
    int8_t                          p_daps_target_r16 = -30;
    ul_pwr_sharing_daps_mode_r16_e_ ul_pwr_sharing_daps_mode_r16;
  };

  // member variables
  bool                    pwr_coordination_r16_present = false;
  pwr_coordination_r16_s_ pwr_coordination_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfigRestrictInfoDAPS-v1640 ::= SEQUENCE
struct cfg_restrict_info_daps_v1640_s {
  uint16_t source_feature_set_per_dl_cc_r16 = 1;
  uint16_t source_feature_set_per_ul_cc_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentInfo ::= SEQUENCE
struct reest_info_s {
  uint16_t                  source_pci = 0;
  fixed_bitstring<16>       target_cell_short_mac_i;
  reestab_ncell_info_list_l add_reestab_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config ::= SEQUENCE
struct as_cfg_s {
  bool          ext = false;
  dyn_octstring rrc_recfg;
  // ...
  // group 0
  dyn_octstring source_rb_sn_cfg;
  dyn_octstring source_scg_nr_cfg;
  dyn_octstring source_scg_eutra_cfg;
  // group 1
  bool source_scg_cfg_present = false;
  // group 2
  copy_ptr<sdt_cfg_r17_s> sdt_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context ::= SEQUENCE
struct as_context_s {
  bool                    ext                       = false;
  bool                    reest_info_present        = false;
  bool                    cfg_restrict_info_present = false;
  reest_info_s            reest_info;
  cfg_restrict_info_scg_s cfg_restrict_info;
  // ...
  // group 0
  copy_ptr<ran_notif_area_info_c> ran_notif_area_info;
  // group 1
  dyn_octstring ue_assist_info;
  // group 2
  copy_ptr<band_combination_info_sn_s> sel_band_combination_sn;
  // group 3
  copy_ptr<cfg_restrict_info_daps_r16_s> cfg_restrict_info_daps_r16;
  dyn_octstring                          sidelink_ue_info_nr_r16;
  dyn_octstring                          sidelink_ue_info_eutra_r16;
  dyn_octstring                          ue_assist_info_eutra_r16;
  dyn_octstring                          ue_assist_info_scg_r16;
  copy_ptr<need_for_gaps_info_nr_r16_s>  need_for_gaps_info_nr_r16;
  // group 4
  copy_ptr<cfg_restrict_info_daps_v1640_s> cfg_restrict_info_daps_v1640;
  // group 5
  copy_ptr<need_for_gap_ncsg_info_nr_r17_s>    need_for_gap_ncsg_info_nr_r17;
  copy_ptr<need_for_gap_ncsg_info_eutra_r17_s> need_for_gap_ncsg_info_eutra_r17;
  dyn_octstring                                mbs_interest_ind_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRM-Config ::= SEQUENCE
struct rrm_cfg_s {
  struct ue_inactive_time_opts {
    enum options {
      s1,
      s2,
      s3,
      s5,
      s7,
      s10,
      s15,
      s20,
      s25,
      s30,
      s40,
      s50,
      min1,
      min1s20,
      min1s40,
      min2,
      min2s30,
      min3,
      min3s30,
      min4,
      min5,
      min6,
      min7,
      min8,
      min9,
      min10,
      min12,
      min14,
      min17,
      min20,
      min24,
      min28,
      min33,
      min38,
      min44,
      min50,
      hr1,
      hr1min30,
      hr2,
      hr2min30,
      hr3,
      hr3min30,
      hr4,
      hr5,
      hr6,
      hr8,
      hr10,
      hr13,
      hr16,
      hr20,
      day1,
      day1hr12,
      day2,
      day2hr12,
      day3,
      day4,
      day5,
      day7,
      day10,
      day14,
      day19,
      day24,
      day30,
      day_more_than30,
      nulltype
    } value;

    const char* to_string() const;
  };
  using ue_inactive_time_e_ = enumerated<ue_inactive_time_opts>;

  // member variables
  bool                   ext                      = false;
  bool                   ue_inactive_time_present = false;
  ue_inactive_time_e_    ue_inactive_time;
  meas_result_list2_nr_l candidate_cell_info_list;
  // ...
  // group 0
  copy_ptr<meas_result_serv_freq_list_eutra_scg_l> candidate_cell_info_list_sn_eutra;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-IEs ::= SEQUENCE
struct ho_prep_info_ies_s {
  bool                        source_cfg_present   = false;
  bool                        rrm_cfg_present      = false;
  bool                        as_context_present   = false;
  bool                        non_crit_ext_present = false;
  ue_cap_rat_container_list_l ue_cap_rat_list;
  as_cfg_s                    source_cfg;
  rrm_cfg_s                   rrm_cfg;
  as_context_s                as_context;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation ::= SEQUENCE
struct ho_prep_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_prep_info, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ho_prep_info_ies_s& ho_prep_info()
      {
        assert_choice_type(types::ho_prep_info, type_, "c1");
        return c;
      }
      const ho_prep_info_ies_s& ho_prep_info() const
      {
        assert_choice_type(types::ho_prep_info, type_, "c1");
        return c;
      }
      ho_prep_info_ies_s& set_ho_prep_info();
      void                set_spare3();
      void                set_spare2();
      void                set_spare1();

    private:
      types              type_;
      ho_prep_info_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    const c1_c_& c1() const
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    c1_c_& set_c1();
    void   set_crit_exts_future();

  private:
    types type_;
    c1_c_ c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "cell_group_config.h"
#include "dl_dcch_msg.h"
#include "nr_sidelink_preconf.h"
#include "pc5_rrc_definitions.h"
#include "ul_dcch_msg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MIB ::= SEQUENCE
struct mib_s {
  struct sub_carrier_spacing_common_opts {
    enum options { scs15or60, scs30or120, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sub_carrier_spacing_common_e_ = enumerated<sub_carrier_spacing_common_opts>;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dmrs_type_a_position_e_ = enumerated<dmrs_type_a_position_opts>;
  struct cell_barred_opts {
    enum options { barred, not_barred, nulltype } value;

    const char* to_string() const;
  };
  using cell_barred_e_ = enumerated<cell_barred_opts>;
  struct intra_freq_resel_opts {
    enum options { allowed, not_allowed, nulltype } value;

    const char* to_string() const;
  };
  using intra_freq_resel_e_ = enumerated<intra_freq_resel_opts>;

  // member variables
  fixed_bitstring<6>            sys_frame_num;
  sub_carrier_spacing_common_e_ sub_carrier_spacing_common;
  uint8_t                       ssb_subcarrier_offset = 0;
  dmrs_type_a_position_e_       dmrs_type_a_position;
  pdcch_cfg_sib1_s              pdcch_cfg_sib1;
  cell_barred_e_                cell_barred;
  intra_freq_resel_e_           intra_freq_resel;
  fixed_bitstring<1>            spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-BCH-MessageType ::= CHOICE
struct bcch_bch_msg_type_c {
  struct types_opts {
    enum options { mib, msg_class_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bcch_bch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mib_s& mib()
  {
    assert_choice_type(types::mib, type_, "BCCH-BCH-MessageType");
    return c;
  }
  const mib_s& mib() const
  {
    assert_choice_type(types::mib, type_, "BCCH-BCH-MessageType");
    return c;
  }
  mib_s& set_mib();
  void   set_msg_class_ext();

private:
  types type_;
  mib_s c;
};

// BCCH-BCH-Message ::= SEQUENCE
struct bcch_bch_msg_s {
  bcch_bch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-NS-PmaxValue ::= SEQUENCE
struct eutra_ns_pmax_value_s {
  bool     add_pmax_present          = false;
  bool     add_spec_emission_present = false;
  int8_t   add_pmax                  = -30;
  uint16_t add_spec_emission         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-NS-PmaxList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-NS-PmaxValue
using eutra_ns_pmax_list_l = dyn_array<eutra_ns_pmax_value_s>;

// SI-RequestResources ::= SEQUENCE
struct si_request_res_s {
  bool    ra_assoc_period_idx_present      = false;
  bool    ra_ssb_occasion_mask_idx_present = false;
  uint8_t ra_preamb_start_idx              = 0;
  uint8_t ra_assoc_period_idx              = 0;
  uint8_t ra_ssb_occasion_mask_idx         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-TypeInfo-v1700 ::= SEQUENCE
struct sib_type_info_v1700_s {
  struct sib_type_r17_c_ {
    struct type1_r17_opts {
      enum options {
        sib_type15,
        sib_type16,
        sib_type17,
        sib_type18,
        sib_type19,
        sib_type20,
        sib_type21,
        spare9,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        // ...
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using type1_r17_e_ = enumerated<type1_r17_opts, true>;
    struct type2_r17_s_ {
      struct pos_sib_type_r17_opts {
        enum options {
          pos_sib_type1_neg9,
          pos_sib_type1_neg10,
          pos_sib_type2_neg24,
          pos_sib_type2_neg25,
          pos_sib_type6_neg4,
          pos_sib_type6_neg5,
          pos_sib_type6_neg6,
          spare9,
          spare8,
          spare7,
          spare6,
          spare5,
          spare4,
          spare3,
          spare2,
          spare1,
          // ...
          nulltype
        } value;

        const char* to_string() const;
      };
      using pos_sib_type_r17_e_ = enumerated<pos_sib_type_r17_opts, true>;

      // member variables
      bool                encrypted_r17_present = false;
      bool                gnss_id_r17_present   = false;
      bool                sbas_id_r17_present   = false;
      pos_sib_type_r17_e_ pos_sib_type_r17;
      gnss_id_r16_s       gnss_id_r17;
      sbas_id_r16_s       sbas_id_r17;
    };
    struct types_opts {
      enum options { type1_r17, type2_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sib_type_r17_c_() = default;
    sib_type_r17_c_(const sib_type_r17_c_& other);
    sib_type_r17_c_& operator=(const sib_type_r17_c_& other);
    ~sib_type_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_r17_e_& type1_r17()
    {
      assert_choice_type(types::type1_r17, type_, "sibType-r17");
      return c.get<type1_r17_e_>();
    }
    type2_r17_s_& type2_r17()
    {
      assert_choice_type(types::type2_r17, type_, "sibType-r17");
      return c.get<type2_r17_s_>();
    }
    const type1_r17_e_& type1_r17() const
    {
      assert_choice_type(types::type1_r17, type_, "sibType-r17");
      return c.get<type1_r17_e_>();
    }
    const type2_r17_s_& type2_r17() const
    {
      assert_choice_type(types::type2_r17, type_, "sibType-r17");
      return c.get<type2_r17_s_>();
    }
    type1_r17_e_& set_type1_r17();
    type2_r17_s_& set_type2_r17();

  private:
    types                         type_;
    choice_buffer_t<type2_r17_s_> c;

    void destroy_();
  };

  // member variables
  bool            value_tag_r17_present  = false;
  bool            area_scope_r17_present = false;
  sib_type_r17_c_ sib_type_r17;
  uint8_t         value_tag_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceCellListNR-r17 ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using slice_cell_list_nr_r17_l = dyn_array<pci_range_s>;

// EUTRA-FreqNeighCellInfo ::= SEQUENCE
struct eutra_freq_neigh_cell_info_s {
  bool                   q_rx_lev_min_offset_cell_present = false;
  bool                   q_qual_min_offset_cell_present   = false;
  uint16_t               pci                              = 0;
  eutra_q_offset_range_e dummy;
  uint8_t                q_rx_lev_min_offset_cell = 1;
  uint8_t                q_qual_min_offset_cell   = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-MultiBandInfo ::= SEQUENCE
struct eutra_multi_band_info_s {
  uint16_t             eutra_freq_band_ind = 1;
  eutra_ns_pmax_list_l eutra_ns_pmax_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqNeighCellInfo ::= SEQUENCE
struct inter_freq_neigh_cell_info_s {
  bool             ext                                  = false;
  bool             q_rx_lev_min_offset_cell_present     = false;
  bool             q_rx_lev_min_offset_cell_sul_present = false;
  bool             q_qual_min_offset_cell_present       = false;
  uint16_t         pci                                  = 0;
  q_offset_range_e q_offset_cell;
  uint8_t          q_rx_lev_min_offset_cell     = 1;
  uint8_t          q_rx_lev_min_offset_cell_sul = 1;
  uint8_t          q_qual_min_offset_cell       = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqNeighCellInfo-v1610 ::= SEQUENCE
struct inter_freq_neigh_cell_info_v1610_s {
  bool                            ssb_position_qcl_r16_present = false;
  ssb_position_qcl_relation_r16_e ssb_position_qcl_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqNeighCellInfo-v1710 ::= SEQUENCE
struct inter_freq_neigh_cell_info_v1710_s {
  bool                            ssb_position_qcl_r17_present = false;
  ssb_position_qcl_relation_r17_e ssb_position_qcl_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SI-RequestConfig ::= SEQUENCE
struct si_request_cfg_s {
  struct rach_occasions_si_s_ {
    struct ssb_per_rach_occasion_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using ssb_per_rach_occasion_e_ = enumerated<ssb_per_rach_occasion_opts>;

    // member variables
    rach_cfg_generic_s       rach_cfg_si;
    ssb_per_rach_occasion_e_ ssb_per_rach_occasion;
  };
  struct si_request_period_opts {
    enum options { one, two, four, six, eight, ten, twelve, sixteen, nulltype } value;

    const char* to_string() const;
  };
  using si_request_period_e_ = enumerated<si_request_period_opts>;
  using si_request_res_l_    = dyn_array<si_request_res_s>;

  // member variables
  bool                 rach_occasions_si_present = false;
  bool                 si_request_period_present = false;
  rach_occasions_si_s_ rach_occasions_si;
  si_request_period_e_ si_request_period;
  si_request_res_l_    si_request_res;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-Mapping-v1700 ::= SEQUENCE (SIZE (1..32)) OF SIB-TypeInfo-v1700
using sib_map_v1700_l = dyn_array<sib_type_info_v1700_s>;

// SliceInfo-r17 ::= SEQUENCE
struct slice_info_r17_s {
  struct slice_cell_list_nr_r17_c_ {
    struct types_opts {
      enum options { slice_allowed_cell_list_nr_r17, slice_excluded_cell_list_nr_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    slice_cell_list_nr_r17_c_() = default;
    slice_cell_list_nr_r17_c_(const slice_cell_list_nr_r17_c_& other);
    slice_cell_list_nr_r17_c_& operator=(const slice_cell_list_nr_r17_c_& other);
    ~slice_cell_list_nr_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    slice_cell_list_nr_r17_l& slice_allowed_cell_list_nr_r17()
    {
      assert_choice_type(types::slice_allowed_cell_list_nr_r17, type_, "sliceCellListNR-r17");
      return c.get<slice_cell_list_nr_r17_l>();
    }
    slice_cell_list_nr_r17_l& slice_excluded_cell_list_nr_r17()
    {
      assert_choice_type(types::slice_excluded_cell_list_nr_r17, type_, "sliceCellListNR-r17");
      return c.get<slice_cell_list_nr_r17_l>();
    }
    const slice_cell_list_nr_r17_l& slice_allowed_cell_list_nr_r17() const
    {
      assert_choice_type(types::slice_allowed_cell_list_nr_r17, type_, "sliceCellListNR-r17");
      return c.get<slice_cell_list_nr_r17_l>();
    }
    const slice_cell_list_nr_r17_l& slice_excluded_cell_list_nr_r17() const
    {
      assert_choice_type(types::slice_excluded_cell_list_nr_r17, type_, "sliceCellListNR-r17");
      return c.get<slice_cell_list_nr_r17_l>();
    }
    slice_cell_list_nr_r17_l& set_slice_allowed_cell_list_nr_r17();
    slice_cell_list_nr_r17_l& set_slice_excluded_cell_list_nr_r17();

  private:
    types                                     type_;
    choice_buffer_t<slice_cell_list_nr_r17_l> c;

    void destroy_();
  };

  // member variables
  bool                      nsag_cell_resel_prio_r17_present     = false;
  bool                      nsag_cell_resel_sub_prio_r17_present = false;
  bool                      slice_cell_list_nr_r17_present       = false;
  nsag_id_info_r17_s        nsag_id_info_r17;
  uint8_t                   nsag_cell_resel_prio_r17 = 0;
  cell_resel_sub_prio_e     nsag_cell_resel_sub_prio_r17;
  slice_cell_list_nr_r17_c_ slice_cell_list_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-FreqExcludedCellList ::= SEQUENCE (SIZE (1..16)) OF EUTRA-PhysCellIdRange
using eutra_freq_excluded_cell_list_l = dyn_array<eutra_pci_range_s>;

// EUTRA-FreqNeighCellList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-FreqNeighCellInfo
using eutra_freq_neigh_cell_list_l = dyn_array<eutra_freq_neigh_cell_info_s>;

// EUTRA-FreqNeighHSDN-CellList-r17 ::= SEQUENCE (SIZE (1..8)) OF EUTRA-PhysCellIdRange
using eutra_freq_neigh_hsdn_cell_list_r17_l = dyn_array<eutra_pci_range_s>;

// EUTRA-MultiBandInfoList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-MultiBandInfo
using eutra_multi_band_info_list_l = dyn_array<eutra_multi_band_info_s>;

// InterFreqAllowedCellList-r16 ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using inter_freq_allowed_cell_list_r16_l = dyn_array<pci_range_s>;

// InterFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
struct inter_freq_cag_cell_list_per_plmn_r16_s {
  using cag_cell_list_r16_l_ = dyn_array<pci_range_s>;

  // member variables
  uint8_t              plmn_id_idx_r16 = 1;
  cag_cell_list_r16_l_ cag_cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqExcludedCellList ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using inter_freq_excluded_cell_list_l = dyn_array<pci_range_s>;

// InterFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF InterFreqNeighCellInfo
using inter_freq_neigh_cell_list_l = dyn_array<inter_freq_neigh_cell_info_s>;

// InterFreqNeighCellList-v1610 ::= SEQUENCE (SIZE (1..16)) OF InterFreqNeighCellInfo-v1610
using inter_freq_neigh_cell_list_v1610_l = dyn_array<inter_freq_neigh_cell_info_v1610_s>;

// InterFreqNeighCellList-v1710 ::= SEQUENCE (SIZE (1..16)) OF InterFreqNeighCellInfo-v1710
using inter_freq_neigh_cell_list_v1710_l = dyn_array<inter_freq_neigh_cell_info_v1710_s>;

// InterFreqNeighHSDN-CellList-r17 ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using inter_freq_neigh_hsdn_cell_list_r17_l = dyn_array<pci_range_s>;

// MBS-FSAI-List-r17 ::= SEQUENCE (SIZE (1..64)) OF OCTET STRING (SIZE (3))
using mbs_fsai_list_r17_l = dyn_array<fixed_octstring<3>>;

// PDSCH-ConfigPTM-r17 ::= SEQUENCE
struct pdsch_cfg_ptm_r17_s {
  struct pdsch_aggregation_factor_r17_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdsch_aggregation_factor_r17_e_ = enumerated<pdsch_aggregation_factor_r17_opts>;

  // member variables
  bool                            data_scrambling_id_pdsch_r17_present = false;
  bool                            dmrs_scrambling_id0_r17_present      = false;
  bool                            pdsch_aggregation_factor_r17_present = false;
  uint16_t                        data_scrambling_id_pdsch_r17         = 0;
  uint32_t                        dmrs_scrambling_id0_r17              = 0;
  pdsch_aggregation_factor_r17_e_ pdsch_aggregation_factor_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSIB-Type-r16 ::= SEQUENCE
struct pos_sib_type_r16_s {
  struct pos_sib_type_r16_opts {
    enum options {
      pos_sib_type1_neg1,
      pos_sib_type1_neg2,
      pos_sib_type1_neg3,
      pos_sib_type1_neg4,
      pos_sib_type1_neg5,
      pos_sib_type1_neg6,
      pos_sib_type1_neg7,
      pos_sib_type1_neg8,
      pos_sib_type2_neg1,
      pos_sib_type2_neg2,
      pos_sib_type2_neg3,
      pos_sib_type2_neg4,
      pos_sib_type2_neg5,
      pos_sib_type2_neg6,
      pos_sib_type2_neg7,
      pos_sib_type2_neg8,
      pos_sib_type2_neg9,
      pos_sib_type2_neg10,
      pos_sib_type2_neg11,
      pos_sib_type2_neg12,
      pos_sib_type2_neg13,
      pos_sib_type2_neg14,
      pos_sib_type2_neg15,
      pos_sib_type2_neg16,
      pos_sib_type2_neg17,
      pos_sib_type2_neg18,
      pos_sib_type2_neg19,
      pos_sib_type2_neg20,
      pos_sib_type2_neg21,
      pos_sib_type2_neg22,
      pos_sib_type2_neg23,
      pos_sib_type3_neg1,
      pos_sib_type4_neg1,
      pos_sib_type5_neg1,
      pos_sib_type6_neg1,
      pos_sib_type6_neg2,
      pos_sib_type6_neg3,
      // ...
      nulltype
    } value;

    const char* to_string() const;
  };
  using pos_sib_type_r16_e_ = enumerated<pos_sib_type_r16_opts, true>;

  // member variables
  bool                encrypted_r16_present  = false;
  bool                gnss_id_r16_present    = false;
  bool                sbas_id_r16_present    = false;
  bool                area_scope_r16_present = false;
  gnss_id_r16_s       gnss_id_r16;
  sbas_id_r16_s       sbas_id_r16;
  pos_sib_type_r16_e_ pos_sib_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SI-SchedulingInfo-v1740 ::= SEQUENCE
struct si_sched_info_v1740_s {
  bool             si_request_cfg_red_cap_r17_present = false;
  si_request_cfg_s si_request_cfg_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC2-LP-r16 ::= SEQUENCE
struct ssb_mtc2_lp_r16_s {
  using pci_list_l_ = dyn_array<uint16_t>;
  struct periodicity_opts {
    enum options { sf10, sf20, sf40, sf80, sf160, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts>;

  // member variables
  pci_list_l_    pci_list;
  periodicity_e_ periodicity;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfo2-r17 ::= SEQUENCE
struct sched_info2_r17_s {
  struct si_broadcast_status_r17_opts {
    enum options { broadcasting, not_broadcasting, nulltype } value;

    const char* to_string() const;
  };
  using si_broadcast_status_r17_e_ = enumerated<si_broadcast_status_r17_opts>;
  struct si_periodicity_r17_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using si_periodicity_r17_e_ = enumerated<si_periodicity_r17_opts>;

  // member variables
  si_broadcast_status_r17_e_ si_broadcast_status_r17;
  uint16_t                   si_win_position_r17 = 1;
  si_periodicity_r17_e_      si_periodicity_r17;
  sib_map_v1700_l            sib_map_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceInfoList-r17 ::= SEQUENCE (SIZE (1..8)) OF SliceInfo-r17
using slice_info_list_r17_l = dyn_array<slice_info_r17_s>;

// SpeedStateScaleFactors ::= SEQUENCE
struct speed_state_scale_factors_s {
  struct sf_medium_opts {
    enum options { odot25, odot5, odot75, ldot0, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using sf_medium_e_ = enumerated<sf_medium_opts>;
  struct sf_high_opts {
    enum options { odot25, odot5, odot75, ldot0, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using sf_high_e_ = enumerated<sf_high_opts>;

  // member variables
  sf_medium_e_ sf_medium;
  sf_high_e_   sf_high;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringInfoSet-v1700 ::= SEQUENCE
struct uac_barr_info_set_v1700_s {
  struct uac_barr_factor_for_ai3_r17_opts {
    enum options { p00, p05, p10, p15, p20, p25, p30, p40, p50, p60, p70, p75, p80, p85, p90, p95, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using uac_barr_factor_for_ai3_r17_e_ = enumerated<uac_barr_factor_for_ai3_r17_opts>;

  // member variables
  bool                           uac_barr_factor_for_ai3_r17_present = false;
  uac_barr_factor_for_ai3_r17_e_ uac_barr_factor_for_ai3_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqEUTRA ::= SEQUENCE
struct carrier_freq_eutra_s {
  struct thresh_x_q_s_ {
    uint8_t thresh_x_high_q = 0;
    uint8_t thresh_x_low_q  = 0;
  };

  // member variables
  bool                            cell_resel_prio_present     = false;
  bool                            cell_resel_sub_prio_present = false;
  bool                            thresh_x_q_present          = false;
  uint32_t                        carrier_freq                = 0;
  eutra_multi_band_info_list_l    eutra_multi_band_info_list;
  eutra_freq_neigh_cell_list_l    eutra_freq_neigh_cell_list;
  eutra_freq_excluded_cell_list_l eutra_excluded_cell_list;
  eutra_allowed_meas_bw_e         allowed_meas_bw;
  bool                            presence_ant_port1 = false;
  uint8_t                         cell_resel_prio    = 0;
  cell_resel_sub_prio_e           cell_resel_sub_prio;
  uint8_t                         thresh_x_high = 0;
  uint8_t                         thresh_x_low  = 0;
  int8_t                          q_rx_lev_min  = -70;
  int8_t                          q_qual_min    = -34;
  int8_t                          p_max_eutra   = -30;
  thresh_x_q_s_                   thresh_x_q;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqEUTRA-v1610 ::= SEQUENCE
struct carrier_freq_eutra_v1610_s {
  bool high_speed_eutra_carrier_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqEUTRA-v1700 ::= SEQUENCE
struct carrier_freq_eutra_v1700_s {
  eutra_freq_neigh_hsdn_cell_list_r17_l eutra_freq_neigh_hsdn_cell_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPrioritySlicing-r17 ::= SEQUENCE
struct freq_prio_slicing_r17_s {
  uint8_t               dl_implicit_carrier_freq_r17 = 0;
  slice_info_list_r17_l slice_info_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HRNN-r16 ::= SEQUENCE
struct hrnn_r16_s {
  bounded_octstring<1, 48> hrnn_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo ::= SEQUENCE
struct inter_freq_carrier_freq_info_s {
  struct thresh_x_q_s_ {
    uint8_t thresh_x_high_q = 0;
    uint8_t thresh_x_low_q  = 0;
  };

  // member variables
  bool                            ext                                        = false;
  bool                            nrof_ss_blocks_to_average_present          = false;
  bool                            abs_thresh_ss_blocks_consolidation_present = false;
  bool                            smtc_present                               = false;
  bool                            ssb_to_measure_present                     = false;
  bool                            ss_rssi_meas_present                       = false;
  bool                            q_rx_lev_min_sul_present                   = false;
  bool                            q_qual_min_present                         = false;
  bool                            p_max_present                              = false;
  bool                            t_resel_nr_sf_present                      = false;
  bool                            thresh_x_q_present                         = false;
  bool                            cell_resel_prio_present                    = false;
  bool                            cell_resel_sub_prio_present                = false;
  bool                            q_offset_freq_present                      = false;
  uint32_t                        dl_carrier_freq                            = 0;
  multi_freq_band_list_nr_sib_l   freq_band_list;
  multi_freq_band_list_nr_sib_l   freq_band_list_sul;
  uint8_t                         nrof_ss_blocks_to_average = 2;
  thres_nr_s                      abs_thresh_ss_blocks_consolidation;
  ssb_mtc_s                       smtc;
  subcarrier_spacing_e            ssb_subcarrier_spacing;
  ssb_to_measure_c                ssb_to_measure;
  bool                            derive_ssb_idx_from_cell = false;
  ss_rssi_meas_s                  ss_rssi_meas;
  int8_t                          q_rx_lev_min     = -70;
  int8_t                          q_rx_lev_min_sul = -70;
  int8_t                          q_qual_min       = -43;
  int8_t                          p_max            = -30;
  uint8_t                         t_resel_nr       = 0;
  speed_state_scale_factors_s     t_resel_nr_sf;
  uint8_t                         thresh_x_high_p = 0;
  uint8_t                         thresh_x_low_p  = 0;
  thresh_x_q_s_                   thresh_x_q;
  uint8_t                         cell_resel_prio = 0;
  cell_resel_sub_prio_e           cell_resel_sub_prio;
  q_offset_range_e                q_offset_freq;
  inter_freq_neigh_cell_list_l    inter_freq_neigh_cell_list;
  inter_freq_excluded_cell_list_l inter_freq_excluded_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1610 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1610_s {
  using inter_freq_cag_cell_list_r16_l_ = dyn_array<inter_freq_cag_cell_list_per_plmn_r16_s>;

  // member variables
  bool                               smtc2_lp_r16_present                = false;
  bool                               ssb_position_qcl_common_r16_present = false;
  inter_freq_neigh_cell_list_v1610_l inter_freq_neigh_cell_list_v1610;
  ssb_mtc2_lp_r16_s                  smtc2_lp_r16;
  inter_freq_allowed_cell_list_r16_l inter_freq_allowed_cell_list_r16;
  ssb_position_qcl_relation_r16_e    ssb_position_qcl_common_r16;
  inter_freq_cag_cell_list_r16_l_    inter_freq_cag_cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1700 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1700_s {
  bool                                  high_speed_meas_inter_freq_r17_present = false;
  bool                                  red_cap_access_allowed_r17_present     = false;
  bool                                  ssb_position_qcl_common_r17_present    = false;
  inter_freq_neigh_hsdn_cell_list_r17_l inter_freq_neigh_hsdn_cell_list_r17;
  ssb_position_qcl_relation_r17_e       ssb_position_qcl_common_r17;
  inter_freq_neigh_cell_list_v1710_l    inter_freq_neigh_cell_list_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1720 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1720_s {
  ssb_mtc4_list_r17_l smtc4list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1730 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1730_s {
  bool ch_access_mode2_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqNeighCellInfo ::= SEQUENCE
struct intra_freq_neigh_cell_info_s {
  bool             ext                                  = false;
  bool             q_rx_lev_min_offset_cell_present     = false;
  bool             q_rx_lev_min_offset_cell_sul_present = false;
  bool             q_qual_min_offset_cell_present       = false;
  uint16_t         pci                                  = 0;
  q_offset_range_e q_offset_cell;
  uint8_t          q_rx_lev_min_offset_cell     = 1;
  uint8_t          q_rx_lev_min_offset_cell_sul = 1;
  uint8_t          q_qual_min_offset_cell       = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqNeighCellInfo-v1610 ::= SEQUENCE
struct intra_freq_neigh_cell_info_v1610_s {
  bool                            ssb_position_qcl_r16_present = false;
  ssb_position_qcl_relation_r16_e ssb_position_qcl_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqNeighCellInfo-v1710 ::= SEQUENCE
struct intra_freq_neigh_cell_info_v1710_s {
  bool                            ssb_position_qcl_r17_present = false;
  ssb_position_qcl_relation_r17_e ssb_position_qcl_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationAndBandwidthBroadcast-r17 ::= CHOICE
struct location_and_bw_broadcast_r17_c {
  struct types_opts {
    enum options { same_as_sib1_cfg_location_and_bw, location_and_bw, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  location_and_bw_broadcast_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& location_and_bw()
  {
    assert_choice_type(types::location_and_bw, type_, "LocationAndBandwidthBroadcast-r17");
    return c;
  }
  const uint16_t& location_and_bw() const
  {
    assert_choice_type(types::location_and_bw, type_, "LocationAndBandwidthBroadcast-r17");
    return c;
  }
  void      set_same_as_sib1_cfg_location_and_bw();
  uint16_t& set_location_and_bw();

private:
  types    type_;
  uint16_t c;
};

// MBS-FSAI-InterFreq-r17 ::= SEQUENCE
struct mbs_fsai_inter_freq_r17_s {
  uint32_t            dl_carrier_freq_r17 = 0;
  mbs_fsai_list_r17_l mbs_fsai_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCCH-RepetitionPeriodAndOffset-r17 ::= CHOICE
struct mcch_repeat_period_and_offset_r17_c {
  struct types_opts {
    enum options {
      rf1_r17,
      rf2_r17,
      rf4_r17,
      rf8_r17,
      rf16_r17,
      rf32_r17,
      rf64_r17,
      rf128_r17,
      rf256_r17,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mcch_repeat_period_and_offset_r17_c() = default;
  mcch_repeat_period_and_offset_r17_c(const mcch_repeat_period_and_offset_r17_c& other);
  mcch_repeat_period_and_offset_r17_c& operator=(const mcch_repeat_period_and_offset_r17_c& other);
  ~mcch_repeat_period_and_offset_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& rf1_r17()
  {
    assert_choice_type(types::rf1_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& rf2_r17()
  {
    assert_choice_type(types::rf2_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& rf4_r17()
  {
    assert_choice_type(types::rf4_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& rf8_r17()
  {
    assert_choice_type(types::rf8_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& rf16_r17()
  {
    assert_choice_type(types::rf16_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& rf32_r17()
  {
    assert_choice_type(types::rf32_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& rf64_r17()
  {
    assert_choice_type(types::rf64_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& rf128_r17()
  {
    assert_choice_type(types::rf128_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  uint16_t& rf256_r17()
  {
    assert_choice_type(types::rf256_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint16_t>();
  }
  const uint8_t& rf1_r17() const
  {
    assert_choice_type(types::rf1_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& rf2_r17() const
  {
    assert_choice_type(types::rf2_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& rf4_r17() const
  {
    assert_choice_type(types::rf4_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& rf8_r17() const
  {
    assert_choice_type(types::rf8_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& rf16_r17() const
  {
    assert_choice_type(types::rf16_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& rf32_r17() const
  {
    assert_choice_type(types::rf32_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& rf64_r17() const
  {
    assert_choice_type(types::rf64_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& rf128_r17() const
  {
    assert_choice_type(types::rf128_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint8_t>();
  }
  const uint16_t& rf256_r17() const
  {
    assert_choice_type(types::rf256_r17, type_, "MCCH-RepetitionPeriodAndOffset-r17");
    return c.get<uint16_t>();
  }
  uint8_t&  set_rf1_r17();
  uint8_t&  set_rf2_r17();
  uint8_t&  set_rf4_r17();
  uint8_t&  set_rf8_r17();
  uint8_t&  set_rf16_r17();
  uint8_t&  set_rf32_r17();
  uint8_t&  set_rf64_r17();
  uint8_t&  set_rf128_r17();
  uint16_t& set_rf256_r17();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// NTN-NeighCellConfig-r17 ::= SEQUENCE
struct ntn_neigh_cell_cfg_r17_s {
  bool          ntn_cfg_r17_present      = false;
  bool          carrier_freq_r17_present = false;
  bool          pci_r17_present          = false;
  ntn_cfg_r17_s ntn_cfg_r17;
  uint32_t      carrier_freq_r17 = 0;
  uint16_t      pci_r17          = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigBroadcast-r17 ::= SEQUENCE
struct pdsch_cfg_broadcast_r17_s {
  using pdsch_cfg_list_r17_l_                     = dyn_array<pdsch_cfg_ptm_r17_s>;
  using rate_match_pattern_to_add_mod_list_r17_l_ = dyn_array<rate_match_pattern_s>;
  struct mcs_table_r17_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_r17_e_ = enumerated<mcs_table_r17_opts>;
  struct xoverhead_r17_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using xoverhead_r17_e_ = enumerated<xoverhead_r17_opts>;

  // member variables
  bool                                      lte_crs_to_match_around_r17_present = false;
  bool                                      mcs_table_r17_present               = false;
  bool                                      xoverhead_r17_present               = false;
  pdsch_cfg_list_r17_l_                     pdsch_cfg_list_r17;
  pdsch_time_domain_res_alloc_list_r16_l    pdsch_time_domain_alloc_list_r17;
  rate_match_pattern_to_add_mod_list_r17_l_ rate_match_pattern_to_add_mod_list_r17;
  rate_match_pattern_lte_crs_s              lte_crs_to_match_around_r17;
  mcs_table_r17_e_                          mcs_table_r17;
  xoverhead_r17_e_                          xoverhead_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSIB-MappingInfo-r16 ::= SEQUENCE (SIZE (1..32)) OF PosSIB-Type-r16
using pos_sib_map_info_r16_l = dyn_array<pos_sib_type_r16_s>;

// RedCap-ConfigCommonSIB-r17 ::= SEQUENCE
struct red_cap_cfg_common_sib_r17_s {
  struct cell_barred_red_cap_r17_s_ {
    struct cell_barred_red_cap1_rx_r17_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    using cell_barred_red_cap1_rx_r17_e_ = enumerated<cell_barred_red_cap1_rx_r17_opts>;
    struct cell_barred_red_cap2_rx_r17_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    using cell_barred_red_cap2_rx_r17_e_ = enumerated<cell_barred_red_cap2_rx_r17_opts>;

    // member variables
    cell_barred_red_cap1_rx_r17_e_ cell_barred_red_cap1_rx_r17;
    cell_barred_red_cap2_rx_r17_e_ cell_barred_red_cap2_rx_r17;
  };

  // member variables
  bool                       ext                                     = false;
  bool                       half_duplex_red_cap_allowed_r17_present = false;
  bool                       cell_barred_red_cap_r17_present         = false;
  cell_barred_red_cap_r17_s_ cell_barred_red_cap_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDT-ConfigCommonSIB-r17 ::= SEQUENCE
struct sdt_cfg_common_sib_r17_s {
  struct sdt_lc_ch_sr_delay_timer_r17_opts {
    enum options { sf20, sf40, sf64, sf128, sf512, sf1024, sf2560, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using sdt_lc_ch_sr_delay_timer_r17_e_ = enumerated<sdt_lc_ch_sr_delay_timer_r17_opts>;
  struct sdt_data_volume_thres_r17_opts {
    enum options {
      byte32,
      byte100,
      byte200,
      byte400,
      byte600,
      byte800,
      byte1000,
      byte2000,
      byte4000,
      byte8000,
      byte9000,
      byte10000,
      byte12000,
      byte24000,
      byte48000,
      byte96000,
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using sdt_data_volume_thres_r17_e_ = enumerated<sdt_data_volume_thres_r17_opts>;
  struct t319a_r17_opts {
    enum options {
      ms100,
      ms200,
      ms300,
      ms400,
      ms600,
      ms1000,
      ms2000,
      ms3000,
      ms4000,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t319a_r17_e_ = enumerated<t319a_r17_opts>;

  // member variables
  bool                            sdt_rsrp_thres_r17_present           = false;
  bool                            sdt_lc_ch_sr_delay_timer_r17_present = false;
  uint8_t                         sdt_rsrp_thres_r17                   = 0;
  sdt_lc_ch_sr_delay_timer_r17_e_ sdt_lc_ch_sr_delay_timer_r17;
  sdt_data_volume_thres_r17_e_    sdt_data_volume_thres_r17;
  t319a_r17_e_                    t319a_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SI-SchedulingInfo-v1700 ::= SEQUENCE
struct si_sched_info_v1700_s {
  using sched_info_list2_r17_l_ = dyn_array<sched_info2_r17_s>;

  // member variables
  bool                    dummy_present = false;
  sched_info_list2_r17_l_ sched_info_list2_r17;
  si_request_cfg_s        dummy;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-TypeInfo ::= SEQUENCE
struct sib_type_info_s {
  struct type_opts {
    enum options {
      sib_type2,
      sib_type3,
      sib_type4,
      sib_type5,
      sib_type6,
      sib_type7,
      sib_type8,
      sib_type9,
      sib_type10_v1610,
      sib_type11_v1610,
      sib_type12_v1610,
      sib_type13_v1610,
      sib_type14_v1610,
      spare3,
      spare2,
      spare1,
      // ...
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using type_e_ = enumerated<type_opts, true>;

  // member variables
  bool    value_tag_present  = false;
  bool    area_scope_present = false;
  type_e_ type;
  uint8_t value_tag = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB1-v1740-IEs ::= SEQUENCE
struct sib1_v1740_ies_s {
  bool                  si_sched_info_v1740_present = false;
  bool                  non_crit_ext_present        = false;
  si_sched_info_v1740_s si_sched_info_v1740;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringInfoSetList-v1700 ::= SEQUENCE (SIZE (1..8)) OF UAC-BarringInfoSet-v1700
using uac_barr_info_set_list_v1700_l = dyn_array<uac_barr_info_set_v1700_s>;

// UAC-BarringPerCat ::= SEQUENCE
struct uac_barr_per_cat_s {
  uint8_t access_category       = 1;
  uint8_t uac_barr_info_set_idx = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ApplicableDisasterInfo-r17 ::= CHOICE
struct applicable_disaster_info_r17_c {
  using ded_plmns_r17_l_ = dyn_array<plmn_id_s>;
  struct types_opts {
    enum options { no_disaster_roaming_r17, disaster_related_ind_r17, common_plmns_r17, ded_plmns_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  applicable_disaster_info_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ded_plmns_r17_l_& ded_plmns_r17()
  {
    assert_choice_type(types::ded_plmns_r17, type_, "ApplicableDisasterInfo-r17");
    return c;
  }
  const ded_plmns_r17_l_& ded_plmns_r17() const
  {
    assert_choice_type(types::ded_plmns_r17, type_, "ApplicableDisasterInfo-r17");
    return c;
  }
  void              set_no_disaster_roaming_r17();
  void              set_disaster_related_ind_r17();
  void              set_common_plmns_r17();
  ded_plmns_r17_l_& set_ded_plmns_r17();

private:
  types            type_;
  ded_plmns_r17_l_ c;
};

// CFR-ConfigMCCH-MTCH-r17 ::= SEQUENCE
struct cfr_cfg_mcch_mtch_r17_s {
  bool                            location_and_bw_broadcast_r17_present = false;
  bool                            pdsch_cfg_mcch_r17_present            = false;
  bool                            common_coreset_ext_r17_present        = false;
  location_and_bw_broadcast_r17_c location_and_bw_broadcast_r17;
  pdsch_cfg_broadcast_r17_s       pdsch_cfg_mcch_r17;
  coreset_s                       common_coreset_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqListEUTRA ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqEUTRA
using carrier_freq_list_eutra_l = dyn_array<carrier_freq_eutra_s>;

// CarrierFreqListEUTRA-v1610 ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqEUTRA-v1610
using carrier_freq_list_eutra_v1610_l = dyn_array<carrier_freq_eutra_v1610_s>;

// CarrierFreqListEUTRA-v1700 ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqEUTRA-v1700
using carrier_freq_list_eutra_v1700_l = dyn_array<carrier_freq_eutra_v1700_s>;

// FreqPriorityListSlicing-r17 ::= SEQUENCE (SIZE (1..9)) OF FreqPrioritySlicing-r17
using freq_prio_list_slicing_r17_l = dyn_array<freq_prio_slicing_r17_s>;

// GIN-Element-r17 ::= SEQUENCE
struct gin_elem_r17_s {
  using nid_list_r17_l_ = bounded_array<fixed_bitstring<44>, 24>;

  // member variables
  plmn_id_s       plmn_id_r17;
  nid_list_r17_l_ nid_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GINs-PerSNPN-r17 ::= SEQUENCE
struct gi_ns_per_sn_pn_r17_s {
  bool                     supported_gi_ns_r17_present = false;
  bounded_bitstring<1, 24> supported_gi_ns_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HRNN-List-r16 ::= SEQUENCE (SIZE (1..12)) OF HRNN-r16
using hrnn_list_r16_l = dyn_array<hrnn_r16_s>;

// InterFreqCarrierFreqList ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo
using inter_freq_carrier_freq_list_l = dyn_array<inter_freq_carrier_freq_info_s>;

// InterFreqCarrierFreqList-v1610 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1610
using inter_freq_carrier_freq_list_v1610_l = dyn_array<inter_freq_carrier_freq_info_v1610_s>;

// InterFreqCarrierFreqList-v1700 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1700
using inter_freq_carrier_freq_list_v1700_l = dyn_array<inter_freq_carrier_freq_info_v1700_s>;

// InterFreqCarrierFreqList-v1720 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1720
using inter_freq_carrier_freq_list_v1720_l = dyn_array<inter_freq_carrier_freq_info_v1720_s>;

// InterFreqCarrierFreqList-v1730 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1730
using inter_freq_carrier_freq_list_v1730_l = dyn_array<inter_freq_carrier_freq_info_v1730_s>;

// IntraFreqAllowedCellList-r16 ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using intra_freq_allowed_cell_list_r16_l = dyn_array<pci_range_s>;

// IntraFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
struct intra_freq_cag_cell_list_per_plmn_r16_s {
  using cag_cell_list_r16_l_ = dyn_array<pci_range_s>;

  // member variables
  uint8_t              plmn_id_idx_r16 = 1;
  cag_cell_list_r16_l_ cag_cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqExcludedCellList ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using intra_freq_excluded_cell_list_l = dyn_array<pci_range_s>;

// IntraFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF IntraFreqNeighCellInfo
using intra_freq_neigh_cell_list_l = dyn_array<intra_freq_neigh_cell_info_s>;

// IntraFreqNeighCellList-v1610 ::= SEQUENCE (SIZE (1..16)) OF IntraFreqNeighCellInfo-v1610
using intra_freq_neigh_cell_list_v1610_l = dyn_array<intra_freq_neigh_cell_info_v1610_s>;

// IntraFreqNeighCellList-v1710 ::= SEQUENCE (SIZE (1..16)) OF IntraFreqNeighCellInfo-v1710
using intra_freq_neigh_cell_list_v1710_l = dyn_array<intra_freq_neigh_cell_info_v1710_s>;

// IntraFreqNeighHSDN-CellList-r17 ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using intra_freq_neigh_hsdn_cell_list_r17_l = dyn_array<pci_range_s>;

// MBS-FSAI-InterFreqList-r17 ::= SEQUENCE (SIZE (1..8)) OF MBS-FSAI-InterFreq-r17
using mbs_fsai_inter_freq_list_r17_l = dyn_array<mbs_fsai_inter_freq_r17_s>;

// MCCH-Config-r17 ::= SEQUENCE
struct mcch_cfg_r17_s {
  struct mcch_win_dur_r17_opts {
    enum options { sl2, sl4, sl8, sl10, sl20, sl40, sl80, sl160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mcch_win_dur_r17_e_ = enumerated<mcch_win_dur_r17_opts>;
  struct mcch_mod_period_r17_opts {
    enum options {
      rf2,
      rf4,
      rf8,
      rf16,
      rf32,
      rf64,
      rf128,
      rf256,
      rf512,
      rf1024,
      r2048,
      rf4096,
      rf8192,
      rf16384,
      rf32768,
      rf65536,
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using mcch_mod_period_r17_e_ = enumerated<mcch_mod_period_r17_opts>;

  // member variables
  bool                                mcch_win_dur_r17_present = false;
  mcch_repeat_period_and_offset_r17_c mcch_repeat_period_and_offset_r17;
  uint8_t                             mcch_win_start_slot_r17 = 0;
  mcch_win_dur_r17_e_                 mcch_win_dur_r17;
  mcch_mod_period_r17_e_              mcch_mod_period_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdleConfigSIB-r16 ::= SEQUENCE
struct meas_idle_cfg_sib_r16_s {
  using meas_idle_carrier_list_nr_r16_l_    = dyn_array<meas_idle_carrier_nr_r16_s>;
  using meas_idle_carrier_list_eutra_r16_l_ = dyn_array<meas_idle_carrier_eutra_r16_s>;

  // member variables
  bool                                ext = false;
  meas_idle_carrier_list_nr_r16_l_    meas_idle_carrier_list_nr_r16;
  meas_idle_carrier_list_eutra_r16_l_ meas_idle_carrier_list_eutra_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityStateParameters ::= SEQUENCE
struct mob_state_params_s {
  struct t_eval_opts {
    enum options { s30, s60, s120, s180, s240, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using t_eval_e_ = enumerated<t_eval_opts>;
  struct t_hyst_normal_opts {
    enum options { s30, s60, s120, s180, s240, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using t_hyst_normal_e_ = enumerated<t_hyst_normal_opts>;

  // member variables
  t_eval_e_        t_eval;
  t_hyst_normal_e_ t_hyst_normal;
  uint8_t          n_cell_change_medium = 1;
  uint8_t          n_cell_change_high   = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NTN-NeighCellConfigList-r17 ::= SEQUENCE (SIZE (1..4)) OF NTN-NeighCellConfig-r17
using ntn_neigh_cell_cfg_list_r17_l = dyn_array<ntn_neigh_cell_cfg_r17_s>;

// PosSchedulingInfo-r16 ::= SEQUENCE
struct pos_sched_info_r16_s {
  struct pos_si_periodicity_r16_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using pos_si_periodicity_r16_e_ = enumerated<pos_si_periodicity_r16_opts>;
  struct pos_si_broadcast_status_r16_opts {
    enum options { broadcasting, not_broadcasting, nulltype } value;

    const char* to_string() const;
  };
  using pos_si_broadcast_status_r16_e_ = enumerated<pos_si_broadcast_status_r16_opts>;

  // member variables
  bool                           ext                           = false;
  bool                           offset_to_si_used_r16_present = false;
  pos_si_periodicity_r16_e_      pos_si_periodicity_r16;
  pos_si_broadcast_status_r16_e_ pos_si_broadcast_status_r16;
  pos_sib_map_info_r16_l         pos_sib_map_info_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RangeToBestCell ::= Q-OffsetRange
using range_to_best_cell_e = q_offset_range_e;

// SIB-Mapping ::= SEQUENCE (SIZE (1..32)) OF SIB-TypeInfo
using sib_map_l = dyn_array<sib_type_info_s>;

// SIB1-v1700-IEs ::= SEQUENCE
struct sib1_v1700_ies_s {
  struct uac_barr_info_v1700_s_ {
    uac_barr_info_set_list_v1700_l uac_barr_info_set_list_v1700;
  };
  struct feature_priorities_r17_s_ {
    bool    red_cap_prio_r17_present      = false;
    bool    slicing_prio_r17_present      = false;
    bool    msg3_repeats_prio_r17_present = false;
    bool    sdt_prio_r17_present          = false;
    uint8_t red_cap_prio_r17              = 0;
    uint8_t slicing_prio_r17              = 0;
    uint8_t msg3_repeats_prio_r17         = 0;
    uint8_t sdt_prio_r17                  = 0;
  };
  struct intra_freq_resel_red_cap_r17_opts {
    enum options { allowed, not_allowed, nulltype } value;

    const char* to_string() const;
  };
  using intra_freq_resel_red_cap_r17_e_ = enumerated<intra_freq_resel_red_cap_r17_opts>;
  struct cell_barred_ntn_r17_opts {
    enum options { barred, not_barred, nulltype } value;

    const char* to_string() const;
  };
  using cell_barred_ntn_r17_e_ = enumerated<cell_barred_ntn_r17_opts>;

  // member variables
  bool                            hsdn_cell_r17_present                = false;
  bool                            uac_barr_info_v1700_present          = false;
  bool                            sdt_cfg_common_r17_present           = false;
  bool                            red_cap_cfg_common_r17_present       = false;
  bool                            feature_priorities_r17_present       = false;
  bool                            si_sched_info_v1700_present          = false;
  bool                            hyper_sfn_r17_present                = false;
  bool                            edrx_allowed_idle_r17_present        = false;
  bool                            edrx_allowed_inactive_r17_present    = false;
  bool                            intra_freq_resel_red_cap_r17_present = false;
  bool                            cell_barred_ntn_r17_present          = false;
  bool                            non_crit_ext_present                 = false;
  uac_barr_info_v1700_s_          uac_barr_info_v1700;
  sdt_cfg_common_sib_r17_s        sdt_cfg_common_r17;
  red_cap_cfg_common_sib_r17_s    red_cap_cfg_common_r17;
  feature_priorities_r17_s_       feature_priorities_r17;
  si_sched_info_v1700_s           si_sched_info_v1700;
  fixed_bitstring<10>             hyper_sfn_r17;
  intra_freq_resel_red_cap_r17_e_ intra_freq_resel_red_cap_r17;
  cell_barred_ntn_r17_e_          cell_barred_ntn_r17;
  sib1_v1740_ies_s                non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-AC1-SelectAssistInfo-r16 ::= ENUMERATED
struct uac_ac1_select_assist_info_r16_opts {
  enum options { a, b, c, not_cfg, nulltype } value;

  const char* to_string() const;
};
using uac_ac1_select_assist_info_r16_e = enumerated<uac_ac1_select_assist_info_r16_opts>;

// UAC-BarringPerCatList ::= SEQUENCE (SIZE (1..63)) OF UAC-BarringPerCat
using uac_barr_per_cat_list_l = dyn_array<uac_barr_per_cat_s>;

// PosSI-SchedulingInfo-r16 ::= SEQUENCE
struct pos_si_sched_info_r16_s {
  using pos_sched_info_list_r16_l_ = dyn_array<pos_sched_info_r16_s>;

  // member variables
  bool                       ext                                = false;
  bool                       pos_si_request_cfg_r16_present     = false;
  bool                       pos_si_request_cfg_sul_r16_present = false;
  pos_sched_info_list_r16_l_ pos_sched_info_list_r16;
  si_request_cfg_s           pos_si_request_cfg_r16;
  si_request_cfg_s           pos_si_request_cfg_sul_r16;
  // ...
  // group 0
  copy_ptr<si_request_cfg_s> pos_si_request_cfg_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB1-v1630-IEs ::= SEQUENCE
struct sib1_v1630_ies_s {
  struct uac_barr_info_v1630_s_ {
    using uac_ac1_select_assist_info_r16_l_ = bounded_array<uac_ac1_select_assist_info_r16_e, 12>;

    // member variables
    uac_ac1_select_assist_info_r16_l_ uac_ac1_select_assist_info_r16;
  };

  // member variables
  bool                   uac_barr_info_v1630_present = false;
  bool                   non_crit_ext_present        = false;
  uac_barr_info_v1630_s_ uac_barr_info_v1630;
  sib1_v1700_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB10-r16 ::= SEQUENCE
struct sib10_r16_s {
  bool            ext = false;
  hrnn_list_r16_l hrnn_list_r16;
  dyn_octstring   late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB11-r16 ::= SEQUENCE
struct sib11_r16_s {
  bool                    ext                           = false;
  bool                    meas_idle_cfg_sib_r16_present = false;
  meas_idle_cfg_sib_r16_s meas_idle_cfg_sib_r16;
  dyn_octstring           late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB12-r16 ::= SEQUENCE
struct sib12_r16_s {
  struct segment_type_r16_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  using segment_type_r16_e_ = enumerated<segment_type_r16_opts>;

  // member variables
  uint8_t             segment_num_r16 = 0;
  segment_type_r16_e_ segment_type_r16;
  dyn_octstring       segment_container_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB13-r16 ::= SEQUENCE
struct sib13_r16_s {
  bool          ext = false;
  dyn_octstring sl_v2x_cfg_common_r16;
  dyn_octstring dummy;
  dyn_octstring tdd_cfg_r16;
  dyn_octstring late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB14-r16 ::= SEQUENCE
struct sib14_r16_s {
  bool          ext = false;
  dyn_octstring sl_v2x_cfg_common_ext_r16;
  dyn_octstring late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB15-r17 ::= SEQUENCE
struct sib15_r17_s {
  using common_plmns_with_disaster_condition_r17_l_ = dyn_array<plmn_id_s>;
  using applicable_disaster_info_list_r17_l_        = dyn_array<applicable_disaster_info_r17_c>;

  // member variables
  bool                                        ext = false;
  common_plmns_with_disaster_condition_r17_l_ common_plmns_with_disaster_condition_r17;
  applicable_disaster_info_list_r17_l_        applicable_disaster_info_list_r17;
  dyn_octstring                               late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB16-r17 ::= SEQUENCE
struct sib16_r17_s {
  bool                         ext = false;
  freq_prio_list_slicing_r17_l freq_prio_list_slicing_r17;
  dyn_octstring                late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB17-r17 ::= SEQUENCE
struct sib17_r17_s {
  struct segment_type_r17_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  using segment_type_r17_e_ = enumerated<segment_type_r17_opts>;

  // member variables
  uint8_t             segment_num_r17 = 0;
  segment_type_r17_e_ segment_type_r17;
  dyn_octstring       segment_container_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB18-r17 ::= SEQUENCE
struct sib18_r17_s {
  using gin_elem_list_r17_l_       = dyn_array<gin_elem_r17_s>;
  using gins_per_sn_pn_list_r17_l_ = dyn_array<gi_ns_per_sn_pn_r17_s>;

  // member variables
  bool                       ext = false;
  gin_elem_list_r17_l_       gin_elem_list_r17;
  gins_per_sn_pn_list_r17_l_ gins_per_sn_pn_list_r17;
  dyn_octstring              late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB19-r17 ::= SEQUENCE
struct sib19_r17_s {
  bool                          ext                         = false;
  bool                          ntn_cfg_r17_present         = false;
  bool                          t_service_r17_present       = false;
  bool                          distance_thresh_r17_present = false;
  ntn_cfg_r17_s                 ntn_cfg_r17;
  uint64_t                      t_service_r17 = 0;
  dyn_octstring                 ref_location_r17;
  uint16_t                      distance_thresh_r17 = 0;
  ntn_neigh_cell_cfg_list_r17_l ntn_neigh_cell_cfg_list_r17;
  dyn_octstring                 late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<ntn_neigh_cell_cfg_list_r17_l> ntn_neigh_cell_cfg_list_ext_v1720;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB2 ::= SEQUENCE
struct sib2_s {
  struct cell_resel_info_common_s_ {
    struct q_hyst_opts {
      enum options {
        db0,
        db1,
        db2,
        db3,
        db4,
        db5,
        db6,
        db8,
        db10,
        db12,
        db14,
        db16,
        db18,
        db20,
        db22,
        db24,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using q_hyst_e_ = enumerated<q_hyst_opts>;
    struct speed_state_resel_pars_s_ {
      struct q_hyst_sf_s_ {
        struct sf_medium_opts {
          enum options { db_neg6, db_neg4, db_neg2, db0, nulltype } value;
          typedef int8_t number_type;

          const char* to_string() const;
          int8_t      to_number() const;
        };
        using sf_medium_e_ = enumerated<sf_medium_opts>;
        struct sf_high_opts {
          enum options { db_neg6, db_neg4, db_neg2, db0, nulltype } value;
          typedef int8_t number_type;

          const char* to_string() const;
          int8_t      to_number() const;
        };
        using sf_high_e_ = enumerated<sf_high_opts>;

        // member variables
        sf_medium_e_ sf_medium;
        sf_high_e_   sf_high;
      };

      // member variables
      mob_state_params_s mob_state_params;
      q_hyst_sf_s_       q_hyst_sf;
    };

    // member variables
    bool                      ext                                        = false;
    bool                      nrof_ss_blocks_to_average_present          = false;
    bool                      abs_thresh_ss_blocks_consolidation_present = false;
    bool                      range_to_best_cell_present                 = false;
    bool                      speed_state_resel_pars_present             = false;
    uint8_t                   nrof_ss_blocks_to_average                  = 2;
    thres_nr_s                abs_thresh_ss_blocks_consolidation;
    range_to_best_cell_e      range_to_best_cell;
    q_hyst_e_                 q_hyst;
    speed_state_resel_pars_s_ speed_state_resel_pars;
    // ...
  };
  struct cell_resel_serving_freq_info_s_ {
    bool                  ext                          = false;
    bool                  s_non_intra_search_p_present = false;
    bool                  s_non_intra_search_q_present = false;
    bool                  thresh_serving_low_q_present = false;
    bool                  cell_resel_sub_prio_present  = false;
    uint8_t               s_non_intra_search_p         = 0;
    uint8_t               s_non_intra_search_q         = 0;
    uint8_t               thresh_serving_low_p         = 0;
    uint8_t               thresh_serving_low_q         = 0;
    uint8_t               cell_resel_prio              = 0;
    cell_resel_sub_prio_e cell_resel_sub_prio;
    // ...
  };
  struct intra_freq_cell_resel_info_s_ {
    bool                          ext                      = false;
    bool                          q_rx_lev_min_sul_present = false;
    bool                          q_qual_min_present       = false;
    bool                          s_intra_search_q_present = false;
    bool                          p_max_present            = false;
    bool                          smtc_present             = false;
    bool                          ss_rssi_meas_present     = false;
    bool                          ssb_to_measure_present   = false;
    int8_t                        q_rx_lev_min             = -70;
    int8_t                        q_rx_lev_min_sul         = -70;
    int8_t                        q_qual_min               = -43;
    uint8_t                       s_intra_search_p         = 0;
    uint8_t                       s_intra_search_q         = 0;
    uint8_t                       t_resel_nr               = 0;
    multi_freq_band_list_nr_sib_l freq_band_list;
    multi_freq_band_list_nr_sib_l freq_band_list_sul;
    int8_t                        p_max = -30;
    ssb_mtc_s                     smtc;
    ss_rssi_meas_s                ss_rssi_meas;
    ssb_to_measure_c              ssb_to_measure;
    bool                          derive_ssb_idx_from_cell = false;
    // ...
    // group 0
    copy_ptr<speed_state_scale_factors_s> t_resel_nr_sf;
    // group 1
    bool                            ssb_position_qcl_common_r16_present = false;
    copy_ptr<ssb_mtc2_lp_r16_s>     smtc2_lp_r16;
    ssb_position_qcl_relation_r16_e ssb_position_qcl_common_r16;
    // group 2
    bool                            ssb_position_qcl_common_r17_present = false;
    ssb_position_qcl_relation_r17_e ssb_position_qcl_common_r17;
    // group 3
    copy_ptr<ssb_mtc4_list_r17_l> smtc4list_r17;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  struct relaxed_meas_r16_s_ {
    struct low_mob_eval_r16_s_ {
      struct s_search_delta_p_r16_opts {
        enum options { db3, db6, db9, db12, db15, spare3, spare2, spare1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using s_search_delta_p_r16_e_ = enumerated<s_search_delta_p_r16_opts>;
      struct t_search_delta_p_r16_opts {
        enum options {
          s5,
          s10,
          s20,
          s30,
          s60,
          s120,
          s180,
          s240,
          s300,
          spare7,
          spare6,
          spare5,
          spare4,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      using t_search_delta_p_r16_e_ = enumerated<t_search_delta_p_r16_opts>;

      // member variables
      s_search_delta_p_r16_e_ s_search_delta_p_r16;
      t_search_delta_p_r16_e_ t_search_delta_p_r16;
    };
    struct cell_edge_eval_r16_s_ {
      bool    s_search_thres_q_r16_present = false;
      uint8_t s_search_thres_p_r16         = 0;
      uint8_t s_search_thres_q_r16         = 0;
    };

    // member variables
    bool                  low_mob_eval_r16_present                   = false;
    bool                  cell_edge_eval_r16_present                 = false;
    bool                  combine_relaxed_meas_condition_r16_present = false;
    bool                  high_prio_meas_relax_r16_present           = false;
    low_mob_eval_r16_s_   low_mob_eval_r16;
    cell_edge_eval_r16_s_ cell_edge_eval_r16;
  };
  struct relaxed_meas_r17_s_ {
    struct stationary_mob_eval_r17_s_ {
      struct s_search_delta_p_stationary_r17_opts {
        enum options { db2, db3, db6, db9, db12, db15, spare2, spare1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using s_search_delta_p_stationary_r17_e_ = enumerated<s_search_delta_p_stationary_r17_opts>;
      struct t_search_delta_p_stationary_r17_opts {
        enum options {
          s5,
          s10,
          s20,
          s30,
          s60,
          s120,
          s180,
          s240,
          s300,
          spare7,
          spare6,
          spare5,
          spare4,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      using t_search_delta_p_stationary_r17_e_ = enumerated<t_search_delta_p_stationary_r17_opts>;

      // member variables
      s_search_delta_p_stationary_r17_e_ s_search_delta_p_stationary_r17;
      t_search_delta_p_stationary_r17_e_ t_search_delta_p_stationary_r17;
    };
    struct cell_edge_eval_while_stationary_r17_s_ {
      bool    s_search_thres_q2_r17_present = false;
      uint8_t s_search_thres_p2_r17         = 0;
      uint8_t s_search_thres_q2_r17         = 0;
    };

    // member variables
    bool                                   cell_edge_eval_while_stationary_r17_present = false;
    bool                                   combine_relaxed_meas_condition2_r17_present = false;
    stationary_mob_eval_r17_s_             stationary_mob_eval_r17;
    cell_edge_eval_while_stationary_r17_s_ cell_edge_eval_while_stationary_r17;
  };

  // member variables
  bool                            ext = false;
  cell_resel_info_common_s_       cell_resel_info_common;
  cell_resel_serving_freq_info_s_ cell_resel_serving_freq_info;
  intra_freq_cell_resel_info_s_   intra_freq_cell_resel_info;
  // ...
  // group 0
  copy_ptr<relaxed_meas_r16_s_> relaxed_meas_r16;
  // group 1
  bool                          cell_equivalent_size_r17_present = false;
  uint8_t                       cell_equivalent_size_r17         = 2;
  copy_ptr<relaxed_meas_r17_s_> relaxed_meas_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB20-r17 ::= SEQUENCE
struct sib20_r17_s {
  bool                    ext                           = false;
  bool                    cfr_cfg_mcch_mtch_r17_present = false;
  mcch_cfg_r17_s          mcch_cfg_r17;
  cfr_cfg_mcch_mtch_r17_s cfr_cfg_mcch_mtch_r17;
  dyn_octstring           late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB21-r17 ::= SEQUENCE
struct sib21_r17_s {
  bool                           ext = false;
  mbs_fsai_list_r17_l            mbs_fsai_intra_freq_r17;
  mbs_fsai_inter_freq_list_r17_l mbs_fsai_inter_freq_list_r17;
  dyn_octstring                  late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB3 ::= SEQUENCE
struct sib3_s {
  using intra_freq_cag_cell_list_r16_l_ = dyn_array<intra_freq_cag_cell_list_per_plmn_r16_s>;

  // member variables
  bool                            ext = false;
  intra_freq_neigh_cell_list_l    intra_freq_neigh_cell_list;
  intra_freq_excluded_cell_list_l intra_freq_excluded_cell_list;
  dyn_octstring                   late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<intra_freq_neigh_cell_list_v1610_l> intra_freq_neigh_cell_list_v1610;
  copy_ptr<intra_freq_allowed_cell_list_r16_l> intra_freq_allowed_cell_list_r16;
  copy_ptr<intra_freq_cag_cell_list_r16_l_>    intra_freq_cag_cell_list_r16;
  // group 1
  copy_ptr<intra_freq_neigh_hsdn_cell_list_r17_l> intra_freq_neigh_hsdn_cell_list_r17;
  copy_ptr<intra_freq_neigh_cell_list_v1710_l>    intra_freq_neigh_cell_list_v1710;
  // group 2
  bool ch_access_mode2_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB4 ::= SEQUENCE
struct sib4_s {
  bool                           ext = false;
  inter_freq_carrier_freq_list_l inter_freq_carrier_freq_list;
  dyn_octstring                  late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<inter_freq_carrier_freq_list_v1610_l> inter_freq_carrier_freq_list_v1610;
  // group 1
  copy_ptr<inter_freq_carrier_freq_list_v1700_l> inter_freq_carrier_freq_list_v1700;
  // group 2
  copy_ptr<inter_freq_carrier_freq_list_v1720_l> inter_freq_carrier_freq_list_v1720;
  // group 3
  copy_ptr<inter_freq_carrier_freq_list_v1730_l> inter_freq_carrier_freq_list_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB5 ::= SEQUENCE
struct sib5_s {
  bool                        ext                      = false;
  bool                        t_resel_eutra_sf_present = false;
  carrier_freq_list_eutra_l   carrier_freq_list_eutra;
  uint8_t                     t_resel_eutra = 0;
  speed_state_scale_factors_s t_resel_eutra_sf;
  dyn_octstring               late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<carrier_freq_list_eutra_v1610_l> carrier_freq_list_eutra_v1610;
  // group 1
  bool                                      idle_mode_meas_voice_fallback_r17_present = false;
  copy_ptr<carrier_freq_list_eutra_v1700_l> carrier_freq_list_eutra_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB6 ::= SEQUENCE
struct sib6_s {
  bool                ext = false;
  fixed_bitstring<16> msg_id;
  fixed_bitstring<16> serial_num;
  fixed_octstring<2>  warning_type;
  dyn_octstring       late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB7 ::= SEQUENCE
struct sib7_s {
  struct warning_msg_segment_type_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  using warning_msg_segment_type_e_ = enumerated<warning_msg_segment_type_opts>;

  // member variables
  bool                        ext                        = false;
  bool                        data_coding_scheme_present = false;
  fixed_bitstring<16>         msg_id;
  fixed_bitstring<16>         serial_num;
  warning_msg_segment_type_e_ warning_msg_segment_type;
  uint8_t                     warning_msg_segment_num = 0;
  dyn_octstring               warning_msg_segment;
  fixed_octstring<1>          data_coding_scheme;
  dyn_octstring               late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB8 ::= SEQUENCE
struct sib8_s {
  struct warning_msg_segment_type_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  using warning_msg_segment_type_e_ = enumerated<warning_msg_segment_type_opts>;

  // member variables
  bool                        ext                        = false;
  bool                        data_coding_scheme_present = false;
  fixed_bitstring<16>         msg_id;
  fixed_bitstring<16>         serial_num;
  warning_msg_segment_type_e_ warning_msg_segment_type;
  uint8_t                     warning_msg_segment_num = 0;
  dyn_octstring               warning_msg_segment;
  fixed_octstring<1>          data_coding_scheme;
  dyn_octstring               warning_area_coordinates_segment;
  dyn_octstring               late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB9 ::= SEQUENCE
struct sib9_s {
  struct time_info_s_ {
    bool               day_light_saving_time_present = false;
    bool               leap_seconds_present          = false;
    bool               local_time_offset_present     = false;
    uint64_t           time_info_utc                 = 0;
    fixed_bitstring<2> day_light_saving_time;
    int16_t            leap_seconds      = -127;
    int8_t             local_time_offset = -63;
  };

  // member variables
  bool          ext               = false;
  bool          time_info_present = false;
  time_info_s_  time_info;
  dyn_octstring late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<ref_time_info_r16_s> ref_time_info_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIBpos-r16 ::= SEQUENCE
struct sib_pos_r16_s {
  bool          ext = false;
  dyn_octstring assist_data_sib_elem_r16;
  dyn_octstring late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfo ::= SEQUENCE
struct sched_info_s {
  struct si_broadcast_status_opts {
    enum options { broadcasting, not_broadcasting, nulltype } value;

    const char* to_string() const;
  };
  using si_broadcast_status_e_ = enumerated<si_broadcast_status_opts>;
  struct si_periodicity_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using si_periodicity_e_ = enumerated<si_periodicity_opts>;

  // member variables
  si_broadcast_status_e_ si_broadcast_status;
  si_periodicity_e_      si_periodicity;
  sib_map_l              sib_map_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringInfoSet ::= SEQUENCE
struct uac_barr_info_set_s {
  struct uac_barr_factor_opts {
    enum options { p00, p05, p10, p15, p20, p25, p30, p40, p50, p60, p70, p75, p80, p85, p90, p95, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using uac_barr_factor_e_ = enumerated<uac_barr_factor_opts>;
  struct uac_barr_time_opts {
    enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using uac_barr_time_e_ = enumerated<uac_barr_time_opts>;

  // member variables
  uac_barr_factor_e_ uac_barr_factor;
  uac_barr_time_e_   uac_barr_time;
  fixed_bitstring<7> uac_barr_for_access_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringPerPLMN ::= SEQUENCE
struct uac_barr_per_plmn_s {
  struct uac_ac_barr_list_type_c_ {
    using uac_implicit_ac_barr_list_l_ = std::array<uint8_t, 63>;
    struct types_opts {
      enum options { uac_implicit_ac_barr_list, uac_explicit_ac_barr_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    uac_ac_barr_list_type_c_() = default;
    uac_ac_barr_list_type_c_(const uac_ac_barr_list_type_c_& other);
    uac_ac_barr_list_type_c_& operator=(const uac_ac_barr_list_type_c_& other);
    ~uac_ac_barr_list_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uac_implicit_ac_barr_list_l_& uac_implicit_ac_barr_list()
    {
      assert_choice_type(types::uac_implicit_ac_barr_list, type_, "uac-ACBarringListType");
      return c.get<uac_implicit_ac_barr_list_l_>();
    }
    uac_barr_per_cat_list_l& uac_explicit_ac_barr_list()
    {
      assert_choice_type(types::uac_explicit_ac_barr_list, type_, "uac-ACBarringListType");
      return c.get<uac_barr_per_cat_list_l>();
    }
    const uac_implicit_ac_barr_list_l_& uac_implicit_ac_barr_list() const
    {
      assert_choice_type(types::uac_implicit_ac_barr_list, type_, "uac-ACBarringListType");
      return c.get<uac_implicit_ac_barr_list_l_>();
    }
    const uac_barr_per_cat_list_l& uac_explicit_ac_barr_list() const
    {
      assert_choice_type(types::uac_explicit_ac_barr_list, type_, "uac-ACBarringListType");
      return c.get<uac_barr_per_cat_list_l>();
    }
    uac_implicit_ac_barr_list_l_& set_uac_implicit_ac_barr_list();
    uac_barr_per_cat_list_l&      set_uac_explicit_ac_barr_list();

  private:
    types                                                                  type_;
    choice_buffer_t<uac_barr_per_cat_list_l, uac_implicit_ac_barr_list_l_> c;

    void destroy_();
  };

  // member variables
  bool                     uac_ac_barr_list_type_present = false;
  uint8_t                  plmn_id_idx                   = 1;
  uac_ac_barr_list_type_c_ uac_ac_barr_list_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConnEstFailureControl ::= SEQUENCE
struct conn_est_fail_ctrl_s {
  struct conn_est_fail_count_opts {
    enum options { n1, n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using conn_est_fail_count_e_ = enumerated<conn_est_fail_count_opts>;
  struct conn_est_fail_offset_validity_opts {
    enum options { s30, s60, s120, s240, s300, s420, s600, s900, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using conn_est_fail_offset_validity_e_ = enumerated<conn_est_fail_offset_validity_opts>;

  // member variables
  bool                             conn_est_fail_offset_present = false;
  conn_est_fail_count_e_           conn_est_fail_count;
  conn_est_fail_offset_validity_e_ conn_est_fail_offset_validity;
  uint8_t                          conn_est_fail_offset = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSystemInformation-r16-IEs ::= SEQUENCE
struct pos_sys_info_r16_ies_s {
  struct pos_sib_type_and_info_r16_item_c_ {
    struct types_opts {
      enum options {
        pos_sib1_1_r16,
        pos_sib1_2_r16,
        pos_sib1_3_r16,
        pos_sib1_4_r16,
        pos_sib1_5_r16,
        pos_sib1_6_r16,
        pos_sib1_7_r16,
        pos_sib1_8_r16,
        pos_sib2_1_r16,
        pos_sib2_2_r16,
        pos_sib2_3_r16,
        pos_sib2_4_r16,
        pos_sib2_5_r16,
        pos_sib2_6_r16,
        pos_sib2_7_r16,
        pos_sib2_8_r16,
        pos_sib2_9_r16,
        pos_sib2_10_r16,
        pos_sib2_11_r16,
        pos_sib2_12_r16,
        pos_sib2_13_r16,
        pos_sib2_14_r16,
        pos_sib2_15_r16,
        pos_sib2_16_r16,
        pos_sib2_17_r16,
        pos_sib2_18_r16,
        pos_sib2_19_r16,
        pos_sib2_20_r16,
        pos_sib2_21_r16,
        pos_sib2_22_r16,
        pos_sib2_23_r16,
        pos_sib3_1_r16,
        pos_sib4_1_r16,
        pos_sib5_1_r16,
        pos_sib6_1_r16,
        pos_sib6_2_r16,
        pos_sib6_3_r16,
        // ...
        pos_sib1_9_v1700,
        pos_sib1_10_v1700,
        pos_sib2_24_v1700,
        pos_sib2_25_v1700,
        pos_sib6_4_v1700,
        pos_sib6_5_v1700,
        pos_sib6_6_v1700,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 7>;

    // choice methods
    pos_sib_type_and_info_r16_item_c_() = default;
    pos_sib_type_and_info_r16_item_c_(const pos_sib_type_and_info_r16_item_c_& other);
    pos_sib_type_and_info_r16_item_c_& operator=(const pos_sib_type_and_info_r16_item_c_& other);
    ~pos_sib_type_and_info_r16_item_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sib_pos_r16_s& pos_sib1_1_r16()
    {
      assert_choice_type(types::pos_sib1_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_2_r16()
    {
      assert_choice_type(types::pos_sib1_2_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_3_r16()
    {
      assert_choice_type(types::pos_sib1_3_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_4_r16()
    {
      assert_choice_type(types::pos_sib1_4_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_5_r16()
    {
      assert_choice_type(types::pos_sib1_5_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_6_r16()
    {
      assert_choice_type(types::pos_sib1_6_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_7_r16()
    {
      assert_choice_type(types::pos_sib1_7_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_8_r16()
    {
      assert_choice_type(types::pos_sib1_8_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_1_r16()
    {
      assert_choice_type(types::pos_sib2_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_2_r16()
    {
      assert_choice_type(types::pos_sib2_2_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_3_r16()
    {
      assert_choice_type(types::pos_sib2_3_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_4_r16()
    {
      assert_choice_type(types::pos_sib2_4_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_5_r16()
    {
      assert_choice_type(types::pos_sib2_5_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_6_r16()
    {
      assert_choice_type(types::pos_sib2_6_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_7_r16()
    {
      assert_choice_type(types::pos_sib2_7_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_8_r16()
    {
      assert_choice_type(types::pos_sib2_8_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_9_r16()
    {
      assert_choice_type(types::pos_sib2_9_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_10_r16()
    {
      assert_choice_type(types::pos_sib2_10_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_11_r16()
    {
      assert_choice_type(types::pos_sib2_11_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_12_r16()
    {
      assert_choice_type(types::pos_sib2_12_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_13_r16()
    {
      assert_choice_type(types::pos_sib2_13_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_14_r16()
    {
      assert_choice_type(types::pos_sib2_14_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_15_r16()
    {
      assert_choice_type(types::pos_sib2_15_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_16_r16()
    {
      assert_choice_type(types::pos_sib2_16_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_17_r16()
    {
      assert_choice_type(types::pos_sib2_17_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_18_r16()
    {
      assert_choice_type(types::pos_sib2_18_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_19_r16()
    {
      assert_choice_type(types::pos_sib2_19_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_20_r16()
    {
      assert_choice_type(types::pos_sib2_20_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_21_r16()
    {
      assert_choice_type(types::pos_sib2_21_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_22_r16()
    {
      assert_choice_type(types::pos_sib2_22_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_23_r16()
    {
      assert_choice_type(types::pos_sib2_23_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib3_1_r16()
    {
      assert_choice_type(types::pos_sib3_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib4_1_r16()
    {
      assert_choice_type(types::pos_sib4_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib5_1_r16()
    {
      assert_choice_type(types::pos_sib5_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib6_1_r16()
    {
      assert_choice_type(types::pos_sib6_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib6_2_r16()
    {
      assert_choice_type(types::pos_sib6_2_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib6_3_r16()
    {
      assert_choice_type(types::pos_sib6_3_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_9_v1700()
    {
      assert_choice_type(types::pos_sib1_9_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib1_10_v1700()
    {
      assert_choice_type(types::pos_sib1_10_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_24_v1700()
    {
      assert_choice_type(types::pos_sib2_24_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib2_25_v1700()
    {
      assert_choice_type(types::pos_sib2_25_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib6_4_v1700()
    {
      assert_choice_type(types::pos_sib6_4_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib6_5_v1700()
    {
      assert_choice_type(types::pos_sib6_5_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& pos_sib6_6_v1700()
    {
      assert_choice_type(types::pos_sib6_6_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_1_r16() const
    {
      assert_choice_type(types::pos_sib1_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_2_r16() const
    {
      assert_choice_type(types::pos_sib1_2_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_3_r16() const
    {
      assert_choice_type(types::pos_sib1_3_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_4_r16() const
    {
      assert_choice_type(types::pos_sib1_4_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_5_r16() const
    {
      assert_choice_type(types::pos_sib1_5_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_6_r16() const
    {
      assert_choice_type(types::pos_sib1_6_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_7_r16() const
    {
      assert_choice_type(types::pos_sib1_7_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_8_r16() const
    {
      assert_choice_type(types::pos_sib1_8_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_1_r16() const
    {
      assert_choice_type(types::pos_sib2_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_2_r16() const
    {
      assert_choice_type(types::pos_sib2_2_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_3_r16() const
    {
      assert_choice_type(types::pos_sib2_3_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_4_r16() const
    {
      assert_choice_type(types::pos_sib2_4_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_5_r16() const
    {
      assert_choice_type(types::pos_sib2_5_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_6_r16() const
    {
      assert_choice_type(types::pos_sib2_6_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_7_r16() const
    {
      assert_choice_type(types::pos_sib2_7_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_8_r16() const
    {
      assert_choice_type(types::pos_sib2_8_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_9_r16() const
    {
      assert_choice_type(types::pos_sib2_9_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_10_r16() const
    {
      assert_choice_type(types::pos_sib2_10_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_11_r16() const
    {
      assert_choice_type(types::pos_sib2_11_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_12_r16() const
    {
      assert_choice_type(types::pos_sib2_12_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_13_r16() const
    {
      assert_choice_type(types::pos_sib2_13_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_14_r16() const
    {
      assert_choice_type(types::pos_sib2_14_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_15_r16() const
    {
      assert_choice_type(types::pos_sib2_15_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_16_r16() const
    {
      assert_choice_type(types::pos_sib2_16_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_17_r16() const
    {
      assert_choice_type(types::pos_sib2_17_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_18_r16() const
    {
      assert_choice_type(types::pos_sib2_18_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_19_r16() const
    {
      assert_choice_type(types::pos_sib2_19_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_20_r16() const
    {
      assert_choice_type(types::pos_sib2_20_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_21_r16() const
    {
      assert_choice_type(types::pos_sib2_21_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_22_r16() const
    {
      assert_choice_type(types::pos_sib2_22_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_23_r16() const
    {
      assert_choice_type(types::pos_sib2_23_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib3_1_r16() const
    {
      assert_choice_type(types::pos_sib3_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib4_1_r16() const
    {
      assert_choice_type(types::pos_sib4_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib5_1_r16() const
    {
      assert_choice_type(types::pos_sib5_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib6_1_r16() const
    {
      assert_choice_type(types::pos_sib6_1_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib6_2_r16() const
    {
      assert_choice_type(types::pos_sib6_2_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib6_3_r16() const
    {
      assert_choice_type(types::pos_sib6_3_r16, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_9_v1700() const
    {
      assert_choice_type(types::pos_sib1_9_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib1_10_v1700() const
    {
      assert_choice_type(types::pos_sib1_10_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_24_v1700() const
    {
      assert_choice_type(types::pos_sib2_24_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib2_25_v1700() const
    {
      assert_choice_type(types::pos_sib2_25_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib6_4_v1700() const
    {
      assert_choice_type(types::pos_sib6_4_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib6_5_v1700() const
    {
      assert_choice_type(types::pos_sib6_5_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    const sib_pos_r16_s& pos_sib6_6_v1700() const
    {
      assert_choice_type(types::pos_sib6_6_v1700, type_, "posSIB-TypeAndInfo-r16-item");
      return c.get<sib_pos_r16_s>();
    }
    sib_pos_r16_s& set_pos_sib1_1_r16();
    sib_pos_r16_s& set_pos_sib1_2_r16();
    sib_pos_r16_s& set_pos_sib1_3_r16();
    sib_pos_r16_s& set_pos_sib1_4_r16();
    sib_pos_r16_s& set_pos_sib1_5_r16();
    sib_pos_r16_s& set_pos_sib1_6_r16();
    sib_pos_r16_s& set_pos_sib1_7_r16();
    sib_pos_r16_s& set_pos_sib1_8_r16();
    sib_pos_r16_s& set_pos_sib2_1_r16();
    sib_pos_r16_s& set_pos_sib2_2_r16();
    sib_pos_r16_s& set_pos_sib2_3_r16();
    sib_pos_r16_s& set_pos_sib2_4_r16();
    sib_pos_r16_s& set_pos_sib2_5_r16();
    sib_pos_r16_s& set_pos_sib2_6_r16();
    sib_pos_r16_s& set_pos_sib2_7_r16();
    sib_pos_r16_s& set_pos_sib2_8_r16();
    sib_pos_r16_s& set_pos_sib2_9_r16();
    sib_pos_r16_s& set_pos_sib2_10_r16();
    sib_pos_r16_s& set_pos_sib2_11_r16();
    sib_pos_r16_s& set_pos_sib2_12_r16();
    sib_pos_r16_s& set_pos_sib2_13_r16();
    sib_pos_r16_s& set_pos_sib2_14_r16();
    sib_pos_r16_s& set_pos_sib2_15_r16();
    sib_pos_r16_s& set_pos_sib2_16_r16();
    sib_pos_r16_s& set_pos_sib2_17_r16();
    sib_pos_r16_s& set_pos_sib2_18_r16();
    sib_pos_r16_s& set_pos_sib2_19_r16();
    sib_pos_r16_s& set_pos_sib2_20_r16();
    sib_pos_r16_s& set_pos_sib2_21_r16();
    sib_pos_r16_s& set_pos_sib2_22_r16();
    sib_pos_r16_s& set_pos_sib2_23_r16();
    sib_pos_r16_s& set_pos_sib3_1_r16();
    sib_pos_r16_s& set_pos_sib4_1_r16();
    sib_pos_r16_s& set_pos_sib5_1_r16();
    sib_pos_r16_s& set_pos_sib6_1_r16();
    sib_pos_r16_s& set_pos_sib6_2_r16();
    sib_pos_r16_s& set_pos_sib6_3_r16();
    sib_pos_r16_s& set_pos_sib1_9_v1700();
    sib_pos_r16_s& set_pos_sib1_10_v1700();
    sib_pos_r16_s& set_pos_sib2_24_v1700();
    sib_pos_r16_s& set_pos_sib2_25_v1700();
    sib_pos_r16_s& set_pos_sib6_4_v1700();
    sib_pos_r16_s& set_pos_sib6_5_v1700();
    sib_pos_r16_s& set_pos_sib6_6_v1700();

  private:
    types                          type_;
    choice_buffer_t<sib_pos_r16_s> c;

    void destroy_();
  };
  using pos_sib_type_and_info_r16_l_ = dyn_array<pos_sib_type_and_info_r16_item_c_>;

  // member variables
  bool                         non_crit_ext_present = false;
  pos_sib_type_and_info_r16_l_ pos_sib_type_and_info_r16;
  dyn_octstring                late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SI-SchedulingInfo ::= SEQUENCE
struct si_sched_info_s {
  using sched_info_list_l_ = dyn_array<sched_info_s>;
  struct si_win_len_opts {
    enum options { s5, s10, s20, s40, s80, s160, s320, s640, s1280, s2560_v1710, s5120_v1710, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using si_win_len_e_ = enumerated<si_win_len_opts>;

  // member variables
  bool                ext                        = false;
  bool                si_request_cfg_present     = false;
  bool                si_request_cfg_sul_present = false;
  bool                sys_info_area_id_present   = false;
  sched_info_list_l_  sched_info_list;
  si_win_len_e_       si_win_len;
  si_request_cfg_s    si_request_cfg;
  si_request_cfg_s    si_request_cfg_sul;
  fixed_bitstring<24> sys_info_area_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB1-v1610-IEs ::= SEQUENCE
struct sib1_v1610_ies_s {
  bool                    idle_mode_meass_eutra_r16_present = false;
  bool                    idle_mode_meass_nr_r16_present    = false;
  bool                    pos_si_sched_info_r16_present     = false;
  bool                    non_crit_ext_present              = false;
  pos_si_sched_info_r16_s pos_si_sched_info_r16;
  sib1_v1630_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation-IEs ::= SEQUENCE
struct sys_info_ies_s {
  struct sib_type_and_info_item_c_ {
    struct types_opts {
      enum options {
        sib2,
        sib3,
        sib4,
        sib5,
        sib6,
        sib7,
        sib8,
        sib9,
        // ...
        sib10_v1610,
        sib11_v1610,
        sib12_v1610,
        sib13_v1610,
        sib14_v1610,
        sib15_v1700,
        sib16_v1700,
        sib17_v1700,
        sib18_v1700,
        sib19_v1700,
        sib20_v1700,
        sib21_v1700,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 12>;

    // choice methods
    sib_type_and_info_item_c_() = default;
    sib_type_and_info_item_c_(const sib_type_and_info_item_c_& other);
    sib_type_and_info_item_c_& operator=(const sib_type_and_info_item_c_& other);
    ~sib_type_and_info_item_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sib2_s& sib2()
    {
      assert_choice_type(types::sib2, type_, "sib-TypeAndInfo-item");
      return c.get<sib2_s>();
    }
    sib3_s& sib3()
    {
      assert_choice_type(types::sib3, type_, "sib-TypeAndInfo-item");
      return c.get<sib3_s>();
    }
    sib4_s& sib4()
    {
      assert_choice_type(types::sib4, type_, "sib-TypeAndInfo-item");
      return c.get<sib4_s>();
    }
    sib5_s& sib5()
    {
      assert_choice_type(types::sib5, type_, "sib-TypeAndInfo-item");
      return c.get<sib5_s>();
    }
    sib6_s& sib6()
    {
      assert_choice_type(types::sib6, type_, "sib-TypeAndInfo-item");
      return c.get<sib6_s>();
    }
    sib7_s& sib7()
    {
      assert_choice_type(types::sib7, type_, "sib-TypeAndInfo-item");
      return c.get<sib7_s>();
    }
    sib8_s& sib8()
    {
      assert_choice_type(types::sib8, type_, "sib-TypeAndInfo-item");
      return c.get<sib8_s>();
    }
    sib9_s& sib9()
    {
      assert_choice_type(types::sib9, type_, "sib-TypeAndInfo-item");
      return c.get<sib9_s>();
    }
    sib10_r16_s& sib10_v1610()
    {
      assert_choice_type(types::sib10_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib10_r16_s>();
    }
    sib11_r16_s& sib11_v1610()
    {
      assert_choice_type(types::sib11_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib11_r16_s>();
    }
    sib12_r16_s& sib12_v1610()
    {
      assert_choice_type(types::sib12_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib12_r16_s>();
    }
    sib13_r16_s& sib13_v1610()
    {
      assert_choice_type(types::sib13_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib13_r16_s>();
    }
    sib14_r16_s& sib14_v1610()
    {
      assert_choice_type(types::sib14_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib14_r16_s>();
    }
    sib15_r17_s& sib15_v1700()
    {
      assert_choice_type(types::sib15_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib15_r17_s>();
    }
    sib16_r17_s& sib16_v1700()
    {
      assert_choice_type(types::sib16_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib16_r17_s>();
    }
    sib17_r17_s& sib17_v1700()
    {
      assert_choice_type(types::sib17_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib17_r17_s>();
    }
    sib18_r17_s& sib18_v1700()
    {
      assert_choice_type(types::sib18_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib18_r17_s>();
    }
    sib19_r17_s& sib19_v1700()
    {
      assert_choice_type(types::sib19_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib19_r17_s>();
    }
    sib20_r17_s& sib20_v1700()
    {
      assert_choice_type(types::sib20_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib20_r17_s>();
    }
    sib21_r17_s& sib21_v1700()
    {
      assert_choice_type(types::sib21_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib21_r17_s>();
    }
    const sib2_s& sib2() const
    {
      assert_choice_type(types::sib2, type_, "sib-TypeAndInfo-item");
      return c.get<sib2_s>();
    }
    const sib3_s& sib3() const
    {
      assert_choice_type(types::sib3, type_, "sib-TypeAndInfo-item");
      return c.get<sib3_s>();
    }
    const sib4_s& sib4() const
    {
      assert_choice_type(types::sib4, type_, "sib-TypeAndInfo-item");
      return c.get<sib4_s>();
    }
    const sib5_s& sib5() const
    {
      assert_choice_type(types::sib5, type_, "sib-TypeAndInfo-item");
      return c.get<sib5_s>();
    }
    const sib6_s& sib6() const
    {
      assert_choice_type(types::sib6, type_, "sib-TypeAndInfo-item");
      return c.get<sib6_s>();
    }
    const sib7_s& sib7() const
    {
      assert_choice_type(types::sib7, type_, "sib-TypeAndInfo-item");
      return c.get<sib7_s>();
    }
    const sib8_s& sib8() const
    {
      assert_choice_type(types::sib8, type_, "sib-TypeAndInfo-item");
      return c.get<sib8_s>();
    }
    const sib9_s& sib9() const
    {
      assert_choice_type(types::sib9, type_, "sib-TypeAndInfo-item");
      return c.get<sib9_s>();
    }
    const sib10_r16_s& sib10_v1610() const
    {
      assert_choice_type(types::sib10_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib10_r16_s>();
    }
    const sib11_r16_s& sib11_v1610() const
    {
      assert_choice_type(types::sib11_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib11_r16_s>();
    }
    const sib12_r16_s& sib12_v1610() const
    {
      assert_choice_type(types::sib12_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib12_r16_s>();
    }
    const sib13_r16_s& sib13_v1610() const
    {
      assert_choice_type(types::sib13_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib13_r16_s>();
    }
    const sib14_r16_s& sib14_v1610() const
    {
      assert_choice_type(types::sib14_v1610, type_, "sib-TypeAndInfo-item");
      return c.get<sib14_r16_s>();
    }
    const sib15_r17_s& sib15_v1700() const
    {
      assert_choice_type(types::sib15_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib15_r17_s>();
    }
    const sib16_r17_s& sib16_v1700() const
    {
      assert_choice_type(types::sib16_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib16_r17_s>();
    }
    const sib17_r17_s& sib17_v1700() const
    {
      assert_choice_type(types::sib17_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib17_r17_s>();
    }
    const sib18_r17_s& sib18_v1700() const
    {
      assert_choice_type(types::sib18_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib18_r17_s>();
    }
    const sib19_r17_s& sib19_v1700() const
    {
      assert_choice_type(types::sib19_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib19_r17_s>();
    }
    const sib20_r17_s& sib20_v1700() const
    {
      assert_choice_type(types::sib20_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib20_r17_s>();
    }
    const sib21_r17_s& sib21_v1700() const
    {
      assert_choice_type(types::sib21_v1700, type_, "sib-TypeAndInfo-item");
      return c.get<sib21_r17_s>();
    }
    sib2_s&      set_sib2();
    sib3_s&      set_sib3();
    sib4_s&      set_sib4();
    sib5_s&      set_sib5();
    sib6_s&      set_sib6();
    sib7_s&      set_sib7();
    sib8_s&      set_sib8();
    sib9_s&      set_sib9();
    sib10_r16_s& set_sib10_v1610();
    sib11_r16_s& set_sib11_v1610();
    sib12_r16_s& set_sib12_v1610();
    sib13_r16_s& set_sib13_v1610();
    sib14_r16_s& set_sib14_v1610();
    sib15_r17_s& set_sib15_v1700();
    sib16_r17_s& set_sib16_v1700();
    sib17_r17_s& set_sib17_v1700();
    sib18_r17_s& set_sib18_v1700();
    sib19_r17_s& set_sib19_v1700();
    sib20_r17_s& set_sib20_v1700();
    sib21_r17_s& set_sib21_v1700();

  private:
    types type_;
    choice_buffer_t<sib10_r16_s,
                    sib11_r16_s,
                    sib12_r16_s,
                    sib13_r16_s,
                    sib14_r16_s,
                    sib15_r17_s,
                    sib16_r17_s,
                    sib17_r17_s,
                    sib18_r17_s,
                    sib19_r17_s,
                    sib20_r17_s,
                    sib21_r17_s,
                    sib2_s,
                    sib3_s,
                    sib4_s,
                    sib5_s,
                    sib6_s,
                    sib7_s,
                    sib8_s,
                    sib9_s>
        c;

    void destroy_();
  };
  using sib_type_and_info_l_ = dyn_array<sib_type_and_info_item_c_>;

  // member variables
  bool                 non_crit_ext_present = false;
  sib_type_and_info_l_ sib_type_and_info;
  dyn_octstring        late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-AccessCategory1-SelectionAssistanceInfo ::= ENUMERATED
struct uac_access_category1_sel_assist_info_opts {
  enum options { a, b, c, nulltype } value;

  const char* to_string() const;
};
using uac_access_category1_sel_assist_info_e = enumerated<uac_access_category1_sel_assist_info_opts>;

// UAC-BarringInfoSetList ::= SEQUENCE (SIZE (1..8)) OF UAC-BarringInfoSet
using uac_barr_info_set_list_l = dyn_array<uac_barr_info_set_s>;

// UAC-BarringPerPLMN-List ::= SEQUENCE (SIZE (1..12)) OF UAC-BarringPerPLMN
using uac_barr_per_plmn_list_l = dyn_array<uac_barr_per_plmn_s>;

// UE-TimersAndConstants ::= SEQUENCE
struct ue_timers_and_consts_s {
  struct t300_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t300_e_ = enumerated<t300_opts>;
  struct t301_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t301_e_ = enumerated<t301_opts>;
  struct t310_opts {
    enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t310_e_ = enumerated<t310_opts>;
  struct n310_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using n310_e_ = enumerated<n310_opts>;
  struct t311_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t311_e_ = enumerated<t311_opts>;
  struct n311_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using n311_e_ = enumerated<n311_opts>;
  struct t319_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t319_e_ = enumerated<t319_opts>;

  // member variables
  bool    ext = false;
  t300_e_ t300;
  t301_e_ t301;
  t310_e_ t310;
  n310_e_ n310;
  t311_e_ t311;
  n311_e_ n311;
  t319_e_ t319;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB1 ::= SEQUENCE
struct sib1_s {
  struct cell_sel_info_s_ {
    bool    q_rx_lev_min_offset_present = false;
    bool    q_rx_lev_min_sul_present    = false;
    bool    q_qual_min_present          = false;
    bool    q_qual_min_offset_present   = false;
    int8_t  q_rx_lev_min                = -70;
    uint8_t q_rx_lev_min_offset         = 1;
    int8_t  q_rx_lev_min_sul            = -70;
    int8_t  q_qual_min                  = -43;
    uint8_t q_qual_min_offset           = 1;
  };
  struct uac_barr_info_s_ {
    struct uac_access_category1_sel_assist_info_c_ {
      using individual_plmn_list_l_ = bounded_array<uac_access_category1_sel_assist_info_e, 12>;
      struct types_opts {
        enum options { plmn_common, individual_plmn_list, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      uac_access_category1_sel_assist_info_c_() = default;
      uac_access_category1_sel_assist_info_c_(const uac_access_category1_sel_assist_info_c_& other);
      uac_access_category1_sel_assist_info_c_& operator=(const uac_access_category1_sel_assist_info_c_& other);
      ~uac_access_category1_sel_assist_info_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uac_access_category1_sel_assist_info_e& plmn_common()
      {
        assert_choice_type(types::plmn_common, type_, "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<uac_access_category1_sel_assist_info_e>();
      }
      individual_plmn_list_l_& individual_plmn_list()
      {
        assert_choice_type(types::individual_plmn_list, type_, "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<individual_plmn_list_l_>();
      }
      const uac_access_category1_sel_assist_info_e& plmn_common() const
      {
        assert_choice_type(types::plmn_common, type_, "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<uac_access_category1_sel_assist_info_e>();
      }
      const individual_plmn_list_l_& individual_plmn_list() const
      {
        assert_choice_type(types::individual_plmn_list, type_, "uac-AccessCategory1-SelectionAssistanceInfo");
        return c.get<individual_plmn_list_l_>();
      }
      uac_access_category1_sel_assist_info_e& set_plmn_common();
      individual_plmn_list_l_&                set_individual_plmn_list();

    private:
      types                                    type_;
      choice_buffer_t<individual_plmn_list_l_> c;

      void destroy_();
    };

    // member variables
    bool                                    uac_access_category1_sel_assist_info_present = false;
    uac_barr_per_cat_list_l                 uac_barr_for_common;
    uac_barr_per_plmn_list_l                uac_barr_per_plmn_list;
    uac_barr_info_set_list_l                uac_barr_info_set_list;
    uac_access_category1_sel_assist_info_c_ uac_access_category1_sel_assist_info;
  };

  // member variables
  bool                          cell_sel_info_present           = false;
  bool                          conn_est_fail_ctrl_present      = false;
  bool                          si_sched_info_present           = false;
  bool                          serving_cell_cfg_common_present = false;
  bool                          ims_emergency_support_present   = false;
  bool                          ecall_over_ims_support_present  = false;
  bool                          ue_timers_and_consts_present    = false;
  bool                          uac_barr_info_present           = false;
  bool                          use_full_resume_id_present      = false;
  bool                          non_crit_ext_present            = false;
  cell_sel_info_s_              cell_sel_info;
  cell_access_related_info_s    cell_access_related_info;
  conn_est_fail_ctrl_s          conn_est_fail_ctrl;
  si_sched_info_s               si_sched_info;
  serving_cell_cfg_common_sib_s serving_cell_cfg_common;
  ue_timers_and_consts_s        ue_timers_and_consts;
  uac_barr_info_s_              uac_barr_info;
  dyn_octstring                 late_non_crit_ext;
  sib1_v1610_ies_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation ::= SEQUENCE
struct sys_info_s {
  struct crit_exts_c_ {
    struct crit_exts_future_r16_c_ {
      struct types_opts {
        enum options { pos_sys_info_r16, crit_exts_future, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      crit_exts_future_r16_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      pos_sys_info_r16_ies_s& pos_sys_info_r16()
      {
        assert_choice_type(types::pos_sys_info_r16, type_, "criticalExtensionsFuture-r16");
        return c;
      }
      const pos_sys_info_r16_ies_s& pos_sys_info_r16() const
      {
        assert_choice_type(types::pos_sys_info_r16, type_, "criticalExtensionsFuture-r16");
        return c;
      }
      pos_sys_info_r16_ies_s& set_pos_sys_info_r16();
      void                    set_crit_exts_future();

    private:
      types                  type_;
      pos_sys_info_r16_ies_s c;
    };
    struct types_opts {
      enum options { sys_info, crit_exts_future_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sys_info_ies_s& sys_info()
    {
      assert_choice_type(types::sys_info, type_, "criticalExtensions");
      return c.get<sys_info_ies_s>();
    }
    crit_exts_future_r16_c_& crit_exts_future_r16()
    {
      assert_choice_type(types::crit_exts_future_r16, type_, "criticalExtensions");
      return c.get<crit_exts_future_r16_c_>();
    }
    const sys_info_ies_s& sys_info() const
    {
      assert_choice_type(types::sys_info, type_, "criticalExtensions");
      return c.get<sys_info_ies_s>();
    }
    const crit_exts_future_r16_c_& crit_exts_future_r16() const
    {
      assert_choice_type(types::crit_exts_future_r16, type_, "criticalExtensions");
      return c.get<crit_exts_future_r16_c_>();
    }
    sys_info_ies_s&          set_sys_info();
    crit_exts_future_r16_c_& set_crit_exts_future_r16();

  private:
    types                                                    type_;
    choice_buffer_t<crit_exts_future_r16_c_, sys_info_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-DL-SCH-MessageType ::= CHOICE
struct bcch_dl_sch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { sys_info, sib_type1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sys_info_s& sys_info()
    {
      assert_choice_type(types::sys_info, type_, "c1");
      return c.get<sys_info_s>();
    }
    sib1_s& sib_type1()
    {
      assert_choice_type(types::sib_type1, type_, "c1");
      return c.get<sib1_s>();
    }
    const sys_info_s& sys_info() const
    {
      assert_choice_type(types::sys_info, type_, "c1");
      return c.get<sys_info_s>();
    }
    const sib1_s& sib_type1() const
    {
      assert_choice_type(types::sib_type1, type_, "c1");
      return c.get<sib1_s>();
    }
    sys_info_s& set_sys_info();
    sib1_s&     set_sib_type1();

  private:
    types                               type_;
    choice_buffer_t<sib1_s, sys_info_s> c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bcch_dl_sch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// BCCH-DL-SCH-Message ::= SEQUENCE
struct bcch_dl_sch_msg_s {
  bcch_dl_sch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRB-PDCP-ConfigBroadcast-r17 ::= SEQUENCE
struct mrb_pdcp_cfg_broadcast_r17_s {
  struct hdr_compress_r17_c_ {
    struct rohc_s_ {
      struct profiles_r17_s_ {
        bool profile0x0000_r17 = false;
        bool profile0x0001_r17 = false;
        bool profile0x0002_r17 = false;
      };

      // member variables
      bool            max_c_id_r17_present = false;
      uint8_t         max_c_id_r17         = 1;
      profiles_r17_s_ profiles_r17;
    };
    struct types_opts {
      enum options { not_used, rohc, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    hdr_compress_r17_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rohc_s_& rohc()
    {
      assert_choice_type(types::rohc, type_, "headerCompression-r17");
      return c;
    }
    const rohc_s_& rohc() const
    {
      assert_choice_type(types::rohc, type_, "headerCompression-r17");
      return c;
    }
    void     set_not_used();
    rohc_s_& set_rohc();

  private:
    types   type_;
    rohc_s_ c;
  };
  struct t_reordering_r17_opts {
    enum options { ms1, ms10, ms40, ms160, ms500, ms1000, ms1250, ms2750, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t_reordering_r17_e_ = enumerated<t_reordering_r17_opts>;

  // member variables
  bool                pdcp_sn_size_dl_r17_present = false;
  bool                t_reordering_r17_present    = false;
  hdr_compress_r17_c_ hdr_compress_r17;
  t_reordering_r17_e_ t_reordering_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRB-RLC-ConfigBroadcast-r17 ::= SEQUENCE
struct mrb_rlc_cfg_broadcast_r17_s {
  bool           sn_field_len_r17_present = false;
  bool           t_reassembly_r17_present = false;
  uint8_t        lc_ch_id_r17             = 1;
  t_reassembly_e t_reassembly_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRB-InfoBroadcast-r17 ::= SEQUENCE
struct mrb_info_broadcast_r17_s {
  bool                         ext = false;
  mrb_pdcp_cfg_broadcast_r17_s pdcp_cfg_r17;
  mrb_rlc_cfg_broadcast_r17_s  rlc_cfg_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRB-ListBroadcast-r17 ::= SEQUENCE (SIZE (1..4)) OF MRB-InfoBroadcast-r17
using mrb_list_broadcast_r17_l = dyn_array<mrb_info_broadcast_r17_s>;

// MBS-NeighbourCell-r17 ::= SEQUENCE
struct mbs_neighbour_cell_r17_s {
  bool     carrier_freq_r17_present = false;
  uint16_t pci_r17                  = 0;
  uint32_t carrier_freq_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-SessionInfo-r17 ::= SEQUENCE
struct mbs_session_info_r17_s {
  bool                     mtch_sched_info_r17_present      = false;
  bool                     mtch_neighbour_cell_r17_present  = false;
  bool                     pdsch_cfg_idx_r17_present        = false;
  bool                     mtch_ssb_map_win_idx_r17_present = false;
  tmgi_r17_s               mbs_session_id_r17;
  uint32_t                 g_rnti_r17 = 0;
  mrb_list_broadcast_r17_l mrb_list_broadcast_r17;
  uint8_t                  mtch_sched_info_r17 = 0;
  fixed_bitstring<8>       mtch_neighbour_cell_r17;
  uint8_t                  pdsch_cfg_idx_r17        = 0;
  uint8_t                  mtch_ssb_map_win_idx_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MTCH-SSB-MappingWindowCycleOffset-r17 ::= CHOICE
struct mtch_ssb_map_win_cycle_offset_r17_c {
  struct types_opts {
    enum options { ms10, ms20, ms32, ms64, ms128, ms256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mtch_ssb_map_win_cycle_offset_r17_c() = default;
  mtch_ssb_map_win_cycle_offset_r17_c(const mtch_ssb_map_win_cycle_offset_r17_c& other);
  mtch_ssb_map_win_cycle_offset_r17_c& operator=(const mtch_ssb_map_win_cycle_offset_r17_c& other);
  ~mtch_ssb_map_win_cycle_offset_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& ms10()
  {
    assert_choice_type(types::ms10, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& ms20()
  {
    assert_choice_type(types::ms20, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& ms32()
  {
    assert_choice_type(types::ms32, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& ms64()
  {
    assert_choice_type(types::ms64, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  uint8_t& ms128()
  {
    assert_choice_type(types::ms128, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  uint16_t& ms256()
  {
    assert_choice_type(types::ms256, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint16_t>();
  }
  const uint8_t& ms10() const
  {
    assert_choice_type(types::ms10, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& ms20() const
  {
    assert_choice_type(types::ms20, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& ms32() const
  {
    assert_choice_type(types::ms32, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& ms64() const
  {
    assert_choice_type(types::ms64, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  const uint8_t& ms128() const
  {
    assert_choice_type(types::ms128, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint8_t>();
  }
  const uint16_t& ms256() const
  {
    assert_choice_type(types::ms256, type_, "MTCH-SSB-MappingWindowCycleOffset-r17");
    return c.get<uint16_t>();
  }
  uint8_t&  set_ms10();
  uint8_t&  set_ms20();
  uint8_t&  set_ms32();
  uint8_t&  set_ms64();
  uint8_t&  set_ms128();
  uint16_t& set_ms256();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MBS-NeighbourCellList-r17 ::= SEQUENCE (SIZE (0..8)) OF MBS-NeighbourCell-r17
using mbs_neighbour_cell_list_r17_l = dyn_array<mbs_neighbour_cell_r17_s>;

// MBS-SessionInfoList-r17 ::= SEQUENCE (SIZE (1..1024)) OF MBS-SessionInfo-r17
using mbs_session_info_list_r17_l = dyn_array<mbs_session_info_r17_s>;

// MTCH-SSB-MappingWindowList-r17 ::= SEQUENCE (SIZE (1..16)) OF MTCH-SSB-MappingWindowCycleOffset-r17
using mtch_ssb_map_win_list_r17_l = dyn_array<mtch_ssb_map_win_cycle_offset_r17_c>;

// MBSBroadcastConfiguration-r17-IEs ::= SEQUENCE
struct mbs_broadcast_cfg_r17_ies_s {
  using drx_cfg_ptm_list_r17_l_ = dyn_array<drx_cfg_ptm_r17_s>;

  // member variables
  bool                          mbs_neighbour_cell_list_r17_present = false;
  bool                          pdsch_cfg_mtch_r17_present          = false;
  bool                          non_crit_ext_present                = false;
  mbs_session_info_list_r17_l   mbs_session_info_list_r17;
  mbs_neighbour_cell_list_r17_l mbs_neighbour_cell_list_r17;
  drx_cfg_ptm_list_r17_l_       drx_cfg_ptm_list_r17;
  pdsch_cfg_broadcast_r17_s     pdsch_cfg_mtch_r17;
  mtch_ssb_map_win_list_r17_l   mtch_ssb_map_win_list_r17;
  dyn_octstring                 late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSBroadcastConfiguration-r17 ::= SEQUENCE
struct mbs_broadcast_cfg_r17_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mbs_broadcast_cfg_r17, crit_exts_future, nulltype } value;

      const char* to_string() const;
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
    mbs_broadcast_cfg_r17_ies_s& mbs_broadcast_cfg_r17()
    {
      assert_choice_type(types::mbs_broadcast_cfg_r17, type_, "criticalExtensions");
      return c;
    }
    const mbs_broadcast_cfg_r17_ies_s& mbs_broadcast_cfg_r17() const
    {
      assert_choice_type(types::mbs_broadcast_cfg_r17, type_, "criticalExtensions");
      return c;
    }
    mbs_broadcast_cfg_r17_ies_s& set_mbs_broadcast_cfg_r17();
    void                         set_crit_exts_future();

  private:
    types                       type_;
    mbs_broadcast_cfg_r17_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCCH-MessageType-r17 ::= CHOICE
struct mcch_msg_type_r17_c {
  struct c1_c_ {
    struct types_opts {
      enum options { mbs_broadcast_cfg_r17, spare1, nulltype } value;

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
    mbs_broadcast_cfg_r17_s& mbs_broadcast_cfg_r17()
    {
      assert_choice_type(types::mbs_broadcast_cfg_r17, type_, "c1");
      return c;
    }
    const mbs_broadcast_cfg_r17_s& mbs_broadcast_cfg_r17() const
    {
      assert_choice_type(types::mbs_broadcast_cfg_r17, type_, "c1");
      return c;
    }
    mbs_broadcast_cfg_r17_s& set_mbs_broadcast_cfg_r17();
    void                     set_spare1();

  private:
    types                   type_;
    mbs_broadcast_cfg_r17_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mcch_msg_type_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "MCCH-MessageType-r17");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "MCCH-MessageType-r17");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// MCCH-Message-r17 ::= SEQUENCE
struct mcch_msg_r17_s {
  mcch_msg_type_r17_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecord-v1700 ::= SEQUENCE
struct paging_record_v1700_s {
  bool paging_cause_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingUE-Identity ::= CHOICE
struct paging_ue_id_c {
  struct types_opts {
    enum options { ng_5_g_s_tmsi, full_i_rnti, /*...*/ nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  paging_ue_id_c() = default;
  paging_ue_id_c(const paging_ue_id_c& other);
  paging_ue_id_c& operator=(const paging_ue_id_c& other);
  ~paging_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<48>& ng_5_g_s_tmsi()
  {
    assert_choice_type(types::ng_5_g_s_tmsi, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48>>();
  }
  fixed_bitstring<40>& full_i_rnti()
  {
    assert_choice_type(types::full_i_rnti, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40>>();
  }
  const fixed_bitstring<48>& ng_5_g_s_tmsi() const
  {
    assert_choice_type(types::ng_5_g_s_tmsi, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48>>();
  }
  const fixed_bitstring<40>& full_i_rnti() const
  {
    assert_choice_type(types::full_i_rnti, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40>>();
  }
  fixed_bitstring<48>& set_ng_5_g_s_tmsi();
  fixed_bitstring<40>& set_full_i_rnti();

private:
  types                                type_;
  choice_buffer_t<fixed_bitstring<48>> c;

  void destroy_();
};

// PagingGroupList-r17 ::= SEQUENCE (SIZE (1..32)) OF TMGI-r17
using paging_group_list_r17_l = dyn_array<tmgi_r17_s>;

// PagingRecord ::= SEQUENCE
struct paging_record_s {
  bool           ext                 = false;
  bool           access_type_present = false;
  paging_ue_id_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList-v1700 ::= SEQUENCE (SIZE (1..32)) OF PagingRecord-v1700
using paging_record_list_v1700_l = dyn_array<paging_record_v1700_s>;

// Paging-v1700-IEs ::= SEQUENCE
struct paging_v1700_ies_s {
  bool                       non_crit_ext_present = false;
  paging_record_list_v1700_l paging_record_list_v1700;
  paging_group_list_r17_l    paging_group_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList ::= SEQUENCE (SIZE (1..32)) OF PagingRecord
using paging_record_list_l = dyn_array<paging_record_s>;

// Paging ::= SEQUENCE
struct paging_s {
  bool                 non_crit_ext_present = false;
  paging_record_list_l paging_record_list;
  dyn_octstring        late_non_crit_ext;
  paging_v1700_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-MessageType ::= CHOICE
struct pcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { paging, spare1, nulltype } value;

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
    paging_s& paging()
    {
      assert_choice_type(types::paging, type_, "c1");
      return c;
    }
    const paging_s& paging() const
    {
      assert_choice_type(types::paging, type_, "c1");
      return c;
    }
    paging_s& set_paging();
    void      set_spare1();

  private:
    types    type_;
    paging_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  pcch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// PCCH-Message ::= SEQUENCE
struct pcch_msg_s {
  pcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest1-IEs ::= SEQUENCE
struct rrc_resume_request1_ies_s {
  fixed_bitstring<40> resume_id;
  fixed_bitstring<16> resume_mac_i;
  resume_cause_e      resume_cause;
  fixed_bitstring<1>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest1 ::= SEQUENCE
struct rrc_resume_request1_s {
  rrc_resume_request1_ies_s rrc_resume_request1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-CCCH1-MessageType ::= CHOICE
struct ul_ccch1_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_resume_request1, spare3, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
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
    rrc_resume_request1_s& rrc_resume_request1()
    {
      assert_choice_type(types::rrc_resume_request1, type_, "c1");
      return c;
    }
    const rrc_resume_request1_s& rrc_resume_request1() const
    {
      assert_choice_type(types::rrc_resume_request1, type_, "c1");
      return c;
    }
    rrc_resume_request1_s& set_rrc_resume_request1();
    void                   set_spare3();
    void                   set_spare2();
    void                   set_spare1();

  private:
    types                 type_;
    rrc_resume_request1_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ul_ccch1_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-CCCH1-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-CCCH1-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// UL-CCCH1-Message ::= SEQUENCE
struct ul_ccch1_msg_s {
  ul_ccch1_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReportAppLayerList-r17 ::= SEQUENCE (SIZE (1..16)) OF MeasReportAppLayer-r17
using meas_report_app_layer_list_r17_l = dyn_array<meas_report_app_layer_r17_s>;

// MeasurementReportAppLayer-r17-IEs ::= SEQUENCE
struct meas_report_app_layer_r17_ies_s {
  bool                             non_crit_ext_present = false;
  meas_report_app_layer_list_r17_l meas_report_app_layer_list_r17;
  dyn_octstring                    late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AI-RNTI-r16 ::= INTEGER (0..65535)
using ai_rnti_r16 = integer<uint32_t, 0, 65535>;

// BWP-DownlinkDedicatedSDT-r17 ::= SEQUENCE
struct bwp_dl_ded_sdt_r17_s {
  bool                         ext                   = false;
  bool                         pdcch_cfg_r17_present = false;
  bool                         pdsch_cfg_r17_present = false;
  setup_release_c<pdcch_cfg_s> pdcch_cfg_r17;
  setup_release_c<pdsch_cfg_s> pdsch_cfg_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-UplinkDedicatedSDT-r17 ::= SEQUENCE
struct bwp_ul_ded_sdt_r17_s {
  bool                                ext                   = false;
  bool                                pusch_cfg_r17_present = false;
  setup_release_c<pusch_cfg_s>        pusch_cfg_r17;
  cfg_grant_cfg_to_add_mod_list_r16_l cfg_grant_cfg_to_add_mod_list_r17;
  cfg_grant_cfg_to_release_list_r16_l cfg_grant_cfg_to_release_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCA-MixedExt-r16 ::= SEQUENCE
struct pdcch_blind_detection_ca_mixed_ext_r16_s {
  uint8_t pdcch_blind_detection_ca1_r16 = 1;
  uint8_t pdcch_blind_detection_ca2_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCG-UE-MixedExt-r16 ::= SEQUENCE
struct pdcch_blind_detection_cg_ue_mixed_ext_r16_s {
  uint8_t pdcch_blind_detection_cg_ue1_r16 = 0;
  uint8_t pdcch_blind_detection_cg_ue2_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionMixedList-r16 ::= SEQUENCE
struct pdcch_blind_detection_mixed_list_r16_s {
  struct pdcch_blind_detection_ca_mixed_ext_r16_c_ {
    struct types_opts {
      enum options {
        pdcch_blind_detection_ca_mixed_v16a0,
        pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    pdcch_blind_detection_ca_mixed_ext_r16_c_() = default;
    pdcch_blind_detection_ca_mixed_ext_r16_c_(const pdcch_blind_detection_ca_mixed_ext_r16_c_& other);
    pdcch_blind_detection_ca_mixed_ext_r16_c_& operator=(const pdcch_blind_detection_ca_mixed_ext_r16_c_& other);
    ~pdcch_blind_detection_ca_mixed_ext_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_ca_mixed_v16a0()
    {
      assert_choice_type(types::pdcch_blind_detection_ca_mixed_v16a0, type_, "pdcch-BlindDetectionCA-MixedExt-r16");
      return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
    }
    pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0()
    {
      assert_choice_type(
          types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0, type_, "pdcch-BlindDetectionCA-MixedExt-r16");
      return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
    }
    const pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_ca_mixed_v16a0() const
    {
      assert_choice_type(types::pdcch_blind_detection_ca_mixed_v16a0, type_, "pdcch-BlindDetectionCA-MixedExt-r16");
      return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
    }
    const pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0() const
    {
      assert_choice_type(
          types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0, type_, "pdcch-BlindDetectionCA-MixedExt-r16");
      return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
    }
    pdcch_blind_detection_ca_mixed_ext_r16_s& set_pdcch_blind_detection_ca_mixed_v16a0();
    pdcch_blind_detection_ca_mixed_ext_r16_s& set_pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0();

  private:
    types                                                     type_;
    choice_buffer_t<pdcch_blind_detection_ca_mixed_ext_r16_s> c;

    void destroy_();
  };
  struct pdcch_blind_detection_cg_ue_mixed_ext_r16_s_ {
    pdcch_blind_detection_cg_ue_mixed_ext_r16_s pdcch_blind_detection_mcg_ue_mixed_v16a0;
    pdcch_blind_detection_cg_ue_mixed_ext_r16_s pdcch_blind_detection_scg_ue_mixed_v16a0;
  };

  // member variables
  bool                                         pdcch_blind_detection_ca_mixed_ext_r16_present    = false;
  bool                                         pdcch_blind_detection_cg_ue_mixed_ext_r16_present = false;
  pdcch_blind_detection_ca_mixed_ext_r16_c_    pdcch_blind_detection_ca_mixed_ext_r16;
  pdcch_blind_detection_cg_ue_mixed_ext_r16_s_ pdcch_blind_detection_cg_ue_mixed_ext_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v16a0 ::= SEQUENCE
struct ca_params_nr_v16a0_s {
  using pdcch_blind_detection_mixed_list_r16_l_ = dyn_array<pdcch_blind_detection_mixed_list_r16_s>;

  // member variables
  pdcch_blind_detection_mixed_list_r16_l_ pdcch_blind_detection_mixed_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v16a0 ::= SEQUENCE
struct ca_params_nrdc_v16a0_s {
  bool                 ca_params_nr_for_dc_v16a0_present = false;
  ca_params_nr_v16a0_s ca_params_nr_for_dc_v16a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v16a0 ::= SEQUENCE
struct band_combination_v16a0_s {
  bool                   ca_params_nr_v16a0_present   = false;
  bool                   ca_params_nrdc_v16a0_present = false;
  ca_params_nr_v16a0_s   ca_params_nr_v16a0;
  ca_params_nrdc_v16a0_s ca_params_nrdc_v16a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v16a0 ::= SEQUENCE
struct band_combination_ul_tx_switch_v16a0_s {
  bool                     band_combination_v16a0_present = false;
  band_combination_v16a0_s band_combination_v16a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationList-UplinkTxSwitch-v16a0 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v16a0
using band_combination_list_ul_tx_switch_v16a0_l = dyn_array<band_combination_ul_tx_switch_v16a0_s>;

// BandCombinationList-v1570 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1570
using band_combination_list_v1570_l = dyn_array<band_combination_v1570_s>;

// BandCombinationList-v1580 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1580
using band_combination_list_v1580_l = dyn_array<band_combination_v1580_s>;

// BandCombinationList-v1590 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1590
using band_combination_list_v1590_l = dyn_array<band_combination_v1590_s>;

// BandCombinationList-v15g0 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v15g0
using band_combination_list_v15g0_l = dyn_array<band_combination_v15g0_s>;

// BandCombinationList-v16a0 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v16a0
using band_combination_list_v16a0_l = dyn_array<band_combination_v16a0_s>;

// CG-SDT-ConfigLCH-Restriction-r17 ::= SEQUENCE
struct cg_sdt_cfg_lch_restrict_r17_s {
  using allowed_cg_list_r17_l_ = bounded_array<uint8_t, 31>;

  // member variables
  bool                   cfg_grant_type1_allowed_r17_present = false;
  bool                   allowed_cg_list_r17_present         = false;
  uint8_t                lc_ch_id_r17                        = 1;
  allowed_cg_list_r17_l_ allowed_cg_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGrouping-r16 ::= SEQUENCE
struct cell_grouping_r16_s {
  using mcg_r16_l_ = dyn_array<uint16_t>;
  using scg_r16_l_ = dyn_array<uint16_t>;
  struct mode_r16_opts {
    enum options { sync, async, nulltype } value;

    const char* to_string() const;
  };
  using mode_r16_e_ = enumerated<mode_r16_opts>;

  // member variables
  mcg_r16_l_  mcg_r16;
  scg_r16_l_  scg_r16;
  mode_r16_e_ mode_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CondReconfigExecCondSCG-r17 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (1..64)
using cond_recfg_exec_cond_scg_r17_l = bounded_array<uint8_t, 2>;

// MeasAndMobParametersMRDC-XDD-Diff-v1560 ::= SEQUENCE
struct meas_and_mob_params_mrdc_xdd_diff_v1560_s {
  bool sftd_meas_pscell_nedc_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-v1560 ::= SEQUENCE
struct meas_and_mob_params_mrdc_v1560_s {
  bool                                      meas_and_mob_params_mrdc_xdd_diff_v1560_present = false;
  meas_and_mob_params_mrdc_xdd_diff_v1560_s meas_and_mob_params_mrdc_xdd_diff_v1560;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-Common-v1730 ::= SEQUENCE
struct meas_and_mob_params_mrdc_common_v1730_s {
  struct independent_gap_cfg_max_cc_r17_s_ {
    bool    fr1_only_r17_present    = false;
    bool    fr2_only_r17_present    = false;
    bool    fr1_and_fr2_r17_present = false;
    uint8_t fr1_only_r17            = 1;
    uint8_t fr2_only_r17            = 1;
    uint8_t fr1_and_fr2_r17         = 1;
  };

  // member variables
  independent_gap_cfg_max_cc_r17_s_ independent_gap_cfg_max_cc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-v1730 ::= SEQUENCE
struct meas_and_mob_params_mrdc_v1730_s {
  bool                                    meas_and_mob_params_mrdc_common_v1730_present = false;
  meas_and_mob_params_mrdc_common_v1730_s meas_and_mob_params_mrdc_common_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellListSFTD-EUTRA ::= SEQUENCE (SIZE (1..3)) OF MeasResultSFTD-EUTRA
using meas_result_cell_list_sftd_eutra_l = dyn_array<meas_result_sftd_eutra_s>;

// MeasResultSCG-Failure ::= SEQUENCE
struct meas_result_scg_fail_s {
  bool                   ext = false;
  meas_result_list2_nr_l meas_result_per_mo_list;
  // ...
  // group 0
  copy_ptr<location_info_r16_s> location_info_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersCommon-v16a0 ::= SEQUENCE
struct phy_params_common_v16a0_s {
  bool srs_periodicity_and_offset_ext_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-Parameters-v16a0 ::= SEQUENCE
struct phy_params_v16a0_s {
  bool                      phy_params_common_v16a0_present = false;
  phy_params_common_v16a0_s phy_params_common_v16a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NAICS-Capability-Entry ::= SEQUENCE
struct naics_cap_entry_s {
  struct nof_aggr_prb_opts {
    enum options {
      n50,
      n75,
      n100,
      n125,
      n150,
      n175,
      n200,
      n225,
      n250,
      n275,
      n300,
      n350,
      n400,
      n450,
      n500,
      spare,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using nof_aggr_prb_e_ = enumerated<nof_aggr_prb_opts>;

  // member variables
  bool            ext                  = false;
  uint8_t         nof_naics_capable_cc = 1;
  nof_aggr_prb_e_ nof_aggr_prb;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersMRDC ::= SEQUENCE
struct phy_params_mrdc_s {
  using naics_cap_list_l_ = dyn_array<naics_cap_entry_s>;

  // member variables
  bool              ext = false;
  naics_cap_list_l_ naics_cap_list;
  // ...
  // group 0
  copy_ptr<carrier_aggregation_variant_s> sp_cell_placement;
  // group 1
  bool tdd_pcell_ul_tx_all_ul_sf_r16_present = false;
  bool fdd_pcell_ul_tx_all_ul_sf_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-v15g0 ::= SEQUENCE
struct rf_params_v15g0_s {
  band_combination_list_v15g0_l supported_band_combination_list_v15g0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-v16a0 ::= SEQUENCE
struct rf_params_v16a0_s {
  band_combination_list_v16a0_l              supported_band_combination_list_v16a0;
  band_combination_list_ul_tx_switch_v16a0_l supported_band_combination_list_ul_tx_switch_v16a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandNR-v16c0 ::= SEQUENCE
struct band_nr_v16c0_s {
  bool ext                               = false;
  bool pusch_repeat_type_a_v16c0_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-v16c0 ::= SEQUENCE
struct rf_params_v16c0_s {
  using supported_band_list_nr_v16c0_l_ = dyn_array<band_nr_v16c0_s>;

  // member variables
  supported_band_list_nr_v16c0_l_ supported_band_list_nr_v16c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-ParametersMRDC ::= SEQUENCE
struct rf_params_mrdc_s {
  struct supported_band_combination_list_nedc_only_v15a0_s_ {
    band_combination_list_v1540_l supported_band_combination_list_v1540;
    band_combination_list_v1560_l supported_band_combination_list_v1560;
    band_combination_list_v1570_l supported_band_combination_list_v1570;
    band_combination_list_v1580_l supported_band_combination_list_v1580;
    band_combination_list_v1590_l supported_band_combination_list_v1590;
  };
  struct supported_band_combination_list_nedc_only_v1720_s_ {
    band_combination_list_v1700_l supported_band_combination_list_v1700;
    band_combination_list_v1720_l supported_band_combination_list_v1720;
  };

  // member variables
  bool                    ext = false;
  band_combination_list_l supported_band_combination_list;
  freq_band_list_l        applied_freq_band_list_filt;
  // ...
  // group 0
  bool                                    srs_switching_time_requested_present = false;
  copy_ptr<band_combination_list_v1540_l> supported_band_combination_list_v1540;
  // group 1
  copy_ptr<band_combination_list_v1550_l> supported_band_combination_list_v1550;
  // group 2
  copy_ptr<band_combination_list_v1560_l> supported_band_combination_list_v1560;
  copy_ptr<band_combination_list_l>       supported_band_combination_list_nedc_only;
  // group 3
  copy_ptr<band_combination_list_v1570_l> supported_band_combination_list_v1570;
  // group 4
  copy_ptr<band_combination_list_v1580_l> supported_band_combination_list_v1580;
  // group 5
  copy_ptr<band_combination_list_v1590_l> supported_band_combination_list_v1590;
  // group 6
  copy_ptr<supported_band_combination_list_nedc_only_v15a0_s_> supported_band_combination_list_nedc_only_v15a0;
  // group 7
  copy_ptr<band_combination_list_v1610_l>            supported_band_combination_list_v1610;
  copy_ptr<band_combination_list_v1610_l>            supported_band_combination_list_nedc_only_v1610;
  copy_ptr<band_combination_list_ul_tx_switch_r16_l> supported_band_combination_list_ul_tx_switch_r16;
  // group 8
  copy_ptr<band_combination_list_v1630_l>              supported_band_combination_list_v1630;
  copy_ptr<band_combination_list_v1630_l>              supported_band_combination_list_nedc_only_v1630;
  copy_ptr<band_combination_list_ul_tx_switch_v1630_l> supported_band_combination_list_ul_tx_switch_v1630;
  // group 9
  copy_ptr<band_combination_list_v1640_l>              supported_band_combination_list_v1640;
  copy_ptr<band_combination_list_v1640_l>              supported_band_combination_list_nedc_only_v1640;
  copy_ptr<band_combination_list_ul_tx_switch_v1640_l> supported_band_combination_list_ul_tx_switch_v1640;
  // group 10
  copy_ptr<band_combination_list_ul_tx_switch_v1670_l> supported_band_combination_list_ul_tx_switch_v1670;
  // group 11
  copy_ptr<band_combination_list_v1700_l>              supported_band_combination_list_v1700;
  copy_ptr<band_combination_list_ul_tx_switch_v1700_l> supported_band_combination_list_ul_tx_switch_v1700;
  // group 12
  copy_ptr<band_combination_list_v1720_l>                      supported_band_combination_list_v1720;
  copy_ptr<supported_band_combination_list_nedc_only_v1720_s_> supported_band_combination_list_nedc_only_v1720;
  copy_ptr<band_combination_list_ul_tx_switch_v1720_l>         supported_band_combination_list_ul_tx_switch_v1720;
  // group 13
  copy_ptr<band_combination_list_v1730_l>              supported_band_combination_list_v1730;
  copy_ptr<band_combination_list_v1730_l>              supported_band_combination_list_nedc_only_v1730;
  copy_ptr<band_combination_list_ul_tx_switch_v1730_l> supported_band_combination_list_ul_tx_switch_v1730;
  // group 14
  copy_ptr<band_combination_list_v1740_l>              supported_band_combination_list_v1740;
  copy_ptr<band_combination_list_v1740_l>              supported_band_combination_list_nedc_only_v1740;
  copy_ptr<band_combination_list_ul_tx_switch_v1740_l> supported_band_combination_list_ul_tx_switch_v1740;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-ParametersMRDC-v15g0 ::= SEQUENCE
struct rf_params_mrdc_v15g0_s {
  band_combination_list_v15g0_l supported_band_combination_list_v15g0;
  band_combination_list_v15g0_l supported_band_combination_list_nedc_only_v15g0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-SDT-TA-ValidationConfig-r17 ::= SEQUENCE
struct cg_sdt_ta_validation_cfg_r17_s {
  struct cg_sdt_rsrp_change_thres_r17_opts {
    enum options {
      db2,
      db4,
      db6,
      db8,
      db10,
      db14,
      db18,
      db22,
      db26,
      db30,
      db34,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using cg_sdt_rsrp_change_thres_r17_e_ = enumerated<cg_sdt_rsrp_change_thres_r17_opts>;

  // member variables
  cg_sdt_rsrp_change_thres_r17_e_ cg_sdt_rsrp_change_thres_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDT-MAC-PHY-CG-Config-r17 ::= SEQUENCE
struct sdt_mac_phy_cg_cfg_r17_s {
  using cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_l_ = dyn_array<cg_sdt_cfg_lch_restrict_r17_s>;
  using cg_sdt_cfg_lch_restrict_to_release_list_r17_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool                                            ext                                  = false;
  bool                                            cg_sdt_cfg_init_bwp_n_ul_r17_present = false;
  bool                                            cg_sdt_cfg_init_bwp_sul_r17_present  = false;
  bool                                            cg_sdt_cfg_init_bwp_dl_r17_present   = false;
  bool                                            cg_sdt_time_align_timer_r17_present  = false;
  bool                                            cg_sdt_rsrp_thres_ssb_r17_present    = false;
  bool                                            cg_sdt_ta_validation_cfg_r17_present = false;
  bool                                            cg_sdt_cs_rnti_r17_present           = false;
  cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_l_  cg_sdt_cfg_lch_restrict_to_add_mod_list_r17;
  cg_sdt_cfg_lch_restrict_to_release_list_r17_l_  cg_sdt_cfg_lch_restrict_to_release_list_r17;
  setup_release_c<bwp_ul_ded_sdt_r17_s>           cg_sdt_cfg_init_bwp_n_ul_r17;
  setup_release_c<bwp_ul_ded_sdt_r17_s>           cg_sdt_cfg_init_bwp_sul_r17;
  bwp_dl_ded_sdt_r17_s                            cg_sdt_cfg_init_bwp_dl_r17;
  time_align_timer_e                              cg_sdt_time_align_timer_r17;
  uint8_t                                         cg_sdt_rsrp_thres_ssb_r17 = 0;
  setup_release_c<cg_sdt_ta_validation_cfg_r17_s> cg_sdt_ta_validation_cfg_r17;
  uint32_t                                        cg_sdt_cs_rnti_r17 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-ConfigCommonNR-r16 ::= SEQUENCE
struct sl_cfg_common_nr_r16_s {
  using sl_freq_info_list_r16_l_        = dyn_array<sl_freq_cfg_common_r16_s>;
  using sl_radio_bearer_cfg_list_r16_l_ = dyn_array<sl_radio_bearer_cfg_r16_s>;
  using sl_rlc_bearer_cfg_list_r16_l_   = dyn_array<sl_rlc_bearer_cfg_r16_s>;
  struct t400_r16_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t400_r16_e_ = enumerated<t400_r16_opts>;
  struct sl_max_num_consecutive_dtx_r16_opts {
    enum options { n1, n2, n3, n4, n6, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sl_max_num_consecutive_dtx_r16_e_ = enumerated<sl_max_num_consecutive_dtx_r16_opts>;

  // member variables
  bool                                    sl_ue_sel_cfg_r16_present              = false;
  bool                                    sl_meas_cfg_common_r16_present         = false;
  bool                                    sl_csi_acquisition_r16_present         = false;
  bool                                    sl_offset_dfn_r16_present              = false;
  bool                                    t400_r16_present                       = false;
  bool                                    sl_max_num_consecutive_dtx_r16_present = false;
  bool                                    sl_ssb_prio_nr_r16_present             = false;
  sl_freq_info_list_r16_l_                sl_freq_info_list_r16;
  sl_ue_sel_cfg_r16_s                     sl_ue_sel_cfg_r16;
  sl_nr_anchor_carrier_freq_list_r16_l    sl_nr_anchor_carrier_freq_list_r16;
  sl_eutra_anchor_carrier_freq_list_r16_l sl_eutra_anchor_carrier_freq_list_r16;
  sl_radio_bearer_cfg_list_r16_l_         sl_radio_bearer_cfg_list_r16;
  sl_rlc_bearer_cfg_list_r16_l_           sl_rlc_bearer_cfg_list_r16;
  sl_meas_cfg_common_r16_s                sl_meas_cfg_common_r16;
  uint16_t                                sl_offset_dfn_r16 = 1;
  t400_r16_e_                             t400_r16;
  sl_max_num_consecutive_dtx_r16_e_       sl_max_num_consecutive_dtx_r16;
  uint8_t                                 sl_ssb_prio_nr_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscConfigCommon-r17 ::= SEQUENCE
struct sl_disc_cfg_common_r17_s {
  sl_relay_ue_cfg_r17_s  sl_relay_ue_cfg_common_r17;
  sl_remote_ue_cfg_r17_s sl_remote_ue_cfg_common_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-TimersAndConstantsRemoteUE-r17 ::= SEQUENCE
struct ue_timers_and_consts_remote_ue_r17_s {
  struct t300_remote_ue_r17_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t300_remote_ue_r17_e_ = enumerated<t300_remote_ue_r17_opts>;
  struct t301_remote_ue_r17_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t301_remote_ue_r17_e_ = enumerated<t301_remote_ue_r17_opts>;
  struct t319_remote_ue_r17_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t319_remote_ue_r17_e_ = enumerated<t319_remote_ue_r17_opts>;

  // member variables
  bool                  ext                        = false;
  bool                  t300_remote_ue_r17_present = false;
  bool                  t301_remote_ue_r17_present = false;
  bool                  t319_remote_ue_r17_present = false;
  t300_remote_ue_r17_e_ t300_remote_ue_r17;
  t301_remote_ue_r17_e_ t301_remote_ue_r17;
  t319_remote_ue_r17_e_ t319_remote_ue_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB12-IEs-r16 ::= SEQUENCE
struct sib12_ies_r16_s {
  bool                   ext = false;
  sl_cfg_common_nr_r16_s sl_cfg_common_nr_r16;
  dyn_octstring          late_non_crit_ext;
  // ...
  // group 0
  bool                                           sl_l2_u2_n_relay_r17_present           = false;
  bool                                           sl_non_relay_discovery_r17_present     = false;
  bool                                           sl_l3_u2_n_relay_discovery_r17_present = false;
  copy_ptr<sl_drx_cfg_gc_bc_r17_s>               sl_drx_cfg_common_gc_bc_r17;
  copy_ptr<sl_disc_cfg_common_r17_s>             sl_disc_cfg_common_r17;
  copy_ptr<ue_timers_and_consts_remote_ue_r17_s> sl_timers_and_consts_remote_ue_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRS-ResourceSet-r17 ::= SEQUENCE
struct trs_res_set_r17_s {
  struct pwr_ctrl_offset_ss_r17_opts {
    enum options { db_neg3, db0, db3, db6, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using pwr_ctrl_offset_ss_r17_e_ = enumerated<pwr_ctrl_offset_ss_r17_opts>;
  struct scrambling_id_info_r17_c_ {
    using scrambling_id_per_res_list_with2_r17_l_ = std::array<uint16_t, 2>;
    using scrambling_id_per_res_list_with4_r17_l_ = std::array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scrambling_id_for_common_r17,
        scrambling_id_per_res_list_with2_r17,
        scrambling_id_per_res_list_with4_r17,
        // ...
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scrambling_id_info_r17_c_() = default;
    scrambling_id_info_r17_c_(const scrambling_id_info_r17_c_& other);
    scrambling_id_info_r17_c_& operator=(const scrambling_id_info_r17_c_& other);
    ~scrambling_id_info_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& scrambling_id_for_common_r17()
    {
      assert_choice_type(types::scrambling_id_for_common_r17, type_, "scramblingID-Info-r17");
      return c.get<uint16_t>();
    }
    scrambling_id_per_res_list_with2_r17_l_& scrambling_id_per_res_list_with2_r17()
    {
      assert_choice_type(types::scrambling_id_per_res_list_with2_r17, type_, "scramblingID-Info-r17");
      return c.get<scrambling_id_per_res_list_with2_r17_l_>();
    }
    scrambling_id_per_res_list_with4_r17_l_& scrambling_id_per_res_list_with4_r17()
    {
      assert_choice_type(types::scrambling_id_per_res_list_with4_r17, type_, "scramblingID-Info-r17");
      return c.get<scrambling_id_per_res_list_with4_r17_l_>();
    }
    const uint16_t& scrambling_id_for_common_r17() const
    {
      assert_choice_type(types::scrambling_id_for_common_r17, type_, "scramblingID-Info-r17");
      return c.get<uint16_t>();
    }
    const scrambling_id_per_res_list_with2_r17_l_& scrambling_id_per_res_list_with2_r17() const
    {
      assert_choice_type(types::scrambling_id_per_res_list_with2_r17, type_, "scramblingID-Info-r17");
      return c.get<scrambling_id_per_res_list_with2_r17_l_>();
    }
    const scrambling_id_per_res_list_with4_r17_l_& scrambling_id_per_res_list_with4_r17() const
    {
      assert_choice_type(types::scrambling_id_per_res_list_with4_r17, type_, "scramblingID-Info-r17");
      return c.get<scrambling_id_per_res_list_with4_r17_l_>();
    }
    uint16_t&                                set_scrambling_id_for_common_r17();
    scrambling_id_per_res_list_with2_r17_l_& set_scrambling_id_per_res_list_with2_r17();
    scrambling_id_per_res_list_with4_r17_l_& set_scrambling_id_per_res_list_with4_r17();

  private:
    types                                                                                             type_;
    choice_buffer_t<scrambling_id_per_res_list_with2_r17_l_, scrambling_id_per_res_list_with4_r17_l_> c;

    void destroy_();
  };
  struct periodicity_and_offset_r17_c_ {
    struct types_opts {
      enum options { slots10, slots20, slots40, slots80, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    periodicity_and_offset_r17_c_() = default;
    periodicity_and_offset_r17_c_(const periodicity_and_offset_r17_c_& other);
    periodicity_and_offset_r17_c_& operator=(const periodicity_and_offset_r17_c_& other);
    ~periodicity_and_offset_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& slots10()
    {
      assert_choice_type(types::slots10, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& slots20()
    {
      assert_choice_type(types::slots20, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& slots40()
    {
      assert_choice_type(types::slots40, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& slots80()
    {
      assert_choice_type(types::slots80, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& slots10() const
    {
      assert_choice_type(types::slots10, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& slots20() const
    {
      assert_choice_type(types::slots20, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& slots40() const
    {
      assert_choice_type(types::slots40, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& slots80() const
    {
      assert_choice_type(types::slots80, type_, "periodicityAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_slots10();
    uint8_t& set_slots20();
    uint8_t& set_slots40();
    uint8_t& set_slots80();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct nrof_res_r17_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_res_r17_e_ = enumerated<nrof_res_r17_opts>;

  // member variables
  pwr_ctrl_offset_ss_r17_e_     pwr_ctrl_offset_ss_r17;
  scrambling_id_info_r17_c_     scrambling_id_info_r17;
  uint8_t                       first_ofdm_symbol_in_time_domain_r17 = 0;
  uint16_t                      start_rb_r17                         = 0;
  uint16_t                      nrof_rbs_r17                         = 24;
  uint8_t                       ssb_idx_r17                          = 0;
  periodicity_and_offset_r17_c_ periodicity_and_offset_r17;
  fixed_bitstring<4>            freq_domain_alloc_r17;
  uint8_t                       ind_bit_id_r17 = 0;
  nrof_res_r17_e_               nrof_res_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB17-IEs-r17 ::= SEQUENCE
struct sib17_ies_r17_s {
  using trs_res_set_cfg_r17_l_ = dyn_array<trs_res_set_r17_s>;
  struct validity_dur_r17_opts {
    enum options {
      t1,
      t2,
      t4,
      t8,
      t16,
      t32,
      t64,
      t128,
      t256,
      t512,
      infinity,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  using validity_dur_r17_e_ = enumerated<validity_dur_r17_opts>;

  // member variables
  bool                   ext                      = false;
  bool                   validity_dur_r17_present = false;
  trs_res_set_cfg_r17_l_ trs_res_set_cfg_r17;
  validity_dur_r17_e_    validity_dur_r17;
  dyn_octstring          late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-ServingCellInfo-r17 ::= SEQUENCE
struct sl_serving_cell_info_r17_s {
  uint16_t sl_pci_r17             = 0;
  uint32_t sl_carrier_freq_nr_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-PosConfig-r17 ::= SEQUENCE
struct srs_pos_cfg_r17_s {
  using srs_pos_res_set_to_release_list_r17_l_ = bounded_array<uint8_t, 16>;
  using srs_pos_res_set_to_add_mod_list_r17_l_ = dyn_array<srs_pos_res_set_r16_s>;
  using srs_pos_res_to_release_list_r17_l_     = dyn_array<uint8_t>;
  using srs_pos_res_to_add_mod_list_r17_l_     = dyn_array<srs_pos_res_r16_s>;

  // member variables
  srs_pos_res_set_to_release_list_r17_l_ srs_pos_res_set_to_release_list_r17;
  srs_pos_res_set_to_add_mod_list_r17_l_ srs_pos_res_set_to_add_mod_list_r17;
  srs_pos_res_to_release_list_r17_l_     srs_pos_res_to_release_list_r17;
  srs_pos_res_to_add_mod_list_r17_l_     srs_pos_res_to_add_mod_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSRP-ChangeThreshold-r17 ::= ENUMERATED
struct rsrp_change_thres_r17_opts {
  enum options {
    db4,
    db6,
    db8,
    db10,
    db14,
    db18,
    db22,
    db26,
    db30,
    db34,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using rsrp_change_thres_r17_e = enumerated<rsrp_change_thres_r17_opts>;

// SRS-PosRRC-InactiveConfig-r17 ::= SEQUENCE
struct srs_pos_rrc_inactive_cfg_r17_s {
  bool                    srs_pos_cfg_n_ul_r17_present                   = false;
  bool                    srs_pos_cfg_sul_r17_present                    = false;
  bool                    bwp_n_ul_r17_present                           = false;
  bool                    bwp_sul_r17_present                            = false;
  bool                    inactive_pos_srs_time_align_timer_r17_present  = false;
  bool                    inactive_pos_srs_rsrp_change_thres_r17_present = false;
  srs_pos_cfg_r17_s       srs_pos_cfg_n_ul_r17;
  srs_pos_cfg_r17_s       srs_pos_cfg_sul_r17;
  bwp_s                   bwp_n_ul_r17;
  bwp_s                   bwp_sul_r17;
  time_align_timer_e      inactive_pos_srs_time_align_timer_r17;
  rsrp_change_thres_r17_e inactive_pos_srs_rsrp_change_thres_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequestFilterCommon ::= SEQUENCE
struct ue_cap_request_filt_common_s {
  struct mrdc_request_s_ {
    bool omit_en_dc_present    = false;
    bool include_nr_dc_present = false;
    bool include_ne_dc_present = false;
  };
  struct codebook_type_request_r16_s_ {
    bool type1_single_panel_r16_present = false;
    bool type1_multi_panel_r16_present  = false;
    bool type2_r16_present              = false;
    bool type2_port_sel_r16_present     = false;
  };
  using requested_cell_grouping_r16_l_ = dyn_array<cell_grouping_r16_s>;

  // member variables
  bool            ext                  = false;
  bool            mrdc_request_present = false;
  mrdc_request_s_ mrdc_request;
  // ...
  // group 0
  bool                                   ul_tx_switch_request_r16_present = false;
  copy_ptr<codebook_type_request_r16_s_> codebook_type_request_r16;
  // group 1
  copy_ptr<requested_cell_grouping_r16_l_> requested_cell_grouping_r16;
  // group 2
  bool fallback_group_five_request_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequestFilterNR-v1710 ::= SEQUENCE
struct ue_cap_request_filt_nr_v1710_s {
  bool sidelink_request_r17_present = false;
  bool non_crit_ext_present         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequestFilterNR-v1540 ::= SEQUENCE
struct ue_cap_request_filt_nr_v1540_s {
  bool                           srs_switching_time_request_present = false;
  bool                           non_crit_ext_present               = false;
  ue_cap_request_filt_nr_v1710_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequestFilterNR ::= SEQUENCE
struct ue_cap_request_filt_nr_s {
  bool                           non_crit_ext_present = false;
  freq_band_list_l               freq_band_list_filt;
  ue_cap_request_filt_nr_v1540_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-Capability-v1730 ::= SEQUENCE
struct ue_mrdc_cap_v1730_s {
  bool                             meas_and_mob_params_mrdc_v1730_present = false;
  bool                             non_crit_ext_present                   = false;
  meas_and_mob_params_mrdc_v1730_s meas_and_mob_params_mrdc_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GeneralParametersMRDC-v1610 ::= SEQUENCE
struct general_params_mrdc_v1610_s {
  bool f1c_over_eutra_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-ParametersMRDC-v1610 ::= SEQUENCE
struct pdcp_params_mrdc_v1610_s {
  bool scg_drb_nr_iab_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-Capability-v1700 ::= SEQUENCE
struct ue_mrdc_cap_v1700_s {
  bool                             non_crit_ext_present = false;
  meas_and_mob_params_mrdc_v1700_s meas_and_mob_params_mrdc_v1700;
  ue_mrdc_cap_v1730_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-Capability-v1610 ::= SEQUENCE
struct ue_mrdc_cap_v1610_s {
  bool                             meas_and_mob_params_mrdc_v1610_present = false;
  bool                             general_params_mrdc_v1610_present      = false;
  bool                             pdcp_params_mrdc_v1610_present         = false;
  bool                             non_crit_ext_present                   = false;
  meas_and_mob_params_mrdc_v1610_s meas_and_mob_params_mrdc_v1610;
  general_params_mrdc_v1610_s      general_params_mrdc_v1610;
  pdcp_params_mrdc_v1610_s         pdcp_params_mrdc_v1610;
  ue_mrdc_cap_v1700_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-CapabilityAddXDD-Mode-v1560 ::= SEQUENCE
struct ue_mrdc_cap_add_xdd_mode_v1560_s {
  bool                                      meas_and_mob_params_mrdc_xdd_diff_v1560_present = false;
  meas_and_mob_params_mrdc_xdd_diff_v1560_s meas_and_mob_params_mrdc_xdd_diff_v1560;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-ParametersMRDC ::= SEQUENCE
struct pdcp_params_mrdc_s {
  bool pdcp_dupl_split_srb_present = false;
  bool pdcp_dupl_split_drb_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-Capability-v1560 ::= SEQUENCE
struct ue_mrdc_cap_v1560_s {
  bool                             meas_and_mob_params_mrdc_v1560_present = false;
  bool                             fdd_add_ue_mrdc_cap_v1560_present      = false;
  bool                             tdd_add_ue_mrdc_cap_v1560_present      = false;
  bool                             non_crit_ext_present                   = false;
  dyn_octstring                    rx_filts;
  meas_and_mob_params_mrdc_v1560_s meas_and_mob_params_mrdc_v1560;
  ue_mrdc_cap_add_xdd_mode_v1560_s fdd_add_ue_mrdc_cap_v1560;
  ue_mrdc_cap_add_xdd_mode_v1560_s tdd_add_ue_mrdc_cap_v1560;
  ue_mrdc_cap_v1610_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-Capability ::= SEQUENCE
struct ue_mrdc_cap_s {
  using feature_set_combinations_l_ = dyn_array<feature_set_combination_l>;

  // member variables
  bool                           meas_and_mob_params_mrdc_present = false;
  bool                           phy_params_mrdc_v1530_present    = false;
  bool                           general_params_mrdc_present      = false;
  bool                           fdd_add_ue_mrdc_cap_present      = false;
  bool                           tdd_add_ue_mrdc_cap_present      = false;
  bool                           fr1_add_ue_mrdc_cap_present      = false;
  bool                           fr2_add_ue_mrdc_cap_present      = false;
  bool                           pdcp_params_mrdc_v1530_present   = false;
  bool                           non_crit_ext_present             = false;
  meas_and_mob_params_mrdc_s     meas_and_mob_params_mrdc;
  phy_params_mrdc_s              phy_params_mrdc_v1530;
  rf_params_mrdc_s               rf_params_mrdc;
  general_params_mrdc_xdd_diff_s general_params_mrdc;
  ue_mrdc_cap_add_xdd_mode_s     fdd_add_ue_mrdc_cap;
  ue_mrdc_cap_add_xdd_mode_s     tdd_add_ue_mrdc_cap;
  ue_mrdc_cap_add_frx_mode_s     fr1_add_ue_mrdc_cap;
  ue_mrdc_cap_add_frx_mode_s     fr2_add_ue_mrdc_cap;
  feature_set_combinations_l_    feature_set_combinations;
  pdcp_params_mrdc_s             pdcp_params_mrdc_v1530;
  dyn_octstring                  late_non_crit_ext;
  ue_mrdc_cap_v1560_s            non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-Capability-v15g0 ::= SEQUENCE
struct ue_mrdc_cap_v15g0_s {
  bool                   rf_params_mrdc_v15g0_present = false;
  bool                   non_crit_ext_present         = false;
  rf_params_mrdc_v15g0_s rf_params_mrdc_v15g0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v16c0 ::= SEQUENCE
struct ue_nr_cap_v16c0_s {
  bool              rf_params_v16c0_present = false;
  bool              non_crit_ext_present    = false;
  rf_params_v16c0_s rf_params_v16c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v16a0 ::= SEQUENCE
struct ue_nr_cap_v16a0_s {
  bool               phy_params_v16a0_present = false;
  bool               rf_params_v16a0_present  = false;
  bool               non_crit_ext_present     = false;
  phy_params_v16a0_s phy_params_v16a0;
  rf_params_v16a0_s  rf_params_v16a0;
  ue_nr_cap_v16c0_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v15j0 ::= SEQUENCE
struct ue_nr_cap_v15j0_s {
  bool              non_crit_ext_present = false;
  dyn_octstring     late_non_crit_ext;
  ue_nr_cap_v16a0_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRDC-Parameters-v15c0 ::= SEQUENCE
struct nrdc_params_v15c0_s {
  bool pdcp_dupl_split_srb_present = false;
  bool pdcp_dupl_split_drb_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v15g0 ::= SEQUENCE
struct ue_nr_cap_v15g0_s {
  bool              rf_params_v15g0_present = false;
  bool              non_crit_ext_present    = false;
  rf_params_v15g0_s rf_params_v15g0;
  ue_nr_cap_v15j0_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v15c0 ::= SEQUENCE
struct ue_nr_cap_v15c0_s {
  bool                nrdc_params_v15c0_present           = false;
  bool                partial_fr2_fallback_rx_req_present = false;
  bool                non_crit_ext_present                = false;
  nrdc_params_v15c0_s nrdc_params_v15c0;
  ue_nr_cap_v15g0_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-v1610-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1610_ies_s {
  bool rrc_seg_allowed_r16_present = false;
  bool non_crit_ext_present        = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-v1560-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1560_ies_s {
  bool                         cap_request_filt_common_present = false;
  bool                         non_crit_ext_present            = false;
  ue_cap_request_filt_common_s cap_request_filt_common;
  ue_cap_enquiry_v1610_ies_s   non_crit_ext;

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

// BandCombinationInfo ::= SEQUENCE
struct band_combination_info_s {
  using allowed_feature_sets_list_l_ = dyn_array<uint8_t>;

  // member variables
  uint32_t                     band_combination_idx = 1;
  allowed_feature_sets_list_l_ allowed_feature_sets_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// BandCombinationInfoList ::= SEQUENCE (SIZE (1..65536)) OF BandCombinationInfo
using band_combination_info_list_l = dyn_array<band_combination_info_s>;

// ReestabNCellInfoList ::= SEQUENCE (SIZE (1..32)) OF ReestabNCellInfo
using reestab_ncell_info_list_l = dyn_array<reestab_ncell_info_s>;

// SelectedBandEntriesMN ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..32)
using sel_band_entries_mn_l = bounded_array<uint8_t, 32>;

// T-Offset-r16 ::= ENUMERATED
struct t_offset_r16_opts {
  enum options { ms0dot5, ms0dot75, ms1, ms1dot5, ms2, ms2dot5, ms3, spare1, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using t_offset_r16_e = enumerated<t_offset_r16_opts>;

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

// ConfigRestrictInfoSCG ::= SEQUENCE
struct cfg_restrict_info_scg_s {
  struct pwr_coordination_fr1_s_ {
    bool   p_max_nr_fr1_present = false;
    bool   p_max_eutra_present  = false;
    bool   p_max_ue_fr1_present = false;
    int8_t p_max_nr_fr1         = -30;
    int8_t p_max_eutra          = -30;
    int8_t p_max_ue_fr1         = -30;
  };
  struct serv_cell_idx_range_scg_s_ {
    uint8_t low_bound = 0;
    uint8_t up_bound  = 0;
  };
  using sel_band_entries_mn_list_l_ = dyn_array<sel_band_entries_mn_l>;
  struct pwr_coordination_fr2_r16_s_ {
    bool   p_max_nr_fr2_mcg_r16_present = false;
    bool   p_max_nr_fr2_scg_r16_present = false;
    bool   p_max_ue_fr2_r16_present     = false;
    int8_t p_max_nr_fr2_mcg_r16         = -30;
    int8_t p_max_nr_fr2_scg_r16         = -30;
    int8_t p_max_ue_fr2_r16             = -30;
  };
  struct nrdc_pc_mode_fr1_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrdc_pc_mode_fr1_r16_e_ = enumerated<nrdc_pc_mode_fr1_r16_opts>;
  struct nrdc_pc_mode_fr2_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrdc_pc_mode_fr2_r16_e_ = enumerated<nrdc_pc_mode_fr2_r16_opts>;

  // member variables
  bool                         ext                             = false;
  bool                         pwr_coordination_fr1_present    = false;
  bool                         serv_cell_idx_range_scg_present = false;
  bool                         max_meas_freqs_scg_present      = false;
  bool                         dummy_present                   = false;
  band_combination_info_list_l allowed_bc_list_mrdc;
  pwr_coordination_fr1_s_      pwr_coordination_fr1;
  serv_cell_idx_range_scg_s_   serv_cell_idx_range_scg;
  uint8_t                      max_meas_freqs_scg = 1;
  uint8_t                      dummy              = 1;
  // ...
  // group 0
  bool                                  pdcch_blind_detection_scg_present        = false;
  bool                                  max_num_rohc_context_sessions_sn_present = false;
  copy_ptr<sel_band_entries_mn_list_l_> sel_band_entries_mn_list;
  uint8_t                               pdcch_blind_detection_scg        = 1;
  uint16_t                              max_num_rohc_context_sessions_sn = 0;
  // group 1
  bool    max_intra_freq_meas_identities_scg_present = false;
  bool    max_inter_freq_meas_identities_scg_present = false;
  uint8_t max_intra_freq_meas_identities_scg         = 1;
  uint8_t max_inter_freq_meas_identities_scg         = 1;
  // group 2
  bool                                  p_max_nr_fr1_mcg_r16_present        = false;
  bool                                  nrdc_pc_mode_fr1_r16_present        = false;
  bool                                  nrdc_pc_mode_fr2_r16_present        = false;
  bool                                  max_meas_srs_res_scg_r16_present    = false;
  bool                                  max_meas_cli_res_scg_r16_present    = false;
  bool                                  max_num_ehc_contexts_sn_r16_present = false;
  bool                                  max_toffset_r16_present             = false;
  int8_t                                p_max_nr_fr1_mcg_r16                = -30;
  copy_ptr<pwr_coordination_fr2_r16_s_> pwr_coordination_fr2_r16;
  nrdc_pc_mode_fr1_r16_e_               nrdc_pc_mode_fr1_r16;
  nrdc_pc_mode_fr2_r16_e_               nrdc_pc_mode_fr2_r16;
  uint8_t                               max_meas_srs_res_scg_r16    = 0;
  uint8_t                               max_meas_cli_res_scg_r16    = 0;
  uint32_t                              max_num_ehc_contexts_sn_r16 = 0;
  copy_ptr<overheat_assist_s>           allowed_reduced_cfg_for_overheat_r16;
  t_offset_r16_e                        max_toffset_r16;
  // group 3
  bool                            max_num_udc_drb_r17_present         = false;
  bool                            max_num_cp_cc_andidates_r17_present = false;
  copy_ptr<overheat_assist_r17_s> allowed_reduced_cfg_for_overheat_r17;
  uint8_t                         max_num_udc_drb_r17         = 0;
  uint8_t                         max_num_cp_cc_andidates_r17 = 0;

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

// AffectedCarrierFreqCombEUTRA ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..262143)
using affected_carrier_freq_comb_eutra_l = bounded_array<uint32_t, 32>;

// AffectedCarrierFreqCombNR ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..3279165)
using affected_carrier_freq_comb_nr_l = bounded_array<uint32_t, 32>;

// VictimSystemType ::= SEQUENCE
struct victim_sys_type_s {
  bool gps_present       = false;
  bool glonass_present   = false;
  bool bds_present       = false;
  bool galileo_present   = false;
  bool wlan_present      = false;
  bool bluetooth_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqCombInfoMRDC ::= SEQUENCE
struct affected_carrier_freq_comb_info_mrdc_s {
  struct interference_direction_mrdc_opts {
    enum options { eutra_nr, nr, other, utra_nr_other, nr_other, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  using interference_direction_mrdc_e_ = enumerated<interference_direction_mrdc_opts>;
  struct affected_carrier_freq_comb_mrdc_s_ {
    affected_carrier_freq_comb_eutra_l affected_carrier_freq_comb_eutra;
    affected_carrier_freq_comb_nr_l    affected_carrier_freq_comb_nr;
  };

  // member variables
  bool                               affected_carrier_freq_comb_mrdc_present = false;
  victim_sys_type_s                  victim_sys_type;
  interference_direction_mrdc_e_     interference_direction_mrdc;
  affected_carrier_freq_comb_mrdc_s_ affected_carrier_freq_comb_mrdc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-CandidateInfoId-r17 ::= SEQUENCE
struct cg_candidate_info_id_r17_s {
  uint32_t ssb_freq_r17 = 0;
  uint16_t pci_r17      = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-CandidateInfo-r17 ::= SEQUENCE
struct cg_candidate_info_r17_s {
  cg_candidate_info_id_r17_s cg_candidate_info_id_r17;
  dyn_octstring              candidate_cg_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-CandidateList-r17-IEs ::= SEQUENCE
struct cg_candidate_list_r17_ies_s {
  using cg_candidate_to_add_mod_list_r17_l_ = dyn_array<cg_candidate_info_r17_s>;
  using cg_candidate_to_release_list_r17_l_ = dyn_array<cg_candidate_info_id_r17_s>;

  // member variables
  bool                                non_crit_ext_present = false;
  cg_candidate_to_add_mod_list_r17_l_ cg_candidate_to_add_mod_list_r17;
  cg_candidate_to_release_list_r17_l_ cg_candidate_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-CandidateList ::= SEQUENCE
struct cg_candidate_list_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { cg_candidate_list_r17, spare3, spare2, spare1, nulltype } value;

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
      cg_candidate_list_r17_ies_s& cg_candidate_list_r17()
      {
        assert_choice_type(types::cg_candidate_list_r17, type_, "c1");
        return c;
      }
      const cg_candidate_list_r17_ies_s& cg_candidate_list_r17() const
      {
        assert_choice_type(types::cg_candidate_list_r17, type_, "c1");
        return c;
      }
      cg_candidate_list_r17_ies_s& set_cg_candidate_list_r17();
      void                         set_spare3();
      void                         set_spare2();
      void                         set_spare1();

    private:
      types                       type_;
      cg_candidate_list_r17_ies_s c;
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

// CandidateCell-r17 ::= SEQUENCE
struct candidate_cell_r17_s {
  uint16_t      pci_r17 = 0;
  dyn_octstring cond_execution_cond_scg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCellInfo-r17 ::= SEQUENCE
struct candidate_cell_info_r17_s {
  using candidate_list_r17_l_ = dyn_array<candidate_cell_r17_s>;

  // member variables
  uint32_t              ssb_freq_r17 = 0;
  candidate_list_r17_l_ candidate_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyConfig-NR-r16 ::= SEQUENCE
struct freq_cfg_nr_r16_s {
  uint16_t             freq_band_ind_nr_r16       = 1;
  uint32_t             carrier_center_freq_nr_r16 = 0;
  uint16_t             carrier_bw_nr_r16          = 1;
  subcarrier_spacing_e subcarrier_spacing_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TransmissionBandwidth-EUTRA-r16 ::= ENUMERATED
struct tx_bw_eutra_r16_opts {
  enum options { rb6, rb15, rb25, rb50, rb75, rb100, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using tx_bw_eutra_r16_e = enumerated<tx_bw_eutra_r16_opts>;

// CG-Config-v1730-IEs ::= SEQUENCE
struct cg_cfg_v1730_ies_s {
  bool    fr1_carriers_scg_r17_present = false;
  bool    fr2_carriers_scg_r17_present = false;
  bool    non_crit_ext_present         = false;
  uint8_t fr1_carriers_scg_r17         = 1;
  uint8_t fr2_carriers_scg_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCellInfoListCPC-r17 ::= SEQUENCE (SIZE (1..8)) OF CandidateCellInfo-r17
using candidate_cell_info_list_cp_c_r17_l = dyn_array<candidate_cell_info_r17_s>;

// ServCellInfoXCG-EUTRA-r16 ::= SEQUENCE
struct serv_cell_info_xcg_eutra_r16_s {
  bool              ext                               = false;
  bool              dl_carrier_freq_eutra_r16_present = false;
  bool              ul_carrier_freq_eutra_r16_present = false;
  bool              tx_bw_eutra_r16_present           = false;
  uint32_t          dl_carrier_freq_eutra_r16         = 0;
  uint32_t          ul_carrier_freq_eutra_r16         = 0;
  tx_bw_eutra_r16_e tx_bw_eutra_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServCellInfoXCG-NR-r16 ::= SEQUENCE
struct serv_cell_info_xcg_nr_r16_s {
  bool              ext                         = false;
  bool              dl_freq_info_nr_r16_present = false;
  bool              ul_freq_info_nr_r16_present = false;
  freq_cfg_nr_r16_s dl_freq_info_nr_r16;
  freq_cfg_nr_r16_s ul_freq_info_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-v1700-IEs ::= SEQUENCE
struct cg_cfg_v1700_ies_s {
  bool                                two_phr_mode_scg_r17_present = false;
  bool                                non_crit_ext_present         = false;
  candidate_cell_info_list_cp_c_r17_l candidate_cell_info_list_cp_c_r17;
  cg_cfg_v1730_ies_s                  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServCellInfoListSCG-EUTRA-r16 ::= SEQUENCE (SIZE (1..32)) OF ServCellInfoXCG-EUTRA-r16
using serv_cell_info_list_scg_eutra_r16_l = dyn_array<serv_cell_info_xcg_eutra_r16_s>;

// ServCellInfoListSCG-NR-r16 ::= SEQUENCE (SIZE (1..32)) OF ServCellInfoXCG-NR-r16
using serv_cell_info_list_scg_nr_r16_l = dyn_array<serv_cell_info_xcg_nr_r16_s>;

// CG-Config-v1640-IEs ::= SEQUENCE
struct cg_cfg_v1640_ies_s {
  bool                                non_crit_ext_present = false;
  serv_cell_info_list_scg_nr_r16_l    serv_cell_info_list_scg_nr_r16;
  serv_cell_info_list_scg_eutra_r16_l serv_cell_info_list_scg_eutra_r16;
  cg_cfg_v1700_ies_s                  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-v1630-IEs ::= SEQUENCE
struct cg_cfg_v1630_ies_s {
  bool               sel_toffset_r16_present = false;
  bool               non_crit_ext_present    = false;
  t_offset_r16_e     sel_toffset_r16;
  cg_cfg_v1640_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-v1620-IEs ::= SEQUENCE
struct cg_cfg_v1620_ies_s {
  bool               non_crit_ext_present = false;
  dyn_octstring      ue_assist_info_scg_r16;
  cg_cfg_v1630_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Info2 ::= SEQUENCE
struct drx_info2_s {
  struct drx_on_dur_timer_c_ {
    struct milli_seconds_opts {
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
        ms30,
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
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using milli_seconds_e_ = enumerated<milli_seconds_opts>;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    drx_on_dur_timer_c_() = default;
    drx_on_dur_timer_c_(const drx_on_dur_timer_c_& other);
    drx_on_dur_timer_c_& operator=(const drx_on_dur_timer_c_& other);
    ~drx_on_dur_timer_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer");
      return c.get<milli_seconds_e_>();
    }
    uint8_t&          set_sub_milli_seconds();
    milli_seconds_e_& set_milli_seconds();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  drx_on_dur_timer_c_ drx_on_dur_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-v1610-IEs ::= SEQUENCE
struct cg_cfg_v1610_ies_s {
  bool               drx_info_scg2_present = false;
  bool               non_crit_ext_present  = false;
  drx_info2_s        drx_info_scg2;
  cg_cfg_v1620_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-UplinkCarrierSCG ::= SEQUENCE
struct ph_ul_carrier_scg_s {
  struct ph_type1or3_opts {
    enum options { type1, type3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ph_type1or3_e_ = enumerated<ph_type1or3_opts>;

  // member variables
  bool           ext = false;
  ph_type1or3_e_ ph_type1or3;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-v1590-IEs ::= SEQUENCE
struct cg_cfg_v1590_ies_s {
  using scell_frequencies_sn_nr_l_    = bounded_array<uint32_t, 31>;
  using scell_frequencies_sn_eutra_l_ = bounded_array<uint32_t, 31>;

  // member variables
  bool                          non_crit_ext_present = false;
  scell_frequencies_sn_nr_l_    scell_frequencies_sn_nr;
  scell_frequencies_sn_eutra_l_ scell_frequencies_sn_eutra;
  cg_cfg_v1610_ies_s            non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateServingFreqListEUTRA ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..262143)
using candidate_serving_freq_list_eutra_l = bounded_array<uint32_t, 32>;

// PH-InfoSCG ::= SEQUENCE
struct ph_info_scg_s {
  bool                ext                         = false;
  bool                ph_supplementary_ul_present = false;
  uint8_t             serv_cell_idx               = 0;
  ph_ul_carrier_scg_s ph_ul;
  ph_ul_carrier_scg_s ph_supplementary_ul;
  // ...
  // group 0
  bool two_srs_pusch_repeat_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-v1560-IEs ::= SEQUENCE
struct cg_cfg_v1560_ies_s {
  struct report_cgi_request_eutra_s_ {
    struct requested_cell_info_eutra_s_ {
      uint32_t eutra_freq                         = 0;
      uint16_t cell_for_which_to_report_cgi_eutra = 0;
    };

    // member variables
    bool                         requested_cell_info_eutra_present = false;
    requested_cell_info_eutra_s_ requested_cell_info_eutra;
  };

  // member variables
  bool                                pscell_freq_eutra_present        = false;
  bool                                need_for_gaps_present            = false;
  bool                                drx_cfg_scg_present              = false;
  bool                                report_cgi_request_eutra_present = false;
  bool                                non_crit_ext_present             = false;
  uint32_t                            pscell_freq_eutra                = 0;
  dyn_octstring                       scg_cell_group_cfg_eutra;
  dyn_octstring                       candidate_cell_info_list_sn_eutra;
  candidate_serving_freq_list_eutra_l candidate_serving_freq_list_eutra;
  drx_cfg_s                           drx_cfg_scg;
  report_cgi_request_eutra_s_         report_cgi_request_eutra;
  cg_cfg_v1590_ies_s                  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FR-Info ::= SEQUENCE
struct fr_info_s {
  struct fr_type_opts {
    enum options { fr1, fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using fr_type_e_ = enumerated<fr_type_opts>;

  // member variables
  uint8_t    serv_cell_idx = 0;
  fr_type_e_ fr_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-FreqInfo ::= SEQUENCE
struct nr_freq_info_s {
  bool     ext                   = false;
  bool     measured_freq_present = false;
  uint32_t measured_freq         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-TypeListSCG ::= SEQUENCE (SIZE (1..32)) OF PH-InfoSCG
using ph_type_list_scg_l = dyn_array<ph_info_scg_s>;

// CG-Config-v1540-IEs ::= SEQUENCE
struct cg_cfg_v1540_ies_s {
  struct report_cgi_request_nr_s_ {
    struct requested_cell_info_s_ {
      uint32_t ssb_freq                     = 0;
      uint16_t cell_for_which_to_report_cgi = 0;
    };

    // member variables
    bool                   requested_cell_info_present = false;
    requested_cell_info_s_ requested_cell_info;
  };

  // member variables
  bool                     pscell_freq_present           = false;
  bool                     report_cgi_request_nr_present = false;
  bool                     non_crit_ext_present          = false;
  uint32_t                 pscell_freq                   = 0;
  report_cgi_request_nr_s_ report_cgi_request_nr;
  ph_type_list_scg_l       ph_info_scg;
  cg_cfg_v1560_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateServingFreqListNR ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..3279165)
using candidate_serving_freq_list_nr_l = bounded_array<uint32_t, 32>;

// ConfigRestrictModReqSCG ::= SEQUENCE
struct cfg_restrict_mod_req_scg_s {
  bool                       ext                         = false;
  bool                       requested_bc_mrdc_present   = false;
  bool                       requested_p_max_fr1_present = false;
  band_combination_info_sn_s requested_bc_mrdc;
  int8_t                     requested_p_max_fr1 = -30;
  // ...
  // group 0
  bool    requested_pdcch_blind_detection_scg_present = false;
  bool    requested_p_max_eutra_present               = false;
  uint8_t requested_pdcch_blind_detection_scg         = 1;
  int8_t  requested_p_max_eutra                       = -30;
  // group 1
  bool           requested_p_max_fr2_r16_present                  = false;
  bool           requested_max_inter_freq_meas_id_scg_r16_present = false;
  bool           requested_max_intra_freq_meas_id_scg_r16_present = false;
  bool           requested_toffset_r16_present                    = false;
  int8_t         requested_p_max_fr2_r16                          = -30;
  uint8_t        requested_max_inter_freq_meas_id_scg_r16         = 1;
  uint8_t        requested_max_intra_freq_meas_id_scg_r16         = 1;
  t_offset_r16_e requested_toffset_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Info ::= SEQUENCE
struct drx_info_s {
  struct drx_long_cycle_start_offset_c_ {
    struct types_opts {
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
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    drx_long_cycle_start_offset_c_() = default;
    drx_long_cycle_start_offset_c_(const drx_long_cycle_start_offset_c_& other);
    drx_long_cycle_start_offset_c_& operator=(const drx_long_cycle_start_offset_c_& other);
    ~drx_long_cycle_start_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms32()
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms60()
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms64()
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms70()
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms80()
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms128()
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms160()
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint16_t& ms256()
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms320()
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms512()
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms640()
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1024()
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280()
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2048()
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560()
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120()
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms10240()
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms32() const
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms60() const
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms64() const
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms70() const
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80() const
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms128() const
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160() const
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& ms256() const
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms320() const
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms512() const
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640() const
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1024() const
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280() const
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2048() const
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560() const
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120() const
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10240() const
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint8_t&  set_ms10();
    uint8_t&  set_ms20();
    uint8_t&  set_ms32();
    uint8_t&  set_ms40();
    uint8_t&  set_ms60();
    uint8_t&  set_ms64();
    uint8_t&  set_ms70();
    uint8_t&  set_ms80();
    uint8_t&  set_ms128();
    uint8_t&  set_ms160();
    uint16_t& set_ms256();
    uint16_t& set_ms320();
    uint16_t& set_ms512();
    uint16_t& set_ms640();
    uint16_t& set_ms1024();
    uint16_t& set_ms1280();
    uint16_t& set_ms2048();
    uint16_t& set_ms2560();
    uint16_t& set_ms5120();
    uint16_t& set_ms10240();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct short_drx_s_ {
    struct drx_short_cycle_opts {
      enum options {
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms7,
        ms8,
        ms10,
        ms14,
        ms16,
        ms20,
        ms30,
        ms32,
        ms35,
        ms40,
        ms64,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        spare9,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using drx_short_cycle_e_ = enumerated<drx_short_cycle_opts>;

    // member variables
    drx_short_cycle_e_ drx_short_cycle;
    uint8_t            drx_short_cycle_timer = 1;
  };

  // member variables
  bool                           short_drx_present = false;
  drx_long_cycle_start_offset_c_ drx_long_cycle_start_offset;
  short_drx_s_                   short_drx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FR-InfoList ::= SEQUENCE (SIZE (1..31)) OF FR-Info
using fr_info_list_l = dyn_array<fr_info_s>;

// MeasConfigSN ::= SEQUENCE
struct meas_cfg_sn_s {
  using measured_frequencies_sn_l_ = dyn_array<nr_freq_info_s>;

  // member variables
  bool                       ext = false;
  measured_frequencies_sn_l_ measured_frequencies_sn;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config-IEs ::= SEQUENCE
struct cg_cfg_ies_s {
  bool                             cfg_restrict_mod_req_present = false;
  bool                             drx_info_scg_present         = false;
  bool                             meas_cfg_sn_present          = false;
  bool                             sel_band_combination_present = false;
  bool                             non_crit_ext_present         = false;
  dyn_octstring                    scg_cell_group_cfg;
  dyn_octstring                    scg_rb_cfg;
  cfg_restrict_mod_req_scg_s       cfg_restrict_mod_req;
  drx_info_s                       drx_info_scg;
  dyn_octstring                    candidate_cell_info_list_sn;
  meas_cfg_sn_s                    meas_cfg_sn;
  band_combination_info_sn_s       sel_band_combination;
  fr_info_list_l                   fr_info_list_scg;
  candidate_serving_freq_list_nr_l candidate_serving_freq_list_nr;
  cg_cfg_v1540_ies_s               non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-Config ::= SEQUENCE
struct cg_cfg_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { cg_cfg, spare3, spare2, spare1, nulltype } value;

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
      cg_cfg_ies_s& cg_cfg()
      {
        assert_choice_type(types::cg_cfg, type_, "c1");
        return c;
      }
      const cg_cfg_ies_s& cg_cfg() const
      {
        assert_choice_type(types::cg_cfg, type_, "c1");
        return c;
      }
      cg_cfg_ies_s& set_cg_cfg();
      void          set_spare3();
      void          set_spare2();
      void          set_spare1();

    private:
      types        type_;
      cg_cfg_ies_s c;
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

// CandidateCellCPC-r17 ::= SEQUENCE
struct candidate_cell_cp_c_r17_s {
  using candidate_cell_list_r17_l_ = bounded_array<uint16_t, 8>;

  // member variables
  uint32_t                   ssb_freq_r17 = 0;
  candidate_cell_list_r17_l_ candidate_cell_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo-v1730-IEs ::= SEQUENCE
struct cg_cfg_info_v1730_ies_s {
  bool    fr1_carriers_mcg_r17_present = false;
  bool    fr2_carriers_mcg_r17_present = false;
  bool    non_crit_ext_present         = false;
  uint8_t fr1_carriers_mcg_r17         = 1;
  uint8_t fr2_carriers_mcg_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCellListCPC-r17 ::= SEQUENCE (SIZE (1..8)) OF CandidateCellCPC-r17
using candidate_cell_list_cp_c_r17_l = dyn_array<candidate_cell_cp_c_r17_s>;

// CG-ConfigInfo-v1700-IEs ::= SEQUENCE
struct cg_cfg_info_v1700_ies_s {
  bool                           two_phr_mode_mcg_r17_present                = false;
  bool                           low_mob_eval_connected_in_pcell_r17_present = false;
  bool                           non_crit_ext_present                        = false;
  candidate_cell_list_cp_c_r17_l candidate_cell_list_cp_c_r17;
  cg_cfg_info_v1730_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServCellInfoListMCG-EUTRA-r16 ::= SEQUENCE (SIZE (1..32)) OF ServCellInfoXCG-EUTRA-r16
using serv_cell_info_list_mcg_eutra_r16_l = dyn_array<serv_cell_info_xcg_eutra_r16_s>;

// ServCellInfoListMCG-NR-r16 ::= SEQUENCE (SIZE (1..32)) OF ServCellInfoXCG-NR-r16
using serv_cell_info_list_mcg_nr_r16_l = dyn_array<serv_cell_info_xcg_nr_r16_s>;

// CG-ConfigInfo-v1640-IEs ::= SEQUENCE
struct cg_cfg_info_v1640_ies_s {
  bool                                non_crit_ext_present = false;
  serv_cell_info_list_mcg_nr_r16_l    serv_cell_info_list_mcg_nr_r16;
  serv_cell_info_list_mcg_eutra_r16_l serv_cell_info_list_mcg_eutra_r16;
  cg_cfg_info_v1700_ies_s             non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo-v1620-IEs ::= SEQUENCE
struct cg_cfg_info_v1620_ies_s {
  bool                    non_crit_ext_present = false;
  dyn_octstring           ue_assist_info_source_scg_r16;
  cg_cfg_info_v1640_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo-v1610-IEs ::= SEQUENCE
struct cg_cfg_info_v1610_ies_s {
  struct scg_fail_info_r16_s_ {
    struct fail_type_r16_opts {
      enum options {
        scg_lbt_fail_r16,
        beam_fail_recovery_fail_r16,
        t312_expiry_r16,
        bh_rlf_r16,
        beam_fail_r17,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using fail_type_r16_e_ = enumerated<fail_type_r16_opts>;

    // member variables
    fail_type_r16_e_ fail_type_r16;
    dyn_octstring    meas_result_scg_r16;
  };
  struct dummy1_s_ {
    struct fail_type_eutra_r16_opts {
      enum options {
        scg_lbt_fail_r16,
        beam_fail_recovery_fail_r16,
        t312_expiry_r16,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using fail_type_eutra_r16_e_ = enumerated<fail_type_eutra_r16_opts>;

    // member variables
    fail_type_eutra_r16_e_ fail_type_eutra_r16;
    dyn_octstring          meas_result_scg_eutra_r16;
  };

  // member variables
  bool                    drx_info_mcg2_present     = false;
  bool                    aligned_drx_ind_present   = false;
  bool                    scg_fail_info_r16_present = false;
  bool                    dummy1_present            = false;
  bool                    non_crit_ext_present      = false;
  drx_info2_s             drx_info_mcg2;
  scg_fail_info_r16_s_    scg_fail_info_r16;
  dummy1_s_               dummy1;
  dyn_octstring           sidelink_ue_info_nr_r16;
  dyn_octstring           sidelink_ue_info_eutra_r16;
  cg_cfg_info_v1620_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo-v1590-IEs ::= SEQUENCE
struct cg_cfg_info_v1590_ies_s {
  using serv_frequencies_mn_nr_l_ = bounded_array<uint32_t, 31>;

  // member variables
  bool                      non_crit_ext_present = false;
  serv_frequencies_mn_nr_l_ serv_frequencies_mn_nr;
  cg_cfg_info_v1610_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-UplinkCarrierMCG ::= SEQUENCE
struct ph_ul_carrier_mcg_s {
  struct ph_type1or3_opts {
    enum options { type1, type3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ph_type1or3_e_ = enumerated<ph_type1or3_opts>;

  // member variables
  bool           ext = false;
  ph_type1or3_e_ ph_type1or3;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SFTD-FrequencyList-EUTRA ::= SEQUENCE (SIZE (1..3)) OF INTEGER (0..262143)
using sftd_freq_list_eutra_l = bounded_array<uint32_t, 3>;

// SFTD-FrequencyList-NR ::= SEQUENCE (SIZE (1..3)) OF INTEGER (0..3279165)
using sftd_freq_list_nr_l = bounded_array<uint32_t, 3>;

// CG-ConfigInfo-v1570-IEs ::= SEQUENCE
struct cg_cfg_info_v1570_ies_s {
  bool                    non_crit_ext_present = false;
  sftd_freq_list_nr_l     sftd_freq_list_nr;
  sftd_freq_list_eutra_l  sftd_freq_list_eutra;
  cg_cfg_info_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-InfoMCG ::= SEQUENCE
struct ph_info_mcg_s {
  bool                ext                         = false;
  bool                ph_supplementary_ul_present = false;
  uint8_t             serv_cell_idx               = 0;
  ph_ul_carrier_mcg_s ph_ul;
  ph_ul_carrier_mcg_s ph_supplementary_ul;
  // ...
  // group 0
  bool two_srs_pusch_repeat_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo-v1560-IEs ::= SEQUENCE
struct cg_cfg_info_v1560_ies_s {
  struct scg_fail_info_eutra_s_ {
    struct fail_type_eutra_opts {
      enum options { t313_expiry, random_access_problem, rlc_max_num_retx, scg_change_fail, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using fail_type_eutra_e_ = enumerated<fail_type_eutra_opts>;

    // member variables
    fail_type_eutra_e_ fail_type_eutra;
    dyn_octstring      meas_result_scg_eutra;
  };
  struct meas_result_report_cgi_eutra_s_ {
    uint32_t         eutra_freq                         = 0;
    uint16_t         cell_for_which_to_report_cgi_eutra = 0;
    cgi_info_eutra_s cgi_info_eutra;
  };

  // member variables
  bool                               scg_fail_info_eutra_present          = false;
  bool                               drx_cfg_mcg_present                  = false;
  bool                               meas_result_report_cgi_eutra_present = false;
  bool                               non_crit_ext_present                 = false;
  dyn_octstring                      candidate_cell_info_list_mn_eutra;
  dyn_octstring                      candidate_cell_info_list_sn_eutra;
  dyn_octstring                      source_cfg_scg_eutra;
  scg_fail_info_eutra_s_             scg_fail_info_eutra;
  drx_cfg_s                          drx_cfg_mcg;
  meas_result_report_cgi_eutra_s_    meas_result_report_cgi_eutra;
  meas_result_cell_list_sftd_eutra_l meas_result_cell_list_sftd_eutra;
  fr_info_list_l                     fr_info_list_mcg;
  cg_cfg_info_v1570_ies_s            non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PH-TypeListMCG ::= SEQUENCE (SIZE (1..32)) OF PH-InfoMCG
using ph_type_list_mcg_l = dyn_array<ph_info_mcg_s>;

// CG-ConfigInfo-v1540-IEs ::= SEQUENCE
struct cg_cfg_info_v1540_ies_s {
  struct meas_result_report_cgi_s_ {
    uint32_t      ssb_freq                     = 0;
    uint16_t      cell_for_which_to_report_cgi = 0;
    cgi_info_nr_s cgi_info;
  };

  // member variables
  bool                      meas_result_report_cgi_present = false;
  bool                      non_crit_ext_present           = false;
  ph_type_list_mcg_l        ph_info_mcg;
  meas_result_report_cgi_s_ meas_result_report_cgi;
  cg_cfg_info_v1560_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-AssistanceInfo ::= SEQUENCE
struct mrdc_assist_info_s {
  using affected_carrier_freq_comb_info_list_mrdc_l_ = dyn_array<affected_carrier_freq_comb_info_mrdc_s>;

  // member variables
  bool                                         ext = false;
  affected_carrier_freq_comb_info_list_mrdc_l_ affected_carrier_freq_comb_info_list_mrdc;
  // ...
  // group 0
  dyn_octstring overheat_assist_scg_r16;
  // group 1
  dyn_octstring overheat_assist_scg_fr2_2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasConfigMN ::= SEQUENCE
struct meas_cfg_mn_s {
  using measured_frequencies_mn_l_ = dyn_array<nr_freq_info_s>;
  struct gap_purpose_opts {
    enum options { per_ue, per_fr1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using gap_purpose_e_ = enumerated<gap_purpose_opts>;

  // member variables
  bool                       ext                  = false;
  bool                       meas_gap_cfg_present = false;
  bool                       gap_purpose_present  = false;
  measured_frequencies_mn_l_ measured_frequencies_mn;
  setup_release_c<gap_cfg_s> meas_gap_cfg;
  gap_purpose_e_             gap_purpose;
  // ...
  // group 0
  copy_ptr<setup_release_c<gap_cfg_s>> meas_gap_cfg_fr2;
  // group 1
  bool inter_freq_no_gap_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo-IEs ::= SEQUENCE
struct cg_cfg_info_ies_s {
  struct scg_fail_info_s_ {
    struct fail_type_opts {
      enum options {
        t310_expiry,
        random_access_problem,
        rlc_max_num_retx,
        synch_recfg_fail_scg,
        scg_recfg_fail,
        srb3_integrity_fail,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using fail_type_e_ = enumerated<fail_type_opts>;

    // member variables
    fail_type_e_  fail_type;
    dyn_octstring meas_result_scg;
  };

  // member variables
  bool                            scg_fail_info_present     = false;
  bool                            cfg_restrict_info_present = false;
  bool                            drx_info_mcg_present      = false;
  bool                            meas_cfg_mn_present       = false;
  bool                            mrdc_assist_info_present  = false;
  bool                            non_crit_ext_present      = false;
  dyn_octstring                   ue_cap_info;
  meas_result_list2_nr_l          candidate_cell_info_list_mn;
  dyn_octstring                   candidate_cell_info_list_sn;
  meas_result_cell_list_sftd_nr_l meas_result_cell_list_sftd_nr;
  scg_fail_info_s_                scg_fail_info;
  cfg_restrict_info_scg_s         cfg_restrict_info;
  drx_info_s                      drx_info_mcg;
  meas_cfg_mn_s                   meas_cfg_mn;
  dyn_octstring                   source_cfg_scg;
  dyn_octstring                   scg_rb_cfg;
  dyn_octstring                   mcg_rb_cfg;
  mrdc_assist_info_s              mrdc_assist_info;
  cg_cfg_info_v1540_ies_s         non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-ConfigInfo ::= SEQUENCE
struct cg_cfg_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { cg_cfg_info, spare3, spare2, spare1, nulltype } value;

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
      cg_cfg_info_ies_s& cg_cfg_info()
      {
        assert_choice_type(types::cg_cfg_info, type_, "c1");
        return c;
      }
      const cg_cfg_info_ies_s& cg_cfg_info() const
      {
        assert_choice_type(types::cg_cfg_info, type_, "c1");
        return c;
      }
      cg_cfg_info_ies_s& set_cg_cfg_info();
      void               set_spare3();
      void               set_spare2();
      void               set_spare1();

    private:
      types             type_;
      cg_cfg_info_ies_s c;
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

// HandoverCommand-IEs ::= SEQUENCE
struct ho_cmd_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ho_cmd_msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCommand ::= SEQUENCE
struct ho_cmd_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_cmd, spare3, spare2, spare1, nulltype } value;

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
      ho_cmd_ies_s& ho_cmd()
      {
        assert_choice_type(types::ho_cmd, type_, "c1");
        return c;
      }
      const ho_cmd_ies_s& ho_cmd() const
      {
        assert_choice_type(types::ho_cmd, type_, "c1");
        return c;
      }
      ho_cmd_ies_s& set_ho_cmd();
      void          set_spare3();
      void          set_spare2();
      void          set_spare1();

    private:
      types        type_;
      ho_cmd_ies_s c;
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

// MeasTiming ::= SEQUENCE
struct meas_timing_s {
  struct freq_and_timing_s_ {
    bool                 ss_rssi_meas_present = false;
    uint32_t             carrier_freq         = 0;
    subcarrier_spacing_e ssb_subcarrier_spacing;
    ssb_mtc_s            ssb_meas_timing_cfg;
    ss_rssi_meas_s       ss_rssi_meas;
  };

  // member variables
  bool               ext                     = false;
  bool               freq_and_timing_present = false;
  freq_and_timing_s_ freq_and_timing;
  // ...
  // group 0
  bool                       pci_present = false;
  copy_ptr<ssb_to_measure_c> ssb_to_measure;
  uint16_t                   pci = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTimingList ::= SEQUENCE (SIZE (1..32)) OF MeasTiming
using meas_timing_list_l = dyn_array<meas_timing_s>;

// MeasurementTimingConfiguration-v1610-IEs ::= SEQUENCE
struct meas_timing_cfg_v1610_ies_s {
  struct csi_rs_cfg_r16_s_ {
    subcarrier_spacing_e csi_rs_subcarrier_spacing_r16;
    csi_rs_cell_mob_s    csi_rs_cell_mob_r16;
    uint32_t             ref_ssb_freq_r16 = 0;
  };

  // member variables
  bool              non_crit_ext_present = false;
  csi_rs_cfg_r16_s_ csi_rs_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementTimingConfiguration-v1550-IEs ::= SEQUENCE
struct meas_timing_cfg_v1550_ies_s {
  bool                        non_crit_ext_present      = false;
  bool                        camp_on_first_ssb         = false;
  bool                        ps_cell_only_on_first_ssb = false;
  meas_timing_cfg_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementTimingConfiguration-IEs ::= SEQUENCE
struct meas_timing_cfg_ies_s {
  bool                        non_crit_ext_present = false;
  meas_timing_list_l          meas_timing;
  meas_timing_cfg_v1550_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementTimingConfiguration ::= SEQUENCE
struct meas_timing_cfg_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { meas_timing_conf, spare3, spare2, spare1, nulltype } value;

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
      meas_timing_cfg_ies_s& meas_timing_conf()
      {
        assert_choice_type(types::meas_timing_conf, type_, "c1");
        return c;
      }
      const meas_timing_cfg_ies_s& meas_timing_conf() const
      {
        assert_choice_type(types::meas_timing_conf, type_, "c1");
        return c;
      }
      meas_timing_cfg_ies_s& set_meas_timing_conf();
      void                   set_spare3();
      void                   set_spare2();
      void                   set_spare1();

    private:
      types                 type_;
      meas_timing_cfg_ies_s c;
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

// SL-AccessInfo-L2U2N-r17 ::= SEQUENCE
struct sl_access_info_l2_u2_n_r17_s {
  bool                       ext = false;
  cell_access_related_info_s cell_access_related_info_r17;
  sl_serving_cell_info_r17_s sl_serving_cell_info_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation-IEs ::= SEQUENCE
struct ue_radio_access_cap_info_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ue_radio_access_cap_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation ::= SEQUENCE
struct ue_radio_access_cap_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options {
          ue_radio_access_cap_info,
          spare7,
          spare6,
          spare5,
          spare4,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;

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
      ue_radio_access_cap_info_ies_s& ue_radio_access_cap_info()
      {
        assert_choice_type(types::ue_radio_access_cap_info, type_, "c1");
        return c;
      }
      const ue_radio_access_cap_info_ies_s& ue_radio_access_cap_info() const
      {
        assert_choice_type(types::ue_radio_access_cap_info, type_, "c1");
        return c;
      }
      ue_radio_access_cap_info_ies_s& set_ue_radio_access_cap_info();
      void                            set_spare7();
      void                            set_spare6();
      void                            set_spare5();
      void                            set_spare4();
      void                            set_spare3();
      void                            set_spare2();
      void                            set_spare1();

    private:
      types                          type_;
      ue_radio_access_cap_info_ies_s c;
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

// UERadioPagingInformation-v1700-IEs ::= SEQUENCE
struct ue_radio_paging_info_v1700_ies_s {
  struct nof_rx_red_cap_r17_opts {
    enum options { one, two, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nof_rx_red_cap_r17_e_                 = enumerated<nof_rx_red_cap_r17_opts>;
  using half_duplex_fdd_type_a_red_cap_r17_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                  inactive_state_po_determination_r17_present = false;
  bool                                  nof_rx_red_cap_r17_present                  = false;
  bool                                  non_crit_ext_present                        = false;
  dyn_octstring                         ue_radio_paging_info_r17;
  nof_rx_red_cap_r17_e_                 nof_rx_red_cap_r17;
  half_duplex_fdd_type_a_red_cap_r17_l_ half_duplex_fdd_type_a_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation-v15e0-IEs ::= SEQUENCE
struct ue_radio_paging_info_v15e0_ies_s {
  bool                             dl_sched_offset_pdsch_type_a_fdd_fr1_present = false;
  bool                             dl_sched_offset_pdsch_type_a_tdd_fr1_present = false;
  bool                             dl_sched_offset_pdsch_type_a_tdd_fr2_present = false;
  bool                             dl_sched_offset_pdsch_type_b_fdd_fr1_present = false;
  bool                             dl_sched_offset_pdsch_type_b_tdd_fr1_present = false;
  bool                             dl_sched_offset_pdsch_type_b_tdd_fr2_present = false;
  bool                             non_crit_ext_present                         = false;
  ue_radio_paging_info_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation-IEs ::= SEQUENCE
struct ue_radio_paging_info_ies_s {
  using supported_band_list_nr_for_paging_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                 non_crit_ext_present = false;
  supported_band_list_nr_for_paging_l_ supported_band_list_nr_for_paging;
  ue_radio_paging_info_v15e0_ies_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation ::= SEQUENCE
struct ue_radio_paging_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_radio_paging_info, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

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
      ue_radio_paging_info_ies_s& ue_radio_paging_info()
      {
        assert_choice_type(types::ue_radio_paging_info, type_, "c1");
        return c;
      }
      const ue_radio_paging_info_ies_s& ue_radio_paging_info() const
      {
        assert_choice_type(types::ue_radio_paging_info, type_, "c1");
        return c;
      }
      ue_radio_paging_info_ies_s& set_ue_radio_paging_info();
      void                        set_spare7();
      void                        set_spare6();
      void                        set_spare5();
      void                        set_spare4();
      void                        set_spare3();
      void                        set_spare2();
      void                        set_spare1();

    private:
      types                      type_;
      ue_radio_paging_info_ies_s c;
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

// VarMobilityHistoryReport-r16 ::= VisitedCellInfoList-r16
using var_mob_history_report_r16_l = visited_cell_info_list_r16_l;

} // namespace rrc_nr
} // namespace asn1

/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "dl_dcch_msg.h"
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
  typedef enumerated<sub_carrier_spacing_common_opts> sub_carrier_spacing_common_e_;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_type_a_position_opts> dmrs_type_a_position_e_;
  struct cell_barred_opts {
    enum options { barred, not_barred, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_barred_opts> cell_barred_e_;
  struct intra_freq_resel_opts {
    enum options { allowed, not_allowed, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<intra_freq_resel_opts> intra_freq_resel_e_;

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
  typedef enumerated<types_opts> types;

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
    typedef enumerated<type1_r17_opts, true> type1_r17_e_;
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
      typedef enumerated<pos_sib_type_r17_opts, true> pos_sib_type_r17_e_;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<ssb_per_rach_occasion_opts> ssb_per_rach_occasion_e_;

    // member variables
    rach_cfg_generic_s       rach_cfg_si;
    ssb_per_rach_occasion_e_ ssb_per_rach_occasion;
  };
  struct si_request_period_opts {
    enum options { one, two, four, six, eight, ten, twelve, sixteen, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<si_request_period_opts> si_request_period_e_;
  using si_request_res_l_ = dyn_array<si_request_res_s>;

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
    typedef enumerated<types_opts> types;

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

// TA-Info-r17 ::= SEQUENCE
struct ta_info_r17_s {
  bool     ta_common_drift_r17_present         = false;
  bool     ta_common_drift_variant_r17_present = false;
  uint32_t ta_common_r17                       = 0;
  int32_t  ta_common_drift_r17                 = -257303;
  uint16_t ta_common_drift_variant_r17         = 0;

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

// NTN-Config-r17 ::= SEQUENCE
struct ntn_cfg_r17_s {
  struct ntn_ul_sync_validity_dur_r17_opts {
    enum options { s5, s10, s15, s20, s25, s30, s35, s40, s45, s50, s55, s60, s120, s180, s240, s900, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ntn_ul_sync_validity_dur_r17_opts> ntn_ul_sync_validity_dur_r17_e_;
  struct ntn_polarization_dl_r17_opts {
    enum options { rhcp, lhcp, linear, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ntn_polarization_dl_r17_opts> ntn_polarization_dl_r17_e_;
  struct ntn_polarization_ul_r17_opts {
    enum options { rhcp, lhcp, linear, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ntn_polarization_ul_r17_opts> ntn_polarization_ul_r17_e_;

  // member variables
  bool                            ext                                  = false;
  bool                            epoch_time_r17_present               = false;
  bool                            ntn_ul_sync_validity_dur_r17_present = false;
  bool                            cell_specific_koffset_r17_present    = false;
  bool                            kmac_r17_present                     = false;
  bool                            ta_info_r17_present                  = false;
  bool                            ntn_polarization_dl_r17_present      = false;
  bool                            ntn_polarization_ul_r17_present      = false;
  bool                            ephemeris_info_r17_present           = false;
  bool                            ta_report_r17_present                = false;
  epoch_time_r17_s                epoch_time_r17;
  ntn_ul_sync_validity_dur_r17_e_ ntn_ul_sync_validity_dur_r17;
  uint16_t                        cell_specific_koffset_r17 = 1;
  uint16_t                        kmac_r17                  = 1;
  ta_info_r17_s                   ta_info_r17;
  ntn_polarization_dl_r17_e_      ntn_polarization_dl_r17;
  ntn_polarization_ul_r17_e_      ntn_polarization_ul_r17;
  ephemeris_info_r17_c            ephemeris_info_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigPTM-r17 ::= SEQUENCE
struct pdsch_cfg_ptm_r17_s {
  struct pdsch_aggregation_factor_r17_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_aggregation_factor_r17_opts> pdsch_aggregation_factor_r17_e_;

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
  typedef enumerated<pos_sib_type_r16_opts, true> pos_sib_type_r16_e_;

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
  typedef enumerated<periodicity_opts> periodicity_e_;

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
  typedef enumerated<si_broadcast_status_r17_opts> si_broadcast_status_r17_e_;
  struct si_periodicity_r17_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_periodicity_r17_opts> si_periodicity_r17_e_;

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
  typedef enumerated<sf_medium_opts> sf_medium_e_;
  struct sf_high_opts {
    enum options { odot25, odot5, odot75, ldot0, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<sf_high_opts> sf_high_e_;

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
  typedef enumerated<uac_barr_factor_for_ai3_r17_opts> uac_barr_factor_for_ai3_r17_e_;

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
  typedef enumerated<types_opts> types;

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
  typedef enumerated<types_opts> types;

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
  typedef enumerated<mcs_table_r17_opts> mcs_table_r17_e_;
  struct xoverhead_r17_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<xoverhead_r17_opts> xoverhead_r17_e_;

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
    typedef enumerated<cell_barred_red_cap1_rx_r17_opts> cell_barred_red_cap1_rx_r17_e_;
    struct cell_barred_red_cap2_rx_r17_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_red_cap2_rx_r17_opts> cell_barred_red_cap2_rx_r17_e_;

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
  typedef enumerated<sdt_lc_ch_sr_delay_timer_r17_opts> sdt_lc_ch_sr_delay_timer_r17_e_;
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
  typedef enumerated<sdt_data_volume_thres_r17_opts> sdt_data_volume_thres_r17_e_;
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
  typedef enumerated<t319a_r17_opts> t319a_r17_e_;

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
  typedef enumerated<type_opts, true> type_e_;

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
  typedef enumerated<types_opts> types;

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
  typedef enumerated<mcch_win_dur_r17_opts> mcch_win_dur_r17_e_;
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
  typedef enumerated<mcch_mod_period_r17_opts> mcch_mod_period_r17_e_;

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
  typedef enumerated<t_eval_opts> t_eval_e_;
  struct t_hyst_normal_opts {
    enum options { s30, s60, s120, s180, s240, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t_hyst_normal_opts> t_hyst_normal_e_;

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
  typedef enumerated<pos_si_periodicity_r16_opts> pos_si_periodicity_r16_e_;
  struct pos_si_broadcast_status_r16_opts {
    enum options { broadcasting, not_broadcasting, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pos_si_broadcast_status_r16_opts> pos_si_broadcast_status_r16_e_;

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
  typedef enumerated<intra_freq_resel_red_cap_r17_opts> intra_freq_resel_red_cap_r17_e_;
  struct cell_barred_ntn_r17_opts {
    enum options { barred, not_barred, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_barred_ntn_r17_opts> cell_barred_ntn_r17_e_;

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
typedef enumerated<uac_ac1_select_assist_info_r16_opts> uac_ac1_select_assist_info_r16_e;

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
  typedef enumerated<segment_type_r16_opts> segment_type_r16_e_;

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
  typedef enumerated<segment_type_r17_opts> segment_type_r17_e_;

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
    typedef enumerated<q_hyst_opts> q_hyst_e_;
    struct speed_state_resel_pars_s_ {
      struct q_hyst_sf_s_ {
        struct sf_medium_opts {
          enum options { db_neg6, db_neg4, db_neg2, db0, nulltype } value;
          typedef int8_t number_type;

          const char* to_string() const;
          int8_t      to_number() const;
        };
        typedef enumerated<sf_medium_opts> sf_medium_e_;
        struct sf_high_opts {
          enum options { db_neg6, db_neg4, db_neg2, db0, nulltype } value;
          typedef int8_t number_type;

          const char* to_string() const;
          int8_t      to_number() const;
        };
        typedef enumerated<sf_high_opts> sf_high_e_;

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
      typedef enumerated<s_search_delta_p_r16_opts> s_search_delta_p_r16_e_;
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
      typedef enumerated<t_search_delta_p_r16_opts> t_search_delta_p_r16_e_;

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
      typedef enumerated<s_search_delta_p_stationary_r17_opts> s_search_delta_p_stationary_r17_e_;
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
      typedef enumerated<t_search_delta_p_stationary_r17_opts> t_search_delta_p_stationary_r17_e_;

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
  typedef enumerated<warning_msg_segment_type_opts> warning_msg_segment_type_e_;

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
  typedef enumerated<warning_msg_segment_type_opts> warning_msg_segment_type_e_;

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
  typedef enumerated<si_broadcast_status_opts> si_broadcast_status_e_;
  struct si_periodicity_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_periodicity_opts> si_periodicity_e_;

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
  typedef enumerated<uac_barr_factor_opts> uac_barr_factor_e_;
  struct uac_barr_time_opts {
    enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<uac_barr_time_opts> uac_barr_time_e_;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<conn_est_fail_count_opts> conn_est_fail_count_e_;
  struct conn_est_fail_offset_validity_opts {
    enum options { s30, s60, s120, s240, s300, s420, s600, s900, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<conn_est_fail_offset_validity_opts> conn_est_fail_offset_validity_e_;

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
    typedef enumerated<types_opts, true, 7> types;

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
  typedef enumerated<si_win_len_opts> si_win_len_e_;

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
    typedef enumerated<types_opts, true, 12> types;

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
typedef enumerated<uac_access_category1_sel_assist_info_opts> uac_access_category1_sel_assist_info_e;

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
  typedef enumerated<t300_opts> t300_e_;
  struct t301_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t301_opts> t301_e_;
  struct t310_opts {
    enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t310_opts> t310_e_;
  struct n310_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n310_opts> n310_e_;
  struct t311_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t311_opts> t311_e_;
  struct n311_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n311_opts> n311_e_;
  struct t319_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t319_opts> t319_e_;

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
      typedef enumerated<types_opts> types;

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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<types_opts> types;

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

// T-Reassembly ::= ENUMERATED
struct t_reassembly_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms110,
    ms120,
    ms130,
    ms140,
    ms150,
    ms160,
    ms170,
    ms180,
    ms190,
    ms200,
    spare1,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<t_reassembly_opts> t_reassembly_e;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<t_reordering_r17_opts> t_reordering_r17_e_;

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
  typedef enumerated<types_opts> types;

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

// DRX-ConfigPTM-r17 ::= SEQUENCE
struct drx_cfg_ptm_r17_s {
  struct drx_on_dur_timer_ptm_r17_c_ {
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
    typedef enumerated<milli_seconds_opts> milli_seconds_e_;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    drx_on_dur_timer_ptm_r17_c_() = default;
    drx_on_dur_timer_ptm_r17_c_(const drx_on_dur_timer_ptm_r17_c_& other);
    drx_on_dur_timer_ptm_r17_c_& operator=(const drx_on_dur_timer_ptm_r17_c_& other);
    ~drx_on_dur_timer_ptm_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<milli_seconds_e_>();
    }
    uint8_t&          set_sub_milli_seconds();
    milli_seconds_e_& set_milli_seconds();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct drx_inactivity_timer_ptm_r17_opts {
    enum options {
      ms0,
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
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
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
  typedef enumerated<drx_inactivity_timer_ptm_r17_opts> drx_inactivity_timer_ptm_r17_e_;
  struct drx_retx_timer_dl_ptm_r17_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
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
  typedef enumerated<drx_retx_timer_dl_ptm_r17_opts> drx_retx_timer_dl_ptm_r17_e_;
  struct drx_long_cycle_start_offset_ptm_r17_c_ {
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
    typedef enumerated<types_opts> types;

    // choice methods
    drx_long_cycle_start_offset_ptm_r17_c_() = default;
    drx_long_cycle_start_offset_ptm_r17_c_(const drx_long_cycle_start_offset_ptm_r17_c_& other);
    drx_long_cycle_start_offset_ptm_r17_c_& operator=(const drx_long_cycle_start_offset_ptm_r17_c_& other);
    ~drx_long_cycle_start_offset_ptm_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms32()
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms60()
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms64()
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms70()
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms80()
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms128()
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms160()
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint16_t& ms256()
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms320()
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms512()
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms640()
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms1024()
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280()
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms2048()
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560()
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120()
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms10240()
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms32() const
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms60() const
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms64() const
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms70() const
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80() const
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms128() const
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160() const
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& ms256() const
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms320() const
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms512() const
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640() const
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1024() const
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280() const
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2048() const
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560() const
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120() const
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10240() const
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffsetPTM-r17");
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

  // member variables
  bool                                   drx_harq_rtt_timer_dl_ptm_r17_present = false;
  bool                                   drx_retx_timer_dl_ptm_r17_present     = false;
  drx_on_dur_timer_ptm_r17_c_            drx_on_dur_timer_ptm_r17;
  drx_inactivity_timer_ptm_r17_e_        drx_inactivity_timer_ptm_r17;
  uint8_t                                drx_harq_rtt_timer_dl_ptm_r17 = 0;
  drx_retx_timer_dl_ptm_r17_e_           drx_retx_timer_dl_ptm_r17;
  drx_long_cycle_start_offset_ptm_r17_c_ drx_long_cycle_start_offset_ptm_r17;
  uint8_t                                drx_slot_offset_ptm_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<types_opts> types;

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
  typedef enumerated<types_opts, true> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<types_opts> types;

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

// BH-LogicalChannelIdentity-r16 ::= CHOICE
struct bh_lc_ch_id_r16_c {
  struct types_opts {
    enum options { bh_lc_ch_id_r16, bh_lc_ch_id_ext_r16, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  bh_lc_ch_id_r16_c() = default;
  bh_lc_ch_id_r16_c(const bh_lc_ch_id_r16_c& other);
  bh_lc_ch_id_r16_c& operator=(const bh_lc_ch_id_r16_c& other);
  ~bh_lc_ch_id_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& bh_lc_ch_id_r16()
  {
    assert_choice_type(types::bh_lc_ch_id_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint8_t>();
  }
  uint32_t& bh_lc_ch_id_ext_r16()
  {
    assert_choice_type(types::bh_lc_ch_id_ext_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint32_t>();
  }
  const uint8_t& bh_lc_ch_id_r16() const
  {
    assert_choice_type(types::bh_lc_ch_id_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint8_t>();
  }
  const uint32_t& bh_lc_ch_id_ext_r16() const
  {
    assert_choice_type(types::bh_lc_ch_id_ext_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint32_t>();
  }
  uint8_t&  set_bh_lc_ch_id_r16();
  uint32_t& set_bh_lc_ch_id_ext_r16();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// T-StatusProhibit ::= ENUMERATED
struct t_status_prohibit_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms105,
    ms110,
    ms115,
    ms120,
    ms125,
    ms130,
    ms135,
    ms140,
    ms145,
    ms150,
    ms155,
    ms160,
    ms165,
    ms170,
    ms175,
    ms180,
    ms185,
    ms190,
    ms195,
    ms200,
    ms205,
    ms210,
    ms215,
    ms220,
    ms225,
    ms230,
    ms235,
    ms240,
    ms245,
    ms250,
    ms300,
    ms350,
    ms400,
    ms450,
    ms500,
    ms800,
    ms1000,
    ms1200,
    ms1600,
    ms2000,
    ms2400,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_status_prohibit_opts> t_status_prohibit_e;

// DL-AM-RLC ::= SEQUENCE
struct dl_am_rlc_s {
  bool                sn_field_len_present = false;
  sn_field_len_am_e   sn_field_len;
  t_reassembly_e      t_reassembly;
  t_status_prohibit_e t_status_prohibit;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-UM-RLC ::= SEQUENCE
struct dl_um_rlc_s {
  bool              sn_field_len_present = false;
  sn_field_len_um_e sn_field_len;
  t_reassembly_e    t_reassembly;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-AM-RLC ::= SEQUENCE
struct ul_am_rlc_s {
  struct max_retx_thres_opts {
    enum options { t1, t2, t3, t4, t6, t8, t16, t32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_retx_thres_opts> max_retx_thres_e_;

  // member variables
  bool              sn_field_len_present = false;
  sn_field_len_am_e sn_field_len;
  t_poll_retx_e     t_poll_retx;
  poll_pdu_e        poll_pdu;
  poll_byte_e       poll_byte;
  max_retx_thres_e_ max_retx_thres;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-UM-RLC ::= SEQUENCE
struct ul_um_rlc_s {
  bool              sn_field_len_present = false;
  sn_field_len_um_e sn_field_len;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogicalChannelConfig ::= SEQUENCE
struct lc_ch_cfg_s {
  struct ul_specific_params_s_ {
    struct prioritised_bit_rate_opts {
      enum options {
        kbps0,
        kbps8,
        kbps16,
        kbps32,
        kbps64,
        kbps128,
        kbps256,
        kbps512,
        kbps1024,
        kbps2048,
        kbps4096,
        kbps8192,
        kbps16384,
        kbps32768,
        kbps65536,
        infinity,
        nulltype
      } value;
      typedef int32_t number_type;

      const char* to_string() const;
      int32_t     to_number() const;
    };
    typedef enumerated<prioritised_bit_rate_opts> prioritised_bit_rate_e_;
    struct bucket_size_dur_opts {
      enum options {
        ms5,
        ms10,
        ms20,
        ms50,
        ms100,
        ms150,
        ms300,
        ms500,
        ms1000,
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
    typedef enumerated<bucket_size_dur_opts> bucket_size_dur_e_;
    using allowed_serving_cells_l_ = bounded_array<uint8_t, 31>;
    using allowed_scs_list_l_      = bounded_array<subcarrier_spacing_e, 5>;
    struct max_pusch_dur_opts {
      enum options { ms0p02, ms0p04, ms0p0625, ms0p125, ms0p25, ms0p5, ms0p01_v1700, spare1, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<max_pusch_dur_opts> max_pusch_dur_e_;
    struct bit_rate_query_prohibit_timer_opts {
      enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<bit_rate_query_prohibit_timer_opts> bit_rate_query_prohibit_timer_e_;
    using allowed_cg_list_r16_l_ = bounded_array<uint8_t, 31>;
    struct allowed_phy_prio_idx_r16_opts {
      enum options { p0, p1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<allowed_phy_prio_idx_r16_opts> allowed_phy_prio_idx_r16_e_;
    struct allowed_harq_mode_r17_opts {
      enum options { harq_mode_a, harq_mode_b, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<allowed_harq_mode_r17_opts> allowed_harq_mode_r17_e_;

    // member variables
    bool                     ext                             = false;
    bool                     max_pusch_dur_present           = false;
    bool                     cfg_grant_type1_allowed_present = false;
    bool                     lc_ch_group_present             = false;
    bool                     sched_request_id_present        = false;
    uint8_t                  prio                            = 1;
    prioritised_bit_rate_e_  prioritised_bit_rate;
    bucket_size_dur_e_       bucket_size_dur;
    allowed_serving_cells_l_ allowed_serving_cells;
    allowed_scs_list_l_      allowed_scs_list;
    max_pusch_dur_e_         max_pusch_dur;
    uint8_t                  lc_ch_group                  = 0;
    uint8_t                  sched_request_id             = 0;
    bool                     lc_ch_sr_mask                = false;
    bool                     lc_ch_sr_delay_timer_applied = false;
    // ...
    bool                             bit_rate_query_prohibit_timer_present = false;
    bit_rate_query_prohibit_timer_e_ bit_rate_query_prohibit_timer;
    // group 0
    bool                             allowed_phy_prio_idx_r16_present = false;
    copy_ptr<allowed_cg_list_r16_l_> allowed_cg_list_r16;
    allowed_phy_prio_idx_r16_e_      allowed_phy_prio_idx_r16;
    // group 1
    bool                     lc_ch_group_iab_ext_r17_present = false;
    bool                     allowed_harq_mode_r17_present   = false;
    uint16_t                 lc_ch_group_iab_ext_r17         = 0;
    allowed_harq_mode_r17_e_ allowed_harq_mode_r17;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  struct bit_rate_multiplier_r16_opts {
    enum options { x40, x70, x100, x200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<bit_rate_multiplier_r16_opts> bit_rate_multiplier_r16_e_;

  // member variables
  bool                  ext                        = false;
  bool                  ul_specific_params_present = false;
  ul_specific_params_s_ ul_specific_params;
  // ...
  // group 0
  bool                       ch_access_prio_r16_present      = false;
  bool                       bit_rate_multiplier_r16_present = false;
  uint8_t                    ch_access_prio_r16              = 1;
  bit_rate_multiplier_r16_e_ bit_rate_multiplier_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config ::= CHOICE
struct rlc_cfg_c {
  struct am_s_ {
    ul_am_rlc_s ul_am_rlc;
    dl_am_rlc_s dl_am_rlc;
  };
  struct um_bi_dir_s_ {
    ul_um_rlc_s ul_um_rlc;
    dl_um_rlc_s dl_um_rlc;
  };
  struct um_uni_dir_ul_s_ {
    ul_um_rlc_s ul_um_rlc;
  };
  struct um_uni_dir_dl_s_ {
    dl_um_rlc_s dl_um_rlc;
  };
  struct types_opts {
    enum options { am, um_bi_dir, um_uni_dir_ul, um_uni_dir_dl, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  rlc_cfg_c() = default;
  rlc_cfg_c(const rlc_cfg_c& other);
  rlc_cfg_c& operator=(const rlc_cfg_c& other);
  ~rlc_cfg_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  am_s_& am()
  {
    assert_choice_type(types::am, type_, "RLC-Config");
    return c.get<am_s_>();
  }
  um_bi_dir_s_& um_bi_dir()
  {
    assert_choice_type(types::um_bi_dir, type_, "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  um_uni_dir_ul_s_& um_uni_dir_ul()
  {
    assert_choice_type(types::um_uni_dir_ul, type_, "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  um_uni_dir_dl_s_& um_uni_dir_dl()
  {
    assert_choice_type(types::um_uni_dir_dl, type_, "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  const am_s_& am() const
  {
    assert_choice_type(types::am, type_, "RLC-Config");
    return c.get<am_s_>();
  }
  const um_bi_dir_s_& um_bi_dir() const
  {
    assert_choice_type(types::um_bi_dir, type_, "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  const um_uni_dir_ul_s_& um_uni_dir_ul() const
  {
    assert_choice_type(types::um_uni_dir_ul, type_, "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  const um_uni_dir_dl_s_& um_uni_dir_dl() const
  {
    assert_choice_type(types::um_uni_dir_dl, type_, "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  am_s_&            set_am();
  um_bi_dir_s_&     set_um_bi_dir();
  um_uni_dir_ul_s_& set_um_uni_dir_ul();
  um_uni_dir_dl_s_& set_um_uni_dir_dl();

private:
  types                                                                    type_;
  choice_buffer_t<am_s_, um_bi_dir_s_, um_uni_dir_dl_s_, um_uni_dir_ul_s_> c;

  void destroy_();
};

// BH-RLC-ChannelConfig-r16 ::= SEQUENCE
struct bh_rlc_ch_cfg_r16_s {
  bool                ext                         = false;
  bool                bh_lc_ch_id_r16_present     = false;
  bool                reestablish_rlc_r16_present = false;
  bool                rlc_cfg_r16_present         = false;
  bool                mac_lc_ch_cfg_r16_present   = false;
  bh_lc_ch_id_r16_c   bh_lc_ch_id_r16;
  fixed_bitstring<16> bh_rlc_ch_id_r16;
  rlc_cfg_c           rlc_cfg_r16;
  lc_ch_cfg_s         mac_lc_ch_cfg_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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
    typedef enumerated<types_opts> types;

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

// CarrierState-r17 ::= CHOICE
struct carrier_state_r17_c {
  struct types_opts {
    enum options { de_activ_r17, active_bwp_r17, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  carrier_state_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& active_bwp_r17()
  {
    assert_choice_type(types::active_bwp_r17, type_, "CarrierState-r17");
    return c;
  }
  const uint8_t& active_bwp_r17() const
  {
    assert_choice_type(types::active_bwp_r17, type_, "CarrierState-r17");
    return c;
  }
  void     set_de_activ_r17();
  uint8_t& set_active_bwp_r17();

private:
  types   type_;
  uint8_t c;
};

// CC-State-r17 ::= SEQUENCE
struct cc_state_r17_s {
  bool                dl_carrier_r17_present = false;
  bool                ul_carrier_r17_present = false;
  carrier_state_r17_c dl_carrier_r17;
  carrier_state_r17_c ul_carrier_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-CSIRS-Resource ::= SEQUENCE
struct cfra_csi_rs_res_s {
  using ra_occasion_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                ext    = false;
  uint8_t             csi_rs = 0;
  ra_occasion_list_l_ ra_occasion_list;
  uint8_t             ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-SSB-Resource ::= SEQUENCE
struct cfra_ssb_res_s {
  bool    ext           = false;
  uint8_t ssb           = 0;
  uint8_t ra_preamb_idx = 0;
  // ...
  // group 0
  bool     msg_a_pusch_res_idx_r16_present = false;
  uint16_t msg_a_pusch_res_idx_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA ::= SEQUENCE
struct cfra_s {
  struct occasions_s_ {
    struct ssb_per_rach_occasion_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<ssb_per_rach_occasion_opts> ssb_per_rach_occasion_e_;

    // member variables
    bool                     ssb_per_rach_occasion_present = false;
    rach_cfg_generic_s       rach_cfg_generic;
    ssb_per_rach_occasion_e_ ssb_per_rach_occasion;
  };
  struct res_c_ {
    struct ssb_s_ {
      using ssb_res_list_l_ = dyn_array<cfra_ssb_res_s>;

      // member variables
      ssb_res_list_l_ ssb_res_list;
      uint8_t         ra_ssb_occasion_mask_idx = 0;
    };
    struct csirs_s_ {
      using csirs_res_list_l_ = dyn_array<cfra_csi_rs_res_s>;

      // member variables
      csirs_res_list_l_ csirs_res_list;
      uint8_t           rsrp_thres_csi_rs = 0;
    };
    struct types_opts {
      enum options { ssb, csirs, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_c_() = default;
    res_c_(const res_c_& other);
    res_c_& operator=(const res_c_& other);
    ~res_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ssb_s_& ssb()
    {
      assert_choice_type(types::ssb, type_, "resources");
      return c.get<ssb_s_>();
    }
    csirs_s_& csirs()
    {
      assert_choice_type(types::csirs, type_, "resources");
      return c.get<csirs_s_>();
    }
    const ssb_s_& ssb() const
    {
      assert_choice_type(types::ssb, type_, "resources");
      return c.get<ssb_s_>();
    }
    const csirs_s_& csirs() const
    {
      assert_choice_type(types::csirs, type_, "resources");
      return c.get<csirs_s_>();
    }
    ssb_s_&   set_ssb();
    csirs_s_& set_csirs();

  private:
    types                             type_;
    choice_buffer_t<csirs_s_, ssb_s_> c;

    void destroy_();
  };

  // member variables
  bool         ext               = false;
  bool         occasions_present = false;
  occasions_s_ occasions;
  res_c_       res;
  // ...
  // group 0
  bool    total_nof_ra_preambs_present = false;
  uint8_t total_nof_ra_preambs         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-TwoStep-r16 ::= SEQUENCE
struct cfra_two_step_r16_s {
  struct occasions_two_step_ra_r16_s_ {
    struct ssb_per_rach_occasion_two_step_ra_r16_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<ssb_per_rach_occasion_two_step_ra_r16_opts> ssb_per_rach_occasion_two_step_ra_r16_e_;

    // member variables
    rach_cfg_generic_two_step_ra_r16_s       rach_cfg_generic_two_step_ra_r16;
    ssb_per_rach_occasion_two_step_ra_r16_e_ ssb_per_rach_occasion_two_step_ra_r16;
  };
  struct msg_a_trans_max_r16_opts {
    enum options { n1, n2, n4, n6, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<msg_a_trans_max_r16_opts> msg_a_trans_max_r16_e_;
  struct res_two_step_r16_s_ {
    using ssb_res_list_l_ = dyn_array<cfra_ssb_res_s>;

    // member variables
    ssb_res_list_l_ ssb_res_list;
    uint8_t         ra_ssb_occasion_mask_idx = 0;
  };

  // member variables
  bool                         ext                               = false;
  bool                         occasions_two_step_ra_r16_present = false;
  bool                         msg_a_trans_max_r16_present       = false;
  occasions_two_step_ra_r16_s_ occasions_two_step_ra_r16;
  msg_a_pusch_res_r16_s        msg_a_cfra_pusch_r16;
  msg_a_trans_max_r16_e_       msg_a_trans_max_r16;
  res_two_step_r16_s_          res_two_step_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// FrequencyInfoDL ::= SEQUENCE
struct freq_info_dl_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                         ext                       = false;
  bool                         absolute_freq_ssb_present = false;
  uint32_t                     absolute_freq_ssb         = 0;
  multi_freq_band_list_nr_l    freq_band_list;
  uint32_t                     absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_ scs_specific_carrier_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoUL ::= SEQUENCE
struct freq_info_ul_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                         ext                           = false;
  bool                         absolute_freq_point_a_present = false;
  bool                         add_spec_emission_present     = false;
  bool                         p_max_present                 = false;
  bool                         freq_shift7p5khz_present      = false;
  multi_freq_band_list_nr_l    freq_band_list;
  uint32_t                     absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_ scs_specific_carrier_list;
  uint8_t                      add_spec_emission = 0;
  int8_t                       p_max             = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkConfigCommon ::= SEQUENCE
struct dl_cfg_common_s {
  bool            ext                  = false;
  bool            freq_info_dl_present = false;
  bool            init_dl_bwp_present  = false;
  freq_info_dl_s  freq_info_dl;
  bwp_dl_common_s init_dl_bwp;
  // ...
  // group 0
  copy_ptr<bwp_dl_common_s> init_dl_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfig-v1700 ::= SEQUENCE
struct high_speed_cfg_v1700_s {
  bool ext                                    = false;
  bool high_speed_meas_ca_scell_r17_present   = false;
  bool high_speed_meas_inter_freq_r17_present = false;
  bool high_speed_demod_ca_scell_r17_present  = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T-ReassemblyExt-r17 ::= ENUMERATED
struct t_reassembly_ext_r17_opts {
  enum options { ms210, ms220, ms340, ms350, ms550, ms1100, ms1650, ms2200, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_reassembly_ext_r17_opts> t_reassembly_ext_r17_e;

// T-StatusProhibit-v1610 ::= ENUMERATED
struct t_status_prohibit_v1610_opts {
  enum options { ms1, ms2, ms3, ms4, spare4, spare3, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<t_status_prohibit_v1610_opts> t_status_prohibit_v1610_e;

// UplinkConfigCommon ::= SEQUENCE
struct ul_cfg_common_s {
  bool               freq_info_ul_present = false;
  bool               init_ul_bwp_present  = false;
  freq_info_ul_s     freq_info_ul;
  bwp_ul_common_s    init_ul_bwp;
  time_align_timer_e dummy;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfigCommon-v1700 ::= SEQUENCE
struct ul_cfg_common_v1700_s {
  bool            init_ul_bwp_red_cap_r17_present = false;
  bwp_ul_common_s init_ul_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPS-UplinkPowerConfig-r16 ::= SEQUENCE
struct daps_ul_pwr_cfg_r16_s {
  struct ul_pwr_sharing_daps_mode_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ul_pwr_sharing_daps_mode_r16_opts> ul_pwr_sharing_daps_mode_r16_e_;

  // member variables
  int8_t                          p_daps_source_r16 = -30;
  int8_t                          p_daps_target_r16 = -30;
  ul_pwr_sharing_daps_mode_r16_e_ ul_pwr_sharing_daps_mode_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-AM-RLC-v1610 ::= SEQUENCE
struct dl_am_rlc_v1610_s {
  bool                      ext                             = false;
  bool                      t_status_prohibit_v1610_present = false;
  t_status_prohibit_v1610_e t_status_prohibit_v1610;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-AM-RLC-v1700 ::= SEQUENCE
struct dl_am_rlc_v1700_s {
  bool                   t_reassembly_ext_r17_present = false;
  t_reassembly_ext_r17_e t_reassembly_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-UM-RLC-v1700 ::= SEQUENCE
struct dl_um_rlc_v1700_s {
  bool                   t_reassembly_ext_r17_present = false;
  t_reassembly_ext_r17_e t_reassembly_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraBandCC-Combination-r17 ::= SEQUENCE (SIZE (1..32)) OF CC-State-r17
using intra_band_cc_combination_r17_l = dyn_array<cc_state_r17_s>;

// MPE-Config-FR2-r16 ::= SEQUENCE
struct mpe_cfg_fr2_r16_s {
  struct mpe_prohibit_timer_r16_opts {
    enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mpe_prohibit_timer_r16_opts> mpe_prohibit_timer_r16_e_;
  struct mpe_thres_r16_opts {
    enum options { db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mpe_thres_r16_opts> mpe_thres_r16_e_;

  // member variables
  mpe_prohibit_timer_r16_e_ mpe_prohibit_timer_r16;
  mpe_thres_r16_e_          mpe_thres_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MPE-Config-FR2-r17 ::= SEQUENCE
struct mpe_cfg_fr2_r17_s {
  struct mpe_prohibit_timer_r17_opts {
    enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mpe_prohibit_timer_r17_opts> mpe_prohibit_timer_r17_e_;
  struct mpe_thres_r17_opts {
    enum options { db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mpe_thres_r17_opts> mpe_thres_r17_e_;

  // member variables
  bool                      ext = false;
  mpe_prohibit_timer_r17_e_ mpe_prohibit_timer_r17;
  mpe_thres_r17_e_          mpe_thres_r17;
  uint8_t                   nof_n_r17 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdsch_harq_ack_codebook_list_r16_item_opts {
  enum options { semi_static, dyn, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<pdsch_harq_ack_codebook_list_r16_item_opts> pdsch_harq_ack_codebook_list_r16_item_e_;

// PDSCH-HARQ-ACK-CodebookList-r16 ::= SEQUENCE (SIZE (1..2)) OF PDSCH-HARQ-ACK-CodebookList-r16-item
using pdsch_harq_ack_codebook_list_r16_l = bounded_array<pdsch_harq_ack_codebook_list_r16_item_e_, 2>;

// RACH-ConfigDedicated ::= SEQUENCE
struct rach_cfg_ded_s {
  bool                ext                       = false;
  bool                cfra_present              = false;
  bool                ra_prioritization_present = false;
  cfra_s              cfra;
  ra_prioritization_s ra_prioritization;
  // ...
  // group 0
  copy_ptr<ra_prioritization_s> ra_prioritization_two_step_r16;
  copy_ptr<cfra_two_step_r16_s> cfra_two_step_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PathSwitchConfig-r17 ::= SEQUENCE
struct sl_path_switch_cfg_r17_s {
  struct t420_r17_opts {
    enum options { ms50, ms100, ms150, ms200, ms500, ms1000, ms2000, ms10000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t420_r17_opts> t420_r17_e_;

  // member variables
  bool                ext = false;
  fixed_bitstring<24> target_relay_ue_id_r17;
  t420_r17_e_         t420_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestToAddMod ::= SEQUENCE
struct sched_request_to_add_mod_s {
  struct sr_prohibit_timer_opts {
    enum options { ms1, ms2, ms4, ms8, ms16, ms32, ms64, ms128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sr_prohibit_timer_opts> sr_prohibit_timer_e_;
  struct sr_trans_max_opts {
    enum options { n4, n8, n16, n32, n64, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sr_trans_max_opts> sr_trans_max_e_;

  // member variables
  bool                 sr_prohibit_timer_present = false;
  uint8_t              sched_request_id          = 0;
  sr_prohibit_timer_e_ sr_prohibit_timer;
  sr_trans_max_e_      sr_trans_max;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestToAddModExt-v1700 ::= SEQUENCE
struct sched_request_to_add_mod_ext_v1700_s {
  struct sr_prohibit_timer_v1700_opts {
    enum options {
      ms192,
      ms256,
      ms320,
      ms384,
      ms448,
      ms512,
      ms576,
      ms640,
      ms1082,
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
  typedef enumerated<sr_prohibit_timer_v1700_opts> sr_prohibit_timer_v1700_e_;

  // member variables
  bool                       sr_prohibit_timer_v1700_present = false;
  sr_prohibit_timer_v1700_e_ sr_prohibit_timer_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfigCommon ::= SEQUENCE
struct serving_cell_cfg_common_s {
  struct n_timing_advance_offset_opts {
    enum options { n0, n25600, n39936, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<n_timing_advance_offset_opts> n_timing_advance_offset_e_;
  struct ssb_positions_in_burst_c_ {
    struct types_opts {
      enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ssb_positions_in_burst_c_() = default;
    ssb_positions_in_burst_c_(const ssb_positions_in_burst_c_& other);
    ssb_positions_in_burst_c_& operator=(const ssb_positions_in_burst_c_& other);
    ~ssb_positions_in_burst_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& short_bitmap()
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<8>& medium_bitmap()
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<64>& long_bitmap()
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64>>();
    }
    const fixed_bitstring<4>& short_bitmap() const
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<8>& medium_bitmap() const
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<64>& long_bitmap() const
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64>>();
    }
    fixed_bitstring<4>&  set_short_bitmap();
    fixed_bitstring<8>&  set_medium_bitmap();
    fixed_bitstring<64>& set_long_bitmap();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<64>> c;

    void destroy_();
  };
  struct ssb_periodicity_serving_cell_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ssb_periodicity_serving_cell_opts> ssb_periodicity_serving_cell_e_;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_type_a_position_opts> dmrs_type_a_position_e_;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  struct ch_access_mode_r16_c_ {
    struct types_opts {
      enum options { dyn, semi_static, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ch_access_mode_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    semi_static_ch_access_cfg_r16_s& semi_static()
    {
      assert_choice_type(types::semi_static, type_, "channelAccessMode-r16");
      return c;
    }
    const semi_static_ch_access_cfg_r16_s& semi_static() const
    {
      assert_choice_type(types::semi_static, type_, "channelAccessMode-r16");
      return c;
    }
    void                             set_dyn();
    semi_static_ch_access_cfg_r16_s& set_semi_static();

  private:
    types                           type_;
    semi_static_ch_access_cfg_r16_s c;
  };
  struct discovery_burst_win_len_r16_opts {
    enum options { ms0dot5, ms1, ms2, ms3, ms4, ms5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<discovery_burst_win_len_r16_opts> discovery_burst_win_len_r16_e_;
  struct discovery_burst_win_len_r17_opts {
    enum options { ms0dot125, ms0dot25, ms0dot5, ms0dot75, ms1, ms1dot25, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<discovery_burst_win_len_r17_opts> discovery_burst_win_len_r17_e_;
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

  // member variables
  bool                                          ext                                  = false;
  bool                                          pci_present                          = false;
  bool                                          dl_cfg_common_present                = false;
  bool                                          ul_cfg_common_present                = false;
  bool                                          supplementary_ul_cfg_present         = false;
  bool                                          n_timing_advance_offset_present      = false;
  bool                                          ssb_positions_in_burst_present       = false;
  bool                                          ssb_periodicity_serving_cell_present = false;
  bool                                          lte_crs_to_match_around_present      = false;
  bool                                          ssb_subcarrier_spacing_present       = false;
  bool                                          tdd_ul_dl_cfg_common_present         = false;
  uint16_t                                      pci                                  = 0;
  dl_cfg_common_s                               dl_cfg_common;
  ul_cfg_common_s                               ul_cfg_common;
  ul_cfg_common_s                               supplementary_ul_cfg;
  n_timing_advance_offset_e_                    n_timing_advance_offset;
  ssb_positions_in_burst_c_                     ssb_positions_in_burst;
  ssb_periodicity_serving_cell_e_               ssb_periodicity_serving_cell;
  dmrs_type_a_position_e_                       dmrs_type_a_position;
  setup_release_c<rate_match_pattern_lte_crs_s> lte_crs_to_match_around;
  rate_match_pattern_to_add_mod_list_l_         rate_match_pattern_to_add_mod_list;
  rate_match_pattern_to_release_list_l_         rate_match_pattern_to_release_list;
  subcarrier_spacing_e                          ssb_subcarrier_spacing;
  tdd_ul_dl_cfg_common_s                        tdd_ul_dl_cfg_common;
  int8_t                                        ss_pbch_block_pwr = -60;
  // ...
  // group 0
  bool                            discovery_burst_win_len_r16_present = false;
  bool                            ssb_position_qcl_r16_present        = false;
  copy_ptr<ch_access_mode_r16_c_> ch_access_mode_r16;
  discovery_burst_win_len_r16_e_  discovery_burst_win_len_r16;
  ssb_position_qcl_relation_r16_e ssb_position_qcl_r16;
  copy_ptr<high_speed_cfg_r16_s>  high_speed_cfg_r16;
  // group 1
  bool                               ch_access_mode2_r17_present         = false;
  bool                               discovery_burst_win_len_r17_present = false;
  bool                               ssb_position_qcl_r17_present        = false;
  copy_ptr<high_speed_cfg_v1700_s>   high_speed_cfg_v1700;
  discovery_burst_win_len_r17_e_     discovery_burst_win_len_r17;
  ssb_position_qcl_relation_r17_e    ssb_position_qcl_r17;
  copy_ptr<high_speed_cfg_fr2_r17_s> high_speed_cfg_fr2_r17;
  copy_ptr<ul_cfg_common_v1700_s>    ul_cfg_common_v1700;
  copy_ptr<ntn_cfg_r17_s>            ntn_cfg_r17;
  // group 2
  copy_ptr<feature_priorities_r17_s_> feature_priorities_r17;
  // group 3
  bool ra_ch_access_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAG ::= SEQUENCE
struct tag_s {
  bool               ext    = false;
  uint8_t            tag_id = 0;
  time_align_timer_e time_align_timer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DCP-Config-r16 ::= SEQUENCE
struct d_cp_cfg_r16_s {
  bool     ps_wake_up_r16_present               = false;
  bool     ps_tx_periodic_l1_rsrp_r16_present   = false;
  bool     ps_tx_other_periodic_csi_r16_present = false;
  uint32_t ps_rnti_r16                          = 0;
  uint8_t  ps_offset_r16                        = 1;
  uint8_t  size_dci_2_6_r16                     = 1;
  uint8_t  ps_position_dci_2_6_r16              = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Config ::= SEQUENCE
struct drx_cfg_s {
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
    typedef enumerated<milli_seconds_opts> milli_seconds_e_;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
  struct drx_inactivity_timer_opts {
    enum options {
      ms0,
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
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
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
  typedef enumerated<drx_inactivity_timer_opts> drx_inactivity_timer_e_;
  struct drx_retx_timer_dl_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
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
  typedef enumerated<drx_retx_timer_dl_opts> drx_retx_timer_dl_e_;
  struct drx_retx_timer_ul_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
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
  typedef enumerated<drx_retx_timer_ul_opts> drx_retx_timer_ul_e_;
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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<drx_short_cycle_opts> drx_short_cycle_e_;

    // member variables
    drx_short_cycle_e_ drx_short_cycle;
    uint8_t            drx_short_cycle_timer = 1;
  };

  // member variables
  bool                           short_drx_present = false;
  drx_on_dur_timer_c_            drx_on_dur_timer;
  drx_inactivity_timer_e_        drx_inactivity_timer;
  uint8_t                        drx_harq_rtt_timer_dl = 0;
  uint8_t                        drx_harq_rtt_timer_ul = 0;
  drx_retx_timer_dl_e_           drx_retx_timer_dl;
  drx_retx_timer_ul_e_           drx_retx_timer_ul;
  drx_long_cycle_start_offset_c_ drx_long_cycle_start_offset;
  short_drx_s_                   short_drx;
  uint8_t                        drx_slot_offset = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-ConfigExt-v1700 ::= SEQUENCE
struct drx_cfg_ext_v1700_s {
  uint16_t drx_harq_rtt_timer_dl_r17 = 0;
  uint16_t drx_harq_rtt_timer_ul_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-ConfigSL-r17 ::= SEQUENCE
struct drx_cfg_sl_r17_s {
  struct drx_retx_timer_sl_r17_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
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
  typedef enumerated<drx_retx_timer_sl_r17_opts> drx_retx_timer_sl_r17_e_;

  // member variables
  uint8_t                  drx_harq_rtt_timer_sl_r17 = 0;
  drx_retx_timer_sl_r17_e_ drx_retx_timer_sl_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-ConfigSecondaryGroup-r16 ::= SEQUENCE
struct drx_cfg_secondary_group_r16_s {
  struct drx_on_dur_timer_r16_c_ {
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
    typedef enumerated<milli_seconds_opts> milli_seconds_e_;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    drx_on_dur_timer_r16_c_() = default;
    drx_on_dur_timer_r16_c_(const drx_on_dur_timer_r16_c_& other);
    drx_on_dur_timer_r16_c_& operator=(const drx_on_dur_timer_r16_c_& other);
    ~drx_on_dur_timer_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<milli_seconds_e_>();
    }
    uint8_t&          set_sub_milli_seconds();
    milli_seconds_e_& set_milli_seconds();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct drx_inactivity_timer_r16_opts {
    enum options {
      ms0,
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
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
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
  typedef enumerated<drx_inactivity_timer_r16_opts> drx_inactivity_timer_r16_e_;

  // member variables
  drx_on_dur_timer_r16_c_     drx_on_dur_timer_r16;
  drx_inactivity_timer_r16_e_ drx_inactivity_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataInactivityTimer ::= ENUMERATED
struct data_inactivity_timer_opts {
  enum options { s1, s2, s3, s5, s7, s10, s15, s20, s40, s50, s60, s80, s100, s120, s150, s180, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<data_inactivity_timer_opts> data_inactivity_timer_e;

// DeactivatedSCG-Config-r17 ::= SEQUENCE
struct deactiv_scg_cfg_r17_s {
  bool ext             = false;
  bool bfd_and_rlm_r17 = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GoodServingCellEvaluation-r17 ::= SEQUENCE
struct good_serving_cell_eval_r17_s {
  struct offset_r17_opts {
    enum options { db2, db4, db6, db8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<offset_r17_opts> offset_r17_e_;

  // member variables
  bool          offset_r17_present = false;
  offset_r17_e_ offset_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraBandCC-CombinationReqList-r17 ::= SEQUENCE
struct intra_band_cc_combination_req_list_r17_s {
  using serv_cell_idx_list_r17_l_  = bounded_array<uint8_t, 32>;
  using cc_combination_list_r17_l_ = dyn_array<intra_band_cc_combination_r17_l>;

  // member variables
  serv_cell_idx_list_r17_l_  serv_cell_idx_list_r17;
  cc_combination_list_r17_l_ cc_combination_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-RNTI-SpecificConfig-r17 ::= SEQUENCE
struct mbs_rnti_specific_cfg_r17_s {
  struct group_common_rnti_r17_c_ {
    struct types_opts {
      enum options { g_rnti, g_cs_rnti, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    group_common_rnti_r17_c_() = default;
    group_common_rnti_r17_c_(const group_common_rnti_r17_c_& other);
    group_common_rnti_r17_c_& operator=(const group_common_rnti_r17_c_& other);
    ~group_common_rnti_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t& g_rnti()
    {
      assert_choice_type(types::g_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    uint32_t& g_cs_rnti()
    {
      assert_choice_type(types::g_cs_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    const uint32_t& g_rnti() const
    {
      assert_choice_type(types::g_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    const uint32_t& g_cs_rnti() const
    {
      assert_choice_type(types::g_cs_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    uint32_t& set_g_rnti();
    uint32_t& set_g_cs_rnti();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct harq_feedback_enabler_multicast_r17_opts {
    enum options { dci_enabler, enabled, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<harq_feedback_enabler_multicast_r17_opts> harq_feedback_enabler_multicast_r17_e_;
  struct harq_feedback_option_multicast_r17_opts {
    enum options { ack_nack, nack_only, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<harq_feedback_option_multicast_r17_opts> harq_feedback_option_multicast_r17_e_;
  struct pdsch_aggregation_factor_r17_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_aggregation_factor_r17_opts> pdsch_aggregation_factor_r17_e_;

  // member variables
  bool                                   drx_cfg_ptm_r17_present                     = false;
  bool                                   harq_feedback_enabler_multicast_r17_present = false;
  bool                                   harq_feedback_option_multicast_r17_present  = false;
  bool                                   pdsch_aggregation_factor_r17_present        = false;
  uint8_t                                mbs_rnti_specific_cfg_id_r17                = 0;
  group_common_rnti_r17_c_               group_common_rnti_r17;
  setup_release_c<drx_cfg_ptm_r17_s>     drx_cfg_ptm_r17;
  harq_feedback_enabler_multicast_r17_e_ harq_feedback_enabler_multicast_r17;
  harq_feedback_option_multicast_r17_e_  harq_feedback_option_multicast_r17;
  pdsch_aggregation_factor_r17_e_        pdsch_aggregation_factor_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastConfig-r17 ::= SEQUENCE
struct multicast_cfg_r17_s {
  struct type1_codebook_generation_mode_r17_opts {
    enum options { mode1, mode2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<type1_codebook_generation_mode_r17_opts> type1_codebook_generation_mode_r17_e_;

  // member variables
  bool pdsch_harq_ack_codebook_list_multicast_r17_present = false;
  bool type1_codebook_generation_mode_r17_present         = false;
  setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>
                                        pdsch_harq_ack_codebook_list_multicast_r17;
  type1_codebook_generation_mode_r17_e_ type1_codebook_generation_mode_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastRLC-BearerConfig-r17 ::= SEQUENCE
struct multicast_rlc_bearer_cfg_r17_s {
  bool     is_ptm_entity_r17_present   = false;
  uint16_t served_mbs_radio_bearer_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCA-CombIndicator-r16 ::= SEQUENCE
struct pdcch_blind_detection_ca_comb_ind_r16_s {
  uint8_t pdcch_blind_detection_ca1_r16 = 1;
  uint8_t pdcch_blind_detection_ca2_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCA-CombIndicator-r17 ::= SEQUENCE
struct pdcch_blind_detection_ca_comb_ind_r17_s {
  bool    pdcch_blind_detection_ca1_r17_present = false;
  bool    pdcch_blind_detection_ca2_r17_present = false;
  uint8_t pdcch_blind_detection_ca1_r17         = 1;
  uint8_t pdcch_blind_detection_ca2_r17         = 1;
  uint8_t pdcch_blind_detection_ca3_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-HARQ-ACK-EnhType3-r17 ::= SEQUENCE
struct pdsch_harq_ack_enh_type3_r17_s {
  struct applicable_r17_c_ {
    using per_cc_l_   = bounded_array<uint8_t, 32>;
    using per_harq_l_ = bounded_array<fixed_bitstring<16>, 32>;
    struct types_opts {
      enum options { per_cc, per_harq, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    applicable_r17_c_() = default;
    applicable_r17_c_(const applicable_r17_c_& other);
    applicable_r17_c_& operator=(const applicable_r17_c_& other);
    ~applicable_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    per_cc_l_& per_cc()
    {
      assert_choice_type(types::per_cc, type_, "applicable-r17");
      return c.get<per_cc_l_>();
    }
    per_harq_l_& per_harq()
    {
      assert_choice_type(types::per_harq, type_, "applicable-r17");
      return c.get<per_harq_l_>();
    }
    const per_cc_l_& per_cc() const
    {
      assert_choice_type(types::per_cc, type_, "applicable-r17");
      return c.get<per_cc_l_>();
    }
    const per_harq_l_& per_harq() const
    {
      assert_choice_type(types::per_harq, type_, "applicable-r17");
      return c.get<per_harq_l_>();
    }
    per_cc_l_&   set_per_cc();
    per_harq_l_& set_per_harq();

  private:
    types                                   type_;
    choice_buffer_t<per_cc_l_, per_harq_l_> c;

    void destroy_();
  };

  // member variables
  bool              ext                                      = false;
  bool              pdsch_harq_ack_enh_type3_ndi_r17_present = false;
  bool              pdsch_harq_ack_enh_type3_cbg_r17_present = false;
  uint8_t           pdsch_harq_ack_enh_type3_idx_r17         = 0;
  applicable_r17_c_ applicable_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PHR-Config ::= SEQUENCE
struct phr_cfg_s {
  struct phr_periodic_timer_opts {
    enum options { sf10, sf20, sf50, sf100, sf200, sf500, sf1000, infinity, nulltype } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<phr_periodic_timer_opts> phr_periodic_timer_e_;
  struct phr_prohibit_timer_opts {
    enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<phr_prohibit_timer_opts> phr_prohibit_timer_e_;
  struct phr_tx_pwr_factor_change_opts {
    enum options { db1, db3, db6, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<phr_tx_pwr_factor_change_opts> phr_tx_pwr_factor_change_e_;
  struct phr_mode_other_cg_opts {
    enum options { real, virtual_value, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<phr_mode_other_cg_opts> phr_mode_other_cg_e_;

  // member variables
  bool                        ext = false;
  phr_periodic_timer_e_       phr_periodic_timer;
  phr_prohibit_timer_e_       phr_prohibit_timer;
  phr_tx_pwr_factor_change_e_ phr_tx_pwr_factor_change;
  bool                        multiple_phr         = false;
  bool                        dummy                = false;
  bool                        phr_type2_other_cell = false;
  phr_mode_other_cg_e_        phr_mode_other_cg;
  // ...
  // group 0
  copy_ptr<setup_release_c<mpe_cfg_fr2_r16_s>> mpe_report_fr2_r16;
  // group 1
  bool                                         two_phr_mode_r17_present = false;
  copy_ptr<setup_release_c<mpe_cfg_fr2_r17_s>> mpe_report_fr2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config-v1610 ::= SEQUENCE
struct rlc_cfg_v1610_s {
  dl_am_rlc_v1610_s dl_am_rlc_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config-v1700 ::= SEQUENCE
struct rlc_cfg_v1700_s {
  dl_am_rlc_v1700_s dl_am_rlc_v1700;
  dl_um_rlc_v1700_s dl_um_rlc_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLF-TimersAndConstants ::= SEQUENCE
struct rlf_timers_and_consts_s {
  struct t310_opts {
    enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, ms4000, ms6000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t310_opts> t310_e_;
  struct n310_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n310_opts> n310_e_;
  struct n311_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n311_opts> n311_e_;
  struct t311_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t311_opts> t311_e_;

  // member variables
  bool    ext = false;
  t310_e_ t310;
  n310_e_ n310;
  n311_e_ n311;
  // ...
  // group 0
  t311_e_ t311;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReconfigurationWithSync ::= SEQUENCE
struct recfg_with_sync_s {
  struct t304_opts {
    enum options { ms50, ms100, ms150, ms200, ms500, ms1000, ms2000, ms10000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t304_opts> t304_e_;
  struct rach_cfg_ded_c_ {
    struct types_opts {
      enum options { ul, supplementary_ul, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    rach_cfg_ded_c_() = default;
    rach_cfg_ded_c_(const rach_cfg_ded_c_& other);
    rach_cfg_ded_c_& operator=(const rach_cfg_ded_c_& other);
    ~rach_cfg_ded_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rach_cfg_ded_s& ul()
    {
      assert_choice_type(types::ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    rach_cfg_ded_s& supplementary_ul()
    {
      assert_choice_type(types::supplementary_ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    const rach_cfg_ded_s& ul() const
    {
      assert_choice_type(types::ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    const rach_cfg_ded_s& supplementary_ul() const
    {
      assert_choice_type(types::supplementary_ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    rach_cfg_ded_s& set_ul();
    rach_cfg_ded_s& set_supplementary_ul();

  private:
    types                           type_;
    choice_buffer_t<rach_cfg_ded_s> c;

    void destroy_();
  };

  // member variables
  bool                      ext                        = false;
  bool                      sp_cell_cfg_common_present = false;
  bool                      rach_cfg_ded_present       = false;
  serving_cell_cfg_common_s sp_cell_cfg_common;
  uint32_t                  new_ue_id = 0;
  t304_e_                   t304;
  rach_cfg_ded_c_           rach_cfg_ded;
  // ...
  // group 0
  copy_ptr<ssb_mtc_s> smtc;
  // group 1
  copy_ptr<daps_ul_pwr_cfg_r16_s> daps_ul_pwr_cfg_r16;
  // group 2
  copy_ptr<sl_path_switch_cfg_r17_s> sl_path_switch_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestConfig ::= SEQUENCE
struct sched_request_cfg_s {
  using sched_request_to_add_mod_list_l_ = dyn_array<sched_request_to_add_mod_s>;
  using sched_request_to_release_list_l_ = bounded_array<uint8_t, 8>;

  // member variables
  sched_request_to_add_mod_list_l_ sched_request_to_add_mod_list;
  sched_request_to_release_list_l_ sched_request_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestConfig-v1700 ::= SEQUENCE
struct sched_request_cfg_v1700_s {
  using sched_request_to_add_mod_list_ext_v1700_l_ = dyn_array<sched_request_to_add_mod_ext_v1700_s>;

  // member variables
  sched_request_to_add_mod_list_ext_v1700_l_ sched_request_to_add_mod_list_ext_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAG-Config ::= SEQUENCE
struct tag_cfg_s {
  using tag_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using tag_to_add_mod_list_l_ = dyn_array<tag_s>;

  // member variables
  tag_to_release_list_l_ tag_to_release_list;
  tag_to_add_mod_list_l_ tag_to_add_mod_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAR-Config-r17 ::= SEQUENCE
struct tar_cfg_r17_s {
  struct offset_thres_ta_r17_opts {
    enum options {
      ms0dot5,
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms7,
      ms8,
      ms9,
      ms10,
      ms11,
      ms12,
      ms13,
      ms14,
      ms15,
      spare13,
      spare12,
      spare11,
      spare10,
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
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<offset_thres_ta_r17_opts> offset_thres_ta_r17_e_;

  // member variables
  bool                   ext                           = false;
  bool                   offset_thres_ta_r17_present   = false;
  bool                   timing_advance_sr_r17_present = false;
  offset_thres_ta_r17_e_ offset_thres_ta_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-ResourceConfig-r17 ::= SEQUENCE
struct iab_res_cfg_r17_s {
  using slot_list_r17_l_ = dyn_array<uint16_t>;
  struct periodicity_slot_list_r17_opts {
    enum options { ms0p5, ms0p625, ms1, ms1p25, ms2, ms2p5, ms5, ms10, ms20, ms40, ms80, ms160, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<periodicity_slot_list_r17_opts> periodicity_slot_list_r17_e_;

  // member variables
  bool                         ext                                      = false;
  bool                         periodicity_slot_list_r17_present        = false;
  bool                         slot_list_subcarrier_spacing_r17_present = false;
  uint32_t                     iab_res_cfg_id_r17                       = 0;
  slot_list_r17_l_             slot_list_r17;
  periodicity_slot_list_r17_e_ periodicity_slot_list_r17;
  subcarrier_spacing_e         slot_list_subcarrier_spacing_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-CellGroupConfig ::= SEQUENCE
struct mac_cell_group_cfg_s {
  using g_rnti_cfg_to_add_mod_list_r17_l_    = dyn_array<mbs_rnti_specific_cfg_r17_s>;
  using g_rnti_cfg_to_release_list_r17_l_    = bounded_array<uint8_t, 16>;
  using g_cs_rnti_cfg_to_add_mod_list_r17_l_ = dyn_array<mbs_rnti_specific_cfg_r17_s>;
  using g_cs_rnti_cfg_to_release_list_r17_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                       ext                       = false;
  bool                       drx_cfg_present           = false;
  bool                       sched_request_cfg_present = false;
  bool                       bsr_cfg_present           = false;
  bool                       tag_cfg_present           = false;
  bool                       phr_cfg_present           = false;
  setup_release_c<drx_cfg_s> drx_cfg;
  sched_request_cfg_s        sched_request_cfg;
  bsr_cfg_s                  bsr_cfg;
  tag_cfg_s                  tag_cfg;
  setup_release_c<phr_cfg_s> phr_cfg;
  bool                       skip_ul_tx_dyn = false;
  // ...
  // group 0
  bool                                               csi_mask_present = false;
  bool                                               csi_mask         = false;
  copy_ptr<setup_release_c<data_inactivity_timer_e>> data_inactivity_timer;
  // group 1
  bool                                                     use_pre_bsr_r16_present                = false;
  bool                                                     sched_request_id_lbt_scell_r16_present = false;
  bool                                                     lch_based_prioritization_r16_present   = false;
  bool                                                     sched_request_id_bfr_scell_r16_present = false;
  uint8_t                                                  sched_request_id_lbt_scell_r16         = 0;
  uint8_t                                                  sched_request_id_bfr_scell_r16         = 0;
  copy_ptr<setup_release_c<drx_cfg_secondary_group_r16_s>> drx_cfg_secondary_group_r16;
  // group 2
  bool enhanced_skip_ul_tx_dyn_r16_present = false;
  bool enhanced_skip_ul_tx_cfg_r16_present = false;
  // group 3
  bool                                           intra_cg_prioritization_r17_present               = false;
  bool                                           sched_request_id_bfr_r17_present                  = false;
  bool                                           sched_request_id_bfr2_r17_present                 = false;
  bool                                           allow_csi_srs_tx_multicast_drx_active_r17_present = false;
  copy_ptr<setup_release_c<drx_cfg_sl_r17_s>>    drx_cfg_sl_r17;
  copy_ptr<setup_release_c<drx_cfg_ext_v1700_s>> drx_cfg_ext_v1700;
  uint8_t                                        sched_request_id_bfr_r17  = 0;
  uint8_t                                        sched_request_id_bfr2_r17 = 0;
  copy_ptr<sched_request_cfg_v1700_s>            sched_request_cfg_v1700;
  copy_ptr<setup_release_c<tar_cfg_r17_s>>       tar_cfg_r17;
  copy_ptr<g_rnti_cfg_to_add_mod_list_r17_l_>    g_rnti_cfg_to_add_mod_list_r17;
  copy_ptr<g_rnti_cfg_to_release_list_r17_l_>    g_rnti_cfg_to_release_list_r17;
  copy_ptr<g_cs_rnti_cfg_to_add_mod_list_r17_l_> g_cs_rnti_cfg_to_add_mod_list_r17;
  copy_ptr<g_cs_rnti_cfg_to_release_list_r17_l_> g_cs_rnti_cfg_to_release_list_r17;
  bool                                           allow_csi_srs_tx_multicast_drx_active_r17 = false;
  // group 4
  bool    sched_request_id_pos_mg_request_r17_present = false;
  bool    drx_last_tx_ul_r17_present                  = false;
  uint8_t sched_request_id_pos_mg_request_r17         = 0;
  // group 5
  bool pos_mg_request_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysicalCellGroupConfig ::= SEQUENCE
struct phys_cell_group_cfg_s {
  struct pdsch_harq_ack_codebook_opts {
    enum options { semi_static, dyn, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pdsch_harq_ack_codebook_opts> pdsch_harq_ack_codebook_e_;
  struct xscale_opts {
    enum options { db0, db6, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<xscale_opts> xscale_e_;
  struct harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_opts>
      harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e_;
  struct harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_opts>
      harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e_;
  struct pdsch_harq_ack_codebook_secondary_pucch_group_r16_opts {
    enum options { semi_static, dyn, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pdsch_harq_ack_codebook_secondary_pucch_group_r16_opts>
      pdsch_harq_ack_codebook_secondary_pucch_group_r16_e_;
  struct nrdc_p_cmode_fr1_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrdc_p_cmode_fr1_r16_opts> nrdc_p_cmode_fr1_r16_e_;
  struct nrdc_p_cmode_fr2_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrdc_p_cmode_fr2_r16_opts> nrdc_p_cmode_fr2_r16_e_;
  struct dl_assign_idx_dci_1_2_r16_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dl_assign_idx_dci_1_2_r16_opts> dl_assign_idx_dci_1_2_r16_e_;
  struct ack_nack_feedback_mode_r16_opts {
    enum options { joint, separate, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ack_nack_feedback_mode_r16_opts> ack_nack_feedback_mode_r16_e_;
  using pdsch_harq_ack_enh_type3_to_add_mod_list_r17_l_           = dyn_array<pdsch_harq_ack_enh_type3_r17_s>;
  using pdsch_harq_ack_enh_type3_to_release_list_r17_l_           = bounded_array<uint8_t, 8>;
  using pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17_l_ = dyn_array<pdsch_harq_ack_enh_type3_r17_s>;
  using pdsch_harq_ack_enh_type3_secondary_to_release_list_r17_l_ = bounded_array<uint8_t, 8>;
  using pucch_s_scell_pattern_r17_l_                              = dyn_array<uint8_t>;
  using pucch_s_scell_pattern_secondary_pucch_group_r17_l_        = dyn_array<uint8_t>;

  // member variables
  bool                                         ext                                     = false;
  bool                                         harq_ack_spatial_bundling_pucch_present = false;
  bool                                         harq_ack_spatial_bundling_pusch_present = false;
  bool                                         p_nr_fr1_present                        = false;
  bool                                         tpc_srs_rnti_present                    = false;
  bool                                         tpc_pucch_rnti_present                  = false;
  bool                                         tpc_pusch_rnti_present                  = false;
  bool                                         sp_csi_rnti_present                     = false;
  bool                                         cs_rnti_present                         = false;
  int8_t                                       p_nr_fr1                                = -30;
  pdsch_harq_ack_codebook_e_                   pdsch_harq_ack_codebook;
  uint32_t                                     tpc_srs_rnti   = 0;
  uint32_t                                     tpc_pucch_rnti = 0;
  uint32_t                                     tpc_pusch_rnti = 0;
  uint32_t                                     sp_csi_rnti    = 0;
  setup_release_c<integer<uint32_t, 0, 65535>> cs_rnti;
  // ...
  // group 0
  bool     mcs_c_rnti_present = false;
  bool     p_ue_fr1_present   = false;
  uint32_t mcs_c_rnti         = 0;
  int8_t   p_ue_fr1           = -30;
  // group 1
  bool      xscale_present = false;
  xscale_e_ xscale;
  // group 2
  copy_ptr<setup_release_c<integer<uint8_t, 1, 15>>> pdcch_blind_detection;
  // group 3
  bool                                      harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present = false;
  bool                                      harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present = false;
  bool                                      pdsch_harq_ack_codebook_secondary_pucch_group_r16_present         = false;
  bool                                      p_nr_fr2_r16_present                                              = false;
  bool                                      p_ue_fr2_r16_present                                              = false;
  bool                                      nrdc_p_cmode_fr1_r16_present                                      = false;
  bool                                      nrdc_p_cmode_fr2_r16_present                                      = false;
  bool                                      pdsch_harq_ack_codebook_r16_present                               = false;
  bool                                      nfi_total_dai_included_r16_present                                = false;
  bool                                      ul_total_dai_included_r16_present                                 = false;
  bool                                      pdsch_harq_ack_one_shot_feedback_r16_present                      = false;
  bool                                      pdsch_harq_ack_one_shot_feedback_ndi_r16_present                  = false;
  bool                                      pdsch_harq_ack_one_shot_feedback_cbg_r16_present                  = false;
  bool                                      dl_assign_idx_dci_0_2_r16_present                                 = false;
  bool                                      dl_assign_idx_dci_1_2_r16_present                                 = false;
  bool                                      ack_nack_feedback_mode_r16_present                                = false;
  bool                                      bd_factor_r_r16_present                                           = false;
  copy_ptr<setup_release_c<d_cp_cfg_r16_s>> dcp_cfg_r16;
  harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e_
      harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16;
  harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e_
                                                       harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16;
  pdsch_harq_ack_codebook_secondary_pucch_group_r16_e_ pdsch_harq_ack_codebook_secondary_pucch_group_r16;
  int8_t                                               p_nr_fr2_r16 = -30;
  int8_t                                               p_ue_fr2_r16 = -30;
  nrdc_p_cmode_fr1_r16_e_                              nrdc_p_cmode_fr1_r16;
  nrdc_p_cmode_fr2_r16_e_                              nrdc_p_cmode_fr2_r16;
  dl_assign_idx_dci_1_2_r16_e_                         dl_assign_idx_dci_1_2_r16;
  copy_ptr<setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>>
                                                                     pdsch_harq_ack_codebook_list_r16;
  ack_nack_feedback_mode_r16_e_                                      ack_nack_feedback_mode_r16;
  copy_ptr<setup_release_c<pdcch_blind_detection_ca_comb_ind_r16_s>> pdcch_blind_detection_ca_comb_ind_r16;
  copy_ptr<setup_release_c<integer<uint8_t, 1, 15>>>                 pdcch_blind_detection2_r16;
  copy_ptr<setup_release_c<integer<uint8_t, 1, 15>>>                 pdcch_blind_detection3_r16;
  // group 4
  bool pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present = false;
  bool pdsch_harq_ack_enh_type3_dci_field_r17_present                       = false;
  bool pdsch_harq_ack_retx_r17_present                                      = false;
  bool pdsch_harq_ack_retx_secondary_pucch_group_r17_present                = false;
  bool pucch_s_scell_r17_present                                            = false;
  bool pucch_s_scell_secondary_pucch_group_r17_present                      = false;
  bool pucch_s_scell_dyn_r17_present                                        = false;
  bool pucch_s_scell_dyn_secondary_pucch_group_r17_present                  = false;
  bool uci_mux_with_diff_prio_r17_present                                   = false;
  bool uci_mux_with_diff_prio_secondary_pucch_group_r17_present             = false;
  bool simul_pucch_pusch_r17_present                                        = false;
  bool simul_pucch_pusch_secondary_pucch_group_r17_present                  = false;
  bool prio_low_dg_high_cg_r17_present                                      = false;
  bool prio_high_dg_low_cg_r17_present                                      = false;
  bool two_qcl_type_d_for_pdcch_repeat_r17_present                          = false;
  copy_ptr<pdsch_harq_ack_enh_type3_to_add_mod_list_r17_l_> pdsch_harq_ack_enh_type3_to_add_mod_list_r17;
  copy_ptr<pdsch_harq_ack_enh_type3_to_release_list_r17_l_> pdsch_harq_ack_enh_type3_to_release_list_r17;
  copy_ptr<pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17_l_>
      pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17;
  copy_ptr<pdsch_harq_ack_enh_type3_secondary_to_release_list_r17_l_>
                                                               pdsch_harq_ack_enh_type3_secondary_to_release_list_r17;
  uint8_t                                                      pucch_s_scell_r17                       = 1;
  uint8_t                                                      pucch_s_scell_secondary_pucch_group_r17 = 1;
  copy_ptr<pucch_s_scell_pattern_r17_l_>                       pucch_s_scell_pattern_r17;
  copy_ptr<pucch_s_scell_pattern_secondary_pucch_group_r17_l_> pucch_s_scell_pattern_secondary_pucch_group_r17;
  copy_ptr<setup_release_c<multicast_cfg_r17_s>>               multicast_cfg_r17;
  copy_ptr<setup_release_c<pdcch_blind_detection_ca_comb_ind_r17_s>> pdcch_blind_detection_ca_comb_ind_r17;
  // group 5
  bool simul_sr_pusch_diff_pucch_groups_r17_present = false;
  // group 6
  bool intra_band_nc_prach_simul_tx_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-BearerConfig ::= SEQUENCE
struct rlc_bearer_cfg_s {
  struct served_radio_bearer_c_ {
    struct types_opts {
      enum options { srb_id, drb_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    served_radio_bearer_c_() = default;
    served_radio_bearer_c_(const served_radio_bearer_c_& other);
    served_radio_bearer_c_& operator=(const served_radio_bearer_c_& other);
    ~served_radio_bearer_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& srb_id()
    {
      assert_choice_type(types::srb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    uint8_t& drb_id()
    {
      assert_choice_type(types::drb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    const uint8_t& srb_id() const
    {
      assert_choice_type(types::srb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    const uint8_t& drb_id() const
    {
      assert_choice_type(types::drb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    uint8_t& set_srb_id();
    uint8_t& set_drb_id();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                   ext                         = false;
  bool                   served_radio_bearer_present = false;
  bool                   reestablish_rlc_present     = false;
  bool                   rlc_cfg_present             = false;
  bool                   mac_lc_ch_cfg_present       = false;
  uint8_t                lc_ch_id                    = 1;
  served_radio_bearer_c_ served_radio_bearer;
  rlc_cfg_c              rlc_cfg;
  lc_ch_cfg_s            mac_lc_ch_cfg;
  // ...
  // group 0
  copy_ptr<rlc_cfg_v1610_s> rlc_cfg_v1610;
  // group 1
  bool                                     lc_ch_id_ext_r17_present             = false;
  bool                                     served_radio_bearer_srb4_r17_present = false;
  copy_ptr<rlc_cfg_v1700_s>                rlc_cfg_v1700;
  uint32_t                                 lc_ch_id_ext_r17 = 320;
  copy_ptr<multicast_rlc_bearer_cfg_r17_s> multicast_rlc_bearer_cfg_r17;
  uint8_t                                  served_radio_bearer_srb4_r17 = 4;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportUplinkTxDirectCurrentMoreCarrier-r17 ::= SEQUENCE (SIZE (1..32)) OF IntraBandCC-CombinationReqList-r17
using report_ul_tx_direct_current_more_carrier_r17_l = dyn_array<intra_band_cc_combination_req_list_r17_s>;

// SCellConfig ::= SEQUENCE
struct scell_cfg_s {
  bool                      ext                      = false;
  bool                      scell_cfg_common_present = false;
  bool                      scell_cfg_ded_present    = false;
  uint8_t                   scell_idx                = 1;
  serving_cell_cfg_common_s scell_cfg_common;
  serving_cell_cfg_s        scell_cfg_ded;
  // ...
  // group 0
  copy_ptr<ssb_mtc_s> smtc;
  // group 1
  bool scell_state_r16_present             = false;
  bool secondary_drx_group_cfg_r16_present = false;
  // group 2
  bool                                     pre_conf_gap_status_r17_present = false;
  fixed_bitstring<8>                       pre_conf_gap_status_r17;
  copy_ptr<good_serving_cell_eval_r17_s>   good_serving_cell_eval_bfd_r17;
  copy_ptr<setup_release_c<dyn_octstring>> scell_sib20_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpCellConfig ::= SEQUENCE
struct sp_cell_cfg_s {
  struct low_mob_eval_connected_r17_s_ {
    struct s_search_delta_p_connected_r17_opts {
      enum options { db3, db6, db9, db12, db15, spare3, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<s_search_delta_p_connected_r17_opts> s_search_delta_p_connected_r17_e_;
    struct t_search_delta_p_connected_r17_opts {
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
    typedef enumerated<t_search_delta_p_connected_r17_opts> t_search_delta_p_connected_r17_e_;

    // member variables
    s_search_delta_p_connected_r17_e_ s_search_delta_p_connected_r17;
    t_search_delta_p_connected_r17_e_ t_search_delta_p_connected_r17;
  };

  // member variables
  bool                                     ext                                   = false;
  bool                                     serv_cell_idx_present                 = false;
  bool                                     recfg_with_sync_present               = false;
  bool                                     rlf_timers_and_consts_present         = false;
  bool                                     rlm_in_sync_out_of_sync_thres_present = false;
  bool                                     sp_cell_cfg_ded_present               = false;
  uint8_t                                  serv_cell_idx                         = 0;
  recfg_with_sync_s                        recfg_with_sync;
  setup_release_c<rlf_timers_and_consts_s> rlf_timers_and_consts;
  serving_cell_cfg_s                       sp_cell_cfg_ded;
  // ...
  // group 0
  copy_ptr<low_mob_eval_connected_r17_s_>          low_mob_eval_connected_r17;
  copy_ptr<good_serving_cell_eval_r17_s>           good_serving_cell_eval_rlm_r17;
  copy_ptr<good_serving_cell_eval_r17_s>           good_serving_cell_eval_bfd_r17;
  copy_ptr<setup_release_c<deactiv_scg_cfg_r17_s>> deactiv_scg_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Uu-RelayRLC-ChannelConfig-r17 ::= SEQUENCE
struct uu_relay_rlc_ch_cfg_r17_s {
  bool        ext                         = false;
  bool        uu_lc_ch_id_r17_present     = false;
  bool        reestablish_rlc_r17_present = false;
  bool        rlc_cfg_r17_present         = false;
  bool        mac_lc_ch_cfg_r17_present   = false;
  uint8_t     uu_lc_ch_id_r17             = 1;
  uint8_t     uu_relay_rlc_ch_id_r17      = 1;
  rlc_cfg_c   rlc_cfg_r17;
  lc_ch_cfg_s mac_lc_ch_cfg_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGroupConfig ::= SEQUENCE
struct cell_group_cfg_s {
  using rlc_bearer_to_add_mod_list_l_    = dyn_array<rlc_bearer_cfg_s>;
  using rlc_bearer_to_release_list_l_    = bounded_array<uint8_t, 32>;
  using scell_to_add_mod_list_l_         = dyn_array<scell_cfg_s>;
  using scell_to_release_list_l_         = bounded_array<uint8_t, 31>;
  using bh_rlc_ch_to_add_mod_list_r16_l_ = dyn_array<bh_rlc_ch_cfg_r16_s>;
  using bh_rlc_ch_to_release_list_r16_l_ = dyn_array<fixed_bitstring<16>>;
  struct f1c_transfer_path_r16_opts {
    enum options { lte, nr, both, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<f1c_transfer_path_r16_opts> f1c_transfer_path_r16_e_;
  using simul_tci_upd_list1_r16_l_     = bounded_array<uint8_t, 32>;
  using simul_tci_upd_list2_r16_l_     = bounded_array<uint8_t, 32>;
  using simul_spatial_upd_list1_r16_l_ = bounded_array<uint8_t, 32>;
  using simul_spatial_upd_list2_r16_l_ = bounded_array<uint8_t, 32>;
  struct ul_tx_switching_option_r16_opts {
    enum options { switched_ul, dual_ul, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ul_tx_switching_option_r16_opts> ul_tx_switching_option_r16_e_;
  struct f1c_transfer_path_nrdc_r17_opts {
    enum options { mcg, scg, both, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<f1c_transfer_path_nrdc_r17_opts> f1c_transfer_path_nrdc_r17_e_;
  struct ul_tx_switching_dual_ul_tx_state_r17_opts {
    enum options { one_t, two_t, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ul_tx_switching_dual_ul_tx_state_r17_opts> ul_tx_switching_dual_ul_tx_state_r17_e_;
  using uu_relay_rlc_ch_to_add_mod_list_r17_l_ = dyn_array<uu_relay_rlc_ch_cfg_r17_s>;
  using uu_relay_rlc_ch_to_release_list_r17_l_ = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list1_r17_l_           = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list2_r17_l_           = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list3_r17_l_           = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list4_r17_l_           = bounded_array<uint8_t, 32>;
  using rlc_bearer_to_release_list_ext_r17_l_  = bounded_array<uint32_t, 32>;
  using iab_res_cfg_to_add_mod_list_r17_l_     = dyn_array<iab_res_cfg_r17_s>;
  using iab_res_cfg_to_release_list_r17_l_     = dyn_array<uint32_t>;

  // member variables
  bool                          ext                         = false;
  bool                          mac_cell_group_cfg_present  = false;
  bool                          phys_cell_group_cfg_present = false;
  bool                          sp_cell_cfg_present         = false;
  uint8_t                       cell_group_id               = 0;
  rlc_bearer_to_add_mod_list_l_ rlc_bearer_to_add_mod_list;
  rlc_bearer_to_release_list_l_ rlc_bearer_to_release_list;
  mac_cell_group_cfg_s          mac_cell_group_cfg;
  phys_cell_group_cfg_s         phys_cell_group_cfg;
  sp_cell_cfg_s                 sp_cell_cfg;
  scell_to_add_mod_list_l_      scell_to_add_mod_list;
  scell_to_release_list_l_      scell_to_release_list;
  // ...
  // group 0
  bool report_ul_tx_direct_current_present = false;
  // group 1
  bool                                       bap_address_r16_present                  = false;
  bool                                       f1c_transfer_path_r16_present            = false;
  bool                                       ul_tx_switching_option_r16_present       = false;
  bool                                       ul_tx_switching_pwr_boosting_r16_present = false;
  fixed_bitstring<10>                        bap_address_r16;
  copy_ptr<bh_rlc_ch_to_add_mod_list_r16_l_> bh_rlc_ch_to_add_mod_list_r16;
  copy_ptr<bh_rlc_ch_to_release_list_r16_l_> bh_rlc_ch_to_release_list_r16;
  f1c_transfer_path_r16_e_                   f1c_transfer_path_r16;
  copy_ptr<simul_tci_upd_list1_r16_l_>       simul_tci_upd_list1_r16;
  copy_ptr<simul_tci_upd_list2_r16_l_>       simul_tci_upd_list2_r16;
  copy_ptr<simul_spatial_upd_list1_r16_l_>   simul_spatial_upd_list1_r16;
  copy_ptr<simul_spatial_upd_list2_r16_l_>   simul_spatial_upd_list2_r16;
  ul_tx_switching_option_r16_e_              ul_tx_switching_option_r16;
  // group 2
  bool report_ul_tx_direct_current_two_carrier_r16_present = false;
  // group 3
  bool                                             f1c_transfer_path_nrdc_r17_present           = false;
  bool                                             ul_tx_switching_2_t_mode_r17_present         = false;
  bool                                             ul_tx_switching_dual_ul_tx_state_r17_present = false;
  f1c_transfer_path_nrdc_r17_e_                    f1c_transfer_path_nrdc_r17;
  ul_tx_switching_dual_ul_tx_state_r17_e_          ul_tx_switching_dual_ul_tx_state_r17;
  copy_ptr<uu_relay_rlc_ch_to_add_mod_list_r17_l_> uu_relay_rlc_ch_to_add_mod_list_r17;
  copy_ptr<uu_relay_rlc_ch_to_release_list_r17_l_> uu_relay_rlc_ch_to_release_list_r17;
  copy_ptr<simul_u_tci_upd_list1_r17_l_>           simul_u_tci_upd_list1_r17;
  copy_ptr<simul_u_tci_upd_list2_r17_l_>           simul_u_tci_upd_list2_r17;
  copy_ptr<simul_u_tci_upd_list3_r17_l_>           simul_u_tci_upd_list3_r17;
  copy_ptr<simul_u_tci_upd_list4_r17_l_>           simul_u_tci_upd_list4_r17;
  copy_ptr<rlc_bearer_to_release_list_ext_r17_l_>  rlc_bearer_to_release_list_ext_r17;
  copy_ptr<iab_res_cfg_to_add_mod_list_r17_l_>     iab_res_cfg_to_add_mod_list_r17;
  copy_ptr<iab_res_cfg_to_release_list_r17_l_>     iab_res_cfg_to_release_list_r17;
  // group 4
  copy_ptr<report_ul_tx_direct_current_more_carrier_r17_l> report_ul_tx_direct_current_more_carrier_r17;

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
  typedef enumerated<mode_r16_opts> mode_r16_e_;

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
  typedef enumerated<nof_aggr_prb_opts> nof_aggr_prb_e_;

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
  typedef enumerated<cg_sdt_rsrp_change_thres_r17_opts> cg_sdt_rsrp_change_thres_r17_e_;

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

// SL-BWP-DiscPoolConfigCommon-r17 ::= SEQUENCE
struct sl_bwp_disc_pool_cfg_common_r17_s {
  using sl_disc_rx_pool_r17_l_     = dyn_array<sl_res_pool_r16_s>;
  using sl_disc_tx_pool_sel_r17_l_ = dyn_array<sl_res_pool_cfg_r16_s>;

  // member variables
  bool                       ext = false;
  sl_disc_rx_pool_r17_l_     sl_disc_rx_pool_r17;
  sl_disc_tx_pool_sel_r17_l_ sl_disc_tx_pool_sel_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-BWP-PoolConfigCommon-r16 ::= SEQUENCE
struct sl_bwp_pool_cfg_common_r16_s {
  using sl_rx_pool_r16_l_            = dyn_array<sl_res_pool_r16_s>;
  using sl_tx_pool_sel_normal_r16_l_ = dyn_array<sl_res_pool_cfg_r16_s>;

  // member variables
  bool                         sl_tx_pool_exceptional_r16_present = false;
  sl_rx_pool_r16_l_            sl_rx_pool_r16;
  sl_tx_pool_sel_normal_r16_l_ sl_tx_pool_sel_normal_r16;
  sl_res_pool_cfg_r16_s        sl_tx_pool_exceptional_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-BWP-ConfigCommon-r16 ::= SEQUENCE
struct sl_bwp_cfg_common_r16_s {
  bool                         ext                                = false;
  bool                         sl_bwp_generic_r16_present         = false;
  bool                         sl_bwp_pool_cfg_common_r16_present = false;
  sl_bwp_generic_r16_s         sl_bwp_generic_r16;
  sl_bwp_pool_cfg_common_r16_s sl_bwp_pool_cfg_common_r16;
  // ...
  // group 0
  copy_ptr<sl_bwp_pool_cfg_common_r16_s>      sl_bwp_pool_cfg_common_ps_r17;
  copy_ptr<sl_bwp_disc_pool_cfg_common_r17_s> sl_bwp_disc_pool_cfg_common_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-EUTRA-AnchorCarrierFreqList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..262143)
using sl_eutra_anchor_carrier_freq_list_r16_l = bounded_array<uint32_t, 8>;

// SL-FreqConfigCommon-r16 ::= SEQUENCE
struct sl_freq_cfg_common_r16_s {
  using sl_scs_specific_carrier_list_r16_l_ = dyn_array<scs_specific_carrier_s>;
  using sl_bwp_list_r16_l_                  = dyn_array<sl_bwp_cfg_common_r16_s>;
  struct sl_sync_prio_r16_opts {
    enum options { gnss, gnb_enb, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sl_sync_prio_r16_opts> sl_sync_prio_r16_e_;

  // member variables
  bool                                ext                              = false;
  bool                                sl_absolute_freq_ssb_r16_present = false;
  bool                                freq_shift7p5khz_sl_r16_present  = false;
  bool                                sl_sync_prio_r16_present         = false;
  bool                                sl_nb_as_sync_r16_present        = false;
  sl_scs_specific_carrier_list_r16_l_ sl_scs_specific_carrier_list_r16;
  uint32_t                            sl_absolute_freq_point_a_r16 = 0;
  uint32_t                            sl_absolute_freq_ssb_r16     = 0;
  int8_t                              value_n_r16                  = -1;
  sl_bwp_list_r16_l_                  sl_bwp_list_r16;
  sl_sync_prio_r16_e_                 sl_sync_prio_r16;
  bool                                sl_nb_as_sync_r16 = false;
  sl_sync_cfg_list_r16_l              sl_sync_cfg_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-MeasConfigCommon-r16 ::= SEQUENCE
struct sl_meas_cfg_common_r16_s {
  bool                     ext                             = false;
  bool                     sl_quant_cfg_common_r16_present = false;
  sl_meas_obj_list_r16_l   sl_meas_obj_list_common_r16;
  sl_report_cfg_list_r16_l sl_report_cfg_list_common_r16;
  sl_meas_id_list_r16_l    sl_meas_id_list_common_r16;
  sl_quant_cfg_r16_s       sl_quant_cfg_common_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-NR-AnchorCarrierFreqList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..3279165)
using sl_nr_anchor_carrier_freq_list_r16_l = bounded_array<uint32_t, 8>;

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
  typedef enumerated<t400_r16_opts> t400_r16_e_;
  struct sl_max_num_consecutive_dtx_r16_opts {
    enum options { n1, n2, n3, n4, n6, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sl_max_num_consecutive_dtx_r16_opts> sl_max_num_consecutive_dtx_r16_e_;

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
  typedef enumerated<t300_remote_ue_r17_opts> t300_remote_ue_r17_e_;
  struct t301_remote_ue_r17_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t301_remote_ue_r17_opts> t301_remote_ue_r17_e_;
  struct t319_remote_ue_r17_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t319_remote_ue_r17_opts> t319_remote_ue_r17_e_;

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
  typedef enumerated<pwr_ctrl_offset_ss_r17_opts> pwr_ctrl_offset_ss_r17_e_;
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
    typedef enumerated<types_opts, true> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<nrof_res_r17_opts> nrof_res_r17_e_;

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
  typedef enumerated<validity_dur_r17_opts> validity_dur_r17_e_;

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
typedef enumerated<rsrp_change_thres_r17_opts> rsrp_change_thres_r17_e;

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
typedef enumerated<t_offset_r16_opts> t_offset_r16_e;

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
    typedef enumerated<ul_pwr_sharing_daps_mode_r16_opts> ul_pwr_sharing_daps_mode_r16_e_;

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
  typedef enumerated<nrdc_pc_mode_fr1_r16_opts> nrdc_pc_mode_fr1_r16_e_;
  struct nrdc_pc_mode_fr2_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrdc_pc_mode_fr2_r16_opts> nrdc_pc_mode_fr2_r16_e_;

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
  typedef enumerated<interference_direction_mrdc_opts> interference_direction_mrdc_e_;
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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
typedef enumerated<tx_bw_eutra_r16_opts> tx_bw_eutra_r16_e;

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
    typedef enumerated<milli_seconds_opts> milli_seconds_e_;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
  typedef enumerated<ph_type1or3_opts> ph_type1or3_e_;

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
  typedef enumerated<fr_type_opts> fr_type_e_;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<drx_short_cycle_opts> drx_short_cycle_e_;

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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<fail_type_r16_opts> fail_type_r16_e_;

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
    typedef enumerated<fail_type_eutra_r16_opts> fail_type_eutra_r16_e_;

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
  typedef enumerated<ph_type1or3_opts> ph_type1or3_e_;

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
    typedef enumerated<fail_type_eutra_opts> fail_type_eutra_e_;

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
  typedef enumerated<gap_purpose_opts> gap_purpose_e_;

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
    typedef enumerated<fail_type_opts> fail_type_e_;

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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<ue_inactive_time_opts> ue_inactive_time_e_;

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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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

// SL-PreconfigGeneral-r16 ::= SEQUENCE
struct sl_precfg_general_r16_s {
  bool                   ext                       = false;
  bool                   sl_tdd_cfg_r16_present    = false;
  bool                   reserved_bits_r16_present = false;
  tdd_ul_dl_cfg_common_s sl_tdd_cfg_r16;
  fixed_bitstring<2>     reserved_bits_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxProfile-r17 ::= ENUMERATED
struct sl_tx_profile_r17_opts {
  enum options { drx_compatible, drx_incompatible, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<sl_tx_profile_r17_opts> sl_tx_profile_r17_e;

// SL-RoHC-Profiles-r16 ::= SEQUENCE
struct sl_ro_hc_profiles_r16_s {
  bool profile0x0001_r16 = false;
  bool profile0x0002_r16 = false;
  bool profile0x0003_r16 = false;
  bool profile0x0004_r16 = false;
  bool profile0x0006_r16 = false;
  bool profile0x0101_r16 = false;
  bool profile0x0102_r16 = false;
  bool profile0x0103_r16 = false;
  bool profile0x0104_r16 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxProfileList-r17 ::= SEQUENCE (SIZE (1..256)) OF SL-TxProfile-r17
using sl_tx_profile_list_r17_l = dyn_array<sl_tx_profile_r17_e>;

// SidelinkPreconfigNR-r16 ::= SEQUENCE
struct sidelink_precfg_nr_r16_s {
  using sl_precfg_freq_info_list_r16_l_     = dyn_array<sl_freq_cfg_common_r16_s>;
  using sl_radio_bearer_pre_cfg_list_r16_l_ = dyn_array<sl_radio_bearer_cfg_r16_s>;
  using sl_rlc_bearer_pre_cfg_list_r16_l_   = dyn_array<sl_rlc_bearer_cfg_r16_s>;
  struct t400_r16_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t400_r16_opts> t400_r16_e_;
  struct sl_max_num_consecutive_dtx_r16_opts {
    enum options { n1, n2, n3, n4, n6, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sl_max_num_consecutive_dtx_r16_opts> sl_max_num_consecutive_dtx_r16_e_;

  // member variables
  bool                                    ext                                    = false;
  bool                                    sl_meas_pre_cfg_r16_present            = false;
  bool                                    sl_offset_dfn_r16_present              = false;
  bool                                    t400_r16_present                       = false;
  bool                                    sl_max_num_consecutive_dtx_r16_present = false;
  bool                                    sl_ssb_prio_nr_r16_present             = false;
  bool                                    sl_precfg_general_r16_present          = false;
  bool                                    sl_ue_sel_pre_cfg_r16_present          = false;
  bool                                    sl_csi_acquisition_r16_present         = false;
  bool                                    sl_ro_hc_profiles_r16_present          = false;
  bool                                    sl_max_c_id_r16_present                = false;
  sl_precfg_freq_info_list_r16_l_         sl_precfg_freq_info_list_r16;
  sl_nr_anchor_carrier_freq_list_r16_l    sl_precfg_nr_anchor_carrier_freq_list_r16;
  sl_eutra_anchor_carrier_freq_list_r16_l sl_precfg_eutra_anchor_carrier_freq_list_r16;
  sl_radio_bearer_pre_cfg_list_r16_l_     sl_radio_bearer_pre_cfg_list_r16;
  sl_rlc_bearer_pre_cfg_list_r16_l_       sl_rlc_bearer_pre_cfg_list_r16;
  sl_meas_cfg_common_r16_s                sl_meas_pre_cfg_r16;
  uint16_t                                sl_offset_dfn_r16 = 1;
  t400_r16_e_                             t400_r16;
  sl_max_num_consecutive_dtx_r16_e_       sl_max_num_consecutive_dtx_r16;
  uint8_t                                 sl_ssb_prio_nr_r16 = 1;
  sl_precfg_general_r16_s                 sl_precfg_general_r16;
  sl_ue_sel_cfg_r16_s                     sl_ue_sel_pre_cfg_r16;
  sl_ro_hc_profiles_r16_s                 sl_ro_hc_profiles_r16;
  uint16_t                                sl_max_c_id_r16 = 1;
  // ...
  // group 0
  copy_ptr<sl_drx_cfg_gc_bc_r17_s>   sl_drx_pre_cfg_gc_bc_r17;
  copy_ptr<sl_tx_profile_list_r17_l> sl_tx_profile_list_r17;
  copy_ptr<sl_remote_ue_cfg_r17_s>   sl_precfg_disc_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigurationNR-r16 ::= SEQUENCE
struct sl_precfg_nr_r16_s {
  bool                     ext = false;
  sidelink_precfg_nr_r16_s sidelink_precfg_nr_r16;
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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<nof_rx_red_cap_r17_opts> nof_rx_red_cap_r17_e_;
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
      typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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

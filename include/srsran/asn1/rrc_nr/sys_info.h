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

#include "serving_cell.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

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

// GNSS-ID-r16 ::= SEQUENCE
struct gnss_id_r16_s {
  struct gnss_id_r16_opts {
    enum options { gps, sbas, qzss, galileo, glonass, bds, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using gnss_id_r16_e_ = enumerated<gnss_id_r16_opts, true>;

  // member variables
  bool           ext = false;
  gnss_id_r16_e_ gnss_id_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-Range ::= SEQUENCE
struct pci_range_s {
  struct range_opts {
    enum options {
      n4,
      n8,
      n12,
      n16,
      n24,
      n32,
      n48,
      n64,
      n84,
      n96,
      n128,
      n168,
      n252,
      n504,
      n1008,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using range_e_ = enumerated<range_opts>;

  // member variables
  bool     range_present = false;
  uint16_t start         = 0;
  range_e_ range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SBAS-ID-r16 ::= SEQUENCE
struct sbas_id_r16_s {
  struct sbas_id_r16_opts {
    enum options { waas, egnos, msas, gagan, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using sbas_id_r16_e_ = enumerated<sbas_id_r16_opts, true>;

  // member variables
  bool           ext = false;
  sbas_id_r16_e_ sbas_id_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionSubPriority ::= ENUMERATED
struct cell_resel_sub_prio_opts {
  enum options { odot2, odot4, odot6, odot8, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using cell_resel_sub_prio_e = enumerated<cell_resel_sub_prio_opts>;

// EUTRA-NS-PmaxList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-NS-PmaxValue
using eutra_ns_pmax_list_l = dyn_array<eutra_ns_pmax_value_s>;

// EUTRA-Q-OffsetRange ::= ENUMERATED
struct eutra_q_offset_range_opts {
  enum options {
    db_neg24,
    db_neg22,
    db_neg20,
    db_neg18,
    db_neg16,
    db_neg14,
    db_neg12,
    db_neg10,
    db_neg8,
    db_neg6,
    db_neg5,
    db_neg4,
    db_neg3,
    db_neg2,
    db_neg1,
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
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using eutra_q_offset_range_e = enumerated<eutra_q_offset_range_opts>;

// NSAG-IdentityInfo-r17 ::= SEQUENCE
struct nsag_id_info_r17_s {
  bool                tac_r17_present = false;
  fixed_bitstring<8>  nsag_id_r17;
  fixed_bitstring<24> tac_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Orbital-r17 ::= SEQUENCE
struct orbital_r17_s {
  uint64_t semi_major_axis_r17 = 0;
  uint32_t eccentricity_r17    = 0;
  uint32_t periapsis_r17       = 0;
  uint32_t longitude_r17       = 0;
  int32_t  inclination_r17     = -67108864;
  uint32_t mean_anomaly_r17    = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositionVelocity-r17 ::= SEQUENCE
struct position_velocity_r17_s {
  int32_t position_x_r17  = -33554432;
  int32_t position_y_r17  = -33554432;
  int32_t position_z_r17  = -33554432;
  int32_t velocity_vx_r17 = -131072;
  int32_t velocity_vy_r17 = -131072;
  int32_t velocity_vz_r17 = -131072;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Q-OffsetRange ::= ENUMERATED
struct q_offset_range_opts {
  enum options {
    db_neg24,
    db_neg22,
    db_neg20,
    db_neg18,
    db_neg16,
    db_neg14,
    db_neg12,
    db_neg10,
    db_neg8,
    db_neg6,
    db_neg5,
    db_neg4,
    db_neg3,
    db_neg2,
    db_neg1,
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
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using q_offset_range_e = enumerated<q_offset_range_opts>;

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

// SSB-PositionQCL-Relation-r16 ::= ENUMERATED
struct ssb_position_qcl_relation_r16_opts {
  enum options { n1, n2, n4, n8, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using ssb_position_qcl_relation_r16_e = enumerated<ssb_position_qcl_relation_r16_opts>;

// SSB-PositionQCL-Relation-r17 ::= ENUMERATED
struct ssb_position_qcl_relation_r17_opts {
  enum options { n32, n64, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using ssb_position_qcl_relation_r17_e = enumerated<ssb_position_qcl_relation_r17_opts>;

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

// EUTRA-PhysCellIdRange ::= SEQUENCE
struct eutra_pci_range_s {
  struct range_opts {
    enum options {
      n4,
      n8,
      n12,
      n16,
      n24,
      n32,
      n48,
      n64,
      n84,
      n96,
      n128,
      n168,
      n252,
      n504,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using range_e_ = enumerated<range_opts>;

  // member variables
  bool     range_present = false;
  uint16_t start         = 0;
  range_e_ range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EphemerisInfo-r17 ::= CHOICE
struct ephemeris_info_r17_c {
  struct types_opts {
    enum options { position_velocity_r17, orbital_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ephemeris_info_r17_c() = default;
  ephemeris_info_r17_c(const ephemeris_info_r17_c& other);
  ephemeris_info_r17_c& operator=(const ephemeris_info_r17_c& other);
  ~ephemeris_info_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  position_velocity_r17_s& position_velocity_r17()
  {
    assert_choice_type(types::position_velocity_r17, type_, "EphemerisInfo-r17");
    return c.get<position_velocity_r17_s>();
  }
  orbital_r17_s& orbital_r17()
  {
    assert_choice_type(types::orbital_r17, type_, "EphemerisInfo-r17");
    return c.get<orbital_r17_s>();
  }
  const position_velocity_r17_s& position_velocity_r17() const
  {
    assert_choice_type(types::position_velocity_r17, type_, "EphemerisInfo-r17");
    return c.get<position_velocity_r17_s>();
  }
  const orbital_r17_s& orbital_r17() const
  {
    assert_choice_type(types::orbital_r17, type_, "EphemerisInfo-r17");
    return c.get<orbital_r17_s>();
  }
  position_velocity_r17_s& set_position_velocity_r17();
  orbital_r17_s&           set_orbital_r17();

private:
  types                                                   type_;
  choice_buffer_t<orbital_r17_s, position_velocity_r17_s> c;

  void destroy_();
};

// EpochTime-r17 ::= SEQUENCE
struct epoch_time_r17_s {
  uint16_t sfn_r17          = 0;
  uint8_t  sub_frame_nr_r17 = 0;

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

// SSB-MTC4-r17 ::= SEQUENCE
struct ssb_mtc4_r17_s {
  using pci_list_r17_l_ = dyn_array<uint16_t>;

  // member variables
  pci_list_r17_l_ pci_list_r17;
  uint8_t         offset_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// BeamMeasConfigIdle-NR-r16 ::= SEQUENCE
struct beam_meas_cfg_idle_nr_r16_s {
  struct report_quant_rs_idxes_r16_opts {
    enum options { rsrp, rsrq, both, nulltype } value;

    const char* to_string() const;
  };
  using report_quant_rs_idxes_r16_e_ = enumerated<report_quant_rs_idxes_r16_opts>;

  // member variables
  report_quant_rs_idxes_r16_e_ report_quant_rs_idxes_r16;
  uint8_t                      max_nrof_rs_idxes_to_report_r16 = 1;
  bool                         include_beam_meass_r16          = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CAG-IdentityInfo-r16 ::= SEQUENCE
struct cag_id_info_r16_s {
  bool                manual_ca_gsel_allowed_r16_present = false;
  fixed_bitstring<32> cag_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellListEUTRA-r16 ::= SEQUENCE (SIZE (1..8)) OF EUTRA-PhysCellIdRange
using cell_list_eutra_r16_l = dyn_array<eutra_pci_range_s>;

// CellListNR-r16 ::= SEQUENCE (SIZE (1..8)) OF PCI-Range
using cell_list_nr_r16_l = dyn_array<pci_range_s>;

// EUTRA-AllowedMeasBandwidth ::= ENUMERATED
struct eutra_allowed_meas_bw_opts {
  enum options { mbw6, mbw15, mbw25, mbw50, mbw75, mbw100, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using eutra_allowed_meas_bw_e = enumerated<eutra_allowed_meas_bw_opts>;

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

// MultiFrequencyBandListNR ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..1024)
using multi_freq_band_list_nr_l = bounded_array<uint16_t, 8>;

// NTN-Config-r17 ::= SEQUENCE
struct ntn_cfg_r17_s {
  struct ntn_ul_sync_validity_dur_r17_opts {
    enum options { s5, s10, s15, s20, s25, s30, s35, s40, s45, s50, s55, s60, s120, s180, s240, s900, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ntn_ul_sync_validity_dur_r17_e_ = enumerated<ntn_ul_sync_validity_dur_r17_opts>;
  struct ntn_polarization_dl_r17_opts {
    enum options { rhcp, lhcp, linear, nulltype } value;

    const char* to_string() const;
  };
  using ntn_polarization_dl_r17_e_ = enumerated<ntn_polarization_dl_r17_opts>;
  struct ntn_polarization_ul_r17_opts {
    enum options { rhcp, lhcp, linear, nulltype } value;

    const char* to_string() const;
  };
  using ntn_polarization_ul_r17_e_ = enumerated<ntn_polarization_ul_r17_opts>;

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

// SS-RSSI-Measurement ::= SEQUENCE
struct ss_rssi_meas_s {
  bounded_bitstring<1, 80> meas_slots;
  uint8_t                  end_symbol = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC ::= SEQUENCE
struct ssb_mtc_s {
  struct periodicity_and_offset_c_ {
    struct types_opts {
      enum options { sf5, sf10, sf20, sf40, sf80, sf160, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    periodicity_and_offset_c_() = default;
    periodicity_and_offset_c_(const periodicity_and_offset_c_& other);
    periodicity_and_offset_c_& operator=(const periodicity_and_offset_c_& other);
    ~periodicity_and_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sf5()
    {
      assert_choice_type(types::sf5, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf10()
    {
      assert_choice_type(types::sf10, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf20()
    {
      assert_choice_type(types::sf20, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf40()
    {
      assert_choice_type(types::sf40, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf80()
    {
      assert_choice_type(types::sf80, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sf160()
    {
      assert_choice_type(types::sf160, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf5() const
    {
      assert_choice_type(types::sf5, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf10() const
    {
      assert_choice_type(types::sf10, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf20() const
    {
      assert_choice_type(types::sf20, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf40() const
    {
      assert_choice_type(types::sf40, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf80() const
    {
      assert_choice_type(types::sf80, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sf160() const
    {
      assert_choice_type(types::sf160, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& set_sf5();
    uint8_t& set_sf10();
    uint8_t& set_sf20();
    uint8_t& set_sf40();
    uint8_t& set_sf80();
    uint8_t& set_sf160();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct dur_opts {
    enum options { sf1, sf2, sf3, sf4, sf5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dur_e_ = enumerated<dur_opts>;

  // member variables
  periodicity_and_offset_c_ periodicity_and_offset;
  dur_e_                    dur;

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

// SSB-MTC4List-r17 ::= SEQUENCE (SIZE (1..3)) OF SSB-MTC4-r17
using ssb_mtc4_list_r17_l = dyn_array<ssb_mtc4_r17_s>;

// SSB-ToMeasure ::= CHOICE
struct ssb_to_measure_c {
  struct types_opts {
    enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ssb_to_measure_c() = default;
  ssb_to_measure_c(const ssb_to_measure_c& other);
  ssb_to_measure_c& operator=(const ssb_to_measure_c& other);
  ~ssb_to_measure_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<4>& short_bitmap()
  {
    assert_choice_type(types::short_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<4>>();
  }
  fixed_bitstring<8>& medium_bitmap()
  {
    assert_choice_type(types::medium_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<8>>();
  }
  fixed_bitstring<64>& long_bitmap()
  {
    assert_choice_type(types::long_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<64>>();
  }
  const fixed_bitstring<4>& short_bitmap() const
  {
    assert_choice_type(types::short_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<4>>();
  }
  const fixed_bitstring<8>& medium_bitmap() const
  {
    assert_choice_type(types::medium_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<8>>();
  }
  const fixed_bitstring<64>& long_bitmap() const
  {
    assert_choice_type(types::long_bitmap, type_, "SSB-ToMeasure");
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

// ThresholdNR ::= SEQUENCE
struct thres_nr_s {
  bool    thres_rsrp_present = false;
  bool    thres_rsrq_present = false;
  bool    thres_sinr_present = false;
  uint8_t thres_rsrp         = 0;
  uint8_t thres_rsrq         = 0;
  uint8_t thres_sinr         = 0;

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

// MeasIdleCarrierEUTRA-r16 ::= SEQUENCE
struct meas_idle_carrier_eutra_r16_s {
  struct report_quantities_eutra_r16_opts {
    enum options { rsrp, rsrq, both, nulltype } value;

    const char* to_string() const;
  };
  using report_quantities_eutra_r16_e_ = enumerated<report_quantities_eutra_r16_opts>;
  struct quality_thres_eutra_r16_s_ {
    bool    idle_rsrp_thres_eutra_r16_present = false;
    bool    idle_rsrq_thres_eutra_r16_present = false;
    uint8_t idle_rsrp_thres_eutra_r16         = 0;
    int8_t  idle_rsrq_thres_eutra_r16         = -30;
  };

  // member variables
  bool                           ext                             = false;
  bool                           quality_thres_eutra_r16_present = false;
  uint32_t                       carrier_freq_eutra_r16          = 0;
  eutra_allowed_meas_bw_e        allowed_meas_bw_r16;
  cell_list_eutra_r16_l          meas_cell_list_eutra_r16;
  report_quantities_eutra_r16_e_ report_quantities_eutra_r16;
  quality_thres_eutra_r16_s_     quality_thres_eutra_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdleCarrierNR-r16 ::= SEQUENCE
struct meas_idle_carrier_nr_r16_s {
  struct report_quantities_r16_opts {
    enum options { rsrp, rsrq, both, nulltype } value;

    const char* to_string() const;
  };
  using report_quantities_r16_e_ = enumerated<report_quantities_r16_opts>;
  struct quality_thres_r16_s_ {
    bool    idle_rsrp_thres_nr_r16_present = false;
    bool    idle_rsrq_thres_nr_r16_present = false;
    uint8_t idle_rsrp_thres_nr_r16         = 0;
    uint8_t idle_rsrq_thres_nr_r16         = 0;
  };
  struct ssb_meas_cfg_r16_s_ {
    bool             nrof_ss_blocks_to_average_r16_present          = false;
    bool             abs_thresh_ss_blocks_consolidation_r16_present = false;
    bool             smtc_r16_present                               = false;
    bool             ssb_to_measure_r16_present                     = false;
    bool             ss_rssi_meas_r16_present                       = false;
    uint8_t          nrof_ss_blocks_to_average_r16                  = 2;
    thres_nr_s       abs_thresh_ss_blocks_consolidation_r16;
    ssb_mtc_s        smtc_r16;
    ssb_to_measure_c ssb_to_measure_r16;
    bool             derive_ssb_idx_from_cell_r16 = false;
    ss_rssi_meas_s   ss_rssi_meas_r16;
  };

  // member variables
  bool                        ext                            = false;
  bool                        quality_thres_r16_present      = false;
  bool                        ssb_meas_cfg_r16_present       = false;
  bool                        beam_meas_cfg_idle_r16_present = false;
  uint32_t                    carrier_freq_r16               = 0;
  subcarrier_spacing_e        ssb_subcarrier_spacing_r16;
  multi_freq_band_list_nr_l   freq_band_list;
  cell_list_nr_r16_l          meas_cell_list_nr_r16;
  report_quantities_r16_e_    report_quantities_r16;
  quality_thres_r16_s_        quality_thres_r16;
  ssb_meas_cfg_r16_s_         ssb_meas_cfg_r16;
  beam_meas_cfg_idle_nr_r16_s beam_meas_cfg_idle_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPN-Identity-r16 ::= CHOICE
struct npn_id_r16_c {
  struct pni_npn_r16_s_ {
    using cag_id_list_r16_l_ = dyn_array<cag_id_info_r16_s>;

    // member variables
    plmn_id_s          plmn_id_r16;
    cag_id_list_r16_l_ cag_id_list_r16;
  };
  struct snpn_r16_s_ {
    using nid_list_r16_l_ = bounded_array<fixed_bitstring<44>, 12>;

    // member variables
    plmn_id_s       plmn_id_r16;
    nid_list_r16_l_ nid_list_r16;
  };
  struct types_opts {
    enum options { pni_npn_r16, snpn_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_id_r16_c() = default;
  npn_id_r16_c(const npn_id_r16_c& other);
  npn_id_r16_c& operator=(const npn_id_r16_c& other);
  ~npn_id_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  pni_npn_r16_s_& pni_npn_r16()
  {
    assert_choice_type(types::pni_npn_r16, type_, "NPN-Identity-r16");
    return c.get<pni_npn_r16_s_>();
  }
  snpn_r16_s_& snpn_r16()
  {
    assert_choice_type(types::snpn_r16, type_, "NPN-Identity-r16");
    return c.get<snpn_r16_s_>();
  }
  const pni_npn_r16_s_& pni_npn_r16() const
  {
    assert_choice_type(types::pni_npn_r16, type_, "NPN-Identity-r16");
    return c.get<pni_npn_r16_s_>();
  }
  const snpn_r16_s_& snpn_r16() const
  {
    assert_choice_type(types::snpn_r16, type_, "NPN-Identity-r16");
    return c.get<snpn_r16_s_>();
  }
  pni_npn_r16_s_& set_pni_npn_r16();
  snpn_r16_s_&    set_snpn_r16();

private:
  types                                        type_;
  choice_buffer_t<pni_npn_r16_s_, snpn_r16_s_> c;

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

// ReferenceTime-r16 ::= SEQUENCE
struct ref_time_r16_s {
  uint32_t ref_days_r16             = 0;
  uint32_t ref_seconds_r16          = 0;
  uint16_t ref_milli_seconds_r16    = 0;
  uint32_t ref_ten_nano_seconds_r16 = 0;

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

// NPN-IdentityInfo-r16 ::= SEQUENCE
struct npn_id_info_r16_s {
  using npn_id_list_r16_l_ = dyn_array<npn_id_r16_c>;
  struct cell_reserved_for_oper_r16_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  using cell_reserved_for_oper_r16_e_ = enumerated<cell_reserved_for_oper_r16_opts>;

  // member variables
  bool                          ext                     = false;
  bool                          ranac_r16_present       = false;
  bool                          iab_support_r16_present = false;
  npn_id_list_r16_l_            npn_id_list_r16;
  fixed_bitstring<24>           tac_r16;
  uint16_t                      ranac_r16 = 0;
  fixed_bitstring<36>           cell_id_r16;
  cell_reserved_for_oper_r16_e_ cell_reserved_for_oper_r16;
  // ...
  // group 0
  bool    gnb_id_len_r17_present = false;
  uint8_t gnb_id_len_r17         = 22;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NTN-NeighCellConfigList-r17 ::= SEQUENCE (SIZE (1..4)) OF NTN-NeighCellConfig-r17
using ntn_neigh_cell_cfg_list_r17_l = dyn_array<ntn_neigh_cell_cfg_r17_s>;

// PLMN-IdentityInfo ::= SEQUENCE
struct plmn_id_info_s {
  using plmn_id_list_l_ = dyn_array<plmn_id_s>;
  struct cell_reserved_for_oper_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  using cell_reserved_for_oper_e_ = enumerated<cell_reserved_for_oper_opts>;
  using tracking_area_list_r17_l_ = bounded_array<fixed_bitstring<24>, 12>;

  // member variables
  bool                      ext           = false;
  bool                      tac_present   = false;
  bool                      ranac_present = false;
  plmn_id_list_l_           plmn_id_list;
  fixed_bitstring<24>       tac;
  uint16_t                  ranac = 0;
  fixed_bitstring<36>       cell_id;
  cell_reserved_for_oper_e_ cell_reserved_for_oper;
  // ...
  // group 0
  bool iab_support_r16_present = false;
  // group 1
  bool                                gnb_id_len_r17_present = false;
  copy_ptr<tracking_area_list_r17_l_> tracking_area_list_r17;
  uint8_t                             gnb_id_len_r17 = 22;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// ReferenceTimeInfo-r16 ::= SEQUENCE
struct ref_time_info_r16_s {
  bool           uncertainty_r16_present    = false;
  bool           time_info_type_r16_present = false;
  bool           ref_sfn_r16_present        = false;
  ref_time_r16_s time_r16;
  uint16_t       uncertainty_r16 = 0;
  uint16_t       ref_sfn_r16     = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// NPN-IdentityInfoList-r16 ::= SEQUENCE (SIZE (1..12)) OF NPN-IdentityInfo-r16
using npn_id_info_list_r16_l = dyn_array<npn_id_info_r16_s>;

// PLMN-IdentityInfoList ::= SEQUENCE (SIZE (1..12)) OF PLMN-IdentityInfo
using plmn_id_info_list_l = dyn_array<plmn_id_info_s>;

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

// SNPN-AccessInfo-r17 ::= SEQUENCE
struct sn_pn_access_info_r17_s {
  bool ext_ch_supported_r17_present                = false;
  bool ext_ch_without_cfg_allowed_r17_present      = false;
  bool onboarding_enabled_r17_present              = false;
  bool ims_emergency_support_for_sn_pn_r17_present = false;

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

// CellAccessRelatedInfo ::= SEQUENCE
struct cell_access_related_info_s {
  using snpn_access_info_list_r17_l_ = dyn_array<sn_pn_access_info_r17_s>;

  // member variables
  bool                ext                                 = false;
  bool                cell_reserved_for_other_use_present = false;
  plmn_id_info_list_l plmn_id_info_list;
  // ...
  // group 0
  bool                             cell_reserved_for_future_use_r16_present = false;
  copy_ptr<npn_id_info_list_r16_l> npn_id_info_list_r16;
  // group 1
  copy_ptr<snpn_access_info_list_r17_l_> snpn_access_info_list_r17;

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

} // namespace rrc_nr
} // namespace asn1
